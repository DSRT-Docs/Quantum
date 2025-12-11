#pragma once

/**
 * @file Transform.h
 * @brief 3D transform implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "Mat4.h"

namespace DSRT {
    
    /**
     * @brief 3D transform class
     */
    class Transform {
    public:
        Vec3 position;
        Quaternion rotation;
        Vec3 scale;
        
        /**
         * @brief Constructs an identity transform
         */
        constexpr Transform() 
            : position(0.0f, 0.0f, 0.0f)
            , rotation(Quaternion::Identity())
            , scale(1.0f, 1.0f, 1.0f) {}
        
        /**
         * @brief Constructs a transform with given components
         * @param position Position
         * @param rotation Rotation
         * @param scale Scale
         */
        constexpr Transform(const Vec3& position, 
                          const Quaternion& rotation = Quaternion::Identity(),
                          const Vec3& scale = Vec3(1.0f, 1.0f, 1.0f))
            : position(position), rotation(rotation), scale(scale) {}
        
        /**
         * @brief Constructs a transform from matrix
         * @param mat Transformation matrix
         */
        explicit Transform(const Mat4& mat) {
            FromMatrix(mat);
        }
        
        // Access operators
        constexpr bool operator==(const Transform& other) const {
            return position == other.position && 
                   rotation == other.rotation && 
                   scale == other.scale;
        }
        
        constexpr bool operator!=(const Transform& other) const {
            return !(*this == other);
        }
        
        /**
         * @brief Combines transforms (this * other)
         * @param other Other transform
         * @return Combined transform
         */
        Transform operator*(const Transform& other) const {
            Transform result;
            result.position = position + rotation.Rotate(other.position * scale);
            result.rotation = rotation * other.rotation;
            result.scale = scale * other.scale;
            return result;
        }
        
        /**
         * @brief Inverses the transform
         * @return Inverse transform
         */
        Transform Inverse() const {
            Transform result;
            result.rotation = rotation.Conjugate();
            result.scale = Vec3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
            result.position = result.rotation.Rotate(-position) * result.scale;
            return result;
        }
        
        /**
         * @brief Transforms a point
         * @param point Point to transform
         * @return Transformed point
         */
        Vec3 TransformPoint(const Vec3& point) const {
            return position + rotation.Rotate(point * scale);
        }
        
        /**
         * @brief Transforms a direction (ignores translation)
         * @param direction Direction to transform
         * @return Transformed direction
         */
        Vec3 TransformDirection(const Vec3& direction) const {
            return rotation.Rotate(direction);
        }
        
        /**
         * @brief Inverse transforms a point
         * @param point Point to inverse transform
         * @return Inverse transformed point
         */
        Vec3 InverseTransformPoint(const Vec3& point) const {
            Vec3 invScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
            return rotation.Conjugate().Rotate((point - position) * invScale);
        }
        
        /**
         * @brief Inverse transforms a direction
         * @param direction Direction to inverse transform
         * @return Inverse transformed direction
         */
        Vec3 InverseTransformDirection(const Vec3& direction) const {
            return rotation.Conjugate().Rotate(direction);
        }
        
        /**
         * @brief Gets forward direction
         * @return Forward vector
         */
        Vec3 Forward() const {
            return rotation.Rotate(Vec3::Forward());
        }
        
        /**
         * @brief Gets backward direction
         * @return Backward vector
         */
        Vec3 Backward() const {
            return rotation.Rotate(Vec3::Back());
        }
        
        /**
         * @brief Gets right direction
         * @return Right vector
         */
        Vec3 Right() const {
            return rotation.Rotate(Vec3::Right());
        }
        
        /**
         * @brief Gets left direction
         * @return Left vector
         */
        Vec3 Left() const {
            return rotation.Rotate(Vec3::Left());
        }
        
        /**
         * @brief Gets up direction
         * @return Up vector
         */
        Vec3 Up() const {
            return rotation.Rotate(Vec3::Up());
        }
        
        /**
         * @brief Gets down direction
         * @return Down vector
         */
        Vec3 Down() const {
            return rotation.Rotate(Vec3::Down());
        }
        
        /**
         * @brief Converts to transformation matrix
         * @return Transformation matrix
         */
        Mat4 ToMatrix() const {
            Mat4 translationMat = Mat4::Translation(position);
            Mat4 rotationMat = rotation.ToMatrix();
            Mat4 scaleMat = Mat4::Scaling(scale);
            return translationMat * rotationMat * scaleMat;
        }
        
        /**
         * @brief Converts to transformation matrix without scale
         * @return Transformation matrix without scale
         */
        Mat4 ToMatrixNoScale() const {
            Mat4 translationMat = Mat4::Translation(position);
            Mat4 rotationMat = rotation.ToMatrix();
            return translationMat * rotationMat;
        }
        
        /**
         * @brief Sets from transformation matrix
         * @param mat Transformation matrix
         */
        void FromMatrix(const Mat4& mat) {
            // Extract position
            position = Vec3(mat(3, 0), mat(3, 1), mat(3, 2));
            
            // Extract scale
            scale.x = Vec3(mat(0, 0), mat(1, 0), mat(2, 0)).Length();
            scale.y = Vec3(mat(0, 1), mat(1, 1), mat(2, 1)).Length();
            scale.z = Vec3(mat(0, 2), mat(1, 2), mat(2, 2)).Length();
            
            // Remove scale from matrix
            Mat4 rotationMat = mat;
            if (scale.x != 0.0f) {
                float32 invX = 1.0f / scale.x;
                rotationMat(0, 0) *= invX;
                rotationMat(1, 0) *= invX;
                rotationMat(2, 0) *= invX;
            }
            if (scale.y != 0.0f) {
                float32 invY = 1.0f / scale.y;
                rotationMat(0, 1) *= invY;
                rotationMat(1, 1) *= invY;
                rotationMat(2, 1) *= invY;
            }
            if (scale.z != 0.0f) {
                float32 invZ = 1.0f / scale.z;
                rotationMat(0, 2) *= invZ;
                rotationMat(1, 2) *= invZ;
                rotationMat(2, 2) *= invZ;
            }
            
            // Extract rotation
            rotation = Quaternion::FromMatrix(rotationMat);
        }
        
        /**
         * @brief Linearly interpolates between transforms
         * @param other Target transform
         * @param t Interpolation factor (0 to 1)
         * @return Interpolated transform
         */
        Transform Lerp(const Transform& other, float32 t) const {
            Transform result;
            result.position = position.Lerp(other.position, t);
            result.rotation = rotation.Slerp(other.rotation, t);
            result.scale = scale.Lerp(other.scale, t);
            return result;
        }
        
        /**
         * @brief Sets position
         * @param newPosition New position
         */
        void SetPosition(const Vec3& newPosition) {
            position = newPosition;
        }
        
        /**
         * @brief Sets rotation
         * @param newRotation New rotation
         */
        void SetRotation(const Quaternion& newRotation) {
            rotation = newRotation;
        }
        
        /**
         * @brief Sets rotation from Euler angles
         * @param euler Euler angles in radians
         */
        void SetRotation(const Vec3& euler) {
            rotation = Quaternion::FromEuler(euler);
        }
        
        /**
         * @brief Sets rotation from axis-angle
         * @param axis Rotation axis
         * @param angle Rotation angle in radians
         */
        void SetRotation(const Vec3& axis, float32 angle) {
            rotation = Quaternion::FromAxisAngle(axis, angle);
        }
        
        /**
         * @brief Sets scale
         * @param newScale New scale
         */
        void SetScale(const Vec3& newScale) {
            scale = newScale;
        }
        
        /**
         * @brief Sets uniform scale
         * @param newScale New scale factor
         */
        void SetScale(float32 newScale) {
            scale = Vec3(newScale, newScale, newScale);
        }
        
        /**
         * @brief Translates the transform
         * @param translation Translation vector
         */
        void Translate(const Vec3& translation) {
            position += translation;
        }
        
        /**
         * @brief Translates the transform relative to its rotation
         * @param translation Local translation vector
         */
        void TranslateLocal(const Vec3& translation) {
            position += rotation.Rotate(translation);
        }
        
        /**
         * @brief Rotates the transform
         * @param rot Rotation quaternion
         */
        void Rotate(const Quaternion& rot) {
            rotation = rot * rotation;
        }
        
        /**
         * @brief Rotates the transform around axis
         * @param axis Rotation axis
         * @param angle Rotation angle in radians
         */
        void Rotate(const Vec3& axis, float32 angle) {
            rotation = Quaternion::FromAxisAngle(axis, angle) * rotation;
        }
        
        /**
         * @brief Rotates the transform around local axis
         * @param axis Local rotation axis
         * @param angle Rotation angle in radians
         */
        void RotateLocal(const Vec3& axis, float32 angle) {
            Vec3 worldAxis = rotation.Rotate(axis);
            rotation = Quaternion::FromAxisAngle(worldAxis, angle) * rotation;
        }
        
        /**
         * @brief Scales the transform
         * @param scaling Scale vector
         */
        void Scale(const Vec3& scaling) {
            scale = scale * scaling;
        }
        
        /**
         * @brief Uniformly scales the transform
         * @param scaling Scale factor
         */
        void Scale(float32 scaling) {
            scale = scale * scaling;
        }
        
        /**
         * @brief Looks at target point
         * @param target Target point
         * @param up Up direction
         */
        void LookAt(const Vec3& target, const Vec3& up = Vec3::Up()) {
            Vec3 direction = (target - position).Normalized();
            rotation = Quaternion::LookRotation(direction, up);
        }
        
        /**
         * @brief Resets transform to identity
         */
        void Reset() {
            position = Vec3(0.0f, 0.0f, 0.0f);
            rotation = Quaternion::Identity();
            scale = Vec3(1.0f, 1.0f, 1.0f);
        }
        
        /**
         * @brief Checks if transform is identity
         * @param epsilon Tolerance
         * @return True if identity transform
         */
        bool IsIdentity(float32 epsilon = 1e-6f) const {
            return position.IsZero(epsilon) && 
                   rotation.IsIdentity(epsilon) && 
                   (scale - Vec3(1.0f, 1.0f, 1.0f)).IsZero(epsilon);
        }
        
        // Static methods
        
        /**
         * @brief Identity transform
         * @return Identity transform
         */
        static constexpr Transform Identity() {
            return Transform();
        }
        
        /**
         * @brief Creates translation transform
         * @param translation Translation vector
         * @return Translation transform
         */
        static constexpr Transform Translation(const Vec3& translation) {
            return Transform(translation, Quaternion::Identity(), Vec3::One());
        }
        
        /**
         * @brief Creates rotation transform
         * @param rotation Rotation quaternion
         * @return Rotation transform
         */
        static constexpr Transform Rotation(const Quaternion& rotation) {
            return Transform(Vec3::Zero(), rotation, Vec3::One());
        }
        
        /**
         * @brief Creates scaling transform
         * @param scale Scale vector
         * @return Scaling transform
         */
        static constexpr Transform Scaling(const Vec3& scale) {
            return Transform(Vec3::Zero(), Quaternion::Identity(), scale);
        }
    };
    
} // namespace DSRT
