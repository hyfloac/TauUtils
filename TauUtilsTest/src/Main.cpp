#include <Console.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

#include <DeBruijn.hpp>

extern void RefTests();
extern void StringTests();
extern void DragonTests();
extern void MathTests();
extern void Base64Tests();
extern void JsonTests();

int main(int argCount, char* args[]){
    Console::Create();
    Console::Init();
    StringTests();
    RefTests();
    // DragonTests();
    MathTests();
    Base64Tests();
    // JsonTests();

    ::tau::TestContainer::Instance().PrintTotals();

    // {
    //     ConPrinter::PrintLn();
    //     ConPrinter::PrintLn("DeBrujin:");
    //     const auto deBrujinMask = ::tau::ComputeBinaryDeBruijnSequence<u64>(6);
    //     ConPrinter::PrintLn("Mask: 0x{XP0}", deBrujinMask);
    //     auto table = ::tau::GenerateDeBruijnTable(deBrujinMask, 64, 58);
    //     for(auto x : table)
    //     {
    //         ConPrinter::Print("{}, ", x);
    //     }
    //     ConPrinter::PrintLn();
    // }
    
    return 0;
}
