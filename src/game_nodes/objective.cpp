#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"

/////////////////////////////////
/// ObjectiveNode
/////////////////////////////////

ObjectiveNode::ObjectiveNode(Level* level) : Node(level) {
    TextureHandle pp;
    TextureLoadOp load_op = renderer_TextureLoadOp(&pp, "assets/objective.png");
    opengl_LoadTexture(&load_op);

    renderer_FreeTextureLoadOp(&load_op);

    auto* texNode = new TextureNode(level, pp, 100, 100);

    AddChild(texNode);

}

void ObjectiveNode::PreUpdate() {
    Node:: PreUpdate();

    Node::PreUpdate();
    aabb.position = position;
    aabb.size = { 32, 32 };
}

void ObjectiveNode::Update() {
    Node::Update();
}

void ObjectiveNode::Render(CommandBuffer* buffer){
    Node::Render(buffer);
}
