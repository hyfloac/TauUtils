#include <semver/SemVerConstraintAST.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

using namespace tau::semver;
using namespace tau::semver::ast;

namespace equal {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return Equal(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} did not match itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} did not match {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} did not match {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void EqualTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(3, 1, 0, "a8fc9e"),
            SemVer(3, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleFalseTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleTrueTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleFalseTest(target, checkVal);
    }
}

}

namespace not_equal {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return NotEqual(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} was equal to itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} was equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} was equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void NotEqualTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(3, 1, 0, "a8fc9e"),
            SemVer(3, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleTrueTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleFalseTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleTrueTest(target, checkVal);
    }
}

}

namespace less {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return Less(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} was less than itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} less than {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} less than {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunFalseTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void LessTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleTrueTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "fdd90f");
        const SemVer checkVal(4, 2, 16, EReleaseType::Alpha, "fdd90f");
        RunReversibleFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "60849e");
        const SemVer checkVal(4, 3, 16, EReleaseType::Gamma, "64f1ee");
        RunReversibleTrueTest(target, checkVal);
    }
}

}

namespace less_equal {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return LessEqual(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} was not less than or equal to itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} less than or equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} less than or equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunFalseTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void LessEqualTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleTrueTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleAlwaysTrueTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleAlwaysTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "fdd90f");
        const SemVer checkVal(4, 2, 16, EReleaseType::Alpha, "fdd90f");
        RunReversibleFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "60849e");
        const SemVer checkVal(4, 3, 16, EReleaseType::Gamma, "64f1ee");
        RunReversibleTrueTest(target, checkVal);
    }
}

}

namespace greater {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return Greater(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} was greater than itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} greater than {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} greater than {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunFalseTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void GreaterTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleFalseTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "fdd90f");
        const SemVer checkVal(4, 2, 16, EReleaseType::Alpha, "fdd90f");
        RunReversibleTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "60849e");
        const SemVer checkVal(4, 3, 16, EReleaseType::Gamma, "64f1ee");
        RunReversibleFalseTest(target, checkVal);
    }
}

}

namespace greater_equal {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkVal) noexcept
{
    return GreaterEqual(checkVal);
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} was not greater than or equal to itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} greater than or equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunFalseTest(const SemVer& target, const SemVer& checkVal) noexcept
{
   const Ref<SemVerConstraintAST> checker = Comparison(checkVal);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} greater than or equal to {}.", target.ToString<c8>(), checkVal.ToString<c8>());
}

static void RunReversibleTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunFalseTest(b, a);
}

static void RunReversibleFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysTrueTest(const SemVer& a, const SemVer& b) noexcept
{
    RunTrueTest(a, b);
    RunTrueTest(b, a);
}

static void RunReversibleAlwaysFalseTest(const SemVer& a, const SemVer& b) noexcept
{
    RunFalseTest(a, b);
    RunFalseTest(b, a);
}

static void GreaterEqualTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 1; ++i)
        {
            RunReversibleFalseTest(targets[i], targets[i + 1]);
        }
    }

    {
        const SemVer target(3, 2, 0, "ff9a21");
        const SemVer checkVal(3, 2, 0);
        RunReversibleAlwaysTrueTest(target, checkVal);
    }

    {
        const SemVer target(3, 2, 16, EReleaseType::ReleaseCandidate, "b8bf1d");
        const SemVer checkVal(3, 2, 16, EReleaseType::ReleaseCandidate);
        RunReversibleAlwaysTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 0, "92a7b3");
        const SemVer checkVal(4, 2, 0, "1075fe");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "b8bf1d");
        const SemVer checkVal(4, 2, 16, EReleaseType::Gamma, "f8b8c8");
        RunReversibleAlwaysFalseTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "fdd90f");
        const SemVer checkVal(4, 2, 16, EReleaseType::Alpha, "fdd90f");
        RunReversibleTrueTest(target, checkVal);
    }

    {
        const SemVer target(4, 2, 16, EReleaseType::Gamma, "60849e");
        const SemVer checkVal(4, 3, 16, EReleaseType::Gamma, "64f1ee");
        RunReversibleFalseTest(target, checkVal);
    }
}

}

namespace and_ {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkValMin, const SemVer& checkValMax) noexcept
{
    return And(GreaterEqual(checkValMin), LessEqual(checkValMax));
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target, target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} was bounded by itself.", target.ToString<c8>());
}

static void RunTrueTest(const SemVer& target, const SemVer& checkValMin, const SemVer& checkValMax) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(checkValMin, checkValMax);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} bounded by [{}, {}].", target.ToString<c8>(), checkValMin.ToString<c8>(), checkValMax.ToString<c8>());
}

// static void RunFalseTest(const SemVer& target, const SemVer& checkValMin, const SemVer& checkValMax) noexcept
// {
//     const Ref<SemVerConstraintAST> checker = Comparison(checkValMin, checkValMax);
//
//     SemVerConstraintExecutor executor(target);
//     TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} bounded by [{}, {}].", target.ToString<c8>(), checkValMin.ToString<c8>(), checkValMax.ToString<c8>());
// }

static void AndTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 2; ++i)
        {
            RunTrueTest(targets[i + 1], targets[i], targets[i + 2]);
        }
    }
}

}

namespace or_ {

static Ref<SemVerConstraintAST> Comparison(const SemVer& checkValMax, const SemVer& checkValMin) noexcept
{
    return Or(Less(checkValMin), Greater(checkValMax));
}

static void RunSameTest(const SemVer& target) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(target, target);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} was inverse bounded by itself.", target.ToString<c8>());
}

// static void RunTrueTest(const SemVer& target, const SemVer& checkValMax, const SemVer& checkValMin) noexcept
// {
//     const Ref<SemVerConstraintAST> checker = Comparison(checkValMax, checkValMin);
//
//     SemVerConstraintExecutor executor(target);
//     TAU_UNIT_TRUE(executor.Execute(checker), u8"Version {} inverse bounded by [, {}) || ({}, ].", target.ToString<c8>(), checkValMax.ToString<c8>(), checkValMin.ToString<c8>());
// }

static void RunFalseTest(const SemVer& target, const SemVer& checkValMax, const SemVer& checkValMin) noexcept
{
    const Ref<SemVerConstraintAST> checker = Comparison(checkValMax, checkValMin);

    SemVerConstraintExecutor executor(target);
    TAU_UNIT_FALSE(executor.Execute(checker), u8"Version {} inverse bounded by [, {}) || ({}, ].", target.ToString<c8>(), checkValMax.ToString<c8>(), checkValMin.ToString<c8>());
}

static void OrTests()
{
    TAU_UNIT_TEST();

    {
        const SemVer targets[] = {
            SemVer(1, 0),
            SemVer(1, 1, 23),
            SemVer(1, 1, 24, EReleaseType::Alpha),
            SemVer(1, 1, 24, EReleaseType::Beta),
            SemVer(1, 1, 24, EReleaseType::Gamma),
            SemVer(1, 1, 24, EReleaseType::ReleaseCandidate),
            SemVer(1, 1, 24, EReleaseType::Release),
            SemVer(1, 1, 25, EReleaseType::Alpha),
            SemVer(1, 1, 25),
            SemVer(1, 2, 23),
            SemVer(1, 2, 26),
            SemVer(2, 0, 52),
            SemVer(3, 5, 52),
            SemVer(4, 2, 20),
            SemVer(30, 1, 0, "a8fc9e"),
            SemVer(30, 1, 16, EReleaseType::ReleaseCandidate, "997fa1"),
        };

        for(const SemVer& target : targets)
        {
            RunSameTest(target);
        }

        for(uSys i = 0; i < ::std::size(targets) - 2; ++i)
        {
            RunFalseTest(targets[i + 1], targets[i + 2], targets[i]);
        }
    }
}

}

void SemVerTests()
{
    equal::EqualTests();
    not_equal::NotEqualTests();
    less::LessTests();
    less_equal::LessEqualTests();
    greater::GreaterTests();
    greater_equal::GreaterEqualTests();
    and_::AndTests();
    or_::OrTests();
}
