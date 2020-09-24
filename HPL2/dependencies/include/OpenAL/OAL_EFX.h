#ifndef _OAL_EFX_H
#define _OAL_EFX_H

#include "OAL_Types.h"
#include "OAL_Device.h"
#include "OAL_SourceManager.h"
#include "OAL_EFXManager.h"

extern cOAL_Device* gpDevice;

cOAL_Filter* OAL_Filter_Create ();
void OAL_Filter_SetGain ( cOAL_Filter* apFilter, float afGain);
void OAL_Filter_SetGainHF ( cOAL_Filter* apFilter, float afGainHF );
void OAL_Filter_SetType ( cOAL_Filter* apFilter, eOALFilterType aeType );


cOAL_Effect_Reverb*	OAL_Effect_Reverb_Create();
void OAL_Effect_Reverb_SetDensity( cOAL_Effect_Reverb* apEffect, float afDensity);
void OAL_Effect_Reverb_SetDiffusion ( cOAL_Effect_Reverb* apEffect, float afDiffusion);
void OAL_Effect_Reverb_SetGain ( cOAL_Effect_Reverb* apEffect, float afGain);
void OAL_Effect_Reverb_SetGainHF ( cOAL_Effect_Reverb* apEffect, float afGainHF);
void OAL_Effect_Reverb_SetGainLF ( cOAL_Effect_Reverb* apEffect, float afGainLF);
void OAL_Effect_Reverb_SetDecayTime ( cOAL_Effect_Reverb* apEffect, float afDecayTime);
void OAL_Effect_Reverb_SetDecayHFRatio ( cOAL_Effect_Reverb* apEffect, float afDecayHFRatio );
void OAL_Effect_Reverb_SetDecayLFRatio ( cOAL_Effect_Reverb* apEffect, float afDecayLFRatio );
void OAL_Effect_Reverb_SetReflectionsGain ( cOAL_Effect_Reverb* apEffect, float afReflectionsGain );
void OAL_Effect_Reverb_SetReflectionsDelay( cOAL_Effect_Reverb* apEffect, float afReflectionsDelay );
void OAL_Effect_Reverb_SetReflectionsPan( cOAL_Effect_Reverb* apEffect, float afReflectionsPan[3] );
void OAL_Effect_Reverb_SetLateReverbGain( cOAL_Effect_Reverb* apEffect, float afLateReverbGain);
void OAL_Effect_Reverb_SetLateReverbDelay ( cOAL_Effect_Reverb* apEffect, float afLateReverbDelay);
void OAL_Effect_Reverb_SetLateReverbPan (cOAL_Effect_Reverb* apEffect, float afLateReverbPan[3]);
void OAL_Effect_Reverb_SetEchoTime (cOAL_Effect_Reverb* apEffect, float afEchoTime);
void OAL_Effect_Reverb_SetEchoDepth (cOAL_Effect_Reverb* apEffect, float afEchoDepth);
void OAL_Effect_Reverb_SetModulationTime (cOAL_Effect_Reverb* apEffect, float afModulationTime);
void OAL_Effect_Reverb_SetModulationDepth (cOAL_Effect_Reverb* apEffect, float afModulationDepth);
void OAL_Effect_Reverb_SetAirAbsorptionGainHF (cOAL_Effect_Reverb* apEffect, float afAirAbsorptionGainHF);
void OAL_Effect_Reverb_SetHFReference (cOAL_Effect_Reverb* apEffect, float afHFReference);
void OAL_Effect_Reverb_SetLFReference (cOAL_Effect_Reverb* apEffect, float afLFReference);
void OAL_Effect_Reverb_SetRoomRolloffFactor (cOAL_Effect_Reverb* apEffect, float afRoomRolloffFactor);
void OAL_Effect_Reverb_SetDecayHFLimit (cOAL_Effect_Reverb* apEffect, ALboolean abDecayHFLimit);


void OAL_Filter_Destroy( cOAL_Filter* apFilter );
void OAL_Effect_Destroy( cOAL_Effect* apEffect );

////////////////////////////////////////////
// Effect Slot Management

// OAL_UseEffect : Will return an integer value referencing to the Effect Slot to which the Effect passed as parameter will attach, -1 if failure
int OAL_UseEffect ( cOAL_Effect* apEffect );
bool OAL_EffectSlot_AttachEffect(int alSlotId, cOAL_Effect* apEffect);
void OAL_EffectSlot_SetGain ( int alSlotHandle, float afGain );
void OAL_EffectSlot_SetAutoAdjust ( int alSlotHandle, bool abAutoAdjust );
void OAL_UpdateEffectSlots();
////////////////////////////////////////////
// Source EFX stuff

void OAL_Source_SetDirectFilter( int alSourceHandle, cOAL_Filter* apFilter );
void OAL_Source_SetAuxSend ( int alSourceHandle, int alAuxSend, int alSlotHandle, cOAL_Filter* apFilter );
void OAL_Source_SetAuxSendSlot ( int alSourceHandle, int alAuxSend, int alSlotHandle);
void OAL_Source_SetAuxSendFilter ( int alSourceHandle, int alAuxSend, cOAL_Filter* apFilter );

void OAL_Source_SetFiltering( int alSourceHandle, bool abEnabled, int alFlags);
void OAL_Source_SetFilterType( int alSourceHandle, eOALFilterType aeType);
void OAL_Source_SetFilterGain( int alSourceHandle, float afGain);
void OAL_Source_SetFilterGainHF( int alSourceHandle, float afGainHF);

#endif	// _OAL_EFX_H
