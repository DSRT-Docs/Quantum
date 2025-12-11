#pragma once

/**
 * @file DSRTHash.h
 * @brief Hash function implementations
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "DSRTString.h"

namespace DSRT {
    
    /**
     * @brief Hash function for generic types
     * @tparam T Type to hash
     */
    template<typename T>
    struct Hash {
        /**
         * @brief Computes hash value
         * @param value Value to hash
         * @return Hash value
         */
        size_t operator()(const T& value) const {
            // Default implementation for integral types
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for bool
    template<>
    struct Hash<bool> {
        size_t operator()(bool value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for char
    template<>
    struct Hash<char> {
        size_t operator()(char value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for signed char
    template<>
    struct Hash<signed char> {
        size_t operator()(signed char value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for unsigned char
    template<>
    struct Hash<unsigned char> {
        size_t operator()(unsigned char value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for short
    template<>
    struct Hash<short> {
        size_t operator()(short value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for unsigned short
    template<>
    struct Hash<unsigned short> {
        size_t operator()(unsigned short value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for int
    template<>
    struct Hash<int> {
        size_t operator()(int value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for unsigned int
    template<>
    struct Hash<unsigned int> {
        size_t operator()(unsigned int value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for long
    template<>
    struct Hash<long> {
        size_t operator()(long value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for unsigned long
    template<>
    struct Hash<unsigned long> {
        size_t operator()(unsigned long value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for long long
    template<>
    struct Hash<long long> {
        size_t operator()(long long value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for unsigned long long
    template<>
    struct Hash<unsigned long long> {
        size_t operator()(unsigned long long value) const {
            return static_cast<size_t>(value);
        }
    };
    
    // Specialization for float
    template<>
    struct Hash<float> {
        size_t operator()(float value) const {
            // Hash the bit representation
            union {
                float f;
                uint32 u;
            } converter;
            converter.f = value;
            return static_cast<size_t>(converter.u);
        }
    };
    
    // Specialization for double
    template<>
    struct Hash<double> {
        size_t operator()(double value) const {
            // Hash the bit representation
            union {
                double d;
                uint64 u;
            } converter;
            converter.d = value;
            return static_cast<size_t>(converter.u);
        }
    };
    
    // Specialization for pointers
    template<typename T>
    struct Hash<T*> {
        size_t operator()(T* ptr) const {
            return reinterpret_cast<size_t>(ptr);
        }
    };
    
    // Specialization for const pointers
    template<typename T>
    struct Hash<const T*> {
        size_t operator()(const T* ptr) const {
            return reinterpret_cast<size_t>(ptr);
        }
    };
    
    // Specialization for String
    template<>
    struct Hash<String> {
        size_t operator()(const String& str) const {
            return HashString(str.Data(), str.Length());
        }
    };
    
    // Specialization for C strings
    template<>
    struct Hash<const char*> {
        size_t operator()(const char* str) const {
            return HashString(str, StringLength(str));
        }
    };
    
    // Specialization for char arrays
    template<size_t N>
    struct Hash<char[N]> {
        size_t operator()(const char (&str)[N]) const {
            return HashString(str, N - 1); // Exclude null terminator
        }
    };
    
    /**
     * @brief Combines two hash values
     * @param seed First hash value (modified in place)
     * @param hash Second hash value
     */
    inline void HashCombine(size_t& seed, size_t hash) {
        // Boost-like hash combine
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    /**
     * @brief Computes hash for string data
     * @param data String data
     * @param length String length
     * @return Hash value
     */
    inline size_t HashString(const char* data, size_t length) {
        // FNV-1a hash algorithm
        const size_t prime = 0x100000001b3;
        const size_t offset = 0xcbf29ce484222325;
        
        size_t hash = offset;
        for (size_t i = 0; i < length; ++i) {
            hash ^= static_cast<size_t>(data[i]);
            hash *= prime;
        }
        
        return hash;
    }
    
    /**
     * @brief Computes hash for integer array
     * @param data Integer array
     * @param length Array length
     * @return Hash value
     */
    inline size_t HashIntegers(const uint32* data, size_t length) {
        // Jenkins one-at-a-time hash
        size_t hash = 0;
        for (size_t i = 0; i < length; ++i) {
            hash += data[i];
            hash += hash << 10;
            hash ^= hash >> 6;
        }
        
        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;
        
        return hash;
    }
    
    /**
     * @brief Computes hash for byte array
     * @param data Byte array
     * @param length Array length
     * @return Hash value
     */
    inline size_t HashBytes(const uint8* data, size_t length) {
        // MurmurHash3 32-bit
        const uint32 c1 = 0xcc9e2d51;
        const uint32 c2 = 0x1b873593;
        const uint32 seed = 0;
        
        uint32 h1 = seed;
        uint32 k1;
        
        const uint8* tail = data + (length & ~3);
        const uint32* blocks = reinterpret_cast<const uint32*>(tail);
        
        // Body
        for (const uint32* block = reinterpret_cast<const uint32*>(data); 
             block < blocks; ++block) {
            k1 = *block;
            
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);
            k1 *= c2;
            
            h1 ^= k1;
            h1 = (h1 << 13) | (h1 >> 19);
            h1 = h1 * 5 + 0xe6546b64;
        }
        
        // Tail
        k1 = 0;
        switch (length & 3) {
            case 3: k1 ^= tail[2] << 16;
            case 2: k1 ^= tail[1] << 8;
            case 1: k1 ^= tail[0];
                    k1 *= c1;
                    k1 = (k1 << 15) | (k1 >> 17);
                    k1 *= c2;
                    h1 ^= k1;
        }
        
        // Finalization
        h1 ^= length;
        h1 ^= h1 >> 16;
        h1 *= 0x85ebca6b;
        h1 ^= h1 >> 13;
        h1 *= 0xc2b2ae35;
        h1 ^= h1 >> 16;
        
        return static_cast<size_t>(h1);
    }
    
    /**
     * @brief Hash function object for use with standard containers
     */
    struct Hasher {
        template<typename T>
        size_t operator()(const T& value) const {
            return Hash<T>()(value);
        }
    };
    
} // namespace DSRT
