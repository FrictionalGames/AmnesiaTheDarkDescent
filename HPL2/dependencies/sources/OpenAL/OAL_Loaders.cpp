/**
	@file OAL_Loaders.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Implementation of Loading/Unloading functions
*/

#include "OpenAL/OAL_Loaders.h"
#include "OpenAL/OAL_Device.h"
#include "OpenAL/OAL_Sample.h"
#include "OpenAL/OAL_Stream.h"

extern cOAL_Device* gpDevice;

//------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cOAL_Sample* OAL_Sample_Load(const wstring& asFilename)
{
	if(gpDevice==NULL) return NULL;

	return gpDevice->LoadSample(asFilename);
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

cOAL_Stream* OAL_Stream_Load(const wstring& asFilename)
{
	if(gpDevice==NULL) return NULL;
	
	return gpDevice->LoadStream(asFilename);
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

