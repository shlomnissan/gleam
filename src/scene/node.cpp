/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/node.hpp"

#include "vglx/cameras/camera.hpp"
#include "vglx/scene/scene.hpp"

#include "events/event_dispatcher.hpp"
#include "utilities/assert.hpp"
#include "utilities/logger.hpp"

#include <algorithm>
#include <queue>
#include <ranges>

namespace vglx {

struct Node::Impl {
    std::vector<std::unique_ptr<Node>> children;

    Scene* scene {nullptr};

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

    if (impl_->attached && impl_->scene) {
        raw->AttachSubtree(impl_->scene, impl_->scene->GetContext());
    }

    return raw;
}

auto Node::DetachImpl(Node* node) -> std::unique_ptr<Node> {
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

    auto out_node = std::move(*it);
    impl_->children.erase(it);

    out_node->DetachSubtree();
    out_node->impl_->parent = nullptr;
    out_node->transform.touched = true;

    return out_node;
}

auto Node::Remove(Node* node) -> void {
    (void)DetachImpl(node);
}

auto Node::RemoveAllChildren() -> void {
    for (auto& child : impl_->children) {
        child->DetachSubtree();
        child->impl_->parent = nullptr;
        child->transform.touched = true;
    }
    impl_->children.clear();
}

auto Node::GetChildren() const -> std::span<const std::unique_ptr<Node>> {
    return impl_->children;
}

auto Node::GetChild(std::string_view name) const -> Node* {
    for (const auto& child : impl_->children) {
        if (child->Name() == name) return child.get();
    }
    return nullptr;
}

auto Node::IsChild(const Node* node) const -> bool {
    if (node == nullptr) {
        return false;
    }

    auto to_process = std::queue<Node*> {};
    for (const auto& child : GetChildren()) {
        VGLX_ASSERT(child != nullptr, "Null child in children list");
        to_process.push(child.get());
    }

    while (!to_process.empty()) {
        auto len = to_process.size();
        for (size_t i = 0; i < len; ++i) {
            const auto current = to_process.front();
            to_process.pop();
            if (current == node) return true;
            for (const auto& child : current->GetChildren()) {
                VGLX_ASSERT(child != nullptr, "Null child in children list");
                to_process.push(child.get());
            }
        }
    }

    return false;
}

auto Node::GetParent() const -> const Node* {
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

    for (const auto& child : GetChildren()) {
        VGLX_ASSERT(child != nullptr, "Null child in children list");
        child->UpdateTransformHierarchy();
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

auto Node::GetScene() const -> const Scene* {
    return impl_->scene;
}

auto Node::LookAt(const Vector3& target) -> void {
    transform.LookAt(GetWorldPosition(), target, up);
}

auto Node::AttachSubtree(Scene* scene, SharedContextPointer context) -> void {
    if (impl_->attached) return;
    impl_->attached = true;
    impl_->scene = scene;
    OnAttached(context);
    for (const auto& child : impl_->children) {
        VGLX_ASSERT(child != nullptr, "Null child in children list");
        child->AttachSubtree(scene, context);
    }
}

auto Node::DetachSubtree() -> void {
    if (!impl_->attached) return;
    impl_->attached = false;
    impl_->scene = nullptr;
    transform.touched = true;
    for (auto& child : impl_->children) child->DetachSubtree();
}

Node::~Node() = default;

}