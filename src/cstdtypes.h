/*
 *  cstdtypes.h
 *  
 */

#ifndef INCLUDED_CSTDTYPES__H
#define INCLUDED_CSTDTYPES__H


// C language standard, 1999
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
#include "_windows/inttypes.h"		// For precise conversion between integer types. (New with C99)
#include "_windows/stdint.h"		// For defining various integer types. (New with C99)
#else
#include <inttypes.h>		// For precise conversion between integer types. (New with C99)
#include <stdint.h>			// For defining various integer types. (New with C99)
#endif



// This would be the *correct* or *proper* replacement for broken wchar_t. 
// Unfortunatly no compiler distribution on any useful platform has this "standard" header file. 
// C language, ISO/IEC TR 19769, 2004
//#include <uchar.h>		// Defines character types char16_t, char32_t, and functions mbrtoc16(), c16rtomb(), mbrtoc32(), c32rtomb()


#ifdef __cplusplus
// No compiler distribution on any useful platform has this "standard" header file. 
// C++ language, ISO/IEC TR 19769 (WG14 N1040), 2006
//#include <cuchar>			// Defines character types char16_t, char32_t, and functions mbrtoc16(), c16rtomb(), mbrtoc32(), c32rtomb()
#endif // #ifdef __cplusplus


#ifndef int8
    typedef int8_t  int8;
#endif    
#ifndef uint8
    typedef uint8_t  uint8;
#endif    
#ifndef int16
    typedef int16_t  int16;
#endif    
#ifndef uint16
    typedef uint16_t  uint16;
#endif    
#ifndef int32
    typedef int32_t  int32;
#endif    
#ifndef uint32
    typedef uint32_t  uint32;
#endif    
#ifndef int64
    typedef int64_t  int64;
#endif    
#ifndef uint64
    typedef uint64_t  uint64;
#endif    


#ifndef char8
    typedef char  char8;
#endif    
#ifndef char16
    #if defined char16_t
        typedef char16_t  char16;
    #else
        typedef uint16_t  char16;
    #endif
#endif    
#ifndef char32
    #if defined char32_t
        typedef char32_t  char32;
    #else
        typedef uint32_t  char32;
    #endif
#endif    


#endif  // #ifndef INCLUDED_CSTDTYPES__H
