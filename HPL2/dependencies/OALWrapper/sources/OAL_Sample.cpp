/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Sample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Definition of Base class for containing samples
*/

#include "OALWrapper/OAL_Types.h"
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Sample.h"
#include "OALWrapper/OAL_Buffer.h"
#include "OALWrapper/OAL_Source.h"



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
