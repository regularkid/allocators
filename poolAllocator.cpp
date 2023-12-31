// -----------------------------------------------------------------------------
// Allocator with fixed size blocks. Prevents fragmentation. Useful as a general
// purpose allocator if all of your allocations are roughly the same size OR if
// you make a "multi-pool" version with a bunch of pools of different sizes.
class PoolAllocator
{
public:
    static void Init(const size_t chunkSize, const size_t numChunks)
    {
        m_chunkSize = chunkSize;
        m_numChunks = numChunks;
        m_freeChunks = numChunks;
        m_base = malloc(chunkSize * numChunks);
        m_free = m_base;
        printf("PoolAllocator::Init baseAddress[0x%p]\n", m_base);

        Chunk* chunk = static_cast<Chunk*>(m_base);
        for (size_t i = 0; i < numChunks; ++i)
        {
            chunk->m_next = chunk + chunkSize;
            chunk = chunk->m_next;
        }
    }

    static void Kill()
    {
        printf("PoolAllocator::Kill\n");
        free(m_base);
        m_base = nullptr;
        m_free = nullptr;
    }

    template <class T>
    static T* Allocate()
    {
        if (sizeof(T) > m_chunkSize)
        {
            printf("PoolAllocator::Allocate failed - cannot allocate size:[%llu] chunkSize:[%llu]\n", sizeof(T), m_chunkSize);
            return nullptr;
        }

        if (m_freeChunks == 0)
        {
            printf("PoolAllocator::Allocate failed - cannot allocate; out of chunks\n");
            return nullptr;
        }

        void* ptr = m_free;
        m_free = static_cast<Chunk*>(m_free)->m_next;
        --m_freeChunks;
        T* t = new (ptr) T;
        printf("PoolAllocator::Allocate address:[0x%p] freeAddress[0x%p] numFreeChunks[%llu]\n", ptr, m_free, m_freeChunks);
        return t;
    }

    template <class T>
    static void Deallocate(T* ptr)
    {
        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
        chunk->m_next = reinterpret_cast<Chunk*>(m_free);
        m_free = chunk;
        ++m_freeChunks;
        ptr->~T();

        printf("PoolAllocator::Deallocate address:[0x%p] freeAddress[0x%p] numFreeChunks[%llu]\n", ptr, m_free, m_freeChunks);
    }

private:
    static void* m_base;
    static void* m_free;
    static size_t m_chunkSize;
    static size_t m_numChunks;
    static size_t m_freeChunks;

    struct Chunk
    {
        Chunk* m_next = nullptr;
    };
};

void* PoolAllocator::m_base = nullptr;
void* PoolAllocator::m_free = nullptr;
size_t PoolAllocator::m_chunkSize = 0;
size_t PoolAllocator::m_numChunks = 0;
size_t PoolAllocator::m_freeChunks = 0;