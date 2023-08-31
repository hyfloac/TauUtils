#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

#include "NumTypes.hpp"

namespace tau::string {

namespace utf8 {

template<typename C32Alias = c32>
[[nodiscard]] inline constexpr iSys CalculateCodeUnits(const C32Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    iSys outCodeUnits = priorCodeUnits;
    for(iSys i = startCodeUnit; i < inCodeUnits; ++i)
    {
        if(inString[i] <= 0x7F) // 1 Byte
        {
            ++outCodeUnits;
        }
        else if(inString[i] <= 0x07FF) // 2 Bytes
        {
            outCodeUnits += 2;
        }
        else if(inString[i] <= 0xFFFF) // 3 Bytes
        {
            outCodeUnits += 3;
        }
        else if(inString[i] <= 0x10FFFF) // 4 Bytes
        {
            outCodeUnits += 4;
        }
        else
        {
            return -1;
        }
    }
    return outCodeUnits;
}

template<typename C8Alias = c8>
[[nodiscard]] inline constexpr iSys CalculateCodePoints(const C8Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodePoints = 0) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    iSys startingIndex = startCodeUnit;
    if(startingIndex == 0 && inCodeUnits >= 3)
    {
        if(inString[0] == 0xEF &&
            inString[1] == 0xBB &&
            inString[2] == 0xBF) // Skip BOM
        {
            startingIndex = 3;
        }
    }

    iSys outCodePoints = priorCodePoints;
    for(iSys i = startingIndex; i < inCodeUnits;)
    {
        if((inString[i] & 0x80) == 0) // 1 Byte
        {
            ++i;
        }
        else if((inString[i] & 0xE0) == 0xC0) // 2 Bytes
        {
            i += 2;
        }
        else if((inString[i] & 0xF0) == 0xE0) // 3 Bytes
        {
            i += 3;
        }
        else if((inString[i] & 0xF8) == 0xF0) // 4 Bytes
        {
            i += 4;
        }
        else
        {
            return -1;
        }
        ++outCodePoints;
    }
    return outCodePoints;
}

}

namespace utf16 {

template<typename C32Alias = c32>
[[nodiscard]] inline constexpr iSys CalculateCodeUnits(const C32Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0, const bool skipBom = false) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    iSys outCodeUnits = priorCodeUnits + (skipBom ? 0 : 1);
    for(iSys i = startCodeUnit; i < inCodeUnits; ++i)
    {
        if(inString[i] <= 0xD7FF || (inString[i] >= 0xE000 && inString[i] <= 0xFFFF)) // 1 Short
        {
            ++outCodeUnits;
        }
        else if(inString[i] <= 0x10FFFF) // 2 Shorts
        {
            outCodeUnits += 2;
        }
        else
        {
            return -1;
        }
    }
    return outCodeUnits;
}

template<typename C16Alias = c16>
[[nodiscard]] inline constexpr iSys CalculateCodePoints(const C16Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodePoints = 0) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    bool flipEndian = false;
    iSys startingIndex = startCodeUnit;
    if(startingIndex == 0 && inCodeUnits >= 1)
    {
        if(inString[0] == 0xFEFF) // Skip BOM
        {
            startingIndex = 1;
        }
        else if(inString[0] == 0xFFFE) // Big endian
        {
            flipEndian = true;
            startingIndex = 1;
        }
    }

    iSys outCodePoints = priorCodePoints;
    for(iSys i = startingIndex; i < inCodeUnits;)
    {
        if(!flipEndian)
        {
            if(inString[i] <= 0xD7FF || inString[i] >= 0xE000) // 1 Short
            {
                ++i;
            }
            else // 2 Shorts
            {
                i += 2;
            }
        }
        else
        {
            const uSys lowByte = inString[i] & 0xFF;
            if(lowByte <= 0xD7 || (lowByte >= 0xE0 && lowByte <= 0xFF)) // 1 Short
            {
                ++i;
            }
            else // 2 Shorts
            {
                i += 2;
            }
        }
        ++outCodePoints;
    }
    return outCodePoints;
}

}

namespace utf8_16 {

template<typename C8Alias = c8>
[[nodiscard]] inline constexpr iSys CalculateCodeUnits(const C8Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0, const bool skipBom = false) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits < 0)
    { return -1; }

    iSys startingIndex = startCodeUnit;
    if(startingIndex == 0 && inCodeUnits >= 3)
    {
        if(inString[0] == 0xEF &&
            inString[1] == 0xBB &&
            inString[2] == 0xBF) // Skip BOM
        {
            startingIndex = 3;
        }
    }

    iSys outCodeUnits = priorCodeUnits + (skipBom ? 0 : 1); // Prior code points, plus BOM
    for(iSys i = startingIndex; i < inCodeUnits;)
    {
        if((inString[i] & 0x80) == 0) // 1 Byte
        {
            ++i;
            ++outCodeUnits;
        }
        else if((inString[i] & 0xE0) == 0xC0) // 2 Bytes
        {
            i += 2;
            ++outCodeUnits;
        }
        else if((inString[i] & 0xF0) == 0xE0) // 3 Bytes
        {
            i += 3;
            if(i + 3 >= inCodeUnits)
            {
                const c32 val = static_cast<c32>((inString[i] << 16) | (inString[i + 1] << 8) | inString[i + 2]);

                // The min and max bits of a surrogate pair interlaced with UTF8 encoding bits.
                if(val >= 0xED9F8F && val <= 0xEE8080)
                {
                    outCodeUnits += 2;
                }
                else
                {
                    ++outCodeUnits;
                }
            }
        }
        else if((inString[i] & 0xF8) == 0xF0) // 4 Bytes
        {
            i += 4;
            outCodeUnits += 2;
        }
        else
        { return -1; }
    }
    return outCodeUnits;
}

template<typename C16Alias = c16>
[[nodiscard]] inline constexpr iSys CalculateCodeUnitsC16(const C16Alias* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0, const bool skipBom = false) noexcept
{
    if(inCodeUnits == 0)
    { return 0; }

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    bool flipEndian = false;
    iSys startingIndex = startCodeUnit;
    if(startingIndex == 0 && inCodeUnits >= 1)
    {
        if(inString[0] == 0xFEFF) // Skip BOM
        {
            startingIndex = 1;
        }
        else if(inString[0] == 0xFFFE) // Big endian
        {
            flipEndian = true;
            startingIndex = 1;
        }
    }

    iSys outCodeUnits = priorCodeUnits + (skipBom ? 0 : 3); // Prior code points
    for(iSys i = startingIndex; i < inCodeUnits;)
    {
        if(!flipEndian)
        {
            if(inString[i] <= 0xD7FF || inString[i] >= 0xE000) // 1 Short
            {
                ++i;

                if(inString[i] <= 0x7F)
                {
                    ++outCodeUnits;
                }
                else if(inString[1] <= 0x7FF)
                {
                    outCodeUnits += 2;
                }
                else
                {
                    outCodeUnits += 3;
                }
            }
            else // 2 Shorts
            {
                i += 2;
                outCodeUnits += 4;
            }
        }
        else
        {
            const uSys lowByte = inString[i] & 0xFF;
            if(lowByte <= 0xD7 || (lowByte >= 0xE0 && lowByte <= 0xFF)) // 1 Short
            {
                ++i;

                if((inString[i] >> 8) <= 0x7F)
                {
                    ++outCodeUnits;
                }
                else if(lowByte <= 0x07)
                {
                    outCodeUnits += 2;
                }
                else
                {
                    outCodeUnits += 3;
                }
            }
            else // 2 Shorts
            {
                i += 2;
                outCodeUnits += 4;
            }
        }
    }
    return outCodeUnits;
}

template<>
[[nodiscard]] inline constexpr iSys CalculateCodeUnits<c16>(const c16* const inString, const iSys inCodeUnits, const iSys startCodeUnit, const iSys priorCodeUnits, const bool skipBom) noexcept
{
    return CalculateCodeUnitsC16(inString, inCodeUnits, startCodeUnit, priorCodeUnits, skipBom);
}

template<>
[[nodiscard]] inline constexpr iSys CalculateCodeUnits<wchar_t>(const wchar_t* const inString, const iSys inCodeUnits, const iSys startCodeUnit, const iSys priorCodeUnits, const bool skipBom) noexcept
{
    return CalculateCodeUnitsC16(inString, inCodeUnits, startCodeUnit, priorCodeUnits, skipBom);
}

}

template<typename CharOut, typename CharIn, iSys Len>
struct CodeUnitCountConst final
{
    [[nodiscard]] inline static consteval iSys Count(const CharIn* const inString) noexcept
    {
        return -1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<c8, c8, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c8* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<c8, char, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const char* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<c8, c16, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c16* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c8, wchar_t, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const wchar_t* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c8, c32, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c32* const inString) noexcept
    {
        return utf8::CalculateCodeUnits(inString, Len - 1, 0, 0);
    }
};

template<iSys Len>
struct CodeUnitCountConst<char, c8, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c8* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<char, char, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const char* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<char, c16, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c16* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<char, wchar_t, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const wchar_t* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<char, c32, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c32* const inString) noexcept
    {
        return utf8::CalculateCodeUnits(inString, Len - 1, 0, 0);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c16, c8, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c8* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c16, char, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const char* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c16, c16, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c16* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<c16, wchar_t, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const wchar_t* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<c16, c32, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c32* const inString) noexcept
    {
        return utf16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<wchar_t, c8, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c8* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<wchar_t, char, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const char* const inString) noexcept
    {
        return utf8_16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
    }
};

template<iSys Len>
struct CodeUnitCountConst<wchar_t, c16, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c16* const inString) noexcept
    {
        if constexpr(sizeof(wchar_t) == sizeof(c16))
        {
            return Len - 1;
        }
        else if constexpr(sizeof(wchar_t) == sizeof(c32))
        {
            return utf16::CalculateCodePoints(inString, Len - 1, 0, 0);
        }
        else
        {
            return -1;
        }
    }
};

template<iSys Len>
struct CodeUnitCountConst<wchar_t, wchar_t, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const wchar_t* const inString) noexcept
    {
        return Len - 1;
    }
};

template<iSys Len>
struct CodeUnitCountConst<wchar_t, c32, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c32* const inString) noexcept
    {
        if constexpr(sizeof(wchar_t) == sizeof(c16))
        {
            return utf16::CalculateCodeUnits(inString, Len - 1, 0, 0, true);
        }
        else if constexpr(sizeof(wchar_t) == sizeof(c32))
        {
            return Len - 1;
        }
        else
        {
            return -1;
        }
    }
};

template<iSys Len>
struct CodeUnitCountConst<c32, c8, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c8* const inString) noexcept
    {
        return utf8::CalculateCodePoints(inString, Len - 1, 0, 0);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c32, char, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const char* const inString) noexcept
    {
        return utf8::CalculateCodePoints(inString, Len - 1, 0, 0);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c32, c16, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c16* const inString) noexcept
    {
        return utf16::CalculateCodePoints(inString, Len - 1, 0, 0);
    }
};

template<iSys Len>
struct CodeUnitCountConst<c32, wchar_t, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const wchar_t* const inString) noexcept
    {
        if constexpr(sizeof(wchar_t) == sizeof(c16))
        {
            return utf16::CalculateCodePoints(inString, Len - 1, 0, 0);
        }
        else if constexpr(sizeof(wchar_t) == sizeof(c32))
        {
            return Len - 1;
        }
        else
        {
            return -1;
        }
    }
};

template<iSys Len>
struct CodeUnitCountConst<c32, c32, Len> final
{
    [[nodiscard]] inline static consteval iSys Count(const c32* const inString) noexcept
    {
        return Len - 1;
    }
};

}
