#pragma once

#include "NumTypes.hpp"
#include "allocator/TauAllocator.hpp"

class ReferenceCounter final
{
public:
    using Type = iSys;
private:
    Type* m_RefCount;
public:
    constexpr ReferenceCounter() noexcept
        : m_RefCount(TU_NEW_C(Type, 1))
    { }

    explicit constexpr ReferenceCounter(Type* const refCount) noexcept
        : m_RefCount(refCount)
    { ++(*m_RefCount); }

    constexpr ReferenceCounter(::std::nullptr_t) noexcept
        : m_RefCount(nullptr)
    { }

    constexpr ~ReferenceCounter() noexcept
    { OnDestroy(); }

    constexpr ReferenceCounter(const ReferenceCounter& copy) noexcept
        : m_RefCount(copy.m_RefCount)
    { OnCopy(); }

    constexpr ReferenceCounter(ReferenceCounter&& move) noexcept
        : m_RefCount(move.m_RefCount)
    { move.m_RefCount = nullptr; }

    constexpr ReferenceCounter& operator=(const ReferenceCounter& copy) noexcept
    {
        if(this == &copy)
        {
            return *this;
        }

        OnDestroy();

        m_RefCount = copy.m_RefCount;

        OnCopy();

        return *this;
    }

    constexpr ReferenceCounter& operator=(ReferenceCounter&& move) noexcept
    {
        if(this == &move)
        {
            return *this;
        }

        OnDestroy();

        m_RefCount = move.m_RefCount;

        move.m_RefCount = nullptr;

        return *this;
    }

    [[nodiscard]] constexpr Type RefCount() const noexcept { return m_RefCount ? *m_RefCount : 0; }
    [[nodiscard]] constexpr const Type* RefCountPtr() const noexcept { return m_RefCount; }

    [[nodiscard]] constexpr bool IsAboutToBeDestroyed() const noexcept { return RefCount() == 1; }
private:
    inline constexpr void OnCopy() const noexcept
    {
        if(m_RefCount)
        {
            ++(*m_RefCount);
        }
    }

    inline constexpr void OnDestroy() const noexcept
    {
        if(m_RefCount && --(*m_RefCount) <= 0)
        {
            TU_DELETE(m_RefCount);
        }
    }
};
