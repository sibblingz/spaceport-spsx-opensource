#ifndef INCLUDED_ENCODE_VORBIS__H
#define INCLUDED_ENCODE_VORBIS__H

#include <stdio.h>

// encode vorbis, write ogg file
//void EncodeFromPCM( const char* szOutputFilepath, const char* szInputFilepath, float fQuality  );
int EncodePCMBufferToOgg(signed char *pBuffer, int inputSize, int channels, int samplerate, int bytesPerSample, FILE *pOutputFile, float fQuality);
void EncodeFileToOgg(const char* szOutputFilepath, const char* szInputFilepath, float fQuality);


#endif  // #ifndef INCLUDED_ENCODE_VORBIS__H
