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

#ifndef HPL_SOUNDHANDLER_H
#define HPL_SOUNDHANDLER_H

#include <list>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "sound/SoundTypes.h"
#include "engine/EngineTypes.h"

#include "physics/PhysicsWorld.h"

namespace hpl {
	
	class iLowLevelSound;
	class iSoundChannel;
	class cWorld;

	//----------------------------------------

	enum eSoundEntryType
	{
		eSoundEntryType_World = eFlagBit_0,
		eSoundEntryType_Gui =	eFlagBit_1,
		eSoundEntryType_All =	eFlagBit_All
	};

	//----------------------------------------
	
	class cSoundRayCallback :public iPhysicsRayCallback
	{
	public:
		void Reset();
		bool HasCollided(){ return mbHasCollided;}
		
		bool BeforeIntersect(iPhysicsBody *pBody);
		bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);
		
	private:
		bool mbHasCollided;
		int mlCount;
	};

	//----------------------------------------

	////////////////////////////////////////////////////
	//////////// SOUND ENTRY ///////////////////////////
	////////////////////////////////////////////////////

	//----------------------------------------

	class cSoundHandler;

	//----------------------------------------
	
	class cSoundEntry
	{
	public:
		cSoundEntry(const tString& asName, iSoundChannel* apSound, float afVolume,
					eSoundEntryType aType, bool ab3D,
					bool abStream,int alId, 
					cSoundHandler *apSoundHandler);
		~cSoundEntry();

		bool Update(float afTimeStep);

		inline const tString& GetName() const { return msName;}
		inline eSoundEntryType GetType() const { return mType; }
		inline int GetId() const { return mlId; }
		inline iSoundChannel* GetChannel() const { return mpSound; }
		
		inline bool IsFirstTime(){ return mbFirstTime; }

		void Stop();
		void SetPaused(bool abX);

		void SetDefaultVolume(float afVolume);
		void SetDefaultSpeed(float afSpeed);

		void SetVolumeMul(float afMul);
		void SetSpeedMul(float afMul);

		void FadeVolumeMulTo(float afDestMul, float afSpeed);
		void FadeSpeedMulTo(float afDestMul, float afSpeed);
		
		void FadeOut(float afSpeed);
		void FadeIn(float afVolumeMul,float afSpeed);

		iSoundEntryCallback* GetCallBack(){ return mpCallback;}
		void SetCallBack(iSoundEntryCallback* apCallback){ mpCallback = apCallback;}

		void SetStopDisabled(bool abX){mbStopDisabled = abX;}
		bool GetStopDisabled(){ return mbStopDisabled;}
		
	private:
		void UpdateVolumeMulFade(float afTimeStep);
		void UpdateSpeedMulFade(float afTimeStep);

		void Update3DSpecifics(float afTimeStep);
		
		tString msName;
		iSoundChannel* mpSound;
		cSoundHandler *mpSoundHandler;

		eSoundEntryType mType;
		int mlId;

		bool mb3D;

		float mfNormalVolume;
		float mfVolumeMul;
		float mfVolumeFadeDest;
		float mfVolumeFadeSpeed;
		bool mbStopAfterFadeOut;
				
		float mfNormalSpeed;
		float mfSpeedMul;
		float mfSpeedFadeDest;
		float mfSpeedFadeSpeed;

		bool mbFirstTime;
		
		float mfBlockMul;
		float mfBlockFadeDest;
		float mfBlockFadeSpeed;

		bool mbStream;
		bool mbStopDisabled;

		iSoundEntryCallback *mpCallback;
	};
	
	//----------------------------------------
	
	////////////////////////////////////////////////////
	//////////// SOUND HANDLER ///////////////////////
	////////////////////////////////////////////////////
	
	
	//----------------------------------------

	typedef std::list<cSoundEntry*> tSoundEntryList;
	typedef tSoundEntryList::iterator tSoundEntryListIt;
	typedef cSTLIterator<cSoundEntry,tSoundEntryList,tSoundEntryListIt> tSoundEntryIterator;

	class cResources;

	//----------------------------------------

	class cSoundHandler
	{
	friend class cSoundEntry;
	public:
		cSoundHandler(iLowLevelSound* apLowLevelSound, cResources* apResources);
		~cSoundHandler();

		void Update(float afTimeStep);

		/**
		 * Plays a sound
		 * \param asName Name of the file to load. 
		 * \param abLoop If the sound shall be looping
		 * \param afVolume The start volume of the sound
		 * \param avPos The position to play it at
		 * \param afMinDist Inside this distance max volume (afVolume) is heard, after it is lowered, only used if 3D.
		 * \param afMaxDist Beyond this distance volume is 0
		 * \param mEntryType The type of sound. Use to determine what effects affect it.
		 * \param abRelative If position is relative to listener
		 * \param ab3D If the sound uses 3D calculations to determine volume and other world effects.
		 * \param alPriorityModifier The priority of the sound, if 3D then this number is added to a calculated priority based on distance from listener.
		 * \return 
		 */
		cSoundEntry* Play(	const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,  
							float afMinDist,float afMaxDist, eSoundEntryType aEntryType, 
							bool abRelative, bool ab3D, int alPriorityModifier, bool abStream,
							bool *apNotEnoughChannels=NULL);
		
		cSoundEntry* Play3D(	const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,  
								float afMinDist,float afMaxDist, 
								eSoundEntryType aEntryType = eSoundEntryType_World, 
								bool abRelative=false,
								int alPriorityModifier=0, bool abStream=false, bool *apNotEnoughChannels=NULL);
		
		cSoundEntry* PlayGuiStream(	const tString& asFileName,bool abLoop,float afVolume,const cVector3f& avPos=cVector3f(0,0,1),
									eSoundEntryType aEntryType = eSoundEntryType_Gui, bool *apNotEnoughChannels=NULL);

		cSoundEntry* PlayGui(	const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos=cVector3f(0,0,1),
								eSoundEntryType aEntryType = eSoundEntryType_Gui, bool *apNotEnoughChannels=NULL);

		cSoundEntry* PlaySoundEntityGui(const tString& asName,bool abLoop,float afVolume,
										eSoundEntryType aEntryType = eSoundEntryType_Gui,
										const cVector3f& avPos=cVector3f(0,0,1), bool *apNotEnoughChannels=NULL);


		void SetSilent(bool abX){ mbSilent = abX; }
		bool GetSilent(){ return mbSilent; }
		
		bool Stop(const tString& asName);
		bool StopAllExcept(const tString& asName);
		
		void StopAll(tFlag mTypes);
		void PauseAll(tFlag mTypes);
		void ResumeAll(tFlag mTypes);
		void FadeOutAll(tFlag mTypes,float afFadeSpeed, bool abDisableStop);

		bool IsPlaying(const tString& asName);

		bool IsValid(cSoundEntry *apEntry, int alID);
		
		/**
		 * Set the global volume for a specific type(s). If alId >= 0 then the settings will be applied to that id, else a free id will be found. 
		 * \param afVolume Global Volume to be set.
		 * \param mAffectedTypes Type(s) affected by this setting.
		 * \param alId The Id to the setting applies to, if < 0, the the a free one is found.
		 * \return if alId >=0, alId is returned, else the lowest free id.
		 */
		int SetGlobalVolume(float afVolume, tFlag mAffectedTypes, int alId);
		int SetGlobalSpeed(float afSpeed,tFlag mAffectedTypes, int alId);

		int FadeGlobalVolume(float afDestVolume, float afSpeed,tFlag mAffectedTypes, int alId, bool abDestroyIdAtDest);
		int FadeGlobalSpeed(float afDestSpeed, float afSpeed,tFlag mAffectedTypes, int alId, bool abDestroyIdAtDest);

		float GetGlobalVolume(eSoundEntryType aType);
		float GetGlobalSpeed(eSoundEntryType aType);

		cMultipleSettingsHandler* GetGlobalVolumeSettingsHandler(){ return &mGlobalVolumeHandler;}
		cMultipleSettingsHandler* GetGlobalSpeedSettingsHandler(){ return &mGlobalSpeedHandler;}

		void SetWorld(cWorld *apWorld);

		iSoundChannel* CreateChannel(const tString& asName, int alPriority, bool abStream, bool *apNotEnoughChannels);

		tSoundEntryList* GetEntryList();
		
		bool CheckSoundIsBlocked(const cVector3f& avSoundPosition);
	
	private:
		cSoundEntry* GetEntry(const tString& asName);

		iLowLevelSound* mpLowLevelSound;
		cResources* mpResources;

		tSoundEntryList m_lstSoundEntries;

		//tSoundEntryList m_lstSoundEntriesPool;
		
		bool mbSilent;

		cWorld *mpWorld;

		cSoundRayCallback mSoundRayCallback;

		int mlCount;
		int mlIdCount;

		float mfGlobalVolume[2];
		float mfGlobalSpeed[2];

		cMultipleSettingsHandler mGlobalVolumeHandler;
		cMultipleSettingsHandler mGlobalSpeedHandler;
	};

};
#endif // HPL_SOUNDHANDLER_H
