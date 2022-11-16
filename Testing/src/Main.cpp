#include <Console.hpp>
#include <TauUnit.hpp>

extern void RefTests();
extern void StringTests();
extern void DragonTests();
extern void MathTests();
extern void Base64Tests();
extern void JsonTests();

int main(int argCount, char* args[])
{
    Console::Create();
    Console::Init();
    StringTests();
    // RefTests();
    // DragonTests();
    // MathTests();
    // Base64Tests();
    // JsonTests();

    ::tau::TestContainer::Instance().PrintTotals();

    return 0;
}
