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

#ifndef HPL_SOUND_ENTITY_H
#define HPL_SOUND_ENTITY_H

#include "scene/Entity3D.h"
#include "sound/SoundTypes.h"

namespace hpl {

	class cSoundHandler;
	class cSoundEntityData;
	class cSoundEntityManager;
	class cWorld;
	class cSoundEntry;

	class cSoundEntity;

	class cSoundEntityChannelCallback : public iSoundEntryCallback
	{
	public:
		void OnPriorityRelease();

		cSoundEntity *mpEntity;
	};

	//------------------------------------------

	class iSoundEntityGlobalCallback
	{
	public:
		virtual void OnStart(cSoundEntity *apSoundEntity)=0;
	};

	typedef std::list<iSoundEntityGlobalCallback*> tSoundEntityGlobalCallbackList;
	typedef tSoundEntityGlobalCallbackList::iterator tSoundEntityGlobalCallbackListIt;

	//------------------------------------------

	class cSoundEntity : public iEntity3D
	{
	#ifdef __GNUC__
		typedef iEntity3D __super;
	#endif
	friend class cSoundEntityChannelCallback;
	public:
		cSoundEntity(const tString& asName,cSoundEntityData *apData,
						cSoundEntityManager *apSoundEntityManager,
						cWorld *apWorld,
						cSoundHandler *apSoundHandler, bool abRemoveWhenOver, int alCreationID);
		~cSoundEntity();

		//void Setup(const tString& asName,cSoundEntityData *apData, bool abRemoveWhenOver, alCreationID);

		void Play(bool abPlayStart=true);
		void Stop(bool abPlayEnd=true);

		void FadeIn(float afSpeed);
		void FadeOut(float afSpeed);

        bool IsStopped();
		bool IsFadingOut();
		bool GetRemoveWhenOver();

		void SetVolume(float afX){ mfVolume = afX;}
		float GetVolume(){ return mfVolume;}

		float GetMinDistance(){ return mfMinDistance;}
		float GetMaxDistance(){ return mfMaxDistance;}
		void SetMinDistance(float afX){ mfMinDistance = afX;}
		void SetMaxDistance(float afX){ mfMaxDistance = afX;}

		void SetForcePlayAsGUISound(bool abX){ mbForcePlayAsGUISound = abX;}
		bool GetForcePlayAsGUISound(){ return mbForcePlayAsGUISound;}
		
		/**
		 * Gets the sound entry. return can be NULL whether validity is checked or not, best is to ALWAYS check validity, unless the entry has been validated in same code block.
		 */
		cSoundEntry* GetSoundEntry(eSoundEntityType aType, bool abCheckEntryValidity);

        //Entity implementation
		void UpdateLogic(float afTimeStep);

		tString GetEntityType(){ return "SoundEntity";}

		cSoundEntityData* GetData(){ return mpData;}

		int GetCreationID(){ return mlCreationID; }

		static void AddGlobalCallback(iSoundEntityGlobalCallback *apCallback);
		static void RemoveGlobalCallback(iSoundEntityGlobalCallback *apCallback);


	private:
		bool CheckIsOutOfRange();

		bool PlaySound(eSoundEntityType aType,bool abLoop);

		float GetListenerSqrLength();

		cSoundEntityManager *mpSoundEntityManager;
		cSoundHandler *mpSoundHandler;
		cSoundEntityData *mpData;
		cWorld *mpWorld;

		cSoundEntry *mvSoundEntries[3];
		int mvSoundEntryID[3];

		int mlCreationID;

		bool mbForcePlayAsGUISound;

		float mfMinDistance;
		float mfMaxDistance;
		
		bool mbStopped;
		bool mbRemoveWhenOver;

		bool mbOutOfRange;

		float mfIntervalCount;

		cSoundEntityChannelCallback *mpSoundCallback;

		bool mbStarted;

		bool mbFadingOut;

		bool mbLog;
	
		float mfVolume;

		bool mbPrioRemove;

		bool mbSkipStartEnd;

		float mfSleepCount;

		static tSoundEntityGlobalCallbackList mlstGobalCallbacks;
	};

};
#endif // HPL_SOUND_ENTITY_H
