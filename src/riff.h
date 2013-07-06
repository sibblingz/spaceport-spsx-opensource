/*
 *  riff.h
 *  
 *  
 */

#ifndef INCLUDED_RIFF__H
#define INCLUDED_RIFF__H

#include "cstdtypes.h"


struct WavFormat
{
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
};


// Read 12 bytes.
// Check "RIFF" fourCC, skip the size, then check "WAVE" fourCC.
// RETURNS: the read position after the "WAVE" fourCC. Returns NULL if its not a .wav file.
uint8_t* IsWaveFile( uint8_t* pReadPosition, uint32_t buffersize );

// Search and find the "fmt " chunk. Skip chunks that are not "fmt ".  Read the WavFormat structure.
// RETURNS: the read position after the "fmt " chunk.
uint8_t* ReadFormatFromBuffer( WavFormat& result, uint8_t* pReadPosition, uint32_t buffersize );

// Search and find the "data" chunk. Skip chunks that are not "data".  
// RETURNS: the read position pointing to the "data" fourCC.  Returns NULL if "data" fourCC could not be found
uint8_t* SeekDataChunk( uint8_t* pReadPosition, uint32_t buffersize );

// Get the data from the data chunk.
// OUTPUT: pointer and size of wave data
void GetWaveData( uint8_t*& result, uint32_t& resultsize, uint8_t* pDataChunkFourCC, uint8_t* pFileBuffer, uint32_t filesize );



// Helper functions
int32_t BufferCheckFourCC( uint8_t*& pos, const char* fourCC );
int32_t BufferReadInt32( uint8_t*& pos );
int16_t BufferReadInt16( uint8_t*& pos );
uint8_t* BufferReadStruct( uint8_t* pStruct, uint32_t bytesize, uint8_t*& pos );
void BufferSkipNBytes( uint8_t*& pos, uint32_t n );


#endif  // #ifndef INCLUDED_RIFF__H
