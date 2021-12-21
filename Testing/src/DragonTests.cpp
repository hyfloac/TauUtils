#include <String.hpp>
#include <ConPrinter.hpp>
#include <Console.hpp>
#include <Dragon4.hpp>
#include <bit>
#include <cmath>
#include <string>

static void PrintCommonFloats()
{
    ConPrinter::Print(u"Begin PrintCommonFloats\n");

    c16 buffer[32];

    {
        constexpr f32 f = 1.0f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::Print(u"1.0 as float: {}\n", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = -1.0f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::Print(u"-1.0 as float: {}\n", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 3.141592653589794f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::Print(u"Pi as float: {}\n", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 2.7182818284590452353602874713526624977572470936999595749669676277f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::Print(u"e as float: {}\n", static_cast<const c16*>(buffer));
    }

    ConPrinter::Print(u"End PrintCommonFloats\n");
}

static void PrintCommonDoubles()
{
    ConPrinter::Print(u"Begin PrintCommonDoubles\n");

    wchar_t buffer[64];

    {
        constexpr f64 f = 1.0;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::Print(u"1.0 as double: {}\n", static_cast<const wchar_t*>(buffer));
    }

    {
        constexpr f64 f = -1.0;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::Print(u"-1.0 as double: %{}\n", static_cast<const wchar_t*>(buffer));
    }

    {
        constexpr f64 f = 3.1415926535897932384626433832795028841971693993751058209749445923;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::Print(u"Pi as double: {}\n", static_cast<const wchar_t*>(buffer));
    }

    {
        constexpr f64 f = 2.7182818284590452353602874713526624977572470936999595749669676277;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::Print(u"e as double: {}\n", static_cast<const wchar_t*>(buffer));
    }

    ConPrinter::Print(u"End PrintCommonDoubles\n");
}

static void TestAllFloats()
{
    ConPrinter::Print(u"Begin TestAllFloats\n");

    char buffer[192];

    u32 successCount = 0;
    u32 failCount = 0;
    u32 zeroFailCount = 0;
    u32 infNanCount = 0;

    for(u64 i = 0; i <= 0xFFFFFFFF; ++i)
    {
        if(((i >> 23) & 0xFF) == 0)
        {
            i |= 1 << 23;
        }

        const u32 i32Val = static_cast<u32>(i);
        const f32 fVal = ::std::bit_cast<f32>(i32Val);

        if(i32Val % 1000000 == 0)
        {
            ConPrinter::Print(u"Progress: {}\n", i32Val);
            ConPrinter::Print(u"Successes: {}, Failures: {}, Zero Failures: {}, Inf/NaN: {}, Total: {XP}\n", successCount, failCount, zeroFailCount, infNanCount, successCount + failCount + zeroFailCount + infNanCount);

        }

        if(::std::isinf(fVal) || ::std::isnan(fVal))
        {
            ++infNanCount;
            continue;
        }

        PrintFloat32(buffer, std::size(buffer), fVal, PrintFloatFormat::Positional, -static_cast<i32>(std::size(buffer)));

        const f32 roundTrip = ::std::strtof(buffer, nullptr);

        // ConPrinter::printW(L"FVal: %\n", static_cast<const char*>(buffer));

        if(roundTrip != fVal)
        {
            // if(roundTrip == 0.0)
            // {
            //     ++zeroFailCount;
            //     continue;
            // }

            ++failCount;

            ConPrinter::Print(u"Round trip conversion of {} failed. Round trip result: {}\n", static_cast<const char*>(buffer), roundTrip);
        }
        else
        {
            ++successCount;
        }
    }

    ConPrinter::Print(u"Successes: {}, Failures: {}, Zero Failures: {}, Inf/NaN: {}, Total: {XP}\n", successCount, failCount, zeroFailCount, infNanCount, successCount + failCount + zeroFailCount + infNanCount);

    ConPrinter::Print(u"End TestAllFloats\n");
}

void DragonTests()
{
    PrintCommonFloats();
    PrintCommonDoubles();
    TestAllFloats();
}
