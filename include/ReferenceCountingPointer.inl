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
    : _refCount(1)
    , _allocator(allocator)
{ (void) new(this + 1) T(TauAllocatorUtils::_forward<Args>(args)...); }

template<typename T>
inline ReferenceCountDataObject<T>::~ReferenceCountDataObject() noexcept
{ reinterpret_cast<T*>(this + 1)->~T(); }

template<typename T>
template<typename... Args>
inline SWReferenceCount<T>::SWReferenceCount(TauAllocator& allocator, Args&&... args) noexcept
    : _strongRefCount(1)
    , _weakRefCount(0)
    , _allocator(allocator)
{ (void) new(this + 1) T(TauAllocatorUtils::_forward<Args>(args)...); }

template<typename ToT, typename FromT>
inline const ReferenceCountDataObject<ToT>& RCDOCast(const ReferenceCountDataObject<FromT>& obj) noexcept
{ return reinterpret_cast<const ReferenceCountDataObject<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline ReferenceCountDataObject<ToT>& RCDOCast(ReferenceCountDataObject<FromT>& obj) noexcept
{ return reinterpret_cast<ReferenceCountDataObject<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline ReferenceCountDataObject<ToT>&& RCDOCast(ReferenceCountDataObject<FromT>&& obj) noexcept
{ return reinterpret_cast<ReferenceCountDataObject<ToT>&&>(obj); }

template<typename ToT, typename FromT>
inline const SWReferenceCount<ToT>& SWRCCast(const SWReferenceCount<FromT>& obj) noexcept
{ return reinterpret_cast<const SWReferenceCount<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline SWReferenceCount<ToT>& SWRCCast(SWReferenceCount<FromT>& obj) noexcept
{ return reinterpret_cast<SWReferenceCount<ToT>&>(obj); }

template<typename ToT, typename FromT>
inline SWReferenceCount<ToT>&& SWRCCast(SWReferenceCount<FromT>&& obj) noexcept
{ return reinterpret_cast<SWReferenceCount<ToT>&&>(obj); }
}

template<typename T>
inline constexpr uSys ReferenceCountingPointer<T>::allocSize() noexcept
{ return sizeof(RCDO<T>); }

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
ReferenceCountingPointer<T>::ReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept
    : _rcdo(nullptr)
    , _tPtr(nullptr)
{
    void* raw = allocator.allocate(sizeof(RCDO<T>) + sizeof(T));
    _rcdo = new(raw) RCDO<T>(allocator, TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _rcdo->objPtr();
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
ReferenceCountingPointer<T>::ReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : _rcdo(nullptr)
    , _tPtr(nullptr)
{
    void* raw = DefaultTauAllocator::Instance().allocate(sizeof(RCDO<T>) + sizeof(T));
    _rcdo = new(raw) RCDO<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::_forward<Arg0>(arg0), TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _rcdo->objPtr();
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _rcdo(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp._tPtr))
{
    if(_rcdo) 
    { _rcdo->addRef(); }
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _rcdo(reinterpret_cast<RCDO<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp._tPtr))
{ rcp._getBlock() = nullptr; }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer(const nullptr_t) noexcept
    : _rcdo(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline constexpr ReferenceCountingPointer<T>::ReferenceCountingPointer() noexcept
    : _rcdo(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline ReferenceCountingPointer<T>::~ReferenceCountingPointer() noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }
}

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(const ReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _rcdo(copy._rcdo)
    , _tPtr(copy._tPtr)
{
    if(_rcdo) 
    { _rcdo->addRef(); }
}

template<typename T>
inline ReferenceCountingPointer<T>::ReferenceCountingPointer(ReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _rcdo(move._rcdo)
    , _tPtr(move._tPtr)
{ move._rcdo = nullptr; }

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const nullptr_t) noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    _rcdo = nullptr;
    _tPtr = nullptr;

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _rcdo = copy._rcdo;
    _tPtr = copy._tPtr;

    if(_rcdo)
    { _rcdo->addRef(); }

    return *this;
}

template<typename T>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _rcdo = move._rcdo;
    _tPtr = move._tPtr;

    move._rcdo = nullptr;

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(const ReferenceCountingPointer<TT>& copy) noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _rcdo = reinterpret_cast<RCDO<T>*>(copy._getBlock());
    _tPtr = static_cast<T*>(copy._tPtr);

    if(_rcdo)
    { _rcdo->addRef(); }

    return *this;
}

template<typename T>
template<typename TT>
inline ReferenceCountingPointer<T>& ReferenceCountingPointer<T>::operator=(ReferenceCountingPointer<TT>&& move) noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _rcdo = reinterpret_cast<RCDO<T>*>(move._getBlock());
    _tPtr = static_cast<T*>(move._tPtr);

    move._getBlock() = nullptr;

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void ReferenceCountingPointer<T>::reset(Allocator& allocator, Args&&... args) noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    void* raw = allocator.allocate(sizeof(RCDO<T>) + sizeof(T));
    _rcdo = new(raw) RCDO<T>(allocator, TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _rcdo->objPtr();
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void ReferenceCountingPointer<T>::reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(_rcdo && _rcdo->release() == 0)
    { _rcdo->_allocator.deallocateT(_rcdo); }

    void* raw = DefaultTauAllocator::Instance().allocate(sizeof(RCDO<T>) + sizeof(T));
    _rcdo = new(raw) RCDO<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::_forward<Arg0>(arg0), TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _rcdo->objPtr();
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator==(const T* const& ptr) const noexcept
{
    if(_rcdo && ptr)
    { return _rcdo->objPtr() == ptr; }
    if(!_rcdo && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool ReferenceCountingPointer<T>::operator!=(const T* const& ptr) const noexcept
{
    if(_rcdo && ptr)
    { return _rcdo->objPtr() != ptr; }
    if(!_rcdo && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys StrongReferenceCountingPointer<T>::allocSize() noexcept
{ return sizeof(SWRC<T>); }

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{
    void* raw = allocator.allocate(sizeof(SWRC<T>) + sizeof(T));
    _swrc = new(raw) SWRC<T>(allocator, TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _swrc->objPtr();
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{
    void* raw = DefaultTauAllocator::Instance().allocate(sizeof(SWRC<T>) + sizeof(T));
    _swrc = new(raw) SWRC<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::_forward<Arg0>(arg0), TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _swrc->objPtr();
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp._tPtr))
{
    if(_swrc) 
    { _swrc->addRefStrong(); }
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp._tPtr))
{ rcp._getBlock() = nullptr; }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const nullptr_t) noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline constexpr StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer() noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline StrongReferenceCountingPointer<T>::~StrongReferenceCountingPointer() noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(const StrongReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _swrc(copy._swrc)
    , _tPtr(copy._tPtr)
{
    if(_swrc) 
    { _swrc->addRefStrong(); }
}

template<typename T>
inline StrongReferenceCountingPointer<T>::StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _swrc(move._swrc)
    , _tPtr(move._tPtr)
{ move._swrc = nullptr; }

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;
    _tPtr = copy._tPtr;

    if(_swrc) 
    { _swrc->addRefStrong(); }

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = move._swrc;
    _tPtr = move._tPtr;

    move._swrc = nullptr;

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const nullptr_t) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    _swrc = nullptr;
    _tPtr = nullptr;

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());
    _tPtr = static_cast<T*>(copy._tPtr);

    if(_swrc)
    { _swrc->addRefStrong(); }

    return *this;
}

template<typename T>
template<typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(StrongReferenceCountingPointer<TT>&& move) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = reinterpret_cast<SWRC<T>*>(move._getBlock());
    _tPtr = static_cast<T*>(move._tPtr);

    move._getBlock() = nullptr;

    return *this;
}

template<typename T>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocate(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        _swrc = copy._rcdo;
        _tPtr = copy._tPtr;

        _swrc->addRefStrong();
    }
    else
    {
        _swrc = nullptr;
        _tPtr = nullptr;
    }

    return *this;
}

template <typename T>
template <typename TT>
inline StrongReferenceCountingPointer<T>& StrongReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocate(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    if(copy)
    {
        _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());
        _tPtr = static_cast<T*>(copy._tPtr);

        _swrc->addRefStrong();
    }
    else
    {
        _swrc = nullptr;
        _tPtr = nullptr;
    }

    return *this;
}

template<typename T>
template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int>>
inline void StrongReferenceCountingPointer<T>::reset(Allocator& allocator, Args&&... args) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    void* raw = allocator.allocate(sizeof(SWRC<T>) + sizeof(T));
    _swrc = new(raw) SWRC<T>(allocator, TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _swrc->objPtr();
}

template<typename T>
template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int>>
inline void StrongReferenceCountingPointer<T>::reset(Arg0&& arg0, Args&&... args) noexcept
{
    if(_swrc && _swrc->_strongRefCount == 1)
    {
        _swrc->destroyObj();
        if(!_swrc->_weakRefCount)
        { _swrc->_allocator.deallocateT(_swrc); }
    }
    else if(_swrc && _swrc->_strongRefCount > 1)
    { _swrc->releaseStrong(); }

    void* raw = DefaultTauAllocator::Instance().allocate(sizeof(SWRC<T>) + sizeof(T));
    _swrc = new(raw) SWRC<T>(DefaultTauAllocator::Instance(), TauAllocatorUtils::_forward<Arg0>(arg0), TauAllocatorUtils::_forward<Args>(args)...);
    _tPtr = _swrc->objPtr();
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc == ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const StrongReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc != ptr._swrc; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc == ptr._rcdo; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const WeakReferenceCountingPointer<T>& ptr) const noexcept
{ return _swrc != ptr._rcdo; }

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator==(const T* const& ptr) const noexcept
{
    if(_swrc && ptr)
    { return _swrc->objPtr() == ptr; }
    if(!_swrc && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool StrongReferenceCountingPointer<T>::operator!=(const T* const& ptr) const noexcept
{
    if(_swrc && ptr)
    { return _swrc->objPtr() != ptr; }
    if(!_swrc && !ptr)
    { return false; }
    return true;
}

template<typename T>
inline constexpr uSys WeakReferenceCountingPointer<T>::allocSize() noexcept
{ return sizeof(SWRC<T>); }

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<T>& ptr) noexcept
    : TReferenceCountingPointerBase<T>(ptr)
    , _swrc(ptr._swrc)
    , _tPtr(ptr._tPtr)
{
    if(_swrc) 
    { _swrc->addRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp.get()))
{
    if(_swrc) 
    { _swrc->addRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<TT>& rcp) noexcept
    : TReferenceCountingPointerBase<T>(rcp)
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp.get()))
{
    if(_swrc) 
    { _swrc->addRefWeak(); }
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<TT>&& rcp) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(rcp))
    , _swrc(reinterpret_cast<SWRC<T>*>(rcp._getBlock()))
    , _tPtr(static_cast<T*>(rcp.get()))
{ rcp._getBlock() = nullptr; }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const nullptr_t) noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline constexpr WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer() noexcept
    : _swrc(nullptr)
    , _tPtr(nullptr)
{ }

template<typename T>
inline WeakReferenceCountingPointer<T>::~WeakReferenceCountingPointer() noexcept
{
    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(const WeakReferenceCountingPointer<T>& copy) noexcept
    : TReferenceCountingPointerBase<T>(copy)
    , _swrc(copy._swrc)
    , _tPtr(copy._tPtr)
{
    if(_swrc) 
    { _swrc->addRefWeak(); }
}

template<typename T>
inline WeakReferenceCountingPointer<T>::WeakReferenceCountingPointer(WeakReferenceCountingPointer<T>&& move) noexcept
    : TReferenceCountingPointerBase<T>(::std::move(move))
    , _swrc(move._swrc)
    , _tPtr(move._tPtr)
{ move._swrc = nullptr; }

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<T>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;
    _tPtr = copy._tPtr;

    if(_swrc) 
    { _swrc->addRefWeak(); }

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<T>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(::std::move(move));

    _swrc = move._swrc;
    _tPtr = move._tPtr;

    move._swrc = nullptr;

    return *this;
}

template<typename T>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<T>& copy) noexcept
{
    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = copy._swrc;
    _tPtr = copy._tPtr;

    if(_swrc) 
    { _swrc->addRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept
{
    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());
    _tPtr = static_cast<T*>(copy._tPtr);

    if(_swrc) 
    { _swrc->addRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept
{
    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator =(copy);

    _swrc = reinterpret_cast<SWRC<T>*>(copy._getBlock());
    _tPtr = static_cast<T*>(copy._tPtr);

    if(_swrc) 
    { _swrc->addRefWeak(); }

    return *this;
}

template<typename T>
template<typename TT>
inline WeakReferenceCountingPointer<T>& WeakReferenceCountingPointer<T>::operator=(WeakReferenceCountingPointer<TT>&& move) noexcept
{
    if(_swrc && _swrc->releaseWeak() == 0 && !_swrc->_strongRefCount)
    { _swrc->_allocator.deallocate(_swrc); }

    TReferenceCountingPointerBase<T>::operator=(::std::move(move));

    _swrc = reinterpret_cast<SWRC<T>*>(move._getBlock());
    _tPtr = static_cast<T*>(move._tPtr);

    move._getBlock() = nullptr;

    return *this;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator==(const T* const& ptr) const noexcept
{
    if(_swrc && ptr)
    { return _swrc->objPtr() == ptr; }
    if(!_swrc && !ptr)
    { return true; }
    return false;
}

template<typename T>
inline bool WeakReferenceCountingPointer<T>::operator!=(const T* const& ptr) const noexcept
{
    if(_swrc && ptr)
    { return _swrc->objPtr() != ptr; }
    if(!_swrc && !ptr)
    { return false; }
    return true;
}
