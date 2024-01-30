#ifndef GAME_NODES_H
#define GAME_NODES_H
#include <string>

#include "node.h"

class TestNode final : public Node {
public:
    explicit TestNode(Level* level)
        : Node(level) {
    }
    void Update() override;
};

class CameraNode final : public Node {
public:
    explicit CameraNode(Level* level)
        : Node(level) {
    }
    void Update() override;
};


class PlayerNode : public Node {
public:
    explicit PlayerNode(Level* level);
    void PreUpdate() override;
    void Update() override;
    void Render(CommandBuffer* buffer) override;
    
    AABB aabb;

private:
Node* rotation_root;
Node* idle_node;
    Node* walk_node;
};


extern PlayerNode* global_player_pointer;


class EnemyNode : public Node {
public:
    explicit EnemyNode(Level* level);
    void PreUpdate() override;
    void Update() override;
    void Render(CommandBuffer* buffer);
private:
    AABB aabb;
};



class RotatingNode final : public Node {
public:
    explicit RotatingNode(Level* level)
        : Node(level) {
    }

    float speed = 1;

    void Update() override;
};



class CanvasNode final: public Node {
public:
    explicit CanvasNode(Level* level)
        : Node(level) {
    }

void Render(CommandBuffer* buffer) override;

};

class TextNode : public Node {
public:
    explicit TextNode(Level* level, Font* font, std::string str);

void Render(CommandBuffer* buffer) override;

protected:
    Font* font;
    std::string str;

};

class ScoreNode final: public TextNode {
public:
    explicit ScoreNode(Level* level, Font* font)
        : TextNode(level, font, "Score: 0") {}

    void Update() override;
};

class CountDownNode final: public TextNode {
public:
    explicit CountDownNode(Level* level, Font* font);

    void Update() override;

private:
    float remaining_time;
};


class TextureNode: public Node {
public:
    explicit TextureNode(Level* level, TextureHandle texture_handle, int width, int height)
        : Node(level), texture_handle(texture_handle), width(width), height(height) {
    }

int depth = 10;
    TextureHandle texture_handle;
    int width;
    int height;

    void Render(CommandBuffer* buffer) override;
};

class SpritesheetNode: public Node {
public:
    explicit SpritesheetNode(Level* level, TextureHandle texture_handle, int width, int height, int rows, int columns);

int depth = 10;
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






class ObjectiveNode : public Node, public CollisionHandler {
public:
    u8 value;
    explicit ObjectiveNode(Level* level, u8 value);
    void PreUpdate() override;
    void Update() override;
    void Render(CommandBuffer* buffer) override;

    void OnCollide(Collider* collider) override;

private:
    AABB aabb;
    SpritesheetNode* spritesheet_node;
};


#endif //GAME_NODES_H
