// ReSharper disable CppClangTidyCertDcl58Cpp
#pragma once

#if defined(STRING_IN_DEV) || 1
#include "String.hpp"
#endif

#include <cstring>
#include <cwctype>
#include <locale>
#include "TUMaths.hpp"
#include "allocator/TauAllocator.hpp"

namespace tau::string {

template<typename Char>
template<uSys Len>
constexpr LargeString<Char>::LargeString(const Char(& str)[Len]) noexcept
    : RefCount(nullptr)
    , Buffer(str)
{ }

template<typename Char>
constexpr LargeString<Char>::LargeString() noexcept
    : RefCount(nullptr)
    , Buffer(nullptr)
{ }

template<typename Char>
LargeString<Char>::LargeString(const Char* const buffer, const uSys codeUnits) noexcept
{
    void* newBuffer = ::TauUtilsAllocate((codeUnits + 1) * sizeof(Char) + sizeof(ReferenceCounter::Type));
    RefCount = ReferenceCounter(::new(newBuffer) ReferenceCounter::Type(0));
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
            ::TauUtilsDeallocate(const_cast<Char*>(Buffer));
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
        ::new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
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
        TU_DELETE(refCount);
    }
    else
    {
        ::new(Buffer.LargeString) LargeString<Char>(refCount, buffer);
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
        (void) ::std::memcpy(Buffer.StackString, copy.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::new(Buffer.LargeString) LargeString<Char>(copy.Buffer.AsLargeString());
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
        (void) ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::new(Buffer.LargeString) LargeString<Char>(::std::move(move.Buffer.AsLargeString()));
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
        (void) ::std::memcpy(Buffer.StackString, copy.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::new(Buffer.LargeString) LargeString<Char>(copy.Buffer.AsLargeString());
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
        (void) ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::new(Buffer.LargeString) LargeString<Char>(::std::move(move.Buffer.AsLargeString()));
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

    ::new(Buffer.LargeString) LargeString<Char>(str);

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
        ::new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
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
    
    ::new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
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

template<typename Char>
inline uSys strLength(const Char* const str) noexcept
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
inline i32 strCompare(const C* lhs, const C* rhs) noexcept
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
inline i32 strCompare(const C* lhs, const C* rhs, const uSys length) noexcept
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

template<typename Char, uSys Len>
inline constexpr uSys cexpr::strlen(const Char(&str)[Len]) noexcept
{ return Len - 1; }

template<typename Char, uSys Len>
inline constexpr bool cexpr::streq(const Char(&lhs)[Len], const Char(&rhs)[Len])
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

template<typename Char, uSys Len>
inline constexpr bool cexpr::streqIgnoreCase(const Char(&lhs)[Len], const Char(&rhs)[Len])
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

template<typename Char, uSys Len>
inline constexpr uSys cexpr::findHashCode(const Char(&str)[Len]) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; str[i]; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename Char>
inline StringIteratorT<Char>::StringIteratorT(const Char* const string, const uSys length, const uSys index) noexcept
    : _string(string)
    , _length(length)
    , _index(index)
{ }

template<typename Char>
inline StringIteratorT<Char>& StringIteratorT<Char>::operator++() noexcept
{
    if(_index < _length - 1)
    { ++_index; }
    return *this;
}

template<typename Char>
inline StringIteratorT<Char>& StringIteratorT<Char>::operator--() noexcept
{
    if(_index > 0)
    { --_index; }
    return *this;
}

template<typename Char>
inline StringIteratorT<Char> StringIteratorT<Char>::operator++(int) noexcept
{
    const StringIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
inline StringIteratorT<Char> StringIteratorT<Char>::operator--(int) noexcept
{
    const StringIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
inline Char StringIteratorT<Char>::operator*() const noexcept
{ return _string[_index]; }

template<typename Char>
inline bool StringIteratorT<Char>::operator==(const StringIteratorT<Char>& other) const noexcept
{ return _index == other._index; }

template<typename Char>
inline bool StringIteratorT<Char>::operator!=(const StringIteratorT<Char>& other) const noexcept
{ return _index != other._index; }

template<typename Char>
DynStringCodeUnitIteratorT<Char>::DynStringCodeUnitIteratorT(const StringData& string, const uSys index, const uSys start) noexcept
    : m_String(string)
    , m_Start(start)
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
    if(m_Index > m_Start)
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
DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start) noexcept
    : m_String(string)
    , m_Start(start)
    , m_Index(index)
    , m_CurrentCodePoint(string.Length > 0 ? string.String()[index] : Char{ '\0' })
{ }

template<typename Char>
DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start, const c32 currentCodePoint) noexcept
    : m_String(string)
    , m_Start(start)
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
    if(m_Index > m_Start)
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

template<typename Char>
template<uSys Len>
inline constexpr ConstExprStringT<Char>::ConstExprStringT(const Char(&str)[Len]) noexcept
    : m_String(str)
    , m_Length(Len - 1)
    , m_Hash(cexpr::findHashCode(str))
{ }

template<typename Char>
template<uSys Len>
inline constexpr ConstExprStringT<Char>& ConstExprStringT<Char>::operator=(const Char(&str)[Len]) noexcept
{
    m_String = str;
    m_Length = Len - 1;
    m_Hash = cexpr::findHashCode(str);

    return *this;
}

template<typename Char>
template<uSys Len>
inline constexpr bool ConstExprStringT<Char>::equals(const Char(&str)[Len]) const noexcept
{
    if(m_String == str) { return true; }
    if(m_Length != Len) { return false; }
    return strCompare(m_String, str) == 0;
}

template<typename Char>
inline bool ConstExprStringT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_String == other.String()) { return true; }
    if(m_Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline bool ConstExprStringT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length != other.m_Length) { return false; }
    if(m_Hash != other.m_Hash) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline bool ConstExprStringT<Char>::Equals(const DynStringT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline bool ConstExprStringT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String(), m_Length) == 0;
}

template<typename Char>
inline bool ConstExprStringT<Char>::Equals(const Char* const str) const noexcept
{
    if(m_String == str) { return true; }
    return strCompare(m_String, str) == 0;
}

template<typename Char>
inline i32 ConstExprStringT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline i32 ConstExprStringT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline i32 ConstExprStringT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline i32 ConstExprStringT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{ return strCompare(m_String, other.String(), minT(m_Length, other.Length())); }

template<typename Char>
inline i32 ConstExprStringT<Char>::CompareTo(const Char* const str) const noexcept
{ return strCompare(m_String, str); }

template<typename Char>
inline Char ConstExprStringT<Char>::operator[](const uSys index) const noexcept
{ return m_String[index]; }

template<typename Char>
inline Char ConstExprStringT<Char>::at(const uSys index) const noexcept
{
    if(index >= m_Length) { return Char{ '\0' }; }
    return m_String[index];
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
inline DynStringT<Char> DynStringT<Char>::PassControl(const Char* const str, ::std::function<void(Char*)> deleteStr) noexcept
{
    const uSys len = strLength(str);
    if(len < 16)
    {
        DynStringT<Char> tmp(str);
        // Use callback to delete string.
        if(deleteStr)
        {
            deleteStr(const_cast<Char* const>(str));
        }
        return tmp;
    }
    return DynStringT<Char>(str, len);
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::PassControl(ReferenceCounter::Type* const refCount, const Char* const str, const uSys length, ::std::function<void(Char*)> deleteStr, ::std::function<void(ReferenceCounter::Type*)> deleteRefCount) noexcept
{
    if(length < 16)
    {
        DynStringT<Char> tmp(str);
        if(deleteStr)
        {
            deleteStr(const_cast<Char* const>(str));
        }
        if(deleteRefCount)
        {
            deleteRefCount(refCount);
        }
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
inline DynStringT<Char>::DynStringT(const uSys length, const Char* string) noexcept
    : m_Data(string, length)
    , m_Hash(findHashCode(string, length))
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT(const ConstExprStringT<Char>& string) noexcept
    : m_Data(string.String())
    , m_Hash(string.HashCode())
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT(const DynStringViewT<Char>& string) noexcept
    : m_Data(string.String(), string.Length())
    , m_Hash(findHashCode(string.String(), string.Length()))
{ }

template<typename Char>
inline DynStringT<Char>::DynStringT(const StringBaseT<Char>&string) noexcept
    : m_Data(string.String(), string.Length())
    , m_Hash(findHashCode(string.String(), string.Length()))
{ }

template<typename Char>
inline DynStringT<Char>& DynStringT<Char>::operator=(const Char* const string) noexcept
{
    if(!string)
    {
        return *this;
    }
    
    if(string == m_Data.String())
    {
        return *this;
    }

    const uSys length = strLength(string);

    m_Data.Reset(string, length);
    m_Hash = findHashCode(string);
    
    return *this;
}

template<typename Char>
inline DynStringT<Char>& DynStringT<Char>::operator=(const ConstExprStringT<Char>& string) noexcept
{
    m_Data.Reset(string, string.Length());
    m_Hash = findHashCode(string);

    return *this;
}

template<typename Char>
inline DynStringT<Char>& DynStringT<Char>::operator=(const DynStringViewT<Char>& string) noexcept
{
    m_Data.Reset(string, string.Length());
    m_Hash = findHashCode(string, string.Length());

    return *this;
}

template<typename Char>
inline bool DynStringT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_Data.Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_Data.String(), other.String(), m_Data.Length) == 0;
}

template<typename Char>
inline bool DynStringT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_Data.Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(String(), other.String()) == 0;
}

template<typename Char>
inline bool DynStringT<Char>::Equals(const DynStringT<Char>& other) const noexcept
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

template<typename Char>
inline bool DynStringT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(m_Data.Length != other.Length() ) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_Data.String(), other.String(), m_Data.Length) == 0;
}

template<typename Char>
inline bool DynStringT<Char>::Equals(const Char* const str) const noexcept
{ return strCompare(String(), str) == 0; }

template<typename Char>
inline i32 DynStringT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline i32 DynStringT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline i32 DynStringT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline i32 DynStringT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{ return strCompare(String(), other.String(), other.Length()); }

template<typename Char>
inline i32 DynStringT<Char>::CompareTo(const Char* const str) const noexcept
{ return strCompare(String(), str); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::Concat(const StringBaseT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::Concat(const ConstExprStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::Concat(const DynStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::Concat(const DynStringViewT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::Concat(const Char* const other) const noexcept
{ return _concat(strLength(other), other); }

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::_concat(const uSys len, const Char* const str) const noexcept
{
    const uSys newLen = m_Data.Length + len;
    if(newLen >= 16)
    {
        Char* const newStr = TU_NEW_ARR(Char, newLen + 1);
        newStr[newLen] = '\0';
        (void) ::std::memcpy(newStr, String(), m_Data.Length * sizeof(Char));
        (void) ::std::memcpy(newStr + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(newStr, newLen);
    }
    else
    {
        Char buf[16];
        buf[newLen] = Char{ '\0' };
        
        (void) ::std::memcpy(buf, m_Data.Buffer.StackString, sizeof(buf)); // Perform a fast intrinsic copy.
        (void) ::std::memcpy(buf + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(static_cast<const Char*>(buf), newLen);
    }
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::SubStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > m_Data.Length)
    {
        return DynStringT<Char>();
    }

    if(length >= 16)
    {
        Char* const sub = TU_NEW_ARR(Char, length + 1);
        sub[length] = Char { '\0' };
        (void) ::std::memcpy(sub, String() + begin, length * sizeof(Char));
        return DynStringT<Char>(sub, length);
    }

    return DynStringT<Char>(length, String() + begin);
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::SubString(const uSys begin, const uSys end) const noexcept
{
    return SubStringLen(begin, end - begin);
}

template<typename Char>
inline DynStringT<Char> DynStringT<Char>::SubString(const uSys from) const noexcept
{
    return SubStringLen(from, Length() - from);
}

template<typename Char>
inline Char DynStringT<Char>::operator[](const uSys index) const noexcept
{ return String()[index]; }

template<typename Char>
inline Char DynStringT<Char>::At(const uSys index) const noexcept
{
    if(index >= m_Data.Length) { return '\0'; }
    return String()[index];
}

template<typename Char>
inline DynStringViewT<Char>::DynStringViewT(const uSys begin, const uSys length, const DynStringT<Char>& str) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(length)
    , m_Hash(findHashCode(m_Data.String() + begin, length))
{ }

template<typename Char>
inline DynStringViewT<Char>::DynStringViewT(const uSys begin, const uSys length, const DynStringViewT<Char>& str) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(length)
    , m_Hash(findHashCode(m_Data.String() + begin, length))
{ }

template<typename Char>
inline DynStringViewT<Char>::DynStringViewT(const DynStringT<Char>& str, const uSys begin, const uSys end) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(end - begin)
    , m_Hash(findHashCode(m_Data.String() + begin, m_Length))
{ }

template<typename Char>
inline DynStringViewT<Char>::DynStringViewT(const DynStringViewT<Char>& str, const uSys begin, const uSys end) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(end - begin)
    , m_Hash(findHashCode(m_Data.String() + begin, m_Length))
{ }

template<typename Char>
inline DynStringViewT<Char> DynStringViewT<Char>::Create(const DynStringT<Char>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename Char>
inline DynStringViewT<Char> DynStringViewT<Char>::Create(const DynStringViewT<Char>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename Char>
inline DynStringViewT<Char>& DynStringViewT<Char>::Reset(const DynStringT<Char>& str, const uSys begin, const uSys end) noexcept
{
    if(end > str.length())
    { return *this; }
    
    m_Data = str.m_Data;
    m_Start = begin;
    m_Length = end - begin;
    m_Hash = findHashCode(m_Data.String() + begin, m_Length);

    return *this;
}

template<typename Char>
inline DynStringViewT<Char>& DynStringViewT<Char>::ResetLen(const DynStringT<Char>& str, const uSys begin, const uSys length) noexcept
{
    if(begin + length > str.length())
    { return *this; }
    
    m_Data = str.m_Data;
    m_Start = begin;
    m_Length = length;
    m_Hash = findHashCode(m_Data.String() + begin, length);

    return *this;
}

template<typename Char>
inline bool DynStringViewT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }

    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline bool DynStringViewT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline bool DynStringViewT<Char>::Equals(const DynStringT<Char>& other) const noexcept
{
    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline bool DynStringViewT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(this == &other) { return true; }

    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline bool DynStringViewT<Char>::Equals(const Char* str) const noexcept
{
    return strCompare(m_Data.String() + m_Start, str, m_Length) == 0;
}

template<typename Char>
inline i32 DynStringViewT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline i32 DynStringViewT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline i32 DynStringViewT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{
    if(this == &other) { return 0; }
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline i32 DynStringViewT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline i32 DynStringViewT<Char>::CompareTo(const Char* str) const noexcept
{
    return strCompare(m_Data.String() + m_Start, str, m_Length);
}

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::Concat(const StringBaseT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::Concat(const ConstExprStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::Concat(const DynStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::Concat(const DynStringViewT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::Concat(const Char* const other) const noexcept
{
    const uSys otherLen = strLength(other);
    return _concat(otherLen, other);
}

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::_concat(const uSys len, const Char* const str) noexcept
{
    const uSys newLen = m_Length + len;
    if(newLen >= 16)
    {
        Char* const newStr = TU_NEW_ARR(Char, newLen + 1);
        newStr[newLen] = Char{ '\0' };
        (void) ::std::memcpy(newStr, String(), m_Length * sizeof(Char));
        (void) ::std::memcpy(newStr + m_Length, str, len * sizeof(Char));
        return DynStringT<Char>(newStr, newLen);
    }
    else
    {
        DynStringT<Char> tmp(newLen);
        tmp.String()[newLen] = Char{ '\0' };
        (void) ::std::memcpy(tmp.String(), String(), m_Length * sizeof(Char));
        (void) ::std::memcpy(tmp.String() + Length(), str, len * sizeof(Char));
        tmp.m_Hash = findHashCode(tmp.String());
        return tmp;
    }
}

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::SubStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > m_Length)
    { return DynStringT<Char>(); }

    if(length >= 16)
    {
        Char* const sub = TU_NEW_ARR(Char, length + 1);
        sub[length] = Char { '\0' };
        (void) ::std::memcpy(sub, String() + begin, length * sizeof(Char));
        return DynStringT<Char>(sub, length);
    }

    return DynStringT<Char>(length, String() + begin);
}

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::SubString(const uSys begin, const uSys end) const noexcept
{
    return SubStringLen(begin, end - begin);
}

template<typename Char>
inline DynStringT<Char> DynStringViewT<Char>::SubString(const uSys from) const noexcept
{
    return SubStringLen(from, m_Length - from);
}

template<typename Char>
inline Char DynStringViewT<Char>::operator[](const uSys index) const noexcept
{ return String()[index]; }

template<typename Char>
inline Char DynStringViewT<Char>::At(const uSys index) const noexcept
{
    if(index >= m_Length) { return Char{ '\0' }; }
    return String()[index];
}

static inline constexpr uSys STRING_BUILDER_DEFAULT_SIZE = 64;

template<typename Char>
inline StringBuilderT<Char>::StringBuilderT() noexcept
    : m_String(TU_NEW_ARR(Char, STRING_BUILDER_DEFAULT_SIZE))
    , m_Length(0)
    , m_Size(STRING_BUILDER_DEFAULT_SIZE)
{ }

template<typename Char>
inline StringBuilderT<Char>::StringBuilderT(const uSys initialSize) noexcept
    : m_String(TU_NEW_ARR(Char, maxT(initialSize, 1)))
    , m_Length(0)
    , m_Size(maxT(initialSize, 2))
{ }

template<typename Char>
inline StringBuilderT<Char>::StringBuilderT(const Char* const string) noexcept
    : m_String(nullptr)
    , m_Length(strLength(string))
    , m_Size(m_Length + STRING_BUILDER_DEFAULT_SIZE)
{
    Char* const str = TU_NEW_ARR(Char, m_Size);

    if(string)
    { (void) ::std::memcpy(str, string, (m_Length + 1) * sizeof(Char)); }

    m_String = str;
}

template<typename Char>
inline StringBuilderT<Char>::~StringBuilderT() noexcept
{ TU_DELETE_ARR(m_String); }

template<typename Char>
inline StringBuilderT<Char>::StringBuilderT(const StringBuilderT<Char>& copy) noexcept
    : m_String(TU_NEW_ARR(Char, copy.m_Size))
    , m_Length(copy.m_Length)
    , m_Size(copy.m_Size)
{ (void) ::std::memcpy(m_String, copy.m_String, m_Size); }

template<typename Char>
inline StringBuilderT<Char>::StringBuilderT(StringBuilderT<Char>&& move) noexcept
    : m_String(move.m_String)
    , m_Length(move.m_Length)
    , m_Size(move.m_Size)
{ move.m_String = nullptr; }

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::operator=(const StringBuilderT<Char>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    TU_DELETE_ARR(m_String);

    m_String = TU_NEW_ARR(Char, copy.m_Size);
    m_Length = copy.m_Length;
    m_Size = copy.m_Size;

    (void) ::std::memcpy(m_String, copy.m_String, m_Size * sizeof(Char));

    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::operator=(StringBuilderT<Char>&& move) noexcept
{
    if(this == &move)
    { return *this; } 

    TU_DELETE_ARR(m_String);

    m_String = move.m_String;
    m_Length = move.m_Length;
    m_Size = move.m_Size;

    move.m_String = nullptr;

    return *this;
}

template<typename Char>
inline bool StringBuilderT<Char>::equals(const StringBuilderT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length == other.m_Length)
    {
        return strCompare(m_String, other.m_String) == 0;
    }
    return false;
}

template<typename Char>
inline i32 StringBuilderT<Char>::compareTo(const StringBuilderT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return 0; }
    return strCompare(m_String, other.m_String);
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const StringBaseT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const ConstExprStringT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const DynStringT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const DynStringViewT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const StringBuilderT<Char>& string) noexcept
{
    Append(string.m_String, string.m_Length);
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const Char* const string) noexcept
{
    Append(string, strLength(string));
    return *this;
}

template<typename Char>
template<typename CharDeduced>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const ::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) noexcept
{
    const Char string[2] = { c, Char{ '\0' } };
    Append(string, 1);
    return *this;
}

template<>
inline StringBuilderT<c8>& StringBuilderT<c8>::append(const c32 c) noexcept
{
    using Char = c8;

    Char string[5] = { Char{ '\0' }, Char{ '\0' }, Char{ '\0' }, Char{ '\0' }, Char{ '\0' } };

    if(c <= 0x7F)
    {
        string[0] = static_cast<Char>(c);
        Append(string, 1);
    }
    else if(c <= 0x07FF)
    {
        string[0] = Char{ 0b11000000 } | static_cast<Char>(c >> 6);
        string[1] = Char{ 0b10000000 } | static_cast<Char>(c & 0x3F);
        Append(string, 2);
    }
    else if(c <= 0xFFFF)
    {
        string[0] = Char{ 0b11100000 } | static_cast<Char>(c >> 12);
        string[1] = Char{ 0b10000000 } | static_cast<Char>((c >> 6) & 0x3F);
        string[2] = Char{ 0b10000000 } | static_cast<Char>(c & 0x3F);
        Append(string, 3);
    }
    else if(c <= 0x10FFFF)
    {
        string[0] = Char{ 0b11100000 } | static_cast<Char>(c >> 18);
        string[1] = Char{ 0b10000000 } | static_cast<Char>((c >> 12) & 0x3F);
        string[2] = Char{ 0b10000000 } | static_cast<Char>((c >> 6) & 0x3F);
        string[3] = Char{ 0b10000000 } | static_cast<Char>(c & 0x3F);
        Append(string, 4);
    }

    return *this;
}

template<>
inline StringBuilderT<c16>& StringBuilderT<c16>::append(const c32 c) noexcept
{
    using Char = c16;

    Char string[3] = { Char{ '\0' }, Char{ '\0' }, Char{ '\0' } };

    if(c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF))
    {
        string[0] = static_cast<Char>(c);
        Append(string, 1);
    }
    else if(c <= 0x10FFFF)
    {
        const c32 uPrime = c - 0x10000;

        string[0] = static_cast<Char>(0xD800 + (uPrime >> 10));
        string[1] = static_cast<Char>(0xDC00 + (uPrime & 0x3FF));
        Append(string, 2);
    }

    return *this;
}

template<>
inline StringBuilderT<c32>& StringBuilderT<c32>::append(const c32 c) noexcept
{
    const c32 string[2] = { c, U'\0' };
    Append(string, 1);
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const StringBaseT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const ConstExprStringT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const DynStringT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const DynStringViewT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const StringBuilderT<CharFrom>& string) noexcept
{
    Append(string.m_String, string.m_Length);
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline StringBuilderT<Char>& StringBuilderT<Char>::append(const CharFrom* const string) noexcept
{
    Append(string, strLength(string));
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::reset() noexcept
{
    m_Length = 0;
    m_String[0] = Char { '\0' };
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::reset(const uSys newSize) noexcept
{
    if(newSize == 0) 
    { return *this; }

    TU_DELETE_ARR(m_String);

    m_String = TU_NEW_ARR(Char, newSize);
    m_Size = newSize;
    m_Length = 0;
    m_String[0] = Char{ '\0' };

    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::resetIf(const uSys maxSize, const uSys newSize) noexcept
{
    if(newSize == 0) { return *this; }
    if(maxSize <= newSize) { return *this; }

    if(m_Size >= maxSize)
    {
        TU_DELETE_ARR(m_String);
        m_String = TU_NEW_ARR(Char, newSize);
        m_Size = newSize;
    }

    m_Length = 0;
    m_String[0] = Char{ '\0' };

    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::backspace() noexcept
{
    if(m_Length > 0)
    {
        --m_Length;
        m_String[m_Length] = '\0';
    }
    return *this;
}

template<typename Char>
inline StringBuilderT<Char>& StringBuilderT<Char>::backspace(const uSys count) noexcept
{
    if(m_Length > 0)
    {
        const uSys newLen = m_Length - count;
        if(newLen > m_Length)
        { return *this; }

        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
    return *this;
}

template<typename Char>
inline Char StringBuilderT<Char>::operator[](const uSys index) const noexcept
{ return m_String[index]; }

template<typename Char>
inline Char StringBuilderT<Char>::at(const uSys index) const noexcept
{
    if(index >= m_Length) { return '\0'; }
    return m_String[index];
}

template<typename Char>
inline void StringBuilderT<Char>::AssertSize(const uSys newLength) noexcept
{
    if(newLength >= m_Size)
    {
        const uSys newSize = newLength + (newLength >> 1);
        Char* const newStr = TU_NEW_ARR(Char, newSize);
        (void) ::std::memcpy(newStr, m_String, (m_Length + 1) * sizeof(Char));
        TU_DELETE_ARR(m_String);
        m_String = newStr;
        m_Size = newSize;
    }
}

template<typename Char>
inline void StringBuilderT<Char>::Append(const Char* const string, const uSys length) noexcept
{
    const uSys newLen = m_Length + length;
    AssertSize(newLen);
    (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<typename Char>
template<typename CharFrom>
inline void StringBuilderT<Char>::Append(const CharFrom* const string, const uSys length) noexcept
{
    const uSys newLen = m_Length + length;
    AssertSize(newLen);
    (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<char>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    ::std::mbstate_t state;
    uSys len;
    errno_t error = wcsrtombs_s(&len, nullptr, 0, &string, 0, &state);

    if(error != 0)
    {
        return;
    }

    const uSys newLen = m_Length + len - 1;
    AssertSize(newLen);

    error = wcsrtombs_s(&len, m_String + m_Length, len + 1, &string, len, &state);

    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<char>::Append<c8>(const c8* string, const uSys length) noexcept
{
    Append(reinterpret_cast<const char*>(string), length);
}

template<>
template<>
inline void StringBuilderT<char>::Append<c16>(const c16* string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, reinterpret_cast<c8*>(m_String + m_Length), static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), true);
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<char>::Append<c32>(const c32* string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, reinterpret_cast<c8*>(m_String + m_Length), static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<wchar_t>::Append<char>(const char* string, const uSys length) noexcept
{
    ::std::mbstate_t state;
    uSys len;
    errno_t error = mbsrtowcs_s(&len, nullptr, 0, &string, 0, &state);

    if(error != 0)
    {
        return;
    }

    const uSys newLen = m_Length + len - 1;
    AssertSize(newLen);

    error = mbsrtowcs_s(&len, m_String + m_Length, len + 1, &string, len, &state);

    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<wchar_t>::Append<c8>(const c8* string, const uSys length) noexcept
{
    Append(reinterpret_cast<const char*>(string), length);
}

template<>
template<>
inline void StringBuilderT<wchar_t>::Append<c16>(const c16* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        Append(reinterpret_cast<const wchar_t*>(string), length);
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {

        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodePoints(string, static_cast<iSys>(length));
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform(string, reinterpret_cast<c32*>(m_String + m_Length), static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
}

template<>
template<>
inline void StringBuilderT<wchar_t>::Append<c32>(const c32* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform(string, reinterpret_cast<c16*>(m_String + m_Length), static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        Append(reinterpret_cast<const wchar_t*>(string), length);
    }
}

template<>
template<>
inline void StringBuilderT<c8>::Append<c16>(const c16* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), true);
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c8>::Append<c32>(const c32* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c8>::Append<char>(const char* string, const uSys length) noexcept
{
    Append(reinterpret_cast<const c8*>(string), length);
}

template<>
template<>
inline void StringBuilderT<c8>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    ::std::mbstate_t state;
    uSys len;
    errno_t error = wcsrtombs_s(&len, nullptr, 0, &string, 0, &state);

    if(error != 0)
    {
        return;
    }

    const uSys newLen = m_Length + len;
    AssertSize(newLen);

    error = wcsrtombs_s(&len, reinterpret_cast<char*>(m_String + m_Length), len + 1, &string, len, &state);

    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c16>::Append<c8>(const c8* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c16>::Append<c32>(const c32* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf16::CalculateCodeUnits(string, static_cast<iSys>(length), true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c16>::Append<char>(const char* string, const uSys length) noexcept
{
    Append(reinterpret_cast<const c8*>(string), length);
}

template<>
template<>
inline void StringBuilderT<c16>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        Append(reinterpret_cast<const c16*>(string), length);
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        Append(reinterpret_cast<const c32*>(string), length);
    }
}

template<>
template<>
inline void StringBuilderT<c32>::Append<c8>(const c8* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodePoints(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c32>::Append<c16>(const c16* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf16::CalculateCodePoints(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<>
template<>
inline void StringBuilderT<c32>::Append<char>(const char* string, const uSys length) noexcept
{
    Append(reinterpret_cast<const c8*>(string), length);
}

template<>
template<>
inline void StringBuilderT<c32>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        Append(reinterpret_cast<const c16*>(string), length);
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        Append(reinterpret_cast<const c32*>(string), length);
    }
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
    using Char = wchar_t;

    const char* rawStr = string.String();

    ::std::mbstate_t state;
    uSys len;
    errno_t error = mbsrtowcs_s(&len, nullptr, 0, &rawStr, 0, &state);

    if(error != 0)
    {
        return DynStringT<Char>();
    }

    if(len < 16)
    {
        Char newStr[16];
        error = mbsrtowcs_s(&len, newStr, 16, &rawStr, 15, &state);
        
        if(error != 0)
        {
            return DynStringT<Char>();
        }

        return DynStringT<Char>::passControl(newStr);
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(Char));
        
        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        Char* const newStr = ::new(refCount + 1) Char[len + 1];
        error = mbsrtowcs_s(&len, newStr, len + 1, &rawStr, len, &state);
        
        if(error != 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<Char>();
        }

        return DynStringT<Char>::passControl(refCount, newStr, len, [](const Char* str) {}, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<char> StringCast<char, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    using Char = char;

    const wchar_t* rawStr = string.String();

    ::std::mbstate_t state;
    uSys len;
    errno_t error = wcsrtombs_s(&len, nullptr, 0, &rawStr, 0, &state);
    
    if(error != 0)
    {
        return DynStringT<Char>();
    }

    if(len < 16)
    {
        Char newStr[16];
        error = wcsrtombs_s(&len, newStr, 16, &rawStr, 15, &state);

        if(error != 0)
        {
            return DynStringT<Char>();
        }

        return DynStringT<Char>::passControl(newStr);
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(Char));
        
        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        Char* const newStr = ::new(refCount + 1) Char[len + 1];
        error = wcsrtombs_s(&len, newStr, len + 1, &rawStr, len, &state);

        if(error != 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<Char>();
        }

        return DynStringT<Char>::passControl(refCount, newStr, len, [](const Char* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

namespace std
{

template<typename Char>
struct hash<StringBaseT<Char>>
{
    inline uSys operator()(const ConstExprStringT<Char>& str) const noexcept
    { return str.HashCode(); }
};

template<typename Char>
struct hash<ConstExprStringT<Char>>
{
    inline uSys operator()(const ConstExprStringT<Char>& str) const noexcept
    { return str.HashCode(); }
};

template<typename Char>
struct hash<DynStringT<Char>>
{
    inline uSys operator()(const DynStringT<Char>& str) const noexcept
    { return str.HashCode(); }
};

template<typename Char>
struct hash<DynStringViewT<Char>>
{
    inline uSys operator()(const DynStringViewT<Char>& str) const noexcept
    { return str.HashCode(); }
};

}
