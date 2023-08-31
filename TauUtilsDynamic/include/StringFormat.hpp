#pragma once

#include "NumTypes.hpp"
#include "String.hpp"
#include "Dragon4.hpp"
#include "IntToString.hpp"

// template<typename Context, typename Char>
// inline consteval auto InternalFormatConstEval(const Context& context, const Char* fmt, const iSys length) noexcept
// {
//     return context.HandlerLength(fmt, length);
// }
//
// template<typename Context, typename Char, typename CurrArg, typename... Args>
// inline consteval auto InternalFormatConstEval(const Context& context, const Char* fmt, const iSys length, CurrArg currArg, const Args&... args) noexcept
// {
//     u32 count = 0;
//     iSys blockBegin = 0;
//
//     for(iSys i = 0; i < length; ++i)
//     {
//         if(fmt[i] == Char { '{' })
//         {
//             const auto ctx0 = context.HandlerLength(fmt + blockBegin, i - blockBegin);
//
//             if(fmt[i + 1] == Char { '}' })
//             {
//                 const auto ctx1 = ctx0.Handler(currArg);
//                 return InternalFormat(ctx1, fmt + i + 2, length - i - 2, args...);
//             }
//             // Capital Hex
//             else if(fmt[i + 1] == Char { 'X' })
//             {
//                 if(fmt[i + 2] == Char { 'p' } || fmt[i + 2] == Char { 'P' })
//                 {
//                     if(fmt[i + 3] == Char { '0' })
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.template HandlerHexPad0<true>(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                     }
//                     else
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.template HandlerHexPad<true>(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                     }
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         const auto ctx1 = ctx0.template HandlerHex<true>(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                     else
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                 }
//             }
//             // Lower Hex
//             else if(fmt[i + 1] == Char { 'x' })
//             {
//                 if(fmt[i + 2] == Char { 'p' } || fmt[i + 2] == Char { 'P' })
//                 {
//                     if(fmt[i + 3] == Char { '0' })
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.template HandlerHexPad0<false>(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                     }
//                     else
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.template HandlerHexPad<false>(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                     }
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         const auto ctx1 = ctx0.template HandlerHex<false>(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                     else
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                 }
//             }
//             // Padded Int
//             else if(fmt[i + 1] == Char { 'p' } || fmt[i + 1] == Char { 'P' })
//             {
//                 if(fmt[i + 2] == Char { '0' })
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         const auto ctx1 = ctx0.HandlerIntPad0(currArg);
//                         return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                     }
//                     else
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg);
//                         return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                     }
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         const auto ctx1 = ctx0.HandlerIntPadS(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                     else
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                 }
//             }
//             // Float
//             else if(fmt[i + 1] == Char { 'f' })
//             {
//                 // Simple float. Essentially the same as not having a format specifier.
//                 if(fmt[i + 2] == Char { '}' })
//                 {
//                     const auto ctx1 = ctx0.Handler(currArg);
//                     return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                 }
//                 else
//                 {
//                     i32 precision = 0;
//
//                     switch(fmt[i + 2])
//                     {
//                         case Char { '0' }: precision = 0; break;
//                         case Char { '1' }: precision = 1; break;
//                         case Char { '2' }: precision = 2; break;
//                         case Char { '3' }: precision = 3; break;
//                         case Char { '4' }: precision = 4; break;
//                         case Char { '5' }: precision = 5; break;
//                         case Char { '6' }: precision = 6; break;
//                         case Char { '7' }: precision = 7; break;
//                         case Char { '8' }: precision = 8; break;
//                         case Char { '9' }: precision = 9; break;
//                         default: return count;
//                     }
//
//                     // Single Precision Specifier.
//                     if(fmt[i + 3] == Char { '}' })
//                     {
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg, PrintFloatFormat::Positional, precision);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                     }
//                     // Dual Precision Specifier.
//                     else
//                     {
//                         precision *= 10;
//
//                         switch(fmt[i + 2])
//                         {
//                             case Char { '0' }: precision += 0; break;
//                             case Char { '1' }: precision += 1; break;
//                             case Char { '2' }: precision += 2; break;
//                             case Char { '3' }: precision += 3; break;
//                             case Char { '4' }: precision += 4; break;
//                             case Char { '5' }: precision += 5; break;
//                             case Char { '6' }: precision += 6; break;
//                             case Char { '7' }: precision += 7; break;
//                             case Char { '8' }: precision += 8; break;
//                             case Char { '9' }: precision += 9; break;
//                             default: return count;
//                         }
//
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg, PrintFloatFormat::Positional, precision);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                     }
//                 }
//             }
//             // Scientific Float
//             else if(fmt[i + 1] == Char { 'e' })
//             {
//                 // Scientific float.
//                 if(fmt[i + 2] == Char { '}' })
//                 {
//                     if constexpr(::std::is_floating_point_v<CurrArg>)
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg, PrintFloatFormat::Scientific);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                     else
//                     {
//                         const auto ctx1 = ctx0.Handler(currArg);
//                         return InternalFormat(ctx1, fmt + i + 3, length - i - 3, args...);
//                     }
//                 }
//                 else
//                 {
//                     i32 precision = 0;
//
//                     switch(fmt[i + 2])
//                     {
//                         case Char { '0' }: precision = 0; break;
//                         case Char { '1' }: precision = 1; break;
//                         case Char { '2' }: precision = 2; break;
//                         case Char { '3' }: precision = 3; break;
//                         case Char { '4' }: precision = 4; break;
//                         case Char { '5' }: precision = 5; break;
//                         case Char { '6' }: precision = 6; break;
//                         case Char { '7' }: precision = 7; break;
//                         case Char { '8' }: precision = 8; break;
//                         case Char { '9' }: precision = 9; break;
//                         default: return count;
//                     }
//
//                     // Single Precision Specifier.
//                     if(fmt[i + 3] == Char { '}' })
//                     {
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg, PrintFloatFormat::Scientific, precision);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 4, length - i - 4, args...);
//                         }
//                     }
//                     // Dual Precision Specifier.
//                     else
//                     {
//                         precision *= 10;
//
//                         switch(fmt[i + 2])
//                         {
//                             case Char { '0' }: precision += 0; break;
//                             case Char { '1' }: precision += 1; break;
//                             case Char { '2' }: precision += 2; break;
//                             case Char { '3' }: precision += 3; break;
//                             case Char { '4' }: precision += 4; break;
//                             case Char { '5' }: precision += 5; break;
//                             case Char { '6' }: precision += 6; break;
//                             case Char { '7' }: precision += 7; break;
//                             case Char { '8' }: precision += 8; break;
//                             case Char { '9' }: precision += 9; break;
//                             default: return count;
//                         }
//
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg, PrintFloatFormat::Scientific, precision);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                         else
//                         {
//                             const auto ctx1 = ctx0.Handler(currArg);
//                             return InternalFormat(ctx1, fmt + i + 5, length - i - 5, args...);
//                         }
//                     }
//                 }
//             }
//         }
//         else if(fmt[i] == Char { '\'' })
//         {
//             if(fmt[i + 1] == Char { '\'' })
//             {
//                 const auto ctx = context.HandlerLength(fmt + blockBegin, i - blockBegin);
//                 return InternalFormatConstEval(ctx, fmt + 1, length - i - 1, currArg, args...);
//                 // blockBegin = i + 2;
//                 // ++count;
//                 // ++i;
//             }
//             else if(fmt[i + 1] == Char { '{' })
//             {
//                 if(fmt[i + 2] == Char { '\'' })
//                 {
//                     const auto ctx0 = context.HandlerLength(fmt + blockBegin, i - blockBegin);
//                     const auto ctx1 = ctx0.Handler(Char { '{' });
//                     return InternalFormatConstEval(ctx1, fmt + 2, length - i - 2, currArg, args...);
//                     // blockBegin = i + 3;
//                     // ++count;
//                     // i += 2;
//                 }
//             }
//             else
//             {
//                 return context;
//             }
//         }
//         else
//         {
//             ++count;
//         }
//     }
//
//     return context;
// }
//
// template<typename Context, typename Char, iSys FmtLen, typename CurrArg, typename... Args>
// inline constexpr auto InternalFormatConstEval0(Context& context, const Char(&fmt)[FmtLen], CurrArg currArg, const Args&... args) noexcept
// {
//     return InternalFormatConstEval(context, fmt, FmtLen - 1, currArg, args...);
// }
//
// template<typename Context, typename Char, iSys FmtLen, typename CurrArg, typename... Args>
// inline constexpr auto InternalFormatConstEval0(Context& context, const ConstExprStackStringT<Char, FmtLen> fmt, CurrArg currArg, const Args&... args) noexcept
// {
//     return InternalFormatConstEval(context, fmt.String(), FmtLen - 1, currArg, args...);
// }

template<typename Context, typename Char>
inline constexpr u32 InternalFormat(Context& context, const Char* fmt, const iSys length) noexcept
{
    return context.HandlerLength(fmt, length);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline constexpr u32 InternalFormat(Context& context, const Char* fmt, const iSys length, CurrArg currArg, const Args&... args) noexcept
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
inline constexpr u32 InternalFormat0(Context& context, const Char* fmt, CurrArg currArg, const Args&... args) noexcept
{
    return InternalFormat(context, fmt, strLength(fmt), currArg, args...);
}

template<typename Context, typename Char, typename CurrArg, typename... Args>
inline constexpr u32 InternalFormat0(Context& context, const StringBaseT<Char>& fmt, CurrArg currArg, const Args&... args) noexcept
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
    u32 constexpr Handler(const T* const p) noexcept
    {
        CharOut buffer[sizeof(uPtr) * 2 + 1];
        (void) ::tau::XtoAP < true, uPtr, CharOut, CharOut { '0' } > (::std::bit_cast<uPtr>(p), buffer);
        Builder.Append(buffer, sizeof(uPtr) * 2);
        return sizeof(uPtr) * 2;
    }

    template<typename Int>
    u32 constexpr HandlerIntPad0(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoAP0(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<typename Int>
    u32 constexpr HandlerIntPadS(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoAPS(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int>
    u32 constexpr HandlerHex(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoA<Uppercase>(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int, CharOut PadChar = ' '>
    u32 constexpr HandlerHexPad(const Int d) noexcept
    {
        CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoAP<Uppercase, Int, CharOut, PadChar>(d, buffer);
        Builder.Append(buffer);
        return writeLen;
    }

    template<bool Uppercase, typename Int>
    u32 constexpr HandlerHexPad0(const Int d) noexcept
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

    constexpr u32 Handler(const f32 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) noexcept
    {
        CharOut buffer[192];
        const uSys writeLen = PrintFloat32(buffer, ::std::size(buffer), f, format, precision);
        Builder.Append(buffer);
        return writeLen;
    }

    constexpr u32 Handler(const f64 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) noexcept
    {
        CharOut buffer[256];
        const uSys writeLen = PrintFloat64(buffer, ::std::size(buffer), f, format, precision);
        Builder.Append(buffer);
        return writeLen;
    }

    StringBuilderT<CharOut> Builder;
};

// template<typename CharOut, iSys Len>
// struct ConstEvalStringFormatContext final
// {
//     DEFAULT_CONSTRUCT_PUC(ConstEvalStringFormatContext);
//     DEFAULT_DESTRUCT_C(ConstEvalStringFormatContext);
//     DEFAULT_CM_PUC(ConstEvalStringFormatContext);
// public:
//     constexpr ConstEvalStringFormatContext(const ConstEvalStringBuilderT<CharOut, Len>& builder) noexcept
//         : Builder(builder)
//     { }
//
//     constexpr ConstEvalStringFormatContext(ConstEvalStringBuilderT<CharOut, Len>&& builder) noexcept
//         : Builder(::std::move(builder))
//     { }
//
//     template<typename Char>
//     consteval auto HandlerLength(const Char* const str, const iSys length) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str, length));
//     }
//
//     consteval auto Handler(const char c) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(c));
//     }
//
//     consteval auto Handler(const wchar_t c) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(c));
//     }
//
//     consteval auto Handler(const c8 c) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(c));
//     }
//
//     consteval auto Handler(const c16 c) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(c));
//     }
//
//     consteval auto Handler(const c32 c) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(c));
//     }
//
//     consteval auto Handler(const char* const str) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str));
//     }
//
//     consteval auto Handler(const wchar_t* const str) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str));
//     }
//
//     consteval auto Handler(const c8* const str) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str));
//     }
//
//     consteval auto Handler(const c16* const str) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str));
//     }
//
//     consteval auto Handler(const c32* const str) const noexcept
//     {
//         return ConstEvalStringFormatContext(Builder.Append(str));
//     }
//
//     consteval auto Handler(const bool b) const noexcept
//     {
//         if(b)
//         {
//             constexpr c8 str[] = u8"true";
//             return ConstEvalStringFormatContext(Builder.Append(str));
//         }
//         else
//         {
//             constexpr c8 str[] = u8"false";
//             return ConstEvalStringFormatContext(Builder.Append(str));
//         }
//     }
//
//     template<typename Int>
//     inline consteval auto HandlerInt(const Int d) const noexcept
//     {
//         CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
//         ::tau::ItoA(d, buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<typename T>
//     consteval auto Handler(const T* const p) const noexcept
//     {
//         CharOut buffer[sizeof(uPtr) * 2 + 1];
//         (void) ::tau::XtoAP<true, uPtr, CharOut, CharOut { '0' }> (::std::bit_cast<uPtr>(p), buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<typename Int>
//     consteval auto HandlerIntPad0(const Int d) const noexcept
//     {
//         CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
//         ::tau::ItoAP0(d, buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<typename Int>
//     consteval auto HandlerIntPadS(const Int d) const noexcept
//     {
//         CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
//         ::tau::ItoAPS(d, buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<bool Uppercase, typename Int>
//     consteval auto HandlerHex(const Int d) const noexcept
//     {
//         CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
//         ::tau::XtoA<Uppercase>(d, buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<bool Uppercase, typename Int, CharOut PadChar = ' '>
//     consteval auto HandlerHexPad(const Int d) const noexcept
//     {
//         CharOut buffer[::tau::MaxCharCount<Int>::Value + 1];
//         ::tau::XtoAP<Uppercase, Int, CharOut, PadChar>(d, buffer);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     template<bool Uppercase, typename Int>
//     consteval auto HandlerHexPad0(const Int d) const noexcept
//     {
//         return ConstEvalStringFormatContext(HandlerHexPad<Uppercase, Int, '0'>(d));
//     }
//
//     consteval auto Handler(const i8  d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const i16 d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const i32 d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const i64 d) const noexcept { return HandlerInt(d); }
//
//     consteval auto Handler(const u8  d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const u16 d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const u32 d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const u64 d) const noexcept { return HandlerInt(d); }
//
//     consteval auto Handler(const          long d) const noexcept { return HandlerInt(d); }
//     consteval auto Handler(const unsigned long d) const noexcept { return HandlerInt(d); }
//
//     consteval auto Handler(const f32 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) const noexcept
//     {
//         CharOut buffer[192];
//         PrintFloat32(buffer, ::std::size(buffer), f, format, precision);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     consteval auto Handler(const f64 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) const noexcept
//     {
//         CharOut buffer[256];
//         PrintFloat64(buffer, ::std::size(buffer), f, format, precision);
//         return ConstEvalStringFormatContext(Builder.Append(buffer));
//     }
//
//     ConstEvalStringBuilderT<CharOut, Len> Builder;
// };

template<typename CharOut, typename CharIn, typename CurrArg, typename... Args>
inline constexpr DynStringT<CharOut> Format(const CharIn* fmt, CurrArg currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToString();
}

template<typename CharOut, typename CharIn, typename CurrArg, typename... Args>
inline constexpr DynStringT<CharOut> Format(const StringBaseT<CharIn>& fmt, CurrArg currArg, const Args&... args) noexcept
{
    StringFormatContext<CharOut> context;
    InternalFormat0(context, fmt, currArg, args...);
    return context.Builder.ToString();
}

// template<typename CharOut, typename CharIn, iSys FmtLen, typename CurrArg, typename... Args>
// inline consteval auto FormatConstEval(const CharIn(&fmt)[FmtLen], CurrArg currArg, const Args&... args) noexcept
// {
//     ConstEvalStringFormatContext<CharOut, 0> context;
//     const auto ctx0 = InternalFormatConstEval0(context, fmt, currArg, args...);
//     return ctx0.Builder.ToString();
// }
//
// template<typename CharOut, typename CharIn, iSys FmtLen, typename CurrArg, typename... Args>
// inline consteval auto FormatConstEval(const ConstExprStackStringT<CharIn, FmtLen>& fmt, CurrArg currArg, const Args&... args) noexcept
// {
//     ConstEvalStringFormatContext<CharOut, 0> context;
//     const auto ctx0 = InternalFormatConstEval0(context, fmt, currArg, args...);
//     return ctx0.Builder.ToString();
// }

