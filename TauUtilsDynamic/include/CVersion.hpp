#pragma once

#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
  #define _WIN32
#endif

#ifdef __cplusplus
  #ifdef _MSVC_LANG
    #define CPP_VERSION_DATE _MSVC_LANG
  #else 
    #define CPP_VERSION_DATE __cplusplus
  #endif
#elif defined(_MSVC_LANG)
  #define CPP_VERSION_DATE _MSVC_LANG
#else
  #define CPP_VERSION_DATE 1
  #warning("__cplusplus is not defined.")
#endif


#if CPP_VERSION_DATE > 202002L
  #define CPP_VERSION_23

  #ifndef CPP_VERSION
    #define CPP_VERSION 23
  #endif
#endif

#if CPP_VERSION_DATE >= 202002L
  #define CPP_VERSION_20

  #ifndef CPP_VERSION
    #define CPP_VERSION 20
  #endif
#endif
      
#if CPP_VERSION_DATE >= 201703L || (defined(_MSC_VER) && _MSC_VER > 1900)
  #define CPP_VERSION_17

  #ifndef CPP_VERSION
    #define CPP_VERSION 17
  #endif
#endif

#if CPP_VERSION_DATE >= 201402L
  #define CPP_VERSION_14

  #ifndef CPP_VERSION
    #define CPP_VERSION 14
  #endif
#endif

#if CPP_VERSION_DATE >= 201103L
  #define CPP_VERSION_11

  #ifndef CPP_VERSION
    #define CPP_VERSION 11
  #endif
#endif

#if CPP_VERSION_DATE >= 199711L
  #define CPP_VERSION_98

  #ifndef CPP_VERSION
    #define CPP_VERSION 98
  #endif
#endif
      
#if CPP_VERSION_DATE >= 1
  #define CPP_VERSION_1

  #ifndef CPP_VERSION
    #define CPP_VERSION 1
  #endif
#endif
