/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Loaders.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Implementation of Loading/Unloading functions
*/

#include "OALWrapper/OAL_Loaders.h"
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Sample.h"
#include "OALWrapper/OAL_Stream.h"

using namespace std;

extern cOAL_Device* gpDevice;

//------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cOAL_Sample* OAL_Sample_Load(const string &asFilename, eOAL_SampleFormat format)
{
	if (gpDevice == NULL) return NULL;

	return gpDevice->LoadSample(asFilename, format);
}

//------------------------------------------------------------------------

cOAL_Sample* OAL_Sample_Load(const wstring& asFilename, eOAL_SampleFormat format)
{
	if(gpDevice==NULL) return NULL;

	return gpDevice->LoadSample(asFilename, format);
}

cOAL_Sample* OAL_Sample_LoadFromBuffer(const void* apBuffer, size_t aSize, eOAL_SampleFormat format)
{
	if(gpDevice==NULL) return NULL;

	return gpDevice->LoadSampleFromBuffer(apBuffer, aSize, format);
}
//------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void OAL_Sample_Unload(cOAL_Sample* apSample)
{
	if (gpDevice == NULL) return;

	gpDevice->UnloadSample(apSample);
}

//------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cOAL_Stream* OAL_Stream_Load(const string &asFilename, eOAL_SampleFormat format)
{
	if (gpDevice == NULL) return NULL;

	return gpDevice->LoadStream(asFilename, format);
}

//------------------------------------------------------------------------

cOAL_Stream* OAL_Stream_Load(const wstring& asFilename, eOAL_SampleFormat fomat)
{
	if(gpDevice==NULL) return NULL;
	
	return gpDevice->LoadStream(asFilename, fomat);
}

//------------------------------------------------------------------------

cOAL_Stream* OAL_Stream_LoadFromBuffer(const void* apBuffer, size_t aSize, eOAL_SampleFormat format)
{
	if(gpDevice==NULL) return NULL;

	return gpDevice->LoadStreamFromBuffer(apBuffer, aSize, format);
}

//------------------------------------------------------------------------

cOAL_Stream*	OAL_Stream_LoadCustom ( const tStreamCallbacks& aCallback, const tStreamInfo& aInfo, void* apData )
{
	if(gpDevice==NULL) return NULL;
	return gpDevice->LoadCustomStream(aCallback, aInfo, apData);
}

//------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void OAL_Stream_Unload(cOAL_Stream* apStream)
{
	if (gpDevice == NULL) return;
	gpDevice->UnloadStream(apStream);
}

//------------------------------------------------------------------------

void OAL_Sample_SetLoop(cOAL_Sample* apSample, bool abLoop)
{
	if (gpDevice == NULL) return;

	if (apSample) 
		apSample->SetLoop(abLoop);
}

//------------------------------------------------------------------------

void OAL_Stream_SetLoop(cOAL_Stream* apStream, bool abLoop)
{
	if (gpDevice == NULL) return;

	if (apStream) 
		apStream->SetLoop(abLoop);
}

//------------------------------------------------------------------------

int	OAL_Sample_GetChannels(cOAL_Sample* apSample)
{
	if (gpDevice == NULL) return 0;

	if (apSample != NULL)
		return (apSample->GetChannels());

	return 0;
}

//------------------------------------------------------------------------

int	OAL_Stream_GetChannels(cOAL_Stream* apStream)
{
	if (gpDevice == NULL) return 0;

	if (apStream != NULL)
		return (apStream->GetChannels());

	return 0;
}

//------------------------------------------------------------------------

