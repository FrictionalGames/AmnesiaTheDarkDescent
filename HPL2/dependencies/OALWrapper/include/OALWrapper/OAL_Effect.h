/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_EFFECT_H
#define _OAL_EFFECT_H

#include "OAL_LowLevelObject.h"

class cOAL_Effect : public iOAL_LowLevelObject
{
public:
	cOAL_Effect();
	~cOAL_Effect();

	bool CreateLowLevelID();
	bool DestroyLowLevelID();

	void SaveObjectState(){}
	void RestoreObjectState(){}

	bool IsValidObject();

	inline ALuint GetEffectID() { return mlEffectId; }

	inline bool	NeedsUpdate () { return mbNeedsUpdate; }
	inline void SetUpdated() { mbNeedsUpdate = false; }

//	virtual void SetParams( cOAL_Effect_Params& aParams ){}
//	virtual cOAL_Effect_Params GetParams(){};

	void Lock();
	void Unlock();
    
protected:
	ALuint	mlEffectId;
	bool	mbNeedsUpdate;

	SDL_mutex* mpMutex;
};

#endif	// _OAL_EFFECT_H


