/*
 *  spsx.cpp
 *  Created by Paul Miller
 *  
 */

#include "spsx.h"
#include "fileutil.h"
#include "decode.h"
#include "encode_vorbis.h"
#include "perftimer.h"


void ConvertFile( const char* szOutputFilepath, const char* szInputFilepath, float fQuality, bool verbose )
{
    ASSERT( szInputFilepath );
    ASSERT( szOutputFilepath );
    if( !szInputFilepath || !szOutputFilepath )
    {
        return;
    }

    // make temporary file name .wav
    char szTempDirectory[2048] = ".";
    memset( szTempDirectory, 0, sizeof(szTempDirectory) );
    GetTempDirectory( szTempDirectory, "spsx" );

    
    const char* szFilename = PathFindFileName( szInputFilepath );
    char szIntermediateFile[2048];
    strcpy( szIntermediateFile, szFilename );
    char* ext = (char*) PathFindFileExtension( szIntermediateFile );
    if( ext && *ext == '.' ) {
        strcpy( (ext + 1), "wav" );
    }
    
    char szIntermediateFilepath[2048];
    memset( szIntermediateFilepath, 0, sizeof(szIntermediateFilepath) );
    PathConcat( szIntermediateFilepath, szTempDirectory, szIntermediateFile );
    
    // decode the input file
	if(verbose)
		printf( "decoding: %s -> %s\n", szInputFilepath, szIntermediateFilepath );
    PerfTimer decodetimer;
    
	decodetimer.Start();
	
	//std::vector<unsigned char> decoded;
    DecodeToPCM( szIntermediateFilepath, szInputFilepath);

    decodetimer.Stop();
    
    // encode vorbis, write ogg file
	if(verbose)
		printf( "encoding: %s -> %s\n", szIntermediateFilepath, szOutputFilepath );
    PerfTimer encodetimer;
    encodetimer.Start();

#if 0
	FILE* pOutputFile = fopen( szOutputFilepath, "wb" );
    if( pOutputFile )
    {
		EncodePCMBufferToOgg((signed char *)&decoded[0], decoded.size(), 2, 44100, 4, pOutputFile, fQuality);
	}
#else
	EncodeFileToOgg(szOutputFilepath, szIntermediateFilepath, fQuality);
#endif
	
    encodetimer.Stop();
    
    float decodeSeconds = decodetimer.Elapsed();
    float encodeSeconds = encodetimer.Elapsed();
    
    uint32_t inputlength = GetFileLength( szInputFilepath );
    uint32_t intermediatelength = GetFileLength( szIntermediateFilepath );
    uint32_t outputlength = GetFileLength( szOutputFilepath );
    
	if(verbose)
	{
		printf( "\n" );
		
		printf( "%s  size = %d  decodingtime = %f seconds\n", szInputFilepath, inputlength, decodeSeconds );
		printf( "%s  size = %d  \n", szIntermediateFilepath, intermediatelength );
		printf( "%s  size = %d  encodingtime = %f seconds\n", szOutputFilepath, outputlength, encodeSeconds );
		
		printf( "done\n" );
	}

    // delete the temporary file (or the user's hard drive will be full of uncompressed wav files)
    //DeleteFile( szIntermediateFilepath );

    return;
}
