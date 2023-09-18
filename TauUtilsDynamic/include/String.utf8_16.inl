#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string::utf8_16 {


template<typename C8Alias = c8, typename C16Alias = c16>
inline constexpr iSys TransformC8ToC16(const C8Alias* const inString, C16Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    using namespace tau::string::utf8;
    using namespace tau::string::utf16;

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits) // Just count if the outString is null or too short.
    {
        return utf8_16::CalculateCodeUnits(inString, inCodeUnits);
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

        iSys outCodeUnit = skipBom ? 0 : 1;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const c32 c = utf8::DecodeCodePointForwardUnsafe<C8Alias>(inString, i, inCodeUnits);
            if(c == static_cast<c32>(-1))
            { return -1; }

            const iSys priorCodeUnitsModifier = EncodeCodePoint<C16Alias>(c, outString, outCodeUnit, outCodeUnits, flipEndian);

            if(priorCodeUnitsModifier == -1)
            { return -1; }
            // else if(priorCodeUnitsModifier != 0)
            // { return utf8_16::CalculateCodeUnits<C8Alias>(inString, inCodeUnits, i + 1, outCodeUnit + priorCodeUnitsModifier - 1); }
        }
        return outCodeUnit;
    }
}

template<typename C8Alias = c8, typename C16Alias = c16>
inline constexpr iSys TransformC16ToC8(const C16Alias* const inString, C8Alias* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    using namespace tau::string::utf8;
    using namespace tau::string::utf16;

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits - 1) // Just count if the outString is null or too short. If the outString is less than the code units,
                                                                          // minus the BOM, then it is guaranteed to be too short.
    {
        return utf8_16::CalculateCodeUnits(inString, inCodeUnits);
    }
    else
    {
        bool flipEndian = false;
        iSys startingIndex = 0;
        if(inCodeUnits >= 1)
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

        if(!skipBom)
        {
            outString[0] = 0xEF;
            outString[1] = 0xBB;
            outString[2] = 0xBF;
        }

        iSys outCodeUnit = skipBom ? 0 : 3;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const c32 c = DecodeCodePointForwardUnsafe<C16Alias>(inString, i, inCodeUnits, flipEndian);
            if(c == static_cast<c32>(-1))
            { return -1; }

            const iSys priorCodeUnitsModifier = utf8::EncodeCodePoint<C8Alias>(c, outString, outCodeUnit, outCodeUnits);
            
            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return utf8_16::CalculateCodeUnits<C16Alias>(inString, inCodeUnits, i + 1, outCodeUnit); }
        }
        return outCodeUnit;
    }
}

inline constexpr iSys Transform(const c8* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    return TransformC8ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c16* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    return TransformC16ToC8(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

inline constexpr iSys Transform(const char* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    return TransformC8ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const c16* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    return TransformC16ToC8(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

inline constexpr iSys Transform(const c8* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return TransformC8ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const wchar_t* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return TransformC16ToC8(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

inline constexpr iSys Transform(const char* const inString, wchar_t* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return TransformC8ToC16(inString, outString, inCodeUnits, outCodeUnits, flipEndian, skipBom);
}

inline constexpr iSys Transform(const wchar_t* const inString, char* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool skipBom = false) noexcept
{
    if constexpr(sizeof(wchar_t) != sizeof(c16))
    {
        return -1;
    }

    return TransformC16ToC8(inString, outString, inCodeUnits, outCodeUnits, skipBom);
}

}
