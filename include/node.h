#ifndef NODE_H
#define NODE_H

#include <vector>

#include "game.h"
#include "renderer.h"
#include "include/types.h"

class Node {

public:
    virtual ~Node();
    explicit Node(Level* level);

    Level* level;
    Node* parent = nullptr;
	V2 position = V2();
    float rotation = 0;
    bool started = false;
    bool visible = true;
    bool updating = true;

    std::vector<Node*> children = {};

    Mat3f GetRelativeMatrix() const;
    Mat3f GetAbsoluteMatrix() const;
    Mat2f GetAbsoluteRotationMatrix() const;

    V2 GetAbsolutePosition() const;

    void AddChild(Node* child);
    void RemoveChild(Node* child);

    /**
     * \brief Call AddChild and RemoveChild to change parenting structure
     * \param parent
     */
    void SetParent(Node* parent);

    virtual void Start();

    void TryPreUpdate();
    void TryUpdate();
    void TryRender(CommandBuffer* buffer);

    virtual void Stop();
protected:

    virtual void PreUpdate();
    virtual void Update();
    virtual void Render(CommandBuffer* buffer);
};

#endif //NODE_H
