/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_OggSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing Ogg Vorbis Sample data 
*/

#include "OALWrapper/OAL_OggSample.h"
#include "OALWrapper/OAL_Buffer.h"
#include <cstdlib>
#include <string>
#include <cstring>

#include <vorbis/vorbisfile.h>

using namespace std;

typedef struct
{
	unsigned char* data;
	ogg_int64_t size;
	ogg_int64_t pos;
} OAL_OggMemoryFile;

static size_t OAL_OggBufferRead(void* dest, size_t eltSize, size_t nelts, OAL_OggMemoryFile* src)
{
	size_t len = eltSize * nelts;
	if ( (src->pos + len) > src->size)
	{
		len = src->size - src->pos;
	}
	if (len > 0)
	{
		memcpy( dest, (src->data + src->pos), len);
		src->pos += len;
	}
	return len;
}

static int OAL_OggBufferSeek(OAL_OggMemoryFile* src, ogg_int64_t pos, int whence)
{
	switch (whence) {
		case SEEK_CUR:
			src->pos += pos;
			break;
		case SEEK_END:
			src->pos = src->size - pos;
			break;
		case SEEK_SET:
			src->pos = pos;
			break;
		default:
			return -1;
	}
	if (src->pos < 0) {
		src->pos = 0;
		return -1;
	}
	if (src->pos > src->size) {
		return -1;
	}
	return 0;
}

static int OAL_OggBufferClose(OAL_OggMemoryFile* src)
{
	return 0;
}

static long OAL_OggBufferTell(OAL_OggMemoryFile *src)
{
	return src->pos;
}

static ov_callbacks OAL_CALLBACKS_BUFFER = {
	(size_t (*)(void *, size_t, size_t, void *))	OAL_OggBufferRead,
	(int (*)(void *, ogg_int64_t, int))				OAL_OggBufferSeek,
	(int (*)(void *))								OAL_OggBufferClose,
	(long (*)(void *))								OAL_OggBufferTell
};

//-------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

bool cOAL_OggSample::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_OggSample::CreateFromFile()");
	FUNC_USES_AL;

	if(mbStatus==false)
		return false;

	Reset();
	
	int lOpenResult;

	msFilename = asFilename;

	FILE *fileHandle = OpenFileW(asFilename, L"rb");
	
	// If no file is present, set the error status and return
	if(!fileHandle)
	{
		mbStatus = false;
		return false;
	}

	// If not an Ogg file, set status and exit
	OggVorbis_File ovFileHandle;
	if((lOpenResult = ov_open_callbacks(fileHandle, &ovFileHandle, NULL, 0, OV_CALLBACKS_DEFAULT))<0)
	{
		fclose ( fileHandle );
		mbStatus = false;
		return false;
	}

	return LoadOgg(ovFileHandle);
}

bool cOAL_OggSample::CreateFromBuffer(const void* apBuffer, size_t aSize)
{
	DEF_FUNC_NAME("cOAL_OggSample::CreateFromBuffer()");
	FUNC_USES_AL;

	if(mbStatus==false)
		return false;

	Reset();

	int lOpenResult;

	msFilename = L":buffer:";

	// If no buffer is set, set the error status and return
	if(!apBuffer)
	{
		mbStatus = false;
		return false;
	}

	// If not an Ogg file, set status and exit
	OAL_OggMemoryFile fakeFile = { (unsigned char*)apBuffer, aSize, 0 };
	OggVorbis_File ovFileHandle;
	if((lOpenResult = ov_open_callbacks(&fakeFile, &ovFileHandle, NULL, 0, OAL_CALLBACKS_BUFFER))<0)
	{
		mbStatus = false;
		return false;
	}
	return LoadOgg(ovFileHandle);
}

bool cOAL_OggSample::LoadOgg(OggVorbis_File& ovFileHandle)
{
	char *pPCMBuffer;
	bool bEOF = false;
	int lCurrent_section;
	long lDataSize = 0;

	// Get file info
	vorbis_info *viFileInfo = ov_info ( &ovFileHandle, -1 );
	mlChannels = viFileInfo->channels;
	mFormat = (mlChannels == 2)?AL_FORMAT_STEREO16:AL_FORMAT_MONO16;
	mlFrequency = viFileInfo->rate;
	mlSamples = (long) ov_pcm_total ( &ovFileHandle, -1 );
	mfTotalTime = ov_time_total( &ovFileHandle, -1 );

	// Reserve memory for 'mlChannels' channels of 'mlSamples' * 2 bytes of data each
	int lSizeInBytes = mlSamples * mlChannels * GetBytesPerSample();
	pPCMBuffer = (char *) malloc (lSizeInBytes);
	memset (pPCMBuffer, 0, lSizeInBytes);

	// Loop which loads chunks of decoded data into a buffer
	while(!bEOF)
	{
		long lChunkSize = ov_read ( &ovFileHandle			,										
									pPCMBuffer + lDataSize	, 
									STREAMING_BLOCK_SIZE	, 
									SYS_ENDIANNESS			,
									2, 1, &lCurrent_section );

		if(lChunkSize == 0)
			bEOF = true;
		// If we get a negative value, then something went wrong. Clean up and set error status.
		else if(lChunkSize < 0)
		{
			free(pPCMBuffer);
			ov_clear(&ovFileHandle);
			// ov_clear closes the file handle for us
			mbStatus = false;
			return false;
		}
		else 
			lDataSize += lChunkSize;
	}

	cOAL_Buffer* pBuffer = mvBuffers[0];
	if(lDataSize)
	{
		// If something went wrong, set error status. Clean up afterwards.
		mbStatus = pBuffer->Feed((ALvoid*)pPCMBuffer, lDataSize);
	}
	free(pPCMBuffer);
	ov_clear(&ovFileHandle);
	// ov_clear closes the file handle for us
	return true;
}
