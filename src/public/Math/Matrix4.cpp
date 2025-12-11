#include "DSRT/Math/Matrix4.h"
#include "DSRT/Math/MathConstants.h"
#include <cmath>
#include <cstring>
#include <sstream>

namespace DSRT {
namespace Math {

Matrix4::Matrix4() {
    std::memset(m, 0, sizeof(m));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Matrix4::Matrix4(const float data[16]) {
    std::memcpy(m, data, sizeof(m));
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03,
                 float m10, float m11, float m12, float m13,
                 float m20, float m21, float m22, float m23,
                 float m30, float m31, float m32, float m33) {
    m[0] = m00; m[4] = m10; m[8] = m20; m[12] = m30;
    m[1] = m01; m[5] = m11; m[9] = m21; m[13] = m31;
    m[2] = m02; m[6] = m12; m[10] = m22; m[14] = m32;
    m[3] = m03; m[7] = m13; m[11] = m23; m[15] = m33;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i + j * 4] = 
                m[i + 0 * 4] * other.m[0 + j * 4] +
                m[i + 1 * 4] * other.m[1 + j * 4] +
                m[i + 2 * 4] * other.m[2 + j * 4] +
                m[i + 3 * 4] * other.m[3 + j * 4];
        }
    }
    
    return result;
}

Vector4 Matrix4::operator*(const Vector4& vec) const {
    return Vector4(
        m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w,
        m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w,
        m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
        m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
    );
}

Matrix4 Matrix4::operator+(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 16; ++i) {
        result.m[i] = m[i] + other.m[i];
    }
    return result;
}

Matrix4 Matrix4::operator-(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 16; ++i) {
        result.m[i] = m[i] - other.m[i];
    }
    return result;
}

Matrix4 Matrix4::operator*(float scalar) const {
    Matrix4 result;
    for (int i = 0; i < 16; ++i) {
        result.m[i] = m[i] * scalar;
    }
    return result;
}

Matrix4 Matrix4::Inverse() const {
    Matrix4 inv;
    
    float invDet = 1.0f / Determinant();
    if (std::fabs(invDet) < EPSILON_F) {
        return Identity(); // Singular matrix, return identity
    }
    
    inv.m[0] = invDet * (m[5] * (m[10] * m[15] - m[11] * m[14]) +
                        m[6] * (m[11] * m[13] - m[9] * m[15]) +
                        m[7] * (m[9] * m[14] - m[10] * m[13]));
    
    inv.m[1] = invDet * (m[1] * (m[11] * m[14] - m[10] * m[15]) +
                        m[2] * (m[9] * m[15] - m[11] * m[13]) +
                        m[3] * (m[10] * m[13] - m[9] * m[14]));
    
    inv.m[2] = invDet * (m[1] * (m[6] * m[15] - m[7] * m[14]) +
                        m[2] * (m[7] * m[13] - m[5] * m[15]) +
                        m[3] * (m[5] * m[14] - m[6] * m[13]));
    
    inv.m[3] = invDet * (m[1] * (m[7] * m[10] - m[6] * m[11]) +
                        m[2] * (m[5] * m[11] - m[7] * m[9]) +
                        m[3] * (m[6] * m[9] - m[5] * m[10]));
    
    inv.m[4] = invDet * (m[4] * (m[11] * m[14] - m[10] * m[15]) +
                        m[6] * (m[8] * m[15] - m[11] * m[12]) +
                        m[7] * (m[10] * m[12] - m[8] * m[14]));
    
    inv.m[5] = invDet * (m[0] * (m[10] * m[15] - m[11] * m[14]) +
                        m[2] * (m[11] * m[12] - m[8] * m[15]) +
                        m[3] * (m[8] * m[14] - m[10] * m[12]));
    
    inv.m[6] = invDet * (m[0] * (m[7] * m[14] - m[6] * m[15]) +
                        m[2] * (m[4] * m[15] - m[7] * m[12]) +
                        m[3] * (m[6] * m[12] - m[4] * m[14]));
    
    inv.m[7] = invDet * (m[0] * (m[6] * m[11] - m[7] * m[10]) +
                        m[2] * (m[7] * m[8] - m[4] * m[11]) +
                        m[3] * (m[4] * m[10] - m[6] * m[8]));
    
    inv.m[8] = invDet * (m[4] * (m[9] * m[15] - m[11] * m[13]) +
                        m[5] * (m[11] * m[12] - m[8] * m[15]) +
                        m[7] * (m[8] * m[13] - m[9] * m[12]));
    
    inv.m[9] = invDet * (m[0] * (m[11] * m[13] - m[9] * m[15]) +
                        m[1] * (m[8] * m[15] - m[11] * m[12]) +
                        m[3] * (m[9] * m[12] - m[8] * m[13]));
    
    inv.m[10] = invDet * (m[0] * (m[5] * m[15] - m[7] * m[13]) +
                         m[1] * (m[7] * m[12] - m[4] * m[15]) +
                         m[3] * (m[4] * m[13] - m[5] * m[12]));
    
    inv.m[11] = invDet * (m[0] * (m[7] * m[9] - m[5] * m[11]) +
                         m[1] * (m[4] * m[11] - m[7] * m[8]) +
                         m[3] * (m[5] * m[8] - m[4] * m[9]));
    
    inv.m[12] = invDet * (m[4] * (m[10] * m[13] - m[9] * m[14]) +
                         m[5] * (m[8] * m[14] - m[10] * m[12]) +
                         m[6] * (m[9] * m[12] - m[8] * m[13]));
    
    inv.m[13] = invDet * (m[0] * (m[9] * m[14] - m[10] * m[13]) +
                         m[1] * (m[10] * m[12] - m[8] * m[14]) +
                         m[2] * (m[8] * m[13] - m[9] * m[12]));
    
    inv.m[14] = invDet * (m[0] * (m[6] * m[13] - m[5] * m[14]) +
                         m[1] * (m[4] * m[14] - m[6] * m[12]) +
                         m[2] * (m[5] * m[12] - m[4] * m[13]));
    
    inv.m[15] = invDet * (m[0] * (m[5] * m[10] - m[6] * m[9]) +
                         m[1] * (m[6] * m[8] - m[4] * m[10]) +
                         m[2] * (m[4] * m[9] - m[5] * m[8]));
    
    return inv;
}

Matrix4 Matrix4::Transpose() const {
    Matrix4 result;
    
    result.m[0] = m[0]; result.m[4] = m[1]; result.m[8] = m[2]; result.m[12] = m[3];
    result.m[1] = m[4]; result.m[5] = m[5]; result.m[9] = m[6]; result.m[13] = m[7];
    result.m[2] = m[8]; result.m[6] = m[9]; result.m[10] = m[10]; result.m[14] = m[11];
    result.m[3] = m[12]; result.m[7] = m[13]; result.m[11] = m[14]; result.m[15] = m[15];
    
    return result;
}

float Matrix4::Determinant() const {
    return m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) +
                  m[6] * (m[11] * m[13] - m[9] * m[15]) +
                  m[7] * (m[9] * m[14] - m[10] * m[13])) -
           m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) +
                  m[6] * (m[11] * m[12] - m[8] * m[15]) +
                  m[7] * (m[8] * m[14] - m[10] * m[12])) +
           m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) +
                  m[5] * (m[11] * m[12] - m[8] * m[15]) +
                  m[7] * (m[8] * m[13] - m[9] * m[12])) -
           m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) +
                  m[5] * (m[10] * m[12] - m[8] * m[14]) +
                  m[6] * (m[8] * m[13] - m[9] * m[12]));
}

bool Matrix4::IsIdentity() const {
    for (int i = 0; i < 16; ++i) {
        if (i % 5 == 0) { // Diagonal elements
            if (std::fabs(m[i] - 1.0f) > EPSILON_F) return false;
        } else {
            if (std::fabs(m[i]) > EPSILON_F) return false;
        }
    }
    return true;
}

Matrix4 Matrix4::Translation(const Vector3& translation) {
    Matrix4 mat = Identity();
    mat.m[12] = translation.x;
    mat.m[13] = translation.y;
    mat.m[14] = translation.z;
    return mat;
}

Matrix4 Matrix4::Rotation(const Vector3& axis, float angle) {
    Vector3 a = axis.Normalized();
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;
    
    Matrix4 mat;
    
    mat.m[0] = t * a.x * a.x + c;
    mat.m[1] = t * a.x * a.y - s * a.z;
    mat.m[2] = t * a.x * a.z + s * a.y;
    mat.m[3] = 0.0f;
    
    mat.m[4] = t * a.x * a.y + s * a.z;
    mat.m[5] = t * a.y * a.y + c;
    mat.m[6] = t * a.y * a.z - s * a.x;
    mat.m[7] = 0.0f;
    
    mat.m[8] = t * a.x * a.z - s * a.y;
    mat.m[9] = t * a.y * a.z + s * a.x;
    mat.m[10] = t * a.z * a.z + c;
    mat.m[11] = 0.0f;
    
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    
    return mat;
}

Matrix4 Matrix4::RotationEuler(float pitch, float yaw, float roll) {
    float cp = std::cos(pitch * DEG_TO_RAD);
    float sp = std::sin(pitch * DEG_TO_RAD);
    float cy = std::cos(yaw * DEG_TO_RAD);
    float sy = std::sin(yaw * DEG_TO_RAD);
    float cr = std::cos(roll * DEG_TO_RAD);
    float sr = std::sin(roll * DEG_TO_RAD);
    
    Matrix4 mat;
    
    mat.m[0] = cy * cr + sy * sp * sr;
    mat.m[1] = sr * cp;
    mat.m[2] = -sy * cr + cy * sp * sr;
    mat.m[3] = 0.0f;
    
    mat.m[4] = -cy * sr + sy * sp * cr;
    mat.m[5] = cr * cp;
    mat.m[6] = sr * sy + cy * sp * cr;
    mat.m[7] = 0.0f;
    
    mat.m[8] = sy * cp;
    mat.m[9] = -sp;
    mat.m[10] = cy * cp;
    mat.m[11] = 0.0f;
    
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    
    return mat;
}

Matrix4 Matrix4::Scaling(const Vector3& scale) {
    Matrix4 mat = Identity();
    mat.m[0] = scale.x;
    mat.m[5] = scale.y;
    mat.m[10] = scale.z;
    return mat;
}

Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    Vector3 z = (eye - target).Normalized();
    Vector3 x = up.Cross(z).Normalized();
    Vector3 y = z.Cross(x);
    
    Matrix4 mat;
    
    mat.m[0] = x.x;
    mat.m[1] = y.x;
    mat.m[2] = z.x;
    mat.m[3] = 0.0f;
    
    mat.m[4] = x.y;
    mat.m[5] = y.y;
    mat.m[6] = z.y;
    mat.m[7] = 0.0f;
    
    mat.m[8] = x.z;
    mat.m[9] = y.z;
    mat.m[10] = z.z;
    mat.m[11] = 0.0f;
    
    mat.m[12] = -x.Dot(eye);
    mat.m[13] = -y.Dot(eye);
    mat.m[14] = -z.Dot(eye);
    mat.m[15] = 1.0f;
    
    return mat;
}

Matrix4 Matrix4::Perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / std::tan(fov * DEG_TO_RAD * 0.5f);
    float rangeInv = 1.0f / (near - far);
    
    Matrix4 mat;
    
    mat.m[0] = f / aspect;
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    
    mat.m[4] = 0.0f;
    mat.m[5] = f;
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    
    mat.m[8] = 0.0f;
    mat.m[9] = 0.0f;
    mat.m[10] = (far + near) * rangeInv;
    mat.m[11] = -1.0f;
    
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 2.0f * far * near * rangeInv;
    mat.m[15] = 0.0f;
    
    return mat;
}

Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 mat = Identity();
    
    mat.m[0] = 2.0f / (right - left);
    mat.m[5] = 2.0f / (top - bottom);
    mat.m[10] = -2.0f / (far - near);
    
    mat.m[12] = -(right + left) / (right - left);
    mat.m[13] = -(top + bottom) / (top - bottom);
    mat.m[14] = -(far + near) / (far - near);
    
    return mat;
}

Vector3 Matrix4::GetTranslation() const {
    return Vector3(m[12], m[13], m[14]);
}

Matrix4 Matrix4::GetRotation() const {
    Vector3 scale = GetScale();
    
    Matrix4 rot;
    
    rot.m[0] = m[0] / scale.x;
    rot.m[1] = m[1] / scale.x;
    rot.m[2] = m[2] / scale.x;
    rot.m[3] = 0.0f;
    
    rot.m[4] = m[4] / scale.y;
    rot.m[5] = m[5] / scale.y;
    rot.m[6] = m[6] / scale.y;
    rot.m[7] = 0.0f;
    
    rot.m[8] = m[8] / scale.z;
    rot.m[9] = m[9] / scale.z;
    rot.m[10] = m[10] / scale.z;
    rot.m[11] = 0.0f;
    
    rot.m[12] = 0.0f;
    rot.m[13] = 0.0f;
    rot.m[14] = 0.0f;
    rot.m[15] = 1.0f;
    
    return rot;
}

Vector3 Matrix4::GetScale() const {
    return Vector3(
        Vector3(m[0], m[1], m[2]).Length(),
        Vector3(m[4], m[5], m[6]).Length(),
        Vector3(m[8], m[9], m[10]).Length()
    );
}

void Matrix4::Decompose(Vector3& translation, Matrix4& rotation, Vector3& scale) const {
    translation = GetTranslation();
    scale = GetScale();
    rotation = GetRotation();
}

Matrix4 Matrix4::Identity() {
    return Matrix4();
}

Matrix4 Matrix4::Zero() {
    Matrix4 mat;
    std::memset(mat.m, 0, sizeof(mat.m));
    return mat;
}

float Matrix4::At(int row, int col) const {
    return m[col * 4 + row];
}

void Matrix4::Set(int row, int col, float value) {
    m[col * 4 + row] = value;
}

float& Matrix4::operator[](int index) {
    return m[index];
}

const float& Matrix4::operator[](int index) const {
    return m[index];
}

std::string Matrix4::ToString() const {
    std::ostringstream ss;
    ss << "Matrix4(\n";
    for (int row = 0; row < 4; ++row) {
        ss << "  ";
        for (int col = 0; col < 4; ++col) {
            ss << At(row, col);
            if (col < 3) ss << ", ";
        }
        if (row < 3) ss << "\n";
    }
    ss << "\n)";
    return ss.str();
}

} // namespace Math
} // namespace DSRT
