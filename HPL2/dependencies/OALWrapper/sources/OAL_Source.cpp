/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Source.cpp
	@author Luis Rodero
	@date 2006-10-10
	@version 0.1
	Wrapping for OpenAL Source Objects
*/


#include "OALWrapper/OAL_Source.h"
#include "OALWrapper/OAL_Buffer.h"
#include "OALWrapper/OAL_Sample.h"
#include "OALWrapper/OAL_Stream.h"
#include "OALWrapper/OAL_EffectSlot.h"
#include "OALWrapper/OAL_Effect.h"
#include "OALWrapper/OAL_Filter.h"
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_SourceManager.h"

#include <SDL_mutex.h>

//--------------------------------------------------------------------------------

extern cOAL_Device*	gpDevice;

using namespace std;

//--------------------------------------------------------------------------------

cOAL_Source::cOAL_Source(cOAL_SourceManager *apSourceManager, int alId, int alSends): iOAL_LowLevelObject("Source"),
																					  mlId(alId), 
																					  mlPriority(255), 
																					  mlRefCount(0), 
																					  mpSourceMutex(NULL), 
																					  mpSourceManager(NULL), 
																					  mpAudioData(NULL),
																					  mbPlaying(false), 
																					  mbPaused(false), 
																					  mbNeedsReset(true),
																					  mpFilter(NULL),
																					  mpDirectFilter(NULL)
{
	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "", "cOAL_Source constructor called...\n" );
	if(apSourceManager)
	{
		mpSourceManager = apSourceManager;
		
		if(mpSourceManager->IsThreadAlive())
			mpSourceMutex = SDL_CreateMutex();
	}

	if(alSends && gpDevice->IsEFXActive())
	{
		LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "", "Activating EFX Source stuff\n" );
		for (int i=0; i<alSends; ++i)
		{
			cOAL_SourceSend* pSend = new cOAL_SourceSend;
			mvSends.push_back(pSend);
		}
	}

	// This resets the source
	if(CreateLowLevelID())
	{
		LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info,"", "Success\n" );
		Stop();
	}
	else
		LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Error,"", "Failed\n" );
}

//--------------------------------------------------------------------------------

cOAL_Source::~cOAL_Source()
{
	LogMsg("", eOAL_LogVerbose_High, eOAL_LogMsg_Command, "Source Destructor called\n" );
	Stop();

	if(mpSourceMutex)
	{
		LogMsg("", eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Destroying Mutex...\n" );
		SDL_DestroyMutex(mpSourceMutex);
		mpSourceMutex = NULL;
	}

	if(mpFilter)
	{
		LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Destroying Filter...\n" );
		delete mpFilter;
		mpFilter = NULL;
	}
	
	if(mvSends.empty()==false)
	{
		LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Destroying Sends...\n" );
		for(unsigned int i = 0; i<mvSends.size(); ++i)
			delete mvSends[i];
	}
	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Destroying Low Level Object...\n" );
	DestroyLowLevelID();
}

//--------------------------------------------------------------------------------

bool cOAL_Source::CreateLowLevelID()
{
	DEF_FUNC_NAME(CreateLowLevelID);
	FUNC_USES_AL;

	RUN_AL_FUNC(alGenSources(1, &mlObjectId));
	
	return !AL_ERROR_OCCURED;
}

//--------------------------------------------------------------------------------

bool cOAL_Source::DestroyLowLevelID()
{
	DEF_FUNC_NAME(DestroyLowLevelID);
	FUNC_USES_AL;

	RUN_AL_FUNC(alDeleteSources(1, &mlObjectId));
	
	return !AL_ERROR_OCCURED;
}

//--------------------------------------------------------------------------------

bool cOAL_Source::IsValidObject()
{
	DEF_FUNC_NAME(cOAL_Source::IsValidObject);
	FUNC_USES_AL;

	bool bIsValid = RUN_AL_FUNC((alIsSource(mlObjectId) == AL_TRUE));
	
	return bIsValid;
}

//--------------------------------------------------------------------------------

void cOAL_Source::Lock()
{
	if(mpSourceManager->IsThreadAlive())
		SDL_LockMutex(mpSourceMutex);
}

void cOAL_Source::Unlock()
{
	if ( mpSourceManager->IsThreadAlive() )
		SDL_UnlockMutex(mpSourceMutex);
}

//--------------------------------------------------------------------------------

int cOAL_Source::GetProcessedBuffers()
{
	DEF_FUNC_NAME(cOAL_Source::GetProcessedBuffers);
	FUNC_USES_AL;

	int lProcessedBuffers;

	RUN_AL_FUNC(alGetSourcei( mlObjectId, AL_BUFFERS_PROCESSED, &lProcessedBuffers ));
	
	return lProcessedBuffers;
}

//--------------------------------------------------------------------------------

int cOAL_Source::GetQueuedBuffers()
{
	DEF_FUNC_NAME(cOAL_Source::GetQueuedBuffers);
	FUNC_USES_AL;

	int lQueuedBuffers;

	RUN_AL_FUNC(alGetSourcei( mlObjectId, AL_BUFFERS_QUEUED, &lQueuedBuffers ));
	
	return lQueuedBuffers;
}

//--------------------------------------------------------------------------------

eOAL_SourceStatus cOAL_Source::GetSourceStatus ()
{
	DEF_FUNC_NAME(cOAL_Source::GetSourceStatus);
	FUNC_USES_AL;

	int lStatus = 0;
	
	RUN_AL_FUNC(alGetSourcei(mlObjectId, AL_SOURCE_STATE, &lStatus));
		
	if(mpAudioData == NULL)
		return eOAL_SourceStatus_Free;

	if(lStatus==AL_STOPPED || lStatus==AL_INITIAL)
	{
		if(mbPaused)
			return eOAL_SourceStatus_Busy;
		if(mbPlaying)
		{
			if(mpAudioData->HasBufferUnderrun())
				return eOAL_SourceStatus_Busy_BufferUnderrun;
		}
        return eOAL_SourceStatus_Free;
	}
    return eOAL_SourceStatus_Busy;
}

//--------------------------------------------------------------------------------

eOAL_AudioDataType cOAL_Source::GetSourceType()
{
	if(mpAudioData)
		return mpAudioData->GetType();

	return eOAL_AudioDataType_None;
}

//--------------------------------------------------------------------------------

void cOAL_Source::Queue(cOAL_Buffer* apBuffer)
{
	DEF_FUNC_NAME(cOAL_Source::~cOAL_Source());
	FUNC_USES_AL;

	RUN_AL_FUNC(alSourceQueueBuffers( mlObjectId, 1, apBuffer->GetObjectIDPointer()));
}

//--------------------------------------------------------------------------------

cOAL_Buffer* cOAL_Source::Unqueue()
{
	DEF_FUNC_NAME(cOAL_Source::~cOAL_Source());
	FUNC_USES_AL;

	ALuint lTempBuffer;
	RUN_AL_FUNC(alSourceUnqueueBuffers(mlObjectId,1,&lTempBuffer));

	return mpAudioData->GetBufferByID(lTempBuffer);
}

//--------------------------------------------------------------------------------

void cOAL_Source::LowLevelPlay()
{
	DEF_FUNC_NAME(cOAL_Source::LowLevelPlay);
	FUNC_USES_AL;

	if(!IsValidObject())
	{
		//OAL_Source_Log(mlId,2,"Invalid source name:%d\n", mlSource);
		return;
	}
	if(mpAudioData==NULL)
		return;

	RUN_AL_FUNC(alSourcePlay(mlObjectId));
}

//--------------------------------------------------------------------------------

void cOAL_Source::LowLevelStop()
{
	DEF_FUNC_NAME(cOAL_Source::LowLevelStop);
	FUNC_USES_AL;

	if(!IsValidObject())
	{
		//OAL_Source_Log(mlId,2,"Invalid source name:%d\n", mlSource);
		return;
	}

	//LowLevelSetMuted(true);
	RUN_AL_FUNC(alSourceStop(mlObjectId));
}

//--------------------------------------------------------------------------------

void cOAL_Source::LowLevelPause()
{
	DEF_FUNC_NAME(cOAL_Source::LowLevelPause);
	FUNC_USES_AL;

	if(!IsValidObject())
	{
		//OAL_Source_Log(mlId,2,"Invalid source name:%d\n", mlSource);
		return;
	}

	RUN_AL_FUNC(alSourcePause(mlObjectId));

	//OAL_Source_Log(mlId,0,"LowLevelPause() - status:%s\n",GetDebugInfo().c_str() );

}

//--------------------------------------------------------------------------------

bool cOAL_Source::LowLevelSet(eOAL_SourceParam aParam)
{
	DEF_FUNC_NAME(cOAL_Source::LowLevelSet);
	FUNC_USES_AL;

	if(IsValidObject()==false)
	{
		//OAL_Source_Log(mlId,2,"Invalid source name:%d\n", mlSource);
		return false;
	}

	switch (aParam)
	{
	case eOAL_SourceParam_Gain:
		RUN_AL_FUNC(alSourcef( mlObjectId, AL_GAIN, mfGain ));
		break;
	case eOAL_SourceParam_Pitch:
		RUN_AL_FUNC(alSourcef( mlObjectId, AL_PITCH, mfPitch ));
		break;
	case eOAL_SourceParam_Position:
		RUN_AL_FUNC(alSourcefv ( mlObjectId, AL_POSITION, mvPos ));
		break;
	case eOAL_SourceParam_PositionRelative:
		RUN_AL_FUNC(alSourcei ( mlObjectId, AL_SOURCE_RELATIVE, mbRelativePosition ? AL_TRUE : AL_FALSE ));
		break;
	case eOAL_SourceParam_Velocity:
        RUN_AL_FUNC(alSourcefv ( mlObjectId, AL_VELOCITY, mvVel ));
		break;
	case eOAL_SourceParam_Buffer:
		RUN_AL_FUNC( alSourceQueueBuffers(mlObjectId, mpAudioData->GetBuffersUsed(), mpAudioData->GetOALBufferPointer()));
		mpSourceManager->ReserveVoices(mpAudioData->GetChannels());
		break;	
	default:
		break;
	}

	return !AL_ERROR_OCCURED;
}

//--------------------------------------------------------------------------------

void cOAL_Source::LowLevelSetMuted( bool abX )
{
	DEF_FUNC_NAME(cOAL_Source::LowLevelStop);
	FUNC_USES_AL;

	if(IsValidObject()==false)
	{
		//OAL_Source_Log(mlId,2,"Invalid source name:%d\n", mlSource);
		return;
	}
	if(abX)
	{
		RUN_AL_FUNC(alSourcef(mlObjectId, AL_GAIN, 0.00001f));
	}
	else
		LowLevelSet(eOAL_SourceParam_Gain);
}

//--------------------------------------------------------------------------------
void cOAL_Source::IncRefCount()
{
	if(++mlRefCount>=0x80000)
		mlRefCount = 0;
}

//--------------------------------------------------------------------------------

int cOAL_Source::GetPackedHandle(int alRef, int alId)
{
	return (alRef<<12) | alId;
}

//--------------------------------------------------------------------------------

double cOAL_Source::GetElapsedTime()
{
	DEF_FUNC_NAME("cOAL_Source::GetElapsedTime()");
	FUNC_USES_AL;

	//Get the time offset (in seconds) of the current playing buffer.
	double fElapsed = mpAudioData->GetProcessedBuffersTime();

	if(gpDevice->IsExtensionAvailable(OAL_AL_EXT_OFFSET))
	{		
		float fTime;
        RUN_AL_FUNC(alGetSourcef(mlObjectId,AL_SEC_OFFSET, &fTime));
		fElapsed += fTime;
	}

	if(fElapsed>mpAudioData->GetTotalTime())
		fElapsed-=mpAudioData->GetTotalTime();

	return fElapsed;
}

//--------------------------------------------------------------------------------

double cOAL_Source::GetTotalTime()
{
	if(mpAudioData)
		return mpAudioData->GetTotalTime();
	
	return 0;
}

//--------------------------------------------------------------------------------

int	cOAL_Source::BindData(cOAL_Sample *apSample)
{
	Stop();

	if(apSample==NULL || 
		apSample->GetStatus()==false)
		return -1;
	
	mbNeedsReset = true;
	
	mpAudioData = apSample;
	if(LowLevelSet(eOAL_SourceParam_Buffer) == false)
		return -1;

	mpAudioData->AddBoundSource(this);
	IncRefCount();

	//LogMsg("",eOAL_LogVerbose_High,eOAL_LogMsg_Info, "Sample %s attached\n", mpAudioData->GetFilename().c_str());

	return GetPackedHandle(mlRefCount, mlId);
}

//--------------------------------------------------------------------------------

int	cOAL_Source::BindData(cOAL_Stream *apStream)
{
	Stop();

	if(apStream==NULL || 
		apStream->GetStatus()==false || 
		apStream->IsLocked())
		return -1;

	mbNeedsReset = true;

	mpAudioData = apStream;
	mlPriority = 256;
	apStream->Seek(0);
	if(LowLevelSet(eOAL_SourceParam_Buffer) == false)
		return -1;

    mpAudioData->AddBoundSource ( this );
	//mpSourceManager->AddActiveStream(this);
	IncRefCount();

	//LogMsg("",eOAL_LogVerbose_High,eOAL_LogMsg_Info, "Stream %s attached\n", mpAudioData->GetFilename().c_str());

	return GetPackedHandle(mlRefCount, mlId);
}

//--------------------------------------------------------------------------------

void cOAL_Source::Play()
{
	if(mbPlaying)
		return;
	
	mbPlaying = true;

	if(mbPaused==false)
		LowLevelPlay();
}

//--------------------------------------------------------------------------------

void cOAL_Source::Stop ( bool abRemove )
{
	DEF_FUNC_NAME("cOAL_Source::Stop");
	FUNC_USES_AL;

	if(!mbNeedsReset)
		return;

	LowLevelStop();

	SetLoop(false);
	SetGain(0);
	SetPitch(1);

	if(gpDevice->IsEFXActive())
	{
        SetDirectFilter(NULL);
		SetFilterEnabled(false, 0xFFFFFFFF);
		SetFilterGain(1);
		SetFilterGainHF(1);
		SetFilterGainLF(1);
		SetFilterType(eOALFilterType_Null);
		for(int i=0; i<(int)mvSends.size(); ++i)
			SetAuxSend(i,NULL,NULL);
	}
	mlPriority = 255;
	mbPaused = false;
	mbPlaying = false;
	mbNeedsReset = false;

	/*
    // No buffer attached so leave now while you still can
    if(mSourceType==eOAL_SourceType_Undetermined)
        return;*/

	if(GetQueuedBuffers()!=0)
	{
		RUN_AL_FUNC(alSourcei (mlObjectId,AL_BUFFER,AL_NONE));
	}
	
	if(mpAudioData)
	{
		mpSourceManager->ReleaseVoices(mpAudioData->GetChannels());
		
		// If Stop() is called from the cOAL_Sample destructor, the following will make some iterator used go mad and
		// crash, kinda a workaround 
		if(abRemove) 
			mpAudioData->RemoveBoundSource(this);
		mpAudioData = NULL;
	}
}

//--------------------------------------------------------------------------------

void cOAL_Source::Update()
{
	//Check if source needs update
	eOAL_SourceStatus sourceStatus = GetSourceStatus();

	if(sourceStatus!=eOAL_SourceStatus_Busy &&
	   sourceStatus!=eOAL_SourceStatus_Busy_BufferUnderrun )
			return;

	// Check if the sound data is still healthy
	if(mpAudioData==NULL || 
		mpAudioData->GetStatus()==false) {
		Stop();
		return;
	}

	//hpl::Log(" OAL: Updating source %p audiodata: %p - mbPlaying %d status %d elapsed %f/%f\n",this,mpAudioData, mbPlaying, sourceStatus, GetElapsedTime(),GetTotalTime());
	mpAudioData->Update();

	// If the source had a buffer underrun (this means it ran out of data and stopped), tell it to continue playing
	if(sourceStatus==eOAL_SourceStatus_Busy_BufferUnderrun)
	{
		//OAL_Source_Log(mlObjectId, 1,"Buffer underrun occured");
		LowLevelPlay();
	}
}

//--------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void SetGain(float afGain)
//	-	Sets the Source Gain (Volume)
///////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::SetGain(float afGain)
{
	if(afGain<=0) afGain=0.00001f;
	if(afGain>1) afGain=1;

	mfGain = afGain;
	LowLevelSet(eOAL_SourceParam_Gain);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////
//	void SetPitch(float afPitch)
//	-	Sets the Source Pitch ( Frequency multiple )
//////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::SetPitch(float afPitch)
{
	if(afPitch<=0) afPitch=0.0001f;
	if(afPitch>10) afPitch=10;

	mfPitch = afPitch;
	LowLevelSet(eOAL_SourceParam_Pitch);
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////
//	void SetPaused(const bool abPaused)
//	-	Sets if the Source should pause or not
//////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::Pause(bool abPaused)
{
	if(abPaused)
	{
		if(mbPaused==false)
			LowLevelPause();
	}
	else
	{
		if(mbPaused)
			LowLevelPlay();
	}
	mbPaused=abPaused;
}

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////
//	void SetLoop ( const bool abLoop )
//	-	Sets if the Source should loop
//////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::SetLoop(const bool abLoop)
{
	DEF_FUNC_NAME("cOAL_Source::SetLoop()");
	FUNC_USES_AL;

	//LogMsg("",eOAL_LogVerbose_High,eOAL_LogMsg_Info, "mbLoop set to %s\n", abLoop?"true":"false");
	mbLoop = abLoop;

	switch(GetSourceType())
	{
	case eOAL_AudioDataType_Sample:
		RUN_AL_FUNC(alSourcei(mlObjectId, AL_LOOPING, (ALint)abLoop));
		break;
	}
}

//////////////////////////////////////////////////////
//	void SetMinMaxDistance ( const float afMin, const float afMax )
//	-	Sets the source falloff distances
//////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::SetMinMaxDistance(const float afMin, const float afMax)
{
	DEF_FUNC_NAME("cOAL_Source::SeMinMaxDistance()");
	FUNC_USES_AL;

    RUN_AL_FUNC(alSourcef(mlObjectId, AL_REFERENCE_DISTANCE, afMin));
    RUN_AL_FUNC(alSourcef(mlObjectId, AL_MAX_DISTANCE, afMax));
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetElapsedTime(double afTime)
{
	DEF_FUNC_NAME("cOAL_Source::SetElapsedTime()");
	FUNC_USES_AL;

	switch(GetSourceType())
	{
	case eOAL_AudioDataType_Sample:
		RUN_AL_FUNC(alSourcef(mlObjectId, AL_SEC_OFFSET, (float)afTime));
		break;
	case eOAL_AudioDataType_Stream:
		{
			cOAL_Stream* pStream = (cOAL_Stream*)mpAudioData;
			pStream->Seek((float)afTime/mpAudioData->GetTotalTime());
		}
		break;
	}
}

void cOAL_Source::SetPriority(unsigned int alX)
{ 
	if(GetSourceType()!=eOAL_AudioDataType_Stream) 
	{
		mlPriority = alX; 
		if (mlPriority > 255) 
			mlPriority = 255; 
	}
	else
		mlPriority = 256;
}

//--------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void SetAttributes ( const float *apPos, const float* apVel )
//	-	Sets Source Position and Velocity ( for 3D sound )
///////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cOAL_Source::SetPosition ( const float* apPos )
{
	mvPos[0] = apPos[0];
	mvPos[1] = apPos[1];
	mvPos[2] = apPos[2];
	LowLevelSet(eOAL_SourceParam_Position);
}

void cOAL_Source::SetPositionRelative( bool abRelative )
{
    mbRelativePosition = abRelative;
    LowLevelSet(eOAL_SourceParam_PositionRelative);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetVelocity ( const float* apVel )
{
	mvVel[0] = apVel[0];
	mvVel[1] = apVel[1];
	mvVel[2] = apVel[2];
	LowLevelSet(eOAL_SourceParam_Velocity);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetDirectFilter ( cOAL_Filter* apFilter )
{
	if(!gpDevice->IsEFXActive())
		return;

	DEF_FUNC_NAME("cOAL_Source::SetDirectFilter()");
	FUNC_USES_AL;

	ALuint lFilter;

	mpDirectFilter = apFilter;

	if(apFilter!=NULL)
		lFilter = apFilter->GetObjectID();
	else
		lFilter = AL_FILTER_NULL;
	
	RUN_AL_FUNC(alSourcei(mlObjectId, AL_DIRECT_FILTER, lFilter));
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetAuxSend( int alSendId, cOAL_EffectSlot* apSlot, cOAL_Filter* apFilter)
{
	if(!gpDevice->IsEFXActive())
		return;

	DEF_FUNC_NAME("");
	FUNC_USES_AL;
	
	if(alSendId<0 || 
	   alSendId >= gpDevice->GetEFXSends())
		return;
	cOAL_SourceSend* pSend = mvSends[alSendId];

	pSend->SetSlot(apSlot);
	pSend->SetFilter(apFilter);

	RUN_AL_FUNC(alSource3i(mlObjectId, AL_AUXILIARY_SEND_FILTER, pSend->GetSlot(), alSendId, pSend->GetFilter()));
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetAuxSendSlot( int alSendId, cOAL_EffectSlot* apSlot )
{
	if(!gpDevice->IsEFXActive())
		return;

	DEF_FUNC_NAME("");
	FUNC_USES_AL;
	
	if(alSendId<0 || 
	   alSendId >= gpDevice->GetEFXSends())
		return;
	cOAL_SourceSend* pSend = mvSends[alSendId];

	pSend->SetSlot(apSlot);
		
	RUN_AL_FUNC(alSource3i(mlObjectId, AL_AUXILIARY_SEND_FILTER, pSend->GetSlot(), alSendId, pSend->GetFilter()));
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetAuxSendFilter( int alSendId, cOAL_Filter* apFilter )
{
	if(!gpDevice->IsEFXActive())
		return;

	DEF_FUNC_NAME("");
	FUNC_USES_AL;
	
	if(alSendId<0 || 
	   alSendId>= gpDevice->GetEFXSends())
		return;
	cOAL_SourceSend* pSend = mvSends[alSendId];

	pSend->SetFilter(apFilter);
	
	RUN_AL_FUNC(alSource3i(mlObjectId, AL_AUXILIARY_SEND_FILTER, pSend->GetSlot(), alSendId, pSend->GetFilter()));
}

//--------------------------------------------------------------------------------

void cOAL_Source::UpdateFiltering(unsigned int alSends)
{
	if(!gpDevice->IsEFXActive())
		return;

	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if(mpDirectFilter)
		RUN_AL_FUNC(alSourcei(mlObjectId, AL_DIRECT_FILTER, mpDirectFilter->GetObjectID()););

	cOAL_SourceSend* pSend = NULL;

	for(int i=0; i<(int)mvSends.size(); ++i)
	{
		pSend = mvSends[i];
		RUN_AL_FUNC(alSource3i(mlObjectId, AL_AUXILIARY_SEND_FILTER, pSend->GetSlot(), i, pSend->GetFilter()));
	}
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetFilterType(eOALFilterType aType )
{
	if (!gpDevice->IsEFXActive())
		return;
	
	mpFilter->SetType(aType);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetFilterGain (float afGain)
{
	if (!gpDevice->IsEFXActive())
		return;
	
	mpFilter->SetGain(afGain);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetFilterGainHF(float afGainHF)
{
	if (!gpDevice->IsEFXActive())
		return;
	
	mpFilter->SetGainHF(afGainHF);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetFilterGainLF(float afGainLF)
{
	if (!gpDevice->IsEFXActive())
		return;
	
	mpFilter->SetGainLF(afGainLF);
}

//--------------------------------------------------------------------------------

void cOAL_Source::SetFilterEnabled(bool abEnabled, int alFlags)
{
	if(!gpDevice->IsEFXActive())
		return;

	if(alFlags & 0x00000001)
	{
		if(abEnabled)
			SetDirectFilter(mpFilter);
		else
			SetDirectFilter(NULL);
	}
	if(alFlags & 0x00000002)
	{
		if(abEnabled)
			SetAuxSendFilter(0,mpFilter);
		else
			SetAuxSendFilter(0,NULL);
	}
}

//--------------------------------------------------------------------------------

void cOAL_Source::UpdateFiltering()
{
	SetFilterEnabled(true, 0x3);
}

//--------------------------------------------------------------------------------
/*
void cOAL_Source::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq < mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);
	char sourceId[20];

	sprintf(sourceId, "Source no. %d - ",mlId);

	
	switch(aMessageType)
	{
	case eOAL_LogMsg_Command:
		sMessage.append("[COMMAND] ");
		break;
	case eOAL_LogMsg_Info:
		sMessage.append("[INFO] ");
		break;
	case eOAL_LogMsg_Error:
		sMessage.append("[ERROR] ");
	default:
		break;
	}

	sMessage.append(sourceId).append(text);

	Write(sMessage);
}
*/
//--------------------------------------------------------------------------------

