#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "TUConfig.hpp"
#include "PageAllocator.hpp"
#include <atomic>
#include <cassert>
#include <memory>
#include <bit>
#include <type_traits>

#include "TUMaths.hpp"

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

extern "C" TAU_UTILS_LIB void* TauUtilsAllocateNonConst(uSys size) noexcept;
extern "C" TAU_UTILS_LIB void TauUtilsDeallocateNonConst(void* obj) noexcept;

template<typename T>
inline constexpr void TauUtilsDestructTArr(T* const obj, const uSys elementCount) noexcept
{
    if(!obj)
    {
        return;
    }

    for(uSys i = 0; i < elementCount; ++i)
    {
        obj[i].~T();
    }
}

template<typename T, typename... Args>
constexpr T* TauUtilsAllocateT(Args&&... args) noexcept
{
    if(std::is_constant_evaluated())
    {
        return new T(::std::forward<Args>(args)...);
    }
    else
    {
        return ::new(TauUtilsAllocateNonConst(sizeof(T))) T(::std::forward<Args>(args)...);
    }
}

template<typename T>
constexpr void TauUtilsDeallocateT(T* obj) noexcept
{
    if(!obj)
    {
        return;
    }

    if(::std::is_constant_evaluated())
    {
        delete obj;
    }
    else
    {
        obj->~T();
        TauUtilsDeallocateNonConst(obj);
    }
}

template<typename T>
constexpr T* TauUtilsAllocateTArr(const uSys elementCount) noexcept
{
    if(std::is_constant_evaluated())
    {
        return new T[elementCount];
    }
    else
    {
        return ::new(TauUtilsAllocateNonConst(sizeof(T) * elementCount)) T[elementCount];
    }
}

template<typename T>
constexpr void TauUtilsDeallocateTArr(T* obj, const uSys elementCount) noexcept
{
    if(!obj)
    {
        return;
    }

    if(::std::is_constant_evaluated())
    {
        delete[] obj;
    }
    else
    {
        TauUtilsDestructTArr(obj, elementCount);
        TauUtilsDeallocateNonConst(obj);
    }
}

constexpr void* TauUtilsAllocate(uSys size) noexcept
{
    if(std::is_constant_evaluated())
    {
        return new u8[size];
    }
    else
    {
        return TauUtilsAllocateNonConst(size);
    }
}

constexpr void TauUtilsDeallocate(void* obj) noexcept
{
    if(::std::is_constant_evaluated())
    {
        delete ::std::bit_cast<u8*>(obj);
    }
    else
    {
        TauUtilsDeallocateNonConst(obj);
    }
}

#define TU_NEW(TYPE, ...) ::new(::TauUtilsAllocate(sizeof(TYPE))) TYPE(__VA_ARGS__)
#define TU_NEW_ARR(TYPE, COUNT) ::new(::TauUtilsAllocate(sizeof(TYPE) * COUNT)) TYPE[COUNT]
#define TU_NEW_C(TYPE, ...) ::TauUtilsAllocateT<TYPE>(__VA_ARGS__)
#define TU_DELETE(PTR) ::TauUtilsDeallocateT(PTR)
#define TU_DELETE_ARR(PTR) ::TauUtilsDeallocate(PTR)
#define TU_DELETE_T_ARR(PTR, COUNT) ::TauUtilsDeallocateTArr(PTR, COUNT)

class TAU_UTILS_LIB TauAllocator
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
        return ::new(allocation) T(::std::forward<Args>(args)...);
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
    { return AllocateT(::std::forward<Args>(args)...); }
    
    template<typename T>
    void deallocateT(T* const obj) noexcept
    { DeallocateT(obj); }
};

template<AllocationTracking AllocTracking = AllocationTracking::None>
class BasicTauAllocator final : public TauAllocator
{
    DEFAULT_CONSTRUCT_PI(BasicTauAllocator);
    DEFAULT_DESTRUCT(BasicTauAllocator);
    DELETE_CM(BasicTauAllocator);
public:
    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    { return ::TauUtilsAllocate(size); }

    void Deallocate(void* const obj) noexcept override
    { ::TauUtilsDeallocate(obj); }
};

template<>
class TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::None> final : public TauAllocator
{
    DEFAULT_CONSTRUCT_PI(BasicTauAllocator);
    DEFAULT_DESTRUCT(BasicTauAllocator);
    DELETE_CM(BasicTauAllocator);
public:
    static constexpr BasicTauAllocator& Instance() noexcept
    { return s_Instance; }
public:
    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    { return ::TauUtilsAllocate(size); }

    void Deallocate(void* const obj) noexcept override
    { ::TauUtilsDeallocate(obj); }
private:
    static BasicTauAllocator s_Instance;
};

template<>
class TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::Count> final : public TauAllocator
{
    DEFAULT_DESTRUCT(BasicTauAllocator);
    DELETE_CM(BasicTauAllocator);
public:
    static constexpr BasicTauAllocator& Instance() noexcept
    { return s_Instance; }
private:
    BasicTauAllocator() noexcept
        : m_Count(0)
    { }
public:
    [[nodiscard]] iSys Count() const noexcept { return m_Count; }

    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    {
        ++m_Count;
        return ::TauUtilsAllocate(size);
    }

    void Deallocate(void* const obj) noexcept override
    {
        if(m_Count == 0)
        {
            assert(m_Count != 0);
            return;
        }

        --m_Count;
        ::TauUtilsDeallocate(obj);
    }
private:
    ::std::atomic<iSys> m_Count;
private:
    static BasicTauAllocator s_Instance;
};

template<>
class TAU_UTILS_LIB BasicTauAllocator<AllocationTracking::DoubleDeleteCount> final : public TauAllocator
{
    DEFAULT_DESTRUCT(BasicTauAllocator);
    DELETE_CM(BasicTauAllocator);
public:
    static constexpr BasicTauAllocator& Instance() noexcept
    { return s_Instance; }
private:
    BasicTauAllocator() noexcept
        : m_Count(0)
    { }
public:
    [[nodiscard]] iSys Count() const noexcept { return m_Count; }

    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    {
        ++m_Count;

        void* const allocation = ::TauUtilsAllocate(size + sizeof(iSys));
        iSys* const allocationCount = ::new(allocation) iSys(1);

        return allocationCount + 1;
    }

    void Deallocate(void* const obj) noexcept override
    {
        if(m_Count == 0)
        {
            assert(m_Count != 0);
            return;
        }

        --m_Count;

        iSys* const allocationCount = static_cast<iSys*>(obj) - 1;

        if(*allocationCount == 0)
        {
            assert(*allocationCount != 0);
            return;
        }

        --(*allocationCount);
        
    }
private:
    ::std::atomic<iSys> m_Count;
private:
    static BasicTauAllocator s_Instance;
};

class TAU_UTILS_LIB PageTauAllocator final : public TauAllocator
{
    DEFAULT_CONSTRUCT_PI(PageTauAllocator);
    DEFAULT_DESTRUCT(PageTauAllocator);
    DELETE_CM(PageTauAllocator);
public:
    static PageTauAllocator& Instance() noexcept
    {
        static PageTauAllocator instance;
        return instance;
    }
public:
    [[nodiscard]] void* Allocate(const uSys size) noexcept override
    { return PageAllocator::Alloc(DivCeil(size, PageAllocator::PageSize())); }

    void Deallocate(void* const obj) noexcept override
    { ::PageAllocator::Free(obj); }
};

using DefaultTauAllocator = BasicTauAllocator<AllocationTracking::None>;
using DefaultCountingTauAllocator = BasicTauAllocator<AllocationTracking::Count>;
using DefaultDoubleDeleteTauAllocator = BasicTauAllocator<AllocationTracking::DoubleDeleteCount>;
