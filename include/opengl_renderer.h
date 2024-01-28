#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "include/renderer.h"
#include "include/arena.h"
#include "include/types.h"

#define BLOOM_MIPS 5

struct BloomMip
{
    u32 texture;
    u32 width;
    u32 height;
};

struct ProgramBase
{
    u32 id;
    u32 proj;
};

struct DownsampleProgram
{
    ProgramBase base;
    u32 res;
};

struct PostprocessProgram
{
    ProgramBase base;
    u32 bloom_buffer;
    u32 mask_buffer;
};

struct OpenGLContext
{
    Arena render_arena;

    ProgramBase sprite_shader;
    ProgramBase font_shader;
    PostprocessProgram post_shader;

    u32 render_framebuffer;
    u32 color_buffer;
    u32 vertex_buffer;

    u32 bloom_framebuffer;
    BloomMip bloom_mips[BLOOM_MIPS];
    DownsampleProgram downsample_shader;
    ProgramBase upsample_shader;

    u32 mask_vertex_buffer;
    u32 mask_framebuffer;
    u32 mask_color_buffer;
    ProgramBase mask_shader;

    u32 quad_arr;
    u32 draw_arr;
    u32 mask_arr;

    u32 render_width;
    u32 render_height;
};

void opengl_Init(u32 width, u32 height);
void opengl_RenderCommands(CommandBuffer* buffer);
void opengl_LoadTexture(TextureLoadOp* load_op);
void opengl_SetupFont(Font* font, const char* filepath, u32 font_size);

#endif
