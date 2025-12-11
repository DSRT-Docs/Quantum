#include "DSRT/Math/Vector3.h"
#include <cmath>
#include <sstream>
#include <algorithm>

namespace DSRT {
namespace Math {

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
Vector3::Vector3(float s) : x(s), y(s), z(s) {}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    if (std::fabs(scalar) < EPSILON) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    float invScalar = 1.0f / scalar;
    return Vector3(x * invScalar, y * invScalar, z * invScalar);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    if (std::fabs(scalar) < EPSILON) {
        x = y = z = 0.0f;
    } else {
        float invScalar = 1.0f / scalar;
        x *= invScalar;
        y *= invScalar;
        z *= invScalar;
    }
    return *this;
}

float Vector3::Dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vector3::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSq() const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalized() const {
    float len = Length();
    if (len > EPSILON) {
        return Vector3(x / len, y / len, z / len);
    }
    return Vector3(0.0f, 0.0f, 0.0f);
}

void Vector3::Normalize() {
    float len = Length();
    if (len > EPSILON) {
        x /= len;
        y /= len;
        z /= len;
    }
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t) {
    float clampedT = std::clamp(t, 0.0f, 1.0f);
    return Vector3(
        a.x + (b.x - a.x) * clampedT,
        a.y + (b.y - a.y) * clampedT,
        a.z + (b.z - a.z) * clampedT
    );
}

Vector3 Vector3::Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
Vector3 Vector3::One() { return Vector3(1.0f, 1.0f, 1.0f); }
Vector3 Vector3::Up() { return Vector3(0.0f, 1.0f, 0.0f); }
Vector3 Vector3::Down() { return Vector3(0.0f, -1.0f, 0.0f); }
Vector3 Vector3::Left() { return Vector3(-1.0f, 0.0f, 0.0f); }
Vector3 Vector3::Right() { return Vector3(1.0f, 0.0f, 0.0f); }
Vector3 Vector3::Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
Vector3 Vector3::Back() { return Vector3(0.0f, 0.0f, -1.0f); }

std::string Vector3::ToString() const {
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

} // namespace Math
} // namespace DSRT
