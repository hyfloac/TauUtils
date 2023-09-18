// ReSharper disable CppClangTidyCertDcl58Cpp
// ReSharper disable CppRedundantNamespaceDefinition
#pragma once

#include "Objects.hpp"
#include "NumTypes.hpp"
#include "Safeties.hpp"
#include <type_traits>
#include <functional>

#ifndef TAU_RTTI_DEBUG
  #if defined(TAU_PRODUCTION) || 1
    #define TAU_RTTI_DEBUG 0
  #else
    #define TAU_RTTI_DEBUG 1
  #endif
#endif

/**
 * A lightweight wrapper for RTTI.
 *
 *   Depending on whether debug mode is enabled this either
 * stores an incremental integer or a pointer to itself.
 *
 *   When debug mode is enabled this stores a pointer to itself
 * as well as the types name, and the parent RTTI structure. We
 * store the pointer because it will be unique, and consistent.
 * This potentially can make certain parts easier to debug.
 *
 *   When debug mode is not enabled this stores an incremental
 * integer. This integer is incremented at runtime and is
 * dependent on the order in which RTTI is checked. This is
 * because the integer is generated when the class is lazily
 * instantiated. The RTTI structure is stored as a static
 * member within a specialized function. Static function
 * variables are instantiated on the first call to the
 * function. Thus if the order in which types are checked
 * changes between runs, the underlying RTTI value will change,
 * making everything significantly more obfuscated, while still
 * being able to uniquely identify each type. This integer is
 * also incremented using atomics, so it is safe to instantiate
 * the type from multiple threads simultaneously. There is also
 * a template type, this is a type safety restriction that
 * ensures that two entirely different types with the same
 * underlying RTTI value aren't identified as the same by
 * accident.
 */
template<typename T>
class RunTimeType;

namespace std {
    template<typename T>
    struct hash<RunTimeType<T>> final
    {
        [[nodiscard]] inline ::std::size_t operator()(const RunTimeType<T>& rtt) const noexcept;
    };
}

#if TAU_RTTI_DEBUG
template<typename T>
class RunTimeType final
{
    DEFAULT_DESTRUCT(RunTimeType);
    DEFAULT_CM_PU(RunTimeType);
public:
    RunTimeType(const char* const name, const RunTimeType<T>* const parent = nullptr) noexcept
        : m_Uid(this)
        , m_Name(name)
        , m_Parent(parent)
    { }

    [[nodiscard]] const char* Name() const noexcept { return m_Name; }
    [[nodiscard]] const RunTimeType<T>* Parent() const noexcept { return m_Parent; }

    [[nodiscard]] const char* name() const noexcept { return m_Name; }
    [[nodiscard]] const RunTimeType<T>* parent() const noexcept { return m_Parent; }

    [[nodiscard]] bool operator ==(const RunTimeType<T>& other) const noexcept { return m_Uid == other.m_Uid; }
    [[nodiscard]] bool operator !=(const RunTimeType<T>& other) const noexcept { return m_Uid != other.m_Uid; }
private:
    void* m_Uid;
    const char* m_Name;
    const RunTimeType<T>* m_Parent;
private:
    friend struct std::hash<RunTimeType<T>>;
};
#else
template<typename T>
class RunTimeType final
{
    DEFAULT_DESTRUCT(RunTimeType);
    DEFAULT_CM_PU(RunTimeType);
public:
    inline RunTimeType() noexcept
        : m_Uid(this)
    { }

    [[nodiscard]] const char* Name() const noexcept { return nullptr; }
    [[nodiscard]] const RunTimeType<T>* Parent() const noexcept { return nullptr; }

    [[nodiscard]] const char* name() const noexcept { return nullptr; }
    [[nodiscard]] const RunTimeType<T>* parent() const noexcept { return nullptr; }

    [[nodiscard]] bool operator ==(const RunTimeType<T>& other) const noexcept { return m_Uid == other.m_Uid; }
    [[nodiscard]] bool operator !=(const RunTimeType<T>& other) const noexcept { return m_Uid != other.m_Uid; }
private:
    void* m_Uid;
private:
    friend struct std::hash<RunTimeType<T>>;
};
#endif

template<typename T>
[[nodiscard]] inline ::std::size_t std::hash<RunTimeType<T>>::operator()(const RunTimeType<T>& rtt) const noexcept
{ return static_cast<::std::size_t>(rtt.m_Uid); }

#define TAU_RTTI_STRING0(X) #X
#define TAU_RTTI_STRING(X) TAU_RTTI_STRING0(X)

#if TAU_RTTI_DEBUG
  #define RTT_BASE_IMPL(TYPE) \
    public:                                                                       \
        [[nodiscard]] static RunTimeType<TYPE> _getStaticRTType() noexcept;       \
        [[nodiscard]] virtual RunTimeType<_TYPE> _getRTType() const noexcept = 0; \

  #define RTT_BASE_IMPL_TU(TYPE) \
        RunTimeType<TYPE> TYPE::_getStaticRTType() noexcept      \
        { static RunTimeType<_TYPE> type(TAU_RTTI_STRING(TYPE)); \
          return type; }

  #define RTTD_BASE_IMPL(TYPE) \
    public:                                                                        \
        [[nodiscard]] static RunTimeType<TYPE> _getStaticRTType_##TYPE() noexcept; \
        [[nodiscard]] virtual RunTimeType<TYPE> _getRTType_##TYPE() const noexcept = 0;

  #define RTTD_BASE_IMPL_TU(TYPE) \
        RunTimeType<TYPE> TYPE::_getStaticRTType_##TYPE() noexcept    \
        { static RunTimeType<TYPE> type(TAU_RTTI_STRING(TYPE)); \
          return type; }
#else
  #define RTT_BASE_IMPL(TYPE) \
    public:                   \
        [[nodiscard]] virtual RunTimeType<TYPE> _getRTType() const noexcept = 0;

  #define RTT_BASE_IMPL_TU(TYPE)

  #define RTTD_BASE_IMPL(_TYPE) \
    public:                     \
        [[nodiscard]] virtual RunTimeType<TYPE> _getRTType_##TYPE() const noexcept = 0;

  #define RTTD_BASE_IMPL_TU(TYPE)
#endif

#if TAU_RTTI_DEBUG
  #define RTT_IMPL(TYPE, BASE_TYPE) \
    public:                                                                               \
        [[nodiscard]] static RunTimeType<BASE_TYPE> _getStaticRTType() noexcept;          \
        [[nodiscard]] virtual RunTimeType<BASE_TYPE> _getRTType() const noexcept override \
        { return TYPE::_getStaticRTType(); }

  #define RTT_IMPL_TU(TYPE, BASE_TYPE) \
    RunTimeType<BASE_TYPE> TYPE::_getStaticRTType() noexcept                                     \
    { static RunTimeType<BASE_TYPE> type(TAU_RTTI_STRING(TYPE), &BASE_TYPE::_getStaticRTType()); \
      return type; } \

  #define RTTD_IMPL(TYPE, BASE_TYPE) \
    public:                                                                                           \
        [[nodiscard]] static RunTimeType<BASE_TYPE> _getStaticRTType_##BASE_TYPE() noexcept;          \
        [[nodiscard]] virtual RunTimeType<BASE_TYPE> _getRTType_##BASE_TYPE() const noexcept override \
        { return TYPE::_getStaticRTType_##BASE_TYPE(); }

  #define RTTD_IMPL_TU(TYPE, BASE_TYPE) \
     RunTimeType<BASE_TYPE> TYPE::_getStaticRTType_##BASE_TYPE() noexcept                                      \
     { static RunTimeType<BASE_TYPE> type(TAU_RTTI_STRING(TYPE), &_BASE_TYPE::_getStaticRTType_##BASE_TYPE()); \
       return type; }
#else
  #define RTT_IMPL(TYPE, BASE_TYPE) \
    public: \
        [[nodiscard]] static RunTimeType<BASE_TYPE> _getStaticRTType() noexcept;          \
        [[nodiscard]] virtual RunTimeType<BASE_TYPE> _getRTType() const noexcept override \
        { return TYPE::_getStaticRTType(); }

  #define RTT_IMPL_TU(TYPE, BASE_TYPE) \
    RunTimeType<BASE_TYPE> TYPE::_getStaticRTType() noexcept \
    { static RunTimeType<BASE_TYPE> type;                    \
      return type; } 

  #define RTTD_IMPL(TYPE, BASE_TYPE) \
    public: \
        [[nodiscard]] static RunTimeType<BASE_TYPE> _getStaticRTType_##BASE_TYPE() noexcept;          \
        [[nodiscard]] virtual RunTimeType<BASE_TYPE> _getRTType_##BASE_TYPE() const noexcept override \
        { return TYPE::_getStaticRTType_##BASE_TYPE(); }

  #define RTTD_IMPL_TU(TYPE, BASE_TYPE) \
    RunTimeType<BASE_TYPE> TYPE::_getStaticRTType_##BASE_TYPE() noexcept \
    { static RunTimeType<BASE_TYPE> type;                                \
      return type; }
#endif

#define RTT_BASE_CHECK(TYPE) \
    public:                                                                       \
        template<typename T>                                                      \
        [[nodiscard]] bool _isRTType() const noexcept                             \
        { return T::_getStaticRTType() == _getRTType(); }                         \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const TYPE& obj) noexcept             \
        { return obj._isRTType<T>(); }                                            \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const TYPE* const obj) noexcept       \
        { return obj->_isRTType<T>(); }                                           \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const CPPRef<TYPE>& obj) noexcept     \
        { return obj->_isRTType<T>(); }                                           \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const CPPWeakRef<TYPE>& obj) noexcept \
        { return obj->_isRTType<T>(); }                                           \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const Ref<TYPE>& obj) noexcept        \
        { return obj->_isRTType<T>(); }                                           \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const StrongRef<TYPE>& obj) noexcept  \
        { return obj->_isRTType<T>(); }                                           \
        template<typename T>                                                      \
        [[nodiscard]] static bool _isRTType(const WeakRef<TYPE>& obj) noexcept    \
        { return obj->_isRTType<T>(); }

#define RTTD_BASE_CHECK(TYPE) \
    public:                                                                              \
        template<typename T>                                                             \
        [[nodiscard]] bool _isRTType_##TYPE() const noexcept                             \
        { return T::_getStaticRTType_##TYPE() == _getRTType_##TYPE(); }                  \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const TYPE& obj) noexcept             \
        { return obj._isRTType_##TYPE<T>(); }                                            \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const TYPE* const obj) noexcept       \
        { return obj->_isRTType_##TYPE<T>(); }                                           \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const CPPRef<TYPE>& obj) noexcept     \
        { return obj->_isRTType_##TYPE<T>(); }                                           \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const CPPWeakRef<TYPE>& obj) noexcept \
        { return obj->_isRTType_##TYPE<T>(); }                                           \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const Ref<TYPE>& obj) noexcept        \
        { return obj->_isRTType_##TYPE<T>(); }                                           \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const StrongRef<TYPE>& obj) noexcept  \
        { return obj->_isRTType_##TYPE<T>(); }                                           \
        template<typename T>                                                             \
        [[nodiscard]] static bool _isRTType_##TYPE(const WeakRef<TYPE>& obj) noexcept    \
        { return obj->_isRTType_##TYPE<T>(); }

#define RTT_BASE_CAST(TYPE) \
    public:                                                                            \
        template<typename T>                                                           \
        [[nodiscard]] T* _castRTType() noexcept                                        \
        { return _isRTType<T>() ? static_cast<T*>(this) : nullptr; }                   \
        template<typename T>                                                           \
        [[nodiscard]] const T* _castRTType() const noexcept                            \
        { return _isRTType<T>() ? static_cast<const T*>(this) : nullptr; }             \
        template<typename T>                                                           \
        [[nodiscard]] static T* _castRTType(TYPE& obj) noexcept                        \
        { return obj._castRTType<T>(); }                                               \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const TYPE& obj) noexcept            \
        { return obj._castRTType<T>(); }                                               \
        template<typename T>                                                           \
        [[nodiscard]] static T* _castRTType(TYPE* const obj) noexcept                  \
        { return obj->_castRTType<T>(); }                                              \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const TYPE* const obj) noexcept      \
        { return obj->_castRTType<T>(); }                                              \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const CPPRef<TYPE>& obj) noexcept    \
        { return obj->_isRTType<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const Ref<TYPE>& obj) noexcept       \
        { return obj->_isRTType<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const StrongRef<TYPE>& obj) noexcept \
        { return obj->_isRTType<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                           \
        [[nodiscard]] static const T* _castRTType(const WeakRef<TYPE>& obj) noexcept   \
        { return obj->_isRTType<T>() ? RefStaticCast<T>(obj) : nullptr; }

#define RTTD_BASE_CAST(TYPE) \
    public:                                                                                   \
        template<typename T>                                                                  \
        [[nodiscard]]_T* _castRTType_##TYPE() noexcept                                        \
        { return _isRTType_##TYPE<T>() ? static_cast<T*>(this) : nullptr; }                   \
        template<typename T>                                                                  \
        [[nodiscard]] const T* _castRTType_##TYPE() const noexcept                            \
        { return _isRTType_##TYPE<T>() ? static_cast<const T*>(this) : nullptr; }             \
        template<typename T>                                                                  \
        [[nodiscard]] static T* _castRTType_##TYPE(TYPE& obj) noexcept                        \
        { return obj._castRTType_##TYPE<T>(); }                                               \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const TYPE& obj) noexcept            \
        { return obj._castRTType_##TYPE<T>(); }                                               \
        template<typename T>                                                                  \
        [[nodiscard]] static T* _castRTType_##TYPE(TYPE* const obj) noexcept                  \
        { return obj->_castRTType_##TYPE<T>(); }                                              \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const TYPE* const obj) noexcept      \
        { return obj->_castRTType_##TYPE<T>(); }                                              \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const CPPRef<TYPE>& obj) noexcept    \
        { return obj->_isRTType_##TYPE<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const Ref<TYPE>& obj) noexcept       \
        { return obj->_isRTType_##TYPE<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const StrongRef<TYPE>& obj) noexcept \
        { return obj->_isRTType_##TYPE<T>() ? RefStaticCast<T>(obj) : nullptr; }              \
        template<typename T>                                                                  \
        [[nodiscard]] static const T* _castRTType_##TYPE(const WeakRef<TYPE>& obj) noexcept   \
        { return obj->_isRTType_##TYPE<T>() ? RefStaticCast<T>(obj) : nullptr; }


template<typename TargetType, typename InputType, ::std::enable_if_t<::std::is_base_of_v<::std::remove_pointer_t<::std::remove_reference_t<InputType>>, TargetType>, int> = 0>
[[nodiscard]] bool rtt_check(const InputType& in) noexcept
{ return ::std::remove_pointer_t<::std::remove_reference_t<InputType>>::template _isRTType<TargetType>(in); }

template<typename TargetType, typename InputType, ::std::enable_if_t<::std::is_base_of_v<::std::remove_pointer_t<::std::remove_reference_t<InputType>>, TargetType>, int> = 0>
[[nodiscard]] TargetType* rtt_cast(const InputType& in) noexcept
{ return ::std::remove_pointer_t<::std::remove_reference_t<InputType>>::template _castRTType<TargetType>(in); }

#define RTT_CHECK(VAR, T) (::std::remove_pointer_t<::std::remove_reference_t<decltype(VAR)>>::_isRTType<T>(VAR))
#define RTT_CAST(VAR, T)  (::std::remove_pointer_t<::std::remove_reference_t<decltype(VAR)>>::_castRTType<T>(VAR))

#define RTTD_CHECK(VAR, T, BASE_TYPE) (::std::remove_pointer_t<::std::remove_reference_t<decltype(VAR)>>::_isRTType_##BASE_TYPE<T>(VAR))
#define RTTD_CAST(VAR, T, BASE_TYPE)  (::std::remove_pointer_t<::std::remove_reference_t<decltype(VAR)>>::_castRTType_##BASE_TYPE<T>(VAR))                   
