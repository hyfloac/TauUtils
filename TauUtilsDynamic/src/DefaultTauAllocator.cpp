#include "allocator/TauAllocator.hpp"
#include <type_traits>
#include <bit>

TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::None> BasicTauAllocator<AllocationTracking::None>::s_Instance;
TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::Count> BasicTauAllocator<AllocationTracking::Count>::s_Instance;
TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::DoubleDeleteCount> BasicTauAllocator<AllocationTracking::DoubleDeleteCount>::s_Instance;

extern "C" TAU_UTILS_LIB void* TauUtilsAllocateNonConst(const uSys size) noexcept
{
    return ::operator new(size, ::std::nothrow);
}

extern "C" TAU_UTILS_LIB void TauUtilsDeallocateNonConst(void* const obj) noexcept
{
    ::operator delete(obj, ::std::nothrow);
}
