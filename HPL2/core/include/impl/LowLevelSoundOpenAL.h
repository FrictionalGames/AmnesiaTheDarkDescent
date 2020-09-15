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

#ifndef HPL_LOWLEVELSOUND_OPENAL_H
#define HPL_LOWLEVELSOUND_OPENAL_H

#include "sound/LowLevelSound.h"

class cOAL_Effect_Reverb;


namespace hpl 
{

	class cSoundDeviceIdentifierOpenAL : public iSoundDeviceIdentifier
	{
	public:
		cSoundDeviceIdentifierOpenAL(int alID, const tString& asName, bool abDefault);

		int GetID() { return mlID; }
		const tString& GetName() { return msName; }
		bool IsDefault() { return mbDefault; }
	private:
		int mlID;
		tString msName;
		bool mbDefault;
	};

	class cLowLevelSoundOpenAL : public iLowLevelSound
	{
	public:
		cLowLevelSoundOpenAL();
		~cLowLevelSoundOpenAL();

		void GetSupportedFormats(tStringList &alstFormats);

		iSoundData* LoadSoundData(const tString& asName,const tWString& asFilePath,
									const tString& asType, bool abStream,bool abLoopStream);

		void UpdateSound(float afTimeStep);

		void SetListenerAttributes (const cVector3f &avPos,const cVector3f &avVel,
								const cVector3f &avForward,const cVector3f &avUp);
		void SetListenerPosition(const cVector3f &avPos);

		void SetSetRolloffFactor(float afFactor);

		void SetListenerAttenuation (bool abEnabled);

//		void LogSoundStatus();

		void Init(int alSoundDeviceID, bool abUseEnvAudio,int alMaxChannels, 
					int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
					int alMaxMonoSourceHint, int alMaxStereoSourceHint,
					int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog);

		void SetVolume(float afVolume);

		void SetEnvVolume( float afEnvVolume );

		iSoundEnvironment* LoadSoundEnvironment (const tString& asFilePath);
		void SetSoundEnvironment ( iSoundEnvironment* apSoundEnv );
		void FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT );

		iSoundDeviceIdentifier* GetCurrentSoundDevice();

	private:
		iSoundDeviceIdentifier* GetFirstValidDefaultDevice();
		iSoundDeviceIdentifier* GetFirstDefaultDevice();

		tString mvFormats[30];
		bool	mbLogSounds;
		bool	mbInitialized;
		int		mlEffectSlotId;
		bool	mbNullEffectAttached;

		cOAL_Effect_Reverb* mpEffect;
		int mlCurrentSoundDevID;
	};
};
#endif // HPL_LOWLEVELSOUND_OPENAL_H

