/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#include "OALWrapper/OAL_AudioData.h"

#include "OALWrapper/OAL_Buffer.h"


//-------------------------------------------------------------------------------

class cOAL_Buffer;
class cOAL_Source;

iOAL_AudioData::iOAL_AudioData(eOAL_AudioDataType aType, int alBuffersToCreate)
{
	mType = aType;
	for(int i=0;i<alBuffersToCreate;++i)
		mvBuffers.push_back(new cOAL_Buffer(this));

	Reset();
}

iOAL_AudioData::~iOAL_AudioData()
{
	for(int i=0;i<(int)mvBuffers.size();++i)
		delete mvBuffers[i];
}

//-------------------------------------------------------------------------------

void iOAL_AudioData::Reset()
{
	mlBuffersUsed = 0;
	mlSamples = 0;
	mbLoop = false;
	mbEOF = false;
	mbStatus = true;
	mfTotalTime = 0;
	mlFrequency = 0;
	mlChannels = 0;
}

//-------------------------------------------------------------------------------

int iOAL_AudioData::GetBytesPerSample()
{
	int lBytesPerSample;
	if(mFormat==AL_FORMAT_MONO16 || 
		mFormat==AL_FORMAT_STEREO16)
		lBytesPerSample = 2;
	else
		lBytesPerSample = 1;

	return lBytesPerSample;
}

//-------------------------------------------------------------------------------

cOAL_Buffer* iOAL_AudioData::GetBufferByID(ALuint alID)
{
	for(int i=0;i<(int)mvBuffers.size();++i)
	{
		cOAL_Buffer* pBuffer = mvBuffers[i];
		if(alID==pBuffer->GetObjectID())
			return pBuffer;
	}
	return NULL;
}
