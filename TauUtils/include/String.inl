#pragma once

#if defined(STRING_IN_DEV) || 1
#include "String.hpp"
#endif

#include <cstring>
#include <cwctype>
#include <locale>
#include <new>
#include "TUMaths.hpp"

namespace tau::string {

template<typename Char>
template<uSys Len>
constexpr LargeString<Char>::LargeString(const Char(& str)[Len]) noexcept
    : RefCount(nullptr)
    , Buffer(nullptr)
{ }

template<typename Char>
constexpr LargeString<Char>::LargeString() noexcept
    : RefCount(nullptr)
    , Buffer(nullptr)
{ }

template<typename Char>
LargeString<Char>::LargeString(const Char* const buffer, const uSys codeUnits) noexcept
{
    void* newBuffer = operator new((codeUnits + 1) * sizeof(Char) + sizeof(ReferenceCounter::Type), ::std::nothrow);
    RefCount = ReferenceCounter(new(newBuffer) ReferenceCounter::Type(0));
    Char* cBuffer = reinterpret_cast<Char*>(reinterpret_cast<ReferenceCounter::Type*>(newBuffer) + 1);
    cBuffer[codeUnits] = Char{ '\0' };

    (void) ::std::memcpy(cBuffer, buffer, codeUnits * sizeof(Char));
    Buffer = cBuffer;
}

template<typename Char>
LargeString<Char>::LargeString(ReferenceCounter::Type* const refCount, const Char* const buffer) noexcept
    : RefCount(refCount)
    , Buffer(buffer)
{ }

template<typename Char>
LargeString<Char>::~LargeString() noexcept
{
    if(RefCount.RefCount() == 1)
    {
        if(reinterpret_cast<const void*>(RefCount.RefCountPtr() + 1) != reinterpret_cast<const void*>(Buffer))
        {
            delete[] Buffer;
        }
    }
}

template<typename Char>
constexpr StringData<Char>::StringData() noexcept
    : Length(0)
{ Buffer.StackString[0] = Char{ '\0' }; }

template<typename Char>
constexpr StringData<Char>::StringData(nullptr_t) noexcept
    : Length(0)
{ Buffer.StackString[0] = Char{ '\0' }; }

template<typename Char>
template<uSys Len>
constexpr StringData<Char>::StringData(const Char(& str)[Len]) noexcept
    : Buffer(str)
    , Length(Len)
{ }

template<typename Char>
StringData<Char>::StringData(const Char* const buffer, const uSys codeUnits) noexcept
    : Length(codeUnits)
{
    if(codeUnits < 16)
    {
        (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
        Buffer.StackString[codeUnits] = Char{ '\0' };
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
    }
}

template<typename Char>
StringData<Char>::StringData(ReferenceCounter::Type* const refCount, const Char* const buffer, const uSys codeUnits) noexcept
    : Length(codeUnits)
{
    if(codeUnits < 16)
    {
        (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
        Buffer.StackString[codeUnits] = Char{ '\0' };
        delete refCount;
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(refCount, buffer);
    }
}

template<typename Char>
StringData<Char>::~StringData() noexcept
{
    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }
}

template<typename Char>
StringData<Char>::StringData(const StringData& copy) noexcept
    : Length(copy.Length)
{
    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        ::std::memcpy(Buffer.StackString, copy.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(copy.Buffer.AsLargeString());
    }
}

template<typename Char>
StringData<Char>::StringData(StringData&& move) noexcept
    : Length(move.Length)
{
    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(::std::move(move.Buffer.AsLargeString()));
    }
}

template<typename Char>
StringData<Char>& StringData<Char>::operator=(const StringData& copy) noexcept
{
    if(this == &copy)
    {
        return *this;
    }

    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = copy.Length;

    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        ::std::memcpy(Buffer.StackString, copy.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(copy.Buffer.AsLargeString());
    }

    return *this;
}

template<typename Char>
StringData<Char>& StringData<Char>::operator=(StringData&& move) noexcept
{
    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = move.Length;

    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(::std::move(move.Buffer.AsLargeString()));
    }

    return *this;
}

template<typename Char>
template<uSys Len>
StringData<Char>& StringData<Char>::operator=(const Char(& str)[Len]) noexcept
{
    if(String() == str)
    { return *this; }
    
    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = Len;

    new(Buffer.LargeString) LargeString<Char>(str);

    return *this;
}

template<typename Char>
void StringData<Char>::Reset(const Char* const buffer, const uSys codeUnits) noexcept
{
    if(String() == buffer)
    { return; }

    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = codeUnits;
    
    if(codeUnits < 16)
    {
        (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
        Buffer.StackString[codeUnits] = Char{ '\0' };
    }
    else
    {
        new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
    }
}

template<typename Char>
void StringData<Char>::Reset(ReferenceCounter::Type* const refCount, const Char* const buffer, const uSys codeUnits) noexcept
{
    if(String() == buffer)
    { return; }

    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = codeUnits;
    
    new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
}

}

template<>
inline char toLower<char>(const char c) noexcept
{
    if(c >= 'A' && c <= 'Z')
    {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

template<>
inline wchar_t toLower<wchar_t>(const wchar_t c) noexcept
{ return static_cast<wchar_t>(::std::towlower(c)); }

template<>
inline c8 toLower<c8>(const c8 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }

template<>
inline c16 toLower<c16>(const c16 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }

template<>
inline c32 toLower<c32>(const c32 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }

template<>
inline char toUpper<char>(const char c) noexcept
{
    if(c >= 'a' && c <= 'z')
    {
        return static_cast<char>((c - 'a') + 'A');
    }
    return c;
}

template<>
inline wchar_t toUpper<wchar_t>(const wchar_t c) noexcept
{ return static_cast<wchar_t>(::std::towupper(c)); }

template<>
inline c8 toUpper<c8>(const c8 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }

template<>
inline c16 toUpper<c16>(const c16 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }

template<>
inline c32 toUpper<c32>(const c32 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }

template<typename C>
inline uSys toLower(const C* const str, C* const store) noexcept
{
    uSys i = 0;
    if(store)
    {
        for(; str[i] != '\0'; ++i)
        {
            store[i] = toLower(str[i]);
        }
    }
    else
    {
        for(; str[i] != '\0'; ++i);
    }

    return i;
}

template<typename C>
inline uSys toUpper(const C* const str, C* const store) noexcept
{
    uSys i = 0;
    if(store)
    {
        for(; str[i] != '\0'; ++i)
        {
            store[i] = toUpper(str[i]);
        }
    }
    else
    {
        for(; str[i] != '\0'; ++i);
    }

    return i;
}

template<typename C>
inline uSys findHashCode(const C* str) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; str[i]; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename C>
inline uSys findHashCode(const C* str, const uSys len) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; i < len; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename C>
inline uSys strLength(const C* const str) noexcept
{
    if(!str)
    { return 0; }

    uSys i = 0;
    // ReSharper disable once CppPossiblyErroneousEmptyStatements
    for(; str[i]; ++i);
    return i;
}

template<>
inline uSys strLength<char>(const char* const str) noexcept
{ return ::std::strlen(str); }

template<>
inline uSys strLength<wchar_t>(const wchar_t* const str) noexcept
{ return ::std::wcslen(str); }

template<typename C>
inline i32 strCompare(const C* const lhs, const C* const rhs) noexcept
{
    while(*lhs != '\0' && *rhs != '\0')
    {
        if(*lhs != *rhs)
        {
            if(*lhs < *rhs)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        ++lhs;
        ++rhs;
    }

    if(*lhs == '\0' && *rhs == '\0')
    {
        return 0;
    }
    else if(*lhs == '\0')
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

template<>
inline i32 strCompare(const char* const lhs, const char* const rhs) noexcept
{ return ::std::strcmp(lhs, rhs); }

template<>
inline i32 strCompare(const wchar_t* const lhs, const wchar_t* const rhs) noexcept
{ return ::std::wcscmp(lhs, rhs); }

template<typename C>
inline i32 strCompare(const C* const lhs, const C* const rhs, const uSys length) noexcept
{
    uSys i = 0;
    while(*lhs != '\0' && *rhs != '\0' && i < length)
    {
        if(*lhs != *rhs)
        {
            if(*lhs < *rhs)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        ++lhs;
        ++rhs;
        ++i;
    }

    if(*lhs == *rhs)
    {
        return 0;
    }
    else if(*lhs == '\0')
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

template<>
inline i32 strCompare(const char* const lhs, const char* const rhs, const uSys length) noexcept
{ return ::std::strncmp(lhs, rhs, length); }

template<>
inline i32 strCompare(const wchar_t* const lhs, const wchar_t* const rhs, const uSys length) noexcept
{ return ::std::wcsncmp(lhs, rhs, length); }

template<typename _C, uSys _Len>
inline constexpr uSys cexpr::strlen(const _C(&str)[_Len]) noexcept
{ return _Len - 1; }

template<typename _C, uSys _Len>
inline constexpr bool cexpr::streq(const _C(&lhs)[_Len], const _C(&rhs)[_Len])
{
    while(*lhs != '\0' && *rhs != '\0')
    {
        if(*lhs != *rhs)
        { return false; }
        ++lhs;
        ++rhs;
    }

    return *lhs == '\0' && *rhs == '\0';
}

template<typename _C, uSys _Len>
inline constexpr bool cexpr::streqIgnoreCase(const _C(&lhs)[_Len], const _C(&rhs)[_Len])
{
    while(*lhs != '\0' && *rhs != '\0')
    {
        if(toLower(*lhs) != toLower(*rhs))
        { return false; }
        ++lhs;
        ++rhs;
    }

    return *lhs == '\0' && *rhs == '\0';
}

template<typename _C, uSys _Len>
inline constexpr uSys cexpr::findHashCode(const _C(&str)[_Len]) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; str[i]; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename _C>
inline StringIteratorT<_C>::StringIteratorT(const _C* const string, const uSys length, const uSys index) noexcept
    : _string(string)
    , _length(length)
    , _index(index)
{ }

template<typename _C>
inline StringIteratorT<_C>& StringIteratorT<_C>::operator++() noexcept
{
    if(_index < _length - 1)
    { ++_index; }
    return *this;
}

template<typename _C>
inline StringIteratorT<_C>& StringIteratorT<_C>::operator--() noexcept
{
    if(_index > 0)
    { --_index; }
    return *this;
}

template<typename _C>
inline StringIteratorT<_C> StringIteratorT<_C>::operator++(int) noexcept
{
    const StringIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename _C>
inline StringIteratorT<_C> StringIteratorT<_C>::operator--(int) noexcept
{
    const StringIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename _C>
inline _C StringIteratorT<_C>::operator*() const noexcept
{ return _string[_index]; }

template<typename _C>
inline bool StringIteratorT<_C>::operator==(const StringIteratorT<_C>& other) const noexcept
{ return _index == other._index; }

template<typename _C>
inline bool StringIteratorT<_C>::operator!=(const StringIteratorT<_C>& other) const noexcept
{ return _index != other._index; }

template<typename Char>
DynStringCodeUnitIteratorT<Char>::DynStringCodeUnitIteratorT(const StringData& string, const uSys index) noexcept
    : m_String(string)
    , m_Index(index)
{ }

template<typename Char>
DynStringCodeUnitIteratorT<Char>& DynStringCodeUnitIteratorT<Char>::operator++() noexcept
{
    if(m_Index < m_String.Length - 1)
    { ++m_Index; }
    return *this;
}

template<typename Char>
DynStringCodeUnitIteratorT<Char>& DynStringCodeUnitIteratorT<Char>::operator--() noexcept
{
    if(m_Index > 0)
    { --m_Index; }
    return *this;
}

template<typename Char>
DynStringCodeUnitIteratorT<Char> DynStringCodeUnitIteratorT<Char>::operator++(int) noexcept
{
    const DynStringCodeUnitIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
DynStringCodeUnitIteratorT<Char> DynStringCodeUnitIteratorT<Char>::operator--(int) noexcept
{
    const DynStringCodeUnitIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
Char DynStringCodeUnitIteratorT<Char>::operator*() const noexcept
{ return m_String.String()[m_Index]; }

template<typename Char>
bool DynStringCodeUnitIteratorT<Char>::operator==(const DynStringCodeUnitIteratorT<Char>& other) const noexcept
{ return m_Index == other.m_Index; }

template<typename Char>
bool DynStringCodeUnitIteratorT<Char>::operator!=(const DynStringCodeUnitIteratorT<Char>& other) const noexcept
{ return m_Index != other.m_Index; }

template<typename Char>
DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index) noexcept
    : m_String(string)
    , m_Index(index)
    , m_CurrentCodePoint(string.Length > 0 ? string.String()[index] : U'\0')
{ }

template<typename Char>
DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const c32 currentCodePoint) noexcept
    : m_String(string)
    , m_Index(index)
    , m_CurrentCodePoint(currentCodePoint)
{ }

template<typename Char>
DynStringCodePointIteratorT<Char>& DynStringCodePointIteratorT<Char>::operator++() noexcept
{
    if(m_Index < m_String.Length - 1)
    { ++m_Index; }
    return *this;
}

template<typename Char>
DynStringCodePointIteratorT<Char>& DynStringCodePointIteratorT<Char>::operator--() noexcept
{
    if(m_Index > 0)
    { --m_Index; }
    return *this;
}

template<typename Char>
DynStringCodePointIteratorT<Char> DynStringCodePointIteratorT<Char>::operator++(int) noexcept
{
    const DynStringCodePointIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
DynStringCodePointIteratorT<Char> DynStringCodePointIteratorT<Char>::operator--(int) noexcept
{
    const DynStringCodePointIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
c32 DynStringCodePointIteratorT<Char>::operator*() const noexcept
{ return m_CurrentCodePoint; }

template<typename Char>
bool DynStringCodePointIteratorT<Char>::operator==(const DynStringCodePointIteratorT<Char>& other) const noexcept
{ return m_Index == other.m_Index; }

template<typename Char>
bool DynStringCodePointIteratorT<Char>::operator!=(const DynStringCodePointIteratorT<Char>& other) const noexcept
{ return m_Index != other.m_Index; }

template<typename _C>
template<uSys _Len>
inline constexpr ConstExprStringT<_C>::ConstExprStringT(const _C(&str)[_Len]) noexcept
    : _string(str)
    , _length(_Len - 1)
    , _hash(cexpr::findHashCode(str))
{ }

template<typename _C>
template<uSys _Len>
inline constexpr ConstExprStringT<_C>& ConstExprStringT<_C>::operator=(const _C(&str)[_Len]) noexcept
{
    _string = str;
    _length = _Len - 1;
    _hash = cexpr::findHashCode(str);

    return *this;
}

template<typename _C>
template<uSys _Len>
inline constexpr bool ConstExprStringT<_C>::equals(const _C(&str)[_Len]) const noexcept
{
    if(_string == str) { return true; }
    if(_length != _Len) { return false; }
    return strCompare(_string, str) == 0;
}

template<typename _C>
inline bool ConstExprStringT<_C>::equals(const ConstExprStringT<_C>& other) const noexcept
{
    if(_string == other._string) { return true; }
    if(_length != other._length) { return false; }
    if(_hash != other._hash) { return false; }
    return strCompare(_string, other.c_str()) == 0;
}

template<typename _C>
inline bool ConstExprStringT<_C>::equals(const DynStringT<_C>& other) const noexcept
{
    if(_length != other._length || _hash != other._hash)
    { return false; }
    return strCompare(_string, other.c_str()) == 0;
}

template<typename _C>
inline bool ConstExprStringT<_C>::equals(const DynStringViewT<_C>& other) const noexcept
{
    if(_length != other._length || _hash != other._hash)
    { return false; }
    return strCompare(_string, other.c_str(), _length) == 0;
}

template<typename _C>
inline bool ConstExprStringT<_C>::equals(const _C* const str) const noexcept
{
    if(_string == str) { return true; }
    return strCompare(_string, str) == 0;
}

template<typename _C>
inline i32 ConstExprStringT<_C>::compareTo(const ConstExprStringT<_C>& other) const noexcept
{ return strCompare(_string, other._string()); }

template<typename _C>
inline i32 ConstExprStringT<_C>::compareTo(const DynStringT<_C>& other) const noexcept
{ return strCompare(_string, other.c_str()); }

template<typename _C>
inline i32 ConstExprStringT<_C>::compareTo(const DynStringViewT<_C>& other) const noexcept
{ return strCompare(_string, other._string, _length); }

template<typename _C>
inline i32 ConstExprStringT<_C>::compareTo(const _C* const str) const noexcept
{ return strCompare(_string, str); }

template<typename _C>
inline _C ConstExprStringT<_C>::operator[](const uSys index) const noexcept
{ return _string[index]; }

template<typename _C>
inline _C ConstExprStringT<_C>::at(const uSys index) const noexcept
{
    if(index >= _length) { return _C{ '\0' }; }
    return _string[index];
}

template<typename Char>
template<uSys Len>
constexpr DynStringT<Char> DynStringT<Char>::FromStatic(const Char(& str)[Len]) noexcept
{
    if(Len < 16)
    {
        return DynStringT<Char>(static_cast<const Char*>(str));
    }

    return DynStringT<Char>(str);
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::passControl(const Char* const str) noexcept
{
    const uSys len = strLength(str);
    if(len < 16)
    {
        DynStringT<Char> tmp(str);
        delete str;
        return tmp;
    }
    return DynStringT<Char>(str, len);
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::passControl(ReferenceCounter::Type* const refCount, const Char* const str, const uSys length) noexcept
{
    if(length < 16)
    {
        DynStringT<Char> tmp(str);
        delete refCount;
        return tmp;
    }
    return DynStringT<Char>(refCount, str, length);
}

template<typename Char>
template<uSys Len>
constexpr DynStringT<Char>::DynStringT(const Char(& str)[Len]) noexcept
    : m_Data(str)
    , m_Hash(cexpr::findHashCode(str))
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT(const Char* const string, const uSys length) noexcept
    : m_Data(string, length)
    , m_Hash(findHashCode(string))
{ }

template<typename Char>
DynStringT<Char>::DynStringT(ReferenceCounter::Type* const refCount, const Char* const string, const uSys length) noexcept
    : m_Data(refCount, string, length)
    , m_Hash(findHashCode(string))
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT() noexcept
    : m_Data()
    , m_Hash(0)
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT(const Char* const string) noexcept
    : m_Data(string, strLength(string))
    , m_Hash(findHashCode(string))
{ }


template<typename Char>
inline DynStringT<Char>& DynStringT<Char>::operator=(const Char* const string) noexcept
{
    if(!string)
    {
        return *this;
    }

    /**
     * _largeString.string and _stackString store the same address.
     */
    if(string == m_Data.String())
    {
        return *this;
    }

    const uSys length = strLength(string);

    m_Data.Reset(string, length);
    m_Hash = findHashCode(string);
    
    return *this;
}

template<typename _C>
inline bool DynStringT<_C>::equals(const ConstExprStringT<_C>& other) const noexcept
{
    if(m_Data.Length != other._length ) { return false; }
    if(m_Hash != other._hash) { return false; }
    return strCompare(c_str(), other._string) == 0;
}

template<typename _C>
inline bool DynStringT<_C>::equals(const DynStringT<_C>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_Data.Length == other.m_Data.Length && m_Hash == other.m_Hash)
    {
        if(m_Data.Length >= 16 && m_Data.String() == other.m_Data.String())
        {
            return true;
        }

        return strCompare(m_Data.String(), other.m_Data.String()) == 0;
    }
    return false;
}

template<typename _C>
inline bool DynStringT<_C>::equals(const DynStringViewT<_C>& other) const noexcept
{
    if(m_Data.Length != other._length ) { return false; }
    if(m_Hash != other._hash) { return false; }
    return strCompare(m_Data.String(), other._string, other.Length) == 0;
}

template<typename _C>
inline bool DynStringT<_C>::equals(const _C* const str) const noexcept
{ return strCompare(c_str(), str) == 0; }

template<typename _C>
inline i32 DynStringT<_C>::compareTo(const ConstExprStringT<_C>& other) const noexcept
{ return strCompare(c_str(), other._string()); }

template<typename _C>
inline i32 DynStringT<_C>::compareTo(const DynStringT<_C>& other) const noexcept
{ return strCompare(c_str(), other.c_str()); }

template<typename _C>
inline i32 DynStringT<_C>::compareTo(const DynStringViewT<_C>& other) const noexcept
{ return strCompare(c_str(), other._string, other._length); }

template<typename _C>
inline i32 DynStringT<_C>::compareTo(const _C* const str) const noexcept
{ return strCompare(c_str(), str); }

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::concat(const ConstExprStringT<_C>& other) const noexcept
{ return _concat(other._length, other._string); }

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::concat(const DynStringT<_C>& other) const noexcept
{ return _concat(other.m_Data.Length, other.c_str()); }

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::concat(const DynStringViewT<_C>& other) const noexcept
{ return _concat(other._length, other._string); }

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::concat(const _C* const other) const noexcept
{ return _concat(strLength(other), other); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::_concat(const uSys len, const Char* const str) const noexcept
{
    const uSys newLen = m_Data.Length + len;
    if(newLen >= 16)
    {
        Char* const newStr = new(::std::nothrow) Char[newLen + 1];
        newStr[newLen] = '\0';
        ::std::memcpy(newStr, c_str(), m_Data.Length * sizeof(Char));
        ::std::memcpy(newStr + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(newStr, newLen);
    }
    else
    {
        Char buf[16];
        buf[newLen] = Char{'\0'};
        
        ::std::memcpy(buf, m_Data.Buffer.StackString, 16); // Perform a fast intrinsic copy.
        ::std::memcpy(buf + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(static_cast<const Char*>(buf), newLen);
    }
}

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::subString(const uSys begin, const uSys end) const noexcept
{
    if(begin >= end || end > m_Data.Length)
    { return DynStringT<_C>(); }

    const uSys length = end - begin;

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + begin, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + begin);
}

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::subStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > m_Data.Length)
    {
        return DynStringT<_C>("");
    }

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + begin, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + begin);
}

template<typename _C>
inline DynStringT<_C> DynStringT<_C>::subString(const uSys from) const noexcept
{
    if(from > m_Data.Length)
    {
        return DynStringT<_C>("");
    }

    const uSys length = m_Data.Length - from;

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + from, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + from);
}

template<typename _C>
inline _C DynStringT<_C>::operator[](const uSys index) const noexcept
{ return c_str()[index]; }

template<typename _C>
inline _C DynStringT<_C>::at(const uSys index) const noexcept
{
    if(index >= m_Data.Length) { return '\0'; }
    return c_str()[index];
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(const uSys begin, const uSys length, const DynStringT<_C>& str) noexcept
    : _string(str.c_str() + begin)
    , _refCount(null)
    , _length(length)
    , _hash(findHashCode(_string, length))
{
    if(str._length >= 16)
    {
        _refCount = str._largeString.refCount;
        ++(*_refCount);
    }
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(const uSys begin, const uSys length, const DynStringViewT<_C>& str) noexcept
    : _string(str.c_str() + begin)
    , _refCount(str._refCount)
    , _length(length)
    , _hash(findHashCode(_string, length))
{
    if(_refCount) 
    { ++(*_refCount); }
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(const DynStringT<_C>& str, const uSys begin, const uSys end) noexcept
    : _string(str.c_str() + begin)
    , _refCount(null)
    , _length(end - begin)
    , _hash(findHashCode(_string, _length))
{
    if(str._length >= 16)
    {
        _refCount = str._largeString.refCount;
        ++(*_refCount);
    }
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(const DynStringViewT<_C>& str, const uSys begin, const uSys end) noexcept
    : _string(str.c_str() + begin)
    , _refCount(str._refCount)
    , _length(end - begin)
    , _hash(findHashCode(_string, _length))
{
    if(_refCount) 
    { ++(*_refCount); }
}

template<typename _C>
inline DynStringViewT<_C>::~DynStringViewT() noexcept
{
    if(_refCount && --(*_refCount) == 0)
    {
        delete _refCount;
        // Was this allocated as a single block.
        if(reinterpret_cast<iPtr>(_refCount) != reinterpret_cast<iPtr>(_string) - static_cast<iPtr>(sizeof(uSys)))
        {
            delete[] _string;
        }
    }
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(const DynStringViewT<_C>& copy) noexcept
    : _string(copy._string)
    , _refCount(copy._refCount)
    , _length(copy._length)
    , _hash(copy._hash)
{
    if(_refCount) 
    { ++(*_refCount); }
}

template<typename _C>
inline DynStringViewT<_C>::DynStringViewT(DynStringViewT<_C>&& move) noexcept
    : _string(move._string)
    , _refCount(move._refCount)
    , _length(move._length)
    , _hash(move._hash)
{ move._refCount = null; }

template<typename _C>
inline DynStringViewT<_C>& DynStringViewT<_C>::operator=(const DynStringViewT<_C>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    if(_refCount && --(*_refCount) == 0)
    {
        delete _refCount;
        // Was this allocated as a single block.
        if(reinterpret_cast<iPtr>(_refCount) != reinterpret_cast<iPtr>(_string) - sizeof(uSys))
        {
            delete[] _string;
        }
    }

    _string = copy._string;
    _refCount = copy._refCount;
    _length = copy._length;
    _hash = copy._hash;

    if(_refCount) { ++(*_refCount); }

    return *this;
}

template<typename _C>
inline DynStringViewT<_C>& DynStringViewT<_C>::operator=(DynStringViewT<_C>&& move) noexcept
{
    if(this == &move)
    { return *this; }

    if(_refCount && --(*_refCount) == 0)
    {
        delete _refCount;
        // Was this allocated as a single block.
        if(reinterpret_cast<iPtr>(_refCount) != reinterpret_cast<iPtr>(_string) - sizeof(uSys))
        {
            delete[] _string;
        }
    }

    _string = move._string;
    _refCount = move._refCount;
    _length = move._length;
    _hash = move._hash;

    move._refCount = null;

    return *this;
}

template<typename _C>
inline DynStringViewT<_C> DynStringViewT<_C>::create(const DynStringT<_C>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename _C>
inline DynStringViewT<_C> DynStringViewT<_C>::create(const DynStringViewT<_C>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename _C>
inline DynStringViewT<_C>& DynStringViewT<_C>::reset(const DynStringT<_C>& str, const uSys begin, const uSys end) noexcept
{
    if(end > str._length)
    { return *this; }

    if(_refCount && --(*_refCount) == 0)
    {
        delete _refCount;
        // Was this allocated as a single block.
        if(reinterpret_cast<iPtr>(_refCount) != reinterpret_cast<iPtr>(_string) - sizeof(uSys))
        {
            delete[] _string;
        }
    }

    _string = str.c_str() + begin;
    if(str._length >= 16)
    {
        _refCount = str._largeString.refCount;
        ++(*_refCount);
    }
    else
    {
        _refCount = null;
    }
    _length = end - begin;
    _hash = findHashCode(_string, _length);

    return *this;
}

template<typename _C>
inline DynStringViewT<_C>& DynStringViewT<_C>::resetLen(const DynStringT<_C>& str, const uSys begin, const uSys length) noexcept
{
    if(begin + length > str._length)
    { return *this; }

    if(_refCount && --(*_refCount) == 0)
    {
        delete _refCount;
        // Was this allocated as a single block.
        if(reinterpret_cast<iPtr>(_refCount) != reinterpret_cast<iPtr>(_string) - sizeof(uSys))
        {
            delete[] _string;
        }
    }

    _string = str.c_str() + begin;
    if(str._length >= 16)
    {
        _refCount = str._largeString.refCount;
        ++(*_refCount);
    }
    else
    {
        _refCount = null;
    }
    _length = length;
    _hash = findHashCode(_string, _length);

    return *this;
}

template<typename _C>
inline bool DynStringViewT<_C>::equals(const DynStringT<_C>& other) const noexcept
{
    if(_length == other._length && _hash == other._hash)
    {
        return strCompare(_string, other.c_str()) == 0;
    }
    return false;
}

template<typename _C>
inline bool DynStringViewT<_C>::equals(const DynStringViewT<_C>& other) const noexcept
{
    if(this == &other) { return true; }
    if(_length == other._length && _hash == other._hash)
    {
        return strCompare(_string, other._string, _length) == 0;
    }
    return false;
}

template<typename _C>
inline bool DynStringViewT<_C>::equals(const _C* str) const noexcept
{
    return strCompare(_string, str, _length) == 0;
}

template<typename _C>
inline i32 DynStringViewT<_C>::compareTo(const DynStringViewT<_C>& other) const noexcept
{
    if(this == &other) { return 0; }
    return strCompare(_string, other._string, minT(_length, other._length));
}

template<typename _C>
inline i32 DynStringViewT<_C>::compareTo(const DynStringT<_C>& other) const noexcept
{
    return strCompare(_string, other.c_str(), minT(_length, other._length));
}

template<typename _C>
inline i32 DynStringViewT<_C>::compareTo(const _C* str) const noexcept
{
    return strCompare(_string, str, _length);
}

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::concat(const DynStringT<_C>& other) const noexcept
{ return _concat(other._length, other._length >= 16 ? other._largeString.string : other._stackString); }

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::concat(const DynStringViewT<_C>& other) const noexcept
{ return _concat(other._length, other._string); }

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::concat(const _C* const other) const noexcept
{
    const uSys otherLen = strlen(other);
    return _concat(otherLen, other);
}

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::_concat(const uSys len, const _C* const str) noexcept
{
    const uSys newLen = _length + len;
    if(newLen >= 16)
    {
        _C* const newStr = new(::std::nothrow) _C[newLen + 1];
        newStr[newLen] = '\0';
        ::std::memcpy(newStr, _string, _length * sizeof(_C));
        ::std::memcpy(newStr + _length, str, len * sizeof(_C));
        return DynStringT<_C>(newStr, newLen);
    }
    else
    {
        DynStringT<_C> tmp(newLen);
        tmp._stackString[newLen] = '\0';
        ::std::memcpy(tmp._stackString, _string, _length * sizeof(_C));
        ::std::memcpy(tmp._stackString + _length, str, len * sizeof(_C));
        tmp._hash = findHashCode(tmp._stackString);
        return tmp;
    }
}

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::subString(const uSys begin, const uSys end) const noexcept
{
    if(begin >= end || end > _length)
    { return DynStringT<_C>(); }

    const uSys length = end - begin;

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + begin, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + begin);
}

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::subStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > _length)
    { return DynStringT<_C>(""); }

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + begin, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + begin);
}

template<typename _C>
inline DynStringT<_C> DynStringViewT<_C>::subString(const uSys from) const noexcept
{
    if(from > _length)
    { return DynStringT<_C>(""); }

    const uSys length = _length - from;

    if(length >= 16)
    {
        _C* const sub = new(::std::nothrow) _C[length + 1];
        sub[length] = '\0';
        ::std::memcpy(sub, c_str() + from, length * sizeof(_C));
        return DynStringT<_C>(sub, length);
    }

    return DynStringT<_C>(length, c_str() + from);
}

template<typename _C>
inline _C DynStringViewT<_C>::operator[](const uSys index) const noexcept
{ return _string[index]; }

template<typename _C>
inline _C DynStringViewT<_C>::at(const uSys index) const noexcept
{
    if(index >= _length) { return '\0'; }
    return _string[index];
}

template<typename _C>
inline StringBuilderT<_C>::StringBuilderT() noexcept
    : _string(new(::std::nothrow) _C[64])
    , _length(0)
    , _size(64)
{ }

template<typename _C>
inline StringBuilderT<_C>::StringBuilderT(const uSys initialSize) noexcept
    : _string(new(::std::nothrow) _C[maxT(initialSize, 1)])
    , _length(0)
    , _size(maxT(initialSize, 1))
{ }

template<typename _C>
inline StringBuilderT<_C>::StringBuilderT(const _C* const string) noexcept
    : _string(null)
    , _length(strLength(string))
    , _size(_length + 64)
{
    _C* const str = new(::std::nothrow) _C[_size];

    if(string)
    { ::std::memcpy(str, string, (_length + 1) * sizeof(_C)); }

    _string = str;
}

template<typename _C>
inline StringBuilderT<_C>::~StringBuilderT() noexcept
{ delete[] _string; }

template<typename _C>
inline StringBuilderT<_C>::StringBuilderT(const StringBuilderT<_C>& copy) noexcept
    : _string(new(::std::nothrow) _C[copy._size])
    , _length(copy._length)
    , _size(copy._size)
{ ::std::memcpy(_string, copy._string, _size); }

template<typename _C>
inline StringBuilderT<_C>::StringBuilderT(StringBuilderT<_C>&& move) noexcept
    : _string(move._string)
    , _length(move._length)
    , _size(move._size)
{ move._string = null; }

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::operator=(const StringBuilderT<_C>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    delete[] _string;

    _string = new(::std::nothrow) _C[copy._size];
    _length = copy._length;
    _size = copy._size;

    ::std::memcpy(_string, copy._string, _size * sizeof(_C));

    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::operator=(StringBuilderT<_C>&& move) noexcept
{
    if(this == &move)
    { return *this; } 

    delete[] _string;

    _string = move._string;
    _length = move._length;
    _size = move._size;

    move._string = null;

    return *this;
}

template<typename _C>
inline bool StringBuilderT<_C>::equals(const StringBuilderT<_C>& other) const noexcept
{
    if(_string == other._string) { return true; }
    if(_length == other._length)
    {
        return strCompare(_string, other._string) == 0;
    }
    return false;
}

template<typename _C>
inline i32 StringBuilderT<_C>::compareTo(const StringBuilderT<_C>& other) const noexcept
{
    if(_string == other._string) { return 0; }
    return strCompare(_string, other._string);
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const ConstExprStringT<_C>& string) noexcept
{
    append(string._string, string._length);
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const DynStringT<_C>& string) noexcept
{
    append(string.c_str(), string._length);
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const DynStringViewT<_C>& string) noexcept
{
    append(string._string, string._length);
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const StringBuilderT<_C>& string) noexcept
{
    append(string._string, string._length);
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const _C* const string) noexcept
{
    append(string, strLength(string));
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const _C c) noexcept
{
    const _C string[2] = { c,_C{ '\0' } };
    append(string, 1);
    return *this;
}

template<typename _C>
inline StringBuilderT<_C>& StringBuilderT<_C>::append(const c32 c) noexcept
{
    _C string[5] = { _C{ '\0' }, _C{ '\0' }, _C{ '\0' }, _C{ '\0' }, _C{ '\0' } };

    if(c <= 0x7F)
    {
        string[0] = static_cast<_C>(c);
        append(string, 1);
    }
    else if(c <= 0x07FF)
    {
        string[0] = _C{ 0b11000000 } | static_cast<_C>(c >> 6);
        string[1] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
        append(string, 2);
    }
    else if(c <= 0xFFFF)
    {
        string[0] = _C{ 0b11100000 } | static_cast<_C>(c >> 12);
        string[1] = _C{ 0b10000000 } | static_cast<_C>((c >> 6) & 0x3F);
        string[2] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
        append(string, 3);
    }
    else if(c <= 0x10FFFF)
    {
        string[0] = _C{ 0b11100000 } | static_cast<_C>(c >> 18);
        string[1] = _C{ 0b10000000 } | static_cast<_C>((c >> 12) & 0x3F);
        string[2] = _C{ 0b10000000 } | static_cast<_C>((c >> 6) & 0x3F);
        string[3] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
        append(string, 4);
    }

    return *this;
}
//
// template<>
// inline StringBuilderT<c16>& StringBuilderT<c16>::append(const c32 c) noexcept
// {
//     c16 string[3] = { c16{ '\0' }, c16{ '\0' }, c16{ '\0' } };
//
//     if(c <= 0x7F)
//     {
//         string[0] = static_cast<_C>(c);
//         append(string, 1);
//     }
//     else if(c <= 0x07FF)
//     {
//         string[0] = _C{ 0b11000000 } | static_cast<_C>(c >> 6);
//         string[1] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
//         append(string, 2);
//     }
//     else if(c <= 0xFFFF)
//     {
//         string[0] = _C{ 0b11100000 } | static_cast<_C>(c >> 12);
//         string[1] = _C{ 0b10000000 } | static_cast<_C>((c >> 6) & 0x3F);
//         string[2] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
//         append(string, 3);
//     }
//     else if(c <= 0x10FFFF)
//     {
//         string[0] = _C{ 0b11100000 } | static_cast<_C>(c >> 18);
//         string[1] = _C{ 0b10000000 } | static_cast<_C>((c >> 12) & 0x3F);
//         string[2] = _C{ 0b10000000 } | static_cast<_C>((c >> 6) & 0x3F);
//         string[3] = _C{ 0b10000000 } | static_cast<_C>(c & 0x3F);
//         append(string, 4);
//     }
//
//     return *this;
// }

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::reset() noexcept
{
    _length = 0;
    _string[0] = '\0';
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::reset(const uSys newSize) noexcept
{
    if(newSize == 0) 
    { return *this; }
    delete[] _string;
    _string = new(::std::nothrow) Char[newSize];
    _size = newSize;
    _length = 0;
    _string[0] = '\0';
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::resetIf(const uSys maxSize, const uSys newSize) noexcept
{
    if(newSize == 0) { return *this; }
    if(maxSize <= newSize) { return *this; }
    if(_size >= maxSize)
    {
        delete[] _string;
        _string = new(::std::nothrow) Char[newSize];
        _size = newSize;
    }
    _length = 0;
    _string[0] = '\0';
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::backspace() noexcept
{
    if(_length > 0)
    {
        --_length;
        _string[_length] = '\0';
    }
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::backspace(const uSys count) noexcept
{
    if(_length > 0)
    {
        const uSys newLen = _length - count;
        if(newLen > _length)
        { return *this; }

        _length = newLen;
        _string[_length] = '\0';
    }
    return *this;
}

template<typename Char>
inline Char StringBuilderT<Char>::operator[](const uSys index) const noexcept
{ return _string[index]; }

template<typename Char>
inline Char StringBuilderT<Char>::at(const uSys index) const noexcept
{
    if(index >= _length) { return '\0'; }
    return _string[index];
}

template<typename Char>
inline void StringBuilderT<Char>::append(const Char* const string, const uSys length) noexcept
{
    const uSys newLen = _length + length;
    if(newLen >= _size)
    {
        const uSys newSize = newLen + (newLen >> 1);
        Char* const newStr = new(::std::nothrow) Char[newSize];
        ::std::memcpy(newStr, _string, (_length + 1) * sizeof(Char));
        _string = newStr;
        _size = newSize;
    }
    ::std::memcpy(_string + _length, string, length * sizeof(Char));
    _length += length;
    _string[_length] = '\0';
}

template<typename CTo, typename CFrom>
DynStringT<CTo> StringCast(const DynStringT<CFrom>&) noexcept
{ return DynStringT<CTo>(); }

template<>
inline DynStringT<char> StringCast(const DynStringT<char>& string) noexcept
{ return string; }

template<>
inline DynStringT<wchar_t> StringCast(const DynStringT<wchar_t>& string) noexcept
{ return string; }

template<>
inline DynStringT<c8> StringCast(const DynStringT<c8>& string) noexcept
{ return string; }

template<>
inline DynStringT<c16> StringCast(const DynStringT<c16>& string) noexcept
{ return string; }

template<>
inline DynStringT<c32> StringCast(const DynStringT<c32>& string) noexcept
{ return string; }

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, char>(const DynStringT<char>& string) noexcept
{
    const char* rawStr = string.c_str();

    ::std::mbstate_t state;
    uSys len;
    errno_t error = mbsrtowcs_s(&len, nullptr, 0, &rawStr, 0, &state);

    if(error != 0)
    {
        return DynStringT<wchar_t>();
    }

    if(len < 16)
    {
        wchar_t newStr[16];
        error = mbsrtowcs_s(&len, newStr, 16, &rawStr, 15, &state);
        
        if(error != 0)
        {
            return DynStringT<wchar_t>();
        }

        return DynStringT<wchar_t>::passControl(newStr);
    }
    else
    {
        void* const placement = operator new(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(wchar_t), ::std::nothrow);
        
        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        wchar_t* const newStr = new(refCount + 1) wchar_t[len + 1];
        error = mbsrtowcs_s(&len, newStr, len + 1, &rawStr, len, &state);
        
        if(error != 0)
        {
            operator delete(placement);
            return DynStringT<wchar_t>();
        }

        return DynStringT<wchar_t>::passControl(refCount, newStr, len);
    }
}

template<>
inline DynStringT<char> StringCast<char, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    const wchar_t* rawStr = string.c_str();

    ::std::mbstate_t state;
    uSys len;
    errno_t error = wcsrtombs_s(&len, nullptr, 0, &rawStr, 0, &state);
    
    if(error != 0)
    {
        return DynStringT<char>();
    }

    if(len < 16)
    {
        char newStr[16];
        error = wcsrtombs_s(&len, newStr, 16, &rawStr, 15, &state);

        if(error != 0)
        {
            return DynStringT<char>();
        }

        return DynStringT<char>::passControl(newStr);
    }
    else
    {
        void* const placement = operator new(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(char), ::std::nothrow);
        
        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        char* const newStr = new(refCount + 1) char[len + 1];
        error = wcsrtombs_s(&len, newStr, len + 1, &rawStr, len, &state);

        if(error != 0)
        {
            operator delete(placement);
            return DynStringT<char>();
        }

        return DynStringT<char>::passControl(refCount, newStr, len);
    }
}

namespace std
{

template<typename Char>
struct hash<ConstExprStringT<Char>>
{
    inline uSys operator()(const ConstExprStringT<Char>& str) const noexcept
    { return str.hashCode(); }
};

template<typename Char>
struct hash<DynStringT<Char>>
{
    inline uSys operator()(const DynStringT<Char>& str) const noexcept
    { return str.hashCode(); }
};

template<typename Char>
struct hash<DynStringViewT<Char>>
{
    inline uSys operator()(const DynStringViewT<Char>& str) const noexcept
    { return str.hashCode(); }
};

}
