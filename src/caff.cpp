/*
 *  caff.cpp
 *  
 *  
 */

#include "caff.h"
#include "cstdlib.h"
#include "platform_debug.h"
#include "fileutil.h"
#include "riff.h"



int64_t BufferReadInt64( uint8_t*& pos )
{
	int64_t result = 0;
	memcpy( &result, pos, 8 );
	(pos) += 8;
	return result;	
}



// Read 8 bytes
// Check "caff" fourCC, skip the version and flags
// RETURNS: the read position after the "WAVE" fourCC. Returns NULL if its not a .wav file.
uint8_t* IsCoreAudioFile( uint8_t* pReadPosition, uint32_t buffersize )
{
    if( 0 != BufferCheckFourCC( pReadPosition, "caff" ) )
    {	
        //TRACE( "IsCoreAudioFile()  %.4s  %s\n", pReadPosition-4, "Not a .wav file" );
        return NULL;	
    }
    else
    {
        //int16_t version = 
        BufferReadInt16( pReadPosition );
        // version = ENDIAN_FLIP16( version );
        
        //int16_t flags = 
        BufferReadInt16( pReadPosition );
        // flags = ENDIAN_FLIP16( flags );
    }
    return pReadPosition;
}

// Search and find the "desc" chunk. Skip chunks that are not "desc".  Read the CoreAudioFormat structure.
// RETURNS: the read position after the "desc" chunk.
uint8_t* ReadFormatFromBuffer( CoreAudioFormat& result, uint8_t* pReadPosition, uint32_t buffersize )
{
    char chunkName[5];
    memset(chunkName, 0, 5);

    // Skip chunks that are not the "desc" chunck
    while( 0 != BufferCheckFourCC( pReadPosition, "desc" ) )
    {	
        strncpy( chunkName, (const char*)(pReadPosition-4), 4 );
        int64_t chunksize = BufferReadInt64( pReadPosition );
        chunksize = ENDIAN_FLIP64( chunksize );
        pReadPosition += chunksize; // skip this whole chunk

        //TRACE( "ReadFormatFromBuffer()  skipping extra chunk %s %d\n", chunkName, chunksize ); 
        //result = -3;
        //TRACE( "ReadFormatFromBuffer() 3  %.4s  %s: %s\n", pReadPosition-4, "Not a .wav file",  inFilePath );	
    }

    int64_t chunksizeFmt = BufferReadInt64( pReadPosition );
    chunksizeFmt = ENDIAN_FLIP64( chunksizeFmt );
    uint8_t* pFmtChunk = pReadPosition;	
    
    BufferReadStruct( (uint8_t*) &result, sizeof(CoreAudioFormat), pReadPosition );
    result.mSampleRate          = ENDIAN_FLIP_DOUBLE( result.mSampleRate );
    //result.mFormatID            = ENDIAN_FLIP32( result.mFormatID );
    result.mFormatFlags         = ENDIAN_FLIP32( result.mFormatFlags );
    result.mBytesPerPacket      = ENDIAN_FLIP32( result.mBytesPerPacket );
    result.mFramesPerPacket     = ENDIAN_FLIP32( result.mFramesPerPacket );
    result.mChannelsPerFrame    = ENDIAN_FLIP32( result.mChannelsPerFrame );
    result.mBitsPerChannel      = ENDIAN_FLIP32( result.mBitsPerChannel );
    
    const char* pFormatId = (char*) &result.mFormatID;
    float sampleRate = result.mSampleRate;
//    TRACE( "ReadFormatFromBuffer() format=%.4s  channels=%d  sampleRate=%f   mBitsPerChannel=%d   chunksizeFmt=%d \n", 
//        pFormatId, result.mChannelsPerFrame, sampleRate, result.mBitsPerChannel,  chunksizeFmt );
        
    pReadPosition = pFmtChunk + chunksizeFmt;
    return pReadPosition;   

}

// Search and find the "data" chunk. Skip chunks that are not "data".  
// RETURNS: the read position pointing to the "data" fourCC.  Returns NULL if "data" fourCC could not be found
uint8_t* SeekDataChunkCA( uint8_t* pReadPosition, uint32_t buffersize )
{
    return SeekDataChunk( pReadPosition, buffersize );
}

// Get the data from the data chunk.
// OUTPUT: pointer and size of lpcm data
void GetPCMData( uint8_t*& result, uint32_t& resultsize, uint8_t* pDataChunkFourCC, uint8_t* pFileBuffer, uint32_t filesize )
{
    uint8_t* pReadPosition = pDataChunkFourCC;
    if( 0 == BufferCheckFourCC( pReadPosition, "data" ) )
    {
        int64_t dataChunkSize = BufferReadInt64( pReadPosition );
        dataChunkSize = ENDIAN_FLIP64( dataChunkSize );
        
        //int32_t fileheaderSize = pReadPosition - pFileBuffer;
        
        result = pReadPosition;
        resultsize = dataChunkSize;
    }
}

