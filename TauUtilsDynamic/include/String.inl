// ReSharper disable CppClangTidyCertDcl58Cpp
// ReSharper disable CppUnreachableCode
#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

#pragma warning(push, 0)
#include <cstring>
#include <cwctype>
#include <cuchar>
#include <locale>
#include <bit>
#include <type_traits>
#include <memory>
#pragma warning(pop)

#include "String.hpp"
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
constexpr LargeString<Char>::LargeString(const Char* const buffer, const uSys codeUnits) noexcept
{
    Char* cBuffer;

    if(::std::is_constant_evaluated())
    {
        // void* newBuffer = ::TauUtilsAllocate((codeUnits + 1) * sizeof(Char) + sizeof(ReferenceCounter::Type));
        RefCount = ReferenceCounter();
        cBuffer = ::TauUtilsAllocateTArr<Char>(codeUnits + 1);
        
    }
    else
    {
        void* newBuffer = ::TauUtilsAllocate((codeUnits + 1) * sizeof(Char) + sizeof(ReferenceCounter::Type));
        RefCount = ReferenceCounter(::std::construct_at(::std::bit_cast<ReferenceCounter::Type*>(newBuffer), 0));
        cBuffer = ::std::bit_cast<Char*>(::std::bit_cast<ReferenceCounter::Type*>(newBuffer) + 1);
    }

    cBuffer[codeUnits] = Char { '\0' };

    (void) ::std::ranges::copy_n(buffer, codeUnits, cBuffer);
    // (void) ::std::memcpy(cBuffer, buffer, codeUnits * sizeof(Char));
    Buffer = cBuffer;
}

template<typename Char>
constexpr LargeString<Char>::LargeString(ReferenceCounter::Type* const refCount, const Char* const buffer) noexcept
    : RefCount(refCount)
    , Buffer(buffer)
{ }

template<typename Char>
constexpr LargeString<Char>::~LargeString() noexcept
{
    if(RefCount.RefCount() == 1)
    {
        if(static_cast<const void*>(RefCount.RefCountPtr() + 1) != static_cast<const void*>(Buffer))
        {
            if(::std::is_constant_evaluated())
            {
                // When executing within a constexpr environment, this just maps to delete[],
                // so we don't need to know the element count.
                ::TauUtilsDeallocateTArr(const_cast<Char*>(Buffer), 0);
            }
            else
            {
                ::TauUtilsDeallocate(const_cast<Char*>(Buffer));
            }
        }
    }
}

template<typename Char>
constexpr StringData<Char>::StringData() noexcept
    : Length(0)
{ Buffer.StackString[0] = Char{ '\0' }; }

template<typename Char>
constexpr StringData<Char>::StringData(::std::nullptr_t) noexcept
    : Length(0)
{ Buffer.StackString[0] = Char{ '\0' }; }

template<typename Char>
template<uSys Len>
constexpr StringData<Char>::StringData(const Char(& str)[Len]) noexcept
    : Buffer { }
    , Length(Len)
{
    if constexpr(Len < 16)
    {
        Buffer.StackString[Len] = Char { '\0' };
        (void) ::std::ranges::copy_n(str, Len, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, buffer, Len * sizeof(Char));
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), str, Len);
    }
}

template<typename Char>
constexpr StringData<Char>::StringData(const Char* const buffer, const uSys codeUnits) noexcept
    : Buffer { }
    , Length(codeUnits)
{
    if(codeUnits < 16)
    {
        Buffer.StackString[codeUnits] = Char { '\0' };
        (void) ::std::ranges::copy_n(buffer, codeUnits, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), buffer, codeUnits);
    }
}

template<typename Char>
constexpr StringData<Char>::StringData(ReferenceCounter::Type* const refCount, const Char* const buffer, const uSys codeUnits) noexcept
    : Length(codeUnits)
{
    if(codeUnits < 16)
    {
        Buffer.StackString[codeUnits] = Char { '\0' };
        (void) ::std::ranges::copy_n(buffer, codeUnits, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
        ::TauUtilsDeallocate(refCount);
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), refCount, buffer);
    }
}

template<typename Char>
constexpr StringData<Char>::~StringData() noexcept
{
    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }
}

template<typename Char>
constexpr StringData<Char>::StringData(const StringData& copy) noexcept
    : Length(copy.Length)
{
    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        (void) ::std::ranges::copy_n(copy.Buffer.StackString, 16, Buffer.StackString);
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), copy.Buffer.AsLargeString());
    }
}

template<typename Char>
constexpr StringData<Char>::StringData(StringData&& move) noexcept
    : Length(move.Length)
{
    if(Length == 0)
    {
        Buffer.StackString[0] = Char{ '\0' };
    }
    else if(Length < 16)
    {
        // If we pass in length we fallback to the slower generic memcpy implementation instead of the intrinsic.
        (void) ::std::ranges::copy_n(move.Buffer.StackString, 16, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), ::std::move(move.Buffer.AsLargeString()));
    }
}

template<typename Char>
constexpr StringData<Char>& StringData<Char>::operator=(const StringData& copy) noexcept
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
        (void) ::std::ranges::copy_n(copy.Buffer.StackString, 16, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, copy.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), copy.Buffer.AsLargeString());
    }

    return *this;
}

template<typename Char>
constexpr StringData<Char>& StringData<Char>::operator=(StringData&& move) noexcept
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
        (void) ::std::ranges::copy_n(move.Buffer.StackString, 16, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, move.Buffer.StackString, 16 * sizeof(Char));
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), ::std::move(move.Buffer.AsLargeString()));
    }

    return *this;
}

template<typename Char>
template<uSys Len>
constexpr StringData<Char>& StringData<Char>::operator=(const Char(& str)[Len]) noexcept
{
    if(String() == str)
    { return *this; }
    
    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = Len;

    ::std::construct_at(Buffer.AsLargeStringPtr(), str);

    return *this;
}

template<typename Char>
constexpr void StringData<Char>::Reset(const Char* const buffer, const uSys codeUnits) noexcept
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
        (void) ::std::ranges::copy_n(buffer, codeUnits, Buffer.StackString);
        // (void) ::std::memcpy(Buffer.StackString, buffer, codeUnits * sizeof(Char));
        Buffer.StackString[codeUnits] = Char{ '\0' };
    }
    else
    {
        ::std::construct_at(Buffer.AsLargeStringPtr(), buffer, codeUnits);
        // ::new(Buffer.LargeString) LargeString<Char>(buffer, codeUnits);
    }
}

template<typename Char>
constexpr void StringData<Char>::Reset(ReferenceCounter::Type* const refCount, const Char* const buffer, const uSys codeUnits) noexcept
{
    if(String() == buffer)
    { return; }

    if(Length >= 16)
    {
        Buffer.AsLargeString().~LargeString();
    }

    Length = codeUnits;

    ::std::construct_at(Buffer.AsLargeStringPtr(), refCount, buffer);
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

#ifdef _WIN32
template<>
inline c8 toLower<c8>(const c8 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }

template<>
inline c16 toLower<c16>(const c16 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }

template<>
inline c32 toLower<c32>(const c32 c) noexcept
{ return ::std::tolower(c, ::std::locale()); }
#else
template<>
inline c8 toLower<c8>(const c8 c) noexcept
{ return static_cast<c8>(::std::tolower(static_cast<char>(c), ::std::locale())); }

template<>
inline c16 toLower<c16>(const c16 c) noexcept
{ return static_cast<c16>(::std::tolower(static_cast<wchar_t>(c), ::std::locale())); }

template<>
inline c32 toLower<c32>(const c32 c) noexcept
{ return static_cast<c32>(::std::tolower(static_cast<wchar_t>(c), ::std::locale())); }
#endif

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

#ifdef _WIN32
template<>
inline c8 toUpper<c8>(const c8 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }

template<>
inline c16 toUpper<c16>(const c16 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }

template<>
inline c32 toUpper<c32>(const c32 c) noexcept
{ return ::std::toupper(c, ::std::locale()); }
#else
template<>
inline c8 toUpper<c8>(const c8 c) noexcept
{ return static_cast<c8>(::std::toupper(static_cast<char>(c), ::std::locale())); }

template<>
inline c16 toUpper<c16>(const c16 c) noexcept
{ return static_cast<c16>(::std::toupper(static_cast<wchar_t>(c), ::std::locale())); }

template<>
inline c32 toUpper<c32>(const c32 c) noexcept
{ return static_cast<c32>(::std::toupper(static_cast<wchar_t>(c), ::std::locale())); }
#endif

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
inline constexpr uSys findHashCode(const C* str) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; str[i]; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename C>
inline constexpr uSys findHashCode(const C* str, const uSys len) noexcept
{
    uSys hash = 0;
    for(uSys i = 0; i < len; ++i)
    {
        hash = 31u * hash + static_cast<uSys>(str[i]);
    }
    return hash;
}

template<typename Char>
inline constexpr uSys strLengthConst(const Char* const str) noexcept
{
    if(!str)
    { return 0; }

    uSys i = 0;
    // ReSharper disable once CppPossiblyErroneousEmptyStatements
    for(; str[i]; ++i);
    return i;
}

template<typename Char>
inline constexpr uSys strLength(const Char* const str) noexcept
{
    return strLengthConst(str);
}

template<>
inline constexpr uSys strLength<char>(const char* const str) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strLengthConst(str);
    }
    else
    {
        return ::std::strlen(str);
    }
}

template<>
inline constexpr uSys strLength<wchar_t>(const wchar_t* const str) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strLengthConst(str);
    }
    else
    {
        return ::std::wcslen(str);
    }
}

template<typename C>
inline constexpr i32 strCompareConst(const C* lhs, const C* rhs) noexcept
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

template<typename C>
inline constexpr i32 strCompare(const C* lhs, const C* rhs) noexcept
{
    return strCompareConst(lhs, rhs);
}

template<>
inline constexpr i32 strCompare(const char* const lhs, const char* const rhs) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strCompareConst(lhs, rhs);
    }
    else
    {
        return ::std::strcmp(lhs, rhs);
    }
}

template<>
inline constexpr i32 strCompare(const wchar_t* const lhs, const wchar_t* const rhs) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strCompareConst(lhs, rhs);
    }
    else
    {
        return ::std::wcscmp(lhs, rhs);
    }
}

template<typename C>
inline constexpr i32 strCompareConst(const C* lhs, const C* rhs, const uSys length) noexcept
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

template<typename C>
inline constexpr i32 strCompare(const C* lhs, const C* rhs, const uSys length) noexcept
{
    return strCompareConst(lhs, rhs, length);
}

template<>
inline i32 strCompare(const char* const lhs, const char* const rhs, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strCompareConst(lhs, rhs, length);
    }
    else
    {
        return ::std::strncmp(lhs, rhs, length);
    }
}

template<>
inline i32 strCompare(const wchar_t* const lhs, const wchar_t* const rhs, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        return strCompareConst(lhs, rhs, length);
    }
    else
    {
        return ::std::wcsncmp(lhs, rhs, length);
    }
}

template<typename Char, uSys Len>
inline constexpr uSys cexpr::strlen(const Char(&str)[Len]) noexcept
{ return Len - 1; }

template<typename Char, uSys Len>
inline constexpr bool cexpr::streq(const Char(&lhs)[Len], const Char(&rhs)[Len])
{
    i32 i = 0;
    while(lhs[i] != '\0' && rhs[i] != '\0')
    {
        if(lhs[i] != rhs[i])
        { return false; }
        ++i;
    }

    return *lhs == '\0' && *rhs == '\0';
}

template<typename Char, uSys Len>
inline constexpr bool cexpr::streqIgnoreCase(const Char(&lhs)[Len], const Char(&rhs)[Len])
{
    i32 i = 0;
    while(lhs[i] != '\0' && rhs[i] != '\0')
    {
        if(toLower(lhs[i]) != toLower(rhs[i]))
        { return false; }
        ++i;
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
inline constexpr StringIteratorT<Char>::StringIteratorT(const Char* const string, const uSys length, const uSys index) noexcept
    : m_String(string)
    , m_Length(length)
    , m_Index(index)
{ }

template<typename Char>
inline constexpr StringIteratorT<Char>& StringIteratorT<Char>::operator++() noexcept
{
    if(m_Index < m_Length - 1)
    { ++m_Index; }
    return *this;
}

template<typename Char>
inline constexpr StringIteratorT<Char>& StringIteratorT<Char>::operator--() noexcept
{
    if(m_Index > 0)
    { --m_Index; }
    return *this;
}

template<typename Char>
inline constexpr StringIteratorT<Char> StringIteratorT<Char>::operator++(int) noexcept
{
    const StringIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
inline constexpr StringIteratorT<Char> StringIteratorT<Char>::operator--(int) noexcept
{
    const StringIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
inline constexpr Char StringIteratorT<Char>::operator*() const noexcept
{ return m_String[m_Index]; }

template<typename Char>
inline constexpr bool StringIteratorT<Char>::operator==(const StringIteratorT<Char>& other) const noexcept
{ return m_Index == other.m_Index; }

template<typename Char>
inline constexpr bool StringIteratorT<Char>::operator!=(const StringIteratorT<Char>& other) const noexcept
{ return m_Index != other.m_Index; }

template<typename Char>
inline constexpr DynStringCodeUnitIteratorT<Char>::DynStringCodeUnitIteratorT(const StringData& string, const iSys index, const iSys start) noexcept
    : m_String(string)
    , m_Start(ClampT<iSys>(start, 0, minT<iSys>(string.Length, index)))
    , m_Index(maxT(index, m_Start))
{ }

template<typename Char>
inline constexpr DynStringCodeUnitIteratorT<Char>& DynStringCodeUnitIteratorT<Char>::operator++() noexcept
{
    if(m_Index < m_String.Length)
    { ++m_Index; }
    return *this;
}

template<typename Char>
inline constexpr DynStringCodeUnitIteratorT<Char>& DynStringCodeUnitIteratorT<Char>::operator--() noexcept
{
    if(m_Index > m_Start)
    { --m_Index; }
    return *this;
}

template<typename Char>
inline constexpr DynStringCodeUnitIteratorT<Char> DynStringCodeUnitIteratorT<Char>::operator++(int) noexcept
{
    const DynStringCodeUnitIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
inline constexpr DynStringCodeUnitIteratorT<Char> DynStringCodeUnitIteratorT<Char>::operator--(int) noexcept
{
    const DynStringCodeUnitIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
inline constexpr Char DynStringCodeUnitIteratorT<Char>::operator*() const noexcept
{
    if(m_Index < m_Start || m_Index >= m_String.Length)
    {
        return Char{0};
    }

    return m_String.String()[m_Index];
}

template<typename Char>
inline constexpr bool DynStringCodeUnitIteratorT<Char>::operator==(const DynStringCodeUnitIteratorT<Char>& other) const noexcept
{ return m_Index == other.m_Index; }

template<typename Char>
inline constexpr bool DynStringCodeUnitIteratorT<Char>::operator!=(const DynStringCodeUnitIteratorT<Char>& other) const noexcept
{ return m_Index != other.m_Index; }

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const iSys index, const iSys start) noexcept
    : m_String(string)
    , m_Start(ClampT<iSys>(start, 0, minT<iSys>(string.Length, index)))
    , m_Index(maxT(index, m_Start))
    , m_CurrentCodePoint(string.Length > 0 ? string.String()[index] : Char{ '\0' })
{ }

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start) noexcept
    : DynStringCodePointIteratorT(string, static_cast<iSys>(index), static_cast<iSys>(start))
{ }

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const iSys index, const iSys start, const c32 currentCodePoint) noexcept
    : m_String(string)
    , m_Start(ClampT<iSys>(start, 0, minT<iSys>(string.Length, index)))
    , m_Index(maxT(index, m_Start))
    , m_CurrentCodePoint(currentCodePoint)
{ }

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start, const c32 currentCodePoint) noexcept
    : DynStringCodePointIteratorT(string, static_cast<iSys>(index), static_cast<iSys>(start), currentCodePoint)
{ }

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>& DynStringCodePointIteratorT<Char>::operator++() noexcept
{
    if(m_Index < m_String.Length - 1)
    { ++m_Index; }
    return *this;
}

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char>& DynStringCodePointIteratorT<Char>::operator--() noexcept
{
    if(m_Index > m_Start)
    { --m_Index; }
    return *this;
}

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char> DynStringCodePointIteratorT<Char>::operator++(int) noexcept
{
    const DynStringCodePointIteratorT copy(*this);
    operator++();
    return copy;
}

template<typename Char>
inline constexpr DynStringCodePointIteratorT<Char> DynStringCodePointIteratorT<Char>::operator--(int) noexcept
{
    const DynStringCodePointIteratorT copy(*this);
    operator--();
    return copy;
}

template<typename Char>
inline constexpr c32 DynStringCodePointIteratorT<Char>::operator*() const noexcept
{ return m_CurrentCodePoint; }

template<typename Char>
inline constexpr bool DynStringCodePointIteratorT<Char>::operator==(const DynStringCodePointIteratorT<Char>& other) const noexcept
{ return m_Index == other.m_Index; }

template<typename Char>
inline constexpr bool DynStringCodePointIteratorT<Char>::operator!=(const DynStringCodePointIteratorT<Char>& other) const noexcept
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
inline constexpr bool ConstExprStringT<Char>::Equals(const Char(&str)[Len]) const noexcept
{
    if(m_String == str) { return true; }
    if(m_Length != Len) { return false; }
    return strCompare(m_String, str) == 0;
}

template<typename Char>
inline constexpr bool ConstExprStringT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_String == other.String()) { return true; }
    if(m_Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline constexpr bool ConstExprStringT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length != other.m_Length) { return false; }
    if(m_Hash != other.m_Hash) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline constexpr bool ConstExprStringT<Char>::Equals(const DynStringT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char>
inline constexpr bool ConstExprStringT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String(), m_Length) == 0;
}

template<typename Char>
inline constexpr bool ConstExprStringT<Char>::Equals(const Char* const str) const noexcept
{
    if(m_String == str) { return true; }
    return strCompare(m_String, str) == 0;
}

template<typename Char>
inline constexpr i32 ConstExprStringT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline constexpr i32 ConstExprStringT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline constexpr i32 ConstExprStringT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char>
inline constexpr i32 ConstExprStringT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{ return strCompare(m_String, other.String(), minT(m_Length, other.Length())); }

template<typename Char>
inline constexpr i32 ConstExprStringT<Char>::CompareTo(const Char* const str) const noexcept
{ return strCompare(m_String, str); }

template<typename Char>
inline constexpr Char ConstExprStringT<Char>::operator[](const uSys index) const noexcept
{ return m_String[index]; }

template<typename Char>
inline constexpr Char ConstExprStringT<Char>::at(const uSys index) const noexcept
{
    if(index >= m_Length) { return Char{ '\0' }; }
    return m_String[index];
}

template<typename Char, uSys Len>
inline consteval ConstExprStackStringT<Char, Len>::ConstExprStackStringT(const Char(&str)[Len]) noexcept
    : m_String(str)
    , m_Length(Len - 1)
    , m_Hash(cexpr::findHashCode(str))
{ }

template<typename Char, uSys Len>
inline consteval ConstExprStackStringT<Char, Len>::ConstExprStackStringT(const Char* const str, const uSys len) noexcept
    : m_String{ }
    , m_Length(len)
    , m_Hash(findHashCode(str))
{ ::std::ranges::copy_n(str, len, m_String); }

template<typename Char, uSys Len>
template<uSys OLen>
inline consteval bool ConstExprStackStringT<Char, Len>::Equals(const Char(&str)[OLen]) const noexcept
{
    if(m_String == str) { return true; }
    if(m_Length != OLen) { return false; }
    return strCompare(m_String, str) == 0;
}

template<typename Char, uSys Len>
template<uSys OLen>
inline consteval bool ConstExprStackStringT<Char, Len>::Equals(const ConstExprStackStringT<Char, OLen>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length != OLen) { return false; }
    if(m_Hash != other.m_Hash) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char, uSys Len>
inline constexpr bool ConstExprStackStringT<Char, Len>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_String == other.String()) { return true; }
    if(m_Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char, uSys Len>
inline constexpr bool ConstExprStackStringT<Char, Len>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length != other.m_Length) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char, uSys Len>
inline constexpr bool ConstExprStackStringT<Char, Len>::Equals(const DynStringT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String()) == 0;
}

template<typename Char, uSys Len>
inline constexpr bool ConstExprStackStringT<Char, Len>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(m_Length != other.Length() || m_Hash != other.HashCode())
    { return false; }
    return strCompare(m_String, other.String(), Len) == 0;
}

template<typename Char, uSys Len>
inline constexpr bool ConstExprStackStringT<Char, Len>::Equals(const Char* const str) const noexcept
{
    if(m_String == str) { return true; }
    return strCompare(m_String, str) == 0;
}

template<typename Char, uSys Len>
inline constexpr i32 ConstExprStackStringT<Char, Len>::CompareTo(const StringBaseT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char, uSys Len>
inline constexpr i32 ConstExprStackStringT<Char, Len>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char, uSys Len>
inline constexpr i32 ConstExprStackStringT<Char, Len>::CompareTo(const DynStringT<Char>& other) const noexcept
{ return strCompare(m_String, other.String()); }

template<typename Char, uSys Len>
inline constexpr i32 ConstExprStackStringT<Char, Len>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{ return strCompare(m_String, other.String(), minT(Len, other.Length())); }

template<typename Char, uSys Len>
inline constexpr i32 ConstExprStackStringT<Char, Len>::CompareTo(const Char* const str) const noexcept
{ return strCompare(m_String, str); }

template<typename Char, uSys Len>
inline constexpr Char ConstExprStackStringT<Char, Len>::operator[](const uSys index) const noexcept
{ return m_String[index]; }

template<typename Char, uSys Len>
inline constexpr Char ConstExprStackStringT<Char, Len>::at(const uSys index) const noexcept
{
    if(index >= m_Length) { return Char { '\0' }; }
    return m_String[index];
}

template<typename Char>
template<uSys Len>
inline constexpr DynStringT<Char> DynStringT<Char>::FromStatic(const Char(& str)[Len]) noexcept
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
inline DynStringT<Char> DynStringT<Char>::PassControl(
        ReferenceCounter::Type* const refCount,
        const Char* const str,
        const uSys length,
        const ::std::function<void(Char*)>& deleteStr,
        const ::std::function<void(ReferenceCounter::Type*)>& deleteRefCount
    ) noexcept
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
inline constexpr DynStringT<Char>::DynStringT(const Char* const string, const uSys length) noexcept
    : m_Data(string, length)
    , m_Hash(findHashCode(string))
{ }

template<typename Char>
constexpr DynStringT<Char>::DynStringT(ReferenceCounter::Type* const refCount, const Char* const string, const uSys length) noexcept
    : m_Data(refCount, string, length)
    , m_Hash(findHashCode(string))
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT() noexcept
    : m_Data()
    , m_Hash(0)
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT(const Char* const string) noexcept
    : m_Data(string, strLength(string))
    , m_Hash(findHashCode(string))
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT(const uSys length, const Char* string) noexcept
    : m_Data(string, length)
    , m_Hash(findHashCode(string, length))
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT(const ConstExprStringT<Char>& string) noexcept
    : m_Data(string.String(), string.Length())
    , m_Hash(string.HashCode())
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT(const DynStringViewT<Char>& string) noexcept
    : m_Data(string.String(), string.Length())
    , m_Hash(string.HashCode())
{ }

template<typename Char>
inline constexpr DynStringT<Char>::DynStringT(const StringBaseT<Char>&string) noexcept
    : m_Data(string.String(), string.Length())
    , m_Hash(string.HashCode())
{ }

template<typename Char>
inline constexpr DynStringT<Char>& DynStringT<Char>::operator=(const Char* const string) noexcept
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
inline constexpr DynStringT<Char>& DynStringT<Char>::operator=(const ConstExprStringT<Char>& string) noexcept
{
    m_Data.Reset(string, string.Length());
    m_Hash = string.HashCode();

    return *this;
}

template<typename Char>
inline constexpr DynStringT<Char>& DynStringT<Char>::operator=(const DynStringViewT<Char>& string) noexcept
{
    m_Data.Reset(string, string.Length());
    m_Hash = string.HashCode();

    return *this;
}

template<typename Char>
inline constexpr DynStringT<Char>& DynStringT<Char>::operator=(const StringBaseT<Char>& string) noexcept
{
    m_Data.Reset(string, string.Length());
    m_Hash = string.HashCode();

    return *this;
}

template<typename Char>
inline constexpr bool DynStringT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }
    if(m_Data.Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_Data.String(), other.String(), m_Data.Length) == 0;
}

template<typename Char>
inline constexpr bool DynStringT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_Data.Length != other.Length()) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(String(), other.String()) == 0;
}

template<typename Char>
inline constexpr bool DynStringT<Char>::Equals(const DynStringT<Char>& other) const noexcept
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
inline constexpr bool DynStringT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(m_Data.Length != other.Length() ) { return false; }
    if(m_Hash != other.HashCode()) { return false; }
    return strCompare(m_Data.String(), other.String(), m_Data.Length) == 0;
}

template<typename Char>
inline constexpr bool DynStringT<Char>::Equals(const Char* const str) const noexcept
{ return strCompare(String(), str) == 0; }

template<typename Char>
inline constexpr i32 DynStringT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline constexpr i32 DynStringT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline constexpr i32 DynStringT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{ return strCompare(String(), other.String()); }

template<typename Char>
inline constexpr i32 DynStringT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{ return strCompare(String(), other.String(), other.Length()); }

template<typename Char>
inline constexpr i32 DynStringT<Char>::CompareTo(const Char* const str) const noexcept
{ return strCompare(String(), str); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::Concat(const StringBaseT<Char>& other) const noexcept
{ return InternalConcat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::Concat(const ConstExprStringT<Char>& other) const noexcept
{ return InternalConcat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::Concat(const DynStringT<Char>& other) const noexcept
{ return InternalConcat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::Concat(const DynStringViewT<Char>& other) const noexcept
{ return InternalConcat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::Concat(const Char* const other) const noexcept
{ return InternalConcat(strLength(other), other); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::InternalConcat(const uSys len, const Char* const str) const noexcept
{
    const uSys newLen = m_Data.Length + len;
    if(newLen >= 16)
    {
        Char* const newStr = ::std::bit_cast<Char*>(::TauUtilsAllocate(sizeof(Char) * (newLen + 1)));
        newStr[newLen] = '\0';
        (void) ::std::ranges::copy_n(String(), m_Data.Length, newStr);
        // (void) ::std::memcpy(newStr, String(), m_Data.Length * sizeof(Char));
        (void) ::std::ranges::copy_n(str, len, newStr + m_Data.Length);
        // (void) ::std::memcpy(newStr + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(newStr, newLen);
    }
    else
    {
        Char buf[16];
        buf[newLen] = Char{ '\0' };
        
        (void) ::std::ranges::copy_n(m_Data.Buffer.StackString, sizeof(buf), buf); // Perform a fast intrinsic copy.
        // (void) ::std::memcpy(buf, m_Data.Buffer.StackString, sizeof(buf)); // Perform a fast intrinsic copy.
        (void) ::std::ranges::copy_n(str, len, buf + m_Data.Length);
        // (void) ::std::memcpy(buf + m_Data.Length, str, len * sizeof(Char));
        return DynStringT<Char>(static_cast<const Char*>(buf), newLen);
    }
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::SubStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > m_Data.Length)
    {
        return DynStringT<Char>();
    }

    if(length >= 16)
    {
        Char* const sub = ::std::bit_cast<Char*>(::TauUtilsAllocate(sizeof(Char) * (length + 1)));
        sub[length] = Char { '\0' };
        (void) ::std::ranges::copy_n(String() + begin, length, sub);
        // (void) ::std::memcpy(sub, String() + begin, length * sizeof(Char));
        return DynStringT<Char>(sub, length);
    }

    return DynStringT<Char>(length, String() + begin);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::SubString(const uSys begin, const uSys end) const noexcept
{
    return SubStringLen(begin, end - begin);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringT<Char>::SubString(const uSys from) const noexcept
{
    return SubStringLen(from, Length() - from);
}

template<typename Char>
inline constexpr Char DynStringT<Char>::operator[](const uSys index) const noexcept
{ return String()[index]; }

template<typename Char>
inline constexpr Char DynStringT<Char>::At(const uSys index) const noexcept
{
    if(index >= m_Data.Length) { return '\0'; }
    return String()[index];
}

template<typename Char>
inline constexpr DynStringViewT<Char>::DynStringViewT(const uSys begin, const uSys length, const DynStringT<Char>& str) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(length)
    , m_Hash(findHashCode(m_Data.String() + begin, length))
{ }

template<typename Char>
inline constexpr DynStringViewT<Char>::DynStringViewT(const uSys begin, const uSys length, const DynStringViewT<Char>& str) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(length)
    , m_Hash(findHashCode(m_Data.String() + begin, length))
{ }

template<typename Char>
inline constexpr DynStringViewT<Char>::DynStringViewT(const DynStringT<Char>& str, const uSys begin, const uSys end) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(end - begin)
    , m_Hash(findHashCode(m_Data.String() + begin, m_Length))
{ }

template<typename Char>
inline constexpr DynStringViewT<Char>::DynStringViewT(const DynStringViewT<Char>& str, const uSys begin, const uSys end) noexcept
    : m_Data(str.m_Data)
    , m_Start(begin)
    , m_Length(end - begin)
    , m_Hash(findHashCode(m_Data.String() + begin, m_Length))
{ }

template<typename Char>
inline constexpr DynStringViewT<Char>::DynStringViewT(const DynStringT<Char>& str) noexcept
    : m_Data(str.m_Data)
    , m_Start(0)
    , m_Length(str.Length())
    , m_Hash(str.HashCode())
{ }

template<typename Char>
inline constexpr DynStringViewT<Char> DynStringViewT<Char>::Create(const DynStringT<Char>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename Char>
inline constexpr DynStringViewT<Char> DynStringViewT<Char>::Create(const DynStringViewT<Char>& str, const uSys begin, const uSys length) noexcept
{ return DynStringViewT(begin, length, str); }

template<typename Char>
inline constexpr DynStringViewT<Char>& DynStringViewT<Char>::Reset(const DynStringT<Char>& str, const uSys begin, const uSys end) noexcept
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
inline constexpr DynStringViewT<Char>& DynStringViewT<Char>::ResetLen(const DynStringT<Char>& str, const uSys begin, const uSys length) noexcept
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
inline constexpr bool DynStringViewT<Char>::Equals(const StringBaseT<Char>& other) const noexcept
{
    if(this == &other) { return true; }

    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline constexpr bool DynStringViewT<Char>::Equals(const ConstExprStringT<Char>& other) const noexcept
{
    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline constexpr bool DynStringViewT<Char>::Equals(const DynStringT<Char>& other) const noexcept
{
    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline constexpr bool DynStringViewT<Char>::Equals(const DynStringViewT<Char>& other) const noexcept
{
    if(this == &other) { return true; }

    if(m_Length == other.Length() && m_Hash == other.HashCode())
    {
        return strCompare(m_Data.String() + m_Start, other.String(), m_Length) == 0;
    }
    return false;
}

template<typename Char>
inline constexpr bool DynStringViewT<Char>::Equals(const Char* str) const noexcept
{
    return strCompare(m_Data.String() + m_Start, str, m_Length) == 0;
}

template<typename Char>
inline constexpr i32 DynStringViewT<Char>::CompareTo(const StringBaseT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline constexpr i32 DynStringViewT<Char>::CompareTo(const ConstExprStringT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline constexpr i32 DynStringViewT<Char>::CompareTo(const DynStringViewT<Char>& other) const noexcept
{
    if(this == &other) { return 0; }
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline constexpr i32 DynStringViewT<Char>::CompareTo(const DynStringT<Char>& other) const noexcept
{
    return strCompare(m_Data.String() + m_Start, other.String(), minT(m_Length, other.Length()));
}

template<typename Char>
inline constexpr i32 DynStringViewT<Char>::CompareTo(const Char* str) const noexcept
{
    return strCompare(m_Data.String() + m_Start, str, m_Length);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::Concat(const StringBaseT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::Concat(const ConstExprStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::Concat(const DynStringT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::Concat(const DynStringViewT<Char>& other) const noexcept
{ return _concat(other.Length(), other.String()); }

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::Concat(const Char* const other) const noexcept
{
    const uSys otherLen = strLength(other);
    return _concat(otherLen, other);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::_concat(const uSys len, const Char* const str) noexcept
{
    const uSys newLen = m_Length + len;
    if(newLen >= 16)
    {
        Char* const newStr = ::TauUtilsAllocateTArr<Char>(newLen + 1);
        newStr[newLen] = Char{ '\0' };
        (void) ::std::ranges::copy_n(String(), m_Length, newStr);
        // (void) ::std::memcpy(newStr, String(), m_Length * sizeof(Char));
        (void) ::std::ranges::copy_n(str, len, newStr + m_Length);
        // (void) ::std::memcpy(newStr + m_Length, str, len * sizeof(Char));
        return DynStringT<Char>(newStr, newLen);
    }
    else
    {
        DynStringT<Char> tmp(newLen);
        tmp.String()[newLen] = Char{ '\0' };
        (void) ::std::ranges::copy_n(String(), m_Length, tmp.String());
        // (void) ::std::memcpy(tmp.String(), String(), m_Length * sizeof(Char));
        (void) ::std::ranges::copy_n(str, len, tmp.String() + Length());
        // (void) ::std::memcpy(tmp.String() + Length(), str, len * sizeof(Char));
        tmp.m_Hash = findHashCode(tmp.String());
        return tmp;
    }
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::SubStringLen(const uSys begin, const uSys length) const noexcept
{
    if(begin + length > m_Length)
    { return DynStringT<Char>(); }

    if(length >= 16)
    {
        Char* const sub = ::TauUtilsAllocateTArr<Char>(length + 1);
        sub[length] = Char { '\0' };
        (void) ::std::ranges::copy_n(String() + begin, length, sub);
        // (void) ::std::memcpy(sub, String() + begin, length * sizeof(Char));
        return DynStringT<Char>(sub, length);
    }

    return DynStringT<Char>(length, String() + begin);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::SubString(const uSys begin, const uSys end) const noexcept
{
    return SubStringLen(begin, end - begin);
}

template<typename Char>
inline constexpr DynStringT<Char> DynStringViewT<Char>::SubString(const uSys from) const noexcept
{
    return SubStringLen(from, m_Length - from);
}

template<typename Char>
inline constexpr Char DynStringViewT<Char>::operator[](const uSys index) const noexcept
{ return String()[index]; }

template<typename Char>
inline constexpr Char DynStringViewT<Char>::At(const uSys index) const noexcept
{
    if(index >= m_Length) { return Char{ '\0' }; }
    return String()[index];
}

static inline constexpr uSys STRING_BUILDER_DEFAULT_SIZE = 64;

template<typename Char>
inline constexpr StringBuilderT<Char>::StringBuilderT() noexcept
    : m_String(::TauUtilsAllocateTArr<Char>(STRING_BUILDER_DEFAULT_SIZE))
    , m_Length(0)
    , m_Size(STRING_BUILDER_DEFAULT_SIZE)
{
    m_String[0] = Char { '\0' };
    // TODO: Fix the buffer overrun so this isn't necessary.
    ::std::fill_n(m_String, m_Size, Char { '\0' });
}

template<typename Char>
inline constexpr StringBuilderT<Char>::StringBuilderT(const uSys initialSize) noexcept
    : m_String(::TauUtilsAllocateTArr<Char>(maxT(initialSize, static_cast<uSys>(1))))
    , m_Length(0)
    , m_Size(maxT(initialSize, static_cast<uSys>(2)))
{
    m_String[0] = Char { '\0' };
    // TODO: Fix the buffer overrun so this isn't necessary.
    ::std::fill_n(m_String, m_Size, Char { '\0' });
}

template<typename Char>
inline constexpr StringBuilderT<Char>::StringBuilderT(const Char* const string) noexcept
    : m_String(nullptr)
    , m_Length(strLength(string))
    , m_Size(m_Length + STRING_BUILDER_DEFAULT_SIZE)
{
    Char* const str = ::TauUtilsAllocateTArr<Char>(m_Size);

    if(string)
    {
        (void) ::std::ranges::copy_n(string, m_Length + 1, str);
        // (void) ::std::memcpy(str, string, (m_Length + 1) * sizeof(Char));
    }

    m_String = str;
}

template<typename Char>
inline constexpr StringBuilderT<Char>::~StringBuilderT() noexcept
{ ::TauUtilsDeallocateTArr(m_String, m_Size); }

template<typename Char>
inline constexpr StringBuilderT<Char>::StringBuilderT(const StringBuilderT<Char>& copy) noexcept
    : m_String(::std::bit_cast<Char*>(::TauUtilsAllocateTArr<Char>(copy.m_Size)))
    , m_Length(copy.m_Length)
    , m_Size(copy.m_Size)
{
    (void) ::std::ranges::copy_n(copy.m_String, m_Size, m_String);
    // (void) ::std::memcpy(m_String, copy.m_String, m_Size);
}

template<typename Char>
inline constexpr StringBuilderT<Char>::StringBuilderT(StringBuilderT<Char>&& move) noexcept
    : m_String(move.m_String)
    , m_Length(move.m_Length)
    , m_Size(move.m_Size)
{ move.m_String = nullptr; }

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::operator=(const StringBuilderT<Char>& copy) noexcept
{
    if(this == &copy)
    { return *this; }

    ::TauUtilsDeallocateTArr(m_String, m_Size);

    m_String = ::TauUtilsAllocateTArr<Char>(copy.m_Size);
    m_Length = copy.m_Length;
    m_Size = copy.m_Size;

    (void) ::std::ranges::copy_n(copy.m_String, m_Size, m_String);
    // (void) ::std::memcpy(m_String, copy.m_String, m_Size * sizeof(Char));

    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::operator=(StringBuilderT<Char>&& move) noexcept
{
    if(this == &move)
    { return *this; } 

    ::TauUtilsDeallocateTArr(m_String, m_Size);

    m_String = move.m_String;
    m_Length = move.m_Length;
    m_Size = move.m_Size;

    move.m_String = nullptr;

    return *this;
}

template<typename Char>
inline constexpr bool StringBuilderT<Char>::equals(const StringBuilderT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return true; }
    if(m_Length == other.m_Length)
    {
        return strCompare(m_String, other.m_String) == 0;
    }
    return false;
}

template<typename Char>
inline constexpr i32 StringBuilderT<Char>::compareTo(const StringBuilderT<Char>& other) const noexcept
{
    if(m_String == other.m_String) { return 0; }
    return strCompare(m_String, other.m_String);
}

template<typename Char>
inline constexpr void StringBuilderT<Char>::Append(const Char* const string, const uSys length) noexcept
{
    const uSys newLen = m_Length + length;
    AssertSize(newLen);
    (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
    // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

template<typename Char>
template<typename CharFrom>
inline constexpr void StringBuilderT<Char>::Append(const CharFrom* const string, const uSys length) noexcept
{
    const uSys newLen = m_Length + length;
    AssertSize(newLen);
    (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
    // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
    m_Length = newLen;
    m_String[m_Length] = Char { '\0' };
}

template<>
template<>
inline constexpr void StringBuilderT<char>::Append<c8>(const c8* string, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
    else
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<char>::Append<c16>(const c16* string, const uSys length) noexcept
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
inline constexpr void StringBuilderT<char>::Append<c32>(const c32* string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = '\0';
}

#if !defined(_MSVC_LANG)
inline int mbsrtowcs_s(
   uSys* const pReturnValue,
   wchar_t* const wcstr,
   const uSys sizeInWords,
   const char** const mbstr,
   const uSys count,
   mbstate_t* const mbstate
)
{
    (void) sizeInWords;

    const uSys retValue = ::std::mbsrtowcs(wcstr, mbstr, count, mbstate);

    if(pReturnValue)
    {
        *pReturnValue = retValue;
    }

    if(retValue == static_cast<uSys>(-1))
    {
        return errno;
    }

    if(retValue > sizeInWords)
    {
        return ERANGE;
    }

    return 0;
}

inline int wcsrtombs_s(
   uSys* const pReturnValue,
   char* const mbstr,
   const uSys sizeInBytes,
   const wchar_t** const wcstr,
   const uSys count,
   mbstate_t* const mbstate
)
{
    (void) sizeInBytes;

    const uSys retValue = ::std::wcsrtombs(mbstr, wcstr, count, mbstate);

    if(pReturnValue)
    {
        *pReturnValue = retValue;
    }

    if(retValue == static_cast<uSys>(-1))
    {
        return errno;
    }

    if(retValue > sizeInBytes)
    {
        return ERANGE;
    }

    return 0;
}
#endif

template<>
template<>
inline constexpr void StringBuilderT<wchar_t>::Append<char>(const char* string, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        if constexpr(sizeof(wchar_t) == sizeof(u16))
        {
            const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
            const uSys newLen = m_Length + codeUnitLength;
            AssertSize(newLen);
            (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
            m_Length = newLen;
            m_String[m_Length] = L'\0';
        }
        else if constexpr(sizeof(wchar_t) == sizeof(c32))
        {
            const uSys codeUnitLength = ::tau::string::utf8::CalculateCodePoints(string, static_cast<iSys>(length));
            const uSys newLen = m_Length + codeUnitLength;
            AssertSize(newLen);
            (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
            m_Length = newLen;
            m_String[m_Length] = L'\0';
        }
    }
    else
    {
        const char* stringReal = string;
        DynString stringHolder;

        if(strLength(string) != length)
        {
            stringHolder = DynString(length, string);
            stringReal = stringHolder.String();
        }

        ::std::mbstate_t state;
        uSys len;
        int error = mbsrtowcs_s(&len, nullptr, 0, &stringReal, 0, &state);

        if(error != 0 && error != ERANGE)
        {
            return;
        }

        const uSys newLen = m_Length + len;
        AssertSize(newLen);

        error = mbsrtowcs_s(&len, m_String + m_Length, len + 1, &stringReal, len, &state);

        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<wchar_t>::Append<c8>(const c8* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(u16))
    {
        const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
        m_Length = newLen;
        m_String[m_Length] = L'\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        const uSys codeUnitLength = ::tau::string::utf8::CalculateCodePoints(string, static_cast<iSys>(length));
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
        m_Length = newLen;
        m_String[m_Length] = L'\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<wchar_t>::Append<c16>(const c16* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = L'\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodePoints(string, static_cast<iSys>(length));
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
        m_Length = newLen;
        m_String[m_Length] = L'\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<wchar_t>::Append<c32>(const c32* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
        m_Length = newLen;
        m_String[m_Length] = L'\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
#ifndef _WIN32
#pragma warning(push, 0)
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = L'\0';
#pragma warning(pop)
#endif
    }
}

template<>
template<>
inline constexpr void StringBuilderT<c8>::Append<c16>(const c16* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), true);
    m_Length = newLen;
    m_String[m_Length] = u8'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c8>::Append<c32>(const c32* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = u8'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c8>::Append<char>(const char* string, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        m_Length = newLen;
        m_String[m_Length] = u8'\0';
    }
    else
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = u8'\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<c8>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(u16))
    {
        const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), true);
        m_Length = newLen;
        m_String[m_Length] = u8'\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
        m_Length = newLen;
        m_String[m_Length] = u8'\0';
    }

    // ::std::mbstate_t state;
    // uSys len;
    // int error = wcsrtombs_s(&len, nullptr, 0, &string, 0, &state);
    //
    // if(error != 0)
    // {
    //     return;
    // }
    //
    // const uSys newLen = m_Length + len;
    // AssertSize(newLen);
    //
    // error = wcsrtombs_s(&len, ::std::bit_cast<char*>(m_String + m_Length), len + 1, &string, len, &state);
    //
    // m_Length = newLen;
    // m_String[m_Length] = '\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c16>::Append<c8>(const c8* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
    m_Length = newLen;
    m_String[m_Length] = u'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c16>::Append<c32>(const c32* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf16::CalculateCodeUnits(string, static_cast<iSys>(length), true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
    m_Length = newLen;
    m_String[m_Length] = u'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c16>::Append<char>(const char* string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
    m_Length = newLen;
    m_String[m_Length] = u'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c16>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = u'\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodeUnits(string, static_cast<iSys>(length), true);
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform<wchar_t, c16>(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), false, true);
        m_Length = newLen;
        m_String[m_Length] = u'\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<c32>::Append<c8>(const c8* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodePoints(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = U'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c32>::Append<c16>(const c16* const string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf16::CalculateCodePoints(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = U'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c32>::Append<char>(const char* string, const uSys length) noexcept
{
    const uSys codeUnitLength = ::tau::string::utf8::CalculateCodePoints(string, static_cast<iSys>(length));
    const uSys newLen = m_Length + codeUnitLength;
    AssertSize(newLen);
    (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
    m_Length = newLen;
    m_String[m_Length] = U'\0';
}

template<>
template<>
inline constexpr void StringBuilderT<c32>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        const uSys codeUnitLength = ::tau::string::utf16::CalculateCodePoints(string, static_cast<iSys>(length));
        const uSys newLen = m_Length + codeUnitLength;
        AssertSize(newLen);
        (void) ::tau::string::utf16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        const uSys newLen = m_Length + length;
        AssertSize(newLen);
        (void) ::std::ranges::copy_n(string, length, m_String + m_Length);
        // (void) ::std::memcpy(m_String + m_Length, string, length * sizeof(Char));
        m_Length = newLen;
        m_String[m_Length] = U'\0';
    }
}

template<>
template<>
inline constexpr void StringBuilderT<char>::Append<wchar_t>(const wchar_t* string, const uSys length) noexcept
{
    if(::std::is_constant_evaluated())
    {
        if constexpr(sizeof(wchar_t) == sizeof(u16))
        {
            const uSys codeUnitLength = ::tau::string::utf8_16::CalculateCodeUnits(string, static_cast<iSys>(length), 0, 0, true);
            const uSys newLen = m_Length + codeUnitLength;
            AssertSize(newLen);
            (void) ::tau::string::utf8_16::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength), true);
            m_Length = newLen;
            m_String[m_Length] = '\0';
        }
        else if constexpr(sizeof(wchar_t) == sizeof(c32))
        {
            const uSys codeUnitLength = ::tau::string::utf8::CalculateCodeUnits(string, static_cast<iSys>(length));
            const uSys newLen = m_Length + codeUnitLength;
            AssertSize(newLen);
            (void) ::tau::string::utf8::Transform(string, m_String + m_Length, static_cast<iSys>(length), static_cast<iSys>(codeUnitLength));
            m_Length = newLen;
            m_String[m_Length] = '\0';
        }
    }
    else
    {
        const wchar_t* stringReal = string;
        WDynString stringHolder;

        if(strLength(string) != length)
        {
            stringHolder = WDynString(length, string);
            stringReal = stringHolder.String();
        }

        ::std::mbstate_t state;
        (void) ::std::memset(&state, 0, sizeof(state));
        uSys len;
        int error = wcsrtombs_s(&len, nullptr, 0, &stringReal, 0, &state);

        if(error != 0 && error != ERANGE)
        {
            return;
        }

        const uSys newLen = m_Length + len;
        AssertSize(newLen);

        (void) ::std::memset(&state, 0, sizeof(state));
        error = wcsrtombs_s(&len, m_String + m_Length, len + 1, &stringReal, len, &state);

        m_Length = newLen;
        m_String[m_Length] = '\0';
    }
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const StringBaseT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const ConstExprStringT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const DynStringT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const DynStringViewT<Char>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const StringBuilderT<Char>& string) noexcept
{
    Append(string.m_String, string.m_Length);
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const Char* const string) noexcept
{
    Append(string, strLength(string));
    return *this;
}

template<typename Char>
template<typename CharDeduced>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const ::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) noexcept
{
    const Char string[2] = { c, Char{ '\0' } };
    Append(string, 1);
    return *this;
}

template<>
inline constexpr StringBuilderT<c8>& StringBuilderT<c8>::Append(const c32 c) noexcept
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
inline constexpr StringBuilderT<c16>& StringBuilderT<c16>::Append(const c32 c) noexcept
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
inline constexpr StringBuilderT<c32>& StringBuilderT<c32>::Append(const c32 c) noexcept
{
    const c32 string[2] = { c, U'\0' };
    Append(string, 1);
    return *this;
}

template<>
inline constexpr StringBuilderT<char>& StringBuilderT<char>::Append(const c32 c) noexcept
{
    const c32 string[2] = { c, U'\0' };
    Append(string, 1);
    return *this;
}

template<>
inline constexpr StringBuilderT<wchar_t>& StringBuilderT<wchar_t>::Append(const c32 c) noexcept
{
    const c32 string[2] = { c, U'\0' };
    Append(string, 1);
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const StringBaseT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const ConstExprStringT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const DynStringT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const DynStringViewT<CharFrom>& string) noexcept
{
    Append(string.String(), string.Length());
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const StringBuilderT<CharFrom>& string) noexcept
{
    Append(string.m_String, string.m_Length);
    return *this;
}

template<typename Char>
template<typename CharFrom>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Append(const CharFrom* const string) noexcept
{
    Append(string, strLength(string));
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Reset() noexcept
{
    m_Length = 0;
    m_String[0] = Char { '\0' };
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Reset(const uSys newSize) noexcept
{
    if(newSize == 0) 
    { return *this; }

    ::TauUtilsDeallocateTArr(m_String, m_Size);

    m_String = ::TauUtilsAllocateTArr<Char>(newSize);
    m_Size = newSize;
    m_Length = 0;
    m_String[0] = Char{ '\0' };

    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::ResetIf(const uSys maxSize, const uSys newSize) noexcept
{
    if(newSize == 0) { return *this; }
    if(maxSize <= newSize) { return *this; }

    if(m_Size >= maxSize)
    {
        ::TauUtilsDeallocateTArr(m_String, m_Size);
        m_String = ::TauUtilsAllocateTArr<Char>(newSize);
        m_Size = newSize;
    }

    m_Length = 0;
    m_String[0] = Char{ '\0' };

    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Backspace() noexcept
{
    if(m_Length > 0)
    {
        --m_Length;
        m_String[m_Length] = '\0';
    }
    return *this;
}

template<typename Char>
inline constexpr StringBuilderT<Char>& StringBuilderT<Char>::Backspace(const uSys count) noexcept
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
inline constexpr Char StringBuilderT<Char>::operator[](const uSys index) const noexcept
{ return m_String[index]; }

template<typename Char>
inline constexpr Char StringBuilderT<Char>::At(const uSys index) const noexcept
{
    if(index >= m_Length) { return '\0'; }
    return m_String[index];
}

template<typename Char>
inline constexpr void StringBuilderT<Char>::AssertSize(const uSys newLength) noexcept
{
    if(newLength >= m_Size)
    {
        const uSys newSize = newLength + (newLength >> 1);
        Char* const newStr = ::TauUtilsAllocateTArr<Char>(newSize);
        (void) ::std::ranges::copy_n(m_String, m_Length + 1, newStr);
        // (void) ::std::memcpy(newStr, m_String, (m_Length + 1) * sizeof(Char));
        ::TauUtilsDeallocateTArr(m_String, m_Size);
        m_String = newStr;
        m_Size = newSize;
    }
}

template<typename Char, iSys Len>
inline consteval ConstEvalStringBuilderT<Char, Len>::ConstEvalStringBuilderT() noexcept
    : m_String{ }
{ }

template<typename Char, iSys Len>
inline consteval ConstEvalStringBuilderT<Char, Len>::ConstEvalStringBuilderT(const Char(&string)[Len]) noexcept
    : m_String{ }
{
    (void) ::std::ranges::copy_n(string, Len, m_String);
}

template<typename Char, iSys Len>
inline consteval ConstEvalStringBuilderT<Char, Len>::ConstEvalStringBuilderT(const ConstEvalStringBuilderT<Char, Len>& copy) noexcept
    : m_String{ }
{
    (void) ::std::ranges::copy_n(copy.m_String, Len, m_String);
}

template<typename Char, iSys Len>
inline consteval ConstEvalStringBuilderT<Char, Len>::ConstEvalStringBuilderT(ConstEvalStringBuilderT<Char, Len>&& move) noexcept
    : m_String{ }
{
    (void) ::std::ranges::copy_n(move.m_String, Len, m_String);
}

template<typename Char, iSys Len>
template<iSys OLen>
inline consteval ConstEvalStringBuilderT<Char, Len + OLen - 1> ConstEvalStringBuilderT<Char, Len>::Append(const ConstExprStackStringT<Char, OLen>& string) const noexcept
{
    Char newString[Len + OLen - 1];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::std::ranges::copy_n(string.String(), OLen, &newString[Len]);
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
template<iSys OLen>
inline consteval ConstEvalStringBuilderT<Char, Len + OLen - 1> ConstEvalStringBuilderT<Char, Len>::Append(const Char(&str)[OLen]) const noexcept
{
    Char newString[Len + OLen - 1];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::std::ranges::copy_n(str, OLen, &newString[Len]);
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(const Char* const str, const iSys oLen) const noexcept
{
    Char newString[Len + oLen - 1];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::std::ranges::copy_n(str, oLen, &newString[Len]);
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
template<typename CharDeduced>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) const noexcept
{
    const Char string[2] = { c, Char{ '\0' } };
    return Append(string);
}

template<typename Char, iSys Len>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(c32 c) const noexcept
{
    const c32 string[2] = { c, U'\0' };
    return Append(string);
    // Char newString[Len + ::tau::string::CodeUnitCountConst<Char, c32, 1>::Count(&c)];
    // (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    // newString[Len] = c;
    // newString[Len + 1] = Char { '\0' };
    // return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
template<typename CharFrom, iSys OLen>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(const ConstExprStackStringT<CharFrom, OLen>& string) const noexcept
{
    constexpr iSys FromLen = ::tau::string::CodeUnitCountConst<Char, CharFrom, 1>::Count(string.m_String);
    constexpr iSys NewLen = Len + FromLen;
    Char newString[NewLen];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::tau::string::Transform(string.m_String, &newString[Len], OLen, FromLen, false, true);
    newString[NewLen - 1] = Char { '\0' };
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
template<typename CharFrom, iSys OLen>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(const CharFrom(&str)[OLen]) const noexcept
{
    constexpr iSys FromLen = ::tau::string::CodeUnitCountConst<Char, CharFrom, 1>::Count(str);
    constexpr iSys NewLen = Len + FromLen;
    Char newString[NewLen];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::tau::string::Transform(str, &newString[Len], OLen, FromLen, false, true);
    newString[NewLen - 1] = Char { '\0' };
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
template<typename CharFrom>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Append(const CharFrom* const str, const iSys oLen) const noexcept
{
    constexpr iSys FromLen = ::tau::string::CodeUnitCountConst<Char, CharFrom, 1>::Count(str);
    constexpr iSys NewLen = Len + FromLen;
    Char newString[NewLen];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    (void) ::tau::string::Transform(str, &newString[Len], oLen, FromLen, false, true);
    newString[NewLen - 1] = Char { '\0' };
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
inline consteval ConstEvalStringBuilderT<Char, maxT(Len - 1, 0)> ConstEvalStringBuilderT<Char, Len>::Backspace() const noexcept
{
    Char newString[Len - 1];
    (void) ::std::ranges::copy_n(m_String, Len - 1, newString);
    newString[Len - 2] = Char { '\0' };
    return ConstEvalStringBuilderT(newString);
}

template<typename Char, iSys Len>
inline consteval auto ConstEvalStringBuilderT<Char, Len>::Backspace(const uSys count) const noexcept
{
    Char newString[Len - count];
    (void) ::std::ranges::copy_n(m_String, Len - count, newString);
    newString[Len - count - 1] = Char { '\0' };
    return ConstEvalStringBuilderT(newString);
}

// template<typename CTo, typename CFrom>
// DynStringT<CTo> StringCast(const DynStringT<CFrom>&) noexcept = delete;

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
    int error = mbsrtowcs_s(&len, nullptr, 0, &rawStr, 0, &state);

    if(error != 0 && error != ERANGE)
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
    int error = wcsrtombs_s(&len, nullptr, 0, &rawStr, 0, &state);
    
    if(error != 0 && error != ERANGE)
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



template<typename CTo, typename CFrom>
DynStringT<CTo> StringCastFlipEndian(const DynStringT<CFrom>&) noexcept = delete;

template<>
inline DynStringT<char> StringCastFlipEndian(const DynStringT<char>& string) noexcept
{ return string; }

template<>
inline DynStringT<c8> StringCastFlipEndian(const DynStringT<c8>& string) noexcept
{ return string; }



template<typename CTo, typename CFrom>
DynStringT<CTo> StringCastSkipBom(const DynStringT<CFrom>&) noexcept = delete;

template<>
inline DynStringT<c32> StringCastSkipBom(const DynStringT<c32>& string) noexcept
{ return string; }

template<>
inline DynStringT<char> StringCastSkipBom(const DynStringT<char>& string) noexcept
{ return string; }



template<typename CTo, typename CFrom>
DynStringT<CTo> StringCastFlipEndianSkipBom(const DynStringT<CFrom>&) noexcept = delete;

template<>
inline DynStringT<char> StringCastFlipEndianSkipBom(const DynStringT<char>&string) noexcept
{ return string; }

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
