#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"

namespace TauAllocatorUtils {

template<typename T>
struct RemoveReference final
{ using Type = T; };

template<typename T>
struct RemoveReference<T&> final
{ using Type = T; };

template<typename T>
struct RemoveReference<T&&> final
{ using Type = T; };

template<typename T>
struct RemoveConst final
{ using Type = T; };

template<typename T>
struct RemoveConst<const T> final
{ using Type = T; };

template<typename T>
struct RemoveVolatile final
{ using Type = T; };

template<typename T>
struct RemoveVolatile<const T> final
{ using Type = T; };

template<typename T>
struct RemoveCV final
{ using Type = T; };

template<typename T>
struct RemoveCV<const T> final
{ using Type = T; };

template<typename T>
struct RemoveCV<volatile T> final
{ using Type = T; };

template<typename T>
struct RemoveCV<const volatile T> final
{ using Type = T; };

template<typename T>
using RemoveReferenceT = typename RemoveReference<T>::Type;
template<typename T>
using RemoveConstT = typename RemoveConst<T>::Type;
template<typename T>
using RemoveVolatileT = typename RemoveVolatile<T>::Type;
template<typename T>
using RemoveCVT = typename RemoveCV<T>::Type;

template<typename T>
[[nodiscard]] constexpr inline T&& Forward(RemoveReferenceT<T>& ref) noexcept
{ return static_cast<T&&>(ref); }

template<typename T>
[[nodiscard]] constexpr inline T&& Forward(RemoveReferenceT<T>&& ref) noexcept
{ return static_cast<T&&>(ref); }

template<typename T>
[[nodiscard]] constexpr RemoveReferenceT<T>&& Move(T&& arg) noexcept
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
    [[nodiscard]] virtual void* Allocate(uSys size) noexcept = 0;

    virtual void Deallocate(void* obj) noexcept = 0;

    template<typename T, typename... Args>
    [[nodiscard]] T* AllocateT(Args&&... args) noexcept
    {
        void* const allocation = Allocate(sizeof(T));
        if(!allocation) { return nullptr; }
        return new(allocation) T(TauAllocatorUtils::Forward<Args>(args)...);
    }

    template<typename T>
    void DeallocateT(T* const obj) noexcept
    {
        if(!obj) { return; }
        obj->~T();
        Deallocate(obj);
    }

    [[nodiscard]] void* allocate(const uSys size) noexcept { return Allocate(size); }
    void deallocate(void* const obj) noexcept { return Deallocate(obj); }
    
    template<typename T, typename... Args>
    [[nodiscard]] T* allocateT(Args&&... args) noexcept
    { return AllocateT(TauAllocatorUtils::Forward<Args>(args)...); }
    
    template<typename T>
    void deallocateT(T* const obj) noexcept
    { DeallocateT(obj); }
};

class DefaultTauAllocator final : public TauAllocator
{
    DEFAULT_CONSTRUCT_PI(DefaultTauAllocator);
    DEFAULT_DESTRUCT(DefaultTauAllocator);
    DELETE_CM(DefaultTauAllocator);
public:
    static constexpr DefaultTauAllocator& Instance() noexcept
    { return instance; }
private:
    static DefaultTauAllocator instance;
public:
    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    { return operator new(size); }

    void Deallocate(void* const obj) noexcept override
    { operator delete(obj); }
};
