/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Sample.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Declaration of Base class for containing samples
*/

#ifndef _OAL_SAMPLE_H
#define _OAL_SAMPLE_H

#include "OAL_Types.h"
#include "OAL_Helper.h"
#include "OAL_Source.h"
#include "OAL_AudioData.h"

class cOAL_Sample : public iOAL_AudioData
{
public:
	
	cOAL_Sample();
	virtual ~cOAL_Sample();

	void Destroy(){}

	void AddBoundSource(cOAL_Source* apSource) { if(apSource) mlstBoundSources.push_back(apSource); }
	void RemoveBoundSource(cOAL_Source* apSource) { if(apSource) mlstBoundSources.remove(apSource); }

	void Update(){}

	ALuint* GetOALBufferPointer();

	double GetProcessedBuffersTime() { return 0; }

	bool HasBufferUnderrun() { return false; }
	bool NeedsRebuffering()  { return false; }

	//void LogMsg("",eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aeMessageType, const char* asMessage, ...);

	//string	GetDebugInfo();
protected:
	tSourceList mlstBoundSources;
};




#endif	// _OAL_SAMPLE_H
