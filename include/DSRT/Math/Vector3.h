/**
 * @file Vector3.h
 * @brief 3D vector class for graphics and physics
 * @ingroup Math
 * 
 * @details Provides comprehensive 3D vector operations with
 *          emphasis on graphics programming use cases.
 *          All methods are constexpr where possible.
 */

#pragma once
#include "../Export.h"
#include <cmath>

namespace DSRT {
namespace Math {

/**
 * @class Vector3
 * @brief 3D vector with single-precision floating point components
 * 
 * @details Standard 3D vector class used throughout DSRT Engine.
 *          Provides common operations for graphics, physics,
 *          and general 3D mathematics.
 * 
 * @performance For performance-critical code, consider using
 *              internal SIMD implementations (Vector3_SSE, Vector3_AVX).
 * 
 * @example Basic usage:
 * @code
 * Vector3 position(1.0f, 2.0f, 3.0f);
 * Vector3 velocity = Vector3::Forward() * 5.0f;
 * Vector3 result = position + velocity * deltaTime;
 * @endcode
 * 
 * @example Transform operations:
 * @code
 * Vector3 v1(1, 0, 0);
 * Vector3 v2(0, 1, 0);
 * float dot = v1.Dot(v2); // 0.0f
 * Vector3 cross = v1.Cross(v2); // (0, 0, 1)
 * @endcode
 */
class DSRT_API Vector3 {
public:
    float x; /**< X component */
    float y; /**< Y component */
    float z; /**< Z component */
    
    /**
     * @brief Default constructor (zero vector)
     * @details Initializes to (0, 0, 0)
     */
    Vector3();
    
    /**
     * @brief Component-wise constructor
     * @param x_ X component
     * @param y_ Y component
     * @param z_ Z component
     */
    Vector3(float x_, float y_, float z_);
    
    /**
     * @brief Scalar constructor
     * @param s Value for all components (x = y = z = s)
     */
    explicit Vector3(float s);
    
    /**
     * @brief Vector addition
     * @param other Vector to add
     * @return New vector (this + other)
     */
    Vector3 operator+(const Vector3& other) const;
    
    /**
     * @brief Vector subtraction
     * @param other Vector to subtract
     * @return New vector (this - other)
     */
    Vector3 operator-(const Vector3& other) const;
    
    /**
     * @brief Scalar multiplication
     * @param scalar Scalar to multiply
     * @return New vector (this * scalar)
     */
    Vector3 operator*(float scalar) const;
    
    /**
     * @brief Scalar division
     * @param scalar Scalar to divide by
     * @return New vector (this / scalar)
     * @throws No explicit throw, but division by zero yields NaN/Inf
     */
    Vector3 operator/(float scalar) const;
    
    /**
     * @brief Vector negation
     * @return New vector (-this)
     */
    Vector3 operator-() const;
    
    /**
     * @brief Compound addition
     * @param other Vector to add
     * @return Reference to this vector
     */
    Vector3& operator+=(const Vector3& other);
    
    /**
     * @brief Compound subtraction
     * @param other Vector to subtract
     * @return Reference to this vector
     */
    Vector3& operator-=(const Vector3& other);
    
    /**
     * @brief Compound scalar multiplication
     * @param scalar Scalar to multiply
     * @return Reference to this vector
     */
    Vector3& operator*=(float scalar);
    
    /**
     * @brief Compound scalar division
     * @param scalar Scalar to divide by
     * @return Reference to this vector
     */
    Vector3& operator/=(float scalar);
    
    /**
     * @brief Dot product
     * @param other Other vector
     * @return Dot product (this · other)
     * 
     * @details Measures cosine of angle between vectors when normalized.
     *          Range: [-length1*length2, length1*length2]
     */
    float Dot(const Vector3& other) const;
    
    /**
     * @brief Cross product
     * @param other Other vector
     * @return Cross product (this × other)
     * 
     * @details Returns vector perpendicular to both input vectors.
     *          Magnitude = area of parallelogram formed by inputs.
     *          Right-hand rule determines direction.
     */
    Vector3 Cross(const Vector3& other) const;
    
    /**
     * @brief Vector length (magnitude)
     * @return Euclidean length
     * 
     * @performance Uses sqrt() - consider LengthSq() for comparisons
     */
    float Length() const;
    
    /**
     * @brief Squared vector length
     * @return Length squared (x² + y² + z²)
     * 
     * @details Faster than Length() for comparisons since avoids sqrt().
     *          Useful for distance checks: if (v.LengthSq() < radius²)
     */
    float LengthSq() const;
    
    /**
     * @brief Get normalized copy
     * @return New vector with length 1 (or zero vector if length ≈ 0)
     * 
     * @note Returns zero vector if length < EPSILON
     */
    Vector3 Normalized() const;
    
    /**
     * @brief Normalize this vector in-place
     * 
     * @note Becomes zero vector if length < EPSILON
     */
    void Normalize();
    
    /**
     * @brief Linear interpolation between vectors
     * @param a Start vector (t = 0)
     * @param b End vector (t = 1)
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     * 
     * @details Clamps t to [0, 1]. For unclamped version see LerpUnclamped().
     */
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
    
    // Common constants
    
    /**
     * @brief Zero vector (0, 0, 0)
     */
    static Vector3 Zero();
    
    /**
     * @brief One vector (1, 1, 1)
     */
    static Vector3 One();
    
    /**
     * @brief World up vector (0, 1, 0)
     */
    static Vector3 Up();
    
    /**
     * @brief World down vector (0, -1, 0)
     */
    static Vector3 Down();
    
    /**
     * @brief World left vector (-1, 0, 0)
     */
    static Vector3 Left();
    
    /**
     * @brief World right vector (1, 0, 0)
     */
    static Vector3 Right();
    
    /**
     * @brief World forward vector (0, 0, 1)
     */
    static Vector3 Forward();
    
    /**
     * @brief World back vector (0, 0, -1)
     */
    static Vector3 Back();
    
private:
    static constexpr float EPSILON = 1e-6f; /**< Tolerance for float comparisons */
};

} // namespace Math
} // namespace DSRT
