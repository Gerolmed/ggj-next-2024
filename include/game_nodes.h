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
    explicit PlayerNode(Level* level);
    void PreUpdate() override;
    void Update() override;

private:
    AABB* aabb;
};



class TextureNode: public Node {
public:
    explicit TextureNode(Level* level, TextureHandle texture_handle, int width, int height)
        : Node(level), texture_handle(texture_handle), width(width), height(height) {
    }

    TextureHandle texture_handle;
    int width;
    int height;

    void Render(CommandBuffer* buffer) override;
};

class SpritesheetNode: public Node {
public:
    explicit SpritesheetNode(Level* level, TextureHandle texture_handle, int width, int height, int rows, int columns);

    TextureHandle texture_handle;
    int width;
    int height;
    const int rows;
    const int columns;
    int current_frame = 0;

    void Render(CommandBuffer* buffer) override;

private:
    float item_height;
    float item_width;
};

class AnimatedSpriteNode: public SpritesheetNode {
public:
    float seconds_per_frame = 1;
    int frame_count;

    explicit AnimatedSpriteNode(Level* level, TextureHandle texture_handle, int width, int height, int rows, int columns);


    void Update() override;
private:
    float currentDelta = 0;
};

#endif //GAME_NODES_H
