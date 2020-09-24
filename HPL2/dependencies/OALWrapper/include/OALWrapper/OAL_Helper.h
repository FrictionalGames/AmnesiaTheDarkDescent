/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Helper.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
    Set of Helper functions
*/
#ifndef _OAL_HELPER_H
#define _OAL_HELPER_H

//-----------------------------------------------------------------------------------------

extern bool gbLogSounds;

extern void OAL_Log(int alMessageType, const char *asMessage,...);
extern void OAL_Source_Log(int mlSourceId, int alMessageType, const char *asMessage,...);

//-----------------------------------------------------------------------------------------

#include "OAL_Types.h"

//-----------------------------------------------------------------------------------------

#define DEF_FUNC_NAME(X) std::string sFuncName(#X);

#define FUNC_USES_AL  bool bALError = false; ClearALErrors(sFuncName);
#define FUNC_USES_ALC bool bALCError = false; ClearALCErrors(sFuncName);

#define RUN_AL_FUNC(f) \
		f; \
		bALError = OAL_GetALError(); \
		if ( bALError && iOAL_LoggerObject::IsLogEnabled() ) \
			iOAL_LoggerObject::LogMsg("",eOAL_LogVerbose_None,eOAL_LogMsg_Error, "%s: %s raised %s\n", #f, sFuncName.c_str(), OAL_GetALErrorString().c_str()); 

#define RUN_ALC_FUNC(f) \
		f; \
		bALCError = OAL_GetALCError(); \
		if ( bALCError && iOAL_LoggerObject::IsLogEnabled() ) \
			iOAL_LoggerObject::LogMsg("", eOAL_LogVerbose_None,eOAL_LogMsg_Error, "%s: %s raised %s\n", #f, sFuncName.c_str(), OAL_GetALCErrorString().c_str()); 

#define AL_ERROR_OCCURED bALError
#define ALC_ERROR_OCCURED bALCError

//-----------------------------------------------------------------------------------------

std::string	GetExtension(const std::string& asFilename);
std::wstring GetExtensionW(const std::wstring& asFilename);

//-----------------------------------------------------------------------------------------

bool			OAL_GetALError ();
std::string			OAL_GetALErrorString();	

bool			OAL_GetALCError ();
std::string			OAL_GetALCErrorString();	

std::string			WString2String(const std::wstring& asWString);
std::wstring			String2WString(const std::string& asString);

FILE *          OpenFileW(const std::wstring& asFileName, const std::wstring asMode);
FILE *          OpenFile(const std::string& asFileName, const std::string asMode);

//-----------------------------------------------------------------------------------------

void ClearALErrors( const std::string& asFunction );
bool CheckALErrors( const std::string& asFunc1, const std::string& asFunc2 );
void ClearALCErrors( const std::string& asFunction );
bool CheckALCErrors( const std::string& asFunction );

//-----------------------------------------------------------------------------------------



#endif	// _OAL_HELPER_H
