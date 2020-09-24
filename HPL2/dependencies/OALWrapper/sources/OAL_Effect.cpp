/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#include "OALWrapper/OAL_Effect.h"
#include "OALWrapper/OAL_Helper.h"

#include <SDL_mutex.h>

cOAL_Effect::cOAL_Effect() : iOAL_LowLevelObject("Effect"),
							 mbNeedsUpdate(true), 
							 mpMutex(NULL)
{
	mbStatus = CreateLowLevelID();
}

cOAL_Effect::~cOAL_Effect()
{
	DestroyLowLevelID();
}

bool cOAL_Effect::CreateLowLevelID()
{
	DEF_FUNC_NAME("cOAL_Effect::CreateLowLevelID()");
	FUNC_USES_AL;

	
	RUN_AL_FUNC(alGenEffects ( 1, &mlEffectId );)
	
	return (!AL_ERROR_OCCURED && (IsValidObject()));
}


bool cOAL_Effect::DestroyLowLevelID()
{
	DEF_FUNC_NAME("cOAL_Effect::DestroyLowLevelID()");
	FUNC_USES_AL;

	bool bSuccess = true;
	if ( IsValidObject() )
	{
		RUN_AL_FUNC(alDeleteEffects( 1, &mlEffectId ));
		
		bSuccess = bSuccess && !AL_ERROR_OCCURED;
	}
	return bSuccess;
}

bool cOAL_Effect::IsValidObject()
{
	DEF_FUNC_NAME("cOAL_Effect::IsValidObject()");
	FUNC_USES_AL;

	return (alIsEffect(mlEffectId) == AL_TRUE);
}


void cOAL_Effect::Lock()
{
	SDL_LockMutex(mpMutex);
}

void cOAL_Effect::Unlock()
{
	SDL_UnlockMutex(mpMutex);
}
