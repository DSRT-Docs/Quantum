/**
 * @file Memory.h
 * @brief Memory management system for DSRT Engine
 * @ingroup Core
 * 
 * @details Provides custom allocators and memory tracking for
 *          game development scenarios (frame-based allocation,
 *          pooling, etc.)
 */

#pragma once
#include "Export.h"
#include "Types.h"
#include <cstddef>

namespace DSRT {
namespace Core {

/**
 * @class Allocator
 * @brief Abstract base class for memory allocators
 * @details Implement custom allocation strategies by inheriting from this class
 */
class DSRT_API Allocator {
public:
    virtual ~Allocator() = default;
    
    /**
     * @brief Allocate memory
     * @param size Number of bytes to allocate
     * @param alignment Memory alignment (default 16 for SIMD)
     * @return Pointer to allocated memory, or nullptr on failure
     */
    virtual void* Allocate(size_t size, size_t alignment = 16) = 0;
    
    /**
     * @brief Deallocate memory
     * @param ptr Pointer to memory previously allocated by this allocator
     */
    virtual void Deallocate(void* ptr) = 0;
    
    /**
     * @brief Get total allocated memory
     * @return Size in bytes of currently allocated memory
     */
    virtual size_t GetAllocatedSize() const = 0;
    
    /**
     * @brief Get total pool size
     * @return Total size in bytes of the allocator's memory pool
     */
    virtual size_t GetTotalSize() const = 0;
};

/**
 * @class LinearAllocator
 * @brief Simple linear/stack allocator
 * @details Allocates sequentially from a fixed block, resets en masse
 * @warning Cannot free individual allocations, only reset entire block
 * 
 * @example
 * @code
 * LinearAllocator allocator(buffer, 1024 * 1024); // 1MB buffer
 * void* data1 = allocator.Allocate(256); // First 256 bytes
 * void* data2 = allocator.Allocate(512); // Next 512 bytes
 * allocator.Reset(); // All allocations freed
 * @endcode
 */
class DSRT_API LinearAllocator : public Allocator {
public:
    /**
     * @brief Construct linear allocator
     * @param memory Pre-allocated memory block
     * @param size Size of memory block in bytes
     */
    LinearAllocator(void* memory, size_t size);
    
    ~LinearAllocator();
    
    void* Allocate(size_t size, size_t alignment = 16) override;
    void Deallocate(void* ptr) override;
    size_t GetAllocatedSize() const override;
    size_t GetTotalSize() const override;
    
    /**
     * @brief Reset allocator to initial state
     * @details Frees all allocations made through this allocator
     */
    void Reset();
    
private:
    void* m_Memory;         /**< Base memory address */
    size_t m_TotalSize;     /**< Total pool size */
    size_t m_UsedSize;      /**< Currently used size */
};

/**
 * @class PoolAllocator
 * @brief Fixed-size block allocator
 * @details Efficient for allocating many objects of same size
 *          (entities, particles, etc.)
 * 
 * @example
 * @code
 * PoolAllocator pool(sizeof(Particle), 1000); // 1000 particles
 * Particle* p1 = static_cast<Particle*>(pool.Allocate());
 * Particle* p2 = static_cast<Particle*>(pool.Allocate());
 * pool.Deallocate(p1);
 * @endcode
 */
class DSRT_API PoolAllocator : public Allocator {
public:
    /**
     * @brief Construct pool allocator
     * @param blockSize Size of each block in bytes
     * @param blockCount Number of blocks in pool
     */
    PoolAllocator(size_t blockSize, size_t blockCount);
    
    ~PoolAllocator();
    
    void* Allocate(size_t size, size_t alignment = 16) override;
    void Deallocate(void* ptr) override;
    size_t GetAllocatedSize() const override;
    size_t GetTotalSize() const override;
    
private:
    struct Block {
        Block* next; /**< Next free block in linked list */
    };
    
    void* m_Memory;         /**< Raw memory pool */
    size_t m_BlockSize;     /**< Size of each block */
    size_t m_BlockCount;    /**< Total number of blocks */
    size_t m_FreeCount;     /**< Number of free blocks */
    Block* m_FreeList;      /**< Linked list of free blocks */
};

/**
 * @class Memory
 * @brief Global memory management system
 * @details Provides access to different allocator types and
 *          tracks overall memory usage
 * 
 * @note Thread-safe for allocations from different threads
 */
class DSRT_API Memory {
public:
    /**
     * @brief Initialize memory system
     * @details Must be called before any memory allocation
     */
    static void Initialize();
    
    /**
     * @brief Shutdown memory system
     * @details Validates all memory has been freed
     * @throws std::runtime_error if memory leaks detected
     */
    static void Shutdown();
    
    /**
     * @brief Get default general-purpose allocator
     * @return Reference to default allocator
     */
    static Allocator& GetDefaultAllocator();
    
    /**
     * @brief Get frame allocator (resets each frame)
     * @return Reference to frame allocator
     * @warning Allocations persist only for current frame
     */
    static Allocator& GetFrameAllocator();
    
    /**
     * @brief Get persistent allocator (long-lived objects)
     * @return Reference to persistent allocator
     */
    static Allocator& GetPersistentAllocator();
    
    /**
     * @brief Allocate memory using default allocator
     * @param size Number of bytes to allocate
     * @param alignment Memory alignment
     * @return Pointer to allocated memory
     * @throws std::bad_alloc if allocation fails
     */
    static void* Allocate(size_t size, size_t alignment = 16);
    
    /**
     * @brief Deallocate memory using default allocator
     * @param ptr Pointer to memory to deallocate
     */
    static void Deallocate(void* ptr);
    
    /**
     * @brief Get total allocated memory across all allocators
     * @return Total allocated bytes
     */
    static size_t GetTotalAllocated();
    
    /**
     * @brief Get peak allocated memory
     * @return Maximum bytes allocated at any point
     */
    static size_t GetPeakAllocated();
    
private:
    static UniqueRef<Allocator> s_DefaultAllocator;
    static UniqueRef<Allocator> s_FrameAllocator;
    static UniqueRef<Allocator> s_PersistentAllocator;
    
    static size_t s_TotalAllocated;
    static size_t s_PeakAllocated;
};

/**
 * @class MemoryPool
 * @brief Type-safe memory pool for specific object types
 * @tparam T Type of objects to pool
 * 
 * @details Combines efficiency of pool allocation with
 *          type safety and constructor/destructor calls
 */
template<typename T>
class DSRT_API MemoryPool {
public:
    /**
     * @brief Construct memory pool
     * @param capacity Maximum number of objects in pool
     */
    MemoryPool(size_t capacity);
    
    ~MemoryPool();
    
    /**
     * @brief Allocate and construct object
     * @return Pointer to constructed object
     * @throws std::bad_alloc if pool is full
     */
    T* Allocate();
    
    /**
     * @brief Destruct and deallocate object
     * @param ptr Pointer to object to deallocate
     */
    void Deallocate(T* ptr);
    
    /**
     * @brief Get pool capacity
     * @return Maximum number of objects
     */
    size_t GetCapacity() const;
    
    /**
     * @brief Get number of used slots
     * @return Currently allocated objects count
     */
    size_t GetUsedCount() const;
    
private:
    union PoolItem {
        T object;           /**< Object storage */
        PoolItem* next;     /**< Next free slot */
    };
    
    PoolItem* m_Memory;     /**< Pool storage */
    PoolItem* m_FreeList;   /**< Free list head */
    size_t m_Capacity;      /**< Total capacity */
    size_t m_UsedCount;     /**< Used slots count */
};

} // namespace Core
} // namespace DSRT
