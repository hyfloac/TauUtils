#include <String.hpp>
#include <ConPrinter.hpp>
#include <Console.hpp>

static void TestIterateCodeUnits()
{
    ConPrinter::Print(u"Begin TestIterateCodeUnits\n");
    
    {
        const DynStringT<char> utfString("Hello, World! char\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<char> iter(utfString.beginUnits()); iter != utfString.endUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<wchar_t> utfString(L"Hello, World! wchar_t\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<wchar_t> iter(utfString.beginUnits()); iter != utfString.endUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<c8> utfString(u8"Hello, World! c8\n");
        
        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c8> iter(utfString.beginUnits()); iter != utfString.endUnits(); ++iter)
        {
            // ConPrinter::print(*iter);
        }
    }

    {
        const DynStringT<c16> utfString(u"Hello, World! c16\n");

        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c16> iter(utfString.beginUnits()); iter != utfString.endUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    {
        const DynStringT<c32> utfString(U"Hello, World! c32\n");

        ConPrinter::Print(utfString);
        // Console::Write(utfString.c_str(), utfString.length());

        for(DynStringCodeUnitIteratorT<c32> iter(utfString.beginUnits()); iter != utfString.endUnits(); ++iter)
        {
            // Console::print(*iter);
        }
    }

    ConPrinter::Print(u"End TestIterateCodeUnits\n");
}

void StringTests()
{
    TestIterateCodeUnits();
}
