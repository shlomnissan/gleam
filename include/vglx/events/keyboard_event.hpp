/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/events/event.hpp"

namespace vglx {

enum class Key;

/**
 * @brief Represents a keyboard input event.
 *
 * A keyboard event is dispatched when a key is pressed or released. It extends
 * the base @ref Event with data specific to keyboard input: the @ref
 * KeyboardEvent::Type "interaction type" and the @ref KeyboardEvent::key "key code".
 *
 * Keyboard events are dispatched through the @ref Scene hierarchy where
 * nodes can override the @ref Node::OnKeyboardEvent handler and optionally
 * mark the event as @ref Event::handled "handled". When handled
 * is set to `true` the event stops propagating to other nodes.
 *
 * @code
 * class MyNode : public vglx::Node {
 * public:
 *   auto OnKeyboardEvent(vglx::KeyboardEvent* event) -> void override {
 *     if (event->type == vglx::KeyboardEvent::Type::Pressed) {
 *       if (event->key == vglx::Key::Space) {
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
struct VGLX_EXPORT KeyboardEvent : public Event {
    /**
     * @brief Enumerates all keyboard event types.
     *
     * Distinguishes between an initial press and a release. Engines that
     * support key-repeat should surface repeat behavior at a higher layer,
     * leaving this enum to represent the physical transitions only.
     */
    enum class Type {
        Pressed, ///< Key transitioned to the down state.
        Released ///< Key transitioned to the up state.
    };

    /// @brief The interaction @ref KeyboardEvent::Type "type" for this event.
    KeyboardEvent::Type type;

    /**
     * @brief Key code associated with the event.
     *
     * Identifies which key triggered the event using the key enumeration.
     * Refer to the source code for enum details.
     */
    Key key;

    /**
     * @brief Identifies this event as @ref Event::Type "Event::Type::Keyboard".
     */
    auto GetType() const -> Event::Type override {
        return Event::Type::Keyboard;
    }
};

enum class Key {
    None,
    Space,
    Apostrophe,
    Comma,
    Minus,
    Period,
    Slash,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    Semicolon,
    Equal,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,
    Backslash,
    RightBracket,
    GraveAccent,
    World1,
    World2,
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    Keypad0,
    Keypad1,
    Keypad2,
    Keypad3,
    Keypad4,
    Keypad5,
    Keypad6,
    Keypad7,
    Keypad8,
    Keypad9,
    KeypadDecimal,
    KeypadDivide,
    KeypadMultiply,
    KeypadSubtract,
    KeypadAdd,
    KeypadEnter,
    KeypadEqual,
    LeftShift,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu,
};

}