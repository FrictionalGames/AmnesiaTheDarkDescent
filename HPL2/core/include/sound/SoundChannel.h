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

#ifndef HPL_SOUND_CHANNEL_H
#define HPL_SOUND_CHANNEL_H

#include <list>
#include "math/MathTypes.h"


namespace hpl {
	class iSoundData;
	class cSoundManager;
	
	//--------------------------------------------

	class iSoundChannel
	{
	public:
		iSoundChannel(iSoundData* apData,cSoundManager* apSoundManger);
		virtual ~iSoundChannel();

		virtual void Play()=0;
		virtual void Stop()=0;
		
		virtual void SetPaused(bool abX)=0;
		virtual void SetSpeed(float afSpeed)=0;
		virtual void SetVolume (float afVolume)=0; 
		virtual void SetLooping (bool abLoop)=0;
		virtual void SetPan (float afPan)=0;
		virtual void Set3D(bool ab3D)=0;

		virtual void SetPriority(int alX)=0;
		virtual int GetPriority()=0;

		void SetPriorityModifier(int alX){ 
			mlPriorityModifier = alX;
			SetPriority(GetPriority());
		}
		int GetPriorityModifier(){ return mlPriorityModifier;}

		virtual void SetPositionIsRelative(bool abRelative)=0;
		virtual void SetPosition(const cVector3f &avPos)=0;
		void SetRelPosition(const cVector3f &avPos){ mvRelPosition = avPos;}

		virtual void SetVelocity(const cVector3f &avVel)=0;
		
		virtual void SetMinDistance(float fMin)=0;
		virtual void SetMaxDistance(float fMax)=0;

		virtual bool IsPlaying()=0;
		virtual bool IsBufferUnderrun()=0;
		virtual double GetElapsedTime()=0;
		virtual double GetTotalTime()=0;
		virtual void SetElapsedTime(double afTime)=0;

		bool GetPaused(){return mbPaused;}
		float GetSpeed(){return mfSpeed;}
		float GetVolume (){return mfVolume;} 
		bool GetLooping (){return mbLooping;}
		float GetPan (){return mfPan;}
		bool Get3D (){return mb3D;}

		bool GetStopUsed(){ return mbStopUsed;}

		bool GetBlockable(){ return mbBlockable;}
		void SetBlockable(bool abX){ mbBlockable = abX;}
		void SetBlockVolumeMul(float afX){ mfBlockVolumeMul = afX;}
		float GetBlockVolumeMul(){ return mfBlockVolumeMul;}

		bool GetPositionIsRelative(){return mbPositionRelative;}
		
		const cVector3f& GetRelPosition(){ return mvRelPosition;}
		const cVector3f& GetPosition(){ return mvPosition;}
		const cVector3f& GetVelocity (){ return mvVelocity;}
		
		float GetMinDistance(){ return mfMinDistance;} 
		float GetMaxDistance(){ return mfMaxDistance;} 

		iSoundData* GetData(){return mpData;}

		//virtual void SetFiltering ( iFilter* apFilter, int alFlags ) = 0;
		virtual void SetAffectedByEnv ( bool abAffected )  { mbAffectedByEnv = abAffected; }
		virtual void SetFiltering ( bool abEnabled, int alFlags ) = 0;
		virtual void SetFilterGain(float afGain) =0;
		virtual void SetFilterGainHF(float afGainHF)=0;
		
	protected:
		void DestroyData();

		iSoundData* mpData;
		cSoundManager* mpSoundManger;

		bool mbLooping;
		bool mbPaused;
		bool mbPositionRelative;
		bool mb3D;

		float mfSpeed;
		float mfVolume;
		float mfPan;
		float mfMaxDistance;
		float mfMinDistance;

		cVector3f mvVelocity;
		cVector3f mvPosition;
		cVector3f mvRelPosition;

		bool mbBlockable;
		float mfBlockVolumeMul;

		bool mbAffectedByEnv;
        

		int mlPriority;
		int mlPriorityModifier;

		bool mbStopUsed;
	};

	typedef std::list<iSoundChannel*> tSoundChannelList;
	typedef tSoundChannelList::iterator tSoundChannelListIt;
};
#endif // HPL_SOUND_CHANNEL_H
