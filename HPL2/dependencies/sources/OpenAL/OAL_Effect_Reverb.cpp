#include "OpenAL/OAL_Effect_Reverb.h"

void ClampValue ( float *apValue, float afMin, float afMax );

void cOAL_Effect_Reverb::SetDensity( float afDensity)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

    if (mfDensity == afDensity)
		return;

	Lock();

    mfDensity = afDensity;
	ClampValue(&mfDensity, 0.0f, 1.0f);

    ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_DENSITY : AL_REVERB_DENSITY);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfDensity));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetDiffusion ( float afDiffusion)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;
	
	if (mfDiffusion == afDiffusion)
		return;

	Lock();

	mfDiffusion = afDiffusion;
	ClampValue(&mfDiffusion, 0.0f, 1.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_DIFFUSION : AL_REVERB_DIFFUSION);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfDiffusion));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetGain ( float afGain)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfGain == afGain)
		return;

	Lock();

	mfGain = afGain;
	ClampValue(&mfGain, 0.0f, 1.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_GAIN : AL_REVERB_GAIN);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfGain));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetGainHF ( float afGainHF)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfGainHF == afGainHF)
		return;

	Lock();

	mfGainHF = afGainHF;
	ClampValue(&mfGainHF, 0.0f, 1.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_GAINHF : AL_REVERB_GAINHF);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfGainHF));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetGainLF ( float afGainLF)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfGainLF == afGainLF)
		return;

	Lock();


	mfGainLF = afGainLF;

	ClampValue(&mfGainLF, 0.0f, 1.0f);

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_GAINLF, mfGainLF));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetDecayTime ( float afDecayTime)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfDecayTime == afDecayTime)
		return;

	Lock();

	mfDecayTime = afDecayTime;
	ClampValue(&mfDecayTime, 0.1f, 2.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_DECAY_TIME : AL_REVERB_DECAY_TIME);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfDecayTime));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetDecayHFRatio ( float afDecayHFRatio )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfDecayHFRatio == afDecayHFRatio)
		return;

	Lock();

	mfDecayHFRatio = afDecayHFRatio;
	ClampValue(&mfDecayHFRatio, 0.1f, 2.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_DECAY_HFRATIO : AL_REVERB_DECAY_HFRATIO);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfDecayHFRatio));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetDecayLFRatio ( float afDecayLFRatio )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

    if (mfDecayLFRatio == afDecayLFRatio)
		return;

	Lock();

	mfDecayLFRatio = afDecayLFRatio;
	ClampValue(&mfDecayLFRatio, 0.1f, 2.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_DECAY_LFRATIO, mfDecayLFRatio));

}

void cOAL_Effect_Reverb::SetReflectionsGain ( float afReflectionsGain )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfReflectionsGain == afReflectionsGain)
		return;

	Lock();

	mfReflectionsGain = afReflectionsGain;
	ClampValue(&mfReflectionsGain, 0.1f, 2.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_REFLECTIONS_GAIN : AL_REVERB_REFLECTIONS_GAIN);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfReflectionsGain));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetReflectionsDelay( float afReflectionsDelay )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfReflectionsDelay == afReflectionsDelay)
		return;

	Lock();

	mfReflectionsDelay = afReflectionsDelay;
	ClampValue(&mfReflectionsDelay, 0.0f, 0.3f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_REFLECTIONS_DELAY : AL_REVERB_REFLECTIONS_DELAY);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfReflectionsDelay));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetReflectionsPan( float afReflectionsPan[3] )
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if ( (mfReflectionsPan[0] == afReflectionsPan[0]) && 
		 (mfReflectionsPan[1] == afReflectionsPan[1]) && 
		 (mfReflectionsPan[2] == afReflectionsPan[2]) )
		return;

	Lock();
    
	mfReflectionsPan[0] = afReflectionsPan[0];
	mfReflectionsPan[1] = afReflectionsPan[1];
	mfReflectionsPan[2] = afReflectionsPan[2];

	ClampValue(&mfReflectionsPan[0], -1.0f, 1.0f);
	ClampValue(&mfReflectionsPan[1], -1.0f, 1.0f);
	ClampValue(&mfReflectionsPan[2], -1.0f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectfv( mlEffectId, AL_EAXREVERB_REFLECTIONS_PAN, mfReflectionsPan));
	
}

void cOAL_Effect_Reverb::SetLateReverbGain( float afLateReverbGain)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	Lock();

	if (mfLateReverbGain == afLateReverbGain)
		return;

	mfLateReverbGain = afLateReverbGain;
	ClampValue(&mfLateReverbGain, 0.0f, 10.0f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_LATE_REVERB_GAIN : AL_REVERB_LATE_REVERB_GAIN);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfLateReverbGain));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetLateReverbDelay ( float afLateReverbDelay)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfLateReverbDelay == afLateReverbDelay)
		return;

	Lock();

	mfLateReverbDelay = afLateReverbDelay;
	ClampValue(&mfLateReverbDelay, 0.0f, 0.1f);

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_LATE_REVERB_DELAY : AL_REVERB_LATE_REVERB_DELAY);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfLateReverbDelay));

	mbNeedsUpdate = true; 

	Unlock();
}

void cOAL_Effect_Reverb::SetLateReverbPan (float afLateReverbPan[3])
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if ( (mfLateReverbPan[0] == afLateReverbPan[0]) && 
		 (mfLateReverbPan[1] == afLateReverbPan[1]) && 
		 (mfLateReverbPan[2] == afLateReverbPan[2]) )
		return;

	Lock();
    
	mfLateReverbPan[0] = afLateReverbPan[0];
	mfLateReverbPan[1] = afLateReverbPan[1];
	mfLateReverbPan[2] = afLateReverbPan[2];

	ClampValue(&mfLateReverbPan[0], -1.0f, 1.0f);
	ClampValue(&mfLateReverbPan[1], -1.0f, 1.0f);
	ClampValue(&mfLateReverbPan[2], -1.0f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectfv( mlEffectId, AL_EAXREVERB_LATE_REVERB_PAN, mfLateReverbPan));

}

void cOAL_Effect_Reverb::SetEchoTime (float afEchoTime)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;
    
	if (mfEchoTime == afEchoTime)
		return;

	Lock();

	mfEchoTime = afEchoTime;

	ClampValue(&mfEchoTime, 0.075f, 0.25f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_ECHO_TIME, mfEchoTime));


}

void cOAL_Effect_Reverb::SetEchoDepth (float afEchoDepth)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfEchoDepth == afEchoDepth)
		return;

	Lock();


	mfEchoDepth = afEchoDepth;

	ClampValue(&mfEchoDepth, 0.0f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_ECHO_DEPTH, mfEchoDepth));

}

void cOAL_Effect_Reverb::SetModulationTime (float afModulationTime)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

    if (mfModulationTime == afModulationTime)
		return;

	Lock();

	mfModulationTime = afModulationTime;

	ClampValue(&mfModulationTime, 0.04f, 4.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_MODULATION_TIME, mfModulationTime));

}

void cOAL_Effect_Reverb::SetModulationDepth (float afModulationDepth)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfModulationDepth == afModulationDepth)
		return;

	Lock();

	mfModulationDepth = afModulationDepth;

	ClampValue(&mfModulationDepth, 0.0f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_MODULATION_DEPTH, mfModulationDepth));

	
}

void cOAL_Effect_Reverb::SetAirAbsorptionGainHF (float afAirAbsorptionGainHF)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfAirAbsorptionGainHF == afAirAbsorptionGainHF)
		return;

	Lock();

	mfAirAbsorptionGainHF = afAirAbsorptionGainHF;
	ClampValue(&mfAirAbsorptionGainHF, 0.892f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_AIR_ABSORPTION_GAINHF : AL_REVERB_AIR_ABSORPTION_GAINHF);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfAirAbsorptionGainHF));

	
}

void cOAL_Effect_Reverb::SetHFReference (float afHFReference)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfHFReference == afHFReference)
		return;

	Lock();

	mfHFReference = afHFReference;

	ClampValue(&mfHFReference, 1000.0f, 20000.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_HFREFERENCE, mfHFReference));


}

void cOAL_Effect_Reverb::SetLFReference (float afLFReference)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfLFReference == afLFReference)
		return;

	Lock();

	mfLFReference = afLFReference;

	ClampValue(&mfLFReference, 20.0f, 1000.0f);

	mbNeedsUpdate = true; 

	Unlock();

	if (!mbEAXReverb)
		return;

	RUN_AL_FUNC( alEffectf( mlEffectId, AL_EAXREVERB_LFREFERENCE, mfLFReference));

}

void cOAL_Effect_Reverb::SetRoomRolloffFactor (float afRoomRolloffFactor)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mfRoomRolloffFactor == afRoomRolloffFactor)
		return;

	Lock();

	mfRoomRolloffFactor = afRoomRolloffFactor;
	ClampValue(&mfRoomRolloffFactor, 0.0f, 1.0f);

	mbNeedsUpdate = true; 

	Unlock();

	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_ROOM_ROLLOFF_FACTOR : AL_REVERB_ROOM_ROLLOFF_FACTOR);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mfRoomRolloffFactor));


}

void cOAL_Effect_Reverb::SetDecayHFLimit (ALboolean abDecayHFLimit)
{
	DEF_FUNC_NAME("");
	FUNC_USES_AL;

	if (mbDecayHFLimit == abDecayHFLimit)
		return;

	Lock();

	mbDecayHFLimit = abDecayHFLimit;
	
	ALuint lParameter = (mbEAXReverb? AL_EAXREVERB_DECAY_HFLIMIT : AL_REVERB_DECAY_HFLIMIT);

	RUN_AL_FUNC( alEffectf( mlEffectId, lParameter, mbDecayHFLimit));

	mbNeedsUpdate = true;

	Unlock();

}

void ClampValue ( float *apValue, float afMin, float afMax )
{
	if (*apValue < afMin)
		*apValue = afMin;
	else if (*apValue > afMax)
		*apValue = afMax;
}
