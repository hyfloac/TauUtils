#include <Console.hpp>

extern void RefTests();
extern void StringTests();
extern void DragonTests();
extern void MathTests();
extern void Base64Tests();

int main(int argCount, char* args[])
{
    Console::Create();
    Console::Init();
    // StringTests();
    // RefTests();
    DragonTests();
    // MathTests();
    // Base64Tests();

    return 0;
}
