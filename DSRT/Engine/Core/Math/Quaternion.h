#pragma once

/**
 * @file Quaternion.h
 * @brief Quaternion implementation for rotations
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include <cmath>

namespace DSRT {
    
    /**
     * @brief Quaternion class for 3D rotations
     */
    class Quaternion {
    public:
        float32 x, y, z, w;
        
        /**
         * @brief Constructs an identity quaternion
         */
        constexpr Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        
        /**
         * @brief Constructs a quaternion with given components
         * @param x X component
         * @param y Y component
         * @param z Z component
         * @param w W component
         */
        constexpr Quaternion(float32 x, float32 y, float32 z, float32 w)
            : x(x), y(y), z(z), w(w) {}
        
        /**
         * @brief Constructs a quaternion from axis-angle
         * @param axis Rotation axis (must be normalized)
         * @param angle Rotation angle in radians
         */
        Quaternion(const Vec3& axis, float32 angle) {
            DSRT_ASSERT(axis.IsNormalized());
            float32 halfAngle = angle * 0.5f;
            float32 s = std::sin(halfAngle);
            x = axis.x * s;
            y = axis.y * s;
            z = axis.z * s;
            w = std::cos(halfAngle);
        }
        
        /**
         * @brief Constructs a quaternion from Euler angles
         * @param pitch X rotation in radians
         * @param yaw Y rotation in radians
         * @param roll Z rotation in radians
         */
        Quaternion(float32 pitch, float32 yaw, float32 roll) {
            float32 halfPitch = pitch * 0.5f;
            float32 halfYaw = yaw * 0.5f;
            float32 halfRoll = roll * 0.5f;
            
            float32 cy = std::cos(halfYaw);
            float32 sy = std::sin(halfYaw);
            float32 cp = std::cos(halfPitch);
            float32 sp = std::sin(halfPitch);
            float32 cr = std::cos(halfRoll);
            float32 sr = std::sin(halfRoll);
            
            w = cr * cp * cy + sr * sp * sy;
            x = sr * cp * cy - cr * sp * sy;
            y = cr * sp * cy + sr * cp * sy;
            z = cr * cp * sy - sr * sp * cy;
        }
        
        /**
         * @brief Constructs a quaternion from rotation matrix
         * @param mat Rotation matrix
         */
        explicit Quaternion(const Mat4& mat) {
            float32 trace = mat(0, 0) + mat(1, 1) + mat(2, 2);
            
            if (trace > 0.0f) {
                float32 s = 0.5f / std::sqrt(trace + 1.0f);
                w = 0.25f / s;
                x = (mat(2, 1) - mat(1, 2)) * s;
                y = (mat(0, 2) - mat(2, 0)) * s;
                z = (mat(1, 0) - mat(0, 1)) * s;
            } else {
                if (mat(0, 0) > mat(1, 1) && mat(0, 0) > mat(2, 2)) {
                    float32 s = 2.0f * std::sqrt(1.0f + mat(0, 0) - 
                                                mat(1, 1) - mat(2, 2));
                    w = (mat(2, 1) - mat(1, 2)) / s;
                    x = 0.25f * s;
                    y = (mat(0, 1) + mat(1, 0)) / s;
                    z = (mat(0, 2) + mat(2, 0)) / s;
                } else if (mat(1, 1) > mat(2, 2)) {
                    float32 s = 2.0f * std::sqrt(1.0f + mat(1, 1) - 
                                                mat(0, 0) - mat(2, 2));
                    w = (mat(0, 2) - mat(2, 0)) / s;
                    x = (mat(0, 1) + mat(1, 0)) / s;
                    y = 0.25f * s;
                    z = (mat(1, 2) + mat(2, 1)) / s;
                } else {
                    float32 s = 2.0f * std::sqrt(1.0f + mat(2, 2) - 
                                                mat(0, 0) - mat(1, 1));
                    w = (mat(1, 0) - mat(0, 1)) / s;
                    x = (mat(0, 2) + mat(2, 0)) / s;
                    y = (mat(1, 2) + mat(2, 1)) / s;
                    z = 0.25f * s;
                }
            }
        }
        
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
        constexpr Vec4 ToVec4() const {
            return Vec4(x, y, z, w);
        }
        
        constexpr Vec3 XYZ() const {
            return Vec3(x, y, z);
        }
        
        // Arithmetic operators
        constexpr Quaternion operator+(const Quaternion& other) const {
            return Quaternion(x + other.x, y + other.y, 
                             z + other.z, w + other.w);
        }
        
        constexpr Quaternion operator-(const Quaternion& other) const {
            return Quaternion(x - other.x, y - other.y, 
                             z - other.z, w - other.w);
        }
        
        constexpr Quaternion operator*(const Quaternion& other) const {
            return Quaternion(
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w,
                w * other.w - x * other.x - y * other.y - z * other.z
            );
        }
        
        constexpr Quaternion operator*(float32 scalar) const {
            return Quaternion(x * scalar, y * scalar, 
                             z * scalar, w * scalar);
        }
        
        constexpr Quaternion operator/(float32 scalar) const {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            return *this * inv;
        }
        
        // Assignment operators
        constexpr Quaternion& operator+=(const Quaternion& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }
        
        constexpr Quaternion& operator-=(const Quaternion& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }
        
        constexpr Quaternion& operator*=(const Quaternion& other) {
            *this = *this * other;
            return *this;
        }
        
        constexpr Quaternion& operator*=(float32 scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }
        
        constexpr Quaternion& operator/=(float32 scalar) {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            *this *= inv;
            return *this;
        }
        
        // Unary operators
        constexpr Quaternion operator+() const {
            return *this;
        }
        
        constexpr Quaternion operator-() const {
            return Quaternion(-x, -y, -z, -w);
        }
        
        // Comparison operators
        constexpr bool operator==(const Quaternion& other) const {
            return x == other.x && y == other.y && 
                   z == other.z && w == other.w;
        }
        
        constexpr bool operator!=(const Quaternion& other) const {
            return !(*this == other);
        }
        
        // Quaternion operations
        
        /**
         * @brief Computes conjugate
         * @return Conjugate quaternion
         */
        constexpr Quaternion Conjugate() const {
            return Quaternion(-x, -y, -z, w);
        }
        
        /**
         * @brief Computes inverse
         * @return Inverse quaternion
         */
        constexpr Quaternion Inverse() const {
            float32 normSq = NormSquared();
            DSRT_ASSERT(normSq > 0.0f);
            return Conjugate() / normSq;
        }
        
        /**
         * @brief Computes squared norm
         * @return Squared norm
         */
        constexpr float32 NormSquared() const {
            return x * x + y * y + z * z + w * w;
        }
        
        /**
         * @brief Computes norm (length)
         * @return Norm
         */
        float32 Norm() const {
            return std::sqrt(NormSquared());
        }
        
        /**
         * @brief Normalizes the quaternion
         * @return Normalized quaternion
         */
        Quaternion Normalized() const {
            float32 norm = Norm();
            DSRT_ASSERT(norm > 0.0f);
            return *this / norm;
        }
        
        /**
         * @brief Normalizes this quaternion in place
         * @return Reference to this quaternion
         */
        Quaternion& Normalize() {
            float32 norm = Norm();
            DSRT_ASSERT(norm > 0.0f);
            *this /= norm;
            return *this;
        }
        
        /**
         * @brief Checks if quaternion is normalized
         * @param epsilon Tolerance
         * @return True if normalized
         */
        bool IsNormalized(float32 epsilon = 1e-6f) const {
            return std::abs(NormSquared() - 1.0f) <= epsilon;
        }
        
        /**
         * @brief Checks if quaternion is identity
         * @param epsilon Tolerance
         * @return True if identity quaternion
         */
        bool IsIdentity(float32 epsilon = 1e-6f) const {
            return std::abs(x) <= epsilon && 
                   std::abs(y) <= epsilon && 
                   std::abs(z) <= epsilon && 
                   std::abs(w - 1.0f) <= epsilon;
        }
        
        /**
         * @brief Rotates a vector by this quaternion
         * @param vec Vector to rotate
         * @return Rotated vector
         */
        Vec3 Rotate(const Vec3& vec) const {
            DSRT_ASSERT(IsNormalized());
            
            // Optimized vector rotation using quaternion multiplication
            // q * v * q^-1, where v is a pure quaternion (0, vec)
            Vec3 u(x, y, z);
            float32 s = w;
            
            return u * 2.0f * u.Dot(vec) + 
                   vec * (s * s - u.Dot(u)) + 
                   u.Cross(vec) * 2.0f * s;
        }
        
        /**
         * @brief Converts to rotation matrix
         * @return Rotation matrix
         */
        Mat4 ToMatrix() const {
            DSRT_ASSERT(IsNormalized());
            
            float32 xx = x * x;
            float32 xy = x * y;
            float32 xz = x * z;
            float32 xw = x * w;
            
            float32 yy = y * y;
            float32 yz = y * z;
            float32 yw = y * w;
            
            float32 zz = z * z;
            float32 zw = z * w;
            
            Mat4 mat;
            mat(0, 0) = 1.0f - 2.0f * (yy + zz);
            mat(0, 1) = 2.0f * (xy - zw);
            mat(0, 2) = 2.0f * (xz + yw);
            
            mat(1, 0) = 2.0f * (xy + zw);
            mat(1, 1) = 1.0f - 2.0f * (xx + zz);
            mat(1, 2) = 2.0f * (yz - xw);
            
            mat(2, 0) = 2.0f * (xz - yw);
            mat(2, 1) = 2.0f * (yz + xw);
            mat(2, 2) = 1.0f - 2.0f * (xx + yy);
            
            return mat;
        }
        
        /**
         * @brief Converts to Euler angles (pitch, yaw, roll)
         * @return Euler angles in radians
         */
        Vec3 ToEuler() const {
            DSRT_ASSERT(IsNormalized());
            
            // Roll (x-axis rotation)
            float32 sinr_cosp = 2.0f * (w * x + y * z);
            float32 cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
            float32 roll = std::atan2(sinr_cosp, cosr_cosp);
            
            // Pitch (y-axis rotation)
            float32 sinp = 2.0f * (w * y - z * x);
            float32 pitch = std::abs(sinp) >= 1.0f ?
                std::copysign(3.14159265358979323846f / 2.0f, sinp) :
                std::asin(sinp);
            
            // Yaw (z-axis rotation)
            float32 siny_cosp = 2.0f * (w * z + x * y);
            float32 cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
            float32 yaw = std::atan2(siny_cosp, cosy_cosp);
            
            return Vec3(pitch, yaw, roll);
        }
        
        /**
         * @brief Gets rotation axis and angle
         * @param axis Output rotation axis
         * @param angle Output rotation angle in radians
         */
        void ToAxisAngle(Vec3& axis, float32& angle) const {
            DSRT_ASSERT(IsNormalized());
            
            float32 s = std::sqrt(1.0f - w * w);
            if (s < 1e-6f) {
                axis = Vec3(1.0f, 0.0f, 0.0f);
                angle = 0.0f;
            } else {
                axis = Vec3(x / s, y / s, z / s);
                angle = 2.0f * std::acos(w);
            }
        }
        
        /**
         * @brief Linearly interpolates between two quaternions
         * @param other Target quaternion
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated quaternion
         */
        constexpr Quaternion Lerp(const Quaternion& other, float32 t) const {
            return *this + (other - *this) * t;
        }
        
        /**
         * @brief Spherically interpolates between two quaternions
         * @param other Target quaternion
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated quaternion
         */
        Quaternion Slerp(const Quaternion& other, float32 t) const {
            DSRT_ASSERT(IsNormalized());
            DSRT_ASSERT(other.IsNormalized());
            
            float32 cosHalfTheta = Dot(other);
            
            // If quaternions are opposite, use lerp to avoid division by zero
            if (std::abs(cosHalfTheta) >= 1.0f) {
                return *this;
            }
            
            float32 halfTheta = std::acos(cosHalfTheta);
            float32 sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);
            
            // Avoid division by zero
            if (std::abs(sinHalfTheta) < 1e-6f) {
                return Quaternion(
                    x * 0.5f + other.x * 0.5f,
                    y * 0.5f + other.y * 0.5f,
                    z * 0.5f + other.z * 0.5f,
                    w * 0.5f + other.w * 0.5f
                ).Normalized();
            }
            
            float32 ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
            float32 ratioB = std::sin(t * halfTheta) / sinHalfTheta;
            
            return Quaternion(
                x * ratioA + other.x * ratioB,
                y * ratioA + other.y * ratioB,
                z * ratioA + other.z * ratioB,
                w * ratioA + other.w * ratioB
            );
        }
        
        /**
         * @brief Normalized linear interpolation
         * @param other Target quaternion
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated quaternion
         */
        Quaternion Nlerp(const Quaternion& other, float32 t) const {
            return Lerp(other, t).Normalized();
        }
        
        /**
         * @brief Computes dot product
         * @param other Other quaternion
         * @return Dot product
         */
        constexpr float32 Dot(const Quaternion& other) const {
            return x * other.x + y * other.y + 
                   z * other.z + w * other.w;
        }
        
        // Static methods
        
        /**
         * @brief Identity quaternion
         * @return Identity quaternion
         */
        static constexpr Quaternion Identity() {
            return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        }
        
        /**
         * @brief Creates quaternion from Euler angles
         * @param pitch X rotation in radians
         * @param yaw Y rotation in radians
         * @param roll Z rotation in radians
         * @return Quaternion
         */
        static Quaternion FromEuler(float32 pitch, float32 yaw, float32 roll) {
            return Quaternion(pitch, yaw, roll);
        }
        
        /**
         * @brief Creates quaternion from Euler angles
         * @param euler Euler angles vector (pitch, yaw, roll)
         * @return Quaternion
         */
        static Quaternion FromEuler(const Vec3& euler) {
            return Quaternion(euler.x, euler.y, euler.z);
        }
        
        /**
         * @brief Creates quaternion from axis-angle
         * @param axis Rotation axis
         * @param angle Rotation angle in radians
         * @return Quaternion
         */
        static Quaternion FromAxisAngle(const Vec3& axis, float32 angle) {
            return Quaternion(axis, angle);
        }
        
        /**
         * @brief Creates quaternion from rotation matrix
         * @param mat Rotation matrix
         * @return Quaternion
         */
        static Quaternion FromMatrix(const Mat4& mat) {
            return Quaternion(mat);
        }
        
        /**
         * @brief Creates quaternion that rotates from direction A to B
         * @param from Start direction
         * @param to Target direction
         * @return Rotation quaternion
         */
        static Quaternion FromToRotation(const Vec3& from, const Vec3& to) {
            Vec3 v0 = from.Normalized();
            Vec3 v1 = to.Normalized();
            
            float32 dot = v0.Dot(v1);
            
            if (dot > 0.999999f) {
                return Quaternion::Identity();
            } else if (dot < -0.999999f) {
                // 180 degree rotation about any orthogonal axis
                Vec3 axis = Vec3::UnitX().Cross(v0);
                if (axis.IsZero()) {
                    axis = Vec3::UnitY().Cross(v0);
                }
                axis.Normalize();
                return Quaternion(axis, 3.14159265358979323846f);
            } else {
                float32 s = std::sqrt((1.0f + dot) * 2.0f);
                float32 invs = 1.0f / s;
                
                Vec3 c = v0.Cross(v1);
                
                return Quaternion(
                    c.x * invs,
                    c.y * invs,
                    c.z * invs,
                    s * 0.5f
                );
            }
        }
        
        /**
         * @brief Creates quaternion from look rotation
         * @param forward Forward direction
         * @param up Up direction
         * @return Look rotation quaternion
         */
        static Quaternion LookRotation(const Vec3& forward, const Vec3& up = Vec3::Up()) {
            Vec3 f = forward.Normalized();
            Vec3 u = up.Normalized();
            Vec3 r = u.Cross(f).Normalized();
            u = f.Cross(r);
            
            Mat4 mat;
            mat(0, 0) = r.x;
            mat(1, 0) = r.y;
            mat(2, 0) = r.z;
            
            mat(0, 1) = u.x;
            mat(1, 1) = u.y;
            mat(2, 1) = u.z;
            
            mat(0, 2) = f.x;
            mat(1, 2) = f.y;
            mat(2, 2) = f.z;
            
            return FromMatrix(mat);
        }
    };
    
    // Scalar-quaternion operations
    constexpr Quaternion operator*(float32 scalar, const Quaternion& quat) {
        return quat * scalar;
    }
    
    // Vector rotation using quaternion
    inline Vec3 operator*(const Quaternion& quat, const Vec3& vec) {
        return quat.Rotate(vec);
    }
    
} // namespace DSRT
