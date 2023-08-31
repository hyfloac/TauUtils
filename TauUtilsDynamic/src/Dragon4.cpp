/******************************************************************************
  Copyright (c) 2014 Ryan Juckett
  http://www.ryanjuckett.com/

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software. -- It has been modified.

  3. This notice may not be removed or altered from any source
     distribution.
******************************************************************************/

#include "Dragon4.hpp"
#include "Objects.hpp"
#include "TUMaths.hpp"

#include <cmath>
#include <climits>
#include <cstring>
#include <immintrin.h>

#define BIG_INT_MAX_BLOCKS (35)

class BigInt final
{
    DEFAULT_DESTRUCT(BigInt);
public:
    constexpr BigInt() noexcept
        : m_Length(0)
        , m_Blocks{}
    { }

    constexpr BigInt(const BigInt& copy) noexcept
        : m_Length(copy.m_Length)
        , m_Blocks{}
    {
        for(iSys i = 0; i < copy.m_Length; ++i)
        {
            m_Blocks[i] = copy.m_Blocks[i];
        }
    }

    constexpr BigInt(BigInt&& move) noexcept
        : m_Length(move.m_Length)
        , m_Blocks{}
    {
        for(iSys i = 0; i < move.m_Length; ++i)
        {
            m_Blocks[i] = move.m_Blocks[i];
        }
    }

    constexpr BigInt& operator=(const BigInt& copy) noexcept
    {
        if(this == &copy)
        {
            return *this;
        }

        m_Length = copy.m_Length;

        for(iSys i = 0; i < copy.m_Length; ++i)
        {
            m_Blocks[i] = copy.m_Blocks[i];
        }

        return *this;
    }

    constexpr BigInt& operator=(BigInt&& move) noexcept
    {
        m_Length = move.m_Length;

        for(iSys i = 0; i < move.m_Length; ++i)
        {
            m_Blocks[i] = move.m_Blocks[i];
        }

        return *this;
    }

    [[nodiscard]] constexpr iSys Length() const noexcept { return m_Length; }
    [[nodiscard]] constexpr iSys Length64() const noexcept { return (m_Length + 1) / 2; }
    [[nodiscard]] constexpr u32 GetBlock(const uSys index) const noexcept { return m_Blocks[index]; }
    constexpr void SetZero() noexcept { m_Length = 0; }
    [[nodiscard]] constexpr bool IsZero() const noexcept { return m_Length == 0; }
    [[nodiscard]] constexpr u32 GetU32() const noexcept { return m_Length == 0 ? 0 : m_Blocks[0]; }

    constexpr void SetU32(const u32 val) noexcept
    {
        if(val == 0)
        {
            m_Length = 0;
        }
        else
        {
            m_Blocks[0] = val;
            m_Length = 1;
        }
    }

    constexpr void SetU64(const u64 val) noexcept
    {
        if(val == 0)
        {
            m_Length = 0;
        }
        else if(val > 0xFFFFFFFF)
        {
            m_Blocks[0] = static_cast<u32>(val & 0xFFFFFFFF);
            m_Blocks[1] = static_cast<u32>(val >> 32);
            m_Length = 2;
        }
        else
        {
            m_Blocks[0] = static_cast<u32>(val & 0xFFFFFFFF);
            m_Length = 1;
        }
    }
public:
    [[nodiscard]] static constexpr i32 CompareTo(const BigInt& left, const BigInt& right) noexcept
    {
        const iSys lengthDiff = left.m_Length - right.m_Length;
        if(lengthDiff != 0)
        {
            return lengthDiff < 0 ? -1 : 1;
        }

        for(iSys i = left.m_Length - 1; i >= 0; --i)
        {
            if(left.m_Blocks[i] > right.m_Blocks[i])
            {
                return 1;
            }
            else if(left.m_Blocks[i] < right.m_Blocks[i])
            {
                return -1;
            }
        }

        return 0;
    }

    static constexpr void Add(BigInt* const result, const BigInt& left, const BigInt& right) noexcept
    {
        const BigInt* large;
        const BigInt* small;
        if(left.m_Length < right.m_Length)
        {
            small = &left;
            large = &right;
        }
        else
        {
            small = &right;
            large = &left;
        }
#if 0
        unsigned char carry = 0;

        const iSys smallLength = small->Length64();
        const iSys largeLength = large->Length64();

        for(iSys i = 0; i < smallLength; ++i)
        {
            u64 sum;
            carry = _addcarry_u64(carry, large->m_Blocks64[i], small->m_Blocks64[i], &sum);
            result->m_Blocks64[i] = sum;
        }

        for(iSys i = smallLength; i < largeLength; ++i)
        {
            u64 sum;
            carry = _addcarry_u64(carry, large->m_Blocks64[i], 0, &sum);
            result->m_Blocks64[i] = sum;
        }

        if(carry != 0)
        {
            result->m_Blocks[large->m_Length] = 1;
            result->m_Length = large->m_Length + 1;
        }
        else
        {
            result->m_Length = large->m_Length;
        }
#else
        u64 carry = 0;

        for(iSys i = 0; i < small->m_Length; ++i)
        {
            const u64 sum = carry + static_cast<u64>(large->m_Blocks[i]) + static_cast<u64>(small->m_Blocks[i]);
            carry = sum >> 32;
            result->m_Blocks[i] = static_cast<u32>(sum & 0xFFFFFFFF);
        }

        for(iSys i = small->m_Length; i < large->m_Length; ++i)
        {
            const u64 sum = carry + static_cast<u64>(large->m_Blocks[i]);
            carry = sum >> 32;
            result->m_Blocks[i] = static_cast<u32>(sum & 0xFFFFFFFF);
        }

        if(carry != 0)
        {
            result->m_Blocks[large->m_Length] = 1;
            result->m_Length = large->m_Length + 1;
        }
        else
        {
            result->m_Length = large->m_Length;
        }
#endif
    }

    static constexpr void Mul(BigInt* const result, const BigInt& left, const BigInt& right) noexcept
    {
        const BigInt* small;
        const BigInt* large;
        if(left.m_Length < right.m_Length)
        {
            small = &left;
            large = &right;
        }
        else
        {
            small = &right;
            large = &left;
        }

        const i32 maxResultLen = left.m_Length + right.m_Length;

        for(iSys i = 0; i < maxResultLen; ++i)
        {
            result->m_Blocks[i] = 0;
        }

        for(iSys i = 0; i < small->m_Length; ++i)
        {
            const u32 multiplier = small->m_Blocks[i];
            if(multiplier != 0)
            {
                u64 carry = 0;

                for(iSys j = 0; j < large->m_Length; ++j)
                {
                    const u64 product = result->m_Blocks[j] + static_cast<u64>(large->m_Blocks[j]) * multiplier + carry;
                    carry = product >> 32;
                    result->m_Blocks[j] = static_cast<u32>(product & 0xFFFFFFFF);
                }

                result->m_Blocks[large->m_Length] = static_cast<u32>(carry & 0xFFFFFFFF);
            }
        }

        if(maxResultLen > 0 && result->m_Blocks[maxResultLen - 1] == 0)
        {
            result->m_Length = maxResultLen - 1;
        }
        else
        {
            result->m_Length = maxResultLen;
        }
    }
    
    static constexpr void Mul(BigInt* const result, const BigInt& left, const u32* const rightBlocks, const i32 rightLength) noexcept
    {
        const u32* smallBlocks;
        const u32* largeBlocks;
        u32 smallLength;
        u32 largeLength;
        if(left.m_Length < rightLength)
        {
            smallBlocks = left.m_Blocks;
            largeBlocks = rightBlocks;
            smallLength = left.m_Length;
            largeLength = rightLength;
        }
        else
        {
            smallBlocks = rightBlocks;
            largeBlocks = left.m_Blocks;
            smallLength = rightLength;
            largeLength = left.m_Length;
        }

        const i32 maxResultLen = left.m_Length + rightLength;

        for(iSys i = 0; i < maxResultLen; ++i)
        {
            result->m_Blocks[i] = 0;
        }

        for(iSys i = 0; i < smallLength; ++i)
        {
            const u32 multiplier = smallBlocks[i];
            if(multiplier != 0)
            {
                u64 carry = 0;

                for(iSys j = 0; j < largeLength; ++j)
                {
                    const u64 product = result->m_Blocks[j] + static_cast<u64>(largeBlocks[j]) * multiplier + carry;
                    carry = product >> 32;
                    result->m_Blocks[j] = static_cast<u32>(product & 0xFFFFFFFF);
                }

                result->m_Blocks[largeLength] = static_cast<u32>(carry & 0xFFFFFFFF);
            }
        }

        if(maxResultLen > 0 && result->m_Blocks[maxResultLen - 1] == 0)
        {
            result->m_Length = maxResultLen - 1;
        }
        else
        {
            result->m_Length = maxResultLen;
        }
    }

    static constexpr void Mul(BigInt* const result, const BigInt& left, const u32 right) noexcept
    {
        u64 carry = 0;
        for(iSys i = 0; i < left.m_Length; ++i)
        {
            const u64 product = static_cast<u64>(left.m_Blocks[i]) * right + carry;
            carry = product >> 32;
            result->m_Blocks[i] = static_cast<u32>(product & 0xFFFFFFFF);
        }

        if(carry != 0)
        {
            result->m_Blocks[left.m_Length] = static_cast<u32>(carry);
            result->m_Length = left.m_Length + 1;
        }
        else
        {
            result->m_Length = left.m_Length;
        }
    }

    static constexpr void Mul2(BigInt* const result, const BigInt& left) noexcept
    {
        u64 carry = 0;
        for(iSys i = 0; i < left.m_Length; ++i)
        {
            const u64 product = (static_cast<u64>(left.m_Blocks[i]) << 1) | carry;
            carry = product >> 31;
            result->m_Blocks[i] = static_cast<u32>(product & 0xFFFFFFFF);
        }

        if(carry != 0)
        {
            result->m_Blocks[left.m_Length] = static_cast<u32>(carry);
            result->m_Length = left.m_Length + 1;
        }
        else
        {
            result->m_Length = left.m_Length;
        }
    }

    static constexpr void Mul2(BigInt* const result) noexcept
    {
        u64 carry = 0;
        for(iSys i = 0; i < result->m_Length; ++i)
        {
            const u64 product = (static_cast<u64>(result->m_Blocks[i]) << 1) | carry;
            carry = product >> 31;
            result->m_Blocks[i] = static_cast<u32>(product & 0xFFFFFFFF);
        }

        if(carry != 0)
        {
            result->m_Blocks[result->m_Length] = static_cast<u32>(carry);
            ++result->m_Length;
        }
    }

    static constexpr void Mul10(BigInt* const result) noexcept
    {
        u64 carry = 0;
        for(iSys i = 0; i < result->m_Length; ++i)
        {
            const u64 product = static_cast<u64>(result->m_Blocks[i]) * 10 + carry;
            carry = product >> 32;
            result->m_Blocks[i] = static_cast<u32>(product & 0xFFFFFFFF);
        }

        if(carry != 0)
        {
            result->m_Blocks[result->m_Length] = static_cast<u32>(carry);
            ++result->m_Length;
        }
    }

    static constexpr void Pow10(BigInt* const result, u32 exponent) noexcept
    {
        BigInt tmp0;
        BigInt tmp1;
        BigInt* currTmp = &tmp0;
        BigInt* nextTmp = &tmp1;

        const u32 smallExp = exponent & 0x07;
        currTmp->SetU32(PowerOf10_U32[smallExp]);

        exponent >>= 3;

        for(u32 tableIndex = 0; exponent != 0; ++tableIndex, exponent >>= 1)
        {
            if(exponent & 1)
            {
                Mul(nextTmp, *currTmp, PowerOf10_Big + PowerOf10IndicesAndLengths_Big[tableIndex * 2 + 0], PowerOf10IndicesAndLengths_Big[tableIndex * 2 + 1]);

                BigInt* swap = currTmp;
                currTmp = nextTmp;
                nextTmp = swap;
            }
        }

        *result = *currTmp;
    }

    static constexpr void MulPow10(BigInt* result, const BigInt& in, u32 exponent) noexcept
    {
        BigInt tmp0;
        BigInt tmp1;
        BigInt* currTmp = &tmp0;
        BigInt* nextTmp = &tmp1;

        u32 smallExp = exponent & 0x07;
        if(smallExp == 0)
        {
            tmp0 = in;
        }
        else
        {
            Mul(currTmp, in, PowerOf10_U32[smallExp]);
        }

        exponent >>= 3;

        for(u32 tableIndex = 0; exponent != 0; ++tableIndex, exponent >>= 1)
        {
            if(exponent & 1)
            {
                Mul(nextTmp, *currTmp, PowerOf10_Big + PowerOf10IndicesAndLengths_Big[tableIndex * 2 + 0], PowerOf10IndicesAndLengths_Big[tableIndex * 2 + 1]);
                
                BigInt* swap = currTmp;
                currTmp = nextTmp;
                nextTmp = swap;
            }
        }

        *result = *currTmp;
    }

    static constexpr void Pow2(BigInt* result, const u32 exponent) noexcept
    {
        const i32 blockIndex = static_cast<i32>(exponent / 32);
        const i32 bitIndex = static_cast<i32>(exponent % 32);

        for(iSys i = 0; i <= blockIndex; ++i)
        {
            result->m_Blocks[i] = 0;
        }

        result->m_Length = blockIndex + 1;
        result->m_Blocks[blockIndex] |= 1 << bitIndex;
    }

    static constexpr u32 DivWithRem_MaxQuotient9(BigInt* dividend, const BigInt& divisor)
    {
        i32 length = divisor.m_Length;

        if(dividend->m_Length < length)
        { return 0; }
        
        u32 quotient = dividend->m_Blocks[length - 1] / (divisor.m_Blocks[length - 1] + 1);

        if(quotient != 0)
        {
            u64 borrow = 0;
            u64 carry = 0;

            for(iSys i = 0; i < divisor.m_Length; ++i)
            {
                const u64 product = static_cast<u64>(divisor.m_Blocks[i]) * static_cast<u64>(quotient) + carry;
                carry = product >> 32;

                const u64 difference = static_cast<u64>(dividend->m_Blocks[i]) - (product & 0xFFFFFFFF) - borrow;
                borrow = (difference >> 32) & 1;

                dividend->m_Blocks[i] = static_cast<u32>(difference & 0xFFFFFFFF);
            }
            
            // remove all leading zero blocks from dividend
            while(length > 0 && dividend->m_Blocks[length - 1] == 0)
            {
                --length;
            }

            dividend->m_Length = length;
        }

        // If the dividend is still larger than the divisor, we overshot our estimate quotient. To correct,
        // we increment the quotient and subtract one more divisor from the dividend.
        if(CompareTo(*dividend, divisor) >= 0)
        {
            ++quotient;

            // dividend = dividend - divisor
            u64 borrow = 0;
            for(iSys i = 0; i < divisor.m_Length; ++i)
            {
                const u64 difference = static_cast<u64>(dividend->m_Blocks[i]) - static_cast<u64>(divisor.m_Blocks[i]) - borrow;
                borrow = (difference >> 32) & 1;

                dividend->m_Blocks[i] = static_cast<u32>(difference & 0xFFFFFFFF);
            }
            
            // remove all leading zero blocks from dividend
            while(length > 0 && dividend->m_Blocks[length - 1] == 0)
            {
                --length;
            }

            dividend->m_Length = length;
        }

        return quotient;
    }

    static constexpr void ShiftLeft(BigInt* result, const i32 shift)
    {
        constexpr i32 BitCount = sizeof(u32) * CHAR_BIT;

        const i32 shiftBlocks = shift / BitCount;
        const i32 shiftBits = shift % BitCount;
        
        if(shiftBits == 0)
        {
            for(iSys i = result->m_Length - 1; i >= 0; --i)
            {
                result->m_Blocks[i + shiftBlocks] = result->m_Blocks[i]; 
            }

            for(iSys i = 0; i < shiftBlocks; ++i)
            {
                result->m_Blocks[i] = 0;
            }

            result->m_Length += shiftBlocks;
        }
        else
        {
            i32 inBlockIndex = result->m_Length - 1;
            i32 outBlockIndex = result->m_Length + shiftBlocks;

            result->m_Length = outBlockIndex + 1;

            const u32 lowBitsShift = 32 - shiftBits;
            u32 highBits = 0;
            u32 lowBits = 0;

            for(; inBlockIndex > 0; --inBlockIndex, --outBlockIndex)
            {
                const u32 block = result->m_Blocks[inBlockIndex];
                lowBits = block >> lowBitsShift;

                result->m_Blocks[outBlockIndex] = highBits | lowBits;
                highBits = block << shiftBits;
            }

            result->m_Blocks[outBlockIndex] = highBits | lowBits;
            result->m_Blocks[outBlockIndex - 1] = result->m_Blocks[inBlockIndex] << shiftBits;

            for(iSys i = 0; i < shiftBlocks; ++i)
            {
                result->m_Blocks[i] = 0;
            }

            if(result->m_Blocks[result->m_Length - 1] == 0)
            {
                --result->m_Length;
            }
        }
    }
private:
    constexpr static u32 PowerOf10_U32[] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000
    };
    constexpr static i32 PowerOf10IndicesAndLengths_Big[] = {
        0, 1,
        1, 2,
        3, 4,
        7, 7,
        14, 14,
        28, 27
    };
    constexpr static u32 PowerOf10_Big[] = {
        100000000,
        0x6fc10000, 0x002386f2,
        0x00000000, 0x85acef81, 0x2d6d415b, 0x000004ee,
        0x00000000, 0x00000000, 0xbf6a1f01, 0x6e38ed64, 0xdaa797ed, 0xe93ff9f4, 0x00184f03,

        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x2e953e01, 0x03df9909, 0x0f1538fd,
        0x2374e42f, 0xd3cff5ec, 0xc404dc08, 0xbccdb0da, 0xa6337f19, 0xe91f2603, 0x0000024e,

        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x982e7c01, 0xbed3875b, 0xd8d99f72, 0x12152f87, 0x6bde50c6, 0xcf4a6e70,
        0xd595d80f, 0x26b2716e, 0xadc666b0, 0x1d153624, 0x3c42d35a, 0x63ff540e, 0xcc5573c0,
        0x65f9ef17, 0x55bc28f2, 0x80dcc7f7, 0xf46eeddc, 0x5fdcefce, 0x000553f7
    };
private:
    i32 m_Length;
    union
    {
        u32 m_Blocks[BIG_INT_MAX_BLOCKS];
        u64 m_Blocks64[(BIG_INT_MAX_BLOCKS + 1) / 2];
    };
};

union FloatUnion32 final
{
    f32 f;
    u32 i;

    [[nodiscard]] bool IsNegative() const noexcept { return (i >> 31) != 0; }
    [[nodiscard]] u32 GetExponent() const noexcept { return (i >> 23) & 0xFF; }
    [[nodiscard]] u32 GetMantissa() const noexcept { return i & 0x7FFFFF; }
};

union FloatUnion64 final
{
    f64 f;
    u64 i;

    [[nodiscard]] bool IsNegative() const noexcept { return (i >> 63) != 0; }
    [[nodiscard]] u32 GetExponent() const noexcept { return (i >> 52) & 0x7FF; }
    [[nodiscard]] u64 GetMantissa() const noexcept { return i & 0xFFFFFFFFFFFFFull; }
};

//******************************************************************************
// This is an implementation the Dragon4 algorithm to convert a binary number
// in floating point format to a decimal number in string format. The function
// returns the number of digits written to the output buffer and the output is
// not NUL terminated.
//
// The floating point input value is (mantissa * 2^exponent).
//
// See the following papers for more information on the algorithm:
//  "How to Print Floating-Point Numbers Accurately"
//    Steele and White
//    http://kurtstephens.com/files/p372-steele.pdf
//  "Printing Floating-Point Numbers Quickly and Accurately"
//    Burger and Dybvig
//    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.72.4656&rep=rep1&type=pdf
//******************************************************************************
template<typename Char>
static constexpr uSys Dragon4T(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, u32 cutoffNumber, Char* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{
    if(mantissa == 0)
    {
        *outBuffer = Char{'0'};
        *outExponent = 0;
        return 1;
    }
        
    Char* currDigit = outBuffer;
    
    // compute the initial state in integral form such that 
    //  value     = scaledValue / scale
    //  marginLow = scaledMarginLow / scale
    BigInt scale;           // positive scale applied to value and margin such that they can be
                            //  represented as whole numbers
    BigInt scaledValue;	    // scale * mantissa
    BigInt scaledMarginLow;	// scale * 0.5 * (distance between this floating-point number and its
                            //  immediate lower value)
    
    // For normalized IEEE floating point values, each time the exponent is incremented the margin also
    // doubles. That creates a subset of transition numbers where the high margin is twice the size of
    // the low margin.
    BigInt* scaledMarginHigh;
    BigInt optionalMarginHigh;

    if(hasUnequalMargins)
    {
        // if we have no fractional component
        if(exponent > 0)
        {
            // 1) Expand the input value by multiplying out the mantissa and exponent. This represents
            //    the input value in its whole number representation.
            // 2) Apply an additional scale of 2 such that later comparisons against the margin values
            //    are simplified.
            // 3) Set the margin value to the lowest mantissa bit's scale.
            
            // scaledValue      = 2 * 2 * mantissa * 2**exponent
            scaledValue.SetU64(4 * mantissa);
            BigInt::ShiftLeft(&scaledValue, exponent);

            // scale = 2 * 2 * 1
            scale.SetU32(4);

            // scaledMarginLow = 2 * 2**(exponent-1)
            BigInt::Pow2(&scaledMarginLow, exponent);

            // scaledMarginHigh = 2 * 2 * 2**(exponent-1)
            BigInt::Pow2(&optionalMarginHigh, exponent + 1);
        }
        else // else we have a fractional exponent
        {
            // In order to track the mantissa data as an integer, we store it as is with a large scale
            
            // scaledValue = 2 * 2 * mantissa
            scaledValue.SetU64( 4 * mantissa);	
            
            // scale = 2 * 2 * 2**(-exponent)
            BigInt::Pow2(&scale, 2 - exponent);
            
            // scaledMarginLow = 2 * 2**(-1)
            scaledMarginLow.SetU32(1);
            
            // scaledMarginHigh = 2 * 2 * 2**(-1)
            optionalMarginHigh.SetU32(2);
        }

        // the high and low margins are different
        scaledMarginHigh = &optionalMarginHigh;
    }
    else
    {
        // if we have no fractional component
        if(exponent > 0)
        {
            // 1) Expand the input value by multiplying out the mantissa and exponent. This represents
            //    the input value in its whole number representation.
            // 2) Apply an additional scale of 2 such that later comparisons against the margin values
            //    are simplified.
            // 3) Set the margin value to the lowest mantissa bit's scale.
            
            // scaledValue = 2 * mantissa * 2**exponent
            scaledValue.SetU64(2 * mantissa);
            BigInt::ShiftLeft(&scaledValue, exponent);
            
            // scale = 2 * 1
            scale.SetU32(2);

            // scaledMarginLow = 2 * 2**(exponent-1)
            BigInt::Pow2(&scaledMarginLow, exponent);
        }
        // else we have a fractional exponent
        else
        {
            // In order to track the mantissa data as an integer, we store it as is with a large scale

            // scaledValue = 2 * mantissa
            scaledValue.SetU64(2 * mantissa);
            
            // scale = 2 * 2**(-exponent)
            BigInt::Pow2(&scale, 1 - exponent);

            // scaledMarginLow = 2 * 2**(-1)
            scaledMarginLow.SetU32(1);
        }
    
        // the high and low margins are equal
        scaledMarginHigh = &scaledMarginLow;
    }

    
    // Compute an estimate for digitExponent that will be correct or undershoot by one.
    // This optimization is based on the paper "Printing Floating-Point Numbers Quickly and Accurately"
    // by Burger and Dybvig http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.72.4656&rep=rep1&type=pdf
    // We perform an additional subtraction of 0.69 to increase the frequency of a failed estimate
    // because that lets us take a faster branch in the code. 0.69 is chosen because 0.69 + log10(2) is
    // less than one by a reasonable epsilon that will account for any floating point error.
    //
    // We want to set digitExponent to floor(log10(v)) + 1
    //  v = mantissa*2^exponent
    //  log2(v) = log2(mantissa) + exponent;
    //  log10(v) = log2(v) * log10(2)
    //  floor(log2(v)) = mantissaHighBitIdx + exponent;
    //  log10(v) - log10(2) < (mantissaHighBitIdx + exponent) * log10(2) <= log10(v)
    //  log10(v) < (mantissaHighBitIdx + exponent) * log10(2) + log10(2) <= log10(v) + log10(2)
    //  floor( log10(v) ) < ceil( (mantissaHighBitIdx + exponent) * log10(2) ) <= floor( log10(v) ) + 1
    constexpr f64 log10_2 = 0.30102999566398119521373889472449;
    i32 digitExponent = static_cast<i32>(::std::ceil(static_cast<f64>(static_cast<i32>(mantissaHighBitIndex) + exponent) * log10_2 - 0.69));
    
    // if the digit exponent is smaller than the smallest desired digit for fractional cutoff,
    // pull the digit back into legal range at which point we will round to the appropriate value.
    // Note that while our value for digitExponent is still an estimate, this is safe because it
    // only increases the number. This will either correct digitExponent to an accurate value or it
    // will clamp it above the accurate value.
    if(cutoffMode == CutoffMode::FractionLength && digitExponent <= -static_cast<i32>(cutoffNumber))
    {
        digitExponent = 1 - static_cast<i32>(cutoffNumber);
    }
    
    // Divide value by 10^digitExponent. 
    if(digitExponent > 0)
    {
        // The exponent is positive creating a division so we multiply up the scale.
        BigInt tmp;
        BigInt::MulPow10(&tmp, scale, digitExponent);
        scale = tmp;
    }
    else if(digitExponent < 0)
    {
        // The exponent is negative creating a multiplication so we multiply up the scaledValue,
        // scaledMarginLow and scaledMarginHigh.
        BigInt pow10;
        BigInt::Pow10(&pow10, -digitExponent);

        BigInt tmp;
        BigInt::Mul(&tmp, scaledValue, pow10);
        scaledValue = tmp;

        BigInt::Mul(&tmp, scaledMarginLow, pow10);
        scaledMarginLow = tmp;

        if(scaledMarginHigh != &scaledMarginLow)
        {
            BigInt::Mul2(scaledMarginHigh, scaledMarginLow);
        }
    }

    // If (value >= 1), our estimate for digitExponent was too low
    if(BigInt::CompareTo(scaledValue, scale) >= 0)
    {
        // The exponent estimate was incorrect.
        // Increment the exponent and don't perform the pre-multiply needed
        // for the first loop iteration.
        ++digitExponent;
    }
    else
    {
        // The exponent estimate was correct.
        // Multiply larger by the output base to prepare for the first loop iteration.
        BigInt::Mul10(&scaledValue);
        BigInt::Mul10(&scaledMarginLow);
        if(scaledMarginHigh != &scaledMarginLow)
        {
            BigInt::Mul2(scaledMarginHigh, scaledMarginLow);
        }
    }

    // Compute the cutoff exponent (the exponent of the final digit to print).
    // Default to the maximum size of the output buffer.
    i32 cutoffExponent = static_cast<i32>(digitExponent - bufferSize);
    switch(cutoffMode)
    {
        case CutoffMode::Unique: break;
        case CutoffMode::TotalLength:
        {
            // print cutoffNumber of digits or until we reach the buffer size
            const i32 desiredCutoffExponent = digitExponent - static_cast<i32>(cutoffNumber);
            if(desiredCutoffExponent > cutoffExponent)
            {
                cutoffExponent = desiredCutoffExponent;
            }
            break;
        }
        case CutoffMode::FractionLength:
        {
            // print cutoffNumber digits past the decimal point or until we reach the buffer size
            const i32 desiredCutoffExponent = -static_cast<i32>(cutoffNumber);
            if(desiredCutoffExponent > cutoffExponent)
            {
                cutoffExponent = desiredCutoffExponent;
            }
            break;
        }
    }
    
    // Output the exponent of the first digit we will print
    *outExponent = digitExponent - 1;

    // In preparation for calling BigInt::DivWithRem_MaxQuotient9(), 
    // we need to scale up our values such that the highest block of the denominator
    // is greater than or equal to 8. We also need to guarantee that the numerator
    // can never have a length greater than the denominator after each loop iteration.
    // This requires the highest block of the denominator to be less than or equal to
    // 429496729 which is the highest number that can be multiplied by 10 without
    // overflowing to a new block.
    const u32 highBlock = scale.GetBlock(scale.Length() - 1);

    if(highBlock < 8 || highBlock > 0x19999999)
    {
        // Perform a bit shift on all values to get the highest block of the denominator into
        // the range [8,429496729]. We are more likely to make accurate quotient estimations
        // in BigInt_DivideWithRemainder_MaxQuotient9() with higher denominator values so
        // we shift the denominator to place the highest bit at index 27 of the highest block.
        // This is safe because (2^28 - 1) = 268435455 which is less than 429496729. This means
        // that all values with a highest bit at index 27 are within range.			
        const u32 highBlockLog2 = log2i(highBlock);
        const i32 shift = static_cast<i32>(32 + 27 - highBlockLog2) % 32;

        BigInt::ShiftLeft(&scale, shift);
        BigInt::ShiftLeft(&scaledValue, shift);
        BigInt::ShiftLeft(&scaledMarginLow, shift);

        if(scaledMarginHigh != &scaledMarginLow)
        {
            BigInt::Mul2(scaledMarginHigh, scaledMarginLow);
        }
    }

    // These values are used to inspect why the print loop terminated so we can properly
    // round the final digit.
    bool low; // did the value get within marginLow distance from zero
    bool high; // did the value get within marginHigh distance from one
    u32 outDigit; // current digit being output

    if(cutoffMode == CutoffMode::Unique)
    {
        // For the unique cutoff mode, we will try to print until we have reached a level of
        // precision that uniquely distinguishes this value from its neighbors. If we run
        // out of space in the output buffer, we terminate early.
        while(true)
        {
            --digitExponent;
            
            // divide out the scale to extract the digit
            outDigit = BigInt::DivWithRem_MaxQuotient9(&scaledValue, scale);
            
            // update the high end of the value
            BigInt scaledValueHigh;
            BigInt::Add(&scaledValueHigh, scaledValue, *scaledMarginHigh);
            
            // stop looping if we are far enough away from our neighboring values
            // or if we have reached the cutoff digit
            low = BigInt::CompareTo(scaledValue, scaledMarginLow) < 0;
            high = BigInt::CompareTo(scaledValueHigh, scale) > 0;

            if(low || high || digitExponent == cutoffExponent)
            {
                break;
            }
            
            // store the output digit
            *currDigit = static_cast<Char>(Char{'0'} + outDigit);
            ++currDigit;
            
            // multiply larger by the output base
            BigInt::Mul10(&scaledValue);
            BigInt::Mul10(&scaledMarginLow);
            if(scaledMarginHigh != &scaledMarginLow)
            {
                BigInt::Mul2(scaledMarginHigh, scaledMarginLow);
            }
        }
    }
    else
    {
        // For length based cutoff modes, we will try to print until we
        // have exhausted all precision (i.e. all remaining digits are zeros) or
        // until we reach the desired cutoff digit.
        low = false;
        high = false;

        while(true)
        {
            --digitExponent;
            
            
            // divide out the scale to extract the digit
            outDigit = BigInt::DivWithRem_MaxQuotient9(&scaledValue, scale);

            if(scaledValue.IsZero() || digitExponent == cutoffExponent)
            {
                break;
            }

            // store the output digit
            *currDigit = static_cast<Char>(Char{'0'} + outDigit);
            ++currDigit;
            
            // multiply larger by the output base
            BigInt::Mul10(&scaledValue);
        }
    }

    bool roundDown = low;
    
    // if it is legal to round up and down
    if(low == high)
    {
        // round to the closest digit by comparing value with 0.5. To do this we need to convert
        // the inequality to large integer values.
        //  compare( value, 0.5 )
        //  compare( scale * value, scale * 0.5 )
        //  compare( 2 * scale * value, scale )
        BigInt::Mul2(&scaledValue);
        const i32 compare = BigInt::CompareTo(scaledValue, scale);
        roundDown = compare < 0;

        // if we are directly in the middle, round towards the even digit (i.e. IEEE rouding rules)
        if(compare == 0)
        {
            roundDown = (outDigit & 1) == 0;
        }
    }

    // print the rounded digit
    if(roundDown)
    {
        *currDigit = static_cast<Char>(Char{'0'} + outDigit);
        ++currDigit;
    }
    else
    {
        // handle rounding up
        if(outDigit == 9)
        {
            // find the first non-nine prior digit
            while(true)
            {
                // if we are at the first digit
                if(currDigit == outBuffer)
                {
                    *currDigit = Char{'1'};
                    ++currDigit;
                    ++(*outExponent);
                    break;
                }

                --currDigit;
                if(*currDigit != Char{'9'})
                {
                    ++(*currDigit);
                    ++currDigit;
                    break;
                }
            }
        }
        else
        {
            // values in the range [0,8] can perform a simple round up
            *currDigit = static_cast<Char>(Char{'0'} + outDigit + 1);
            ++currDigit;
        }
    }

    return static_cast<u32>(currDigit - outBuffer);
}

TAU_LIB constexpr uSys Dragon4(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, const u32 cutoffNumber, char* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{ return Dragon4T(mantissa,exponent, mantissaHighBitIndex, hasUnequalMargins, cutoffMode, cutoffNumber, outBuffer, bufferSize, outExponent); }
TAU_LIB constexpr uSys Dragon4(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, const u32 cutoffNumber, wchar_t* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{ return Dragon4T(mantissa,exponent, mantissaHighBitIndex, hasUnequalMargins, cutoffMode, cutoffNumber, outBuffer, bufferSize, outExponent); }
TAU_LIB constexpr uSys Dragon4(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, const u32 cutoffNumber, c8* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{ return Dragon4T(mantissa,exponent, mantissaHighBitIndex, hasUnequalMargins, cutoffMode, cutoffNumber, outBuffer, bufferSize, outExponent); }
TAU_LIB constexpr uSys Dragon4(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, const u32 cutoffNumber, c16* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{ return Dragon4T(mantissa,exponent, mantissaHighBitIndex, hasUnequalMargins, cutoffMode, cutoffNumber, outBuffer, bufferSize, outExponent); }
TAU_LIB constexpr uSys Dragon4(const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const CutoffMode cutoffMode, const u32 cutoffNumber, c32* const outBuffer, const uSys bufferSize, i32* const outExponent) noexcept
{ return Dragon4T(mantissa,exponent, mantissaHighBitIndex, hasUnequalMargins, cutoffMode, cutoffNumber, outBuffer, bufferSize, outExponent); }

template<typename Char>
static constexpr uSys FormatPositional(Char* const outBuffer, const uSys bufferSize, const u64 mantissa, const i32 exponent, const i32 mantissaHighBitIndex, const bool hasUnequalMargins, const i32 precision) noexcept
{
    i32 printExponent;
    uSys numPrintDigits;

    uSys maxPrintLen = bufferSize - 1;

    if(precision < 0)
    {
        numPrintDigits = Dragon4T(mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, CutoffMode::Unique, 0, outBuffer, maxPrintLen, &printExponent);
    }
    else
    {
        numPrintDigits = Dragon4T(mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, CutoffMode::FractionLength, precision, outBuffer, maxPrintLen, &printExponent);
    }

	// Track the number of digits past the decimal point that have been printed.
	uSys numFractionDigits = 0;

    // If output has a whole number.
    if(printExponent >= 0)
    {
        // Leave the whole number at the start of the buffer.
		uSys numWholeDigits = printExponent + 1;
        if(numPrintDigits < numWholeDigits)
        {
            if(numWholeDigits > maxPrintLen)
            {
                numWholeDigits = maxPrintLen;
            }

            for(; numPrintDigits < numWholeDigits; ++numPrintDigits)
            {
                outBuffer[numPrintDigits] = Char{'0'};
            }
        }
        else if(numPrintDigits > numWholeDigits) // Insert the decimal point prior to the fraction.
        {
            numFractionDigits = numPrintDigits - numWholeDigits;
            const uSys maxFractionDigits = maxPrintLen - numWholeDigits - 1;
            if(numFractionDigits > maxFractionDigits)
            {
                numFractionDigits = maxFractionDigits;
            }

            ::std::memmove(outBuffer + numWholeDigits + 1, outBuffer + numWholeDigits, numFractionDigits * sizeof(Char));
            outBuffer[numWholeDigits] = Char{'.'};
            numPrintDigits = numWholeDigits + numFractionDigits + 1;
        }
    }
    else
    {
        // Shift out the fraction to make room for the leading zeros.
		if(maxPrintLen > 2)
		{
		    uSys numFractionZeros = static_cast<uSys>(-(printExponent + 1));
            const uSys maxFractionZeros = maxPrintLen - 2;
            if(numFractionZeros > maxFractionZeros)
            {
                numFractionZeros = maxFractionZeros;
            }

            uSys digitsStartIndex = 2 + numFractionZeros;

            // Shift the significant digits right such that there is room for leading zeros.
			numFractionDigits = numPrintDigits;
            const uSys maxFractionDigits = maxPrintLen - digitsStartIndex;
            if(numFractionDigits > maxFractionDigits)
            {
                numFractionDigits = maxFractionDigits;
            }

            ::std::memmove(outBuffer + digitsStartIndex, outBuffer, numFractionDigits * sizeof(Char));

            for(uSys i = 2; i < digitsStartIndex; ++i)
            {
                outBuffer[i] = Char{'0'};
            }

            numFractionDigits += numFractionZeros;
            numPrintDigits = numFractionDigits;
		}

        if(maxPrintLen > 1)
        {
            outBuffer[1] = Char{'.'};
            ++numPrintDigits;
        }

        if(maxPrintLen > 0)
        {
            outBuffer[0] = Char{'0'};
            ++numPrintDigits;
        }
    }

    // Add trailing zeros up to precision length.
	if(precision > static_cast<i32>(numFractionDigits) && numPrintDigits < maxPrintLen)
    {
        // Add a decimal point if this is the first fractional digit we are printing.
		if(numFractionDigits == 0)
		{
		    outBuffer[numPrintDigits++] = Char{'.'};
		}

        // Compute the number of trailing zeros needed.
        uSys totalDigits = numPrintDigits + (precision - numFractionDigits);
        if(totalDigits > maxPrintLen)
        {
            totalDigits = maxPrintLen;
        }

        for(; numPrintDigits < totalDigits; ++numPrintDigits)
        {
            outBuffer[numPrintDigits] = Char{'0'};
        }
    }

    outBuffer[numPrintDigits] = Char{'\0'};

    return numPrintDigits;
}

template<typename Char>
static constexpr uSys FormatScientific(Char* const outBuffer, const uSys bufferSize, const u64 mantissa, const i32 exponent, const u32 mantissaHighBitIndex, const bool hasUnequalMargins, const i32 precision) noexcept
{
    i32 printExponent;
    uSys numPrintDigits;

    uSys maxPrintLen = bufferSize - 1;

    if(precision < 0)
    {
        numPrintDigits = Dragon4T(mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, CutoffMode::Unique, 0, outBuffer, maxPrintLen, &printExponent);
    }
    else
    {
        numPrintDigits = Dragon4T(mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, CutoffMode::FractionLength, precision, outBuffer, maxPrintLen, &printExponent);
    }

    uSys writeIndex = 0;
    
    if(bufferSize > 1)
    {
        ++writeIndex;
    }

	// Track the number of digits past the decimal point that have been printed.
	uSys numFractionDigits = numPrintDigits - 1;

    if(numFractionDigits > 0 && bufferSize > 1)
    {
        const uSys maxFractionDigits = bufferSize - 2;
        if(numFractionDigits > maxFractionDigits)
        {
            numFractionDigits = maxFractionDigits;
        }

        (void) ::std::memmove(outBuffer + writeIndex + 1, outBuffer + writeIndex, numFractionDigits * sizeof(Char));
        outBuffer[1] = '.';
        writeIndex += numFractionDigits + 1;
    }
    
    // Add trailing zeros up to precision length.
	if(precision > static_cast<i32>(numFractionDigits) && bufferSize > 1)
    {
        // Add a decimal point if this is the first fractional digit we are printing.
		if(numFractionDigits == 0)
		{
		    outBuffer[writeIndex++] = Char{'.'};
		}

        // Compute the number of trailing zeros needed.
        uSys numZeros = precision - numFractionDigits;
        if(numZeros > bufferSize - 1)
        {
            numZeros = bufferSize - 1;
        }

        for(uSys i = 0; i < numZeros; ++i)
        {
            outBuffer[i + writeIndex] = Char{'0'};
        }
        writeIndex += numZeros;
    }

    // print the exponent into a local buffer and copy into output buffer
	if(bufferSize > 1)
    {
        Char expBuffer[5];
        expBuffer[0] = Char{'e'};

        if(printExponent >= 0)
        {
            expBuffer[1] = Char{'+'};
        }
        else
        {
            expBuffer[1] = Char{'-'};
            printExponent = -printExponent;
        }

        const u32 hundredsPlace = printExponent / 100;
        const u32 tensPlace = (printExponent - hundredsPlace * 100) / 10;
        const u32 onesPlace = (printExponent - hundredsPlace * 100 - tensPlace * 10);

        u32 expIndex = 2;

        if(hundredsPlace)
        {
            expBuffer[expIndex++] = static_cast<Char>(Char{'0'} + hundredsPlace);
        }
        if(hundredsPlace || tensPlace)
        {
            expBuffer[expIndex++] = static_cast<Char>(Char{'0'} + tensPlace);
        }
        expBuffer[expIndex++] = static_cast<Char>(Char{'0'} + onesPlace);

        const uSys maxExponentSize = bufferSize - 1;
        const uSys exponentSize = maxExponentSize > expIndex ? expIndex : maxExponentSize;
        ::std::memcpy(outBuffer + writeIndex, expBuffer, exponentSize * sizeof(Char));

        writeIndex += exponentSize;
    }

    outBuffer[writeIndex] = Char{'\0'};

    return writeIndex;
}

template<typename Char>
static constexpr uSys PrintInfNaN(Char* const outBuffer, const uSys bufferSize, const u64 mantissa) noexcept
{
    const uSys printLen = bufferSize - 1 > 3 ? 3 : bufferSize - 1;
    if(mantissa == 0)
    {
        switch(printLen - 1)
        {
            case 2: outBuffer[2] = Char{'f'};
            case 1: outBuffer[1] = Char{'n'};
            case 0: outBuffer[0] = Char{'I'};
            default: break;
        }
        outBuffer[printLen] = Char{'\0'};
    }
    else
    {
        switch(printLen - 1)
        {
            case 2: outBuffer[2] = Char{'N'};
            case 1: outBuffer[1] = Char{'a'};
            case 0: outBuffer[0] = Char{'N'};
            default: break;
        }
        outBuffer[printLen] = Char{'\0'};
    }
    return printLen;
}

template<typename Char>
static constexpr uSys PrintFloat32T(Char* outBuffer, uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{
    if(bufferSize == 0)
    {
        return 0;
    }

    if(bufferSize == 1)
    {
        outBuffer[0] = Char{'\0'};
        return 0;
    }

    FloatUnion32 fUnion;
    fUnion.f = value;
    const u32 floatExponent = fUnion.GetExponent();
    const u32 floatMantissa = fUnion.GetMantissa();
    u32 prefixLength = 0;

    if(fUnion.IsNegative())
    {
        outBuffer[0] = Char{'-'};
        ++outBuffer;
        --bufferSize;
        prefixLength = 1;
    }

    if(floatExponent == 0xFF)
    {
        return PrintInfNaN(outBuffer, bufferSize, floatMantissa) + prefixLength;
    }
    else
    {
        u32 mantissa;
        i32 exponent;
        u32 mantissaHighBitIndex;
        bool hasUnequalMargins;

        if(floatExponent != 0)
        {
            // normalized
			// The floating point equation is:
			//  value = (1 + mantissa/2^23) * 2 ^ (exponent-127)
			// We convert the integer equation by factoring a 2^23 out of the exponent
			//  value = (1 + mantissa/2^23) * 2^23 * 2 ^ (exponent-127-23)
			//  value = (2^23 + mantissa) * 2 ^ (exponent-127-23)
			// Because of the implied 1 in front of the mantissa we have 24 bits of precision.
			//   m = (2^23 + mantissa)
			//   e = (exponent-127-23)
			mantissa = (1 << 23) | floatMantissa;
            exponent = static_cast<i32>(floatExponent) - 127 - 23;
            mantissaHighBitIndex = 23;
            hasUnequalMargins = floatExponent != 1 && floatMantissa == 0;
        }
        else
        {
            // denormalized
			// The floating point equation is:
			//  value = (mantissa/2^23) * 2 ^ (1-127)
			// We convert the integer equation by factoring a 2^23 out of the exponent
			//  value = (mantissa/2^23) * 2^23 * 2 ^ (1-127-23)
			//  value = mantissa * 2 ^ (1-127-23)
			// We have up to 23 bits of precision.
			//   m = (mantissa)
			//   e = (1-127-23)
			mantissa = floatMantissa;
            exponent = 1 - 127 - 23;
            mantissaHighBitIndex = log2i(mantissa);
            hasUnequalMargins = false;
        }

        switch(format)
        {
            case PrintFloatFormat::Positional: return FormatPositional(outBuffer, bufferSize, mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, precision) + prefixLength;
            case PrintFloatFormat::Scientific: return FormatScientific(outBuffer, bufferSize, mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, precision) + prefixLength;
            default:
                outBuffer[0] = Char{'\0'};
                return 0;
        }
    }
}

template<typename Char>
static constexpr uSys PrintFloat64T(Char* outBuffer, uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{
    if(bufferSize == 0)
    {
        return 0;
    }

    if(bufferSize == 1)
    {
        outBuffer[0] = Char{'\0'};
        return 0;
    }

    FloatUnion64 fUnion;
    fUnion.f = value;
    const u32 floatExponent = fUnion.GetExponent();
    const u64 floatMantissa = fUnion.GetMantissa();
    u32 prefixLength = 0;

    if(fUnion.IsNegative())
    {
        outBuffer[0] = Char{'-'};
        ++outBuffer;
        --bufferSize;
        prefixLength = 1;
    }

    if(floatExponent == 0x7FF)
    {
        return PrintInfNaN(outBuffer, bufferSize, floatMantissa) + prefixLength;
    }
    else
    {
        u64 mantissa;
        i32 exponent;
        u32 mantissaHighBitIndex;
        bool hasUnequalMargins;

        if(floatExponent != 0)
        {
            // normal
			// The floating point equation is:
			//  value = (1 + mantissa/2^52) * 2 ^ (exponent-1023)
			// We convert the integer equation by factoring a 2^52 out of the exponent
			//  value = (1 + mantissa/2^52) * 2^52 * 2 ^ (exponent-1023-52)
			//  value = (2^52 + mantissa) * 2 ^ (exponent-1023-52)
			// Because of the implied 1 in front of the mantissa we have 53 bits of precision.
			//   m = (2^52 + mantissa)
			//   e = (exponent-1023+1-53)
			mantissa = (1ull << 52) | floatMantissa;
            exponent = static_cast<i32>(floatExponent) - 1023 - 52;
            mantissaHighBitIndex = 52;
            hasUnequalMargins = floatExponent != 1 && floatMantissa == 0;
        }
        else
        {
            // subnormal
			// The floating point equation is:
			//  value = (mantissa/2^52) * 2 ^ (1-1023)
			// We convert the integer equation by factoring a 2^52 out of the exponent
			//  value = (mantissa/2^52) * 2^52 * 2 ^ (1-1023-52)
			//  value = mantissa * 2 ^ (1-1023-52)
			// We have up to 52 bits of precision.
			//   m = (mantissa)
			//   e = (1-1023-52)
			mantissa = floatMantissa;
            exponent = 1 - 1023 - 52;
            mantissaHighBitIndex = log2i(mantissa);
            hasUnequalMargins = false;
        }

        switch(format)
        {
            case PrintFloatFormat::Positional: return FormatPositional(outBuffer, bufferSize, mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, precision) + prefixLength;
            case PrintFloatFormat::Scientific: return FormatScientific(outBuffer, bufferSize, mantissa, exponent, mantissaHighBitIndex, hasUnequalMargins, precision) + prefixLength;
            default:
                outBuffer[0] = Char{'\0'};
                return 0;
        }
    }
}

TAU_LIB constexpr uSys PrintFloat32(char* const outBuffer, const uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat32T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat32(wchar_t* const outBuffer, const uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat32T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat32(c8* const outBuffer, const uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat32T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat32(c16* const outBuffer, const uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat32T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat32(c32* const outBuffer, const uSys bufferSize, const f32 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat32T(outBuffer, bufferSize, value, format, precision); }

TAU_LIB constexpr uSys PrintFloat64(char* const outBuffer, const uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat64T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat64(wchar_t* const outBuffer, const uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat64T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat64(c8* const outBuffer, const uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat64T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat64(c16* const outBuffer, const uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat64T(outBuffer, bufferSize, value, format, precision); }
TAU_LIB constexpr uSys PrintFloat64(c32* const outBuffer, const uSys bufferSize, const f64 value, const PrintFloatFormat format, const i32 precision) noexcept
{ return PrintFloat64T(outBuffer, bufferSize, value, format, precision); }