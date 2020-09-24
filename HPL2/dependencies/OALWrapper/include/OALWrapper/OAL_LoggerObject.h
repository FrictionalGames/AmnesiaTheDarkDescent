/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_LOGGEROBJECT_H
#define _OAL_LOGGEROBJECT_H

#include "OAL_Types.h"

//---------------------------------------------------------------------------------------

typedef enum
{
	eOAL_LogMsg_Text,
	eOAL_LogMsg_Command,
	eOAL_LogMsg_Info,
	eOAL_LogMsg_Error,
	eOAL_LogMsg_Default,
} eOAL_LogMsg;


typedef enum 
{
	eOAL_LogVerbose_None,
	eOAL_LogVerbose_Low,
	eOAL_LogVerbose_Medium,
	eOAL_LogVerbose_High,
	eOAL_LogVerbose_Default,
} eOAL_LogVerbose;


typedef enum
{
	eOAL_LogOutput_File,
	eOAL_LogOutput_Console,
	eOAL_LogOutput_Default,
} eOAL_LogOutput;

//---------------------------------------------------------------------------------------

class iOAL_LoggerObject
{
public:
	static void LogMsg(const std::string& asIDStr,eOAL_LogVerbose aVerbose, eOAL_LogMsg aType, const char* asMessage, ...);
	
	static void Write(const std::string& asMessage);

	inline static void SetLogEnabled ( bool abEnable ) { mbLogEnabled = abEnable; }
	inline static void SetLogOutput ( eOAL_LogOutput aOutput ) { mLogOutput = aOutput; }
	inline static void SetLogVerbose ( eOAL_LogVerbose aLevel ) { mLogVerboseLevel = aLevel; }
	static void SetLogFilename ( const std::string& asLogFilename );


	inline static bool IsLogEnabled () { return mbLogEnabled; }
	inline static eOAL_LogOutput GetLogOutput ( ) { return mLogOutput; }
	inline static eOAL_LogVerbose GetLogVerboseLevel () { return mLogVerboseLevel; }
	inline static std::wstring GetLogFilename ( ) { return msLogFile; }

protected:
	static bool					mbLogEnabled;
	static eOAL_LogOutput		mLogOutput;
	static eOAL_LogVerbose		mLogVerboseLevel;
	static std::wstring				msLogFile;

};

//---------------------------------------------------------------------------------------

#endif

