#include "allocator/TauAllocator.hpp"

TAU_LIB BasicTauAllocator<AllocationTracking::None> BasicTauAllocator<AllocationTracking::None>::s_Instance;
TAU_LIB BasicTauAllocator<AllocationTracking::Count> BasicTauAllocator<AllocationTracking::Count>::s_Instance;
TAU_LIB BasicTauAllocator<AllocationTracking::DoubleDeleteCount> BasicTauAllocator<AllocationTracking::DoubleDeleteCount>::s_Instance;

extern "C" TAU_LIB void* TauUtilsAllocate(const uSys size) noexcept
{
    return ::operator new(size, ::std::nothrow);
}

extern "C" TAU_LIB void TauUtilsDeallocate(void* const obj) noexcept
{
    ::operator delete(obj, ::std::nothrow);
}
