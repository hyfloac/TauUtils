#pragma once
// ReSharper disable CppClangTidyClangDiagnosticUnusedFunction

// ReSharper disable once CppInconsistentNaming
#define __STDC_WANT_LIB_EXT1__ 1  // NOLINT(clang-diagnostic-reserved-id-macro, clang-diagnostic-reserved-macro-identifier)
#pragma warning(push, 0)
#include <cctype>
#include <functional>
#pragma warning(pop)

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "TauMacros.hpp"
#include "ReferenceCounter.hpp"

#define STR_SWITCH(PARAM, BLOCK, DEFAULT_BLOCK) \
{ \
    const auto& _tmp__strSwitch_ = PARAM; \
    bool _switch_defaulted_ = true; \
    switch(_tmp__strSwitch_.hashCode()) BLOCK \
    if(_switch_defaulted_) DEFAULT_BLOCK \
}

#define STR_CASE(CASE, BLOCK) \
case ConstExprString(CASE).hashCode(): \
    if(ConstExprString(CASE).equals(_tmp__strSwitch_.c_str())) { \
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

    LargeString(const Char* buffer, uSys codeUnits) noexcept;
    LargeString(ReferenceCounter::Type* refCount, const Char* buffer) noexcept;

    ~LargeString() noexcept;
};

template<typename Char>
union SmallLargeString final
{
    using LargeString_t = tau::string::LargeString<Char>;

    u8 LargeString[sizeof(LargeString_t)];
    Char StackString[16];

    [[nodiscard]]       LargeString_t& AsLargeString()       noexcept { return *reinterpret_cast<LargeString_t*>(LargeString); }
    [[nodiscard]] const LargeString_t& AsLargeString() const noexcept { return *reinterpret_cast<const LargeString_t*>(LargeString); }
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

    StringData(const Char* buffer, uSys codeUnits) noexcept;
    StringData(ReferenceCounter::Type* refCount, const Char* buffer, uSys codeUnits) noexcept;

    ~StringData() noexcept;

    StringData(const StringData& copy) noexcept;
    StringData(StringData&& move) noexcept;

    StringData& operator=(const StringData& copy) noexcept;
    StringData& operator=(StringData&& move) noexcept;
    
    template<uSys Len>
    StringData& operator=(const Char (&str)[Len]) noexcept;

    void Reset(const Char* buffer, uSys codeUnits) noexcept;
    void Reset(ReferenceCounter::Type* refCount, const Char* buffer, uSys codeUnits) noexcept;

    [[nodiscard]] const Char* String() const noexcept { return Length < 16 ? Buffer.StackString : Buffer.AsLargeString().Buffer; }
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
uSys findHashCode([[tau::nonnull]] const Char* str) noexcept;

template<typename Char>
uSys findHashCode(const Char* str, uSys len) noexcept;

template<typename Char>
uSys strLength(const Char* str) noexcept;

template<typename Char>
i32 strCompare(const Char* lhs, const Char* rhs) noexcept;

template<typename Char>
i32 strCompare(const Char* lhs, const Char* rhs, uSys length) noexcept;

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

namespace cexpr
{
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
    const Char* _string;
    uSys _length;
    uSys _index;
public:
    StringIteratorT(const Char* string, uSys length, uSys index) noexcept;

    StringIteratorT<Char>& operator++() noexcept;
    StringIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] StringIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] StringIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] inline Char operator*() const noexcept;

    [[nodiscard]] bool operator ==(const StringIteratorT<Char>& other) const noexcept;
    [[nodiscard]] bool operator !=(const StringIteratorT<Char>& other) const noexcept;
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
    DynStringCodeUnitIteratorT(const StringData& string, uSys index, uSys start = 0) noexcept;

    DynStringCodeUnitIteratorT<Char>& operator++() noexcept;
    DynStringCodeUnitIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] DynStringCodeUnitIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] DynStringCodeUnitIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] inline Char operator*() const noexcept;

    [[nodiscard]] bool operator ==(const DynStringCodeUnitIteratorT<Char>& other) const noexcept;
    [[nodiscard]] bool operator !=(const DynStringCodeUnitIteratorT<Char>& other) const noexcept;
};

template<typename Char>
class DynStringCodePointIteratorT final
{
    DEFAULT_DESTRUCT(DynStringCodePointIteratorT);
    DEFAULT_CM_PU(DynStringCodePointIteratorT);
private:
    using StringData = tau::string::StringData<Char>;

    StringData m_String;
    uSys m_Start;
    uSys m_Index;
    c32 m_CurrentCodePoint;
public:
    DynStringCodePointIteratorT(const StringData& string, uSys index, uSys start = 0) noexcept;
protected:
    DynStringCodePointIteratorT(const StringData& string, uSys index, uSys start, c32 currentCodePoint) noexcept;
public:
    DynStringCodePointIteratorT<Char>& operator++() noexcept;
    DynStringCodePointIteratorT<Char>& operator--() noexcept;

    [[nodiscard]] DynStringCodePointIteratorT<Char> operator++(int) noexcept;
    [[nodiscard]] DynStringCodePointIteratorT<Char> operator--(int) noexcept;

    [[nodiscard]] inline c32 operator*() const noexcept;

    [[nodiscard]] bool operator ==(const DynStringCodePointIteratorT<Char>& other) const noexcept;
    [[nodiscard]] bool operator !=(const DynStringCodePointIteratorT<Char>& other) const noexcept;
};

template<typename Char>
class ConstExprStringT final : public StringBaseT<Char>
{
    DEFAULT_DESTRUCT(ConstExprStringT);
    DEFAULT_CM_PU(ConstExprStringT);
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
    
    [[nodiscard]] StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, m_Length, 0); }
    [[nodiscard]] StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, m_Length, m_Length - 1); }

    template<uSys Len>
    [[nodiscard]] constexpr bool equals(const Char (&str)[Len]) const noexcept;
    
    [[nodiscard]] bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const Char* str) const noexcept override;
    
    [[nodiscard]] Char operator [](uSys index) const noexcept;
    [[nodiscard]] Char at(uSys index) const noexcept;
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

    DynStringT(const Char* string, uSys length) noexcept;
    DynStringT(ReferenceCounter::Type* refCount, const Char* string, uSys length) noexcept;
public:
    DynStringT() noexcept;
    DynStringT(const Char* string) noexcept;
    DynStringT(uSys length, const Char* string) noexcept;
    DynStringT(const ConstExprStringT<Char>& string) noexcept;
    DynStringT(const DynStringViewT<Char>& string) noexcept;
    DynStringT(const StringBaseT<Char>& string) noexcept;
    
    DynStringT<Char>& operator=(const Char* string) noexcept;
    DynStringT<Char>& operator=(const ConstExprStringT<Char>& string) noexcept;
    DynStringT<Char>& operator=(const DynStringViewT<Char>& string) noexcept;

    [[nodiscard]] [[tau::nonnull]] const Char* String() const noexcept override { return m_Data.String(); }
    [[nodiscard]] uSys Length() const noexcept override { return m_Data.Length; }
    [[nodiscard]] uSys HashCode() const noexcept override { return m_Hash; }
    
    [[nodiscard]] bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const Char* str) const noexcept override;

    [[nodiscard]] DynStringCodePointIteratorT<Char> Begin() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length); }
    [[nodiscard]] DynStringCodePointIteratorT<Char>   End() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length - 1); }

    [[nodiscard]] DynStringCodePointIteratorT<Char> begin() const noexcept { return begin(); }
    [[nodiscard]] DynStringCodePointIteratorT<Char>   end() const noexcept { return end(); }

    [[nodiscard]] DynStringCodeUnitIteratorT<Char> BeginUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, 0); }
    [[nodiscard]] DynStringCodeUnitIteratorT<Char>   EndUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Data.Length - 1); }

    [[nodiscard]] DynStringT<Char> Concat(const StringBaseT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const ConstExprStringT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const DynStringT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const DynStringViewT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const Char* other) const noexcept;

    [[nodiscard]] DynStringT<Char> operator +(const StringBaseT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const ConstExprStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const DynStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const DynStringViewT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const Char* const             other) const noexcept { return concat(other); }

    [[nodiscard]] DynStringT<Char> SubStringLen([[tau::inclusive]] uSys begin, uSys length) const noexcept;
    [[nodiscard]] DynStringT<Char> SubString([[tau::inclusive]] uSys begin, [[tau::exclusive]] uSys end) const noexcept;
    [[nodiscard]] DynStringT<Char> SubString([[tau::inclusive]] uSys from) const noexcept;
    
    [[nodiscard]] Char operator [](uSys index) const noexcept;
    [[nodiscard]] Char At(uSys index) const noexcept;
private:
    [[nodiscard]] DynStringT<Char> _concat(uSys len, const Char* str) const noexcept;
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
    DynStringViewT(uSys begin, uSys length, const DynStringT<Char>& str) noexcept;
    DynStringViewT(uSys begin, uSys length, const DynStringViewT<Char>& str) noexcept;
public:
    DynStringViewT(const DynStringT<Char>& str, uSys begin, uSys end) noexcept;
    DynStringViewT(const DynStringViewT<Char>& str, uSys begin, uSys end) noexcept;

    DynStringViewT(DynStringT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    DynStringViewT(DynStringViewT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    
    [[nodiscard]] static inline DynStringViewT<Char> Create(const DynStringT<Char>& str, uSys begin, uSys length) noexcept;
    [[nodiscard]] static inline DynStringViewT<Char> Create(const DynStringViewT<Char>& str, uSys begin, uSys length) noexcept;

    [[nodiscard]] static inline DynStringViewT<Char> Create(DynStringT<Char>&& str, uSys begin, uSys length) noexcept = delete;
    [[nodiscard]] static inline DynStringViewT<Char> Create(DynStringViewT<Char>&& str, uSys begin, uSys length) noexcept = delete;

    DynStringViewT<Char>& Reset(const DynStringT<Char>& str, uSys begin, uSys end) noexcept;
    DynStringViewT<Char>& ResetLen(const DynStringT<Char>& str, uSys begin, uSys length) noexcept;

    DynStringViewT<Char>& Reset(DynStringT<Char>&& str, uSys begin, uSys end) noexcept = delete;
    DynStringViewT<Char>& ResetLen(DynStringT<Char>&& str, uSys begin, uSys length) noexcept = delete;

    [[nodiscard]] const Char* String() const noexcept override { return m_Data.String() + m_Start; }
    [[nodiscard]] uSys Length() const noexcept override { return m_Length; }
    [[nodiscard]] uSys HashCode() const noexcept override { return m_Hash; }
    
    [[nodiscard]] DynStringCodePointIteratorT<Char> Begin() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length, m_Start); }
    [[nodiscard]] DynStringCodePointIteratorT<Char>   End() const noexcept { return DynStringCodePointIteratorT<Char>(m_Data.String(), m_Data.Length - 1, m_Start); }

    [[nodiscard]] DynStringCodePointIteratorT<Char> begin() const noexcept { return begin(); }
    [[nodiscard]] DynStringCodePointIteratorT<Char>   end() const noexcept { return end(); }

    [[nodiscard]] DynStringCodeUnitIteratorT<Char> BeginUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Start, m_Start); }
    [[nodiscard]] DynStringCodeUnitIteratorT<Char>   EndUnits() const noexcept { return DynStringCodeUnitIteratorT<Char>(m_Data, m_Data.Length - 1, m_Start); }

    [[nodiscard]] bool Equals(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] bool Equals(const Char* str) const noexcept override;

    [[nodiscard]] i32 CompareTo(const StringBaseT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const ConstExprStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringViewT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const DynStringT<Char>& other) const noexcept override;
    [[nodiscard]] i32 CompareTo(const Char* str) const noexcept override;

    [[nodiscard]] DynStringT<Char> Concat(const StringBaseT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const ConstExprStringT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const DynStringT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const DynStringViewT<Char>& other) const noexcept;
    [[nodiscard]] DynStringT<Char> Concat(const Char* other) const noexcept;

    [[nodiscard]] DynStringT<Char> operator +(const StringBaseT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const ConstExprStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const DynStringT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const DynStringViewT<Char>& other) const noexcept { return concat(other); }
    [[nodiscard]] DynStringT<Char> operator +(const Char* const             other) const noexcept { return concat(other); }

    [[nodiscard]] inline DynStringT<Char> SubStringLen([[tau::inclusive]] uSys begin, uSys length) const noexcept;
    [[nodiscard]] inline DynStringT<Char> SubString([[tau::inclusive]] uSys begin, [[tau::exclusive]] uSys end) const noexcept;
    [[nodiscard]] inline DynStringT<Char> SubString([[tau::inclusive]] uSys from) const noexcept;
    
    [[nodiscard]] inline Char operator [](uSys index) const noexcept;
    [[nodiscard]] inline Char At(uSys index) const noexcept;
private:
    [[nodiscard]] DynStringT<Char> _concat(uSys len, const Char* str) noexcept;
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
    StringBuilderT() noexcept;
    StringBuilderT(uSys initialSize) noexcept;
    StringBuilderT(const Char* string) noexcept;

    ~StringBuilderT() noexcept;

    StringBuilderT(const StringBuilderT<Char>& copy) noexcept;
    StringBuilderT(StringBuilderT<Char>&& move) noexcept;

    StringBuilderT<Char>& operator =(const StringBuilderT<Char>& copy) noexcept;
    StringBuilderT<Char>& operator =(StringBuilderT<Char>&& move) noexcept;

    [[nodiscard]] const Char* String() const noexcept { return m_String; }
    [[nodiscard]] const Char* c_str() const noexcept { return m_String; }
    [[nodiscard]] uSys Length() const noexcept { return m_Length; }
    [[nodiscard]] uSys length() const noexcept { return m_Length; }
    [[nodiscard]] uSys Size() const noexcept { return m_Size; }
    [[nodiscard]] uSys size() const noexcept { return m_Size; }

    [[nodiscard]] bool equals(const StringBuilderT<Char>& other) const noexcept;

    [[nodiscard]] i32 compareTo(const StringBuilderT<Char>& other) const noexcept;

    [[nodiscard]] StringIteratorT<Char> begin() const noexcept { return StringIteratorT<Char>(m_String, m_Length, 0); }
    [[nodiscard]] StringIteratorT<Char>   end() const noexcept { return StringIteratorT<Char>(m_String, m_Length, m_Length - 1); }

    inline StringBuilderT<Char>& append(const StringBaseT<Char>& string) noexcept;
    inline StringBuilderT<Char>& append(const ConstExprStringT<Char>& string) noexcept;
    inline StringBuilderT<Char>& append(const DynStringT<Char>& string) noexcept;
    inline StringBuilderT<Char>& append(const DynStringViewT<Char>& string) noexcept;
    inline StringBuilderT<Char>& append(const StringBuilderT<Char>& string) noexcept;
    inline StringBuilderT<Char>& append(const Char* string) noexcept;
    template<typename CharDeduced = Char>
    inline StringBuilderT<Char>& append(::std::enable_if_t<!::std::is_same_v<CharDeduced, c32>> c) noexcept;
    inline StringBuilderT<Char>& append(c32 c) noexcept;

    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const StringBaseT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const ConstExprStringT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const DynStringT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const DynStringViewT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const StringBuilderT<CharFrom>& string) noexcept;
    template<typename CharFrom>
    inline StringBuilderT<Char>& append(const CharFrom* string) noexcept;

    inline StringBuilderT<Char>& reset() noexcept;
    inline StringBuilderT<Char>& reset(uSys newSize) noexcept;
    inline StringBuilderT<Char>& resetIf(uSys maxSize, uSys newSize) noexcept;

    inline StringBuilderT<Char>& backspace() noexcept;
    inline StringBuilderT<Char>& backspace(uSys count) noexcept;

#if __has_feature(__cpp_impl_three_way_comparison)
    [[nodiscard]] i32 operator <=> (const StringBuilderT<Char>& other) const noexcept { return compareTo(other); }
#endif

    [[nodiscard]] inline bool operator ==(const StringBuilderT<Char>& other) const noexcept { return  equals(other); }
    [[nodiscard]] inline bool operator !=(const StringBuilderT<Char>& other) const noexcept { return !equals(other); }
    [[nodiscard]] inline bool operator < (const StringBuilderT<Char>& other) const noexcept { return compareTo(other) <  0; }
    [[nodiscard]] inline bool operator > (const StringBuilderT<Char>& other) const noexcept { return compareTo(other) >  0; }
    [[nodiscard]] inline bool operator <=(const StringBuilderT<Char>& other) const noexcept { return compareTo(other) <= 0; }
    [[nodiscard]] inline bool operator >=(const StringBuilderT<Char>& other) const noexcept { return compareTo(other) >= 0; }

    [[nodiscard]] inline StringBuilderT<Char>& operator +(const ConstExprStringT<Char>& other) noexcept { return append(other); }
    [[nodiscard]] inline StringBuilderT<Char>& operator +(const DynStringT<Char>& other)       noexcept { return append(other); }
    [[nodiscard]] inline StringBuilderT<Char>& operator +(const DynStringViewT<Char>& other)   noexcept { return append(other); }
    [[nodiscard]] inline StringBuilderT<Char>& operator +(const StringBuilderT<Char>& other)   noexcept { return append(other); }
    [[nodiscard]] inline StringBuilderT<Char>& operator +(const Char* other)                   noexcept { return append(other); }

    inline StringBuilderT<Char>& operator +=(const ConstExprStringT<Char>& other) noexcept { return append(other); }
    inline StringBuilderT<Char>& operator +=(const DynStringT<Char>& other)       noexcept { return append(other); }
    inline StringBuilderT<Char>& operator +=(const DynStringViewT<Char>& other)   noexcept { return append(other); }
    inline StringBuilderT<Char>& operator +=(const StringBuilderT<Char>& other)   noexcept { return append(other); }
    inline StringBuilderT<Char>& operator +=(const Char* other)                   noexcept { return append(other); }

    [[nodiscard]] inline DynStringT<Char> toString() const noexcept { return DynStringT<Char>(m_String); }

    [[nodiscard]] inline Char operator[](uSys index) const noexcept;
    [[nodiscard]] inline Char at(uSys index) const noexcept;
private:
    void AssertSize(uSys newLength) noexcept;

    void Append(const Char* string, uSys length) noexcept;

    template<typename CharFrom>
    void Append(const CharFrom* string, uSys length) noexcept;

    void append(const Char* const string, const uSys length) noexcept { Append(string, length); }
    template<typename CharFrom>
    void append(const CharFrom* const string, const uSys length) noexcept { Append(string, length); }
private:
    Char* m_String;
    uSys m_Length;
    uSys m_Size;
};

template<typename CharTo, typename CharFrom>
DynStringT<CharTo> StringCast(const DynStringT<CharFrom>& string) noexcept;

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
#include "String.inl"
#include "String.UnicodeIterator.inl"
#include "String.Cast.inl"
#include "String.Unicode.inl"
