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

    static void* allocate(const size_t numBytes)
    {
        if (numBytes == 0)
        {
            return nullptr;
        }

        printf("scratchAllocator::allocate\n");
        init();

        size_t alignment = 8;
        switch (numBytes)
        {
            case 1: alignment = 1; break;
            case 2: alignment = 2; break;
            case 3: alignment = 4; break;
            case 4: alignment = 4; break;
            default: alignment = 8;
        }

        size_t curPos = (size_t)m_cur;
        size_t newPos = curPos & ~(alignment - 1);
        if (newPos < curPos)
        {
            newPos += alignment;
        }
        m_cur = (char*)newPos;
        char* newPtr = m_cur;
        m_cur += numBytes;
        return newPtr;
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
};

int main()
{
    printf("----- allocators -----\n");

    char* c = (char*)scratchAllocator::allocate(sizeof(char));
    printf("c : 0x%p\n", (void*)c);

    int* i = (int*)scratchAllocator::allocate(sizeof(int));
    printf("i : 0x%p\n", (void*)i);

    short* s = (short*)scratchAllocator::allocate(sizeof(short));
    printf("s : 0x%p\n", (void*)s);

    long long* l = (long long*)scratchAllocator::allocate(sizeof(long long));
    printf("l : 0x%p\n", (void*)l);

    *c = 1;
    *s = 2;
    *i = 3;
    *l = 4;

    printf("c : %d\n", *c);
    printf("s : %d\n", *s);
    printf("i : %d\n", *i);
    printf("l : %lld\n", *l);

    scratchAllocator::reset();

    char* c2 = (char*)scratchAllocator::allocate(sizeof(char));
    printf("c2 : 0x%p\n", (void*)c2);

    *c2 = 5;
    printf("c2 : %d\n", *c2);

    return 0;
}