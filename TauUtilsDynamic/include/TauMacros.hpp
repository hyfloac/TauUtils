#pragma once

#include "CVersion.hpp"

#ifdef _MSC_VER
  #define INLINE_PRAGMA(X) __pragma(X)
#elif defined(__GNUC__)
  #define INLINE_PRAGMA(X) _Pragma(X)
#else
  #define INLINE_PRAGMA(X)
#endif

#ifdef CPP_VERSION_14
  #define DEPRECATED [[deprecated]]
  #define DEPRECATED_R(REASON) [[deprecated(REASON)]]
#elif defined(__GNUC__)
  #ifdef UTILS_DISABLE_PRAGMA
    #define DEPRECATED __attribute__((deprecated))
    #define DEPRECATED_R(REASON) __attribute__((deprecated(REASON)))
  #else
    #define DEPRECATED INLINE_PRAGMA(deprecated)
    #define DEPRECATED_R(REASON) INLINE_PRAGMA(deprecated)
  #endif
#elif defined(_MSC_VER)
  #ifdef UTILS_DISABLE_PRAGMA
    #define DEPRECATED __declspec(deprecated)
    #define DEPRECATED_R(REASON) __declspec(deprecated(REASON))
  #else
    #define DEPRECATED INLINE_PRAGMA(deprecated)
    #define DEPRECATED_R(REASON) INLINE_PRAGMA(deprecated)
  #endif
#else
  #warning "Your compiler needs to have to ability to use DEPRECATED"
  #define DEPRECATED
  #define DEPRECATED_R(REASON)
#endif

#ifndef CPP_VERSION_11
  #ifdef USE_C_NULL
    #define nullptr ((void*) 0)
  #else
    #define nullptr NULL
  #endif
#endif

#ifdef _WIN32
  #define DEBUG_BREAK __debugbreak()
#else
  #define DEBUG_BREAK asm volatile ("int $3;")
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
  #define RESTRICT __restrict
#else
  #define RESTRICT
#endif

#if defined(_WIN32)
  #define NOVTABLE __declspec(novtable)
#else
  #define NOVTABLE
#endif

#if defined(__clang__) && defined(CPP_VERSION_11)
    #define DYNAMIC_EXPORT [[gnu::dllexport]]
    #define DYNAMIC_IMPORT [[gnu::dllimport]]
#elif defined(_WIN32)
  #define DYNAMIC_EXPORT __declspec(dllexport)
  #define DYNAMIC_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
  #define DYNAMIC_EXPORT __attribute__((visibility("default")))
  #define DYNAMIC_IMPORT 
#else
  #define DYNAMIC_EXPORT
  #define DYNAMIC_IMPORT
#endif

#define UNUSED(X0)                  ((void) (X0))
#define UNUSED1(X0)                 ((void) (X0))
#define UNUSED2(X0, X1)             ((void) (X0)); ((void) (X1))
#define UNUSED3(X0, X1, X2)         ((void) (X0)); ((void) (X1)); ((void) (X2))
#define UNUSED4(X0, X1, X2, X3)     ((void) (X0)); ((void) (X1)); ((void) (X2)); ((void) (X3))
#define UNUSED5(X0, X1, X2, X3, X4) ((void) (X0)); ((void) (X1)); ((void) (X2)); ((void) (X3)); ((void) (X4))
      
#ifdef TAU_UTILS_BUILD_SHARED
  #define TAU_LIB DYNAMIC_EXPORT
#elif defined(TAU_UTILS_BUILD_STATIC)
  #define TAU_LIB
#elif defined(TAU_UTILS_IMPORT_SHARED)
  #define TAU_LIB DYNAMIC_IMPORT
#else
  #define TAU_LIB
#endif
