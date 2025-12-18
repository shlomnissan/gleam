/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/core/identity.hpp"
#include "vglx/core/shared_context.hpp"
#include "vglx/events/keyboard_event.hpp"
#include "vglx/events/mouse_event.hpp"
#include "vglx/math/matrix4.hpp"
#include "vglx/math/transform3.hpp"
#include "vglx/math/vector3.hpp"

#include <concepts>
#include <memory>
#include <span>

namespace vglx {

class Scene;

/**
 * @brief Base class for all scene graph nodes.
 *
 * Node represents a transformable object in the scene hierarchy. It manages
 * local and world transforms, parent–child relationships, and hooks for per-frame
 * updates and input events. All renderable, camera, and light objects derive
 * from this class and share the same hierarchical behavior.
 *
 * The scene graph is a tree: each node can have zero or more children and at
 * most one parent. Transforms always propagate downward, changing a parent
 * updates the world-space transforms of all descendants. Input events propagate
 * bottom-up through the scene graph: children receive the event before their
 * parents, and any node can stop propagation by marking the event as handled.
 *
 * Nodes can opt out of automatic transform updates via @ref transform_auto_update
 * or disable visibility tests using @ref frustum_culled when needed.
 *
 * @ingroup SceneGroup
 */
class VGLX_EXPORT Node : public Identity {
public:
    /**
     * @brief Enumerates all node categories.
     *
     * Used by the engine and renderer to distinguish between cameras, meshes,
     * lights, and generic nodes. Custom node types typically reuse one of the
     * existing categories.
     */
    enum class Type {
        Camera, ///< Perspective or orthographic camera.
        Default, ///< Generic node without special behavior.
        InstancedMesh, ///< Node containing instanced meshes.
        Light, ///< Light source (directional, point, or spot).
        Mesh, ///< Single mesh with an associated material.
        Renderable, ///< Any node that can be rendered to the screen.
        Scene, ///< Root of a scene hierarchy.
        Sprite ///< Billboarded sprite.
    };

    /// @brief Local transform (position, rotation, scale) of this node.
    Transform3 transform;

    /// @brief Local up direction used by helpers such as @ref LookAt.
    Vector3 up {Vector3::Up()};

    /// @brief When `true` the world transform is automatically updated each frame.
    bool transform_auto_update {true};

     /// @brief When `true` this node participates in view frustum culling.
    bool frustum_culled {true};

    /**
     * @brief Constructs a node.
     */
    Node();

    /**
     * @brief Creates an instance of @ref Node.
     */
    [[nodiscard]] static auto Create() -> std::unique_ptr<Node> {
        return std::make_unique<Node>();
    }

    /**
     * @brief Returns the node's type identifier.
     *
     * Subclasses override this to report their specific type. The default is
     * @ref Node::Type "Node::Type::Default".
     */
    [[nodiscard]] virtual auto GetNodeType() const -> Node::Type {
        return Node::Type::Default;
    }

    /**
     * @brief Returns whether this node is renderable.
     *
     * Renderable subclasses (such as @ref Mesh and @ref InstancedMesh) override
     * this and return `true`. The base implementation always returns `false`.
     */
    [[nodiscard]] virtual auto IsRenderable() const -> bool {
        return false;
    }

    /**
     * @name Hierarchy
     * @{
     */

    /**
     * @brief Adds a child node to this node and returns a non-owning reference.
     *
     * This overload transfers ownership of `node` into this node’s children list.
     * The scene graph is the sole owner of all nodes. The returned pointer is a
     * non-owning reference that remains valid only while the node is attached to
     * the scene graph.
     *
     * @param node Node to attach. Ownership is transferred.
     *
     * @warning The returned pointer becomes invalid if the node is removed from the
     * scene graph or if the owning scene is destroyed.
     */
    template <typename T>
    requires std::derived_from<T, Node>
    auto Add(std::unique_ptr<T> node) -> T* {
        return static_cast<T*>(AddImpl(std::unique_ptr<Node>(std::move(node))));
    }

    /**
     * @brief Detaches a direct child node from this node and returns ownership.
     *
     * Removes `node` from this node’s children list without destroying it and
     * returns the owned subtree as a `std::unique_ptr`. The detached node’s parent
     * pointer is cleared, its attached state is reset, and its transform is marked dirty.
     *
     * @param node Direct child node to detach.
     */
    template <typename T>
    requires std::derived_from<T, Node>
    [[nodiscard]] auto Detach(T* node) -> std::unique_ptr<T> {
        std::unique_ptr<Node> base = DetachImpl(node);
        return std::unique_ptr<T>(static_cast<T*>(base.release()));
    }

    /**
     * @brief Removes a direct child node from this node and destroys it.
     *
     * If `node` exists in the children list it is detached and destroyed as part
     * of removing its owning `std::unique_ptr`.
     *
     * @param node Direct child node to remove.
     *
     * @warning Any external pointers or references to the removed node become invalid
     * immediately after this call.
     */
    auto Remove(Node* node) -> void;

    /**
     * @brief Removes all children from this node.
     *
     * Each child receives a @ref SceneEvent::Type "SceneEvent::NodeRemoved"
     * event, has its parent pointer cleared, its `attached` flag reset, and
     * its transform marked dirty. After all children are processed, the
     * children list is emptied.
     */
    auto RemoveAllChildren() -> void;

    /**
     * @brief Recursively updates world transforms for this node and its descendants.
     *
     * If `transform_auto_update` is enabled and the node’s world transform is
     * dirty, it is recomputed from the parent’s world transform (or from the local
     * transform if this node is a root). The world transform is then marked clean
     * for this update cycle. The method then recurses into each child.
     *
     * This is the primary mechanism used by the renderer and scene step to update
     * transform propagation across the entire hierarchy.
     */
    auto UpdateTransformHierarchy() -> void;

    /**
     * @brief Ensures this node’s world transform is up to date.
     *
     * If the parent’s world transform may be outdated, the method updates the
     * parent first. Then, if this node's transform is dirty, the world transform
     * is recomputed without affecting siblings or children. This method does not
     * recurse into children, unlike @ref UpdateTransformHierarchy.
     *
     * This is typically used when querying world-space properties on a single node.
     */
    auto UpdateWorldTransform() -> void;

    /**
     * @brief Returns a view of this node’s direct children.
     *
     * The returned span exposes read-only access to the owning node's
     * `std::unique_ptr<Node>` objects for each child. Ownership
     * is retained by the scene graph.
     *
     * @note This function exposes the internal storage type used
     * by the scene graph. It is intended for inspection and
     * iteration only.
     */
    [[nodiscard]] auto Children() const -> std::span<const std::unique_ptr<Node>>;

    /**
     * @brief Checks whether the given node exists anywhere in this node’s subtree.
     *
     * The search is breadth-first: all descendants (not just direct children) are
     * examined.
     *
     * @param node Node to test for membership in the subtree.
     */
    [[nodiscard]] auto IsChild(const Node* node) const -> bool;

    /**
     * @brief Returns this node’s parent.
     */
    [[nodiscard]] auto Parent() const -> const Node*;

    [[nodiscard]] auto GetScene() const -> const Scene*;

    /**
     * @brief Returns whether this node’s world transform must be recomputed.
     *
     * A world transform is considered dirty if either the local transform has been
     * modified, or if the parent’s world transform was updated during the current
     * update cycle.
     */
    [[nodiscard]] auto ShouldUpdateWorldTransform() const -> bool;

    /**
     * @brief Returns the node’s world-space position.
     *
     * Ensures the world transform is current via @ref UpdateWorldTransform, then
     * extracts the translation column of the world matrix.
     */
    [[nodiscard]] auto GetWorldPosition() -> Vector3;

    /**
     * @brief Returns the node’s world transform matrix.
     *
     * If transform auto-updates are enabled, calls @ref UpdateTransformHierarchy
     * to refresh this node and all descendants. Otherwise, returns the cached
     * world transform as-is.
     */
    [[nodiscard]] auto GetWorldTransform() -> Matrix4;

    /// @}

    /**
     * @name Transformations
     * @{
     */

    /**
     * @brief Rotates the node so its forward direction points at the target.
     *
     * @param target World-space point to look at.
     */
    virtual auto LookAt(const Vector3& target) -> void;

    /**
     * @brief Translates the node along its local X axis.
     *
     * @param value Offset in local X.
     */
    auto TranslateX(float value) -> void { transform.Translate({value, 0.0f, 0.0f}); }

    /**
     * @brief Translates the node along its local Y axis.
     *
     * @param value Offset in local Y.
     */
    auto TranslateY(float value) -> void { transform.Translate({0.0f, value, 0.0f}); }

    /**
     * @brief Translates the node along its local Z axis.
     *
     * @param value Offset in local Z.
     */
    auto TranslateZ(float value) -> void { transform.Translate({0.0f, 0.0f, value}); }

    /**
     * @brief Rotates the node around its local X axis.
     *
     * @param angle Angle in radians.
     */
    auto RotateX(float angle) -> void { transform.Rotate(Vector3::Right(), angle); }

    /**
     * @brief Rotates the node around its local Y axis.
     *
     * @param angle Angle in radians.
     */
    auto RotateY(float angle) -> void { transform.Rotate(Vector3::Up(), angle); }

    /**
     * @brief Rotates the node around its local Z axis.
     *
     * @param angle Angle in radians.
     */
    auto RotateZ(float angle) -> void { transform.Rotate(Vector3::Forward(), angle); }

    /**
     * @brief Sets the local scale of the node.
     *
     * @param value New scale vector.
     */
    auto SetScale(const Vector3& value) -> void { transform.SetScale(value); }

    /// @}

    /**
     * @name Event hooks
     * @{
     */

    /**
     * @brief Per-frame update callback.
     *
     * Called once per frame with the elapsed time since the last frame in
     * seconds. Override this to implement node behavior.
     *
     * @param delta Time step in seconds.
     */
    virtual auto OnUpdate(float delta) -> void {}

    /**
     * @brief Called when the node is attached to a scene context.
     *
     * Override this to perform initialization that depends on the
     * application's @ref SharedContext "shared context".
     *
     * @param context Shared engine context.
     */
    virtual auto OnAttached(SharedContextPointer context) -> void {}

    /**
     * @brief Keyboard event handler.
     *
     * Override this to react to keyboard events. Events can be marked as
     * handled to stop propagation.
     *
     * @param event Keyboard event pointer.
     */
    virtual auto OnKeyboardEvent(KeyboardEvent* event) -> void {}

    /**
     * @brief Mouse event handler.
     *
     * Override this to react to cursor, button, or scroll events. Events can
     * be marked as handled to stop propagation.
     *
     * @param event Mouse event pointer.
     */
    virtual auto OnMouseEvent(MouseEvent* event) -> void {}

    /// @}

    virtual ~Node();

private:
    /// @cond INTERNAL
    class Impl;
    std::unique_ptr<Impl> impl_;

    [[nodiscard]] auto AddImpl(std::unique_ptr<Node> node) -> Node*;
    [[nodiscard]] auto DetachImpl(Node* node) -> std::unique_ptr<Node>;

    friend class Scene;
    auto AttachSubtree(Scene* scene, SharedContextPointer context) -> void;
    auto DetachSubtree() -> void;
    /// @endcond
};

}