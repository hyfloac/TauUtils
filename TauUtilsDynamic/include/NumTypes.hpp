#pragma once

#ifdef TAU_NUMTYPES_USE_CSTDLIB
  #pragma warning(push, 0)
  #include <cstddef>
  #include <cstdint>
  #pragma warning(pop)

  typedef ::std::int8_t   i8;
  typedef ::std::int16_t  i16;
  typedef ::std::int32_t  i32;
  typedef ::std::int64_t  i64;
  
  typedef ::std::uint8_t  u8;
  typedef ::std::uint16_t u16;
  typedef ::std::uint32_t u32;
  typedef ::std::uint64_t u64;
#else
  typedef signed char      i8;
  typedef signed short     i16;
  typedef signed int       i32;
  typedef signed long long i64;
  
  typedef unsigned char      u8;
  typedef unsigned short     u16;
  typedef unsigned int       u32;
  typedef unsigned long long u64;

  #if defined(_MSVC_LANG)
    #ifndef INT8_MIN
      #define INT8_MIN         (-127i8 - 1)
    #endif
    #ifndef INT16_MIN
      #define INT16_MIN        (-32767i16 - 1)
    #endif
    #ifndef INT32_MIN
      #define INT32_MIN        (-2147483647i32 - 1)
    #endif
    #ifndef INT64_MIN
      #define INT64_MIN        (-9223372036854775807i64 - 1)
    #endif
    #ifndef INT8_MAX
      #define INT8_MAX         127i8
    #endif
    #ifndef INT16_MAX
      #define INT16_MAX        32767i16
    #endif
    #ifndef INT32_MAX
      #define INT32_MAX        2147483647i32
    #endif
    #ifndef INT64_MAX
      #define INT64_MAX        9223372036854775807i64
    #endif
    #ifndef UINT8_MAX
      #define UINT8_MAX        0xffui8
    #endif
    #ifndef UINT16_MAX
      #define UINT16_MAX       0xffffui16
    #endif
    #ifndef UINT32_MAX
      #define UINT32_MAX       0xffffffffui32
    #endif
    #ifndef UINT64_MAX
      #define UINT64_MAX       0xffffffffffffffffui64
    #endif
  #else
    #ifndef INT8_MIN
      #define INT8_MIN         (-128)
    #endif
    #ifndef INT16_MIN
      #define INT16_MIN        (-32768)
    #endif
    #ifndef INT32_MIN
      /*
         Note:  the literal "most negative int" cannot be written in C --
         the rules in the standard (section 6.4.4.1 in C99) will give it
         an unsigned type, so INT32_MIN (and the most negative member of
         any larger signed type) must be written via a constant expression.
      */
      #define INT32_MIN        (-INT32_MAX - 1)
    #endif
    #ifndef INT64_MIN
      /*
         Note:  the literal "most negative int" cannot be written in C --
         the rules in the standard (section 6.4.4.1 in C99) will give it
         an unsigned type, so INT32_MIN (and the most negative member of
         any larger signed type) must be written via a constant expression.
      */
      #define INT64_MIN        (-INT64_MAX - 1)
    #endif
    #ifndef INT8_MAX
      #define INT8_MAX         127
    #endif
    #ifndef INT16_MAX
      #define INT16_MAX        32767
    #endif
    #ifndef INT32_MAX
      #define INT32_MAX        2147483647
    #endif
    #ifndef INT64_MAX
      #define INT64_MAX        9223372036854775807LL
    #endif
    #ifndef UINT8_MAX
      #define UINT8_MAX        0xff
    #endif
    #ifndef UINT16_MAX
      #define UINT16_MAX       0xffff
    #endif
    #ifndef UINT32_MAX
      #define UINT32_MAX       0xffffffffU
    #endif
    #ifndef UINT64_MAX
      #define UINT64_MAX       0xffffffffffffffffULL
    #endif
  #endif
#endif

// Boolean bit masks
typedef u8  b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

#if !defined(__cpp_char8_t)
  enum char8_t : unsigned char { };
#endif

typedef char8_t  c8;
typedef char16_t c16;
typedef char32_t c32;

typedef float  single;
typedef float  f32;
typedef double f64;

#if defined(_MSVC_LANG) || !defined(TAU_NUMTYPES_USE_CSTDLIB)
  #if defined(_WIN64) || (defined(_M_X64) && _M_X64 == 100) || (defined(__aarch64__) && __aarch64__)
    typedef i64 iSys;
    typedef u64 uSys;

    typedef i64 iPtr;
    typedef u64 uPtr;
  #else
    typedef i32 iSys;
    typedef u32 uSys;

    typedef i32 iPtr;
    typedef u32 uPtr;
  #endif
#else
  typedef ::std::ptrdiff_t iSys;
  typedef ::std::size_t    uSys;

  typedef ::std::intptr_t  iPtr;
  typedef ::std::uintptr_t uPtr;
#endif

inline constexpr uSys operator "" _uz(const uSys n) noexcept
{ return static_cast<uSys>(n); }

inline constexpr iSys operator "" _iz(const uSys n) noexcept
{ return static_cast<iSys>(n); }

template<typename T>
struct IntMaxMin final
{
    static constexpr T Min = 0;
    static constexpr T Max = 0;
};

template<>
struct IntMaxMin<i8> final
{
    static constexpr i8 Min = INT8_MIN;
    static constexpr i8 Max = INT8_MAX;
};

template<>
struct IntMaxMin<i16> final
{
    static constexpr i16 Min = INT16_MIN;
    static constexpr i16 Max = INT16_MAX;
};

template<>
struct IntMaxMin<i32> final
{
    static constexpr i32 Min = INT32_MIN;
    static constexpr i32 Max = INT32_MAX;
};

template<>
struct IntMaxMin<i64> final
{
    static constexpr i64 Min = INT64_MIN;
    static constexpr i64 Max = INT64_MAX;
};

template<>
struct IntMaxMin<u8> final
{
    static constexpr u8 Min = 0;
    static constexpr u8 Max = UINT8_MAX;
};

template<>
struct IntMaxMin<u16> final
{
    static constexpr u16 Min = 0;
    static constexpr u16 Max = UINT16_MAX;
};

template<>
struct IntMaxMin<u32> final
{
    static constexpr u32 Min = 0;
    static constexpr u32 Max = UINT32_MAX;
};

template<>
struct IntMaxMin<u64> final
{
    static constexpr u64 Min = 0;
    static constexpr u64 Max = UINT64_MAX;
};

template<>
struct IntMaxMin<c8> final
{
    static constexpr c8 Min = 0;
    static constexpr c8 Max = UINT8_MAX;
};

template<>
struct IntMaxMin<c16> final
{
    static constexpr c16 Min = 0;
    static constexpr c16 Max = UINT16_MAX;
};

template<>
struct IntMaxMin<c32> final
{
    static constexpr c32 Min = 0;
    static constexpr c32 Max = UINT32_MAX;
};

constexpr i8 operator ""_i8(const unsigned long long x)
{
    return static_cast<i8>(x);
}

constexpr i16 operator ""_i16(const unsigned long long x)
{
    return static_cast<i16>(x);
}

constexpr i32 operator ""_i32(const unsigned long long x)
{
    return static_cast<i32>(x);
}

constexpr i64 operator ""_i64(const unsigned long long x)
{
    return static_cast<i64>(x);
}

constexpr u8 operator ""_u8(const unsigned long long x)
{
    return static_cast<u8>(x);
}

constexpr u16 operator ""_u16(const unsigned long long x)
{
    return static_cast<u16>(x);
}

constexpr u32 operator ""_u32(const unsigned long long x)
{
    return static_cast<u32>(x);
}

constexpr u64 operator ""_u64(const unsigned long long x)
{
    // ReSharper disable once CppRedundantCastExpression
    return static_cast<u64>(x);
}
