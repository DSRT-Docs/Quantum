#pragma once

/**
 * @file Vec4.h
 * @brief 4D vector implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "Vec2.h"
#include "Vec3.h"
#include <cmath>

namespace DSRT {
    
    /**
     * @brief 4D vector class
     */
    class Vec4 {
    public:
        float32 x, y, z, w;
        
        /**
         * @brief Constructs a zero vector
         */
        constexpr Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        
        /**
         * @brief Constructs a vector with given components
         * @param x X component
         * @param y Y component
         * @param z Z component
         * @param w W component
         */
        constexpr Vec4(float32 x, float32 y, float32 z, float32 w) 
            : x(x), y(y), z(z), w(w) {}
        
        /**
         * @brief Constructs a vector from Vec3 and W component
         * @param xyz 3D vector
         * @param w W component
         */
        constexpr Vec4(const Vec3& xyz, float32 w = 0.0f) 
            : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        
        /**
         * @brief Constructs a vector from Vec2 and Z, W components
         * @param xy 2D vector
         * @param z Z component
         * @param w W component
         */
        constexpr Vec4(const Vec2& xy, float32 z = 0.0f, float32 w = 0.0f) 
            : x(xy.x), y(xy.y), z(z), w(w) {}
        
        /**
         * @brief Constructs a vector with uniform components
         * @param scalar Value for all components
         */
        explicit constexpr Vec4(float32 scalar) 
            : x(scalar), y(scalar), z(scalar), w(scalar) {}
        
        // Access operators
        constexpr float32& operator[](int index) {
            DSRT_ASSERT(index >= 0 && index < 4);
            return (&x)[index];
        }
        
        constexpr const float32& operator[](int index) const {
            DSRT_ASSERT(index >= 0 && index < 4);
            return (&x)[index];
        }
        
        // Type conversion
        constexpr Vec2 XY() const {
            return Vec2(x, y);
        }
        
        constexpr Vec2 XZ() const {
            return Vec2(x, z);
        }
        
        constexpr Vec2 XW() const {
            return Vec2(x, w);
        }
        
        constexpr Vec2 YZ() const {
            return Vec2(y, z);
        }
        
        constexpr Vec2 YW() const {
            return Vec2(y, w);
        }
        
        constexpr Vec2 ZW() const {
            return Vec2(z, w);
        }
        
        constexpr Vec3 XYZ() const {
            return Vec3(x, y, z);
        }
        
        constexpr Vec3 XYW() const {
            return Vec3(x, y, w);
        }
        
        constexpr Vec3 XZW() const {
            return Vec3(x, z, w);
        }
        
        constexpr Vec3 YZW() const {
            return Vec3(y, z, w);
        }
        
        // Arithmetic operators
        constexpr Vec4 operator+(const Vec4& other) const {
            return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
        }
        
        constexpr Vec4 operator-(const Vec4& other) const {
            return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
        }
        
        constexpr Vec4 operator*(const Vec4& other) const {
            return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
        }
        
        constexpr Vec4 operator/(const Vec4& other) const {
            return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
        }
        
        constexpr Vec4 operator+(float32 scalar) const {
            return Vec4(x + scalar, y + scalar, z + scalar, w + scalar);
        }
        
        constexpr Vec4 operator-(float32 scalar) const {
            return Vec4(x - scalar, y - scalar, z - scalar, w - scalar);
        }
        
        constexpr Vec4 operator*(float32 scalar) const {
            return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        
        constexpr Vec4 operator/(float32 scalar) const {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            return Vec4(x * inv, y * inv, z * inv, w * inv);
        }
        
        // Assignment operators
        constexpr Vec4& operator+=(const Vec4& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }
        
        constexpr Vec4& operator-=(const Vec4& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }
        
        constexpr Vec4& operator*=(const Vec4& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }
        
        constexpr Vec4& operator/=(const Vec4& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }
        
        constexpr Vec4& operator+=(float32 scalar) {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            return *this;
        }
        
        constexpr Vec4& operator-=(float32 scalar) {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            return *this;
        }
        
        constexpr Vec4& operator*=(float32 scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }
        
        constexpr Vec4& operator/=(float32 scalar) {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
            return *this;
        }
        
        // Unary operators
        constexpr Vec4 operator+() const {
            return *this;
        }
        
        constexpr Vec4 operator-() const {
            return Vec4(-x, -y, -z, -w);
        }
        
        // Comparison operators
        constexpr bool operator==(const Vec4& other) const {
            return x == other.x && y == other.y && 
                   z == other.z && w == other.w;
        }
        
        constexpr bool operator!=(const Vec4& other) const {
            return !(*this == other);
        }
        
        // Vector operations
        
        /**
         * @brief Computes dot product
         * @param other Other vector
         * @return Dot product
         */
        constexpr float32 Dot(const Vec4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }
        
        /**
         * @brief Computes squared length
         * @return Squared length
         */
        constexpr float32 LengthSquared() const {
            return Dot(*this);
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
        Vec4 Normalized() const {
            float32 len = Length();
            DSRT_ASSERT(len > 0.0f);
            return *this / len;
        }
        
        /**
         * @brief Normalizes this vector in place
         * @return Reference to this vector
         */
        Vec4& Normalize() {
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
        float32 Distance(const Vec4& other) const {
            return (*this - other).Length();
        }
        
        /**
         * @brief Computes squared distance to another vector
         * @param other Other vector
         * @return Squared distance
         */
        constexpr float32 DistanceSquared(const Vec4& other) const {
            Vec4 diff = *this - other;
            return diff.Dot(diff);
        }
        
        /**
         * @brief Linearly interpolates between two vectors
         * @param other Target vector
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated vector
         */
        constexpr Vec4 Lerp(const Vec4& other, float32 t) const {
            return *this + (other - *this) * t;
        }
        
        /**
         * @brief Clamps vector components
         * @param min Minimum value
         * @param max Maximum value
         * @return Clamped vector
         */
        constexpr Vec4 Clamp(float32 min, float32 max) const {
            return Vec4(
                x < min ? min : (x > max ? max : x),
                y < min ? min : (y > max ? max : y),
                z < min ? min : (z > max ? max : z),
                w < min ? min : (w > max ? max : w)
            );
        }
        
        /**
         * @brief Clamps vector length
         * @param maxLength Maximum length
         * @return Clamped vector
         */
        Vec4 ClampLength(float32 maxLength) const {
            float32 len = Length();
            if (len > maxLength && len > 0.0f) {
                return *this * (maxLength / len);
            }
            return *this;
        }
        
        /**
         * @brief Homogenizes the vector (divides by w)
         * @return Homogenized vector (w becomes 1)
         */
        Vec4 Homogenize() const {
            if (w == 0.0f) return *this;
            return Vec4(x / w, y / w, z / w, 1.0f);
        }
        
        // Static methods
        
        /**
         * @brief Zero vector
         * @return Zero vector
         */
        static constexpr Vec4 Zero() {
            return Vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief One vector
         * @return Vector with all ones
         */
        static constexpr Vec4 One() {
            return Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        
        /**
         * @brief Unit X vector
         * @return (1, 0, 0, 0)
         */
        static constexpr Vec4 UnitX() {
            return Vec4(1.0f, 0.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief Unit Y vector
         * @return (0, 1, 0, 0)
         */
        static constexpr Vec4 UnitY() {
            return Vec4(0.0f, 1.0f, 0.0f, 0.0f);
        }
        
        /**
         * @brief Unit Z vector
         * @return (0, 0, 1, 0)
         */
        static constexpr Vec4 UnitZ() {
            return Vec4(0.0f, 0.0f, 1.0f, 0.0f);
        }
        
        /**
         * @brief Unit W vector
         * @return (0, 0, 0, 1)
         */
        static constexpr Vec4 UnitW() {
            return Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        
        /**
         * @brief Minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static constexpr Vec4 Min(const Vec4& a, const Vec4& b) {
            return Vec4(
                a.x < b.x ? a.x : b.x,
                a.y < b.y ? a.y : b.y,
                a.z < b.z ? a.z : b.z,
                a.w < b.w ? a.w : b.w
            );
        }
        
        /**
         * @brief Maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static constexpr Vec4 Max(const Vec4& a, const Vec4& b) {
            return Vec4(
                a.x > b.x ? a.x : b.x,
                a.y > b.y ? a.y : b.y,
                a.z > b.z ? a.z : b.z,
                a.w > b.w ? a.w : b.w
            );
        }
        
        /**
         * @brief Computes absolute value of vector
         * @param v Input vector
         * @return Absolute vector
         */
        static constexpr Vec4 Abs(const Vec4& v) {
            return Vec4(std::abs(v.x), std::abs(v.y), 
                       std::abs(v.z), std::abs(v.w));
        }
        
        /**
         * @brief Computes floor of vector components
         * @param v Input vector
         * @return Floor vector
         */
        static Vec4 Floor(const Vec4& v) {
            return Vec4(std::floor(v.x), std::floor(v.y), 
                       std::floor(v.z), std::floor(v.w));
        }
        
        /**
         * @brief Computes ceil of vector components
         * @param v Input vector
         * @return Ceil vector
         */
        static Vec4 Ceil(const Vec4& v) {
            return Vec4(std::ceil(v.x), std::ceil(v.y), 
                       std::ceil(v.z), std::ceil(v.w));
        }
        
        /**
         * @brief Computes round of vector components
         * @param v Input vector
         * @return Round vector
         */
        static Vec4 Round(const Vec4& v) {
            return Vec4(std::round(v.x), std::round(v.y), 
                       std::round(v.z), std::round(v.w));
        }
        
        /**
         * @brief Computes sign of vector components
         * @param v Input vector
         * @return Sign vector (-1, 0, or 1 per component)
         */
        static Vec4 Sign(const Vec4& v) {
            return Vec4(
                v.x > 0.0f ? 1.0f : (v.x < 0.0f ? -1.0f : 0.0f),
                v.y > 0.0f ? 1.0f : (v.y < 0.0f ? -1.0f : 0.0f),
                v.z > 0.0f ? 1.0f : (v.z < 0.0f ? -1.0f : 0.0f),
                v.w > 0.0f ? 1.0f : (v.w < 0.0f ? -1.0f : 0.0f)
            );
        }
    };
    
    // Scalar-vector operations
    constexpr Vec4 operator+(float32 scalar, const Vec4& vec) {
        return vec + scalar;
    }
    
    constexpr Vec4 operator-(float32 scalar, const Vec4& vec) {
        return Vec4(scalar - vec.x, scalar - vec.y, 
                   scalar - vec.z, scalar - vec.w);
    }
    
    constexpr Vec4 operator*(float32 scalar, const Vec4& vec) {
        return vec * scalar;
    }
    
    constexpr Vec4 operator/(float32 scalar, const Vec4& vec) {
        return Vec4(scalar / vec.x, scalar / vec.y, 
                   scalar / vec.z, scalar / vec.w);
    }
    
} // namespace DSRT
