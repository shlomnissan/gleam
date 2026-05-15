/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/vector3.hpp"
#include "vglx/math/vector4.hpp"

#include <array>

namespace vglx {

/**
 * @brief Represents a 4×4 floating-point matrix.
 *
 * This class stores a 4×4 matrix in column-major order, consistent with the
 * rest of the math module. It is used for affine transforms in 3D graphics,
 * including world, view, and projection matrices. Matrix4 supports construction
 * from individual components or column vectors, as well as element and column
 * access and equality comparison.
 *
 * @ingroup MathGroup
 */
class Matrix4 {
public:
    /**
     * @brief Constructs an uninitialized 4×4 matrix.
     */
    constexpr Matrix4() = default;

    /**
     * @brief Constructs a diagonal matrix.
     *
     * @param value Value to place on the diagonal.
     */
    explicit constexpr Matrix4(float value) : Matrix4(
        value, 0.0f, 0.0f, 0.0f,
        0.0f, value, 0.0f, 0.0f,
        0.0f, 0.0f, value, 0.0f,
        0.0f, 0.0f, 0.0f, value
    ) {}

    /**
     * @brief Constructs a matrix from individual components (row-major).
     *
     * @param n00 First row, first element.
     * @param n01 First row, second element.
     * @param n02 First row, third element.
     * @param n03 First row, fourth element.
     * @param n10 Second row, first element.
     * @param n11 Second row, second element.
     * @param n12 Second row, third element.
     * @param n13 Second row, fourth element.
     * @param n20 Third row, first element.
     * @param n21 Third row, second element.
     * @param n22 Third row, third element.
     * @param n23 Third row, fourth element.
     * @param n30 Fourth row, first element.
     * @param n31 Fourth row, second element.
     * @param n32 Fourth row, third element.
     * @param n33 Fourth row, fourth element.
     */
    constexpr Matrix4(
        float n00, float n01, float n02, float n03,
        float n10, float n11, float n12, float n13,
        float n20, float n21, float n22, float n23,
        float n30, float n31, float n32, float n33
    ) : m {{
        Vector4(n00, n10, n20, n30),
        Vector4(n01, n11, n21, n31),
        Vector4(n02, n12, n22, n32),
        Vector4(n03, n13, n23, n33)
    }} {}

    /**
     * @brief Constructs a matrix from four column vectors.
     *
     * @param vec0 First column.
     * @param vec1 Second column.
     * @param vec2 Third column.
     * @param vec3 Fourth column.
     */
    constexpr Matrix4(
        const Vector4& vec0,
        const Vector4& vec1,
        const Vector4& vec2,
        const Vector4& vec3
    ) : m {{vec0, vec1, vec2, vec3}} {}

    /**
     * @brief Returns the identity matrix.
     */
    [[nodiscard]] static constexpr auto Identity() -> Matrix4 {
        return Matrix4 {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    /**
     * @brief Accesses an element by `(row, column)`.
     *
     * @param row Row index.
     * @param col Column index.
     */
    [[nodiscard]] constexpr auto operator()(int row, int col) -> float& {
        return m[col][row];
    }

    /**
     * @brief Accesses an element by `(row, column)`.
     *
     * @param row Row index.
     * @param col Column index.
     */
    [[nodiscard]] constexpr auto operator()(int row, int col) const -> const float {
        return m[col][row];
    }

    /**
     * @brief Accesses a column vector.
     *
     * @param col Column index.
     */
    [[nodiscard]] constexpr auto operator[](int col) -> Vector4& {
        return m[col];
    }

    /**
     * @brief Accesses a column vector.
     *
     * @param col Column index.
     */
    [[nodiscard]] constexpr auto operator[](int col) const -> const Vector4&{
        return m[col];
    }

    /**
     * @brief Compares two matrices for equality.
     */
    constexpr auto operator==(const Matrix4&) const -> bool = default;

private:
    std::array<Vector4, 4> m;
};

/**
 * @brief Multiplies two 4×4 matrices.
 * @related Matrix4
 *
 * @param a Left matrix.
 * @param b Right matrix.
 */
[[nodiscard]] constexpr auto operator*(const Matrix4& a, const Matrix4& b) -> Matrix4 {
    return Matrix4 {
        a(0, 0) * b(0, 0) + a(0, 1) * b(1, 0) + a(0, 2) * b(2, 0) + a(0, 3) * b(3, 0),
        a(0, 0) * b(0, 1) + a(0, 1) * b(1, 1) + a(0, 2) * b(2, 1) + a(0, 3) * b(3, 1),
        a(0, 0) * b(0, 2) + a(0, 1) * b(1, 2) + a(0, 2) * b(2, 2) + a(0, 3) * b(3, 2),
        a(0, 0) * b(0, 3) + a(0, 1) * b(1, 3) + a(0, 2) * b(2, 3) + a(0, 3) * b(3, 3),
        a(1, 0) * b(0, 0) + a(1, 1) * b(1, 0) + a(1, 2) * b(2, 0) + a(1, 3) * b(3, 0),
        a(1, 0) * b(0, 1) + a(1, 1) * b(1, 1) + a(1, 2) * b(2, 1) + a(1, 3) * b(3, 1),
        a(1, 0) * b(0, 2) + a(1, 1) * b(1, 2) + a(1, 2) * b(2, 2) + a(1, 3) * b(3, 2),
        a(1, 0) * b(0, 3) + a(1, 1) * b(1, 3) + a(1, 2) * b(2, 3) + a(1, 3) * b(3, 3),
        a(2, 0) * b(0, 0) + a(2, 1) * b(1, 0) + a(2, 2) * b(2, 0) + a(2, 3) * b(3, 0),
        a(2, 0) * b(0, 1) + a(2, 1) * b(1, 1) + a(2, 2) * b(2, 1) + a(2, 3) * b(3, 1),
        a(2, 0) * b(0, 2) + a(2, 1) * b(1, 2) + a(2, 2) * b(2, 2) + a(2, 3) * b(3, 2),
        a(2, 0) * b(0, 3) + a(2, 1) * b(1, 3) + a(2, 2) * b(2, 3) + a(2, 3) * b(3, 3),
        a(3, 0) * b(0, 0) + a(3, 1) * b(1, 0) + a(3, 2) * b(2, 0) + a(3, 3) * b(3, 0),
        a(3, 0) * b(0, 1) + a(3, 1) * b(1, 1) + a(3, 2) * b(2, 1) + a(3, 3) * b(3, 1),
        a(3, 0) * b(0, 2) + a(3, 1) * b(1, 2) + a(3, 2) * b(2, 2) + a(3, 3) * b(3, 2),
        a(3, 0) * b(0, 3) + a(3, 1) * b(1, 3) + a(3, 2) * b(2, 3) + a(3, 3) * b(3, 3)
    };
}

/**
 * @brief Multiplies a matrix by a 4D vector.
 * @related Matrix4
 *
 * @param mat Input matrix.
 * @param vec Input vector.
 */
[[nodiscard]] constexpr auto operator*(const Matrix4& mat, const Vector4& vec) -> Vector4 {
    return Vector4 {
        mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z + mat(0, 3) * vec.w,
        mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z + mat(1, 3) * vec.w,
        mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z + mat(2, 3) * vec.w,
        mat(3, 0) * vec.x + mat(3, 1) * vec.y + mat(3, 2) * vec.z + mat(3, 3) * vec.w
    };
}

/**
 * @brief Transforms a 3D point by a 4×4 matrix.
 * @related Matrix4
 *
 * The input vector is treated as a position with `w = 1.0`.
 *
 * @param mat Input matrix.
 * @param vec Input vector.
 */
[[nodiscard]] constexpr auto operator*(const Matrix4& mat, const Vector3& vec) -> Vector3 {
    return Vector3 {
        mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z + mat(0, 3) * 1.0f,
        mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z + mat(1, 3) * 1.0f,
        mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z + mat(2, 3) * 1.0f
    };
}

/**
 * @brief Computes the determinant of a 4×4 matrix.
 * @related Matrix4
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Determinant(const Matrix4& mat) -> float {
    return mat(0, 0) * (
        mat(1, 1) * (mat(2, 2) * mat(3, 3) - mat(2, 3) * mat(3, 2)) -
        mat(1, 2) * (mat(2, 1) * mat(3, 3) - mat(2, 3) * mat(3, 1)) +
        mat(1, 3) * (mat(2, 1) * mat(3, 2) - mat(2, 2) * mat(3, 1))
    ) - mat(0, 1) * (
        mat(1, 0) * (mat(2, 2) * mat(3, 3) - mat(2, 3) * mat(3, 2)) -
        mat(1, 2) * (mat(2, 0) * mat(3, 3) - mat(2, 3) * mat(3, 0)) +
        mat(1, 3) * (mat(2, 0) * mat(3, 2) - mat(2, 2) * mat(3, 0))
    ) + mat(0, 2) * (
        mat(1, 0) * (mat(2, 1) * mat(3, 3) - mat(2, 3) * mat(3, 1)) -
        mat(1, 1) * (mat(2, 0) * mat(3, 3) - mat(2, 3) * mat(3, 0)) +
        mat(1, 3) * (mat(2, 0) * mat(3, 1) - mat(2, 1) * mat(3, 0))
    ) - mat(0, 3) * (
        mat(1, 0) * (mat(2, 1) * mat(3, 2) - mat(2, 2) * mat(3, 1)) -
        mat(1, 1) * (mat(2, 0) * mat(3, 2) - mat(2, 2) * mat(3, 0)) +
        mat(1, 2) * (mat(2, 0) * mat(3, 1) - mat(2, 1) * mat(3, 0))
    );
}

/**
 * @brief Computes the inverse of a 4×4 matrix.
 * @related Matrix4
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Inverse(const Matrix4& mat) -> Matrix4 {
    const auto& a = Vector3 {mat[0].x, mat[0].y, mat[0].z};
    const auto& b = Vector3 {mat[1].x, mat[1].y, mat[1].z};
    const auto& c = Vector3 {mat[2].x, mat[2].y, mat[2].z};
    const auto& d = Vector3 {mat[3].x, mat[3].y, mat[3].z};

    const float& x = mat(3, 0);
    const float& y = mat(3, 1);
    const float& z = mat(3, 2);
    const float& w = mat(3, 3);

    auto s = Cross(a, b);
    auto t = Cross(c, d);
    auto u = (a * y) - (b * x);
    auto v = (c * w) - (d * z);

    const auto inv_det = 1.0f / (Dot(s, v) + Dot(t, u));
    s *= inv_det;
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;

    auto r0 = Cross(b, v) + t * y;
    auto r1 = Cross(v, a) - t * x;
    auto r2 = Cross(d, u) + s * w;
    auto r3 = Cross(u, c) - s * z;

    return Matrix4 {
        r0.x, r0.y, r0.z, -Dot(b, t),
        r1.x, r1.y, r1.z,  Dot(a, t),
        r2.x, r2.y, r2.z, -Dot(d, s),
        r3.x, r3.y, r3.z,  Dot(c, s)
    };
}

/**
 * @brief Returns the transpose of a 4×4 matrix.
 * @related Matrix4
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Transpose(const Matrix4& mat) -> Matrix4 {
    auto output = Matrix4 {};
    output[0] = {mat[0][0], mat[1][0], mat[2][0], mat[3][0]};
    output[1] = {mat[0][1], mat[1][1], mat[2][1], mat[3][1]};
    output[2] = {mat[0][2], mat[1][2], mat[2][2], mat[3][2]};
    output[3] = {mat[0][3], mat[1][3], mat[2][3], mat[3][3]};
    return output;
}

}