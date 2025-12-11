#pragma once

/**
 * @file DSRTArray.h
 * @brief Dynamic array implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "DSRTMemory.h"
#include <initializer_list>

namespace DSRT {
    
    /**
     * @brief Dynamic array class
     * @tparam T Element type
     */
    template<typename T>
    class Array {
    public:
        using ValueType = T;
        using SizeType = size_t;
        using DifferenceType = ptrdiff_t;
        using Reference = T&;
        using ConstReference = const T&;
        using Pointer = T*;
        using ConstPointer = const T*;
        using Iterator = T*;
        using ConstIterator = const T*;
        
        /**
         * @brief Constructs an empty array
         */
        Array() : m_data(nullptr), m_size(0), m_capacity(0) {}
        
        /**
         * @brief Constructs an array with initial capacity
         * @param capacity Initial capacity
         */
        explicit Array(SizeType capacity) : m_data(nullptr), m_size(0), m_capacity(0) {
            Reserve(capacity);
        }
        
        /**
         * @brief Constructs an array with initial elements
         * @param count Number of elements
         * @param value Initial value
         */
        Array(SizeType count, const T& value) : m_data(nullptr), m_size(0), m_capacity(0) {
            Resize(count, value);
        }
        
        /**
         * @brief Constructs an array from initializer list
         * @param list Initializer list
         */
        Array(std::initializer_list<T> list) : m_data(nullptr), m_size(0), m_capacity(0) {
            Reserve(list.size());
            for (const auto& item : list) {
                PushBack(item);
            }
        }
        
        /**
         * @brief Copy constructor
         * @param other Array to copy
         */
        Array(const Array& other) : m_data(nullptr), m_size(0), m_capacity(0) {
            Reserve(other.m_size);
            for (SizeType i = 0; i < other.m_size; ++i) {
                new (&m_data[i]) T(other.m_data[i]);
            }
            m_size = other.m_size;
        }
        
        /**
         * @brief Move constructor
         * @param other Array to move
         */
        Array(Array&& other) noexcept 
            : m_data(other.m_data)
            , m_size(other.m_size)
            , m_capacity(other.m_capacity) {
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        
        /**
         * @brief Destructor
         */
        ~Array() {
            Clear();
            Memory::Free(m_data);
        }
        
        /**
         * @brief Copy assignment operator
         * @param other Array to copy
         * @return Reference to this array
         */
        Array& operator=(const Array& other) {
            if (this != &other) {
                Clear();
                Reserve(other.m_size);
                for (SizeType i = 0; i < other.m_size; ++i) {
                    new (&m_data[i]) T(other.m_data[i]);
                }
                m_size = other.m_size;
            }
            return *this;
        }
        
        /**
         * @brief Move assignment operator
         * @param other Array to move
         * @return Reference to this array
         */
        Array& operator=(Array&& other) noexcept {
            if (this != &other) {
                Clear();
                Memory::Free(m_data);
                
                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                
                other.m_data = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
            }
            return *this;
        }
        
        /**
         * @brief Array subscript operator
         * @param index Element index
         * @return Reference to element
         */
        Reference operator[](SizeType index) {
            DSRT_ASSERT(index < m_size);
            return m_data[index];
        }
        
        /**
         * @brief Array subscript operator (const)
         * @param index Element index
         * @return Const reference to element
         */
        ConstReference operator[](SizeType index) const {
            DSRT_ASSERT(index < m_size);
            return m_data[index];
        }
        
        /**
         * @brief Gets pointer to data
         * @return Pointer to data
         */
        Pointer Data() { return m_data; }
        
        /**
         * @brief Gets pointer to data (const)
         * @return Const pointer to data
         */
        ConstPointer Data() const { return m_data; }
        
        /**
         * @brief Gets array size
         * @return Number of elements
         */
        SizeType Size() const { return m_size; }
        
        /**
         * @brief Gets array capacity
         * @return Capacity in elements
         */
        SizeType Capacity() const { return m_capacity; }
        
        /**
         * @brief Checks if array is empty
         * @return True if empty
         */
        bool IsEmpty() const { return m_size == 0; }
        
        // Element access
        
        /**
         * @brief Gets first element
         * @return Reference to first element
         */
        Reference Front() {
            DSRT_ASSERT(m_size > 0);
            return m_data[0];
        }
        
        /**
         * @brief Gets first element (const)
         * @return Const reference to first element
         */
        ConstReference Front() const {
            DSRT_ASSERT(m_size > 0);
            return m_data[0];
        }
        
        /**
         * @brief Gets last element
         * @return Reference to last element
         */
        Reference Back() {
            DSRT_ASSERT(m_size > 0);
            return m_data[m_size - 1];
        }
        
        /**
         * @brief Gets last element (const)
         * @return Const reference to last element
         */
        ConstReference Back() const {
            DSRT_ASSERT(m_size > 0);
            return m_data[m_size - 1];
        }
        
        /**
         * @brief Gets element at index with bounds checking
         * @param index Element index
         * @return Reference to element
         */
        Reference At(SizeType index) {
            DSRT_ASSERT(index < m_size);
            return m_data[index];
        }
        
        /**
         * @brief Gets element at index with bounds checking (const)
         * @param index Element index
         * @return Const reference to element
         */
        ConstReference At(SizeType index) const {
            DSRT_ASSERT(index < m_size);
            return m_data[index];
        }
        
        // Modifiers
        
        /**
         * @brief Clears the array
         */
        void Clear() {
            for (SizeType i = 0; i < m_size; ++i) {
                m_data[i].~T();
            }
            m_size = 0;
        }
        
        /**
         * @brief Reserves capacity
         * @param newCapacity New capacity
         */
        void Reserve(SizeType newCapacity) {
            if (newCapacity <= m_capacity) return;
            
            T* newData = static_cast<T*>(Memory::Allocate(newCapacity * sizeof(T)));
            
            // Move existing elements
            for (SizeType i = 0; i < m_size; ++i) {
                new (&newData[i]) T(std::move(m_data[i]));
                m_data[i].~T();
            }
            
            Memory::Free(m_data);
            m_data = newData;
            m_capacity = newCapacity;
        }
        
        /**
         * @brief Resizes the array
         * @param newSize New size
         */
        void Resize(SizeType newSize) {
            Resize(newSize, T());
        }
        
        /**
         * @brief Resizes the array with default value
         * @param newSize New size
         * @param value Default value for new elements
         */
        void Resize(SizeType newSize, const T& value) {
            if (newSize > m_capacity) {
                Reserve(CalculateGrowth(newSize));
            }
            
            if (newSize > m_size) {
                // Construct new elements
                for (SizeType i = m_size; i < newSize; ++i) {
                    new (&m_data[i]) T(value);
                }
            } else if (newSize < m_size) {
                // Destroy removed elements
                for (SizeType i = newSize; i < m_size; ++i) {
                    m_data[i].~T();
                }
            }
            
            m_size = newSize;
        }
        
        /**
         * @brief Shrinks capacity to fit size
         */
        void ShrinkToFit() {
            if (m_size == m_capacity) return;
            
            if (m_size == 0) {
                Memory::Free(m_data);
                m_data = nullptr;
                m_capacity = 0;
            } else {
                T* newData = static_cast<T*>(Memory::Allocate(m_size * sizeof(T)));
                
                for (SizeType i = 0; i < m_size; ++i) {
                    new (&newData[i]) T(std::move(m_data[i]));
                    m_data[i].~T();
                }
                
                Memory::Free(m_data);
                m_data = newData;
                m_capacity = m_size;
            }
        }
        
        /**
         * @brief Adds element to the end
         * @param value Element to add
         */
        void PushBack(const T& value) {
            if (m_size == m_capacity) {
                Reserve(CalculateGrowth(m_size + 1));
            }
            new (&m_data[m_size]) T(value);
            ++m_size;
        }
        
        /**
         * @brief Adds element to the end (move)
         * @param value Element to add
         */
        void PushBack(T&& value) {
            if (m_size == m_capacity) {
                Reserve(CalculateGrowth(m_size + 1));
            }
            new (&m_data[m_size]) T(std::move(value));
            ++m_size;
        }
        
        /**
         * @brief Constructs element in place at the end
         * @tparam Args Argument types
         * @param args Constructor arguments
         * @return Reference to constructed element
         */
        template<typename... Args>
        Reference EmplaceBack(Args&&... args) {
            if (m_size == m_capacity) {
                Reserve(CalculateGrowth(m_size + 1));
            }
            new (&m_data[m_size]) T(std::forward<Args>(args)...);
            ++m_size;
            return m_data[m_size - 1];
        }
        
        /**
         * @brief Removes last element
         */
        void PopBack() {
            DSRT_ASSERT(m_size > 0);
            m_data[m_size - 1].~T();
            --m_size;
        }
        
        /**
         * @brief Inserts element at position
         * @param position Insert position
         * @param value Element to insert
         * @return Iterator to inserted element
         */
        Iterator Insert(ConstIterator position, const T& value) {
            return Insert(position, 1, value);
        }
        
        /**
         * @brief Inserts element at position (move)
         * @param position Insert position
         * @param value Element to insert
         * @return Iterator to inserted element
         */
        Iterator Insert(ConstIterator position, T&& value) {
            SizeType index = position - m_data;
            DSRT_ASSERT(index <= m_size);
            
            if (m_size == m_capacity) {
                Reserve(CalculateGrowth(m_size + 1));
            }
            
            // Move elements to make space
            for (SizeType i = m_size; i > index; --i) {
                new (&m_data[i]) T(std::move(m_data[i - 1]));
                m_data[i - 1].~T();
            }
            
            // Insert new element
            new (&m_data[index]) T(std::move(value));
            ++m_size;
            
            return m_data + index;
        }
        
        /**
         * @brief Inserts multiple elements at position
         * @param position Insert position
         * @param count Number of elements to insert
         * @param value Value to insert
         * @return Iterator to first inserted element
         */
        Iterator Insert(ConstIterator position, SizeType count, const T& value) {
            SizeType index = position - m_data;
            DSRT_ASSERT(index <= m_size);
            
            if (count == 0) return m_data + index;
            
            if (m_size + count > m_capacity) {
                Reserve(CalculateGrowth(m_size + count));
            }
            
            // Move elements to make space
            for (SizeType i = m_size + count - 1; i >= index + count; --i) {
                new (&m_data[i]) T(std::move(m_data[i - count]));
                m_data[i - count].~T();
            }
            
            // Insert new elements
            for (SizeType i = 0; i < count; ++i) {
                new (&m_data[index + i]) T(value);
            }
            
            m_size += count;
            return m_data + index;
        }
        
        /**
         * @brief Erases element at position
         * @param position Element position
         * @return Iterator to next element
         */
        Iterator Erase(ConstIterator position) {
            SizeType index = position - m_data;
            DSRT_ASSERT(index < m_size);
            
            m_data[index].~T();
            
            // Move elements to fill gap
            for (SizeType i = index; i < m_size - 1; ++i) {
                new (&m_data[i]) T(std::move(m_data[i + 1]));
                m_data[i + 1].~T();
            }
            
            --m_size;
            return m_data + index;
        }
        
        /**
         * @brief Erases range of elements
         * @param first First element to erase
         * @param last One past last element to erase
         * @return Iterator to next element
         */
        Iterator Erase(ConstIterator first, ConstIterator last) {
            SizeType firstIndex = first - m_data;
            SizeType lastIndex = last - m_data;
            DSRT_ASSERT(firstIndex <= lastIndex);
            DSRT_ASSERT(lastIndex <= m_size);
            
            if (firstIndex == lastIndex) return m_data + firstIndex;
            
            SizeType count = lastIndex - firstIndex;
            
            // Destroy erased elements
            for (SizeType i = firstIndex; i < lastIndex; ++i) {
                m_data[i].~T();
            }
            
            // Move elements to fill gap
            for (SizeType i = firstIndex; i < m_size - count; ++i) {
                new (&m_data[i]) T(std::move(m_data[i + count]));
                m_data[i + count].~T();
            }
            
            m_size -= count;
            return m_data + firstIndex;
        }
        
        // Iterators
        
        /**
         * @brief Gets iterator to beginning
         * @return Iterator to first element
         */
        Iterator Begin() { return m_data; }
        
        /**
         * @brief Gets iterator to beginning (const)
         * @return Const iterator to first element
         */
        ConstIterator Begin() const { return m_data; }
        
        /**
         * @brief Gets iterator to end
         * @return Iterator to one past last element
         */
        Iterator End() { return m_data + m_size; }
        
        /**
         * @brief Gets iterator to end (const)
         * @return Const iterator to one past last element
         */
        ConstIterator End() const { return m_data + m_size; }
        
        // Search
        
        /**
         * @brief Finds element in array
         * @param value Value to find
         * @return Iterator to found element, or End() if not found
         */
        Iterator Find(const T& value) {
            for (SizeType i = 0; i < m_size; ++i) {
                if (m_data[i] == value) {
                    return m_data + i;
                }
            }
            return End();
        }
        
        /**
         * @brief Finds element in array (const)
         * @param value Value to find
         * @return Const iterator to found element, or End() if not found
         */
        ConstIterator Find(const T& value) const {
            for (SizeType i = 0; i < m_size; ++i) {
                if (m_data[i] == value) {
                    return m_data + i;
                }
            }
            return End();
        }
        
        /**
         * @brief Checks if array contains value
         * @param value Value to check
         * @return True if contains value
         */
        bool Contains(const T& value) const {
            return Find(value) != End();
        }
        
        // Comparison
        
        /**
         * @brief Compares arrays for equality
         * @param other Array to compare
         * @return True if arrays are equal
         */
        bool operator==(const Array& other) const {
            if (m_size != other.m_size) return false;
            for (SizeType i = 0; i < m_size; ++i) {
                if (m_data[i] != other.m_data[i]) return false;
            }
            return true;
        }
        
        /**
         * @brief Compares arrays for inequality
         * @param other Array to compare
         * @return True if arrays are not equal
         */
        bool operator!=(const Array& other) const {
            return !(*this == other);
        }
        
    private:
        T* m_data;
        SizeType m_size;
        SizeType m_capacity;
        
        /**
         * @brief Calculates new capacity for growth
         * @param newSize Requested size
         * @return New capacity
         */
        SizeType CalculateGrowth(SizeType newSize) const {
            SizeType geometric = m_capacity + m_capacity / 2;
            if (geometric < newSize) {
                return newSize;
            }
            return geometric;
        }
    };
    
} // namespace DSRT
