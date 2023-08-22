#include <TUMaths.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

// static void TestValAddWithOverflow(const u32 a, const u32 b, const u32 expectedResult, const u32 expectedCarry)
// {
//     u32 carry;
//     const u32 result = AddReturnOverflowU32(a, b, &carry);
//     TAU_UNIT_EQ(result, expectedResult, "Adding 0x{X} [{}] and 0x{X} [{}] did not return the correct result of 0x{X} [{}], instead resulting in 0x{X} [{}].", a, a, b, b, expectedResult, expectedResult, result, result);
//     TAU_UNIT_EQ(carry, expectedCarry, "Adding 0x{X} [{}] and 0x{X} [{}] did not return the correct carry of [{}].", a, a, b, b, expectedCarry);
//     // ConPrinter::PrintLn("0x{X} [{}] + 0x{X} [{}] = 0x{X} [{}], with Carry {}.", a, a, b, b, result, result, carry);
// }

// static void TestAddWithOverflow()
// {
//     TAU_UNIT_TEST();
//
//     TestValAddWithOverflow(0x80000000, 0x80000000, 0x80000000 + 0x80000000, 1);
//     TestValAddWithOverflow(1, 1, 2, 0);
//     TestValAddWithOverflow(0, 0, 0, 0);
//     TestValAddWithOverflow(1, 2, 3, 0);
//     TestValAddWithOverflow(0x80001000, 2, 0x80001002, 0);
//     TestValAddWithOverflow(0x80001000, 0x80004000, 0x5000, 1);
//     TestValAddWithOverflow(0x8000A000, 0x80008000, 0x12000, 1);
// }

static void TestLeadingZerosU8()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x01)), 7, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x03)), 6, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x02)), 6, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x08)), 4, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x80)), 0, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x33)), 2, "Leading zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u8>(0x78)), 1, "Leading zeros (u8) were not correct. {}");
}

static void TestLeadingZerosU16()
{
    TAU_UNIT_TEST();
    
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0001)), 15, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0003)), 14, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0002)), 14, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0008)), 12, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0080)), 8, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0033)), 10, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0078)), 9, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0101)), 7, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0303)), 6, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0202)), 6, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x0808)), 4, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x8080)), 0, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x3333)), 2, "Leading zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u16>(0x7878)), 1, "Leading zeros (u16) were not correct. {}");
}

static void TestLeadingZerosU32()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0001)), 16 + 15, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0003)), 16 + 14, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0002)), 16 + 14, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0008)), 16 + 12, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0080)), 16 + 8,  "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0033)), 16 + 10, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0078)), 16 + 9, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0101)), 16 + 7, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0303)), 16 + 6, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0202)), 16 + 6, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x0808)), 16 + 4, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x8080)), 16 + 0, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x3333)), 16 + 2, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x7878)), 16 + 1, "Leading zeros (u32) were not correct. {}");

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00010001)), 15, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00030003)), 14, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00020002)), 14, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00080008)), 12, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00800080)), 8, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00330033)), 10, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x00780078)), 9, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x01010101)), 7, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x03030303)), 6, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x02020202)), 6, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x08080808)), 4, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x80808080)), 0, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x33333333)), 2, "Leading zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u32>(0x78787878)), 1, "Leading zeros (u32) were not correct. {}");
}

static void TestLeadingZerosU64()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0001)), 48 + 15, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0003)), 48 + 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0002)), 48 + 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0008)), 48 + 12, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0080)), 48 + 8,  "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0033)), 48 + 10, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0078)), 48 + 9, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0101)), 48 + 7, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0303)), 48 + 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0202)), 48 + 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0808)), 48 + 4, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x8080)), 48 + 0, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x3333)), 48 + 2, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x7878)), 48 + 1, "Leading zeros (u64) were not correct. {}");

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00010001)), 32 + 15, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00030003)), 32 + 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00020002)), 32 + 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00080008)), 32 + 12, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00800080)), 32 + 8,  "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00330033)), 32 + 10, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x00780078)), 32 + 9, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x01010101)), 32 + 7, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x03030303)), 32 + 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x02020202)), 32 + 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x08080808)), 32 + 4, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x80808080)), 32 + 0, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x33333333)), 32 + 2, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x78787878)), 32 + 1, "Leading zeros (u64) were not correct. {}");

    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0001000100010001ull)), 15, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0003000300030003ull)), 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0002000200020002ull)), 14, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0008000800080008ull)), 12, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0080008000800080ull)), 8, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0033003300330033ull)), 10, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0078007800780078ull)), 9, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0101010101010101ull)), 7, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0303030303030303ull)), 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0202020202020202ull)), 6, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x0808080808080808ull)), 4, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x8080808080808080ull)), 0, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x3333333333333333ull)), 2, "Leading zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountLeadingZeroC(static_cast<u64>(0x7878787878787878ull)), 1, "Leading zeros (u64) were not correct. {}");
}

static void TestTrailingZerosU8()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x01)), 0, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x03)), 0, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x02)), 1, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x08)), 3, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x80)), 7, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x33)), 0, "Trailing zeros (u8) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u8>(0x78)), 3, "Trailing zeros (u8) were not correct. {}");
}

static void TestTrailingZerosU16()
{
    TAU_UNIT_TEST();
    
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0001)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0003)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0002)), 1, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0008)), 3, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0080)), 7, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0033)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0078)), 3, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0101)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0303)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0202)), 1, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0808)), 3, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x8080)), 7, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x3333)), 0, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x7878)), 3, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0100)), 8, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0300)), 8, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0200)), 9, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x0800)), 11, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x8000)), 15, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x3300)), 8, "Trailing zeros (u16) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u16>(0x7800)), 11, "Trailing zeros (u16) were not correct. {}");
}

static void TestTrailingZerosU32()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0001)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0003)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0002)), 1, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0008)), 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0080)), 7, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0033)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0078)), 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0101)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0303)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0202)), 1, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0808)), 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x8080)), 7, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x3333)), 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x7878)), 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0100)), 8, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0300)), 8, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0200)), 9, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x0800)), 11, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x8000)), 15, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x3300)), 8,  "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x7800)), 11, "Trailing zeros (u32) were not correct. {}");

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00010000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00030000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00020000)), 16 + 1, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00080000)), 16 + 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00800000)), 16 + 7, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00330000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x00780000)), 16 + 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x01010000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x03030000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x02020000)), 16 + 1, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x08080000)), 16 + 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x80800000)), 16 + 7, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x33330000)), 16 + 0, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x78780000)), 16 + 3, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x01000000)), 16 + 8, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x03000000)), 16 + 8, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x02000000)), 16 + 9, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x08000000)), 16 + 11, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x80000000)), 16 + 15, "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x33000000)), 16 + 8,  "Trailing zeros (u32) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u32>(0x78000000)), 16 + 11, "Trailing zeros (u32) were not correct. {}");
}

static void TestTrailingZerosU64()
{
    TAU_UNIT_TEST();

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0001)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0003)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0002)), 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0008)), 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0080)), 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0033)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0078)), 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0101)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0303)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0202)), 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0808)), 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8080)), 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3333)), 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7878)), 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0100)), 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0300)), 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0200)), 9, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0800)), 11, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8000)), 15, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3300)), 8,  "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7800)), 11, "Trailing zeros (u64) were not correct. {}");

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00010000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00030000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00020000)), 16 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00080000)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00800000)), 16 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00330000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x00780000)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x01010000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x03030000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x02020000)), 16 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x08080000)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x80800000)), 16 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x33330000)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x78780000)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x01000000)), 16 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x03000000)), 16 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x02000000)), 16 + 9, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x08000000)), 16 + 11, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x80000000)), 16 + 15, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x33000000)), 16 + 8,  "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x78000000)), 16 + 11, "Trailing zeros (u64) were not correct. {}");

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0001000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0003000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0002000000000000ull)), 48 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0008000000000000ull)), 48 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0080000000000000ull)), 48 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0033000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0078000000000000ull)), 48 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0101000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0303000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0202000000000000ull)), 48 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0808000000000000ull)), 48 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8080000000000000ull)), 48 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3333000000000000ull)), 48 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7878000000000000ull)), 48 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0100000000000000ull)), 48 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0300000000000000ull)), 48 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0200000000000000ull)), 48 + 9, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0800000000000000ull)), 48 + 11, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8000000000000000ull)), 48 + 15, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3300000000000000ull)), 48 + 8,  "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7800000000000000ull)), 48 + 11, "Trailing zeros (u64) were not correct. {}");

    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0001000000010000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0003000000030000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0002000000020000ull)), 16 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0008000000080000ull)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0080000000800000ull)), 16 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0033000000330000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0078000000780000ull)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0101000001010000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0303000003030000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0202000002020000ull)), 16 + 1, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0808000008080000ull)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8080000080800000ull)), 16 + 7, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3333000033330000ull)), 16 + 0, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7878000078780000ull)), 16 + 3, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0100000001000000ull)), 16 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0300000003000000ull)), 16 + 8, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0200000002000000ull)), 16 + 9, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x0800000008000000ull)), 16 + 11, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x8000000080000000ull)), 16 + 15, "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x3300000033000000ull)), 16 + 8,  "Trailing zeros (u64) were not correct. {}");
    TAU_UNIT_EQ(CountTrailingZeroC(static_cast<u64>(0x7800000078000000ull)), 16 + 11, "Trailing zeros (u64) were not correct. {}");
}

void MathTests()
{
    // TestAddWithOverflow();
    TestLeadingZerosU8();
    TestLeadingZerosU16();
    TestLeadingZerosU32();
    TestLeadingZerosU64();
    TestTrailingZerosU8();
    TestTrailingZerosU16();
    TestTrailingZerosU32();
    TestTrailingZerosU64();
}
