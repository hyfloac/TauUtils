#pragma once

#if defined(STRING_IN_DEV) || 0
#include "String.hpp"
#endif

template<>
inline DynStringT<c32> StringCast(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8::CalculateCodePoints(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c32>();
    }

    if(len < 16)
    {
        c32 newStr[16];
        newStr[len] = U'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            return DynStringT<c32>();
        }

        return DynStringT(static_cast<const c32*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c32));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c32* const newStr = ::new(refCount + 1) c32[len + 1];
        newStr[len] = U'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c32>();
        }

        return DynStringT<c32>::passControl(refCount, newStr, len, [](const c32* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c8> StringCast(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf8::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c8>();
    }

    if(len < 16)
    {
        c8 newStr[16];
        newStr[len] = u8'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            return DynStringT<c8>();
        }

        return DynStringT<c8>(static_cast<const c8*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c8));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c8* const newStr = ::new(refCount + 1) c8[len + 1];
        newStr[len] = u8'\0';

        if(tau::string::utf8::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c8>();
        }

        return DynStringT<c8>::passControl(refCount, newStr, len, [](const c8* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c32> StringCast(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.String();

    const iSys len = tau::string::utf16::CalculateCodePoints(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c32>();
    }

    if(len < 16)
    {
        c32 newStr[16];
        newStr[len] = U'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            return DynStringT<c32>();
        }

        return DynStringT(static_cast<const c32*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c32));

        if(!placement)
        {
            return DynStringT<c32>();
        }

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c32* const newStr = ::new(refCount + 1) c32[len + 1];
        newStr[len] = U'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c32>();
        }

        return DynStringT<c32>::passControl(refCount, newStr, len, [](const c32* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCast(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, false, false) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c16* const newStr = ::new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, false, false) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c8> StringCast(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c8>();
    }

    if(len < 16)
    {
        c8 newStr[16];
        newStr[len] = u8'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            return DynStringT<c8>();
        }

        return DynStringT(static_cast<const c8*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c8));

        if(!placement)
        {
            return DynStringT<c8>();
        }

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c8* const newStr = ::new(refCount + 1) c8[len + 1];
        newStr[len] = u8'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c8>();
        }

        return DynStringT<c8>::passControl(refCount, newStr, len, [](const c8* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCast(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c16* const newStr = ::new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}



template<>
inline DynStringT<c16> StringCastFlipEndian(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.String();

    const iSys len = static_cast<iSys>(string.Length());

    if(len <= 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        tau::string::utf16::FlipEndian(rawStr, newStr, len);

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        tau::string::utf16::FlipEndian(rawStr, newStr, len);

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c32> StringCastFlipEndian(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = static_cast<iSys>(string.Length());

    if(len <= 0)
    {
        return DynStringT<c32>();
    }

    if(len < 16)
    {
        c32 newStr[16];
        newStr[len] = U'\0';

        tau::string::utf16::FlipEndian(rawStr, newStr, len);

        return DynStringT<c32>(static_cast<const c32*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c32));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c32* const newStr = new(refCount + 1) c32[len + 1];
        newStr[len] = U'\0';

        tau::string::utf16::FlipEndian(rawStr, newStr, len);

        return DynStringT<c32>::passControl(refCount, newStr, len, [](const c32* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCastFlipEndian(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true, false) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true, false) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCastFlipEndian(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()));

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}



template<>
inline DynStringT<c16> StringCastSkipBom(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()), 0, 0, true);

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, false, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c16* const newStr = ::new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, false, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c8> StringCastSkipBom(const DynStringT<c16>& string) noexcept
{
    const c16* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()), 0, 0, true);

    if(len < 0)
    {
        return DynStringT<c8>();
    }

    if(len < 16)
    {
        c8 newStr[16];
        newStr[len] = u8'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true) <= 0)
        {
            return DynStringT<c8>();
        }

        return DynStringT(static_cast<const c8*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c8));

        if(!placement)
        {
            return DynStringT<c8>();
        }

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c8* const newStr = ::new(refCount + 1) c8[len + 1];
        newStr[len] = u8'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c8>();
        }

        return DynStringT<c8>::passControl(refCount, newStr, len, [](const c8* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCastSkipBom(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()), 0, 0, true);

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, false, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        c16* const newStr = ::new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, false, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c8> StringCastSkipBom(const DynStringT<c8>& string) noexcept
{
    using CharIn = c8;
    using CharOut = c8;

    const CharIn* rawStr = string.String();

    iSys len = static_cast<iSys>(string.Length());

    if(len <= 0)
    {
        return DynStringT<CharOut>();
    }

    if(len >= 3)
    {
        if(rawStr[0] == 0xEF &&
           rawStr[1] == 0xBB &&
           rawStr[2] == 0xBF) // Skip BOM
        {
            len -= 3;
            rawStr += 3;
        }
        else
        {
            return string;
        }
    }
    else
    {
        return string;
    }

    if(len < 16)
    {
        CharOut newStr[16];
        newStr[len] = u8'\0';

        (void) ::std::copy_n(rawStr, len, newStr);

        return DynStringT<CharOut>(static_cast<const CharOut*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(CharOut));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        CharOut* const newStr = ::new(refCount + 1) CharOut[len + 1];
        newStr[len] = u8'\0';

        (void) ::std::copy_n(rawStr, len, newStr);

        return DynStringT<CharOut>::passControl(refCount, newStr, len, [](const CharOut* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCastSkipBom(const DynStringT<c16>& string) noexcept
{
    using CharIn = c16;
    using CharOut = c16;

    const CharIn* rawStr = string.String();

    iSys len = static_cast<iSys>(string.Length());

    if(len <= 0)
    {
        return DynStringT<CharOut>();
    }

    if(len >= 1)
    {
        if(rawStr[0] == 0xFEFF || rawStr[0] == 0xFFFE) // Skip BOM
        {
            len -= 1;
            rawStr += 1;
        }
        else
        {
            return string;
        }
    }
    else
    {
        return string;
    }

    if(len < 16)
    {
        CharOut newStr[16];
        newStr[len] = u'\0';

        (void) ::std::copy_n(rawStr, len, newStr);

        return DynStringT<CharOut>(static_cast<const CharOut*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(CharOut));

        ReferenceCounter::Type* const refCount = ::new(placement) ReferenceCounter::Type(1);
        CharOut* const newStr = ::new(refCount + 1) CharOut[len + 1];
        newStr[len] = u'\0';

        (void) ::std::copy_n(rawStr, len, newStr);

        return DynStringT<CharOut>::passControl(refCount, newStr, len, [](const CharOut* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}




template<>
inline DynStringT<c16> StringCastFlipEndianSkipBom(const DynStringT<c32>& string) noexcept
{
    const c32* const rawStr = string.String();

    const iSys len = tau::string::utf16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()), 0, 0, true);

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

template<>
inline DynStringT<c16> StringCastFlipEndianSkipBom(const DynStringT<c8>& string) noexcept
{
    const c8* const rawStr = string.String();

    const iSys len = tau::string::utf8_16::CalculateCodeUnits(rawStr, static_cast<iSys>(string.Length()), 0, 0, true);

    if(len < 0)
    {
        return DynStringT<c16>();
    }

    if(len < 16)
    {
        c16 newStr[16];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16, true, true) <= 0)
        {
            return DynStringT<c16>();
        }

        return DynStringT<c16>(static_cast<const c16*>(newStr));
    }
    else
    {
        void* const placement = ::TauUtilsAllocate(sizeof(ReferenceCounter::Type) + (len + 1) * sizeof(c16));

        ReferenceCounter::Type* const refCount = new(placement) ReferenceCounter::Type(1);
        c16* const newStr = new(refCount + 1) c16[len + 1];
        newStr[len] = u'\0';

        if(tau::string::utf8_16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len, true, true) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}
