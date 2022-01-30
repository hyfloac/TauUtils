#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "String.hpp"
#include "Console.hpp"
#include "ToString.hpp"
#include "Dragon4.hpp"

template<typename T>
struct TPrinter final
{
    static i32 Print(const T& t) noexcept { return 0; }
};

class ConPrinter final
{
    DELETE_CONSTRUCT(ConPrinter);
    DELETE_DESTRUCT(ConPrinter);
    DELETE_CM(ConPrinter);
public:
    static u32 Print(const char c)    noexcept { return Console::Write(c); }
    static u32 Print(const wchar_t c) noexcept { return Console::Write(c); }
    static u32 Print(const c8 c)      noexcept { return Console::Write(c); }
    static u32 Print(const c16 c)     noexcept { return Console::Write(c); }
    static u32 Print(const c32 c)     noexcept { return Console::Write(c); }

    static u32 Print(const char* const str)    noexcept { return Console::Write(str, strLength(str)); }
    static u32 Print(const wchar_t* const str) noexcept { return Console::Write(str, strLength(str)); }
    static u32 Print(const c8* const str)      noexcept { return Console::Write(str, strLength(str)); }
    static u32 Print(const c16* const str)     noexcept { return Console::Write(str, strLength(str)); }
    static u32 Print(const c32* const str)     noexcept { return Console::Write(str, strLength(str)); }

    static u32 Print(const bool b) noexcept
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
    static u32 PrintInt(const Int d) noexcept
    {
        c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoA(d, buffer);
        return Console::Write(buffer, writeLen);
    }

    template<typename Int>
    static u32 PrintIntPad(const Int d) noexcept
    {
        c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::ItoAP(d, buffer);
        return Console::Write(buffer, writeLen);
    }

    template<bool Uppercase, typename Int>
    static u32 PrintHex(const Int d) noexcept
    {
        c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoA<Uppercase>(d, buffer);
        return Console::Write(buffer, writeLen);
    }

    template<bool Uppercase, typename Int>
    static u32 PrintHexPad(const Int d) noexcept
    {
        c16 buffer[::tau::MaxCharCount<Int>::Value + 1];
        const i32 writeLen = ::tau::XtoAP<Uppercase>(d, buffer);
        return Console::Write(buffer, writeLen);
    }

    static u32 Print(const i8  d) noexcept { return PrintInt(d); }
    static u32 Print(const i16 d) noexcept { return PrintInt(d); }
    static u32 Print(const i32 d) noexcept { return PrintInt(d); }
    static u32 Print(const i64 d) noexcept { return PrintInt(d); }

    static u32 Print(const u8  d) noexcept { return PrintInt(d); }
    static u32 Print(const u16 d) noexcept { return PrintInt(d); }
    static u32 Print(const u32 d) noexcept { return PrintInt(d); }
    static u32 Print(const u64 d) noexcept { return PrintInt(d); }
    
    static u32 Print(const f32 f, PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 6) noexcept
    {
        c16 buffer[192];
        const uSys writeLen = PrintFloat32(buffer, ::std::size(buffer), f, format, precision);
        return Console::Write(buffer, writeLen);
    }

    static u32 Print(const f64 f, PrintFloatFormat format = PrintFloatFormat::Positional, const i32 precision = 17) noexcept
    {
        c16 buffer[256];
        const uSys writeLen = PrintFloat64(buffer, ::std::size(buffer), f, format, precision);
        return Console::Write(buffer, writeLen);
    }
    
    template<typename T>
    static u32 print(const T* const p) noexcept
    {
        c16 buffer[sizeof(uPtr) * 2 + 1];
        (void) ::tau::XtoAP<true>(reinterpret_cast<uPtr>(p), buffer);
        return Console::Write(buffer, sizeof(uPtr) * 2);
    }

    template<typename Char>
    static u32 Print(const ConstExprStringT<Char>& str) noexcept { return Console::Write(str.c_str(), str.length()); }
    template<typename Char>
    static u32 Print(const DynStringT<Char>& str)       noexcept { return Console::Write(str.c_str(), str.length()); }
    template<typename Char>
    static u32 Print(const DynStringViewT<Char>& str)   noexcept { return Console::Write(str.c_str(), str.length()); }
    
    template<typename T>
    static u32 Print(const T& t) noexcept { return TPrinter<T>::Print(t); }
    
    template<typename Char, typename CurrArg, typename... Args>
    static u32 Print(const Char* fmt, CurrArg currArg, Args... args) noexcept
    {
        u32 count = 0;
        i32 blockBegin = 0;

        for(uSys i = 0; fmt[i]; ++i)
        {
            if(fmt[i] == Char{'{'})
            {
                Console::Write(fmt + blockBegin, i - blockBegin);

                if(fmt[i + 1] == Char{'}'})
                {
                    count += Print(currArg);
                    count += Print(fmt + i + 2, args...);
                    break;
                }
                else if(fmt[i + 1] == Char{'X'})
                {
                    if(fmt[i + 2] == Char{'p'} || fmt[i + 2] == Char{'P'})
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += PrintHexPad<true>(currArg);
                        }
                        else
                        {
                            count += Print(currArg);
                        }
                        count += Print(fmt + i + 4, args...);
                        break;   
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += PrintHex<true>(currArg);
                        }
                        else
                        {
                            count += Print(currArg);
                        }
                        count += Print(fmt + i + 3, args...);
                        break;
                    }
                }
                else if(fmt[i + 1] == Char{'x'})
                {
                    if(fmt[i + 2] == Char{'p'} || fmt[i + 2] == Char{'P'})
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += PrintHexPad<false>(currArg);
                        }
                        else
                        {
                            count += Print(currArg);
                        }
                        count += Print(fmt + i + 4, args...);
                        break;   
                    }
                    else
                    {
                        if constexpr(::std::is_integral_v<CurrArg>)
                        {
                            count += PrintHex<false>(currArg);
                        }
                        else
                        {
                            count += Print(currArg);
                        }
                        count += Print(fmt + i + 3, args...);
                        break;
                    }
                }
            }
            else if(fmt[i] == Char{'\''})
            {
                if(fmt[i + 1] == Char{'\''})
                {
                    Console::Write(fmt + blockBegin, i - blockBegin);
                    blockBegin = i + 2;
                    ++count;
                    ++i;
                }
                else if(fmt[i + 1] == Char{'{'})
                {
                    if(fmt[i + 2] == Char{'\''})
                    {
                        Console::Write(fmt + blockBegin, i - blockBegin);
                        Console::Write(u'{');
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
};
