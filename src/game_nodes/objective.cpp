#include "include/game_nodes.h"
#include "include/input.h"
#include "include/opengl_renderer.h"

/////////////////////////////////
/// ObjectiveNode
/////////////////////////////////

ObjectiveNode::ObjectiveNode(Level* level, u8 value) : Node(level) {
    TextureHandle pp;
    TextureLoadOp load_op;

    switch(value) {
  case 5:
    load_op = renderer_TextureLoadOp(&pp, "assets/objectives/blue_vase.png");
    break;
  case 10:
    load_op = renderer_TextureLoadOp(&pp, "assets/objectives/red_vase.png");
    break;
  default:
    load_op = renderer_TextureLoadOp(&pp, "assets/objectives/green_vase.png");
    }    


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
