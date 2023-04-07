#pragma once

#include "NumTypes.hpp"
#include "String.hpp"
#include "Dragon4.hpp"

template<typename Context, typename Char>
inline u32 InternalFormat(Context& context, const Char* fmt, const iSys length) noexcept
{
    return context.HandlerLength(fmt, length);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline u32 InternalFormat(Context& context, const Char* fmt, const iSys length, CurrArg currArg, const Args&... args) noexcept
{
    u32 count = 0;
    iSys blockBegin = 0;

    for(iSys i = 0; i < length; ++i)
    {
        if(fmt[i] == Char{ '{' })
        {
            context.HandlerLength(fmt + blockBegin, i - blockBegin);

            if(fmt[i + 1] == Char{ '}' })
            {
                count += context.Handler(currArg);
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
                            count += context.template HandlerHexPad0<true>(currArg);
                        }
                        else
                        {
                            count += context.Handler(currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += context.template HandlerHexPad<true>(currArg);
                        }
                        else
                        {
                            count += context.Handler(currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    break;
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += context.template HandlerHex<true>(currArg);
                    }
                    else
                    {
                        count += context.Handler(currArg);
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
                            count += context.template HandlerHexPad0<false>(currArg);
                        }
                        else
                        {
                            count += context.Handler(currArg);
                        }
                        count += InternalFormat(context, fmt + i + 5, length - i - 5, args...);
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += context.template HandlerHexPad<false>(currArg);
                        }
                        else
                        {
                            count += context.Handler(currArg);
                        }
                        count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                    }
                    break;
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += context.template HandlerHex<false>(currArg);
                    }
                    else
                    {
                        count += context.Handler(currArg);
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
                        count += context.HandlerIntPad0(currArg);
                    }
                    else
                    {
                        count += context.Handler(currArg);
                    }
                    count += InternalFormat(context, fmt + i + 4, length - i - 4, args...);
                }
                else
                {
                    if constexpr(::std::is_integral_v<CurrArg>)
                    {
                        count += context.HandlerIntPadS(currArg);
                    }
                    else
                    {
                        count += context.Handler(currArg);
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
                    count += context.Handler(currArg);
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
                            count += context.Handler(currArg, PrintFloatFormat::Positional, precision);
                        }
                        else
                        {
                            count += context.Handler(currArg);
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
                            count += context.Handler(currArg, PrintFloatFormat::Positional, precision);
                        }
                        else
                        {
                            count += context.Handler(currArg);
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
                        count += context.Handler(currArg, PrintFloatFormat::Scientific);
                    }
                    else
                    {
                        count += context.Handler(currArg);
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
                            count += context.Handler(currArg, PrintFloatFormat::Scientific, precision);
                        }
                        else
                        {
                            count += context.Handler(currArg);
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
                            count += context.Handler(currArg, PrintFloatFormat::Scientific, precision);
                        }
                        else
                        {
                            count += context.Handler(currArg);
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
                context.HandlerLength(fmt + blockBegin, i - blockBegin);
                blockBegin = i + 2;
                ++count;
                ++i;
            }
            else if(fmt[i + 1] == Char{ '{' })
            {
                if(fmt[i + 2] == Char{ '\'' })
                {
                    context.HandlerLength(fmt + blockBegin, i - blockBegin);
                    context.Handler(Char{ '{' });
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
inline u32 InternalFormat0(Context& context, const Char* fmt, CurrArg currArg, const Args&... args) noexcept
{
    return InternalFormat(context, fmt, strLength(fmt), currArg, args...);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline u32 InternalFormat0(Context& context, const StringBase& fmt, CurrArg currArg, const Args&... args) noexcept
{
    return InternalFormat(context, fmt.String(), fmt.Length(), currArg, args...);
}

struct NoOpFormatContext final
{
    template<typename T>
    u32 Handler(const T& t) noexcept
    { return 0; }

    template<typename Char>
    u32 Handler(const Char* const str, const uSys length) noexcept
    { return 0; }
    
    template<typename Int>
    u32 HandlerIntPad0(const Int t) noexcept
    { return 0; }

    template<typename Int>
    u32 HandlerIntPadS(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int>
    u32 HandlerHex(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int, c16 PadChar = u' '>
    u32 HandlerHexPad(const Int t) noexcept
    { return 0; }

    template<bool Uppercase, typename Int>
    u32 HandlerHexPad0(const Int t) noexcept
    { return 0; }

    u32 Handler(const f32 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) noexcept
    { return 0; }

    u32 Handler(const f64 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) noexcept
    { return 0; }
};
