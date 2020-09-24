#include "OpenAL/OAL_AudioData.h"

#include "OpenAL/OAL_Buffer.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"

//-------------------------------------------------------------------------------

class cOAL_Buffer;
class cOAL_Source;

iOAL_AudioData::iOAL_AudioData(eOAL_AudioDataType aType, int alBuffersToCreate)
{
	mType = aType;
	for(int i=0;i<alBuffersToCreate;++i)
		mvBuffers.push_back(hplNew(cOAL_Buffer,(this)));

	Reset();

	//hpl::Log(" OAL: Creating audiodata %p\n", this);
}

iOAL_AudioData::~iOAL_AudioData()
{
	//hpl::Log(" OAL: Destroying audio data %p ('%s')\n", this, hpl::cString::To8Char(msFilename).c_str());

	for(int i=0;i<(int)mvBuffers.size();++i)
		hplDelete(mvBuffers[i]);
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
