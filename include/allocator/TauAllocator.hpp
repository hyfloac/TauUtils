#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"

namespace TauAllocatorUtils {
template<typename T>
struct RemoveReference final
{ using type = T; };

template<typename T>
struct RemoveReference<T&> final
{ using type = T; };

template<typename T>
struct RemoveReference<T&&> final
{ using type = T; };

template<typename T>
using RemoveReferenceT = typename RemoveReference<T>::type;

template<typename T>
[[nodiscard]] constexpr inline T&& _forward(RemoveReferenceT<T>& ref) noexcept
{ return static_cast<T&&>(ref); }

template<typename T>
[[nodiscard]] constexpr inline T&& _forward(RemoveReferenceT<T>&& ref) noexcept
{ return static_cast<T&&>(ref); }

template<typename T>
[[nodiscard]] constexpr RemoveReferenceT<T>&& _move(T&& arg) noexcept
{ return static_cast<RemoveReferenceT<T>&&>(arg); }
}

enum class AllocationAlignment : uSys { };
enum class PageCountVal : uSys { };

enum class AllocationTracking
{
    /**
     * Doesn't perform any allocation tracking.
     */
    None,
    /**
     *   Tracks every time an object is allocated and deallocated.
     * This doesn't track the individual allocations of any given
     * object, but instead keeps track of the number of
     * allocations. Performing a double delete is not tracked.
     */
    Count,
    /**
     *   Tracks every time an object is allocated and deallocated.
     * This also tracks any time an object is double deleted. A
     * double deletion doesn't count towards the deallocation
     * count.
     */
    DoubleDeleteCount
};

class TauAllocator
{
    DEFAULT_CONSTRUCT_PO(TauAllocator);
    DEFAULT_DESTRUCT_VI(TauAllocator);
    DELETE_CM(TauAllocator);
public:
    [[nodiscard]] virtual void* allocate(uSys size) noexcept = 0;

    virtual void deallocate(void* obj) noexcept = 0;

    template<typename T, typename... Args>
    T* allocateT(Args&&... args) noexcept
    {
        void* const allocation = allocate(sizeof(T));
        if(!allocation) { return nullptr; }
        return new(allocation) T(TauAllocatorUtils::_forward<Args>(args)...);
    }

    template<typename T>
    void deallocateT(T* const obj) noexcept
    {
        if(!obj) { return; }
        obj->~T();
        deallocate(obj);
    }
};

class DefaultTauAllocator final : public TauAllocator
{
    DEFAULT_CONSTRUCT_PI(DefaultTauAllocator);
    DEFAULT_DESTRUCT(DefaultTauAllocator);
    DELETE_CM(DefaultTauAllocator);
public:
    static DefaultTauAllocator& Instance() noexcept;
public:
    [[nodiscard]] void* allocate(const uSys size) noexcept override
    { return operator new(size); }

    void deallocate(void* const obj) noexcept override
    { operator delete(obj); }
};
