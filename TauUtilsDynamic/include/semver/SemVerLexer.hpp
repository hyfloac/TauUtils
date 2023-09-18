#pragma once

#include "NumTypes.hpp"
#include "String.hpp"
#include "StringFormat.hpp"
#include <vector>

namespace tau::semver {

enum class SemVerTokenType : i32
{
    Error = -1,
    Eof = 0,
    String,
    Char,
    Number
};

struct SemVerToken final
{
    SemVerTokenType Type;
    u32 Column;
    u32 Length;
    union
    {
        C8DynString String;
        c32 Char;
        u32 Int;
    };
public:
    SemVerToken(const c32 c) noexcept
        : Type(SemVerTokenType::Char)
        , Column(0)
        , Length(0)
        , Char(c)
    { }

    SemVerToken(const u32 i) noexcept
        : Type(SemVerTokenType::Number)
        , Column(0)
        , Length(0)
        , Int(i)
    { }
    
    [[nodiscard]] static SemVerToken MakeEof() noexcept
    { return SemVerToken(SemVerTokenType::Eof); }

    [[nodiscard]] static SemVerToken MakeError(const C8DynString& str) noexcept
    { return SemVerToken(SemVerTokenType::Error, str); }

    [[nodiscard]] static SemVerToken MakeError(C8DynString&& str) noexcept
    { return SemVerToken(SemVerTokenType::Error, ::std::move(str)); }

    [[nodiscard]] static SemVerToken MakeString(const C8DynString & str) noexcept
    { return SemVerToken(SemVerTokenType::String, str); }

    [[nodiscard]] static SemVerToken MakeString(C8DynString && str) noexcept
    { return SemVerToken(SemVerTokenType::String, ::std::move(str)); }

    ~SemVerToken() noexcept
    { Destruct(); }

    SemVerToken(const SemVerToken& other)
        : Type(other.Type)
        , Column(other.Column)
        , Length(other.Length)
        , String()
    { Copy(other); }

    SemVerToken(SemVerToken&& other) noexcept
        : Type(other.Type)
        , Column(other.Column)
        , Length(other.Length)
        , String()
    { Move(::std::move(other)); }

    SemVerToken& operator=(const SemVerToken& other)
    {
        if(this == &other)
        {
            return *this;
        }

        Destruct();

        Type = other.Type;
        Column = other.Column;
        Length = other.Length;

        Copy(other);

        return *this;
    }

    SemVerToken& operator=(SemVerToken&& other) noexcept
    {
        if(this == &other)
        {
            return *this;
        }

        Destruct();

        Type = other.Type;
        Column = other.Column;
        Length = other.Length;

        Move(::std::move(other));

        return *this;
    }
    
    SemVerToken& SetColumn(const u32 column) noexcept
    {
        Column = column;
        return *this;
    }

    SemVerToken& SetLength(const u32 length) noexcept
    {
        Length = length;
        return *this;
    }
private:
    SemVerToken(const SemVerTokenType type) noexcept
        : Type(type)
        , Column(0)
        , Length(0)
        , Int(0)
    { }

    SemVerToken(const SemVerTokenType type, const C8DynString& str) noexcept
        : Type(type)
        , Column(0)
        , Length(0)
        , String(str)
    { }

    SemVerToken(const SemVerTokenType type, C8DynString&& str) noexcept
        : Type(type)
        , Column(0)
        , Length(0)
        , String(::std::move(str))
    { }

    void Destruct()
    {
        switch(Type)
        {
            case SemVerTokenType::Error:
            case SemVerTokenType::String:
                String.~DynStringT();
                break;
            default: break;
        }
    }

    void Copy(const SemVerToken& copy)
    {
        switch(Type)
        {
            case SemVerTokenType::Error:
            case SemVerTokenType::String:
                String = copy.String;
                break;
            case SemVerTokenType::Char:
                Char = copy.Char;
                break;
            case SemVerTokenType::Number:
                Int = copy.Int;
                break;
            default: break;
        }
    }

    void Move(SemVerToken&& copy)
    {
        switch(Type)
        {
            case SemVerTokenType::Error:
            case SemVerTokenType::String:
                String = ::std::move(copy.String);
                break;
            case SemVerTokenType::Char:
                Char = copy.Char;
                break;
            case SemVerTokenType::Number:
                Int = copy.Int;
                break;
            default: break;
        }
    }
};

inline constexpr C8ConstExprString ReleaseTypeAlpha(u8"alpha");
inline constexpr C8ConstExprString ReleaseTypeBeta(u8"beta");
inline constexpr C8ConstExprString ReleaseTypeGamma(u8"gamma");
inline constexpr C8ConstExprString ReleaseTypeReleaseCandidate(u8"rc");

[[nodiscard]] inline ::std::vector<SemVerToken> LexSemVer(const C8DynString& str) noexcept
{
    ::std::vector<SemVerToken> tokens;

    DynStringCodePointIteratorT<c8> iter = str.Begin();
    const DynStringCodePointIteratorT<c8> end = str.End();


    for(; iter != end; ++iter)
    {
        c32 c = *iter;

        if(c == U' ')
        {
            continue;
        }

        if(c == U'\0')
        {
            break;
        }

        switch(c)
        {
            case U'=':
            case U'!':
            case U'<':
            case U'>':
            case U'|':
            case U'.':
            case U'-':
            case U'~':
            case U'^':
                tokens.emplace_back(c);
                continue;
            default: break;
        }

        if(c >= U'0' && c <= '9')
        {
            u32 currentInt = 0;

            do
            {
                currentInt *= 10;
                currentInt += c - U'0';

                ++iter;

                if(iter == end)
                {
                    break;
                }

                c = *iter;
            }
            while(c >= U'0' && c <= '9');

            tokens.emplace_back(currentInt);
            --iter;
        }
        else if(c >= U'a' && c <= U'z')
        {
            C8StringBuilder builder;

            do
            {
                builder.Append(c);

                ++iter;

                if(iter == end)
                {
                    break;
                }

                c = *iter;
            } while(c >= U'a' && c <= U'z');

            const C8DynString lexedString = builder.ToString();

            if(!lexedString.Equals(ReleaseTypeAlpha) &&
               !lexedString.Equals(ReleaseTypeBeta) &&
               !lexedString.Equals(ReleaseTypeGamma) &&
               !lexedString.Equals(ReleaseTypeReleaseCandidate))
            {
                tokens.push_back(SemVerToken::MakeError(Format<c8>(u8"Invalid string \"{}\".", lexedString)));
            }

            tokens.push_back(SemVerToken::MakeString(lexedString));
            --iter;
        }
        else
        {
            tokens.push_back(SemVerToken::MakeError(Format<c8>(u8"Invalid character ''{}''.", c)));
            break;
        }
    }

    // tokens.push_back(SemVerToken::MakeEof());

    return tokens;
}

}
