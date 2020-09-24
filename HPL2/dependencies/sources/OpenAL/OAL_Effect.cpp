#include "OpenAL/OAL_Effect.h"
#include "OpenAL/OAL_Helper.h"


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
