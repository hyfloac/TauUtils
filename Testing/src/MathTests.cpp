#include <TUMaths.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

static void TestValAddWithOverflow(const u32 a, const u32 b, const u32 expectedResult, const u32 expectedCarry)
{
    u32 carry;
    const u32 result = AddReturnOverflowU32(a, b, &carry);
    TAU_UNIT_EQ(result, expectedResult, "Adding 0x{X} [{}] and 0x{X} [{}] did not return the correct result of 0x{X} [{}], instead resulting in 0x{X} [{}].", a, a, b, b, expectedResult, expectedResult, result, result);
    TAU_UNIT_EQ(carry, expectedCarry, "Adding 0x{X} [{}] and 0x{X} [{}] did not return the correct carry of [{}].", a, a, b, b, expectedCarry);
    // ConPrinter::PrintLn("0x{X} [{}] + 0x{X} [{}] = 0x{X} [{}], with Carry {}.", a, a, b, b, result, result, carry);
}

static void TestAddWithOverflow()
{
    TAU_UNIT_TEST();

    TestValAddWithOverflow(0x80000000, 0x80000000, 0x80000000 + 0x80000000, 1);
    TestValAddWithOverflow(1, 1, 2, 0);
    TestValAddWithOverflow(0, 0, 0, 0);
    TestValAddWithOverflow(1, 2, 3, 0);
    TestValAddWithOverflow(0x80001000, 2, 0x80001002, 0);
    TestValAddWithOverflow(0x80001000, 0x80004000, 0x5000, 1);
    TestValAddWithOverflow(0x8000A000, 0x80008000, 0x12000, 1);
}

void MathTests()
{
    TestAddWithOverflow();
}
