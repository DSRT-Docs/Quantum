/**
 * @file Matrix4.h
 * @brief 4x4 matrix for 3D transformations
 * @ingroup Math
 * 
 * @details Column-major 4x4 matrix class for 3D graphics transformations.
 *          Supports view, projection, and world transformation matrices.
 */

#pragma once
#include "Export.h"
#include "Vector3.h"
#include "Vector4.h"

namespace DSRT {
namespace Math {

/**
 * @class Matrix4
 * @brief 4x4 column-major transformation matrix
 * 
 * @details Standard representation for 3D transformations in computer graphics.
 *          Column-major storage compatible with OpenGL and Vulkan.
 * 
 * @layout Column-major order:
 *   [0]  [4]  [8]  [12]    m00 m10 m20 m30
 *   [1]  [5]  [9]  [13]    m01 m11 m21 m31
 *   [2]  [6]  [10] [14]    m02 m12 m22 m32
 *   [3]  [7]  [11] [15]    m03 m13 m23 m33
 * 
 * @example Create transformation matrix:
 * @code
 * Matrix4 transform = Matrix4::Translation(Vector3(10, 0, 0)) *
 *                     Matrix4::Rotation(Vector3::Up(), 45.0f) *
 *                     Matrix4::Scaling(Vector3(2, 2, 2));
 * @endcode
 * 
 * @example View and projection matrices:
 * @code
 * Matrix4 view = Matrix4::LookAt(cameraPos, target, Vector3::Up());
 * Matrix4 proj = Matrix4::Perspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
 * Matrix4 mvp = proj * view * model;
 * @endcode
 */
class DSRT_API Matrix4 {
public:
    float m[16]; /**< Matrix elements in column-major order */
    
    /**
     * @brief Default constructor (identity matrix)
     */
    Matrix4();
    
    /**
     * @brief Construct from float array
     * @param data 16 floats in column-major order
     */
    explicit Matrix4(const float data[16]);
    
    /**
     * @brief Construct from individual elements
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
    Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);
    
    /**
     * @brief Matrix multiplication
     * @param other Right-hand matrix
     * @return Product matrix (this × other)
     * 
     * @note Order matters: A * B applies B then A
     */
    Matrix4 operator*(const Matrix4& other) const;
    
    /**
     * @brief Matrix-vector multiplication
     * @param vec 4D vector
     * @return Transformed vector
     */
    Vector4 operator*(const Vector4& vec) const;
    
    /**
     * @brief Matrix addition
     * @param other Matrix to add
     * @return Sum matrix
     */
    Matrix4 operator+(const Matrix4& other) const;
    
    /**
     * @brief Matrix subtraction
     * @param other Matrix to subtract
     * @return Difference matrix
     */
    Matrix4 operator-(const Matrix4& other) const;
    
    /**
     * @brief Scalar multiplication
     * @param scalar Scalar value
     * @return Scaled matrix
     */
    Matrix4 operator*(float scalar) const;
    
    /**
     * @brief Matrix inverse
     * @return Inverse matrix
     * @throws std::runtime_error if matrix is singular
     */
    Matrix4 Inverse() const;
    
    /**
     * @brief Matrix transpose
     * @return Transposed matrix
     */
    Matrix4 Transpose() const;
    
    /**
     * @brief Matrix determinant
     * @return Determinant value
     */
    float Determinant() const;
    
    /**
     * @brief Check if matrix is identity
     * @return true if matrix is identity (within epsilon)
     */
    bool IsIdentity() const;
    
    // Factory methods for common transformations
    
    /**
     * @brief Create translation matrix
     * @param translation Translation vector
     * @return Translation matrix
     */
    static Matrix4 Translation(const Vector3& translation);
    
    /**
     * @brief Create rotation matrix from axis-angle
     * @param axis Rotation axis (normalized)
     * @param angle Rotation angle in radians
     * @return Rotation matrix
     */
    static Matrix4 Rotation(const Vector3& axis, float angle);
    
    /**
     * @brief Create rotation matrix from Euler angles
     * @param pitch X-axis rotation in degrees
     * @param yaw Y-axis rotation in degrees
     * @param roll Z-axis rotation in degrees
     * @return Rotation matrix
     * 
     * @note Rotation order: roll (Z) → pitch (X) → yaw (Y)
     */
    static Matrix4 RotationEuler(float pitch, float yaw, float roll);
    
    /**
     * @brief Create scaling matrix
     * @param scale Scaling factors
     * @return Scaling matrix
     */
    static Matrix4 Scaling(const Vector3& scale);
    
    // View and projection matrices
    
    /**
     * @brief Create look-at view matrix
     * @param eye Camera position
     * @param target Look-at target
     * @param up Up vector
     * @return View matrix
     */
    static Matrix4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    
    /**
     * @brief Create perspective projection matrix
     * @param fov Vertical field of view in degrees
     * @param aspect Width/height aspect ratio
     * @param near Near clipping plane
     * @param far Far clipping plane
     * @return Perspective matrix
     */
    static Matrix4 Perspective(float fov, float aspect, float near, float far);
    
    /**
     * @brief Create orthographic projection matrix
     * @param left Left clipping plane
     * @param right Right clipping plane
     * @param bottom Bottom clipping plane
     * @param top Top clipping plane
     * @param near Near clipping plane
     * @param far Far clipping plane
     * @return Orthographic matrix
     */
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
    
    // Decomposition methods
    
    /**
     * @brief Extract translation from matrix
     * @return Translation vector (last column)
     */
    Vector3 GetTranslation() const;
    
    /**
     * @brief Extract rotation from matrix
     * @return Pure rotation matrix
     */
    Matrix4 GetRotation() const;
    
    /**
     * @brief Extract scale from matrix
     * @return Scaling factors
     */
    Vector3 GetScale() const;
    
    /**
     * @brief Decompose matrix into translation, rotation, scale
     * @param[out] translation Extracted translation
     * @param[out] rotation Extracted rotation
     * @param[out] scale Extracted scale
     */
    void Decompose(Vector3& translation, Matrix4& rotation, Vector3& scale) const;
    
    // Constants
    
    /**
     * @brief Identity matrix
     * @return 4x4 identity matrix
     */
    static Matrix4 Identity();
    
    /**
     * @brief Zero matrix
     * @return 4x4 zero matrix
     */
    static Matrix4 Zero();
    
    // Element access
    
    /**
     * @brief Array access operator
     * @param index Element index [0-15]
     * @return Reference to element
     */
    float& operator[](int index);
    
    /**
     * @brief Const array access operator
     * @param index Element index [0-15]
     * @return Const reference to element
     */
    const float& operator[](int index) const;
    
    /**
     * @brief Get element at row/column
     * @param row Row index [0-3]
     * @param col Column index [0-3]
     * @return Element value
     */
    float At(int row, int col) const;
    
    /**
     * @brief Set element at row/column
     * @param row Row index [0-3]
     * @param col Column index [0-3]
     * @param value New value
     */
    void Set(int row, int col, float value);
};

} // namespace Math
} // namespace DSRT
