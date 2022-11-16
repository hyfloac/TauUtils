#pragma once

#include "TauMacros.hpp"

#ifdef TAU_UTILS_BUILD_SHARED
  #define TAU_LIB DYNAMIC_EXPORT
#elif defined(TAU_UTILS_BUILD_STATIC)
  #define TAU_LIB
#elif defined(TAU_UTILS_IMPORT_SHARED)
  #define TAU_LIB DYNAMIC_IMPORT
#else
  #define TAU_LIB
#endif
