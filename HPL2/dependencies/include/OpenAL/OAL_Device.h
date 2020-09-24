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

	bool ReInit ( const char* asDeviceName, const int alOutputFreq = 44100, 
				int alSourceNumHint = 32, int alReqMajorVersion = 1, 
				int alReqMinorVersion = 1,
				int alMaxMonoSourceHint=0, int alMaxStereoSourceHint =0);

	///////////////////////////////////////////////////////
	// Info methods
    
	inline string& GetDeviceName ()				{ return msDeviceName;	}
	inline string& GetVendorName ()				{ return msVendorName;	}
	inline string& GetRenderer ()					{ return msRenderer;	}
	inline int GetMajorVersion ()					{ return mlMajorVersion;}
	inline int GetMinorVersion ()					{ return mlMinorVersion;}
	inline int GetNumSources ()					{ if (mpSourceManager) return mpSourceManager->GetNumVoices(); else return 0;	}
	inline bool* GetExtensions ()					{ return mvbExtensions; }
	inline bool IsExtensionAvailable (int alWhich)	{ if ( (alWhich >= 0) && (alWhich < NUM_EXTENSIONS) ) return mvbExtensions[alWhich]; else return false; }
	inline bool IsEFXActive ()						{ return (mbEFXActive && (mpEFXManager!=NULL)); }
	inline int	GetEFXSends()						{ return mlEFXSends; } 
	static string& GetExtensionName(int alWhich);
	static string GetDefaultDeviceName();
	static vector<string> GetOutputDevices();

	
	
	inline const ALCdevice*		GetDevice()			{ return mpDevice; }
	inline cOAL_SourceManager*	GetSourceManager()	{ return mpSourceManager; }
	inline cOAL_EFXManager*		GetEFXManager()		{ return mpEFXManager; }

	bool					RegainContext ();

	void SetDistanceModel ( eOAL_DistanceModel aeModel );

	///////////////////////////////////////////////////////
	// Loaders
	cOAL_Sample* LoadSample(const wstring& asFileName);
	cOAL_Stream* LoadStream(const wstring& asFileName);
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
	string GetIDString();

private:
	string				msDeviceName;
	string				msVendorName;
	string				msRenderer;
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
