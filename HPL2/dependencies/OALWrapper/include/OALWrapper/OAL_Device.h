/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Device.h
	@author Luis Rodero
	@date 2006-10-27
	@version 0.1
	Class used for management of OpenAL Devices
*/

#ifndef _OAL_DEVICE_H
#define _OAL_DEVICE_H

#include "OAL_Types.h"
#include "OAL_Init.h"
#include "OAL_Helper.h"
#include "OAL_SourceManager.h"
#include "OAL_EFXManager.h"
#include "OAL_LoggerObject.h"


class cOAL_Device : public iOAL_LoggerObject
{
public:

	cOAL_Device ();
	~cOAL_Device ();


	///////////////////////////////////////////////////////
	// Init methods
	bool Init(cOAL_Init_Params& acParams);
	void Close();

	///////////////////////////////////////////////////////
	// Info methods
    
	inline std::string& GetDeviceName ()				{ return msDeviceName;	}
	inline std::string& GetVendorName ()				{ return msVendorName;	}
	inline std::string& GetRenderer ()					{ return msRenderer;	}
	inline int GetMajorVersion ()					{ return mlMajorVersion;}
	inline int GetMinorVersion ()					{ return mlMinorVersion;}
	inline int GetNumSources ()					{ if (mpSourceManager) return mpSourceManager->GetNumVoices(); else return 0;	}
	inline bool* GetExtensions ()					{ return mvbExtensions; }
	inline bool IsExtensionAvailable (int alWhich)	{ if ( (alWhich >= 0) && (alWhich < NUM_EXTENSIONS) ) return mvbExtensions[alWhich]; else return false; }
	inline bool IsEFXActive ()						{ return (mbEFXActive && (mpEFXManager!=NULL)); }
	inline int	GetEFXSends()						{ return mlEFXSends; } 
	static std::string& GetExtensionName(int alWhich); 
	static std::string GetDefaultDeviceName();
	static std::vector<std::string>GetOutputDevices();

	
	
	inline const ALCdevice*		GetDevice()			{ return mpDevice; }
	inline cOAL_SourceManager*	GetSourceManager()	{ return mpSourceManager; }
	inline cOAL_EFXManager*		GetEFXManager()		{ return mpEFXManager; }

	bool					RegainContext ();

	void SetDistanceModel ( eOAL_DistanceModel aeModel );

	///////////////////////////////////////////////////////
	// Loaders
	cOAL_Sample* LoadSample(const std::string& asFileName, eOAL_SampleFormat format);
	cOAL_Sample* LoadSample(const std::wstring& asFileName, eOAL_SampleFormat format);
	cOAL_Sample* LoadSampleFromBuffer(const void *apBuffer, size_t aSize, eOAL_SampleFormat format);
	cOAL_Stream* LoadStream(const std::string& asFileName, eOAL_SampleFormat format);
	cOAL_Stream* LoadStream(const std::wstring& asFileName, eOAL_SampleFormat format);
	cOAL_Stream* LoadStreamFromBuffer(const void *apBuffer, size_t aSize, eOAL_SampleFormat format);
	cOAL_Stream* LoadCustomStream(const tStreamCallbacks& aCallback, const tStreamInfo& aInfo, void* apData);
	void UnloadSample(cOAL_Sample* apSample);
	void UnloadStream(cOAL_Stream* apStream);

	///////////////////////////////////////////////////////
	// Playback / Source Management
	int PlaySample( int alSource, cOAL_Sample *apSample, int alPriority, float afVolume, bool abStartPaused );
	int PlayStream ( int alSource, cOAL_Stream *apStream, float afVolume, bool abStartPaused );

	cOAL_Source* GetSource ( int alSourceHandle );

	///////////////////////////////////////////////////////
	// Listener Management
	void SetListenerGain ( const float afGain );
	void SetListenerPosition (const float* apPos );
	void SetListenerVelocity (const float* apVel );
	void SetListenerOrientation (const float* apForward, const float* apUp);


	////////////////////////////////////////////////////////
	// Logging stuff
	std::string GetIDString();

private:
	std::string			msDeviceName;
	std::string			msVendorName;
	std::string			msRenderer;
	int					mlMajorVersion;
	int					mlMinorVersion;
	bool				mvbExtensions[NUM_EXTENSIONS];
	ALCdevice*			mpDevice;
	ALCcontext*			mpContext;


	cOAL_SourceManager*	mpSourceManager;

	cOAL_EFXManager*	mpEFXManager;
	bool				mbEFXActive;
	int					mlEFXSends;
   
	tSampleList mlstSamples;
	tStreamList	mlstStreams;
};

#endif	// _OAL_DEVICE_H
