#pragma once

#if defined(TAU_INDEV) || 0
#include "SemVerConstraintAST.hpp"
#endif

inline void tau::semver::ast::SemVerConstraintEqualAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintEqualAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintNotEqualAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintNotEqualAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintGreaterAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintGreaterAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintGreaterEqualAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintGreaterEqualAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintLessAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintLessAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintLessEqualAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintLessEqualAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintAndAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintAndAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}

inline void tau::semver::ast::SemVerConstraintOrAST::Visit(SemVerConstraintVisitor& visitor) const noexcept
{
    visitor.Visit(*this);
}

inline bool tau::semver::ast::SemVerConstraintOrAST::Execute(SemVerConstraintExecutor& executor) const noexcept
{
    return executor.Execute(*this);
}
