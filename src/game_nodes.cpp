#include "include/game_nodes.h"

#include "include/input.h"
#include "include/renderer.h"
#include "include/time.h"
#include "include/opengl_renderer.h"
#include <stdio.h>


void TestNode::Update() {
    Node::Update();

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    // printf("Update test (Pressed: %d, Up: %d, Down: %d)\n", input_KeySpace.is_pressed, input_KeySpace.up, input_KeySpace.down);
}

EnemyNode::EnemyNode(Level* level) : Node(level) {
    TextureHandle pp;
    TextureLoadOp load_op = renderer_TextureLoadOp(&pp, "assets/enemy.png");
    opengl_LoadTexture(&load_op);

    renderer_FreeTextureLoadOp(&load_op);

    auto* texNode = new TextureNode(level, pp, 100, 100);

    AddChild(texNode);

}

void EnemyNode::PreUpdate() {
    Node:: PreUpdate();

    Node::PreUpdate();
    aabb.position = position;
    aabb.size = { 32, 32 };
}

void EnemyNode::Update() {
    Node::Update();
}

void EnemyNode::Render(CommandBuffer* buffer){
    Node::Render(buffer);

    aabb.position = {INFINITY,INFINITY};

    float time = glfwGetTime();

    V2 ray = v2(position.x + sin( time), position.y + cos(time));
    float t = game_Raycast(level, position, ray);

    renderer_PushLine(buffer, v2(position.x, position.y),
                      v2(position.x + ray.x * t, position.y + ray.y * t), 30, 1, v3(0, 0, 1));
    aabb.position = {position.x, position.y};


    renderer_PushLine(buffer, v2(0),
                      v2(ray.x * t, ray.y * t), 30, 1, v3(0, 0, 1));
}


PlayerNode::PlayerNode(Level* level) : Node(level) {

    TextureHandle pp;
    TextureLoadOp load_op = renderer_TextureLoadOp(&pp, "assets/player/player_idle.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    rotation_root = new Node(level);
    AddChild(rotation_root);

    auto* texNode = new TextureNode(level, pp, 32, 32);
    rotation_root->AddChild(texNode);
}


void PlayerNode::PreUpdate() {
    Node::PreUpdate();
    aabb.position = position - v2(9);
    aabb.size = {18, 18};

}

void PlayerNode::Update() {
    auto movement = v2(0, 0);
    float speed = 300;
    if (input_KeyA.is_pressed) {
        movement.x -= 1;
    }
    if (input_KeyD.is_pressed) {
        movement.x += 1;
    }
    if (input_KeyW.is_pressed) {
        movement.y += 1;
    }
    if (input_KeyS.is_pressed) {
        movement.y -= 1;
    }

    movement = movement.Norm();

    aabb.move_and_collide({movement.x * (time_deltatime * speed) ,0}, level);
    aabb.move_and_collide({0, movement.y * (time_deltatime * speed)}, level);
    position = aabb.position + v2(9);

    if(movement.SqrMagnitude() > 0.1f) {
        // rotation_root->rotation = degreesToRadians(90);
    }

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    Node::Update();
}

void TextureNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    const V2 down_left = V2{-std::ceil(width / 2.0f), -std::ceil(height / 2.0f)} + GetAbsolutePosition();
    const V2 up_right = V2{std::floor(width / 2.0f), std::floor(height / 2.0f)} + GetAbsolutePosition();
    renderer_PushSprite(buffer,
                        down_left, up_right, 10,
                        {{0, 0}, {1, 1}},
                        GetAbsoluteRotationMatrix(), V3{1, 1, 1}, texture_handle
    );
}

SpritesheetNode::SpritesheetNode(Level* level, TextureHandle texture_handle, int width, int height, int rows,
                                 int columns)
    : Node(level), texture_handle(texture_handle), width(width), height(height), rows(rows), columns(columns) {
    item_height = 1.0f / rows;
    item_width = 1.0f / columns;
}


void SpritesheetNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    const V2 down_left = V2{-std::ceil(width / 2.0f), -std::ceil(height / 2.0f)} + GetAbsolutePosition();
    const V2 up_right = V2{std::floor(width / 2.0f), std::floor(height / 2.0f)} + GetAbsolutePosition();
    const float remainingX = current_frame % columns;
    const V2 uv_bot_left = V2{
        remainingX * item_width, (rows - ((current_frame - remainingX) / columns + 1)) * item_height
    };
    renderer_PushSprite(buffer,
                        down_left, up_right, 10,
                        {uv_bot_left, uv_bot_left + V2{item_width, item_height}},
                        GetAbsoluteRotationMatrix(), V3{1, 1, 1}, texture_handle
    );
}

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
