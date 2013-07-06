/*
 *  platform_debug.h
 *  Created by Paul Miller 
 *  
 */

#ifndef INCLUDED_PLATFORM_DEBUG__H
#define INCLUDED_PLATFORM_DEBUG__H


// ASSERT() should stop execution on the line of code where an error is detected. 
// The standard assert() doesn't do this on all platforms.

#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
    #define _DEBUG_BREAK() 	__asm { int 3 }         
#elif defined(__APPLE__)
    #define _DEBUG_BREAK() 	asm("int $3")   // __asm { int 3 }    // __asm__ int 3        
#else
    #define _DEBUG_BREAK() 	do{  int* __pNULL = 0;  *__pNULL = 0;  } while(0)       
#endif


#ifndef DEBUG_ASSERT_APPLICATION_NAME
    #define DEBUG_ASSERT_APPLICATION_NAME "spsx"
#endif


#if defined (DEBUG) || defined (_DEBUG) || defined (__DEBUG)
 
    // Debug Build

	extern bool AmIBeingDebugged(void);

	extern void AssertMessageImpl( const char* szApplicationName, const char* szExpression, const char* szExceptionLabel, 
		const char* szError, const char* szFileName, long nLineNumber, int nErrorCode );

	#define _DEBUG_ASSERT_MESSAGE( _APPNAME, _EXPR, _EXCEPT, _ERRSTR, _FILE, _LINE, _ERRCODE )	\
		AssertMessageImpl( _APPNAME, _EXPR, _EXCEPT, _ERRSTR, _FILE, _LINE, _ERRCODE );			\
		if( AmIBeingDebugged() ) {		\
			_DEBUG_BREAK();				\
		}
		 
		
	#define _ASSERT_IMPL( _EXPRESSION, _ERRSTR )                              \
		do                                                                    \
		{                                                                     \
		  if( !(_EXPRESSION) )												  \
		  {                                                                   \
			  _DEBUG_ASSERT_MESSAGE(                                          \
				  DEBUG_ASSERT_APPLICATION_NAME,							  \
				  #_EXPRESSION,                                               \
				  0,                                                          \
				  0,                                                          \
				  __FILE__,                                                   \
				  __LINE__,                                                   \
				  0 );                                                        \
		  }                                                                   \
		} while ( 0 )


	void _trace_impl( const char* fmt, ... );

	#define _TRACE_IMPL   _trace_impl   
	
#else

    // Release Build

	inline bool AmIBeingDebugged(void) { return false; }

	inline void AssertMessageImpl( const char* szApplicationName, const char* szExpression, const char* szExceptionLabel, 
		const char* szError, const char* szFileName, long nLineNumber, int nErrorCode ) { }

	#define _DEBUG_ASSERT_MESSAGE( _APPNAME, _EXPRESSION, _EXCEPTION, _ERRORSTRING, _FILE, _LINE, _ERRORCODE )							 

	#define _ASSERT_IMPL( _EXPRESSION, _ERRSTR )
   

	inline void _trace_impl( const char* fmt, ... ) { }
    
	#define _TRACE_IMPL   _trace_impl   
 
#endif 
      




/*=============================================================================
	use this for development
=============================================================================*/

// Compile Time Assert - if constant expression is FALSE, compiler generates an error
#define CT_ASSERT( _EXPR )				do{ typedef int ct_assert##__LINE__ [(_EXPR)? 1 : -1]; } while(0)

// Run Time Assert - if variable expression is FALSE, execution stops.
#define ASSERT( _EXPR )					_ASSERT_IMPL( (_EXPR), (0) )    

#define ASSERT_NULL( _EXPR )			_ASSERT_IMPL( NULL == (_EXPR), (0) )

// With an error string.
#define ASSERTE( _EXPR, _ERRSTR )		_ASSERT_IMPL( (_EXPR), (_ERRSTR) )  


// Trace
#define TRACE							_TRACE_IMPL




#endif  // INCLUDED_PLATFORM_DEBUG__H

