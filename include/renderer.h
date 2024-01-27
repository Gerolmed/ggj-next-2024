#ifndef RENDERER_H
#define RENDERER_H

#include "include/types.h"


#define GLYPH_COUNT 128

struct TextureHandle
{
    u32 id;
};

struct Glyph
{
    u32 width;
    u32 height;
    u32 start_x;
    u32 left;
    u32 top;
    u32 advance;
};

struct Font
{
    u32 atlas_width;
    u32 atlas_height;
    Glyph glyphs[GLYPH_COUNT];
    TextureHandle handle;
};

struct TextureLoadOp
{
    i32 width;
    i32 height;
    i32 num_channels;
    u8* data;
    TextureHandle* handle;
};

struct Vertex
{
    V2 pos;
    V2 uv;
    V3 color;
};

struct RenderSettings
{
    u32 width;
    u32 height;
};

struct CommandBuffer
{
    u32 byte_len;
    u32 curr_len;
    u8* cmd_memory;

    u32 vertex_count;
    u32 vertex_curr;
    Vertex* vertex_buffer;

    u32 index_count;
    u32 index_curr;
    u32* index_buffer;

    V2 base;

    RenderSettings settings;
    // Note: proj and view matrix
    Mat4 proj;
};

enum CommandEntryType 
{
    Clear = 0,
    DrawQuads = 1,
    PostProcessPass = 2,
};

enum QuadType
{
    QuadTypeSprite = 0,
    QuadTypeFont = 1,
};

struct CommandEntryHeader
{
    u32 type;
};

struct CommandEntry_Clear
{
    CommandEntryHeader header;
    V3 color;
};

struct CommandEntry_DrawQuads
{
    CommandEntryHeader header;
    u32 index_offset;
    u32 index_count;
    u32 type;
    TextureHandle* texture;
};

struct CommandEntry_PostprocessPass
{
    CommandEntryHeader header;
};

CommandBuffer renderer_Buffer(u32 byte_len, u8* cmd_memory, 
                              u32 vertex_count, Vertex* vertex_buffer, 
                              u32 index_count, u32* index_buffer,
                              Mat4 proj, u32 width, u32 height);

TextureLoadOp renderer_TextureLoadOp(TextureHandle* handle, const char* path);

void renderer_FreeTextureLoadOp(TextureLoadOp* load_op);

void renderer_PushClear(CommandBuffer* buffer, V3 color);

void renderer_PushSprite(CommandBuffer* buffer, 
                         V2 down_left, V2 up_right, 
                         V2 uv_down_left, V2 uv_up_right,
                         Mat2f rot, V3 color, TextureHandle* texture);

void renderer_PushString(CommandBuffer* buffer, Font* font, const char* str, V2 pos);

void renderer_PushPostprocessPass(CommandBuffer* buffer);

void renderer_PushBase(CommandBuffer* buffer, V2 base);

#endif
