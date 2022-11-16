#pragma once

#if defined(IN_DEV) || 1
  #include "ToString.hpp"
#endif

namespace tau {

template<typename Char>
inline DynStringT<Char> ToString(const bool val) noexcept
{
    Char buffer[MaxCharCount<bool>::Value];
    const i32 length = BtoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const i8 val) noexcept
{
    Char buffer[MaxCharCount<i8>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const i16 val) noexcept
{
    Char buffer[MaxCharCount<i16>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const i32 val) noexcept
{
    Char buffer[MaxCharCount<i32>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const i64 val) noexcept
{
    Char buffer[MaxCharCount<i64>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const u8 val) noexcept
{
    Char buffer[MaxCharCount<u8>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const u16 val) noexcept
{
    Char buffer[MaxCharCount<u16>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const u32 val) noexcept
{
    Char buffer[MaxCharCount<u32>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

template<typename Char>
inline DynStringT<Char> ToString(const u64 val) noexcept
{
    Char buffer[MaxCharCount<u64>::Value];
    const i32 length = ItoA(val, buffer);
    if(length < 0)
    {
        return DynStringT<Char>();
    }

    return DynStringT<Char>(length, buffer);
}

}
