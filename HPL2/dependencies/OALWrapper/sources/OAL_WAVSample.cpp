/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_WAVSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#include "OALWrapper/OAL_WAVSample.h"
#include "OALWrapper/OAL_Buffer.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Device.h"

// Pull in for OAL Types
#include <ogg/ogg.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Load ( const string &asFilename )
//	-	Loads sample data from a WAV file
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cOAL_WAVSample::CreateFromFile(const wstring &asFilename)
{
	FILE *fileHandle = OpenFileW(asFilename, L"rb");
	fseek(fileHandle, 0, SEEK_END);
	size_t pos = ftell(fileHandle);
	fseek(fileHandle, 0, SEEK_SET);

	void *buffer = malloc(pos);
	
	fread(buffer, pos, 1, fileHandle);
	fclose(fileHandle);
	bool result = CreateFromBuffer(buffer, pos);
	free(buffer);

	return result;
}

struct RIFF_Header {
	char chunkID[4];
	ogg_int32_t chunkSize;
	char format[4];
};

struct WAVE_Format {
	char subChunkID[4];
	ogg_int32_t subChunkSize;
	ogg_int16_t audioFormat;
	ogg_int16_t numChannels;
	ogg_int32_t sampleRate;
	ogg_int32_t byteRate;
	ogg_int16_t blockAlign;
	ogg_int16_t bitsPerSample;
};

struct WAVE_Data {
  char subChunkID[4]; //should contain the word data
  ogg_int32_t subChunkSize; //Stores the size of the data block
};

struct RIFF_SubChunk {
  char subChunkID[4]; //should contain the word data
  ogg_int32_t subChunkSize; //Stores the size of the data block
};

static const char* find_chunk(const char* start, const char* end, const char* chunkID)
{
	RIFF_SubChunk chunk;
	const char* ptr = start;
	while (ptr < (end - sizeof(RIFF_SubChunk)))
	{
		memcpy(&chunk, ptr, sizeof(RIFF_SubChunk));

		if (chunk.subChunkID[0] == chunkID[0] &&
			chunk.subChunkID[1] == chunkID[1] &&
			chunk.subChunkID[2] == chunkID[2] &&
			chunk.subChunkID[3] == chunkID[3])
		{
			return ptr;
		}
		ptr += sizeof(RIFF_SubChunk) + chunk.subChunkSize;
	}
	return 0;
}

template<typename T>
inline const char*readStruct(T& dest, const char*& ptr)
{
	const char* ret;
	memcpy(&dest, ptr, sizeof(T));
	ptr += sizeof(RIFF_SubChunk);
	ret = ptr;
	ptr += dest.subChunkSize;
	return ret;
}

bool cOAL_WAVSample::CreateFromBuffer(const void* apBuffer, size_t aSize)
{
	const char* start = (const char*)apBuffer;
	const char* end = start + aSize;
	const char* ptr = start;
	RIFF_Header riff_header;
	WAVE_Format wave_format;
	WAVE_Data wave_data;

	memcpy(&riff_header, ptr, sizeof(RIFF_Header));
	ptr += sizeof(RIFF_Header);

	if (riff_header.chunkID[0] != 'R' ||
		riff_header.chunkID[1] != 'I' ||
		riff_header.chunkID[2] != 'F' ||
		riff_header.chunkID[3] != 'F' ||
		riff_header.format[0] != 'W' ||
		riff_header.format[1] != 'A' ||
		riff_header.format[2] != 'V' ||
		riff_header.format[3] != 'E')
	{
		return false;
	}
	
	ptr = find_chunk(ptr, end, "fmt ");
	if (!ptr) {
		return false;
	}
	readStruct(wave_format, ptr);
	
	if (wave_format.audioFormat != 1) {
		return false;
	}

	ptr = find_chunk(ptr, end, "data");
	if (!ptr) {
		return false;
	}

	const char* base = readStruct(wave_data, ptr);

	size_t size = wave_data.subChunkSize;
	if (size > (end - base)) {
		return false;
	}

	mlChannels = wave_format.numChannels;
	if (mlChannels == 2)
	{
		if (wave_format.bitsPerSample == 8)
		{
			mFormat = AL_FORMAT_STEREO8;
			mlSamples = size / 2;
		}
		else //if (wave_format.bitsPerSample == 16)
		{
			mlSamples = size / 4;
			mFormat = AL_FORMAT_STEREO16;
		}
	} else //if (mlChannels == 1)
	{
		if (wave_format.bitsPerSample == 8)
		{
			mlSamples = size;
			mFormat = AL_FORMAT_MONO8;
		}
		else //if (wave_format.bitsPerSample == 16)
		{
			mlSamples = size / 2;
			mFormat = AL_FORMAT_MONO16;
		}
	}
	mlFrequency = wave_format.sampleRate;
	mfTotalTime = float(mlSamples) / float(mlFrequency);
	
	cOAL_Buffer* pBuffer = mvBuffers[0];
	mbStatus = pBuffer->Feed((ALvoid*)base, size);

	return true;
}

//------------------------------------------------------------------


