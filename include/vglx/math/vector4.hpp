/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include "vglx/math/utilities.hpp"

#include <algorithm>
#include <cassert>

namespace vglx {

struct Vector4;
auto constexpr Dot(const Vector4& a, const Vector4& b) -> float;

/**
 * @brief Represents a 4D floating-point vector.
 *
 * Vector4 stores four components `(x, y, z, w)` and is typically used for
 * homogeneous coordinates, and general 4D math. It provides basic arithmetic,
 * and utility helpers.
 *
 * @ingroup MathGroup
 */
struct VGLX_EXPORT Vector4 {
    /// @brief X component.
    float x;
    /// @brief Y component.
    float y;
    /// @brief Z component.
    float z;
    /// @brief W component.
    float w;

    /**
     * @brief Constructs an uninitialized vector.
     */
    constexpr Vector4() = default;

    /**
     * @brief Constructs a vector with all components set to the same value.
     *
     * @param value Value to assign to all four components.
     */
    constexpr Vector4(float value) : Vector4(value, value, value, value) {}

    /**
     * @brief Constructs a vector from individual components.
     *
     * @param x X component.
     * @param y Y component.
     * @param z Z component.
     * @param w W component.
     */
    constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    /**
     * @brief Returns the zero vector.
     */
    [[nodiscard]] static constexpr auto Zero() -> Vector4 { return {0.0f}; }

    /**
     * @brief Computes the vector length.
     */
    [[nodiscard]] constexpr auto Length() const -> float { return math::Sqrt(Dot(*this, *this)); }

    /**
     * @brief Computes the squared vector length.
     *
     * Useful when comparing lengths without paying the cost of a square root.
     */
    [[nodiscard]] constexpr auto LengthSquared() const -> float { return Dot(*this, *this); }

    /**
     * @brief Accesses a component by index.
     *
     * @param i Index: `0 → x`, `1 → y`, `2 → z`, `3 → w`.
     */
    [[nodiscard]] constexpr auto operator[](int i) -> float& {
        assert(i >= 0 && i < 4);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return x; // unreachable
        }
    }

    /**
     * @brief Accesses a component by index.
     *
     * @param i Index: `0 → x`, `1 → y`, `2 → z`, `3 → w`.
     */
    [[nodiscard]] constexpr auto operator[](int i) const -> float {
        assert(i >= 0 && i < 4);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return x; // unreachable
        }
    }

    /**
     * @brief Adds another vector in-place.
     *
     * @param v Vector to add.
     */
    constexpr auto operator+=(const Vector4& v) -> Vector4& {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    /**
     * @brief Subtracts another vector in-place.
     *
     * @param v Vector to subtract.
     */
    constexpr auto operator-=(const Vector4& v) -> Vector4& {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    /**
     * @brief Multiplies the vector by a scalar in-place.
     *
     * @param n Scalar value.
     */
    constexpr auto operator*=(float n) -> Vector4& {
        x *= n;
        y *= n;
        z *= n;
        w *= n;
        return *this;
    }

    /**
     * @brief Multiplies the vector component-wise by another vector in-place.
     *
     * @param v Vector to multiply.
     */
    constexpr auto operator*=(const Vector4& v) -> Vector4& {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    /**
     * @brief Applies a component-wise minimum.
     *
     * @param v Vector to compare against.
     */
    constexpr auto Min(const Vector4& v) -> Vector4& {
        x = std::min(x, v.x);
        y = std::min(y, v.y);
        z = std::min(z, v.z);
        w = std::min(w, v.w);
        return *this;
    };

    /**
     * @brief Applies a component-wise maximum.
     *
     * @param v Vector to compare against.
     */
    constexpr auto Max(const Vector4& v) -> Vector4& {
        x = std::max(x, v.x);
        y = std::max(y, v.y);
        z = std::max(z, v.z);
        w = std::max(w, v.w);
        return *this;
    };

    /**
     * @brief Normalizes the vector in-place.
     *
     * If the length is zero, the vector is unchanged.
     */
    constexpr auto Normalize() -> Vector4& {
        const auto len = Length();
        return len == 0.0f ? *this : (*this *= (1.0f / len));
    }

    /**
     * @brief Compares two vectors for equality.
     */
    constexpr auto operator==(const Vector4&) const -> bool = default;
};

/**
 * @brief Adds two 4D vectors.
 * @related Vector4
 *
 * @param a First vector.
 * @param b Second vector.
 */
[[nodiscard]] constexpr auto operator+(const Vector4& a, const Vector4& b) -> Vector4 {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

/**
 * @brief Subtracts one 4D vector from another.
 * @related Vector4
 *
 * @param a First vector.
 * @param b Second vector.
 */
[[nodiscard]] constexpr auto operator-(const Vector4& a, const Vector4& b) -> Vector4 {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

/**
 * @brief Multiplies a vector by a scalar.
 * @related Vector4
 *
 * @param v Input vector.
 * @param n Scalar value.
 */
[[nodiscard]] constexpr auto operator*(const Vector4& v, float n) -> Vector4 {
    return {v.x * n, v.y * n, v.z * n, v.w * n};
}

/**
 * @brief Multiplies a scalar by avector.
 * @related Vector4
 *
 * @param n Scalar value.
 * @param v Input vector.
 */
[[nodiscard]] constexpr auto operator*(float n, const Vector4& v) -> Vector4 {
    return v * n;
}

/**
 * @brief Multiplies two vectors component-wise.
 * @related Vector4
 *
 * @param a First vector.
 * @param b Second vector.
 */
[[nodiscard]] constexpr auto operator*(const Vector4& a, const Vector4& b) -> Vector4 {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

/**
 * @brief Divides a vector by a scalar.
 * @related Vector4
 *
 * @param v Input vector.
 * @param n Scalar value.
 */
[[nodiscard]] constexpr auto operator/(const Vector4& v, float n) -> Vector4 {
    n = 1.0f / n;
    return {v.x * n, v.y * n, v.z * n, v.w * n};
}

/**
 * @brief Computes the dot product of two 4D vectors.
 * @related Vector4
 *
 * Computes the scalar product ($a_x b_x + a_y b_y + a_z b_z + a_w b_w$),
 * which measures how aligned the two vectors are.
 *
 * @param a First vector.
 * @param b Second vector.
 */
[[nodiscard]] constexpr auto Dot(const Vector4& a, const Vector4& b) -> float {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/**
 * @brief Linearly interpolates between two 4D vectors.
 * @related Vector4
 *
 * @param v1 Start vector.
 * @param v2 End vector.
 * @param f Interpolation factor in $[0, 1]$.
 */
[[nodiscard]] constexpr auto Lerp(const Vector4& v1, const Vector4& v2, float f) {
    return v1 + (v2 - v1) * f;
}

/**
 * @brief Returns a normalized copy of a vector.
 * @related Vector4
 *
 * If the input has zero length, the zero vector is returned.
 *
 * @param v Input vector.
 */
[[nodiscard]] constexpr auto Normalize(const Vector4& v) -> Vector4 {
    const auto len = v.Length();
    return len == 0.0f ? Vector4::Zero() : v * (1.0f / len);
}

}