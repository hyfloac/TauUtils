#pragma once

#include "../Objects.hpp"
#include "../NumTypes.hpp"
#include "../ReferenceCountingPointer.hpp"
#include "../String.hpp"

#include <vector>
#include <unordered_map>

#include "Safeties.hpp"

namespace tau::json {

enum class EJType
{
    String,
    Integer,
    Real,
    Object,
    Array,
    Bool,
    Null
};

class JValue;
class JString;
class JInteger;
class JReal;
class JObject;
class JArray;
class JBool;
class JNull;

class JValueWrapper
{
    DEFAULT_DESTRUCT(JValueWrapper);
    DEFAULT_CM_PU(JValueWrapper);
protected:
    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JString>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(asFunc)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JInteger>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(asFunc)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JReal>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(asFunc)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JObject>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(asFunc)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JArray>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(asFunc)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JBool>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(asFunc)
        , m_AsNull(DefaultAsNull)
    { }

    JValueWrapper(const StrongRef<JValue>& value, StrongRef<JNull>(*asFunc)(const StrongRef<JValue>&)) noexcept
        : m_Value(RefCast<JValue>(value))
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(asFunc)
    { }
public:
    JValueWrapper() noexcept
        : m_Value(nullptr)
        , m_AsString(DefaultAsString)
        , m_AsInteger(DefaultAsInteger)
        , m_AsReal(DefaultAsReal)
        , m_AsObject(DefaultAsObject)
        , m_AsArray(DefaultAsArray)
        , m_AsBool(DefaultAsBool)
        , m_AsNull(DefaultAsNull)
    { }

    [[nodiscard]] EJType Type() const noexcept;

    [[nodiscard]] StrongRef<JString>   AsString() const noexcept { return m_AsString(m_Value); }
    [[nodiscard]] StrongRef<JInteger> AsInteger() const noexcept { return m_AsInteger(m_Value); }
    [[nodiscard]] StrongRef<JReal>       AsReal() const noexcept { return m_AsReal(m_Value); }
    [[nodiscard]] StrongRef<JObject>   AsObject() const noexcept { return m_AsObject(m_Value); }
    [[nodiscard]] StrongRef<JArray>     AsArray() const noexcept { return m_AsArray(m_Value); }
    [[nodiscard]] StrongRef<JBool>       AsBool() const noexcept { return m_AsBool(m_Value); }
    [[nodiscard]] StrongRef<JNull>       AsNull() const noexcept { return m_AsNull(m_Value); }
protected:
    [[nodiscard]] static StrongRef<JString>   DefaultAsString(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JInteger> DefaultAsInteger(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JReal>       DefaultAsReal(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JObject>   DefaultAsObject(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JArray>     DefaultAsArray(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JBool>       DefaultAsBool(const StrongRef<JValue>&) noexcept { return nullptr; }
    [[nodiscard]] static StrongRef<JNull>       DefaultAsNull(const StrongRef<JValue>&) noexcept { return nullptr; }
protected:
    StrongRef<JValue> m_Value;
    StrongRef<JString>(*m_AsString)(const StrongRef<JValue>&);
    StrongRef<JInteger>(*m_AsInteger)(const StrongRef<JValue>&);
    StrongRef<JReal>(*m_AsReal)(const StrongRef<JValue>&);
    StrongRef<JObject>(*m_AsObject)(const StrongRef<JValue>&);
    StrongRef<JArray>(*m_AsArray)(const StrongRef<JValue>&);
    StrongRef<JBool>(*m_AsBool)(const StrongRef<JValue>&);
    StrongRef<JNull>(*m_AsNull)(const StrongRef<JValue>&);
};

class JValue
{
    DEFAULT_DESTRUCT_VI(JValue);
    DEFAULT_CM_PO(JValue);
protected:
    JValue(const EJType type, const WeakRef<JValue>& parent) noexcept
        : m_Type(type)
        , m_Parent(parent)
    { }
public:
    [[nodiscard]] EJType Type() const noexcept { return m_Type; }
    [[nodiscard]] JValueWrapper Parent() const noexcept
    {
        StrongReferenceCountingPointer<JValue> holder;
        holder = m_Parent;

        return m_Parent ? m_Parent->InternalGetWrapper(holder) : JValueWrapper();
    }

    [[nodiscard]] virtual JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept { return {}; }

    [[nodiscard]] virtual StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept { return nullptr; }
private:
    EJType m_Type;
    WeakRef<JValue> m_Parent;
};

inline EJType JValueWrapper::Type() const noexcept
{
    return m_Value ? m_Value->Type() : EJType::Null;
}

class JString final : public JValue
{
    DEFAULT_DESTRUCT(JString);
    DEFAULT_CM_PU(JString);
public:
    template<typename Char>
    JString(const DynStringT<Char>& string, const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::String, parent)
        , m_Value(StringCast<c8>(string))
    { }

    [[nodiscard]] const C8DynString& String() const noexcept { return m_Value; }
    [[nodiscard]] operator const C8DynString&() const noexcept { return m_Value; }
    [[nodiscard]] const C8DynString& operator()() const noexcept { return m_Value; }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        return StrongRef<JString>(m_Value, parent);
    }
private:
    C8DynString m_Value;
};

class JInteger final : public JValue
{
    DEFAULT_DESTRUCT(JInteger);
    DEFAULT_CM_PU(JInteger);
public:
    JInteger(const u64 value, const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Integer, parent)
        , m_Value(value)
    { }

    JInteger(const i64 value, const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Integer, parent)
        , m_Value(static_cast<u64>(value))
    { }

    [[nodiscard]] u64 U64() const noexcept { return m_Value; }
    [[nodiscard]] u64 U32() const noexcept { return static_cast<u32>(m_Value); }
    [[nodiscard]] u64 U16() const noexcept { return static_cast<u16>(m_Value); }
    [[nodiscard]] u64 U8()  const noexcept { return static_cast<u8>(m_Value); }

    [[nodiscard]] u64 I64() const noexcept { return static_cast<i64>(m_Value); }
    [[nodiscard]] u64 I32() const noexcept { return static_cast<i32>(m_Value); }
    [[nodiscard]] u64 I16() const noexcept { return static_cast<i16>(m_Value); }
    [[nodiscard]] u64 I8()  const noexcept { return static_cast<i8>(m_Value);  }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        return StrongRef<JInteger>(m_Value, parent);
    }
private:
    u64 m_Value;
};

class JReal final : public JValue
{
    DEFAULT_DESTRUCT(JReal);
    DEFAULT_CM_PU(JReal);
public:
    JReal(const double value, const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Real, parent)
        , m_Value(value)
    { }

    [[nodiscard]] double Double() const noexcept { return m_Value; }
    [[nodiscard]] operator double() const noexcept { return m_Value; }
    [[nodiscard]] double operator()() const noexcept { return m_Value; }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        return StrongRef<JReal>(m_Value, parent);
    }
private:
    double m_Value;
};

class JObject final : public JValue
{
    DEFAULT_DESTRUCT(JObject);
    DEFAULT_CM_PU(JObject);
public:
    JObject(const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Object, parent)
        , m_Object()
    { }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        StrongRef<JObject> object(parent);

        for(const auto& pair : m_Object)
        {
            object->Insert(object, pair.first, pair.second);
        }

        return object;
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const StrongRef<JValue>& value) noexcept
    {
        m_Object[StringCast<c8>(key)] = value->Duplicate(self);
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, StrongRef<JValue>&& value) noexcept
    {
        m_Object[StringCast<c8>(key)] = value->Duplicate(self);
    }

    template<typename CharKey, typename CharValue>
    void Insert(const WeakRef<JObject>& self, const DynStringT<CharKey>& key, const DynStringT<CharValue>& value) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JString>(StringCast<c8>(value), self);
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const u64 value) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JInteger>(value, self);
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const i64 value) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JInteger>(value, self);
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const double value) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JReal>(value, self);
    }

    template<typename Char>
    StrongRef<JObject> InsertObject(const WeakRef<JObject>& self, const DynStringT<Char>& key) noexcept
    {
        StrongRef<JObject> object(self);
        m_Object[StringCast<c8>(key)] = object;
        return object;
    }

    template<typename Char>
    StrongRef<JArray> InsertArray(const WeakRef<JObject>& self, const DynStringT<Char>& key) noexcept
    {
        StrongRef<JArray> array(self);
        m_Object[StringCast<c8>(key)] = array;
        return array;
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const bool value) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JBool>(value, self);
    }

    template<typename Char>
    void Insert(const WeakRef<JObject>& self, const DynStringT<Char>& key, const nullptr_t) noexcept
    {
        m_Object[StringCast<c8>(key)] = StrongRef<JNull>(self);
    }

    template<typename Char>
    [[nodiscard]] bool Contains(const DynStringT<Char>& key) const noexcept
    {
        return m_Object.contains(key);
    }

    template<typename Char>
    [[nodiscard]] JValueWrapper Get(const DynStringT<Char>& key) const noexcept
    {
        if(!m_Object.contains(key))
        {
            return {};
        }

        const StrongRef<JValue>& value = m_Object[key];

        return value->InternalGetWrapper(value);
    }

    template<typename Char>
    [[nodiscard]] JValueWrapper operator[](const DynStringT<Char>& key) const noexcept
    {
        const StrongRef<JValue>& value = m_Object[key];

        return value->InternalGetWrapper(value);
    }

    [[nodiscard]] const ::std::unordered_map<C8DynString, StrongRef<JValue>>& Iterable() const noexcept { return m_Object; }
private:
    ::std::unordered_map<C8DynString, StrongRef<JValue>> m_Object;
};

class JArray final : public JValue
{
    DEFAULT_DESTRUCT(JArray);
    DEFAULT_CM_PU(JArray);
public:
    JArray(const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Array, parent)
    { }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        StrongRef<JArray> array(parent);

        for(const auto& value : m_Array)
        {
            array->Append(array, value);
        }

        return array;
    }
    
    void Append(const WeakRef<JArray>& self, const StrongRef<JValue>& value) noexcept
    {
        m_Array.push_back(value->Duplicate(self));
    }
    
    void Append(const WeakRef<JArray>& self, StrongRef<JValue>&& value) noexcept
    {
        m_Array.push_back(value->Duplicate(self));
    }

    template<typename CharValue>
    void Append(const WeakRef<JArray>& self, const DynStringT<CharValue>& value) noexcept
    {
        m_Array.push_back(StrongRef<JString>(StringCast<c8>(value), self));
    }
    
    void Append(const WeakRef<JArray>& self, const u64 value) noexcept
    {
        m_Array.push_back(StrongRef<JInteger>(value, self));
    }
    
    void Append(const WeakRef<JArray>& self, const i64 value) noexcept
    {
        m_Array.push_back(StrongRef<JInteger>(value, self));
    }

    void Append(const WeakRef<JArray>& self, const double value) noexcept
    {
        m_Array.push_back(StrongRef<JReal>(value, self));
    }

    StrongRef<JObject> AppendObject(const WeakRef<JArray>& self) noexcept
    {
        StrongRef<JObject> object(self);
        m_Array.push_back(object);
        return object;
    }

    StrongRef<JArray> AppendArray(const WeakRef<JArray>& self) noexcept
    {
        StrongRef<JArray> array(self);
        m_Array.push_back(array);
        return array;
    }

    void Append(const WeakRef<JArray>& self, const bool value) noexcept
    {
        m_Array.push_back(StrongRef<JBool>(value, self));
    }

    void Append(const WeakRef<JArray>& self, const nullptr_t) noexcept
    {
        m_Array.push_back(StrongRef<JNull>(self));
    }
    
    [[nodiscard]] uSys Length() const noexcept
    {
        return m_Array.size();
    }
    
    [[nodiscard]] JValueWrapper Get(const uSys index) const noexcept
    {
        if(index >= m_Array.size())
        {
            return {};
        }

        const StrongRef<JValue>& value = m_Array[index];

        return value->InternalGetWrapper(value);
    }
    
    [[nodiscard]] JValueWrapper operator[](const uSys index) const noexcept
    {
        const StrongRef<JValue>& value = m_Array[index];

        return value->InternalGetWrapper(value);
    }

    [[nodiscard]] const ::std::vector<StrongRef<JValue>>& Iterable() const noexcept { return m_Array; }
private:
    ::std::vector<StrongRef<JValue>> m_Array;
};

class JBool final : public JValue
{
    DEFAULT_DESTRUCT(JBool);
    DEFAULT_CM_PU(JBool);
public:
    JBool(const bool value, const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Bool, parent)
        , m_Value(value)
    { }

    [[nodiscard]] bool Bool() const noexcept { return m_Value; }
    [[nodiscard]] operator bool() const noexcept { return m_Value; }
    [[nodiscard]] bool operator()() const noexcept { return m_Value; }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        return StrongRef<JBool>(m_Value, parent);
    }
private:
    bool m_Value;
};

class JNull final : public JValue
{
    DEFAULT_DESTRUCT(JNull);
    DEFAULT_CM_PU(JNull);
public:
    JNull(const WeakRef<JValue>& parent) noexcept
        : JValue(EJType::Null, parent)
    { }

    [[nodiscard]] nullptr_t Bool() const noexcept { return nullptr; }
    [[nodiscard]] operator nullptr_t() const noexcept { return nullptr; }
    [[nodiscard]] nullptr_t operator()() const noexcept { return nullptr; }

    [[nodiscard]] JValueWrapper InternalGetWrapper(const StrongRef<JValue>& value) const noexcept override;

    [[nodiscard]] StrongRef<JValue> Duplicate(const WeakRef<JValue>& parent) const noexcept override
    {
        return StrongRef<JNull>(parent);
    }
};

class JStringWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JStringWrapper);
    DEFAULT_CM_PU(JStringWrapper);
public:
    JStringWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsString)
    { }

    [[nodiscard]] static StrongRef<JString> AsString(const StrongRef<JValue>& value) noexcept { return RefCast<JString>(value); }
};

class JIntegerWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JIntegerWrapper);
    DEFAULT_CM_PU(JIntegerWrapper);
public:
    JIntegerWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsInteger)
    { }

    [[nodiscard]] static StrongRef<JInteger> AsInteger(const StrongRef<JValue>& value) noexcept { return RefCast<JInteger>(value); }
};

class JRealWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JRealWrapper);
    DEFAULT_CM_PU(JRealWrapper);
public:
    JRealWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsReal)
    { }

    [[nodiscard]] static StrongRef<JReal> AsReal(const StrongRef<JValue>& value) noexcept { return RefCast<JReal>(value); }
};

class JObjectWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JObjectWrapper);
    DEFAULT_CM_PU(JObjectWrapper);
public:
    JObjectWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsObject)
    { }

    [[nodiscard]] static StrongRef<JObject> AsObject(const StrongRef<JValue>& value) noexcept { return RefCast<JObject>(value); }
};

class JArrayWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JArrayWrapper);
    DEFAULT_CM_PU(JArrayWrapper);
public:
    JArrayWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsArray)
    { }

    [[nodiscard]] static StrongRef<JArray> AsArray(const StrongRef<JValue>& value) noexcept { return RefCast<JArray>(value); }
};

class JBoolWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JBoolWrapper);
    DEFAULT_CM_PU(JBoolWrapper);
public:
    JBoolWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsBool)
    { }

    [[nodiscard]] static StrongRef<JBool> AsBool(const StrongRef<JValue>& value) noexcept { return RefCast<JBool>(value); }
};

class JNullWrapper final : public JValueWrapper
{
    DEFAULT_DESTRUCT(JNullWrapper);
    DEFAULT_CM_PU(JNullWrapper);
public:
    JNullWrapper(const StrongRef<JValue>& value) noexcept
        : JValueWrapper(value, AsNull)
    { }

    [[nodiscard]] static StrongRef<JNull> AsNull(const StrongRef<JValue>& value) noexcept { return RefCast<JNull>(value); }
};

static_assert(sizeof(JStringWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JIntegerWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JRealWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JObjectWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JArrayWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JBoolWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");
static_assert(sizeof(JNullWrapper) == sizeof(JValueWrapper), "JWrapper sizes do not match");

inline JValueWrapper JString::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JStringWrapper(value);
}

inline JValueWrapper JInteger::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JIntegerWrapper(value);
}

inline JValueWrapper JReal::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JRealWrapper(value);
}

inline JValueWrapper JObject::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JObjectWrapper(value);
}

inline JValueWrapper JArray::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JArrayWrapper(value);
}

inline JValueWrapper JBool::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JBoolWrapper(value);
}

inline JValueWrapper JNull::InternalGetWrapper(const StrongRef<JValue>& value) const noexcept
{
    return JNullWrapper(value);
}

class JRoot final
{
    DEFAULT_DESTRUCT(JRoot);
    DEFAULT_CM_PU(JRoot);
private:
    JRoot(int)
        : m_Root(nullptr)
    {
        StrongRef<JObject> object;
        object.Reset(nullptr);
        m_Root = object;
    }

    JRoot(nullptr_t)
        : m_Root(nullptr)
    {
        StrongRef<JArray> array;
        array.Reset(nullptr);
        m_Root = array;
    }
public:
    [[nodiscard]] static JRoot CreateObject() noexcept
    {
        return JRoot(0);
    }

    [[nodiscard]] static JRoot CreateArray() noexcept
    {
        return JRoot(nullptr);
    }

    [[nodiscard]] EJType Type() const noexcept { return m_Root->Type(); }
    [[nodiscard]] JValueWrapper Get() const noexcept { return m_Root->InternalGetWrapper(m_Root); }
private:
    StrongRef<JValue> m_Root;
};

}
