#pragma once

#include "NumTypes.hpp"

#define ENUM_FLAG_FLIP(ENUM) \
    inline ENUM operator ~(const ENUM a) noexcept \
    { return static_cast<ENUM>(~static_cast<u64>(a)); }

#define ENUM_FLAG_BINOP(ENUM, OP) \
    inline ENUM operator OP(const ENUM a, const ENUM b) noexcept \
    { return static_cast<ENUM>(static_cast<u64>(a) OP static_cast<u64>(b)); } \
    template<typename Int> \
    inline Int operator OP(const Int a, const ENUM b) noexcept \
    { return static_cast<ENUM>(a OP static_cast<Int>(b)); }


#define ENUM_FLAG_COMPOUND_BINOP(ENUM, OP) \
    inline ENUM& operator OP##=(ENUM& a, const ENUM b) noexcept \
    { \
        a = a OP b; \
        return a; \
    } \
    template<typename Int> \
    inline Int& operator OP##=(Int& a, const ENUM b) noexcept \
    { \
        a = a OP b; \
        return a; \
    }

#define ENUM_FLAG_BINOPS(ENUM) \
    ENUM_FLAG_BINOP(ENUM, |) \
    ENUM_FLAG_BINOP(ENUM, &) \
    ENUM_FLAG_BINOP(ENUM, ^)

#define ENUM_FLAG_COMPOUND_BINOPS(ENUM) \
    ENUM_FLAG_COMPOUND_BINOP(ENUM, |) \
    ENUM_FLAG_COMPOUND_BINOP(ENUM, &) \
    ENUM_FLAG_COMPOUND_BINOP(ENUM, ^)

#define ENUM_FLAG_DIRECT_ACCESSOR(ENUM) \
    [[nodiscard]] inline bool HasFlag(const ENUM in, const ENUM flag) noexcept \
    { return (static_cast<u64>(in) & static_cast<u64>(flag)) != 0; } \
    [[nodiscard]] inline ENUM SetFlag(const ENUM& in, const ENUM flag) noexcept \
    { return in | flag; } \
    inline ENUM& SetFlag(ENUM& in, const ENUM flag) noexcept \
    { return in |= flag; } \
    [[nodiscard]] inline ENUM UnsetFlag(const ENUM& in, const ENUM flag) noexcept \
    { return in & ~flag; } \
    inline ENUM& UnsetFlag(ENUM& in, const ENUM flag) noexcept \
    { return in &= ~flag; } \
    [[nodiscard]] inline ENUM SetFlag(const ENUM& in, const ENUM flag, const bool enable) \
    { return enable ? SetFlag(in, flag) : UnsetFlag(in, flag); } \
    [[nodiscard]] inline ENUM& SetFlag(ENUM& in, const ENUM flag, const bool enable) \
    { return enable ? SetFlag(in, flag) : UnsetFlag(in, flag); }

#define ENUM_FLAG_DIRECT_ACCESSOR_INT(ENUM) \
    template<typename Int> \
    [[nodiscard]] inline bool HasFlag(const Int in, const ENUM flag) noexcept \
    { return (in & static_cast<Int>(flag)) != 0; } \
    template<typename Int> \
    [[nodiscard]] inline Int SetFlag(const Int& in, const ENUM flag) noexcept \
    { return in | static_cast<Int>(flag); } \
    template<typename Int> \
    inline Int& SetFlag(Int& in, const ENUM flag) noexcept \
    { return in |= static_cast<Int>(flag); } \
    template<typename Int> \
    [[nodiscard]] inline Int UnsetFlag(const Int& in, const ENUM flag) noexcept \
    { return in & ~static_cast<Int>(flag); } \
    template<typename Int> \
    inline Int& UnsetFlag(Int& in, const ENUM flag) noexcept \
    { return in &= ~static_cast<Int>(flag); } \
    template<typename Int> \
    [[nodiscard]] inline Int SetFlag(const Int& in, const ENUM flag, const bool enable) \
    { return enable ? SetFlag(in, flag) : UnsetFlag(in, flag); } \
    template<typename Int> \
    [[nodiscard]] inline Int& SetFlag(Int& in, const ENUM flag, const bool enable) \
    { return enable ? SetFlag(in, flag) : UnsetFlag(in, flag); }

#define ENUM_FLAGS(ENUM) \
    ENUM_FLAG_FLIP(ENUM) \
    ENUM_FLAG_BINOPS(ENUM) \
    ENUM_FLAG_COMPOUND_BINOPS(ENUM) \
    ENUM_FLAG_DIRECT_ACCESSOR(ENUM) \
    ENUM_FLAG_DIRECT_ACCESSOR_INT(ENUM)

// /*
//  *   A series of templates for producing binary arithmetic
//  * on enum bit fields.
//  */
//
// template<typename _T> inline _T  operator ~(_T a) noexcept;
//
// template<typename _T> inline _T operator |(_T a, _T b) noexcept;
// template<typename _T> inline _T operator &(_T a, _T b) noexcept;
// template<typename _T> inline _T operator ^(_T a, _T b) noexcept;
//
// template<typename _T> inline _T& operator |=(_T& a, const _T& b) noexcept;
// template<typename _T> inline _T& operator &=(_T& a, const _T& b) noexcept;
// template<typename _T> inline _T& operator ^=(_T& a, const _T& b) noexcept;
//
// template<typename _Int, typename _Enum> inline _Int  operator |(_Int a, _Enum b) noexcept;
// template<typename _Int, typename _Enum> inline _Int  operator &(_Int a, _Enum b) noexcept;
// template<typename _Int, typename _Enum> inline _Int  operator ^(_Int a, _Enum b) noexcept;
//
// template<typename _Int, typename _Enum> inline _Int& operator |=(_Int& a, const _Enum& b) noexcept;
// template<typename _Int, typename _Enum> inline _Int& operator &=(_Int& a, const _Enum& b) noexcept;
// template<typename _Int, typename _Enum> inline _Int& operator ^=(_Int& a, const _Enum& b) noexcept;
//
// template<typename _T> inline bool  hasFlag(      _T  in, _T flag) noexcept;
// template<typename _T> inline _T    setFlag(const _T& in, _T flag) noexcept;
// template<typename _T> inline _T&   setFlag(      _T& in, _T flag) noexcept;
// template<typename _T> inline _T  unsetFlag(const _T& in, _T flag) noexcept;
// template<typename _T> inline _T& unsetFlag(      _T& in, _T flag) noexcept;
//
// template<typename _Int, typename _Enum> inline bool    hasFlag(      _Int  in, _Enum flag) noexcept;
// template<typename _Int, typename _Enum> inline _Int    setFlag(const _Int& in, _Enum flag) noexcept;
// template<typename _Int, typename _Enum> inline _Int&   setFlag(      _Int& in, _Enum flag) noexcept;
// template<typename _Int, typename _Enum> inline _Int  unsetFlag(const _Int& in, _Enum flag) noexcept;
// template<typename _Int, typename _Enum> inline _Int& unsetFlag(      _Int& in, _Enum flag) noexcept;
//
// template<typename _T> inline _T setFlag(const _T& in, _T flag, bool enable) noexcept;
// template<typename _T> inline _T setFlag(      _T& in, _T flag, bool enable) noexcept;
//
// template<typename _Int, typename _Enum> inline _Int setFlag(const _Int& in, _Enum flag, bool enable) noexcept;
// template<typename _Int, typename _Enum> inline _Int setFlag(      _Int& in, _Enum flag, bool enable) noexcept;
//
// #include "EnumBitFields.inl"
