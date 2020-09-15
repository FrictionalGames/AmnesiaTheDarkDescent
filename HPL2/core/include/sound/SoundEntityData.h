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

#ifndef HPL_SOUND_ENTITY_DATA_H
#define HPL_SOUND_ENTITY_DATA_H

#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"
#include "sound/SoundTypes.h"

namespace hpl {

	class cResources;
	class cXmlElement;
	class cSound;

	class cSoundEntityData : public iResourceBase
	{
	friend class cSoundEntity;
	public:
		cSoundEntityData(const tString& asName, cResources *apResources, cSound *apSound);
		~cSoundEntityData();

		const tString& GetSoundName(eSoundEntityType aType, int alIdx);
		const tString& GetRandomSoundName(eSoundEntityType aType, bool abSkipPrevious);
		int GetSoundNum(eSoundEntityType aType);
		void SetSoundName(const tString& asName, eSoundEntityType aType, int alIdx);
		void AddSoundName(const tString& asName, eSoundEntityType aType);
		inline bool HasSound(eSoundEntityType aType){ return mvSoundNameVecs[aType].empty()==false;}
		
		void PreloadSounds();

		bool CreateFromFile(const tWString &asFile);

		void SetFadeStart(bool abX){mbFadeStart = abX;}
		bool GetFadeStart(){return mbFadeStart;}
		void SetFadeStop(bool abX){mbFadeStop = abX;}
		bool GetFadeStop(){return mbFadeStop;}

		void SetVolume(float afX){mfVolume = afX;}
		float GetVolume(){return mfVolume;}
		void SetMaxDistance(float afX){mfMaxDistance =afX;}
		float GetMaxDistance(){return mfMaxDistance;}
		void SetMinDistance(float afX){mfMinDistance = afX;}
		float GetMinDistance(){return mfMinDistance;}

		void SetStream(bool abX){mbStream = abX;}
		bool GetStream(){return mbStream;}
		void SetLoop(bool abX){mbLoop = abX;}
		bool GetLoop(){return mbLoop;}
		void SetUse3D(bool abX){ mbUse3D = abX;}
		bool GetUse3D(){ return mbUse3D;}

		void SetBlockable(bool abX){ mbBlockable = abX;}
		bool GetBlockable(){ return mbBlockable;}
		void SetBlockVolumeMul(float afX){ mfBlockVolumeMul = afX;}
		float GetBlockVolumeMul(){ return mfBlockVolumeMul;}

		void SetRandom(float afX){mfRandom = afX;}
		float GetRandom(){return mfRandom;}
		void SetInterval(float afX){mfInterval = afX;}
		float GetInterval(){return mfInterval;}

		void SetPriority(int alX){ mlPriority = alX;}
		int GetPriority(){ return mlPriority;}
		
		//Resource implementation
		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}
	
	protected:
		void LoadSoundsInElement(cXmlElement *apElement, tStringVec *apStringVec);
		void PreloadSoundsOfType(eSoundEntityType aType);

		cResources *mpResources;
		cSound *mpSound;

		tStringVec mvSoundNameVecs[eSoundEntityType_LastEnum];
		int mlPrevious[eSoundEntityType_LastEnum];

		bool mbFadeStart;
		bool mbFadeStop;
        
		float mfVolume;
		float mfMaxDistance;
		float mfMinDistance;
		
		bool mbStream;
		bool mbLoop;
		bool mbUse3D;
		
		bool mbBlockable;
		float mfBlockVolumeMul;

		float mfRandom;
		float mfInterval;

		int mlPriority;
	};
};
#endif // HPL_SOUND_DATA_H
