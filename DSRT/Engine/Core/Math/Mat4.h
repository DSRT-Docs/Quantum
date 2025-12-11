#pragma once

/**
 * @file Mat4.h
 * @brief 4x4 matrix implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "Vec3.h"
#include "Vec4.h"
#include <cmath>

namespace DSRT {
    
    /**
     * @brief 4x4 matrix class
     * @note Column-major storage for OpenGL compatibility
     */
    class Mat4 {
    public:
        union {
            float32 m[16];
            float32 m4[4][4];
            Vec4 columns[4];
        };
        
        /**
         * @brief Constructs an identity matrix
         */
        constexpr Mat4() {
            SetIdentity();
        }
        
        /**
         * @brief Constructs a matrix with given values
         * @param m00 Element [0][0]
         * @param m01 Element [0][1]
         * @param m02 Element [0][2]
         * @param m03 Element [0][3]
         * @param m10 Element [1][0]
         * @param m11 Element [1][1]
         * @param m12 Element [1][2]
         * @param m13 Element [1][3]
         * @param m20 Element [2][0]
         * @param m21 Element [2][1]
         * @param m22 Element [2][2]
         * @param m23 Element [2][3]
         * @param m30 Element [3][0]
         * @param m31 Element [3][1]
         * @param m32 Element [3][2]
         * @param m33 Element [3][3]
         */
        constexpr Mat4(float32 m00, float32 m01, float32 m02, float32 m03,
                      float32 m10, float32 m11, float32 m12, float32 m13,
                      float32 m20, float32 m21, float32 m22, float32 m23,
                      float32 m30, float32 m31, float32 m32, float32 m33) {
            m[0] = m00; m[4] = m01; m[8] = m02; m[12] = m03;
            m[1] = m10; m[5] = m11; m[9] = m12; m[13] = m13;
            m[2] = m20; m[6] = m21; m[10] = m22; m[14] = m23;
            m[3] = m30; m[7] = m31; m[11] = m32; m[15] = m33;
        }
        
        /**
         * @brief Constructs a matrix from column vectors
         * @param col0 First column
         * @param col1 Second column
         * @param col2 Third column
         * @param col3 Fourth column
         */
        constexpr Mat4(const Vec4& col0, const Vec4& col1,
                      const Vec4& col2, const Vec4& col3) {
            columns[0] = col0;
            columns[1] = col1;
            columns[2] = col2;
            columns[3] = col3;
        }
        
        // Access operators
        constexpr float32& operator()(int row, int col) {
            DSRT_ASSERT(row >= 0 && row < 4 && col >= 0 && col < 4);
            return m[col * 4 + row]; // Column-major
        }
        
        constexpr const float32& operator()(int row, int col) const {
            DSRT_ASSERT(row >= 0 && row < 4 && col >= 0 && col < 4);
            return m[col * 4 + row]; // Column-major
        }
        
        constexpr float32* Data() { return m; }
        constexpr const float32* Data() const { return m; }
        
        // Arithmetic operators
        constexpr Mat4 operator+(const Mat4& other) const {
            Mat4 result;
            for (int i = 0; i < 16; ++i) {
                result.m[i] = m[i] + other.m[i];
            }
            return result;
        }
        
        constexpr Mat4 operator-(const Mat4& other) const {
            Mat4 result;
            for (int i = 0; i < 16; ++i) {
                result.m[i] = m[i] - other.m[i];
            }
            return result;
        }
        
        constexpr Mat4 operator*(const Mat4& other) const {
            Mat4 result;
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    float32 sum = 0.0f;
                    for (int k = 0; k < 4; ++k) {
                        sum += (*this)(row, k) * other(k, col);
                    }
                    result(row, col) = sum;
                }
            }
            return result;
        }
        
        constexpr Vec4 operator*(const Vec4& vec) const {
            return Vec4(
                m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w,
                m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w,
                m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
                m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
            );
        }
        
        constexpr Mat4 operator*(float32 scalar) const {
            Mat4 result;
            for (int i = 0; i < 16; ++i) {
                result.m[i] = m[i] * scalar;
            }
            return result;
        }
        
        constexpr Mat4 operator/(float32 scalar) const {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            Mat4 result;
            for (int i = 0; i < 16; ++i) {
                result.m[i] = m[i] * inv;
            }
            return result;
        }
        
        // Assignment operators
        constexpr Mat4& operator+=(const Mat4& other) {
            for (int i = 0; i < 16; ++i) {
                m[i] += other.m[i];
            }
            return *this;
        }
        
        constexpr Mat4& operator-=(const Mat4& other) {
            for (int i = 0; i < 16; ++i) {
                m[i] -= other.m[i];
            }
            return *this;
        }
        
        constexpr Mat4& operator*=(const Mat4& other) {
            *this = *this * other;
            return *this;
        }
        
        constexpr Mat4& operator*=(float32 scalar) {
            for (int i = 0; i < 16; ++i) {
                m[i] *= scalar;
            }
            return *this;
        }
        
        constexpr Mat4& operator/=(float32 scalar) {
            DSRT_ASSERT(scalar != 0.0f);
            float32 inv = 1.0f / scalar;
            for (int i = 0; i < 16; ++i) {
                m[i] *= inv;
            }
            return *this;
        }
        
        // Comparison operators
        constexpr bool operator==(const Mat4& other) const {
            for (int i = 0; i < 16; ++i) {
                if (m[i] != other.m[i]) return false;
            }
            return true;
        }
        
        constexpr bool operator!=(const Mat4& other) const {
            return !(*this == other);
        }
        
        // Matrix operations
        
        /**
         * @brief Sets matrix to identity
         * @return Reference to this matrix
         */
        constexpr Mat4& SetIdentity() {
            m[0] = 1.0f; m[4] = 0.0f; m[8] = 0.0f; m[12] = 0.0f;
            m[1] = 0.0f; m[5] = 1.0f; m[9] = 0.0f; m[13] = 0.0f;
            m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = 0.0f;
            m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
            return *this;
        }
        
        /**
         * @brief Sets matrix to zero
         * @return Reference to this matrix
         */
        constexpr Mat4& SetZero() {
            for (int i = 0; i < 16; ++i) {
                m[i] = 0.0f;
            }
            return *this;
        }
        
        /**
         * @brief Transposes the matrix
         * @return Transposed matrix
         */
        constexpr Mat4 Transposed() const {
            Mat4 result;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    result(row, col) = (*this)(col, row);
                }
            }
            return result;
        }
        
        /**
         * @brief Transposes this matrix in place
         * @return Reference to this matrix
         */
        constexpr Mat4& Transpose() {
            *this = Transposed();
            return *this;
        }
        
        /**
         * @brief Computes matrix determinant
         * @return Determinant
         */
        float32 Determinant() const {
            float32 det = 0.0f;
            
            // Using Laplace expansion for clarity
            float32 minor[12];
            
            minor[0] = m[5] * m[10] * m[15];
            minor[1] = m[6] * m[11] * m[13];
            minor[2] = m[7] * m[9] * m[14];
            minor[3] = m[4] * m[11] * m[14];
            minor[4] = m[6] * m[8] * m[15];
            minor[5] = m[7] * m[10] * m[12];
            minor[6] = m[4] * m[10] * m[15];
            minor[7] = m[6] * m[8] * m[13];
            minor[8] = m[7] * m[9] * m[12];
            minor[9] = m[4] * m[9] * m[14];
            minor[10] = m[5] * m[8] * m[15];
            minor[11] = m[7] * m[10] * m[13];
            
            det += m[0] * (minor[0] + minor[1] + minor[2] 
                          - minor[3] - minor[4] - minor[5]);
            det -= m[1] * (minor[6] + minor[7] + minor[8] 
                          - minor[9] - minor[10] - minor[11]);
            det += m[2] * (m[4] * m[9] * m[15] + m[5] * m[11] * m[12] 
                          + m[7] * m[8] * m[13] - m[7] * m[9] * m[12] 
                          - m[5] * m[8] * m[15] - m[4] * m[11] * m[13]);
            det -= m[3] * (m[4] * m[9] * m[14] + m[5] * m[10] * m[12] 
                          + m[6] * m[8] * m[13] - m[6] * m[9] * m[12] 
                          - m[5] * m[8] * m[14] - m[4] * m[10] * m[13]);
            
            return det;
        }
        
        /**
         * @brief Computes inverse matrix
         * @return Inverse matrix
         */
        Mat4 Inverted() const {
            float32 det = Determinant();
            DSRT_ASSERT(det != 0.0f);
            
            Mat4 inv;
            float32 invDet = 1.0f / det;
            
            inv.m[0] = (m[5] * m[10] * m[15] + m[6] * m[11] * m[13] 
                       + m[7] * m[9] * m[14] - m[7] * m[10] * m[13] 
                       - m[6] * m[9] * m[15] - m[5] * m[11] * m[14]) * invDet;
            
            inv.m[1] = (m[1] * m[11] * m[14] + m[2] * m[9] * m[15] 
                       + m[3] * m[10] * m[13] - m[3] * m[10] * m[13] 
                       - m[2] * m[11] * m[13] - m[1] * m[10] * m[15]) * invDet;
            
            inv.m[2] = (m[1] * m[6] * m[15] + m[2] * m[7] * m[13] 
                       + m[3] * m[5] * m[14] - m[3] * m[6] * m[13] 
                       - m[2] * m[5] * m[15] - m[1] * m[7] * m[14]) * invDet;
            
            inv.m[3] = (m[1] * m[6] * m[11] + m[2] * m[7] * m[9] 
                       + m[3] * m[5] * m[10] - m[3] * m[6] * m[9] 
                       - m[2] * m[5] * m[11] - m[1] * m[7] * m[10]) * invDet;
            
            inv.m[4] = (m[4] * m[11] * m[14] + m[6] * m[8] * m[15] 
                       + m[7] * m[10] * m[12] - m[7] * m[11] * m[12] 
                       - m[6] * m[10] * m[15] - m[4] * m[10] * m[15]) * invDet;
            
            inv.m[5] = (m[0] * m[10] * m[15] + m[2] * m[11] * m[12] 
                       + m[3] * m[8] * m[14] - m[3] * m[10] * m[12] 
                       - m[2] * m[8] * m[15] - m[0] * m[11] * m[14]) * invDet;
            
            inv.m[6] = (m[0] * m[7] * m[14] + m[2] * m[4] * m[15] 
                       + m[3] * m[6] * m[12] - m[3] * m[7] * m[12] 
                       - m[2] * m[5] * m[14] - m[0] * m[6] * m[15]) * invDet;
            
            inv.m[7] = (m[0] * m[7] * m[10] + m[2] * m[4] * m[11] 
                       + m[3] * m[6] * m[8] - m[3] * m[7] * m[8] 
                       - m[2] * m[5] * m[11] - m[0] * m[6] * m[10]) * invDet;
            
            inv.m[8] = (m[4] * m[9] * m[15] + m[5] * m[11] * m[12] 
                       + m[7] * m[8] * m[13] - m[7] * m[9] * m[12] 
                       - m[5] * m[8] * m[15] - m[4] * m[11] * m[13]) * invDet;
            
            inv.m[9] = (m[0] * m[9] * m[15] + m[1] * m[11] * m[12] 
                       + m[3] * m[8] * m[13] - m[3] * m[9] * m[12] 
                       - m[1] * m[8] * m[15] - m[0] * m[11] * m[13]) * invDet;
            
            inv.m[10] = (m[0] * m[5] * m[15] + m[1] * m[7] * m[12] 
                        + m[3] * m[4] * m[13] - m[3] * m[5] * m[12] 
                        - m[1] * m[4] * m[15] - m[0] * m[7] * m[13]) * invDet;
            
            inv.m[11] = (m[0] * m[5] * m[11] + m[1] * m[7] * m[8] 
                        + m[3] * m[4] * m[9] - m[3] * m[5] * m[8] 
                        - m[1] * m[4] * m[11] - m[0] * m[7] * m[9]) * invDet;
            
            inv.m[12] = (m[4] * m[9] * m[14] + m[5] * m[10] * m[12] 
                        + m[6] * m[8] * m[13] - m[6] * m[9] * m[12] 
                        - m[5] * m[8] * m[14] - m[4] * m[10] * m[13]) * invDet;
            
            inv.m[13] = (m[0] * m[10] * m[13] + m[1] * m[8] * m[14] 
                        + m[2] * m[9] * m[12] - m[2] * m[10] * m[12] 
                        - m[1] * m[9] * m[12] - m[0] * m[8] * m[14]) * invDet;
            
            inv.m[14] = (m[0] * m[6] * m[13] + m[1] * m[4] * m[14] 
                        + m[2] * m[5] * m[12] - m[2] * m[6] * m[12] 
                        - m[1] * m[5] * m[14] - m[0] * m[4] * m[13]) * invDet;
            
            inv.m[15] = (m[0] * m[6] * m[9] + m[1] * m[4] * m[10] 
                        + m[2] * m[5] * m[8] - m[2] * m[6] * m[8] 
                        - m[1] * m[5] * m[10] - m[0] * m[4] * m[9]) * invDet;
            
            return inv;
        }
        
        /**
         * @brief Inverts this matrix in place
         * @return Reference to this matrix
         */
        Mat4& Invert() {
            *this = Inverted();
            return *this;
        }
        
        /**
         * @brief Checks if matrix is identity
         * @param epsilon Tolerance
         * @return True if identity matrix
         */
        bool IsIdentity(float32 epsilon = 1e-6f) const {
            if (std::abs(m[0] - 1.0f) > epsilon) return false;
            if (std::abs(m[5] - 1.0f) > epsilon) return false;
            if (std::abs(m[10] - 1.0f) > epsilon) return false;
            if (std::abs(m[15] - 1.0f) > epsilon) return false;
            
            for (int i = 1; i < 16; ++i) {
                if (i == 0 || i == 5 || i == 10 || i == 15) continue;
                if (std::abs(m[i]) > epsilon) return false;
            }
            return true;
        }
        
        /**
         * @brief Extracts translation component
         * @return Translation vector
         */
        constexpr Vec3 GetTranslation() const {
            return Vec3(m[12], m[13], m[14]);
        }
        
        /**
         * @brief Extracts rotation component (assumes uniform scale)
         * @return Rotation matrix
         */
        Mat3 GetRotation() const;
        
        /**
         * @brief Extracts scale component
         * @return Scale vector
         */
        constexpr Vec3 GetScale() const {
            return Vec3(
                Vec3(m[0], m[1], m[2]).Length(),
                Vec3(m[4], m[5], m[6]).Length(),
                Vec3(m[8], m[9], m[10]).Length()
            );
        }
        
        // Static factory methods
        
        /**
         * @brief Creates identity matrix
         * @return Identity matrix
         */
        static constexpr Mat4 Identity() {
            return Mat4();
        }
        
        /**
         * @brief Creates zero matrix
         * @return Zero matrix
         */
        static constexpr Mat4 Zero() {
            Mat4 mat;
            mat.SetZero();
            return mat;
        }
        
        /**
         * @brief Creates translation matrix
         * @param translation Translation vector
         * @return Translation matrix
         */
        static constexpr Mat4 Translation(const Vec3& translation) {
            Mat4 mat;
            mat.m[12] = translation.x;
            mat.m[13] = translation.y;
            mat.m[14] = translation.z;
            return mat;
        }
        
        /**
         * @brief Creates translation matrix
         * @param x X translation
         * @param y Y translation
         * @param z Z translation
         * @return Translation matrix
         */
        static constexpr Mat4 Translation(float32 x, float32 y, float32 z) {
            return Translation(Vec3(x, y, z));
        }
        
        /**
         * @brief Creates rotation matrix around X axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix
         */
        static Mat4 RotationX(float32 angle) {
            float32 c = std::cos(angle);
            float32 s = std::sin(angle);
            
            Mat4 mat;
            mat.m[5] = c;
            mat.m[6] = s;
            mat.m[9] = -s;
            mat.m[10] = c;
            return mat;
        }
        
        /**
         * @brief Creates rotation matrix around Y axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix
         */
        static Mat4 RotationY(float32 angle) {
            float32 c = std::cos(angle);
            float32 s = std::sin(angle);
            
            Mat4 mat;
            mat.m[0] = c;
            mat.m[2] = -s;
            mat.m[8] = s;
            mat.m[10] = c;
            return mat;
        }
        
        /**
         * @brief Creates rotation matrix around Z axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix
         */
        static Mat4 RotationZ(float32 angle) {
            float32 c = std::cos(angle);
            float32 s = std::sin(angle);
            
            Mat4 mat;
            mat.m[0] = c;
            mat.m[1] = s;
            mat.m[4] = -s;
            mat.m[5] = c;
            return mat;
        }
        
        /**
         * @brief Creates rotation matrix from axis-angle
         * @param axis Rotation axis (must be normalized)
         * @param angle Rotation angle in radians
         * @return Rotation matrix
         */
        static Mat4 RotationAxis(const Vec3& axis, float32 angle) {
            DSRT_ASSERT(axis.IsNormalized());
            
            float32 c = std::cos(angle);
            float32 s = std::sin(angle);
            float32 t = 1.0f - c;
            
            float32 x = axis.x;
            float32 y = axis.y;
            float32 z = axis.z;
            
            Mat4 mat;
            mat.m[0] = t * x * x + c;
            mat.m[1] = t * x * y + s * z;
            mat.m[2] = t * x * z - s * y;
            
            mat.m[4] = t * x * y - s * z;
            mat.m[5] = t * y * y + c;
            mat.m[6] = t * y * z + s * x;
            
            mat.m[8] = t * x * z + s * y;
            mat.m[9] = t * y * z - s * x;
            mat.m[10] = t * z * z + c;
            
            return mat;
        }
        
        /**
         * @brief Creates scaling matrix
         * @param scale Scale vector
         * @return Scaling matrix
         */
        static constexpr Mat4 Scaling(const Vec3& scale) {
            Mat4 mat;
            mat.m[0] = scale.x;
            mat.m[5] = scale.y;
            mat.m[10] = scale.z;
            return mat;
        }
        
        /**
         * @brief Creates scaling matrix
         * @param x X scale
         * @param y Y scale
         * @param z Z scale
         * @return Scaling matrix
         */
        static constexpr Mat4 Scaling(float32 x, float32 y, float32 z) {
            return Scaling(Vec3(x, y, z));
        }
        
        /**
         * @brief Creates uniform scaling matrix
         * @param scale Scale factor
         * @return Scaling matrix
         */
        static constexpr Mat4 Scaling(float32 scale) {
            return Scaling(Vec3(scale, scale, scale));
        }
        
        /**
         * @brief Creates look-at matrix
         * @param eye Eye position
         * @param target Target position
         * @param up Up vector
         * @return Look-at matrix
         */
        static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
            Vec3 z = (eye - target).Normalized();
            Vec3 x = up.Cross(z).Normalized();
            Vec3 y = z.Cross(x);
            
            Mat4 mat;
            mat.m[0] = x.x;
            mat.m[1] = y.x;
            mat.m[2] = z.x;
            
            mat.m[4] = x.y;
            mat.m[5] = y.y;
            mat.m[6] = z.y;
            
            mat.m[8] = x.z;
            mat.m[9] = y.z;
            mat.m[10] = z.z;
            
            mat.m[12] = -x.Dot(eye);
            mat.m[13] = -y.Dot(eye);
            mat.m[14] = -z.Dot(eye);
            
            return mat;
        }
        
        /**
         * @brief Creates perspective projection matrix
         * @param fov Field of view in radians
         * @param aspect Aspect ratio (width/height)
         * @param near Near plane distance
         * @param far Far plane distance
         * @return Perspective matrix
         */
        static Mat4 Perspective(float32 fov, float32 aspect, 
                               float32 near, float32 far) {
            DSRT_ASSERT(fov > 0.0f && aspect > 0.0f);
            DSRT_ASSERT(near > 0.0f && far > near);
            
            float32 tanHalfFov = std::tan(fov * 0.5f);
            float32 range = far - near;
            
            Mat4 mat;
            mat.m[0] = 1.0f / (aspect * tanHalfFov);
            mat.m[5] = 1.0f / tanHalfFov;
            mat.m[10] = -(far + near) / range;
            mat.m[11] = -1.0f;
            mat.m[14] = -(2.0f * far * near) / range;
            mat.m[15] = 0.0f;
            
            return mat;
        }
        
        /**
         * @brief Creates orthographic projection matrix
         * @param left Left plane
         * @param right Right plane
         * @param bottom Bottom plane
         * @param top Top plane
         * @param near Near plane
         * @param far Far plane
         * @return Orthographic matrix
         */
        static constexpr Mat4 Orthographic(float32 left, float32 right,
                                          float32 bottom, float32 top,
                                          float32 near, float32 far) {
            DSRT_ASSERT(left != right && bottom != top && near != far);
            
            Mat4 mat;
            mat.m[0] = 2.0f / (right - left);
            mat.m[5] = 2.0f / (top - bottom);
            mat.m[10] = -2.0f / (far - near);
            mat.m[12] = -(right + left) / (right - left);
            mat.m[13] = -(top + bottom) / (top - bottom);
            mat.m[14] = -(far + near) / (far - near);
            
            return mat;
        }
        
        /**
         * @brief Creates viewport matrix
         * @param x Viewport X position
         * @param y Viewport Y position
         * @param width Viewport width
         * @param height Viewport height
         * @param minDepth Minimum depth value
         * @param maxDepth Maximum depth value
         * @return Viewport matrix
         */
        static constexpr Mat4 Viewport(float32 x, float32 y,
                                      float32 width, float32 height,
                                      float32 minDepth = 0.0f,
                                      float32 maxDepth = 1.0f) {
            DSRT_ASSERT(width > 0.0f && height > 0.0f);
            DSRT_ASSERT(maxDepth > minDepth);
            
            Mat4 mat;
            mat.m[0] = width * 0.5f;
            mat.m[5] = -height * 0.5f;
            mat.m[10] = maxDepth - minDepth;
            mat.m[12] = x + width * 0.5f;
            mat.m[13] = y + height * 0.5f;
            mat.m[14] = minDepth;
            
            return mat;
        }
    };
    
    // Scalar-matrix operations
    constexpr Mat4 operator*(float32 scalar, const Mat4& mat) {
        return mat * scalar;
    }
    
} // namespace DSRT
