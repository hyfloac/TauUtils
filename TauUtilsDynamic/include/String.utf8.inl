#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

#include "allocator/TauAllocator.hpp"

static_assert(sizeof(c8) == 1, "char8_t was not 1 byte.");
static_assert(sizeof(c16) == 2, "char16_t was not 2 bytes.");
static_assert(sizeof(c32) == 4, "char32_t was not 4 bytes.");

namespace tau::string::utf8 {

template<typename C8Alias = c8>
[[nodiscard]] inline constexpr c32 DecodeCodePointForwardUnsafe(const C8Alias* const str, iSys& index, const iSys inCodeUnits) noexcept
{
    if((str[index] & 0x80) == 0) // U+0000 - U+007F
    {
        return static_cast<c32>(str[index]);
    }
    else
    {
        if((str[index] & 0xE0) == 0xC0) // U+0080 - U+07FF
        {
            if(index + 1 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            const c32 byte1Bits = (str[index] & 0x1F) << 6;
            ++index;
            const c32 byte2Bits = (str[index] & 0x3F);
            return byte1Bits | byte2Bits;
        }
        else if((str[index] & 0xF0) == 0xE0) // U+0800 - U+FFFF
        {
            if(index + 2 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            const c32 byte1Bits = (str[index] & 0x0F) << 12;
            ++index;
            const c32 byte2Bits = (str[index] & 0x3F) << 6;
            ++index;
            const c32 byte3Bits = (str[index] & 0x3F);
            return byte1Bits | byte2Bits | byte3Bits;
        }
        else if((str[index] & 0xF0) == 0xF0) // U+10000 - U+10FFFF
        {
            if(index + 3 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            const c32 byte1Bits = (str[index] & 0x07) << 18;
            ++index;
            const c32 byte2Bits = (str[index] & 0x3F) << 12;
            ++index;
            const c32 byte3Bits = (str[index] & 0x3F) << 6;
            ++index;
            const c32 byte4Bits = (str[index] & 0x3F);
            return byte1Bits | byte2Bits | byte3Bits | byte4Bits;
        }
        else
        {
            return static_cast<c32>(-1);
        }
    }
}

template<typename C8Alias = c8>
inline constexpr iSys EncodeCodePoint(const c32 c, C8Alias* const outString, iSys& outCodeUnit, const iSys outCodeUnits) noexcept
{
    if(c <= 0x7F) // 1 Byte
    {
        if(outCodeUnit >= outCodeUnits)
        { return 1; }

        outString[outCodeUnit++] = static_cast<C8Alias>(c);
    }
    else if(c <= 0x07FF) // 2 Bytes
    {
        if(outCodeUnit + 1 >= outCodeUnits)
        { return 2; }
        
        const c32 byte2 = (c & 0x3F) << 8;
        const c32 byte1 = (c & 0x07C0) >> 6;

        const c32 word = byte2 | byte1 | 0x80C0;
        ::std::memcpy(outString + outCodeUnit, &word, 2);
        outCodeUnit += 2;
    }
    else if(c <= 0xFFFF) // 3 Bytes
    {
        if(outCodeUnit + 2 >= outCodeUnits)
        { return 3; }
        
        const c32 byte3 = (c & 0x3F) << 16;
        const c32 byte2 = (c & 0x0FC0) << 2;
        const c32 byte1 = (c & 0xF000) >> 12;

        const c32 word = byte3 | byte2 | byte1 | 0x8080E0;
        ::std::memcpy(outString + outCodeUnit, &word, 3);
        outCodeUnit += 3;
    }
    else if(c <= 0x10FFFF) // 4 Bytes
    {
        if(outCodeUnit + 3 >= outCodeUnits)
        { return 4; }
        
        const c32 byte4 = (c & 0x3F) << 24;
        const c32 byte3 = (c & 0x0FC0) << 10;
        const c32 byte2 = (c & 0x03F000) >> 4;
        const c32 byte1 = (c & 0x1C0000) >> 18;

        const c32 word = byte4 | byte3 | byte2 | byte1 | 0x808080F0;
        ::std::memcpy(outString + outCodeUnit, &word, 4);
        outCodeUnit += 4;
    }
    else
    { return -1; }

    return 0;
}

template<typename C8Alias = c8, typename C32Alias = c32>
inline constexpr iSys TransformC8ToC32(const C8Alias* const inString, C32Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < (inCodeUnits - 3) / 4) // Just count if the outString is null or too short. If the outString is less than 1/4 of the code units,
                                                                                // minus the BOM, then it is guaranteed to be too short.
    {
        return CalculateCodePoints(inString, inCodeUnits);
    }
    else
    {
        iSys startingIndex = 0;
        if(inCodeUnits >= 3)
        {
            if(inString[0] == 0xEF &&
               inString[1] == 0xBB &&
               inString[2] == 0xBF) // Skip BOM
            {
                startingIndex = 3;
            }
        }

        iSys outCodeUnit = 0;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const c32 c = DecodeCodePointForwardUnsafe(inString, i, inCodeUnits);
            if(c == static_cast<c32>(-1))
            { return -1; }
            outString[outCodeUnit++] = c;

            if(outCodeUnit >= outCodeUnits)
            {
                return CalculateCodePoints(inString, inCodeUnits, i + 1, outCodeUnit);
            }
        }
        return outCodeUnit;
    }
}

template<typename C8Alias = c8, typename C32Alias = c32>
inline constexpr iSys TransformC32ToC8(const C32Alias* const inString, C8Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits) // Just count if the outString is null or too short.
    {
        return CalculateCodeUnits(inString, inCodeUnits);
    }
    else
    {
        iSys outCodeUnit = 0;
        for(iSys i = 0; i < inCodeUnits; ++i)
        {
            const iSys priorCodeUnitsModifier = EncodeCodePoint(inString[i], outString, outCodeUnit, outCodeUnits);

            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return CalculateCodeUnits(inString, inCodeUnits, i + 1, outCodeUnit + priorCodeUnitsModifier - 1); }
        }
        return outCodeUnit;
    }
}

template<typename CharIn, typename CharOut>
inline constexpr iSys Transform(const CharIn* const inString, CharOut* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return -1;
}

template<>
inline constexpr iSys Transform<c8, c32>(const c8* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC8ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<c32, c8>(const c32* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC32ToC8(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<char, c32>(const char* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC8ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<c32, char>(const c32* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC32ToC8(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<c8, wchar_t>(const c8* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC8ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<wchar_t, c8>(const wchar_t* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC32ToC8(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<char, wchar_t>(const char* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC8ToC32(inString, outString, inCodeUnits, outCodeUnits);
}

template<>
inline constexpr iSys Transform<wchar_t, char>(const wchar_t* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
{
    return TransformC32ToC8(inString, outString, inCodeUnits, outCodeUnits);
}

}
