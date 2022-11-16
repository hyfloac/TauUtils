// ReSharper disable CppClangTidyBugproneUnhandledSelfAssignment
#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "allocator/TauAllocator.hpp"
#include "TUConfig.hpp"
#include <type_traits>
#include <atomic>

#ifndef TAU_RCP_EXTRA_SAFETY
  #define TAU_RCP_EXTRA_SAFETY 1
#endif

#if TAU_RCP_EXTRA_SAFETY
  #define TAU_CHECK_RC(RC) (RC)
#else
  #define TAU_CHECK_RC(RC) (true)
#endif

namespace ReferenceCountingPointerUtils {

/**
 *   This does not store a direct T because we may want to
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
    using Type = TauAllocatorUtils::RemoveConstT<TauAllocatorUtils::RemoveReferenceT<T>>;
public:
    static constexpr ReferenceCountDataObject<T>* Null() noexcept;
public:
    ::std::atomic<uSys> m_RefCount;
    TauAllocator& m_Allocator;
public:
    template<typename... Args>
    ReferenceCountDataObject(TauAllocator& allocator, Args&&... args) noexcept;

    constexpr ReferenceCountDataObject(nullptr_t) noexcept;

    ~ReferenceCountDataObject() noexcept;

    [[nodiscard]]       Type* ObjPtr()       noexcept { return reinterpret_cast<Type*>(this + 1); }
    [[nodiscard]] const Type* ObjPtr() const noexcept { return reinterpret_cast<Type*>(this + 1); }

    uSys AddRef() noexcept
    {
        return ++m_RefCount;
    }

    uSys Release() noexcept
    {
        return --m_RefCount;
    }
};

/**
 *   This does not store a direct T because we may have to
 * destruct the value before the entire object is destroyed.
 */
template<typename T>
struct SWReferenceCount final
{
    DEFAULT_DESTRUCT(SWReferenceCount);
    DELETE_CM(SWReferenceCount);
public:
    using Type = TauAllocatorUtils::RemoveConstT<TauAllocatorUtils::RemoveReferenceT<T>>;
public:
    static constexpr SWReferenceCount<T>* Null() noexcept;
public:
    ::std::atomic<uSys> m_StrongRefCount;
    ::std::atomic<uSys> m_WeakRefCount;
    TauAllocator& m_Allocator;
public:
    template<typename... Args>
    SWReferenceCount(TauAllocator& allocator, Args&&... args) noexcept;

    constexpr SWReferenceCount(nullptr_t) noexcept;

    [[nodiscard]]       Type* ObjPtr()       noexcept { return reinterpret_cast<Type*>(this + 1); }
    [[nodiscard]] const Type* ObjPtr() const noexcept { return reinterpret_cast<Type*>(this + 1); }

    void DestroyObj() noexcept
    { ObjPtr()->~Type(); }

    uSys AddRefStrong() noexcept
    {
        return ++m_StrongRefCount;
    }

    uSys ReleaseStrong() noexcept
    {
        return --m_StrongRefCount;
    }

    uSys AddRefWeak() noexcept
    {
        return ++m_WeakRefCount;
    }

    uSys ReleaseWeak() noexcept
    {
        return --m_WeakRefCount;
    }
};

template<typename ToT, typename FromT>
constexpr const ReferenceCountDataObject<ToT>& RCDOCast(const ReferenceCountDataObject<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
constexpr ReferenceCountDataObject<ToT>& RCDOCast(ReferenceCountDataObject<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
constexpr ReferenceCountDataObject<ToT>&& RCDOCast(ReferenceCountDataObject<FromT>&& obj) noexcept;

template<typename ToT, typename FromT>
constexpr const ReferenceCountDataObject<ToT>* RCDOCast(const ReferenceCountDataObject<FromT>* obj) noexcept;

template<typename ToT, typename FromT>
constexpr ReferenceCountDataObject<ToT>* RCDOCast(ReferenceCountDataObject<FromT>* obj) noexcept;

template<typename ToT, typename FromT>
constexpr const SWReferenceCount<ToT>& SWRCCast(const SWReferenceCount<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
constexpr SWReferenceCount<ToT>& SWRCCast(SWReferenceCount<FromT>& obj) noexcept;

template<typename ToT, typename FromT>
constexpr SWReferenceCount<ToT>&& SWRCCast(SWReferenceCount<FromT>&& obj) noexcept;

template<typename ToT, typename FromT>
constexpr const SWReferenceCount<ToT>* SWRCCast(const SWReferenceCount<FromT>* obj) noexcept;

template<typename ToT, typename FromT>
constexpr SWReferenceCount<ToT>* SWRCCast(SWReferenceCount<FromT>* obj) noexcept;

TAU_LIB extern ReferenceCountDataObject<void*> NullRCDO;
TAU_LIB extern SWReferenceCount<void*> NullSWRC;
}

class ReferenceCountingPointerBase
{
    DEFAULT_CONSTRUCT_PO(ReferenceCountingPointerBase);
    DEFAULT_DESTRUCT_VI(ReferenceCountingPointerBase);
    DEFAULT_CM_PO(ReferenceCountingPointerBase);
public:
    [[nodiscard]] virtual uSys RefCount() const noexcept = 0;

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
public:
    using Type = TauAllocatorUtils::RemoveConstT<TauAllocatorUtils::RemoveReferenceT<T>>;
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
    [[nodiscard]] virtual       Type& operator  *()                = 0;
    [[nodiscard]] virtual const Type& operator  *() const          = 0;
    [[nodiscard]] virtual       Type* operator ->()       noexcept = 0;
    [[nodiscard]] virtual const Type* operator ->() const noexcept = 0;

    [[nodiscard]] virtual       Type* Get()       noexcept = 0;
    [[nodiscard]] virtual const Type* Get() const noexcept = 0;

    [[nodiscard]] virtual bool operator ==(const Type* ptr) const noexcept = 0;
    [[nodiscard]] virtual bool operator !=(const Type* ptr) const noexcept = 0;
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

    using Type = typename RCDO<T>::Type;
public:
    [[nodiscard]] static constexpr uSys AllocSize() noexcept;
private:
    RCDO<T>* _rcdo;
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
    void Reset(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    void Reset(Arg0&& arg0, Args&&... args) noexcept;

    [[nodiscard]]       Type& operator  *()                override { return *_rcdo->ObjPtr(); }
    [[nodiscard]] const Type& operator  *() const          override { return *_rcdo->ObjPtr(); }
    [[nodiscard]]       Type* operator ->()       noexcept override { return  TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount.load() ? _rcdo->ObjPtr() : nullptr; }
    [[nodiscard]] const Type* operator ->() const noexcept override { return  TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount.load() ? _rcdo->ObjPtr() : nullptr; }

    [[nodiscard]]       Type* Get()       noexcept override { return TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount.load() ? _rcdo->ObjPtr() : nullptr; }
    [[nodiscard]] const Type* Get() const noexcept override { return TAU_CHECK_RC(_rcdo) && _rcdo->m_RefCount.load() ? _rcdo->ObjPtr() : nullptr; }

    [[nodiscard]] uSys RefCount() const noexcept override { return TAU_CHECK_RC(_rcdo) ? _rcdo->m_RefCount.load() : static_cast<uSys>(0); }

    [[nodiscard]] RCDO<T>*& _getBlock()       noexcept { return _rcdo; }
    [[nodiscard]] RCDO<T>*  _getBlock() const noexcept { return _rcdo; }

    [[nodiscard]] operator bool() const noexcept override { return _rcdo; }

    [[nodiscard]] bool operator ==(const ReferenceCountingPointer<T>& ptr) const noexcept { return _rcdo == ptr._rcdo; }
    [[nodiscard]] bool operator !=(const ReferenceCountingPointer<T>& ptr) const noexcept { return _rcdo != ptr._rcdo; }

    [[nodiscard]] bool operator ==(const Type* ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const Type* ptr) const noexcept override;

    [[nodiscard]] bool operator ==(nullptr_t) const noexcept override { return !TAU_CHECK_RC(_rcdo) || !_rcdo->m_RefCount.load(); }
    [[nodiscard]] bool operator !=(nullptr_t) const noexcept override { return  TAU_CHECK_RC(_rcdo) &&  _rcdo->m_RefCount.load(); }
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

    using Type = typename SWRC<T>::Type;
public:
    [[nodiscard]] static constexpr uSys AllocSize() noexcept;
private:
    SWRC<T>* _swrc;
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
    void Reset(Allocator& allocator, Args&&... args) noexcept;

    template<typename Arg0, typename... Args, ::std::enable_if_t<!::std::is_base_of_v<TauAllocator, Arg0>, int> = 0>
    void Reset(Arg0&& arg0, Args&&... args) noexcept;

    [[nodiscard]]       Type& operator  *()                override { return *_swrc->ObjPtr(); }
    [[nodiscard]] const Type& operator  *() const          override { return *_swrc->ObjPtr(); }
    [[nodiscard]]       Type* operator ->()       noexcept override { return  TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }
    [[nodiscard]] const Type* operator ->() const noexcept override { return  TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }

    [[nodiscard]]       Type* Get()       noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }
    [[nodiscard]] const Type* Get() const noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }

    [[nodiscard]] uSys RefCount()       const noexcept override { return TAU_CHECK_RC(_swrc) ? _swrc->m_StrongRefCount.load() : static_cast<uSys>(0); }
    [[nodiscard]] uSys StrongRefCount() const noexcept          { return TAU_CHECK_RC(_swrc) ? _swrc->m_StrongRefCount.load() : static_cast<uSys>(0); }
    [[nodiscard]] uSys WeakRefCount()   const noexcept          { return TAU_CHECK_RC(_swrc) ? _swrc->m_WeakRefCount.load()   : static_cast<uSys>(0); }

    [[nodiscard]] SWRC<T>*& _getBlock()       noexcept { return _swrc; }
    [[nodiscard]] SWRC<T>*  _getBlock() const noexcept { return _swrc; }

    [[nodiscard]] operator bool() const noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount; }

    [[nodiscard]] bool operator ==(const StrongReferenceCountingPointer<T>& ptr) const noexcept;
    [[nodiscard]] bool operator !=(const StrongReferenceCountingPointer<T>& ptr) const noexcept;

    [[nodiscard]] bool operator ==(const WeakReferenceCountingPointer<T>& ptr) const noexcept;
    [[nodiscard]] bool operator !=(const WeakReferenceCountingPointer<T>& ptr) const noexcept;

    [[nodiscard]] bool operator ==(const Type* ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const Type* ptr) const noexcept override;

    [[nodiscard]] bool operator ==(nullptr_t) const noexcept override { return !TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount.load(); }
    [[nodiscard]] bool operator !=(nullptr_t) const noexcept override { return  TAU_CHECK_RC(_swrc) &&  _swrc->m_StrongRefCount.load(); }
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

    using Type = typename SWRC<T>::Type;
public:
    [[nodiscard]] static constexpr uSys AllocSize() noexcept;
private:
    SWRC<T>* _swrc;
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

    [[nodiscard]]       T& operator  *()                override { return *_swrc->ObjPtr(); }
    [[nodiscard]] const T& operator  *() const          override { return *_swrc->ObjPtr(); }
    [[nodiscard]]       T* operator ->()       noexcept override { return  TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }
    [[nodiscard]] const T* operator ->() const noexcept override { return  TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }

    [[nodiscard]]       T* Get()       noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }
    [[nodiscard]] const T* Get() const noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load() ? _swrc->ObjPtr() : nullptr; }

    [[nodiscard]] uSys RefCount()       const noexcept override { return TAU_CHECK_RC(_swrc) ? _swrc->m_WeakRefCount.load() : static_cast<uSys>(0); }
    [[nodiscard]] uSys StrongRefCount() const noexcept          { return TAU_CHECK_RC(_swrc) ? _swrc->m_StrongRefCount.load() : static_cast<uSys>(0); }
    [[nodiscard]] uSys WeakRefCount()   const noexcept          { return TAU_CHECK_RC(_swrc) ? _swrc->m_WeakRefCount.load() : static_cast<uSys>(0); }

    [[nodiscard]] SWRC<T>*& _getBlock()       noexcept { return _swrc; }
    [[nodiscard]] SWRC<T>*  _getBlock() const noexcept { return _swrc; }

    [[nodiscard]] operator bool() const noexcept override { return TAU_CHECK_RC(_swrc) && _swrc->m_StrongRefCount.load(); }

    [[nodiscard]] bool operator ==(const StrongReferenceCountingPointer<T>& ptr) const noexcept { return _swrc == ptr._swrc; }
    [[nodiscard]] bool operator !=(const StrongReferenceCountingPointer<T>& ptr) const noexcept { return _swrc != ptr._swrc; }

    [[nodiscard]] bool operator ==(const WeakReferenceCountingPointer<T>& ptr) const noexcept { return _swrc == ptr._swrc; }
    [[nodiscard]] bool operator !=(const WeakReferenceCountingPointer<T>& ptr) const noexcept { return _swrc != ptr._swrc; }

    [[nodiscard]] bool operator ==(const T* ptr) const noexcept override;
    [[nodiscard]] bool operator !=(const T* ptr) const noexcept override;

    [[nodiscard]] bool operator ==(const nullptr_t) const noexcept override { return !TAU_CHECK_RC(_swrc) || !_swrc->m_StrongRefCount.load(); }
    [[nodiscard]] bool operator !=(const nullptr_t) const noexcept override { return  TAU_CHECK_RC(_swrc) &&  _swrc->m_StrongRefCount.load(); }
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
