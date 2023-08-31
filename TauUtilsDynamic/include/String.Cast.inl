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

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), 16) <= 0)
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

        if(tau::string::utf16::Transform(rawStr, newStr, static_cast<iSys>(string.Length()), len) <= 0)
        {
            ::TauUtilsDeallocate(placement);
            return DynStringT<c16>();
        }

        return DynStringT<c16>::passControl(refCount, newStr, len, [](const c16* str) { }, [](ReferenceCounter::Type* refCount) { ::TauUtilsDeallocate(refCount); });
    }
}

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
        newStr[len] = U'\0';

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
        newStr[len] = U'\0';

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
