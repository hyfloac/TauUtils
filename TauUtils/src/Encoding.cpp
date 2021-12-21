#include "Encoding.hpp"
#include <unordered_map>

namespace tau::utils::internal {

::std::unordered_map<const char*, Ref<Encoding>> g_Encodings;

}

NullableRef<Encoding> Encoding::ForName(const char* const name) noexcept
{
    using tau::utils::internal::g_Encodings;

    if(g_Encodings.contains(name))
    {
        return g_Encodings.at(name);
    }

    return nullptr;
}

void Encoding::RegisterEncoding(const char* const name, const NullableRef<Encoding>& codec) noexcept
{
    tau::utils::internal::g_Encodings.insert_or_assign(name, codec);
}

void Encoding::RegisterEncoding(const char* const name, NullableRef<Encoding>&& codec) noexcept
{
    tau::utils::internal::g_Encodings.insert_or_assign(name, ::std::move(codec));
}

void Encoding::UnregisterEncoding(const char* const name)
{
    tau::utils::internal::g_Encodings.erase(name);
}

class DefaultEncoding final : public Encoding
{
    DEFAULT_CONSTRUCT_PU(DefaultEncoding);
    DEFAULT_DESTRUCT(DefaultEncoding);
    DEFAULT_CM_PU(DefaultEncoding);
public:
    c32 ToUpper(c32 c) noexcept override;
    c32 ToLower(c32 c) noexcept override;

    iSys ToUpper(const char* originalString, char* newString, uSys newStringLength) noexcept override;
    iSys ToUpper(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept override;
    iSys ToUpper(const c8* originalString, c8* newString, uSys newStringLength) noexcept override;
    iSys ToUpper(const c16* originalString, c16* newString, uSys newStringLength) noexcept override;
    iSys ToUpper(const c32* originalString, c32* newString, uSys newStringLength) noexcept override;

    iSys ToLower(const char* originalString, char* newString, uSys newStringLength) noexcept override;
    iSys ToLower(const wchar_t* originalString, wchar_t* newString, uSys newStringLength) noexcept override;
    iSys ToLower(const c8* originalString, c8* newString, uSys newStringLength) noexcept override;
    iSys ToLower(const c16* originalString, c16* newString, uSys newStringLength) noexcept override;
    iSys ToLower(const c32* originalString, c32* newString, uSys newStringLength) noexcept override;
};

c32 DefaultEncoding::ToUpper(const c32 c) noexcept
{
    // a-z
    if((c >= 0x61 && c <= 0x7A) ||   // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
       (c >= 0xE0 && c <= 0xFE))     // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER THORN
    { return c - 0x20; }

    if(c == 0x03BC) // MICRO SIGN
    { return 0xB5; }

    if((c >= 0x0100 && c <= 0x012F) || // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
       (c >= 0x0132 && c <= 0x0148) || // LATIN SMALL LIGATURE IJ - LATIN SMALL LETTER N WITH CARON
       (c >= 0x014A && c <= 0x017E) || // LATIN SMALL LETTER ENG - LATIN SMALL LETTER Z WITH CARON
       (c >= 0x0182 && c <= 0x0185))   // LATIN SMALL LETTER B WITH TOPBAR - LATIN SMALL LETTER TONE SIX
    { return c % 2 == 0 ? c : c - 1; }

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

c32 DefaultEncoding::ToLower(c32 c) noexcept
{
    return Encoding::ToLower(c);
}
