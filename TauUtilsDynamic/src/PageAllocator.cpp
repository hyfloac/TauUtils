#include "allocator/PageAllocator.hpp"

bool PageAllocator::_initialized = false;
uSys PageAllocator::_pageSize = 0;

#ifdef _WIN32

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

void PageAllocator::Init() noexcept
{
    if(!_initialized)
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        _pageSize = sysInfo.dwPageSize;

        _initialized = true;
    }
}


uSys PageAllocator::PageSize() noexcept
{
    /*   Screw it, I'm tired of dealing with problems of this value
       not being initialized. */
    if(!_initialized)
    { Init(); }
    return _pageSize;
}

#endif
