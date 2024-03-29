#pragma once

#pragma warning(push, 0)
#include <cstring>
#include <utility>
#pragma warning(pop)

#include "NumTypes.hpp"
#include "allocator/PageAllocator.hpp"
#include "allocator/TauAllocator.hpp"

namespace ArrayListUtils
{
/**
 * The control block for an array list.
 *
 * @element refCount
 *     The number of references to this arraylist. because
 *   there is reference counting you cannot implicitly copy the
 *   entire array. This is also used with the iterators to
 *   ensure the data is not freed before iteration is complete.
 * @element elementCount
 *     The number of elements in the array.
 * @element dataSize
 *     The total size in bytes of usage. This is of the data
 *   array as well as the control block. This is because they
 *   are contained in the same block of memory to decrease the
 *   number of required allocations.
 * @element numReservedPages
 *     The total number of pages that have been reserved. The
 *   allocator takes advantage of VirtualAlloc to reduce memory
 *   and computation overhead. This is done by reserving a
 *   number pages from system memory. Reserving pages doesn't
 *   actually consume any memory, it simply reserves a
 *   contiguous series of addresses in your program. This
 *   allows us to expand the array by commiting pages, instead
 *   of the classic reallocate and copy strategy. This means
 *   we can be more conservative with the number of allocations
 *   we perform, and eliminate the need for unnecessary data
 *   copying.
 * @element committedPages
 *     This is the number of reserved pages that have been
 *    committed. A committed page is one where the address
 *    space correlates to a real block of RAM.
 */
struct CtrlBlockData
{
    uSys refCount;
    uSys elementCount;
    uSys dataSize;
    uSys committedPages;
};

struct ControlBlock final
{
    union
    {
        CtrlBlockData data;
        char _alignment[64];
    };
};
}

template<typename T>
class ArrayListIterator final
{
public:
    using ControlBlock = ArrayListUtils::ControlBlock;
    using value_type = T;
private:
    ControlBlock* _ctrlBlock;
    T* _arr;
    uSys _index;
public:
    inline ArrayListIterator(ControlBlock* const ctrlBlock, T* const arr, const uSys index) noexcept
        : _ctrlBlock(ctrlBlock)
        , _arr(arr)
        , _index(index)
    { ++_ctrlBlock->data.refCount; }

    inline ~ArrayListIterator() noexcept
    {
        if(_ctrlBlock && --_ctrlBlock->data.refCount == 0)
        { PageAllocator::Free(_ctrlBlock); }
    }

    inline ArrayListIterator(const ArrayListIterator<T>& copy) noexcept
        : _ctrlBlock(copy._ctrlBlock)
        , _arr(copy._arr)
        , _index(copy._index)
    { ++_ctrlBlock->data.refCount; }

    inline ArrayListIterator(ArrayListIterator<T>&& move) noexcept
        : _ctrlBlock(move._ctrlBlock)
        , _arr(move._arr)
        , _index(move._index)
    { move._ctrlBlock = nullptr; }

    inline ArrayListIterator<T>& operator =(const ArrayListIterator<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        _ctrlBlock = copy._ctrlBlock;
        _arr = copy._arr;
        _index = copy._index;

        ++_ctrlBlock->data.refCount;

        return *this;
    }

    inline ArrayListIterator<T>& operator =(ArrayListIterator<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        _ctrlBlock = move._ctrlBlock;
        _arr = move._arr;
        _index = move._index;

        move._ctrlBlock = nullptr;

        return *this;
    }

    inline ArrayListIterator<T>& operator++() noexcept
    {
        if(_index < _ctrlBlock->data.elementCount)
        { ++_index; }
        return *this;
    }

    inline ArrayListIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
        return *this;
    }

    [[nodiscard]] inline ArrayListIterator<T> operator++(int) noexcept
    {
        const ArrayListIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] inline ArrayListIterator<T> operator--(int) noexcept
    {
        const ArrayListIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] inline uSys index() const noexcept { return _index; }

    [[nodiscard]] inline       T& operator*()       noexcept { return _arr[_index]; }
    [[nodiscard]] inline const T& operator*() const noexcept { return _arr[_index]; }

    [[nodiscard]] inline bool operator ==(const ArrayListIterator<T> & other) const noexcept { return _index == other._index; }
    [[nodiscard]] inline bool operator !=(const ArrayListIterator<T> & other) const noexcept { return _index != other._index; }
};

template<typename T>
class ConstArrayListIterator final
{
public:
    using ControlBlock = ArrayListUtils::ControlBlock;
    using value_type = T;
private:
    ControlBlock* _ctrlBlock;
    const T* _arr;
    uSys _index;
public:
    inline ConstArrayListIterator(ControlBlock* const ctrlBlock, const T* const arr, const uSys index) noexcept
        : _ctrlBlock(ctrlBlock)
        , _arr(arr)
        , _index(index)
    { ++_ctrlBlock->data.refCount; }

    inline ~ConstArrayListIterator() noexcept
    {
        if(_ctrlBlock && --_ctrlBlock->data.refCount == 0)
        { PageAllocator::Free(_ctrlBlock); }
    }

    inline ConstArrayListIterator(const ConstArrayListIterator<T>& copy) noexcept
        : _ctrlBlock(copy._ctrlBlock)
        , _arr(copy._arr)
        , _index(copy._index)
    { ++_ctrlBlock->data.refCount; }

    inline ConstArrayListIterator(ConstArrayListIterator<T>&& move) noexcept
        : _ctrlBlock(move._ctrlBlock)
        , _arr(move._arr)
        , _index(move._index)
    { move._ctrlBlock = nullptr; }

    inline ConstArrayListIterator<T>& operator =(const ConstArrayListIterator<T>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        if(--_ctrlBlock->data.refCount == 0)
        { PageAllocator::Free(_ctrlBlock); }

        _ctrlBlock = copy._ctrlBlock;
        _arr = copy._arr;
        _index = copy._index;

        ++_ctrlBlock->data.refCount;

        return *this;
    }

    inline ConstArrayListIterator<T>& operator =(ConstArrayListIterator<T>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        if(--_ctrlBlock->data.refCount == 0)
        { PageAllocator::Free(_ctrlBlock); }

        _ctrlBlock = move._ctrlBlock;
        _arr = move._arr;
        _index = move._index;

        move._ctrlBlock = nullptr;

        return *this;
    }

    inline ConstArrayListIterator<T>& operator++() noexcept
    {
        if(_index < _ctrlBlock->data.elementCount)
        { ++_index; }
        return *this;
    }

    inline ConstArrayListIterator<T>& operator--() noexcept
    {
        if(_index > 0)
        { --_index; }
        return *this;
    }

    [[nodiscard]] inline ConstArrayListIterator<T> operator++(int) noexcept
    {
        const ArrayListIterator<T> copy(*this);
        this->operator++();
        return copy;
    }

    [[nodiscard]] inline ConstArrayListIterator<T> operator--(int) noexcept
    {
        const ArrayListIterator<T> copy(*this);
        this->operator--();
        return copy;
    }

    [[nodiscard]] inline uSys index() const noexcept { return _index; }

    [[nodiscard]] inline const T& operator*() const noexcept { return _arr[_index]; }

    [[nodiscard]] inline bool operator ==(const ConstArrayListIterator<T>& other) const noexcept { return _index == other._index; }
    [[nodiscard]] inline bool operator !=(const ConstArrayListIterator<T>& other) const noexcept { return _index != other._index; }
};

enum class ALMoveMethod
{
    MemCopy = 0,
    MoveConstruct
};

template<typename T, ALMoveMethod MoveMethod = ALMoveMethod::MemCopy>
class ArrayList final { };

template<typename T>
class ArrayList<T, ALMoveMethod::MemCopy> final
{
public:
    using ControlBlock = ArrayListUtils::ControlBlock;
private:
    ControlBlock* _ctrlBlock;
    T* _arr;
public:
    ArrayList(const uSys maxElements) noexcept
        : _ctrlBlock(reinterpret_cast<ControlBlock*>(PageAllocator::Reserve((maxElements * sizeof(T)) / PageAllocator::PageSize() + 1)))
        , _arr(reinterpret_cast<T*>(_ctrlBlock + 1))
    {
        (void) PageAllocator::CommitPage(_ctrlBlock);
        _ctrlBlock->data.refCount = 1;
        _ctrlBlock->data.elementCount = 0;
        _ctrlBlock->data.dataSize = sizeof(ControlBlock);
        _ctrlBlock->data.committedPages = 1;
    }

    ~ArrayList() noexcept
    {
        if(_ctrlBlock && --_ctrlBlock->data.refCount == 0)
        {
            for(uSys i = 0; i < _ctrlBlock->data.elementCount; ++i)
            {
                _arr[i].~T();
            }

            PageAllocator::Free(_ctrlBlock);
        }
    }

    ArrayList(const ArrayList<T, ALMoveMethod::MemCopy>& copy) noexcept
        : _ctrlBlock(copy._ctrlBlock)
        , _arr(copy._arr)
    { ++_ctrlBlock->data.refCount; }

    ArrayList(ArrayList<T, ALMoveMethod::MemCopy>&& move) noexcept
        : _ctrlBlock(move._ctrlBlock)
        , _arr(move._arr)
    { move._ctrlBlock = nullptr; }

    ArrayList<T, ALMoveMethod::MemCopy>& operator=(const ArrayList<T, ALMoveMethod::MemCopy>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        _ctrlBlock = copy._ctrlBlock;
        _arr = copy._arr;

        ++_ctrlBlock->data.refCount;

        return *this;
    }

    ArrayList<T, ALMoveMethod::MemCopy>& operator=(ArrayList<T, ALMoveMethod::MemCopy>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        _ctrlBlock = move._ctrlBlock;
        _arr = move._arr;

        move._ctrlBlock = nullptr;

        return *this;
    }
    
    [[nodiscard]]       T* arr()       noexcept { return _arr; }
    [[nodiscard]] const T* arr() const noexcept { return _arr; }
    [[nodiscard]] uSys  count() const noexcept { return _ctrlBlock->data.elementCount; }
    [[nodiscard]] uSys   size() const noexcept { return _ctrlBlock->data.elementCount; }
    [[nodiscard]] uSys length() const noexcept { return _ctrlBlock->data.elementCount; }

    void add(const T& val) noexcept
    {
        assertSize();
        _arr[_ctrlBlock->data.elementCount++] = val;
        _ctrlBlock->data.dataSize += sizeof(T);
    }

    template<typename... _Args>
    T& emplace(_Args&&... args) noexcept
    {
        assertSize();
        void* const placement = _arr + _ctrlBlock->data.elementCount;
        T* const ret = new(placement) T(TauAllocatorUtils::Forward<_Args>(args)...);
        ++_ctrlBlock->data.elementCount;
        _ctrlBlock->data.dataSize += sizeof(T);

        return *ret;
    }

    [[nodiscard]] T* placement(uSys* index = nullptr) noexcept
    {
        assertSize();
        T* const placementBlock = _arr + _ctrlBlock->data.elementCount;
        if(index)
        { *index = _ctrlBlock->data.elementCount; }
        ++_ctrlBlock->data.elementCount;
        _ctrlBlock->data.dataSize += sizeof(T);
        return placementBlock;
    }

    void removeFast(const uSys index) noexcept
    {
        if(index + 1 > _ctrlBlock->data.elementCount)
        { return; }

        _arr[index].~T();

        --_ctrlBlock->data.elementCount;
        if(index == _ctrlBlock->data.elementCount)
        { return; }

        (void) std::memcpy(_arr + index, _arr + _ctrlBlock->data.elementCount, sizeof(T));
        _ctrlBlock->data.dataSize -= sizeof(T);

        attemptRelease();
    }

    void remove(const uSys index) noexcept
    {
        if(index + 1 > _ctrlBlock->data.elementCount)
        { return; }

        _arr[index].~_T();

        --_ctrlBlock->data.elementCount;
        if(index == _ctrlBlock->data.elementCount)
        { return; }

        (void) std::memmove(_arr + index, _arr + index + 1, (_ctrlBlock->data.elementCount - index) * sizeof(T));
        _ctrlBlock->data.dataSize -= sizeof(T);

        attemptRelease();
    }

    void clear(const bool releasePages = true) noexcept
    {
        for(uSys i = 0; i < _ctrlBlock->data.elementCount; ++i)
        {
            _arr[i].~_T();
        }

        _ctrlBlock->data.dataSize = sizeof(ControlBlock);
        _ctrlBlock->data.elementCount = 0;

        if(releasePages)
        {
            PageAllocator::DecommitPages(_ctrlBlock + PageAllocator::PageSize(), _ctrlBlock->data.committedPages - 1);
        }
    }

    [[nodiscard]] T* at(const uSys index) noexcept
    {
        if(index >= _ctrlBlock->data.elementCount)
        { return nullptr; }

        return _arr[index];
    }

    [[nodiscard]] const T* at(const uSys index) const noexcept
    {
        if(index >= _ctrlBlock->data.elementCount)
        { return nullptr; }

        return &_arr[index];
    }

    [[nodiscard]]       T* get(const uSys index)       noexcept { return at(index); }
    [[nodiscard]] const T* get(const uSys index) const noexcept { return at(index); }

    [[nodiscard]]       T& operator[](const uSys index)       noexcept { return _arr[index]; }
    [[nodiscard]] const T& operator[](const uSys index) const noexcept { return _arr[index]; }

    [[nodiscard]] inline ArrayListIterator<T> begin() noexcept { return ArrayListIterator<T>(_ctrlBlock, _arr, 0);                        }
    [[nodiscard]] inline ArrayListIterator<T>   end() noexcept { return ArrayListIterator<T>(_ctrlBlock, _arr, _ctrlBlock->data.elementCount); }

    [[nodiscard]] inline ConstArrayListIterator<T> begin() const noexcept { return ConstArrayListIterator<T>(_ctrlBlock, _arr, 0);                        }
    [[nodiscard]] inline ConstArrayListIterator<T>   end() const noexcept { return ConstArrayListIterator<T>(_ctrlBlock, _arr, _ctrlBlock->data.elementCount); }
private:
    void assertSize() noexcept
    {
        const uSys pageBytes = _ctrlBlock->data.committedPages * PageAllocator::PageSize();
        if(_ctrlBlock->data.dataSize + sizeof(T) >= pageBytes)
        {
            (void) PageAllocator::CommitPage(reinterpret_cast<u8*>(_ctrlBlock) + pageBytes);
            ++_ctrlBlock->data.committedPages;
        }
    }

    void attemptRelease() noexcept
    {
        // If there are two empty pages, release the last page.
        const uSys pageBytes = (_ctrlBlock->data.committedPages - 1) * PageAllocator::PageSize();
        if(pageBytes - _ctrlBlock->data.dataSize <= PageAllocator::PageSize())
        {
            (void) PageAllocator::DecommitPage(reinterpret_cast<u8*>(_ctrlBlock) + pageBytes);
            --_ctrlBlock->data.committedPages;
        }
    }
};

template<typename T>
class ArrayList<T, ALMoveMethod::MoveConstruct> final
{
public:
    using ControlBlock = ArrayListUtils::ControlBlock;
private:
    ControlBlock* _ctrlBlock;
    T* _arr;
public:
    ArrayList(const uSys maxElements) noexcept
        : _ctrlBlock(reinterpret_cast<ControlBlock*>(PageAllocator::Reserve(maxElements / PageAllocator::PageSize() + 1)))
        , _arr(reinterpret_cast<T*>(_ctrlBlock + 1))
    {
        (void)PageAllocator::CommitPage(_ctrlBlock);
        _ctrlBlock->data.refCount = 1;
        _ctrlBlock->data.elementCount = 0;
        _ctrlBlock->data.dataSize = sizeof(ControlBlock);
        _ctrlBlock->data.committedPages = 1;
    }

    ~ArrayList() noexcept
    {
        if(_ctrlBlock && --_ctrlBlock->data.refCount == 0)
        {
            for(uSys i = 0; i < _ctrlBlock->data.elementCount; ++i)
            {
                _arr[i].~_T();
            }

            PageAllocator::Free(_ctrlBlock);
        }
    }

    ArrayList(const ArrayList<T, ALMoveMethod::MoveConstruct>& copy) noexcept
        : _ctrlBlock(copy._ctrlBlock)
        , _arr(copy._arr)
    { ++_ctrlBlock->data.refCount; }

    ArrayList(ArrayList<T, ALMoveMethod::MoveConstruct>&& move) noexcept
        : _ctrlBlock(move._ctrlBlock)
        , _arr(move._arr)
    { move._ctrlBlock = nullptr; }

    ArrayList<T, ALMoveMethod::MoveConstruct>& operator=(const ArrayList<T, ALMoveMethod::MoveConstruct>& copy) noexcept
    {
        if(this == &copy)
        { return *this; }

        _ctrlBlock = copy._ctrlBlock;
        _arr = copy._arr;

        ++_ctrlBlock->data.refCount;

        return *this;
    }

    ArrayList<T, ALMoveMethod::MoveConstruct>& operator=(ArrayList<T, ALMoveMethod::MoveConstruct>&& move) noexcept
    {
        if(this == &move)
        { return *this; }

        _ctrlBlock = move._ctrlBlock;
        _arr = move._arr;

        move._ctrlBlock = nullptr;

        return *this;
    }

    [[nodiscard]]       T* arr()       noexcept { return _arr; }
    [[nodiscard]] const T* arr() const noexcept { return _arr; }
    [[nodiscard]] uSys  count() const noexcept { return _ctrlBlock->data.elementCount; }
    [[nodiscard]] uSys   size() const noexcept { return _ctrlBlock->data.elementCount; }
    [[nodiscard]] uSys length() const noexcept { return _ctrlBlock->data.elementCount; }

    void add(const T& val) noexcept
    {
        assertSize();
        _arr[_ctrlBlock->data.elementCount++] = val;
        _ctrlBlock->data.dataSize += sizeof(T);
    }

    template<typename... _Args>
    T& emplace(_Args&&... args) noexcept
    {
        assertSize();
        void* const placement = _arr + _ctrlBlock->data.elementCount;
        T* ret = new(placement) T(TauAllocatorUtils::Forward<_Args>(args)...);
        ++_ctrlBlock->data.elementCount;
        _ctrlBlock->data.dataSize += sizeof(T);

        return *ret;
    }

    void removeFast(const uSys index) noexcept
    {
        if(index + 1 > _ctrlBlock->data.elementCount)
        { return; }

        _arr[index].~_T();

        --_ctrlBlock->data.elementCount;
        if(index == _ctrlBlock->data.elementCount)
        { return; }

        _arr[index] = ::std::move(_arr[_ctrlBlock->data.elementCount - 1]);
        _ctrlBlock->data.dataSize -= sizeof(T);

        attemptRelease();
    }

    void remove(const uSys index) noexcept
    {
        if(index + 1 > _ctrlBlock->data.elementCount)
        { return; }

        _arr[index].~_T();

        --_ctrlBlock->data.elementCount;
        if(index == _ctrlBlock->data.elementCount)
        { return; }

        const uSys count = _ctrlBlock->data.elementCount - index - 1;

        for(uSys i = 0; i < count; ++i)
        {
            _arr[index + i] = ::std::move(_arr[index + i + 1]);
        }
        _ctrlBlock->data.dataSize -= sizeof(T);

        attemptRelease();
    }

    void clear(const bool releasePages = true) noexcept
    {
        for(uSys i = 0; i < _ctrlBlock->data.elementCount; ++i)
        {
            _arr[i].~_T();
        }

        if(releasePages)
        {
            PageAllocator::DecommitPages(_ctrlBlock + PageAllocator::PageSize(), _ctrlBlock->data.committedPages - 1);
        }
    }

    [[nodiscard]] T* at(const uSys index) noexcept
    {
        if(index >= _ctrlBlock->data.elementCount)
        { return nullptr; }

        return _arr[index];
    }

    [[nodiscard]] const T* at(const uSys index) const noexcept
    {
        if(index >= _ctrlBlock->data.elementCount)
        { return nullptr; }

        return &_arr[index];
    }

    [[nodiscard]]       T* get(const uSys index)       noexcept { return at(index); }
    [[nodiscard]] const T* get(const uSys index) const noexcept { return at(index); }

    [[nodiscard]]       T& operator[](const uSys index)       noexcept { return _arr[index]; }
    [[nodiscard]] const T& operator[](const uSys index) const noexcept { return _arr[index]; }

    [[nodiscard]] inline ArrayListIterator<T> begin() noexcept { return ArrayListIterator<T>(_ctrlBlock, _arr, 0);                        }
    [[nodiscard]] inline ArrayListIterator<T>   end() noexcept { return ArrayListIterator<T>(_ctrlBlock, _arr, _ctrlBlock->data.elementCount); }

    [[nodiscard]] inline ConstArrayListIterator<T> begin() const noexcept { return ConstArrayListIterator<T>(_ctrlBlock, _arr, 0);                        }
    [[nodiscard]] inline ConstArrayListIterator<T>   end() const noexcept { return ConstArrayListIterator<T>(_ctrlBlock, _arr, _ctrlBlock->data.elementCount); }
private:
    void assertSize() noexcept
    {
        const uSys pageBytes = _ctrlBlock->data.committedPages * PageAllocator::PageSize();
        if(_ctrlBlock->data.dataSize + sizeof(T) >= pageBytes)
        {
            (void) PageAllocator::CommitPage(reinterpret_cast<u8*>(_ctrlBlock) + pageBytes);
            ++_ctrlBlock->data.committedPages;
        }
    }

    void attemptRelease() noexcept
    {
        // If there are two empty pages, release the last page.
        const uSys pageBytes = (_ctrlBlock->data.committedPages - 1) * PageAllocator::PageSize();
        if(pageBytes - _ctrlBlock->data.dataSize <= PageAllocator::PageSize())
        {
            (void) PageAllocator::DecommitPage(reinterpret_cast<u8*>(_ctrlBlock) + pageBytes);
            --_ctrlBlock->data.committedPages;
        }
    }
};
