#pragma once

#pragma warning(push, 0)
#include <cstring>
#include <iterator>
#pragma warning(pop)

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "allocator/TauAllocator.hpp"
#include "ReferenceCounter.hpp"

template<typename T>
class ConstRefDynArray;

template<typename T>
class DynArrayIterator final
{
    DEFAULT_DESTRUCT(DynArrayIterator);
    DEFAULT_CM_PU(DynArrayIterator);
public:
    DynArrayIterator(T* const arr, const uSys length, const uSys index) noexcept
        : m_Array(arr)
        , m_Length(length)
        , m_Index(index)
    { }

    DynArrayIterator<T>& operator++() noexcept
    {
        if(m_Index < m_Length)
        { ++m_Index; }
        return *this;
    }

    DynArrayIterator<T>& operator--() noexcept
    {
        if(m_Index > 0)
        { --m_Index; }
        return *this;
    }

    [[nodiscard]] DynArrayIterator<T> operator++(int) noexcept
    {
        const DynArrayIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] DynArrayIterator<T> operator--(int) noexcept
    {
        const DynArrayIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] T& operator*() noexcept
    { return m_Array[m_Index]; }

    [[nodiscard]] const T& operator*() const noexcept
    { return m_Array[m_Index]; }
    
    [[nodiscard]] bool operator ==(const DynArrayIterator<T>& other) const noexcept
    { return m_Index == other.m_Index; }
    [[nodiscard]] bool operator !=(const DynArrayIterator<T>& other) const noexcept
    { return m_Index != other.m_Index; }
private:
    T* m_Array;
    uSys m_Length;
    uSys m_Index;
};

template<typename T>
class ConstDynArrayIterator final
{
    DEFAULT_DESTRUCT(ConstDynArrayIterator);
    DEFAULT_CM_PU(ConstDynArrayIterator);
public:
    ConstDynArrayIterator(T* const arr, const uSys length, const uSys index) noexcept
        : m_Array(arr)
        , m_Length(length)
        , m_Index(index)
    { }

    ConstDynArrayIterator<T>& operator++() noexcept
    {
        if(m_Index < m_Length)
        { ++m_Index; }
        return *this;
    }

    ConstDynArrayIterator<T>& operator--() noexcept
    {
        if(m_Index > 0)
        { --m_Index; }
        return *this;
    }

    [[nodiscard]] ConstDynArrayIterator<T> operator++(int) noexcept
    {
        const ConstDynArrayIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] ConstDynArrayIterator<T> operator--(int) noexcept
    {
        const ConstDynArrayIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] const T& operator*() const noexcept
    { return m_Array[m_Index]; }
    
    [[nodiscard]] bool operator ==(const ConstDynArrayIterator<T>& other) const noexcept
    { return m_Index == other.m_Index; }
    [[nodiscard]] bool operator !=(const ConstDynArrayIterator<T>& other) const noexcept
    { return m_Index != other.m_Index; }
private:
    T* m_Array;
    uSys m_Length;
    uSys m_Index;
};

template<typename T>
class RefDynArrayIterator final
{
public:
    RefDynArrayIterator(T* const arr, const ReferenceCounter& refCount, const uSys length, const uSys index) noexcept
        : m_Array(arr)
        , m_RefCount(refCount)
        , m_Length(length)
        , m_Index(index)
    { }

    ~RefDynArrayIterator() noexcept
    {
        OnDestroy();
    }

    RefDynArrayIterator(const RefDynArrayIterator& copy) noexcept
        : m_Array(copy.m_Array)
        , m_RefCount(copy.m_RefCount)
        , m_Length(copy.m_Length)
        , m_Index(copy.m_Index)
    { }

    RefDynArrayIterator(RefDynArrayIterator&& move) noexcept
        : m_Array(move.m_Array)
        , m_RefCount(::std::move(move.m_RefCount))
        , m_Length(move.m_Length)
        , m_Index(move.m_Index)
    {
        move.m_Array = nullptr;
    }

    RefDynArrayIterator& operator=(const RefDynArrayIterator& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        OnDestroy();

        m_Array = copy.m_Array;
        m_RefCount = copy.m_RefCount;
        m_Length = copy.m_Length;
        m_Index = copy.m_Index;

        return *this;
    }

    RefDynArrayIterator& operator=(RefDynArrayIterator&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        OnDestroy();

        m_Array = move.m_Array;
        m_RefCount = ::std::move(move.m_RefCount);
        m_Length = move.m_Length;
        m_Index = move.m_Index;

        move.m_Array = nullptr;

        return *this;
    }

    RefDynArrayIterator<T>& operator++() noexcept
    {
        if(m_Index < m_Length)
        { ++m_Index; }
        return *this;
    }

    RefDynArrayIterator<T>& operator--() noexcept
    {
        if(m_Index > 0)
        { --m_Index; }
        return *this;
    }

    [[nodiscard]] RefDynArrayIterator<T> operator++(int) noexcept
    {
        const RefDynArrayIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] RefDynArrayIterator<T> operator--(int) noexcept
    {
        const RefDynArrayIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] T& operator*() noexcept
    { return m_Array[m_Index]; }

    [[nodiscard]] const T& operator*() const noexcept
    { return m_Array[m_Index]; }
    
    [[nodiscard]] bool operator ==(const RefDynArrayIterator<T>& other) const noexcept
    { return m_Index == other.m_Index; }
    [[nodiscard]] bool operator !=(const RefDynArrayIterator<T>& other) const noexcept
    { return m_Index != other.m_Index; }
private:
    void OnDestroy() noexcept
    {
        if(m_RefCount.IsAboutToBeDestroyed())
        {
            ::TauUtilsDestructTArr(m_Array, m_Length);
            ::TauUtilsDeallocate(reinterpret_cast<ReferenceCounter::Type*>(m_Array) - 1);
        }
    }
private:
    T* m_Array;
    ReferenceCounter m_RefCount;
    uSys m_Length;
    uSys m_Index;
};

template<typename T>
class ConstRefDynArrayIterator final
{
public:
    ConstRefDynArrayIterator(T* const arr, const ReferenceCounter& refCount, const uSys length, const uSys index) noexcept
        : m_Array(arr)
        , m_RefCount(refCount)
        , m_Length(length)
        , m_Index(index)
    { }

    ~ConstRefDynArrayIterator() noexcept
    {
        OnDestroy();
    }

    ConstRefDynArrayIterator(const ConstRefDynArrayIterator<T>& copy) noexcept
        : m_Array(copy.m_Array)
        , m_RefCount(copy.m_RefCount)
        , m_Length(copy.m_Length)
        , m_Index(copy.m_Index)
    { }

    ConstRefDynArrayIterator(ConstRefDynArrayIterator<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_RefCount(::std::move(move.m_RefCount))
        , m_Length(move.m_Length)
        , m_Index(move.m_Index)
    {
        move.m_Array = nullptr;
    }

    ConstRefDynArrayIterator<T>& operator=(const ConstRefDynArrayIterator<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        OnDestroy();

        m_Array = copy.m_Array;
        m_RefCount = copy.m_RefCount;
        m_Length = copy.m_Length;
        m_Index = copy.m_Index;

        return *this;
    }

    ConstRefDynArrayIterator<T>& operator=(ConstRefDynArrayIterator<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        OnDestroy();

        m_Array = move.m_Array;
        m_RefCount = ::std::move(move.m_RefCount);
        m_Length = move.m_Length;
        m_Index = move.m_Index;

        move.m_Array = nullptr;

        return *this;
    }

    ConstRefDynArrayIterator<T>& operator++() noexcept
    {
        if(m_Index < m_Length)
        { ++m_Index; }
        return *this;
    }

    ConstRefDynArrayIterator<T>& operator--() noexcept
    {
        if(m_Index > 0)
        { --m_Index; }
        return *this;
    }

    [[nodiscard]] ConstRefDynArrayIterator<T> operator++(int) noexcept
    {
        const ConstRefDynArrayIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] ConstRefDynArrayIterator<T> operator--(int) noexcept
    {
        const ConstRefDynArrayIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] const T& operator*() const noexcept
    { return m_Array[m_Index]; }
    
    [[nodiscard]] bool operator ==(const ConstRefDynArrayIterator<T>& other) const noexcept
    { return m_Index == other.m_Index; }
    [[nodiscard]] bool operator !=(const ConstRefDynArrayIterator<T>& other) const noexcept
    { return m_Index != other.m_Index; }
private:
    void OnDestroy() noexcept
    {
        if(m_RefCount.IsAboutToBeDestroyed())
        {
            ::TauUtilsDestructTArr(m_Array, m_Length);
            ::TauUtilsDeallocate(reinterpret_cast<ReferenceCounter::Type*>(m_Array) - 1);
        }
    }
private:
    T* m_Array;
    ReferenceCounter m_RefCount;
    uSys m_Length;
    uSys m_Index;
};

template<typename T>
class DynArray final
{
public:
    DynArray() noexcept
        : m_Array(nullptr)
        , m_Length(0)
    { }

    explicit DynArray(const uSys length) noexcept
        : m_Array(TU_NEW_ARR(T, length))
        , m_Length(length)
    { }

    ~DynArray() noexcept
    { TU_DELETE_T_ARR(m_Array, m_Length); }

    DynArray(const DynArray<T>& copy) noexcept
        : m_Array(TU_NEW_ARR(T, copy.m_Length))
        , m_Length(copy.m_Length)
    { (void) ::std::copy(copy.m_Array, copy.m_Array + copy.m_Length, m_Array); }

    DynArray(DynArray<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_Length(move.m_Length)
    { move.m_Array = nullptr; }

    DynArray<T>& operator=(const DynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        TU_DELETE_T_ARR(m_Array, m_Length);

        m_Array = TU_NEW_ARR(T, copy.m_Length);
        m_Length = copy.m_Length;

        (void) ::std::copy(copy.m_Array, copy.m_Array + copy.m_Length, m_Array);
        
        return *this;
    }

    DynArray<T>& operator=(DynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        TU_DELETE_T_ARR(m_Array, m_Length);

        m_Array = move.m_Array;
        m_Length = move.m_Length;

        move.m_Array = nullptr;

        return *this;
    }

    [[nodiscard]] operator const T*() const noexcept { return m_Array; }
    [[nodiscard]] operator       T*()       noexcept { return m_Array; }

    [[nodiscard]] const T* arr() const noexcept { return m_Array; }
    [[nodiscard]]       T* arr()       noexcept { return m_Array; }

    [[nodiscard]] const T* Array() const noexcept { return m_Array; }
    [[nodiscard]]       T* Array()       noexcept { return m_Array; }

    [[nodiscard]] uSys   size() const noexcept { return m_Length; }
    [[nodiscard]] uSys length() const noexcept { return m_Length; }
    [[nodiscard]] uSys  count() const noexcept { return m_Length; }

    [[nodiscard]] uSys   Size() const noexcept { return m_Length; }
    [[nodiscard]] uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] uSys  Count() const noexcept { return m_Length; }

    [[nodiscard]] operator uSys() const { return m_Length; }

    [[nodiscard]] DynArrayIterator<T> begin() noexcept { return DynArrayIterator<T>(m_Array, m_Length, 0);     }
    [[nodiscard]] DynArrayIterator<T>   end() noexcept { return DynArrayIterator<T>(m_Array, m_Length, m_Length); }

    [[nodiscard]] ConstDynArrayIterator<T> begin() const noexcept { return ConstDynArrayIterator<T>(m_Array, m_Length, 0);     }
    [[nodiscard]] ConstDynArrayIterator<T>   end() const noexcept { return ConstDynArrayIterator<T>(m_Array, m_Length, m_Length); }

    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>> rbegin() noexcept { return ::std::reverse_iterator(end());   }
    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>>   rend() noexcept { return ::std::reverse_iterator(begin()); }

    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator(end());   }
    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator(begin()); }

    void Zero() noexcept
    {
        (void) ::std::memset(m_Array, 0, sizeof(T) * m_Length);
    }

    void DestructCount(const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i].~T();
        }
    }

    void DestructAll() noexcept { DestructCount(m_Length); }

    void MemCpyCount(const T& t, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            (void) ::std::memcpy(&m_Array[i], &t, sizeof(t));
        }
    }

    void MemCpyCount(T* const buffer, const uSys count) const noexcept
    {
        MemCpyCountTo(buffer, count);
    }

    void MemCpyCount(const T* const buffer, const uSys count) noexcept
    {
        MemCpyCountFrom(buffer, count);
    }

    void MemCpyCountTo(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        (void) ::std::memcpy(buffer, m_Array, trueCount * sizeof(T));
    }

    void MemCpyCountFrom(const T* const buffer, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        (void) ::std::memcpy(m_Array, buffer, trueCount * sizeof(T));
    }

    void MemCpyAll(const T& t) noexcept { MemCpyCount(t, m_Length); }
    void MemCpyAll(T* const t) const noexcept { MemCpyCountTo(t, m_Length); }
    void MemCpyAll(const T* const t) noexcept { MemCpyCountFrom(t, m_Length); }
    void MemCpyAllTo(T* const t) const noexcept { MemCpyCountTo(t, m_Length); }
    void MemCpyAllFrom(const T* const t) noexcept { MemCpyCountFrom(t, m_Length); }

    void SetCount(const T& t, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i] = t;
        }
    }

    void SetCountTo(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            buffer[i] = m_Array[i];
        }
    }

    void SetCountFrom(T* const buffer, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i] = buffer[i];
        }
    }

    void SetCount(T* const buffer, const uSys count) const noexcept
    {
        SetCountTo(buffer, count);
    }

    void SetCount(T* const buffer, const uSys count) noexcept
    {
        SetCountFrom(buffer, count);
    }

    void SetAll(const T& t) noexcept { SetCount(t, m_Length); }
    void SetAll(T* const t) const noexcept { SetCountTo(t, m_Length); }
    void SetAll(const T* const t) noexcept { SetCountFrom(t, m_Length); }
    void SetAllTo(T* const t) const noexcept { SetCountTo(t, m_Length); }
    void SetAllFrom(const T* const t) noexcept { SetCountFrom(t, m_Length); }
public:
    static DynArray<T> MakeRaw(const uSys length) noexcept
    {
        return DynArray(::TauUtilsAllocate(sizeof(T) * length), length);
    }
private:
    DynArray(T* const array, const uSys length) noexcept
        : m_Array(array)
        , m_Length(length)
    { }
private:
    T* m_Array;
    uSys m_Length;
};

template<typename T>
class RefDynArray final
{
public:
    RefDynArray() noexcept
        : m_Array(0)
        , m_Length(0)
        , m_RefCount(nullptr)
    { }

    explicit RefDynArray(const uSys size) noexcept
        : m_Length(size)
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + sizeof(T) * size);
        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        m_RefCount = ReferenceCounter(refCount);
        m_Array = ::new(refCount + 1) T[size];
    }

    ~RefDynArray() noexcept
    {
        OnDestroy();
    }

    RefDynArray(const RefDynArray<T>& copy) noexcept
        : m_Array(copy.m_Array)
        , m_Length(copy.m_Length)
        , m_RefCount(copy.m_RefCount)
    { }

    RefDynArray(RefDynArray<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_Length(move.m_Length)
        , m_RefCount(::std::move(move.m_RefCount))
    {
        move.m_Array = nullptr;
    }

    RefDynArray<T>& operator =(const RefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        OnDestroy();

        m_Array = copy.m_Array;
        m_Length = copy.m_Length;
        m_RefCount = copy.m_RefCount;
        
        return *this;
    }

    RefDynArray<T>& operator =(RefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        OnDestroy();

        m_Array = move.m_Array;
        m_Length = move.m_Length;
        m_RefCount = ::std::move(move.m_RefCount);

        move.m_Array = nullptr;

        return *this;
    }

    [[nodiscard]] operator const T*() const noexcept { return m_Array; }
    [[nodiscard]] operator       T*()       noexcept { return m_Array; }

    [[nodiscard]] const T* arr() const noexcept { return m_Array; }
    [[nodiscard]]       T* arr()       noexcept { return m_Array; }

    [[nodiscard]] const T* Array() const noexcept { return m_Array; }
    [[nodiscard]]       T* Array()       noexcept { return m_Array; }

    [[nodiscard]] uSys size()   const noexcept { return m_Length; }
    [[nodiscard]] uSys length() const noexcept { return m_Length; }
    [[nodiscard]] uSys count()  const noexcept { return m_Length; }

    [[nodiscard]] uSys Size()   const noexcept { return m_Length; }
    [[nodiscard]] uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] uSys Count()  const noexcept { return m_Length; }

    [[nodiscard]] operator uSys() const { return m_Length; }

    [[nodiscard]] RefDynArrayIterator<T> begin() noexcept { return RefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, 0); }
    [[nodiscard]] RefDynArrayIterator<T>   end() noexcept { return RefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, m_Length); }

    [[nodiscard]] ConstRefDynArrayIterator<T> begin() const noexcept { return ConstRefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, 0); }
    [[nodiscard]] ConstRefDynArrayIterator<T>   end() const noexcept { return ConstRefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, m_Length); }

    [[nodiscard]] ::std::reverse_iterator<RefDynArrayIterator<T>> rbegin() noexcept { return ::std::reverse_iterator(end());   }
    [[nodiscard]] ::std::reverse_iterator<RefDynArrayIterator<T>>   rend() noexcept { return ::std::reverse_iterator(begin()); }

    [[nodiscard]] ::std::reverse_iterator<ConstRefDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator(end()); }
    [[nodiscard]] ::std::reverse_iterator<ConstRefDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator(begin()); }

    void Zero() noexcept
    {
        (void) ::std::memset(m_Array, 0, sizeof(T) * m_Length);
    }

    void DestructCount(const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i].~T();
        }
    }

    void DestructAll() noexcept { DestructCount(m_Length); }

    void MemCpyCount(const T& t, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            (void) ::std::memcpy(&m_Array[i], &t, sizeof(t));
        }
    }

    void MemCpyCount(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        (void) ::std::memcpy(buffer, m_Array, trueCount * sizeof(T));
    }

    void MemCpyCount(const T* const buffer, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        (void) ::std::memcpy(m_Array, buffer, trueCount * sizeof(T));
    }

    void MemCpyAll(const T& t) noexcept { MemCpyCount(t, m_Length); }
    void MemCpyAll(T* const t) const noexcept { MemCpyCount(t, m_Length); }
    void MemCpyAll(const T* const t) noexcept { MemCpyCount(t, m_Length); }

    void SetCount(const T& t, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i] = t;
        }
    }

    void SetCount(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            buffer[i] = m_Array[i];
        }
    }

    void SetCount(T* const buffer, const uSys count) noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            m_Array[i] = buffer[i];
        }
    }

    void SetAll(const T& t) noexcept { SetCount(t, m_Length); }
    void SetAll(T* const t) const noexcept { SetCount(t, m_Length); }
    void SetAll(const T* const t) noexcept { SetCount(t, m_Length); }
public:
    static RefDynArray<T> MakeRaw(const uSys length) noexcept
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + sizeof(T) * length);
        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        T* const array = static_cast<T*>(refCount + 1);

        return RefDynArray(array, length, refCount);
    }
private:
    RefDynArray(T* const array, const uSys length, ReferenceCounter::Type* const refCount) noexcept
        : m_Array(array)
        , m_Length(length)
        , m_RefCount(refCount)
    { }
private:
    void OnDestroy() noexcept
    {
        if(m_RefCount.IsAboutToBeDestroyed())
        {
            ::TauUtilsDestructTArr(m_Array, m_Length);
            ::TauUtilsDeallocate(reinterpret_cast<ReferenceCounter::Type*>(m_Array) - 1);
        }
    }
private:
    T* m_Array;
    uSys m_Length;
    ReferenceCounter m_RefCount;
private:
    friend class ConstRefDynArray<T>;
};

template<typename T>
class ConstRefDynArray final
{
public:
    ConstRefDynArray(const RefDynArray<T>& copy) noexcept
        : m_Array(copy.m_Array)
        , m_Length(copy.m_Length)
        , m_RefCount(copy.m_RefCount)
    { }

    ConstRefDynArray(RefDynArray<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_Length(move.m_Length)
        , m_RefCount(::std::move(move.m_RefCount))
    {
        move.m_Array = nullptr;
    }

    ~ConstRefDynArray() noexcept
    {
        OnDestroy();
    }

    ConstRefDynArray(const ConstRefDynArray<T>& copy) noexcept
        : m_Array(copy.m_Array)
        , m_Length(copy.m_Length)
        , m_RefCount(copy.m_RefCount)
    { }

    ConstRefDynArray(ConstRefDynArray<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_Length(move.m_Length)
        , m_RefCount(::std::move(move.m_RefCount))
    {
        move.m_Array = nullptr;
    }

    ConstRefDynArray<T>& operator=(const ConstRefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        OnDestroy();

        m_Array = copy.m_Array;
        m_Length = copy.m_Length;
        m_RefCount = copy.m_RefCount;

        return *this;
    }

    ConstRefDynArray<T>& operator =(ConstRefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        OnDestroy();

        m_Array = move.m_Array;
        m_Length = move.m_Length;
        m_RefCount = ::std::move(move.m_RefCount);

        move.m_Array = nullptr;

        return *this;
    }

    ConstRefDynArray<T>& operator =(const RefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        OnDestroy();

        m_Array = copy.m_Array;
        m_Length = copy.m_Length;
        m_RefCount = copy.m_RefCount;
        
        return *this;
    }

    ConstRefDynArray<T>& operator=(RefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        OnDestroy();

        m_Array = move.m_Array;
        m_Length = move.m_Length;
        m_RefCount = ::std::move(move.m_RefCount);

        move.m_Array = nullptr;

        return *this;
    }

    [[nodiscard]] operator const T*() const noexcept { return m_Array; }

    [[nodiscard]] const T* arr() const noexcept { return m_Array; }

    [[nodiscard]] const T* Array() const noexcept { return m_Array; }

    [[nodiscard]] uSys size()   const noexcept { return m_Length; }
    [[nodiscard]] uSys length() const noexcept { return m_Length; }
    [[nodiscard]] uSys count()  const noexcept { return m_Length; }

    [[nodiscard]] uSys Size()   const noexcept { return m_Length; }
    [[nodiscard]] uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] uSys Count()  const noexcept { return m_Length; }

    [[nodiscard]] operator uSys() const { return m_Length; }

    [[nodiscard]] ConstRefDynArrayIterator<T> begin() const noexcept { return ConstRefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, 0); }
    [[nodiscard]] ConstRefDynArrayIterator<T>   end() const noexcept { return ConstRefDynArrayIterator<T>(m_Array, m_RefCount, m_Length, m_Length); }

    [[nodiscard]] ::std::reverse_iterator<ConstRefDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator(end()); }
    [[nodiscard]] ::std::reverse_iterator<ConstRefDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator(begin()); }
private:
    void OnDestroy() noexcept
    {
        if(m_RefCount.IsAboutToBeDestroyed())
        {
            ::TauUtilsDestructTArr(m_Array, m_Length);
            ::TauUtilsDeallocate(reinterpret_cast<ReferenceCounter::Type*>(m_Array) - 1);
        }
    }
private:
    T* m_Array;
    uSys m_Length;
    ReferenceCounter m_RefCount;
};

template<typename T>
class ArrayWrapper final
{
    DEFAULT_DESTRUCT(ArrayWrapper);
    DEFAULT_CM_PU(ArrayWrapper);
private:
    T* m_Array;
    uSys m_Length;
public:
    ArrayWrapper(T* const array, const uSys length) noexcept
        : m_Array(array)
        , m_Length(length)
    { }

    [[nodiscard]] operator const T*() const noexcept { return m_Array; }
    [[nodiscard]] operator       T*()       noexcept { return m_Array; }

    [[nodiscard]] const T* Array() const noexcept { return m_Array; }
    [[nodiscard]]       T* Array()       noexcept { return m_Array; }

    [[nodiscard]] uSys   Size() const noexcept { return m_Length; }
    [[nodiscard]] uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] uSys  Count() const noexcept { return m_Length; }

    [[nodiscard]] operator uSys() const { return m_Length; }

    [[nodiscard]] DynArrayIterator<T> begin() noexcept { return DynArrayIterator<T>(m_Array, m_Length, 0);        }
    [[nodiscard]] DynArrayIterator<T>   end() noexcept { return DynArrayIterator<T>(m_Array, m_Length, m_Length); }

    [[nodiscard]] ConstDynArrayIterator<T> begin() const noexcept { return ConstDynArrayIterator<T>(m_Array, m_Length, 0);        }
    [[nodiscard]] ConstDynArrayIterator<T>   end() const noexcept { return ConstDynArrayIterator<T>(m_Array, m_Length, m_Length); }

    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>> rbegin() noexcept { return ::std::reverse_iterator(end());   }
    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>>   rend() noexcept { return ::std::reverse_iterator(begin()); }

    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator(end());   }
    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator(begin()); }

    void MemCpyCount(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        (void) ::std::memcpy(buffer, m_Array, trueCount * sizeof(T));
    }

    void MemCpyAll(T* const t) const noexcept { MemCpyCount(t, m_Length); }

    void SetCount(T* const buffer, const uSys count) const noexcept
    {
        const uSys trueCount = minT(count, m_Length);
        for(uSys i = 0; i < trueCount; ++i)
        {
            buffer[i] = m_Array[i];
        }
    }

    void SetAll(T* const t) const noexcept { SetCount(t, m_Length); }
};
