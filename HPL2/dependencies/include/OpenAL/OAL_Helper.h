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

#define DEF_FUNC_NAME(X) string sFuncName(#X);

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

bool			OAL_GetALError ();
string			OAL_GetALErrorString();	

bool			OAL_GetALCError ();
string			OAL_GetALCErrorString();	

//-----------------------------------------------------------------------------------------

void ClearALErrors( const string& asFunction );
bool CheckALErrors( const string& asFunc1, const string& asFunc2 );
void ClearALCErrors( const string& asFunction );
bool CheckALCErrors( const string& asFunction );

//-----------------------------------------------------------------------------------------



#endif	// _OAL_HELPER_H
