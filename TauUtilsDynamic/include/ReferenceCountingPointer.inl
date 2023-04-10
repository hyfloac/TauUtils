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
{ (void) ::new(this + 1) Type(TauAllocatorUtils::Forward<Args>(args)...); }

template<typename T>
inline constexpr ReferenceCountDataObject<T>::ReferenceCountDataObject(nullptr_t) noexcept
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
{ (void) ::new(this + 1) T(TauAllocatorUtils::Forward<Args>(args)...); }

template<typename T>
inline constexpr SWReferenceCount<T>::SWReferenceCount(nullptr_t) noexcept
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
    : _rcdo(nullptr)
{
    void* const raw = allocator.Allocate(AllocSize());
    _rcdo = ::new(raw) RCDO<T>(allocator, TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
ReferenceCountingPointer<T>::ReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : _rcdo(nullptr)
{
    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    _rcdo = ::new(raw) RCDO<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::Forward<Arg0>(arg0), TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _rcdo(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount)
    {
        _rcdo->AddRef();
    }
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _rcdo(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount)
    {
        _rcdo->AddRef();
    }
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _rcdo(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
{ rcp._getBlock() = RCDO<TT>::Null(); }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer(const nullptr_t) noexcept
    : _rcdo(RCDO<T>::Null())
{ }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer() noexcept
    : _rcdo(RCDO<T>::Null())
{ }

template<typename T>
inline ReferenceCountingPointer<T>::~ReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }
}

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<T>& dumbCopy) noexcept
    : TReferenceCountingPointerBase<T>(dumbCopy)
    , _rcdo(dumbCopy._rcdo)
{ _rcdo->AddRef(); }

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _rcdo(copy._rcdo)
{ _rcdo->AddRef(); }

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _rcdo(move._rcdo)
{ move._rcdo = RCDO<T>::Null(); }

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const nullptr_t) noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    _rcdo = RCDO<T>::Null();

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _rcdo = copy._rcdo;

    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount)
    {
        _rcdo->AddRef();
    }

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _rcdo = move._rcdo;

    move._rcdo = nullptr;

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _rcdo = reinterpret_cast<RCDO<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount)
    {
        _rcdo->AddRef();
    }

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _rcdo = reinterpret_cast<RCDO<T>*>(move._getBlock());

    move._getBlock() = RCDO<TT>::Null();

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void ReferenceCountingPointer<T>::Reset(Allocator& allocator, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    void* const raw = allocator.Allocate(AllocSize());
    _rcdo = ::new(raw) RCDO<T>(allocator, TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void ReferenceCountingPointer<T>::Reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(_rcdo) && _rcdo->m_RefCount && _rcdo->Release() == 0)
    { _rcdo->m_Allocator.DeallocateT(_rcdo); }

    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    _rcdo = ::new(raw) RCDO<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::Forward<Arg0>(arg0), TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator==(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount && ptr)
    { return _rcdo->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(_rcdo) || !_rcdo->m_RefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator!=(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount && ptr)
    { return _rcdo->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(_rcdo) && !_rcdo->m_RefCount) && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys StrongReferenceCountingPointer<T>::AllocSize() noexcept
{ return sizeof(SWRC<T>) + sizeof(typename SWRC<T>::Type); }

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept
    : _swrc(nullptr)
{
    void* const raw = allocator.Allocate(AllocSize());
    _swrc = ::new(raw) SWRC<T>(allocator, TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : _swrc(nullptr)
{
    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    _swrc = ::new(raw) SWRC<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::Forward<Arg0>(arg0), TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{ rcp._getBlock() = SWRC<TT>::Null(); }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const nullptr_t) noexcept
    : _swrc(SWRC<T>::Null())
{ }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer() noexcept
    : _swrc(SWRC<T>::Null())
{ }

template<typename T>
inline StrongReferenceCountingPointer<T>::~StrongReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.Deallocate(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>& dumbCopy) noexcept
    : TReferenceCountingPointerBase<T>(dumbCopy)
    , _swrc(dumbCopy._swrc)
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _swrc(copy._swrc)
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _swrc(move._swrc)
{ move._swrc = SWRC<T>::Null(); }

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.Deallocate(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;

    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount <= 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.Deallocate(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = move._swrc;

    move._swrc = SWRC<T>::Null();

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const nullptr_t) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount <= 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.DeallocateT(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    _swrc = SWRC<T>::Null();

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.DeallocateT(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefStrong(); }

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.DeallocateT(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = reinterpret_cast<SWRC<T>*>(move._getBlock());

    move._getBlock() = SWRC<TT>::Null();

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.Deallocate(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        _swrc = copy._swrc;

        if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
        { _swrc->AddRefStrong(); }
    }
    else
    {
        _swrc = SWRC<T>::Null();
    }

    return *this;
}

template <typename T>
template <typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.Deallocate(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());

        if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
        { _swrc->AddRefStrong(); }
    }
    else
    {
        _swrc = SWRC<T>::Null();
    }

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void StrongReferenceCountingPointer<T>::Reset(Allocator& allocator, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.DeallocateT(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    void* const raw = allocator.Allocate(AllocSize());
    _swrc = ::new(raw) SWRC<T>(allocator, TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void StrongReferenceCountingPointer<T>::Reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->m_StrongRefCount == 1)
    {
        _swrc->DestroyObj();
        _swrc->ReleaseStrong();
        if(!_swrc->m_WeakRefCount)
        { _swrc->m_Allocator.DeallocateT(_swrc); }
    }
    else if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount > 1)
    { _swrc->ReleaseStrong(); }

    void* const raw = DefaultTauAllocator::Instance().Allocate(AllocSize());
    _swrc = ::new(raw) SWRC<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::Forward<Arg0>(arg0), TauAllocatorUtils::Forward<Args>(args)...);
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc == ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc != ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc == ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc != ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount && ptr)
    { return _swrc->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const Type* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount && ptr)
    { return _swrc->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount) && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys WeakReferenceCountingPointer<T>::AllocSize() noexcept
{ return sizeof(SWRC<T>) + sizeof(typename SWRC<T>::Type); }

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<T>& ptr) noexcept
    : TReferenceCountingPointerBase<T>(ptr)
    , _swrc(ptr._swrc)
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
{ rcp._getBlock() = SWRC<TT>::Null(); }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const nullptr_t) noexcept
    : _swrc(SWRC<T>::Null())
{ }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer() noexcept
    : _swrc(SWRC<T>::Null())
{ }

template<typename T>
inline WeakReferenceCountingPointer<T>::~WeakReferenceCountingPointer() noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _swrc(copy._swrc)
{
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _swrc(move._swrc)
{ move._swrc = SWRC<T>::Null(); }

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;
    
    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = move._swrc;

    move._swrc = SWRC<T>::Null();

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;

    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());

    if(TAU_CHECK_RC(_swrc) && (_swrc->m_StrongRefCount || _swrc->m_WeakRefCount))
    { _swrc->AddRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<TT>&& move) noexcept
{
    if(TAU_FORCE_CHECK_RC(_swrc) && _swrc->ReleaseWeak() == 0 && !_swrc->m_StrongRefCount)
    { _swrc->m_Allocator.Deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator=(::std::move(move));

    _swrc = reinterpret_cast<SWRC<T>*>(move._getBlock());

    move._getBlock() = SWRC<TT>::Null();

    return *this;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator==(const T* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount && ptr)
    { return _swrc->ObjPtr() == ptr; }
    if((!TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount) && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator!=(const T* const ptr) const noexcept
{
    if(TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount && ptr)
    { return _swrc->ObjPtr() != ptr; }
    if((!TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount) && !ptr)
    { return false; }
    return true;
}
