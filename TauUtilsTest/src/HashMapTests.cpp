#include <ds/HashTable.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>
#include <chrono>
#include <unordered_map>
#include <cstdlib>

struct DirectHash final
{
    uSys Key;

    DirectHash(const uSys key) noexcept
        : Key(key)
    { }

    [[nodiscard]] bool operator ==(const DirectHash& other) const noexcept
    {
        return Key == other.Key;
    }
};

template<>
struct std::hash<DirectHash> final {
    uSys operator()(const DirectHash& k) const noexcept
    {
        return k.Key;
    }
};

static void InsertTest() noexcept
{
    TAU_UNIT_TEST();

    ::tau::HashTable<C8DynString, int> hashTable;

    hashTable.Insert(u8"asdf", 64);
    hashTable.Insert(u8"qwerty", 32);
    
    {
        const int* const pAsdf = hashTable.Get(u8"asdf");

        TAU_UNIT_NEQ(pAsdf, nullptr, u8"Key \"asdf\" could not be found.");

        if(pAsdf)
        {
            TAU_UNIT_EQ(*pAsdf, 64, u8"Key \"asdf\" was not 64.");
        }
    }

    {
        const int* const pQwerty = hashTable.Get(u8"qwerty");

        TAU_UNIT_NEQ(pQwerty, nullptr, u8"Key \"qwerty\" could not be found.");

        if(pQwerty)
        {
            TAU_UNIT_EQ(*pQwerty, 32, u8"Key \"qwerty\" was not 32.");
        }
    }
}

static void CollisionTest() noexcept
{
    TAU_UNIT_TEST();

    ::tau::HashTable<DirectHash, int> hashTable;

    hashTable.Insert(2, 64);
    hashTable.Insert(15, 32);
    
    {
        const int* const p2 = hashTable.Get(2);

        TAU_UNIT_NEQ(p2, nullptr, u8"Key \"2\" could not be found.");

        if(p2)
        {
            TAU_UNIT_EQ(*p2, 64, u8"Key \"2\" was not 64.");
        }
    }

    {
        const int* const p15 = hashTable.Get(15);

        TAU_UNIT_NEQ(p15, nullptr, u8"Key \"15\" could not be found.");

        if(p15)
        {
            TAU_UNIT_EQ(*p15, 32, u8"Key \"15\" was not 32.");
        }
    }
}

static void PerfTest(const uSys elementCount) noexcept
{
    using Clock = ::std::chrono::high_resolution_clock;

    constexpr uSys WarmupCount = 3;
    constexpr uSys RetryCount = 10;

    iSys tauTimes[RetryCount];
    iSys stdTimes[RetryCount];

    for(uSys i = 0; i < WarmupCount + RetryCount; ++i)
    {
        {
            ::tau::HashTable<DirectHash, int> hashTable;
            srand(0);

            const auto begin = Clock::now();

            for(uSys i = 0; i < elementCount; ++i)
            {
                const auto key = rand();
                const auto value = rand();

                hashTable.Insert(key, value);
            }

            const auto end = Clock::now();

            if(i >= WarmupCount)
            {
                tauTimes[i - WarmupCount] = (end - begin).count() / 100;
            }
        }

        {
            ::std::unordered_map<DirectHash, int> hashTable;
            srand(0);

            const auto begin = Clock::now();

            for(uSys i = 0; i < elementCount; ++i)
            {
                const auto key = rand();
                const auto value = rand();

                hashTable[key] = value;
            }

            const auto end = Clock::now();

            if(i >= WarmupCount)
            {
                stdTimes[i - WarmupCount] = (end - begin).count() / 100;
            }
        }
    }

    uSys tauTime = 0;
    uSys stdTime = 0;

    for(uSys i = 0; i < RetryCount; ++i)
    {
        tauTime += tauTimes[i];
        stdTime += stdTimes[i];
    }

    tauTime /= RetryCount;
    stdTime /= RetryCount;

    ConPrinter::PrintLn("Tau HashTable     Insert {}  Time: {}", elementCount, tauTime);
    ConPrinter::PrintLn("std unordered_map Insert {}  Time: {}", elementCount, stdTime);
    ConPrinter::PrintLn("                  Insert {} Ratio: {f3}", elementCount, (static_cast<double>(tauTime) / static_cast<double>(stdTime)));
}

static void PerfTest10() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(10);
}

static void PerfTest100() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(100);
}

static void PerfTest1000() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(1000);
}

static void PerfTest10000() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(10000);
}

static void PerfTest50000() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(50000);
}

static void PerfTest100000() noexcept
{
    TAU_UNIT_TEST();
    PerfTest(100000);
}

void HashMapTests()
{
    InsertTest();
    CollisionTest();
    PerfTest10();
    PerfTest100();
    PerfTest1000();
    PerfTest10000();
    PerfTest50000();
    PerfTest100000();
}
