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

    std::vector<Node*> children = {};

    Mat3f GetRelativeMatrix() const;
    Mat3f GetAbsoluteMatrix() const;

    V2 GetAbsolutePosition();

    void AddChild(Node* child);
    void RemoveChild(Node* child);

    /**
     * \brief Call AddChild and RemoveChild to change parenting structure
     * \param parent
     */
    void SetParent(Node* parent);

    virtual void Start();

    virtual void Update();
    virtual void Render(CommandBuffer* buffer);

    virtual void Stop();
};

#endif //NODE_H
