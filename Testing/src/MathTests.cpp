#include <TUMaths.hpp>
#include <ConPrinter.hpp>

static void TestValAddWithOverflow(const u32 a, const u32 b)
{
    u32 carry;
    const u32 result = AddReturnOverflowU32(a, b, &carry);
    ConPrinter::Print("0x{X} [{}] + 0x{X} [{}] = 0x{X} [{}], with Carry {}.\n", a, a, b, b, result, result, carry);
}

static void TestAddWithOverflow()
{
    ConPrinter::Print(u"Begin TestAddWithOverflow\n");

    TestValAddWithOverflow(0x80000000, 0x80000000);
    TestValAddWithOverflow(1, 1);
    TestValAddWithOverflow(0, 0);
    TestValAddWithOverflow(1, 2);
    TestValAddWithOverflow(0x80001000, 2);
    TestValAddWithOverflow(0x80001000, 0x80004000);

    ConPrinter::Print(u"End TestAddWithOverflow\n");
}

void MathTests()
{
    TestAddWithOverflow();
}
