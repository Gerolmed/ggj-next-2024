#include "include/renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>

CommandBuffer renderer_Buffer(u32 byte_len, u8* cmd_memory, 
                              u32 vertex_count, Vertex* vertex_buffer, 
                              u32 index_count, u32* index_buffer,
                              u32 mask_vertex_count, MaskVertex* mask_vertices,
                              Mat4 proj, u32 width, u32 height,
                              TextureHandle white)
{

    CommandBuffer buffer;
    buffer.settings.width = width;
    buffer.settings.height = height;
    buffer.byte_len = byte_len;
    buffer.curr_len = 0;
    buffer.cmd_memory = cmd_memory;

    buffer.vertex_count = vertex_count;
    buffer.vertex_curr = 0;
    buffer.vertex_buffer = vertex_buffer;

    buffer.index_count = index_count;
    buffer.index_curr = 0;
    buffer.index_buffer = index_buffer;

    buffer.mask_vertex_count = mask_vertex_count;
    buffer.mask_vertex_curr = 0;
    buffer.mask_vertex_buffer = mask_vertices;

    buffer.proj = proj;
    buffer.base.x = 0;
    buffer.base.y = 0;
    buffer.white = white;
    return buffer;
}

TextureLoadOp renderer_TextureLoadOp(TextureHandle* handle, const char* path)
{
    stbi_set_flip_vertically_on_load(true);
    TextureLoadOp load_op;
    load_op.handle = handle;
    load_op.data = stbi_load(path, &load_op.width, &load_op.height, 
                             &load_op.num_channels, 0);
    if (!load_op.data) {
        printf("Failed to load texture\n");
        assert(0);
    }

    return load_op;
}

void renderer_FreeTextureLoadOp(TextureLoadOp* load_op)
{
    stbi_image_free(load_op->data);
}

void renderer_PushClear(CommandBuffer* buffer, V3 color)
{
    if (buffer->curr_len + sizeof(CommandEntry_Clear) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on clear\n");
        return;
    }

    CommandEntry_Clear* entry = (CommandEntry_Clear*) (buffer->cmd_memory + buffer->curr_len);
    entry->header.type = Clear;
    entry->color = color;
    buffer->curr_len += sizeof(CommandEntry_Clear);
};

bool PushQuad(CommandBuffer* buffer, 
              V2 v1, V2 v2, V2 v3, V2 v4, float depth,
              V2 uv1, V2 uv2, V2 uv3, V2 uv4, 
              V3 color)
{
    u32 curr = buffer->vertex_curr;
    u32 index = buffer->index_curr;
    if (curr + 4 > buffer->vertex_count || index + 6 > buffer->index_count) {
        return false;
    }

    buffer->vertex_buffer[curr + 0].pos.x = v1.x - buffer->base.x;
    buffer->vertex_buffer[curr + 0].pos.y = v1.y - buffer->base.y;
    buffer->vertex_buffer[curr + 0].pos.z = depth;
    buffer->vertex_buffer[curr + 0].uv.x = uv1.x;
    buffer->vertex_buffer[curr + 0].uv.y = uv1.y;
    buffer->vertex_buffer[curr + 0].color = color;

    buffer->vertex_buffer[curr + 1].pos.x = v2.x - buffer->base.x;
    buffer->vertex_buffer[curr + 1].pos.y = v2.y - buffer->base.y;
    buffer->vertex_buffer[curr + 1].pos.z = depth;
    buffer->vertex_buffer[curr + 1].uv.x = uv2.x;
    buffer->vertex_buffer[curr + 1].uv.y = uv2.y;
    buffer->vertex_buffer[curr + 1].color = color;

    buffer->vertex_buffer[curr + 2].pos.x = v3.x - buffer->base.x;
    buffer->vertex_buffer[curr + 2].pos.y = v3.y - buffer->base.y;
    buffer->vertex_buffer[curr + 2].pos.z = depth;
    buffer->vertex_buffer[curr + 2].uv.x = uv3.x;
    buffer->vertex_buffer[curr + 2].uv.y = uv3.y;
    buffer->vertex_buffer[curr + 2].color = color;

    buffer->vertex_buffer[curr + 3].pos.x = v4.x - buffer->base.x;
    buffer->vertex_buffer[curr + 3].pos.y = v4.y - buffer->base.y;
    buffer->vertex_buffer[curr + 3].pos.z = depth;
    buffer->vertex_buffer[curr + 3].uv.x = uv4.x;
    buffer->vertex_buffer[curr + 3].uv.y = uv4.y;
    buffer->vertex_buffer[curr + 3].color = color;

    buffer->index_buffer[index + 0] = curr + 1;
    buffer->index_buffer[index + 1] = curr + 2;
    buffer->index_buffer[index + 2] = curr + 0;
    buffer->index_buffer[index + 3] = curr + 2;
    buffer->index_buffer[index + 4] = curr + 0;
    buffer->index_buffer[index + 5] = curr + 3;

    buffer->vertex_curr += 4;
    buffer->index_curr += 6;

    return true;
}

bool PushQuad(CommandBuffer* buffer, 
              V2 down_left, V2 up_right, float depth,
              V2 uv_down_left, V2 uv_up_right, 
              Mat2f rot, V3 color)
{
    V2 uv1;
    V2 vert1 = rot * down_left;
    uv1.x = uv_down_left.x;
    uv1.y = uv_down_left.y;

    V2 v2o = v2(down_left.x, up_right.y);
    V2 vert2 = rot * v2o;
    V2 uv2;
    uv2.x = uv_down_left.x;
    uv2.y = uv_up_right.y;

    V2 vert3 = rot * up_right;
    V2 uv3;
    uv3.x = uv_up_right.x;
    uv3.y = uv_up_right.y;

    V2 v4o = v2(up_right.x, down_left.y);
    V2 vert4 = rot * v4o;
    V2 uv4;
    uv4.x = uv_up_right.x;
    uv4.y = uv_down_left.y;

    return PushQuad(buffer, vert1, vert2, vert3, vert4, depth,
                    uv1, uv2, uv3, uv4, color);
}

bool PushQuad(CommandBuffer* buffer, 
              V2 down_left, V2 up_right, float depth,
              V2 uv_down_left, V2 uv_up_right, 
              Mat3f trans, V3 color)
{
    V2 uv1;
    V2 vert1 = (trans * v3(down_left.x, down_left.y, 1)).xy;
    uv1.x = uv_down_left.x;
    uv1.y = uv_down_left.y;

    V2 vert2 = (trans * v3(down_left.x, up_right.y, 1)).xy;
    V2 uv2;
    uv2.x = uv_down_left.x;
    uv2.y = uv_up_right.y;

    V2 vert3 = (trans * v3(up_right.x, up_right.y, 1)).xy;
    V2 uv3;
    uv3.x = uv_up_right.x;
    uv3.y = uv_up_right.y;

    V2 vert4 = (trans * v3(up_right.x, down_left.y, 1)).xy;
    V2 uv4;
    uv4.x = uv_up_right.x;
    uv4.y = uv_down_left.y;

    return PushQuad(buffer, vert1, vert2, vert3, vert4, depth,
                    uv1, uv2, uv3, uv4, color);
}


void renderer_PushSprite(CommandBuffer* buffer, 
                         V2 down_left, V2 up_right, float depth,
                         Sprite sprite,
                         Mat2f rot, V3 color, TextureHandle texture)
{
    if (buffer->curr_len + sizeof(CommandEntry_DrawQuads) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on draw\n");
        return;
    }

    // TODO: Pixelperfect?
    // if (1) {
    //     down_left.x = floor(down_left.x + 0.5);
    //     down_left.y = floor(down_left.y + 0.5);
    //     up_right.x = floor(up_right.x + 0.5);
    //     up_right.y = floor(up_right.y + 0.5);
    // }

    CommandEntry_DrawQuads* draw = (CommandEntry_DrawQuads*) (buffer->cmd_memory + buffer->curr_len);
    draw->header.type = DrawQuads;
    draw->index_offset = buffer->index_curr;
    draw->index_count = 6;
    draw->texture = texture;
    draw->type = QuadTypeSprite;

    if (PushQuad(buffer, 
                 down_left, up_right, depth,
                 sprite.bottom_left, sprite.top_right,
                 rot, color)) {
        buffer->curr_len += sizeof(CommandEntry_DrawQuads);
    }
}

void renderer_PushSprite(CommandBuffer* buffer,
                         V2 down_left, V2 up_right, float depth,
                         Sprite sprite,
                         Mat3f trans, V3 color, TextureHandle texture)
{
    if (buffer->curr_len + sizeof(CommandEntry_DrawQuads) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on draw\n");
        return;
    }

    // TODO: Pixelperfect?
    // if (1) {
    //     down_left.x = floor(down_left.x + 0.5);
    //     down_left.y = floor(down_left.y + 0.5);
    //     up_right.x = floor(up_right.x + 0.5);
    //     up_right.y = floor(up_right.y + 0.5);
    // }

    CommandEntry_DrawQuads* draw = (CommandEntry_DrawQuads*) (buffer->cmd_memory + buffer->curr_len);
    draw->header.type = DrawQuads;
    draw->index_offset = buffer->index_curr;
    draw->index_count = 6;
    draw->texture = texture;
    draw->type = QuadTypeSprite;

    if (PushQuad(buffer, 
                 down_left, up_right, depth,
                 sprite.bottom_left, sprite.top_right,
                 trans, color)) {
        buffer->curr_len += sizeof(CommandEntry_DrawQuads);
    }
}

void renderer_PushLine(CommandBuffer* buffer, V2 start, V2 end, 
                       float depth, float width, V3 color)
{
    if (buffer->curr_len + sizeof(CommandEntry_DrawQuads) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on draw\n");
        return;
    }

    CommandEntry_DrawQuads* draw = (CommandEntry_DrawQuads*) (buffer->cmd_memory + buffer->curr_len);
    draw->header.type = DrawQuads;
    draw->index_offset = buffer->index_curr;
    draw->index_count = 6;
    draw->texture = buffer->white;
    draw->type = QuadTypeSprite;

    V2 dir = v2(end.x - start.x, end.y - start.y).Norm();
    V2 side = v2(-dir.y * width, dir.x * width);

    V2 vert1 = v2(start.x + side.x, start.y + side.y);
    V2 uv1 = v2(0);

    V2 vert2 = v2(start.x - side.x, start.y - side.y);
    V2 uv2 = v2(0, 1);

    V2 vert3 = v2(end.x - side.x, end.y - side.y);
    V2 uv3 = v2(1);

    V2 vert4 = v2(end.x + side.x, end.y + side.y);
    V2 uv4 = v2(1, 0);

    if (PushQuad(buffer, vert1, vert2, vert3, vert4, depth, 
                 uv1, uv2, uv3, uv4, color)) {
        buffer->curr_len += sizeof(CommandEntry_DrawQuads);
    }
}

void renderer_PushOutline(CommandBuffer* buffer, 
                          V2 down_left, V2 up_right, float depth, 
                          float width, V3 color)
{
    renderer_PushLine(buffer, down_left, 
                      v2(down_left.x, up_right.y), depth, 
                      width, color);

    renderer_PushLine(buffer, down_left, 
                      v2(up_right.x, down_left.y), depth, 
                      width, color);

    renderer_PushLine(buffer, up_right, 
                      v2(up_right.x, down_left.y), depth, 
                      width, color);

    renderer_PushLine(buffer, up_right, 
                      v2(down_left.x, up_right.y), depth, 
                      width, color);
}

void renderer_PushString(CommandBuffer* buffer, Font* font, const char* str, 
                         V2 pos, float depth)
{
    if (buffer->curr_len + sizeof(CommandEntry_DrawQuads) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on draw\n");
        return;
    }

    CommandEntry_DrawQuads* draw = (CommandEntry_DrawQuads*) (buffer->cmd_memory + buffer->curr_len);
    draw->header.type = DrawQuads;
    draw->index_offset = buffer->index_curr;
    draw->texture = font->handle;
    draw->type = QuadTypeFont;
    buffer->curr_len += sizeof(CommandEntry_DrawQuads);

    float x_pos = 0;
    u32 index_count = 0;

    V3 color = v3(1);

    while (*str) {
        Glyph* glyph = font->glyphs + *str;

        float uvx0 = (float) glyph->start_x / (float) font->atlas_width;
        float uvy0 = (float) glyph->height / (float) font->atlas_height;
        float uvx1 = uvx0 + (float) glyph->width / (float) font->atlas_width;
        float uvy1 = 0;

        // float uvx0 = 0;
        // float uvy0 = 0;
        // float uvx1 = 1;
        // float uvy1 = 1;

        float y_pos = pos.y + glyph->height - glyph->top;
        V2 down_left = v2(pos.x + x_pos + glyph->left, -y_pos);
        V2 up_right = v2(down_left.x + glyph->width, down_left.y + glyph->height);
        if (!PushQuad(buffer, down_left, up_right, depth,
                      v2(uvx0, uvy0), v2(uvx1, uvy1), 
                      mat2(1), color)) {
            break;
        }

        x_pos += (float) glyph->advance / 64.0f;
        index_count += 6;
        ++str;
    }

    draw->index_count = index_count;
}

void renderer_PushPostprocessPass(CommandBuffer* buffer)
{
    if (buffer->curr_len + sizeof(CommandEntry_PostprocessPass) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on postprocess\n");
        return;
    }

    CommandEntry_PostprocessPass* postprocess = (CommandEntry_PostprocessPass*) 
        (buffer->cmd_memory + buffer->curr_len);

    postprocess->header.type = PostProcessPass;

    buffer->curr_len += sizeof(CommandEntry_PostprocessPass);
}

void renderer_PushBase(CommandBuffer* buffer, V2 base)
{
    buffer->base = base;
}

void renderer_PushMaskOp(CommandBuffer* buffer, u32 vertex_count, MaskVertex* vertices)
{
    if (buffer->curr_len + sizeof(CommandEntry_MaskOp) > buffer->byte_len) {
        printf("Warning: Buffer size exceeded on mask\n");
        return;
    }
    if (buffer->mask_vertex_curr + vertex_count > buffer->mask_vertex_count) {
        printf("Warning: Mask vertex count exceeded\n");
        return;
    }

    CommandEntry_MaskOp* mask_op = (CommandEntry_MaskOp*) (buffer->cmd_memory + buffer->curr_len);
    mask_op->header.type = MaskOp;
    mask_op->vertex_count = vertex_count;
    mask_op->vertex_offset = buffer->mask_vertex_curr;
    u32 cur = buffer->mask_vertex_curr;

    for (u32 i = 0; i < vertex_count; ++i) {
        buffer->mask_vertex_buffer[i + cur].pos.x = vertices[i].pos.x - buffer->base.x;
        buffer->mask_vertex_buffer[i + cur].pos.y = vertices[i].pos.y - buffer->base.y;
    }

    buffer->mask_vertex_curr += vertex_count;
    buffer->curr_len += sizeof(CommandEntry_MaskOp);
}
