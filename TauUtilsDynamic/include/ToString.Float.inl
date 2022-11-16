#pragma once

#if defined(IN_DEV) || 1
#include "ToString.hpp"
#endif

namespace tau {

template<typename Char>
inline DynStringT<Char> ToString(const f32 val, const u32 precision) noexcept
{
    Char str[256];

    ::PrintFloat32(str, ::std::size(str), val, PrintFloatFormat::Positional, precision);

    return DynStringT<Char>::PassControl(reinterpret_cast<Char*>(str));
}

template<typename Char>
inline DynStringT<Char> ToString(const f64 val, const u32 precision) noexcept
{
    Char str[256];

    ::PrintFloat64(str, ::std::size(str), val, PrintFloatFormat::Positional, precision);

    return DynStringT<Char>::PassControl(reinterpret_cast<Char*>(str));
}

}
