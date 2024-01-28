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
    V3 pos;
    V2 uv;
    V3 color;
};

struct MaskVertex
{
    V2 pos;
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

    u32 mask_vertex_count;
    u32 mask_vertex_curr;
    MaskVertex* mask_vertex_buffer;

    u32 index_count;
    u32 index_curr;
    u32* index_buffer;

    V2 base;

    RenderSettings settings;
    // Note: proj and view matrix
    Mat4 proj;

    TextureHandle white;
};

enum CommandEntryType 
{
    Clear = 0,
    DrawQuads = 1,
    PostProcessPass = 2,
    MaskOp = 3,
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
    TextureHandle texture;
};

struct CommandEntry_PostprocessPass
{
    CommandEntryHeader header;
};

struct CommandEntry_MaskOp
{
    CommandEntryHeader header;
    u32 vertex_offset;
    u32 vertex_count;
};

CommandBuffer renderer_Buffer(u32 byte_len, u8* cmd_memory, 
                              u32 vertex_count, Vertex* vertex_buffer, 
                              u32 index_count, u32* index_buffer,
                              u32 mask_vertex_count, MaskVertex* mask_vertices,
                              Mat4 proj, u32 width, u32 height,
                              TextureHandle white);

TextureLoadOp renderer_TextureLoadOp(TextureHandle* handle, const char* path);

void renderer_FreeTextureLoadOp(TextureLoadOp* load_op);

void renderer_PushClear(CommandBuffer* buffer, V3 color);

void renderer_PushSprite(CommandBuffer* buffer, 
                         V2 down_left, V2 up_right, float depth,
                         Sprite sprite,
                         Mat2f rot, V3 color, TextureHandle texture);

/**
 * \brief Pushes an action to render a sprite to the command buffer. This includes pushing a quad internally.
 * \param buffer the buffer to push to
 * \param down_left bottom left corner relative to pivot point. Should not be rotated!
 * \param up_right top right corner relative to pivot point. Should not be rotated!
 * \param depth the depth to render the object at. the higher the closer to the screen
 * \param sprite the uv coordinates for the respecting vertex corners
 * \param transform_matrix a transformation (tranlate + rotate) matrix to modify vertices
 * \param color the texture tint to render with
 * \param texture the texture to render
 */
void renderer_PushSprite(CommandBuffer* buffer,
                         V2 down_left, V2 up_right, float depth,
                         Sprite sprite,
                         Mat3f trans, V3 color, TextureHandle texture);

void renderer_PushLine(CommandBuffer* buffer, V2 start, V2 end, 
                       float depth, float width, V3 color);

void renderer_PushOutline(CommandBuffer* buffer, 
                          V2 down_left, V2 up_right, float depth, 
                          float width, V3 color);

void renderer_PushString(CommandBuffer* buffer, Font* font, const char* str, 
                         V2 pos, float depth);

void renderer_PushPostprocessPass(CommandBuffer* buffer);

void renderer_PushBase(CommandBuffer* buffer, V2 base);

void renderer_PushMaskOp(CommandBuffer* buffer, u32 vertex_count, MaskVertex* vertices);

#endif
