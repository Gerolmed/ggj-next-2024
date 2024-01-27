#ifndef GAME_NODES_H
#define GAME_NODES_H
#include "node.h"

class TestNode final : public Node {
public:
    explicit TestNode(Level* level)
        : Node(level) {
    }
    void Update() override;
};


class PlayerNode : public Node {
public:
    explicit PlayerNode(Level* level)
        : Node(level) {
    }
    void PreUpdate() override;
    void Update() override;

private:
    AABB* aabb;
}



class TextureNode: public Node {
public:
    explicit TextureNode(Level* level, TextureHandle* texture_handle, int width, int height)
        : Node(level), texture_handle(texture_handle), width(width), height(height) {
    }

    TextureHandle* texture_handle;
    int width;
    int height;

    void Render(CommandBuffer* buffer) override;
};

#endif //GAME_NODES_H
