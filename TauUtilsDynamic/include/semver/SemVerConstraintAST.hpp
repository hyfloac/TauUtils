#pragma once

#include "Objects.hpp"
#include "String.hpp"
#include "Safeties.hpp"
#include "SemVer.hpp"

namespace tau::semver {

class SemVerConstraintVisitor;
class SemVerConstraintExecutor;

}

namespace tau::semver::ast {

class SemVerConstraintAST
{
    DEFAULT_CONSTRUCT_POC(SemVerConstraintAST);
    DEFAULT_DESTRUCT_VI(SemVerConstraintAST);
    DEFAULT_CM_POC(SemVerConstraintAST);
public:
    virtual void Visit(SemVerConstraintVisitor& visitor) const noexcept { }
    virtual bool Execute(SemVerConstraintExecutor& executor) const noexcept { return false; }
};

class SemVerConstraintEqualAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintEqualAST);
    DEFAULT_CM_PUC(SemVerConstraintEqualAST);
public:
    SemVerConstraintEqualAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintEqualAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintNotEqualAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintNotEqualAST);
    DEFAULT_CM_PUC(SemVerConstraintNotEqualAST);
public:
    SemVerConstraintNotEqualAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintNotEqualAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintLessAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintLessAST);
    DEFAULT_CM_PUC(SemVerConstraintLessAST);
public:
    SemVerConstraintLessAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintLessAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintLessEqualAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintLessEqualAST);
    DEFAULT_CM_PUC(SemVerConstraintLessEqualAST);
public:
    SemVerConstraintLessEqualAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintLessEqualAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintGreaterAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintGreaterAST);
    DEFAULT_CM_PUC(SemVerConstraintGreaterAST);
public:
    SemVerConstraintGreaterAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintGreaterAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintGreaterEqualAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintGreaterEqualAST);
    DEFAULT_CM_PUC(SemVerConstraintGreaterEqualAST);
public:
    SemVerConstraintGreaterEqualAST(const SemVer& target) noexcept
        : m_Target(target)
    { }
    SemVerConstraintGreaterEqualAST(SemVer&& target) noexcept
        : m_Target(::std::move(target))
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const SemVer& Target() const noexcept { return m_Target; }
private:
    SemVer m_Target;
};

class SemVerConstraintAndAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintAndAST);
    DEFAULT_CM_PUC(SemVerConstraintAndAST);
public:
    SemVerConstraintAndAST(const Ref<SemVerConstraintAST>& left, const Ref<SemVerConstraintAST>& right) noexcept
        : m_Left(left)
        , m_Right(right)
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const Ref<SemVerConstraintAST>& Left() const noexcept { return m_Left; }
    [[nodiscard]] const Ref<SemVerConstraintAST>& Right() const noexcept { return m_Right; }
private:
    Ref<SemVerConstraintAST> m_Left;
    Ref<SemVerConstraintAST> m_Right;
};

class SemVerConstraintOrAST final : public SemVerConstraintAST
{
    DEFAULT_DESTRUCT_C(SemVerConstraintOrAST);
    DEFAULT_CM_PUC(SemVerConstraintOrAST);
public:
    SemVerConstraintOrAST(const Ref<SemVerConstraintAST>& left, const Ref<SemVerConstraintAST>& right) noexcept
        : m_Left(left)
        , m_Right(right)
    { }

    void Visit(SemVerConstraintVisitor& visitor) const noexcept override;
    bool Execute(SemVerConstraintExecutor& executor) const noexcept override;

    [[nodiscard]] const Ref<SemVerConstraintAST>& Left() const noexcept { return m_Left; }
    [[nodiscard]] const Ref<SemVerConstraintAST>& Right() const noexcept { return m_Right; }
private:
    Ref<SemVerConstraintAST> m_Left;
    Ref<SemVerConstraintAST> m_Right;
};

[[nodiscard]] inline Ref<SemVerConstraintAST> Equal(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintEqualAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> NotEqual(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintNotEqualAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> Less(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintLessAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> LessEqual(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintLessEqualAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> Greater(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintGreaterAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> GreaterEqual(const SemVer& constraint) noexcept
{
    return Ref<SemVerConstraintGreaterEqualAST>(constraint);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> And(const Ref<SemVerConstraintAST>& left, const Ref<SemVerConstraintAST>& right) noexcept
{
    return Ref<SemVerConstraintAndAST>(left, right);
}

[[nodiscard]] inline Ref<SemVerConstraintAST> Or(const Ref<SemVerConstraintAST>& left, const Ref<SemVerConstraintAST>& right) noexcept
{
    return Ref<SemVerConstraintOrAST>(left, right);
}

// TODO: Fix the bad BuildMetadata from being stored as part of the constraint.
[[nodiscard]] inline Ref<SemVerConstraintAST> Tilde(const SemVer& constraint) noexcept
{
    // e.g ~1.2
    if(constraint.BuildMetadata.Length() == 0)
    {
        return And(GreaterEqual(constraint), Less(SemVer(constraint.MajorVersion + 1, 0, 0, EReleaseType::Alpha)));
    }
    // e.g ~1.2.3
    else
    {
        return And(GreaterEqual(constraint), Less(SemVer(constraint.MajorVersion, constraint.MinorVersion + 1, 0, EReleaseType::Alpha)));
    }
}

// TODO: Fix the bad BuildMetadata from being stored as part of the constraint.
[[nodiscard]] inline Ref<SemVerConstraintAST> Caret(const SemVer& constraint) noexcept
{
    if(constraint.MajorVersion == 0)
    {
        // e.g ^0.2
        if(constraint.BuildMetadata.Length() == 0)
        {
            return And(GreaterEqual(constraint), Less(SemVer(constraint.MajorVersion, constraint.MinorVersion + 1, 0, EReleaseType::Alpha)));
        }
        // e.g ^0.2.3
        else
        {
            return And(GreaterEqual(constraint), Less(SemVer(constraint.MajorVersion, constraint.MinorVersion, constraint.PatchVersion + 1, EReleaseType::Alpha)));
        }
    }
    // e.g. ^1.2.3 or ^1.2
    else
    {
        return And(GreaterEqual(constraint), Less(SemVer(constraint.MajorVersion + 1, 0, 0, EReleaseType::Alpha)));
    }
}

}

namespace tau::semver {

class SemVerConstraintVisitor
{
    DEFAULT_CONSTRUCT_POC(SemVerConstraintVisitor);
    DEFAULT_DESTRUCT_VI(SemVerConstraintVisitor);
    DEFAULT_CM_POC(SemVerConstraintVisitor);
public:
    virtual void Visit(const Ref<ast::SemVerConstraintAST>& ast) noexcept
    {
        Visit(ast.Get());
    }

    virtual void Visit(const ast::SemVerConstraintAST* const ast) noexcept
    {
        if(ast)
        {
            Visit(*ast);
        }
    }

    virtual void Visit(const ast::SemVerConstraintAST& ast) noexcept
    {
        ast.Visit(*this);
    }

    virtual void Visit(const ast::SemVerConstraintEqualAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintNotEqualAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintGreaterAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintGreaterEqualAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintLessAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintLessEqualAST& ast) noexcept
    { }

    virtual void Visit(const ast::SemVerConstraintAndAST& ast) noexcept
    {
        Visit(ast.Left());
        Visit(ast.Right());
    }

    virtual void Visit(const ast::SemVerConstraintOrAST& ast) noexcept
    {
        Visit(ast.Left());
        Visit(ast.Right());
    }
};

class SemVerConstraintExecutor
{
    DEFAULT_DESTRUCT_VI(SemVerConstraintExecutor);
    DEFAULT_CM_PUC(SemVerConstraintExecutor);
public:
    SemVerConstraintExecutor(const SemVer& currentVersion) noexcept
        : m_CurrentVersion(currentVersion)
    { }

    SemVerConstraintExecutor(SemVer&& currentVersion) noexcept
        : m_CurrentVersion(::std::move(currentVersion))
    { }

    virtual bool Execute(const Ref<ast::SemVerConstraintAST>& ast) noexcept
    {
        return Execute(ast.Get());
    }

    virtual bool Execute(const ast::SemVerConstraintAST* const ast) noexcept
    {
        if(ast)
        {
            return Execute(*ast);
        }

        return false;
    }

    virtual bool Execute(const ast::SemVerConstraintAST& ast) noexcept
    {
        return ast.Execute(*this);
    }

    virtual bool Execute(const ast::SemVerConstraintEqualAST& ast) noexcept
    {
        return m_CurrentVersion == ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintNotEqualAST& ast) noexcept
    {
        return m_CurrentVersion != ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintGreaterAST& ast) noexcept
    {
        return m_CurrentVersion > ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintGreaterEqualAST& ast) noexcept
    {
        return m_CurrentVersion >= ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintLessAST& ast) noexcept
    {
        return m_CurrentVersion < ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintLessEqualAST& ast) noexcept
    {
        return m_CurrentVersion <= ast.Target();
    }

    virtual bool Execute(const ast::SemVerConstraintAndAST& ast) noexcept
    {
        return Execute(ast.Left()) && Execute(ast.Right());
    }

    virtual bool Execute(const ast::SemVerConstraintOrAST& ast) noexcept
    {
        return Execute(ast.Left()) || Execute(ast.Right());
    }
private:
    SemVer m_CurrentVersion;
};

}

#include "SemVerConstraintAST.inl"
