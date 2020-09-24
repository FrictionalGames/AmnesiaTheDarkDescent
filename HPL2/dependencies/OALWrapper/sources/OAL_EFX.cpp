/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#include "OALWrapper/OAL_EFX.h"
#include "OALWrapper/OAL_Types.h"
#include "OALWrapper/OAL_Device.h"
#include "OALWrapper/OAL_Source.h"
#include "OALWrapper/OAL_SourceManager.h"
#include "OALWrapper/OAL_Effect_Reverb.h"
#include "OALWrapper/OAL_Filter.h"
#include "OALWrapper/OAL_EFXManager.h"

extern cOAL_Device* gpDevice;

void OAL_Filter_SetGain ( cOAL_Filter* apFilter, float afGain)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apFilter==NULL))
		return;

	apFilter->SetGain(afGain);
}
void OAL_Filter_SetGainHF ( cOAL_Filter* apFilter, float afGainHF )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apFilter==NULL))
		return;

	apFilter->SetGainHF(afGainHF);
}

void OAL_Filter_SetType ( cOAL_Filter* apFilter, eOALFilterType aeType )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apFilter==NULL))
		return;

	eOALFilterType eOldType = apFilter->GetType();
    apFilter->SetType(aeType);
	if (!apFilter->GetStatus())
		apFilter->SetType(eOldType);
}


void OAL_Effect_Reverb_SetDensity( cOAL_Effect_Reverb* apEffect, float afDensity)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	apEffect->SetDensity(afDensity);
}

void OAL_Effect_Reverb_SetDiffusion ( cOAL_Effect_Reverb* apEffect, float afDiffusion)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	
	apEffect->SetDiffusion ( afDiffusion );
}


void OAL_Effect_Reverb_SetGain ( cOAL_Effect_Reverb* apEffect, float afGain)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	
	apEffect->SetGain ( afGain );
}

void OAL_Effect_Reverb_SetGainHF ( cOAL_Effect_Reverb* apEffect, float afGainHF)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	
	apEffect->SetGainHF ( afGainHF );
}
void OAL_Effect_Reverb_SetGainLF ( cOAL_Effect_Reverb* apEffect, float afGainLF)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	
	apEffect->SetGainLF ( afGainLF );

}
void OAL_Effect_Reverb_SetDecayTime ( cOAL_Effect_Reverb* apEffect, float afDecayTime)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	
	apEffect->SetDecayTime ( afDecayTime );
}

void OAL_Effect_Reverb_SetDecayHFRatio ( cOAL_Effect_Reverb* apEffect, float afDecayHFRatio )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetDecayHFRatio ( afDecayHFRatio );	
}
void OAL_Effect_Reverb_SetDecayLFRatio ( cOAL_Effect_Reverb* apEffect, float afDecayLFRatio )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetDecayLFRatio ( afDecayLFRatio );
}

void OAL_Effect_Reverb_SetReflectionsGain ( cOAL_Effect_Reverb* apEffect, float afReflectionsGain )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetReflectionsGain ( afReflectionsGain );	
}
void OAL_Effect_Reverb_SetReflectionsDelay( cOAL_Effect_Reverb* apEffect, float afReflectionsDelay )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetReflectionsDelay ( afReflectionsDelay );
}

void OAL_Effect_Reverb_SetReflectionsPan( cOAL_Effect_Reverb* apEffect, float afReflectionsPan[3] )
{
    if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetReflectionsPan ( afReflectionsPan );	
}

void OAL_Effect_Reverb_SetLateReverbGain( cOAL_Effect_Reverb* apEffect, float afLateReverbGain)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetLateReverbGain ( afLateReverbGain );
}

void OAL_Effect_Reverb_SetLateReverbDelay ( cOAL_Effect_Reverb* apEffect, float afLateReverbDelay)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetLateReverbDelay ( afLateReverbDelay );
}
void OAL_Effect_Reverb_SetLateReverbPan (cOAL_Effect_Reverb* apEffect, float afLateReverbPan[3])
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetLateReverbPan ( afLateReverbPan );
}
void OAL_Effect_Reverb_SetEchoTime (cOAL_Effect_Reverb* apEffect, float afEchoTime)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetEchoTime ( afEchoTime );
}
void OAL_Effect_Reverb_SetEchoDepth (cOAL_Effect_Reverb* apEffect, float afEchoDepth)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	apEffect->SetEchoDepth ( afEchoDepth );
}
void OAL_Effect_Reverb_SetModulationTime (cOAL_Effect_Reverb* apEffect, float afModulationTime)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetModulationTime ( afModulationTime );
}
void OAL_Effect_Reverb_SetModulationDepth (cOAL_Effect_Reverb* apEffect, float afModulationDepth)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetModulationDepth ( afModulationDepth );
}
void OAL_Effect_Reverb_SetAirAbsorptionGainHF (cOAL_Effect_Reverb* apEffect, float afAirAbsorptionGainHF)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetAirAbsorptionGainHF ( afAirAbsorptionGainHF );
}
void OAL_Effect_Reverb_SetHFReference (cOAL_Effect_Reverb* apEffect, float afHFReference)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetHFReference( afHFReference );
}

void OAL_Effect_Reverb_SetLFReference (cOAL_Effect_Reverb* apEffect, float afLFReference)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetLFReference ( afLFReference );
}
void OAL_Effect_Reverb_SetRoomRolloffFactor (cOAL_Effect_Reverb* apEffect, float afRoomRolloffFactor)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetRoomRolloffFactor ( afRoomRolloffFactor );
}
void OAL_Effect_Reverb_SetDecayHFLimit (cOAL_Effect_Reverb* apEffect, ALboolean abDecayHFLimit)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;
	
	apEffect->SetDecayHFLimit ( abDecayHFLimit );
}


void OAL_Filter_Destroy( cOAL_Filter* apFilter )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apFilter == NULL))
		return;

	gpDevice->GetEFXManager()->DestroyFilter(apFilter);
}

void OAL_Effect_Destroy( cOAL_Effect* apEffect )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect==NULL))
		return;

	gpDevice->GetEFXManager()->DestroyEffect(apEffect);
}


////////////////////////////////////////////
// Source EFX stuff

void OAL_Source_SetDirectFilter( int alSourceHandle, cOAL_Filter* apFilter )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetDirectFilter(apFilter);
		pSource->Unlock();
	}
}

void OAL_Source_SetAuxSend ( int alSourceHandle, int alAuxSend, int alSlotHandle, cOAL_Filter* apFilter )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	cOAL_EffectSlot* pSlot = gpDevice->GetEFXManager()->GetEffectSlot(alSlotHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetAuxSend(alAuxSend,pSlot,apFilter);
		pSource->Unlock();
	}
}

void OAL_Source_SetAuxSendSlot ( int alSourceHandle, int alAuxSend, int alSlotHandle)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	cOAL_EffectSlot* pSlot = gpDevice->GetEFXManager()->GetEffectSlot(alSlotHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetAuxSendSlot(alAuxSend,pSlot);
		pSource->Unlock();
	}

}

void OAL_Source_SetAuxSendFilter ( int alSourceHandle, int alAuxSend, cOAL_Filter* apFilter)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetAuxSendFilter(alAuxSend,apFilter);
		pSource->Unlock();
	}

}

void OAL_Source_SetFiltering( int alSourceHandle, bool abEnabled, int alFlags)
{
    if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetFilterEnabled(abEnabled, alFlags);
		pSource->Unlock();
	}
}

void OAL_Source_SetFilterType( int alSourceHandle, eOALFilterType aeType)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetFilterType(aeType);
		pSource->UpdateFiltering();
		pSource->Unlock();
	}
}
void OAL_Source_SetFilterGain( int alSourceHandle, float afGain)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetFilterGain(afGain);
		pSource->UpdateFiltering();
		pSource->Unlock();
	}
}
void OAL_Source_SetFilterGainHF( int alSourceHandle, float afGainHF)
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return;

	cOAL_Source* pSource = gpDevice->GetSource(alSourceHandle);

	if (pSource)
	{
		pSource->Lock();
		pSource->SetFilterGainHF(afGainHF);
		pSource->UpdateFiltering();
		pSource->Unlock();
	}
}


///////////////////////////////////////////////////////////////
/*
cOAL_Filter_LowPass* OAL_Filter_LowPass_Create ()
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return NULL;
	
	return gpDevice->GetEFXManager()->CreateLowPassFilter();
}
*/
///////////////////////////////////////////////////////////////

cOAL_Filter* OAL_Filter_Create ()
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return NULL;
	
	return gpDevice->GetEFXManager()->CreateFilter();
}

///////////////////////////////////////////////////////////////

cOAL_Effect_Reverb* OAL_Effect_Reverb_Create ()
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive())
		return NULL;

	return gpDevice->GetEFXManager()->CreateReverbEffect();
}

///////////////////////////////////////////////////////////////

bool OAL_EffectSlot_AttachEffect ( int alSlotId, cOAL_Effect* apEffect )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (apEffect == NULL) )
		return false;

	cOAL_EFXManager* pEFXManager = gpDevice->GetEFXManager();
	cOAL_EffectSlot* pSlot = NULL;

	if (pEFXManager)
	{
		pSlot = pEFXManager->GetEffectSlot(alSlotId);
		if (pSlot) 
			return pSlot->AttachEffect(apEffect);
		else
			return false;
	}
	return false;
}

void OAL_EffectSlot_SetGain ( int alSlotId, float afGain )
{
   	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (alSlotId == -1) )
		return;

	cOAL_EffectSlot* pSlot = gpDevice->GetEFXManager()->GetEffectSlot(alSlotId);
	if (pSlot)
		pSlot->SetGain(afGain);
}

void OAL_EffectSlot_SetAutoAdjust ( int alSlotId, bool abAutoAdjust )
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() || (alSlotId != -1) )
		return;

	cOAL_EffectSlot* pSlot = gpDevice->GetEFXManager()->GetEffectSlot(alSlotId);
	if (pSlot)
		pSlot->SetAutoAdjust(abAutoAdjust);
}

void OAL_UpdateEffectSlots()
{
	if ((gpDevice == NULL) || !gpDevice->IsEFXActive() )
		return;

	gpDevice->GetEFXManager()->UpdateSlots();

}