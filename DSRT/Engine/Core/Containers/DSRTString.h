#pragma once

/**
 * @file DSRTString.h
 * @brief String implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "DSRTMemory.h"
#include <cstring>

namespace DSRT {
    
    /**
     * @brief String class
     */
    class String {
    public:
        using SizeType = size_t;
        using CharType = char;
        using Iterator = CharType*;
        using ConstIterator = const CharType*;
        
        static constexpr SizeType SMALL_STRING_SIZE = 16;
        
        /**
         * @brief Constructs an empty string
         */
        String() : m_length(0) {
            m_data.small[0] = '\0';
            m_flags = FLAG_SMALL;
        }
        
        /**
         * @brief Constructs a string from C string
         * @param str C string
         */
        String(const CharType* str) : m_length(0) {
            if (str) {
                m_length = StringLength(str);
                InitFromCString(str, m_length);
            } else {
                m_data.small[0] = '\0';
                m_flags = FLAG_SMALL;
            }
        }
        
        /**
         * @brief Constructs a string from C string with length
         * @param str C string
         * @param length String length
         */
        String(const CharType* str, SizeType length) : m_length(length) {
            InitFromCString(str, length);
        }
        
        /**
         * @brief Constructs a string with repeated character
         * @param count Number of characters
         * @param ch Character to repeat
         */
        String(SizeType count, CharType ch) : m_length(count) {
            if (count <= SMALL_STRING_SIZE) {
                Memory::Set(m_data.small, ch, count);
                m_data.small[count] = '\0';
                m_flags = FLAG_SMALL;
            } else {
                AllocateLarge(count);
                Memory::Set(m_data.large.data, ch, count);
                m_data.large.data[count] = '\0';
            }
        }
        
        /**
         * @brief Copy constructor
         * @param other String to copy
         */
        String(const String& other) : m_length(other.m_length) {
            if (other.IsSmall()) {
                Memory::Copy(m_data.small, other.m_data.small, m_length + 1);
                m_flags = FLAG_SMALL;
            } else {
                AllocateLarge(m_length);
                Memory::Copy(m_data.large.data, other.m_data.large.data, m_length + 1);
                m_data.large.capacity = other.m_data.large.capacity;
            }
        }
        
        /**
         * @brief Move constructor
         * @param other String to move
         */
        String(String&& other) noexcept : m_length(other.m_length) {
            if (other.IsSmall()) {
                Memory::Copy(m_data.small, other.m_data.small, m_length + 1);
                m_flags = FLAG_SMALL;
            } else {
                m_data.large = other.m_data.large;
                m_flags = other.m_flags;
                
                // Reset other to small empty string
                other.m_data.small[0] = '\0';
                other.m_length = 0;
                other.m_flags = FLAG_SMALL;
            }
        }
        
        /**
         * @brief Destructor
         */
        ~String() {
            if (!IsSmall()) {
                Memory::Free(m_data.large.data);
            }
        }
        
        /**
         * @brief Copy assignment operator
         * @param other String to copy
         * @return Reference to this string
         */
        String& operator=(const String& other) {
            if (this != &other) {
                Clear();
                m_length = other.m_length;
                
                if (other.IsSmall()) {
                    if (!IsSmall()) {
                        Memory::Free(m_data.large.data);
                    }
                    Memory::Copy(m_data.small, other.m_data.small, m_length + 1);
                    m_flags = FLAG_SMALL;
                } else {
                    if (IsSmall() || m_data.large.capacity < other.m_length) {
                        if (!IsSmall()) {
                            Memory::Free(m_data.large.data);
                        }
                        AllocateLarge(m_length);
                    }
                    Memory::Copy(m_data.large.data, other.m_data.large.data, m_length + 1);
                    m_data.large.capacity = other.m_data.large.capacity;
                }
            }
            return *this;
        }
        
        /**
         * @brief Move assignment operator
         * @param other String to move
         * @return Reference to this string
         */
        String& operator=(String&& other) noexcept {
            if (this != &other) {
                if (!IsSmall()) {
                    Memory::Free(m_data.large.data);
                }
                
                m_length = other.m_length;
                
                if (other.IsSmall()) {
                    Memory::Copy(m_data.small, other.m_data.small, m_length + 1);
                    m_flags = FLAG_SMALL;
                } else {
                    m_data.large = other.m_data.large;
                    m_flags = other.m_flags;
                }
                
                // Reset other
                other.m_data.small[0] = '\0';
                other.m_length = 0;
                other.m_flags = FLAG_SMALL;
            }
            return *this;
        }
        
        /**
         * @brief Assignment from C string
         * @param str C string
         * @return Reference to this string
         */
        String& operator=(const CharType* str) {
            if (str) {
                SizeType length = StringLength(str);
                Assign(str, length);
            } else {
                Clear();
            }
            return *this;
        }
        
        // Element access
        
        /**
         * @brief Array subscript operator
         * @param index Character index
         * @return Reference to character
         */
        CharType& operator[](SizeType index) {
            DSRT_ASSERT(index < m_length);
            return Data()[index];
        }
        
        /**
         * @brief Array subscript operator (const)
         * @param index Character index
         * @return Const reference to character
         */
        const CharType& operator[](SizeType index) const {
            DSRT_ASSERT(index < m_length);
            return Data()[index];
        }
        
        /**
         * @brief Gets character at index with bounds checking
         * @param index Character index
         * @return Reference to character
         */
        CharType& At(SizeType index) {
            DSRT_ASSERT(index < m_length);
            return Data()[index];
        }
        
        /**
         * @brief Gets character at index with bounds checking (const)
         * @param index Character index
         * @return Const reference to character
         */
        const CharType& At(SizeType index) const {
            DSRT_ASSERT(index < m_length);
            return Data()[index];
        }
        
        /**
         * @brief Gets first character
         * @return Reference to first character
         */
        CharType& Front() {
            DSRT_ASSERT(m_length > 0);
            return Data()[0];
        }
        
        /**
         * @brief Gets first character (const)
         * @return Const reference to first character
         */
        const CharType& Front() const {
            DSRT_ASSERT(m_length > 0);
            return Data()[0];
        }
        
        /**
         * @brief Gets last character
         * @return Reference to last character
         */
        CharType& Back() {
            DSRT_ASSERT(m_length > 0);
            return Data()[m_length - 1];
        }
        
        /**
         * @brief Gets last character (const)
         * @return Const reference to last character
         */
        const CharType& Back() const {
            DSRT_ASSERT(m_length > 0);
            return Data()[m_length - 1];
        }
        
        /**
         * @brief Gets C string pointer
         * @return Pointer to null-terminated string
         */
        const CharType* CStr() const {
            return Data();
        }
        
        /**
         * @brief Gets data pointer
         * @return Pointer to string data
         */
        CharType* Data() {
            return IsSmall() ? m_data.small : m_data.large.data;
        }
        
        /**
         * @brief Gets data pointer (const)
         * @return Const pointer to string data
         */
        const CharType* Data() const {
            return IsSmall() ? m_data.small : m_data.large.data;
        }
        
        // Capacity
        
        /**
         * @brief Gets string length
         * @return Number of characters
         */
        SizeType Length() const { return m_length; }
        
        /**
         * @brief Gets string size (same as length)
         * @return Number of characters
         */
        SizeType Size() const { return m_length; }
        
        /**
         * @brief Gets allocated capacity
         * @return Capacity in characters
         */
        SizeType Capacity() const {
            if (IsSmall()) {
                return SMALL_STRING_SIZE;
            }
            return m_data.large.capacity;
        }
        
        /**
         * @brief Checks if string is empty
         * @return True if empty
         */
        bool IsEmpty() const { return m_length == 0; }
        
        /**
         * @brief Clears the string
         */
        void Clear() {
            if (!IsSmall()) {
                Memory::Free(m_data.large.data);
            }
            m_data.small[0] = '\0';
            m_length = 0;
            m_flags = FLAG_SMALL;
        }
        
        /**
         * @brief Reserves capacity
         * @param newCapacity New capacity
         */
        void Reserve(SizeType newCapacity) {
            if (newCapacity <= Capacity()) return;
            
            if (IsSmall()) {
                // Move from small to large
                LargeData newData;
                newData.capacity = CalculateGrowth(newCapacity);
                newData.data = static_cast<CharType*>(
                    Memory::Allocate(newData.capacity + 1));
                
                Memory::Copy(newData.data, m_data.small, m_length + 1);
                m_data.large = newData;
                m_flags = 0;
            } else {
                // Reallocate large buffer
                SizeType newCap = CalculateGrowth(newCapacity);
                CharType* newData = static_cast<CharType*>(
                    Memory::Allocate(newCap + 1));
                
                Memory::Copy(newData, m_data.large.data, m_length + 1);
                Memory::Free(m_data.large.data);
                
                m_data.large.data = newData;
                m_data.large.capacity = newCap;
            }
        }
        
        /**
         * @brief Shrinks capacity to fit
         */
        void ShrinkToFit() {
            if (IsSmall() || m_length == m_data.large.capacity) return;
            
            if (m_length <= SMALL_STRING_SIZE) {
                // Move from large to small
                LargeData oldData = m_data.large;
                Memory::Copy(m_data.small, oldData.data, m_length + 1);
                Memory::Free(oldData.data);
                m_flags = FLAG_SMALL;
            } else {
                // Reallocate to exact size
                CharType* newData = static_cast<CharType*>(
                    Memory::Allocate(m_length + 1));
                
                Memory::Copy(newData, m_data.large.data, m_length + 1);
                Memory::Free(m_data.large.data);
                
                m_data.large.data = newData;
                m_data.large.capacity = m_length;
            }
        }
        
        // Modifiers
        
        /**
         * @brief Appends string
         * @param str String to append
         * @return Reference to this string
         */
        String& Append(const String& str) {
            return Append(str.Data(), str.m_length);
        }
        
        /**
         * @brief Appends C string
         * @param str C string to append
         * @return Reference to this string
         */
        String& Append(const CharType* str) {
            if (str) {
                SizeType length = StringLength(str);
                return Append(str, length);
            }
            return *this;
        }
        
        /**
         * @brief Appends substring
         * @param str C string to append
         * @param count Number of characters to append
         * @return Reference to this string
         */
        String& Append(const CharType* str, SizeType count) {
            if (count == 0) return *this;
            
            SizeType newLength = m_length + count;
            EnsureCapacity(newLength);
            
            CharType* data = Data();
            Memory::Copy(data + m_length, str, count);
            data[newLength] = '\0';
            m_length = newLength;
            
            return *this;
        }
        
        /**
         * @brief Appends character
         * @param ch Character to append
         * @return Reference to this string
         */
        String& Append(CharType ch) {
            SizeType newLength = m_length + 1;
            EnsureCapacity(newLength);
            
            CharType* data = Data();
            data[m_length] = ch;
            data[newLength] = '\0';
            m_length = newLength;
            
            return *this;
        }
        
        /**
         * @brief Appends repeated character
         * @param count Number of characters
         * @param ch Character to repeat
         * @return Reference to this string
         */
        String& Append(SizeType count, CharType ch) {
            if (count == 0) return *this;
            
            SizeType newLength = m_length + count;
            EnsureCapacity(newLength);
            
            CharType* data = Data();
            Memory::Set(data + m_length, ch, count);
            data[newLength] = '\0';
            m_length = newLength;
            
            return *this;
        }
        
        /**
         * @brief += operator for string
         * @param str String to append
         * @return Reference to this string
         */
        String& operator+=(const String& str) {
            return Append(str);
        }
        
        /**
         * @brief += operator for C string
         * @param str C string to append
         * @return Reference to this string
         */
        String& operator+=(const CharType* str) {
            return Append(str);
        }
        
        /**
         * @brief += operator for character
         * @param ch Character to append
         * @return Reference to this string
         */
        String& operator+=(CharType ch) {
            return Append(ch);
        }
        
        /**
         * @brief Inserts string at position
         * @param pos Insert position
         * @param str String to insert
         * @return Reference to this string
         */
        String& Insert(SizeType pos, const String& str) {
            return Insert(pos, str.Data(), str.m_length);
        }
        
        /**
         * @brief Inserts C string at position
         * @param pos Insert position
         * @param str C string to insert
         * @return Reference to this string
         */
        String& Insert(SizeType pos, const CharType* str) {
            if (str) {
                SizeType length = StringLength(str);
                return Insert(pos, str, length);
            }
            return *this;
        }
        
        /**
         * @brief Inserts substring at position
         * @param pos Insert position
         * @param str C string to insert
         * @param count Number of characters to insert
         * @return Reference to this string
         */
        String& Insert(SizeType pos, const CharType* str, SizeType count) {
            DSRT_ASSERT(pos <= m_length);
            
            if (count == 0) return *this;
            
            SizeType newLength = m_length + count;
            EnsureCapacity(newLength);
            
            CharType* data = Data();
            
            // Move tail to make space
            if (pos < m_length) {
                Memory::Move(data + pos + count, data + pos, m_length - pos);
            }
            
            // Insert new characters
            Memory::Copy(data + pos, str, count);
            data[newLength] = '\0';
            m_length = newLength;
            
            return *this;
        }
        
        /**
         * @brief Erases characters
         * @param pos Erase position
         * @param count Number of characters to erase
         * @return Reference to this string
         */
        String& Erase(SizeType pos, SizeType count = 1) {
            DSRT_ASSERT(pos < m_length);
            
            if (count == 0) return *this;
            
            SizeType eraseCount = (count == npos || pos + count > m_length) ? 
                                 m_length - pos : count;
            
            CharType* data = Data();
            
            // Move tail to fill gap
            if (pos + eraseCount < m_length) {
                Memory::Move(data + pos, data + pos + eraseCount, 
                           m_length - pos - eraseCount);
            }
            
            m_length -= eraseCount;
            data[m_length] = '\0';
            
            return *this;
        }
        
        /**
         * @brief Replaces substring
         * @param pos Replace position
         * @param count Number of characters to replace
         * @param str Replacement string
         * @return Reference to this string
         */
        String& Replace(SizeType pos, SizeType count, const String& str) {
            return Replace(pos, count, str.Data(), str.m_length);
        }
        
        /**
         * @brief Replaces substring with C string
         * @param pos Replace position
         * @param count Number of characters to replace
         * @param str Replacement C string
         * @return Reference to this string
         */
        String& Replace(SizeType pos, SizeType count, const CharType* str) {
            if (str) {
                SizeType length = StringLength(str);
                return Replace(pos, count, str, length);
            }
            return *this;
        }
        
        // Operations
        
        /**
         * @brief Gets substring
         * @param pos Start position
         * @param count Number of characters
         * @return Substring
         */
        String Substr(SizeType pos, SizeType count = npos) const {
            DSRT_ASSERT(pos <= m_length);
            
            if (count == npos || pos + count > m_length) {
                count = m_length - pos;
            }
            
            if (count == 0) return String();
            
            return String(Data() + pos, count);
        }
        
        /**
         * @brief Compares with another string
         * @param str String to compare
         * @return Comparison result
         */
        int Compare(const String& str) const {
            return Compare(str.Data(), str.m_length);
        }
        
        /**
         * @brief Compares with C string
         * @param str C string to compare
         * @return Comparison result
         */
        int Compare(const CharType* str) const {
            if (!str) return 1; // This string is greater
            
            SizeType otherLength = StringLength(str);
            return Compare(str, otherLength);
        }
        
        /**
         * @brief Finds first occurrence of character
         * @param ch Character to find
         * @param pos Start position
         * @return Position of character, or npos if not found
         */
        SizeType Find(CharType ch, SizeType pos = 0) const {
            if (pos >= m_length) return npos;
            
            const CharType* data = Data();
            for (SizeType i = pos; i < m_length; ++i) {
                if (data[i] == ch) {
                    return i;
                }
            }
            
            return npos;
        }
        
        /**
         * @brief Finds first occurrence of substring
         * @param str Substring to find
         * @param pos Start position
         * @return Position of substring, or npos if not found
         */
        SizeType Find(const String& str, SizeType pos = 0) const {
            return Find(str.Data(), pos, str.m_length);
        }
        
        /**
         * @brief Finds first occurrence of C string
         * @param str C string to find
         * @param pos Start position
         * @return Position of string, or npos if not found
         */
        SizeType Find(const CharType* str, SizeType pos = 0) const {
            if (!str) return npos;
            
            SizeType length = StringLength(str);
            return Find(str, pos, length);
        }
        
        /**
         * @brief Finds last occurrence of character
         * @param ch Character to find
         * @param pos Start position (search backwards)
         * @return Position of character, or npos if not found
         */
        SizeType RFind(CharType ch, SizeType pos = npos) const {
            if (m_length == 0) return npos;
            
            if (pos >= m_length) {
                pos = m_length - 1;
            }
            
            const CharType* data = Data();
            for (SizeType i = pos; i != npos; --i) {
                if (data[i] == ch) {
                    return i;
                }
            }
            
            return npos;
        }
        
        /**
         * @brief Finds first occurrence of any character in set
         * @param str Character set
         * @param pos Start position
         * @return Position of character, or npos if not found
         */
        SizeType FindFirstOf(const String& str, SizeType pos = 0) const {
            return FindFirstOf(str.Data(), pos, str.m_length);
        }
        
        /**
         * @brief Finds first occurrence of any character in C string
         * @param str Character set
         * @param pos Start position
         * @return Position of character, or npos if not found
         */
        SizeType FindFirstOf(const CharType* str, SizeType pos = 0) const {
            if (!str) return npos;
            
            SizeType length = StringLength(str);
            return FindFirstOf(str, pos, length);
        }
        
        // Static methods
        
        /**
         * @brief Empty string
         * @return Empty string
         */
        static String Empty() {
            return String();
        }
        
        // Special value
        static const SizeType npos = static_cast<SizeType>(-1);
        
    private:
        struct LargeData {
            CharType* data;
            SizeType capacity;
        };
        
        union DataUnion {
            CharType small[SMALL_STRING_SIZE];
            LargeData large;
        };
        
        DataUnion m_data;
        SizeType m_length;
        uint8 m_flags;
        
        static constexpr uint8 FLAG_SMALL = 0x80;
        
        bool IsSmall() const { return (m_flags & FLAG_SMALL) != 0; }
        
        void InitFromCString(const CharType* str, SizeType length) {
            if (length <= SMALL_STRING_SIZE) {
                Memory::Copy(m_data.small, str, length);
                m_data.small[length] = '\0';
                m_flags = FLAG_SMALL;
            } else {
                AllocateLarge(length);
                Memory::Copy(m_data.large.data, str, length);
                m_data.large.data[length] = '\0';
            }
        }
        
        void AllocateLarge(SizeType length) {
            SizeType capacity = CalculateGrowth(length);
            m_data.large.data = static_cast<CharType*>(
                Memory::Allocate(capacity + 1));
            m_data.large.capacity = capacity;
            m_flags = 0;
        }
        
        SizeType CalculateGrowth(SizeType newSize) const {
            SizeType geometric = Capacity() + Capacity() / 2;
            if (geometric < newSize) {
                return newSize;
            }
            return geometric;
        }
        
        void EnsureCapacity(SizeType newLength) {
            if (newLength <= Capacity()) return;
            
            if (IsSmall()) {
                // Move from small to large
                LargeData newData;
                newData.capacity = CalculateGrowth(newLength);
                newData.data = static_cast<CharType*>(
                    Memory::Allocate(newData.capacity + 1));
                
                Memory::Copy(newData.data, m_data.small, m_length + 1);
                m_data.large = newData;
                m_flags = 0;
            } else {
                // Reallocate large buffer
                SizeType newCap = CalculateGrowth(newLength);
                CharType* newData = static_cast<CharType*>(
                    Memory::Allocate(newCap + 1));
                
                Memory::Copy(newData, m_data.large.data, m_length + 1);
                Memory::Free(m_data.large.data);
                
                m_data.large.data = newData;
                m_data.large.capacity = newCap;
            }
        }
        
        void Assign(const CharType* str, SizeType length) {
            if (length <= Capacity()) {
                CharType* data = Data();
                Memory::Copy(data, str, length);
                data[length] = '\0';
                m_length = length;
                
                if (!IsSmall() && length <= SMALL_STRING_SIZE) {
                    // Switch to small string
                    LargeData oldData = m_data.large;
                    Memory::Copy(m_data.small, oldData.data, length + 1);
                    Memory::Free(oldData.data);
                    m_flags = FLAG_SMALL;
                }
            } else {
                Clear();
                Reserve(length);
                Memory::Copy(Data(), str, length);
                Data()[length] = '\0';
                m_length = length;
            }
        }
        
        int Compare(const CharType* str, SizeType length) const {
            const CharType* data = Data();
            SizeType minLength = m_length < length ? m_length : length;
            
            int result = Memory::Compare(data, str, minLength);
            if (result != 0) return result;
            
            if (m_length < length) return -1;
            if (m_length > length) return 1;
            return 0;
        }
        
        SizeType Find(const CharType* str, SizeType pos, SizeType length) const {
            if (length == 0) return pos <= m_length ? pos : npos;
            if (pos + length > m_length) return npos;
            
            const CharType* data = Data();
            for (SizeType i = pos; i <= m_length - length; ++i) {
                if (Memory::Compare(data + i, str, length) == 0) {
                    return i;
                }
            }
            
            return npos;
        }
        
        SizeType FindFirstOf(const CharType* str, SizeType pos, SizeType length) const {
            if (pos >= m_length || length == 0) return npos;
            
            const CharType* data = Data();
            for (SizeType i = pos; i < m_length; ++i) {
                for (SizeType j = 0; j < length; ++j) {
                    if (data[i] == str[j]) {
                        return i;
                    }
                }
            }
            
            return npos;
        }
        
        String& Replace(SizeType pos, SizeType count, const CharType* str, SizeType length) {
            DSRT_ASSERT(pos <= m_length);
            
            SizeType oldCount = (count == npos || pos + count > m_length) ? 
                               m_length - pos : count;
            
            SizeType newLength = m_length - oldCount + length;
            
            if (length > oldCount) {
                EnsureCapacity(newLength);
            }
            
            CharType* data = Data();
            
            // Move tail if needed
            if (length != oldCount) {
                Memory::Move(data + pos + length, data + pos + oldCount, 
                           m_length - pos - oldCount);
            }
            
            // Copy replacement
            Memory::Copy(data + pos, str, length);
            data[newLength] = '\0';
            m_length = newLength;
            
            return *this;
        }
        
        static SizeType StringLength(const CharType* str) {
            if (!str) return 0;
            
            const CharType* end = str;
            while (*end) ++end;
            return static_cast<SizeType>(end - str);
        }
    };
    
    // Non-member functions
    
    /**
     * @brief Concatenates two strings
     * @param lhs Left string
     * @param rhs Right string
     * @return Concatenated string
     */
    inline String operator+(const String& lhs, const String& rhs) {
        String result(lhs);
        result.Append(rhs);
        return result;
    }
    
    /**
     * @brief Concatenates string and C string
     * @param lhs String
     * @param rhs C string
     * @return Concatenated string
     */
    inline String operator+(const String& lhs, const char* rhs) {
        String result(lhs);
        result.Append(rhs);
        return result;
    }
    
    /**
     * @brief Concatenates C string and string
     * @param lhs C string
     * @param rhs String
     * @return Concatenated string
     */
    inline String operator+(const char* lhs, const String& rhs) {
        String result(lhs);
        result.Append(rhs);
        return result;
    }
    
    /**
     * @brief Concatenates string and character
     * @param lhs String
     * @param rhs Character
     * @return Concatenated string
     */
    inline String operator+(const String& lhs, char rhs) {
        String result(lhs);
        result.Append(rhs);
        return result;
    }
    
    /**
     * @brief Concatenates character and string
     * @param lhs Character
     * @param rhs String
     * @return Concatenated string
     */
    inline String operator+(char lhs, const String& rhs) {
        String result(1, lhs);
        result.Append(rhs);
        return result;
    }
    
    // Comparison operators
    
    inline bool operator==(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) == 0;
    }
    
    inline bool operator!=(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) != 0;
    }
    
    inline bool operator<(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) < 0;
    }
    
    inline bool operator<=(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) <= 0;
    }
    
    inline bool operator>(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) > 0;
    }
    
    inline bool operator>=(const String& lhs, const String& rhs) {
        return lhs.Compare(rhs) >= 0;
    }
    
    inline bool operator==(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) == 0;
    }
    
    inline bool operator!=(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) != 0;
    }
    
    inline bool operator<(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) < 0;
    }
    
    inline bool operator<=(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) <= 0;
    }
    
    inline bool operator>(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) > 0;
    }
    
    inline bool operator>=(const String& lhs, const char* rhs) {
        return lhs.Compare(rhs) >= 0;
    }
    
    inline bool operator==(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) == 0;
    }
    
    inline bool operator!=(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) != 0;
    }
    
    inline bool operator<(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) > 0;
    }
    
    inline bool operator<=(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) >= 0;
    }
    
    inline bool operator>(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) < 0;
    }
    
    inline bool operator>=(const char* lhs, const String& rhs) {
        return rhs.Compare(lhs) <= 0;
    }
    
} // namespace DSRT
