/**
	@file OAL_Init.h
	@author Luis Rodero
	@date 2006-10-10
	@version 0.1
	Set of functions for Initialization/Shutdown, plus misc funcs
*/
#ifndef _OAL_INIT_H
#define _OAL_INIT_H

#include "OAL_Types.h"
#include "OAL_LoggerObject.h"


/////////////////////////////////////////
// Initialization Parameters

class cOAL_Init_Params
{
public:
	cOAL_Init_Params() :  msDeviceName("NULL"), mlMajorVersionReq(1), mlMinorVersionReq(0), 
						mlOutputFreq(44100), mbUseThread(true), mlUpdateFreq(10), 
						mlNumSourcesHint(32), mbVoiceManagement(true), mlMinMonoSourcesHint(0),
						mlMinStereoSourcesHint(0), mlStreamingBufferSize(1), mlStreamingBufferCount(4),
						mbUseEFX(false), mlNumSlotsHint(4), mlNumSendsHint(4), mlSlotUpdateFreq(15)
	{
	}

	~cOAL_Init_Params(){}

	string	msDeviceName;
	int		mlMajorVersionReq;
	int		mlMinorVersionReq;
	int		mlOutputFreq;
	bool	mbUseThread;
	int		mlUpdateFreq;
	int		mlNumSourcesHint;
	bool	mbVoiceManagement;
	int		mlMinMonoSourcesHint;
	int		mlMinStereoSourcesHint;
	int		mlStreamingBufferSize;
	int		mlStreamingBufferCount;
	bool	mbUseEFX;
	int		mlNumSlotsHint;
	int		mlNumSendsHint;
	int		mlSlotUpdateFreq;
};

/////////////////////////////////////////

/*bool	OAL_Init (	const char* asDeviceName = NULL, int alOutputFreq = 44100, 
					bool abUseThread = true, int alUpdateFreq = 100, 
					int alReqMajorVersion = 1, int alReqMinorVersion = 1,
					int alNumSourcesHint = 32, bool mbVoiceManagement = true,
					int alMinMonoSourceHint = 0, int alMinStereoSourceHint = 0,
					int alStreamingBufferSize = 1, int alStreamingBufferCount = 4);
*/
bool	OAL_Init ( cOAL_Init_Params& acParams );
void	OAL_Close ( void );

void	OAL_Update ( void );
void	OAL_SetRollOffFactor ( const float afFactor );
void	OAL_SetDistanceModel ( eOAL_DistanceModel aeModel );


const char*	OAL_Info_GetDeviceName();
const char*	OAL_Info_GetVendorName();
const char*	OAL_Info_GetRendererName();
const int	OAL_Info_GetMajorVersion();
const int	OAL_Info_GetMinorVersion();
const int	OAL_Info_GetNumSources();
const bool	OAL_Info_IsEFXActive();
int OAL_Info_GetStreamBufferCount();
int OAL_Info_GetStreamBufferSize();
string		OAL_Info_GetDefaultOutputDevice();
vector<string> OAL_Info_GetOutputDevices();


void OAL_LogSourcePoolStatus();
void OAL_SetupLogging ( bool abLogSounds, eOAL_LogOutput aeOutput = eOAL_LogOutput_File, eOAL_LogVerbose aVerbose = eOAL_LogVerbose_Low,  string asLogFilename = "OAL.Log" );
void OAL_Log(eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMsg,  const char* asMessage, ... );
#endif	// _OAL_INIT_H
