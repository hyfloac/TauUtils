#pragma once

#include <map>
#include <unordered_map>

#include "Objects.hpp"

template<typename Key, typename Value, class Pr = ::std::less<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class MapKeyIterator : public ::std::map<Key, Value, Pr, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(MapKeyIterator);
    DEFAULT_DESTRUCT(MapKeyIterator);
    DEFAULT_CM_PU(MapKeyIterator);
public:
    using MapIterator = typename ::std::map<Key, Value, Pr, Alloc>::iterator;
public:
    MapKeyIterator(const MapIterator& iterator) noexcept
        : MapIterator(iterator)
    { }

    [[nodiscard]]       Key* operator->()       noexcept { return &(MapIterator::operator->()->first); }
    [[nodiscard]] const Key& operator *() const noexcept { return MapIterator::operator*().first; }
};

template<typename Key, typename Value, class Pr = ::std::less<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class MapConstKeyIterator : public ::std::map<Key, Value, Pr, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(MapConstKeyIterator);
    DEFAULT_DESTRUCT(MapConstKeyIterator);
    DEFAULT_CM_PU(MapConstKeyIterator);
public:
    using MapIterator = typename ::std::map<Key, Value, Pr, Alloc>::iterator;
public:
    MapConstKeyIterator(const MapIterator& iterator) noexcept
        : MapIterator(iterator)
    { }

    [[nodiscard]] const Key* operator->() const noexcept { return &(MapIterator::operator->()->first); }
    [[nodiscard]] const Key& operator *() const noexcept { return MapIterator::operator*().first; }
};

template<typename Key, typename Value, class Hasher = ::std::hash<Key>, class KeyEq = ::std::equal_to<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class HashMapKeyIterator : public ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(HashMapKeyIterator);
    DEFAULT_DESTRUCT(HashMapKeyIterator);
    DEFAULT_CM_PU(HashMapKeyIterator);
public:
    using HashMapIterator = typename ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator;
public:
    HashMapKeyIterator(const HashMapIterator& iterator) noexcept
        : HashMapIterator(iterator)
    { }

    [[nodiscard]]       Key* operator->()       noexcept { return &(HashMapIterator::operator->()->first); }
    [[nodiscard]] const Key& operator *() const noexcept { return HashMapIterator::operator*().first; }
};

template<typename Key, typename Value, class Hasher = ::std::hash<Key>, class KeyEq = ::std::equal_to<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class HashMapConstKeyIterator : public ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(HashMapConstKeyIterator);
    DEFAULT_DESTRUCT(HashMapConstKeyIterator);
    DEFAULT_CM_PU(HashMapConstKeyIterator);
public:
    using HashMapIterator = typename ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator;
public:
    HashMapConstKeyIterator(const HashMapIterator& iterator) noexcept
        : HashMapIterator(iterator)
    { }

    [[nodiscard]] const Key* operator->() const noexcept { return &(HashMapIterator::operator->()->first); }
    [[nodiscard]] const Key& operator *() const noexcept { return HashMapIterator::operator*().first; }
};

template<typename Key, typename Value, class Pr = ::std::less<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class MapValueIterator : public ::std::map<Key, Value, Pr, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(MapValueIterator);
    DEFAULT_DESTRUCT(MapValueIterator);
    DEFAULT_CM_PU(MapValueIterator);
public:
    using MapIterator = typename ::std::map<Key, Value, Pr, Alloc>::iterator;
public:
    MapValueIterator(const MapIterator& iterator) noexcept
        : MapIterator(iterator)
    { }

    [[nodiscard]]       Value* operator->()       noexcept { return &(MapIterator::operator->()->second); }
    [[nodiscard]] const Value& operator *() const noexcept { return MapIterator::operator*().second; }
};

template<typename Key, typename Value, class Pr = ::std::less<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class MapConstValueIterator : public ::std::map<Key, Value, Pr, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(MapConstValueIterator);
    DEFAULT_DESTRUCT(MapConstValueIterator);
    DEFAULT_CM_PU(MapConstValueIterator);
public:
    using MapIterator = typename ::std::map<Key, Value, Pr, Alloc>::iterator;
public:
    MapConstValueIterator(const MapIterator& iterator) noexcept
        : MapIterator(iterator)
    { }

    [[nodiscard]] const Value* operator->() const noexcept { return &(MapIterator::operator->()->second); }
    [[nodiscard]] const Value& operator *() const noexcept { return MapIterator::operator*().second; }
};

template<typename Key, typename Value, class Hasher = ::std::hash<Key>, class KeyEq = ::std::equal_to<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class HashMapValueIterator : public ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(HashMapValueIterator);
    DEFAULT_DESTRUCT(HashMapValueIterator);
    DEFAULT_CM_PU(HashMapValueIterator);
public:
    using HashMapIterator = typename ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator;
public:
    HashMapValueIterator(const HashMapIterator& iterator) noexcept
        : HashMapIterator(iterator)
    { }

    [[nodiscard]]       Value* operator->()       noexcept { return &(HashMapIterator::operator->()->second); }
    [[nodiscard]] const Value& operator *() const noexcept { return HashMapIterator::operator*().second; }
};

template<typename Key, typename Value, class Hasher = ::std::hash<Key>, class KeyEq = ::std::equal_to<Key>, class Alloc = ::std::allocator<::std::pair<const Key, Value>>>
class HashMapConstValueIterator : public ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator
{
    DEFAULT_CONSTRUCT_PU(HashMapConstValueIterator);
    DEFAULT_DESTRUCT(HashMapConstValueIterator);
    DEFAULT_CM_PU(HashMapConstValueIterator);
public:
    using HashMapIterator = typename ::std::unordered_map<Key, Value, Hasher, KeyEq, Alloc>::iterator;
public:
    HashMapConstValueIterator(const HashMapIterator& iterator) noexcept
        : HashMapIterator(iterator)
    { }

    [[nodiscard]] const Value* operator->() const noexcept { return &(HashMapIterator::operator->()->second); }
    [[nodiscard]] const Value& operator *() const noexcept { return HashMapIterator::operator*().second; }
};

template<typename Iter>
struct IteratorBundle final
{
    DEFAULT_CONSTRUCT_PU(IteratorBundle);
    DEFAULT_DESTRUCT(IteratorBundle);
    DEFAULT_CM_PU(IteratorBundle);
public:
    Iter m_Begin;
    Iter m_End;
public:
    IteratorBundle(const Iter& begin, const Iter& end) noexcept
        : m_Begin(begin)
        , m_End(end)
    { }

    [[nodiscard]] Iter begin() const noexcept { return m_Begin; }
    [[nodiscard]] Iter   end() const noexcept { return m_End;   }
};
