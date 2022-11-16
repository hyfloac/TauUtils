#pragma once

#include "IntToString.hpp"
#include "String.hpp"
#include "Dragon4.hpp"

namespace tau {

template<typename CharTo>
inline DynStringT<CharTo> ToString(const CharTo* str) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const CharFrom* str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename CharTo>
inline DynStringT<CharTo> ToString(const DynStringT<CharTo>& str) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const DynStringT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename CharTo>
inline DynStringT<CharTo> ToString(const ConstExprStringT<CharTo>& str) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const ConstExprStringT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename CharTo>
inline DynStringT<CharTo> ToString(const DynStringViewT<CharTo>& str) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const DynStringViewT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename CharTo>
inline DynStringT<CharTo> ToString(const StringBaseT<CharTo>& str) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const StringBaseT<CharFrom>& str, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename CharTo>
inline DynStringT<CharTo> ToString(const CharTo c) noexcept;

template<typename CharTo, typename CharFrom>
inline DynStringT<CharTo> ToString(const CharFrom c, ::std::enable_if_t<!::std::is_same_v<CharTo, CharFrom>, void>* = nullptr) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(bool val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(i8 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(i16 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(i32 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(i64 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(u8 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(u16 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(u32 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(u64 val) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(f32 val, u32 precision = 5) noexcept;

template<typename Char>
inline DynStringT<Char> ToString(f64 val, u32 precision = 5) noexcept;

}

#include "ToString.String.inl"
#include "ToString.Int.inl"
#include "ToString.Float.inl"
