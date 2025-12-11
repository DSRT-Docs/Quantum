#include "DSRT/Core/Memory.h"
#include "DSRT/Core/Assert.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace DSRT {
namespace Core {

// Static members
UniqueRef<Allocator> Memory::s_DefaultAllocator;
UniqueRef<Allocator> Memory::s_FrameAllocator;
UniqueRef<Allocator> Memory::s_PersistentAllocator;
size_t Memory::s_TotalAllocated = 0;
size_t Memory::s_PeakAllocated = 0;

// Default allocator using system malloc/free
class DefaultAllocatorImpl : public Allocator {
public:
    void* Allocate(size_t size, size_t alignment) override {
        if (size == 0) return nullptr;
        
        // Align allocation
        size_t actualSize = size + alignment + sizeof(void*);
        void* raw = malloc(actualSize);
        if (!raw) return nullptr;
        
        void* aligned = std::align(alignment, size, raw, actualSize);
        if (!aligned) {
            free(raw);
            return nullptr;
        }
        
        // Store original pointer for freeing
        *static_cast<void**>(aligned) = raw;
        m_AllocatedSize += size;
        Memory::s_TotalAllocated += size;
        Memory::s_PeakAllocated = std::max(Memory::s_PeakAllocated, Memory::s_TotalAllocated);
        
        return static_cast<char*>(aligned) + sizeof(void*);
    }
    
    void Deallocate(void* ptr) override {
        if (!ptr) return;
        
        void* original = *static_cast<void**>(static_cast<char*>(ptr) - sizeof(void*));
        size_t size = malloc_usable_size(original) - alignment - sizeof(void*);
        
        free(original);
        m_AllocatedSize -= size;
        Memory::s_TotalAllocated -= size;
    }
    
    size_t GetAllocatedSize() const override { return m_AllocatedSize; }
    size_t GetTotalSize() const override { return 0; } // System allocator has no fixed size
    
private:
    size_t m_AllocatedSize = 0;
};

// LinearAllocator implementation
LinearAllocator::LinearAllocator(void* memory, size_t size)
    : m_Memory(memory)
    , m_TotalSize(size)
    , m_UsedSize(0) {}

LinearAllocator::~LinearAllocator() {
    // Memory is owned by caller
}

void* LinearAllocator::Allocate(size_t size, size_t alignment) {
    if (size == 0) return nullptr;
    
    // Calculate aligned offset
    size_t current = reinterpret_cast<size_t>(m_Memory) + m_UsedSize;
    size_t aligned = (current + alignment - 1) & ~(alignment - 1);
    size_t offset = aligned - reinterpret_cast<size_t>(m_Memory);
    
    // Check bounds
    if (offset + size > m_TotalSize) {
        return nullptr;
    }
    
    void* ptr = static_cast<char*>(m_Memory) + offset;
    m_UsedSize = offset + size;
    
    return ptr;
}

void LinearAllocator::Deallocate(void* ptr) {
    // Linear allocator doesn't support individual deallocation
    // Call Reset() to free all allocations
}

size_t LinearAllocator::GetAllocatedSize() const {
    return m_UsedSize;
}

size_t LinearAllocator::GetTotalSize() const {
    return m_TotalSize;
}

void LinearAllocator::Reset() {
    m_UsedSize = 0;
}

// PoolAllocator implementation
PoolAllocator::PoolAllocator(size_t blockSize, size_t blockCount)
    : m_BlockSize(std::max(blockSize, sizeof(Block)))
    , m_BlockCount(blockCount)
    , m_FreeCount(blockCount) {
    
    // Allocate memory pool
    m_Memory = malloc(m_BlockSize * m_BlockCount);
    if (!m_Memory) {
        return;
    }
    
    // Initialize free list
    m_FreeList = nullptr;
    uint8_t* pool = static_cast<uint8_t*>(m_Memory);
    
    for (size_t i = 0; i < m_BlockCount; ++i) {
        Block* block = reinterpret_cast<Block*>(pool + i * m_BlockSize);
        block->next = m_FreeList;
        m_FreeList = block;
    }
}

PoolAllocator::~PoolAllocator() {
    if (m_Memory) {
        free(m_Memory);
    }
}

void* PoolAllocator::Allocate(size_t size, size_t alignment) {
    DSRT_ASSERT(size <= m_BlockSize, "Allocation size exceeds block size");
    DSRT_ASSERT(alignment <= alignof(std::max_align_t), "Alignment too large");
    
    if (!m_FreeList || m_FreeCount == 0) {
        return nullptr;
    }
    
    Block* block = m_FreeList;
    m_FreeList = block->next;
    m_FreeCount--;
    
    return block;
}

void PoolAllocator::Deallocate(void* ptr) {
    if (!ptr) return;
    
    // Add block back to free list
    Block* block = static_cast<Block*>(ptr);
    block->next = m_FreeList;
    m_FreeList = block;
    m_FreeCount++;
}

size_t PoolAllocator::GetAllocatedSize() const {
    return (m_BlockCount - m_FreeCount) * m_BlockSize;
}

size_t PoolAllocator::GetTotalSize() const {
    return m_BlockCount * m_BlockSize;
}

// MemoryPool implementation
template<typename T>
MemoryPool<T>::MemoryPool(size_t capacity)
    : m_Capacity(capacity)
    , m_UsedCount(0) {
    
    m_Memory = static_cast<PoolItem*>(malloc(sizeof(PoolItem) * capacity));
    if (!m_Memory) {
        return;
    }
    
    // Initialize free list
    m_FreeList = nullptr;
    for (size_t i = 0; i < capacity; ++i) {
        PoolItem* item = &m_Memory[i];
        item->next = m_FreeList;
        m_FreeList = item;
    }
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Call destructors for allocated objects
    for (size_t i = 0; i < m_Capacity; ++i) {
        if (m_Memory[i].next == nullptr) { // Object is allocated
            m_Memory[i].object.~T();
        }
    }
    
    free(m_Memory);
}

template<typename T>
T* MemoryPool<T>::Allocate() {
    if (!m_FreeList) {
        return nullptr;
    }
    
    PoolItem* item = m_FreeList;
    m_FreeList = item->next;
    m_UsedCount++;
    
    // Construct object in-place
    new (&item->object) T();
    return &item->object;
}

template<typename T>
void MemoryPool<T>::Deallocate(T* ptr) {
    if (!ptr) return;
    
    // Find which PoolItem contains this object
    PoolItem* item = reinterpret_cast<PoolItem*>(ptr);
    
    // Call destructor
    item->object.~T();
    
    // Add back to free list
    item->next = m_FreeList;
    m_FreeList = item;
    m_UsedCount--;
}

template<typename T>
size_t MemoryPool<T>::GetCapacity() const {
    return m_Capacity;
}

template<typename T>
size_t MemoryPool<T>::GetUsedCount() const {
    return m_UsedCount;
}

// Memory static methods
void Memory::Initialize() {
    // Create default allocators
    size_t defaultHeapSize = 64 * 1024 * 1024; // 64MB
    size_t frameHeapSize = 16 * 1024 * 1024;   // 16MB
    size_t persistentHeapSize = 32 * 1024 * 1024; // 32MB
    
    void* defaultHeap = malloc(defaultHeapSize);
    void* frameHeap = malloc(frameHeapSize);
    void* persistentHeap = malloc(persistentHeapSize);
    
    s_DefaultAllocator = std::make_unique<DefaultAllocatorImpl>();
    s_FrameAllocator = std::make_unique<LinearAllocator>(frameHeap, frameHeapSize);
    s_PersistentAllocator = std::make_unique<LinearAllocator>(persistentHeap, persistentHeapSize);
    
    s_TotalAllocated = 0;
    s_PeakAllocated = 0;
}

void Memory::Shutdown() {
    // Free allocator memory
    if (s_FrameAllocator) {
        void* memory = static_cast<LinearAllocator*>(s_FrameAllocator.get())->m_Memory;
        free(memory);
    }
    
    if (s_PersistentAllocator) {
        void* memory = static_cast<LinearAllocator*>(s_PersistentAllocator.get())->m_Memory;
        free(memory);
    }
    
    s_DefaultAllocator.reset();
    s_FrameAllocator.reset();
    s_PersistentAllocator.reset();
}

Allocator& Memory::GetDefaultAllocator() {
    DSRT_ASSERT(s_DefaultAllocator, "Memory system not initialized");
    return *s_DefaultAllocator;
}

Allocator& Memory::GetFrameAllocator() {
    DSRT_ASSERT(s_FrameAllocator, "Memory system not initialized");
    return *s_FrameAllocator;
}

Allocator& Memory::GetPersistentAllocator() {
    DSRT_ASSERT(s_PersistentAllocator, "Memory system not initialized");
    return *s_PersistentAllocator;
}

void* Memory::Allocate(size_t size, size_t alignment) {
    return GetDefaultAllocator().Allocate(size, alignment);
}

void Memory::Deallocate(void* ptr) {
    GetDefaultAllocator().Deallocate(ptr);
}

size_t Memory::GetTotalAllocated() {
    return s_TotalAllocated;
}

size_t Memory::GetPeakAllocated() {
    return s_PeakAllocated;
}

// Explicit template instantiations
template class MemoryPool<int>;
template class MemoryPool<float>;
template class MemoryPool<double>;
// Add more types as needed

} // namespace Core
} // namespace DSRT
