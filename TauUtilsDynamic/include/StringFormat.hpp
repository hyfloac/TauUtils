#pragma once

#include "NumTypes.hpp"
#include "String.hpp"
#include "Dragon4.hpp"
#include "IntToString.hpp"
#include "StringFormat.GlobalHandlers.hpp"

template<typename Context, typename Char>
inline constexpr u32 InternalFormat(Context& context, const Char* fmt, const iSys length) noexcept
{
    return FormatContext::HandlerLength(context, fmt, length);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline constexpr u32 InternalFormat(Context& context, const Char* fmt, const iSys length, const CurrArg& currArg, const Args&... args) noexcept
{
    u32 count = 0;
    iSys blockBegin = 0;

    for(iSys i = 0; i < length; ++i)
    {
        if(fmt[i] == Char{ '{' })
        {
            FormatContext::HandlerLength(context, fmt + blockBegin, i - blockBegin);

            if(fmt[i + 1] == Char{ '}' })
            {
                count += FormatContext::Handler(context, currArg);
                count += InternalFormat(context, fmt + i + 2, length - i - 2, args...);
                break;
            }
            // Capital Hex
            else if(fmt[i + 1] == Char{ 'X' })
            {
                if(fmt[i + 2] == Char{ 'p' } || fmt[i + 2] == Char{ 'P' })
                {
                    if(fmt[i + 3] == Char{ '0' })
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += FormatContext::HandlerHexPad0<Context, true>(context, currArg);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += FormatContext::HandlerHexPad<Char, Context, true>(context, currArg);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    break;
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += FormatContext::HandlerHex<Context, true>(context, currArg);
                    }
                    else
                    {
                        count += FormatContext::Handler(context, currArg);
                    }
                    count += InternalFormat(context, fmt + i + 3, length - i - 3, args...);
                    break;
                }
            }
            // Lower Hex
            else if(fmt[i + 1] == Char{ 'x' })
            {
                if(fmt[i + 2] == Char{ 'p' } || fmt[i + 2] == Char{ 'P' })
                {
                    if(fmt[i + 3] == Char{ '0' })
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += FormatContext::HandlerHexPad0<Context, false>(context, currArg);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += FormatContext::HandlerHexPad<Char, Context, false>(context, currArg);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    break;
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += FormatContext::HandlerHex<Context, false>(context, currArg);
                    }
                    else
                    {
                        count += FormatContext::Handler(context, currArg);
                    }
                    count += InternalFormat(context, fmt + i + 3, length - i - 3, args...);
                    break;
                }
            }
            // Padded Int
            else if(fmt[i + 1] == Char{ 'p' } || fmt[i + 1] == Char{ 'P' })
            {
                if(fmt[i + 2] == Char{ '0' })
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += FormatContext::HandlerIntPad0(context, currArg);
                    }
                    else
                    {
                        count += FormatContext::Handler(context, currArg);
                    }
                    count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += FormatContext::HandlerIntPadS(context, currArg);
                    }
                    else
                    {
                        count += FormatContext::Handler(context, currArg);
                    }
                    count += InternalFormat(context, fmt + i + 3, length - i - 3, args...);
                }
                break;
            }
            // Float
            else if(fmt[i + 1] == Char{ 'f' })
            {
                // Simple float. Essentially the same as not having a format specifier.
                if(fmt[i + 2] == Char{ '}' })
                {
                    count += FormatContext::Handler(context, currArg);
                    count += InternalFormat(context, fmt + i + 3, length - i - 3, args...);
                    break;
                }
                else
                {
                    i32 precision = 0;

                    switch(fmt[i + 2])
                    {
                        case Char{ '0' }: precision = 0; break;
                        case Char{ '1' }: precision = 1; break;
                        case Char{ '2' }: precision = 2; break;
                        case Char{ '3' }: precision = 3; break;
                        case Char{ '4' }: precision = 4; break;
                        case Char{ '5' }: precision = 5; break;
                        case Char{ '6' }: precision = 6; break;
                        case Char{ '7' }: precision = 7; break;
                        case Char{ '8' }: precision = 8; break;
                        case Char{ '9' }: precision = 9; break;
                        default: return count;
                    }

                    // Single Precision Specifier.
                    if(fmt[i + 3] == Char{ '}' })
                    {
                        if constexpr(::std::is_floating_point_v<CurrArg>)
                        {
                            count += FormatContext::Handler(context, currArg, PrintFloatFormat::Positional, precision);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    // Dual Precision Specifier.
                    else
                    {
                        precision *= 10;

                        switch(fmt[i + 2])
                        {
                            case Char{ '0' }: precision += 0; break;
                            case Char{ '1' }: precision += 1; break;
                            case Char{ '2' }: precision += 2; break;
                            case Char{ '3' }: precision += 3; break;
                            case Char{ '4' }: precision += 4; break;
                            case Char{ '5' }: precision += 5; break;
                            case Char{ '6' }: precision += 6; break;
                            case Char{ '7' }: precision += 7; break;
                            case Char{ '8' }: precision += 8; break;
                            case Char{ '9' }: precision += 9; break;
                            default: return count;
                        }

                        if constexpr(::std::is_floating_point_v<CurrArg>)
                        {
                            count += FormatContext::Handler(context, currArg, PrintFloatFormat::Positional, precision);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                }
            }
            // Scientific Float
            else if(fmt[i + 1] == Char{ 'e' })
            {
                // Scientific float.
                if(fmt[i + 2] == Char{ '}' })
                {
                    if constexpr(::std::is_floating_point_v<CurrArg>)
                    {
                        count += FormatContext::Handler(context, currArg, PrintFloatFormat::Scientific);
                    }
                    else
                    {
                        count += FormatContext::Handler(context, currArg);
                    }
                    count += InternalFormat(context, fmt + i + 3, length - i - 3, args...);
                    break;
                }
                else
                {
                    i32 precision = 0;

                    switch(fmt[i + 2])
                    {
                        case Char{ '0' }: precision = 0; break;
                        case Char{ '1' }: precision = 1; break;
                        case Char{ '2' }: precision = 2; break;
                        case Char{ '3' }: precision = 3; break;
                        case Char{ '4' }: precision = 4; break;
                        case Char{ '5' }: precision = 5; break;
                        case Char{ '6' }: precision = 6; break;
                        case Char{ '7' }: precision = 7; break;
                        case Char{ '8' }: precision = 8; break;
                        case Char{ '9' }: precision = 9; break;
                        default: return count;
                    }

                    // Single Precision Specifier.
                    if(fmt[i + 3] == Char{ '}' })
                    {
                        if constexpr(::std::is_floating_point_v<CurrArg>)
                        {
                            count += FormatContext::Handler(context, currArg, PrintFloatFormat::Scientific, precision);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    // Dual Precision Specifier.
                    else
                    {
                        precision *= 10;

                        switch(fmt[i + 2])
                        {
                            case Char{ '0' }: precision += 0; break;
                            case Char{ '1' }: precision += 1; break;
                            case Char{ '2' }: precision += 2; break;
                            case Char{ '3' }: precision += 3; break;
                            case Char{ '4' }: precision += 4; break;
                            case Char{ '5' }: precision += 5; break;
                            case Char{ '6' }: precision += 6; break;
                            case Char{ '7' }: precision += 7; break;
                            case Char{ '8' }: precision += 8; break;
                            case Char{ '9' }: precision += 9; break;
                            default: return count;
                        }

                        if constexpr(::std::is_floating_point_v<CurrArg>)
                        {
                            count += FormatContext::Handler(context, currArg, PrintFloatFormat::Scientific, precision);
                        }
                        else
                        {
                            count += FormatContext::Handler(context, currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                }
            }
        }
        else if(fmt[i] == Char{ '\'' })
        {
            if(fmt[i + 1] == Char{ '\'' })
            {
                FormatContext::HandlerLength(context, fmt + blockBegin, i - blockBegin);
                blockBegin = i + 2;
                ++count;
                ++i;
            }
            else if(fmt[i + 1] == Char{ '{' })
            {
                if(fmt[i + 2] == Char{ '\'' })
                {
                    FormatContext::HandlerLength(context, fmt + blockBegin, i - blockBegin);
                    FormatContext::Handler(context, Char{ '{' });
                    blockBegin = i + 3;
                    ++count;
                    i += 2;
                }
            }
            else
            {
                return count;
            }
        }
        else
        {
            ++count;
        }
    }

    return count;
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline constexpr u32 InternalFormat0(Context& context, const Char* fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    return InternalFormat(context, fmt, strLength(fmt), currArg, args...);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline constexpr u32 InternalFormat0(Context& context, const StringBaseT<Char>& fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    return InternalFormat(context, fmt.String(), fmt.Length(), currArg, args...);
}

struct NoOpFormatContext final
{
    template<typename T>
    constexpr u32 Handler(const T& t) noexcept
    { return 0; }

    template<typename Char>
    constexpr u32 Handler(const Char* const str, const uSys length) noexcept
    { return 0; }
    
    template<typename Int>
    constexpr u32 HandlerIntPad0(const Int t) noexcept
    { return 0; }

    template<typename Int>
    constexpr u32 HandlerIntPadS(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int>
    constexpr u32 HandlerHex(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int, c16 PadChar = u' '>
    constexpr u32 HandlerHexPad(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int>
    constexpr u32 HandlerHexPad0(const Int t) noexcept
    { return 0; }

    constexpr u32 Handler(const f32 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) noexcept
    { return 0; }

    constexpr u32 Handler(const f64 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) noexcept
    { return 0; }
};

template<typename CharOut>
struct StringFormatContext final
{
    template<typename Char>
    constexpr u32 HandlerLength(const Char* const str, const uSys length) noexcept
    {
        Builder.Append(str, length);
        return static_cast<u32>(length);
    }

    template<typename Char>
    constexpr u32 Handler(const StringBaseT<Char>& str) noexcept
    {
        Builder.Append(str);
        return 1;
    }

    constexpr u32 Handler(const char c) noexcept
    {
        Builder.Append(c);
        return 1;
    }

    constexpr u32 Handler(const wchar_t c) noexcept
    {
        Builder.Append(c);
        return 1;
    }

    constexpr u32 Handler(const c8 c) noexcept
    {
        Builder.Append(c);
        return 1;
    }

    constexpr u32 Handler(const c16 c) noexcept
    {
        Builder.Append(c);
        return 1;
    }

    constexpr u32 Handler(const c32 c) noexcept
    {
        Builder.Append(c);
        return 1;
    }

    constexpr u32 Handler(const char* const str) noexcept
    {
        Builder.Append(str);
        return static_cast<u32>(strLength(str));
    }

    constexpr u32 Handler(const wchar_t* const str) noexcept
    {
        Builder.Append(str);
        return static_cast<u32>(strLength(str));
    }

    constexpr u32 Handler(const c8* const str) noexcept
    {
        Builder.Append(str);
        return static_cast<u32>(strLength(str));
    }

    constexpr u32 Handler(const c16* const str) noexcept
    {
        Builder.Append(str);
        return static_cast<u32>(strLength(str));
    }

    constexpr u32 Handler(const c32* const str) noexcept
    {
        Builder.Append(str);
        return static_cast<u32>(strLength(str));
    }

    constexpr u32 Handler(const bool b) noexcept
    {
        if(b)
        {
            constexpr c8 str[] = u8"true";
            Builder.Append(str);
            return cexpr::strlen(str);
        }
        else
        {
            constexpr c8 str[] = u8"false";
            Builder.Append(str);
            return cexpr::strlen(str);
        }
    }

    template<typename Int>
    inline constexpr u32 HandlerInt(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoA(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<typename T>
    inline constexpr u32 Handler(const T* const p) noexcept
    {
        CharOut buffer[sizeof(uPtr) * 2 + 1];
        (void) ::tau::XtoAP < true, uPtr, CharOut, CharOut { '0' } > (::std::bit_cast<uPtr>(p), buffer);
        Builder.Append(buffer, sizeof(uPtr) * 2);
        return sizeof(uPtr) * 2;
    }

    template<typename Int>
    inline constexpr u32 HandlerIntPad0(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoAP0(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<typename Int>
    inline constexpr u32 HandlerIntPadS(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoAPS(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int>
    inline constexpr u32 HandlerHex(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoA<Uppercase>(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int, CharOut PadChar = ' '>
    inline constexpr u32 HandlerHexPad(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoAP<Uppercase, Int, CharOut, PadChar>(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int>
    inline constexpr u32 HandlerHexPad0(const Int d) noexcept
    {
        return HandlerHexPad<Uppercase, Int, '0'>(d);
    }

    constexpr u32 Handler(const i8  d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const i16 d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const i32 d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const i64 d) noexcept { return HandlerInt(d); }

    constexpr u32 Handler(const u8  d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const u16 d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const u32 d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const u64 d) noexcept { return HandlerInt(d); }

    constexpr u32 Handler(const          long d) noexcept { return HandlerInt(d); }
    constexpr u32 Handler(const unsigned long d) noexcept { return HandlerInt(d); }

    constexpr u32 Handler(const f32 f, const PrintFloatFormat format, const i32 precision = 6) noexcept
    {
        CharOut buffer[192];
        const uSys writeLen = PrintFloat32(buffer, ::std::size(buffer), f, format, precision);
        Builder.Append(buffer);
        return writeLen;
    }

    constexpr u32 Handler(const f64 f, const PrintFloatFormat format, const i32 precision = 17) noexcept
    {
        CharOut buffer[256];
        const uSys writeLen = PrintFloat64(buffer, ::std::size(buffer), f, format, precision);
        Builder.Append(buffer);
        return writeLen;
    }

    StringBuilderT<CharOut> Builder;
};

template<typename CharOut, typename CharIn, typename CurrArg, typename... Args>
inline constexpr DynStringT<CharOut> Format(const CharIn* fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToString();
}

template<typename CharOut, typename CharIn, typename CurrArg, typename... Args>
inline constexpr DynStringT<CharOut> Format(const StringBaseT<CharIn>& fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToString();
}

template<typename CharOut, iSys OutLen, typename CharIn, iSys FmtLen, typename CurrArg, typename... Args>
inline consteval auto FormatConstEval(const CharIn(&fmt)[FmtLen], const CurrArg& currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToStringC<static_cast<uSys>(OutLen)>();
}

template<typename CharOut, iSys OutLen, typename CharIn, iSys FmtLen, typename CurrArg, typename... Args>
inline consteval auto FormatConstEval(const ConstExprStackStringT<CharIn, FmtLen>& fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToStringC<static_cast<uSys>(OutLen)>();
}

