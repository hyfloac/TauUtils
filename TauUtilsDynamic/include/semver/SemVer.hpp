#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "String.hpp"
#include "StringFormat.hpp"

namespace tau::semver {

[[nodiscard]] inline constexpr u32 MakeVersion(const u8 majorVersion, const u8 minorVersion, const u16 patchVersion) noexcept
{
    return (static_cast<u32>(majorVersion) << 24) | (static_cast<u32>(minorVersion) << 16) | static_cast<u32>(patchVersion);
}

[[nodiscard]] inline constexpr u8 GetMajorVersion(const u32 version) noexcept
{
    return static_cast<u8>((version >> 24) & 0xFF);
}

[[nodiscard]] inline constexpr u8 GetMinorVersion(const u32 version) noexcept
{
    return static_cast<u8>((version >> 16) & 0xFF);
}

[[nodiscard]] inline constexpr u16 GetPatchVersion(const u32 version) noexcept
{
    return static_cast<u16>(version & 0xFFFF);
}

enum class EReleaseType
{
    None = 0,
    Release = None,
    ReleaseCandidate,
    Gamma,
    Beta,
    Alpha
};

[[nodiscard]] inline constexpr C8ConstExprString ToString(const EReleaseType releaseType) noexcept
{
    switch(releaseType)
    {
        case EReleaseType::None: return u8"";
        case EReleaseType::ReleaseCandidate: return u8"rc";
        case EReleaseType::Gamma: return u8"gamma";
        case EReleaseType::Beta: return u8"beta";
        case EReleaseType::Alpha: return u8"alpha";
        default: return u8"unknown";
    }
}

struct SemVer final
{
    DEFAULT_CONSTRUCT_PUCV(SemVer);
    DEFAULT_DESTRUCT_C(SemVer);
    DEFAULT_CM_PUC(SemVer);
public:
    using StringType = DynString;
public:
    u32 MajorVersion;
    u32 MinorVersion;
    u32 PatchVersion;
    EReleaseType ReleaseType;
    StringType BuildMetadata;
public:
    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion, const EReleaseType releaseType, const StringType& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(releaseType)
        , BuildMetadata(buildMetadata)
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion, const EReleaseType releaseType, StringType&& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(releaseType)
        , BuildMetadata(::std::move(buildMetadata))
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion, const EReleaseType releaseType) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(releaseType)
        , BuildMetadata()
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion = 0) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(EReleaseType::None)
        , BuildMetadata()
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion, const StringType& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(buildMetadata)
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const u32 patchVersion, StringType&& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(patchVersion)
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(::std::move(buildMetadata))
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, const StringType& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(0)
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(buildMetadata)
    { }

    constexpr SemVer(const u32 majorVersion, const u32 minorVersion, StringType&& buildMetadata) noexcept
        : MajorVersion(majorVersion)
        , MinorVersion(minorVersion)
        , PatchVersion(0)
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(::std::move(buildMetadata))
    { }

    constexpr SemVer(const u32 version, const EReleaseType releaseType, const StringType& buildMetadata) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(releaseType)
        , BuildMetadata(buildMetadata)
    { }

    constexpr SemVer(const u32 version, const EReleaseType releaseType, StringType&& buildMetadata) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(releaseType)
        , BuildMetadata(::std::move(buildMetadata))
    { }

    constexpr SemVer(const u32 version, const EReleaseType releaseType) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(releaseType)
        , BuildMetadata()
    { }
    
    constexpr SemVer(const u32 version, const StringType& buildMetadata) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(buildMetadata)
    { }

    constexpr SemVer(const u32 version, StringType&& buildMetadata) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(EReleaseType::None)
        , BuildMetadata(::std::move(buildMetadata))
    { }

    constexpr SemVer(const u32 version) noexcept
        : MajorVersion(GetMajorVersion(version))
        , MinorVersion(GetMinorVersion(version))
        , PatchVersion(GetPatchVersion(version))
        , ReleaseType(EReleaseType::None)
        , BuildMetadata()
    { }

    [[nodiscard]] constexpr bool operator==(const SemVer& other) const noexcept;
    [[nodiscard]] constexpr bool operator!=(const SemVer& other) const noexcept { return !(*this == other); }

    [[nodiscard]] constexpr bool operator<(const SemVer& other) const noexcept;
    [[nodiscard]] constexpr bool operator>(const SemVer& other) const noexcept;
    [[nodiscard]] constexpr bool operator<=(const SemVer& other) const noexcept { return (*this < other) || (*this == other); }
    [[nodiscard]] constexpr bool operator>=(const SemVer& other) const noexcept { return (*this > other) || (*this == other); }

    template<typename Char>
    [[nodiscard]] constexpr DynStringT<Char> ToString() const noexcept
    {
        if(ReleaseType != EReleaseType::None)
        {
            if(BuildMetadata.Length() > 0)
            {
                return Format<Char>(u8"{}.{}.{}-{}+{}", MajorVersion, MinorVersion, PatchVersion, ::tau::semver::ToString(ReleaseType), BuildMetadata);
            }
            else
            {
                return Format<Char>(u8"{}.{}.{}-{}", MajorVersion, MinorVersion, PatchVersion, ::tau::semver::ToString(ReleaseType));
            }
        }
        else
        {
            if(BuildMetadata.Length() > 0)
            {
                return Format<Char>(u8"{}.{}.{}+{}", MajorVersion, MinorVersion, PatchVersion, BuildMetadata);
            }
            else
            {
                return Format<Char>(u8"{}.{}.{}", MajorVersion, MinorVersion, PatchVersion);
            }
        }
    }
};

inline constexpr bool SemVer::operator==(const SemVer& other) const noexcept
{
    if(MajorVersion != other.MajorVersion)
    {
        return false;
    }

    if(MinorVersion != other.MinorVersion)
    {
        return false;
    }

    if(PatchVersion != other.PatchVersion)
    {
        return false;
    }

    if(ReleaseType != other.ReleaseType)
    {
        return false;
    }

    if(!BuildMetadata.Equals(other.BuildMetadata))
    {
        // If one of the two versions doesn't have any build metadata, consider it equal.
        if(BuildMetadata.Length() != 0 && other.BuildMetadata.Length() != 0)
        {
            return false;
        }
    }

    return true;
}

inline constexpr bool SemVer::operator<(const SemVer& other) const noexcept
{
    if(MajorVersion < other.MajorVersion)
    {
        return true;
    }
    else if(MajorVersion == other.MajorVersion)
    {
        if(MinorVersion < other.MinorVersion)
        {
            return true;
        }
        else if(MinorVersion == other.MinorVersion)
        {
            if(PatchVersion < other.PatchVersion)
            {
                return true;
            }
            else if(PatchVersion == other.PatchVersion)
            {
                if(static_cast<int>(ReleaseType) > static_cast<int>(other.ReleaseType))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

inline constexpr bool SemVer::operator>(const SemVer& other) const noexcept
{
    if(MajorVersion > other.MajorVersion)
    {
        return true;
    }
    else if(MajorVersion == other.MajorVersion)
    {
        if(MinorVersion > other.MinorVersion)
        {
            return true;
        }
        else if(MinorVersion == other.MinorVersion)
        {
            if(PatchVersion > other.PatchVersion)
            {
                return true;
            }
            else if(PatchVersion == other.PatchVersion)
            {
                if(static_cast<int>(ReleaseType) < static_cast<int>(other.ReleaseType))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

}
