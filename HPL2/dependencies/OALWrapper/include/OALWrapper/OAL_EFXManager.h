/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_EFXMANAGER_H
#define _OAL_EFXMANAGER_H

#include "OAL_Types.h"
#include "OAL_Helper.h"
#include "OAL_LoggerObject.h"

typedef enum
{
	OAL_Effect_Type_Reverb,
	OAL_Effect_Type_Default
} OAL_Effect_Type;

typedef enum 
{
	OAL_Filter_Type_LowPass,
	OAL_Filter_Type_HighPass,
	OAL_Filter_Type_BandPass,
	OAL_Filter_Type_Default
} OAL_Filter_Type;


class cOAL_EFXManager : public iOAL_LoggerObject
{
public:
	cOAL_EFXManager();
	~cOAL_EFXManager();

	cOAL_EffectSlot* GetEffectSlot ( int alSlotHandle ) 
	{ 
		if ((alSlotHandle >= 0) && ((unsigned int) alSlotHandle < mpvSlots->size()) )
			return (*mpvSlots)[alSlotHandle]; 
		else 
			return NULL;
	}

	bool Initialize(int alNumSlotsHint, int alNumSends, bool abUseThread, int alSlotUpdateFreq);
	void Destroy();

	cOAL_Effect_Reverb* CreateReverbEffect ();
	
	cOAL_Filter*			CreateFilter();
	/*
	cOAL_Filter_LowPass*	CreateLowPassFilter ();
	cOAL_Filter_HighPass*	CreateHighPassFilter ();
	cOAL_Filter_BandPass*	CreateBandPassFilter ();
*/

	void DestroyEffect ( cOAL_Effect* apEffect );
	void DestroyFilter ( cOAL_Filter* apFilter );

	int AssignSlot( cOAL_Effect* apEffect );

	inline int GetNumSlots() { return mlNumSlots; }
	inline int GetNumSends() { return mlNumSends; }

	int UseEffect ( cOAL_Effect *apEffect );

	void UpdateSlots();

	inline bool IsThreadAlive () { return mbUsingThread; }
	int	GetThreadWaitTime();

	std::string GetIDString();

private:

	int mlNumSlots;
	int mlNumSends;
	
	tSlotVector*	mpvSlots;
	tOALEffectList*	mplstEffectList;
	tOALFilterList* mplstFilterList;

	bool mbUsingThread;
	SDL_Thread*		mpUpdaterThread;
	int				mlThreadWaitTime;

};


#endif	// _OAL_EFXMANAGER_H
