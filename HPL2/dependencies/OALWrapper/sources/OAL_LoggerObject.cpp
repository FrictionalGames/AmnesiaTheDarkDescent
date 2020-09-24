/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */

#include "OALWrapper/OAL_LoggerObject.h"
#include "OALWrapper/OAL_Helper.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#if defined(WIN32)
#define UNICODE
#include <shlobj.h>
#endif

using namespace std;

static wstring BuildLogFilename ( const string& asFilename );


bool					iOAL_LoggerObject::mbLogEnabled = false;
eOAL_LogOutput			iOAL_LoggerObject::mLogOutput = eOAL_LogOutput_File;
eOAL_LogVerbose	iOAL_LoggerObject::mLogVerboseLevel = eOAL_LogVerbose_Low;
wstring					iOAL_LoggerObject::msLogFile = BuildLogFilename("OAL.log");

//---------------------------------------------------------------------------------------

void iOAL_LoggerObject::SetLogFilename ( const string& asLogFilename )
{
	msLogFile = BuildLogFilename ( asLogFilename );
}

//---------------------------------------------------------------------------------------

void iOAL_LoggerObject::LogMsg(const string& asIDStr, eOAL_LogVerbose aVerbose, eOAL_LogMsg aType, const char* asMessage, ...)
{
	if(!mbLogEnabled)
		return;
	if(mLogVerboseLevel < aVerbose)
		return;

	if(asMessage==NULL)
		return;
	
	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);

	switch(aType)
	{
	case eOAL_LogMsg_Command:
		sMessage.append("[COMMAND] ");
		break;
	case eOAL_LogMsg_Info:
		sMessage.append("[INFO] ");
		break;
	case eOAL_LogMsg_Error:
		sMessage.append("[ERROR] ");
	}

	sMessage.append(asIDStr.c_str()).append(text);

	Write(sMessage);

}

//---------------------------------------------------------------------------------------

void iOAL_LoggerObject::Write( const string& asMessage )
{
	if (!mbLogEnabled)
		return;

	FILE* fLog;

	switch(mLogOutput)
	{
	case eOAL_LogOutput_File:
        fLog = OpenFileW(msLogFile, L"a");
		if (fLog != NULL)
		{
			fwrite(asMessage.c_str(), sizeof(char), asMessage.size(), fLog);
			fclose(fLog);
		}
		break;
	case eOAL_LogOutput_Console:
		printf("%s",asMessage.c_str());
		break;
	default:
		break;
	}
}

//---------------------------------------------------------------------------------------

wstring BuildLogFilename ( const string& asFilename )
{
	wstring wsName;
	wstring wsTemp;
	wsName = String2WString(asFilename);
	
	#if defined(WIN32)

		WCHAR sPath[MAX_PATH];
		if(SUCCEEDED(SHGetFolderPath(NULL, 
			CSIDL_PERSONAL | CSIDL_FLAG_CREATE, 
			NULL,0,sPath))) 
		{
			wsTemp = wstring(sPath).append(L"/").append(wsName);
		}
		else
		{
			return L"";
		}
	#else
		string home = string(getenv("HOME"));
		wsTemp = String2WString(home);
		wsTemp.append(L"/").append(wsName);
	#endif

	FILE* pTempFile = NULL;
	wchar_t buffer[100];

	for (unsigned int i = 1; i != 0; i++)
	{
		swprintf(buffer, 100, L"_%d.log", i);

		pTempFile = OpenFileW(wstring(wsTemp).append(wstring(buffer)),L"r");
		if (pTempFile)
		{
			fclose(pTempFile);
			//_wremove (iOAL_LoggerObject::GetLogFilename().c_str());
		}
		else
		{
			wsTemp.append(wstring(buffer));
			break;
		}
	}
	return wsTemp;
}

//---------------------------------------------------------------------------------------
