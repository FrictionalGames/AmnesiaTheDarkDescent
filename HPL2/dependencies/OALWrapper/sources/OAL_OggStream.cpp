/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_OggStream.cpp
	@author Luis Rodero
	@date 2006-10-06
	@version 0.1
	Definition of Base class for containing Ogg Vorbis streams
*/


#include "OALWrapper/OAL_OggStream.h"
#include "OALWrapper/OAL_Buffer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

struct tMemoryReader {
	char* buff;
	ogg_int64_t buff_size;
	ogg_int64_t buff_pos;

	tMemoryReader(const void* aBuff, size_t aBuffSize) : buff(NULL), buff_size(aBuffSize), buff_pos(0)
	{
		buff = new char[buff_size];
		memcpy(buff, aBuff, buff_size);
	}
	~tMemoryReader()
	{
		delete [] buff;
	}
};

size_t memoryRead(void * buff, size_t b, size_t nelts, void *data)
{
	tMemoryReader *of = reinterpret_cast<tMemoryReader*>(data);
	size_t len = b * nelts;
	if (of->buff_pos + len > of->buff_size) {
		len = of->buff_size - of->buff_pos;
	}
	if (len)
		memcpy(buff, of->buff + of->buff_pos, len );
	of->buff_pos += len;
	return len;
}

int memorySeek(void *data, ogg_int64_t seek, int type)
{
	tMemoryReader *of = reinterpret_cast<tMemoryReader*>(data);

	switch (type) {
		case SEEK_CUR:
			of->buff_pos += seek;
			break;
		case SEEK_END:
			of->buff_pos = of->buff_size - seek;
			break;
		case SEEK_SET:
			of->buff_pos = seek;
			break;
		default:
			return -1;
	}
	if ( of->buff_pos < 0) {
		of->buff_pos = 0;
		return -1;
	}
	if ( of->buff_pos > of->buff_size) {
		of->buff_pos = of->buff_size;
		return -1;
	}
	return 0;
}

long memoryTell(void* data)
{
	tMemoryReader *of = reinterpret_cast<tMemoryReader*>(data);
	return of->buff_pos;
}

int memoryClose(void* data)
{
	tMemoryReader *of = reinterpret_cast<tMemoryReader*>(data);
	delete of;
	return 0;
}

static ov_callbacks OV_MEMORY_CALLBACKS = {
	memoryRead,
	memorySeek,
	memoryClose,
	memoryTell
};

//---------------------------------------------------------------------

cOAL_OggStream::cOAL_OggStream(): mbIsValidHandle(false)
{
}

//---------------------------------------------------------------------

cOAL_OggStream::~cOAL_OggStream()
{
	Destroy();
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Stream ( const ALuint alDestBuffer )
//	-	Streams data from the Ogg file to the buffer
///////////////////////////////////////////////////////////

//---------------------------------------------------------------------

bool cOAL_OggStream::Stream(cOAL_Buffer* apDestBuffer)
{
	DEF_FUNC_NAME("cOAL_OggStream::Stream()");

	long lDataSize = 0;

	//hpl::Log("Streaming...\n");
	double fStartTime = GetTime();

	// Loop which loads chunks of decoded data into a buffer
	while(lDataSize < (int)mlBufferSize)
	{
		long lChunkSize = ov_read(&movStreamHandle,
								   mpPCMBuffer+lDataSize,
								   mlBufferSize-lDataSize,
								   SYS_ENDIANNESS,
								   2, 1, &mlCurrent_section);

		// If we get a 0, then we are at the end of the file
		if(lChunkSize == 0)
		{
			break;
		}
		// If we get a negative value, then something went wrong. Clean up and set error status.
		else if(lChunkSize == OV_HOLE)										
			;
		else if(lChunkSize==OV_EINVAL || 
				lChunkSize==OV_EBADLINK || 
				lChunkSize<0)
			mbStatus = false;
		else
			lDataSize += lChunkSize;
	}
	// Bind the data to the Buffer Object
	if(lDataSize) 
		mbStatus = apDestBuffer->Feed(mpPCMBuffer, lDataSize, fStartTime);
	else 
		mbEOF = true;

	return (lDataSize != 0);
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////
// void Seek(float afWhere)
//
///////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void cOAL_OggStream::Seek(float afWhere, bool abForceRebuffer)
{
	mbEOF = false;
	
	if(afWhere < 0)
		afWhere = 0;
	if(afWhere > 1)
		afWhere = 1;

	afWhere = afWhere*mlSamples;

	// Move the pointer to the desired offset
	ov_pcm_seek_page_lap(&movStreamHandle, (long int) afWhere);
	if(abForceRebuffer)
		mbNeedsRebuffering = true;
}

//---------------------------------------------------------------------

double cOAL_OggStream::GetTime()
{
	return ov_time_tell(&movStreamHandle);
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//---------------------------------------------------------------------

bool cOAL_OggStream::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_OggStream::CreateFromFile()");
	
	if(mbStatus==false)
		return false;

	int lOpenResult;
	FILE *pStreamFile = OpenFileW(asFilename, L"rb");

	if (pStreamFile == NULL)
		return false;

	msFilename = asFilename;

	// If not an Ogg file, set status and exit

	lOpenResult = ov_open_callbacks(pStreamFile, &movStreamHandle,
									NULL, 0, OV_CALLBACKS_DEFAULT);
	if(lOpenResult<0)	
	{
		fclose(pStreamFile);
		mbIsValidHandle = false;
		mbStatus = false;
		return false;
	}
	mbIsValidHandle = true;

	// Get file info
	vorbis_info *viFileInfo = ov_info ( &movStreamHandle, -1 );

	mlChannels = viFileInfo->channels;
	mlFrequency = viFileInfo->rate;
	mlSamples = (long) ov_pcm_total ( &movStreamHandle, -1 );
	mFormat = (mlChannels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

	mfTotalTime = ov_time_total( &movStreamHandle, -1 );

	return true;
}

//---------------------------------------------------------------------

bool cOAL_OggStream::CreateFromBuffer(const void *apBuffer, size_t aSize)
{
	DEF_FUNC_NAME("cOAL_OggStream::CreateFromBuffer()");

	if(mbStatus==false)
		return false;

	if (apBuffer == NULL)
		return false;

	// If not an Ogg file, set status and exit
	tMemoryReader *pBufferStream = new tMemoryReader(apBuffer, aSize);

	int lOpenResult = ov_open_callbacks(pBufferStream, &movStreamHandle,
									NULL, 0, OV_MEMORY_CALLBACKS);
	if(lOpenResult<0)
	{
		delete pBufferStream;
		mbIsValidHandle = false;
		mbStatus = false;
		return false;
	}
	mbIsValidHandle = true;

	// Get file info
	vorbis_info *viFileInfo = ov_info ( &movStreamHandle, -1 );

	mlChannels = viFileInfo->channels;
	mlFrequency = viFileInfo->rate;
	mlSamples = (long) ov_pcm_total ( &movStreamHandle, -1 );
	mFormat = (mlChannels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

	mfTotalTime = ov_time_total( &movStreamHandle, -1 );

	return true;
}

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//---------------------------------------------------------------------

void cOAL_OggStream::Destroy()
{
	DEF_FUNC_NAME("cOAL_OggStream::Unload()");
	
	// If we loaded a stream, clear the handle to the Ogg Vorbis file
	if(mbIsValidHandle)
		ov_clear(&movStreamHandle);
}

//---------------------------------------------------------------------

