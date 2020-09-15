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

#ifndef HPL_FMOD_SOUND_CHANNEL_H
#define HPL_FMOD_SOUND_CHANNEL_H

#include "sound/SoundChannel.h"
#include "sound/SoundData.h"

#include <fmod/fmod.h>

namespace hpl {

	class cFmodSoundChannel : public iSoundChannel
	{
	public:
		cFmodSoundChannel(iSoundData* apData, int alChannel, cSoundManager* apSoundManger);
		~cFmodSoundChannel();

		void Play();
		void Stop();

		void SetPaused(bool abX);
		void SetSpeed(float afSpeed);
		void SetVolume (float afVolume); 
		void SetLooping (bool abLoop);
		void SetPan (float afPan);
		void Set3D(bool ab3D);

		void SetPriority(int alX);
		int GetPriority();

		void SetPositionIsRelative(bool abRelative);
		void SetPosition(const cVector3f &avPos);
		void SetVelocity(const cVector3f &avVel);
		
		void SetMinDistance(float fMin);
		void SetMaxDistance(float fMax); 

		bool IsPlaying();
		bool IsBufferUnderrun(){return false;}
		double GetElapsedTime () { return 0;}
		double GetTotalTime () { return 0; }
		void SetElapsedTime(double afTime){}

		void SetAffectedByEnv(bool abAffected) {}
		void SetFiltering ( bool abEnabled, int alFlags) {}
		void SetFilterGain(float afGain) {}
		void SetFilterGainHF(float afGainHF) {}
		
	
	private:
		int mlChannel;
		int mlDefaultFreq;

		float mfPosition[3];
		float mfVelocity[3];
	};
};
#endif // HPL_SOUND_CHANNEL_H
