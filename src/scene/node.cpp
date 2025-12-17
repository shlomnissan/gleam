/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/node.hpp"

#include "vglx/cameras/camera.hpp"

#include "events/event_dispatcher.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <algorithm>
#include <queue>
#include <ranges>

namespace vglx {

struct Node::Impl {
    std::vector<std::unique_ptr<Node>> children;

    Node* parent {nullptr};

    Matrix4 world_transform {1.0f};

    bool world_transform_touched {false};

    bool attached {false};
};

Node::Node() : impl_(std::make_unique<Impl>()) {};

auto Node::AddImpl(std::unique_ptr<Node> node) -> Node* {
    if (node == nullptr) {
        Logger::Log(LogLevel::Error, "Attempting to add invalid node");
        return nullptr;
    }

    VGLX_ASSERT(
        node->impl_->parent == nullptr,
        "Attempting to add a node already owned by another parent"
    );

    auto raw = node.get();

    VGLX_ASSERT(
        raw != this,
        "Cannot add node as a child of itself"
    );

    VGLX_ASSERT(
        !raw->IsChild(this),
        "Cannot add an ancestor as a child (cycle detected)"
    );

    raw->impl_->parent = this;
    impl_->children.emplace_back(std::move(node));

    EventDispatcher::Get().Dispatch(
        "node_added",
        std::make_unique<SceneEvent>(SceneEvent::Type::NodeAdded, raw)
    );

    return raw;
}

auto Node::Detach(Node* node) -> std::unique_ptr<Node> {
    if (node == nullptr) {
        Logger::Log(
            LogLevel::Error,
            "Attempting to detach invalid node"
        );
        return nullptr;
    }

    auto it = std::ranges::find_if(impl_->children, [node](const auto& child){
        return child.get() == node;
    });

    if (it == impl_->children.end()) {
        Logger::Log(
            LogLevel::Warning,
            "Attempting to detach non-child node"
        );
        return nullptr;
    }

    VGLX_ASSERT(
        node->impl_->parent == this,
        "Child list contains node with mismatched parent pointer"
    );

    EventDispatcher::Get().Dispatch(
        "node_removed",
        std::make_unique<SceneEvent>(SceneEvent::Type::NodeRemoved, node)
    );

    auto out_node = std::move(*it);
    impl_->children.erase(it);

    out_node->impl_->parent = nullptr;
    out_node->impl_->attached = false;
    out_node->transform.touched = true;

    return out_node;
}

auto Node::Remove(Node* node) -> void {
    (void)Detach(node);
}

auto Node::RemoveAllChildren() -> void {
    for (const auto& node : impl_->children) {
        EventDispatcher::Get().Dispatch(
            "node_removed",
            std::make_unique<SceneEvent>(SceneEvent::Type::NodeRemoved, node.get())
        );
        node->impl_->parent = nullptr;
        node->impl_->attached = false;
        node->transform.touched = true;
    }
    impl_->children.clear();
}

auto Node::Children() const -> std::span<const std::unique_ptr<Node>> {
    return impl_->children;
}

auto Node::IsChild(const Node* node) const -> bool {
    if (node == nullptr) {
        Logger::Log(
            LogLevel::Error,
            "Attempting to check child relationship of invalid node"
        );
        return false;
    }

    auto to_process = std::queue<Node*> {};
    for (const auto& child : Children()) {
        to_process.push(child.get());
    }

    while (!to_process.empty()) {
        auto len = to_process.size();
        for (auto i = 0; i < len; ++i) {
            const auto current = to_process.front();
            to_process.pop();
            if (current == node) return true;
            for (const auto& child : current->Children()) {
                to_process.push(child.get());
            }
        }
    }

    return false;
}

auto Node::Parent() const -> const Node* {
    return impl_->parent;
}

auto Node::UpdateTransformHierarchy() -> void {
    if (transform_auto_update && ShouldUpdateWorldTransform()) {
        impl_->world_transform = impl_->parent == nullptr
            ? transform.Get()
            : impl_->parent->impl_->world_transform * transform.Get();
        transform.touched = false;
        impl_->world_transform_touched = true;
    }

    for (const auto& child : Children()) {
        if (child) {
            child->UpdateTransformHierarchy();
        }
    }

    impl_->world_transform_touched = false;
}

auto Node::UpdateWorldTransform() -> void {
    if (impl_->parent != nullptr) {
        impl_->parent->UpdateWorldTransform();
    }

    if (ShouldUpdateWorldTransform()) {
        impl_->world_transform = impl_->parent == nullptr
            ? transform.Get()
            : impl_->parent->impl_->world_transform * transform.Get();
        transform.touched = false;
    }
}

auto Node::ShouldUpdateWorldTransform() const -> bool {
    return transform.touched || (impl_->parent && impl_->parent->impl_->world_transform_touched);
}

auto Node::GetWorldPosition() -> Vector3 {
    UpdateWorldTransform();
    auto& t = impl_->world_transform[3];
    return Vector3(t.x, t.y, t.z);
}

auto Node::GetWorldTransform() -> Matrix4 {
    if (transform_auto_update) {
        UpdateTransformHierarchy();
    }
    return impl_->world_transform;
}

Node::~Node() = default;

auto Node::LookAt(const Vector3& target) -> void {
    transform.LookAt(GetWorldPosition(), target, up);
}

auto Node::AttachRecursive(SharedContextPointer context) -> void {
    if (impl_->attached) return;

    OnAttached(context);
    impl_->attached = true;

    for (const auto& child : impl_->children) {
        if (child != nullptr) {
            child->AttachRecursive(context);
        }
    }
}

}