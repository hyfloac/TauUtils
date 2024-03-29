#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "TUMaths.hpp"

/**
 * .-------0-------+ +-------1-------+ +-------2-------+ +-------3-------+ +-------4-------.
 * |0|0|0|0|0|1|1|1| |1|1|2|2|2|2|2|3| |3|3|3|3|4|4|4|4| |4|5|5|5|5|5|6|6| |6|6|6|7|7|7|7|7|
 * *-------0-------+ +-------1-------+ +-------2-------+ +-------3-------+ +-------4-------*
 * 
 * .-------0-------+ +-------1-------+ +-------2-------+ +-------3-------+ +-------4-------+ +-------5-------+ +-------6-------+ +-------7-------.
 * | | | |0|0|0|0|0| | | | |0|0|0|1|1| | | | |1|1|1|1|1| | | | |1|2|2|2|2| | | | |2|2|2|2|3| | | | |3|3|3|3|3| | | | |3|3|4|4|4| | | | |4|4|4|4|4|
 * *-------0-------+ +-------1-------+ +-------2-------+ +-------3-------+ +-------4-------+ +-------5-------+ +-------6-------+ +-------7-------*
 */
template<typename Char, bool EnforcePadding = false, Char PaddingCharAlt = '='>
class Base32Codec final
{
    DELETE_CONSTRUCT(Base32Codec);
    DELETE_DESTRUCT(Base32Codec);
    DELETE_CM(Base32Codec);
public:
    [[nodiscard]] static bool IsBase32Char(const Char c) noexcept
    {
        if(c >= Char{ 'A' } && c <= Char{ 'Z' })
        {
            return true;
        }

        if(c >= Char{ '2' } && c <= Char{ '7' })
        {
            return true;
        }
        
        return false;
    }

    [[nodiscard]] static Char GetBase32Char(const u8 value) noexcept
    {
        if(value & 0xE0)
        {
            return Char{ '\0' };
        }

        return CHAR_MAP[value];
    }

    [[nodiscard]] static u8 GetBase32Bits(const Char base32Char) noexcept
    {
        if(base32Char >= Char{ 'A' } && base32Char <= Char{ 'Z' })
        {
            return static_cast<u8>(base32Char - Char{ 'A' });
        }
        
        if(base32Char >= Char{ '2' } && base32Char <= Char{ '7' })
        {
            return static_cast<u8>(base32Char - Char { '2' } + 26);
        }
        
        return 0xFF;
    }

    [[nodiscard]] static constexpr uSys RequiredLengthWithoutPadding(const uSys byteCount) noexcept
    {
        // We want ceiling here.
        return 1 + ((byteCount * 8 - 1) / 5);
    }

    [[nodiscard]] static constexpr uSys RequiredLengthWithPadding(const uSys byteCount) noexcept
    {
        return AlignToAny<uSys, 5>(RequiredLengthWithoutPadding(byteCount));
    }

    // Ensures the base32 string will always end with at least 1 '=' char.
    // This means it can end with up to 7 '=' chars.
    [[nodiscard]] static constexpr uSys RequiredLengthWithPaddingAndTrailer(const uSys byteCount) noexcept
    {
        const uSys requiredLength = RequiredLengthWithoutPadding(byteCount);
        return requiredLength % 8 == 0 ? requiredLength + 7 : AlignToAny<uSys, 5>(requiredLength);
    }

    [[nodiscard]] static constexpr uSys NumBytesWithoutPadding(const uSys stringLength) noexcept
    {
        // We want flooring here.
        return (stringLength * 5) / 8;
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

        // If we're writing a trail =======, then padding needs to be enabled.
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

        for(uSys i = 0; i < byteStreamLength; i += 5)
        {
            const u8 byte0 = byteStream[i];
            const u8 sex0 = byte0 >> 3;

            // Padding time.
            if(i + 4 >= byteStreamLength)
            {
                // Two '6' required
                if(i + 1 >= byteStreamLength)
                {
                    const u8 sex1 = (byte0 & 0x7) << 2;

                    base64Data[writeIndex++] = GetBase32Char(sex0);
                    base64Data[writeIndex++] = GetBase32Char(sex1);

                    if(pad)
                    {
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                    }
                }
                // Four '=' required
                else if(i + 2 >= byteStreamLength)
                {
                    const u8 byte1 = byteStream[i + 1];

                    const u8 sex1 = ((byte0 & 0x7) << 2) | (byte1 >> 6);
                    const u8 sex2 = (byte1 >> 1) & 0x1F;
                    const u8 sex3 = (byte1 & 0x1) << 4;

                    base64Data[writeIndex++] = GetBase32Char(sex0);
                    base64Data[writeIndex++] = GetBase32Char(sex1);
                    base64Data[writeIndex++] = GetBase32Char(sex2);
                    base64Data[writeIndex++] = GetBase32Char(sex3);

                    if(pad)
                    {
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                    }
                }
                // Three '=' required
                else if(i + 3 >= byteStreamLength)
                {
                    const u8 byte1 = byteStream[i + 1];
                    const u8 byte2 = byteStream[i + 2];

                    const u8 sex1 = ((byte0 & 0x7) << 2) | (byte1 >> 6);
                    const u8 sex2 = (byte1 >> 1) & 0x1F;
                    const u8 sex3 = ((byte1 & 0x1) << 4) | (byte2 >> 4);
                    const u8 sex4 = (byte2 & 0xF) << 1;

                    base64Data[writeIndex++] = GetBase32Char(sex0);
                    base64Data[writeIndex++] = GetBase32Char(sex1);
                    base64Data[writeIndex++] = GetBase32Char(sex2);
                    base64Data[writeIndex++] = GetBase32Char(sex3);
                    base64Data[writeIndex++] = GetBase32Char(sex4);

                    if(pad)
                    {
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                        base64Data[writeIndex++] = '=';
                    }
                }
                // One '=' required
                else // i + 4 >= byteStreamLength
                {
                    const u8 byte1 = byteStream[i + 1];
                    const u8 byte2 = byteStream[i + 2];
                    const u8 byte3 = byteStream[i + 3];

                    const u8 sex1 = ((byte0 & 0x7) << 2) | (byte1 >> 6);
                    const u8 sex2 = (byte1 >> 1) & 0x1F;
                    const u8 sex3 = ((byte1 & 0x1) << 4) | (byte2 >> 4);
                    const u8 sex4 = ((byte2 & 0xF) << 1) | (byte3 >> 7);
                    const u8 sex5 = (byte3 >> 2) & 0x1F;
                    const u8 sex6 = (byte3 & 0x3) << 3;

                    base64Data[writeIndex++] = GetBase32Char(sex0);
                    base64Data[writeIndex++] = GetBase32Char(sex1);
                    base64Data[writeIndex++] = GetBase32Char(sex2);
                    base64Data[writeIndex++] = GetBase32Char(sex3);
                    base64Data[writeIndex++] = GetBase32Char(sex4);
                    base64Data[writeIndex++] = GetBase32Char(sex5);
                    base64Data[writeIndex++] = GetBase32Char(sex6);

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
                const u8 byte3 = byteStream[i + 3];
                const u8 byte4 = byteStream[i + 4];

                const u8 sex1 = ((byte0 & 0x7) << 2) | (byte1 >> 6);
                const u8 sex2 = (byte1 >> 1) & 0x1F;
                const u8 sex3 = ((byte1 & 0x1) << 4) | (byte2 >> 4);
                const u8 sex4 = ((byte2 & 0xF) << 1) | (byte3 >> 7);
                const u8 sex5 = (byte3 >> 2) & 0x1F;
                const u8 sex6 = ((byte3 & 0x3) << 3) | (byte4 >> 5);
                const u8 sex7 = byte4 & 0x1F;

                base64Data[writeIndex++] = GetBase32Char(sex0);
                base64Data[writeIndex++] = GetBase32Char(sex1);
                base64Data[writeIndex++] = GetBase32Char(sex2);
                base64Data[writeIndex++] = GetBase32Char(sex3);
                base64Data[writeIndex++] = GetBase32Char(sex4);
                base64Data[writeIndex++] = GetBase32Char(sex5);
                base64Data[writeIndex++] = GetBase32Char(sex6);
                base64Data[writeIndex++] = GetBase32Char(sex7);

                if(i + 5 == byteStreamLength && trail)
                {
                    base64Data[writeIndex++] = '=';
                    base64Data[writeIndex++] = '=';
                    base64Data[writeIndex++] = '=';
                    base64Data[writeIndex++] = '=';
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
                    if(stringLength > 3 && base64Data[stringLength - 4] == '=')
                    {
                        if(stringLength > 4 && base64Data[stringLength - 5] == '=')
                        {
                            if(stringLength > 5 && base64Data[stringLength - 6] == '=')
                            {
                                if(stringLength > 6 && base64Data[stringLength - 7] == '=')
                                {
                                    padCharCount = 7;
                                }
                                else
                                {
                                    padCharCount = 6;
                                }
                            }
                            else
                            {
                                padCharCount = 5;
                            }
                        }
                        else
                        {
                            padCharCount = 4;
                        }
                    }
                    else
                    {
                        padCharCount = 3;
                    }
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

        for(uSys i = 0; i < requiredIterations; i += 8)
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
                const Char char4 = base64Data[i + 4];
                const Char char5 = base64Data[i + 5];
                const Char char6 = base64Data[i + 6];
                const Char char7 = base64Data[i + 7];

                const u8 sex1 = GetBase64Bits(char1);
                const u8 sex2 = GetBase64Bits(char2);
                const u8 sex3 = GetBase64Bits(char3);
                const u8 sex4 = GetBase64Bits(char4);
                const u8 sex5 = GetBase64Bits(char5);
                const u8 sex6 = GetBase64Bits(char6);
                const u8 sex7 = GetBase64Bits(char7);

                if(sex1 == 0xFF || sex2 == 0xFF || sex3 == 0xFF || sex4 == 0xFF || sex5 == 0xFF || sex6 == 0xFF || sex7 == 0xFF)
                {
                    if(writeLength)
                    {
                        *writeLength = writeIndex;
                    }
                    return -401;
                }

                const u8 byte0 = static_cast<u8>((sex0 << 3) | (sex1 >> 5));
                const u8 byte1 = static_cast<u8>((sex1 << 6) | (sex2 << 1) | (sex3 >> 7));
                const u8 byte2 = static_cast<u8>((sex3 << 4) | (sex4 >> 4));
                const u8 byte3 = static_cast<u8>((sex4 << 7) | (sex5 << 2) | (sex6 >> 6));
                const u8 byte4 = static_cast<u8>((sex6 << 5) | sex7);

                byteStream[writeIndex++] = byte0;
                byteStream[writeIndex++] = byte1;
                byteStream[writeIndex++] = byte2;
                byteStream[writeIndex++] = byte3;
                byteStream[writeIndex++] = byte4;
            }
        }

        return 0;
    }
private:
    static constexpr Char CHAR_MAP[32] = {
        'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R',
        'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', '2', '3', '4', '5',
        '6', '7'
    };
};
