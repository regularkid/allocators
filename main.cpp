#include <stdlib.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
void* operator new(size_t numBytes)
{
    printf("global new\n");
    return malloc(numBytes);
}

// -----------------------------------------------------------------------------
void* operator new[](size_t numBytes)
{
    printf("global new[]\n");
    return malloc(numBytes);
}

// -----------------------------------------------------------------------------
void operator delete(void* ptr)  noexcept
{
    printf("global delete\n");
    free(ptr);
}

// -----------------------------------------------------------------------------
void operator delete(void* ptr, size_t numBytes)
{
    printf("global delete w/ size = %zu\n", numBytes);
    free(ptr);
}

// -----------------------------------------------------------------------------
void operator delete[](void* ptr)
{
    printf("global delete[]\n");
    free(ptr);
}

// -----------------------------------------------------------------------------
void operator delete[](void* ptr, size_t numBytes)
{
    printf("global delete[] w/ size = %zu\n", numBytes);
    free(ptr);
}

// -----------------------------------------------------------------------------
class scratchAllocator
{
public:
    static void init()
    {
        if (!m_base)
        {
            const size_t maxSize = 1024 * 1024;  // 1mb
            m_base = (char*)malloc(maxSize);
            m_cur = m_base;
        }
    }

    static void shutdown()
    {
        delete m_base;
        m_base = nullptr;
        m_cur = nullptr;
    }

    static void* allocate(const size_t numBytes, const size_t alignment)
    {
        if (numBytes == 0)
        {
            return nullptr;
        }

        printf("scratchAllocator::allocate numBytes: %llu, alignment: %llu\n", numBytes,  alignment);
        init();

        // Calculate next available location that has correct alignment
        size_t curPos = (size_t)m_cur;  // Cast to size_t so we can do bit manipulation without compiler complaining
        size_t newPos = curPos & ~(alignment - 1);  // Clear low bits to get an aligned position (alignment - 1 = lower bits that should be 0's for proper alignment get set, then ~ inverts and we & against curPos = aligned pos)
        if (newPos < curPos)    // If newly aligned position is *before* cur pos, then we need to advance to the next aligned location
        {
            newPos += alignment;
        }
        m_cur = (char*)newPos;  // Cast back newly aligned position from integer
        char* newPtr = m_cur;   // Save off this position as the allocation location to return
        m_cur += numBytes;      // Advance internal position by the number of allocated bytes
        return newPtr;
    }

    static void* allocate(const size_t numBytes)
    {
        return allocate(numBytes, 8);   // Default to 8 byte alignment
    }

    template <class T>
    static T* allocate()
    {
        return static_cast<T*>(allocate(sizeof(T), alignof(T)));
    }

    static void reset()
    {
        printf("scratchAllocator::reset\n");
        m_cur = m_base;
    }

private:
    static char* m_base;
    static char* m_cur;
};

char* scratchAllocator::m_base = nullptr;
char* scratchAllocator::m_cur = nullptr;

// -----------------------------------------------------------------------------
class thing
{
public:
    void* operator new(size_t numBytes)
    {
        return scratchAllocator::allocate(numBytes);
    }

    void operator delete(void* ptr)
    {
        //allocator::deallocate(ptr);
    }

public:
    char m_c = 0;
    short m_s = 0;
    int m_i = 0;
    long long m_l = 0;
    void* m_p = nullptr;
};

int main()
{
    printf("----- allocators -----\n");

    char* c = scratchAllocator::allocate<char>();
    printf("c : 0x%p\n", (void*)c);

    int* i = scratchAllocator::allocate<int>();
    printf("i : 0x%p\n", (void*)i);

    short* s = scratchAllocator::allocate<short>();
    printf("s : 0x%p\n", (void*)s);

    long long* l = scratchAllocator::allocate<long long>();
    printf("l : 0x%p\n", (void*)l);

    thing* t = scratchAllocator::allocate<thing>();
    printf("t : 0x%p\n", (void*)t);

    thing* t2 = new thing();
    printf("t2 : 0x%p\n", (void*)t2);

    *c = 1;
    *s = 2;
    *i = 3;
    *l = 4;
    t->m_i = 123;
    t2->m_i = 456;

    printf("c : %d\n", *c);
    printf("s : %d\n", *s);
    printf("i : %d\n", *i);
    printf("l : %lld\n", *l);
    printf("t->m_i : %d\n", t->m_i);
    printf("t2->m_i : %d\n", t2->m_i);

    scratchAllocator::reset();

    char* c2 = scratchAllocator::allocate<char>();
    printf("c2 : 0x%p\n", (void*)c2);

    *c2 = 5;
    printf("c2 : %d\n", *c2);

    return 0;
}