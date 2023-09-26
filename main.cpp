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
class allocator
{
public:
    static void* allocate(const size_t numBytes)
    {
        printf("allcator::allocate\n");
        return malloc(numBytes);
    }

    static void deallocate(void* ptr)
    {
        printf("allocator::deallocate\n");
        free(ptr);
    }
};

// -----------------------------------------------------------------------------
class thing
{
public:
    void* operator new(size_t numBytes)
    {
        return allocator::allocate(numBytes);
    }

    void operator delete(void* ptr)
    {
        allocator::deallocate(ptr);
    }
};

int main()
{
    printf("----- allocators -----\n");

    int* a = new int;
    delete a;

    int* b = new int[5];
    delete[] b;

    thing* t = new thing();
    delete t;

    return 0;
}