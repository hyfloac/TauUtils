#pragma once

#include <new>

#include "NumTypes.hpp"

class ReferenceCounter final
{
public:
    using Type = iSys;
private:
    Type* m_RefCount;
public:
    ReferenceCounter() noexcept
        : m_RefCount(new(::std::nothrow) Type(1))
    { }

    explicit ReferenceCounter(Type* const refCount) noexcept
        : m_RefCount(refCount)
    { ++(*m_RefCount); }

    ReferenceCounter(nullptr_t) noexcept
        : m_RefCount(nullptr)
    { }

    ~ReferenceCounter() noexcept
    { OnDestroy(); }

    ReferenceCounter(const ReferenceCounter& copy) noexcept
        : m_RefCount(copy.m_RefCount)
    { OnCopy(); }

    ReferenceCounter(ReferenceCounter&& move) noexcept
        : m_RefCount(move.m_RefCount)
    { move.m_RefCount = nullptr; }

    ReferenceCounter& operator=(const ReferenceCounter& copy) noexcept
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

    ReferenceCounter& operator=(ReferenceCounter&& move) noexcept
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

    [[nodiscard]] Type RefCount() const noexcept { return m_RefCount ? *m_RefCount : 0; }
    [[nodiscard]] const Type* RefCountPtr() const noexcept { return m_RefCount; }
private:
    inline void OnCopy() noexcept
    {
        if(m_RefCount)
        {
            ++(*m_RefCount);
        }
    }

    inline void OnDestroy() noexcept
    {
        if(m_RefCount && --(*m_RefCount) <= 0)
        {
            delete m_RefCount;
        }
    }
};
