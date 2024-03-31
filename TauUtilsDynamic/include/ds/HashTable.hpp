#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "allocator/TauAllocator.hpp"
#include <functional>
#include <unordered_map>
#include <type_traits>
#include "ConPrinter.hpp"

namespace tau {

template<typename KeyT, typename ValueT>
struct HashBucket final
{
    DEFAULT_DESTRUCT(HashBucket);
    DEFAULT_CM_PU(HashBucket);

    static constexpr uSys TombstoneValue = 0x12AB34CD12AB34CD_uz;

    uSys HashCode;
    u8 KeyBuffer[sizeof(KeyT)];
    u8 ValueBuffer[sizeof(ValueT)];

    HashBucket() noexcept
        : HashCode(TombstoneValue)
        , KeyBuffer { }
        , ValueBuffer { }
    { }

    template<typename... Args>
    HashBucket(const KeyT& key, Args&& ...args) noexcept
        : HashCode(0)
        , KeyBuffer { }
        , ValueBuffer { }
    {

        ::std::construct_at(KeyPtr(), key);
        ::std::construct_at(ValuePtr(), ::std::forward<Args>(args)...);
        HashCode = ::std::hash<KeyT> {}(key);
    }

    template<typename... Args>
    HashBucket(KeyT&& key, Args&& ...args) noexcept
        : HashCode(0)
        , KeyBuffer { }
        , ValueBuffer { }
    {

        ::std::construct_at(KeyPtr(), ::std::move(key));
        ::std::construct_at(ValuePtr(), ::std::forward<Args>(args)...);
        HashCode = ::std::hash<KeyT> {}(key);
    }

    constexpr       KeyT* KeyPtr()       noexcept { return reinterpret_cast<      KeyT*>(KeyBuffer); }
    constexpr const KeyT* KeyPtr() const noexcept { return reinterpret_cast<const KeyT*>(KeyBuffer); }

    constexpr       ValueT* ValuePtr()       noexcept { return reinterpret_cast<      ValueT*>(ValueBuffer); }
    constexpr const ValueT* ValuePtr() const noexcept { return reinterpret_cast<const ValueT*>(ValueBuffer); }
};

template<typename KeyT, typename ValueT>
class HashTable final
{
private:
    static constexpr uSys BucketSizes[] = {
              13,      17,      29, 43, 71,
             139,     353,     733,
            1583,    3343,    7177,
           15331,   40009,   86349,
          174569,  363977,  730003,
         1500341, 3000709, 6001427,
        12002869,
    };

    using Bucket_t = HashBucket<KeyT, ValueT>;
    using BucketArr_t = Bucket_t*;
    // using BucketArr_t = Bucket_t**;
    static constexpr bool IsBucketPointer = ::std::is_pointer_v<::std::remove_pointer_t<BucketArr_t>>;
private:
    TauAllocator* m_ArrayAllocator;
    TauAllocator* m_NodeAllocator;
    uSys m_BucketCountIndex;
    uSys m_BucketCount;
    uSys m_ActiveElementCount;
    BucketArr_t m_Buckets;
public:
    HashTable(
        const uSys minCount = 1,
        TauAllocator& arrayAllocator = DefaultTauAllocator::Instance(), 
        TauAllocator& nodeAllocator = DefaultTauAllocator::Instance()
    ) noexcept
        : m_ArrayAllocator(&arrayAllocator)
        , m_NodeAllocator(&nodeAllocator)
        , m_BucketCountIndex(FindNextBucketCount(minCount))
        , m_BucketCount(BucketSizes[m_BucketCountIndex])
        , m_ActiveElementCount(0)
        , m_Buckets(reinterpret_cast<decltype(m_Buckets)>(arrayAllocator.Allocate(sizeof(*m_Buckets) * m_BucketCount)))
    {
        if constexpr(IsBucketPointer)
        {
            (void) ::std::fill_n(m_Buckets, m_BucketCount, nullptr);
        }
        else
        {
            (void) ::std::fill_n(m_Buckets, m_BucketCount, Bucket_t());
        }
    }

    void Insert(const KeyT& key, const ValueT& value) noexcept
    {
        const uSys hashIndex = PreInsert(key);

        if constexpr(IsBucketPointer)
        {
            m_Buckets[hashIndex] = m_NodeAllocator->AllocateT<Bucket_t>(key, value);
        }
        else
        {
            ::std::construct_at(&m_Buckets[hashIndex], key, value);
        }
    }

    void Insert(const KeyT& key, ValueT&& value) noexcept
    {
        const uSys hashIndex = PreInsert(key);

        if constexpr(IsBucketPointer)
        {
            m_Buckets[hashIndex] = m_NodeAllocator->AllocateT<Bucket_t>(key, ::std::move(value));
        }
        else
        {
            ::std::construct_at(&m_Buckets[hashIndex], key, ::std::move(value));
        }
    }

    [[nodiscard]] bool Contains(const KeyT& key) const noexcept
    {
        const uSys hashCode = ::std::hash<KeyT>::operator()(key);

        uSys hashIndex = hashCode % m_BucketCount;

        for(uSys quadraticIndex = 1; true; ++quadraticIndex)
        {
            if(IsTombstone(m_Buckets[hashIndex]))
            {
                return false;
            }

            if(DeReference(m_Buckets[hashIndex]).HashCode == hashCode)
            {
                if((*const_cast<const KeyT*>(DeReference(m_Buckets[hashIndex]).KeyPtr())) == key)
                {
                    return true;
                }
            }

            hashIndex = (hashCode + quadraticIndex * quadraticIndex) % m_BucketCount;
        }
    }

    [[nodiscard]] ValueT* Get(const KeyT& key) noexcept
    {
        const uSys hashCode = ::std::hash<KeyT> {}(key);

        uSys hashIndex = hashCode % m_BucketCount;

        for(uSys quadraticIndex = 1; true; ++quadraticIndex)
        {
            if(IsTombstone(m_Buckets[hashIndex]))
            {
                return nullptr;
            }

            if(DeReference(m_Buckets[hashIndex]).HashCode == hashCode)
            {
                if((*const_cast<const KeyT*>(DeReference(m_Buckets[hashIndex]).KeyPtr())) == key)
                {
                    return DeReference(m_Buckets[hashIndex]).ValuePtr();
                }
            }

            hashIndex = (hashCode + quadraticIndex * quadraticIndex) % m_BucketCount;
        }
    }

    [[nodiscard]] const ValueT* Get(const KeyT& key) const noexcept
    {
        return const_cast<HashTable<KeyT, ValueT>*>(this)->Get(key);
    }
private:
    [[nodiscard]] uSys PreInsert(const KeyT& key) noexcept
    {
        ++m_ActiveElementCount;

        if(m_ActiveElementCount >= (m_BucketCount >> 1) + (m_BucketCount >> 2))
        {
            Expand();
        }

        return KeyFunc(key, m_Buckets, m_BucketCount);

    }

    [[nodiscard]] static uSys KeyFunc(const uSys hashCode, const decltype(m_Buckets) buckets, const uSys bucketCount) noexcept
    {
        uSys hashIndex = hashCode % bucketCount;

        for(uSys quadraticIndex = 1; !IsTombstone(buckets[hashIndex]); ++quadraticIndex)
        {
            hashIndex = (hashCode + quadraticIndex * quadraticIndex) % bucketCount;
        }

        return hashIndex;
    }

    [[nodiscard]] static uSys KeyFunc(const KeyT& key, const BucketArr_t buckets, const uSys bucketCount) noexcept
    {
        return KeyFunc(::std::hash<KeyT>{}(key), buckets, bucketCount);
    }

    void Expand() noexcept
    {
        ++m_BucketCountIndex;

        if(m_BucketCountIndex >= ::std::size(BucketSizes))
        {
            ::std::exit(static_cast<int>(Bucket_t::TombstoneValue));
        }

        const uSys newBucketCount = BucketSizes[m_BucketCountIndex];

        auto newBuckets = reinterpret_cast<BucketArr_t>(m_ArrayAllocator->Allocate(sizeof(*m_Buckets) * newBucketCount));

        if constexpr(IsBucketPointer)
        {
            (void) ::std::fill_n(newBuckets, newBucketCount, nullptr);
        }
        else
        {
            (void) ::std::fill_n(newBuckets, newBucketCount, Bucket_t());
        }

        for(uSys i = 0; i < m_BucketCount; ++i)
        {
            if(!IsTombstone(m_Buckets[i]))
            {
                const uSys hashIndex = KeyFunc(DeReference(m_Buckets[i]).HashCode, newBuckets, newBucketCount);
                newBuckets[hashIndex] = m_Buckets[i];
            }
        }

        m_BucketCount = newBucketCount;
        m_ArrayAllocator->Deallocate(m_Buckets);
        m_Buckets = newBuckets;
    }
private:
    [[nodiscard]] static constexpr uSys FindNextBucketCount(const uSys minElementCount) noexcept
    {
        constexpr uSys BucketSizeCount = ::std::size(BucketSizes);
        uSys currentIndex = BucketSizeCount >> 1;
        uSys minBound = 0;
        uSys maxBound = BucketSizeCount - 1;

        do
        {
            if(minElementCount == BucketSizes[currentIndex])
            {
                return currentIndex;
            }

            if(minBound == maxBound)
            {
                if(currentIndex >= BucketSizeCount)
                {
                    return static_cast<uSys>(-1);
                }

                return currentIndex;
            }

            if(minElementCount > BucketSizes[currentIndex])
            {
                minBound = currentIndex;
            }
            else if(minElementCount < BucketSizes[currentIndex])
            {
                maxBound = currentIndex;
            }

            const uSys prevCurrentIndex = currentIndex;
            currentIndex = ((maxBound - minBound) >> 1) + minBound;
            if(prevCurrentIndex == currentIndex)
            {
                return currentIndex + 1;
            }
        } while(true);

    }

    [[nodiscard]] static Bucket_t& DeReference(::std::remove_pointer_t<BucketArr_t>& bucket) noexcept
    {
        if constexpr(IsBucketPointer)
        {
            return *bucket;
        }
        else
        {
            return bucket;
        }
    }

    [[nodiscard]] static const Bucket_t& DeReference(const ::std::remove_pointer_t<BucketArr_t>& bucket) noexcept
    {
        if constexpr(IsBucketPointer)
        {
            return *bucket;
        }
        else
        {
            return bucket;
        }
    }

    [[nodiscard]] static bool IsTombstone(const ::std::remove_pointer_t<BucketArr_t>& bucket) noexcept
    {
        if constexpr(IsBucketPointer)
        {
            return !bucket;
        }
        else
        {
            return bucket.HashCode == Bucket_t::TombstoneValue;
        }
    }
};

}
