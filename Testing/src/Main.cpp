#include <Console.hpp>

extern void RefTests();
extern void StringTests();
extern void DragonTests();
extern void MathTests();

int main(int argCount, char* args[])
{
    Console::Create();
    Console::Init();
    // StringTests();
    RefTests();
    // DragonTests();
    // MathTests();

    return 0;
}
