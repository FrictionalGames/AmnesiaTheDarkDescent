/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Filter.h"
#include "OALWrapper/OAL_Helper.h"

extern cOAL_Device* gpDevice;

cOAL_Filter::cOAL_Filter () : iOAL_LowLevelObject("Filter"), 
							  mfGain(1.0f), 
							  mfGainHF(1.0f), 
							  mfGainLF(1.0f), 
							  mType(eOALFilterType_Null)
{
	mbStatus = CreateLowLevelID();
}



cOAL_Filter::~cOAL_Filter ()
{
	DestroyLowLevelID();
}



bool cOAL_Filter::CreateLowLevelID()
{
    DEF_FUNC_NAME("cOAL_Filter::CreateLowLevelID()");
	FUNC_USES_AL;

	// Create a Filter Object
	RUN_AL_FUNC(alGenFilters ( 1, &mlObjectId );)
	
	return (!AL_ERROR_OCCURED && (IsValidObject()));
}



bool cOAL_Filter::DestroyLowLevelID()
{
    DEF_FUNC_NAME("cOAL_Filter::DestroyLowLevelID()");
	FUNC_USES_AL;

	bool bSuccess = true;
	if ( IsValidObject() )
	{
		RUN_AL_FUNC(alDeleteFilters( 1, &mlObjectId ));
		
		bSuccess = bSuccess && !AL_ERROR_OCCURED;
	}
	return bSuccess;
}



bool cOAL_Filter::IsValidObject()
{
	return (alIsFilter(mlObjectId) == AL_TRUE);
}

void cOAL_Filter::SetType(eOALFilterType aType)
{
	DEF_FUNC_NAME (cOAL_Filter::SetType);
	FUNC_USES_AL;

	int lType;
	mType = aType;

	switch (mType)
	{
	case eOALFilterType_LowPass:
		lType = AL_FILTER_LOWPASS;
		break;
	case eOALFilterType_HighPass:
		lType = AL_FILTER_HIGHPASS;
		break;
	case eOALFilterType_BandPass:
		lType = AL_FILTER_BANDPASS;
		break;
	default:
		lType = AL_FILTER_NULL;
		break;
	}

	RUN_AL_FUNC ( alFilteri (mlObjectId, AL_FILTER_TYPE, lType) );
	mbStatus = !AL_ERROR_OCCURED;
}

void cOAL_Filter::SetGain (float afGain)
{
	if (mType == eOALFilterType_Null)
		return;

	DEF_FUNC_NAME (cOAL_Filter::SetGain);
	FUNC_USES_AL;

	if (afGain > 1.0f)
		afGain = 1.0f;
	else if (afGain < 0.0f)
		afGain = 0.0f;

	mfGain = afGain;

	RUN_AL_FUNC ( alFilterf (mlObjectId, 0x0001, mfGain) );
}

void cOAL_Filter::SetGainHF(float afGainHF)
{
	if ((mType != eOALFilterType_LowPass) && (mType != eOALFilterType_BandPass))
		return;

	DEF_FUNC_NAME (cOAL_Filter::SetGainHF);
	FUNC_USES_AL;

	if (afGainHF > 1.0f)
		afGainHF = 1.0f;
	else if (afGainHF < 0.0f)
		afGainHF = 0.0f;

	int lParam;

	switch (mType)
	{
	case eOALFilterType_LowPass:
		lParam = AL_LOWPASS_GAINHF;
		break;
	case eOALFilterType_BandPass:
		lParam = AL_BANDPASS_GAINHF;
		break;
	default:
		break;
	}

	mfGainHF = afGainHF;

	RUN_AL_FUNC ( alFilterf (mlObjectId, lParam, mfGainHF) );
}

void cOAL_Filter::SetGainLF(float afGainLF)
{
	if ((mType != eOALFilterType_HighPass) && (mType != eOALFilterType_BandPass))
		return;

	DEF_FUNC_NAME (cOAL_Filter::SetGainLF);
	FUNC_USES_AL;

	if (afGainLF > 1.0f)
		afGainLF = 1.0f;
	else if (afGainLF < 0.0f)
		afGainLF = 0.0f;

	int lParam;

	switch(mType)
	{
	case eOALFilterType_HighPass:
		lParam = AL_HIGHPASS_GAINLF;
		break;
	case eOALFilterType_BandPass:
		lParam = AL_BANDPASS_GAINLF;
		break;
	default:
		break;
	}

	mfGainLF = afGainLF;

	RUN_AL_FUNC(alFilterf(mlObjectId, lParam, mfGainLF));
}

