// -----------------------------------------------------------------------------
// Simple pointer that is bumped on every allocation.
// Can't deallocate - only way to reclaim memory is to reset/kill everything.
// Useful for frame based allocations for example.
class ScratchAllocator
{
public:
    static void Init()
    {
        if (!m_base)
        {
            const size_t maxSize = 1024 * 1024;  // 1mb
            m_base = (char*)malloc(maxSize);
            m_cur = m_base;
            m_end = m_base + maxSize;

            printf("ScratchAllocator::Init base:[0x%p] size:[%llu]\n", (void*)m_base, maxSize);
        }
    }

    static void Kill()
    {
        printf("ScratchAllocator::Kill size:[%llu]\n", size_t(m_cur - m_base));

        if (m_base)
        {
            free(m_base);
        }
        m_base = nullptr;
        m_cur = nullptr;
    }

    static void Reset()
    {
        printf("ScratchAllocator::Reset\n");
        m_cur = m_base;
    }

    static void* Allocate(const size_t numBytes, const size_t alignment)
    {
        if (numBytes == 0)
        {
            printf("ScratchAllocator::Allocate can't allocate 0 bytes!\n");
            return nullptr;
        }

        // Lazy init
        Init();

        // Calculate next available location that has correct alignment
        size_t curPos = (size_t)m_cur;              // Cast to size_t so we can do bit manipulation without compiler complaining
        size_t newPos = (curPos + (alignment - 1)) & ~(alignment - 1);  // Clear low bits to get an aligned position (alignment - 1 = lower bits that should be 0's for proper alignment get set, then ~ inverts and we & against curPos = aligned pos)
        if ((char*)(newPos + numBytes) >= m_end)
        {
            printf("ScratchAllocator::Allocate failed - out of space! numBytes:[%llu], alignment:[%llu], allocated:[%llu] free:[%llu]\n",
                numBytes,  alignment, size_t(m_cur - m_base), size_t(m_end - m_cur));
            return nullptr;
        }

        m_cur = (char*)newPos;                      // Cast back newly aligned position from integer
        char* newPtr = m_cur;                       // Save off this position as the allocation location to return
        m_cur += numBytes;                          // Advance internal position by the number of allocated bytes

        printf("ScratchAllocator::Allocate numBytes:[%llu], alignment:[%llu], address:[0x%p] allocated:[%llu] free:[%llu]\n",
            numBytes,  alignment, newPtr, size_t(m_cur - m_base), size_t(m_end - m_cur));

        return newPtr;
    }

    template <class T>
    static T* Allocate()
    {
        void* p = Allocate(sizeof(T), alignof(T));
        T* t = new (p) T;
        return t;
    }

    template <class T>
    static void Deallocate(T* t)
    {
        t->~T();
    }

private:
    static char* m_base;
    static char* m_cur;
    static char* m_end;
};

// -----------------------------------------------------------------------------
char* ScratchAllocator::m_base = nullptr;
char* ScratchAllocator::m_cur = nullptr;
char* ScratchAllocator::m_end = nullptr;