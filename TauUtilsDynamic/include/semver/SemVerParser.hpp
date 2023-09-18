#pragma once

#include "NumTypes.hpp"
#include "String.hpp"
#include "SemVerLexer.hpp"
#include "SemVerConstraintAST.hpp"
#include <vector>

namespace tau::semver {

class SemVerParser final
{
    DEFAULT_DESTRUCT(SemVerParser);
    DEFAULT_CM_PU(SemVerParser);
private:
    using FactorFunc = Ref<ast::SemVerConstraintAST>(*)(const SemVer& constraint);
public:
    SemVerParser(const ::std::vector<SemVerToken>& tokens) noexcept
        : m_Tokens(tokens)
        , m_CurrentIndex(0)
        , m_Error()
    { }

    SemVerParser(::std::vector<SemVerToken>&& tokens) noexcept
        : m_Tokens(::std::move(tokens))
        , m_CurrentIndex(0)
        , m_Error()
    { }

    void Reset(const ::std::vector<SemVerToken>& tokens) noexcept
    {
        m_Tokens = tokens;
        m_CurrentIndex = 0;
        m_Error = C8DynString();
    }

    void Reset(::std::vector<SemVerToken>&& tokens) noexcept
    {
        m_Tokens = ::std::move(tokens);
        m_CurrentIndex = 0;
        m_Error = C8DynString();
    }

    [[nodiscard]] const C8DynString& Error() const noexcept { return m_Error; }

    [[nodiscard]] Ref<ast::SemVerConstraintAST> Parse() noexcept
    {
        Ref<ast::SemVerConstraintAST> constraint = ParseConstraint();

        while(m_CurrentIndex < m_Tokens.size() - 1)
        {
            if(!constraint)
            {
                return nullptr;
            }

            if(CurrentToken().Type == SemVerTokenType::Char)
            {
                if(CurrentToken().Char == U'|')
                {
                    if(!Advance())
                    {
                        m_Error = u8"Found a single '|', but was actually followed by EOF.";
                        return nullptr;
                    }

                    if(!Match(CurrentToken(), U'|'))
                    {
                        m_Error = u8"Found a single '|', but was not followed by another '|'.";
                        return nullptr;
                    }

                    if(!Advance())
                    {
                        m_Error = u8"Found a '||' OR operator, but was actually followed by EOF.";
                        return nullptr;
                    }


                    const Ref<ast::SemVerConstraintAST> orConstraint = ParseConstraint();

                    if(!orConstraint)
                    {
                        return nullptr;
                    }

                    constraint = ast::Or(constraint, orConstraint);
                }
                else
                {
                    const Ref<ast::SemVerConstraintAST> andConstraint = ParseConstraint();

                    if(!andConstraint)
                    {
                        return nullptr;
                    }

                    constraint = ast::And(constraint, andConstraint);
                }
            }
            else if(CurrentToken().Type == SemVerTokenType::Number)
            {
                const Ref<ast::SemVerConstraintAST> andConstraint = ParseConstraint();

                if(!andConstraint)
                {
                    return nullptr;
                }

                constraint = ast::And(constraint, andConstraint);
            }
        }

        return constraint;
    }
private:
    /**
     * @brief Parses a single_version_constraint
     *
     * single_version_constraint = [ constraint_type ], [ whitespace ], version_number;
     *
     * @return The parsed constraint, or nullptr in the event of an error.
     */
    [[nodiscard]] Ref<ast::SemVerConstraintAST> ParseConstraint()
    {
        FactorFunc factory = ast::Equal;

        // [ constraint_type ]
        // constraint_type = "=" | "!=" | "<" | "<=" | ">" | ">=" | "~" | "^";
        if(CurrentToken().Type == SemVerTokenType::Char)
        {
            switch(CurrentToken().Char)
            {
                case U'=':
                    factory = ast::Equal;
                    break;
                case U'!':
                    if(!Advance())
                    {
                        m_Error = u8"Found '!' in version constraint, but was actually followed by EOF.";
                        return nullptr;
                    }
                    if(!Match(CurrentToken(), U'='))
                    {
                        m_Error = u8"Found '!' in version constraint, but was not followed by '='.";
                        return nullptr;
                    }
                    factory = ast::NotEqual;
                    break;
                case U'>':
                    if(!Advance())
                    {
                        m_Error = u8"Found '>' in version constraint, but was actually followed by EOF.";
                        return nullptr;
                    }
                    if(CurrentToken().Type == SemVerTokenType::Char)
                    {
                        if(CurrentToken().Char != U'=')
                        {
                            m_Error = u8"Found '>' in version constraint, but was not followed by '=' or a version.";
                            return nullptr;
                        }

                        factory = ast::GreaterEqual;
                    }
                    else
                    {
                        factory = ast::Greater;
                        (void) Retreat();
                    }
                    break;
                case U'<':
                    if(!Advance())
                    {
                        m_Error = u8"Found '<' in version constraint, but was actually followed by EOF.";
                        return nullptr;
                    }
                    if(CurrentToken().Type == SemVerTokenType::Char)
                    {
                        if(CurrentToken().Char != U'=')
                        {
                            m_Error = u8"Found '<>>' in version constraint, but was not followed by '=' or a version.";
                            return nullptr;
                        }

                        factory = ast::LessEqual;
                    }
                    else
                    {
                        factory = ast::Less;
                        (void) Retreat();
                    }
                    break;
                case U'~':
                    factory = ast::Tilde;
                    break;
                case U'^':
                    factory = ast::Caret;
                    break;
                default:
                    m_Error = Format<c8>(u8"Unexpected character ''{}'' encountered while parsing version constraint.", CurrentToken().Char);
                    return nullptr;
            }

            if(!Advance())
            {
                m_Error = u8"Found a version constraint type, but was actually followed by EOF.";
                return nullptr;
            }
        }

        // version_number
        // version_number = number, ".", ( number, [ ".", ( number | "*" ) ] | "*" ), [ release_type ];
        // number
        if(CurrentToken().Type != SemVerTokenType::Number)
        {
            m_Error = u8"Expected a number for a version constraint major version.";
            return nullptr;
        }

        const u32 majorVersion = CurrentToken().Int;

        if(!Advance())
        {
            m_Error = Format<c8>(u8"Found a version constraint with version {}, but was terminated too early by EOF. Version must be at least a MajorVersion and a MinorVersion.", majorVersion);
            return nullptr;
        }

        // ".", ( number, [ ".", ( number | "*" ) ] | "*" )
        if(!Match(CurrentToken(), U'.'))
        {
            m_Error = u8"Expected a decimal separator for a version constraint following the major version.";
            return nullptr;
        }

        if(!Advance())
        {
            m_Error = Format<c8>(u8"Found a version constraint with version {}., but was terminated too early by EOF. Version must be at least a MajorVersion and a MinorVersion.", majorVersion);
            return nullptr;
        }

        // ( number, [ ".", ( number | "*" ) ] | "*" )
        if(CurrentToken().Type != SemVerTokenType::Number)
        {
            m_Error = u8"Expected a number for a version constraint minor version.";
            return nullptr;
        }

        const u32 minorVersion = CurrentToken().Int;

        // A constraint has been found
        if(!Advance())
        {
            // The buildMetadata here is for the ~ and ^ operators.
            return factory(SemVer(majorVersion, minorVersion, ""));
        }

        u32 patchVersion = 0;

        // Handle patch version.
        // [ ".", ( number | "*" ) ]
        if(Match(CurrentToken(), U'.'))
        {
            if(!Advance())
            {
                m_Error = Format<c8>(u8"Found a version constraint with version {}.{}., but was terminated too early by EOF. Version cannot end on a ''.''.", majorVersion, minorVersion);
                return nullptr;
            }

            // number | "*"
            if(CurrentToken().Type != SemVerTokenType::Number)
            {
                m_Error = u8"Expected a number for a version constraint patch version.";
                return nullptr;
            }

            patchVersion = CurrentToken().Int;

            // A constraint has been found
            if(!Advance())
            {
                // The buildMetadata here is for the ~ and ^ operators.
                return factory(SemVer(majorVersion, minorVersion, patchVersion, "1"));
            }
        }

        EReleaseType releaseType = EReleaseType::None;

        // Match release type
        // [ release_type ]
        // release_type = "-alpha" | "-beta" | "-gamma" | "-rc";
        if(Match(CurrentToken(), U'-'))
        {
            if(!Advance())
            {
                m_Error = Format<c8>(u8"Found a version constraint with version {}.{}.{}-, but was terminated too early by EOF. Version cannot end on a ''-''.", majorVersion, minorVersion, patchVersion);
                return nullptr;
            }

            if(CurrentToken().Type != SemVerTokenType::String)
            {
                m_Error = u8"Expected a string for a version constraint release type.";
                return nullptr;
            }

            STR_SWITCH(CurrentToken().String, {
                C8STR_CASE(ReleaseTypeAlpha, {
                    releaseType = EReleaseType::Alpha;
                });
                C8STR_CASE(ReleaseTypeBeta, {
                    releaseType = EReleaseType::Beta;
                });
                C8STR_CASE(ReleaseTypeGamma, {
                    releaseType = EReleaseType::Gamma;
                });
                C8STR_CASE(ReleaseTypeReleaseCandidate, {
                    releaseType = EReleaseType::ReleaseCandidate;
                });
            }, { });

            (void) Advance();
        }

        // A constraint has been found
        // The buildMetadata here is for the ~ and ^ operators.
        return factory(SemVer(majorVersion, minorVersion, patchVersion, releaseType, "1"));
    }

    [[nodiscard]] const SemVerToken& CurrentToken() const noexcept { return m_Tokens[m_CurrentIndex]; }

    bool Advance() noexcept
    {
        if(m_CurrentIndex >= m_Tokens.size() - 1)
        {
            return false;
        }

        ++m_CurrentIndex;
        return true;
    }

    bool Retreat() noexcept
    {
        if(m_CurrentIndex == 0)
        {
            return false;
        }

        --m_CurrentIndex;
        return true;
    }
private:
    [[nodiscard]] static bool Match(const SemVerToken& token, const c32 c) noexcept
    {
        if(token.Type != SemVerTokenType::Char)
        {
            return false;
        }

        return token.Char == c;
    }

    [[nodiscard]] static bool Match(const SemVerToken& token, const u32 i) noexcept
    {
        if(token.Type != SemVerTokenType::Number)
        {
            return false;
        }

        return token.Int == i;
    }
private:
    ::std::vector<SemVerToken> m_Tokens;
    uSys m_CurrentIndex;
    C8DynString m_Error;
};

}
