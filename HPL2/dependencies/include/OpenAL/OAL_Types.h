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

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>

using namespace std;

class cOAL_Source;
class cOAL_Sample;
class cOAL_Stream;
class cOAL_Buffer;

typedef vector<cOAL_Source*>			tSourceVec;
typedef tSourceVec::iterator			tSourceVecIt;

//typedef set<cOAL_Source*>				tSourceSet;
//typedef set<cOAL_Source*>::iterator		tSourceSetIt;

typedef list<cOAL_Source*>				tSourceList;
typedef tSourceList::iterator			tSourceListIt;

typedef list<cOAL_Sample*>				tSampleList;
typedef tSampleList::iterator			tSampleListIt;

typedef list<cOAL_Stream*>				tStreamList;
typedef tStreamList::iterator			tStreamListIt;

typedef vector<cOAL_Buffer*>			tBufferVec;
typedef tBufferVec::iterator			tBufferVecIt;


class cOAL_EffectSlot;

class cOAL_Filter;
class cOAL_Filter_LowPass;
class cOAL_Filter_HighPass;
class cOAL_Filter_BandPass;

class cOAL_Effect;
class cOAL_Effect_Reverb;

class cOAL_SourceSend;

typedef vector<cOAL_EffectSlot*>			tSlotVector;
typedef vector<cOAL_EffectSlot*>::iterator	tSlotVectorIt;

typedef list<cOAL_Effect*>					tOALEffectList;
typedef list<cOAL_Effect*>::iterator		tOALEffectListIt;

typedef list<cOAL_Filter*>					tOALFilterList;
typedef list<cOAL_Filter*>::iterator		tOALFilterListIt;

typedef vector<cOAL_SourceSend*>			tSendVector;
typedef vector<cOAL_SourceSend*>::iterator	tSendVectorIt;

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
