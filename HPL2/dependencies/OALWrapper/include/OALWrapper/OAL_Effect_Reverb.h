/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_EFFECT_REVERB_H
#define _OAL_EFFECT_REVERB_H

#include "OAL_Effect.h"
#include "OAL_Types.h"
#include "OAL_Helper.h"

class cOAL_Effect_Reverb : public cOAL_Effect
{
public:
	cOAL_Effect_Reverb(bool abForceEFXReverb = false): 
							mfDensity(1.0f), 
							mfDiffusion(1.0f),
							mfGain(0.32f),
							mfGainHF(0.89f),
							mfGainLF(0.0f),
							mfDecayTime(1.49f),
							mfDecayHFRatio(0.83f),
							mfDecayLFRatio(1.0f),
							mfReflectionsGain(0.05f),
							mfReflectionsDelay(0.007f),
							mfLateReverbGain(1.25f),
							mfLateReverbDelay(0.011f),
							mfEchoTime(0.25f),
							mfEchoDepth(0.0f),
							mfModulationTime(0.25f),
							mfModulationDepth(0.0f),
							mfAirAbsorptionGainHF(0.994f),
							mfHFReference(5000.0f),
							mfLFReference(250.0f),
							mfRoomRolloffFactor(0.0f),
							mbDecayHFLimit(AL_TRUE),
							mbEAXReverb ( false )
	{
		DEF_FUNC_NAME("cOAL_Effect_Reverb::cOAL_Effect_Reverb");
		FUNC_USES_AL;

		mfReflectionsPan[0] = 0.0f;
		mfReflectionsPan[1] = 0.0f;
		mfReflectionsPan[2] = 0.0f;
		mfLateReverbPan[0] = 0.0f;
		mfLateReverbPan[1] = 0.0f;
		mfLateReverbPan[2] = 0.0f;

		if ( !abForceEFXReverb )
		{
			RUN_AL_FUNC( alEffecti(mlEffectId, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB) );
			mbEAXReverb = !AL_ERROR_OCCURED;
			mbStatus = !AL_ERROR_OCCURED;
		}

		if ( abForceEFXReverb || !mbEAXReverb )
		{
			RUN_AL_FUNC ( alEffecti ( mlEffectId, AL_EFFECT_TYPE, AL_EFFECT_REVERB) );
			mbStatus = !AL_ERROR_OCCURED;		
		}
	}

	~cOAL_Effect_Reverb(){}

	inline float GetDensity( ) { return mfDensity; }
	inline float GetDiffusion ( ) { return mfDiffusion; }
	inline float GetGain ( ) { return mfGain; }
	inline float GetGainHF ( ) { return mfGainHF; }
	inline float GetGainLF ( ) { return mfGainLF; }
	inline float GetDecayTime ( ) { return mfDecayTime; }
	inline float GetDecayHFRatio (  ) { return mfDecayHFRatio; }
	inline float GetDecayLFRatio (  ) { return mfDecayLFRatio; } 
	inline float GetReflectionsGain (  ) { return mfReflectionsGain; }
	inline float GetReflectionsDelay(  ) { return mfReflectionsDelay; } 
	inline float* GetReflectionsPan(  ) { return mfReflectionsPan; }
	inline float GetLateReverbGain( ) { return mfLateReverbGain; } 
	inline float GetLateReverbDelay ( ) { return mfLateReverbDelay; }
	inline float* GetLateReverbPan () { return mfLateReverbPan; }
	inline float GetEchoTime () { return mfEchoTime; }
	inline float GetEchoDepth () { return mfEchoDepth; } 
	inline float GetModulationTime () { return mfModulationTime; }
	inline float GetModulationDepth () { return mfModulationDepth; }
	inline float GetAirAbsorptionGainHF () { return mfAirAbsorptionGainHF; }
	inline float GetHFReference () { return mfHFReference; } 
	inline float GetLFReference () { return mfLFReference; }
	inline float GetRoomRolloffFactor () { return mfRoomRolloffFactor; }
	inline ALboolean GetDecayHFLimit () { return mbDecayHFLimit; } 

	void SetDensity( float afDensity);
	void SetDiffusion ( float afDiffusion);
    void SetGain ( float afGain);
	void SetGainHF ( float afGainHF);
	void SetGainLF ( float afGainLF);
    void SetDecayTime ( float afDecayTime);
	void SetDecayHFRatio ( float afDecayHFRatio );
	void SetDecayLFRatio ( float afDecayLFRatio );
	void SetReflectionsGain ( float afReflectionsGain );
	void SetReflectionsDelay( float afReflectionsDelay );
	void SetReflectionsPan( float afReflectionsPan[3] );
	void SetLateReverbGain( float afLateReverbGain);
	void SetLateReverbDelay ( float afLateReverbDelay);
	void SetLateReverbPan (float afLateReverbPan[3]);
	void SetEchoTime (float afEchoTime);
	void SetEchoDepth (float afEchoDepth);
	void SetModulationTime (float afModulationTime);
	void SetModulationDepth (float afModulationDepth);
	void SetAirAbsorptionGainHF (float afAirAbsorptionGainHF);
	void SetHFReference (float afHFReference);
	void SetLFReference (float afLFReference);
	void SetRoomRolloffFactor (float afRoomRolloffFactor);
	void SetDecayHFLimit (ALboolean abDecayHFLimit);
	

private:

	float mfDensity;
	float mfDiffusion;
    float mfGain;
	float mfGainHF;
	float mfGainLF;
    float mfDecayTime;
	float mfDecayHFRatio;
	float mfDecayLFRatio;
	float mfReflectionsGain;
	float mfReflectionsDelay;
	float mfReflectionsPan[3];
	float mfLateReverbGain;
	float mfLateReverbDelay;
	float mfLateReverbPan[3];
	float mfEchoTime;
	float mfEchoDepth;
	float mfModulationTime;
	float mfModulationDepth;
	float mfAirAbsorptionGainHF;
	float mfHFReference;
	float mfLFReference;
	float mfRoomRolloffFactor;
	ALboolean mbDecayHFLimit;

	bool mbEAXReverb;

};


#endif // _OAL_EFFECT_REVERB_H
