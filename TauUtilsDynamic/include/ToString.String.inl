#pragma once

#if defined(IN_DEV) || 0
  #include "ToString.hpp"
#endif

namespace tau {

template<typename CharTo>
inline DynStringT<CharTo> ToString(const CharTo* str) noexcept
{
    return DynStringT<CharTo>(str);
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const CharFrom* str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    DynStringT<CharFrom> tmp(str);
    return StringCast<CharTo>(tmp);
}

template<typename CharTo>
inline DynStringT<CharTo> ToString(const DynStringT<CharTo>& str) noexcept
{
    return str;
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const DynStringT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    return StringCast<CharTo>(str);
}

template<typename CharTo>
inline DynStringT<CharTo> ToString(const ConstExprStringT<CharTo>& str) noexcept
{
    return DynStringT<CharTo>(str);
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const ConstExprStringT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    DynStringT<CharFrom> tmp(str);
    return StringCast<CharTo>(tmp);
}

template<typename CharTo>
inline DynStringT<CharTo> ToString(const DynStringViewT<CharTo>& str) noexcept
{
    return DynStringT<CharTo>(str);
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const DynStringViewT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    DynStringT<CharFrom> tmp(str);
    return StringCast<CharTo>(tmp);
}

template<typename CharTo>
inline DynStringT<CharTo> ToString(const StringBaseT<CharTo>& str) noexcept
{
    return DynStringT<CharTo>(str);
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const StringBaseT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    DynStringT<CharFrom> tmp(str);
    return StringCast<CharTo>(tmp);
}

template<typename CharTo>
inline DynStringT<CharTo> ToString(const CharTo c) noexcept
{
    CharTo str[2];
    str[0] = c;
    str[1] = CharTo{ '\0' };
    return DynStringT<CharTo>(1, reinterpret_cast<CharTo*>(str));
}

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const CharFrom c, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>*) noexcept
{
    CharFrom str[2];
    str[0] = c;
    str[1] = CharFrom{ '\0' };
    DynStringT<CharFrom> tmp(1, reinterpret_cast<CharFrom*>(str));
    return StringCast<CharTo>(tmp);
}

}
