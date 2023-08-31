#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string {


inline constexpr iSys Transform(const c8* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const c32* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const char* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const c32* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const wchar_t* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const char* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const wchar_t* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, bool flipEndian, bool skipBom) noexcept
{
    return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const c16* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const c32* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const wchar_t* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const c32* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c16* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits);
}

inline constexpr iSys Transform(const wchar_t* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const wchar_t* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c8* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c16* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

inline constexpr iSys Transform(const char* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c16* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

inline constexpr iSys Transform(const c8* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        return utf8::Transform(inString, outString, inCodeUnits, outCodeUnits);
    }
    else
    {
        return -1;
    }
}

inline constexpr iSys Transform(const wchar_t* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}


inline constexpr iSys Transform(const wchar_t* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return utf8_16::Transform(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

}
