#pragma once

#include "TauMacros.hpp"

#ifdef TAU_UTILS_BUILD_SHARED
  #define TAU_UTILS_LIB DYNAMIC_EXPORT
#elif defined(TAU_UTILS_BUILD_STATIC)
  #define TAU_UTILS_LIB
#elif defined(TAU_UTILS_IMPORT_SHARED)
  #define TAU_UTILS_LIB DYNAMIC_IMPORT
#else
  #define TAU_UTILS_LIB
#endif
