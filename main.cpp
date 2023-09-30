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

    // Reset and allocate again
    ScratchAllocator::Reset();
    char* c2 = ScratchAllocator::Allocate<char>();
    *c2 = 5; printf("c2 : %d\n", *c2);

    // Test out of memory erro
    void* big = ScratchAllocator::Allocate(1048574, alignof(char)); (void)big;
    void* oom = ScratchAllocator::Allocate(2, alignof(char)); (void)oom;

    // Test shutdown / freeing of internal mem buffer
    ScratchAllocator::Kill();
}

int main()
{
    TestScratchAllocator();

    return 0;
}