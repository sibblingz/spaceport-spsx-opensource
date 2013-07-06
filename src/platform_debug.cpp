/*
 *  platform_debug.cpp
 *  Created by Paul Miller 
 *  
 */

#include "cstdlib.h"
#include "platform_debug.h"

#if defined (DEBUG) || defined (_DEBUG) || defined (__DEBUG)


#if defined(__APPLE__)

    /*=============================================================================
        APPLE Technical Q&A QA1361
        Detecting the Debugger
        Q: How do I determine if I'm being run under the debugger?
        http://developer.apple.com/qa/qa2004/qa1361.html
    =============================================================================*/

    #include <assert.h>			// C runtime
    #include <stdbool.h>		// C runtime
    #include <sys/types.h>		// POSIX data types
    #include <unistd.h>			// POSIX functions and constants
    #include <sys/sysctl.h>		// BSD

    // Returns true if the current process is being debugged (either
    // running under the debugger or has a debugger attached post facto).
    bool AmIBeingDebugged( void )
    {
        int                 junk;
        int                 mib[4];
        struct kinfo_proc   info;
        size_t              size;

        // Initialize the flags so that, if sysctl fails for some bizarre
        // reason, we get a predictable result.
        info.kp_proc.p_flag = 0;

        // Initialize mib, which tells sysctl the info we want, in this case
        // we're looking for information about a specific process ID.
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = getpid();

        // Call sysctl.
        size = sizeof(info);
        junk = sysctl( mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0 );
        assert( junk == 0 );

        // We're being debugged if the P_TRACED flag is set.
        return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
    }


    /*=============================================================================
        APPLE TRACE macro
    =============================================================================*/

    void _trace_impl( const char* fmt, ... )
    {
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );			
    }


    /*=============================================================================
        APPLE AssertMacros.h
    =============================================================================*/

    void AssertMessageImpl( const char* szApplicationName, const char* szExpression, const char* szExceptionLabel,
        const char* szError, const char* szFileName, long nLineNumber, int nErrorCode )
    {
        if( (szExpression != NULL) && (*szExpression != '\0') )
            fprintf( stderr, "\n\nASSERT: Application: %s  Expression: %s\n", szApplicationName, szExpression );
        else
            fprintf( stderr, "\n\nCHECK: %s:\n", szApplicationName );

        if( szExceptionLabel != NULL && (*szExceptionLabel != '\0') )
            fprintf( stderr, "    exception: %s\n", szExceptionLabel );
        if( szError != NULL && (*szError != '\0') )
            fprintf( stderr, "    %s\n", szError );
        if( szFileName != NULL && (*szFileName != '\0') )
            fprintf( stderr, "    file: %s\n", szFileName );
        if( nLineNumber != 0 )
            fprintf( stderr, "    line: %ld\n", nLineNumber );
        if( nErrorCode != 0 )
            fprintf( stderr, "    error: %d\n", nErrorCode );

        fprintf( stderr, "    \n\n" );
    }





#elif defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)

    // Returns true if the current process is being debugged (either
    // running under the debugger or has a debugger attached post facto).
    bool AmIBeingDebugged( void )
    {
        return ::IsDebuggerPresent();
    }

    /*=============================================================================
        WIN32 TRACE() and ASSERT() macro
    =============================================================================*/

    void _trace_impl( const char* fmt, ... )
    {
        va_list args;
        va_start( args, fmt );
        char buffer[4096];
        vsprintf( buffer, fmt, args );
        OutputDebugStringA( buffer );
        va_end( args );			
    }

    void AssertMessageImpl( const char* szApplicationName, const char* szExpression, const char* szExceptionLabel,
        const char* szError, const char* szFileName, long nLineNumber, int nErrorCode )
    {
        if( (szExpression != NULL) && (*szExpression != '\0') )
            _trace_impl( "\n\nASSERT: Application: %s  Expression: %s\n", szApplicationName, szExpression );
        else
            _trace_impl( "\n\nCHECK: %s:\n", szApplicationName );

        if( szExceptionLabel != NULL && (*szExceptionLabel != '\0') )
            _trace_impl( "    exception: %s\n", szExceptionLabel );
        if( szError != NULL && (*szError != '\0') )
            _trace_impl( "    %s\n", szError );
        if( szFileName != NULL && (*szFileName != '\0') )
            _trace_impl( "    file: %s\n", szFileName );
        if( nLineNumber != 0 )
            _trace_impl( "    line: %ld\n", nLineNumber );
        if( nErrorCode != 0 )
            _trace_impl( "    error: %d\n", nErrorCode );

        _trace_impl( "    \n\n" );
    }



#else

    #error Unknown OS platform.  Dont know how to support debugging on this operating system.

    bool AmIBeingDebugged( void )
    {
        return false;
    }

    void _trace_impl( const char* fmt, ... )
    {
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );			
    }

    void AssertMessageImpl( const char* szApplicationName, const char* szExpression, const char* szExceptionLabel,
        const char* szError, const char* szFileName, long nLineNumber, int nErrorCode )
    {
        if( (szExpression != NULL) && (*szExpression != '\0') )
            _trace_impl( "\n\nASSERT: Application: %s  Expression: %s\n", szApplicationName, szExpression );
        else
            _trace_impl( "\n\nCHECK: %s:\n", szApplicationName );

        if( szExceptionLabel != NULL && (*szExceptionLabel != '\0') )
            _trace_impl( "    exception: %s\n", szExceptionLabel );
        if( szError != NULL && (*szError != '\0') )
            _trace_impl( "    %s\n", szError );
        if( szFileName != NULL && (*szFileName != '\0') )
            _trace_impl( "    file: %s\n", szFileName );
        if( nLineNumber != 0 )
            _trace_impl( "    line: %ld\n", nLineNumber );
        if( nErrorCode != 0 )
            _trace_impl( "    error: %d\n", nErrorCode );

        _trace_impl( "    \n\n" );
    }

#endif

#endif // #if defined (DEBUG) || defined (_DEBUG) || defined (__DEBUG)




// This can be called from the third-party open-source libs.  
extern "C"
int _printf_override_impl( const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	_trace_impl( fmt, args );
	va_end( args );	

	return 0;			
}


