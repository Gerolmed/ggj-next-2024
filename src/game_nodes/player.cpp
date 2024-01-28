
#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"
#include "include/time.h"

/////////////////////////////////
/// PlayerNode
/////////////////////////////////

PlayerNode::PlayerNode(Level* level) : Node(level) {

    TextureHandle pp;
    TextureLoadOp load_op = renderer_TextureLoadOp(&pp, "assets/player/player_idle.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    rotation_root = new Node(level);
    AddChild(rotation_root);

    auto* texNode = new TextureNode(level, pp, 32, 32);
    rotation_root->AddChild(texNode);
    AddChild(new CameraNode(level));
}


void PlayerNode::Render(CommandBuffer* buffer)
{
    Node::Render(buffer);

#ifdef DEBUG
    renderer_PushBase(buffer, v2(0));

    V2 pos = GetAbsolutePosition();

    V2 facing = v2(0, -1);
    V2 side = v2(-facing.y, facing.x);
    V2 r = v2(0.5 * facing.x + 0.5 * side.x, 0.5 * facing.y + 0.5 * side.y);
    V2 l = v2(0.5 * facing.x - 0.5 * side.x, 0.5 * facing.y - 0.5 * side.y);

    float tr = game_Raycast(level, pos, r);
    float tl = game_Raycast(level, pos, l);

    MaskVertex test_mask[3] = {
        v2(0),
        v2(tr * r.x, tr * r.y),
        v2(tl * l.x, tl * l.y),
    };
    renderer_PushMaskOp(buffer, 3, test_mask);
    renderer_PushBase(buffer, level->camera.center);
#endif
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
        movement.x = -movement.x;
        rotation_root->rotation = movement.ToRad();
    }

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    Node::Update();
}
