#pragma once

#include "NumTypes.hpp"
#include "Dragon4.hpp"

namespace FormatContext {

template<typename Context, typename Char>
inline constexpr u32 HandlerLength(Context& ctx, const Char* const str, const uSys length) noexcept
{
    return ctx.HandlerLength(str, length);
}

template<typename Context, typename T>
inline constexpr u32 Handler(Context& ctx, const T& t) noexcept
{
    return ctx.Handler(t);
}

template<typename Context, typename Int>
inline constexpr u32 HandlerInt(Context& ctx, const Int d) noexcept
{
    return ctx.HandlerInt(d);
}

template<typename Context, typename T>
inline constexpr u32 Handler(Context& ctx, const T* const p) noexcept
{
    return ctx.Handler(p);
}

template<typename Context, typename Int>
inline constexpr u32 HandlerIntPad0(Context& ctx, const Int d) noexcept
{
    return ctx.HandlerIntPad0(d);
}

template<typename Context, typename Int>
inline constexpr u32 HandlerIntPadS(Context& ctx, const Int d) noexcept
{
    return ctx.HandlerIntPadS(d);
}

template<typename Context, bool Uppercase, typename Int>
inline constexpr u32 HandlerHex(Context& ctx, const Int d) noexcept
{
    return ctx.template HandlerHex<Uppercase>(d);
}

template<typename CharOut, typename Context, bool Uppercase, typename Int, CharOut PadChar = ' '>
inline constexpr u32 HandlerHexPad(Context& ctx, const Int d) noexcept
{
    return ctx.template HandlerHexPad<Uppercase, Int, PadChar>(d);
}

template<typename Context, bool Uppercase, typename Int>
inline constexpr u32 HandlerHexPad0(Context& ctx, const Int d) noexcept
{
    return ctx.template HandlerHexPad0<Uppercase>(d);
}

template<typename Context>
inline constexpr u32 Handler(Context& ctx, const f32 f, const PrintFloatFormat format, const i32 precision = 6) noexcept
{
    return ctx.Handler(f, format, precision);
}

template<typename Context>
inline constexpr u32 Handler(Context& ctx, const f64 f, const PrintFloatFormat format, const i32 precision = 17) noexcept
{
    return ctx.Handler(f, format, precision);
}

}
