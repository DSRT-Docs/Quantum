#pragma once

/**
 * @file Vec3.h
 * @brief 3D vector implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "Vec2.h"
#include <cmath>

namespace DSRT {
    
    /**
     * @brief 3D vector class
     */
    class Vec3 {
    public:
        float32 x, y, z;
        
        /**
         * @brief Constructs a zero vector
         */
        constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
        
        /**
         * @brief Constructs a vector with given components
         * @param x X component
         * @param y Y component
         * @param z Z component
         */
        constexpr Vec3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
        
        /**
         * @brief Constructs a vector from Vec2 and Z component
         * @param xy 2D vector
         * @param z Z component
         */
        constexpr Vec3(const Vec2& xy, float32 z = 0.0f) : x(xy.x), y(xy.y), z(z) {}
        
        /**
         * @brief Constructs a vector with uniform components
         * @param scalar Value for all components
         */
        explicit constexpr Vec3(float32 scalar) : x(scalar), y(scalar), z(scalar) {}
        
        // Access operators
        constexpr float32& operator[](int index) {
            DSRT_ASSERT(index >= 0 && index < 3);
            return (&x)[index];
        }
        
        constexpr const float32& operator[](int index) const {
            DSRT_ASSERT(index >= 0 && index < 3);
            return (&x)[index];
        }
        
        // Type conversion
        constexpr Vec2 XY() const {
            return Vec2(x, y);
        }
        
        constexpr Vec2 XZ() const {
            return Vec2(x, z);
        }
        
        constexpr Vec2 YZ() const {
            return Vec2(y, z);
        }
        
        // Arithmetic operators
        constexpr Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }
        
        constexpr Vec3 operator-(const Vec3& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }
        
        constexpr Vec3 operator*(const Vec3& other) const {
            return Vec3(x * other.x, y * other.y, z * other.z);
        }
        
        constexpr Vec3 operator/(const Vec3& other) const {
            return Vec3(x / other.x, y / other.y, z / other.z);
        }
        
        constexpr Vec3 operator+(float32 scalar) const {
            return Vec3(x + scalar, y + scalar, z + scalar);
        }
        
        constexpr Vec3 operator-(float32 scalar) const {
            return Vec3(x - scalar, y - scalar, z - scalar);
        }
        
        constexpr Vec3 operator*(float32 scalar) const {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }
        
        constexpr Vec3 operator/(float32 scalar) const {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            return Vec3(x * inv, y * inv, z * inv);
        }
        
        // Assignment operators
        constexpr Vec3& operator+=(const Vec3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        
        constexpr Vec3& operator-=(const Vec3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        
        constexpr Vec3& operator*=(const Vec3& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        
        constexpr Vec3& operator/=(const Vec3& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }
        
        constexpr Vec3& operator+=(float32 scalar) {
            x += scalar;
            y += scalar;
            z += scalar;
            return *this;
        }
        
        constexpr Vec3& operator-=(float32 scalar) {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            return *this;
        }
        
        constexpr Vec3& operator*=(float32 scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        
        constexpr Vec3& operator/=(float32 scalar) {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }
        
        // Unary operators
        constexpr Vec3 operator+() const {
            return *this;
        }
        
        constexpr Vec3 operator-() const {
            return Vec3(-x, -y, -z);
        }
        
        // Comparison operators
        constexpr bool operator==(const Vec3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
        
        constexpr bool operator!=(const Vec3& other) const {
            return !(*this == other);
        }
        
        // Vector operations
        
        /**
         * @brief Computes dot product
         * @param other Other vector
         * @return Dot product
         */
        constexpr float32 Dot(const Vec3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }
        
        /**
         * @brief Computes cross product
         * @param other Other vector
         * @return Cross product vector
         */
        constexpr Vec3 Cross(const Vec3& other) const {
            return Vec3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }
        
        /**
         * @brief Computes squared length
         * @return Squared length
         */
        constexpr float32 LengthSquared() const {
            return x * x + y * y + z * z;
        }
        
        /**
         * @brief Computes length
         * @return Length
         */
        float32 Length() const {
            return std::sqrt(LengthSquared());
        }
        
        /**
         * @brief Normalizes the vector
         * @return Normalized vector
         */
        Vec3 Normalized() const {
            float32 len = Length();
            DSRT_ASSERT(len > 0.0f);
            return *this / len;
        }
        
        /**
         * @brief Normalizes this vector in place
         * @return Reference to this vector
         */
        Vec3& Normalize() {
            float32 len = Length();
            DSRT_ASSERT(len > 0.0f);
            *this /= len;
            return *this;
        }
        
        /**
         * @brief Checks if vector is normalized
         * @param epsilon Tolerance
         * @return True if normalized
         */
        bool IsNormalized(float32 epsilon = 1e-6f) const {
            return std::abs(LengthSquared() - 1.0f) <= epsilon;
        }
        
        /**
         * @brief Checks if vector is zero
         * @param epsilon Tolerance
         * @return True if zero vector
         */
        bool IsZero(float32 epsilon = 1e-6f) const {
            return LengthSquared() <= epsilon;
        }
        
        /**
         * @brief Computes distance to another vector
         * @param other Other vector
         * @return Distance
         */
        float32 Distance(const Vec3& other) const {
            return (*this - other).Length();
        }
        
        /**
         * @brief Computes squared distance to another vector
         * @param other Other vector
         * @return Squared distance
         */
        constexpr float32 DistanceSquared(const Vec3& other) const {
            Vec3 diff = *this - other;
            return diff.Dot(diff);
        }
        
        /**
         * @brief Linearly interpolates between two vectors
         * @param other Target vector
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated vector
         */
        constexpr Vec3 Lerp(const Vec3& other, float32 t) const {
            return *this + (other - *this) * t;
        }
        
        /**
         * @brief Spherically interpolates between two vectors
         * @param other Target vector
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated vector
         */
        Vec3 Slerp(const Vec3& other, float32 t) const {
            float32 dot = Dot(other);
            dot = dot < -1.0f ? -1.0f : (dot > 1.0f ? 1.0f : dot);
            
            float32 theta = std::acos(dot) * t;
            Vec3 relative = other - *this * dot;
            relative.Normalize();
            
            return *this * std::cos(theta) + relative * std::sin(theta);
        }
        
        /**
         * @brief Clamps vector components
         * @param min Minimum value
         * @param max Maximum value
         * @return Clamped vector
         */
        constexpr Vec3 Clamp(float32 min, float32 max) const {
            return Vec3(
                x < min ? min : (x > max ? max : x),
                y < min ? min : (y > max ? max : y),
                z < min ? min : (z > max ? max : z)
            );
        }
        
        /**
         * @brief Clamps vector length
         * @param maxLength Maximum length
         * @return Clamped vector
         */
        Vec3 ClampLength(float32 maxLength) const {
            float32 len = Length();
            if (len > maxLength && len > 0.0f) {
                return *this * (maxLength / len);
            }
            return *this;
        }
        
        /**
         * @brief Reflects vector about a normal
         * @param normal Normal vector (must be normalized)
         * @return Reflected vector
         */
        constexpr Vec3 Reflect(const Vec3& normal) const {
            DSRT_ASSERT(normal.IsNormalized());
            return *this - normal * (2.0f * this->Dot(normal));
        }
        
        /**
         * @brief Refracts vector through a surface
         * @param normal Surface normal
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        Vec3 Refract(const Vec3& normal, float32 eta) const {
            DSRT_ASSERT(normal.IsNormalized());
            float32 dot = Dot(normal);
            float32 k = 1.0f - eta * eta * (1.0f - dot * dot);
            if (k < 0.0f) {
                return Vec3(0.0f);
            }
            return *this * eta - normal * (eta * dot + std::sqrt(k));
        }
        
        /**
         * @brief Projects vector onto another
         * @param other Vector to project onto
         * @return Projected vector
         */
        constexpr Vec3 Project(const Vec3& other) const {
            float32 lenSq = other.LengthSquared();
            if (lenSq == 0.0f) return Vec3(0.0f);
            return other * (this->Dot(other) / lenSq);
        }
        
        /**
         * @brief Projects vector onto a plane
         * @param planeNormal Plane normal
         * @return Projected vector
         */
        constexpr Vec3 ProjectOnPlane(const Vec3& planeNormal) const {
            DSRT_ASSERT(planeNormal.IsNormalized());
            return *this - planeNormal * this->Dot(planeNormal);
        }
        
        // Static methods
        
        /**
         * @brief Zero vector
         * @return Zero vector
         */
        static constexpr Vec3 Zero() {
            return Vec3(0.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief One vector
         * @return Vector with all ones
         */
        static constexpr Vec3 One() {
            return Vec3(1.0f, 1.0f, 1.0f);
        }
        
        /**
         * @brief Unit X vector
         * @return (1, 0, 0)
         */
        static constexpr Vec3 UnitX() {
            return Vec3(1.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief Unit Y vector
         * @return (0, 1, 0)
         */
        static constexpr Vec3 UnitY() {
            return Vec3(0.0f, 1.0f, 0.0f);
        }
        
        /**
         * @brief Unit Z vector
         * @return (0, 0, 1)
         */
        static constexpr Vec3 UnitZ() {
            return Vec3(0.0f, 0.0f, 1.0f);
        }
        
        /**
         * @brief Up vector
         * @return (0, 1, 0)
         */
        static constexpr Vec3 Up() {
            return UnitY();
        }
        
        /**
         * @brief Down vector
         * @return (0, -1, 0)
         */
        static constexpr Vec3 Down() {
            return Vec3(0.0f, -1.0f, 0.0f);
        }
        
        /**
         * @brief Forward vector (OpenGL convention)
         * @return (0, 0, -1)
         */
        static constexpr Vec3 Forward() {
            return Vec3(0.0f, 0.0f, -1.0f);
        }
        
        /**
         * @brief Back vector
         * @return (0, 0, 1)
         */
        static constexpr Vec3 Back() {
            return Vec3(0.0f, 0.0f, 1.0f);
        }
        
        /**
         * @brief Right vector
         * @return (1, 0, 0)
         */
        static constexpr Vec3 Right() {
            return UnitX();
        }
        
        /**
         * @brief Left vector
         * @return (-1, 0, 0)
         */
        static constexpr Vec3 Left() {
            return Vec3(-1.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief Minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static constexpr Vec3 Min(const Vec3& a, const Vec3& b) {
            return Vec3(
                a.x < b.x ? a.x : b.x,
                a.y < b.y ? a.y : b.y,
                a.z < b.z ? a.z : b.z
            );
        }
        
        /**
         * @brief Maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static constexpr Vec3 Max(const Vec3& a, const Vec3& b) {
            return Vec3(
                a.x > b.x ? a.x : b.x,
                a.y > b.y ? a.y : b.y,
                a.z > b.z ? a.z : b.z
            );
        }
        
        /**
         * @brief Computes absolute value of vector
         * @param v Input vector
         * @return Absolute vector
         */
        static constexpr Vec3 Abs(const Vec3& v) {
            return Vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
        }
        
        /**
         * @brief Computes floor of vector components
         * @param v Input vector
         * @return Floor vector
         */
        static Vec3 Floor(const Vec3& v) {
            return Vec3(std::floor(v.x), std::floor(v.y), std::floor(v.z));
        }
        
        /**
         * @brief Computes ceil of vector components
         * @param v Input vector
         * @return Ceil vector
         */
        static Vec3 Ceil(const Vec3& v) {
            return Vec3(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
        }
        
        /**
         * @brief Computes round of vector components
         * @param v Input vector
         * @return Round vector
         */
        static Vec3 Round(const Vec3& v) {
            return Vec3(std::round(v.x), std::round(v.y), std::round(v.z));
        }
        
        /**
         * @brief Computes sign of vector components
         * @param v Input vector
         * @return Sign vector (-1, 0, or 1 per component)
         */
        static Vec3 Sign(const Vec3& v) {
            return Vec3(
                v.x > 0.0f ? 1.0f : (v.x < 0.0f ? -1.0f : 0.0f),
                v.y > 0.0f ? 1.0f : (v.y < 0.0f ? -1.0f : 0.0f),
                v.z > 0.0f ? 1.0f : (v.z < 0.0f ? -1.0f : 0.0f)
            );
        }
        
        /**
         * @brief Computes angle between two vectors
         * @param a First vector
         * @param b Second vector
         * @return Angle in radians
         */
        static float32 Angle(const Vec3& a, const Vec3& b) {
            float32 dot = a.Dot(b);
            dot = dot < -1.0f ? -1.0f : (dot > 1.0f ? 1.0f : dot);
            return std::acos(dot);
        }
        
        /**
         * @brief Computes distance between two points
         * @param a First point
         * @param b Second point
         * @return Distance
         */
        static float32 Distance(const Vec3& a, const Vec3& b) {
            return a.Distance(b);
        }
        
        /**
         * @brief Computes squared distance between two points
         * @param a First point
         * @param b Second point
         * @return Squared distance
         */
        static float32 DistanceSquared(const Vec3& a, const Vec3& b) {
            return a.DistanceSquared(b);
        }
    };
    
    // Scalar-vector operations
    constexpr Vec3 operator+(float32 scalar, const Vec3& vec) {
        return vec + scalar;
    }
    
    constexpr Vec3 operator-(float32 scalar, const Vec3& vec) {
        return Vec3(scalar - vec.x, scalar - vec.y, scalar - vec.z);
    }
    
    constexpr Vec3 operator*(float32 scalar, const Vec3& vec) {
        return vec * scalar;
    }
    
    constexpr Vec3 operator/(float32 scalar, const Vec3& vec) {
        return Vec3(scalar / vec.x, scalar / vec.y, scalar / vec.z);
    }
    
} // namespace DSRT
