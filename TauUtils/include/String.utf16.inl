#pragma once

#if defined(STRING_IN_DEV) || 1
#include "String.hpp"
#endif

namespace tau::string::utf16 {

[[nodiscard]] static inline c32 DecodeCodePointForwardUnsafe(const c16* const str, iSys& index, const iSys inCodeUnits, const bool flipEndian = false) noexcept
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

static inline iSys EncodeCodePoint(const c32 c, c16* const outString, iSys& outCodeUnit, const iSys outCodeUnits, const bool flipEndian = false) noexcept
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

[[nodiscard]] static inline c32 DecodeCodePointForward(const StringData<c16>& string, uSys& index, const bool flipEndian = false) noexcept
{
    if(index >= string.Length)
    { return U'\0'; }

    const c16* const str = string.String();
    const c32 ret = DecodeCodePointForwardUnsafe(str, reinterpret_cast<iSys&>(index), static_cast<iSys>(string.Length), flipEndian);
    return ret == static_cast<c32>(-1) ? U'\0' : ret;
}

[[nodiscard]] static inline c32 DecodeCodePointBackward(const StringData<c16>& string, uSys& index, const bool flipEndian = false) noexcept
{
    if(index >= string.Length)
    { return U'\0'; }

    const c16* const str = string.String();

    if(!flipEndian)
    {
        if(str[index] <= 0xD7FF || str[index] >= 0xE000)
        {
            const c32 ret = static_cast<c32>(str[index]);
            if(index != 0)
            { --index; }

            return ret;
        }
        else
        {
            if(index == 0)
            { return U'\0'; }
            
            const c32 lowBits = str[index--] - 0xDC00;
            const c32 highBits = (str[index] - 0xD800) << 10;

            if(index == 0)
            {
                ++index; // Reset pos
            }
            else
            {
                --index;
            }

            return (highBits | lowBits) + 0x10000;
        }
    }
    else
    {
        const c8* const byteStream = reinterpret_cast<const c8*>(str);

        const c16 codeUnit = (static_cast<c16>(byteStream[index * 2]) << 8) | byteStream[index * 2 + 1];

        if(codeUnit <= 0xD7FF || (codeUnit >= 0xE000 && codeUnit <= 0xFFFF))
        {
            if(index != 0)
            { --index; }
            return static_cast<c32>(codeUnit);
        }
        else
        {
            if(index == 0)
            { return U'\0'; }

            --index;

            const c32 highBits = (static_cast<c16>(byteStream[index * 2]) << 8) | byteStream[index * 2 + 1];

            if(index == 0)
            {
                ++index; // Reset pos
            }
            else
            {
                --index;
            }

            return ((highBits << 10) | static_cast<c32>(codeUnit)) + 0x10000;
        }
    }
}

[[nodiscard]] static inline iSys CalculateCodePoints(const c16* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodePoints = 0) noexcept
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

[[nodiscard]] static inline iSys CalculateCodeUnits(const c32* const inString, const iSys inCodeUnits, const iSys startCodeUnit = 0, const iSys priorCodeUnits = 0) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    iSys outCodeUnits = priorCodeUnits + 1;
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
    
static inline iSys Transform(const c16* const inString, c32* const outString, const iSys inCodeUnits, const iSys outCodeUnits) noexcept
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
    
static inline iSys Transform(const c32* const inString, c16* const outString, const iSys inCodeUnits, const iSys outCodeUnits, const bool flipEndian = false) noexcept
{
    if(!inString || inCodeUnits <= 0)
    { return -1; }

    if(!outString || outCodeUnits <= 0 || outCodeUnits < inCodeUnits + 1) // Just count if the outString is null or too short.
    {
        return CalculateCodeUnits(inString, inCodeUnits);
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

        iSys outCodeUnit = 1;
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

#define C16_INDEX_FLIP_BIT (uSys{1} << ((sizeof(uSys) * CHAR_BIT) - 1))

template<>
inline DynStringCodePointIteratorT<c16>::DynStringCodePointIteratorT(const StringData& string, const uSys index) noexcept
    : m_String(string)
    , m_Index(index)
{
    if(index == 0 && string.String()[0] == 0xFFFE)
    {
        m_CurrentCodePoint = tau::string::utf16::DecodeCodePointForward(string, m_Index);
        m_Index |= C16_INDEX_FLIP_BIT;
    }
    else
    {
        m_CurrentCodePoint = tau::string::utf16::DecodeCodePointForward(string, m_Index);
    }
}

template<>
inline DynStringCodePointIteratorT<c16>& DynStringCodePointIteratorT<c16>::operator++() noexcept
{
    ++m_Index;
    uSys index = m_Index & ~C16_INDEX_FLIP_BIT;
    m_CurrentCodePoint = tau::string::utf16::DecodeCodePointForward(m_String, index);
    m_Index = index | C16_INDEX_FLIP_BIT;
    return *this;
}

template<>
inline DynStringCodePointIteratorT<c16>& DynStringCodePointIteratorT<c16>::operator--() noexcept
{
    uSys index = m_Index & ~C16_INDEX_FLIP_BIT;
    m_CurrentCodePoint = tau::string::utf16::DecodeCodePointBackward(m_String, index);
    m_Index = index | C16_INDEX_FLIP_BIT;
    return *this;
}

template<>
inline bool DynStringCodePointIteratorT<c16>::operator==(const DynStringCodePointIteratorT<c16>& other) const noexcept
{ return (m_Index & ~C16_INDEX_FLIP_BIT) == (other.m_Index & ~C16_INDEX_FLIP_BIT); }

template<>
inline bool DynStringCodePointIteratorT<c16>::operator!=(const DynStringCodePointIteratorT<c16>& other) const noexcept
{ return (m_Index & ~C16_INDEX_FLIP_BIT) != (other.m_Index & ~C16_INDEX_FLIP_BIT); }

template<>
inline DynStringT<c32> StringCast(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.c_str();

    const iSys len = tau::string::utf16::CalculateCodePoints(rawStr, static_cast<iSys>(string.length()));

    if(len < 0)
    {
        return DynStringT<c32>();
    }

    if(len < 16)
    {
        c32 newStr[16];
        newStr[len] = U'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), len))
        {
            return DynStringT<c32>();
        }

        return DynStringT(static_cast<const c32*>(newStr));
    }
    else
    {
        void* const placement = operator new(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c32), ::std::nothrow);

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c32* const newStr = new(refCount + 1) c32[len + 1];
        newStr[len] = U'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), 16))
        {
            operator delete(placement);
            return DynStringT<c32>();
        }

        return DynStringT<c32>::passControl(refCount, newStr, len);
    }
}

template<>
inline DynStringT<c16> StringCast(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.c_str();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), 16))
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = operator new(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16), ::std::nothrow);

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), len))
        {
            operator delete(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len);
    }
}

inline DynStringT<c16> StringCastFlipEndian(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.c_str();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), 16, true))
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = operator new(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16), ::std::nothrow);

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.length()), len, true))
        {
            operator delete(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len);
    }
}
