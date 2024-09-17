#pragma once
// ReSharper disable CppClangTidyClangDiagnosticUnusedFunction

// ReSharper disable once CppInconsistentNaming
#define __STDC_WANT_LIB_EXT1__ 1  // NOLINT(clang-diagnostic-reserved-id-macro, clang-diagnostic-reserved-macro-identifier, modernize-macro-to-enum)
#pragma warning(push, 0)
#include <cctype>
#include <functional>
#pragma warning(pop)

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "TauMacros.hpp"
#include "ReferenceCounter.hpp"
#include "TUMaths.hpp"

#ifndef __has_feature
  #define __has_feature(X) 0
#endif

#define STR_SWITCH(PARAM, BLOCK, DEFAULT_BLOCK) \
{ \
    const auto& _tmp__strSwitch_ = PARAM; \
    bool _switch_defaulted_ = true; \
    switch(_tmp__strSwitch_.HashCode()) BLOCK \
    if(_switch_defaulted_) DEFAULT_BLOCK \
}

#define STR_CASE(CASE, BLOCK) \
case ConstExprString(CASE).HashCode(): \
    if(ConstExprString(CASE).Equals(_tmp__strSwitch_.c_str())) { \
        _switch_defaulted_ = false; \
        BLOCK \
    } else { break; }


#define WSTR_CASE(CASE, BLOCK) \
case WConstExprString(CASE).HashCode(): \
    if(WConstExprString(CASE).Equals(_tmp__strSwitch_.c_str())) { \
        _switch_defaulted_ = false; \
        BLOCK \
    } else { break; }

#define C8STR_CASE(CASE, BLOCK) \
case C8ConstExprString(CASE).HashCode(): \
    if(C8ConstExprString(CASE).Equals(_tmp__strSwitch_.c_str())) { \
        _switch_defaulted_ = false; \
        BLOCK \
    } else { break; }

#define C16STR_CASE(CASE, BLOCK) \
case C16ConstExprString(CASE).HashCode(): \
    if(C16ConstExprString(CASE).Equals(_tmp__strSwitch_.c_str())) { \
        _switch_defaulted_ = false; \
        BLOCK \
    } else { break; }

#define C32STR_CASE(CASE, BLOCK) \
case C32ConstExprString(CASE).HashCode(): \
    if(C32ConstExprString(CASE).Equals(_tmp__strSwitch_.c_str())) { \
        _switch_defaulted_ = false; \
        BLOCK \
    } else { break; }

namespace tau::string {

template<typename Char>
struct LargeString final
{
    DEFAULT_CM_PU(LargeString);
public:
    ReferenceCounter RefCount;
    const Char* Buffer;
public:
    constexpr LargeString() noexcept;
    
    template<uSys Len>
    constexpr LargeString(const Char (&str)[Len]) noexcept;

    constexpr LargeString(const Char* buffer, uSys codeUnits) noexcept;
    constexpr LargeString(ReferenceCounter::Type* refCount, const Char* buffer) noexcept;

    constexpr ~LargeString() noexcept;
};

template<typename Char>
union SmallLargeString final
{
    using LargeString_t = tau::string::LargeString<Char>;

    u8 LargeString[sizeof(LargeString_t)];
    Char StackString[16];

    constexpr SmallLargeString() noexcept
        : StackString { }
    { }

    [[nodiscard]] constexpr       LargeString_t* AsLargeStringPtr()       noexcept { return ::std::bit_cast<      LargeString_t*>(LargeStringRawPtr()); }
    [[nodiscard]] constexpr const LargeString_t* AsLargeStringPtr() const noexcept { return ::std::bit_cast<const LargeString_t*>(LargeStringRawPtr()); }
    [[nodiscard]] constexpr       LargeString_t& AsLargeString()       noexcept { return *AsLargeStringPtr(); }
    [[nodiscard]] constexpr const LargeString_t& AsLargeString() const noexcept { return *AsLargeStringPtr(); }

    [[nodiscard]] constexpr       u8* LargeStringRawPtr()       noexcept { return LargeString; }
    [[nodiscard]] constexpr const u8* LargeStringRawPtr() const noexcept { return LargeString; }
};

template<typename Char>
struct StringData final
{
public:
    SmallLargeString<Char> Buffer;
    uSys Length; // The length in code units
public:
    constexpr StringData() noexcept;
    constexpr StringData(nullptr_t) noexcept;
    
    template<uSys Len>
    constexpr StringData(const Char (&str)[Len]) noexcept;

    constexpr StringData(const Char* buffer, uSys codeUnits) noexcept;
    constexpr StringData(ReferenceCounter::Type* refCount, const Char* buffer, uSys codeUnits) noexcept;

    constexpr ~StringData() noexcept;

    constexpr StringData(const StringData& copy) noexcept;
    constexpr StringData(StringData&& move) noexcept;

    constexpr StringData& operator=(const StringData& copy) noexcept;
    constexpr StringData& operator=(StringData&& move) noexcept;
    
    template<uSys Len>
    constexpr StringData& operator=(const Char (&str)[Len]) noexcept;

    constexpr void Reset(const Char* buffer, uSys codeUnits) noexcept;
    constexpr void Reset(ReferenceCounter::Type* refCount, const Char* buffer, uSys codeUnits) noexcept;

    [[nodiscard]] constexpr const Char* String() const noexcept { return Length < 16 ? Buffer.StackString : Buffer.AsLargeString().Buffer; }
};

}

template<typename Char>
Char toLower(Char c) noexcept = delete;

template<typename Char>
Char toUpper(Char c) noexcept = delete;

/**
 * Converts a string to lower case.
 *
 * @return
 *      The number of characters that are, or would need to be stored.
 */
template<typename Char>
uSys toLower(const Char* str, [[tau::nullable]] Char* store) noexcept;

/**
 * Converts a string to upper case.
 *
 * @return
 *      The number of characters that are, or would need to be stored.
 */
template<typename Char>
uSys toUpper(const Char* str, [[tau::nullable]] Char* store) noexcept;

template<typename Char>
constexpr uSys findHashCode([[tau::nonnull]] const Char* str) noexcept;

template<typename Char>
constexpr uSys findHashCode([[tau::nonnull]] const Char* str, uSys len) noexcept;

template<typename Char>
constexpr uSys strLength([[tau::nonnull]] const Char* str) noexcept;

template<typename Char>
constexpr i32 strCompare([[tau::nonnull]] const Char* lhs, [[tau::nonnull]] const Char* rhs) noexcept;

template<typename Char>
constexpr i32 strCompare([[tau::nonnull]] const Char* lhs, [[tau::nonnull]] const Char* rhs, uSys length) noexcept;

inline bool equalsIgnoreCase(const char* RESTRICT lhs, const char* RESTRICT rhs) noexcept
{
    while(*lhs != '\0' && *rhs != '\0')
    {
        if(tolower(*lhs) != tolower(*rhs))
        { return false; }
        ++lhs;
        ++rhs;
    }

    return *lhs == '\0' && *rhs == '\0';
}

namespace cexpr {

template<typename Char, uSys Len>
constexpr inline uSys strlen(const Char(&str)[Len]) noexcept;

template<typename Char, uSys Len>
constexpr inline bool streq(const Char(&lhs)[Len], const Char(&rhs)[Len]);

template<typename Char, uSys Len>
constexpr inline bool streqIgnoreCase(const Char(&lhs)[Len], const Char(&rhs)[Len]);

template<typename Char, uSys Len>
constexpr inline uSys findHashCode(const Char(&str)[Len]) noexcept;

}

template<typename Char>
class StringBaseT;

template<typename Char>
class StringIteratorT;

template<typename Char>
class ConstExprStringT;

template<typename Char, uSys Len>
class ConstExprStackStringT;

template<typename Char>
class DynStringT;

template<typename Char>
class DynStringViewT;

template<typename Char>
class StringBuilderT;

template<typename Char>
class StringBaseT
{
    DEFAULT_CONSTRUCT_PO(StringBaseT);
    DEFAULT_DESTRUCT_VI(StringBaseT);
    DEFAULT_CM_PO(StringBaseT);
public:
    [[nodiscard]] [[tau::nonnull]] virtual const Char* String() const noexcept = 0;
    [[nodiscard]] virtual uSys Length() const noexcept = 0;
    [[nodiscard]] virtual uSys HashCode() const noexcept = 0;

    [[nodiscard]] virtual bool Equals(const StringBaseT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual bool Equals(const ConstExprStringT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual bool Equals(const DynStringT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual bool Equals(const DynStringViewT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual bool Equals(const Char* other) const noexcept = 0;

    [[nodiscard]] virtual i32 CompareTo(const StringBaseT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual i32 CompareTo(const DynStringT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual i32 CompareTo(const DynStringViewT<Char>& other) const noexcept = 0;
    [[nodiscard]] virtual i32 CompareTo(const Char* other) const noexcept = 0;

    [[nodiscard]] [[tau::nonnull]] const Char* c_str() const noexcept { return String(); }
    [[nodiscard]] uSys length() const noexcept { return Length(); }

    [[nodiscard]] [[tau::nonnull]] operator const Char* () const noexcept { return String(); }
    [[nodiscard]] uSys operator()() const noexcept { return HashCode(); }
    [[nodiscard]] uSys hashCode() const noexcept { return HashCode(); }

    [[nodiscard]] bool equals(const StringBaseT<Char>& other)      const noexcept { return Equals(other); }
    [[nodiscard]] bool equals(const ConstExprStringT<Char>& other) const noexcept { return Equals(other); }
    [[nodiscard]] bool equals(const DynStringT<Char>& other)       const noexcept { return Equals(other); }
    [[nodiscard]] bool equals(const DynStringViewT<Char>& other)   const noexcept { return Equals(other); }
    [[nodiscard]] bool equals(const Char* other)                   const noexcept { return Equals(other); }

    [[nodiscard]] i32 compareTo(const StringBaseT<Char>& other)      const noexcept { return CompareTo(other); }
    [[nodiscard]] i32 compareTo(const ConstExprStringT<Char>& other) const noexcept { return CompareTo(other); }
    [[nodiscard]] i32 compareTo(const DynStringT<Char>& other)       const noexcept { return CompareTo(other); }
    [[nodiscard]] i32 compareTo(const DynStringViewT<Char>& other)   const noexcept { return CompareTo(other); }
    [[nodiscard]] i32 compareTo(const Char* other)                   const noexcept { return CompareTo(other); }

#if __has_feature(__cpp_impl_three_way_comparison)
    [[nodiscard]] i32 operator <=>(const StringBaseT<Char>& other)      const noexcept { return compareTo(other); }
    [[nodiscard]] i32 operator <=>(const ConstExprStringT<Char>& other) const noexcept { return compareTo(other); }
    [[nodiscard]] i32 operator <=>(const DynStringT<Char>& other)       const noexcept { return compareTo(other); }
    [[nodiscard]] i32 operator <=>(const DynStringViewT<Char>& other)   const noexcept { return compareTo(other); }
    [[nodiscard]] i32 operator <=>(const Char* const other)             const noexcept { return compareTo(other); }
#endif

    [[nodiscard]] bool operator ==(const ConstExprStringT<Char>& other) const noexcept { return  equals(other); }
    [[nodiscard]] bool operator !=(const ConstExprStringT<Char>& other) const noexcept { return !equals(other); }
    [[nodiscard]] bool operator < (const ConstExprStringT<Char>& other) const noexcept { return compareTo(other) < 0; }
    [[nodiscard]] bool operator > (const ConstExprStringT<Char>& other) const noexcept { return compareTo(other) > 0; }
    [[nodiscard]] bool operator <=(const ConstExprStringT<Char>& other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] bool operator >=(const ConstExprStringT<Char>& other) const noexcept { return compareTo(other) >= 0; }

    [[nodiscard]] bool operator ==(const DynStringT<Char>& other) const noexcept { return  equals(other); }
    [[nodiscard]] bool operator !=(const DynStringT<Char>& other) const noexcept { return !equals(other); }
    [[nodiscard]] bool operator < (const DynStringT<Char>& other) const noexcept { return compareTo(other) < 0; }
    [[nodiscard]] bool operator > (const DynStringT<Char>& other) const noexcept { return compareTo(other) > 0; }
    [[nodiscard]] bool operator <=(const DynStringT<Char>& other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] bool operator >=(const DynStringT<Char>& other) const noexcept { return compareTo(other) >= 0; }

    [[nodiscard]] bool operator ==(const DynStringViewT<Char>& other) const noexcept { return  equals(other); }
    [[nodiscard]] bool operator !=(const DynStringViewT<Char>& other) const noexcept { return !equals(other); }
    [[nodiscard]] bool operator < (const DynStringViewT<Char>& other) const noexcept { return compareTo(other) < 0; }
    [[nodiscard]] bool operator > (const DynStringViewT<Char>& other) const noexcept { return compareTo(other) > 0; }
    [[nodiscard]] bool operator <=(const DynStringViewT<Char>& other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] bool operator >=(const DynStringViewT<Char>& other) const noexcept { return compareTo(other) >= 0; }

    [[nodiscard]] bool operator ==(const Char* const other) const noexcept { return  equals(other); }
    [[nodiscard]] bool operator !=(const Char* const other) const noexcept { return !equals(other); }
    [[nodiscard]] bool operator < (const Char* const other) const noexcept { return compareTo(other) < 0; }
    [[nodiscard]] bool operator > (const Char* const other) const noexcept { return compareTo(other) > 0; }
    [[nodiscard]] bool operator <=(const Char* const other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] bool operator >=(const Char* const other) const noexcept { return compareTo(other) >= 0; }
};

template<typename Char>
class StringIteratorT final
{
    DEFAULT_DESTRUCT(StringIteratorT);
    DEFAULT_CM_PU(StringIteratorT);
private:
    const Char* m_String;
    uSys m_Length;
    uSys m_Index;
public:
    constexpr StringIteratorT(const Char* string, uSys length, uSys index) noexcept;

    constexpr StringIteratorT<Char>& operator++() noexcept;
    constexpr StringIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] constexpr StringIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] constexpr StringIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] constexpr inline Char operator*() const noexcept;

    [[nodiscard]] constexpr bool operator ==(const StringIteratorT<Char>& other) const noexcept;
    [[nodiscard]] constexpr bool operator !=(const StringIteratorT<Char>& other) const noexcept;
};

template<typename Char>
class DynStringCodeUnitIteratorT final
{
    DEFAULT_DESTRUCT(DynStringCodeUnitIteratorT);
    DEFAULT_CM_PU(DynStringCodeUnitIteratorT);
private:
    using StringData = tau::string::StringData<Char>;

    StringData m_String;
    uSys m_Start;
    uSys m_Index;
public:
    constexpr DynStringCodeUnitIteratorT(const StringData& string, uSys index, uSys start = 0) noexcept;

    constexpr DynStringCodeUnitIteratorT<Char>& operator++() noexcept;
    constexpr DynStringCodeUnitIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] constexpr inline Char operator*() const noexcept;

    [[nodiscard]] constexpr bool operator ==(const DynStringCodeUnitIteratorT<Char>& other) const noexcept;
    [[nodiscard]] constexpr bool operator !=(const DynStringCodeUnitIteratorT<Char>& other) const noexcept;
};

template<typename Char>
class DynStringCodePointIteratorT final
{
    DEFAULT_DESTRUCT(DynStringCodePointIteratorT);
    DEFAULT_CM_PU(DynStringCodePointIteratorT);
private:
    using StringData = tau::string::StringData<Char>;

    StringData m_String;
    iSys m_Start;
    iSys m_Index;
    c32 m_CurrentCodePoint;
public:
    constexpr DynStringCodePointIteratorT(const StringData& string, iSys index, iSys start = 0) noexcept;
    constexpr DynStringCodePointIteratorT(const StringData& string, uSys index, uSys start = 0) noexcept;
protected:
    constexpr DynStringCodePointIteratorT(const StringData& string, iSys index, iSys start, c32 currentCodePoint) noexcept;
    constexpr DynStringCodePointIteratorT(const StringData& string, uSys index, uSys start, c32 currentCodePoint) noexcept;
public:
    constexpr DynStringCodePointIteratorT<Char>& operator++() noexcept;
    constexpr DynStringCodePointIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] constexpr inline c32 operator*() const noexcept;

    [[nodiscard]] constexpr bool operator ==(const DynStringCodePointIteratorT<Char>& other) const noexcept;
    [[nodiscard]] constexpr bool operator !=(const DynStringCodePointIteratorT<Char>& other) const noexcept;
};

template<typename Char>
class ConstExprStringT final : public StringBaseT<Char>
{
    DEFAULT_DESTRUCT_C(ConstExprStringT);
    DEFAULT_CM_PUC(ConstExprStringT);
private:
    const Char* m_String;
    uSys m_Length;
    uSys m_Hash;
public:
    template<uSys Len>
    constexpr ConstExprStringT(const Char (&str)[Len]) noexcept;

    template<uSys Len>
    constexpr ConstExprStringT<Char>& operator=(const Char (&str)[Len]) noexcept;

    [[nodiscard]] constexpr const Char* String() const noexcept override { return m_String; }
    [[nodiscard]] constexpr uSys Length() const noexcept override { return m_Length; }
    [[nodiscard]] constexpr uSys HashCode() const noexcept override { return m_Hash; }
    
    [[nodiscard]] constexpr StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, m_Length, 0); }
    [[nodiscard]] constexpr StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, m_Length, m_Length - 1); }

    template<uSys Len>
    [[nodiscard]] constexpr bool Equals(const Char (&str)[Len]) const noexcept;
    
    [[nodiscard]] constexpr bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] constexpr i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const Char* str) const noexcept override;
    
    [[nodiscard]] constexpr Char operator [](uSys index) const noexcept;
    [[nodiscard]] constexpr Char at(uSys index) const noexcept;
private:
    template<typename CChar>
    friend class ConstExprStringT;

    template<typename CChar, uSys Len>
    friend class ConstExprStackStringT;

    template<typename CChar>
    friend class DynStringT;

    template<typename CChar>
    friend class DynStringViewT;

    template<typename CChar>
    friend class StringBuilderT;
};

template<typename Char, uSys Len>
class ConstExprStackStringT final : public StringBaseT<Char>
{
    DEFAULT_DESTRUCT_C(ConstExprStackStringT);
    DEFAULT_CM_PUC(ConstExprStackStringT);
public:
    Char m_String[Len];
    uSys m_Length;
    uSys m_Hash;
public:
    consteval ConstExprStackStringT(const Char(&str)[Len]) noexcept;

    consteval ConstExprStackStringT(const Char* const str, const uSys length) noexcept;

    [[nodiscard]] constexpr const Char* String() const noexcept override { return m_String; }
    [[nodiscard]] constexpr uSys Length() const noexcept override { return m_Length; }
    [[nodiscard]] constexpr uSys HashCode() const noexcept override { return m_Hash; }

    [[nodiscard]] constexpr StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, m_Length, 0); }
    [[nodiscard]] constexpr StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, m_Length, m_Length - 1); }

    template<uSys OLen>
    [[nodiscard]] consteval bool Equals(const Char(&str)[OLen]) const noexcept;
    template<uSys OLen>
    [[nodiscard]] consteval bool Equals(const ConstExprStackStringT<Char, OLen>& other) const noexcept;

    [[nodiscard]] constexpr bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] constexpr i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const Char* str) const noexcept override;

    [[nodiscard]] constexpr Char operator [](uSys index) const noexcept;
    [[nodiscard]] constexpr Char at(uSys index) const noexcept;
private:
    template<typename CChar>
    friend class ConstExprStringT;

    template<typename CChar>
    friend class DynStringT;

    template<typename CChar>
    friend class DynStringViewT;

    template<typename CChar>
    friend class StringBuilderT;
};

template<typename Char>
class DynStringT final : public StringBaseT<Char>
{
    DEFAULT_DESTRUCT(DynStringT);
    DEFAULT_CM_PU(DynStringT);
public:
    template<uSys Len>
    constexpr static DynStringT<Char> FromStatic(const Char (&str)[Len]) noexcept;

    static DynStringT<Char> PassControl(const Char* str, ::std::function<void(Char*)> deleteStr = nullptr) noexcept;
    static DynStringT<Char> PassControl(ReferenceCounter::Type* refCount, const Char* str, uSys length, ::std::function<void(Char*)> deleteStr, ::std::function<void(ReferenceCounter::Type*)> deleteRefCount) noexcept;

    static DynStringT<Char> passControl(const Char* str, ::std::function<void(Char*)> deleteStr = nullptr) noexcept
    {
        return PassControl(str, deleteStr);
    }

    static DynStringT<Char> passControl(ReferenceCounter::Type* refCount, const Char* str, uSys length, ::std::function<void(Char*)> deleteStr, ::std::function<void(ReferenceCounter::Type*)> deleteRefCount) noexcept
    {
        return PassControl(refCount, str, length, deleteStr, deleteRefCount);
    }
private:
    tau::string::StringData<Char> m_Data;
    uSys m_Hash;
private:
    template<uSys Len>
    constexpr DynStringT(const Char (&str)[Len]) noexcept;

    constexpr DynStringT(const Char* string, uSys length) noexcept;
    constexpr DynStringT(ReferenceCounter::Type* refCount, const Char* string, uSys length) noexcept;
public:
    constexpr DynStringT() noexcept;
    constexpr DynStringT(const Char* string) noexcept;
    constexpr DynStringT(uSys length, const Char* string) noexcept;
    constexpr DynStringT(const ConstExprStringT<Char>& string) noexcept;
    constexpr DynStringT(const DynStringViewT<Char>& string) noexcept;
    constexpr DynStringT(const StringBaseT<Char>& string) noexcept;
    
    constexpr DynStringT<Char>& operator=(const Char* string) noexcept;
    constexpr DynStringT<Char>& operator=(const ConstExprStringT<Char>& string) noexcept;
    constexpr DynStringT<Char>& operator=(const DynStringViewT<Char>& string) noexcept;
    // ReSharper disable once CppHidingFunction
    constexpr DynStringT<Char>& operator=(const StringBaseT<Char>& string) noexcept;

    [[nodiscard]] [[tau::nonnull]] constexpr const Char* String() const noexcept override { return m_Data.String(); }
    [[nodiscard]] constexpr uSys Length() const noexcept override { return m_Data.Length; }
    [[nodiscard]] constexpr uSys HashCode() const noexcept override { return m_Hash; }
    
    [[nodiscard]] constexpr bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] constexpr i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const Char* str) const noexcept override;

    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> Begin() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data, 0_uz); }
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char>   End() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data, m_Data.Length); }

    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> begin() const noexcept { return begin(); }
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char>   end() const noexcept { return end(); }

    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char> BeginUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, 0); }
    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char>   EndUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Data.Length - 1); }

    [[nodiscard]] constexpr DynStringT<Char> Concat(const StringBaseT<Char>&      other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const ConstExprStringT<Char>& other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const DynStringT<Char>&       other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const DynStringViewT<Char>&   other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const Char*                   other) const noexcept;

    [[nodiscard]] constexpr DynStringT<Char> operator +(const StringBaseT<Char>&      other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const ConstExprStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const DynStringT<Char>&       other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const DynStringViewT<Char>&   other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const Char* const             other) const noexcept { return concat(other); }

    [[nodiscard]] constexpr DynStringT<Char> SubStringLen([[tau::inclusive]] uSys begin, uSys length) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> SubString([[tau::inclusive]] uSys begin, [[tau::exclusive]] uSys end) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> SubString([[tau::inclusive]] uSys from) const noexcept;
    
    [[nodiscard]] constexpr Char operator [](uSys index) const noexcept;
    [[nodiscard]] constexpr Char At(uSys index) const noexcept;
private:
    [[nodiscard]] constexpr DynStringT<Char> _concat(uSys len, const Char* str) const noexcept;
private:
    template<typename CharC>
    friend class ConstExprStringT;

    template<typename CharC>
    friend class DynStringT;

    template<typename CharC>
    friend class DynStringViewT;

    template<typename CharC>
    friend class StringBuilderT;
};

template<typename Char>
class DynStringViewT final : public StringBaseT<Char>
{
    DEFAULT_DESTRUCT(DynStringViewT);
    DEFAULT_CM_PU(DynStringViewT);
private:
    tau::string::StringData<Char> m_Data;
    uSys m_Start;
    uSys m_Length;
    uSys m_Hash;
private:
    constexpr DynStringViewT(uSys begin, uSys length, const DynStringT<Char>& str) noexcept;
    constexpr DynStringViewT(uSys begin, uSys length, const DynStringViewT<Char>& str) noexcept;
public:
    constexpr DynStringViewT(const DynStringT<Char>& str, uSys begin, uSys end) noexcept;
    constexpr DynStringViewT(const DynStringViewT<Char>& str, uSys begin, uSys end) noexcept;

    constexpr DynStringViewT(DynStringT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    constexpr DynStringViewT(DynStringViewT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    
    [[nodiscard]] static inline constexpr DynStringViewT<Char> Create(const DynStringT<Char>& str, uSys begin, uSys length) noexcept;
    [[nodiscard]] static inline constexpr DynStringViewT<Char> Create(const DynStringViewT<Char>& str, uSys begin, uSys length) noexcept;

    [[nodiscard]] static inline constexpr DynStringViewT<Char> Create(DynStringT<Char>&& str, uSys begin, uSys length) noexcept = delete;
    [[nodiscard]] static inline constexpr DynStringViewT<Char> Create(DynStringViewT<Char>&& str, uSys begin, uSys length) noexcept = delete;

    constexpr DynStringViewT<Char>& Reset(const DynStringT<Char>& str, uSys begin, uSys end) noexcept;
    constexpr DynStringViewT<Char>& ResetLen(const DynStringT<Char>& str, uSys begin, uSys length) noexcept;

    constexpr DynStringViewT<Char>& Reset(DynStringT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    constexpr DynStringViewT<Char>& ResetLen(DynStringT<Char>&& str, uSys begin, uSys length) noexcept = delete;

    [[nodiscard]] constexpr const Char* String() const noexcept override { return m_Data.String() + m_Start; }
    [[nodiscard]] constexpr uSys Length() const noexcept override { return m_Length; }
    [[nodiscard]] constexpr uSys HashCode() const noexcept override { return m_Hash; }
    
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> Begin() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length, m_Start); }
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char>   End() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length - 1, m_Start); }

    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char> begin() const noexcept { return begin(); }
    [[nodiscard]] constexpr DynStringCodePointIteratorT<Char>   end() const noexcept { return end(); }

    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char> BeginUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Start, m_Start); }
    [[nodiscard]] constexpr DynStringCodeUnitIteratorT<Char>   EndUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Data.Length - 1, m_Start); }

    [[nodiscard]] constexpr bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] constexpr i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] constexpr i32 CompareTo(const Char* str) const noexcept override;

    [[nodiscard]] constexpr DynStringT<Char> Concat(const StringBaseT<Char>& other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const ConstExprStringT<Char>& other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const DynStringT<Char>& other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const DynStringViewT<Char>& other) const noexcept;
    [[nodiscard]] constexpr DynStringT<Char> Concat(const Char* other) const noexcept;

    [[nodiscard]] constexpr DynStringT<Char> operator +(const StringBaseT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const ConstExprStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const DynStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const DynStringViewT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] constexpr DynStringT<Char> operator +(const Char* const             other) const noexcept { return concat(other); }

    [[nodiscard]] inline constexpr DynStringT<Char> SubStringLen([[tau::inclusive]] uSys begin, uSys length) const noexcept;
    [[nodiscard]] inline constexpr DynStringT<Char> SubString([[tau::inclusive]] uSys begin, [[tau::exclusive]] uSys end) const noexcept;
    [[nodiscard]] inline constexpr DynStringT<Char> SubString([[tau::inclusive]] uSys from) const noexcept;
    
    [[nodiscard]] inline constexpr Char operator [](uSys index) const noexcept;
    [[nodiscard]] inline constexpr Char At(uSys index) const noexcept;
private:
    [[nodiscard]] constexpr DynStringT<Char> _concat(uSys len, const Char* str) noexcept;
private:
    template<typename CharC>
    friend class ConstExprStringT;

    template<typename CharC>
    friend class DynStringT;

    template<typename CharC>
    friend class DynStringViewT;

    template<typename CharC>
    friend class StringBuilderT;
};

template<typename Char>
class StringBuilderT final
{
public:
    constexpr StringBuilderT() noexcept;
    constexpr StringBuilderT(uSys initialSize) noexcept;
    constexpr StringBuilderT(const Char* string) noexcept;

    constexpr ~StringBuilderT() noexcept;

    constexpr StringBuilderT(const StringBuilderT<Char>& copy) noexcept;
    constexpr StringBuilderT(StringBuilderT<Char>&& move) noexcept;

    constexpr StringBuilderT<Char>& operator =(const StringBuilderT<Char>& copy) noexcept;
    constexpr StringBuilderT<Char>& operator =(StringBuilderT<Char>&& move) noexcept;

    [[nodiscard]] constexpr const Char* String() const noexcept { return m_String; }
    [[nodiscard]] constexpr const Char* c_str() const noexcept { return m_String; }
    [[nodiscard]] constexpr uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] constexpr uSys length() const noexcept { return m_Length; }
    [[nodiscard]] constexpr uSys Size() const noexcept { return m_Size; }
    [[nodiscard]] constexpr uSys size() const noexcept { return m_Size; }

    [[nodiscard]] constexpr bool equals(const StringBuilderT<Char>& other) const noexcept;

    [[nodiscard]] constexpr i32 compareTo(const StringBuilderT<Char>& other) const noexcept;

    [[nodiscard]] constexpr StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, m_Length, 0); }
    [[nodiscard]] constexpr StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, m_Length, m_Length - 1); }

    inline constexpr StringBuilderT<Char>& Append(const StringBaseT<Char>& string) noexcept;
    inline constexpr StringBuilderT<Char>& Append(const ConstExprStringT<Char>& string) noexcept;
    inline constexpr StringBuilderT<Char>& Append(const DynStringT<Char>& string) noexcept;
    inline constexpr StringBuilderT<Char>& Append(const DynStringViewT<Char>& string) noexcept;
    inline constexpr StringBuilderT<Char>& Append(const StringBuilderT<Char>& string) noexcept;
    inline constexpr StringBuilderT<Char>& Append(const Char* string) noexcept;
    template<typename CharDeduced = Char>
    inline constexpr StringBuilderT<Char>& Append(::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) noexcept;
    inline constexpr StringBuilderT<Char>& Append(c32 c) noexcept;

    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const StringBaseT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const ConstExprStringT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const DynStringT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const DynStringViewT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const StringBuilderT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline constexpr StringBuilderT<Char>& Append(const CharFrom* string) noexcept;

    inline constexpr StringBuilderT<Char>& Reset() noexcept;
    inline constexpr StringBuilderT<Char>& Reset(uSys newSize) noexcept;
    inline constexpr StringBuilderT<Char>& ResetIf(uSys maxSize, uSys newSize) noexcept;
    inline constexpr StringBuilderT<Char>& reset() noexcept { return Reset(); }
    inline constexpr StringBuilderT<Char>& reset(const uSys newSize) noexcept { return Reset(newSize); }
    inline constexpr StringBuilderT<Char>& resetIf(const uSys maxSize, const uSys newSize) noexcept { return ResetIf(maxSize, newSize); }

    inline constexpr StringBuilderT<Char>& Backspace() noexcept;
    inline constexpr StringBuilderT<Char>& Backspace(uSys count) noexcept;
    inline constexpr StringBuilderT<Char>& backspace() noexcept { return Backspace(); }
    inline constexpr StringBuilderT<Char>& backspace(const uSys count) noexcept { return Backspace(count); }

#if __has_feature(__cpp_impl_three_way_comparison)
    [[nodiscard]] i32 operator <=> (const StringBuilderT<Char>& other) const noexcept { return compareTo(other); }
#endif

    [[nodiscard]] inline constexpr bool operator ==(const StringBuilderT<Char>& other) const noexcept { return  equals(other); }
    [[nodiscard]] inline constexpr bool operator !=(const StringBuilderT<Char>& other) const noexcept { return !equals(other); }
    [[nodiscard]] inline constexpr bool operator < (const StringBuilderT<Char>& other) const noexcept { return compareTo(other) <  0; }
    [[nodiscard]] inline constexpr bool operator > (const StringBuilderT<Char>& other) const noexcept { return compareTo(other) >  0; }
    [[nodiscard]] inline constexpr bool operator <=(const StringBuilderT<Char>& other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] inline constexpr bool operator >=(const StringBuilderT<Char>& other) const noexcept { return compareTo(other) >= 0; }

    [[nodiscard]] inline constexpr StringBuilderT<Char>& operator +(const ConstExprStringT<Char>& other) noexcept { return append(other); }
    [[nodiscard]] inline constexpr StringBuilderT<Char>& operator +(const DynStringT<Char>& other)       noexcept { return append(other); }
    [[nodiscard]] inline constexpr StringBuilderT<Char>& operator +(const DynStringViewT<Char>& other)   noexcept { return append(other); }
    [[nodiscard]] inline constexpr StringBuilderT<Char>& operator +(const StringBuilderT<Char>& other)   noexcept { return append(other); }
    [[nodiscard]] inline constexpr StringBuilderT<Char>& operator +(const Char* other)                   noexcept { return append(other); }

    inline constexpr StringBuilderT<Char>& operator +=(const ConstExprStringT<Char>& other) noexcept { return append(other); }
    inline constexpr StringBuilderT<Char>& operator +=(const DynStringT<Char>& other)       noexcept { return append(other); }
    inline constexpr StringBuilderT<Char>& operator +=(const DynStringViewT<Char>& other)   noexcept { return append(other); }
    inline constexpr StringBuilderT<Char>& operator +=(const StringBuilderT<Char>& other)   noexcept { return append(other); }
    inline constexpr StringBuilderT<Char>& operator +=(const Char* other)                   noexcept { return append(other); }

    [[nodiscard]] inline constexpr DynStringT<Char> ToString() const noexcept { return DynStringT<Char>(m_String); }
    [[nodiscard]] inline constexpr DynStringT<Char> toString() const noexcept { return DynStringT<Char>(m_String); }

    template<uSys Len>
    [[nodiscard]] inline consteval ConstExprStackStringT<Char, Len> ToStringC() const noexcept { return ConstExprStackStringT<Char, Len>(m_String, m_Length); }
    
    [[nodiscard]] inline constexpr Char operator[](uSys index) const noexcept;
    [[nodiscard]] inline constexpr Char At(uSys index) const noexcept;
    [[nodiscard]] inline constexpr Char at(const uSys index) const noexcept { return At(index); }
private:
    constexpr void AssertSize(uSys newLength) noexcept;
public:
    constexpr void Append(const Char* string, uSys length) noexcept;

    template<typename CharFrom>
    constexpr void Append(const CharFrom* string, uSys length) noexcept;

    constexpr void append(const Char* const string, const uSys length) noexcept { Append(string, length); }
    template<typename CharFrom>
    constexpr void append(const CharFrom* const string, const uSys length) noexcept { Append(string, length); }
private:
    Char* m_String;
    uSys m_Length;
    uSys m_Size;
};

#include "String.CalculateCodeUnits.inl"

template<typename Char, iSys Len>
class ConstEvalStringBuilderT final
{
    DEFAULT_DESTRUCT_C(ConstEvalStringBuilderT);
public:
    consteval ConstEvalStringBuilderT() noexcept;
    consteval ConstEvalStringBuilderT(const Char(&string)[Len]) noexcept;
    
    consteval ConstEvalStringBuilderT(const ConstEvalStringBuilderT<Char, Len>& copy) noexcept;
    consteval ConstEvalStringBuilderT(ConstEvalStringBuilderT<Char, Len>&& move) noexcept;
    
    [[nodiscard]] consteval const Char* String() const noexcept { return m_String; }
    [[nodiscard]] consteval const Char* c_str() const noexcept { return m_String; }
    [[nodiscard]] consteval uSys Length() const noexcept { return Len; }
    [[nodiscard]] consteval uSys length() const noexcept { return Len; }
    [[nodiscard]] consteval uSys Size() const noexcept { return Len; }
    [[nodiscard]] consteval uSys size() const noexcept { return Len; }

    // template<iSys OLen>
    // [[nodiscard]] consteval bool Equals(const ConstExprStackStringT<Char, OLen>& string) const noexcept;
    //
    // template<iSys OLen>
    // [[nodiscard]] consteval i32 CompareTo(const ConstExprStackStringT<Char, OLen>& string) const noexcept;

    [[nodiscard]] consteval StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, Len, 0); }
    [[nodiscard]] consteval StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, Len, Len - 1); }

    template<iSys OLen>
    inline consteval ConstEvalStringBuilderT<Char, Len + OLen - 1> Append(const ConstExprStackStringT<Char, OLen>& string) const noexcept;

    template<iSys OLen>
    inline consteval ConstEvalStringBuilderT<Char, Len + OLen - 1> Append(const Char(&str)[OLen]) const noexcept;

    inline consteval auto Append(const Char* const str, const iSys oLen) const noexcept;

    template<typename CharDeduced = Char>
    inline consteval auto Append(::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) const noexcept;

    inline consteval auto Append(c32 c) const noexcept;
    
    template<typename CharFrom, iSys OLen>
    inline consteval auto Append(const ConstExprStackStringT<CharFrom, OLen>& string) const noexcept;
    template<typename CharFrom, iSys OLen>
    inline consteval auto Append(const CharFrom(&str)[OLen]) const noexcept;
    template<typename CharFrom>
    inline consteval auto Append(const CharFrom* const str, const iSys oLen) const noexcept;
    
    inline consteval ConstEvalStringBuilderT<Char, maxT(Len - 1, 0)> Backspace() const noexcept;
    inline consteval auto Backspace(uSys count) const noexcept;
    
    [[nodiscard]] inline consteval DynStringT<Char> ToString() const noexcept { return ConstExprStackStringT<Char, Len>(m_String); }
    [[nodiscard]] inline consteval DynStringT<Char> toString() const noexcept { return ConstExprStackStringT<Char, Len>(m_String); }

    [[nodiscard]] inline consteval Char operator[](const uSys index) const noexcept { return m_String[index]; }
    [[nodiscard]] inline consteval Char At(const uSys index) const noexcept { return index >= Len ? Char { '\0' } : m_String[index]; }
private:
    Char m_String[Len];
};

template<typename CharTo, typename CharFrom>
DynStringT<CharTo> StringCast(const DynStringT<CharFrom>& string) noexcept = delete;

using StringBase = StringBaseT<char>;
using WStringBase = StringBaseT<wchar_t>;
using C8StringBase = StringBaseT<c8>;
using C16StringBase = StringBaseT<c16>;
using C32StringBase = StringBaseT<c32>;

using StringIterator = StringIteratorT<char>;
using WStringIterator = StringIteratorT<wchar_t>;
using C8StringIterator = StringIteratorT<c8>;
using C16StringIterator = StringIteratorT<c16>;
using C32StringIterator = StringIteratorT<c32>;

using ConstExprString = ConstExprStringT<char>;
using WConstExprString = ConstExprStringT<wchar_t>;
using C8ConstExprString = ConstExprStringT<c8>;
using C16ConstExprString = ConstExprStringT<c16>;
using C32ConstExprString = ConstExprStringT<c32>;

using DynString = DynStringT<char>;
using WDynString = DynStringT<wchar_t>;
using C8DynString = DynStringT<c8>;
using C16DynString = DynStringT<c16>;
using C32DynString = DynStringT<c32>;

using DynStringView = DynStringViewT<char>;
using WDynStringView = DynStringViewT<wchar_t>;
using C8DynStringView = DynStringViewT<c8>;
using C16DynStringView = DynStringViewT<c16>;
using C32DynStringView = DynStringViewT<c32>;

using StringBuilder = StringBuilderT<char>;
using WStringBuilder = StringBuilderT<wchar_t>;
using C8StringBuilder = StringBuilderT<c8>;
using C16StringBuilder = StringBuilderT<c16>;
using C32StringBuilder = StringBuilderT<c32>;

#include "String.utf8.inl"
#include "String.utf16.inl"
#include "String.utf8_16.inl"
#include "String.UnicodeTransform.inl"
#include "String.inl"
#include "String.UnicodeIterator.inl"
#include "String.Cast.inl"
#include "String.Unicode.inl"
