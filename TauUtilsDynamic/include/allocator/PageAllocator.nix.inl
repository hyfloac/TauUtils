#pragma once

#if defined(__APPLE__) || defined(__linux__)

#if defined(TAU_INDEV) || 1
#include "PageAllocator.hpp"
#endif

#include <sys/mman.h>
#include <unistd.h>

inline void PageAllocator::Init() noexcept
{
    if(!m_Initialized)
    {
        m_PageSize = static_cast<uSys>(getpagesize());

        m_Initialized = true;
    }
}

inline void* PageAllocator::Reserve(const uSys numPages) noexcept
{
    return mmap(nullptr, numPages * PageSize(), PROT_NONE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

inline void* PageAllocator::Alloc(const uSys numPages) noexcept
{
    return mmap(nullptr, numPages * PageSize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

inline void* PageAllocator::CommitPage(void* const page) noexcept
{
    (void) mprotect(page, m_PageSize, PROT_READ | PROT_WRITE);
    return page;
}

inline void* PageAllocator::CommitPages(void* const page, const uSys pageCount) noexcept
{
    (void) mprotect(page, pageCount * m_PageSize, PROT_READ | PROT_WRITE);
    return page;
}

inline void PageAllocator::DecommitPage(void* const page) noexcept
{
    (void) mprotect(page, m_PageSize, PROT_NONE);
}

inline void PageAllocator::DecommitPages(void* const page, const uSys pageCount) noexcept
{
    (void) mprotect(page, pageCount * m_PageSize, PROT_NONE);
}

inline void PageAllocator::Free(void* const page, const uSys pageCount) noexcept
{
    (void) munmap(page, pageCount * m_PageSize);
}

inline void PageAllocator::SetReadWrite(void* const page, const uSys pageCount) noexcept
{
    (void) mprotect(page, pageCount * m_PageSize, PROT_READ | PROT_WRITE);
}

inline void PageAllocator::SetReadOnly(void* const page, const uSys pageCount) noexcept
{
    (void) mprotect(page, pageCount * m_PageSize, PROT_READ);
}

inline void PageAllocator::SetExecute(void* const page, const uSys pageCount) noexcept
{
    (void) mprotect(page, pageCount * m_PageSize, PROT_READ | PROT_EXEC);
}

#endif
