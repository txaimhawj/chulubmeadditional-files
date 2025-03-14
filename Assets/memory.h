#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

namespace CHULUBME {

/**
 * @brief Memory allocator interface
 */
class Allocator {
public:
    virtual ~Allocator() = default;

    // Allocate memory
    virtual void* Allocate(size_t size, size_t alignment = 8) = 0;

    // Free memory
    virtual void Free(void* ptr) = 0;

    // Get total allocated memory
    virtual size_t GetTotalAllocated() const = 0;

    // Get allocation count
    virtual size_t GetAllocationCount() const = 0;
};

/**
 * @brief Linear allocator for temporary allocations
 * Allocates memory linearly and only frees all memory at once
 */
class LinearAllocator : public Allocator {
public:
    LinearAllocator(size_t size);
    ~LinearAllocator() override;

    // Allocate memory
    void* Allocate(size_t size, size_t alignment = 8) override;

    // Free memory (does nothing for individual allocations)
    void Free(void* ptr) override;

    // Reset the allocator (frees all memory)
    void Reset();

    // Get total allocated memory
    size_t GetTotalAllocated() const override;

    // Get allocation count
    size_t GetAllocationCount() const override;

private:
    // Memory block
    void* m_memory;

    // Total size
    size_t m_size;

    // Current offset
    size_t m_offset;

    // Allocation count
    size_t m_allocationCount;
};

/**
 * @brief Pool allocator for fixed-size allocations
 * Efficiently allocates and frees fixed-size memory blocks
 */
class PoolAllocator : public Allocator {
public:
    PoolAllocator(size_t blockSize, size_t blockCount);
    ~PoolAllocator() override;

    // Allocate memory
    void* Allocate(size_t size, size_t alignment = 8) override;

    // Free memory
    void Free(void* ptr) override;

    // Get total allocated memory
    size_t GetTotalAllocated() const override;

    // Get allocation count
    size_t GetAllocationCount() const override;

private:
    // Memory block
    void* m_memory;

    // Free list head
    void* m_freeList;

    // Block size
    size_t m_blockSize;

    // Block count
    size_t m_blockCount;

    // Total size
    size_t m_size;

    // Allocation count
    size_t m_allocationCount;
};

/**
 * @brief Stack allocator for LIFO allocations
 * Allocates memory in a stack-like fashion
 */
class StackAllocator : public Allocator {
public:
    StackAllocator(size_t size);
    ~StackAllocator() override;

    // Allocate memory
    void* Allocate(size_t size, size_t alignment = 8) override;

    // Free memory (must be in LIFO order)
    void Free(void* ptr) override;

    // Get marker for current position
    size_t GetMarker() const;

    // Free to marker
    void FreeToMarker(size_t marker);

    // Reset the allocator (frees all memory)
    void Reset();

    // Get total allocated memory
    size_t GetTotalAllocated() const override;

    // Get allocation count
    size_t GetAllocationCount() const override;

private:
    // Memory block
    void* m_memory;

    // Total size
    size_t m_size;

    // Current offset
    size_t m_offset;

    // Allocation count
    size_t m_allocationCount;
};

/**
 * @brief Heap allocator that wraps system malloc/free
 */
class HeapAllocator : public Allocator {
public:
    HeapAllocator();
    ~HeapAllocator() override;

    // Allocate memory
    void* Allocate(size_t size, size_t alignment = 8) override;

    // Free memory
    void Free(void* ptr) override;

    // Get total allocated memory
    size_t GetTotalAllocated() const override;

    // Get allocation count
    size_t GetAllocationCount() const override;

private:
    // Allocation tracking
    struct AllocationInfo {
        void* originalPtr;
        size_t size;
    };

    // Allocation map
    std::unordered_map<void*, AllocationInfo> m_allocations;

    // Mutex for thread safety
    mutable std::mutex m_mutex;

    // Total allocated memory
    size_t m_totalAllocated;

    // Allocation count
    size_t m_allocationCount;
};

/**
 * @brief Memory manager that provides different allocators for different purposes
 */
class MemoryManager {
public:
    // Allocator types
    enum class AllocatorType {
        Heap,       // General purpose allocations
        Frame,      // Per-frame temporary allocations
        Pool,       // Fixed-size allocations
        Stack       // LIFO allocations
    };

    // Singleton instance
    static MemoryManager& Instance();

    // Initialize the memory manager
    bool Initialize();

    // Shutdown the memory manager
    void Shutdown();

    // Begin frame (resets frame allocator)
    void BeginFrame();

    // End frame
    void EndFrame();

    // Get an allocator
    Allocator* GetAllocator(AllocatorType type);

    // Create a pool allocator
    PoolAllocator* CreatePoolAllocator(const std::string& name, size_t blockSize, size_t blockCount);

    // Create a stack allocator
    StackAllocator* CreateStackAllocator(const std::string& name, size_t size);

    // Destroy a named allocator
    void DestroyAllocator(const std::string& name);

    // Get memory statistics
    struct MemoryStats {
        size_t totalAllocated;
        size_t totalAllocations;
        size_t frameAllocated;
        size_t frameAllocations;
        size_t poolAllocated;
        size_t poolAllocations;
        size_t stackAllocated;
        size_t stackAllocations;
    };
    MemoryStats GetMemoryStats() const;

private:
    // Private constructor for singleton
    MemoryManager();
    ~MemoryManager();

    // Deleted copy constructor and assignment operator
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    // Default allocators
    std::unique_ptr<HeapAllocator> m_heapAllocator;
    std::unique_ptr<LinearAllocator> m_frameAllocator;

    // Named allocators
    std::unordered_map<std::string, std::unique_ptr<Allocator>> m_namedAllocators;

    // Mutex for thread safety
    mutable std::mutex m_mutex;
};

/**
 * @brief Custom allocator for STL containers
 */
template<typename T>
class StlAllocator {
public:
    // STL allocator type definitions
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // Constructor
    StlAllocator(MemoryManager::AllocatorType allocatorType = MemoryManager::AllocatorType::Heap)
        : m_allocatorType(allocatorType) {}

    // Copy constructor
    template<typename U>
    StlAllocator(const StlAllocator<U>& other)
        : m_allocatorType(other.m_allocatorType) {}

    // Rebind allocator to type U
    template<typename U>
    struct rebind {
        using other = StlAllocator<U>;
    };

    // Allocate memory
    pointer allocate(size_type n) {
        Allocator* allocator = MemoryManager::Instance().GetAllocator(m_allocatorType);
        return static_cast<pointer>(allocator->Allocate(n * sizeof(T), alignof(T)));
    }

    // Deallocate memory
    void deallocate(pointer p, size_type n) {
        Allocator* allocator = MemoryManager::Instance().GetAllocator(m_allocatorType);
        allocator->Free(p);
    }

    // Construct object
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new(p) U(std::forward<Args>(args)...);
    }

    // Destroy object
    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    // Equality comparison
    template<typename U>
    bool operator==(const StlAllocator<U>& other) const {
        return m_allocatorType == other.m_allocatorType;
    }

    // Inequality comparison
    template<typename U>
    bool operator!=(const StlAllocator<U>& other) const {
        return m_allocatorType != other.m_allocatorType;
    }

private:
    // Allocator type
    MemoryManager::AllocatorType m_allocatorType;

    // Friend declaration for copy constructor
    template<typename U>
    friend class StlAllocator;
};

/**
 * @brief Memory tracking utilities
 */
class MemoryTracker {
public:
    // Enable/disable memory tracking
    static void Enable(bool enable);

    // Check if memory tracking is enabled
    static bool IsEnabled();

    // Record allocation
    static void RecordAllocation(void* ptr, size_t size, const char* file, int line);

    // Record deallocation
    static void RecordDeallocation(void* ptr);

    // Print memory leaks
    static void PrintLeaks();

private:
    // Allocation info
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
    };

    // Allocation map
    static std::unordered_map<void*, AllocationInfo> s_allocations;

    // Mutex for thread safety
    static std::mutex s_mutex;

    // Enabled flag
    static bool s_enabled;
};

// Memory allocation macros
#ifdef CHULUBME_MEMORY_TRACKING
    #define CHULUBME_NEW(Type, ...) new(MemoryTracker::RecordAllocation(nullptr, sizeof(Type), __FILE__, __LINE__)) Type(__VA_ARGS__)
    #define CHULUBME_DELETE(ptr) do { if(ptr) { MemoryTracker::RecordDeallocation(ptr); delete ptr; ptr = nullptr; } } while(0)
    #define CHULUBME_NEW_ARRAY(Type, count) new(MemoryTracker::RecordAllocation(nullptr, sizeof(Type) * count, __FILE__, __LINE__)) Type[count]
    #define CHULUBME_DELETE_ARRAY(ptr) do { if(ptr) { MemoryTracker::RecordDeallocation(ptr); delete[] ptr; ptr = nullptr; } } while(0)
    #define CHULUBME_MALLOC(size) MemoryTracker::RecordAllocation(malloc(size), size, __FILE__, __LINE__)
    #define CHULUBME_FREE(ptr) do { if(ptr) { MemoryTracker::RecordDeallocation(ptr); free(ptr); ptr = nullptr; } } while(0)
#else
    #define CHULUBME_NEW(Type, ...) new Type(__VA_ARGS__)
    #define CHULUBME_DELETE(ptr) do { if(ptr) { delete ptr; ptr = nullptr; } } while(0)
    #define CHULUBME_NEW_ARRAY(Type, count) new Type[count]
    #define CHULUBME_DELETE_ARRAY(ptr) do { if(ptr) { delete[] ptr; ptr = nullptr; } } while(0)
    #define CHULUBME_MALLOC(size) malloc(size)
    #define CHULUBME_FREE(ptr) do { if(ptr) { free(ptr); ptr = nullptr; } } while(0)
#endif

} // namespace CHULUBME
