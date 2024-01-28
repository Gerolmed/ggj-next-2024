#include "include/opengl_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "include/platform.h"
#include "include/game_math.h"


OpenGLContext OpenGL;

ProgramBase load_program(const char* vertex_file, const char* frag_file)
{
    begin_tmp(&OpenGL.render_arena);

    char info_log[512];
    i32 status;

    char* vertex_code = read_file(vertex_file, NULL, &OpenGL.render_arena);
    assert(vertex_code);
    u32 vertex_prog = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_prog, 1, &vertex_code, NULL);
    glCompileShader(vertex_prog);
    glGetShaderiv(vertex_prog, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertex_prog, 512, NULL, info_log);
        printf("Error compiling vertex shader: %s\n", info_log);
        assert(0);
    }

    char* frag_code = read_file(frag_file, NULL, &OpenGL.render_arena);
    assert(frag_code);
    u32 frag_prog = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_prog, 1, &frag_code, NULL);
    glCompileShader(frag_prog);
    glGetShaderiv(frag_prog, GL_COMPILE_STATUS, &status);
    if(!status) {
        glGetShaderInfoLog(frag_prog, 512, NULL, info_log);
        printf("Error compiling fragment shader: %s\n", info_log);
        assert(0);
    }

    ProgramBase shader;
    shader.id = glCreateProgram();
    glAttachShader(shader.id, vertex_prog);
    glAttachShader(shader.id, frag_prog);
    glLinkProgram(shader.id);
    glGetProgramiv(shader.id, GL_LINK_STATUS, &status);
    if(!status) {
        glGetProgramInfoLog(shader.id, 512, NULL, info_log);
        printf("Error linking shader: %s\n", info_log);
        assert(0);
    }

    glDeleteShader(vertex_prog);
    glDeleteShader(frag_prog);
    end_tmp(&OpenGL.render_arena);

    shader.proj = glGetUniformLocation(shader.id, "proj");
    return shader;
}

void SetupBloomMips()
{
    glGenFramebuffers(1, &OpenGL.bloom_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.bloom_framebuffer);

    u32 bloom_textures[BLOOM_MIPS];
    glGenTextures(BLOOM_MIPS, bloom_textures);

    u32 mip_width = OpenGL.render_width / 2;
    u32 mip_height = OpenGL.render_height / 2;

    for (u32 i = 0; i < BLOOM_MIPS; ++i) {
        BloomMip* mip = OpenGL.bloom_mips + i;
        mip->width = mip_width;
        mip->height = mip_height;
        mip->texture = bloom_textures[i];

        glBindTexture(GL_TEXTURE_2D, mip->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, 
                     mip->width, mip->height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mip_width /= 2;
        mip_height /= 2;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, OpenGL.bloom_mips[0].texture, 0);
    u32 attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
}

void RenderDownsamples()
{
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.bloom_framebuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, OpenGL.color_buffer);
    glBindVertexArray(OpenGL.quad_arr);
    glUseProgram(OpenGL.downsample_shader.base.id);
    glUniform2f(OpenGL.downsample_shader.res, OpenGL.render_width, OpenGL.render_height);

    for (u32 i = 0; i < BLOOM_MIPS; ++i) {
        BloomMip* mip = OpenGL.bloom_mips + i;

        glViewport(0, 0, mip->width, mip->height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                               mip->texture, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUniform2f(OpenGL.downsample_shader.res, mip->width, mip->height);
        glBindTexture(GL_TEXTURE_2D, mip->texture);
    }
}

void RenderUpsamples()
{
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.bloom_framebuffer);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(OpenGL.upsample_shader.id);
    glBindVertexArray(OpenGL.quad_arr);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (u32 i = BLOOM_MIPS - 1; i > 0; --i) {
        u32 next_mip = i - 1;
        glBindTexture(GL_TEXTURE_2D, OpenGL.bloom_mips[i].texture);
        glViewport(0, 0, OpenGL.bloom_mips[next_mip].width, OpenGL.bloom_mips[next_mip].height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                               OpenGL.bloom_mips[next_mip].texture, 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void opengl_Init(u32 render_width, u32 render_height)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to load required extensions\n");
    }
    init_arena(&OpenGL.render_arena, &pool);

    OpenGL.render_width = render_width;
    OpenGL.render_height = render_height;

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    u32 arrs[3];
    glGenVertexArrays(3, arrs);
    glBindVertexArray(arrs[0]);

    u32 buffers[4];
    glGenBuffers(4, buffers);

    float quad_data[] = {
        -1, 1,
        1, 1,
        -1, -1,
        1, -1,
    };

    u32 depth_buffer;
    glGenFramebuffers(1, &OpenGL.render_framebuffer);
    glGenTextures(1, &OpenGL.color_buffer);
    glGenRenderbuffers(1, &depth_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.render_framebuffer);
    glBindTexture(GL_TEXTURE_2D, OpenGL.color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
                 render_width, render_height, 0, 
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, OpenGL.color_buffer, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_width, render_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
    u32 attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glGenFramebuffers(1, &OpenGL.mask_framebuffer);
    glGenTextures(1, &OpenGL.mask_color_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.mask_framebuffer);
    glBindTexture(GL_TEXTURE_2D, OpenGL.mask_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, render_width,
                 render_height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, OpenGL.mask_color_buffer, 0);
    glDrawBuffers(1, attachments);

    SetupBloomMips();

    OpenGL.mask_arr = arrs[2];
    glBindVertexArray(OpenGL.mask_arr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MaskVertex) * 1000, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    OpenGL.mask_vertex_buffer = buffers[3];

    OpenGL.quad_arr = arrs[1];
    glBindVertexArray(OpenGL.quad_arr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, quad_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    OpenGL.draw_arr = arrs[0];
    glBindVertexArray(OpenGL.draw_arr);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 10000, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 10000, NULL, GL_DYNAMIC_DRAW);
    OpenGL.vertex_buffer = buffers[0];

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    OpenGL.sprite_shader = load_program("shader/default.vert", "shader/sprite.frag");
    OpenGL.font_shader = load_program("shader/default.vert", "shader/font.frag");
    OpenGL.post_shader.base = load_program("shader/post.vert", "shader/post.frag");
    OpenGL.post_shader.bloom_buffer = glGetUniformLocation(OpenGL.post_shader.base.id, "bloom_buffer");
    OpenGL.downsample_shader.base = load_program("shader/post.vert", "shader/downsample.frag");
    OpenGL.downsample_shader.res = glGetUniformLocation(OpenGL.downsample_shader.base.id, "res");
    OpenGL.upsample_shader = load_program("shader/post.vert", "shader/upsample.frag");
    OpenGL.mask_shader = load_program("shader/mask.vert", "shader/mask.frag");

    glUseProgram(OpenGL.post_shader.base.id);
    glUniform1i(OpenGL.post_shader.bloom_buffer, 1);
    glUniform1i(OpenGL.post_shader.mask_buffer, 2);
}

void CopyToBuffer(u32 slot, u32 size, void* data)
{
    void* map = glMapBufferRange(slot, 0, size, GL_MAP_WRITE_BIT);
    memcpy(map, data, size);
    glUnmapBuffer(slot);
}

void opengl_RenderCommands(CommandBuffer* buffer)
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.render_framebuffer);
    glViewport(0, 0, OpenGL.render_width, OpenGL.render_height);
    glBindVertexArray(OpenGL.draw_arr);

    // TODO: profile this
    // glBufferData(GL_ARRAY_BUFFER, buffer->vertex_curr * sizeof(Vertex), 
    //              buffer->vertex_buffer, GL_DYNAMIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->index_curr * sizeof(u32), 
    //              buffer->index_buffer, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.mask_vertex_buffer);
    CopyToBuffer(GL_ARRAY_BUFFER, sizeof(MaskVertex) * buffer->mask_vertex_count, 
                 buffer->mask_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.vertex_buffer);
    CopyToBuffer(GL_ARRAY_BUFFER, sizeof(Vertex) * buffer->vertex_curr, buffer->vertex_buffer);
    CopyToBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * buffer->index_curr, buffer->index_buffer);

    glUseProgram(OpenGL.sprite_shader.id);
    glUniformMatrix4fv(OpenGL.sprite_shader.proj, 1, GL_FALSE, &(buffer->proj)[0][0]);
    glUseProgram(OpenGL.font_shader.id);
    glUniformMatrix4fv(OpenGL.font_shader.proj, 1, GL_FALSE, &(buffer->proj)[0][0]);
    glUseProgram(OpenGL.mask_shader.id);
    glUniformMatrix4fv(OpenGL.mask_shader.proj, 1, GL_FALSE, &(buffer->proj)[0][0]);

    u32 offset = 0;
    while (offset < buffer->curr_len) {
        CommandEntryHeader* header = (CommandEntryHeader*) (buffer->cmd_memory + offset);
        switch (header->type) {
            case Clear: 
            {
                glClearColor(0, 0, 0, 1);
                glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.mask_framebuffer);
                glClear(GL_COLOR_BUFFER_BIT);
                glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.render_framebuffer);

                CommandEntry_Clear* clear = (CommandEntry_Clear*) (buffer->cmd_memory + offset);
                glClearColor(clear->color.x, clear->color.y, clear->color.z, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                offset += sizeof(CommandEntry_Clear);
            } break;
            
            case DrawQuads:
            {
                CommandEntry_DrawQuads* draw = (CommandEntry_DrawQuads*) (buffer->cmd_memory + offset);

                u32 shader = OpenGL.sprite_shader.id;
                if (draw->type == QuadTypeFont) {
                    shader = OpenGL.font_shader.id;
                }
                glUseProgram(shader);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, draw->texture.id);
                glDrawElements(GL_TRIANGLES, draw->index_count, 
                               GL_UNSIGNED_INT, (void*) (draw->index_offset * sizeof(u32)));
                offset += sizeof(CommandEntry_DrawQuads);
            } break;

            case PostProcessPass:
            {
                RenderDownsamples();
                RenderUpsamples();

                glDisable(GL_DEPTH_TEST);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glViewport(0, 0, buffer->settings.width, buffer->settings.height);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glBindVertexArray(OpenGL.quad_arr);
                glUseProgram(OpenGL.post_shader.base.id);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, OpenGL.color_buffer);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, OpenGL.bloom_mips[0].texture);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, OpenGL.mask_color_buffer);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                // TODO: Unfuck opengl state here for further rendering

                offset += sizeof(CommandEntry_PostprocessPass);
            } break;

            case MaskOp:
            {
                CommandEntry_MaskOp* draw = (CommandEntry_MaskOp*) (buffer->cmd_memory + offset);
                glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.mask_framebuffer);
                glBindVertexArray(OpenGL.mask_arr);
                glUseProgram(OpenGL.mask_shader.id);

                glDisable(GL_CULL_FACE);
                glDisable(GL_DEPTH_TEST);

                glDrawArrays(GL_TRIANGLE_FAN, draw->vertex_offset, draw->vertex_count);
                // glDrawArrays(GL_TRIANGLES, 0, 3);

                glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.render_framebuffer);
                glBindVertexArray(OpenGL.draw_arr);
                offset += sizeof(CommandEntry_MaskOp);
            }

            default:
            {
            } break;
        }
    }
}

void opengl_LoadTexture(TextureLoadOp* load_op)
{
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    u32 format = GL_RGBA;
    if (load_op->num_channels == 3) {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                 load_op->width, load_op->height, 0, format, 
                 GL_UNSIGNED_BYTE, load_op->data);

    load_op->handle->id = texture;
}

void opengl_SetupFont(Font* font, const char* filepath, u32 font_size) 
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("Could not init freetype library\n");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath, 0, &face)) {
        printf("Failed to load font: %s\n", filepath);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    begin_tmp(&OpenGL.render_arena);

    u8* glyph_buffer[GLYPH_COUNT];

    i32 total_width = 0;
    i32 total_height = 0;
    for (u8 c = 0; c < GLYPH_COUNT; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("Failed to load glyph: %c\n", c);
            continue;
        }

        i32 width = face->glyph->bitmap.width;
        i32 height = face->glyph->bitmap.rows;
        font->glyphs[c].width = width;
        font->glyphs[c].height = height;
        font->glyphs[c].left = face->glyph->bitmap_left;
        font->glyphs[c].top = face->glyph->bitmap_top;
        font->glyphs[c].advance = face->glyph->advance.x;

        u32 buffer_size = sizeof(u8) * width * height;
        glyph_buffer[c] = (u8*) push_size(&OpenGL.render_arena, buffer_size);
        memcpy(glyph_buffer[c], face->glyph->bitmap.buffer, buffer_size);

        font->glyphs[c].start_x = total_width;
        total_width += font->glyphs[c].width;
        total_height = max(total_height, font->glyphs[c].height);

        // extruding glyphs to prevent them from blending
        if (width != 0 && height != 0 ) {
            total_width += 1;
        }
    }
    font->atlas_width = total_width;
    font->atlas_height = total_height;

    u32 atlas;
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                 total_width, total_height,
                 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (u8 c = 0; c < GLYPH_COUNT; ++c) {
        Glyph glyph = font->glyphs[c];
        glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.start_x, 0,
            glyph.width, glyph.height,
            GL_RED, GL_UNSIGNED_BYTE,
            glyph_buffer[c]);
    }
    font->handle.id = atlas;

    // free all resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    end_tmp(&OpenGL.render_arena);

}
