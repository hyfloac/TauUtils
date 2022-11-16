#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"

class PageAllocator final
{
    DELETE_CONSTRUCT(PageAllocator);
    DELETE_CM(PageAllocator);
    DELETE_DESTRUCT(PageAllocator);
private:
    static uSys _pageSize;
    static bool _initialized;
public:
    static void Init() noexcept;

    [[nodiscard]] static void* Reserve(uSys numPages) noexcept;

    /**
     * Reserves and commits the pages.
     */
    [[nodiscard]] static void* Alloc(uSys numPages) noexcept;

    static void* CommitPage(void* page) noexcept;
    static void* CommitPages(void* page, uSys pageCount) noexcept;

    static void DecommitPage(void* page) noexcept;
    static void DecommitPages(void* page, uSys pageCount) noexcept;

    static void Free(void* page) noexcept;

    static void SetReadWrite(void* page, uSys pageCount = 1) noexcept;
    static void SetReadOnly(void* page, uSys pageCount = 1) noexcept;
    static void SetExecute(void* page, uSys pageCount = 1) noexcept;

    [[nodiscard]] static uSys PageSize() noexcept;

    static void init() noexcept { Init(); }
    
    [[nodiscard]] static void* reserve(const uSys numPages) noexcept { return Reserve(numPages); }
    
    /**
     * Reserves and commits the pages.
     */
    [[nodiscard]] static void* alloc(const uSys numPages) noexcept { return Alloc(numPages); }
    
    static void* commitPage(void* const page) noexcept { return CommitPage(page); }
    static void* commitPages(void* const page, const uSys pageCount) noexcept { return CommitPages(page, pageCount); }
    
    static void decommitPage(void* const page) noexcept { DecommitPage(page); }
    static void decommitPages(void* const page, const uSys pageCount) noexcept { DecommitPages(page, pageCount); }
    
    static void free(void* const page) noexcept { Free(page); }
    
    static void setReadWrite(void* const page, const uSys pageCount = 1) noexcept { SetReadWrite(page, pageCount); }
    static void setReadOnly(void* const page, const uSys pageCount = 1) noexcept { SetReadOnly(page, pageCount); }
    static void setExecute(void* const page, const uSys pageCount = 1) noexcept { SetExecute(page, pageCount); }
    
    [[nodiscard]] static uSys pageSize() noexcept { return PageSize(); }
};

#include "PageAllocator.win32.inl"
