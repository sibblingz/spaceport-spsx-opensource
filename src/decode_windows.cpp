/*
 *  decode_windows.cpp
 *  Created by Paul Miller
 *  
 */

#include "stdafx.h"

#include "decode.h"
#include "platform_debug.h"
#include "fileutil.h"

#include <stdio.h>
#include <MMReg.h>
#include <MMSystem.h>
#include <MSAcm.h>
#include <wmsdk.h>

HRESULT OpenFromMemory(BYTE* mp3InputBuffer, DWORD mp3InputBufferSize, const char *outputFilename);

void DecodeToPCM( const char* szOutputFilepath, const char* szInputFilepath)
{
	uint8_t *pBuffer;
	unsigned int size;
	LoadWholeFile(&pBuffer, size, szInputFilepath);
	//printf("buffer size %d\n", size);

	OpenFromMemory(pBuffer, size, szOutputFilepath);
}

#pragma comment(lib, "msacm32.lib") 
#pragma comment(lib, "wmvcore.lib") 
#pragma comment(lib, "winmm.lib") 
#pragma intrinsic(memset,memcpy,memcmp)



struct WAVE_FORMAT
{
	WAVE_FORMAT()
	{
		wFormatTag = 1;
		SetFormat(2, 44100, 16);
	}

	void SetFormat(int numChannels, int sampleRate, int bitsPerSample)
	{
		wChannels = numChannels;
		dwSamplesPerSec = sampleRate;
		wBitsPerSample = bitsPerSample;

		dwAvgBytesPerSec = wChannels * (wBitsPerSample/8) * dwSamplesPerSec;
		wBlockAlign = wChannels * (wBitsPerSample/8);
	}

	WORD	wFormatTag;
	WORD	wChannels;
	DWORD	dwSamplesPerSec;
	DWORD	dwAvgBytesPerSec;
	WORD	wBlockAlign;
	WORD	wBitsPerSample;
};

struct RIFF_HEADER
{
	RIFF_HEADER()
	{
		szRiffID[0] = 'R';
		szRiffID[1] = 'I';
		szRiffID[2] = 'F';
		szRiffID[3] = 'F';

		szRiffFormat[0] = 'W';
		szRiffFormat[1] = 'A';
		szRiffFormat[2] = 'V';
		szRiffFormat[3] = 'E';

		dwRiffSize = 0;//sizeof(RIFF_HEADER);
	}

	char	szRiffID[4];		// 'R','I','F','F'
	DWORD	dwRiffSize;
	char	szRiffFormat[4];	// 'W','A','V','E'
};

struct FMT_BLOCK
{
	FMT_BLOCK()
	{
		szFmtID[0] = 'f';
		szFmtID[1] = 'm';
		szFmtID[2] = 't';
		szFmtID[3] = ' ';

		dwFmtSize = sizeof(WAVE_FORMAT);
	}
	char		szFmtID[4];	// 'f','m','t',' '
	DWORD		dwFmtSize;
	WAVE_FORMAT	wavFormat;
};

struct DATA_BLOCK
{
	DATA_BLOCK()
	{
		szDataID[0] = 'd';
		szDataID[1] = 'a';
		szDataID[2] = 't';
		szDataID[3] = 'a';

		dwDataSize = 0;//dataSize;
	}

	char	szDataID[4];	// 'd','a','t','a'
	DWORD	dwDataSize;
};


struct WAVE_HEADER
{
	WAVE_HEADER()
	{
		//riffHeader.dwRiffSize = dataSize + 36;
	}

	void SetSize(unsigned long size)
	{
		riffHeader.dwRiffSize = size + 36;
		dataBlock.dwDataSize = size;
	}

	RIFF_HEADER riffHeader;
	FMT_BLOCK fmtBlock;
	DATA_BLOCK dataBlock;
};



HRESULT OpenFromMemory(BYTE* mp3InputBuffer, DWORD mp3InputBufferSize, const char *outputFilename)
{
	DWORD bufferLength;
	double durationInSecond;
	BYTE* soundBuffer;

	IWMSyncReader* wmSyncReader;
	IWMHeaderInfo* wmHeaderInfo;
	IWMProfile* wmProfile;
	IWMStreamConfig* wmStreamConfig;
	IWMMediaProps* wmMediaProperties;
	WORD wmStreamNum = 0;
	WMT_ATTR_DATATYPE wmAttrDataType;
	DWORD durationInSecondInt;
	QWORD durationInNano;
	DWORD sizeMediaType;
	DWORD maxFormatSize = 0;
	HACMSTREAM acmMp3stream = NULL;
	HGLOBAL mp3HGlobal;
	IStream* mp3Stream;
 
	// Initialize COM
	CoInitialize(0);
 
	HRESULT res;
	res = WMCreateSyncReader(  NULL, WMT_RIGHT_PLAYBACK , &wmSyncReader );
	ASSERT(res == S_OK);
 
	// Alloc With global and create IStream
	mp3HGlobal = GlobalAlloc(GPTR, mp3InputBufferSize);
	ASSERT(mp3HGlobal != 0);
	void* mp3HGlobalBuffer = GlobalLock(mp3HGlobal);
	memcpy(mp3HGlobalBuffer, mp3InputBuffer, mp3InputBufferSize);
	GlobalUnlock(mp3HGlobal);
	res = CreateStreamOnHGlobal(mp3HGlobal, FALSE, &mp3Stream); ASSERT(res == S_OK);

	// Open MP3 Stream
	res = wmSyncReader->OpenStream(mp3Stream); ASSERT(res == S_OK);
 
	// Get HeaderInfo interface
	res = wmSyncReader->QueryInterface(&wmHeaderInfo); ASSERT(res == S_OK);
 
	// Retrieve mp3 song duration in seconds
	WORD lengthDataType = sizeof(QWORD);
	res = wmHeaderInfo->GetAttributeByName(&wmStreamNum, L"Duration", &wmAttrDataType, (BYTE*)&durationInNano, &lengthDataType ); ASSERT(res == S_OK);
	durationInSecond = ((double)durationInNano)/10000000.0;
	durationInSecondInt = (int)(durationInNano/10000000)+1;

	//res = wmHeaderInfo->GetAttributeByName(&wmStreamNum, L"WM/MediaIsStereo", &wmAttrDataType, (BYTE*)&isStereo, &lengthDataType ); ASSERT(res == S_OK);

 
	// Sequence of call to get the MediaType
	// WAVEFORMATEX for mp3 can then be extract from MediaType
	res = wmSyncReader->QueryInterface(&wmProfile); ASSERT(res == S_OK);
	res = wmProfile->GetStream(0, &wmStreamConfig); ASSERT(res == S_OK);
	res = wmStreamConfig->QueryInterface(&wmMediaProperties); ASSERT(res == S_OK);

	// Retrieve sizeof MediaType
	res = wmMediaProperties->GetMediaType(NULL, &sizeMediaType); ASSERT(res == S_OK);

	// Retrieve MediaType
	WM_MEDIA_TYPE* mediaType = (WM_MEDIA_TYPE*)LocalAlloc(LPTR,sizeMediaType); 
	res = wmMediaProperties->GetMediaType(mediaType, &sizeMediaType); ASSERT(res == S_OK);

	// Check that MediaType is audio
	ASSERT(mediaType->majortype == WMMEDIATYPE_Audio);
	// ASSERT(mediaType->pbFormat == WMFORMAT_WaveFormatEx);

	// Check that input is mp3
	WAVEFORMATEX* inputFormat = (WAVEFORMATEX*)mediaType->pbFormat;
	ASSERT( inputFormat->wFormatTag == WAVE_FORMAT_MPEGLAYER3);
	ASSERT( inputFormat->nSamplesPerSec == 44100);
	//ASSERT( inputFormat->nChannels == 2);

	WAVE_HEADER waveHeader;
	waveHeader.fmtBlock.wavFormat.SetFormat(inputFormat->nChannels, inputFormat->nSamplesPerSec, 16/*inputFormat->wBitsPerSample*/);

	// Define output format
	WAVEFORMATEX pcmFormat;
	pcmFormat.wFormatTag = WAVE_FORMAT_PCM;
	pcmFormat.nChannels = waveHeader.fmtBlock.wavFormat.wChannels;
	pcmFormat.nSamplesPerSec = waveHeader.fmtBlock.wavFormat.dwSamplesPerSec;
	pcmFormat.nAvgBytesPerSec = waveHeader.fmtBlock.wavFormat.dwAvgBytesPerSec;
	pcmFormat.nBlockAlign = waveHeader.fmtBlock.wavFormat.wBlockAlign;
	pcmFormat.wBitsPerSample = waveHeader.fmtBlock.wavFormat.wBitsPerSample;
	pcmFormat.cbSize = 0;
 
	const DWORD MP3_BLOCK_SIZE = 522;
 
	// Define input format
	MPEGLAYER3WAVEFORMAT mp3Format;
	mp3Format.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	mp3Format.wfx.nChannels = 2;
	mp3Format.wfx.nSamplesPerSec = 44100;
	mp3Format.wfx.nAvgBytesPerSec = 128 * (1024 / 8);
	mp3Format.wfx.nBlockAlign = 1;
	mp3Format.wfx.wBitsPerSample = 0;
	mp3Format.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
	mp3Format.wID = MPEGLAYER3_ID_MPEG;
	mp3Format.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
	mp3Format.nBlockSize = MP3_BLOCK_SIZE;
	mp3Format.nFramesPerBlock = 1;
	mp3Format.nCodecDelay = 1393;






	// Release COM interface
	// wmSyncReader->Close();
	wmMediaProperties->Release();
	wmStreamConfig->Release();
	wmProfile->Release();
	wmHeaderInfo->Release();
	wmSyncReader->Release();

	// Free allocated mem
	LocalFree(mediaType);

	// -----------------------------------------------------------------------------------
	// Convert mp3 to pcm using acm driver
	// The following code is mainly inspired from http://david.weekly.org/code/mp3acm.html
	// -----------------------------------------------------------------------------------

	// Get maximum FormatSize for all acm
	res = acmMetrics( NULL, ACM_METRIC_MAX_SIZE_FORMAT, &maxFormatSize ); ASSERT(res == S_OK);



	// Allocate PCM output sound buffer
	unsigned int pcmSize = durationInSecondInt * pcmFormat.nAvgBytesPerSec;
	waveHeader.SetSize(pcmSize);
	bufferLength = pcmSize + sizeof(WAVE_HEADER);
	soundBuffer = (BYTE*)LocalAlloc(LPTR, bufferLength);

	acmMp3stream = NULL;
	MMRESULT openRes = acmStreamOpen( &acmMp3stream, NULL, (LPWAVEFORMATEX)&mp3Format, &pcmFormat, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME);
	switch(openRes)
	{
		case MMSYSERR_NOERROR:
			break; // success!
		
		case MMSYSERR_INVALPARAM:
			ASSERT( !"Invalid parameters passed to acmStreamOpen" );
			return E_FAIL;
		
		case ACMERR_NOTPOSSIBLE:
			ASSERT( !"No ACM filter found capable of decoding MP3" );
			return E_FAIL;
		
		default:
			ASSERT( !"Some error opening ACM decoding stream!" );
			return E_FAIL;
	}

	
	


	// Determine output decompressed buffer size
	unsigned long rawbufsize = 0;
	res = acmStreamSize( acmMp3stream, MP3_BLOCK_SIZE, &rawbufsize, ACM_STREAMSIZEF_SOURCE ); ASSERT(res == S_OK);
	ASSERT( rawbufsize > 0 );

	// allocate our I/O buffers
	static BYTE mp3BlockBuffer[MP3_BLOCK_SIZE];
	//LPBYTE mp3BlockBuffer = (LPBYTE) LocalAlloc( LPTR, MP3_BLOCK_SIZE );
	LPBYTE rawbuf = (LPBYTE) LocalAlloc( LPTR, rawbufsize );

	// prepare the decoder
	static ACMSTREAMHEADER mp3streamHead;
	// memset( &mp3streamHead, 0, sizeof(ACMSTREAMHEADER ) );
	mp3streamHead.cbStruct = sizeof(ACMSTREAMHEADER );
	mp3streamHead.pbSrc = mp3BlockBuffer;
	mp3streamHead.cbSrcLength = MP3_BLOCK_SIZE;
	mp3streamHead.pbDst = rawbuf;
	mp3streamHead.cbDstLength = rawbufsize;
	res = acmStreamPrepareHeader( acmMp3stream, &mp3streamHead, 0 ); ASSERT(res == S_OK);
 
	BYTE* currentOutput = soundBuffer;

	
	memcpy(currentOutput, &waveHeader, sizeof(waveHeader));
	currentOutput += sizeof(waveHeader);

	DWORD totalDecompressedSize = 0;
 
	static ULARGE_INTEGER newPosition;
	static LARGE_INTEGER seekValue;
	res = mp3Stream->Seek(seekValue, STREAM_SEEK_SET, &newPosition); ASSERT(res == S_OK);
 
	while(1) 
	{
		// suck in some MP3 data
		ULONG count;
		res = mp3Stream->Read(mp3BlockBuffer, MP3_BLOCK_SIZE, &count); ASSERT(res == S_OK);
		if( count != MP3_BLOCK_SIZE )
			break;
 
		// convert the data
		res = acmStreamConvert( acmMp3stream, &mp3streamHead, ACM_STREAMCONVERTF_BLOCKALIGN ); 
		ASSERT(res == S_OK);
 
		// write the decoded PCM to disk
		//count = fwrite( rawbuf, 1, mp3streamHead.cbDstLengthUsed, fpOut );
		if(mp3streamHead.cbDstLengthUsed > 0)
		{
			memcpy(currentOutput, rawbuf, mp3streamHead.cbDstLengthUsed);
			totalDecompressedSize += mp3streamHead.cbDstLengthUsed;
			currentOutput += mp3streamHead.cbDstLengthUsed;
		}
	}

	res = acmStreamUnprepareHeader( acmMp3stream, &mp3streamHead, 0); ASSERT(res == S_OK);
	LocalFree(rawbuf);
	res = acmStreamClose( acmMp3stream, 0 ); ASSERT(res == S_OK);

	bool saveRes = SaveWholeFile(soundBuffer, totalDecompressedSize, outputFilename);
	//bool saveRes = SaveWholeFile((uint8_t *)("1234567890"), 10, "test.wav");
/*
	FILE* pOutputFile = fopen( outputFilename, "wb" );
    if( pOutputFile )
    {
		fwrite(currentOutput, totalDecompressedSize, 1, pOutputFile);
        fclose( pOutputFile );
    }
*/

	mp3Stream->Release();
	GlobalFree(mp3HGlobal);
	return S_OK;
}


#pragma function(memset,memcpy,memcmp)