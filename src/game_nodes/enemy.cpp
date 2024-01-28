#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"

/////////////////////////////////
/// EnemyNode
/////////////////////////////////

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
