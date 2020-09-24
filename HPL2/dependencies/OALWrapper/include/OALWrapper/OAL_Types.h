/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_Types.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Types for OpenAL
*/


#ifndef _OAL_TYPES_H
#define _OAL_TYPES_H


////////////////////////////////
// Extension stuff

#define NUM_EXTENSIONS 13
#define NUM_ALC_EXTENSIONS 4
//#define NUM_AL_EXTENSIONS 9

#define OAL_ALC_EXT_CAPTURE 0
#define OAL_ALC_EXT_EFX 1
#define OAL_AL_EXT_OFFSET 2
#define OAL_AL_EXT_LINEAR_DISTANCE 3
#define OAL_AL_EXT_EXPONENT_DISTANCE 4
#define OAL_EAX 5
#define OAL_EAX2_0 6
#define OAL_EAX3_0 7
#define OAL_EAX4_0 8
#define OAL_EAX5_0 9
#define OAL_EAX_RAM 10

////////////////////////////////////////////////////
// Specifies the size of the memory unit in streaming ( currently 4 KB ) 
// (buffer sizes are set using multiples of this unit)
#define STREAMING_BLOCK_SIZE 4096
//#define BUFFERCOUNT 8

#if defined(__ppc__)
	#define SYS_ENDIANNESS 1
#else
	#define SYS_ENDIANNESS 0
#endif

#define OAL_FREE	-3
#define OAL_ALL		-2

#include <string>
#include <vector>
#include <list>

// SDL forward declares
struct SDL_mutex;
struct SDL_Thread;

// This expects the headers from the OALWrapper source (to keep things consistent and clean)
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

typedef enum
{
	eOAL_SampleFormat_Detect = 0,
	eOAL_SampleFormat_Ogg,
	eOAL_SampleFormat_Wav,
	eOAL_SampleFormat_Unknown = 0xff
} eOAL_SampleFormat;

class cOAL_Source;
class cOAL_Sample;
class cOAL_Stream;
class cOAL_Buffer;

typedef struct {
	void(*Init)(void *data);
	double(*GetTime)(void *data);
	void(*Seek)(void *data, float afWhere, bool abForceRebuffer);
	bool(*Stream)(void *data, cOAL_Buffer* apDestBuffer, char *buffer, unsigned int bufferSize, bool& eof);
	void(*Destroy)(void *data);
} tStreamCallbacks;

typedef struct {
	ALint channels;
	ALint frequency;
	ALenum format;
	long int samples;
	double totalTime;
} tStreamInfo;

typedef std::vector<cOAL_Source*>			tSourceVec;
typedef tSourceVec::iterator			tSourceVecIt;

//typedef set<cOAL_Source*>				tSourceSet;
//typedef set<cOAL_Source*>::iterator		tSourceSetIt;

typedef std::list<cOAL_Source*>				tSourceList;
typedef tSourceList::iterator			tSourceListIt;

typedef std::list<cOAL_Sample*>				tSampleList;
typedef tSampleList::iterator			tSampleListIt;

typedef std::list<cOAL_Stream*>				tStreamList;
typedef tStreamList::iterator			tStreamListIt;

typedef std::vector<cOAL_Buffer*>			tBufferVec;
typedef tBufferVec::iterator			tBufferVecIt;


class cOAL_EffectSlot;

class cOAL_Filter;
class cOAL_Filter_LowPass;
class cOAL_Filter_HighPass;
class cOAL_Filter_BandPass;

class cOAL_Effect;
class cOAL_Effect_Reverb;

class cOAL_SourceSend;

typedef std::vector<cOAL_EffectSlot*>			tSlotVector;
typedef tSlotVector::iterator	tSlotVectorIt;

typedef std::list<cOAL_Effect*>					tOALEffectList;
typedef tOALEffectList::iterator		tOALEffectListIt;

typedef std::list<cOAL_Filter*>					tOALFilterList;
typedef tOALFilterList::iterator		tOALFilterListIt;

typedef std::vector<cOAL_SourceSend*>			tSendVector;
typedef tSendVector::iterator	tSendVectorIt;

typedef enum 
{
	eOAL_SourceStatus_Free,
	eOAL_SourceStatus_Busy,
	eOAL_SourceStatus_Busy_BufferUnderrun,
	eOAL_SourceStatus_Default
} eOAL_SourceStatus;

typedef enum
{
	eOAL_DistanceModel_Inverse,
	eOAL_DistanceModel_Inverse_Clamped,
	eOAL_DistanceModel_Linear,
	eOAL_DistanceModel_Linear_Clamped,
	eOAL_DistanceModel_Exponent,
	eOAL_DistanceModel_Exponent_Clamped,
	eOAL_DistanceModel_None,
	eOAL_DistanceModel_Default,
} eOAL_DistanceModel;



//////////////////////////////////////////
/// EFX Stuff
//////////////////////////////////////////


// Function pointers 
////////////////////

// Effect slots

extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

// Effects

extern LPALGENEFFECTS alGenEffects;
extern LPALDELETEEFFECTS alDeleteEffects;
extern LPALISEFFECT alIsEffect;
extern LPALEFFECTI alEffecti;
extern LPALEFFECTIV alEffectiv;
extern LPALEFFECTF alEffectf;
extern LPALEFFECTFV alEffectfv;
extern LPALGETEFFECTI alGetEffecti;
extern LPALGETEFFECTIV alGetEffectiv;
extern LPALGETEFFECTF alGetEffectf;
extern LPALGETEFFECTFV alGetEffectfv;

// Filters

extern LPALGENFILTERS alGenFilters;
extern LPALDELETEFILTERS alDeleteFilters;
extern LPALISFILTER alIsFilter;
extern LPALFILTERI alFilteri;
extern LPALFILTERIV alFilteriv;
extern LPALFILTERF alFilterf;
extern LPALFILTERFV alFilterfv;
extern LPALGETFILTERI alGetFilteri;
extern LPALGETFILTERIV alGetFilteriv;
extern LPALGETFILTERF alGetFilterf;
extern LPALGETFILTERFV alGetFilterfv;


typedef enum _eOALFilterType
{
	eOALFilterType_LowPass,
	eOALFilterType_HighPass,
	eOALFilterType_BandPass,
	eOALFilterType_Null,
} eOALFilterType;


#endif	// _OAL_TYPES_H
