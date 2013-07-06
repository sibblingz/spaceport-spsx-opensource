/*
 *  fileutil.h
 *  Created by Paul Miller
 *  
 */

#ifndef INCLUDED_FILEUTIL__H
#define INCLUDED_FILEUTIL__H

#include "cstdtypes.h"





// Check if a directory exists.
bool DirectoryExists( const char* szDirectoryPath );

// Check if file exists.  (this may open the file using fopen()) 
bool FileExists( const char* szFilepath );

// Check if file exists using stat().  (DOES NOT use fopen() or open())   
bool FileExistsStat( const char* szFilepath );

// get file length, without opening the file
uint32_t GetFileLength( const char* szFilePath );

// Delete the file from the file system.
void DeleteFile( const char* szFilepath );


// Load a whole file, allocates the buffer for you.
bool LoadWholeFile( uint8_t** ppBuffer, uint32_t& length, const char* szFilename );

// Delete a buffer that was allocated by LoadWholeFile().
void DeleteBufferWholeFile( uint8_t** ppBuffer );

// Save a whole file. You manage the buffer.
bool SaveWholeFile( uint8_t* pBuffer, uint32_t size, const char* szFilename );


// Add the path separated to the end of the string.
void PathAddSeparator( char* szPath );

// Concatonate file paths.
void PathConcat( char* szPathResult, const char* szPath1, const char* szPath2 );

// CONVERT "wacky/path/with/slashes.png" TO "wacky_path_with_slashes.png"
// EXAMPLE: PathReplaceChar( szLocalFilename, '/', '_' );
void PathReplaceChar( char* szPath, char find, char replace );

// Scan backwards from end of string, find the '.'
// RETURNS: pointer to the '.' if it occurs, or NULL if a slash is encountered. 
const char* PathFindFileExtension( const char* szPath );

// Scan backwards from end of string, find the delimiter for the file name (either '/' or '\')
// RETURNS: pointer to the character after delimiter if it occurs, or NULL if scanning reaches the beginning of the string. 
const char* PathFindFileName( const char* szPath );


// Get a directory path for writing temporary files
void GetTempDirectory( char* szResult, const char* szUniqueToken );


#endif  // #ifndef INCLUDED_FILEUTIL__H
