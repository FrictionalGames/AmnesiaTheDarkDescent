#include "OpenAL/OAL_EffectSlot.h"
#include "OpenAL/OAL_Effect.h"
#include "OpenAL/OAL_Helper.h"
#include "OpenAL/OAL_EFXManager.h"


cOAL_EffectSlot::cOAL_EffectSlot( cOAL_EFXManager* apEFXManager, int alId) : iOAL_LowLevelObject("EffectSlot"),
																			  mlId(alId),
																			  mpEffect(NULL),
																			  mfGain(1.0f),
																			  mbAutoAdjust (true),
																			  mpEFXManager(apEFXManager)
{
	if (mpEFXManager->IsThreadAlive())
		mpMutex = SDL_CreateMutex();
	mbStatus = CreateLowLevelID();
}

cOAL_EffectSlot::~cOAL_EffectSlot()
{
	if(mpMutex)
		SDL_DestroyMutex(mpMutex);

	DestroyLowLevelID();
}


bool cOAL_EffectSlot::CreateLowLevelID()
{
	DEF_FUNC_NAME("cOAL_EffectSlot::CreateLowLevelID()");
	FUNC_USES_AL;

	RUN_AL_FUNC(alGenAuxiliaryEffectSlots(1,&mlObjectId));

	return (!AL_ERROR_OCCURED && IsValidObject());
}

bool cOAL_EffectSlot::DestroyLowLevelID()
{
	DEF_FUNC_NAME("cOAL_EffectSlot::DestroyLowLevelID()");
	FUNC_USES_AL;

	RUN_AL_FUNC(alDeleteAuxiliaryEffectSlots(1,&mlObjectId));

	return (!AL_ERROR_OCCURED && !IsValidObject());
}

bool cOAL_EffectSlot::IsValidObject()
{
	DEF_FUNC_NAME("cOAL_EffectSlot::IsValidObject()");
	FUNC_USES_AL;

	return (alIsAuxiliaryEffectSlot(mlObjectId) == AL_TRUE);

}

void cOAL_EffectSlot::Reset()
{
	this->SetGain(0);
	this->AttachEffect(NULL);
}

bool cOAL_EffectSlot::AttachEffect ( cOAL_Effect *apEffect )
{
	DEF_FUNC_NAME("cOAL_EffectSlot::AttachEffect()");
	FUNC_USES_AL;

    mpEffect = apEffect;

	RUN_AL_FUNC ( alAuxiliaryEffectSloti (mlObjectId, AL_EFFECTSLOT_EFFECT, ((mpEffect == NULL)? AL_EFFECT_NULL : mpEffect->GetEffectID()) ) );
	
	return !AL_ERROR_OCCURED;
}

void cOAL_EffectSlot::SetGain ( float afGain )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (afGain <0)
		afGain = 0;
	if (afGain >1)
		afGain = 1;
	RUN_AL_FUNC( alAuxiliaryEffectSlotf (mlObjectId, AL_EFFECTSLOT_GAIN, afGain));
}

void cOAL_EffectSlot::SetAutoAdjust( bool abAuto )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	RUN_AL_FUNC( alAuxiliaryEffectSloti (mlObjectId, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, (ALboolean) abAuto));
}

void cOAL_EffectSlot::Update()
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mpEffect)
	{
		mpEffect->Lock();
		if (mpEffect->NeedsUpdate())
		{
			RUN_AL_FUNC( alAuxiliaryEffectSloti(mlObjectId, AL_EFFECTSLOT_EFFECT, mpEffect->GetEffectID()) );
			//LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Slot updated\n");
			mpEffect->SetUpdated();
		}
		mpEffect->Unlock();
	}
}

void cOAL_EffectSlot::Lock()
{
	if ( mpEFXManager->IsThreadAlive() )
		SDL_LockMutex(mpMutex);
}

void cOAL_EffectSlot::Unlock()
{
	if ( mpEFXManager->IsThreadAlive() )
		SDL_UnlockMutex(mpMutex);
}
