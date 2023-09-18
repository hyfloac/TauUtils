#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "String.hpp"
#include "Console.hpp"
#include "ToString.hpp"
#include "Dragon4.hpp"
#include "StringFormat.hpp"

namespace ConPrinter {

inline u32 Print(const char c)    noexcept { return Console::Write(c); }
inline u32 Print(const wchar_t c) noexcept { return Console::Write(c); }
inline u32 Print(const c8 c)      noexcept { return Console::Write(c); }
inline u32 Print(const c16 c)     noexcept { return Console::Write(c); }
inline u32 Print(const c32 c)     noexcept { return Console::Write(c); }

inline u32 PrintLength(const char* const str, const uSys length)    noexcept { return Console::Write(str, length); }
inline u32 PrintLength(const wchar_t* const str, const uSys length) noexcept { return Console::Write(str, length); }
inline u32 PrintLength(const c8* const str, const uSys length)      noexcept { return Console::Write(str, length); }
inline u32 PrintLength(const c16* const str, const uSys length)     noexcept { return Console::Write(str, length); }
inline u32 PrintLength(const c32* const str, const uSys length)     noexcept { return Console::Write(str, length); }

inline u32 Print(const bool b) noexcept
{
    if(b)
    {
        constexpr c16 str[] = u"true";
        return Console::Write(str, cexpr::strlen(str));
    }
    else
    {
        constexpr c16 str[] = u"false";
        return Console::Write(str, cexpr::strlen(str));
    }
}

template<typename Int>
inline u32 PrintInt(const Int d) noexcept
{
    c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
    const i32 writeLen = ::tau::ItoA(d, buffer);
    return Console::Write(buffer, writeLen);
}

template<typename Int>
inline u32 PrintIntPad0(const Int d) noexcept
{
    c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
    const i32 writeLen = ::tau::ItoAP0(d, buffer);
    return Console::Write(buffer, writeLen);
}

template<typename Int>
inline u32 PrintIntPadS(const Int d) noexcept
{
    c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
    const i32 writeLen = ::tau::ItoAPS(d, buffer);
    return Console::Write(buffer, writeLen);
}

template<bool Uppercase, typename Int>
inline u32 PrintHex(const Int d) noexcept
{
    c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
    const i32 writeLen = ::tau::XtoA<Uppercase>(d, buffer);
    return Console::Write(buffer, writeLen);
}

template<bool Uppercase, typename Int, c16 PadChar = u' '>
inline u32 PrintHexPad(const Int d) noexcept
{
    c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
    const i32 writeLen = ::tau::XtoAP<Uppercase, Int, c16, PadChar>(d, buffer);
    return Console::Write(buffer, writeLen);
}

template<bool Uppercase, typename Int>
inline u32 PrintHexPad0(const Int d) noexcept
{
    return PrintHexPad<Uppercase, Int, u'0'>(d);
}

inline u32 Print(const i8  d) noexcept { return PrintInt(d); }
inline u32 Print(const i16 d) noexcept { return PrintInt(d); }
inline u32 Print(const i32 d) noexcept { return PrintInt(d); }
inline u32 Print(const i64 d) noexcept { return PrintInt(d); }

inline u32 Print(const u8  d) noexcept { return PrintInt(d); }
inline u32 Print(const u16 d) noexcept { return PrintInt(d); }
inline u32 Print(const u32 d) noexcept { return PrintInt(d); }
inline u32 Print(const u64 d) noexcept { return PrintInt(d); }

inline u32 Print(const          long d) noexcept { return PrintInt(d); }
inline u32 Print(const unsigned long d) noexcept { return PrintInt(d); }

inline u32 Print(const f32 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) noexcept
{
    c16 buffer[192];
    const uSys writeLen = PrintFloat32(buffer, ::std::size(buffer), f, format, precision);
    return Console::Write(buffer, writeLen);
}

inline u32 Print(const f64 f, const PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) noexcept
{
    c16 buffer[256];
    const uSys writeLen = PrintFloat64(buffer, ::std::size(buffer), f, format, precision);
    return Console::Write(buffer, writeLen);
}

template<typename T>
inline u32 Print(const T* const p) noexcept
{
    c16 buffer[sizeof(uPtr) * 2 + 1];
    (void) ::tau::XtoAP<true, uPtr, c16, u'0'>(reinterpret_cast<uPtr>(p), buffer);
    return Console::Write(buffer, sizeof(uPtr) * 2);
}

inline u32 Print(nullptr_t) noexcept
{
    c16 buffer[sizeof(uPtr) * 2 + 1];
    (void) ::tau::XtoAP<true, uPtr, c16, u'0'>(0, buffer);
    return Console::Write(buffer, sizeof(uPtr) * 2);
}

inline u32 Print(const char* const str)    noexcept
{
    if(!str)
    {
        return Print(reinterpret_cast<const void*>(str));
    }
    return Console::Write(str, strLength(str));
}

inline u32 Print(const wchar_t* const str) noexcept
{
    if(!str)
    {
        return Print(reinterpret_cast<const void*>(str));
    }
    return Console::Write(str, strLength(str));
}

inline u32 Print(const c8* const str)      noexcept
{
    if(!str)
    {
        return Print(reinterpret_cast<const void*>(str));
    }
    return Console::Write(str, strLength(str));
}

inline u32 Print(const c16* const str)     noexcept
{
    if(!str)
    {
        return Print(reinterpret_cast<const void*>(str));
    }
    return Console::Write(str, strLength(str));
}

inline u32 Print(const c32* const str)     noexcept
{
    if(!str)
    {
        return Print(reinterpret_cast<const void*>(str));
    }
    return Console::Write(str, strLength(str));
}

template<typename Char>
inline u32 Print(const ConstExprStringT<Char>& str) noexcept { return Console::Write(str.c_str(), str.length()); }
template<typename Char>
inline u32 Print(const DynStringT<Char>& str)       noexcept { return Console::Write(str.c_str(), str.length()); }
template<typename Char>
inline u32 Print(const DynStringViewT<Char>& str)   noexcept { return Console::Write(str.c_str(), str.length()); }

// template<typename Char, typename CurrArg, typename... Args>
// inline u32 Print(const Char* fmt, CurrArg currArg, const Args&... args) noexcept
// {
//     u32 count = 0;
//     iSys blockBegin = 0;
//
//     for(iSys i = 0; fmt[i]; ++i)
//     {
//         if(fmt[i] == Char{ '{' })
//         {
//             Console::Write(fmt + blockBegin, i - blockBegin);
//
//             if(fmt[i + 1] == Char{ '}' })
//             {
//                 count += Print(currArg);
//                 count += Print(fmt + i + 2, args...);
//                 break;
//             }
//             // Capital Hex
//             else if(fmt[i + 1] == Char{ 'X' })
//             {
//                 if(fmt[i + 2] == Char{ 'p' } || fmt[i + 2] == Char{ 'P' })
//                 {
//                     if(fmt[i + 3] == Char{ '0' })
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             count += PrintHexPad0<true>(currArg);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 5, args...);
//                     }
//                     else
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             count += PrintHexPad<true>(currArg);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 4, args...);
//                     }
//                     break;   
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         count += PrintHex<true>(currArg);
//                     }
//                     else
//                     {
//                         count += Print(currArg);
//                     }
//                     count += Print(fmt + i + 3, args...);
//                     break;
//                 }
//             }
//             // Lower Hex
//             else if(fmt[i + 1] == Char{ 'x' })
//             {
//                 if(fmt[i + 2] == Char{ 'p' } || fmt[i + 2] == Char{ 'P' })
//                 {
//                     if(fmt[i + 3] == Char{ '0' })
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             count += PrintHexPad0<false>(currArg);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 5, args...);
//                     }
//                     else
//                     {
//                         if constexpr(::std::is_integral_v<CurrArg>)
//                         {
//                             count += PrintHexPad<false>(currArg);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 4, args...);
//                     }
//                     break;   
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         count += PrintHex<false>(currArg);
//                     }
//                     else
//                     {
//                         count += Print(currArg);
//                     }
//                     count += Print(fmt + i + 3, args...);
//                     break;
//                 }
//             }
//             // Padded Int
//             else if(fmt[i + 1] == Char{ 'p' } || fmt[i + 1] == Char{ 'P' })
//             {
//                 if(fmt[i + 2] == Char{ '0' })
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         count += PrintIntPad0(currArg);
//                     }
//                     else
//                     {
//                         count += Print(currArg);
//                     }
//                     count += Print(fmt + i + 4, args...);
//                 }
//                 else
//                 {
//                     if constexpr(::std::is_integral_v<CurrArg>)
//                     {
//                         count += PrintIntPadS(currArg);
//                     }
//                     else
//                     {
//                         count += Print(currArg);
//                     }
//                     count += Print(fmt + i + 3, args...);
//                 }
//                 break;
//             }
//             // Float
//             else if(fmt[i + 1] == Char{ 'f' })
//             {
//                 // Simple float. Essentially the same as not having a format specifier.
//                 if(fmt[i + 2] == Char{ '}' })
//                 {
//                     count += Print(currArg);
//                     count += Print(fmt + i + 3, args...);
//                     break;
//                 }
//                 else
//                 {
//                     i32 precision = 0;
//
//                     switch(fmt[i + 2])
//                     {
//                         case Char{ '0' }: precision = 0; break;
//                         case Char{ '1' }: precision = 1; break;
//                         case Char{ '2' }: precision = 2; break;
//                         case Char{ '3' }: precision = 3; break;
//                         case Char{ '4' }: precision = 4; break;
//                         case Char{ '5' }: precision = 5; break;
//                         case Char{ '6' }: precision = 6; break;
//                         case Char{ '7' }: precision = 7; break;
//                         case Char{ '8' }: precision = 8; break;
//                         case Char{ '9' }: precision = 9; break;
//                         default: return count;
//                     }
//
//                     // Single Precision Specifier.
//                     if(fmt[i + 3] == Char{ '}' })
//                     {
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             count += Print(currArg, PrintFloatFormat::Positional, precision);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 4, args...);
//                     }
//                     // Dual Precision Specifier.
//                     else
//                     {
//                         precision *= 10;
//
//                         switch(fmt[i + 2])
//                         {
//                             case Char{ '0' }: precision += 0; break;
//                             case Char{ '1' }: precision += 1; break;
//                             case Char{ '2' }: precision += 2; break;
//                             case Char{ '3' }: precision += 3; break;
//                             case Char{ '4' }: precision += 4; break;
//                             case Char{ '5' }: precision += 5; break;
//                             case Char{ '6' }: precision += 6; break;
//                             case Char{ '7' }: precision += 7; break;
//                             case Char{ '8' }: precision += 8; break;
//                             case Char{ '9' }: precision += 9; break;
//                             default: return count;
//                         }
//
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             count += Print(currArg, PrintFloatFormat::Positional, precision);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 5, args...);
//                     }
//                 }
//             }
//             // Scientific Float
//             else if(fmt[i + 1] == Char{ 'e' })
//             {
//                 // Scientific float.
//                 if(fmt[i + 2] == Char{ '}' })
//                 {
//                     if constexpr(::std::is_floating_point_v<CurrArg>)
//                     {
//                         count += Print(currArg, PrintFloatFormat::Scientific);
//                     }
//                     else
//                     {
//                         count += Print(currArg);
//                     }
//                     count += Print(fmt + i + 3, args...);
//                     break;
//                 }
//                 else
//                 {
//                     i32 precision = 0;
//
//                     switch(fmt[i + 2])
//                     {
//                         case Char{ '0' }: precision = 0; break;
//                         case Char{ '1' }: precision = 1; break;
//                         case Char{ '2' }: precision = 2; break;
//                         case Char{ '3' }: precision = 3; break;
//                         case Char{ '4' }: precision = 4; break;
//                         case Char{ '5' }: precision = 5; break;
//                         case Char{ '6' }: precision = 6; break;
//                         case Char{ '7' }: precision = 7; break;
//                         case Char{ '8' }: precision = 8; break;
//                         case Char{ '9' }: precision = 9; break;
//                         default: return count;
//                     }
//
//                     // Single Precision Specifier.
//                     if(fmt[i + 3] == Char{ '}' })
//                     {
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             count += Print(currArg, PrintFloatFormat::Scientific, precision);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 4, args...);
//                     }
//                     // Dual Precision Specifier.
//                     else
//                     {
//                         precision *= 10;
//
//                         switch(fmt[i + 2])
//                         {
//                             case Char{ '0' }: precision += 0; break;
//                             case Char{ '1' }: precision += 1; break;
//                             case Char{ '2' }: precision += 2; break;
//                             case Char{ '3' }: precision += 3; break;
//                             case Char{ '4' }: precision += 4; break;
//                             case Char{ '5' }: precision += 5; break;
//                             case Char{ '6' }: precision += 6; break;
//                             case Char{ '7' }: precision += 7; break;
//                             case Char{ '8' }: precision += 8; break;
//                             case Char{ '9' }: precision += 9; break;
//                             default: return count;
//                         }
//
//                         if constexpr(::std::is_floating_point_v<CurrArg>)
//                         {
//                             count += Print(currArg, PrintFloatFormat::Scientific, precision);
//                         }
//                         else
//                         {
//                             count += Print(currArg);
//                         }
//                         count += Print(fmt + i + 5, args...);
//                     }
//                 }
//             }
//         }
//         else if(fmt[i] == Char{ '\'' })
//         {
//             if(fmt[i + 1] == Char{ '\'' })
//             {
//                 Console::Write(fmt + blockBegin, i - blockBegin);
//                 blockBegin = i + 2;
//                 ++count;
//                 ++i;
//             }
//             else if(fmt[i + 1] == Char{ '{' })
//             {
//                 if(fmt[i + 2] == Char{ '\'' })
//                 {
//                     Console::Write(fmt + blockBegin, i - blockBegin);
//                     Console::Write(u'{');
//                     blockBegin = i + 3;
//                     ++count;
//                     i += 2;
//                 }
//             }
//             else
//             {
//                 return count;
//             }
//         }
//         else
//         {
//             ++count;
//         }
//     }
//
//     return count;
// }

struct ConPrinterFormatContext final
{
    template<typename T>
    u32 Handler(const T& t) noexcept
    { return ::ConPrinter::Print(t); }

    template<typename Char>
    u32 HandlerLength(const Char* const str, const uSys length) noexcept
    { return ::ConPrinter::PrintLength(str, length); }

    template<typename Int>
    u32 HandlerIntPad0(const Int t) noexcept
    { return ::ConPrinter::PrintIntPad0(t); }

    template<typename Int>
    u32 HandlerIntPadS(const Int t) noexcept
    { return ::ConPrinter::PrintIntPadS(t); }

    template<bool Uppercase, typename Int>
    u32 HandlerHex(const Int t) noexcept
    { return ::ConPrinter::PrintHex<Uppercase>(t); }

    template<bool Uppercase, typename Int, c16 PadChar = u' '>
    u32 HandlerHexPad(const Int t) noexcept
    { return ::ConPrinter::PrintHexPad<Uppercase, Int, PadChar>(t); }

    template<bool Uppercase, typename Int>
    u32 HandlerHexPad0(const Int t) noexcept
    { return ::ConPrinter::PrintHexPad0<Uppercase>(t); }

    u32 Handler(const f32 f, const PrintFloatFormat format, const i32 precision = 6) noexcept
    { return ::ConPrinter::Print(f, format, precision); }

    u32 Handler(const f64 f, const PrintFloatFormat format, const i32 precision = 17) noexcept
    { return ::ConPrinter::Print(f, format, precision); }
};

template<typename Char, typename CurrArg, typename... Args>
inline u32 Print(const Char* fmt, const CurrArg& currArg, const Args&... args) noexcept
{
    ConPrinterFormatContext ctx;
    return InternalFormat0(ctx, fmt, currArg, args...);
}

template<typename... Args>
inline u32 PrintLn(const Args&... args) noexcept
{
    const u32 ret = Print(args...);
    (void) Print(u'\n');
    return ret + 1;
}

inline u32 PrintLn() noexcept
{
    return Print(u'\n');
}

}
