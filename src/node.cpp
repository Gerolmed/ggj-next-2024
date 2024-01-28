#include "include/node.h"
#include <algorithm>

Node::Node(Level* level): level(level) {

}
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

Mat2f Node::GetAbsoluteRotationMatrix() const {
    if (parent == nullptr) return rotationMatrix2f(rotation);
    return parent->GetAbsoluteRotationMatrix() * rotationMatrix2f(rotation);
}
float Node::GetAbsoluteRotation() const {
    if (parent == nullptr) return rotation;
    return parent->GetAbsoluteRotation() + rotation;
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

V2 Node::GetAbsolutePosition() const {
    constexpr V2 localPos = {0,0};
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


void Node::TryUpdate() {
    if(!updating) return;
    Update();
}
void Node::TryPreUpdate() {
    if(!updating) return;
    PreUpdate();
}
void Node::Update() {
    for (const auto& child : children) {
        child->TryUpdate();
    }
}
void Node::PreUpdate() {
    for (const auto& child : children) {
        child->TryPreUpdate();
    }
}

void Node::TryRender(CommandBuffer* buffer) {
    if(!visible) return;
    Render(buffer);
}
void Node::Render(CommandBuffer* buffer) {
    for (const auto& child : children) {
        child->TryRender(buffer);
    }
}


void Node:: CollisionResponse(){}


