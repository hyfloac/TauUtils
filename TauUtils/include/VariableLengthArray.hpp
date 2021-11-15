#pragma once

#ifdef _MSVC_LANG
#pragma warning(push, 0)
  #include <malloc.h>
#pragma warning(pop)
  #define VLA(TYPE, NAME, SIZE) TYPE* NAME = static_cast<TYPE*>(_alloca((SIZE) * sizeof(TYPE)))
#else
  #define VLA(TYPE, NAME, SIZE) TYPE NAME[_SIZE]
#endif

#define VLA_MAX_LEN (512)
