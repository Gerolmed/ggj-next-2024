#include "include/game_nodes.h"
#include "include/time.h"

/////////////////////////////////
/// TestNode
/////////////////////////////////
void TestNode::Update() {
    Node::Update();

    // V2 localPos = GetAbsolutePosition();
    // printf("Update test (X: %f, Y: %f)\n", localPos.x, localPos.y);
    // printf("Update test (Pressed: %d, Up: %d, Down: %d)\n", input_KeySpace.is_pressed, input_KeySpace.up, input_KeySpace.down);
}


/////////////////////////////////
/// RotatingNode
/////////////////////////////////

void RotatingNode::Update() {
    Node::Update();
    rotation += time_deltatime * speed;
    if(rotation >= PI * 2) {
        rotation -= PI * 2;
    }
}

void CameraNode::Update() {
    level->camera.center = GetAbsolutePosition();
}