/*
 *  riff.cpp
 *  
 *  
 */

#include "riff.h"
#include "cstdlib.h"
#include "platform_debug.h"
#include "fileutil.h"


int32_t BufferCheckFourCC( uint8_t*& pos, const char* fourCC )
{
	int result = strncmp( (const char*)pos, fourCC, 4 );
	(pos) += 4;
	return result;
}

int32_t BufferReadInt32( uint8_t*& pos )
{
	int32_t result = 0;
	memcpy( &result, pos, 4 );
	(pos) += 4;
	return result;	
}

int16_t BufferReadInt16( uint8_t*& pos )
{
	int16_t result = 0;
	memcpy( &result, pos, 2 );
	(pos) += 2;
	return result;	
}

uint8_t* BufferReadStruct( uint8_t* pStruct, uint32_t bytesize, uint8_t*& pos )
{
	memcpy( pStruct, pos, bytesize );
	pos += bytesize;
	return pStruct;	
}


void BufferSkipNBytes( uint8_t*& pos, uint32_t n )
{
	(pos) += n;
}


// Read 12 bytes.
// Check "RIFF" fourCC, skip the size, then check "WAVE" fourCC.
// RETURNS: the read position after the "WAVE" fourCC. Returns NULL if its not a .wav file.
uint8_t* IsWaveFile( uint8_t* pReadPosition, uint32_t buffersize )
{
    if( 0 != BufferCheckFourCC( pReadPosition, "RIFF" ) )
    {	
        //TRACE( "IsWaveFile()  %.4s  %s\n", pReadPosition-4, "Not a .wav file" );
        return NULL;	
    }
    else
    {
        //int32_t chunksize = 
        BufferReadInt32( pReadPosition );
        if( 0 != BufferCheckFourCC( pReadPosition, "WAVE" ) )
        {	
            //TRACE( "IsWaveFile()  %.4s  %s\n", pReadPosition-4, "Not a .wav file" );	
            return NULL;	
        }
    }
    return pReadPosition;
}


// Search and find the "fmt " chunk. Skip chunks that are not "fmt ".  Read the WavFormat structure.
// RETURNS: the read position after the "fmt " chunk.
uint8_t* ReadFormatFromBuffer( WavFormat& result, uint8_t* pReadPosition, uint32_t buffersize )
{
    char chunkName[5];
    memset(chunkName, 0, 5);

    // Skip chunks that are not the "fmt " chunck
    while( 0 != BufferCheckFourCC( pReadPosition, "fmt " ) )
    {	
        strncpy( chunkName, (const char*)(pReadPosition-4), 4 );
        int32_t chunksize = BufferReadInt32( pReadPosition );
        pReadPosition += chunksize; // skip this whole chunk

        //TRACE( "ReadFormatFromBuffer()  skipping extra chunk %s %d\n", chunkName, chunksize ); 
        //result = -3;
        //TRACE( "ReadFormatFromBuffer() 3  %.4s  %s: %s\n", pReadPosition-4, "Not a .wav file",  inFilePath );	
    }

    int32_t chunksizeFmt = BufferReadInt32( pReadPosition );
    uint8_t* pFmtChunk = pReadPosition;	
    
    BufferReadStruct( (uint8_t*) &result, sizeof(WavFormat), pReadPosition );
    
//    TRACE( "ReadFormatFromBuffer() format=%d  channel=%d  sampleRate=%d  byteRate=%d  bitsPerSample=%d   chunksizeFmt=%d \n", 
//        result.wFormatTag, result.nChannels, result.nSamplesPerSec, result.nAvgBytesPerSec, result.wBitsPerSample,  chunksizeFmt );
        
    pReadPosition = pFmtChunk + chunksizeFmt;
    return pReadPosition;   
}

// Search and find the "data" chunk. Skip chunks that are not "data".  
// RETURNS: the read position pointing to the "data" fourCC.  Returns NULL if "data" fourCC could not be found
uint8_t* SeekDataChunk( uint8_t* pReadPosition, uint32_t buffersize )
{
    char chunkName[5];
    memset(chunkName, 0, 5);

    // search for Data Chunk
    while( 0 != BufferCheckFourCC( pReadPosition, "data") )
    {	
        strncpy( chunkName, (const char*)(pReadPosition-4), 4);
        int32_t chunksize = BufferReadInt32( pReadPosition );
        pReadPosition += chunksize; // skip this whole chunk

        //TRACE( "SeekDataChunk()  skipping extra chunk %s %d\n", chunkName, chunksize); 
        //result = -4;
        //TRACE( "SeekDataChunk() %.4s  %s: %s\n", pReadPosition-4, "no data chunk in .wav file", inFilePath );	
    }

    // Size of Sample Data	
    //int32_t dataChunkSize = BufferReadInt32( pReadPosition );

    pReadPosition = pReadPosition-4;
    if( 0 == strncmp( (const char*)pReadPosition, "data", 4 ) )
    {
        return pReadPosition;
    }
    return NULL;
}

// OUTPUT: pointer and size of wave data
void GetWaveData( uint8_t*& result, uint32_t& resultsize, uint8_t* pDataChunkFourCC, uint8_t* pFileBuffer, uint32_t filesize )
{
    uint8_t* pReadPosition = pDataChunkFourCC;
    if( 0 == BufferCheckFourCC( pReadPosition, "data" ) )
    {
        uint32_t dataChunkSize = BufferReadInt32( pReadPosition );
    
        //int32_t fileheaderSize = pReadPosition - pFileBuffer;
        
        result = pReadPosition;
        resultsize = dataChunkSize;
    }
}
