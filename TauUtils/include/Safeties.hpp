#pragma once

#pragma warning(push, 0)
#include <cstdio>
#include <memory>
#pragma warning(pop)

#include "ReferenceCountingPointer.hpp"
#include "Utils.hpp"

#define TAU_SAFETIES_str0(_X) #_X
#define TAU_SAFETIES_str(_X) TAU_SAFETIES_str0(_X)

#define TauAssert(STATE) \
    do { \
        if(!(STATE)) { \
            fprintf(stderr, "`" #STATE "` Evaluated to false at line " TAU_SAFETIES_str(__LINE__) " in file \"" TAU_SAFETIES_str(__FILE__) "\".\n"); \
            DEBUG_BREAK; \
        } \
    } while(0)

#define RUNTIME_ERROR(TEXT) \
    do { \
        fprintf(stderr, TEXT "\nOccured at line " TAU_SAFETIES_str(__LINE__) " in file \"" TAU_SAFETIES_str(__FILE__) "\".\n"); \
        DEBUG_BREAK; \
    } while(0)

#define ERROR_CODE(ERR)        do { if(error) { *error = ERR; } return;         } while(0)
#define ERROR_CODE_V(ERR, VAL) do { if(error) { *error = ERR; } return VAL;     } while(0)
#define ERROR_CODE_N(ERR)      do { if(error) { *error = ERR; } return nullptr; } while(0)
#define ERROR_CODE_T(ERR)      do { if(error) { *error = ERR; } return true;    } while(0)
#define ERROR_CODE_F(ERR)      do { if(error) { *error = ERR; } return false;   } while(0)
#define ERROR_CODE_0(ERR)      do { if(error) { *error = ERR; } return 0;       } while(0)
#define ERROR_CODE_1(ERR)      do { if(error) { *error = ERR; } return 1;       } while(0)
#define ERROR_CODE_COND(COND, ERR)        do { if((COND)) { if(error) { *error = ERR; } return;         } } while(0)
#define ERROR_CODE_COND_V(COND, ERR, VAL) do { if((COND)) { if(error) { *error = ERR; } return VAL;     } } while(0)
#define ERROR_CODE_COND_N(COND, ERR)      do { if((COND)) { if(error) { *error = ERR; } return nullptr; } } while(0)
#define ERROR_CODE_COND_T(COND, ERR)      do { if((COND)) { if(error) { *error = ERR; } return true;    } } while(0)
#define ERROR_CODE_COND_F(COND, ERR)      do { if((COND)) { if(error) { *error = ERR; } return false;   } } while(0)
#define ERROR_CODE_COND_0(COND, ERR)      do { if((COND)) { if(error) { *error = ERR; } return 0;       } } while(0)
#define ERROR_CODE_COND_1(COND, ERR)      do { if((COND)) { if(error) { *error = ERR; } return 1;       } } while(0)

#ifdef _DEBUG
  #define Ensure(STATE) TauAssert(STATE)
#else
  #define Ensure(STATE)
#endif

template<typename T, typename D = std::default_delete<T>>
using Scoped = std::unique_ptr<T, D>;

template<typename T>
using CPPRef = std::shared_ptr<T>;

template<typename T>
using CPPStrongRef = std::shared_ptr<T>;

template<typename T>
using CPPWeakRef = std::weak_ptr<T>;

template<typename T>
using Ref = ReferenceCountingPointer<T>;

template<typename T>
using NullableRef = Ref<T>;

template<typename T>
using StrongRef = StrongReferenceCountingPointer<T>;

template<typename T>
using WeakRef = WeakReferenceCountingPointer<T>;

template<typename T>
using NullableStrongRef = StrongRef<T>;

template<typename T>
using NullableWeakRef = WeakRef<T>;

template<typename Out, typename In>
[[nodiscard]] static inline CPPRef<Out> RefStaticCast(const CPPRef<In>& in) noexcept
{ return ::std::static_pointer_cast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline Ref<Out> RefStaticCast(const Ref<In>& in) noexcept
{ return RCPStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline StrongRef<Out> RefStaticCast(const StrongRef<In>& in) noexcept
{ return RCPStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline WeakRef<Out> RefStaticCast(const WeakRef<In>& in) noexcept
{ return RCPStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline CPPRef<Out> RefReinterpretCast(const CPPRef<In>& in) noexcept
{ return ::std::reinterpret_pointer_cast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline Ref<Out> RefReinterpretCast(const Ref<In>& in) noexcept
{ return RCPReinterpretCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline StrongRef<Out> RefReinterpretCast(const StrongRef<In>& in) noexcept
{ return RCPReinterpretCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline WeakRef<Out> RefReinterpretCast(const WeakRef<In>& in) noexcept
{ return RCPReinterpretCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline CPPRef<Out> RefCast(const CPPRef<In>& in) noexcept
{ return RefStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline Ref<Out> RefCast(const Ref<In>& in) noexcept
{ return RefStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline StrongRef<Out> RefCast(const StrongRef<In>& in) noexcept
{ return RefStaticCast<Out>(in); }

template<typename Out, typename In>
[[nodiscard]] static inline WeakRef<Out> RefCast(const WeakRef<In>& in) noexcept
{ return RefStaticCast<Out>(in); }
