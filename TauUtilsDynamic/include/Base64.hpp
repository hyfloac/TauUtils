#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "String.hpp"
#include "TUMaths.hpp"

template<typename Char, bool EnforcePadding = false, Char PlusCharAlt = '+', Char SlashCharAlt = '/', Char PaddingCharAlt = '='>
class Base64Codec final
{
    DELETE_CONSTRUCT(Base64Codec);
    DELETE_DESTRUCT(Base64Codec);
    DELETE_CM(Base64Codec);
public:
    [[nodiscard]] static bool IsBase64Char(const Char c) noexcept
    {
        if(c >= Char{ 'A' } && c <= Char{ 'Z' })
        {
            return true;
        }

        if(c >= Char{ 'a' } && c <= Char{ 'z' })
        {
            return true;
        }

        if(c >= Char{ '0' } && c <= Char{ '9' })
        {
            return true;
        }

        if(c == PlusCharAlt || c == SlashCharAlt)
        {
            return true;
        }

        return false;
    }

    [[nodiscard]] static Char GetBase64Char(const u8 value) noexcept
    {
        if(value & 0xC0)
        {
            return Char{ '\0' };
        }

        return CHAR_MAP[value];
    }

    [[nodiscard]] static u8 GetBase64Bits(const Char base64Char) noexcept
    {
        if(base64Char >= Char{ 'A' } && base64Char <= Char{ 'Z' })
        {
            return static_cast<u8>(base64Char - Char{ 'A' });
        }

        if(base64Char >= Char{ 'a' } && base64Char <= Char{ 'z' })
        {
            return static_cast<u8>(base64Char - Char{ 'a' } + 26);
        }

        if(base64Char >= Char{ '0' } && base64Char <= Char{ '9' })
        {
            return static_cast<u8>(base64Char - Char{ '0' } + 52);
        }

        if(base64Char == PlusCharAlt)
        {
            return 0x3E;
        }

        if(base64Char == SlashCharAlt)
        {
            return 0x3F;
        }

        return 0xFF;
    }

    [[nodiscard]] static constexpr uSys RequiredLengthWithoutPadding(const uSys byteCount) noexcept
    {
        // We want ceiling here.
        return 1 + ((byteCount * 4 - 1) / 3);
    }

    [[nodiscard]] static constexpr uSys RequiredLengthWithPadding(const uSys byteCount) noexcept
    {
        return AlignTo<uSys, 4>(RequiredLengthWithoutPadding(byteCount));
    }

    // Ensures the base64 string will always end with at least 1 '=' char.
    // This means it can end with up to 3 '=' chars.
    [[nodiscard]] static constexpr uSys RequiredLengthWithPaddingAndTrailer(const uSys byteCount) noexcept
    {
        const uSys requiredLength = RequiredLengthWithoutPadding(byteCount);
        return requiredLength % 4 == 0 ? requiredLength + 3 : AlignTo<uSys, 4>(requiredLength);
    }

    [[nodiscard]] static constexpr uSys NumBytesWithoutPadding(const uSys stringLength) noexcept
    {
        // We want flooring here.
        return (stringLength * 3) / 4;
    }

    [[nodiscard]] static int Transform(const u8* const byteStream, const uSys byteStreamLength, Char* const base64Data, const uSys stringLength, uSys* const writeLength, const bool pad = false, const bool trail = false) noexcept
    {
        if(!byteStream)
        {
            return -201;
        }

        if(!byteStreamLength)
        {
            return -202;
        }

        // If we're writing a trail ===, then padding needs to be enabled.
        if(!pad && trail)
        {
            if(writeLength)
            {
                *writeLength = IntMaxMin<uSys>::Max;
            }
            return -203;
        }

        if(pad)
        {
            if(trail)
            {
                const uSys requiredLength = RequiredLengthWithPaddingAndTrailer(byteStreamLength);

                if(writeLength)
                {
                    *writeLength = requiredLength;
                }

                if(stringLength < requiredLength)
                {
                    return -102;
                }
            }
            else
            {
                const uSys requiredLength = RequiredLengthWithPadding(byteStreamLength);

                if(writeLength)
                {
                    *writeLength = requiredLength;
                }

                if(stringLength < requiredLength)
                {
                    return -102;
                }
            }
        }
        else
        {
            const uSys requiredLength = RequiredLengthWithoutPadding(byteStreamLength);

            if(writeLength)
            {
                *writeLength = requiredLength;
            }

            if(stringLength < requiredLength)
            {
                return -102;
            }
        }

        if(!base64Data || !stringLength)
        {
            return 0;
        }

        uSys writeIndex = 0;

        for(uSys i = 0; i < byteStreamLength; i += 3)
        {
            const u8 byte0 = byteStream[i];
            const u8 sex0 = byte0 >> 2;

            // Padding time.
            if(i + 3 > byteStreamLength)
            {
                // Two '=' required
                if(i + 2 > byteStreamLength)
                {
                    const u8 sex1 = (byte0 & 0x3) << 4;

                    base64Data[writeIndex++] = GetBase64Char(sex0);
                    base64Data[writeIndex++] = GetBase64Char(sex1);

                    if(pad)
                    {
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                    }
                }
                // One '=' required
                else // i + 3 > byteStreamLength
                {
                    const u8 byte1 = byteStream[i + 1];

                    const u8 sex1 = ((byte0 & 0x3) << 4) | (byte1 >> 4);
                    const u8 sex2 = (byte1 & 0xF) << 2;

                    base64Data[writeIndex++] = GetBase64Char(sex0);
                    base64Data[writeIndex++] = GetBase64Char(sex1);
                    base64Data[writeIndex++] = GetBase64Char(sex2);

                    if(pad)
                    {
                        base64Data[writeIndex++] = '=';
                    }
                }
            }
            else
            {
                const u8 byte1 = byteStream[i + 1];
                const u8 byte2 = byteStream[i + 2];
                
                const u8 sex1 = ((byte0 & 0x3) << 4) | (byte1 >> 4);
                const u8 sex2 = ((byte1 & 0xF) << 2) | (byte2 >> 6);
                const u8 sex3 = byte2 & 0x3F;

                base64Data[writeIndex++] = GetBase64Char(sex0);
                base64Data[writeIndex++] = GetBase64Char(sex1);
                base64Data[writeIndex++] = GetBase64Char(sex2);
                base64Data[writeIndex++] = GetBase64Char(sex3);

                if(i + 3 == byteStreamLength && trail)
                {
                    base64Data[writeIndex++] = '=';
                    base64Data[writeIndex++] = '=';
                    base64Data[writeIndex++] = '=';
                }
            }
        }

        return 0;
    }

    [[nodiscard]] static int Transform(const Char* const base64Data, const uSys stringLength, u8* const byteStream, const uSys byteStreamLength, uSys* const writeLength) noexcept
    {
        if(!base64Data)
        {
            return -201;
        }

        if(!stringLength)
        {
            return -202;
        }

        uSys padCharCount = 0;

        if(base64Data[stringLength - 1] == '=')
        {
            if(stringLength > 1 && base64Data[stringLength - 2] == '=')
            {
                if(stringLength > 2 && base64Data[stringLength - 3] == '=')
                {
                    padCharCount = 3;
                }
                else
                {
                    padCharCount = 2;
                }
            }
            else
            {
                padCharCount = 1;
            }
        }

        if constexpr(EnforcePadding)
        {
            if(padCharCount == 0)
            {
                if(writeLength)
                {
                    *writeLength = IntMaxMin<uSys>::Max;
                }
                return -301;
            }
        }
        
        const uSys requiredLength = NumBytesWithoutPadding(stringLength - padCharCount);

        if(writeLength)
        {
            *writeLength = requiredLength;
        }

        if(byteStreamLength < requiredLength)
        {
            return -102;
        }

        if(!byteStream || !byteStreamLength)
        {
            return 0;
        }

        uSys writeIndex = 0;

        const uSys requiredIterations = stringLength - padCharCount;

        for(uSys i = 0; i < requiredIterations; i += 4)
        {
            const Char char0 = base64Data[i];
            const u8 sex0 = GetBase64Bits(char0);

            if(sex0 == 0xFF)
            {
                if(writeLength)
                {
                    *writeLength = writeIndex;
                }
                return -401;
            }

            // Handle final bits
            if(i + 4 > requiredIterations)
            {
                // Check that the padding count matches.
                if constexpr(EnforcePadding)
                {
                    if(i + 4 != stringLength)
                    {
                        if(writeLength)
                        {
                            *writeLength = writeIndex;
                        }
                        return -302;
                    }
                }

                // One byte
                if(i + 3 > requiredIterations)
                {
                    const Char char1 = base64Data[i + 1];
                    const u8 sex1 = GetBase64Bits(char1);

                    if(sex1 == 0xFF)
                    {
                        if(writeLength)
                        {
                            *writeLength = writeIndex;
                        }
                        return -401;
                    }

                    const u8 byte0 = static_cast<u8>((sex0 << 2) | (sex1 >> 4));

                    byteStream[writeIndex++] = byte0;
                }
                // Error
                else if(i + 2 > requiredIterations)
                {
                    if(writeLength)
                    {
                        *writeLength = writeIndex;
                    }
                    return -402;
                }
                // Two bytes
                else // i + 4 > requiredIterations
                {
                    const Char char1 = base64Data[i + 1];
                    const Char char2 = base64Data[i + 2];

                    const u8 sex1 = GetBase64Bits(char1);
                    const u8 sex2 = GetBase64Bits(char2);

                    if(sex1 == 0xFF || sex2 == 0xFF)
                    {
                        if(writeLength)
                        {
                            *writeLength = writeIndex;
                        }
                        return -401;
                    }

                    const u8 byte0 = static_cast<u8>((sex0 << 2) | (sex1 >> 4));
                    const u8 byte1 = static_cast<u8>((sex1 << 4) | (sex2 >> 2));

                    byteStream[writeIndex++] = byte0;
                    byteStream[writeIndex++] = byte1;
                }
            }
            else
            {
                const Char char1 = base64Data[i + 1];
                const Char char2 = base64Data[i + 2];
                const Char char3 = base64Data[i + 3];

                const u8 sex1 = GetBase64Bits(char1);
                const u8 sex2 = GetBase64Bits(char2);
                const u8 sex3 = GetBase64Bits(char3);

                if(sex1 == 0xFF || sex2 == 0xFF || sex3 == 0xFF)
                {
                    if(writeLength)
                    {
                        *writeLength = writeIndex;
                    }
                    return -401;
                }

                const u8 byte0 = static_cast<u8>((sex0 << 2) | (sex1 >> 4));
                const u8 byte1 = static_cast<u8>((sex1 << 4) | (sex2 >> 2));
                const u8 byte2 = static_cast<u8>((sex2 << 6) | sex3);
                
                byteStream[writeIndex++] = byte0;
                byteStream[writeIndex++] = byte1;
                byteStream[writeIndex++] = byte2;
            }
        }

        return 0;
    }
private:
    static constexpr Char CHAR_MAP[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        PlusCharAlt, SlashCharAlt
    };
};

template<typename Char>
using DefaultBase64Codec = Base64Codec<Char, false>;
template<typename Char>
using StrictBase64Codec = Base64Codec<Char, true>;
template<typename Char>
using UrlBase64Codec = Base64Codec<Char, false, '-', '_'>;
template<typename Char>
using Utf7Base64Codec = Base64Codec<Char, false>;
