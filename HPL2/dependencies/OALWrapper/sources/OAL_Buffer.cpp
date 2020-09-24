/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Sample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Definition of Base class for containing samples
*/

#include "OALWrapper/OAL_Buffer.h"

#include "OALWrapper/OAL_Types.h"
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Source.h"



//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

cOAL_Buffer::cOAL_Buffer(iOAL_AudioData* apParent) : iOAL_LowLevelObject("Buffer")
{
	CreateLowLevelID();
	mpParent = apParent;
}

//------------------------------------------------------------------

cOAL_Buffer::~cOAL_Buffer() 
{
	DestroyLowLevelID();
}		

//------------------------------------------------------------------

bool cOAL_Buffer::CreateLowLevelID ()
{
	DEF_FUNC_NAME("cOAL_Buffer::CreateLowLevelID()");
	FUNC_USES_AL;

	// Create a Buffer Object which will contain the Buffer name
	RUN_AL_FUNC(alGenBuffers( 1, &mlObjectId);)
	
	return (!AL_ERROR_OCCURED && (IsValidObject()));
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	
//	
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cOAL_Buffer::DestroyLowLevelID ()
{
    DEF_FUNC_NAME("cOAL_Buffer::DestroyLowLevelID()");
	FUNC_USES_AL;

	bool bSuccess = true;
	if ( IsValidObject() )
	{
		RUN_AL_FUNC(alDeleteBuffers( 1, &mlObjectId ));
		
		bSuccess = bSuccess && !AL_ERROR_OCCURED;
	}
	return bSuccess;
}

//------------------------------------------------------------------

bool cOAL_Buffer::IsValidObject()
{
	return (alIsBuffer(mlObjectId) == AL_TRUE);
}

//------------------------------------------------------------------

bool cOAL_Buffer::Feed(ALvoid* apPCMData, int alDataSize, double afStartTime)
{
	DEF_FUNC_NAME(cOAL_Buffer::Feed);
	FUNC_USES_AL;

	RUN_AL_FUNC(alBufferData(mlObjectId, mpParent->GetFormat(), apPCMData, alDataSize, mpParent->GetFrequency()));
	mfStartTime = afStartTime;

	if(AL_ERROR_OCCURED==false)
	{
		mpParent->mlBuffersUsed++;
		mfBufferTime = ((float)alDataSize)/(float)mpParent->GetChannels()/(float)mpParent->GetBytesPerSample()/(float)mpParent->GetFrequency();
	}

    return AL_ERROR_OCCURED==false;
}

//------------------------------------------------------------------

/*
string cOAL_Sample::GetDebugInfo()
{
	char temp[200];
		
	sprintf(temp, "Sample %s, Time %f, channels: %d\n", msFileName.c_str(), mfTotalTime, mlChannels);
	return string(temp);
}
*/

//------------------------------------------------------------------
