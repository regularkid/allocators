#include "jumboIncludes.h"

void TestScratchAllocator()
{
    printf("----- Scratch Allocator Tests -----\n");

    // Allocate basic types + class type (verify constructor called). Prints will verify address + alignments
    char* c = ScratchAllocator::Allocate<char>();
    int* i = ScratchAllocator::Allocate<int>();
    short* s = ScratchAllocator::Allocate<short>();
    long long* l = ScratchAllocator::Allocate<long long>();
    Thing* t = ScratchAllocator::Allocate<Thing>();

    // Test using memory that was allocated
    *c = 1; printf("c : %d\n", *c);
    *s = 2; printf("s : %d\n", *s);
    *i = 3; printf("i : %d\n", *i);
    *l = 4; printf("l : %lld\n", *l);
    t->m_i = 123; printf("t->m_i : %d\n", t->m_i);    

    // Deallocate class type (verify destructor called)
    ScratchAllocator::Deallocate<Thing>(t);

    // Reset and allocate again (verify address used is back at beginning of allocator buffer)
    ScratchAllocator::Reset();
    char* c2 = ScratchAllocator::Allocate<char>();
    *c2 = 5; printf("c2 : %d\n", *c2);

    // Test out of memory error
    void* big = ScratchAllocator::Allocate(1048574, alignof(char)); (void)big;
    void* oom = ScratchAllocator::Allocate(2, alignof(char)); (void)oom;

    // Test shutdown / freeing of internal mem buffer
    ScratchAllocator::Kill();
}

void TestPoolAllocator()
{
    printf("----- Pool Allocator Tests -----\n");

    const size_t chunkSize = 32;
    const size_t numChunks = 10;
    PoolAllocator::Init(chunkSize, numChunks);

    // Allocate basic types + class type (verify constructor called). Prints will verify address + alignments
    char* c = PoolAllocator::Allocate<char>();
    int* i = PoolAllocator::Allocate<int>();
    short* s = PoolAllocator::Allocate<short>();
    long long* l = PoolAllocator::Allocate<long long>();
    Thing* t = PoolAllocator::Allocate<Thing>();

    // Test using memory that was allocated
    *c = 1; printf("c : %d\n", *c);
    *s = 2; printf("s : %d\n", *s);
    *i = 3; printf("i : %d\n", *i);
    *l = 4; printf("l : %lld\n", *l);
    t->m_i = 123; printf("t->m_i : %d\n", t->m_i);    

    // Deallocate class type (verify destructor called)
    PoolAllocator::Deallocate<Thing>(t);

    // Deallocate and verify that new allocations take their spots
    PoolAllocator::Deallocate<char>(c);
    PoolAllocator::Deallocate<short>(s);
    int* i2 = PoolAllocator::Allocate<int>(); (void)i2;
    int* i3 = PoolAllocator::Allocate<int>(); (void)i3;
    int* i4 = PoolAllocator::Allocate<int>(); (void)i4;
    int* i5 = PoolAllocator::Allocate<int>(); (void)i5;
    int* i6 = PoolAllocator::Allocate<int>(); (void)i6;
    int* i7 = PoolAllocator::Allocate<int>(); (void)i7;
    int* i8 = PoolAllocator::Allocate<int>(); (void)i8;
    int* i9 = PoolAllocator::Allocate<int>(); (void)i9;
    int* ia = PoolAllocator::Allocate<int>(); (void)ia;

    class TooBig
    {
        int m_a[9];
    };
    TooBig* tb = PoolAllocator::Allocate<TooBig>(); (void)tb;

    // Test shutdown / freeing of internal mem buffer
    ScratchAllocator::Kill();
}

int main()
{
    TestScratchAllocator();
    TestPoolAllocator();

    return 0;
}