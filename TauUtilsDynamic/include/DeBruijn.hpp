#pragma once

#include "NumTypes.hpp"
#include "BitSet.hpp"

namespace tau {

inline void compute_db(BitSet& sequence, uSys& writeIndex, BitSet& a, const uSys n, const uSys t, const uSys p) noexcept
{
    if(t > n)
    {
        if(n % p == 0)
        {
            for(uSys i = 1; i < p + 1; ++i)
            {
                sequence[writeIndex] = a[i];
                ++writeIndex;
            }
        }
    }
    else
    {
        a[t] = a[t - p];
        compute_db(sequence, writeIndex, a, n, t + 1, p);
        uSys j = a[t - p] + 1;

        for(; j < 2; ++j)
        {
            a[t] = static_cast<bool>(j);
            compute_db(sequence, writeIndex, a, n, t + 1, t);
        }
    }
}

template<typename T>
inline T ComputeBinaryDeBruijnSequence(const uSys n) noexcept
{
    const uSys elementCount = 1 << n;

    BitSet sequence(elementCount, false);

    {
        BitSet a(2 * n, false);
        uSys writeIndex = 0;
        compute_db(sequence, writeIndex, a, n, 1, 1);
    }

    T mask = 0;
    for(uSys i = 0; i < elementCount; ++i)
    {
        mask <<= 1;
        mask |= sequence[i];
    }
    
    return mask;
}

template<typename T>
inline DynArray<T> GenerateDeBruijnTable(T mask, const uSys count, const uSys shift) noexcept
{
    DynArray<T> ret(count);

    for(T i = 0; i < count; ++i)
    {
        ret[mask >> shift] = i;
        mask <<= 1;
    }

    return ret;
}

}
