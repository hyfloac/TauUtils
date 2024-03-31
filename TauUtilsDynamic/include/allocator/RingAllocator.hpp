#pragma once

#include "TauAllocator.hpp"

namespace tau::allocator {

template<AllocationAlignment Alignment = AllocationAlignment{ 1 }>
class TAU_UTILS_LIB RingAllocator final : public TauAllocator
{
    DEFAULT_DESTRUCT(RingAllocator);
    DELETE_CM(RingAllocator);
public:
    RingAllocator(const uSys elementSize, const uSys elementCount, TauAllocator& allocator) noexcept
        : m_WriteIndex(0)
        , m_PoolElementCount(elementCount)
        , m_ElementSize(AlignToAny<Alignment>(elementSize))
        , m_PoolByteCount(m_ElementSize * elementCount)
        , m_RingPool(allocator.Allocate(m_PoolByteCount))
        , m_Allocator(&allocator)
    { }

    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    {
        if(size > m_ElementSize)
        {
            return nullptr;
        }

        if(m_WriteIndex >= m_PoolElementCount)
        {
            m_WriteIndex = 0;
        }

        const uSys writeIndexTmp = m_WriteIndex;
        ++m_WriteIndex;

        return static_cast<u8*>(m_RingPool) + (m_ElementSize * writeIndexTmp);
    }

    void Deallocate(void* const obj) noexcept override
    { }
private:
    uSys m_WriteIndex;
    uSys m_PoolElementCount;
    uSys m_ElementSize;
    uSys m_PoolByteCount;
    void* m_RingPool;
    TauAllocator* m_Allocator;
};

}
