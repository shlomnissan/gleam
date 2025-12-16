/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/events/event.hpp"
#include "vglx/scene/node.hpp"

#include <memory>

namespace vglx {

/**
 * @brief Represents a scene hierarchy change event.
 *
 * A scene event is dispatched when a node is added to or removed from the
 * active scene. It extends the base @ref Event with data specific to scene
 * changes: the @ref SceneEvent::Type "change type" and the affected
 * @ref SceneEvent::node "node reference".
 *
 * This event type is used internally by the active @ref Scene to
 * track hierarchy changes and cannot be handled directly by individual
 * nodes.
 *
 * @ingroup EventsGroup
 */
struct VGLX_EXPORT SceneEvent : public Event {
    /**
     * @brief Enumerates all scene change types.
     *
     * Distinguishes between node additions and removals within the active
     * scene graph.
     */
    enum class Type {
        NodeAdded, ///< Node was added to the scene.
        NodeRemoved ///< Node was removed from the scene.
    };

    /**
     * @brief The node that was added or removed.
     *
     * Holds a shared reference to the affected node at the time the event
     * was generated.
     */
    std::shared_ptr<Node> node;

    /// @brief The @ref SceneEvent::Type "type" of this event.
    SceneEvent::Type type;

    /**
     * @brief Constructs a scene event.
     *
     * @param type The @ref SceneEvent::Type "type" of scene change.
     * @param node The @ref Node "node" that was affected.
     */
    SceneEvent(Type type, std::shared_ptr<Node> node) : type(type), node(node) {}

    /**
     * @brief Identifies this event as @ref Event::Type "Event::Type::Scene".
     */
    auto GetType() const -> Event::Type override {
        return Event::Type::Scene;
    }
};

}