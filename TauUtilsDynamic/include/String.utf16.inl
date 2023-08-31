#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string::utf16 {

[[nodiscard]] inline constexpr c16 FlipEndian(const c16 codeUnit) noexcept
{
    return static_cast<c16>(
        ((codeUnit & 0x00FF) << 8) |
        ((codeUnit & 0xFF00) >> 8)
    );
}

[[nodiscard]] inline constexpr c32 FlipEndian(const c32 codeUnit) noexcept
{
    return static_cast<c32>(
        ((codeUnit & 0x000000FF) << 24) |
        ((codeUnit & 0x0000FF00) << 8) |
        ((codeUnit & 0x00FF0000) >> 8) |
        ((codeUnit & 0xFF000000) >> 24)
    );
}

[[nodiscard]] inline constexpr wchar_t FlipEndian(const wchar_t codeUnit) noexcept
{
    if constexpr(sizeof(wchar_t) == sizeof(c16))
    {
        return static_cast<wchar_t>(FlipEndian(static_cast<c16>(codeUnit)));
    }
    else if constexpr(sizeof(wchar_t) == sizeof(c32))
    {
        return static_cast<wchar_t>(FlipEndian(static_cast<c32>(codeUnit)));
    }
    else
    {
        return codeUnit;
    }
}

template<typename C16Alias = c16>
[[nodiscard]] inline constexpr c32 DecodeCodePointForwardUnsafe(const C16Alias* const str, iSys& index, const iSys inCodeUnits, const bool flipEndian = false) noexcept
{
    if(!flipEndian)
    {
        if(str[index] <= 0xD7FF || (str[index] >= 0xE000/* && str[index] <= 0xFFFF*/))
        {
            return static_cast<c32>(str[index]);
        }
        else
        {
            if(index + 1 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            const c32 highBits = (str[index++] - 0xD800) << 10;
            const c32 lowBits = str[index] - 0xDC00;

            return (highBits | lowBits) + 0x10000;
        }
    }
    else
    {
        const C16Alias codeUnit = FlipEndian(str[index]);

        if(codeUnit <= 0xD7FF || codeUnit >= 0xE000)
        {
            return static_cast<c32>(codeUnit);
        }
        else
        {
            if(index + 1 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            ++index;

            const c32 highBits = (codeUnit - 0xD800) << 10;
            const c32 lowBits = static_cast<c32>(FlipEndian(str[index]) - 0xDC00);

            return (highBits | lowBits) + 0x10000;
        }
    }
}

template<typename C16Alias = c16>
inline constexpr iSys EncodeCodePoint(const c32 c, C16Alias* const outString, iSys& outCodeUnit, const iSys outCodeUnits, const bool flipEndian = false) noexcept
{
    if(c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF))
    {
        if(outCodeUnit >= outCodeUnits)
        { return 1; }

        if(!flipEndian)
        {
            outString[outCodeUnit++] = static_cast<C16Alias>(c);
        }
        else
        {
            outString[outCodeUnit++] = FlipEndian(static_cast<C16Alias>(c));
        }
    }
    else if(c <= 0x10FFFF)
    {
        if(outCodeUnit + 1 >= outCodeUnits)
        { return 2; }

        const c32 codePoint = c - 0x10000;

        if(!flipEndian)
        {
            outString[outCodeUnit++] = static_cast<C16Alias>(codePoint >> 10) + 0xD800;
            outString[outCodeUnit++] = static_cast<C16Alias>(codePoint & 0x3FF) + 0xDC00;
        }
        else
        {
            const C16Alias highBits = static_cast<C16Alias>(codePoint >> 10) + 0xD800;
            const C16Alias lowBits  = static_cast<C16Alias>(codePoint & 0x3FF) + 0xDC00;

            outString[outCodeUnit++] = FlipEndian(highBits);
            outString[outCodeUnit++] = FlipEndian(lowBits);
        }
    }
    else
    { return -1; }

    return 0;
}

template<typename C16Alias = c16, typename C32Alias = c32>
inline constexpr iSys TransformC16ToC32(const C16Alias* const inString, C32Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < (inCodeUnits - 1) / 2) // Just count if the outString is null or too short. If the outString is less than 1/2 of the code units,
                                                                                // minus the BOM, then it is guaranteed to be too short.
    {
        return CalculateCodePoints(inString, inCodeUnits);
    }
    else
    {
        bool flipEndian = false;
        iSys startingIndex = 0;
        if(inCodeUnits >= 3)
        {
            if(inString[0] == 0xFEFF) // Skip BOM
            {
                startingIndex = 1;
            }
            else if(inString[0] == 0xFFFE) // Big Endian
            {
                flipEndian = true; 
                startingIndex = 1;
            }
        }
        
        iSys outCodeUnit = 0;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const C32Alias c = DecodeCodePointForwardUnsafe<C16Alias>(inString, i, inCodeUnits, flipEndian);
            if(c == static_cast<c32>(-1))
            { return -1; }
            outString[outCodeUnit++] = c;
            
            if(outCodeUnit >= outCodeUnits)
            {
                return CalculateCodePoints<C16Alias>(inString, inCodeUnits, i + 1, outCodeUnit);
            }
        }
        return outCodeUnit;
    }
}

template<typename C16Alias = c16, typename C32Alias = c32>
inline constexpr iSys TransformC32ToC16(const C32Alias* const inString, C16Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits) // Just count if the outString is null or too short.
    {
        return CalculateCodeUnits(inString, inCodeUnits);
    }
    else
    {
        if(!skipBom)
        {
            if(flipEndian)
            {
                outString[0] = 0xFFFE;
            }
            else
            {
                outString[0] = 0xFEFF;
            }
        }

        iSys outCodeUnit = skipBom ? 0 : 1;
        for(iSys i = 0; i < inCodeUnits; ++i)
        {
            const iSys priorCodeUnitsModifier = EncodeCodePoint<C16Alias>(inString[i], outString, outCodeUnit, outCodeUnits, flipEndian);

            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return CalculateCodeUnits(inString, inCodeUnits, i + 1, outCodeUnit + priorCodeUnitsModifier - 1); }
        }
        return outCodeUnit;
    }
}

template<typename CharIn, typename CharOut>
inline constexpr iSys Transform(const CharIn* const inString, CharOut* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return -1;
}

template<typename CharIn, typename CharOut>
inline constexpr iSys Transform(const CharIn* const inString, CharOut* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return -1;
}

template<>
inline constexpr iSys Transform<c16, c32>(const c16* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC16ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<c32, c16>(const c32* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return TransformC32ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

template<>
inline constexpr iSys Transform<wchar_t, c32>(const wchar_t* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC16ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<c32, wchar_t>(const c32* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return TransformC32ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

template<>
inline constexpr iSys Transform<c16, wchar_t>(const c16* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC16ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<wchar_t, c16>(const wchar_t* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return TransformC32ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

template<>
inline constexpr iSys Transform<wchar_t, wchar_t>(const wchar_t* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC16ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<wchar_t, wchar_t>(const wchar_t* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian, const bool skipBom) noexcept
{
    return TransformC32ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

}
