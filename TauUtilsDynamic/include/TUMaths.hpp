// ReSharper disable CppClangTidyClangDiagnosticSignCompare
#pragma once

#include "NumTypes.hpp"

/**
 * Equal to pi/180.
 */
#define RADIANS_TO_DEGREES_CONVERTER_VAL 57.29577951308232087679815481410517
 /**
  * Equal to 180/pi
  */
#define DEGREES_TO_RADIANS_CONVERTER_VAL 0.017453292519943295769236907684886

  /**
   * Converts radians to degrees (single precision).
   */
#define RAD_2_DEG_F(F) (float)  ((F) * RADIANS_TO_DEGREES_CONVERTER_VAL)
   /**
    * Converts degrees to radians (single precision).
    */
#define DEG_2_RAD_F(F) (float)  ((F) * DEGREES_TO_RADIANS_CONVERTER_VAL)
    /**
     * Converts radians to degrees (double precision).
     */
#define RAD_2_DEG_D(D) (double) ((D) * RADIANS_TO_DEGREES_CONVERTER_VAL)
     /**
      * Converts degrees to radians (double precision).
      */
#define DEG_2_RAD_D(D) (double) ((D) * DEGREES_TO_RADIANS_CONVERTER_VAL)

#define RAD_2_DEG(F) RAD_2_DEG_F(F)
#define DEG_2_RAD(F) DEG_2_RAD_F(F)

template<typename T>
[[nodiscard]] inline constexpr T minT(const T a, const T b) noexcept
{ return a < b ? a : b; }

template<typename T>
[[nodiscard]] inline constexpr T minT(const T a, const T b, const T c) noexcept
{ return minT(minT(a, b), c); }

template<typename T>
[[nodiscard]] inline constexpr T minT(const T a, const T b, const T c, const T d) noexcept
{ return minT(minT(a, b), minT(c, d)); }

template<typename T>
[[nodiscard]] inline constexpr T maxT(const T a, const T b) noexcept
{ return a > b ? a : b; }

template<typename T>
[[nodiscard]] inline constexpr T maxT(const T a, const T b, const T c) noexcept
{ return maxT(maxT(a, b), c); }

template<typename T>
[[nodiscard]] inline constexpr T maxT(const T a, const T b, const T c, const T d) noexcept
{ return maxT(maxT(a, b), maxT(c, d)); }

template<typename T0, typename T1>
[[nodiscard]] inline constexpr T0 minT(const T0 a, const T1 b) noexcept
{ return a < b ? a : b; }

template<typename T0, typename T1, typename T2>
[[nodiscard]] inline constexpr T0 minT(const T0 a, const T1 b, const T2 c) noexcept
{ return minT(minT(a, b), c); }

template<typename T0, typename T1, typename T2, typename T3>
[[nodiscard]] inline constexpr T0 minT(const T0 a, const T1 b, const T2 c, const T3 d) noexcept
{ return minT(minT(a, b), minT(c, d)); }

template<typename T0, typename T1>
[[nodiscard]] inline constexpr T0 maxT(const T0 a, const T1 b) noexcept
{ return a > b ? a : b; }

template<typename T0, typename T1, typename T2>
[[nodiscard]] inline constexpr T0 maxT(const T0 a, const T1 b, const T2 c) noexcept
{ return maxT(maxT(a, b), c); }

template<typename T0, typename T1, typename T2, typename T3>
[[nodiscard]] inline constexpr T0 maxT(const T0 a, const T1 b, const T2 c, const T3 d) noexcept
{ return maxT(maxT(a, b), maxT(c, d)); }

/**
 * Relative epsilon equals function.
 */
template<typename T>
static inline bool rEpsilonEquals(const T x, const T y, const T epsilon = 1E-5)
{
    const T epsilonRelative = maxT(fabs(x), fabs(y)) * epsilon;
    return fabs(x - y) <= epsilonRelative;
}

/**
 * Absolute epsilon equals function.
 */
template<typename T>
static inline bool aEpsilonEquals(const T x, const T y, const T epsilon = 1E-5)
{ return fabs(x - y) <= epsilon; }

constexpr u32 clzTab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31 };

constexpr u32 clzTab64[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5 };

constexpr u32 ctzTab32[32] = {
     0,  1, 28,  2, 29, 14, 24,  3,
    30, 22, 20, 15, 25, 17,  4,  8, 
    31, 27, 13, 23, 21, 19, 16,  7,
    26, 12, 18,  6, 11,  5, 10,  9
};

constexpr u32 ctzTab64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

template<typename T>
[[nodiscard]] constexpr inline T AlignTo(const T val, const T alignment) noexcept
{
    if(alignment == 1 || val % alignment == 0)
    { return val; }
    return (val + alignment) & ~(alignment - 1);
}

template<typename Tv, typename Ta>
[[nodiscard]] constexpr inline Tv AlignTo(const Tv val, const Ta _alignment) noexcept
{
    const Tv alignment = static_cast<Tv>(_alignment);

    if(alignment == 1 || val % alignment == 0)
    { return val; }
    return (val + alignment) & ~(alignment - 1);
}

template<typename T, T Alignment>
[[nodiscard]] constexpr inline T AlignTo(const T val) noexcept
{
    if constexpr(Alignment == 1)
    { return val; }
    if(val % Alignment == 0)
    { return val; }
    return (val + Alignment) & ~(Alignment - 1);
}

template<typename Tv, typename Ta, Ta Alignment>
[[nodiscard]] constexpr inline Tv AlignTo(const Tv val) noexcept
{
    constexpr Tv alignment = static_cast<Tv>(Alignment);

    if constexpr(alignment == 1)
    { return val; }
    if(val % alignment == 0)
    { return val; }
    return (val + alignment) & ~(alignment - 1);
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

[[nodiscard]] inline u32 _ctz(const u32 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward(&trailingZero, v);
    return trailingZero;
}

[[nodiscard]] inline u32 _clz(const u32 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse(&leadingZero, v);
    return 31 - leadingZero;
}

[[nodiscard]] inline u64 _ctz(const u64 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward64(&trailingZero, v);
    return trailingZero;
}

[[nodiscard]] inline u64 _clz(const u64 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse64(&leadingZero, v);
    return 63 - leadingZero;
}

[[nodiscard]] constexpr inline u32 _ctzC(const u32 v) noexcept
{
    return ctzTab32[((v ^ (v - 1)) * 0x077CB531u) >> 27];
}

[[nodiscard]] constexpr inline u32 _clzC(u32 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return 63 - clzTab32[((v * 0x07C4ACDDu)) >> 27];
}

[[nodiscard]] constexpr inline u32 _ctzC(const u64 v) noexcept
{
    return ctzTab64[((v ^ (v - 1)) * 0x03F79D71B4CB0A89u) >> 58];
}

[[nodiscard]] constexpr inline u32 _clzC(u64 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return 63 - clzTab64[((v - (v >> 1)) * 0x07EDD5E59A4E28C2ull) >> 58];
}
#else
[[nodiscard]] inline u32 _ctz(const u32 v) noexcept
{ return __builtin_ctz(v); }

[[nodiscard]] inline u32 _clz(const u32 v) noexcept
{ return __builtin_clz(v); }

[[nodiscard]] inline u32 _ctz(const u64 v) noexcept
{ return __builtin_ctzll(v); }

[[nodiscard]] inline u32 _clz(const u64 v) noexcept
{ return __builtin_clzll(v); }

[[nodiscard]] constexpr inline u32 _ctzC(const u32 v) noexcept
{ return __builtin_ctz(v); }

[[nodiscard]] constexpr inline u32 _clzC(const u32 v) noexcept
{ return __builtin_clz(v); }

[[nodiscard]] constexpr inline u32 _ctzC(const u64 v) noexcept
{ return __builtin_ctzll(v); }

[[nodiscard]] constexpr inline u32 _clzC(const u64 v) noexcept
{ return __builtin_clzll(v); }
#endif

#if defined(TAU_CROSS_PLATFORM)
[[nodiscard]] constexpr inline u32 nextPowerOf2(u32 v) noexcept
{
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}

[[nodiscard]] constexpr inline u64 nextPowerOf2(u64 v) noexcept
{
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
}

[[nodiscard]] constexpr inline u32 log2i(u32 value) noexcept
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return clzTab32[((value * 0x07C4ACDDu)) >> 27];
}

[[nodiscard]] constexpr inline u32 log2i(u64 value) noexcept
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return clzTab64[((value - (value >> 1)) * 0x07EDD5E59A4E28C2ull) >> 58];
}
#else
[[nodiscard]] constexpr inline u32 nextPowerOf2(const u32 v) noexcept
{
    if(v == 1)
    { return 1; }
    return 1u << (32u - _clzC(v - 1u));
}

[[nodiscard]] constexpr inline u64 nextPowerOf2(const u64 v) noexcept
{
    if(v == 1)
    { return 1; }
    return 1ull << (64ull - _clzC(v - 1ull));
}

[[nodiscard]] constexpr inline u32 log2i(const u32 v) noexcept
{ return 63 - _clzC(v); }

[[nodiscard]] constexpr inline u32 log2i(const u64 v) noexcept
{ return 63 - _clzC(v); }
#endif

extern "C" {

i16 AddReturnOverflowI16(i16 a, i16 b, i16* carryOut) noexcept;
i32 AddReturnOverflowI32(i32 a, i32 b, i32* carryOut) noexcept;
i64 AddReturnOverflowI64(i64 a, i64 b, i64* carryOut) noexcept;

u16 AddReturnOverflowU16(u16 a, u16 b, u16* carryOut) noexcept;
u32 AddReturnOverflowU32(u32 a, u32 b, u32* carryOut) noexcept;
u64 AddReturnOverflowU64(u64 a, u64 b, u64* carryOut) noexcept;

i16 SubReturnBorrowI16(i16 a, i16 b, i16* borrowOut) noexcept;
i32 SubReturnBorrowI32(i32 a, i32 b, i32* borrowOut) noexcept;
i64 SubReturnBorrowI64(i64 a, i64 b, i64* borrowOut) noexcept;

u16 SubReturnBorrowU16(u16 a, u16 b, u16* borrowOut) noexcept;
u32 SubReturnBorrowU32(u32 a, u32 b, u32* borrowOut) noexcept;
u64 SubReturnBorrowU64(u64 a, u64 b, u64* borrowOut) noexcept;

i16 MulReturnOverflowI16(i16 a, i16 b, i16* overflowOut) noexcept;
i32 MulReturnOverflowI32(i32 a, i32 b, i32* overflowOut) noexcept;
i64 MulReturnOverflowI64(i64 a, i64 b, i64* overflowOut) noexcept;

u16 MulReturnOverflowU16(u16 a, u16 b, u16* overflowOut) noexcept;
u32 MulReturnOverflowU32(u32 a, u32 b, u32* overflowOut) noexcept;
u64 MulReturnOverflowU64(u64 a, u64 b, u64* overflowOut) noexcept;

}

inline i16 AddReturnOverflow(const i16 a, const i16 b, i16* const carryOut) noexcept { return AddReturnOverflowI16(a, b, carryOut); }
inline i32 AddReturnOverflow(const i32 a, const i32 b, i32* const carryOut) noexcept { return AddReturnOverflowI32(a, b, carryOut); }
inline i64 AddReturnOverflow(const i64 a, const i64 b, i64* const carryOut) noexcept { return AddReturnOverflowI64(a, b, carryOut); }

inline u16 AddReturnOverflow(const u16 a, const u16 b, u16* const carryOut) noexcept { return AddReturnOverflowU16(a, b, carryOut); }
inline u32 AddReturnOverflow(const u32 a, const u32 b, u32* const carryOut) noexcept { return AddReturnOverflowU32(a, b, carryOut); }
inline u64 AddReturnOverflow(const u64 a, const u64 b, u64* const carryOut) noexcept { return AddReturnOverflowU64(a, b, carryOut); }

inline i16 SubReturnBorrow(const i16 a, const i16 b, i16* const borrowOut) noexcept { return SubReturnBorrowI16(a, b, borrowOut); }
inline i32 SubReturnBorrow(const i32 a, const i32 b, i32* const borrowOut) noexcept { return SubReturnBorrowI32(a, b, borrowOut); }
inline i64 SubReturnBorrow(const i64 a, const i64 b, i64* const borrowOut) noexcept { return SubReturnBorrowI64(a, b, borrowOut); }

inline u16 SubReturnBorrow(const u16 a, const u16 b, u16* const borrowOut) noexcept { return SubReturnBorrowU16(a, b, borrowOut); }
inline u32 SubReturnBorrow(const u32 a, const u32 b, u32* const borrowOut) noexcept { return SubReturnBorrowU32(a, b, borrowOut); }
inline u64 SubReturnBorrow(const u64 a, const u64 b, u64* const borrowOut) noexcept { return SubReturnBorrowU64(a, b, borrowOut); }

inline i16 MulReturnOverflow(const i16 a, const i16 b, i16* const overflowOut) noexcept { return MulReturnOverflowI16(a, b, overflowOut); }
inline i32 MulReturnOverflow(const i32 a, const i32 b, i32* const overflowOut) noexcept { return MulReturnOverflowI32(a, b, overflowOut); }
inline i64 MulReturnOverflow(const i64 a, const i64 b, i64* const overflowOut) noexcept { return MulReturnOverflowI64(a, b, overflowOut); }

inline u16 MulReturnOverflow(const u16 a, const u16 b, u16* const overflowOut) noexcept { return MulReturnOverflowU16(a, b, overflowOut); }
inline u32 MulReturnOverflow(const u32 a, const u32 b, u32* const overflowOut) noexcept { return MulReturnOverflowU32(a, b, overflowOut); }
inline u64 MulReturnOverflow(const u64 a, const u64 b, u64* const overflowOut) noexcept { return MulReturnOverflowU64(a, b, overflowOut); }
