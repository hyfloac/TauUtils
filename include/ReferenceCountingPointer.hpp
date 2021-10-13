// ReSharper disable CppClangTidyBugproneUnhandledSelfAssignment
#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "allocator/TauAllocator.hpp"
#include "AtomicIntrinsics.hpp"
#include <type_traits>

#ifndef TAU_DEFAULT_ATOMIC
  #define TAU_DEFAULT_ATOMIC 0
#endif

namespace ReferenceCountingPointerUtils {
/**
 *   This does not store a direct _T because we may want to
 * cast it to a non-concrete interface, in which case the
 * compiler complains. By using a raw buffer we can convince
 * the compiler that it is still a reference to a concrete
 * type.
 */
template<typename T>
struct ReferenceCountDataObject final
{
    DELETE_CM(ReferenceCountDataObject);
public:
    uSys _refCount;
    TauAllocator& _allocator;
    // u8 _objRaw[sizeof(_T)];
public:
    template<typename... Args>
    ReferenceCountDataObject(TauAllocator& allocator, Args&&... args) noexcept;

    ~ReferenceCountDataObject() noexcept;

    [[nodiscard]] T* objPtr()       noexcept { return reinterpret_cast<_T*>(this + 1); }
    [[nodiscard]] const T* objPtr() const noexcept { return reinterpret_cast<_T*>(this + 1); }

    uSys addRef() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return addRefAtomic();
#else
        return addRefNonatomic();
#endif
    }

    uSys release() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return releaseAtomic();
#else          
        return releaseNonatomic();
#endif
    }

    uSys addRefNonatomic() noexcept
    { return ++_refCount; }

    uSys releaseNonatomic() noexcept
    { return --_refCount; }

    uSys addRefAtomic() noexcept
    { return atomicIncrement(&_refCount); }

    uSys releaseAtomic() noexcept
    { return atomicDecrement(&_refCount); }
};

/**
 *   This does not store a direct _T because we may have to
 * destruct the value before the entire object is destroyed.
 */
template<typename T>
struct SWReferenceCount final
{
    DEFAULT_DESTRUCT(SWReferenceCount);
    DELETE_CM(SWReferenceCount);
public:
    uSys _strongRefCount;
    uSys _weakRefCount;
    TauAllocator& _allocator;
    // u8 _objRaw[sizeof(_T)];
public:
    template<typename... Args>
    SWReferenceCount(TauAllocator& allocator, Args&&... args) noexcept;

    [[nodiscard]] T* objPtr()       noexcept { return reinterpret_cast<_T*>(this + 1); }
    [[nodiscard]] const T* objPtr() const noexcept { return reinterpret_cast<_T*>(this + 1); }

    void destroyObj() noexcept { reinterpret_cast<_T*>(this + 1)->~_T(); }

    uSys addRefStrong() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return addRefStrongAtomic();
#else
        return addRefStrongNonatomic();
#endif
    }

    uSys releaseStrong() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return releaseStrongAtomic();
#else          
        return releaseStrongNonatomic();
#endif
    }

    uSys addRefWeak() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return addRefWeakAtomic();
#else
        return addRefWeakNonatomic();
#endif
    }

    uSys releaseWeak() noexcept
    {
#if TAU_DEFAULT_ATOMIC
        return releaseWeakAtomic();
#else          
        return releaseWeakNonatomic();
#endif
    }

    uSys addRefStrongNonatomic() noexcept
    { return ++_strongRefCount; }

    uSys releaseStrongNonatomic() noexcept
    { return --_strongRefCount; }

    uSys addRefStrongAtomic() noexcept
    { return atomicIncrement(&_strongRefCount); }

    uSys releaseStrongAtomic() noexcept
    { return atomicDecrement(&_strongRefCount); }

    uSys addRefWeakNonatomic() noexcept
    { return ++_weakRefCount; }

    uSys releaseWeakNonatomic() noexcept
    { return --_weakRefCount; }

    uSys addRefWeakAtomic() noexcept
    { return atomicIncrement(&_weakRefCount); }

    uSys releaseWeakAtomic() noexcept
    { return atomicDecrement(&_weakRefCount); }
};

template<typename ToT, typename FromT>
const ReferenceCountDataObject<ToT>& RCDOCast(const ReferenceCountDataObject<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
ReferenceCountDataObject<ToT>& RCDOCast(ReferenceCountDataObject<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
ReferenceCountDataObject<ToT>&& RCDOCast(ReferenceCountDataObject<FromT>&& obj) noexcept;

template<typename ToT, typename FromT>
const SWReferenceCount<ToT>& SWRCCast(const SWReferenceCount<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
SWReferenceCount<ToT>& SWRCCast(SWReferenceCount<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
SWReferenceCount<ToT>&& SWRCCast(SWReferenceCount<FromT>&& obj) noexcept;
}

class ReferenceCountingPointerBase
{
    DEFAULT_CONSTRUCT_PO(ReferenceCountingPointerBase);
    DEFAULT_DESTRUCT_VI(ReferenceCountingPointerBase);
    DEFAULT_CM_PO(ReferenceCountingPointerBase);
public:
    [[nodiscard]] virtual uSys refCount() const noexcept = 0;

    [[nodiscard]] virtual operator bool() const noexcept = 0;

    [[nodiscard]] virtual bool operator ==(nullptr_t) const noexcept = 0;
    [[nodiscard]] virtual bool operator !=(nullptr_t) const noexcept = 0;
};

template<typename T>
class TReferenceCountingPointerBase : public ReferenceCountingPointerBase
{
    DEFAULT_CONSTRUCT_PO(TReferenceCountingPointerBase);
    DEFAULT_DESTRUCT_VI(TReferenceCountingPointerBase);
    DEFAULT_CM_PO(TReferenceCountingPointerBase);
protected:
    template<typename TT>
    TReferenceCountingPointerBase(const TReferenceCountingPointerBase<TT>& copy) noexcept
    { }

    template<typename TT>
    TReferenceCountingPointerBase(TReferenceCountingPointerBase<TT>&& move) noexcept
    { }

    template<typename TT>
    TReferenceCountingPointerBase<T>& operator=(const TReferenceCountingPointerBase<TT>& copy) noexcept
    { return *this; }

    template<typename TT>
    TReferenceCountingPointerBase<T>& operator=(TReferenceCountingPointerBase<TT>&& move) noexcept
    { return *this; }
public:
    [[nodiscard]] virtual T& operator  *()                = 0;
    [[nodiscard]] virtual const T& operator  *() const          = 0;
    [[nodiscard]] virtual T* operator ->()       noexcept = 0;
    [[nodiscard]] virtual const T* operator ->() const noexcept = 0;

    [[nodiscard]] virtual T* get()       noexcept = 0;
    [[nodiscard]] virtual const T* get() const noexcept = 0;

    [[nodiscard]] virtual bool operator ==(const T* const& ptr) const noexcept = 0;
    [[nodiscard]] virtual bool operator !=(const T* const& ptr) const noexcept = 0;
};

template<typename T>
class ReferenceCountingPointer;

template<typename T>
class StrongReferenceCountingPointer;

template<typename T>
class WeakReferenceCountingPointer;

template<typename T>
class ReferenceCountingPointer final : public TReferenceCountingPointerBase<T>
{
public:
    template<typename TT>
    using RCDO = ReferenceCountingPointerUtils::ReferenceCountDataObject<TT>;

    [[nodiscard]] static constexpr uSys allocSize() noexcept;
private:
    RCDO<T>* _rcdo;
    T* _tPtr;
public:
    template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int> = 0>
    explicit ReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    explicit ReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept;

    template<typename TT>
    ReferenceCountingPointer(const ReferenceCountingPointer<TT>& rcp) noexcept;

    template<typename TT>
    ReferenceCountingPointer(ReferenceCountingPointer<TT>&& rcp) noexcept;

    constexpr ReferenceCountingPointer(nullptr_t) noexcept;

    constexpr ReferenceCountingPointer() noexcept;

    ~ReferenceCountingPointer() noexcept override;

    ReferenceCountingPointer(const ReferenceCountingPointer<T>& copy) noexcept;
    ReferenceCountingPointer(ReferenceCountingPointer<T>&& move) noexcept;

    ReferenceCountingPointer<T>& operator=(nullptr_t) noexcept;

    ReferenceCountingPointer<T>& operator=(const ReferenceCountingPointer<T>& copy) noexcept;
    ReferenceCountingPointer<T>& operator=(ReferenceCountingPointer<T>&& move) noexcept;

    template<typename TT>
    ReferenceCountingPointer<T>& operator=(const ReferenceCountingPointer<TT>& copy) noexcept;

    template<typename TT>
    ReferenceCountingPointer<T>& operator=(ReferenceCountingPointer<TT>&& move) noexcept;

    template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int> = 0>
    void reset(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    void reset(Arg0&& arg0, Args&&... args) noexcept;

    [[nodiscard]] T& operator  *()                override { return *_tPtr; }
    [[nodiscard]] const T& operator  *() const          override { return *_tPtr; }
    [[nodiscard]] T* operator ->()       noexcept override { return _tPtr; }
    [[nodiscard]] const T* operator ->() const noexcept override { return _tPtr; }

    [[nodiscard]] T* get()       noexcept override { return _tPtr; }
    [[nodiscard]] const T* get() const noexcept override { return _tPtr; }

    [[nodiscard]] uSys refCount() const noexcept override { return _rcdo ? _rcdo->_refCount : 0; }

    [[nodiscard]] RCDO<T>*& _getBlock()       noexcept { return _rcdo; }
    [[nodiscard]] RCDO<T>*  _getBlock() const noexcept { return _rcdo; }

    [[nodiscard]] operator bool() const noexcept override { return _rcdo; }

    [[nodiscard]] bool operator ==(const ReferenceCountingPointer<T>& ptr) const noexcept { return _rcdo == ptr._rcdo; }
    [[nodiscard]] bool operator !=(const ReferenceCountingPointer<T>& ptr) const noexcept { return _rcdo != ptr._rcdo; }

    [[nodiscard]] bool operator ==(const T* const & ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const T* const & ptr) const noexcept override;

    [[nodiscard]] bool operator ==(nullptr_t) const noexcept override { return !_rcdo; }
    [[nodiscard]] bool operator !=(nullptr_t) const noexcept override { return  _rcdo; }
private:
    template<typename TT>
    friend class ReferenceCountingPointer;
};

template<typename T>
class StrongReferenceCountingPointer final : public TReferenceCountingPointerBase<T>
{
public:
    template<typename TT>
    using SWRC = ReferenceCountingPointerUtils::SWReferenceCount<TT>;

    [[nodiscard]] static constexpr uSys allocSize() noexcept;
private:
    SWRC<T>* _swrc;
    T* _tPtr;
public:
    template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int> = 0>
    explicit StrongReferenceCountingPointer(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    explicit StrongReferenceCountingPointer(Arg0&& arg0, Args&&... args) noexcept;

    template<typename TT>
    StrongReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept;

    template<typename TT>
    StrongReferenceCountingPointer(StrongReferenceCountingPointer<TT>&& rcp) noexcept;

    constexpr StrongReferenceCountingPointer(nullptr_t) noexcept;

    constexpr StrongReferenceCountingPointer() noexcept;

    ~StrongReferenceCountingPointer() noexcept override;

    StrongReferenceCountingPointer(const StrongReferenceCountingPointer<T>& copy) noexcept;
    StrongReferenceCountingPointer(StrongReferenceCountingPointer<T>&& move) noexcept;

    StrongReferenceCountingPointer<T>& operator=(const StrongReferenceCountingPointer<T>& copy) noexcept;
    StrongReferenceCountingPointer<T>& operator=(StrongReferenceCountingPointer<T>&& move) noexcept;

    StrongReferenceCountingPointer<T>& operator=(const WeakReferenceCountingPointer<T>& copy) noexcept;

    StrongReferenceCountingPointer<T>& operator=(nullptr_t) noexcept;

    template<typename TT>
    StrongReferenceCountingPointer<T>& operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept;

    template<typename TT>
    StrongReferenceCountingPointer<T>& operator=(StrongReferenceCountingPointer<TT>&& move) noexcept;

    template<typename TT>
    StrongReferenceCountingPointer<T>& operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept;

    template<typename Allocator, typename... Args, ::std::enable_if_t<::std::is_base_of_v<TauAllocator, Allocator>, int> = 0>
    void reset(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    void reset(Arg0&& arg0, Args&&... args) noexcept;

    [[nodiscard]] T& operator  *()                override { return *_tPtr; }
    [[nodiscard]] const T& operator  *() const          override { return *_tPtr; }
    [[nodiscard]] T* operator ->()       noexcept override { return _tPtr; }
    [[nodiscard]] const T* operator ->() const noexcept override { return _tPtr; }

    [[nodiscard]] T* get()       noexcept override { return _tPtr; }
    [[nodiscard]] const T* get() const noexcept override { return _tPtr; }

    [[nodiscard]] uSys refCount()       const noexcept override { return _swrc ? _swrc->_strongRefCount : 0; }
    [[nodiscard]] uSys strongRefCount() const noexcept          { return _swrc ? _swrc->_strongRefCount : 0; }
    [[nodiscard]] uSys weakRefCount()   const noexcept          { return _swrc ? _swrc->_weakRefCount   : 0; }

    [[nodiscard]] SWRC<T>*& _getBlock()       noexcept { return _swrc; }
    [[nodiscard]] SWRC<T>*  _getBlock() const noexcept { return _swrc; }

    [[nodiscard]] operator bool() const noexcept override { return _swrc; }

    [[nodiscard]] bool operator ==(const StrongReferenceCountingPointer<T>& ptr) const noexcept;
    [[nodiscard]] bool operator !=(const StrongReferenceCountingPointer<T>& ptr) const noexcept;

    [[nodiscard]] bool operator ==(const WeakReferenceCountingPointer<T>& ptr) const noexcept;
    [[nodiscard]] bool operator !=(const WeakReferenceCountingPointer<T>& ptr) const noexcept;

    [[nodiscard]] bool operator ==(const T* const& ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const T* const& ptr) const noexcept override;

    [[nodiscard]] bool operator ==(nullptr_t) const noexcept override { return !_swrc; }
    [[nodiscard]] bool operator !=(nullptr_t) const noexcept override { return  _swrc; }
private:
    friend class WeakReferenceCountingPointer<T>;

    template<typename TT>
    friend class StrongReferenceCountingPointer;

    template<typename TT>
    friend class WeakReferenceCountingPointer;
};

template<typename T>
class WeakReferenceCountingPointer final : public TReferenceCountingPointerBase<T>
{
public:
    template<typename TT>
    using SWRC = ReferenceCountingPointerUtils::SWReferenceCount<TT>;

    [[nodiscard]] static constexpr uSys allocSize() noexcept;
private:
    SWRC<T>* _swrc;
    T* _tPtr;
public:
    WeakReferenceCountingPointer(const StrongReferenceCountingPointer<T>& ptr) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer(const StrongReferenceCountingPointer<TT>& rcp) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer(const WeakReferenceCountingPointer<TT>& rcp) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer(WeakReferenceCountingPointer<TT>&& rcp) noexcept;

    constexpr WeakReferenceCountingPointer(nullptr_t) noexcept;

    constexpr WeakReferenceCountingPointer() noexcept;

    ~WeakReferenceCountingPointer() noexcept override;

    WeakReferenceCountingPointer(const WeakReferenceCountingPointer<T>& copy) noexcept;
    WeakReferenceCountingPointer(WeakReferenceCountingPointer<T>&& move) noexcept;

    WeakReferenceCountingPointer<T>& operator=(const WeakReferenceCountingPointer<T>& copy) noexcept;
    WeakReferenceCountingPointer<T>& operator=(WeakReferenceCountingPointer<T>&& move) noexcept;

    WeakReferenceCountingPointer<T>& operator=(const StrongReferenceCountingPointer<T>& copy) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer<T>& operator=(const StrongReferenceCountingPointer<TT>& copy) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer<T>& operator=(const WeakReferenceCountingPointer<TT>& copy) noexcept;

    template<typename TT>
    WeakReferenceCountingPointer<T>& operator=(WeakReferenceCountingPointer<TT>&& move) noexcept;

    [[nodiscard]] T& operator  *()                override { return *_tPtr; }
    [[nodiscard]] const T& operator  *() const          override { return *_tPtr; }
    [[nodiscard]] T* operator ->()       noexcept override { return _swrc && _swrc->_strongRefCount ? _tPtr : nullptr; }
    [[nodiscard]] const T* operator ->() const noexcept override { return _swrc && _swrc->_strongRefCount ? _tPtr : nullptr; }

    [[nodiscard]] T* get()       noexcept override { return _swrc && _swrc->_strongRefCount ? _tPtr : nullptr; }
    [[nodiscard]] const T* get() const noexcept override { return _swrc && _swrc->_strongRefCount ? _tPtr : nullptr; }

    [[nodiscard]] uSys refCount()       const noexcept override { return _swrc ? _swrc->_weakRefCount   : 0; }
    [[nodiscard]] uSys strongRefCount() const noexcept          { return _swrc ? _swrc->_strongRefCount : 0; }
    [[nodiscard]] uSys weakRefCount()   const noexcept          { return _swrc ? _swrc->_weakRefCount   : 0; }

    [[nodiscard]] SWRC<T>*& _getBlock()       noexcept { return _swrc; }
    [[nodiscard]] SWRC<T>*  _getBlock() const noexcept { return _swrc; }

    [[nodiscard]] operator bool() const noexcept override { return _swrc && _swrc->_strongRefCount; }

    [[nodiscard]] bool operator ==(const StrongReferenceCountingPointer<T>& ptr) const noexcept { return _swrc == ptr._rcdo; }
    [[nodiscard]] bool operator !=(const StrongReferenceCountingPointer<T>& ptr) const noexcept { return _swrc != ptr._rcdo; }

    [[nodiscard]] bool operator ==(const WeakReferenceCountingPointer<T>& ptr) const noexcept { return _swrc == ptr._swrc; }
    [[nodiscard]] bool operator !=(const WeakReferenceCountingPointer<T>& ptr) const noexcept { return _swrc != ptr._swrc; }

    [[nodiscard]] bool operator ==(const T* const& ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const T* const& ptr) const noexcept override;

    [[nodiscard]] bool operator ==(const nullptr_t) const noexcept override { return !_swrc; }
    [[nodiscard]] bool operator !=(const nullptr_t) const noexcept override { return  _swrc; }
private:
    friend class StrongReferenceCountingPointer<T>;

    template<typename TT>
    friend class StrongReferenceCountingPointer;

    template<typename TT>
    friend class WeakReferenceCountingPointer;
};

template<typename ToT, typename FromT>
ReferenceCountingPointer<ToT> RCPStaticCast(const ReferenceCountingPointer<FromT>& ptr) noexcept
{ return ReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
ReferenceCountingPointer<ToT> RCPStaticCast(ReferenceCountingPointer<FromT>&& ptr) noexcept
{ return ReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
StrongReferenceCountingPointer<ToT> RCPStaticCast(const StrongReferenceCountingPointer<FromT>& ptr) noexcept
{ return StrongReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
StrongReferenceCountingPointer<ToT> RCPStaticCast(StrongReferenceCountingPointer<FromT>&& ptr) noexcept
{ return StrongReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
WeakReferenceCountingPointer<ToT> RCPStaticCast(const WeakReferenceCountingPointer<FromT>& ptr) noexcept
{ return WeakReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
WeakReferenceCountingPointer<ToT> RCPStaticCast(WeakReferenceCountingPointer<FromT>&& ptr) noexcept
{ return WeakReferenceCountingPointer<ToT>(ptr); }

template<typename ToT, typename FromT>
ReferenceCountingPointer<ToT> RCPReinterpretCast(const ReferenceCountingPointer<FromT>& ptr) noexcept
{ return reinterpret_cast<const ReferenceCountingPointer<ToT>&>(ptr); }

template<typename ToT, typename FromT>
ReferenceCountingPointer<ToT> RCPReinterpretCast(ReferenceCountingPointer<FromT>&& ptr) noexcept
{ return reinterpret_cast<ReferenceCountingPointer<ToT>&&>(ptr); }

template<typename ToT, typename FromT>
StrongReferenceCountingPointer<ToT> RCPReinterpretCast(const StrongReferenceCountingPointer<FromT>& ptr) noexcept
{ return reinterpret_cast<const StrongReferenceCountingPointer<ToT>&>(ptr); }

template<typename ToT, typename FromT>
StrongReferenceCountingPointer<ToT> RCPReinterpretCast(StrongReferenceCountingPointer<FromT>&& ptr) noexcept
{ return reinterpret_cast<StrongReferenceCountingPointer<ToT>&&>(ptr); }

template<typename ToT, typename FromT>
WeakReferenceCountingPointer<ToT> RCPReinterpretCast(const WeakReferenceCountingPointer<FromT>& ptr) noexcept
{ return reinterpret_cast<const WeakReferenceCountingPointer<ToT>&>(ptr); }

template<typename ToT, typename FromT>
WeakReferenceCountingPointer<ToT> RCPReinterpretCast(WeakReferenceCountingPointer<FromT>&& ptr) noexcept
{ return reinterpret_cast<WeakReferenceCountingPointer<ToT>&&>(ptr); }

#include "ReferenceCountingPointer.inl"
