/**
	@file OAL_WAVSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#include "OpenAL/OAL_WAVSample.h"
#include "OpenAL/OAL_Buffer.h"
#include "OpenAL/OAL_Helper.h"
#include "OpenAL/OAL_Device.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Load ( const string &asFilename )
//	-	Loads sample data from a WAV file
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cOAL_WAVSample::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_WAVSample::Load()");
	FUNC_USES_AL;
	
	if(mbStatus==false)
		return false;

	Reset();

	ALenum	status;
	ALvoid	*pPCMBuffer = NULL;
	ALsizei	lSize;

	msFilename = asFilename;

	// TEMP: Need to get rid of the ALUT funcs to use Unicode here :S
	string sFilename = hpl::cString::To8Char(asFilename);
	///////////////////////////////////////////////////////////////
	// This worked indeed, but didnt return a freeable pointer. 
	// Will be used when some fix is found
	//pPCMBuffer = alutLoadMemoryFromFile ( asFilename.c_str(), &eFormat, &lDataSize, &fFrequency );
	
	#if defined(__APPLE__)
	alutLoadWAVFile ( (ALbyte*) sFilename.c_str(), &mFormat, &pPCMBuffer, &lSize, &mlFrequency);
	#else
	alutLoadWAVFile ( (ALbyte*) sFilename.c_str(), &mFormat, &pPCMBuffer, &lSize, &mlFrequency, AL_FALSE);
	#endif
	status = alutGetError ();
	switch (status)
	{
		case ALUT_ERROR_NO_ERROR:
			break;
        default:
			mbStatus = false;
			break;
	}

	cOAL_Buffer* pBuffer = mvBuffers.front();
	if(pBuffer->Feed(pPCMBuffer, lSize)==false)
	{
		mlBuffersUsed = 1;
		//hplFree ( pPCMBuffer );
		alutUnloadWAV( mFormat, pPCMBuffer, lSize, mlFrequency);
		status = alutGetError();
		mbStatus = false;
		return false;
	}

	RUN_AL_FUNC(alGetBufferi( pBuffer->GetObjectID(), AL_CHANNELS, &mlChannels ));
	
	mlSamples = lSize/(mlChannels*GetBytesPerSample());

	mfTotalTime = ((double)mlSamples)/mlFrequency;
	
	
//	hplFree ( pPCMBuffer );
	alutUnloadWAV( mFormat, pPCMBuffer, lSize, mlFrequency);
	status = alutGetError ();

	return true;
}

//------------------------------------------------------------------


