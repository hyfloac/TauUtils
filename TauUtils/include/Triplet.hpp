#pragma once

#include "Objects.hpp"
#include <utility>

template<typename T0, typename T1, typename T2>
struct Triplet final
{
    DEFAULT_CONSTRUCT_PU(Triplet);
    DEFAULT_DESTRUCT(Triplet);
    DEFAULT_CM_PU(Triplet);
public:
    T0 t0;
    T1 t1;
    T2 t2;
public:
    inline Triplet(const T0& _t0, const T1& _t1, const T2& _t2) noexcept
        : t0(_t0)
        , t1(_t1)
        , t2(_t2)
    { }

    inline Triplet(const T0& _t0, const T1& _t1, T2&& _t2) noexcept
        : t0(_t0)
        , t1(_t1)
        , t2(::std::move(_t2))
    { }

    inline Triplet(const T0& _t0, T1&& _t1, const T2& _t2) noexcept
        : t0(_t0)
        , t1(::std::move(_t1))
        , t2(_t2)
    { }

    inline Triplet(const T0& _t0, T1&& _t1, T2&& _t2) noexcept
        : t0(_t0)
        , t1(::std::move(_t1))
        , t2(::std::move(_t2))
    { }

    inline Triplet(T0&& _t0, const T1& _t1, const T2& _t2) noexcept
        : t0(::std::move(_t0))
        , t1(_t1)
        , t2(_t2)
    { }

    inline Triplet(T0&& _t0, const T1& _t1, T2&& _t2) noexcept
        : t0(::std::move(_t0))
        , t1(_t1)
        , t2(::std::move(_t2))
    { }

    inline Triplet(T0&& _t0, T1&& _t1, const T2& _t2) noexcept
        : t0(::std::move(_t0))
        , t1(::std::move(_t1))
        , t2(_t2)
    { }

    inline Triplet(T0&& _t0, T1&& _t1, T2&& _t2) noexcept
        : t0(::std::move(_t0))
        , t1(::std::move(_t1))
        , t2(::std::move(_t2))
    { }
};
