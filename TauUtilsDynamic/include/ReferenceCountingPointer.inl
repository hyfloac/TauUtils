#pragma once

#pragma warning(push, 0)
#include <memory>
#pragma warning(pop)

#if defined(TAU_INDEV) || 0
#include "ReferenceCountingPointer.hpp"
#endif

namespace ReferenceCountingPointerUtils {

template<typename T>
template<typename... Args>
inline ReferenceCountDataObject<T>::ReferenceCountDataObject(TauAllocator& allocator, Args&&... args) noexcept
    : m_RefCount(1)
    , m_Allocator(allocator)
{ (void) ::new(this + 1) Type(::std::forward<Args>(args)...); }

template<typename T>
inline constexpr ReferenceCountDataObject<T>::ReferenceCountDataObject(::std::nullptr_t) noexcept
    : m_RefCount(0)
    , m_Allocator(DefaultTauAllocator::Instance())
{ }

template<typename T>
inline ReferenceCountDataObject<T>::~ReferenceCountDataObject() noexcept
{
    if(this != RCDOCast<T>(&NullRCDO))
    {
        reinterpret_cast<Type*>(this + 1)->~Type();
    }
}

template<typename T>
inline constexpr ReferenceCountDataObject<T>* ReferenceCountDataObject<T>::Null() noexcept
{ return RCDOCast<T>(&NullRCDO); }

template<typename T>
template<typename... Args>
inline SWReferenceCount<T>::SWReferenceCount(TauAllocator& allocator, Args&&... args) noexcept
    : m_StrongRefCount(1)
    , m_WeakRefCount(0)
    , m_Allocator(allocator)
{ (void) ::new(this + 1) T(::std::forward<Args>(args)...); }

template<typename T>
inline constexpr SWReferenceCount<T>::SWReferenceCount(::std::nullptr_t) noexcept
    : m_StrongRefCount(0)
    , m_WeakRefCount(0)
    , m_Allocator(DefaultTauAllocator::Instance())
{ }

template<typename T>
inline constexpr SWReferenceCount<T>* SWReferenceCount<T>::Null() noexcept
{ return SWRCCast<T>(&NullSWRC); }

template<typename ToT, typename FromT>
inline constexpr const ReferenceCountDataObject<ToT>& RCDOCast(const ReferenceCountDataObject<FromT>& obj) noexcept
{ return reinterpret_cast<const ReferenceCountDataObject<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline constexpr ReferenceCountDataObject<ToT>& RCDOCast(ReferenceCountDataObject<FromT>& obj) noexcept
{ return reinterpret_cast<ReferenceCountDataObject<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline constexpr ReferenceCountDataObject<ToT>&& RCDOCast(ReferenceCountDataObject<FromT>&& obj) noexcept
{ return reinterpret_cast<ReferenceCountDataObject<ToT>&&>(obj); }

template<typename ToT, typename FromT>
inline constexpr const ReferenceCountDataObject<ToT>* RCDOCast(const ReferenceCountDataObject<FromT>* obj) noexcept
{ return reinterpret_cast<const ReferenceCountDataObject<ToT>*>(obj); }

template<typename ToT, typename FromT>
inline constexpr ReferenceCountDataObject<ToT>* RCDOCast(ReferenceCountDataObject<FromT>* obj) noexcept
{ return reinterpret_cast<ReferenceCountDataObject<ToT>*>(obj); }

template<typename ToT, typename FromT>
inline constexpr const SWReferenceCount<ToT>& SWRCCast(const SWReferenceCount<FromT>& obj) noexcept
{ return reinterpret_cast<const SWReferenceCount<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline constexpr SWReferenceCount<ToT>& SWRCCast(SWReferenceCount<FromT>& obj) noexcept
{ return reinterpret_cast<SWReferenceCount<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline constexpr SWReferenceCount<ToT>&& SWRCCast(SWReferenceCount<FromT>&& obj) noexcept
{ return reinterpret_cast<SWReferenceCount<ToT>&&>(obj); }

template<typename ToT, typename FromT>
inline constexpr const SWReferenceCount<ToT>* SWRCCast(const SWReferenceCount<FromT>* obj) noexcept
{ return reinterpret_cast<const SWReferenceCount<ToT>*>(obj); }

template<typename ToT, typename FromT>
inline constexpr SWReferenceCount<ToT>* SWRCCast(SWReferenceCount<FromT>* obj) noexcept
{ return reinterpret_cast<SWReferenceCount<ToT>*>(obj); }

}

template<typename T>
inline constexpr uSys ReferenceCountingPointer<T>::AllocSize() noexcept
{ return sizeof(RCDO<T>) + sizeof(typename RCDO<T>::Type); }

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
ReferenceCountingPointer<T>::ReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept
    : m_RCDO(nullptr)
{
    void* const raw = allocator.Allocate(AllocSize());
    m_RCDO = ::new(raw) RCDO<T>(allocator, ::std::forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
ReferenceCountingPointer<T>::ReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : m_RCDO(nullptr)
{
    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    m_RCDO = ::new(raw) RCDO<T>(DefaultTauAllocator::Instance(), ::std::forward<Arg0>(arg0), ::std::forward<Args>(args)...);
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_RCDO(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount)
    {
        m_RCDO->AddRef();
    }
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_RCDO(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount)
    {
        m_RCDO->AddRef();
    }
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , m_RCDO(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{ rcp._getBlock() = RCDO<TT>::Null(); }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer(const ::std::nullptr_t) noexcept
    : m_RCDO(RCDO<T>::Null())
{ }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer() noexcept
    : m_RCDO(RCDO<T>::Null())
{ }

template<typename T>
inline ReferenceCountingPointer<T>::~ReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }
}

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<T>& dumbCopy) noexcept
    : TReferenceCountingPointerBase<T>(dumbCopy)
    , m_RCDO(dumbCopy.m_RCDO)
{ m_RCDO->AddRef(); }

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , m_RCDO(copy.m_RCDO)
{ m_RCDO->AddRef(); }

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , m_RCDO(move.m_RCDO)
{ move.m_RCDO = RCDO<T>::Null(); }

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ::std::nullptr_t) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    m_RCDO = RCDO<T>::Null();

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_RCDO = copy.m_RCDO;

    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount)
    {
        m_RCDO->AddRef();
    }

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    m_RCDO = move.m_RCDO;

    move.m_RCDO = RCDO<T>::Null();

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_RCDO = reinterpret_cast<RCDO<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount)
    {
        m_RCDO->AddRef();
    }

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    m_RCDO = reinterpret_cast<RCDO<T>*>(move._getBlock());

    move._getBlock() = RCDO<TT>::Null();

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void ReferenceCountingPointer<T>::Reset(Allocator& allocator, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    void* const raw = allocator.Allocate(AllocSize());
    m_RCDO = ::new(raw) RCDO<T>(allocator, ::std::forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void ReferenceCountingPointer<T>::Reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && m_RCDO->Release() == 0)
    { m_RCDO->m_Allocator.DeallocateT(m_RCDO); }

    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    m_RCDO = ::new(raw) RCDO<T>(DefaultTauAllocator::Instance(), ::std::forward<Arg0>(arg0), ::std::forward<Args>(args)...);
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator==(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && ptr)
    { return m_RCDO->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(m_RCDO) || !m_RCDO->m_RefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator!=(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_RCDO) && m_RCDO->m_RefCount && ptr)
    { return m_RCDO->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(m_RCDO) && !m_RCDO->m_RefCount) && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys StrongReferenceCountingPointer<T>::AllocSize() noexcept
{ return sizeof(SWRC<T>) + sizeof(typename SWRC<T>::Type); }

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept
    : m_SWRC(nullptr)
{
    void* const raw = allocator.Allocate(AllocSize());
    m_SWRC = ::new(raw) SWRC<T>(allocator, ::std::forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : m_SWRC(nullptr)
{
    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    m_SWRC = ::new(raw) SWRC<T>(DefaultTauAllocator::Instance(), ::std::forward<Arg0>(arg0), ::std::forward<Args>(args)...);
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{ rcp._getBlock() = SWRC<TT>::Null(); }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const ::std::nullptr_t) noexcept
    : m_SWRC(SWRC<T>::Null())
{ }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer() noexcept
    : m_SWRC(SWRC<T>::Null())
{ }

template<typename T>
inline StrongReferenceCountingPointer<T>::~StrongReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>& dumbCopy) noexcept
    : TReferenceCountingPointerBase<T>(dumbCopy)
    , m_SWRC(dumbCopy.m_SWRC)
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , m_SWRC(copy.m_SWRC)
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , m_SWRC(move.m_SWRC)
{ move.m_SWRC = SWRC<T>::Null(); }

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator=(copy);

    m_SWRC = copy.m_SWRC;

    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount <= 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator=(::std::move(move));

    m_SWRC = move.m_SWRC;

    move.m_SWRC = SWRC<T>::Null();

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const ::std::nullptr_t) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount <= 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.DeallocateT(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    m_SWRC = SWRC<T>::Null();

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.DeallocateT(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_SWRC = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefStrong(); }

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.DeallocateT(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    m_SWRC = reinterpret_cast<SWRC<T>*>(move._getBlock());

    move._getBlock() = SWRC<TT>::Null();

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        m_SWRC = copy.m_SWRC;

        if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
        { m_SWRC->AddRefStrong(); }
    }
    else
    {
        m_SWRC = SWRC<T>::Null();
    }

    return *this;
}

template <typename T>
template <typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        m_SWRC = reinterpret_cast<SWRC<T>*>(copy._getBlock());

        if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
        { m_SWRC->AddRefStrong(); }
    }
    else
    {
        m_SWRC = SWRC<T>::Null();
    }

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void StrongReferenceCountingPointer<T>::Reset(Allocator& allocator, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.DeallocateT(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    void* const raw = allocator.Allocate(AllocSize());
    m_SWRC = ::new(raw) SWRC<T>(allocator, ::std::forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void StrongReferenceCountingPointer<T>::Reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount == 1)
    {
        m_SWRC->DestroyObj();
        m_SWRC->ReleaseStrong();
        if(!m_SWRC->m_WeakRefCount)
        { m_SWRC->m_Allocator.DeallocateT(m_SWRC); }
    }
    else if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount > 1)
    { m_SWRC->ReleaseStrong(); }

    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    m_SWRC = ::new(raw) SWRC<T>(DefaultTauAllocator::Instance(), ::std::forward<Arg0>(arg0), ::std::forward<Args>(args)...);
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return m_SWRC == ptr.m_SWRC; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return m_SWRC != ptr.m_SWRC; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return m_SWRC == ptr.m_SWRC; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return m_SWRC != ptr.m_SWRC; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount && ptr)
    { return m_SWRC->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(m_SWRC) || !m_SWRC->m_StrongRefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount && ptr)
    { return m_SWRC->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(m_SWRC) || !m_SWRC->m_StrongRefCount) && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys WeakReferenceCountingPointer<T>::AllocSize() noexcept
{ return sizeof(SWRC<T>) + sizeof(typename SWRC<T>::Type); }

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<T>& ptr) noexcept
    : TReferenceCountingPointerBase<T>(ptr)
    , m_SWRC(ptr.m_SWRC)
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , m_SWRC(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{ rcp._getBlock() = SWRC<TT>::Null(); }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const ::std::nullptr_t) noexcept
    : m_SWRC(SWRC<T>::Null())
{ }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer() noexcept
    : m_SWRC(SWRC<T>::Null())
{ }

template<typename T>
inline WeakReferenceCountingPointer<T>::~WeakReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , m_SWRC(copy.m_SWRC)
{
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , m_SWRC(move.m_SWRC)
{ move.m_SWRC = SWRC<T>::Null(); }

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_SWRC = copy.m_SWRC;
    
    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    m_SWRC = move.m_SWRC;

    move.m_SWRC = SWRC<T>::Null();

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_SWRC = copy.m_SWRC;

    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_SWRC = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    m_SWRC = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(m_SWRC) && (m_SWRC->m_StrongRefCount || m_SWRC->m_WeakRefCount))
    { m_SWRC->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(m_SWRC) && m_SWRC->ReleaseWeak() == 0 && !m_SWRC->m_StrongRefCount)
    { m_SWRC->m_Allocator.Deallocate(m_SWRC); }

    TReferenceCountingPointerBase<T>::operator=(::std::move(move));

    m_SWRC = reinterpret_cast<SWRC<T>*>(move._getBlock());

    move._getBlock() = SWRC<TT>::Null();

    return *this;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator==(const T* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount && ptr)
    { return m_SWRC->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(m_SWRC) || !m_SWRC->m_StrongRefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator!=(const T* const ptr) const noexcept
{
    if(TAU_CHECK_RC(m_SWRC) && m_SWRC->m_StrongRefCount && ptr)
    { return m_SWRC->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(m_SWRC) || !m_SWRC->m_StrongRefCount) && !ptr)
    { return false; }
    return true;
}
