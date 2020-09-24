/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Funcs.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Main header for the OpenAL Wrapper
*/

#ifndef _OAL_FUNCS_H
#define _OAL_FUNCS_H

#ifdef WIN32
#pragma comment(lib, "libvorbis.lib")
#pragma comment(lib, "libvorbisfile.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "SDL.lib")
#endif

#include "OAL_Types.h"

////////////////////////////////////////////
// Init / Shutdown funcs
////////////////////////////////////////////

#include "OAL_Init.h"

////////////////////////////////////////////
// Playback and Source Management Funcs 
////////////////////////////////////////////

#include "OAL_Playback.h"

////////////////////////////////////////////
// Sound Data Loaders
////////////////////////////////////////////

#include "OAL_Loaders.h"

////////////////////////////////////////////
// EFX Specific Funcs
////////////////////////////////////////////

#include "OAL_EFX.h"


#endif // _OALFUNCS_H
