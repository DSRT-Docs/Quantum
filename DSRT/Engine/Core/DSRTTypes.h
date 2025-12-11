#pragma once

/**
 * @file DSRTTypes.h
 * @brief Engine type definitions and aliases
 */

#include <cstdint>
#include <cstddef>

namespace DSRT {
    
    // Integer types
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;
    
    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;
    
    // Character types
    using char8 = char;
    using char16 = char16_t;
    using char32 = char32_t;
    
    // Floating point types
    using float32 = float;
    using float64 = double;
    
    // Size types
    using size_t = std::size_t;
    using ptrdiff_t = std::ptrdiff_t;
    
    // Boolean
    using bool8 = bool;
    
    // Handle types
    using Handle = uint64;
    
    // Invalid handle value
    constexpr Handle INVALID_HANDLE = 0xFFFFFFFFFFFFFFFF;
    
    // Result codes
    enum class Result : uint32 {
        Success = 0,
        Failure = 1,
        InvalidParameter = 2,
        OutOfMemory = 3,
        NotInitialized = 4,
        AlreadyInitialized = 5,
        NotSupported = 6,
        Timeout = 7,
        Busy = 8,
        NotFound = 9,
        AlreadyExists = 10,
        PermissionDenied = 11,
        CorruptedData = 12,
        InsufficientSpace = 13,
        InvalidState = 14,
        EndOfFile = 15,
        NotImplemented = 16,
        VersionMismatch = 17,
        InvalidFormat = 18,
        DeviceLost = 19,
        UnknownError = 0xFFFFFFFF
    };
    
    // Bitfield type
    template<typename T>
    using Bitfield = T;
    
    // Flags type
    template<typename T>
    using Flags = T;
    
    // Version type
    struct Version {
        uint16 major;
        uint16 minor;
        uint16 patch;
        uint16 build;
        
        constexpr Version(uint16 major = 0, uint16 minor = 0, 
                          uint16 patch = 0, uint16 build = 0)
            : major(major), minor(minor), patch(patch), build(build) {}
        
        constexpr bool operator==(const Version& other) const {
            return major == other.major && minor == other.minor && 
                   patch == other.patch && build == other.build;
        }
        
        constexpr bool operator!=(const Version& other) const {
            return !(*this == other);
        }
        
        constexpr bool operator<(const Version& other) const {
            if (major != other.major) return major < other.major;
            if (minor != other.minor) return minor < other.minor;
            if (patch != other.patch) return patch < other.patch;
            return build < other.build;
        }
        
        constexpr bool operator<=(const Version& other) const {
            return *this < other || *this == other;
        }
        
        constexpr bool operator>(const Version& other) const {
            return !(*this <= other);
        }
        
        constexpr bool operator>=(const Version& other) const {
            return !(*this < other);
        }
    };
    
    // GUID type
    struct GUID {
        uint32 data1;
        uint16 data2;
        uint16 data3;
        uint8 data4[8];
        
        constexpr GUID() : data1(0), data2(0), data3(0), data4{0} {}
        
        constexpr bool operator==(const GUID& other) const {
            if (data1 != other.data1) return false;
            if (data2 != other.data2) return false;
            if (data3 != other.data3) return false;
            for (int i = 0; i < 8; ++i) {
                if (data4[i] != other.data4[i]) return false;
            }
            return true;
        }
        
        constexpr bool operator!=(const GUID& other) const {
            return !(*this == other);
        }
        
        constexpr bool IsNull() const {
            return data1 == 0 && data2 == 0 && data3 == 0 && 
                   data4[0] == 0 && data4[1] == 0 && data4[2] == 0 && 
                   data4[3] == 0 && data4[4] == 0 && data4[5] == 0 && 
                   data4[6] == 0 && data4[7] == 0;
        }
    };
    
    // Color types
    struct ColorRGB {
        float32 r, g, b;
        
        constexpr ColorRGB() : r(0.0f), g(0.0f), b(0.0f) {}
        constexpr ColorRGB(float32 r, float32 g, float32 b) : r(r), g(g), b(b) {}
    };
    
    struct ColorRGBA {
        float32 r, g, b, a;
        
        constexpr ColorRGBA() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
        constexpr ColorRGBA(float32 r, float32 g, float32 b, float32 a = 1.0f)
            : r(r), g(g), b(b), a(a) {}
        constexpr ColorRGBA(const ColorRGB& rgb, float32 a = 1.0f)
            : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}
    };
    
    // Rectangle
    struct Rect {
        int32 x, y;
        int32 width, height;
        
        constexpr Rect() : x(0), y(0), width(0), height(0) {}
        constexpr Rect(int32 x, int32 y, int32 width, int32 height)
            : x(x), y(y), width(width), height(height) {}
        
        constexpr bool Contains(int32 px, int32 py) const {
            return px >= x && px < x + width && 
                   py >= y && py < y + height;
        }
        
        constexpr bool Intersects(const Rect& other) const {
            return !(x + width <= other.x || 
                     other.x + other.width <= x || 
                     y + height <= other.y || 
                     other.y + other.height <= y);
        }
    };
    
    // 2D point
    struct Point2D {
        int32 x, y;
        
        constexpr Point2D() : x(0), y(0) {}
        constexpr Point2D(int32 x, int32 y) : x(x), y(y) {}
    };
    
    // 2D size
    struct Size2D {
        int32 width, height;
        
        constexpr Size2D() : width(0), height(0) {}
        constexpr Size2D(int32 width, int32 height) : width(width), height(height) {}
    };
    
    // Engine limits
    namespace Limits {
        constexpr size_t MAX_PATH_LENGTH = 4096;
        constexpr size_t MAX_NAME_LENGTH = 256;
        constexpr size_t MAX_DESCRIPTION_LENGTH = 1024;
        constexpr size_t MAX_ENTITIES_PER_SCENE = 65536;
        constexpr size_t MAX_COMPONENTS_PER_ENTITY = 64;
        constexpr size_t MAX_MATERIAL_TEXTURES = 16;
        constexpr size_t MAX_BONES_PER_SKELETON = 256;
        constexpr size_t MAX_LIGHTS_PER_SCENE = 256;
        constexpr size_t MAX_RENDER_TARGETS = 8;
        constexpr size_t MAX_VERTEX_ATTRIBUTES = 16;
        constexpr size_t MAX_SHADER_UNIFORMS = 128;
    }
    
} // namespace DSRT
