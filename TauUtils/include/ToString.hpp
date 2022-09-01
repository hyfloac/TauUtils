#pragma once

#include "NumTypes.hpp"
//#include "TUMaths.hpp"
#include <new>

namespace tau {

template<typename T, typename SizeT>
inline void ReverseBuffer(T* const buffer, const SizeT bufferSize) noexcept
{
    const SizeT sizeM1 = bufferSize - 1;

    for(SizeT i = 0, ri = sizeM1, end = bufferSize / 2; i < end; ++i, --ri)
    {
        T tmp = ::std::move(buffer[i]);
        buffer[i] = ::std::move(buffer[ri]);
        buffer[ri] = ::std::move(tmp);
    }
}

template<typename T, uSys BufferSize>
inline void ReverseBuffer(T* const buffer) noexcept
{
    constexpr uSys sizeM1 = BufferSize - 1;

    for(uSys i = 0, ri = sizeM1, end = BufferSize / 2; i < end; ++i, --ri)
    {
        T tmp = ::std::move(buffer[i]);
        buffer[i] = ::std::move(buffer[ri]);
        buffer[ri] = ::std::move(tmp);
    }
}

template<typename T, uSys BufferSize>
inline void ReverseBuffer(T(&buffer)[BufferSize]) noexcept
{
    const uSys sizeM1 = BufferSize - 1;
    
    for(uSys i = 0, ri = sizeM1, end = BufferSize / 2; i < end; ++i, --ri)
    {
        T tmp = ::std::move(buffer[i]);
        buffer[i] = ::std::move(buffer[ri]);
        buffer[ri] = ::std::move(tmp);
    }
}

template<typename Int, typename Char>
inline Char GetNumberUpper(Int val) noexcept
{
    switch(val)
    {
        case 0x0: return Char{'0'};
        case 0x1: return Char{'1'};
        case 0x2: return Char{'2'};
        case 0x3: return Char{'3'};
        case 0x4: return Char{'4'};
        case 0x5: return Char{'5'};
        case 0x6: return Char{'6'};
        case 0x7: return Char{'7'};
        case 0x8: return Char{'8'};
        case 0x9: return Char{'9'};
        case 0xA: return Char{'A'};
        case 0xB: return Char{'B'};
        case 0xC: return Char{'C'};
        case 0xD: return Char{'D'};
        case 0xE: return Char{'E'};
        case 0xF: return Char{'F'};
        default:  return Char{'?'};
    }
}

template<typename Int, typename Char>
inline Char GetNumberLower(Int val) noexcept
{
    switch(val)
    {
        case 0x0: return Char{'0'};
        case 0x1: return Char{'1'};
        case 0x2: return Char{'2'};
        case 0x3: return Char{'3'};
        case 0x4: return Char{'4'};
        case 0x5: return Char{'5'};
        case 0x6: return Char{'6'};
        case 0x7: return Char{'7'};
        case 0x8: return Char{'8'};
        case 0x9: return Char{'9'};
        case 0xA: return Char{'a'};
        case 0xB: return Char{'b'};
        case 0xC: return Char{'c'};
        case 0xD: return Char{'d'};
        case 0xE: return Char{'e'};
        case 0xF: return Char{'f'};
        default:  return Char{'?'};
    }
}

template<typename Int>
struct MaxCharCount final
{ static constexpr uSys Value = 0; };

template<>
struct MaxCharCount<i8> final
{ static constexpr uSys Value = 4; };

template<>
struct MaxCharCount<i16> final
{ static constexpr uSys Value = 6; };

template<>
struct MaxCharCount<i32> final
{ static constexpr uSys Value = 11; };

template<>
struct MaxCharCount<long> final
{ static constexpr uSys Value = 11; };

template<>
struct MaxCharCount<i64> final
{ static constexpr uSys Value = 20; };

template<>
struct MaxCharCount<u8> final
{ static constexpr uSys Value = 3; };

template<>
struct MaxCharCount<u16> final
{ static constexpr uSys Value = 5; };

template<>
struct MaxCharCount<u32> final
{ static constexpr uSys Value = 10; };

template<>
struct MaxCharCount<unsigned long> final
{ static constexpr uSys Value = 10; };

template<>
struct MaxCharCount<u64> final
{ static constexpr uSys Value = 20; };

template<>
struct MaxCharCount<bool> final
{ static constexpr uSys Value = 5; };

template<typename Int, typename Char>
inline i32 ItoA(Int val, Char* const buffer, const uSys bufferSize)
{
    if(val == 0)
    {
        if(bufferSize >= 2)
        {
            buffer[0] = Char{'0'};
            buffer[1] = Char{'\0'};
        }
        return 2;
    }

    bool negative = false;
    uSys writeIndex = 0;

    if constexpr(::std::is_signed_v<Int>)
    {
        if(val < 0)
        {
            if(val == IntMaxMin<Int>::Min)
            {
                if constexpr(::std::is_same_v<Int, i8>)
                {
                    if(bufferSize >= 5)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'1'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'8'};
                        buffer[4] = Char{'\0'};
                    }
                    return 4;
                }
                else if constexpr(::std::is_same_v<Int, i16>)
                {
                    if(bufferSize >= 7)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'3'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'7'};
                        buffer[4] = Char{'6'};
                        buffer[5] = Char{'8'};
                        buffer[6] = Char{'\0'};
                    }
                    return 6;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    if(bufferSize >= 12)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'2'};
                        buffer[2] = Char{'1'};
                        buffer[3] = Char{'4'};
                        buffer[4] = Char{'7'};
                        buffer[5] = Char{'4'};
                        buffer[6] = Char{'8'};
                        buffer[7] = Char{'3'};
                        buffer[8] = Char{'6'};
                        buffer[9] = Char{'4'};
                        buffer[10] = Char{'8'};
                        buffer[11] = Char{'\0'};
                    }
                    return 11;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    if(bufferSize >= 21)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'9'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'2'};
                        buffer[4] = Char{'3'};
                        buffer[5] = Char{'3'};
                        buffer[6] = Char{'7'};
                        buffer[7] = Char{'2'};
                        buffer[8] = Char{'0'};
                        buffer[9] = Char{'3'};
                        buffer[10] = Char{'6'};
                        buffer[11] = Char{'8'};
                        buffer[12] = Char{'5'};
                        buffer[13] = Char{'4'};
                        buffer[14] = Char{'7'};
                        buffer[15] = Char{'7'};
                        buffer[16] = Char{'5'};
                        buffer[17] = Char{'8'};
                        buffer[18] = Char{'0'};
                        buffer[19] = Char{'8'};
                        buffer[20] = Char{'\0'};
                    }
                    return 20;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                if(bufferSize > 1)
                {
                    buffer[0] = Char{'-'};
                }
                val = -val;
                writeIndex = 1;
                negative = true;
            }
        }
    }

    while(val != 0)
    {
        const Int digit = val % 10;
        val /= 10;

        if(writeIndex < bufferSize)
        {
            buffer[writeIndex] = GetNumberUpper<Int>(digit);
        }

        ++writeIndex;
    }

    if(writeIndex < bufferSize)
    {
        if(negative)
        {
            ReverseBuffer(buffer + 1, writeIndex - 1);
        }
        else
        {
            ReverseBuffer(buffer, writeIndex);
        }
        buffer[writeIndex] = Char{'\0'};
    }
    else
    {
        buffer[0] = Char{'\0'};
    }

    return static_cast<i32>(writeIndex);
}

template<typename Int, typename Char, uSys BufferSize>
inline i32 ItoA(Int val, Char(&buffer)[BufferSize])
{
    if(val == 0)
    {
        if constexpr(BufferSize >= 2)
        {
            buffer[0] = Char{'0'};
            buffer[1] = Char{'\0'};
        }
        return 2;
    }

    bool negative = false;
    uSys writeIndex = 0;

    if constexpr(::std::is_signed_v<Int>)
    {
        if(val < 0)
        {
            if(val == IntMaxMin<Int>::Min)
            {
                if constexpr(::std::is_same_v<Int, i8>)
                {
                    if constexpr(BufferSize >= 5)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'1'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'8'};
                        buffer[4] = Char{'\0'};
                    }
                    return 4;
                }
                else if constexpr(::std::is_same_v<Int, i16>)
                {
                    if constexpr(BufferSize >= 7)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'3'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'7'};
                        buffer[4] = Char{'6'};
                        buffer[5] = Char{'8'};
                        buffer[6] = Char{'\0'};
                    }
                    return 6;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    if constexpr(BufferSize >= 12)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'2'};
                        buffer[2] = Char{'1'};
                        buffer[3] = Char{'4'};
                        buffer[4] = Char{'7'};
                        buffer[5] = Char{'4'};
                        buffer[6] = Char{'8'};
                        buffer[7] = Char{'3'};
                        buffer[8] = Char{'6'};
                        buffer[9] = Char{'4'};
                        buffer[10] = Char{'8'};
                        buffer[11] = Char{'\0'};
                    }
                    return 11;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    if constexpr(BufferSize >= 21)
                    {
                        buffer[0] = Char{'-'};
                        buffer[1] = Char{'9'};
                        buffer[2] = Char{'2'};
                        buffer[3] = Char{'2'};
                        buffer[4] = Char{'3'};
                        buffer[5] = Char{'3'};
                        buffer[6] = Char{'7'};
                        buffer[7] = Char{'2'};
                        buffer[8] = Char{'0'};
                        buffer[9] = Char{'3'};
                        buffer[10] = Char{'6'};
                        buffer[11] = Char{'8'};
                        buffer[12] = Char{'5'};
                        buffer[13] = Char{'4'};
                        buffer[14] = Char{'7'};
                        buffer[15] = Char{'7'};
                        buffer[16] = Char{'5'};
                        buffer[17] = Char{'8'};
                        buffer[18] = Char{'0'};
                        buffer[19] = Char{'8'};
                        buffer[20] = Char{'\0'};
                    }
                    return 20;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                if constexpr(BufferSize > 1)
                {
                    buffer[0] = Char{'-'};
                }
                val = -val;
                writeIndex = 1;
                negative = true;
            }
        }
    }

    while(val != 0)
    {
        const Int digit = val % 10;
        val /= 10;

        if(writeIndex < BufferSize)
        {
            buffer[writeIndex] = GetNumberUpper<Int, Char>(digit);
        }

        ++writeIndex;
    }

    if(writeIndex < BufferSize)
    {
        if(negative)
        {
            ReverseBuffer(buffer + 1, writeIndex - 1);
        }
        else
        {
            ReverseBuffer(buffer, writeIndex);
        }
        buffer[writeIndex] = Char{'\0'};
    }
    else
    {
        buffer[0] = Char{'\0'};
    }
    
    return static_cast<i32>(writeIndex);
}

template<bool Uppercase, typename Int, typename Char>
inline i32 XtoA(const Int val, Char* const buffer, const uSys bufferSize)
{
    using UInt = ::std::make_unsigned_t<Int>;

    if(val == 0)
    {
        if(bufferSize >= 2)
        {
            buffer[0] = Char{'0'};
            buffer[1] = Char{'\0'};
        }
        return 2;
    }

    ::std::make_unsigned_t<UInt> uVal = static_cast<::std::make_unsigned_t<UInt>>(val);
    
    uSys writeIndex = 0;

    while(uVal != 0)
    {
        const UInt digit = uVal % 16;
        uVal /= 16;

        if(writeIndex < bufferSize)
        {
            buffer[writeIndex] = Uppercase ? GetNumberUpper<UInt, Char>(digit) : GetNumberLower<UInt, Char>(digit);
        }

        ++writeIndex;
    }

    if(writeIndex < bufferSize)
    {
        ReverseBuffer(buffer, writeIndex);
        buffer[writeIndex] = Char{'\0'};
    }
    else
    {
        buffer[0] = Char{'\0'};
    }
    
    return static_cast<i32>(writeIndex);
}

template<bool Uppercase, typename Int, typename Char, uSys BufferSize>
inline i32 XtoA(const Int val, Char(&buffer)[BufferSize])
{
    using UInt = ::std::make_unsigned_t<Int>;

    if(val == 0)
    {
        if constexpr(BufferSize >= 2)
        {
            buffer[0] = Char{'0'};
            buffer[1] = Char{'\0'};
        }
        return 2;
    }

    ::std::make_unsigned_t<UInt> uVal = static_cast<::std::make_unsigned_t<UInt>>(val);
    
    uSys writeIndex = 0;

    while(uVal != 0)
    {
        const UInt digit = uVal % 16;
        uVal /= 16;

        if(writeIndex < BufferSize)
        {
            buffer[writeIndex] = Uppercase ? GetNumberUpper<UInt, Char>(digit) : GetNumberLower<UInt, Char>(digit);
        }

        ++writeIndex;
    }

    if(writeIndex < BufferSize)
    {
        ReverseBuffer(buffer, writeIndex);
        buffer[writeIndex] = Char{'\0'};
    }
    else
    {
        buffer[0] = Char{'\0'};
    }
    
    return static_cast<i32>(writeIndex);
}

template<typename Int, typename Char, Char PadChar>
inline i32 ItoAP(Int val, Char* const buffer, const uSys bufferSize)
{
    constexpr uSys maxCharCount = MaxCharCount<Int>::Value;

    if(bufferSize < maxCharCount + 1)
    {
        return static_cast<i32>(maxCharCount);
    }

    if(val == 0)
    {
        for(uSys i = 0; i < maxCharCount - 1; ++i)
        {
            buffer[i] = PadChar;
        }
        buffer[maxCharCount - 1] = Char{'0'};
        buffer[maxCharCount] = Char{'\0'};
        return static_cast<i32>(maxCharCount);
    }

    bool negative = false;
    uSys writeIndex = 0;

    if constexpr(::std::is_signed_v<Int>)
    {
        if(val < 0)
        {
            if(val == IntMaxMin<Int>::Min)
            {
                if constexpr(::std::is_same_v<Int, i8>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'1'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'8'};
                    buffer[4] = Char{'\0'};
                    return 4;
                }
                else if constexpr(::std::is_same_v<Int, i16>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'3'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'7'};
                    buffer[4] = Char{'6'};
                    buffer[5] = Char{'8'};
                    buffer[6] = Char{'\0'};
                    return 6;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'2'};
                    buffer[2] = Char{'1'};
                    buffer[3] = Char{'4'};
                    buffer[4] = Char{'7'};
                    buffer[5] = Char{'4'};
                    buffer[6] = Char{'8'};
                    buffer[7] = Char{'3'};
                    buffer[8] = Char{'6'};
                    buffer[9] = Char{'4'};
                    buffer[10] = Char{'8'};
                    buffer[11] = Char{'\0'};
                    return 11;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'9'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'2'};
                    buffer[4] = Char{'3'};
                    buffer[5] = Char{'3'};
                    buffer[6] = Char{'7'};
                    buffer[7] = Char{'2'};
                    buffer[8] = Char{'0'};
                    buffer[9] = Char{'3'};
                    buffer[10] = Char{'6'};
                    buffer[11] = Char{'8'};
                    buffer[12] = Char{'5'};
                    buffer[13] = Char{'4'};
                    buffer[14] = Char{'7'};
                    buffer[15] = Char{'7'};
                    buffer[16] = Char{'5'};
                    buffer[17] = Char{'8'};
                    buffer[18] = Char{'0'};
                    buffer[19] = Char{'8'};
                    buffer[20] = Char{'\0'};
                    return 20;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                buffer[0] = Char{'-'};
                val = -val;
                writeIndex = 1;
                negative = true;
            }
        }
    }

    while(val != 0)
    {
        const Int digit = val % 10;
        val /= 10;
        
        buffer[writeIndex] = GetNumberUpper<Int, Char>(digit);

        ++writeIndex;
    }

    for(; writeIndex < maxCharCount; ++writeIndex)
    {
        buffer[writeIndex] = PadChar;
    }
    
    if(negative)
    {
        ReverseBuffer<MaxCharCount<Int>::Value - 1>(buffer + 1);
    }
    else
    {
        ReverseBuffer<MaxCharCount<Int>::Value>(buffer);
    }
    buffer[maxCharCount] = Char{'\0'};
    
    return static_cast<i32>(maxCharCount);
}

template<typename Int, typename Char, Char PadChar, uSys BufferSize>
inline i32 ItoAP(Int val, Char(&buffer)[BufferSize])
{
    constexpr uSys maxCharCount = MaxCharCount<Int>::Value;

    if constexpr(BufferSize < maxCharCount + 1)
    {
        return static_cast<i32>(maxCharCount);
    }

    if(val == 0)
    {
        for(uSys i = 0; i < maxCharCount - 1; ++i)
        {
            buffer[i] = PadChar;
        }
        buffer[maxCharCount - 1] = Char{'0'};
        buffer[maxCharCount] = Char{'\0'};
        return static_cast<i32>(maxCharCount);
    }

    bool negative = false;
    uSys writeIndex = 0;

    if constexpr(::std::is_signed_v<Int>)
    {
        if(val < 0)
        {
            if(val == IntMaxMin<Int>::Min)
            {
                if constexpr(::std::is_same_v<Int, i8>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'1'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'8'};
                    buffer[4] = Char{'\0'};
                    return 4;
                }
                else if constexpr(::std::is_same_v<Int, i16>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'3'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'7'};
                    buffer[4] = Char{'6'};
                    buffer[5] = Char{'8'};
                    buffer[6] = Char{'\0'};
                    return 6;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'2'};
                    buffer[2] = Char{'1'};
                    buffer[3] = Char{'4'};
                    buffer[4] = Char{'7'};
                    buffer[5] = Char{'4'};
                    buffer[6] = Char{'8'};
                    buffer[7] = Char{'3'};
                    buffer[8] = Char{'6'};
                    buffer[9] = Char{'4'};
                    buffer[10] = Char{'8'};
                    buffer[11] = Char{'\0'};
                    return 11;
                }
                else if constexpr(::std::is_same_v<Int, i32>)
                {
                    buffer[0] = Char{'-'};
                    buffer[1] = Char{'9'};
                    buffer[2] = Char{'2'};
                    buffer[3] = Char{'2'};
                    buffer[4] = Char{'3'};
                    buffer[5] = Char{'3'};
                    buffer[6] = Char{'7'};
                    buffer[7] = Char{'2'};
                    buffer[8] = Char{'0'};
                    buffer[9] = Char{'3'};
                    buffer[10] = Char{'6'};
                    buffer[11] = Char{'8'};
                    buffer[12] = Char{'5'};
                    buffer[13] = Char{'4'};
                    buffer[14] = Char{'7'};
                    buffer[15] = Char{'7'};
                    buffer[16] = Char{'5'};
                    buffer[17] = Char{'8'};
                    buffer[18] = Char{'0'};
                    buffer[19] = Char{'8'};
                    buffer[20] = Char{'\0'};
                    return 20;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                buffer[0] = Char{'-'};
                val = -val;
                writeIndex = 1;
                negative = true;
            }
        }
    }

    while(val != 0)
    {
        const Int digit = val % 10;
        val /= 10;
        
        buffer[writeIndex] = GetNumberUpper<Int, Char>(digit);

        ++writeIndex;
    }

    for(; writeIndex < maxCharCount; ++writeIndex)
    {
        buffer[writeIndex] = PadChar;
    }
    
    if(negative)
    {
        ReverseBuffer<Char, MaxCharCount<Int>::Value - 1>(buffer + 1);
    }
    else
    {
        ReverseBuffer<Char, MaxCharCount<Int>::Value>(buffer);
    }
    buffer[maxCharCount] = Char{'\0'};
    
    return static_cast<i32>(maxCharCount);
}

template<typename Int, typename Char>
inline i32 ItoAP0(const Int val, Char* const buffer, const uSys bufferSize) noexcept
{
    return ItoAP<Int, Char, '0'>(val, buffer, bufferSize);
}

template<typename Int, typename Char>
inline i32 ItoAPS(const Int val, Char* const buffer, const uSys bufferSize) noexcept
{
    return ItoAP<Int, Char, ' '>(val, buffer, bufferSize);
}

template<typename Int, typename Char, uSys BufferSize>
inline i32 ItoAP0(Int val, Char(&buffer)[BufferSize]) noexcept
{
    return ItoAP<Int, Char, '0', BufferSize>(val, buffer);
}

template<typename Int, typename Char, uSys BufferSize>
inline i32 ItoAPS(Int val, Char(&buffer)[BufferSize]) noexcept
{
    return ItoAP<Int, Char, ' ', BufferSize>(val, buffer);
}

template<bool Uppercase, typename Int, typename Char, Char PadChar = '0'>
inline i32 XtoAP(const Int val, Char* const buffer, const uSys bufferSize)
{
    using UInt = ::std::make_unsigned_t<Int>;

    constexpr uSys maxCharCount = sizeof(UInt) * 2;

    if(bufferSize < maxCharCount + 1)
    {
        return static_cast<i32>(maxCharCount);
    }

    if(val == 0)
    {
        for(uSys i = 0; i < maxCharCount - 1; ++i)
        {
            buffer[i] = PadChar;
        }
        buffer[maxCharCount - 1] = Char{'0'};
        buffer[maxCharCount] = Char{'\0'};
        return static_cast<i32>(maxCharCount);
    }
    
    ::std::make_unsigned_t<UInt> uVal = static_cast<::std::make_unsigned_t<UInt>>(val);
    
    uSys writeIndex = 0;

    while(uVal != 0)
    {
        const Int digit = uVal % 16;
        uVal /= 16;
        
        buffer[writeIndex] = Uppercase ? GetNumberUpper<UInt, Char>(digit) : GetNumberLower<UInt, Char>(digit);

        ++writeIndex;
    }

    for(; writeIndex < maxCharCount; ++writeIndex)
    {
        buffer[writeIndex] = PadChar;
    }

    ReverseBuffer<sizeof(UInt) * 2>(buffer);

    buffer[maxCharCount] = Char{'\0'};
    
    return static_cast<i32>(maxCharCount);
}

template<bool Uppercase, typename Int, typename Char, Char PadChar, uSys BufferSize>
inline i32 XtoAP(const Int val, Char(&buffer)[BufferSize])
{
    using UInt = ::std::make_unsigned_t<Int>;

    constexpr uSys maxCharCount = sizeof(UInt) * 2;

    if constexpr(BufferSize < maxCharCount + 1)
    {
        return static_cast<i32>(maxCharCount);
    }

    if(val == 0)
    {
        for(uSys i = 0; i < maxCharCount - 1; ++i)
        {
            buffer[i] = PadChar;
        }
        buffer[maxCharCount - 1] = Char{'0'};
        buffer[maxCharCount] = Char{'\0'};
        return static_cast<i32>(maxCharCount);
    }
    
    ::std::make_unsigned_t<UInt> uVal = static_cast<::std::make_unsigned_t<UInt>>(val);
    
    uSys writeIndex = 0;

    while(uVal != 0)
    {
        const Int digit = uVal % 16;
        uVal /= 16;
        
        buffer[writeIndex] = Uppercase ? GetNumberUpper<UInt, Char>(digit) : GetNumberLower<UInt, Char>(digit);

        ++writeIndex;
    }

    for(; writeIndex < maxCharCount; ++writeIndex)
    {
        buffer[writeIndex] = PadChar;
    }

    ReverseBuffer<Char, sizeof(UInt) * 2>(buffer);

    buffer[maxCharCount] = Char{'\0'};
    
    return static_cast<i32>(maxCharCount);
}

template<typename Int, typename Char>
inline i32 XtoAP0(const Int val, Char* const buffer, const uSys bufferSize) noexcept
{
    return XtoAP<Int, Char, '0'>(val, buffer, bufferSize);
}

template<typename Int, typename Char>
inline i32 XtoAPS(const Int val, Char* const buffer, const uSys bufferSize) noexcept
{
    return XtoAP<Int, Char, ' '>(val, buffer, bufferSize);
}

template<typename Int, typename Char, uSys BufferSize>
inline i32 XtoAP0(Int val, Char(&buffer)[BufferSize]) noexcept
{
    return XtoAP<Int, Char, '0', BufferSize>(val, buffer);
}

template<typename Int, typename Char, uSys BufferSize>
inline i32 XtoAPS(Int val, Char(&buffer)[BufferSize]) noexcept
{
    return XtoAP<Int, Char, ' ', BufferSize>(val, buffer);
}

template<typename Char>
inline i32 BtoA(const bool val, Char* const buffer, const uSys bufferSize) noexcept
{
    if(val)
    {
        if(bufferSize >= 4)
        {
            buffer[0] = Char{'t'};
            buffer[1] = Char{'r'};
            buffer[2] = Char{'u'};
            buffer[3] = Char{'e'};
        }
        return 4;
    }
    else
    {
        if(bufferSize >= 5)
        {
            buffer[0] = Char { 'f' };
            buffer[1] = Char { 'a' };
            buffer[2] = Char { 'l' };
            buffer[3] = Char { 's' };
            buffer[4] = Char { 'e' };
        }
        return 5;
    }
}

template<typename Char, uSys BufferSize>
inline i32 BtoA(const bool val, Char(&buffer)[BufferSize]) noexcept
{
    if(val)
    {
        if constexpr(BufferSize >= 4)
        {
            buffer[0] = Char { 't' };
            buffer[1] = Char { 'r' };
            buffer[2] = Char { 'u' };
            buffer[3] = Char { 'e' };
        }
        return 4;
    }
    else
    {
        if constexpr(BufferSize >= 5)
        {
            buffer[0] = Char { 'f' };
            buffer[1] = Char { 'a' };
            buffer[2] = Char { 'l' };
            buffer[3] = Char { 's' };
            buffer[4] = Char { 'e' };
        }
        return 5;
    }
}

template<typename Char>
inline i32 BtoAP(const bool val, Char* const buffer, const uSys bufferSize) noexcept
{
    if(val)
    {
        if(bufferSize >= 5)
        {
            buffer[0] = Char { ' ' };
            buffer[1] = Char { 't' };
            buffer[2] = Char { 'r' };
            buffer[3] = Char { 'u' };
            buffer[4] = Char { 'e' };
        }
    }
    else
    {
        if(bufferSize >= 5)
        {
            buffer[0] = Char { 'f' };
            buffer[1] = Char { 'a' };
            buffer[2] = Char { 'l' };
            buffer[3] = Char { 's' };
            buffer[4] = Char { 'e' };
        }
    }

    return 5;
}

template<typename Char, uSys BufferSize>
inline i32 BtoAP(const bool val, Char(&buffer)[BufferSize]) noexcept
{
    if(val)
    {
        if constexpr(BufferSize >= 5)
        {
            buffer[0] = Char { ' ' };
            buffer[1] = Char { 't' };
            buffer[2] = Char { 'r' };
            buffer[3] = Char { 'u' };
            buffer[4] = Char { 'e' };
        }
    }
    else
    {
        if constexpr(BufferSize >= 5)
        {
            buffer[0] = Char { 'f' };
            buffer[1] = Char { 'a' };
            buffer[2] = Char { 'l' };
            buffer[3] = Char { 's' };
            buffer[4] = Char { 'e' };
        }
    }

    return 5;
}

}
