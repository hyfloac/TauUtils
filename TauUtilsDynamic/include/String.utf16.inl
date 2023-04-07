#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string::utf16 {

[[nodiscard]] inline c32 DecodeCodePointForwardUnsafe(const c16* const str, iSys& index, const iSys inCodeUnits, const bool flipEndian = false) noexcept
{
    if(!flipEndian)
    {
        if(str[index] <= 0xD7FF || (str[index] >= 0xE000 && str[index] <= 0xFFFF))
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
        const c8* const byteStream = reinterpret_cast<const c8*>(str);

        const c16 codeUnit = static_cast<c16>((byteStream[index * 2] << 8) | byteStream[index * 2 + 1]);

        if(codeUnit <= 0xD7FF || codeUnit >= 0xE000)
        {
            return static_cast<c32>(codeUnit);
        }
        else
        {
            if(index + 1 >= inCodeUnits)
            { return static_cast<c32>(-1); }

            ++index;

            const c32 lowBits = (static_cast<c16>(byteStream[index * 2]) << 8) | byteStream[index * 2 + 1];

            return ((static_cast<c32>(codeUnit) << 10) | lowBits) + 0x10000;
        }
    }
}

inline iSys EncodeCodePoint(const c32 c, c16* const outString, iSys& outCodeUnit, const iSys outCodeUnits, const bool flipEndian = false) noexcept
{
    c8* const byteStream = reinterpret_cast<c8*>(outString);

    if(c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF))
    {
        if(outCodeUnit >= outCodeUnits)
        { return 1; }

        if(!flipEndian)
        {
            outString[outCodeUnit++] = static_cast<c16>(c);
        }
        else
        {
            byteStream[outCodeUnit * 2] = static_cast<c8>(c >> 8);
            byteStream[outCodeUnit * 2 + 1] = static_cast<c8>(c & 0xFF);
            ++outCodeUnit;
        }
    }
    else if(c <= 0x10FFFF)
    {
        if(outCodeUnit + 1 >= outCodeUnits)
        { return 2; }

        const c32 codePoint = c - 0x10000;

        if(!flipEndian)
        {
            outString[outCodeUnit++] = static_cast<c16>(codePoint >> 10) + 0xD800;
            outString[outCodeUnit++] = static_cast<c16>(codePoint & 0x3FF) + 0xDC00;
        }
        else
        {
            const c16 highBits = static_cast<c16>(codePoint >> 10) + 0xD800;
            const c16 lowBits = static_cast<c16>(codePoint & 0x3FF) + 0xDC00;

            byteStream[outCodeUnit * 2] = static_cast<c8>(highBits >> 8);
            byteStream[outCodeUnit * 2 + 1] = static_cast<c8>(highBits & 0xFF);
            ++outCodeUnit;
            
            byteStream[outCodeUnit * 2] = static_cast<c8>(lowBits >> 8);
            byteStream[outCodeUnit * 2 + 1] = static_cast<c8>(lowBits & 0xFF);
            ++outCodeUnit;
        }
    }
    else
    { return -1; }

    return 0;
}

[[nodiscard]] inline iSys CalculateCodePoints(const c16* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodePoints = 0) noexcept
{
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

[[nodiscard]] inline iSys CalculateCodeUnits(const c32* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0, const bool skipBom = false) noexcept
{
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
    
inline iSys Transform(const c16* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
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
            const c32 c = DecodeCodePointForwardUnsafe(inString, i, inCodeUnits, flipEndian);
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
    
inline iSys Transform(const c32* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false, const bool skipBom = false) noexcept
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
            const iSys priorCodeUnitsModifier = EncodeCodePoint(inString[i], outString, outCodeUnit, outCodeUnits, flipEndian);

            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return CalculateCodeUnits(inString, inCodeUnits, i + 1, outCodeUnit + priorCodeUnitsModifier - 1); }
        }
        return outCodeUnit;
    }
}

}
