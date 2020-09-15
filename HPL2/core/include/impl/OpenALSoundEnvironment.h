/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_OPENALSOUNDENVIRONMENT_H
#define HPL_OPENALSOUNDENVIRONMENT_H

#include "sound/SoundEnvironment.h"

namespace hpl
{
	class cSoundManager;

	class cOpenALSoundEnvironment : public iSoundEnvironment
	{
	public:
		cOpenALSoundEnvironment ( );
		
		~cOpenALSoundEnvironment ();

		bool CreateFromFile(const tString &asFile);

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
		inline int	 GetDecayHFLimit () { return mbDecayHFLimit; } 

		inline void SetDensity( float afDensity)									{ mfDensity = afDensity; }
		inline void SetDiffusion ( float afDiffusion)								{ mfDiffusion = afDiffusion; }
		inline void SetGain ( float afGain)											{ mfGain = afGain; }
		inline void SetGainHF ( float afGainHF)										{ mfGainHF = afGainHF; }
		inline void SetGainLF ( float afGainLF)										{ mfGainLF = afGainLF; }
		inline void SetDecayTime ( float afDecayTime)								{ mfDecayTime = afDecayTime; }
		inline void SetDecayHFRatio ( float afDecayHFRatio )						{ mfDecayHFRatio = afDecayHFRatio; }
		inline void SetDecayLFRatio ( float afDecayLFRatio )						{ mfDecayLFRatio = afDecayLFRatio; }
		inline void SetReflectionsGain ( float afReflectionsGain )					{ mfReflectionsGain = afReflectionsGain; }
		inline void SetReflectionsDelay( float afReflectionsDelay )					{ mfReflectionsDelay = afReflectionsDelay; }
		inline void SetReflectionsPan( float afReflectionsPan[3] )					{ mfReflectionsPan[0] = afReflectionsPan[0]; mfReflectionsPan[1] = afReflectionsPan[1]; mfReflectionsPan[2] = afReflectionsPan[3]; }
		inline void SetLateReverbGain( float afLateReverbGain)						{ mfLateReverbGain = afLateReverbGain; }
		inline void SetLateReverbDelay ( float afLateReverbDelay)					{ mfLateReverbDelay = afLateReverbDelay; }
		inline void SetLateReverbPan (float afLateReverbPan[3])						{ mfLateReverbPan[0] = afLateReverbPan[0]; mfLateReverbPan[1] = afLateReverbPan[1]; mfLateReverbPan[2] = afLateReverbPan[3]; }
		inline void SetEchoTime (float afEchoTime)									{ mfEchoTime = afEchoTime; }
		inline void SetEchoDepth (float afEchoDepth)								{ mfEchoDepth = afEchoDepth; }
		inline void SetModulationTime (float afModulationTime)						{ mfModulationTime = afModulationTime; }
		inline void SetModulationDepth (float afModulationDepth)					{ mfModulationDepth = afModulationDepth; }
		inline void SetAirAbsorptionGainHF (float afAirAbsorptionGainHF)			{ mfAirAbsorptionGainHF = afAirAbsorptionGainHF; }
		inline void SetHFReference (float afHFReference)							{ mfHFReference = afHFReference; }
		inline void SetLFReference (float afLFReference)							{ mfLFReference = afLFReference; }
		inline void SetRoomRolloffFactor (float afRoomRolloffFactor)				{ mfRoomRolloffFactor = afRoomRolloffFactor; }
		inline void SetDecayHFLimit (int abDecayHFLimit)							{ mbDecayHFLimit = abDecayHFLimit; }

	protected:

		float	mfDensity;
		float	mfDiffusion;
		float	mfGain;
		float	mfGainHF;
		float	mfGainLF;
		float	mfDecayTime;
		float	mfDecayHFRatio;
		float	mfDecayLFRatio;
		float	mfReflectionsGain;
		float	mfReflectionsDelay;
		float	mfReflectionsPan[3];
		float	mfLateReverbGain;
		float	mfLateReverbDelay;
		float	mfLateReverbPan[3];
		float	mfEchoTime;
		float	mfEchoDepth;
		float	mfModulationTime;
		float	mfModulationDepth;
		float	mfAirAbsorptionGainHF;
		float	mfHFReference;
		float	mfLFReference;
		float	mfRoomRolloffFactor;
		int		mbDecayHFLimit;
	
	};
	

	
}


#endif	// HPL_OPENALSOUNDENVIRONMENT_H




