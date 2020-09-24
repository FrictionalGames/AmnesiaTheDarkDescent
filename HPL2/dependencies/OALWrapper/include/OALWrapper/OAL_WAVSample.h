/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_OggSample.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#ifndef _OAL_WAVSAMPLE_H
#define _OAL_WAVSAMPLE_H

#include "OAL_Sample.h"
#include "OAL_Helper.h"

//-----------------------------------------------------------

class cOAL_WAVSample : public cOAL_Sample
{
public:	
	bool CreateFromFile(const std::wstring& asFilename);
	bool CreateFromBuffer(const void *apBuffer, size_t aSize);
protected:
};

//-----------------------------------------------------------

#endif	// _OAL_SAMPLE_H
