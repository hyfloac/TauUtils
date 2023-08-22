#include <String.hpp>
#include <ConPrinter.hpp>
#include <Dragon4.hpp>
#include <TauUnit.hpp>
#include <bit>
#include <cmath>

static void PrintCommonFloats()
{
    TAU_UNIT_TEST();

    c16 buffer[32];

    {
        constexpr f32 f = 1.0f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"1.0 as float: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = -1.0f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"-1.0 as float: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 3.141592653589794f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"Pi as float: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 2.7182818284590452353602874713526624977572470936999595749669676277f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"e as float: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 12.3456789f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"12.3456789 as float: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f32 f = 0.123456789f;

        PrintFloat32(buffer, 32, f, PrintFloatFormat::Positional, -31);
        ConPrinter::PrintLn(u"0.123456789 as float: {}", static_cast<const c16*>(buffer));
    }
}

static void PrintCommonDoubles()
{
    TAU_UNIT_TEST();

    c16 buffer[64];

    {
        constexpr f64 f = 1.0;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"1.0 as double: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f64 f = -1.0;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"-1.0 as double: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f64 f = 3.1415926535897932384626433832795028841971693993751058209749445923;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"Pi as double: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f64 f = 2.7182818284590452353602874713526624977572470936999595749669676277;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"e as double: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f64 f = 12.3456789;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"12.3456789 as double: {}", static_cast<const c16*>(buffer));
    }

    {
        constexpr f64 f = 0.123456789;

        PrintFloat64(buffer, 64, f, PrintFloatFormat::Positional, -63);
        ConPrinter::PrintLn(u"0.123456789 as double: {}", static_cast<const c16*>(buffer));
    }
}

[[maybe_unused]] static void TestAllFloats()
{
    TAU_UNIT_TEST();

    char buffer[192];

    u32 successCount = 0;
    u32 failCount = 0;
    constexpr u32 zeroFailCount = 0;
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
            ConPrinter::PrintLn(u"Progress: {}", i32Val);
            ConPrinter::PrintLn(u"Successes: {}, Failures: {}, Zero Failures: {}, Inf/NaN: {}, Total: {XP}", successCount, failCount, zeroFailCount, infNanCount, successCount + failCount + zeroFailCount + infNanCount);

        }

        if(::std::isinf(fVal) || ::std::isnan(fVal))
        {
            ++infNanCount;
            continue;
        }

        PrintFloat32(buffer, std::size(buffer), fVal, PrintFloatFormat::Positional, -static_cast<i32>(std::size(buffer)));

        const f32 roundTrip = ::std::strtof(buffer, nullptr);

        // ConPrinter::printW(L"FVal: %", static_cast<const char*>(buffer));

        if(roundTrip != fVal)
        {
            // if(roundTrip == 0.0)
            // {
            //     ++zeroFailCount;
            //     continue;
            // }

            ++failCount;

            ConPrinter::PrintLn(u"Round trip conversion of {} failed. Round trip result: {}", static_cast<const char*>(buffer), roundTrip);
        }
        else
        {
            ++successCount;
        }
    }

    ConPrinter::PrintLn(u"Successes: {}, Failures: {}, Zero Failures: {}, Inf/NaN: {}, Total: {XP}", successCount, failCount, zeroFailCount, infNanCount, successCount + failCount + zeroFailCount + infNanCount);
}

static void PrintCommonFloatsScientific()
{
    TAU_UNIT_TEST();

    {
        constexpr f32 f = 1.0f;
        ConPrinter::PrintLn(u"1.0 as scientific float: {e}", f);
    }

    {
        constexpr f32 f = -1.0f;
        ConPrinter::PrintLn(u"-1.0 as scientific float: {e}", f);
    }

    {
        constexpr f32 f = 3.141592653589794f;
        ConPrinter::PrintLn(u"Pi as scientific float: {e}", f);
    }

    {
        constexpr f32 f = 2.7182818284590452353602874713526624977572470936999595749669676277f;
        ConPrinter::PrintLn(u"e as scientific float: {e}", f);
    }

    {
        constexpr f32 f = 12.3456789f;
        ConPrinter::PrintLn(u"12.3456789 as scientific float: {e}", f);
    }

    {
        constexpr f32 f = 0.123456789f;
        ConPrinter::PrintLn(u"0.123456789 as scientific float: {e}", f);
    }
}

static void PrintCommonDoublesScientific()
{
    TAU_UNIT_TEST();

    {
        constexpr f64 f = 1.0;
        ConPrinter::PrintLn(u"1.0 as scientific double: {e}", f);
    }

    {
        constexpr f64 f = -1.0;
        ConPrinter::PrintLn(u"-1.0 as scientific double: {e}", f);
    }

    {
        constexpr f64 f = 3.1415926535897932384626433832795028841971693993751058209749445923;
        ConPrinter::PrintLn(u"Pi as scientific double: {e}", f);
    }

    {
        constexpr f64 f = 2.7182818284590452353602874713526624977572470936999595749669676277;
        ConPrinter::PrintLn(u"e as scientific double: {e}", f);
    }

    {
        constexpr f64 f = 12.3456789;
        ConPrinter::PrintLn(u"12.3456789 as scientific double: {e}", f);
    }

    {
        constexpr f64 f = 0.123456789;
        ConPrinter::PrintLn(u"0.123456789 as scientific double: {e}", f);
    }
}

static void PrintCommonFloatsPrec5()
{
    {
        constexpr f32 f = 1.0f;
        ConPrinter::PrintLn(u"1.0 as float: {f5}", f);
    }

    {
        constexpr f32 f = -1.0f;
        ConPrinter::PrintLn(u"-1.0 as float: {f5}", f);
    }

    {
        constexpr f32 f = 3.141592653589794f;
        ConPrinter::PrintLn(u"Pi as float: {f5}", f);
    }

    {
        constexpr f32 f = 2.7182818284590452353602874713526624977572470936999595749669676277f;
        ConPrinter::PrintLn(u"e as float: {f5}", f);
    }

    {
        constexpr f32 f = 12.3456789f;
        ConPrinter::PrintLn(u"12.3456789 as float: {f5}", f);
    }

    {
        constexpr f32 f = 0.123456789f;
        ConPrinter::PrintLn(u"0.123456789 as scientific float: {f5}", f);
    }
}

static void PrintCommonDoublesPrec5()
{
    TAU_UNIT_TEST();

    {
        constexpr f64 f = 1.0;
        ConPrinter::PrintLn(u"1.0 as double: {f5}", f);
    }

    {
        constexpr f64 f = -1.0;
        ConPrinter::PrintLn(u"-1.0 as double: {f5}", f);
    }

    {
        constexpr f64 f = 3.1415926535897932384626433832795028841971693993751058209749445923;
        ConPrinter::PrintLn(u"Pi as double: {f5}", f);
    }

    {
        constexpr f64 f = 2.7182818284590452353602874713526624977572470936999595749669676277;
        ConPrinter::PrintLn(u"e as double: {f5}", f);
    }

    {
        constexpr f64 f = 12.3456789;
        ConPrinter::PrintLn(u"12.3456789 as double: {f5}", f);
    }

    {
        constexpr f64 f = 0.123456789;
        ConPrinter::PrintLn(u"0.123456789 as scientific double: {f5}", f);
    }
}

static void PrintCommonFloatsScientificPrec5()
{
    TAU_UNIT_TEST();

    {
        constexpr f32 f = 1.0f;
        ConPrinter::PrintLn(u"1.0 as scientific float: {e5}", f);
    }

    {
        constexpr f32 f = -1.0f;
        ConPrinter::PrintLn(u"-1.0 as scientific float: {e5}", f);
    }

    {
        constexpr f32 f = 3.141592653589794f;
        ConPrinter::PrintLn(u"Pi as scientific float: {e5}", f);
    }

    {
        constexpr f32 f = 2.7182818284590452353602874713526624977572470936999595749669676277f;
        ConPrinter::PrintLn(u"e as scientific float: {e5}",f);
    }

    {
        constexpr f32 f = 12.3456789f;
        ConPrinter::PrintLn(u"12.3456789 as scientific float: {e5}", f);
    }

    {
        constexpr f32 f = 0.123456789f;
        ConPrinter::PrintLn(u"0.123456789 as scientific float: {e5}", f);
    }
}

static void PrintCommonDoublesScientificPrec5()
{
    TAU_UNIT_TEST();

    {
        constexpr f64 f = 1.0;
        ConPrinter::PrintLn(u"1.0 as scientific double: {e5}", f);
    }

    {
        constexpr f64 f = -1.0;
        ConPrinter::PrintLn(u"-1.0 as scientific double: {e5}", f);
    }

    {
        constexpr f64 f = 3.1415926535897932384626433832795028841971693993751058209749445923;
        ConPrinter::PrintLn(u"Pi as scientific double: {e5}", f);
    }

    {
        constexpr f64 f = 2.7182818284590452353602874713526624977572470936999595749669676277;
        ConPrinter::PrintLn(u"e as scientific double: {e5}", f);
    }

    {
        constexpr f64 f = 12.3456789;
        ConPrinter::PrintLn(u"12.3456789 as scientific double: {e5}", f);
    }

    {
        constexpr f64 f = 0.123456789;
        ConPrinter::PrintLn(u"0.123456789 as scientific float: {e5}", f);
    }
}

void DragonTests()
{
    PrintCommonFloats();
    PrintCommonDoubles();
    PrintCommonFloatsScientific();
    PrintCommonDoublesScientific();
    PrintCommonFloatsPrec5();
    PrintCommonDoublesPrec5();
    PrintCommonFloatsScientificPrec5();
    PrintCommonDoublesScientificPrec5();
//    TestAllFloats();
}
