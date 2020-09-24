/*
 * Summary: macros for marking symbols as exportable/importable.
 * Description: macros for marking symbols as exportable/importable.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Igor Zlatovic <igor@zlatkovic.com>
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLEXPORTS_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLEXPORTS_H

/**
 * XMLPUBFUN, XMLPUBVAR, XMLCALL
 *
 * Macros which declare an exportable function, an exportable variable and
 * the calling convention used for functions.
 *
 * Please use an extra block for every platform/compiler combination when
 * modifying this, rather than overlong #ifdef lines. This helps
 * readability as well as the fact that different compilers on the same
 * platform might need different definitions.
 */

/**
 * XMLPUBFUN:
 *
 * Macros which declare an exportable function
 */
#define XMLPUBFUN
/**
 * XMLPUBVAR:
 *
 * Macros which declare an exportable variable
 */
#define XMLPUBVAR extern
/**
 * XMLCALL:
 *
 * Macros which declare the called convention for exported functions
 */
#define XMLCALL

/** DOC_DISABLE */

#if !defined(FBXSDK_SHARED)
	#define LIBXML_STATIC
#endif

/* Windows platform with MS compiler */
#if defined(_WIN32) && defined(_MSC_VER)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(FBXSDK_SHARED) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR extern // __declspec(dllexport)
  #else
    #if !defined(LIBXML_STATIC)
		#define XMLPUBVAR extern //__declspec(dllimport) extern
		#define XMLPUBFUN __declspec(dllimport)
    #else
      #define XMLPUBVAR extern
      #define XMLPUBFUN
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with Borland compiler */
#if defined(_WIN32) && defined(__BORLANDC__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(FBXSDK_SHARED) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport) extern
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR extern
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Windows platform with GNU compiler (Mingw) */
#if defined(_WIN32) && defined(__MINGW32__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(FBXSDK_SHARED) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport) extern
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR extern
    #endif
  #endif
  #define XMLCALL __cdecl
  #if !defined _REENTRANT
    #define _REENTRANT
  #endif
#endif

/* Cygwin platform, GNU compiler */
#if defined(_WIN32) && defined(__CYGWIN__)
  #undef XMLPUBFUN
  #undef XMLPUBVAR
  #undef XMLCALL
  #if defined(FBXSDK_SHARED) && !defined(LIBXML_STATIC)
    #define XMLPUBFUN __declspec(dllexport)
    #define XMLPUBVAR __declspec(dllexport)
  #else
    #define XMLPUBFUN
    #if !defined(LIBXML_STATIC)
      #define XMLPUBVAR __declspec(dllimport) extern
    #else
      #define XMLPUBVAR
    #endif
  #endif
  #define XMLCALL __cdecl
#endif

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLEXPORTS_H

