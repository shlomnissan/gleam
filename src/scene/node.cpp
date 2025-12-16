/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/node.hpp"

#include "vglx/cameras/camera.hpp"

#include "events/event_dispatcher.hpp"
#include "utilities/logger.hpp"

#include <queue>
#include <ranges>

namespace vglx {

struct Node::Impl {
    std::vector<std::shared_ptr<Node>> children;

    Node* parent {nullptr};

    Matrix4 world_transform {1.0f};

    bool world_transform_touched {false};

    bool attached {false};
};

Node::Node() : impl_(std::make_unique<Impl>()) {};

auto Node::Add(const std::shared_ptr<Node>& node) -> void {
    if (node == nullptr) {
        Logger::Log(LogLevel::Error, "Attempting to add invalid node");
        return;
    }

    if (node->impl_->parent) {
        node->impl_->parent->Remove(node);
    }
    node->impl_->parent = this;
    impl_->children.emplace_back(node);

    EventDispatcher::Get().Dispatch(
        "node_added",
        std::make_unique<SceneEvent>(SceneEvent::Type::NodeAdded, node)
    );
}

auto Node::Remove(const std::shared_ptr<Node>& node) -> void {
    if (node == nullptr) {
        Logger::Log(LogLevel::Error, "Attempting to remove invalid node");
        return;
    }

    auto it = std::ranges::find(impl_->children, node);
    if (it != impl_->children.end()) {
        EventDispatcher::Get().Dispatch(
            "node_removed",
            std::make_unique<SceneEvent>(SceneEvent::Type::NodeRemoved, node)
        );
        impl_->children.erase(it);
        node->impl_->parent = nullptr;
        node->impl_->attached = false;
        node->transform.touched = true;
    }
}

auto Node::RemoveAllChildren() -> void {
    for (const auto& node : impl_->children) {
        EventDispatcher::Get().Dispatch(
            "node_removed",
            std::make_unique<SceneEvent>(SceneEvent::Type::NodeRemoved, node)
        );
        node->impl_->parent = nullptr;
        node->impl_->attached = false;
        node->transform.touched = true;
    }
    impl_->children.clear();
}

auto Node::Children() const -> const std::vector<std::shared_ptr<Node>>& {
    return impl_->children;
}

auto Node::IsChild(const Node* node) const -> bool {
    if (node == nullptr) {
        Logger::Log(LogLevel::Error, "Attempting to check child relationship of invalid node");
        return false;
    }

    auto to_process = std::queue<std::shared_ptr<Node>>();
    for (const auto& child : impl_->children) {
        to_process.push(child);
    }

    while (!to_process.empty()) {
        auto len = to_process.size();
        for (auto i = 0; i < len; ++i) {
            const auto current = to_process.front();
            to_process.pop();
            if (current.get() == node) return true;
            for (const auto& child : current->impl_->children) {
                to_process.push(child);
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

    for (const auto child : impl_->children) {
        if (child != nullptr) {
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