/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_EFFECTSLOT_H
#define _OAL_EFFECTSLOT_H

#include "OAL_Types.h"
#include "OAL_LowLevelObject.h"

class cOAL_EFXManager;

class cOAL_EffectSlot : public iOAL_LowLevelObject
{
public:
	cOAL_EffectSlot ( cOAL_EFXManager* apEFXManager, int alId );
	~cOAL_EffectSlot();

	bool CreateLowLevelID();
	bool DestroyLowLevelID();

	void SaveObjectState(){}
	void RestoreObjectState(){}

	bool IsValidObject();

	bool AttachEffect ( cOAL_Effect* apEffect );
	inline bool IsFree() { return (mpEffect == NULL); }

	void SetGain ( float afGain );
	inline float GetGain () { return mfGain; }

	void SetAutoAdjust (bool abAuto);
	bool GetAutoAdjust () { return mbAutoAdjust; }

	void Update();
	void Reset();

	void Lock();
	void Unlock();

private:
	int mlId;
	cOAL_Effect* mpEffect;
	float mfGain;
	bool mbAutoAdjust;
	cOAL_EFXManager* mpEFXManager;

	SDL_mutex*		mpMutex;
    
};

#endif	// _OAL_EFFECTSLOT_H






