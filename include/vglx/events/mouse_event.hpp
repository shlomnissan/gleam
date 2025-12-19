/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/events/event.hpp"
#include "vglx/math/vector2.hpp"

namespace vglx {

enum class MouseButton;

/**
 * @brief Represents a mouse input event.
 *
 * A mouse event is dispatched when the cursor moves, a button is pressed or
 * released, or the scroll wheel is used. It extends the base @ref Event with
 * data specific to mouse input: the @ref MouseEvent::Type "interaction type",
 * the @ref MouseEvent::position "cursor position", and optional
 * @ref MouseEvent::button "button information".
 *
 * Mouse events are dispatched through the @ref Scene hierarchy where nodes can
 * override the @ref Node::OnMouseEvent handler and optionally mark the event
 * as @ref Event::handled "handled". When handled is set to `true` the event
 * stops propagating to other nodes.
 *
 * @code
 * class MyNode : public vglx::Node {
 * public:
 *   auto OnMouseEvent(vglx::MouseEvent* event) -> void override {
 *     if (event->type == vglx::MouseEvent::Type::ButtonPressed) {
 *       if (event->button == vglx::MouseButton::Left) {
 *         // Do something...
 *         event->handled = true; // stop propagation
 *       }
 *     }
 *   }
 * };
 * @endcode
 *
 * @ingroup EventsGroup
 */
struct VGLX_EXPORT MouseEvent : public Event {
    /**
     * @brief Enumerates all mouse event types.
     *
     * Distinguishes between cursor movement, button transitions, and scroll
     * actions. Engines that support continuous mouse tracking or gesture
     * recognition should implement those behaviors at a higher layer.
     */
    enum class Type {
        Moved, ///< Cursor moved.
        ButtonPressed, ///< Button transitioned to the down state.
        ButtonReleased, ///< Button transitioned to the up state.
        Scrolled ///< Scroll wheel moved.
    };

    /**
     * @brief Current cursor position in window coordinates.
     *
     * The coordinate origin is always the top-left corner of the window:
     * X increases to the right and Y increases downward.
     */
    Vector2 position;

    /**
     * @brief Scroll delta since the last scroll event.
     *
     * The delta is expressed in abstract scroll units normalized such that
     * positive Y values represent upward scrolling.
     */
    Vector2 scroll;

    /// @brief The interaction @ref MouseEvent::Type "type" for this event.
    MouseEvent::Type type;

    /**
     * @brief Mouse button associated with the event, if any.
     *
     * Identifies which button triggered the event using the mouse button
     * enumeration. Refer to the source code for enum details.
     */
    MouseButton button;

    /**
     * @brief Modifier keys active during this mouse event.
     *
     * Indicates which keyboard modifiers were held at the time the event was generated.
     * Multiple modifiers may be active depending on platform and input backend.
     * Refer to the source code for enum details.
     */
    int mods;

    /**
     * @brief Identifies this event as @ref Event::Type "Event::Type::Mouse".
     */
    auto GetType() const -> Event::Type override {
        return Event::Type::Mouse;
    }
};

enum class MouseButton {
    None,
    Left,
    Right,
    Middle
};

enum class MouseMod {
    Shift       = 1 << 0,
    Control     = 1 << 1,
    Alt         = 1 << 2,
    Super       = 1 << 3,
    CapsLock    = 1 << 4,
    NumLock     = 1 << 5
};

constexpr auto operator&(int lhs, MouseMod rhs) -> int {
    return lhs & static_cast<int>(rhs);
}

}