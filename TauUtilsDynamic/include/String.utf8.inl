#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

#include "allocator/TauAllocator.hpp"

static_assert(sizeof(c8) == 1, "char8_t was not 1 byte.");
static_assert(sizeof(c16) == 2, "char16_t was not 2 bytes.");
static_assert(sizeof(c32) == 4, "char32_t was not 4 bytes.");

namespace tau::string::utf8 {

[[nodiscard]] inline c32 DecodeCodePointForwardUnsafe(const c8* const str, iSys& index, const iSys inCodeUnits) noexcept
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

inline iSys EncodeCodePoint(const c32 c, c8* const outString, iSys& outCodeUnit, const iSys outCodeUnits) noexcept
{
    if(c <= 0x7F) // 1 Byte
    {
        if(outCodeUnit >= outCodeUnits)
        { return 1; }

        outString[outCodeUnit++] = static_cast<c8>(c);
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

[[nodiscard]] inline c32 DecodeCodePointForward(const StringData<c8>& string, uSys& index) noexcept
{
    if(index >= string.Length)
    { return U'\0'; }

    const c8* const str = string.String();
    const c32 ret = DecodeCodePointForwardUnsafe(str, reinterpret_cast<iSys&>(index), static_cast<iSys>(string.Length));
    return ret == static_cast<c32>(-1) ? U'\0' : ret;
}

[[nodiscard]] inline c32 DecodeCodePointBackward(const StringData<c8>& string, uSys& index) noexcept
{
    if(index >= string.Length)
    {
        return U'\0';
    }
    else
    {
        const c8* const str = string.String();
        if((str[index] & 0x80) == 0) // U+0000 - U+007F
        {
            const c32 ret = static_cast<c32>(str[index]);
            if(index != 0)
            {
                --index;
            }
            return ret;
        }
        else
        {
            if((str[index] & 0xC0) == 0xC0) // U+0080 - U+10FFFF
            {
                if(index == 0)
                { return U'\0'; }

                const c32 byteR1Bits = (str[index] & 0x3F);
                --index;

                if((str[index] & 0xC0) == 0xC0) // U+0800 - U+10FFFF
                {
                    if(index == 0)
                    {
                        ++index; // Reset pos
                        return U'\0';
                    }

                    const c32 byteR2Bits = (str[index] & 0x3F) << 6;
                    --index;
                    
                    if((str[index] & 0xC0) == 0xC0) // U+10000 - U+10FFFF
                    {
                        if(index == 0)
                        {
                            index += 2; // Reset pos
                            return U'\0';
                        }

                        const c32 byteR3Bits = (str[index] & 0x3F) << 12;
                        --index;

                        const c32 byte1Bits = (str[index] & 0x07) << 18;

                        if(index == 0)
                        {
                            index += 3; // Reset pos
                        }
                        else
                        {
                            --index;
                        }

                        return byte1Bits | byteR3Bits | byteR2Bits | byteR1Bits;
                    }
                    else // U+0800 - U+FFFF
                    {
                        const c32 byte1Bits = (str[index] & 0x0F) << 12;
                        
                        if(index == 0)
                        {
                            index += 2; // Reset pos
                        }
                        else
                        {
                            --index;
                        }

                        return byte1Bits | byteR2Bits | byteR1Bits;
                    }
                }
                else // U+0080 - U+07FF
                {
                    const c32 byte1Bits = (str[index] & 0x1F) << 6;
                    
                    if(index == 0)
                    {
                        ++index; // Reset pos
                    }
                    else
                    {
                        --index;
                    }

                    return byte1Bits | byteR1Bits;
                }
            }
            else
            {
                return U'\0'; 
            }
        }
    }
}

[[nodiscard]] inline iSys CalculateCodePoints(const c8* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodePoints = 0) noexcept
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

[[nodiscard]] inline iSys CalculateCodeUnits(const c32* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0) noexcept
{
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

inline iSys Transform(const c8* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
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

inline iSys Transform(const c32* const inString, c8* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
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

}

template<>
inline DynStringCodePointIteratorT<c8>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start) noexcept
    : m_String(string)
    , m_Start(start)
    , m_Index(index)
    , m_CurrentCodePoint(tau::string::utf8::DecodeCodePointForward(string, m_Index))
{ }

template<>
inline DynStringCodePointIteratorT<c8>& DynStringCodePointIteratorT<c8>::operator++() noexcept
{
    ++m_Index;
    m_CurrentCodePoint = tau::string::utf8::DecodeCodePointForward(m_String, m_Index);
    return *this;
}

template<>
inline DynStringCodePointIteratorT<c8>& DynStringCodePointIteratorT<c8>::operator--() noexcept
{
    m_CurrentCodePoint = tau::string::utf8::DecodeCodePointBackward(m_String, m_Index);
    return *this;
}

template<>
inline DynStringT<c32> StringCast(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8::CalculateCodePoints(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c32>();
    }

    if(len < 16)
    {
        c32 newStr[16];
        newStr[len] = U'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            return DynStringT<c32>();
        }

        return DynStringT(static_cast<const c32*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c32));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c32* const newStr = ::new(refCount + 1) c32[len + 1];
        newStr[len] = U'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c32>();
        }

        return DynStringT<c32>::passControl(refCount, newStr, len, [](const c32* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c8> StringCast(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf8::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c8>();
    }

    if(len < 16)
    {
        c8 newStr[16];
        newStr[len] = u8'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            return DynStringT<c8>();
        }

        return DynStringT<c8>(static_cast<const c8*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c8));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c8* const newStr = ::new(refCount + 1) c8[len + 1];
        newStr[len] = u8'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c8>();
        }

        return DynStringT<c8>::passControl(refCount, newStr, len, [](const c8* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}
