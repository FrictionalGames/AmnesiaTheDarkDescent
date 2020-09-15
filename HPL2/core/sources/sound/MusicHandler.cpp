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

#include "sound/MusicHandler.h"
#include "resources/Resources.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundChannel.h"
#include "sound/SoundData.h"
#include "resources/SoundManager.h"
#include "system/Platform.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMusicHandler::cMusicHandler(iLowLevelSound* apLowLevelSound, cResources* apResources)
	{
		mpLowLevelSound = apLowLevelSound;
		mpResources = apResources;

		mpMainSong = NULL;
		mpLock  = NULL;
		mbIsPaused = false;

		mfVolumeMul = 1.0f;
		mfVolumeMulFadeGoal = 1.0f;
		mfVolumeMulFadeSpeed = 1.0f;
	}

	//-----------------------------------------------------------------------

	cMusicHandler::~cMusicHandler()
	{
		if(mpMainSong){
			hplDelete(mpMainSong->mpStream);
			hplDelete(mpMainSong);
		}
		
		tMusicEntryListIt it = mlstFadingSongs.begin();
		while(it != mlstFadingSongs.end())
		{
			cMusicEntry* pSong = *it;
			hplDelete(pSong->mpStream);
			hplDelete(pSong);

			it = mlstFadingSongs.erase(it);
			//it++;
		}

		STLDeleteAll(mlstResumeEntries);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cMusicHandler::Play(const tString& asFileName,float afVolume, float afFadeStepSize, bool abLoop, bool abResume)
	{
		bool bSongIsPlaying = false;  

		if(mpLock!=NULL){
			mpLock->msFileName = asFileName;
			mpLock->mfVolume = afVolume;
			mpLock->mbLoop = abLoop;
			return true;
		}

		if(mpMainSong != NULL)
			if(asFileName == mpMainSong->msFileName) bSongIsPlaying = true;
		
		if(!bSongIsPlaying)
		{
			//Put the previous song in the fading queue
			if(mpMainSong != NULL)
			{
				mpMainSong->mfVolumeAdd = afFadeStepSize; 
				mlstFadingSongs.push_back(mpMainSong);
			}
			
			//If there the song to be played is in the fade que, stop it.
			tMusicEntryListIt it = mlstFadingSongs.begin();
			while(it != mlstFadingSongs.end())
			{
				cMusicEntry* pSong = *it;
				if(pSong->msFileName == asFileName)
				{
					pSong->mfVolume= 0;
					pSong->mpStream->Stop();
					hplDelete(pSong->mpStream);
					hplDelete(pSong);

					it = mlstFadingSongs.erase(it);
				} else {
					it++;
				}
			}

			
			//add it and set its properties
			mpMainSong = hplNew( cMusicEntry, () );
			
			if(LoadAndStart(asFileName, mpMainSong,0,abLoop, abResume)==false)
			{
				hplDelete(mpMainSong);
				mpMainSong = NULL;
				return false;
			}
		}
		else
		{
			if(mpMainSong->mfMaxVolume == afVolume)return true;
		}

		///////////////////////////
		//Set Properties
		mpMainSong->mfMaxVolume = afVolume;
		mpMainSong->mbLoop = abLoop;
		
		if(mpMainSong->mfMaxVolume > mpMainSong->mfVolume)
			mpMainSong->mfVolumeAdd = afFadeStepSize;
		else
			mpMainSong->mfVolumeAdd = -afFadeStepSize;
		
		
		return true;
	}
	
	//-----------------------------------------------------------------------

	void cMusicHandler::Stop(float afFadeStepSize)
	{
		if(mpMainSong==NULL)return;

		if(afFadeStepSize<0)afFadeStepSize=-afFadeStepSize;

		mpMainSong->mfVolumeAdd = afFadeStepSize; 
		
		UpdateResumeEntry(mpMainSong, afFadeStepSize);
		
		if(afFadeStepSize==0)
		{
			mpMainSong->mpStream->SetVolume(0);
			mpMainSong->mpStream->Stop();
			mpMainSong->mfVolume =0;
		}

		mlstFadingSongs.push_back(mpMainSong);
		mpMainSong = NULL;
	}
	
	//-----------------------------------------------------------------------

	void cMusicHandler::Pause()
	{
		if(mpMainSong != NULL)mpMainSong->mpStream->SetPaused(true);

		tMusicEntryListIt it = mlstFadingSongs.begin();
		while(it != mlstFadingSongs.end()){
			(*it)->mpStream->SetPaused(true);
			it++;
		}

		mbIsPaused = true;
	}
	
	//-----------------------------------------------------------------------

	void cMusicHandler::Resume()
	{
		if(mpMainSong != NULL)mpMainSong->mpStream->SetPaused(false);

		tMusicEntryListIt it = mlstFadingSongs.begin();
		while(it != mlstFadingSongs.end()){
			(*it)->mpStream->SetPaused(false);
			it++;
		}

		mbIsPaused = false;
	}

	//-----------------------------------------------------------------------


	void cMusicHandler::Lock(cMusicLock* apLock)
	{
		mpLock = apLock;
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::UnLock()
	{
		mpLock = NULL;
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::FadeVolumeMul(float afDest, float afSpeed)
	{
		mfVolumeMulFadeGoal = afDest;
		mfVolumeMulFadeSpeed = afSpeed;
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::SetVolumeMul(float afMul)
	{
		mfVolumeMul = afMul;
		mfVolumeMulFadeGoal = afMul;
	}
	
	//-----------------------------------------------------------------------

	tString cMusicHandler::GetCurrentSongName()
	{
		if(mpMainSong!=NULL)
			return mpMainSong->msFileName;
		else
			return "";
	}
	
	//-----------------------------------------------------------------------

	float cMusicHandler::GetCurrentSongVolume()
	{
		if(mpMainSong!=NULL)
			return mpMainSong->mfVolume;
		else
			return 0;
	}

	//-----------------------------------------------------------------------

	cMusicEntry* cMusicHandler::GetCurrentSong()
	{
		return mpMainSong;
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::Update(float afTimeStep)
	{
		if(mbIsPaused)return;

		/////////////////////////////////
		// Update volume mul
		if(mfVolumeMul != mfVolumeMulFadeGoal)
		{
			if(mfVolumeMul < mfVolumeMulFadeGoal)
			{
				mfVolumeMul += afTimeStep*mfVolumeMulFadeSpeed;
				if(mfVolumeMul > mfVolumeMulFadeGoal) mfVolumeMul = mfVolumeMulFadeGoal;
			}
			if(mfVolumeMul > mfVolumeMulFadeGoal)
			{
				mfVolumeMul -= afTimeStep*mfVolumeMulFadeSpeed;
				if(mfVolumeMul < mfVolumeMulFadeGoal) mfVolumeMul = mfVolumeMulFadeGoal;
			}
		}


		/////////////////////////////////
		// Update main song
		if(mpMainSong != NULL)
		{
			if(mpMainSong->mpStream->IsPlaying()==false)
			{
				hplDelete(mpMainSong->mpStream);
				hplDelete(mpMainSong);
				mpMainSong = NULL;
			}
			else
			{
				/////////////////////////////////
				//Update the main song
				mpMainSong->mfVolume+=mpMainSong->mfVolumeAdd*afTimeStep;

				if(mpMainSong->mfVolumeAdd>0)
				{
					if(mpMainSong->mfVolume>=mpMainSong->mfMaxVolume)
						mpMainSong->mfVolume= mpMainSong->mfMaxVolume;
				}
				else
				{
					if(mpMainSong->mfVolume<=mpMainSong->mfMaxVolume)
						mpMainSong->mfVolume= mpMainSong->mfMaxVolume;
				}
				
				float fNewVolume = mpMainSong->mfVolume * mfVolumeMul;

				if(mpMainSong->mpStream->GetVolume()!=fNewVolume)
				{
					mpMainSong->mpStream->SetVolume(fNewVolume);
				}
			}
		}


		//Update the fading songs
		tMusicEntryListIt it = mlstFadingSongs.begin();
		while(it != mlstFadingSongs.end())
		{
			cMusicEntry* pSong = *it;
			pSong->mfVolume-=pSong->mfVolumeAdd*afTimeStep;

			if(pSong->mfVolume<=0)
			{
				//Destroy song
				pSong->mfVolume= 0;
				pSong->mpStream->Stop();
				hplDelete(pSong->mpStream);
				hplDelete(pSong);
				
				it = mlstFadingSongs.erase(it);
			}
			else
			{
				pSong->mpStream->SetVolume(pSong->mfVolume* mfVolumeMul);
				it++;
			}
		}
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::ResetResumeData()
	{
		STLDeleteAll(mlstResumeEntries);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMusicResumeEntry* cMusicHandler::GetResumeEntry(const tString& asFileName)
	{
		for(tMusicResumeEntryListIt it = mlstResumeEntries.begin(); it != mlstResumeEntries.end(); ++it)
		{
			cMusicResumeEntry* pEntry = *it;
			if(pEntry->msFileName == asFileName)
			{
				return pEntry;
			}
		}

		cMusicResumeEntry* pEntry = hplNew(cMusicResumeEntry,());
		pEntry->msFileName = asFileName;
		pEntry->mfCurrentPos = 0.0;
		mlstResumeEntries.push_back(pEntry);

		return pEntry;
	}

	//-----------------------------------------------------------------------

	void cMusicHandler::UpdateResumeEntry(cMusicEntry* apSong, float afFadeStepSize)
	{
		cMusicResumeEntry* pResumeEntry = GetResumeEntry(apSong->msFileName);
		
		float fTimeAdd = 0;
		if(afFadeStepSize > 0) fTimeAdd = apSong->mpStream->GetVolume() / afFadeStepSize; 

		pResumeEntry->mfCurrentPos = apSong->mpStream->GetElapsedTime() + fTimeAdd;
	}
	
	//-----------------------------------------------------------------------
	
	bool cMusicHandler::LoadAndStart(const tString& asFileName,cMusicEntry* apSong  ,float afVolume, bool abLoop, bool abResume)
	{
		/////////////////////////
		// Create data
		iSoundData* pData = mpResources->GetSoundManager()->CreateSoundData(asFileName,true,abLoop);
		if(pData==NULL){
			Error("Couldn't load music '%s'\n",asFileName.c_str());
			return false;
		}
		
		
		/////////////////////////
		// Create stream
		iSoundChannel *pStream = pData->CreateChannel(256);
		if(pStream == NULL)
		{
			//Need to destroy channel else it will never be deleted!
			mpResources->GetSoundManager()->Destroy(pData); 
			
			Error("Couldn't stream music '%s'!\n",asFileName.c_str());
			return false;
		}
		
		apSong->msFileName = asFileName;
		apSong->mpStream = pStream;
		apSong->mpStream->SetVolume(afVolume);

		if(abResume)
		{
			cMusicResumeEntry* pResumeEntry = GetResumeEntry(asFileName);
			double fPos = pResumeEntry->mfCurrentPos;
			if(fPos >= pStream->GetTotalTime()) fPos =0;
			pStream->SetElapsedTime(fPos);	

		}
		
		apSong->mpStream->Play();
		
		return true;
	}
	//-----------------------------------------------------------------------

}
