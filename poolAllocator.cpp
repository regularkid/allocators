// -----------------------------------------------------------------------------
// Allocator with fixed size blocks. Prevents fragmentation. Useful as a general
// purpose allocator if all of your allocations are roughly the same size OR if
// you make a "multi-pool" version with a bunch of pools of different sizes.
class PoolAllocator
{
public:
    static void Init()
    {

    }

    static void Kill()
    {

    }

    template <class T>
    static T* Allocate()
    {
        void* ptr = nullptr;
        T* t = new (ptr) T;
        return t;
    }

    template <class T>
    static void Deallocate(T* ptr)
    {
        ptr->~T();
    }

private:
};