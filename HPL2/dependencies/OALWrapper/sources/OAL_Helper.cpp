/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Helper.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
    Set of Helper functions
*/

#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Device.h"

#include <cstdlib>
#include <cstdio>

using namespace std;

ALenum geALErrorState = AL_NO_ERROR;
ALCenum geALCErrorState = ALC_NO_ERROR;

///////////////////////////////////////////////////////////
//	string GetExtension ( const string& asFilename )
//	-	Returns the file extension in lowercase
///////////////////////////////////////////////////////////

string GetExtension(const string& asFilename)
{
	wstring strTemp = GetExtensionW(String2WString(asFilename));

	return WString2String(strTemp);
}

wstring GetExtensionW(const wstring& asFilename)
{
	int lNameOffset = (int) asFilename.find_last_of ('.')+1;

	wstring strExt = asFilename.substr( lNameOffset, asFilename.size() - lNameOffset );
	for ( int i = 0; i < (int) strExt.size(); ++i )
		strExt[i] = tolower( strExt[i] );

	return strExt;
}

///////////////////////////////////////////////////////////
//
// Wide char to byte char and back again convertion utils
///////////////////////////////////////////////////////////

string WString2String(const wstring& asWString)
{
		string sTemp;
		size_t needed = wcstombs(NULL,&asWString[0],asWString.length());
		sTemp.resize(needed);
		wcstombs(&sTemp[0],&asWString[0],asWString.length());
		return sTemp;
}

wstring String2WString(const string& asString)
{
		wstring wsTemp;
		size_t needed = mbstowcs(NULL,&asString[0],asString.length());
		wsTemp.resize(needed);
		mbstowcs(&wsTemp[0],&asString[0],asString.length());
		return wsTemp;
}

///////////////////////////////////////////////////////////
//
// File open wrappers (to abstract out the wide character
///////////////////////////////////////////////////////////

FILE *OpenFileW(const wstring& asFileName, const wstring asMode)
{
#ifdef WIN32
    return _wfopen(asFileName.c_str(), asMode.c_str());
#else
    string sFileName = WString2String(asFileName);
    string sMode = WString2String(asMode);
    
	FILE *fileHandle = fopen(sFileName.c_str(),sMode.c_str());
    return fileHandle;
#endif
}

FILE *OpenFile(const string& asFileName, const string asMode)
{
	return fopen(asFileName.c_str(),asMode.c_str());
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

bool OAL_GetALError()
{
	geALErrorState = alGetError();
	return (geALErrorState != AL_NO_ERROR);
}

bool OAL_GetALCError()
{
	ALCcontext *ctx = alcGetCurrentContext();
	if (ctx==NULL) return false;
	geALCErrorState = alcGetError(alcGetContextsDevice(ctx));
	return (geALCErrorState != ALC_NO_ERROR);
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

string OAL_GetALErrorString( )
{
	switch (geALErrorState)
	{
	case AL_INVALID_VALUE:
		return string("AL_INVALID_VALUE");
	case AL_INVALID_ENUM:
		return string("AL_INVALID_ENUM");
	case AL_INVALID_NAME:
		return string("AL_INVALID_NAME");
	case AL_INVALID_OPERATION:
		return string("AL_INVALID_OPERATION");
	case AL_NO_ERROR:
		return string("AL_NO_ERROR");
	default:
		break;
	}
	return string("AL_NO_ERROR");
}

string OAL_GetALCErrorString( )
{
	switch (geALCErrorState)
	{
		case ALC_NO_ERROR:
			return string("ALC_NO_ERROR");
		case ALC_INVALID_DEVICE:
			return string("ALC_INVALID_DEVICE");
		case ALC_INVALID_CONTEXT:
			return string("ALC_INVALID_CONTEXT");
		case ALC_INVALID_ENUM:
			return string("ALC_INVALID_ENUM");
		case ALC_INVALID_VALUE:
			return string("ALC_INVALID_VALUE");
		case ALC_OUT_OF_MEMORY:
			return string("ALC_OUT_OF_MEMORY");
		default:
            break;
	}
	return string("ALC_NO_ERROR");
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

void ClearALErrors(const string& asFunction)
{
	ALenum eStatus = alGetError();
	//if ( eStatus != AL_NO_ERROR )
	//	OAL_Log(2, "%s ClearALErrors raised %d\n", asFunction.c_str(), eStatus );
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////


bool CheckALErrors(const string& asFunc1, const string& asFunc2)
{
	bool bErrorOccurred = OAL_GetALError();
	//if ( (bErrorOccured) && (cOAL_Device::IsLogEnabled()))
		//OAL_Log(2,"%s: %s raised %s\n", asFunc1.c_str(), asFunc2.c_str(), OAL_GetALErrorString().c_str() );
	return bErrorOccurred;
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

void ClearALCErrors(const string& asFunction)
{
    ALCcontext *ctx = alcGetCurrentContext();
    if (ctx == NULL) return;
	ALCenum eStatus = alcGetError(alcGetContextsDevice(ctx));
	//if ( eStatus != ALC_NO_ERROR )
	//s	OAL_Log(2, "%s ClearALCErrors raised %d\n", asFunction.c_str(), eStatus );
}

///////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////

bool CheckALCErrors(const string& asFunction)
{
	bool bErrorOccurred = OAL_GetALCError();
	//if ( (bErrorOccured) && (gbLogSounds) )
	//	OAL_Log(2,"%s CheckALCErrors raised %s\n", asFunction.c_str(), OAL_GetALCErrorString().c_str() );
	return bErrorOccurred;
}

