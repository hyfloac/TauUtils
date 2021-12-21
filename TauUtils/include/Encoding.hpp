#pragma once

#include "NumTypes.hpp"
#include "Objects.hpp"
#include "Safeties.hpp"

class Encoding
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
    virtual c32 ToUpper(c32 c) noexcept
    { return c; }

    virtual c32 ToLower(c32 c) noexcept
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
