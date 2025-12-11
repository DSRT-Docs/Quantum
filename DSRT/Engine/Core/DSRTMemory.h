#pragma once

/**
 * @file DSRTMemory.h
 * @brief Memory management utilities
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include <cstdlib>
#include <new>

namespace DSRT {
    
    /**
     * @brief Memory allocation functions
     */
    class Memory {
    public:
        /**
         * @brief Allocates memory with specified alignment
         * @param size Size in bytes to allocate
         * @param alignment Memory alignment (must be power of two)
         * @return Pointer to allocated memory
         */
        static void* AllocateAligned(size_t size, size_t alignment = 16);
        
        /**
         * @brief Frees aligned memory
         * @param ptr Pointer to memory to free
         */
        static void FreeAligned(void* ptr);
        
        /**
         * @brief Allocates memory
         * @param size Size in bytes to allocate
         * @return Pointer to allocated memory
         */
        static void* Allocate(size_t size);
        
        /**
         * @brief Frees memory
         * @param ptr Pointer to memory to free
         */
        static void Free(void* ptr);
        
        /**
         * @brief Copies memory
         * @param dest Destination buffer
         * @param src Source buffer
         * @param size Number of bytes to copy
         * @return Pointer to destination buffer
         */
        static void* Copy(void* dest, const void* src, size_t size);
        
        /**
         * @brief Sets memory to specified value
         * @param dest Destination buffer
         * @param value Value to set
         * @param size Number of bytes to set
         * @return Pointer to destination buffer
         */
        static void* Set(void* dest, int value, size_t size);
        
        /**
         * @brief Zeros memory
         * @param dest Destination buffer
         * @param size Number of bytes to zero
         * @return Pointer to destination buffer
         */
        static void* Zero(void* dest, size_t size);
        
        /**
         * @brief Compares memory regions
         * @param ptr1 First buffer
         * @param ptr2 Second buffer
         * @param size Number of bytes to compare
         * @return Negative if ptr1 < ptr2, 0 if equal, positive if ptr1 > ptr2
         */
        static int Compare(const void* ptr1, const void* ptr2, size_t size);
        
        /**
         * @brief Gets memory page size
         * @return Page size in bytes
         */
        static size_t GetPageSize();
        
        /**
         * @brief Allocates memory pages
         * @param size Size in bytes (multiple of page size)
         * @return Pointer to allocated pages
         */
        static void* AllocatePages(size_t size);
        
        /**
         * @brief Frees memory pages
         * @param ptr Pointer to pages to free
         * @param size Size in bytes
         */
        static void FreePages(void* ptr, size_t size);
        
        /**
         * @brief Commits memory pages
         * @param ptr Pointer to pages
         * @param size Size in bytes
         * @return True if successful
         */
        static bool CommitPages(void* ptr, size_t size);
        
        /**
         * @brief Decommits memory pages
         * @param ptr Pointer to pages
         * @param size Size in bytes
         * @return True if successful
         */
        static bool DecommitPages(void* ptr, size_t size);
        
        /**
         * @brief Sets memory protection
         * @param ptr Pointer to memory region
         * @param size Size in bytes
         * @param read Allow read access
         * @param write Allow write access
         * @param execute Allow execute access
         * @return True if successful
         */
        static bool ProtectMemory(void* ptr, size_t size, 
                                 bool read, bool write, bool execute);
        
        /**
         * @brief Gets total physical memory
         * @return Total physical memory in bytes
         */
        static uint64 GetTotalPhysicalMemory();
        
        /**
         * @brief Gets available physical memory
         * @return Available physical memory in bytes
         */
        static uint64 GetAvailablePhysicalMemory();
        
        /**
         * @brief Gets total virtual memory
         * @return Total virtual memory in bytes
         */
        static uint64 GetTotalVirtualMemory();
        
        /**
         * @brief Gets available virtual memory
         * @return Available virtual memory in bytes
         */
        static uint64 GetAvailableVirtualMemory();
        
        /**
         * @brief Gets current process memory usage
         * @return Memory usage in bytes
         */
        static uint64 GetProcessMemoryUsage();
        
        /**
         * @brief Gets peak process memory usage
         * @return Peak memory usage in bytes
         */
        static uint64 GetProcessPeakMemoryUsage();
        
        /**
         * @brief Dumps memory leaks (debug only)
         */
        static void DumpMemoryLeaks();
    };
    
    // Memory utilities
    template<typename T>
    T* AllocateArray(size_t count) {
        return static_cast<T*>(Memory::Allocate(sizeof(T) * count));
    }
    
    template<typename T>
    void FreeArray(T* ptr) {
        Memory::Free(ptr);
    }
    
    template<typename T>
    T* AllocateAlignedArray(size_t count, size_t alignment = alignof(T)) {
        return static_cast<T*>(Memory::AllocateAligned(sizeof(T) * count, alignment));
    }
    
    template<typename T>
    void FreeAlignedArray(T* ptr) {
        Memory::FreeAligned(ptr);
    }
    
    // Placement new for aligned memory
    inline void* operator new(size_t size, void* ptr, size_t alignment) {
        DSRT_UNUSED(alignment);
        return ptr;
    }
    
    inline void operator delete(void*, void*, size_t) {
        // Placement delete
    }
    
    // Memory tracking (debug only)
#if DSRT_CONFIG_DEBUG
    class MemoryTracker {
    public:
        static void TrackAllocation(void* ptr, size_t size, 
                                   const char* file, int line);
        static void TrackDeallocation(void* ptr);
        static void DumpAllocations();
        static size_t GetAllocationCount();
        static size_t GetTotalAllocatedSize();
        
    private:
        MemoryTracker() = delete;
        ~MemoryTracker() = delete;
    };
    
    #define DSRT_NEW new(__FILE__, __LINE__)
    #define DSRT_DELETE delete
#else
    #define DSRT_NEW new
    #define DSRT_DELETE delete
#endif
    
} // namespace DSRT
