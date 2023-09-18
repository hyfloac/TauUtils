#include <semver/SemVerLexer.hpp>
#include <semver/SemVerParser.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>

using namespace tau::semver;

static void PrintTokens(const C8DynString& constraintString)
{
    ConPrinter::PrintLn(u8"Lexing {}", constraintString);

    const ::std::vector<SemVerToken> tokens = LexSemVer(constraintString);

    for(const SemVerToken& token : tokens)
    {
        switch(token.Type)
        {
            case SemVerTokenType::Error:
                ConPrinter::PrintLn(u8"  Error: {}", token.String);
                break;
            case SemVerTokenType::Eof:
                ConPrinter::PrintLn(u8"  Eof");
                break;
            case SemVerTokenType::String:
                ConPrinter::PrintLn(u8"  \"{}\"", token.String);
                break;
            case SemVerTokenType::Char:
                ConPrinter::PrintLn(u8"  ''{}''", token.Char);
                break;
            case SemVerTokenType::Number:
                ConPrinter::PrintLn(u8"  {}", token.Int);
                break;
        }
    }
}

static void SemVerLexerTests()
{
    TAU_UNIT_TEST();

    {
        const C8DynString constraintString = u8">1.0.0 <= 2.0.0 || ^3.15.0-rc";
        PrintTokens(constraintString);
    }

    {
        const C8DynString constraintString = u8">1.0.0 <= 2.0.0 || ^3.15.0-rc";
        ::std::vector<SemVerToken> checkTokens;
        {
            checkTokens.emplace_back(U'>');
            checkTokens.emplace_back(1ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(0ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(0ui32);
            checkTokens.emplace_back(U'<');
            checkTokens.emplace_back(U'=');
            checkTokens.emplace_back(2ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(0ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(0ui32);
            checkTokens.emplace_back(U'|');
            checkTokens.emplace_back(U'|');
            checkTokens.emplace_back(U'^');
            checkTokens.emplace_back(3ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(15ui32);
            checkTokens.emplace_back(U'.');
            checkTokens.emplace_back(0ui32);
            checkTokens.emplace_back(U'-');
            checkTokens.push_back(SemVerToken::MakeString(u8"rc"));
        }

        const ::std::vector<SemVerToken> tokens = LexSemVer(constraintString);

        TAU_UNIT_EQ(checkTokens.size(), tokens.size(), u8"Token count did not match.");

        if(checkTokens.size() == tokens.size())
        {
            for(uSys i = 0; i < checkTokens.size(); ++i)
            {
                TAU_UNIT_TRUE(checkTokens[i].Type == tokens[i].Type, u8"Token types did not match for index {}.", i);

                if(checkTokens[i].Type != tokens[i].Type)
                {
                    break;
                }

                switch(checkTokens[i].Type)
                {
                    case SemVerTokenType::Error:
                    case SemVerTokenType::String:
                        TAU_UNIT_EQ(tokens[i].String, checkTokens[i].String, u8"Token strings did not match for index {}.", i);
                        break;
                    case SemVerTokenType::Char:
                        TAU_UNIT_EQ(tokens[i].Char, checkTokens[i].Char, u8"Token chars did not match for index {}.", i);
                        break;
                    case SemVerTokenType::Number:
                        TAU_UNIT_EQ(tokens[i].Int, checkTokens[i].Int, u8"Token numbers did not match for index {}.", i);
                        break;
                    default: break;
                }
            }
        }
    }
}

static void SemVerParseTests() noexcept
{
    TAU_UNIT_TEST();

    const C8DynString constraintString = u8">1.0.0 <= 2.0.0 || ^3.15.0-rc";
    const ::std::vector<SemVerToken> tokens = LexSemVer(constraintString);

    SemVerParser parser(tokens);

    const Ref<ast::SemVerConstraintAST> ast = parser.Parse();

    TAU_UNIT_NEQ(ast.Get(), nullptr, u8"AST was null.");

    TAU_UNIT_EQ(parser.Error(), u8"", "Parser error was not null.");
}

static void SemVerValidateTests() noexcept
{
    TAU_UNIT_TEST();

    const C8DynString constraintString = u8">=1.0.0 < 2.0.0 || ^3.15.0-rc";
    const ::std::vector<SemVerToken> tokens = LexSemVer(constraintString);

    SemVerParser parser(tokens);

    const Ref<ast::SemVerConstraintAST> ast = parser.Parse();

    TAU_UNIT_NEQ(ast.Get(), nullptr, u8"AST was null.");

    TAU_UNIT_EQ(parser.Error(), u8"", "Parser error was not null.");

    {
        const SemVer target(1, 2, 15);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_TRUE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(2, 0, 0);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(3, 26, 34754);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_TRUE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(3, 11, 0);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(3, 15, 0);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_TRUE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(3, 15, 0, EReleaseType::Beta);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(4, 0, 0, EReleaseType::Beta);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(4, 0, 0, EReleaseType::ReleaseCandidate);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }

    {
        const SemVer target(4, 0, 0, EReleaseType::Release);

        SemVerConstraintExecutor executor(target);
        TAU_UNIT_FALSE(executor.Execute(ast), u8"Version {} did not match constraint {}", target.ToString<c8>(), constraintString);
    }
}

void SemVerParserTests()
{
    SemVerLexerTests();
    SemVerParseTests();
    SemVerValidateTests();
}
