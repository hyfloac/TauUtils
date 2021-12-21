#pragma once

#pragma warning(push, 0)
#include <cstring>
#include <new>
#include <iterator>
#pragma warning(pop)

#include "NumTypes.hpp"
#include "Objects.hpp"

template<typename T>
class ConstRefDynArray;

template<typename T>
class DynArrayIterator final
{
    DEFAULT_DESTRUCT(DynArrayIterator);
    DEFAULT_CM_PU(DynArrayIterator);
private:
    T* _arr;
    uSys _length;
    uSys _index;
public:
    DynArrayIterator(T* const arr, const uSys length, const uSys index) noexcept
        : _arr(arr)
        , _length(length)
        , _index(index)
    { }

    DynArrayIterator<T>& operator++() noexcept
    {
        if(_index < _length)
        { ++_index; }
        return *this;
    }

    DynArrayIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
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
    { return _arr[_index]; }

    [[nodiscard]] const T& operator*() const noexcept
    { return _arr[_index]; }
    
    [[nodiscard]] bool operator ==(const DynArrayIterator<T>& other) const noexcept
    { return _index == other._index; }
    [[nodiscard]] bool operator !=(const DynArrayIterator<T>& other) const noexcept
    { return _index != other._index; }
};

template<typename T>
class ConstDynArrayIterator final
{
    DEFAULT_DESTRUCT(ConstDynArrayIterator);
    DEFAULT_CM_PU(ConstDynArrayIterator);
private:
    T* _arr;
    uSys _length;
    uSys _index;
public:
    ConstDynArrayIterator(T* const arr, const uSys length, const uSys index) noexcept
        : _arr(arr)
        , _length(length)
        , _index(index)
    { }

    ConstDynArrayIterator<T>& operator++() noexcept
    {
        if(_index < _length)
        { ++_index; }
        return *this;
    }

    ConstDynArrayIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
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
    { return _arr[_index]; }
    
    [[nodiscard]] bool operator ==(const ConstDynArrayIterator<T>& other) const noexcept
    { return _index == other._index; }
    [[nodiscard]] bool operator !=(const ConstDynArrayIterator<T>& other) const noexcept
    { return _index != other._index; }
};

template<typename T>
class RefDynArrayIterator final
{
private:
    T* _arr;
    uSys* _refCount;
    uSys _length;
    uSys _index;
public:
    RefDynArrayIterator(T* const arr, uSys* const refCount, const uSys length, const uSys index) noexcept
        : _arr(arr)
        , _refCount(refCount)
        , _length(length)
        , _index(index)
    { ++(*_refCount); }

    ~RefDynArrayIterator() noexcept
    {
        if(_refCount && --(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }
    }

    RefDynArrayIterator(const RefDynArrayIterator& copy) noexcept
        : _arr(copy._arr)
        , _refCount(copy._refCount)
        , _length(copy._length)
        , _index(copy._index)
    { ++(*_refCount); }

    RefDynArrayIterator(RefDynArrayIterator&& move) noexcept
        : _arr(move._arr)
        , _refCount(move._refCount)
        , _length(move._length)
        , _index(move._index)
    {
        move._arr = nullptr;
        move._refCount = nullptr;
    }

    RefDynArrayIterator& operator=(const RefDynArrayIterator& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = copy._arr;
        _refCount = copy._refCount;
        _length = copy._length;
        _index = copy._index;

        ++(*_refCount);

        return *this;
    }

    RefDynArrayIterator& operator=(RefDynArrayIterator&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = move._arr;
        _refCount = move._refCount;
        _length = move._length;
        _index = move._index;

        move._arr = nullptr;
        move._refCount = nullptr;

        return *this;
    }

    RefDynArrayIterator<T>& operator++() noexcept
    {
        if(_index < _length)
        { ++_index; }
        return *this;
    }

    RefDynArrayIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
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
    { return _arr[_index]; }

    [[nodiscard]] const T& operator*() const noexcept
    { return _arr[_index]; }
    
    [[nodiscard]] bool operator ==(const RefDynArrayIterator<T>& other) const noexcept
    { return _index == other._index; }
    [[nodiscard]] bool operator !=(const RefDynArrayIterator<T>& other) const noexcept
    { return _index != other._index; }
};

template<typename T>
class ConstRefDynArrayIterator final
{
private:
    T* _arr;
    uSys* _refCount;
    uSys _length;
    uSys _index;
public:
    ConstRefDynArrayIterator(T* const arr, uSys* refCount, const uSys length, const uSys index) noexcept
        : _arr(arr)
        , _refCount(refCount)
        , _length(length)
        , _index(index)
    { ++(*_refCount); }

    ~ConstRefDynArrayIterator() noexcept
    {
        if(_refCount && --(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }
    }

    ConstRefDynArrayIterator(const ConstRefDynArrayIterator<T>& copy) noexcept
        : _arr(copy._arr)
        , _refCount(copy._refCount)
        , _length(copy._length)
        , _index(copy._index)
    { ++(*_refCount); }

    ConstRefDynArrayIterator(ConstRefDynArrayIterator<T>&& move) noexcept
        : _arr(move._arr)
        , _refCount(move._refCount)
        , _length(move._length)
        , _index(move._index)
    {
        move._arr = nullptr;
        move._refCount = nullptr;
    }

    ConstRefDynArrayIterator<T>& operator=(const ConstRefDynArrayIterator<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = copy._arr;
        _refCount = copy._refCount;
        _length = copy._length;
        _index = copy._index;

        ++(*_refCount);

        return *this;
    }

    ConstRefDynArrayIterator<T>& operator=(ConstRefDynArrayIterator<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = move._arr;
        _refCount = move._refCount;
        _length = move._length;
        _index = move._index;

        move._arr = nullptr;
        move._refCount = nullptr;

        return *this;
    }

    ConstRefDynArrayIterator<T>& operator++() noexcept
    {
        if(_index < _length)
        { ++_index; }
        return *this;
    }

    ConstRefDynArrayIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
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
    { return _arr[_index]; }
    
    [[nodiscard]] bool operator ==(const ConstRefDynArrayIterator<T>& other) const noexcept
    { return _index == other._index; }
    [[nodiscard]] bool operator !=(const ConstRefDynArrayIterator<T>& other) const noexcept
    { return _index != other._index; }
};

template<typename T>
class DynArray final
{
private:
    T* m_Array;
    uSys m_Length;
public:
    explicit DynArray(const uSys length = 0)
        : m_Array(new(::std::nothrow) T[length])
        , m_Length(length)
    { }

    ~DynArray()
    { delete[] m_Array; }

    DynArray(const DynArray<T>& copy)
        : m_Array(new(::std::nothrow) T[copy.m_Length])
        , m_Length(copy.m_Length)
    { ::std::memcpy(m_Array, copy.m_Array, copy.m_Length * sizeof(T)); }

    DynArray(DynArray<T>&& move) noexcept
        : m_Array(move.m_Array)
        , m_Length(move.m_Length)
    { move.m_Array = nullptr; }

    DynArray<T>& operator =(const DynArray<T>& copy)
    {
        if(this == &copy)
        { return *this; }

        delete[] m_Array;

        m_Array = new(::std::nothrow) T[copy.m_Length];
        m_Length = copy.m_Length;

        ::std::memcpy(m_Array, copy.m_Array, copy.m_Length * sizeof(T));

        return *this;
    }

    DynArray<T>& operator =(DynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        delete[] m_Array;

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

    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>> rbegin() noexcept { return ::std::reverse_iterator<DynArrayIterator<T>>(m_Array, m_Length, 0);            }
    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>>   rend() noexcept { return ::std::reverse_iterator<DynArrayIterator<T>>(m_Array, m_Length, m_Length - 1); }

    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator<ConstDynArrayIterator<T>>(m_Array, m_Length, 0);            }
    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator<ConstDynArrayIterator<T>>(m_Array, m_Length, m_Length - 1); }
};

template<typename T>
class RefDynArray final
{
private:
    T* _arr;
    uSys _size;
    uSys* _refCount;
public:
    explicit RefDynArray(const uSys size = 0) noexcept
        : _arr(new(::std::nothrow) T[size])
        , _size(size)
        , _refCount(new(::std::nothrow) uSys(1))
    { }

    ~RefDynArray() noexcept
    {
        if(_refCount && --(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }
    }

    RefDynArray(const RefDynArray<T>& copy) noexcept
        : _arr(copy._arr)
        , _size(copy._size)
        , _refCount(copy._refCount)
    { ++(*_refCount); }

    RefDynArray(RefDynArray<T>&& move) noexcept
        : _arr(move._arr)
        , _size(move._size)
        , _refCount(move._refCount)
    {
        move._arr = nullptr;
        move._refCount = nullptr;
    }

    RefDynArray<T>& operator =(const RefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = copy._arr;
        _size = copy._size;
        _refCount = copy._refCount;

        ++(*_refCount);

        return *this;
    }

    RefDynArray<T>& operator =(RefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = move._arr;
        _size = move._size;
        _refCount = move._refCount;

        move._arr = nullptr;
        move._refCount = nullptr;

        return *this;
    }

    [[nodiscard]] operator const T* () const noexcept { return _arr; }
    [[nodiscard]] operator       T* ()       noexcept { return _arr; }

    [[nodiscard]] const T* arr() const noexcept { return _arr; }
    [[nodiscard]]       T* arr()       noexcept { return _arr; }

    [[nodiscard]] uSys size()   const noexcept { return _size; }
    [[nodiscard]] uSys length() const noexcept { return _size; }
    [[nodiscard]] uSys count()  const noexcept { return _size; }

    [[nodiscard]] operator uSys() const { return _size; }

    [[nodiscard]] RefDynArrayIterator<T> begin() noexcept { return RefDynArrayIterator<T>(_arr, _refCount, _size, 0); }
    [[nodiscard]] RefDynArrayIterator<T> end() noexcept { return RefDynArrayIterator<T>(_arr, _refCount, _size, _size); }

    [[nodiscard]] ConstRefDynArrayIterator<T> begin() const noexcept { return ConstRefDynArrayIterator<T>(_arr, _refCount, _size, 0); }
    [[nodiscard]] ConstRefDynArrayIterator<T> end() const noexcept { return ConstRefDynArrayIterator<T>(_arr, _refCount, _size, _size); }
private:
    friend class ConstRefDynArray<T>;
};

template<typename T>
class ConstRefDynArray final
{
private:
    T* _arr;
    uSys _size;
    uSys* _refCount;
public:
    ConstRefDynArray(const RefDynArray<T>& copy) noexcept
        : _arr(copy.m_Array)
        , _size(copy.m_Length)
        , _refCount(copy._refCount)
    { ++(*_refCount); }

    ConstRefDynArray(RefDynArray<T>&& move) noexcept
        : _arr(move.m_Array)
        , _size(move.m_Length)
        , _refCount(move._refCount)
    {
        move.m_Array = nullptr;
        move._refCount = nullptr;
    }

    ~ConstRefDynArray() noexcept
    {
        if(_refCount && --(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }
    }

    ConstRefDynArray(const ConstRefDynArray<T>& copy) noexcept
        : _arr(copy._arr)
        , _size(copy._size)
        , _refCount(copy._refCount)
    { ++(*_refCount); }

    ConstRefDynArray(ConstRefDynArray<T>&& move) noexcept
        : _arr(move._arr)
        , _size(move._size)
        , _refCount(move._refCount)
    {
        move._arr = nullptr;
        move._refCount = nullptr;
    }

    ConstRefDynArray<T>& operator =(const ConstRefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = copy._arr;
        _size = copy._size;
        _refCount = copy._refCount;

        ++(*_refCount);

        return *this;
    }

    ConstRefDynArray<T>& operator =(ConstRefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = move._arr;
        _size = move._size;
        _refCount = move._refCount;

        move._arr = nullptr;
        move._refCount = nullptr;

        return *this;
    }

    ConstRefDynArray<T>& operator =(const RefDynArray<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = copy.m_Array;
        _size = copy.m_Length;
        _refCount = copy._refCount;

        ++(*_refCount);

        return *this;
    }

    ConstRefDynArray<T>& operator =(RefDynArray<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--(*_refCount) == 0)
        {
            delete[] _arr;
            delete _refCount;
        }

        _arr = move.m_Array;
        _size = move.m_Length;
        _refCount = move._refCount;

        move.m_Array = nullptr;
        move._refCount = nullptr;

        return *this;
    }

    [[nodiscard]] operator const T* () const noexcept { return _arr; }

    [[nodiscard]] const T* arr() const noexcept { return _arr; }

    [[nodiscard]] uSys size()   const noexcept { return _size; }
    [[nodiscard]] uSys length() const noexcept { return _size; }
    [[nodiscard]] uSys count()  const noexcept { return _size; }

    [[nodiscard]] operator uSys() const { return _size; }

    [[nodiscard]] ConstRefDynArrayIterator<T> begin() const noexcept { return ConstRefDynArrayIterator<T>(_arr, _refCount, _size, 0); }
    [[nodiscard]] ConstRefDynArrayIterator<T> end() const noexcept { return ConstRefDynArrayIterator<T>(_arr, _refCount, _size, _size); }
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

    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>> rbegin() noexcept { return ::std::reverse_iterator<DynArrayIterator<T>>(m_Array, m_Length, 0);            }
    [[nodiscard]] ::std::reverse_iterator<DynArrayIterator<T>>   rend() noexcept { return ::std::reverse_iterator<DynArrayIterator<T>>(m_Array, m_Length, m_Length - 1); }

    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>> rbegin() const noexcept { return ::std::reverse_iterator<ConstDynArrayIterator<T>>(m_Array, m_Length, 0);            }
    [[nodiscard]] ::std::reverse_iterator<ConstDynArrayIterator<T>>   rend() const noexcept { return ::std::reverse_iterator<ConstDynArrayIterator<T>>(m_Array, m_Length, m_Length - 1); }
};
