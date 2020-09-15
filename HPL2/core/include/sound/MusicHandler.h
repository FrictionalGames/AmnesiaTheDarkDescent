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

#ifndef HPL_MUSICHANDLER_H
#define HPL_MUSICHANDLER_H

#include <list>
#include "system/SystemTypes.h"

namespace hpl {

	class iLowLevelSound;
	class iSoundChannel;
	
	
	////////////////////////////////////////////////////
	//////////// MUSIC LOCK ///////////////////////////
	////////////////////////////////////////////////////
	
	class cMusicLock
	{
	public:
		cMusicLock() : msFileName(""), mfVolume(0) {}

		tString msFileName;
		float mfVolume;
		bool mbLoop;
	};
	
	////////////////////////////////////////////////////
	//////////// MUSIC ENTRY ///////////////////////////
	////////////////////////////////////////////////////

	class cMusicEntry
	{
	public:
		cMusicEntry() : msFileName(""), mpStream(NULL), mfMaxVolume(1), 
			mfVolume(0), mfVolumeAdd(0.01f){}

		tString msFileName;
		iSoundChannel* mpStream;
		float mfMaxVolume;
		float mfVolume;
		float mfVolumeAdd;
		bool mbLoop;
	};

	typedef std::list<cMusicEntry*> tMusicEntryList;
	typedef tMusicEntryList::iterator tMusicEntryListIt;

	////////////////////////////////////////////////////
	//////////// RESUME ENTRY ///////////////////////////
	////////////////////////////////////////////////////

	class cMusicResumeEntry
	{
	public:
		tString msFileName;
		double mfCurrentPos;
	};

	typedef std::list<cMusicResumeEntry*> tMusicResumeEntryList;
	typedef tMusicResumeEntryList::iterator tMusicResumeEntryListIt;

	////////////////////////////////////////////////////
	//////////// MUSIC HANDLER ///////////////////////
	////////////////////////////////////////////////////

	class cResources;

	class cMusicHandler
	{
	public:
		cMusicHandler(iLowLevelSound* apLowLevelSound, cResources* apResources);
		~cMusicHandler();

		/**
		 * Play a song. Playing a song that is already playing updates it's properties.
		 * \param asFileName file to be played
		 * \param afVolume volume to be played at
		 * \param afFadeStepSize volume increse/decrease per app step when fading to new volume.
		 * \param abLoop If the music should be looped or not.
		 * \return 
		 */
		bool Play(const tString& asFileName,float afVolume, float afFadeStepSize, bool abLoop, bool abResume);
		
		/**
		 * Stop playing the current music.
		 * \param afFadeStepSize volume increse/decrease per app step when fading volume to 0.
		 */
		void Stop(float afFadeStepSize);
		void Pause();
		void Resume();

		void FadeVolumeMul(float afDest, float afSpeed);
		void SetVolumeMul(float afMul);
		float GetVolumeMul(){ return mfVolumeMul;}
		
		
		/**
		 * No more music can be played when locked. Latest song that has been tried to be palyed is saved in lock.
		 * \param apLock 
		 */
		void Lock(cMusicLock* apLock);
		/**
		 * Remove lock.
		 */
		void UnLock();
		tString GetCurrentSongName();
		float GetCurrentSongVolume();
		
		cMusicEntry* GetCurrentSong();
		
		void Update(float afTimeStep);

		void ResetResumeData();

	private:
		iLowLevelSound* mpLowLevelSound;
		cResources* mpResources;

		tMusicEntryList mlstFadingSongs;
		cMusicEntry* mpMainSong;
		cMusicLock* mpLock;
		bool mbIsPaused;

		float mfVolumeMul;
		float mfVolumeMulFadeGoal;
		float mfVolumeMulFadeSpeed;

		tMusicResumeEntryList mlstResumeEntries;

		cMusicResumeEntry* GetResumeEntry(const tString& asFileName);
		void UpdateResumeEntry(cMusicEntry* apSong, float afFadeStepSize);
		bool LoadAndStart(const tString& asFileName,cMusicEntry* apSong  ,float afVolume, bool abLoop, bool abResume);
	};

};
#endif // HPL_MUSICHANDLER_H
