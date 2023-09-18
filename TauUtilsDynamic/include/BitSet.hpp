#pragma once

#include <climits>
#include "DynArray.hpp"

class BitSet final
{
public:
    class BitRef final
    {
        DEFAULT_DESTRUCT(BitRef);
    private:
        uSys& m_Word;
        uSys m_BitMask;
    public:
        BitRef(uSys& word, const uSys bitMask) noexcept
            : m_Word(word)
            , m_BitMask(bitMask)
        { }

        BitRef(const BitRef& copy) noexcept = default;
        BitRef(BitRef&& move) noexcept = default;

        BitRef& operator=(const BitRef& copy) noexcept
        {
            this->operator=(static_cast<bool>(copy));

            return *this;
        }

        BitRef& operator=(BitRef&& move) noexcept
        {
            this->operator=(static_cast<bool>(move));

            return *this;
        }

        [[nodiscard]] operator bool() const noexcept
        { return m_Word & m_BitMask; }

        BitRef& operator=(const bool val) noexcept
        {
            if(val)
            { m_Word |= m_BitMask; }
            else
            { m_Word &= ~m_BitMask; }

            return *this;
        }
    };
public:
    static constexpr uSys BIT_COUNT = sizeof(uSys) * CHAR_BIT;
public:
    BitSet(const uSys bitCount, const bool initialValue = false) noexcept
        : m_BitCount(bitCount)
        , m_Bits((bitCount / BIT_COUNT) + 1)
    { ::std::memset(m_Bits.arr(), initialValue ? 0xFF : 0x00, m_Bits.count() * sizeof(uSys)); }

    [[nodiscard]] uSys BitCount() const noexcept { return m_BitCount; }
    [[nodiscard]] uSys bitCount() const noexcept { return BitCount(); }

    [[nodiscard]] bool operator[](const uSys index) const noexcept
    {
        const uSys wordIndex = index / BIT_COUNT;
        const uSys bitIndex = index % BIT_COUNT;
        // const uSys invBitIndex = BIT_COUNT - bitIndex;

        const uSys word = m_Bits[wordIndex];
        const bool ret = word & (static_cast<uSys>(1) << bitIndex);

        return ret;
    }

    [[nodiscard]] BitRef operator[](const uSys index) noexcept
    {
        const uSys wordIndex = index / BIT_COUNT;
        const uSys bitIndex = index % BIT_COUNT;
        // const uSys invBitIndex = BIT_COUNT - bitIndex;

        return BitRef(m_Bits[wordIndex], static_cast<uSys>(1) << bitIndex);
    }

    [[nodiscard]] bool At(const uSys index) const noexcept
    {
        const uSys wordIndex = index / BIT_COUNT;
        const uSys bitIndex = index % BIT_COUNT;
        // const uSys invBitIndex = BIT_COUNT - bitIndex;

        if(wordIndex >= m_Bits.count())
        { return false; }

        const uSys word = m_Bits[wordIndex];
        const bool ret = word & (static_cast<uSys>(1) << bitIndex);

        return ret;
    }

    void Set(const uSys index, bool value = true) noexcept
    {
        const uSys wordIndex = index / BIT_COUNT;
        const uSys bitIndex = index % BIT_COUNT;
        // const uSys invBitIndex = BIT_COUNT - bitIndex;
        
        if(wordIndex >= m_Bits.count())
        { return; }
        
        if(value)
        { m_Bits[wordIndex] |= static_cast<uSys>(1) << bitIndex; }
        else
        { m_Bits[wordIndex] &= ~(static_cast<uSys>(1) << bitIndex); }
    }

    void Unset(const uSys index) noexcept
    { set(index, false); }

    void Flip(const uSys index) noexcept
    {
        const uSys wordIndex = index / BIT_COUNT;
        const uSys bitIndex = index % BIT_COUNT;
        // const uSys invBitIndex = BIT_COUNT - bitIndex;
        
        if(wordIndex >= m_Bits.count())
        { return; }
        
        m_Bits[wordIndex] ^= static_cast<uSys>(1) << bitIndex;
    }
    
    [[nodiscard]] bool at(const uSys index) const noexcept { return At(index); }
    void set(const uSys index, bool value = true) noexcept { Set(index, value); }
    void unset(const uSys index) noexcept { Unset(index); }
    void flip(const uSys index) noexcept { Flip(index); }
private:
    uSys m_BitCount;
    DynArray<uSys> m_Bits;
};
