/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_CustomStream.cpp
	@author Edward Rudd
	@date 2013-04-16
	@version 0.1
	Definition of Base class for containing Custom streams
*/


#include "OALWrapper/OAL_CustomStream.h"
#include "OALWrapper/OAL_Buffer.h"
#include <cstdio>
#include <cstdlib>

cOAL_CustomStream::cOAL_CustomStream(const tStreamCallbacks& aCallbacks, const tStreamInfo& aInfo, void *apData)
	: mCallbacks(aCallbacks), mInfo(aInfo), mpData(apData)
{
	mlChannels = aInfo.channels;
	mlFrequency = aInfo.frequency;
	mFormat = aInfo.format;
	mlSamples = aInfo.samples;
	mfTotalTime = aInfo.totalTime;
	if (mCallbacks.Init)
		mCallbacks.Init(mpData);
}

cOAL_CustomStream::~cOAL_CustomStream()
{
	Destroy();
}

bool cOAL_CustomStream::Stream(cOAL_Buffer* apDestBuffer)
{
	bool ret = false;
	if (mCallbacks.Stream)
		ret =  mCallbacks.Stream(mpData, apDestBuffer, mpPCMBuffer, mlBufferSize, mbEOF);
	if (!ret) {
		mbStatus = false;
	}
	return ret;
}

void cOAL_CustomStream::Seek(float afWhere, bool abForceRebuffer)
{
	mbEOF = false;
	
	if(afWhere < 0)
		afWhere = 0;
	if(afWhere > 1)
		afWhere = 1;

	if (mCallbacks.Seek)
		mCallbacks.Seek(mpData, afWhere, abForceRebuffer);

	if(abForceRebuffer)
		mbNeedsRebuffering = true;
}

double cOAL_CustomStream::GetTime()
{
	if (mCallbacks.GetTime) {
		return mCallbacks.GetTime(mpData);
	} else {
		return 0;
	}
}

void cOAL_CustomStream::Destroy()
{
	if (mCallbacks.Destroy)
		mCallbacks.Destroy(mpData);
}
