#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

namespace tau::string {

namespace utf8 {

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

}

namespace utf16 {

[[nodiscard]] inline c32 DecodeCodePointForward(const StringData<c16>& string, uSys& index, const bool flipEndian = false) noexcept
{
    if(index >= string.Length)
    { return U'\0'; }

    const c16* const str = string.String();
    const c32 ret = DecodeCodePointForwardUnsafe(str, reinterpret_cast<iSys&>(index), static_cast<iSys>(string.Length), flipEndian);
    return ret == static_cast<c32>(-1) ? U'\0' : ret;
}

[[nodiscard]] inline c32 DecodeCodePointBackward(const StringData<c16>& string, uSys& index, const bool flipEndian = false) noexcept
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

static inline constexpr uSys C16_INDEX_FLIP_BIT = uSys{1} << ((sizeof(uSys) * CHAR_BIT) - 1);

template<>
inline DynStringCodePointIteratorT<c16>::DynStringCodePointIteratorT(const StringData& string, const uSys index, const uSys start) noexcept
    : m_String(string)
    , m_Start(start)
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
