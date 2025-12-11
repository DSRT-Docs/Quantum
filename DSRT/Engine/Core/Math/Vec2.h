#pragma once

/**
 * @file Vec2.h
 * @brief 2D vector implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include <cmath>

namespace DSRT {
    
    /**
     * @brief 2D vector class
     */
    class Vec2 {
    public:
        float32 x, y;
        
        /**
         * @brief Constructs a zero vector
         */
        constexpr Vec2() : x(0.0f), y(0.0f) {}
        
        /**
         * @brief Constructs a vector with given components
         * @param x X component
         * @param y Y component
         */
        constexpr Vec2(float32 x, float32 y) : x(x), y(y) {}
        
        /**
         * @brief Constructs a vector with uniform components
         * @param scalar Value for all components
         */
        explicit constexpr Vec2(float32 scalar) : x(scalar), y(scalar) {}
        
        // Access operators
        constexpr float32& operator[](int index) {
            DSRT_ASSERT(index >= 0 && index < 2);
            return (&x)[index];
        }
        
        constexpr const float32& operator[](int index) const {
            DSRT_ASSERT(index >= 0 && index < 2);
            return (&x)[index];
        }
        
        // Arithmetic operators
        constexpr Vec2 operator+(const Vec2& other) const {
            return Vec2(x + other.x, y + other.y);
        }
        
        constexpr Vec2 operator-(const Vec2& other) const {
            return Vec2(x - other.x, y - other.y);
        }
        
        constexpr Vec2 operator*(const Vec2& other) const {
            return Vec2(x * other.x, y * other.y);
        }
        
        constexpr Vec2 operator/(const Vec2& other) const {
            return Vec2(x / other.x, y / other.y);
        }
        
        constexpr Vec2 operator+(float32 scalar) const {
            return Vec2(x + scalar, y + scalar);
        }
        
        constexpr Vec2 operator-(float32 scalar) const {
            return Vec2(x - scalar, y - scalar);
        }
        
        constexpr Vec2 operator*(float32 scalar) const {
            return Vec2(x * scalar, y * scalar);
        }
        
        constexpr Vec2 operator/(float32 scalar) const {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            return Vec2(x * inv, y * inv);
        }
        
        // Assignment operators
        constexpr Vec2& operator+=(const Vec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
        
        constexpr Vec2& operator-=(const Vec2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        
        constexpr Vec2& operator*=(const Vec2& other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        
        constexpr Vec2& operator/=(const Vec2& other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }
        
        constexpr Vec2& operator+=(float32 scalar) {
            x += scalar;
            y += scalar;
            return *this;
        }
        
        constexpr Vec2& operator-=(float32 scalar) {
            x -= scalar;
            y -= scalar;
            return *this;
        }
        
        constexpr Vec2& operator*=(float32 scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        
        constexpr Vec2& operator/=(float32 scalar) {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            x *= inv;
            y *= inv;
            return *this;
        }
        
        // Unary operators
        constexpr Vec2 operator+() const {
            return *this;
        }
        
        constexpr Vec2 operator-() const {
            return Vec2(-x, -y);
        }
        
        // Comparison operators
        constexpr bool operator==(const Vec2& other) const {
            return x == other.x && y == other.y;
        }
        
        constexpr bool operator!=(const Vec2& other) const {
            return !(*this == other);
        }
        
        // Vector operations
        
        /**
         * @brief Computes dot product
         * @param other Other vector
         * @return Dot product
         */
        constexpr float32 Dot(const Vec2& other) const {
            return x * other.x + y * other.y;
        }
        
        /**
         * @brief Computes cross product (scalar in 2D)
         * @param other Other vector
         * @return Cross product (scalar)
         */
        constexpr float32 Cross(const Vec2& other) const {
            return x * other.y - y * other.x;
        }
        
        /**
         * @brief Computes squared length
         * @return Squared length
         */
        constexpr float32 LengthSquared() const {
            return x * x + y * y;
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
        Vec2 Normalized() const {
            float32 len = Length();
            DSRT_ASSERT(len > 0.0f);
            return *this / len;
        }
        
        /**
         * @brief Normalizes this vector in place
         * @return Reference to this vector
         */
        Vec2& Normalize() {
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
        float32 Distance(const Vec2& other) const {
            return (*this - other).Length();
        }
        
        /**
         * @brief Computes squared distance to another vector
         * @param other Other vector
         * @return Squared distance
         */
        constexpr float32 DistanceSquared(const Vec2& other) const {
            Vec2 diff = *this - other;
            return diff.Dot(diff);
        }
        
        /**
         * @brief Linearly interpolates between two vectors
         * @param other Target vector
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated vector
         */
        constexpr Vec2 Lerp(const Vec2& other, float32 t) const {
            return *this + (other - *this) * t;
        }
        
        /**
         * @brief Clamps vector components
         * @param min Minimum value
         * @param max Maximum value
         * @return Clamped vector
         */
        constexpr Vec2 Clamp(float32 min, float32 max) const {
            return Vec2(
                x < min ? min : (x > max ? max : x),
                y < min ? min : (y > max ? max : y)
            );
        }
        
        /**
         * @brief Clamps vector length
         * @param maxLength Maximum length
         * @return Clamped vector
         */
        Vec2 ClampLength(float32 maxLength) const {
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
        constexpr Vec2 Reflect(const Vec2& normal) const {
            DSRT_ASSERT(normal.IsNormalized());
            return *this - normal * (2.0f * this->Dot(normal));
        }
        
        /**
         * @brief Projects vector onto another
         * @param other Vector to project onto
         * @return Projected vector
         */
        constexpr Vec2 Project(const Vec2& other) const {
            float32 lenSq = other.LengthSquared();
            if (lenSq == 0.0f) return Vec2(0.0f);
            return other * (this->Dot(other) / lenSq);
        }
        
        // Static methods
        
        /**
         * @brief Zero vector
         * @return Zero vector
         */
        static constexpr Vec2 Zero() {
            return Vec2(0.0f, 0.0f);
        }
        
        /**
         * @brief One vector
         * @return Vector with all ones
         */
        static constexpr Vec2 One() {
            return Vec2(1.0f, 1.0f);
        }
        
        /**
         * @brief Unit X vector
         * @return (1, 0)
         */
        static constexpr Vec2 UnitX() {
            return Vec2(1.0f, 0.0f);
        }
        
        /**
         * @brief Unit Y vector
         * @return (0, 1)
         */
        static constexpr Vec2 UnitY() {
            return Vec2(0.0f, 1.0f);
        }
        
        /**
         * @brief Minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static constexpr Vec2 Min(const Vec2& a, const Vec2& b) {
            return Vec2(
                a.x < b.x ? a.x : b.x,
                a.y < b.y ? a.y : b.y
            );
        }
        
        /**
         * @brief Maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static constexpr Vec2 Max(const Vec2& a, const Vec2& b) {
            return Vec2(
                a.x > b.x ? a.x : b.x,
                a.y > b.y ? a.y : b.y
            );
        }
        
        /**
         * @brief Computes absolute value of vector
         * @param v Input vector
         * @return Absolute vector
         */
        static constexpr Vec2 Abs(const Vec2& v) {
            return Vec2(std::abs(v.x), std::abs(v.y));
        }
        
        /**
         * @brief Computes floor of vector components
         * @param v Input vector
         * @return Floor vector
         */
        static Vec2 Floor(const Vec2& v) {
            return Vec2(std::floor(v.x), std::floor(v.y));
        }
        
        /**
         * @brief Computes ceil of vector components
         * @param v Input vector
         * @return Ceil vector
         */
        static Vec2 Ceil(const Vec2& v) {
            return Vec2(std::ceil(v.x), std::ceil(v.y));
        }
        
        /**
         * @brief Computes round of vector components
         * @param v Input vector
         * @return Round vector
         */
        static Vec2 Round(const Vec2& v) {
            return Vec2(std::round(v.x), std::round(v.y));
        }
        
        /**
         * @brief Computes sign of vector components
         * @param v Input vector
         * @return Sign vector (-1, 0, or 1 per component)
         */
        static Vec2 Sign(const Vec2& v) {
            return Vec2(
                v.x > 0.0f ? 1.0f : (v.x < 0.0f ? -1.0f : 0.0f),
                v.y > 0.0f ? 1.0f : (v.y < 0.0f ? -1.0f : 0.0f)
            );
        }
    };
    
    // Scalar-vector operations
    constexpr Vec2 operator+(float32 scalar, const Vec2& vec) {
        return vec + scalar;
    }
    
    constexpr Vec2 operator-(float32 scalar, const Vec2& vec) {
        return Vec2(scalar - vec.x, scalar - vec.y);
    }
    
    constexpr Vec2 operator*(float32 scalar, const Vec2& vec) {
        return vec * scalar;
    }
    
    constexpr Vec2 operator/(float32 scalar, const Vec2& vec) {
        return Vec2(scalar / vec.x, scalar / vec.y);
    }
    
} // namespace DSRT
