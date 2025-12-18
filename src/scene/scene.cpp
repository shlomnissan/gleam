/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/scene/scene.hpp"

#include "events/event_dispatcher.hpp"
#include "utilities/logger.hpp"

namespace vglx {

namespace {

auto handle_node_updates(Node* node, float delta) -> void {
    node->OnUpdate(delta);
    for (const auto& child : node->Children()) {
        handle_node_updates(child.get(), delta);
    }
}

auto handle_input_event(Node* node, Event* event) -> void {
    using enum Event::Type;

    // Events are propagated from the bottom of the scene graph to the top.
    // This allows nodes at the bottom of the graph to mark events as handled
    // and prevent them from being processed by parent nodes.
    for (const auto& child : node->Children()) {
        if (event->handled) return;
        handle_input_event(child.get(), event);
    }

    const auto type = event->GetType();
    if (type == Keyboard)
        node->OnKeyboardEvent(static_cast<KeyboardEvent*>(event));
    if (type == Mouse)
        node->OnMouseEvent(static_cast<MouseEvent*>(event));
}

}

struct Scene::Impl {
    std::shared_ptr<EventListener> event_listener;
    SharedContextPointer context {nullptr};
};

Scene::Scene() : impl_(std::make_unique<Impl>()) {
    using enum Event::Type;

    impl_->event_listener = std::make_shared<EventListener>([&](Event* event) {
        auto type = event->GetType();

        if (type == Keyboard || type == Mouse) {
            if (type == Keyboard) OnKeyboardEvent(static_cast<KeyboardEvent*>(event));
            if (type == Mouse) OnMouseEvent(static_cast<MouseEvent*>(event));
            for (const auto& child : Children()) {
                handle_input_event(child.get(), event);
            }
        }

        if (type == Event::Type::Scene) {
            auto e = static_cast<SceneEvent*>(event);
            if (e->type == SceneEvent::Type::NodeAdded && IsChild(e->node)) {
                e->node->AttachRecursive(impl_->context);
            }
        }
    });

    EventDispatcher::Get().AddEventListener("node_added", impl_->event_listener);
    EventDispatcher::Get().AddEventListener("node_removed", impl_->event_listener);
    EventDispatcher::Get().AddEventListener("keyboard_event", impl_->event_listener);
    EventDispatcher::Get().AddEventListener("mouse_event", impl_->event_listener);
}

auto Scene::Advance(float delta) -> void {
    OnUpdate(delta);
     for (const auto& child : Children()) {
        handle_node_updates(child.get(), delta);
    }
}

auto Scene::SetContext(SharedContextPointer context) -> void {
    impl_->context = context;
    this->AttachRecursive(context);
}

Scene::~Scene() {
    EventDispatcher::Get().RemoveEventListener("node_added", impl_->event_listener);
    EventDispatcher::Get().RemoveEventListener("node_removed", impl_->event_listener);
    EventDispatcher::Get().RemoveEventListener("keyboard_event", impl_->event_listener);
    EventDispatcher::Get().RemoveEventListener("mouse_event", impl_->event_listener);
}

}