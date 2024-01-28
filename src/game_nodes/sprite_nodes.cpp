#include "include/game_nodes.h"
#include "include/opengl_renderer.h"
#include "include/time.h"

/////////////////////////////////
/// SpritesheetNode
/////////////////////////////////

SpritesheetNode::SpritesheetNode(Level* level, TextureHandle texture_handle, int width, int height, int rows,
                                 int columns)
    : Node(level), texture_handle(texture_handle), width(width), height(height), rows(rows), columns(columns) {
    item_height = 1.0f / rows;
    item_width = 1.0f / columns;
}


void SpritesheetNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    const V2 down_left = V2{-std::ceil(width / 2.0f), -std::ceil(height / 2.0f)};
    const V2 up_right = V2{std::floor(width / 2.0f), std::floor(height / 2.0f)};
    const float remainingX = current_frame % columns;
    const V2 uv_bot_left = V2{
        remainingX * item_width, (rows - ((current_frame - remainingX) / columns + 1)) * item_height
    };
    renderer_PushSprite(buffer,
                        down_left, up_right, 10,
                        {uv_bot_left, uv_bot_left + V2{item_width, item_height}},
                        GetAbsoluteMatrix(), V3{1, 1, 1}, texture_handle
    );
}

/////////////////////////////////
/// AnimatedSpriteNode
/////////////////////////////////

AnimatedSpriteNode::AnimatedSpriteNode(Level* level, TextureHandle texture_handle, int width, int height, int rows,
                                       int columns)
    : SpritesheetNode(level, texture_handle, width, height, rows, columns) {
    frame_count = columns * rows;
}

void AnimatedSpriteNode::Update() {
    currentDelta += time_deltatime;
    while (currentDelta > seconds_per_frame) {
        currentDelta -= seconds_per_frame;
        current_frame = (current_frame + 1) % frame_count;
    }
    SpritesheetNode::Update();
}

/////////////////////////////////
/// TextureNode
/////////////////////////////////


void TextureNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    const V2 down_left = V2{-std::ceil(width / 2.0f), -std::ceil(height / 2.0f)};
    const V2 up_right = V2{std::floor(width / 2.0f), std::floor(height / 2.0f)};
    renderer_PushSprite(buffer,
                        down_left, up_right, 10,
                        {{0, 0}, {1, 1}},
                        GetAbsoluteMatrix(), V3{1, 1, 1}, texture_handle
    );
}