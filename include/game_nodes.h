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


class TextureNode: public Node {
public:
    explicit TextureNode(Level* level)
        : Node(level) {
    }

    void Render(CommandBuffer* buffer) override;
};

#endif //GAME_NODES_H
