#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

template<>
inline DynStringT<c8> StringCast<c8, char>(const DynStringT<char>& string) noexcept
{
    return C8DynString(string.Length(), reinterpret_cast<const c8*>(string.String()));
}

template<>
inline DynStringT<char> StringCast<char, c8>(const DynStringT<c8>& string) noexcept
{
    return DynString(string.Length(), reinterpret_cast<const char*>(string.String()));
}

#ifdef _WIN32
template<>
inline DynStringT<c16> StringCast<c16, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return C16DynString(string.Length(), reinterpret_cast<const c16*>(string.String()));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c16>(const DynStringT<c16>& string) noexcept
{
    return WDynString(string.Length(), reinterpret_cast<const wchar_t*>(string.String()));
}

template<>
inline DynStringT<c32> StringCast<c32, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return StringCast<c32>(C16DynString(string.Length(), reinterpret_cast<const c16*>(string.String())));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c32>(const DynStringT<c32>& string) noexcept
{
    const C16DynString tmp = StringCast<c16>(string);
    return WDynString(tmp.Length(), reinterpret_cast<const wchar_t*>(tmp.String()));
}

template<>
inline DynStringT<c8> StringCast<c8, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return StringCast<c8>(StringCast<c16>(string));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c8>(const DynStringT<c8>& string) noexcept
{
    return StringCast<wchar_t>(StringCast<c16>(string));
}
#else
template<>
inline DynStringT<c32> StringCast<c32, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return C32DynString(string.Length(), reinterpret_cast<const c32*>(string.String()));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c32>(const DynStringT<c32>& string) noexcept
{
    return WDynString(string.Length(), reinterpret_cast<const wchar_t*>(string.String()));
}

template<>
inline DynStringT<c16> StringCast<c16, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return StringCast<c16>(C32DynString(string.Length(), reinterpret_cast<const c32*>(string.String())));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c16>(const DynStringT<c16>& string) noexcept
{
    const C32DynString tmp = StringCast<c32>(string);
    return WDynString(tmp.Length(), reinterpret_cast<const wchar_t*>(tmp.String()));
}

template<>
inline DynStringT<c8> StringCast<c8, wchar_t>(const DynStringT<wchar_t>& string) noexcept
{
    return StringCast<c8>(StringCast<c32>(string));
}

template<>
inline DynStringT<wchar_t> StringCast<wchar_t, c8>(const DynStringT<c8>& string) noexcept
{
    return StringCast<wchar_t>(StringCast<c32>(string));
}
#endif

template<>
inline DynStringT<c16> StringCast<c16, char>(const DynStringT<char>& string) noexcept
{
    return StringCast<c16>(C8DynString(string.Length(), reinterpret_cast<const c8*>(string.String())));
}

template<>
inline DynStringT<char> StringCast<char, c16>(const DynStringT<c16>& string) noexcept
{
    const C8DynString tmp = StringCast<c8>(string);
    return DynString(tmp.Length(), reinterpret_cast<const char*>(tmp.String()));
}

template<>
inline DynStringT<c32> StringCast<c32, char>(const DynStringT<char>& string) noexcept
{
    return StringCast<c32>(C8DynString(string.Length(), reinterpret_cast<const c8*>(string.String())));
}

template<>
inline DynStringT<char> StringCast<char, c32>(const DynStringT<c32>& string) noexcept
{
    const C8DynString tmp = StringCast<c8>(string);
    return DynString(tmp.Length(), reinterpret_cast<const char*>(tmp.String()));
}
