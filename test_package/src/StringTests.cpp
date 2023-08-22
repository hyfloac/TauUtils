#include <String.hpp>
#include <ConPrinter.hpp>
#include <ToString.hpp>
#include <TauUnit.hpp>

static void TestIterateCodeUnits()
{
    TAU_UNIT_TEST();

    {
        const DynStringT<char> utfString("Hello, World! char\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<char> iter(utfString.BeginUnits()); iter != utfString.EndUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<wchar_t> utfString(L"Hello, World! wchar_t\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<wchar_t> iter(utfString.BeginUnits()); iter != utfString.EndUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<c8> utfString(u8"Hello, World! c8\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c8> iter(utfString.BeginUnits()); iter != utfString.EndUnits(); ++iter)
        {
            // ConPrinter::print(*iter);
        }
    }

    {
        const DynStringT<c16> utfString(u"Hello, World! c16\n");

        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c16> iter(utfString.BeginUnits()); iter != utfString.EndUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<c32> utfString(U"Hello, World! c32\n");

        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c32> iter(utfString.BeginUnits()); iter != utfString.EndUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }
}

static void ToStringTests()
{
    TAU_UNIT_TEST();

    ConPrinter::PrintLn(tau::ToString<char>(12345));
    ConPrinter::PrintLn(tau::ToString<wchar_t>(12345));
    ConPrinter::PrintLn(tau::ToString<c8>(12345));
    ConPrinter::PrintLn(tau::ToString<c16>(12345));
    ConPrinter::PrintLn(tau::ToString<c32>(12345));

    ConPrinter::PrintLn(tau::ToString<char>(12345.456789));
    ConPrinter::PrintLn(tau::ToString<wchar_t>(12345.456789));
    ConPrinter::PrintLn(tau::ToString<c8>(12345.456789));
    ConPrinter::PrintLn(tau::ToString<c16>(12345.456789));
    ConPrinter::PrintLn(tau::ToString<c32>(12345.456789));

    ConPrinter::Print("{} ", tau::ToString<char>('c'));
    ConPrinter::Print("{} ", tau::ToString<wchar_t>('c'));
    ConPrinter::Print("{} ", tau::ToString<c8>('c'));
    ConPrinter::Print("{} ", tau::ToString<c16>('c'));
    ConPrinter::Print("{} ", tau::ToString<c32>('c'));

    ConPrinter::PrintLn();

    ConPrinter::Print("{} ", tau::ToString<char>(L'w'));
    ConPrinter::Print("{} ", tau::ToString<wchar_t>(L'w'));
    ConPrinter::Print("{} ", tau::ToString<c8>(L'w'));
    ConPrinter::Print("{} ", tau::ToString<c16>(L'w'));
    ConPrinter::Print("{} ", tau::ToString<c32>(L'w'));

    ConPrinter::PrintLn();

    ConPrinter::Print("{} ", tau::ToString<char>(u8'8'));
    ConPrinter::Print("{} ", tau::ToString<wchar_t>(u8'8'));
    ConPrinter::Print("{} ", tau::ToString<c8>(u8'8'));
    ConPrinter::Print("{} ", tau::ToString<c16>(u8'8'));
    ConPrinter::Print("{} ", tau::ToString<c32>(u8'8'));

    ConPrinter::PrintLn();

    ConPrinter::Print("{} ", tau::ToString<char>(u'H'));
    ConPrinter::Print("{} ", tau::ToString<wchar_t>(u'H'));
    ConPrinter::Print("{} ", tau::ToString<c8>(u'H'));
    ConPrinter::Print("{} ", tau::ToString<c16>(u'H'));
    ConPrinter::Print("{} ", tau::ToString<c32>(u'H'));

    ConPrinter::PrintLn();

    ConPrinter::Print("{} ", tau::ToString<char>(U'\U00010330'));
    ConPrinter::Print("{} ", tau::ToString<wchar_t>(U'\U00010330'));
    ConPrinter::Print("{} ", tau::ToString<c8>(U'\U00010330'));
    ConPrinter::Print("{} ", tau::ToString<c16>(U'\U00010330'));
    ConPrinter::Print("{} ", tau::ToString<c32>(U'\U00010330'));

    ConPrinter::PrintLn();
}

template<typename Char>
static void TestStringBuilderT(const Char* const helloWorld)
{
    TAU_UNIT_TEST();

    StringBuilderT<Char> builder;
    builder.Append(u8"Hello");
    builder.Append(u",");
    builder.Append(U" ");
    builder.Append("World");
    builder.Append(L"!");

    const DynStringT<Char> str = builder.toString();

    TAU_UNIT_EQ(str, helloWorld, "Appended string did not match a raw string literal.");

    ConPrinter::PrintLn("{}", str);
}

static void TestStringBuilder()
{
    TestStringBuilderT<c8>(u8"Hello, World!");
    TestStringBuilderT<c16>(u"Hello, World!");
    TestStringBuilderT<c32>(U"Hello, World!");
    TestStringBuilderT<char>("Hello, World!");
    TestStringBuilderT<wchar_t>(L"Hello, World!");
}

template<typename Char>
static void TestStringFormatT(const Char* const helloWorld)
{
    const DynStringT<Char> str = Format<Char>("{}{}{}{}{}{}", u8"Hello", u",", U" ", "World", 0, L"!");

    TAU_UNIT_EQ(str, helloWorld, "Appended string did not match a raw string literal.");

    ConPrinter::PrintLn("{}", str);
}

static void TestStringFormat()
{
    TAU_UNIT_TEST();

    {
        TestStringFormatT<c8>(u8"Hello, World0!");
        TestStringFormatT<c16>(u"Hello, World0!");
        TestStringFormatT<c32>(U"Hello, World0!");
        TestStringFormatT<char>("Hello, World0!");
        TestStringFormatT<wchar_t>(L"Hello, World0!");
    }
}

void StringTests()
{
    TestIterateCodeUnits();
    ToStringTests();
    TestStringBuilder();
    TestStringFormat();
}
