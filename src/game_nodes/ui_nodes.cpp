#include "include/constants.h"
#include "include/game_nodes.h"

void CanvasNode::Render(CommandBuffer* buffer) {
    renderer_PushBase(buffer, v2(HALF_GAME_WIDTH, HALF_GAME_HEIGHT));
    Node::Render(buffer);
    renderer_PushBase(buffer, level->camera.center);
}
