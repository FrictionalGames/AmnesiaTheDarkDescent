/**
	@file OAL_Playback.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Set of functions to manage playback and such in OpenAL
*/
#include "OpenAL/OAL_Types.h"
#include "OpenAL/OAL_Playback.h"
#include "OpenAL/OAL_Source.h"
#include "OpenAL/OAL_Device.h"

extern cOAL_Device* gpDevice;

using namespace std;

//////////////////////////////////////////////////////////////////
////	OAL_Test_Sample_Play ( const cOAL_Sample* apSample )		
////	- Plays a loaded Sample and waits till it ends playing
//////////////////////////////////////////////////////////////////

void OAL_Test_Sample_Play ( cOAL_Sample* apSample )
{
	/*
	if ( apSample )
	{
		cOAL_Source* pTmpSource;
		int lSampleState = 0;
        
		pTmpSource = (*gpSourceList)[0];
		
		pTmpSource->PlaySample( apSample, false, 1.0f );
		while (pTmpSource->GetSourceStatus() == eOALSourceStatus_Busy_Playing )
		{}
	}
	*/
}


//////////////////////////////////////////////////////////////////
////	OAL_Test_Stream_Play ( cOAL_Sample* apStream )
////	- Plays a Stream and waits till it ends playing
//////////////////////////////////////////////////////////////////

void OAL_Test_Stream_Play ( cOAL_Stream* apStream )
{
	/*
	if ( apStream )
	{
		cOAL_Source* pTempSource;
		int lStreamState = 0;

		OAL_Source_Stop ( OAL_ALL );

		pTempSource = (*gpSourceList)[0];
        
		pTempSource->PlayStream( apStream, false, 1.0f );

		while (pTempSource->GetSourceStatus() == eOALSourceStatus_Busy_Playing )
		{
			pTempSource->Update();
		}

	}
	*/
}


//////////////////////////////////////////////////////////////////
////	int OAL_Sample_Play ( int alSource, const cOAL_Sample* apSample )		
////	-	Begins play of a loaded Sample on the specified source. If alSource == OAL_FREE, 
////		it looks for a free source if there is any, if not, it selects that which has the lowest
////		priority below the sample's own prio
//////////////////////////////////////////////////////////////////

int OAL_Sample_Play (	int alSource, cOAL_Sample* apSample, float afVolume, bool abStartPaused,
						int alPriority)
{
	if (gpDevice == NULL) return -1;
	return gpDevice->PlaySample(alSource, apSample,alPriority,afVolume,abStartPaused);
}

//////////////////////////////////////////////////////////////////
////	int OAL_Stream_Play ( int alSource, const cOAL_Stream* apStream, float afVolume, bool abStartPaused )		
////	-	Begins play of an open Stream on the specified source. If alSource == OAL_FREE, 
////		it looks for a free source if there is any, if not, it selects the source with lowest prio
//////////////////////////////////////////////////////////////////

int OAL_Stream_Play ( int alSource, cOAL_Stream* apStream, float afVolume, bool abStartPaused )
{
	if (gpDevice == NULL) return -1;
	return gpDevice->PlayStream(alSource, apStream,afVolume,abStartPaused);
}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_Stop ( int alSource )		
////	-	Stops the specified source.
//////////////////////////////////////////////////////////////////

void OAL_Source_Stop ( int alSource )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);

	if (pSource)
	{
		pSource->Lock();
		pSource->Stop();
		pSource->Unlock();
	}
}

///////////////////////////////////////////////////////////
////	void OAL_Source_SetVolume ( int alSource, float afVolume )
////	-	Sets the volume for the specified Source
///////////////////////////////////////////////////////////

void OAL_Source_SetVolume ( int alSource, float afVolume )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetGain(afVolume);
		pSource->Unlock();
	}
}
//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetPitch ( int alSource, const float afPitch )		
////	-	Sets pitch value for specified source
//////////////////////////////////////////////////////////////////

void OAL_Source_SetPitch ( int alSource, float afPitch )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetPitch(afPitch);
		pSource->Unlock();
	}

}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetPaused ( int alSource, bool abPaused )		
////	-	Pauses the specified source.
//////////////////////////////////////////////////////////////////

void OAL_Source_SetPaused ( int alSource, bool abPaused )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->Pause(abPaused);
		pSource->Unlock();
	}
}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetLoop ( int alSource, bool abPaused )		
////	-	Pauses the specified source.
//////////////////////////////////////////////////////////////////

void OAL_Source_SetLoop ( int alSource, bool abLoop )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetLoop(abLoop);
		pSource->Unlock();
	}
}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetAttributes ( const int alSource, const float* apPos, const float* apVel )
////	-	Sets 3D audio attributes for the specified source.
//////////////////////////////////////////////////////////////////

void OAL_Source_SetAttributes ( const int alSource, const float* apPos, const float* apVel )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetPosition(apPos);
		pSource->SetVelocity(apVel);
		pSource->Unlock();
	}
}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetMinMaxDistance ( const int alSource, const float afMin, const float afMax )
////	-	Sets min and max distance values for the specified source.
//////////////////////////////////////////////////////////////////

void OAL_Source_SetMinMaxDistance ( const int alSource, const float afMin, const float afMax )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		//pSource->SetMinMaxDistance(afMin, afMax);
		pSource->Unlock();
	}
}

//////////////////////////////////////////////////////////////////
////	void OAL_Source_SetPositionRelative ( const int alSource, const bool abRelative )
////	-	Sets if the specified source should use coordinates relative to the Listener.
//////////////////////////////////////////////////////////////////

void OAL_Source_SetPositionRelative ( const int alSource, const bool abRelative )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
//		pSource->SetPositionRelative(abRelative);
		pSource->Unlock();
	}
}

//////////////////////////////////////////////////////////////////
////	void OAL_Listener_SetAttributes ( const float* apPos, const float* apVel, const float* apForward, const float* apUp )
////	-	Sets 3D audio attributes for the listener.
//////////////////////////////////////////////////////////////////

void OAL_Listener_SetAttributes ( const float* apPos, const float* apVel, const float* apForward, const float* apUp )
{
	if (gpDevice == NULL) return;
	gpDevice->SetListenerPosition(apPos);
	gpDevice->SetListenerVelocity(apVel);
	gpDevice->SetListenerOrientation(apForward, apUp);
}

///////////////////////////////////////////////////////////
////	void OAL_Listener_SetMasterVolume ( const float afVolume )
////	-	Sets the master volume
///////////////////////////////////////////////////////////

void OAL_Listener_SetMasterVolume ( float afVolume )
{
	if (gpDevice == NULL) return;
	gpDevice->SetListenerGain(afVolume);
}

///////////////////////////////////////////////////////////
////	void OAL_Source_SetPriority ( const int alSource, const unsigned int alPriority )
////	-	Sets the priority of the specified source
///////////////////////////////////////////////////////////

void OAL_Source_SetPriority ( const int alSource, const unsigned int alPriority )
{
	if (gpDevice == NULL) return;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetPriority(alPriority);
		pSource->Unlock();
	}
}

///////////////////////////////////////////////////////////
////	const unsigned int OAL_Source_GetPriority ( const int alSource )
////	-	Returns the source priority
///////////////////////////////////////////////////////////

unsigned int OAL_Source_GetPriority ( const int alSource )
{
	if (gpDevice == NULL) return 0;
	int lPriority = 0;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		lPriority = pSource->GetPriority();
		pSource->Unlock();
	}
	
	return lPriority;

}

///////////////////////////////////////////////////////////
////	const bool OAL_Source_IsPlaying ( const int alSource )
////	-	Returns true if the specified source is playing audio
///////////////////////////////////////////////////////////

const bool	OAL_Source_IsPlaying ( const int alSource )
{
	if (gpDevice == NULL) return false;
	bool bIsPlaying = false;

	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		eOAL_SourceStatus eStatus = pSource->GetSourceStatus();
		pSource->Unlock();

		bIsPlaying = (eStatus != eOAL_SourceStatus_Free);
	}
	return bIsPlaying;
}
	

const bool	OAL_Source_IsBufferUnderrun ( const int alSource )
{
	return false;
}


void	OAL_Source_SetElapsedTime( const int alSource, double afTime )
{
	if (gpDevice == NULL) return;
	
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		pSource->SetElapsedTime(afTime);
		pSource->Unlock();
	}
}

double	OAL_Source_GetElapsedTime( const int alSource )
{
	if (gpDevice == NULL) return 0;
	double fElapsedTime = 0;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		fElapsedTime = pSource->GetElapsedTime();
		pSource->Unlock();
	}
	
	return fElapsedTime;
}

double	OAL_Source_GetTotalTime( const int alSource )
{
	if (gpDevice == NULL) return 0;
	double fTotalTime = 0;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		fTotalTime = pSource->GetTotalTime();
		pSource->Unlock();
	}

	return fTotalTime;
}

float OAL_Source_GetGain( const int alSource )
{
	if (gpDevice == NULL) return 0;
	float fGain = 0.0f;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		fGain = pSource->GetGain();
		pSource->Unlock();
	}
	return fGain;
}

float OAL_Source_GetPitch( const int alSource )
{
	if (gpDevice == NULL) return 0;
	float fPitch = 0.0f;
	cOAL_Source* pSource = gpDevice->GetSource(alSource);
	if (pSource)
	{
		pSource->Lock();
		fPitch = pSource->GetPitch();
		pSource->Unlock();
	}
	return fPitch;
}

void OAL_Source_Log ( int alSourceId, int alMessageType,  const char* asMessage, ... )
{
	/*
	if (!gbLogSounds)
		return;
	if ( !IsValidSourceId (alSourceId))
		return;

	string sMessage;

	switch ( alMessageType )
	{
	case 0:
		sMessage.append("[COMMAND] ");
		break;
	case 1:
		sMessage.append("[INFO] ");
		break;
	case 2:
		sMessage.append("[ERROR] ");
	default:
		break;
	}

	char text[2048];
	va_list ap;
	if (asMessage == NULL)
		return;	
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);

	sMessage.append(text);

	string sMess = "";
	sMess += text;
	
	char sBuffer[255];

	sprintf(sBuffer, "./OAL/OAL_Source_%d.log", alSourceId);
    string sFileName(sBuffer);
	
    FILE* fSourceLog = fopen(sFileName.c_str(),"a");

	if (fSourceLog != NULL)
	{
		fwrite(sMessage.c_str(), sizeof(char), sMessage.size(), fSourceLog);
		fclose(fSourceLog);
	}
	*/
}
