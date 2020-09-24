/**
	@file OAL_Device.cpp
	@author Luis Rodero
	@date 2006-10-27
	@version 0.1
	Implementation of the cOAL_Device class
*/

#include "OpenAL/OAL_Device.h"
#include "OpenAL/OAL_Source.h"
#include "OpenAL/OAL_OggSample.h"
#include "OpenAL/OAL_WAVSample.h"
#include "OpenAL/OAL_OggStream.h"
#include "OpenAL/OAL_SourceManager.h"

#include "OpenAL/OAL_Filter.h"
#include "OpenAL/OAL_Effect_Reverb.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include <algorithm>
#include <cstring>

//-------------------------------------------------------------------------

//////////////////////////////////////////////
// Human readable extension names 

string sExtensionNames[NUM_EXTENSIONS+1] = 
{ 
	string("ALC_EXT_CAPTURE" ),
	string("ALC_EXT_EFX"),
	string("ALC_ENUMERATION_EXT"),
	string("ALC_ENUMERATE_ALL_EXT"),
	string("AL_EXT_OFFSET"),
	string("AL_EXT_LINEAR_DISTANCE"),
	string("AL_EXT_EXPONENT_DISTANCE"),
	string("EAX"),
	string("EAX2.0"),
	string("EAX3.0"),
	string("EAX4.0"),
	string("EAX5.0"),
	string("EAX-RAM"),
	string("No extension")
};

//-------------------------------------------------------------------------

cOAL_Device::cOAL_Device ( ) : mpDevice(NULL), 
							   mpContext(NULL),
							   mpSourceManager(NULL), 
							   mpEFXManager(NULL),
							   mlEFXSends(0)
{
}

cOAL_Device::~cOAL_Device ()
{
}

//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////
// Inits the OpenAL device
///////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

bool cOAL_Device::Init( cOAL_Init_Params& acParams )
{
	DEF_FUNC_NAME(cOAL_Device::Init);
	FUNC_USES_ALC;
	FUNC_USES_AL;

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Command, "Initializing device: %s...\n", (acParams.msDeviceName.c_str()==NULL)? "\"preferred\"":acParams.msDeviceName.c_str() );

	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Configuring streaming options:\n");

	cOAL_Stream::SetBufferSize(acParams.mlStreamingBufferSize);
	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "\tSetting buffer size to %d bytes\n",cOAL_Stream::GetBufferSize());
	cOAL_Stream::SetBufferCount(acParams.mlStreamingBufferCount);
	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "\tSetting queue length to %d buffers\n",cOAL_Stream::GetBufferCount());

	
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Attempting to open device...\n" );
	// Open the device, if fails return false
	if(acParams.msDeviceName.empty())
		mpDevice = alcOpenDevice(NULL);
	else
        mpDevice = alcOpenDevice(acParams.msDeviceName.c_str());
	
	if(mpDevice == NULL)
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Error, "Error opening device\n" );
		return false;
	}
	
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Success.\n" );
	
	// Get ALC extensions ( ie EFX )
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Checking present ALC extensions\n" );
	for (int i = 0; i < NUM_ALC_EXTENSIONS; ++i)
	{
		mvbExtensions[i] = RUN_ALC_FUNC((alcIsExtensionPresent(mpDevice,sExtensionNames[i].c_str()) == ALC_TRUE));
		if (mvbExtensions[i])
		{
			LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "\t%s\n",sExtensionNames[i].c_str() );
		}
	}

	ALCint lAttrList[] = 
	{
		ALC_FREQUENCY,		acParams.mlOutputFreq,
		#ifdef __APPLE__
		#else
		ALC_MONO_SOURCES,	acParams.mbVoiceManagement ? 256 : acParams.mlMinMonoSourcesHint,
		ALC_STEREO_SOURCES,	acParams.mbVoiceManagement ? 0 : acParams.mlMinStereoSourcesHint,
		#endif
		ALC_MAX_AUXILIARY_SENDS, acParams.mlNumSendsHint,
		0,
	};

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Creating context\n");
	// Create and set a context
	mpContext = RUN_ALC_FUNC(alcCreateContext ( mpDevice, lAttrList ));

	RUN_ALC_FUNC(alcMakeContextCurrent ( mpContext ));

	if (ALC_ERROR_OCCURED)
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Error, "Error creating context\n");
		return false;
	}

	/////////////////////////////////////////////////
	//Retrieve device info, such as extensions
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Checking present AL extensions\n" );
	for (int i = NUM_ALC_EXTENSIONS; i < NUM_EXTENSIONS; ++i)
	{
		mvbExtensions[i] = RUN_AL_FUNC((alIsExtensionPresent(sExtensionNames[i].c_str()) == AL_TRUE));
		if (mvbExtensions[i])
			LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "\t%s\n",sExtensionNames[i].c_str() );
	}

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Retrieving Output Devices\n");
	vector<string> llDevices = GetOutputDevices();
	vector<string>::iterator it;
	for (it = llDevices.begin(); it != llDevices.end(); ++it) {
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "\t%s\n", (*it).c_str());
	}

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Retrieving info\n" );

	// Get device name
	msDeviceName = RUN_ALC_FUNC(alcGetString(mpDevice, ALC_DEVICE_SPECIFIER));

	// Get vendor name (just fancy stuff,not very useful)
	msVendorName = RUN_ALC_FUNC(alGetString( AL_VENDOR ));

	//Get renderer info
	msRenderer = RUN_ALC_FUNC(alGetString ( AL_RENDERER ));

	// Get the OpenAL impl. version
	RUN_ALC_FUNC(alcGetIntegerv ( mpDevice, ALC_MAJOR_VERSION, sizeof(ALCint), &mlMajorVersion ));
	RUN_ALC_FUNC(alcGetIntegerv ( mpDevice, ALC_MINOR_VERSION, sizeof(ALCint), &mlMinorVersion ));

	RUN_ALC_FUNC(alcGetIntegerv ( mpDevice, ALC_MAX_AUXILIARY_SENDS, sizeof(ALCint), &mlEFXSends));

	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Device name: %s\n", msDeviceName.c_str() );
	LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "OpenAL version: %d.%d\n", mlMajorVersion, mlMinorVersion );

	// Check device version
	if ( (mlMajorVersion < acParams.mlMajorVersionReq) ||
			((mlMajorVersion == acParams.mlMajorVersionReq) && (mlMinorVersion < acParams.mlMinorVersionReq)) )
	{
		LogMsg("",eOAL_LogVerbose_None, eOAL_LogMsg_Error, "Version required: %d.%d\n",acParams.mlMajorVersionReq,acParams.mlMinorVersionReq);
		return false;
	}

	//	If alSourceNumHint == -1, create as many sources as possible
	if (acParams.mlNumSourcesHint == -1)
		acParams.mlNumSourcesHint = 4096;

	/////////////////////////////////////////////////
	//Start EFX if requested
    if (acParams.mbUseEFX && IsExtensionAvailable (OAL_ALC_EXT_EFX))
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Starting EFX on request\n" );
		mpEFXManager = hplNew( cOAL_EFXManager, () );
		mbEFXActive = mpEFXManager->Initialize( acParams.mlNumSlotsHint, mlEFXSends, acParams.mbUseThread, acParams.mlSlotUpdateFreq );
		if (!mbEFXActive)
		{
			mpEFXManager->Destroy();
			hplDelete(mpEFXManager);
		}
	}

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Creating Source Manager\n" );
	//Create The source manager
	mpSourceManager = hplNew( cOAL_SourceManager, () );
	if ( mpSourceManager->Initialize( acParams.mbVoiceManagement, acParams.mlNumSourcesHint, acParams.mbUseThread, acParams.mlUpdateFreq, mlEFXSends ) == false)
	{
		LogMsg("",eOAL_LogVerbose_None, eOAL_LogMsg_Error, "Error creating Source Manager\n");
		return false;
	}
	
	return true;
}

//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

void cOAL_Device::Close ()
{
	DEF_FUNC_NAME("cOAL_Device::Close()");
	FUNC_USES_ALC;

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Command, "Closing device...\n" );
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Cleaning up Samples...\n" );
	//Delete samples
	{
		for(tSampleListIt it = mlstSamples.begin(); it!=mlstSamples.end(); ++it )
			hplDelete((*it));
		mlstSamples.clear();
	}
	
	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Cleaning up Streams...\n" );
	{
		for (tStreamListIt it=mlstStreams.begin();it!=mlstStreams.end(); ++it )
			hplDelete((*it));
		mlstStreams.clear();
	}

	if(mpSourceManager) 
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Cleaning up Source Manager...\n" );
		mpSourceManager->Destroy();
		hplDelete(mpSourceManager);
		mpSourceManager = NULL;
	}

	if (mpEFXManager)
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Cleaning up EFX Manager...\n" );
		mpEFXManager->Destroy();
		hplDelete(mpEFXManager);
		mpEFXManager = NULL;
	}

	LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Cleaning up Context and closing Low Level Device...\n" );
	//Delete device and context.
	RUN_ALC_FUNC(alcMakeContextCurrent ( NULL ));
	RUN_ALC_FUNC(alcDestroyContext ( mpContext ));
	RUN_ALC_FUNC(alcCloseDevice ( mpDevice ));

	if (!ALC_ERROR_OCCURED)
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Info, "Done\n" );
}

//-------------------------------------------------------------------------

bool cOAL_Device::RegainContext()
{
	DEF_FUNC_NAME("cOAL_Device::RegainContext");
	FUNC_USES_ALC;

	if (mpContext == NULL)
		return false;
	bool bSuccess = RUN_ALC_FUNC((alcMakeContextCurrent(mpContext) == AL_TRUE));
	
	if (bSuccess)
	{
		RUN_ALC_FUNC(alcProcessContext(mpContext));
	}
	return bSuccess;
}

//-------------------------------------------------------------------------

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Loaders
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

cOAL_Sample* cOAL_Device::LoadSample(const wstring& asFilename)
{
	cOAL_Sample *pSample = NULL;

	// Check file format and load the sample data according to it
	wstring strExt = hpl::cString::GetFileExtW(asFilename);
	if(strExt.compare(L"ogg") == 0 )						// Load an Ogg Vorbis sample
		pSample = hplNew(cOAL_OggSample,());
	else if(strExt.compare(L"wav")==0)				// Load a .WAV sample
		pSample = hplNew(cOAL_WAVSample,());
	
	if(pSample && pSample->CreateFromFile(asFilename) )
		mlstSamples.push_back(pSample);
	else
	{
		hplDelete(pSample);
		pSample = NULL;
	}

	//hpl::Log(" OAL: Loaded sample %p from file: '%s'\n", pSample,hpl::cString::To8Char(asFilename).c_str());

	return pSample;
}

//-------------------------------------------------------------------------

cOAL_Stream* cOAL_Device::LoadStream(const wstring &asFilename)
{
   	cOAL_Stream *pStream = NULL;

	wstring strExt = hpl::cString::GetFileExtW(asFilename);

	/////////////////////////////////
	//Make sure extension is correct
	if(strExt.compare(L"ogg")==0)
	{
		pStream = hplNew(cOAL_OggStream,());
	}
	else
	{
		hpl::Error("File '%s' does not have ogg as extension!\n", hpl::cString::To8Char(asFilename).c_str());
		return NULL;
	}
	
	/////////////////////////////////
	//Create from file
	if(pStream->CreateFromFile(asFilename))
	{
		mlstStreams.push_back(pStream);
		//hpl::Log("Loaded stream %s\n", pStream->msName.c_str());
	}
	else
	{
		hplDelete(pStream);
		pStream = NULL;
	}

	//hpl::Log(" OAL: Loaded stream %p from file: '%s'\n", pStream,hpl::cString::To8Char(asFilename).c_str());

	return pStream;

}

//-------------------------------------------------------------------------

void  cOAL_Device::UnloadSample(cOAL_Sample* apSample)
{
	if(apSample == NULL)
		return;

	//hpl::Log(" OAL: UnLoading sample %p from file: '%s'\n", apSample,hpl::cString::To8Char(apSample->GetFilename()).c_str());
	
	mlstSamples.remove(apSample);
	hplDelete(apSample);
}

//-------------------------------------------------------------------------

void cOAL_Device::UnloadStream(cOAL_Stream* apStream)
{
	if(apStream == NULL)
		return;

	//hpl::Log(" OAL: UnLoading stream %p from file: '%s'\n", apStream,hpl::cString::To8Char(apStream->GetFilename()).c_str());
    
	mlstStreams.remove(apStream);
	hplDelete(apStream);
}

//-------------------------------------------------------------------------

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Listener Management
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

void cOAL_Device::SetListenerGain ( float afGain )
{
	DEF_FUNC_NAME ("cOAL_Device::SetListenerGain()");
	FUNC_USES_AL;

	if ( afGain < 0 )
		afGain = 0;
	if ( afGain > 1 )
		afGain = 1;

	RUN_AL_FUNC(alListenerf ( AL_GAIN, afGain ));
}

//-------------------------------------------------------------------------

void cOAL_Device::SetListenerPosition (const float* apPos )
{
	DEF_FUNC_NAME ("cOAL_Device::SetListenerPosition()");
	FUNC_USES_AL;

	RUN_AL_FUNC(alListenerfv ( AL_POSITION, apPos ));
}

//-------------------------------------------------------------------------

void cOAL_Device::SetListenerVelocity (const float* apVel )
{
	DEF_FUNC_NAME ("cOAL_Device::SetListenerVelocity()");
	FUNC_USES_AL;

	RUN_AL_FUNC(alListenerfv (AL_VELOCITY,apVel));
}

//-------------------------------------------------------------------------

void cOAL_Device::SetListenerOrientation(const float* apForward, const float* apUp)
{
	DEF_FUNC_NAME ("cOAL_Device::SetListenerOrientation()");
	FUNC_USES_AL;

	float fOrientation[6] = { apForward[0], apForward[1], apForward[2], apUp[0], apUp[1], apUp[2] };
	
	RUN_AL_FUNC(alListenerfv ( AL_ORIENTATION, fOrientation ));
}

//-------------------------------------------------------------------------

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Playback Management
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------

int cOAL_Device::PlaySample( int alSource, cOAL_Sample *apSample, int alPriority, float afVolume, bool abStartPaused )
{
	if (apSample == NULL) 
		return -1;

	cOAL_Source *pSource = NULL;
	if (alSource == OAL_FREE)
		pSource = mpSourceManager->GetAvailableSource( alPriority, apSample->GetChannels() );
	else
		pSource = mpSourceManager->GetSource(alSource,true);

	if (pSource == NULL)
		return -1;

	int lSourceHandle;

	pSource->Lock();
	{
		lSourceHandle = pSource->BindData(apSample);
		pSource->SetPriority(alPriority);
		pSource->SetGain(afVolume);
		pSource->Pause(abStartPaused);
		pSource->Play();
	}
	pSource->Unlock();

	return lSourceHandle;
}

//-------------------------------------------------------------------------

int cOAL_Device::PlayStream( int alSource, cOAL_Stream *apStream, float afVolume, bool abStartPaused )
{
	if (apStream == NULL)
		return -1;

	cOAL_Source *pSource = NULL;
	if (alSource == OAL_FREE)
		pSource = mpSourceManager->GetAvailableSource( 256, apStream->GetChannels() );
	else
		pSource = mpSourceManager->GetSource(alSource,true);
	
	if (pSource == NULL)
		return -1;

	int lSourceHandle;

	pSource->Lock();
	{
		lSourceHandle = pSource->BindData(apStream);
		pSource->SetGain(afVolume);
		pSource->Pause(abStartPaused);
		pSource->Play();
	}
	pSource->Unlock();

	if (lSourceHandle != -1) mpSourceManager->AddActiveStream(pSource);

	return lSourceHandle;
}

//-------------------------------------------------------------------------

cOAL_Source* cOAL_Device::GetSource( int alSourceHandle )
{
	if (mpSourceManager)
		return mpSourceManager->GetSource(alSourceHandle);

	return NULL;
}

//-------------------------------------------------------------------------

string& cOAL_Device::GetExtensionName(int alWhich) 
{ 
	if( (alWhich >= 0) && (alWhich < NUM_EXTENSIONS)) 
		return sExtensionNames[alWhich]; 
	else 
		return sExtensionNames[NUM_EXTENSIONS];
}

//-------------------------------------------------------------------------

string cOAL_Device::GetDefaultDeviceName()
{
	DEF_FUNC_NAME(cOAL_Device::GetDefaultDeviceName);
	FUNC_USES_ALC;

	string sDev = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	
	return sDev;
}

//-------------------------------------------------------------------------

vector<string> cOAL_Device::GetOutputDevices()
{
	DEF_FUNC_NAME(cOAL_Device::GetOutputDevices);
	FUNC_USES_ALC;

	vector<string> devices;
	bool bEnumerate = RUN_ALC_FUNC(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE);
	bool bEnumerate_all = RUN_ALC_FUNC(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATE_ALL_EXT") == AL_TRUE);
	if (bEnumerate) {
		const char *s;
		if (bEnumerate_all) {
			// walk devices
			s = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		} else {
			s = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		}
		while (*s != '\0') {
			devices.push_back(string(s));
			// find next string
			while (*s++ != '\0')
				;
		}
	}
	return devices;
}

//-------------------------------------------------------------------------

void cOAL_Device::SetDistanceModel(eOAL_DistanceModel aModel)
{
	DEF_FUNC_NAME("OAL_SetDistanceModel");
	FUNC_USES_AL;

	ALenum distModel;
	switch(aModel)
	{
	case eOAL_DistanceModel_Inverse:
		distModel = AL_INVERSE_DISTANCE;
		break;
	case eOAL_DistanceModel_Inverse_Clamped:
		distModel = AL_INVERSE_DISTANCE_CLAMPED;
		break;
	case eOAL_DistanceModel_Linear:
		distModel = AL_LINEAR_DISTANCE;
		break;
	case eOAL_DistanceModel_Linear_Clamped:
		distModel = AL_LINEAR_DISTANCE_CLAMPED;
		break;
	case eOAL_DistanceModel_Exponent:
		distModel = AL_EXPONENT_DISTANCE;
		break;
	case eOAL_DistanceModel_Exponent_Clamped:
		distModel = AL_EXPONENT_DISTANCE_CLAMPED;
		break;
	case eOAL_DistanceModel_None:
		distModel = AL_NONE;
	default:
		break;
	}

	RUN_AL_FUNC(alDistanceModel(distModel));
}

//-------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
/*
void cOAL_Device::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq > mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);

	
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

	sMessage.append("Device - ").append(text);

	Write(sMessage);
}
*/
//-------------------------------------------------------------------------
