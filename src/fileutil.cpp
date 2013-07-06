/*
 *  fileutil.cpp
 *  Created by Paul Miller
 *  
 */

#include "fileutil.h"
#include "cstdlib.h"

#if defined(__APPLE__)
    #include <sys/stat.h>			// UNIX file status  stat(), lstat(), fstat()
    #include <utime.h>
    #include <unistd.h> 
    
#elif defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
    #include <sys/stat.h>			// file status  stat(), lstat(), fstat() 
    #include <sys/utime.h>			// _utime()	
    #include <direct.h>				// _mkdir()  _rmdir()
	#include <io.h>

	#define stat _stat64

#else

#endif 


// Check if a directory exists.
bool DirectoryExists( const char* szDirectoryPath )
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
	struct _stat64 filestatus;
	int result = stat( szDirectoryPath, &filestatus );	
	if( result == 0 )
	{	
		if( (filestatus.st_mode & _S_IFDIR) )
		{
			return true;
		}
	}
#else
	struct stat filestatus;
	int result = stat( szDirectoryPath, &filestatus );	
	if( result == 0 )
	{	
		if( S_ISDIR( filestatus.st_mode ) )
		{
			return true;
		}
	}
#endif    
	return false;
}

// Check if file exists.  (this may open the file using fopen()) 
bool FileExists( const char* szFilepath )
{
	// open the file
	FILE* pFile = fopen( szFilepath, "rb" );
	if( pFile )
	{
		// close the file
		fclose( pFile );	
		return true;
	}
	return false;	
}

// Check if file exists using stat().  (DOES NOT use fopen() or open())   
bool FileExistsStat( const char* szFilepath )
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
	struct _stat64 filestatus;
	int result = stat( szFilepath, &filestatus );	
#else
	struct stat filestatus;
	int result = stat( szFilepath, &filestatus );	
#endif
	if( result == 0 )
	{	
        return true;
	}    
	return false;
}

// get file length, without opening the file
uint32_t GetFileLength( const char* szFilePath )
{
	uint32_t length = 0;
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
	struct _stat64 filestatus;
	int result = stat( szFilePath, &filestatus );
	if( result == 0 )
	{	
		if( !(filestatus.st_mode & _S_IFDIR) )
		{
			length = (uint32_t) filestatus.st_size;
		}
	}    
#else
	struct stat filestatus;
	int result = lstat( szFilePath, &filestatus );	
	if( result == 0 )
	{	
		if( !S_ISDIR( filestatus.st_mode ) )
		{
			length = (uint32_t) filestatus.st_size;
		}
	}
#endif    	
	else
	{
		length = 0;
	}
	return length;
}

// Delete the file from the file system.
void DeleteFile( const char* szFilepath )
{  
	remove( szFilepath ); 
	//unlink( szFilepath );
}



// Load a whole file, allocates the buffer for you.
bool LoadWholeFile( uint8_t** ppBuffer, uint32_t& length, const char* szFilename )
{
	// open the file
    length = 0;
    *ppBuffer = NULL;
    
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
	struct _stat64 filestatus;
	int result = stat( szFilename, &filestatus );	
    if( (filestatus.st_mode & _S_IFDIR) )
        return false;
#else    
	struct stat filestatus;
	//int result = 
    stat( szFilename, &filestatus );	
    if( S_ISDIR( filestatus.st_mode ) )
        return false;
#endif

	FILE* pFile = fopen( szFilename, "rb" );
	if(!pFile)
        return false;	

    // get the size of the file
    fseek( pFile, 0, SEEK_END );
    length = ftell( pFile );
    fseek( pFile, 0, SEEK_SET );

    // since text files require zero-terminators, just add them to everything.
    *ppBuffer = new uint8_t[ length+1 ];
    (*ppBuffer)[length] = 0;
    fread( *ppBuffer, length, 1 , pFile );

    // close the file
    fclose( pFile );
    return true;
} 

// Delete a buffer that was allocated by LoadWholeFile().
void DeleteBufferWholeFile( uint8_t** ppBuffer )
{
	if( *ppBuffer )
	{
		delete[] ( *ppBuffer );
	}
	*ppBuffer = NULL;
}

// Save a whole file. You manage the buffer.
bool SaveWholeFile( uint8_t* pBuffer, uint32_t size, const char* szFilename )
{
	FILE* pFile = fopen( szFilename, "wb" );
	//ASSERT( pFile );
	if( pFile )
	{
		fwrite( pBuffer, size, 1 , pFile );

		if(ferror(pFile))
			perror("");

		// close the file
		fclose( pFile );
		
		return true;
	}
	return false;
}


static const char szPathSeparator[] = { '/', '\0' };

// Add the path separated to the end of the string.
void PathAddSeparator( char* szPath )
{
	char* p = szPath;
	while( *p != 0 )
		p++;
	p--;
	if( *p != '/' )
	{
		//strcat( szPath, szPathSeparator );	
		p++;
		*p = szPathSeparator[0];
		p++;
		*p = 0;
	}	
}
        
// Concatonate file paths.
void PathConcat( char* szPathResult, const char* szPath1, const char* szPath2 )
{	
	strcat( szPathResult, szPath1 );
	PathAddSeparator( szPathResult );
	strcat( szPathResult, szPath2 );	
} 

// CONVERT "wacky/path/with/slashes.png" TO "wacky_path_with_slashes.png"
// EXAMPLE: PathReplaceChar( szLocalFilename, '/', '_' );
void PathReplaceChar( char* szPath, char find, char replace )
{
	char* p = szPath;
	while( *p != 0 )
	{
		if( *p == find )
			*p = replace;
		p++;	
	}
}

// Scan backwards from end of string, find the '.'
// RETURNS: pointer to the '.' if it occurs, or NULL if a slash is encountered. 
const char* PathFindFileExtension( const char* szPath )
{
	int32_t len = strlen( szPath );
	const char* p = &szPath[ len-1 ];
	while( *p != 0 && *p != '/' && *p != '\\' )
	{
		if( *p == '.' )
			return p;
		p--;
	}
	return NULL;
}

// Scan backwards from end of string, find the delimiter for the file name (either '/' or '\')
// RETURNS: pointer to the character after delimiter if it occurs, or NULL if scanning reaches the beginning of the string. 
const char* PathFindFileName( const char* szPath )
{
	int32_t len = strlen( szPath );
    if(szPath[len - 1] == '/' || szPath[len - 1] == '\\')
        return NULL;
	const char* p = &szPath[ len-2 ];
	while( *p != 0 && p > szPath )
	{
		if( *p == '/' || *p == '\\' )
			return p + 1;
		p--;
	}
	return NULL;
}


// Get a directory path for writing temporary files
void GetTempDirectory( char* szResult, const char* szUniqueToken )
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_MSC_VER)

	char tempPath[MAX_PATH+1];
	int retVal = GetTempPathA(MAX_PATH, tempPath);
	strcpy(szResult, tempPath);

	/*

	if(retVal != 0)
	{
		std::string path = tempPath;

		char buffer[2048];
		//memset( buffer, 0, sizeof(buffer) );
		//sprintf( buffer, "%s-XXXXXX", szUniqueToken );

		path += szUniqueToken;
		path += "-XXXXXX";

		strcpy(buffer, path.c_str());

		if(_mktemp_s( buffer, sizeof(buffer)) == 0)
		{
			//strcpy_s(name, sizeof(name), template);
			int rez = mkdir(buffer);
			if (rez == 0) 
			{
				strcpy(szResult, buffer);
			}
		}
	}
	*/

#else
    // char* mkdtemp(char* template);
    // BSD:  http://www.freebsd.org/cgi/man.cgi?query=mkdtemp&apropos=0&sektion=0&manpath=FreeBSD+9.0-RELEASE&arch=default&format=html
    // Linux: http://linux.die.net/man/3/mkdtemp
    
    char buffer[2048];
    memset( buffer, 0, sizeof(buffer) );
    sprintf( buffer, "/tmp/%s-XXXXXX", szUniqueToken ); 
    
    char* path = mkdtemp( buffer );
    if( path != NULL ) {
        strcpy( szResult, path );
    }
#endif
}

