/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/math/vector3.hpp"
#include "vglx/math/matrix4.hpp"

#include <array>

namespace vglx {

/**
 * @brief Represents a 3×3 floating-point matrix.
 *
 * This class represents a 3×3 matrix stored in column-major order, consistent with
 * the rest of the math module. It is commonly used for linear transforms such as
 * rotation, scaling, or for extracting the upper-left 3×3 portion of a @ref Matrix4.
 * The class supports construction from individual components, vectors, or a full
 * 4×4 matrix, as well as element access and comparison.
 *
 * @ingroup MathGroup
 */
class Matrix3 {
public:
    /**
     * @brief Constructs an uninitialized 3×3 matrix.
     */
    constexpr Matrix3() = default;

    /**
     * @brief Constructs a diagonal matrix.
     *
     * @param value Value to place on the diagonal.
     */
    explicit constexpr Matrix3(float value) : Matrix3({
        value, 0.0f, 0.0f,
        0.0f, value, 0.0f,
        0.0f, 0.0f, value
    }) {}

    /**
     * @brief Constructs a matrix from individual components (row-major).
     *
     * @param n00 First row, first element.
     * @param n01 First row, second element.
     * @param n02 First row, third element.
     * @param n10 Second row, first element.
     * @param n11 Second row, second element.
     * @param n12 Second row, third element.
     * @param n20 Third row, first element.
     * @param n21 Third row, second element.
     * @param n22 Third row, third element.
     */
    constexpr Matrix3(
        float n00, float n01, float n02,
        float n10, float n11, float n12,
        float n20, float n21, float n22
    ) : m {{
        Vector3(n00, n10, n20),
        Vector3(n01, n11, n21),
        Vector3(n02, n12, n22)
    }} {}

    /**
     * @brief Constructs a matrix from three column vectors.
     *
     * @param vec1 First column.
     * @param vec2 Second column.
     * @param vec3 Third column.
     */
    constexpr Matrix3(
        const Vector3& vec0,
        const Vector3& vec1,
        const Vector3& vec2
    ) : m {{vec0, vec1, vec2}} {}

    /**
     * @brief Extracts the upper-left 3×3 block from a 4×4 matrix.
     *
     * @param mat Source matrix.
     */
    constexpr Matrix3(const Matrix4& mat) : Matrix3(
        mat[0].x, mat[1].x, mat[2].x,
        mat[0].y, mat[1].y, mat[2].y,
        mat[0].z, mat[1].z, mat[2].z
    ) {}

    /**
     * @brief Returns the identity matrix.
     */
    [[nodiscard]] static constexpr auto Identity() -> Matrix3 {
        return Matrix3 {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
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
    [[nodiscard]] constexpr auto operator[](int col) -> Vector3& {
        return m[col];
    }

    /**
     * @brief Accesses a column vector.
     *
     * @param col Column index.
     */
    [[nodiscard]] constexpr auto operator[](int col) const -> const Vector3& {
        return m[col];
    }

    /**
     * @brief Compares two matrices for equality.
     */
    constexpr auto operator==(const Matrix3&) const -> bool = default;

private:
    std::array<Vector3, 3> m;
};

/**
 * @brief Multiplies two 3×3 matrices.
 * @related Matrix3
 *
 * @param a Left matrix.
 * @param b Right matrix.
 */
[[nodiscard]] constexpr auto operator*(const Matrix3& a, const Matrix3& b) -> Matrix3 {
    return Matrix3 {
        a(0, 0) * b(0, 0) + a(0, 1) * b(1, 0) + a(0, 2) * b(2, 0),
        a(0, 0) * b(0, 1) + a(0, 1) * b(1, 1) + a(0, 2) * b(2, 1),
        a(0, 0) * b(0, 2) + a(0, 1) * b(1, 2) + a(0, 2) * b(2, 2),
        a(1, 0) * b(0, 0) + a(1, 1) * b(1, 0) + a(1, 2) * b(2, 0),
        a(1, 0) * b(0, 1) + a(1, 1) * b(1, 1) + a(1, 2) * b(2, 1),
        a(1, 0) * b(0, 2) + a(1, 1) * b(1, 2) + a(1, 2) * b(2, 2),
        a(2, 0) * b(0, 0) + a(2, 1) * b(1, 0) + a(2, 2) * b(2, 0),
        a(2, 0) * b(0, 1) + a(2, 1) * b(1, 1) + a(2, 2) * b(2, 1),
        a(2, 0) * b(0, 2) + a(2, 1) * b(1, 2) + a(2, 2) * b(2, 2),
    };
}

/**
 * @brief Multiplies a matrix by a vector.
 * @related Matrix3
 *
 * @param mat Input matrix.
 * @param vec Input vector.
 */
[[nodiscard]] constexpr auto operator*(const Matrix3& mat, const Vector3& vec) -> Vector3 {
    return Vector3 {
        mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z,
        mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z,
        mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z
    };
}

/**
 * @brief Computes the determinant of a 3×3 matrix.
 * @related Matrix3
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Determinant(const Matrix3& mat) -> float {
    return mat(0, 0) * (mat(1, 1) * mat(2, 2) - mat(1, 2) * mat(2, 1))
         + mat(0, 1) * (mat(1, 2) * mat(2, 0) - mat(1, 0) * mat(2, 2))
         + mat(0, 2) * (mat(1, 0) * mat(2, 1) - mat(1, 1) * mat(2, 0));
}

/**
 * @brief Computes the inverse of a 3×3 matrix.
 * @related Matrix3
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Inverse(const Matrix3& mat) -> Matrix3 {
    const auto& a = mat[0];
    const auto& b = mat[1];
    const auto& c = mat[2];

    auto r0 = Cross(b, c);
    auto r1 = Cross(c, a);
    auto r2 = Cross(a, b);

    auto inv_det = 1.0f / Dot(r2, c);

    return Matrix3 {
        r0.x * inv_det, r0.y * inv_det, r0.z * inv_det,
        r1.x * inv_det, r1.y * inv_det, r1.z * inv_det,
        r2.x * inv_det, r2.y * inv_det, r2.z * inv_det
    };
}

/**
 * @brief Returns the transpose of a 3×3 matrix.
 * @related Matrix3
 *
 * @param mat Input matrix.
 */
[[nodiscard]] constexpr auto Transpose(const Matrix3& mat) -> Matrix3 {
    auto output = Matrix3 {};
    output[0] = {mat[0][0], mat[1][0], mat[2][0]};
    output[1] = {mat[0][1], mat[1][1], mat[2][1]};
    output[2] = {mat[0][2], mat[1][2], mat[2][2]};
    return output;
}

}