#pragma once

#ifdef _MSC_VER
  #define ALIGN_MSVC(ALIGNMENT) __declspec(align(ALIGNMENT))
  #define ALIGN_GCC(ALIGNMENT)
#else
  #define ALIGN_MSVC(ALIGNMENT) 
  #define ALIGN_GCC(ALIGNMENT) __attribute__((aligned(ALIGNMENT)))
#endif
