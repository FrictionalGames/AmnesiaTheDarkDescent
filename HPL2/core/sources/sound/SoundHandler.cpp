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

#include "sound/SoundHandler.h"

#include "resources/Resources.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundChannel.h"
#include "resources/SoundManager.h"
#include "sound/SoundData.h"
#include "sound/SoundEntityData.h"
#include "resources/SoundEntityManager.h"

#include "scene/World.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// RAY CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cSoundRayCallback::Reset()
	{
		mbHasCollided = false;
	}

	//-----------------------------------------------------------------------

	bool cSoundRayCallback::BeforeIntersect(iPhysicsBody *pBody)
	{
		if(pBody->GetBlocksSound()) return true;

		return false;
	}
	
	//-----------------------------------------------------------------------

	bool cSoundRayCallback::OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams)
	{
		mbHasCollided = true;
		return false;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SOUND ENTRY
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	const bool gbLogEntry = false;
	//-----------------------------------------------------------------------

	cSoundEntry::cSoundEntry(	const tString& asName, iSoundChannel* apSound, float afVolume, 
								eSoundEntryType aType, bool ab3D, 
								bool abStream, int alId,
								cSoundHandler *apSoundHandler)
	{
		msName = cString::ToLowerCase(asName);
		mpSound = apSound;
		mfNormalVolume = afVolume;
		mType = aType;
		mbStream = abStream;
		mlId = alId;
		mpSoundHandler = apSoundHandler;
		mb3D = ab3D;
		
		////////////////////////
		// Set up defaults
		mfVolumeMul = 1;
		mfVolumeFadeDest = 1;
		mfVolumeFadeSpeed =0;
		mbStopAfterFadeOut = false;
		
		mbStopDisabled = false;

		mfNormalSpeed = 1;

		mfSpeedMul = 1;
		mfSpeedFadeDest = 1;
		mfSpeedFadeSpeed = 0;

		mbFirstTime = true;

		mfBlockMul = 1;
		mfBlockFadeDest = 1;
		mfBlockFadeSpeed = 0;

		mpCallback = NULL;

		if(gbLogEntry)Log("Creating sound entry %d id: %d\n", this, mlId);
	}

	///////////////////////////////////////
	//DEBUG when using pooling test
	/*void cSoundEntry::Setup(const tString& asName, iSoundChannel* apSound, float afVolume,
		eSoundEntryType aType, bool ab3D,
		bool abStream,int alId)
	{
		msName = cString::ToLowerCase(asName);
		mpSound = apSound;
		mfNormalVolume = afVolume;
		mType = aType;
		mbStream = abStream;
		mlId = alId;
		mb3D = ab3D;

		////////////////////////
		// Set up defaults
		mfVolumeMul = 1;
		mfVolumeFadeDest = 1;
		mfVolumeFadeSpeed =0;
		mbStopAfterFadeOut = false;

		mfNormalSpeed = 1;

		mfSpeedMul = 1;
		mfSpeedFadeDest = 1;
		mfSpeedFadeSpeed = 0;

		mbFirstTime = true;

		mfBlockMul = 1;
		mfBlockFadeDest = 1;
		mfBlockFadeSpeed = 0;

		mpCallback = NULL;
	}*/

	//-----------------------------------------------------------------------

	cSoundEntry::~cSoundEntry()
	{
		if(gbLogEntry)Log("Destroying sound entry %d id: %d\n", this, mlId);
		if(mpSound)
		{
			mpSound->Stop();
			hplDelete( mpSound );
		}
	}

	//-----------------------------------------------------------------------
	
	bool cSoundEntry::Update(float afTimeStep)
	{
		////////////////////////////////////////////
		// Update Fading
		UpdateVolumeMulFade(afTimeStep);
		UpdateSpeedMulFade(afTimeStep);

		////////////////////////////////////////////
		// Set final speed 
		float fFinalSpeed = mfNormalSpeed * mfSpeedMul * mpSoundHandler->GetGlobalSpeed(mType);
		if(mpSound->GetSpeed() != fFinalSpeed)
		{
			mpSound->SetSpeed(fFinalSpeed);
		}
	
		//////////////////////////////
		// Update block volume mul fade
		if(mfBlockMul != mfBlockFadeDest)
		{
			mfBlockMul += mfBlockFadeSpeed*afTimeStep;
			if(mfBlockFadeSpeed<0)
			{
				if(mfBlockMul < mfBlockFadeDest)	mfBlockMul = mfBlockFadeDest;
			}
			else
			{
				if(mfBlockMul > mfBlockFadeDest)	mfBlockMul = mfBlockFadeDest;
			}
		}

		//////////////////////////////
		//If relative, update position according to listener
		if(mpSound->GetPositionIsRelative())
		{
			cVector3f vPos = cMath::MatrixMul(	mpSoundHandler->mpLowLevelSound->GetListenerMatrix(),mpSound->GetRelPosition() );
			mpSound->SetPosition(vPos);
		}
		
		/////////////////////////////
		// Update 3D Specifics
		if(mb3D)
		{
			Update3DSpecifics(afTimeStep);

		}
		/////////////////////////////
		// Update Non-3D Specifics
		else
		{
			float fFinalVolume = mfNormalVolume * mfVolumeMul * mpSoundHandler->GetGlobalVolume(mType);
			if(mpSound->GetVolume() != fFinalVolume)
			{
				mpSound->SetVolume(fFinalVolume);
			}
		}

		/////////////////////////////
		// If first time and not silent. Start sound!
		if(mbFirstTime && mpSoundHandler->GetSilent()==false)
		{
			mpSound->Play();
			mbFirstTime = false;
		}

		////////////////////////////////////////////
		// Check if sound is done playing
		// Put this last so sounds has a chance to be played! (updating volume of finished sound should not matter)
		if(mpSound->IsPlaying()==false && mpSound->GetPaused()==false)
		{
			//Check if sound has been stopped because of priority override
			if(	mpCallback &&
				mpSound->GetStopUsed()==false && 
				mpSound->GetLooping() &&
				mfVolumeFadeDest !=0)
			{
				if(gbLogEntry)Log("PriorityRelease sound entry %d id: %d\n", this, mlId);
				mpCallback->OnPriorityRelease();
			}

			mpSound->Stop();

			return false;
		}
		

		return true;
	}

	//----------------------------------------------------------------------

	void cSoundEntry::UpdateVolumeMulFade(float afTimeStep)
	{
		if(mfVolumeMul != mfVolumeFadeDest)
		{
			mfVolumeMul += mfVolumeFadeSpeed * afTimeStep;

			if(mfVolumeFadeSpeed<0 && mfVolumeMul <= mfVolumeFadeDest)
			{
				mfVolumeMul = mfVolumeFadeDest;
			}
			else if( mfVolumeFadeSpeed>=0 && mfVolumeMul >= mfVolumeFadeDest)
			{
				mfVolumeMul = mfVolumeFadeDest;
			}
		}
		
		// Stop sound after fade out is set to do so.
		if( mbStopAfterFadeOut && cMath::Abs(mfVolumeMul) < 0.001f)
		{
			mpSound->Stop();
			mbStopAfterFadeOut = false;
		}
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::UpdateSpeedMulFade(float afTimeStep)
	{
		if(mfSpeedMul != mfSpeedFadeDest)
		{
			mfSpeedMul += mfSpeedFadeSpeed * afTimeStep;

			if(mfSpeedFadeSpeed<0 && mfSpeedMul <= mfSpeedFadeDest)
			{
				mfSpeedMul = mfSpeedFadeDest;
			}
			else if( mfSpeedFadeSpeed>=0 && mfSpeedMul >= mfSpeedFadeDest)
			{
				mfSpeedMul = mfSpeedFadeDest;
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::Update3DSpecifics(float afTimeStep)
	{
		cVector3f vListnerPos = mpSoundHandler->mpLowLevelSound->GetListenerPosition();
		bool bBlocked = false;
		float fDistVolumeMul = 1.0f;
	
		////////////////////////////////////////
		// If outside of max distance just set volume and priority to 0
		float fSqrDist = cMath::Vector3DistSqr(mpSound->GetPosition(),vListnerPos);
		if(fSqrDist >= mpSound->GetMaxDistance() * mpSound->GetMaxDistance())
		{
			mpSound->SetVolume(0);
			mpSound->SetPriority(0);
			return;
		}

		////////////////////////////////////////
		// Check if sound is blocked
		if(mpSoundHandler->CheckSoundIsBlocked(mpSound->GetPosition()))
		{
			mfBlockFadeDest = 0.0f;
			mfBlockFadeSpeed = -1.0f / 0.55f;

			if(mbFirstTime)	mfBlockMul = 0.0f;
		
			//pSound->SetFiltering(true, 0xF); TODO
			bBlocked = true;
		}
		else
		{
			mfBlockFadeDest = 1;
			mfBlockFadeSpeed = 1.0f / 0.2f;

			if(mbFirstTime) mfBlockMul = 1.0f;

			//pSound->SetFiltering(false, 0xF); TODO
		}

		///////////////////////////////////////
		// Update volume based on listener distance
		if(fSqrDist< mpSound->GetMinDistance() * mpSound->GetMinDistance())
		{
			//Set high priority
			mpSound->SetPriority(100);
			fDistVolumeMul = 1.0f; 
		}
		else
		{
			//Set medium priority
			mpSound->SetPriority(10);

			float fDist = cMath::Vector3Dist(mpSound->GetPosition(),vListnerPos);
			float fDelta = fDist - mpSound->GetMinDistance();
			float fMaxDelta = mpSound->GetMaxDistance() - mpSound->GetMinDistance();

			fDistVolumeMul = 1 - (fDelta/fMaxDelta);
			
			//TODO: What is this code needed for?
			//fade between normal and square
			//float fSqr = fVolume * fVolume;
			//fVolume *= mfBlockMul + (1.0f - mfBlockMul)*fSqr;
		}
		
		///////////////////////////////////////
		// Set final volume
		
		// Calculate the amount of block.
		float fBlock = mpSound->GetBlockVolumeMul() + mfBlockMul * (1 - mpSound->GetBlockVolumeMul());
		
		//Multiply all factors to get the final volume.
		mpSound->SetVolume(	mfNormalVolume * mfVolumeMul * fBlock * fDistVolumeMul  * 
							mpSoundHandler->GetGlobalVolume(mType));
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::Stop()
	{
		if(mbStopDisabled) return;

		if(mbFirstTime)
		{
			mpSound->Play();
			mbFirstTime = false;
		}
		
		mpSound->SetPaused(false);
		mpSound->Stop();
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::SetPaused(bool abX)
	{
		mpSound->SetPaused(abX);
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::SetDefaultVolume(float afVolume)
	{
		mfNormalVolume = afVolume;
	}
	
	void cSoundEntry::SetDefaultSpeed(float afSpeed)
	{
		mfNormalSpeed = afSpeed;
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::SetVolumeMul(float afMul)
	{
		mfVolumeMul = afMul;
		mfVolumeFadeDest = mfVolumeMul;
	}
	void cSoundEntry::SetSpeedMul(float afMul)
	{
		mfSpeedMul = afMul;
		mfSpeedFadeDest = mfSpeedMul;
	}

	//-----------------------------------------------------------------------

	void cSoundEntry::FadeVolumeMulTo(float afDestMul, float afSpeed)
	{
		if(gbLogEntry)Log("Fade vol sound entry %d id: %d to %f\n", this, mlId, afDestMul);

		mfVolumeFadeDest = afDestMul;
		mfVolumeFadeSpeed = cMath::GetCorrectSignOfSpeed(mfVolumeMul,mfVolumeFadeDest,afSpeed);
	}

	void cSoundEntry::FadeSpeedMulTo(float afDestMul, float afSpeed)
	{
		mfSpeedFadeDest = afDestMul;
		mfSpeedFadeSpeed = cMath::GetCorrectSignOfSpeed(mfSpeedMul,mfSpeedFadeDest,afSpeed);
	}

	void cSoundEntry::FadeOut(float afSpeed)
	{
		if(gbLogEntry)Log("Fade out sound entry %d id: %d\n", this, mlId);

		mfVolumeFadeDest = 0;
		mfVolumeFadeSpeed = cMath::GetCorrectSignOfSpeed(mfVolumeMul,0,afSpeed);
		mbStopAfterFadeOut = true;
	}

	void cSoundEntry::FadeIn(float afVolumeMul,float afSpeed)
	{
		if(gbLogEntry)Log("Fade in sound entry %d id: %d\n", this, mlId);

		mfVolumeFadeDest = afVolumeMul;
		mfVolumeMul = 0;
		mfVolumeFadeSpeed = cMath::GetCorrectSignOfSpeed(0,mfVolumeFadeDest,afSpeed);
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cSoundHandler::cSoundHandler(iLowLevelSound* apLowLevelSound, cResources* apResources)
	{
		mpLowLevelSound = apLowLevelSound;
		mpResources = apResources;

		mpWorld = NULL;

		mlCount =0;
		mlIdCount = 0;

		mbSilent = false;

		mfGlobalVolume[0] = 1;
		mfGlobalVolume[1] = 1;
		mfGlobalSpeed[0] = 1;
		mfGlobalSpeed[1] = 1;
	}
	
	//-----------------------------------------------------------------------

	cSoundHandler::~cSoundHandler()
	{
		STLDeleteAll(m_lstSoundEntries);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSoundHandler::Update(float afTimeStep)
	{
		///////////////////////////////////////////////
		// Update global volume and speed
		
		mfGlobalVolume[0] = mGlobalVolumeHandler.CalcResults(eSoundEntryType_World,eMultipleSettingsCalcType_Min,1.0f);
		mfGlobalVolume[1] = mGlobalVolumeHandler.CalcResults(eSoundEntryType_Gui,eMultipleSettingsCalcType_Min,1.0f);

		mfGlobalSpeed[0] = mGlobalSpeedHandler.CalcResults(eSoundEntryType_World,eMultipleSettingsCalcType_Min,1.0f);
		mfGlobalSpeed[1] = mGlobalSpeedHandler.CalcResults(eSoundEntryType_Gui,eMultipleSettingsCalcType_Min,1.0f);

		mGlobalVolumeHandler.Update(afTimeStep);
		mGlobalSpeedHandler.Update(afTimeStep);

		///////////////////////////////////////////////
		// Update entries
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end();)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry->Update(afTimeStep) == false)
			{
				hplDelete( pEntry );
				//m_lstSoundEntriesPool.push_back(pEntry);
				it = m_lstSoundEntries.erase(it);
			}
			else
			{
				++it;
			}
		}

		mlCount++;
	}
	
	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::Play(	const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,  
										float afMinDist,float afMaxDist, 
										eSoundEntryType aEntryType,bool abRelative, 
										bool ab3D,int alPriorityModifier, bool abStream, bool *apNotEnoughChannels)
	{
		if(asName == "") return NULL;

		/////////////////////////////////
		//Calculate priority
		int lDistPrio = 0;
		if(ab3D && abRelative==false)
		{
			float fDist = cMath::Vector3Dist(avPos, mpLowLevelSound->GetListenerPosition());
			if(fDist >= afMaxDist)
				lDistPrio = 0;
			else if(fDist >= afMinDist)
				lDistPrio = 10;
			else
				lDistPrio = 100;
		}
			
		///////////////////////////////
		//Create sound channel
		if(apNotEnoughChannels) *apNotEnoughChannels = false;
		bool bNotEnoughChannels;
		iSoundChannel *pSound = CreateChannel(asName,lDistPrio + alPriorityModifier, abStream,&bNotEnoughChannels);
		if(pSound == NULL)
		{
			if(apNotEnoughChannels) *apNotEnoughChannels = bNotEnoughChannels;

			if(bNotEnoughChannels==false)
				Error("Can't find sound '%s'!\n",asName.c_str());
			else
				Warning("Could not start sound '%s', too many sounds playing!\n",asName.c_str());
			
			return NULL;
		}

		/////////////////////////////////
		//Set up channel		
		pSound->SetLooping(abLoop);
		pSound->SetMinDistance(afMinDist);
		pSound->SetMaxDistance(afMaxDist);
		pSound->Set3D(ab3D);
		if(ab3D && abRelative==false)
		{
			pSound->SetPriority(lDistPrio);
			pSound->SetPriorityModifier(alPriorityModifier);
		}
		else
		{
			pSound->SetPriority(alPriorityModifier);
		}
		

		///////////////////////////////////
		//Set sound to use Environment if its a world sound
		if (aEntryType == eSoundEntryType_World)
		{
			pSound->SetAffectedByEnv(true);
		}

		//////////////////////////////////////////////////////////////////
		// Setup Position
		pSound->SetPositionIsRelative(abRelative);
		if(abRelative)
		{
			pSound->SetRelPosition(avPos);
			cVector3f vPos = cMath::MatrixMul(mpLowLevelSound->GetListenerMatrix(), avPos);
			pSound->SetPosition(vPos);
		}
		else
		{
			pSound->SetPosition(avPos);							
		}
					
		////////////////////////
		// Set start volume
		//3D
		if(ab3D)
		{
			pSound->SetVolume(0);		
		}
		//Non-3D
		else
		{
			pSound->SetVolume(0);//afVolume);
		}

		///////////////////////
		// If it is silent do everything as normal except stop the sound at start.
		if(mbSilent)
		{
			pSound->SetLooping(false);
			pSound->Stop();
		}

		////////////////////////
		// Create entry
		cSoundEntry *pEntry = hplNew( cSoundEntry, (asName,pSound,afVolume,aEntryType, ab3D, false,mlIdCount,this) );
		/*cSoundEntry *pEntry = NULL;
		if(m_lstSoundEntriesPool.empty())
		{
			pEntry = hplNew( cSoundEntry, (asName,pSound,afVolume,aEntryType, ab3D, false,mlIdCount,this) );
		}
		else
		{
			pEntry = m_lstSoundEntriesPool.back();
			m_lstSoundEntriesPool.pop_back();

			pEntry->Setup(asName,pSound,afVolume,aEntryType, ab3D, false,mlIdCount);
		}*/

		m_lstSoundEntries.push_back(pEntry);

		mlIdCount++;

		return pEntry;
	}

	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::Play3D(	const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,  
										float afMinDist,float afMaxDist, 
										eSoundEntryType aEntryType, bool abRelative,
										int alPriorityModifier, bool abStream, bool *apNotEnoughChannels)
	{
		return Play(asName,abLoop,afVolume,avPos,afMinDist,afMaxDist,aEntryType,abRelative,true,
					alPriorityModifier, abStream,apNotEnoughChannels);
	}

	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::PlayGui(const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,eSoundEntryType aEntryType, bool *apNotEnoughChannels)
	{
		return Play(asName,abLoop,afVolume,avPos,1.0f,1000.0f,aEntryType,true,false,0,false,apNotEnoughChannels);
	}

	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::PlayGuiStream(const tString& asFileName,bool abLoop,float afVolume,const cVector3f& avPos,eSoundEntryType aEntryType, bool *apNotEnoughChannels)
	{
		return Play(asFileName,abLoop,afVolume,avPos,1.0f,1000.0f,aEntryType,true,false,0,true,apNotEnoughChannels);
	}


	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::PlaySoundEntityGui(	const tString& asName,bool abLoop,float afVolume,
													eSoundEntryType aEntryType,
													const cVector3f& avPos, bool *apNotEnoughChannels)
	{
		cSoundEntityData *pData = mpResources->GetSoundEntityManager()->CreateSoundEntity(asName);
		if(pData == NULL) return NULL;

		tString sSoundName = pData->GetRandomSoundName(eSoundEntityType_Main,true);
		if(sSoundName=="") return NULL;
		
		if(pData->GetStream())
		{
			return PlayGuiStream(sSoundName,abLoop,afVolume * pData->GetVolume(),avPos,aEntryType,apNotEnoughChannels);
		}
		else
		{
			return PlayGui(sSoundName,abLoop,pData->GetVolume() * pData->GetVolume(),avPos,aEntryType,apNotEnoughChannels);
		}
	}
	
	//-----------------------------------------------------------------------

	bool cSoundHandler::Stop(const tString& asName)
	{
		cSoundEntry *pEntry = GetEntry(asName);
		if(pEntry) 
		{
			pEntry->Stop();
		}
		else
		{
			return false;
		}

		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cSoundHandler::StopAllExcept(const tString& asName)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------

	void cSoundHandler::StopAll(tFlag mTypes)
	{
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

            if(pEntry->GetType() & mTypes)
			{
				pEntry->Stop();
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSoundHandler::PauseAll(tFlag mTypes)
	{
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry->GetType() & mTypes)
			{
				pEntry->SetPaused(true);
			}
		}
	}
	//-----------------------------------------------------------------------

	void cSoundHandler::ResumeAll(tFlag mTypes)
	{
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry->GetType() & mTypes)
			{
				pEntry->SetPaused(false);
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSoundHandler::FadeOutAll(tFlag mTypes,float afFadeSpeed, bool abDisableStop)
	{
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry->GetType() & mTypes)
			{
				pEntry->FadeOut(afFadeSpeed);
				if(abDisableStop) pEntry->SetStopDisabled(true);
			}
		}
	}
	
	//-----------------------------------------------------------------------

	bool cSoundHandler::IsPlaying(const tString& asName)
	{
		cSoundEntry *pEntry = GetEntry(asName);
		
		if(pEntry) return pEntry->GetChannel()->IsPlaying();
		
		return false;
	}
	//-----------------------------------------------------------------------
	
	bool cSoundHandler::IsValid(cSoundEntry *apEntry, int alID)
	{
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry == apEntry)
			{
				if(pEntry->GetId() == alID)	return true;
				else						return false;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------
	
	int cSoundHandler::SetGlobalVolume(float afVolume, tFlag aAffectedTypes, int alId)
	{
		if(alId < 0) alId = mGlobalVolumeHandler.CreateEntry();
		
		cMultipleSettingsHandler::cGSEntry* pEntry = mGlobalVolumeHandler.GetEntry(alId, true);
				
		pEntry->SetTypes(aAffectedTypes);
		pEntry->SetValAndDest(afVolume);

        return alId;
	}
	
	//-----------------------------------------------------------------------
	
	int cSoundHandler::SetGlobalSpeed(float afSpeed, tFlag aAffectedTypes, int alId)
	{
		if(alId < 0) alId = mGlobalSpeedHandler.CreateEntry();

		cMultipleSettingsHandler::cGSEntry* pEntry = mGlobalSpeedHandler.GetEntry(alId, true);

		pEntry->SetTypes(aAffectedTypes);
		pEntry->SetValAndDest(afSpeed);

		return alId;
	}

	//-----------------------------------------------------------------------

	int cSoundHandler::FadeGlobalVolume(float afDestVolume, float afSpeed,tFlag aAffectedTypes, int alId, bool abDestroyIdAtDest)
	{	
		if(alId < 0) alId = mGlobalVolumeHandler.CreateEntry();

		cMultipleSettingsHandler::cGSEntry* pEntry = mGlobalVolumeHandler.GetEntry(alId, true);

		pEntry->SetTypes(aAffectedTypes);
		pEntry->FadeTo(afDestVolume,afSpeed,abDestroyIdAtDest);

		return alId;
	}

	int cSoundHandler::FadeGlobalSpeed(float afDestSpeed, float afSpeed,tFlag aAffectedTypes, int alId, bool abDestroyIdAtDest)
	{
		if(alId < 0) alId = mGlobalSpeedHandler.CreateEntry();

		cMultipleSettingsHandler::cGSEntry* pEntry = mGlobalSpeedHandler.GetEntry(alId, true);

		pEntry->SetTypes(aAffectedTypes);
		pEntry->FadeTo(afDestSpeed,afSpeed,abDestroyIdAtDest);

		return alId;
	}

	//-----------------------------------------------------------------------

	float cSoundHandler::GetGlobalVolume(eSoundEntryType aType)
	{
		if(aType == eSoundEntryType_World)	return mfGlobalVolume[0];
		else								return mfGlobalVolume[1];
	}

	float cSoundHandler::GetGlobalSpeed(eSoundEntryType aType)
	{
		if(aType == eSoundEntryType_World)	return mfGlobalSpeed[0];
		else								return mfGlobalSpeed[1];
	}

	//-----------------------------------------------------------------------

	void cSoundHandler::SetWorld(cWorld *apWorld)
	{
		mpWorld = apWorld;
	}

	//-----------------------------------------------------------------------
	
	tSoundEntryList* cSoundHandler::GetEntryList()
	{
		return &m_lstSoundEntries;
	}

	//-----------------------------------------------------------------------

	bool cSoundHandler::CheckSoundIsBlocked(const cVector3f& avSoundPosition)
	{
		if(mpWorld==NULL || mpWorld->GetPhysicsWorld()==NULL) return false;

		iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();

		mSoundRayCallback.Reset();

		pPhysicsWorld->CastRay(	&mSoundRayCallback,avSoundPosition,
								mpLowLevelSound->GetListenerPosition(),
								false,false,false,true);
		
		return mSoundRayCallback.HasCollided();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundEntry* cSoundHandler::GetEntry(const tString& asName)
	{
		tString sLowName = cString::ToLowerCase(asName);
		
		tSoundEntryListIt it = m_lstSoundEntries.begin();
		for(; it != m_lstSoundEntries.end(); ++it)
		{
			cSoundEntry *pEntry = *it;

			if(pEntry->GetName() == sLowName)
			{
				return pEntry;
			}
		}		

		return NULL;
	}
	
	//-----------------------------------------------------------------------
	
	iSoundChannel* cSoundHandler::CreateChannel(const tString& asName, int alPriority, bool abStream, bool *apNotEnoughChannels)
	{
		if(apNotEnoughChannels) *apNotEnoughChannels = false;

		////////////////////////
		//Load the data
		iSoundData* pData = mpResources->GetSoundManager()->CreateSoundData(asName,abStream);
		if(pData == NULL)
		{
			Error("Could not load sound '%s'\n", asName.c_str());
			return NULL;
		}
		
		/////////////////////////
		//Create sound channel
		iSoundChannel* pSound = pData->CreateChannel(alPriority);
		if(pSound == NULL)
		{
			//Need to destroy channel else it will never be deleted!
			if(abStream) mpResources->GetSoundManager()->Destroy(pData);

			if(apNotEnoughChannels) *apNotEnoughChannels = true;
		}
		
		return pSound;
	}
	
	//-----------------------------------------------------------------------

}
