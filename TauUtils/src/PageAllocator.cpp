#include "allocator/PageAllocator.hpp"

#ifdef _WIN32

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

bool PageAllocator::_initialized = false;
uSys PageAllocator::_pageSize = 0;

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

void* PageAllocator::Reserve(const uSys numPages) noexcept
{
    return VirtualAlloc(nullptr, numPages * PageSize(), MEM_RESERVE, PAGE_NOACCESS);
}

void* PageAllocator::Alloc(const uSys numPages) noexcept
{
    return VirtualAlloc(nullptr, numPages * PageSize(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void* PageAllocator::CommitPage(void* const page) noexcept
{
    return VirtualAlloc(page, _pageSize, MEM_COMMIT, PAGE_READWRITE);
}

void* PageAllocator::CommitPages(void* const page, const uSys pageCount) noexcept
{
    return VirtualAlloc(page, pageCount * _pageSize, MEM_COMMIT, PAGE_READWRITE);
}

void PageAllocator::DecommitPage(void* const page) noexcept
{
    VirtualFree(page, 1, MEM_DECOMMIT);
}

void PageAllocator::DecommitPages(void* const page, const uSys pageCount) noexcept
{
    VirtualFree(page, pageCount * _pageSize, MEM_DECOMMIT);
}

void PageAllocator::Free(void* const page) noexcept
{
    VirtualFree(page, 0, MEM_RELEASE);
}

void PageAllocator::SetReadWrite(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * _pageSize, PAGE_READWRITE, &oldProtect);
}

void PageAllocator::SetReadOnly(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * _pageSize, PAGE_READONLY, &oldProtect);
}

void PageAllocator::SetExecute(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * _pageSize, PAGE_EXECUTE_READ, &oldProtect);
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