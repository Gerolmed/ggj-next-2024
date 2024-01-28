#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"

#define ENEMY_RAY_COUNT 40

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

    renderer_PushBase(buffer, v2(0));

    const V2 pos = GetAbsolutePosition();
    const V2 facing = v2(-sin(this->rotation),
                         cos(this->rotation));
    const V2 side = v2(-facing.y, facing.x);
    const float fov = 0.3;

    MaskVertex test_mask[3 * (ENEMY_RAY_COUNT - 1)];

    V2 last_vert = v2(0);
    const V2 r = v2((1 - fov) * facing.x - fov * side.x,
                    (1 - fov) * facing.y - fov * side.y);
    const float distance = game_Raycast(level, pos, r);
    last_vert.x = r.x * distance;
    last_vert.y = r.y * distance;

    float o = 2.0f / ENEMY_RAY_COUNT - 1;
    for (u32 i = 0; i < ENEMY_RAY_COUNT - 1; ++i) {
        o += 2.0f / ENEMY_RAY_COUNT;

        V2 r = v2((1 - fov) * facing.x + o * fov * side.x,
                  (1 - fov) * facing.y + o * fov * side.y);
        float t = game_Raycast(level, pos, r);
        test_mask[3 * i + 0].pos.x = 0;
        test_mask[3 * i + 0].pos.y = 0;
        test_mask[3 * i + 1].pos.x = last_vert.x;
        test_mask[3 * i + 1].pos.y = last_vert.y;
        test_mask[3 * i + 2].pos.x = r.x * t;
        test_mask[3 * i + 2].pos.y = r.y * t;

        last_vert.x = test_mask[3 * i + 2].pos.x;
        last_vert.y = test_mask[3 * i + 2].pos.y;

    }

    renderer_PushMaskOp(buffer, 3 * (ENEMY_RAY_COUNT - 1), test_mask);
    renderer_PushBase(buffer, level->camera.center);
}
