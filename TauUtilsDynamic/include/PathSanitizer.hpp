/**
 * @file 
 *
 * @brief Sanitizes Win32 paths.
 *
 * Largely based on information from https://googleprojectzero.blogspot.com/2016/02/the-definitive-guide-on-win32-to-nt.html
 */
#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "String.hpp"

namespace tau::file::path {

struct PathSanitizerSettings final
{
    DEFAULT_CONSTRUCT_PUC(PathSanitizerSettings);
    DEFAULT_DESTRUCT_C(PathSanitizerSettings);
    DEFAULT_CM_PUC(PathSanitizerSettings);
public:
    b32 BlockUNCPath : 1;
    b32 BlockLocalDevicePath : 1;
    b32 BlockLocalDeviceDriveAbsolutePath : 1;
    b32 BlockRootLocalDevicePath : 1;
    b32 BlockRootLocalDeviceDriveAbsolutePath : 1;
    b32 BlockRootLocalDeviceDriveRelativePath : 1;
    b32 BlockDriveAbsolutePath : 1;
    b32 BlockDriveRelativePath : 1;
    b32 BlockRootedPath : 1;
    b32 BlockDevices : 1;
    b32 BlockNonLetterDrive : 1;
    b32 BlockDisallowedCharacters : 1;
    b32 Pad : 20;

    consteval PathSanitizerSettings(const ::tau::TIPDefault&) noexcept
        : BlockUNCPath(true)
        , BlockLocalDevicePath(false)
        , BlockLocalDeviceDriveAbsolutePath(false)
        , BlockRootLocalDevicePath(false)
        , BlockRootLocalDeviceDriveAbsolutePath(false)
        , BlockRootLocalDeviceDriveRelativePath(false)
        , BlockDriveAbsolutePath(false)
        , BlockDriveRelativePath(false)
        , BlockRootedPath(false)
        , BlockDevices(true)
        , BlockNonLetterDrive(true)
        , BlockDisallowedCharacters(true)
        , Pad(0)
    { }

    consteval PathSanitizerSettings(const ::tau::TIPRecommended&) noexcept
        : BlockUNCPath(true)
        , BlockLocalDevicePath(true)
        , BlockLocalDeviceDriveAbsolutePath(true)
        , BlockRootLocalDevicePath(true)
        , BlockRootLocalDeviceDriveAbsolutePath(true)
        , BlockRootLocalDeviceDriveRelativePath(true)
        , BlockDriveAbsolutePath(true)
        , BlockDriveRelativePath(true)
        , BlockRootedPath(true)
        , BlockDevices(true)
        , BlockNonLetterDrive(true)
        , BlockDisallowedCharacters(true)
        , Pad(0)
    { }
};

#if !defined(TAU_UTILS_PATH_SAN_SKIP_STATIC_ASSERT)
static_assert(sizeof(PathSanitizerSettings) == sizeof(u32), "PathSanitizerBitFields was not the size of of a u32.");
#endif

[[nodiscard]] inline bool IsValidCharset(const wchar_t* const path, const uSys length) noexcept
{
    uSys colonCount = 0;

    for(uSys i = 0; i < length; ++i)
    {
        if(path[i] <= 31)
        { continue; }

        switch(path[i])
        {
            case L'<':
            case L'>':
            case L'\"':
            case L'|':
            case L'?':
            case L'*': return false;
            default: break;
        }

        if(path[i] == L':')
        {
            ++colonCount;
            if(colonCount > 1)
            {
                return false;
            }
        }

        if(path[i] == L'.') // Ensure file doesn't end with a '.'
        {
            if(i + 1 == length || path[i + 1] == L'\\' || path[i + 1] == L'/')
            {
                if(i > 0)
                {
                    if(path[i - 1] != L'\\' && path[i - 1] != L'/')
                    {
                        if(path[i - 1] == L'.')
                        {
                            if(i > 1 && path[i - 2] != L'\\' && path[i - 2] != L'/')
                            {
                                return false;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

template<typename Char>
[[nodiscard]] inline bool CharMatches(const Char checkChar, const Char* const charset) noexcept
{
    while(*charset)
    {
        if(checkChar == *charset)
        {
            return true;
        }

        ++charset;
    }

    return false;
}

[[nodiscard]] inline wchar_t* FixPathSeparator(const wchar_t* const path, const uSys length, const wchar_t* const badPathSeparators, const wchar_t goodPathSeparator) noexcept
{
    wchar_t* const newPath = TU_NEW_ARR(wchar_t, length + 1);
    (void) ::std::memcpy(newPath, path, length * sizeof(wchar_t));
    newPath[length] = L'\0';

    for(uSys i = 0; i < length; ++i)
    {
        if(CharMatches(newPath[i], badPathSeparators))
        {
            newPath[i] = goodPathSeparator;
        }
    }

    return newPath;
}

[[nodiscard]] inline wchar_t* FixPathSeparatorWin32(const wchar_t* const path, const uSys length) noexcept
{ return FixPathSeparator(path, length, L"/", L'\\'); }

[[nodiscard]] inline wchar_t* FixPathSeparatorNix(const wchar_t* const path, const uSys length) noexcept
{ return FixPathSeparator(path, length, L"\\", L'/'); }

/**
 * Validates that the beginning of a path is valid.
 *
 *   This includes checks like drive specification, and UNC
 * paths.
 */
[[nodiscard]] inline bool ValidatePathPrefix(wchar_t* const path, const uSys length, const PathSanitizerSettings settings) noexcept
{
    if(length < 1)
    { return false; }

    if(path[0] == L'\\') // Rooted, UNC Absolute, Local Device, Root Local Device
    {
        if(length >= 2)
        {
            if(path[1] == L'\\') // UNC Absolute, Local Device, Root Local Device
            {
                if(length >= 3)
                {
                    if(path[2] == L'.') // Local Device
                    {
                        if(settings.BlockLocalDevicePath)
                        {
                            return false;
                        }

                        if(length < 3 || path[3] != L'\\') // Malformed
                        {
                            return false;
                        }

                        if(length >= 6 && path[5] == L':') // Drive Specification
                        {
                            if(settings.BlockLocalDeviceDriveAbsolutePath)
                            {
                                return false;
                            }

                            if(settings.BlockNonLetterDrive)
                            {
                                if(iswalpha(path[2]))
                                {
                                    return false;
                                }
                            }

                            if(length < 7 || path[6] != L'\\') // Malformed
                            {
                                return false;
                            }
                        }
                    }
                    else if(path[2] == L'?') // Root Local Device
                    {
                        if(settings.BlockRootLocalDevicePath)
                        {
                            return false;
                        }

                        if(length < 3 || path[3] != L'\\') // Malformed
                        {
                            return false;
                        }

                        if(length >= 6 && path[5] == L':') // Drive Specification
                        {
                            if(length >= 7 && path[6] == L'\\') // Drive Absolute
                            {
                                if(settings.BlockRootLocalDeviceDriveAbsolutePath)
                                {
                                    return false;
                                }
                            }
                            else if(settings.BlockRootLocalDeviceDriveRelativePath) // Drive Relative
                            {
                                return false;
                            }

                            if(settings.BlockNonLetterDrive)
                            {
                                if(iswalpha(path[2]))
                                {
                                    return false;
                                }
                            }
                        }
                    }
                    else // UNC Absolute
                    {
                        if(settings.BlockUNCPath)
                        {
                            return false;
                        }

                        // Drive specification is not allowed in a UNC absolute path.
                        for(uSys i = 1; i < length; ++i)
                        {
                            if(path[i] == L':')
                            {
                                return false;
                            }
                        }
                    }
                }
            }
            else if(path[1] == L'?') // Root Local Device Bypass
            {
                if(settings.BlockRootLocalDevicePath)
                {
                    return false;
                }

                if(length >= 4 && path[2] == '?' && path[3] == '\\') // Fix bypass to proper syntax.
                {
                    path[1] = '\\';
                }
                else // Malformed
                {
                    return false;
                }
            }
            else // Rooted
            {
                if(settings.BlockRootedPath)
                {
                    return false;
                }

                // Drive specification is not allowed in a rooted path.
                for(uSys i = 1; i < length; ++i)
                {
                    if(path[i] == L':')
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(length >= 2)
        {
            if(path[1] == L':') // Drive Absolute, Drive Relative
            {
                if(length >= 3)
                {
                    if(path[2] == L'\\') // Drive Absolute
                    {
                        if(settings.BlockDriveAbsolutePath)
                        {
                            return false;
                        }
                    }
                    else // Drive Relative
                    {
                        if(settings.BlockDriveRelativePath)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * Identifies a Win32 device anywhere in the path.
 */
[[nodiscard]] inline bool ContainsWin32Device(const wchar_t* const path, const uSys length) noexcept
{
    bool possibleDeviceEnd = true;

    for(iSys i = static_cast<iSys>(length) - 1; i >= 0; --i)
    {
        if(possibleDeviceEnd)
        {
            /*
             *   Technically devices don't have to end with [1-9], they can
             * end in an wide character digit, including U+00B2, U+00B3,
             * U+00B9, or Superscript 2, Superscript 3, Superscript 1
             * respectively.
             *
             * Source: https://googleprojectzero.blogspot.com/2016/02/the-definitive-guide-on-win32-to-nt.html
             *
             *   Devices can also technically be multiple characters if you
             * have an expansion card. Should this be the case you should
             * prefix the path with `\\.\`, but we'll just reject it either
             * way to be safe.
             */
            while(i >= 0 && iswdigit(path[i])) // LPT[1-9], COM[1-9]
            {
                --i;
            }

            if(i < 3) // Not enough characters for a device.
            {
                return false;
            }

            switch(path[i])
            {
                case L'n':
                case L'N': // PRN, CON
                {
                    --i;
                    switch(path[i])
                    {
                        case L'r':
                        case L'R': // PRN
                        {
                            --i;
                            if(path[i] == L'P' || path[i] == L'p')
                            {
                                if(i == 0 || path[i - 1] == L'\\')
                                {
                                    return true;
                                }
                                --i;
                            }
                            break;
                        }
                        case L'o':
                        case L'O': // CON
                        {
                            --i;
                            if(path[i] == L'C' || path[i] == L'c')
                            {
                                if(i == 0 || path[i - 1] == L'\\')
                                {
                                    return true;
                                }
                                --i;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case L'x':
                case L'X': // AUX
                {
                    --i;
                    if(path[i] == L'U' || path[i] == L'u')
                    {
                        --i;
                        if(path[i] == L'A' || path[i] == L'a')
                        {
                            if(i == 0 || path[i - 1] == L'\\')
                            {
                                return true;
                            }
                            --i;
                        }
                    }
                    break;
                }
                case L'l':
                case L'L': // NUL
                {
                    --i;
                    if(path[i] == L'U' || path[i] == L'u')
                    {
                        --i;
                        if(path[i] == L'N' || path[i] == L'n')
                        {
                            if(i == 0 || path[i - 1] == L'\\')
                            {
                                return true;
                            }
                            --i;
                        }
                    }
                    break;
                }
                case L'$': // CONIN$, CONOUT$
                {
                    if(i < 5)  // Not enough characters for a device.
                    {
                        return false;
                    }

                    --i;
                    switch(path[i])
                    {
                        case L'n':
                        case L'N': // CONIN$
                        {
                            --i;
                            if(path[i] == L'I' || path[i] == L'i')
                            {
                                --i;
                                if(path[i] == L'N' || path[i] == L'n')
                                {
                                    --i;
                                    if(path[i] == L'O' || path[i] == L'o')
                                    {
                                        --i;
                                        if(path[i] == L'C' || path[i] == L'c')
                                        {
                                            if(i == 0 || path[i - 1] == L'\\')
                                            {
                                                return true;
                                            }
                                            --i;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        case L't':
                        case L'T': // CONOUT$
                        {
                            if(i < 5)  // Not enough characters for a device.
                            {
                                return false;
                            }

                            --i;
                            if(path[i] == L'U' || path[i] == L'u')
                            {
                                --i;
                                if(path[i] == L'O' || path[i] == L'o')
                                {
                                    --i;
                                    if(path[i] == L'N' || path[i] == L'n')
                                    {
                                        --i;
                                        if(path[i] == L'O' || path[i] == L'o')
                                        {
                                            --i;
                                            if(path[i] == L'C' || path[i] == L'c')
                                            {
                                                if(i == 0 || path[i - 1] == L'\\')
                                                {
                                                    return true;
                                                }
                                                --i;
                                            }
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        /**
         *   Technically a device does not have to be at the end of a
         * path, they are allowed to have arbitrary characters
         * (except '\') after the device, as long as the these
         * characters are prefixed by a '.' or ':', this delimiter is
         * allowed to be prefixed by a space. For safety I'm choosing
         * to consider all whitespace (' ', '\t', '\r', '\n) a valid
         * space, even if that isn't actually true.
         *
         * Example:
         *   COM1       - Valid
         *   COM1a      - Invalid
         *   COM1\      - Invalid
         *   COM1 a     - Invalid
         *   COM1:a     - valid
         *   COM1  :  a - valid
         *   COM1.  a   - valid
         */
        switch(path[i])
        {
            case L' ':
            case L'\t':
            case L'\r':
            case L'\n':
            case L'.':
            case L':':
                possibleDeviceEnd = true;
                break;
            default:
                possibleDeviceEnd = false;
                break;
        }
    }

    return false;
}

[[nodiscard]] inline bool CleanDotDirs(wchar_t* const path, const uSys length) noexcept
{
    uSys insert = 0;
    uSys i = 0;

    if(length >= 2 && path[0] == L'\\' && path[1] == L'\\')
    {
        if(length >= 4 && (path[2] == '?' || path[2] == '.'))
        {
            i = 4;
            insert = 4;
        }
        else
        {
            i = 2;
            insert = 2;
        }
    }

    for(; i < length; ++i)
    {
        if(path[i] == '?') // Ensure there are no '?' in the path.
        {
            return false;
        }

        if(path[i] == L'.')
        {
            if(i > 0)
            {
                if(path[i - 1] == L'\\')
                {
                    if(i < length)
                    {
                        if(path[i + 1] == L'\\')
                        {
                            i += 2;
                        }
                        else if(path[i + 1] == L'.')
                        {
                            if(i + 2 < length)
                            {
                                if(path[i + 2] == L'\\')
                                {
                                    iSys j = static_cast<iSys>(insert) - 2;
                                    while(j >= 0 && path[j] != L'\\')
                                    {
                                        --j;
                                        --insert;
                                    }
                                    i += 2;
                                    insert -= 1;
                                    continue;
                                }
                            }
                            else
                            {
                                iSys j = static_cast<iSys>(insert) - 2;
                                while(j >= 0 && path[j] != L'\\')
                                {
                                    --j;
                                    --insert;
                                }
                                i += 2;
                                insert -= 1;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                continue;
            }
        }

        path[insert++] = path[i];
    }

    path[insert] = L'\0';
    return true;
}

[[nodiscard]] inline WDynString SanitizePath(const WStringBase& path, const PathSanitizerSettings settings) noexcept
{
    if(!IsValidCharset(path.String(), path.Length()))
    {
        return WDynString();
    }

#if defined(_WIN32)
    wchar_t* const newPath = FixPathSeparatorWin32(path.String(), path.Length());
#else
    wchar_t* const newPath = FixPathSeparatorNix(path.String(), path.Length());
#endif

    if(!ValidatePathPrefix(newPath, path.Length(), settings))
    {
        TU_DELETE_T_ARR(newPath, path.Length());
        return WDynString();
    }

    if(!CleanDotDirs(newPath, path.Length()))
    {
        TU_DELETE_T_ARR(newPath, path.Length());
        return WDynString();
    }

    const uSys pathLength = ::std::wcslen(newPath);

    const uSys fullPathLen = GetFullPathNameW(newPath, 0, nullptr, nullptr);

    ReferenceCounter::Type* const refCounter = static_cast<ReferenceCounter::Type*>(::TauUtilsAllocate(fullPathLen + 1 + sizeof(ReferenceCounter::Type)));
    *refCounter = 0;
    wchar_t* const fullPath = reinterpret_cast<wchar_t*>(refCounter + 1);
    fullPath[fullPathLen] = L'\0';
    if(GetFullPathNameW(newPath, static_cast<DWORD>(fullPathLen), fullPath, nullptr) != fullPathLen)
    {
        TU_DELETE_T_ARR(newPath, path.Length());
        ::TauUtilsDeallocate(refCounter);
        return WDynString();
    }

    TU_DELETE_T_ARR(newPath, path.Length());

    if(settings.BlockDevices && ContainsWin32Device(fullPath, pathLength))
    {
        ::TauUtilsDeallocate(refCounter);
        return WDynString();
    }

    return WDynString::PassControl(refCounter, fullPath, fullPathLen, nullptr, [](ReferenceCounter::Type* refCounter) { ::TauUtilsDeallocate(refCounter); });
}

}
