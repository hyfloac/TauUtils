#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "Safeties.hpp"
#include "TauMacros.hpp"

class TAU_UTILS_LIB Encoding
{
    DEFAULT_CONSTRUCT_PO(Encoding);
    DEFAULT_DESTRUCT_VI(Encoding);
    DEFAULT_CM_PO(Encoding);
public:
    static Ref<Encoding> ForName(const char* name) noexcept;

    static void RegisterEncoding(const char* name, const Ref<Encoding>& codec) noexcept;
    static void RegisterEncoding(const char* name, Ref<Encoding>&& codec) noexcept;

    static void UnregisterEncoding(const char* name);
public:
    virtual c32 ToUpper(const c32 c) noexcept
    { return c; }

    virtual c32 ToLower(const c32 c) noexcept
    { return c; }

    virtual iSys ToUpper(const char* originalString, char* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToUpper(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToUpper(const c8* originalString, c8* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToUpper(const c16* originalString, c16* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToUpper(const c32* originalString, c32* newString, uSys newStringLength) noexcept = 0;
    
    virtual iSys ToLower(const char* originalString, char* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToLower(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToLower(const c8* originalString, c8* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToLower(const c16* originalString, c16* newString, uSys newStringLength) noexcept = 0;
    virtual iSys ToLower(const c32* originalString, c32* newString, uSys newStringLength) noexcept = 0;
};

class TAU_UTILS_LIB DefaultEncoding final : public Encoding
{
    DEFAULT_CONSTRUCT_PU(DefaultEncoding);
    DEFAULT_DESTRUCT(DefaultEncoding);
    DEFAULT_CM_PU(DefaultEncoding);
public:
    c32 ToUpper(c32 c) noexcept override;
    c32 ToLower(c32 c) noexcept override;

    iSys ToUpper(const char* originalString, char* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToUpper(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToUpper(const c8* originalString, c8* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToUpper(const c16* originalString, c16* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToUpper(const c32* originalString, c32* newString, uSys newStringLength) noexcept override { return -1; }

    iSys ToLower(const char* originalString, char* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToLower(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToLower(const c8* originalString, c8* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToLower(const c16* originalString, c16* newString, uSys newStringLength) noexcept override { return -1; }
    iSys ToLower(const c32* originalString, c32* newString, uSys newStringLength) noexcept override { return -1; }
};

inline c32 DefaultEncoding::ToUpper(const c32 c) noexcept
{
    // a-z
    if((c >= 0x61 && c <= 0x7A) ||   // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
        (c >= 0xE0 && c <= 0xFE))     // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER THORN
    {
        return c - 0x20;
    }

    if(c == 0x03BC) // MICRO SIGN
    { return 0xB5; }

    if((c >= 0x0100 && c <= 0x012F) || // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
        (c >= 0x0132 && c <= 0x0148) || // LATIN SMALL LIGATURE IJ - LATIN SMALL LETTER N WITH CARON
        (c >= 0x014A && c <= 0x017E) || // LATIN SMALL LETTER ENG - LATIN SMALL LETTER Z WITH CARON
        (c >= 0x0182 && c <= 0x0185))   // LATIN SMALL LETTER B WITH TOPBAR - LATIN SMALL LETTER TONE SIX
    {
        return c % 2 == 0 ? c : c - 1;
    }

    if(c == 0x017F) // LATIN SMALL LETTER LONG S
    { return 0x73; }

    if(c == 0x0253) // LATIN SMALL LETTER B WITH HOOK
    { return 0x0181; }

    if(c == 0x0254) // LATIN SMALL LETTER OPEN O
    { return 0x0186; }

    if(c == 0x0188) // LATIN SMALL LETTER C WITH HOOK
    { return 0x0187; }

    return c;
}

inline c32 DefaultEncoding::ToLower(const c32 c) noexcept
{
    return Encoding::ToLower(c);
}
