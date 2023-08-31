#include <Console.hpp>
#include <ConPrinter.hpp>

int main(int argCount, char* args[])
{
    Console::Create();
    Console::Init();

    ConPrinter::PrintLn("Test.");

    return 0;
}
