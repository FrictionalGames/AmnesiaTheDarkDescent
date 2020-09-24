/**
	@file OAL_Sample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Definition of Base class for containing samples
*/

#include "OpenAL/OAL_Types.h"
#include "OpenAL/OAL_Device.h"
#include "OpenAL/OAL_Helper.h"
#include "OpenAL/OAL_Sample.h"
#include "OpenAL/OAL_Buffer.h"
#include "OpenAL/OAL_Source.h"

#include "system/MemoryManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"


//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Initialize ( void )
//	-	Creates a Buffer Object for the sample data
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

cOAL_Sample::cOAL_Sample() : iOAL_AudioData(eOAL_AudioDataType_Sample,1)
{
	
}

//------------------------------------------------------------------

cOAL_Sample::~cOAL_Sample() 
{
	if(mlstBoundSources.empty()==false)
	{
		tSourceListIt it;
		for(it=mlstBoundSources.begin(); it!=mlstBoundSources.end(); ++it)
		{
			cOAL_Source* pSource = *it;
			pSource->Stop(false);
		}
		mlstBoundSources.clear();
	}
}		

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	
//
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

ALuint* cOAL_Sample::GetOALBufferPointer()
{
	 return mvBuffers[0]->GetObjectIDPointer();
}


/*
string cOAL_Sample::GetDebugInfo()
{
	char temp[200];
		
	sprintf(temp, "Sample %s, Time %f, channels: %d\n", msFileName.c_str(), mfTotalTime, mlChannels);
	return string(temp);
}
*/

//------------------------------------------------------------------
/*
void cOAL_Sample::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq < mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);
	char sampleId[20];

	sprintf(sampleId, "Sample %s - ", msFilename.c_str());

	
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
	sMessage.append(sampleId).append(text);

	Write(sMessage);
}
*/
//------------------------------------------------------------------
