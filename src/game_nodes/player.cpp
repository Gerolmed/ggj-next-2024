
#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"
#include "include/time.h"

/////////////////////////////////
/// PlayerNode
/////////////////////////////////

PlayerNode::PlayerNode(Level* level) : Node(level) {

    TextureHandle player_idle;
    TextureLoadOp load_op = renderer_TextureLoadOp(&player_idle, "assets/player/player_idle.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    TextureHandle player_walk;
    load_op = renderer_TextureLoadOp(&player_walk, "assets/player/player_walk.png");
    opengl_LoadTexture(&load_op);
    renderer_FreeTextureLoadOp(&load_op);

    rotation_root = new Node(level);
    AddChild(rotation_root);

    idle_node = new TextureNode(level, player_idle, 32, 32);
    rotation_root->AddChild(idle_node);
    auto* walk_node_temp = new AnimatedSpriteNode(level, player_walk, 32, 32, 1, 4);
    walk_node_temp->seconds_per_frame = 0.1f;
    walk_node = walk_node_temp;
    walk_node->visible = false;
    rotation_root->AddChild(walk_node);
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
    float fov = 0.2;
    V2 r = v2((1 - fov) * facing.x + fov * side.x, (1 - fov) * facing.y + fov * side.y);
    V2 l = v2((1 - fov) * facing.x - fov * side.x, (1 - fov) * facing.y - fov * side.y);

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
        walk_node->visible = true;
        idle_node->visible = false;
        rotation_root->rotation = movement.ToRad();
    } else {
        walk_node->visible = false;
        idle_node->visible = true;
    }

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    Node::Update();
}
