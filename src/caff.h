/*
 *  caff.h
 * 
 *  
 */

#ifndef INCLUDED_CAFF__H
#define INCLUDED_CAFF__H

#include "cstdtypes.h"


struct CoreAudioFormat {
    double mSampleRate;
    uint32_t mFormatID;
    uint32_t mFormatFlags;
    uint32_t mBytesPerPacket;
    uint32_t mFramesPerPacket;
    uint32_t mChannelsPerFrame;
    uint32_t mBitsPerChannel;
};


// Read 8 bytes
// Check "caff" fourCC, skip the version and flags
// RETURNS: the read position after the "WAVE" fourCC. Returns NULL if its not a .wav file.
uint8_t* IsCoreAudioFile( uint8_t* pReadPosition, uint32_t buffersize );

// Search and find the "desc" chunk. Skip chunks that are not "desc".  Read the CoreAudioFormat structure.
// RETURNS: the read position after the "desc" chunk.
uint8_t* ReadFormatFromBuffer( CoreAudioFormat& result, uint8_t* pReadPosition, uint32_t buffersize );

// Search and find the "data" chunk. Skip chunks that are not "data".  
// RETURNS: the read position pointing to the "data" fourCC.  Returns NULL if "data" fourCC could not be found
uint8_t* SeekDataChunkCA( uint8_t* pReadPosition, uint32_t buffersize );

// Get the data from the data chunk.
// OUTPUT: pointer and size of lpcm data
void GetPCMData( uint8_t*& result, uint32_t& resultsize, uint8_t* pDataChunkFourCC, uint8_t* pFileBuffer, uint32_t filesize );


// Helpers
int64_t BufferReadInt64( uint8_t*& pos );



#define ENDIAN_FLIP16( VAL_ )	( (VAL_)<<8 | (VAL_)>>8 ) 

#define ENDIAN_FLIP32( VAL_ )	(  (((VAL_)& 0x000000FF) << 24) | (((VAL_)& 0x0000FF00) << 8) | (((VAL_)& 0x00FF0000) >> 8)  | (((VAL_)& 0xFF000000) >> 24) ) 

#define ENDIAN_FLIP64( VAL_ )	(                 \
    (( ((uint64_t)VAL_) & 0x00000000000000FFULL) << 56) |       \
    (( ((uint64_t)VAL_) & 0x000000000000FF00ULL) << 40) |       \
    (( ((uint64_t)VAL_) & 0x0000000000FF0000ULL) << 24) |       \
    (( ((uint64_t)VAL_) & 0x00000000FF000000ULL) <<  8) |       \
    (( ((uint64_t)VAL_) & 0x000000FF00000000ULL) >>  8) |       \
    (( ((uint64_t)VAL_) & 0x0000FF0000000000ULL) >> 24) |       \
    (( ((uint64_t)VAL_) & 0x00FF000000000000ULL) >> 40) |       \
    (( ((uint64_t)VAL_) & 0xFF00000000000000ULL) >> 56) )       \


inline double ENDIAN_FLIP_DOUBLE( double VAL_ )       
{   
    uint64_t resultval = ENDIAN_FLIP64( *((uint64_t*) &(VAL_)) );
    return *( (double*) (&resultval) );        
}



#endif  // #ifndef INCLUDED_CAFF__H
