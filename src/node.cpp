#include "include/node.h"

Node::Node() = default;
Node::~Node() {
    for (int i = children.size() - 1; i >= 0; --i) {
        auto child = children[i];
        RemoveChild(child);
        delete child;
    }
}



Mat3f Node::GetAbsoluteMatrix() const {
    if (parent == nullptr) return GetRelativeMatrix();
    return parent->GetAbsoluteMatrix() * GetRelativeMatrix();
}

Mat3f Node::GetRelativeMatrix() const {
    float s = sinf(rotation);
    float c = cosf(rotation);
    return Mat3f{{
               c, -s, position.x,
               s, c, position.y,
               0.0f, 0.0f, 1.0f
           }};
}

V2 Node::GetAbsolutePosition() {
    V2 localPos = {0,0};
    return GetAbsoluteMatrix() * localPos;
}



void Node::AddChild(Node* child) {
    children.push_back(child);

    if(started != child->started) {
        if(started) {
            child->Start();
        } else {
            child->Stop();
        }
    }

    if(child->parent != this) child->SetParent(this);
}

void Node::RemoveChild(Node* child) {
    if(child->started) {
        child->Stop();
    }

    const auto value_position = std::find(children.begin(), children.end(), child);
    if (value_position != children.end()) // == myVector.end() means the element was not found
        children.erase(value_position);
}



void Node::SetParent(Node* parent) {
    this->parent = parent;
}


void Node::Start() {
    for (const auto& child : children) {
        child->Start();
    }
}
void Node::Stop() {
    for (const auto& child : children) {
        child->Stop();
    }
}


void Node::Update() {
    for (const auto& child : children) {
        child->Update();
    }
}

void Node::Render() {
    for (const auto& child : children) {
        child->Render();
    }
}



