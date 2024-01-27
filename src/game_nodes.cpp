#include "include/game_nodes.h"

#include "include/input.h"
#include "include/renderer.h"


void TestNode::Update() {
    Node::Update();

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    // printf("Update test (Pressed: %d, Up: %d, Down: %d)\n", input_KeySpace.is_pressed, input_KeySpace.up, input_KeySpace.down);
}


void PlayerNode::PreUpdate() {
    Node::PreUpdate();

}
void PlayerNode::Update() {
    Node::Update();

}

void TextureNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    const V2 down_left = V2{-std::ceil(width/2.0f), -std::ceil(height/2.0f)} + GetAbsolutePosition();
    const V2 up_right = V2{std::floor(width/2.0f), std::floor(height/2.0f)} + GetAbsolutePosition();
    renderer_PushSprite(buffer,
                        down_left, up_right,
                        {0, 0}, {1, 1},
                        GetAbsoluteRotationMatrix(), V3{1,1,1}, texture_handle
    );
}
