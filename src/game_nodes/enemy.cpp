#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"

/////////////////////////////////
/// EnemyNode
/////////////////////////////////

EnemyNode::EnemyNode(Level* level) : Node(level) {
    TextureHandle pp;
    TextureLoadOp load_op = renderer_TextureLoadOp(&pp, "assets/enemies/drone.png");
    opengl_LoadTexture(&load_op);

    renderer_FreeTextureLoadOp(&load_op);

    auto* texNode = new AnimatedSpriteNode(level, pp, 32, 32, 1, 3);
    texNode->seconds_per_frame = 0.1f;
    AddChild(texNode);

}

void EnemyNode::PreUpdate() {
    Node:: PreUpdate();
}

void EnemyNode::Update() {
    Node::Update();
}

void EnemyNode::Render(CommandBuffer* buffer){
    Node::Render(buffer);


    V2 ray = GetAbsoluteRotationMatrix() * v2(0, 1);
    V2 world_pos = GetAbsolutePosition();
    float t = game_Raycast(level, world_pos, ray);

    renderer_PushLine(buffer, v2(world_pos.x, world_pos.y),
                      world_pos + ray * t, 30, 1, v3(0, 0, 1));
}
