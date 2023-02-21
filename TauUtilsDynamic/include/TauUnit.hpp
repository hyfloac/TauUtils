#pragma once

#include "ConPrinter.hpp"
#include "Objects.hpp"
#include "String.hpp"

namespace tau {

class TestContainer final
{
    DEFAULT_CONSTRUCT_PI(TestContainer);
    DEFAULT_DESTRUCT(TestContainer);
    DEFAULT_CM_PU(TestContainer);
public:
    static TestContainer& Instance() noexcept
    {
        static TestContainer container;
        return container;
    }
public:
    [[nodiscard]] uSys TotalTestsRun()    const noexcept { return m_TotalTestsRun; }
    [[nodiscard]] uSys TotalTestsPassed() const noexcept { return m_TotalTestsPassed; }
    [[nodiscard]] uSys LastUnitTestsRun()    const noexcept { return m_LastUnitTestsRun; }
    [[nodiscard]] uSys LastUnitTestsPassed() const noexcept { return m_LastUnitTestsPassed; }

    void ResetUnit() noexcept
    {
        m_LastUnitTestsRun = 0;
        m_LastUnitTestsPassed = 0;
    }

    void BeginTest() noexcept
    {
        ++m_TotalTestsRun;
        ++m_LastUnitTestsRun;
    }

    void PassTest() noexcept
    {
        ++m_TotalTestsPassed;
        ++m_LastUnitTestsPassed;
    }

    void FailTest() noexcept
    { }

    void PrintTotals() noexcept
    {
        const uSys testsFailed = m_TotalTestsRun - m_TotalTestsPassed;

        ConPrinter::PrintLn(u"Total Tests Executed: {}", m_TotalTestsRun);
        ConPrinter::PrintLn(u"Total Tests Passed: \x1B[92m{}\x1B[0m", m_TotalTestsPassed);
        ConPrinter::PrintLn(u"Total Tests Failed: \x1B[91m{}\x1B[0m", testsFailed);
        ConPrinter::PrintLn();
    }
private:
    uSys m_TotalTestsRun;
    uSys m_TotalTestsPassed;

    uSys m_LastUnitTestsRun;
    uSys m_LastUnitTestsPassed;
};

class UnitTest final
{
    DELETE_CM(UnitTest);
public:
    UnitTest(const C8DynString& testName) noexcept
        : m_TestName(testName)
    {
        Init();
    }

    UnitTest(C8DynString&& testName) noexcept
        : m_TestName(::std::move(testName))
    {
        Init();
    }

    UnitTest(const c8* const str) noexcept
        : m_TestName(str)
    {
        Init();
    }

    UnitTest(const c16* const str) noexcept
        : m_TestName(StringCast<c8>(C16DynString(str)))
    {
        Init();
    }

    UnitTest(const c32* const str) noexcept
        : m_TestName(StringCast<c8>(C32DynString(str)))
    {
        Init();
    }

    ~UnitTest() noexcept
    {
        ConPrinter::PrintLn(u"\x1B[96mEnd\x1B[0m {}", m_TestName);

        const uSys testsRun = TestContainer::Instance().LastUnitTestsRun();
        const uSys testsPassed = TestContainer::Instance().LastUnitTestsPassed();
        const uSys testsFailed = testsRun - testsPassed;

        ConPrinter::PrintLn(u"Tests Executed: {}", testsRun);
        ConPrinter::PrintLn(u"Tests Passed: \x1B[92m{}\x1B[0m", testsPassed);
        ConPrinter::PrintLn(u"Tests Failed: \x1B[91m{}\x1B[0m", testsFailed);
        ConPrinter::PrintLn();

    }
private:
    void Init() noexcept
    {
        ConPrinter::PrintLn("\x1B[96mStart\x1B[0m {}", m_TestName);
    }
private:
    C8DynString m_TestName;
};

}

#define TAU_UNIT_TEST() \
    ::tau::TestContainer::Instance().ResetUnit(); \
    ::tau::UnitTest _0_unit_test(reinterpret_cast<const c8*>(__func__))

#define TAU_UNIT_EXP(VALUE, EXPECTED, EXP, ...) \
    do { \
        ::tau::TestContainer::Instance().BeginTest(); \
        if((EXP)) { \
            ::tau::TestContainer::Instance().PassTest(); \
        } else { \
            ConPrinter::Print(u"\x1B[91m[Error]\x1B[0m: "); \
            ConPrinter::PrintLn(__VA_ARGS__, #EXP); \
            ConPrinter::PrintLn(u"    Value: {}, Expected: {}", (VALUE), (EXPECTED)); \
        } \
    } while(0)

#define TAU_UNIT_EQ(VALUE, EXPECTED, ...)  TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) == (EXPECTED), __VA_ARGS__)
#define TAU_UNIT_NEQ(VALUE, EXPECTED, ...) TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) != (EXPECTED), __VA_ARGS__)
#define TAU_UNIT_GT(VALUE, EXPECTED, ...)  TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) > (EXPECTED), __VA_ARGS__)
#define TAU_UNIT_LT(VALUE, EXPECTED, ...)  TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) < (EXPECTED), __VA_ARGS__)
#define TAU_UNIT_GTE(VALUE, EXPECTED, ...) TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) >= (EXPECTED), __VA_ARGS__)
#define TAU_UNIT_LTE(VALUE, EXPECTED, ...) TAU_UNIT_EXP(VALUE, EXPECTED, (VALUE) <= (EXPECTED), __VA_ARGS__)
