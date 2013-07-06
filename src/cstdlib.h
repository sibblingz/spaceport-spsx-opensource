/*
 *  cstdlib.h
 *  compile time = cheap. time spent discovering platform differences the hard way = expensive.
 */

#ifndef INCLUDED_CSTDLIB__H
#define INCLUDED_CSTDLIB__H

#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
#include "stdafx.h"
#endif

#if defined(__APPLE__)
    #include "TargetConditionals.h"
#endif // #if defined(__APPLE__)


// C language standard, 1972-1989
#include <assert.h>			// Contains the assert macro, used to assist with detecting logical errors and other types of bug in debugging versions of a program.
#include <ctype.h>			// Contains functions used to classify characters by their types or to convert between upper and lower case in a way that is independent of the used character set (typically ASCII or one of its extensions, although implementations utilizing EBCDIC are also known).
#include <errno.h>			// For testing error codes reported by library functions.
#include <float.h>			// Contains defined constants specifying the implementation-specific properties of the floating-point library, such as the minimum difference between two different floating-point numbers (_EPSILON), the maximum number of digits of accuracy (_DIG) and the range of numbers which can be represented (_MIN, _MAX).
#include <limits.h>			// Contains defined constants specifying the implementation-specific properties of the integer types, such as the range of numbers which can be represented (_MIN, _MAX).
#include <locale.h>			// For setlocale() and related constants. This is used to choose an appropriate locale.
#include <math.h>			// For computing common mathematical functions
#include <setjmp.h>			// Declares the macros setjmp and longjmp, which are used for non-local exits
#include <signal.h>			// For controlling various exceptional conditions
#include <stdarg.h>			// For accessing a varying number of arguments passed to functions.
#include <stddef.h>			// For defining several useful types and macros.
#include <stdio.h>			// Provides the core input and output capabilities of the C language. This file includes the venerable printf function.
#include <stdlib.h>			// For performing a variety of operations, including conversion, pseudo-random numbers, memory allocation, process control, environment, signalling, searching, and sorting.
#include <string.h>			// For manipulating several kinds of strings.
#include <time.h>			// For converting between various time and date formats.


// Normative Addendum 1, 1995
#include <iso646.h>			// For programming in ISO 646 variant character sets. (New with NA1)

// wchar_t is broken on all platforms.  Its useless because its not portable.
//#include <wchar.h>		// For manipulating wide streams and several kinds of strings using wide characters - key to supporting a range of languages. (New with NA1)
//#include <wctype.h>		// For classifying wide characters. (New with NA1)


// C language standard, 1999
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
#include "_windows/inttypes.h"		// For precise conversion between integer types. (New with C99)
#include "_windows/stdint.h"		// For defining various integer types. (New with C99)
#else
// #include <complex.h>		// A set of functions for manipulating complex numbers. (New with C99)
#include <fenv.h>			// For controlling floating-point environment. (New with C99)
#include <inttypes.h>		// For precise conversion between integer types. (New with C99)
#include <stdbool.h>		// For a boolean data type. (New with C99)
#include <stdint.h>			// For defining various integer types. (New with C99)
//#include <tgmath.h>		// For type-generic mathematical functions. (New with C99)
#endif



#ifdef __cplusplus
// C++ language standard, 1998 (updated in 2003)
#include <bitset>			// Provides the specialized container class std::bitset, a bit array.
#include <deque>			// Provides the container class template std::deque, a double-ended queue.
#include <list>				// Provides the container class template std::list, a doubly-linked list.
#include <map>				// Provides the container class templates std::map and std::multimap, an associative array and multimap.
#include <queue>			// Provides the container adapter class std::queue, a single-ended queue.
#include <set>				// Provides the container class templates std::set and std::multiset, sorted associative containers or sets.
#include <stack>			// Provides the container adapter class std::stack, a stack.
#include <vector>			// Provides the container class template std::vector, a dynamic array.
#include <algorithm>		// Provides definitions of many container algorithms.
#include <functional>		// Provides several function objects, designed for use with the standard algorithms.
#include <iterator>			// Provides classes and templates for working with iterators.
#include <locale>			// Provides classes and templates for working with locales.
#include <memory>			// Provides facilities for memory management in C++, including the class template std::auto_ptr.
//#include <stdexcept>		// Contains standard exception classes such as std::logic_error and std::runtime_error, both derived from std::exception.
#include <utility>			// Provides the template class std::pair, for working with pairs (two-member tuples) of objects.
#include <string>			// Provides the C++ standard string classes and templates.

// C++ streams are about the slowest way to do I/O.  Only used by school teachers who read the lame educational C++ textbooks.  Never used in profitable video games.
//#include <fstream>			// Provides facilities for file-based input and output. See fstream.
//#include <ios>				// Provides several types and functions basic to the operation of iostreams.
//#include <iostream>			// Provides C++ input and output fundamentals. See iostream.
//#include <iosfwd>             // Provides forward declarations of several I/O-related class templates.
//#include <iomanip>			// Provides facilities to manipulate output formatting, such as the base used when formatting integers and the precision of floating point values.
//#include <istream>			// Provides the template class std::istream and other supporting classes for input.
//#include <ostream>			// Provides the template class std::ostream and other supporting classes for output.
//#include <sstream>			// Provides the template class std::sstream and other supporting classes for string manipulation.
//#include <streambuf>

//#include <complex>		// Provides class template std::complex and associated functions for working with complex numbers.
#include <numeric>			// Provides algorithms for numerical processing
//#include <valarray>		// Provides the template class std::valarray, an array class optimized for numeric processing.
#include <limits>			// Provides the template class std::numeric_limits, used for describing properties of fundamental numeric types.
#include <new>				// Provides operators new and delete and other functions and types composing the fundamentals of C++ memory management.
#endif // #ifdef __cplusplus



// This would be the *correct* or *proper* replacement for broken wchar_t. 
// Unfortunatly no compiler distribution on any useful platform has this "standard" header file. 
// C language, ISO/IEC TR 19769, 2004
//#include <uchar.h>		// Defines character types char16_t, char32_t, and functions mbrtoc16(), c16rtomb(), mbrtoc32(), c32rtomb()


#ifdef __cplusplus
// No compiler distribution on any useful platform has this "standard" header file. 
// C++ language, ISO/IEC TR 19769 (WG14 N1040), 2006
//#include <cuchar>			// Defines character types char16_t, char32_t, and functions mbrtoc16(), c16rtomb(), mbrtoc32(), c32rtomb()
#endif // #ifdef __cplusplus



#ifdef __cplusplus
// C++ language standard, March 2011  a.k.a "0x" a.k.a "TR1" 
// Is it available in the free version of XCode using default settings?
// Is it available in the free version of VisualStudio using default settings?
// Is it compatible with all other "standard" header files which we are already #including?
// Can the whole team get work done with it?  Can we ship products on multiple platforms with it?
//#include <array>			// (New with C++ 2011)
//#include <ccomplex>		// (New with C++ 2011)
//#include <cfenv>			// (New with C++ 2011)
//#include <cinttypes>		// (New with C++ 2011)
//#include <complex.h>		// (New with C++ 2011)
//#include <cstdbool>		// (New with C++ 2011)
//#include <cstddef.h>		// (New with C++ 2011)
//#include <cstdint>        // (New with C++ 2011)
//#include <ctgmath>		// (New with C++ 2011)
//#include <fenv.h>			// (New with C++ 2011)
//#include <cinttypes.h>    // (New with C++ 2011)
//#include <random>			// (New with C++ 2011)
//#include <regex>			// (New with C++ 2011)
//#include <stdbool.h>		// (New with C++ 2011)
//#include <stdint.h>		// (New with C++ 2011)
//#include <tgmath.h>		// (New with C++ 2011)
//#include <tuple>			// (New with C++ 2011)
//#include <type_traits>	// (New with C++ 2011)
//#include <unordered_set>	// (New with C++ 2011)
//#include <unordered_map>	// (New with C++ 2011)
#endif  // #ifdef __cplusplus




#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
  #define stricmp _stricmp
#else
  #define stricmp strcasecmp 
  #define strnicmp strncasecmp 
#endif // #if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)


#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
#define isnan		_isnan
#define snprintf	_snprintf
#define atoll		_atoi64
#define vasprintf   _vasprintf
#endif  // #if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)


#ifdef __cplusplus
inline void __cstdlib_test_std_minmax()
{
	// before you #incude <windows.h>  
	// you need to  #define NOMINMAX  
	// otherwise windows.h will  #define min() and max()  

	int i0 = std::min( 0, 1 );
	assert( i0 == 0 );
	int i1 = std::max( 0, 1 );
	assert( i1 == 1 );
	float f0 = std::min( 0.0f, 1.0f );
	assert( f0 == 0.0f );
	float f1 = std::max( 0.0f, 1.0f );
	assert( f1 == 1.0f );
}
#endif  // #ifdef __cplusplus



#endif // #ifndef INCLUDED_CSTDLIB__H

