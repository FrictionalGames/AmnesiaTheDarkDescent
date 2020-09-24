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
#pragma comment(lib, "alut.lib")
#pragma comment(lib, "openal32.lib")
#ifdef USE_SDL2
#pragma comment(lib, "SDL2.lib")
#else
#pragma comment(lib, "SDL.lib")
#endif
#endif

#include "OAL_Types.h"

//#include "OAL_WAVSample.h"
//#include "OAL_OggSample.h"
//#include "OAL_OggStream.h"


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
