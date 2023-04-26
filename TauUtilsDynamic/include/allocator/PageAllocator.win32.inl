#pragma once

#ifdef _WIN32

#if defined(TAU_INDEV) || 0
#include "PageAllocator.hpp"
#endif

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

inline void PageAllocator::Init() noexcept
{
    if(!m_Initialized)
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        m_PageSize = sysInfo.dwPageSize;

        m_Initialized = true;
    }
}

inline void* PageAllocator::Reserve(const uSys numPages) noexcept
{
    return VirtualAlloc(nullptr, numPages * PageSize(), MEM_RESERVE, PAGE_NOACCESS);
}

inline void* PageAllocator::Alloc(const uSys numPages) noexcept
{
    return VirtualAlloc(nullptr, numPages * PageSize(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

inline void* PageAllocator::CommitPage(void* const page) noexcept
{
    return VirtualAlloc(page, m_PageSize, MEM_COMMIT, PAGE_READWRITE);
}

inline void* PageAllocator::CommitPages(void* const page, const uSys pageCount) noexcept
{
    return VirtualAlloc(page, pageCount * m_PageSize, MEM_COMMIT, PAGE_READWRITE);
}

inline void PageAllocator::DecommitPage(void* const page) noexcept
{
    VirtualFree(page, 1, MEM_DECOMMIT);
}

inline void PageAllocator::DecommitPages(void* const page, const uSys pageCount) noexcept
{
    VirtualFree(page, pageCount * m_PageSize, MEM_DECOMMIT);
}

inline void PageAllocator::Free(void* const page) noexcept
{
    VirtualFree(page, 0, MEM_RELEASE);
}

inline void PageAllocator::SetReadWrite(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * m_PageSize, PAGE_READWRITE, &oldProtect);
}

inline void PageAllocator::SetReadOnly(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * m_PageSize, PAGE_READONLY, &oldProtect);
}

inline void PageAllocator::SetExecute(void* const page, const uSys pageCount) noexcept
{
    DWORD oldProtect;
    VirtualProtect(page, pageCount * m_PageSize, PAGE_EXECUTE_READ, &oldProtect);
}

#endif
