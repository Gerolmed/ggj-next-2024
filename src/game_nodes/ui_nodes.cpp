#include "include/constants.h"
#include "include/game_nodes.h"

void CanvasNode::Render(CommandBuffer* buffer) {
    renderer_PushBase(buffer, v2(HALF_GAME_WIDTH, HALF_GAME_HEIGHT));
    Node::Render(buffer);
    renderer_PushBase(buffer, level->camera.center);
}

TextNode::TextNode(Level* level, Font* font, char* str): Node(level), font(font), str(str) {
}


void TextNode::Render(CommandBuffer* buffer) {
    Node::Render(buffer);
    renderer_PushString(buffer, font, str, 
                        GetAbsolutePosition(), DEPTH_UI);
}
