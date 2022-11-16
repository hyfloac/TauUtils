#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string::utf8_16 {

[[nodiscard]] inline iSys CalculateCodeUnits(const c8* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0) noexcept
{
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

    iSys outCodeUnits = priorCodeUnits + 1; // Prior code points, plus BOM
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

[[nodiscard]] inline iSys CalculateCodeUnits(const c16* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0) noexcept
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

    iSys outCodeUnits = priorCodeUnits + 3; // Prior code points
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
    
inline iSys Transform(const c8* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false) noexcept
{
    using namespace tau::string::utf8;
    using namespace tau::string::utf16;

    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits * 2) // Just count if the outString is null or too short.
    {
        return utf8_16::CalculateCodeUnits(inString, inCodeUnits);
    }
    else
    {
        if(flipEndian)
        {
            outString[0] = 0xFFFE;
        }
        else
        {
            outString[0] = 0xFEFF;
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

        iSys outCodeUnit = 1;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const c32 c = DecodeCodePointForwardUnsafe(inString, i, inCodeUnits);
            if(c == static_cast<c32>(-1))
            { return -1; }

            const iSys priorCodeUnitsModifier = EncodeCodePoint(c, outString, outCodeUnit, outCodeUnits, flipEndian);

            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return utf8_16::CalculateCodeUnits(inString, inCodeUnits, i + 1, outCodeUnit + priorCodeUnitsModifier - 1); }
        }
        return outCodeUnit;
    }
}

inline iSys Transform(const c16* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
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

        outString[0] = 0xEF;
        outString[1] = 0xBB;
        outString[2] = 0xBF;

        iSys outCodeUnit = 3;
        for(iSys i = startingIndex; i < inCodeUnits; ++i)
        {
            const c32 c = DecodeCodePointForwardUnsafe(inString, i, inCodeUnits, flipEndian);
            if(c == static_cast<c32>(-1))
            { return -1; }

            const iSys priorCodeUnitsModifier = EncodeCodePoint(c, outString, outCodeUnit, outCodeUnits);
            
            if(priorCodeUnitsModifier == -1)
            { return -1; }
            else if(priorCodeUnitsModifier != 0)
            { return utf8_16::CalculateCodeUnits(inString, inCodeUnits, i + 1, outCodeUnit); }
        }
        return outCodeUnit;
    }
}
    
}

template<>
inline DynStringT<c8> StringCast(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c8>();
    }

    if(len < 16)
    {
        c8 newStr[16];
        newStr[len] = U'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            return DynStringT<c8>();
        }

        return DynStringT(static_cast<const c8*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c8));

        if(!placement)
        {
            return DynStringT<c8>();
        }

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c8* const newStr = ::new(refCount + 1) c8[len + 1];
        newStr[len] = U'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c8>();
        }

        return DynStringT<c8>::passControl(refCount, newStr, len, [](const c8* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCast(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c16* const newStr = ::new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

inline DynStringT<c16> StringCastFlipEndian(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}
