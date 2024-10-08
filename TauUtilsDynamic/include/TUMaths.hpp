// ReSharper disable CppClangTidyClangDiagnosticSignCompare
#pragma once

#include "NumTypes.hpp"
#include "MathDefines.hpp"

/**
* Converts radians to degrees (single precision).
*/
#define RAD_2_DEG_F(F) (float)  ((F) * RADIANS_TO_DEGREES_CONVERTER_VAL<float>)
/**
* Converts degrees to radians (single precision).
*/
#define DEG_2_RAD_F(F) (float)  ((F) * DEGREES_TO_RADIANS_CONVERTER_VAL<float>)
/**
 * Converts radians to degrees (double precision).
 */
#define RAD_2_DEG_D(D) (double) ((D) * RADIANS_TO_DEGREES_CONVERTER_VAL<double>)
 /**
  * Converts degrees to radians (double precision).
  */
#define DEG_2_RAD_D(D) (double) ((D) * DEGREES_TO_RADIANS_CONVERTER_VAL<double>)

#define RAD_2_DEG(F) RAD_2_DEG_F(F)
#define DEG_2_RAD(F) DEG_2_RAD_F(F)

template<typename T>
inline constexpr T Rad2Deg(const T radians) noexcept
{ return radians * RADIANS_TO_DEGREES_CONVERTER_VAL<T>; }

template<typename T>
inline constexpr T Deg2Rad(const T degrees) noexcept
{ return degrees * DEGREES_TO_RADIANS_CONVERTER_VAL<T>; }

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

template<typename T>
[[nodiscard]] inline constexpr T ClampT(const T value, const T min, const T max)
{ return minT(maxT(value, min), max); }

template<typename T0, typename T1, typename T2>
[[nodiscard]] inline constexpr T0 ClampT(const T0 value, const T1 min, const T2 max)
{ return minT(maxT(value, min), max); }

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

template<typename T>
[[nodiscard]] constexpr inline T AlignTo(const T val, const T alignment) noexcept
{
    if(alignment == 1 || val % alignment == 0)
    { return val; }
    return (val + alignment) & ~(alignment - 1);
}

template<typename Tv, typename Ta>
[[nodiscard]] constexpr inline Tv AlignTo(const Tv val, const Ta alignment) noexcept
{
    const Tv alignmentTv = static_cast<Tv>(alignment);

    if(alignmentTv == 1 || val % alignmentTv == 0)
    { return val; }
    return (val + alignmentTv) & ~(alignmentTv - 1);
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

template<typename T>
[[nodiscard]] constexpr inline T AlignToAny(const T val, const T alignment) noexcept
{
    const T mod = val % alignment;
    if(alignment == 1 || mod == 0)
    { return val; }
    return val + (alignment - mod);
}

template<typename Tv, typename Ta>
[[nodiscard]] constexpr inline Tv AlignToAny(const Tv val, const Ta alignment) noexcept
{
    const Tv alignmentTv = static_cast<Tv>(alignment);
    const Tv mod = val % alignmentTv;

    if(alignmentTv == 1 || mod == 0)
    { return val; }
    return val + (alignmentTv - mod);
}

template<typename T, T Alignment>
[[nodiscard]] constexpr inline T AlignToAny(const T val) noexcept
{
    if constexpr(Alignment == 1)
    { return val; }
    const T mod = val % Alignment;
    if(mod == 0)
    { return val; }

    return val + (Alignment - mod);
}

template<typename Tv, typename Ta, Ta Alignment>
[[nodiscard]] constexpr inline Tv AlignToAny(const Tv val) noexcept
{
    constexpr Tv alignment = static_cast<Tv>(Alignment);

    if constexpr(alignment == 1)
    { return val; }
    const Tv mod = val % alignment;
    if(val % alignment == 0)
    { return val; }

    return val + (alignment - mod);
}

constexpr u8  DeBruijnConstant8  = 0x17;
constexpr u16 DeBruijnConstant16 = 0x09AF;
constexpr u32 DeBruijnConstant32 = 0x04653ADFu;
constexpr u64 DeBruijnConstant64 = 0x0218A392CD3D5DBFull;

constexpr u8 DeBruijnTable8[8] = {
     0, 1, 2, 4, 7, 3, 6, 5
};

constexpr u16 DeBruijnTable16[16] = {
     0,  1,  2,  5,  3,  9,  6, 11,
    15,  4,  8, 10, 14,  7, 13, 12
};

constexpr u32 DeBruijnTable32[32] = {
     0,  1,  2,  6,  3, 11,  7, 16,
     4, 14, 12, 21,  8, 23, 17, 26,
    31,  5, 10, 15, 13, 20, 22, 25,
    30,  9, 19, 24, 29, 18, 28, 27
};

constexpr u64 DeBruijnTable64[64] = {
     0,  1,  2,  7,  3, 13,  8, 19,
     4, 25, 14, 28,  9, 34, 20, 40,
     5, 17, 26, 38, 15, 46, 29, 48,
    10, 31, 35, 54, 21, 50, 41, 57,
    63,  6, 12, 18, 24, 27, 33, 39,
    16, 37, 45, 47, 30, 53, 49, 56,
    62, 11, 23, 32, 36, 44, 52, 55,
    61, 22, 43, 51, 60, 42, 59, 58
};

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

[[nodiscard]] inline u8 CountTrailingZero(const u8 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward(&trailingZero, v);
    return static_cast<u8>(trailingZero);
}

[[nodiscard]] inline u16 CountTrailingZero(const u16 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward(&trailingZero, v);
    return static_cast<u16>(trailingZero);
}

[[nodiscard]] inline u32 CountTrailingZero(const u32 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward(&trailingZero, v);
    return trailingZero;
}

[[nodiscard]] inline u64 CountTrailingZero(const u64 v) noexcept
{
    unsigned long trailingZero = 0;
    _BitScanForward64(&trailingZero, v);
    return trailingZero;
}

[[nodiscard]] inline u8 CountLeadingZero(const u8 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse(&leadingZero, v);
    return static_cast<u8>(15 - leadingZero);
}

[[nodiscard]] inline u16 CountLeadingZero(const u16 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse(&leadingZero, v);
    return static_cast<u16>(15 - leadingZero);
}

[[nodiscard]] inline u32 CountLeadingZero(const u32 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse(&leadingZero, v);
    return 31 - leadingZero;
}

[[nodiscard]] inline u64 CountLeadingZero(const u64 v) noexcept
{
    unsigned long leadingZero = 0;
    _BitScanReverse64(&leadingZero, v);
    return static_cast<u64>(63ul - leadingZero);
}

[[nodiscard]] constexpr inline u8 CountTrailingZeroC(u8 v) noexcept
{
    v = v ^ (v - 1);
    return DeBruijnTable8[(((v - (v >> 1)) * DeBruijnConstant8) & 0xFF) >> 5];
}

[[nodiscard]] constexpr inline u16 CountTrailingZeroC(u16 v) noexcept
{
    v = v ^ (v - 1);
    return DeBruijnTable16[(((v - (v >> 1)) * DeBruijnConstant16) & 0xFFFF) >> 12];
}

[[nodiscard]] constexpr inline u32 CountTrailingZeroC(u32 v) noexcept
{
    v = v ^ (v - 1);
    return DeBruijnTable32[(((v - (v >> 1)) * DeBruijnConstant32) & 0xFFFFFFFFu) >> 27];
}

[[nodiscard]] constexpr inline u64 CountTrailingZeroC(u64 v) noexcept
{
    v = v ^ (v - 1);
    return DeBruijnTable64[(((v - (v >> 1)) * DeBruijnConstant64) & 0xFFFFFFFFFFFFFFFFull) >> 58];
}

[[nodiscard]] constexpr inline u8 CountLeadingZeroC(u8 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    return 7 - DeBruijnTable8[((((v - (v >> 1)) * DeBruijnConstant8)) & 0xFF) >> 5];
}

[[nodiscard]] constexpr inline u16 CountLeadingZeroC(u16 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    return 15 - DeBruijnTable16[((((v - (v >> 1)) * DeBruijnConstant16)) & 0xFFFF) >> 12];
}

[[nodiscard]] constexpr inline u32 CountLeadingZeroC(u32 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return 31 - DeBruijnTable32[((((v - (v >> 1)) * DeBruijnConstant32)) & 0xFFFFFFFFu) >> 27];
}

[[nodiscard]] constexpr inline u64 CountLeadingZeroC(u64 v) noexcept
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return 63 - DeBruijnTable64[(((v - (v >> 1)) * DeBruijnConstant64) & 0xFFFFFFFFFFFFFFFFull) >> 58];
}
#else
[[nodiscard]] inline u8 CountTrailingZero(const u8 v) noexcept
{ return static_cast<u8>(__builtin_ctz(static_cast<u32>(v))); }

[[nodiscard]] inline u16 CountTrailingZero(const u16 v) noexcept
{ return static_cast<u16>(__builtin_ctz(static_cast<u32>(v))); }

[[nodiscard]] inline u32 CountTrailingZero(const u32 v) noexcept
{ return __builtin_ctz(v); }

[[nodiscard]] inline u32 CountTrailingZero(const u64 v) noexcept
{ return __builtin_ctzll(v); }

[[nodiscard]] inline u8 CountLeadingZero(const u8 v) noexcept
{ return static_cast<u8>(__builtin_clz(static_cast<u32>(v))) - 24; }

[[nodiscard]] inline u16 CountLeadingZero(const u16 v) noexcept
{ return static_cast<u16>(__builtin_clz(static_cast<u32>(v))) - 16; }

[[nodiscard]] inline u32 CountLeadingZero(const u32 v) noexcept
{ return __builtin_clz(v); }

[[nodiscard]] inline u32 CountLeadingZero(const u64 v) noexcept
{ return __builtin_clzll(v); }

[[nodiscard]] constexpr inline u8 CountTrailingZeroC(const u8 v) noexcept
{ return static_cast<u8>(__builtin_ctz(static_cast<u32>(v))); }

[[nodiscard]] constexpr inline u8 CountLeadingZeroC(const u8 v) noexcept
{ return static_cast<u8>(__builtin_clz(static_cast<u32>(v))) - 24; }

[[nodiscard]] constexpr inline u16 CountTrailingZeroC(const u16 v) noexcept
{ return static_cast<u16>(__builtin_ctz(static_cast<u32>(v))); }

[[nodiscard]] constexpr inline u16 CountLeadingZeroC(const u16 v) noexcept
{ return static_cast<u16>(__builtin_clz(static_cast<u32>(v))) - 16; }

[[nodiscard]] constexpr inline u32 CountTrailingZeroC(const u32 v) noexcept
{ return __builtin_ctz(v); }

[[nodiscard]] constexpr inline u32 CountLeadingZeroC(const u32 v) noexcept
{ return __builtin_clz(v); }

[[nodiscard]] constexpr inline u32 CountTrailingZeroC(const u64 v) noexcept
{ return __builtin_ctzll(v); }

[[nodiscard]] constexpr inline u32 CountLeadingZeroC(const u64 v) noexcept
{ return __builtin_clzll(v); }
#endif

[[nodiscard]] constexpr inline u32 nextPowerOf2(const u8 v) noexcept
{
    if(v == 1)
    { return 1; }
    return static_cast<u8>(1u << (8u - CountLeadingZeroC(v - 1u)));
}

[[nodiscard]] constexpr inline u32 nextPowerOf2(const u16 v) noexcept
{
    if(v == 1)
    { return 1; }
    return static_cast<u16>(1u << (16u - CountLeadingZeroC(v - 1u)));
}

[[nodiscard]] constexpr inline u32 nextPowerOf2(const u32 v) noexcept
{
    if(v == 1)
    { return 1; }
    return 1u << (32u - CountLeadingZeroC(v - 1u));
}

[[nodiscard]] constexpr inline u64 nextPowerOf2(const u64 v) noexcept
{
    if(v == 1)
    { return 1; }
    return 1ull << (64ull - CountLeadingZeroC(v - 1ull));
}

[[nodiscard]] constexpr inline u32 log2i(const u8 v) noexcept
{ return 7 - CountLeadingZeroC(v); }

[[nodiscard]] constexpr inline u32 log2i(const u16 v) noexcept
{ return 15 - CountLeadingZeroC(v); }

[[nodiscard]] constexpr inline u32 log2i(const u32 v) noexcept
{ return 31 - CountLeadingZeroC(v); }

[[nodiscard]] constexpr inline u32 log2i(const u64 v) noexcept
{ return 63 - static_cast<u32>(CountLeadingZeroC(v)); }

template<typename T>
[[nodiscard]] constexpr inline T DivCeil(const T dividend, const T divisor) noexcept
{ return 1 + ((dividend - 1) / divisor); }
