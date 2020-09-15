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

#include "scene/SoundEntity.h"

#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"
#include "sound/SoundEntityData.h"
#include "sound/LowLevelSound.h"
#include "sound/Sound.h"
#include "sound/SoundData.h"
#include "resources/SoundEntityManager.h"
#include "math/Math.h"

#include "engine/Engine.h"
#include "scene/Scene.h"
#include "scene/World.h"

namespace hpl {

	tSoundEntityGlobalCallbackList cSoundEntity::mlstGobalCallbacks;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cSoundEntity::cSoundEntity(const tString& asName,cSoundEntityData *apData,
								cSoundEntityManager *apSoundEntityManager,
								cWorld *apWorld,
								cSoundHandler *apSoundHandler, bool abRemoveWhenOver, int alCreationID)
		:iEntity3D(asName)
	{
		mbRemoveWhenOver = abRemoveWhenOver;
		mlCreationID = alCreationID;

		//Log("Created sound entity '%s'\n",msName.c_str());

		mpSoundHandler = apSoundHandler;
		mpSoundEntityManager = apSoundEntityManager;
		mpWorld = apWorld;

		mpData = apData;
		mpData->PreloadSounds();

		for(int i=0; i<3; i++)
		{
			mvSoundEntries[i] = NULL;
			mvSoundEntryID[i]=-1;
		}

		mfIntervalCount = mpData->GetInterval();

		mfVolume = mpData->GetVolume();

		mfMinDistance = mpData->GetMinDistance();
		mfMaxDistance = mpData->GetMaxDistance();

		mBoundingVolume.SetSize(mfMaxDistance*2);

		mbStopped = false; //If the sound should be stopped
		mbStarted = false; //If the sound started playing-

		mbSkipStartEnd = false; //If the end and start sounds should be skipped.
		
        mbPrioRemove = false; //If the sounds was removed because too many where playing

		mbFadingOut = false; //If the sound is fading out.

		mbOutOfRange = false; //If the sound is out of range.

		mbLog = false;
		
		mfSleepCount = 0;

		mbForcePlayAsGUISound = false;

		mpSoundCallback = hplNew( cSoundEntityChannelCallback, () );
		mpSoundCallback->mpEntity = this;

		if(mpSoundHandler->GetSilent())
		{
			mbStarted = true;
			mbStopped = true;
			mfVolume =0;
			mbSkipStartEnd = true;
			mbRemoveWhenOver = true;
		}

		//if( mpData->GetName() == "scrape_wood.snt") mbLog = true;
		if(mbLog)Log("Created %d\n", this);
	}
	
	cSoundEntity::~cSoundEntity()
	{
		if(mbLog)Log("Deleting %d\n", this);
		for(int i=0; i<3; i++)
		{
			if(mpSoundHandler->IsValid(mvSoundEntries[i], mvSoundEntryID[i]))
			{
				mvSoundEntries[i]->SetCallBack(NULL); //Reset callback, just in case!
				mvSoundEntries[i]->Stop();
			}
		}

		hplDelete(mpSoundCallback);

		//Do _not_ delete sound entity data files now. 
		//Just dec user count and the manager can be cleared of unused files when you want to.
		mpData->DecUserCount();
	}

	///////////////////////////////////////
	//DEBUG when using pooling test
	/*void cSoundEntity::Setup(const tString& asName,cSoundEntityData *apData, bool abRemoveWhenOver)
	{
		/////////////////////////////
		// Entity
		msName = asName;
		mbIsActive = true;

		mpParentNode = NULL;

		m_mtxLocalTransform = cMatrixf::Identity;
		m_mtxWorldTransform = cMatrixf::Identity;

		mbTransformUpdated = true;

		mlCount = 0;

		msSourceFile = "";

		mbApplyTransformToBV = true;
		mbUpdateBoundingVolume = true;

		mpParent = NULL;

		mlIteratorCount =-1;

		mbIsSaved = true;
		mlUniqueID = -1;

		/////////////////////////////
		// Sound ENtityh 
		mbRemoveWhenOver = abRemoveWhenOver;

		mpData = apData;
		mpData->PreloadSounds();

		for(int i=0; i<3; i++)
		{
			mvSoundEntries[i] = NULL;
			mvSoundEntryID[i]=-1;
		}

		mfIntervalCount = mpData->GetInterval();

		mfVolume = mpData->GetVolume();

		mfMinDistance = mpData->GetMinDistance();
		mfMaxDistance = mpData->GetMaxDistance();

		mBoundingVolume.SetSize(mfMaxDistance*2);

		mbStopped = false; //If the sound should be stopped
		mbStarted = false; //If the sound started playing-

		mbSkipStartEnd = false; //If the end and start sounds should be skipped.

		mbPrioRemove = false; //If the sounds was removed because too many where playing

		mbFadingOut = false; //If the sound is fading out.

		mbOutOfRange = false; //If the sound is out of range.

		mbLog = false;

		mfSleepCount = 0;

		mbForcePlayAsGUISound = false;

		mpSoundCallback->mpEntity = this;

		if(mpSoundHandler->GetSilent())
		{
			mbStarted = true;
			mbStopped = true;
			mfVolume =0;
			mbSkipStartEnd = true;
			mbRemoveWhenOver = true;
		}

		//if( mpData->GetName() == "scrape_wood.snt") mbLog = true;
		if(mbLog)Log("Created %d\n", this);
	}*/
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CALLABCK
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cSoundEntityChannelCallback::OnPriorityRelease()
	{
		if(mpEntity->mbStopped || mpEntity->mbFadingOut) return;

		mpEntity->mbPrioRemove = true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundEntry* cSoundEntity::GetSoundEntry(eSoundEntityType aType, bool abCheckEntryValidity)
	{
		if(abCheckEntryValidity && mvSoundEntries[aType])
		{
			if(mpSoundHandler->IsValid(mvSoundEntries[aType], mvSoundEntryID[aType])==false)
			{
				return NULL;
			}
		}

		return mvSoundEntries[aType];
	}

	//-----------------------------------------------------------------------
	void cSoundEntity::Play(bool abPlayStart)
	{
		if(mpSoundHandler->GetSilent())	return;

		if(mbLog)Log("Playing %d\n", this);
		mbStopped =false;
		mbOutOfRange = false;
		mbFadingOut = false;
		
		//Play start sound if settings allow
		if(abPlayStart && mbSkipStartEnd==false)
		{
			if(mpData->GetLoop() && mpData->HasSound(eSoundEntityType_Start))
			{
				PlaySound(eSoundEntityType_Start,false);
				mbStarted = false;
			}
		}
		
		//If start is not playing, play main sound directly.
		if(	mvSoundEntries[eSoundEntityType_Main]==NULL && 
			mvSoundEntries[eSoundEntityType_Start]==NULL && 
			(mpData->GetLoop()== false || mpData->GetInterval()==0))
		{
			PlaySound(eSoundEntityType_Main,mpData->GetLoop());
			mbStarted = true;
		}
	
	}

	//-----------------------------------------------------------------------

	void cSoundEntity::Stop(bool abPlayEnd)
	{
		if(mbStopped) return;

		mbStopped = true;
		mbOutOfRange = false;
		
		if(mvSoundEntries[eSoundEntityType_Main]==NULL) return;

		if(mbLog)Log("Stopping %d\n", this);

		if(mpData->GetLoop())
		{
			if(abPlayEnd  && mbSkipStartEnd==false)
			{
				PlaySound(eSoundEntityType_Stop, false);
			}
			
			if(mpSoundHandler->IsValid(mvSoundEntries[eSoundEntityType_Main], mvSoundEntryID[eSoundEntityType_Main]))
			{
				mvSoundEntries[eSoundEntityType_Main]->Stop();
			}
		}
		else
		{
			if(mpSoundHandler->IsValid(mvSoundEntries[eSoundEntityType_Main], mvSoundEntryID[eSoundEntityType_Main]))
			{
				mvSoundEntries[eSoundEntityType_Main]->Stop();
			}
		}

		mvSoundEntries[eSoundEntityType_Main] = NULL;
	}
	
	//-----------------------------------------------------------------------

	void cSoundEntity::FadeIn(float afSpeed)
	{
		if(mpSoundHandler->GetSilent())	return;

        if(mbLog)Log("Fade in %d\n", this);

		Play(false);
		cSoundEntry* pEntry = GetSoundEntry(eSoundEntityType_Main, false);
		if(pEntry)
		{
			pEntry->FadeIn(1, afSpeed);
		}
	}

	//-----------------------------------------------------------------------

	
	void cSoundEntity::FadeOut(float afSpeed)
	{
		mbFadingOut = true;
	
		if(mpSoundHandler->GetSilent())	return;

		if(mbLog)Log("Fading out %d\n", this);

		if(mvSoundEntries[eSoundEntityType_Main]==NULL)
		{
			mbStopped = true;
			return;
		}

		cSoundEntry* pEntry = GetSoundEntry(eSoundEntityType_Main, true);
		if(pEntry)
		{
			pEntry->FadeOut(afSpeed);
		}
		else
		{
			mvSoundEntries[eSoundEntityType_Main] = NULL;
			mbStopped = true;
		}
	}

	//-----------------------------------------------------------------------


	bool cSoundEntity::IsStopped()
	{
		if(mbStopped && mvSoundEntries[eSoundEntityType_Stop]==NULL)
		{
			return true;
		}

		return false;
	}

	bool cSoundEntity::IsFadingOut()
	{
		return mbFadingOut;
	}

	//-----------------------------------------------------------------------

	
	bool cSoundEntity::GetRemoveWhenOver()
	{
		return mbRemoveWhenOver;
	}
	
	//-----------------------------------------------------------------------

	void cSoundEntity::UpdateLogic(float afTimeStep)
	{
		if(mpSoundHandler->GetSilent())
		{
			return;
		}

		if(mfSleepCount >0){
			mfSleepCount -= afTimeStep;
			return;
		}

		//////////////////////////////////////////////
		//If out of range check if it is inside range.
		if(mbOutOfRange && mbStopped==false)
		{
			if(CheckIsOutOfRange()==false)
			{
				if(mbLog)Log("%d Inside range stop!\n", this);
				Play(false);
				mbOutOfRange = false;
			}
			else
			{
				return;
			}
		}

		
		/////////////////////////////////////////////////
		//Go through all sounds and check if they are playing, 
		//if so update their positions else remove.
		for(int i=0; i<3; i++)
		{
			if(mvSoundEntries[i])
			{
				if(mpSoundHandler->IsValid(mvSoundEntries[i], mvSoundEntryID[i])==false)
				{
					mvSoundEntries[i] = NULL;
					
					//if the sound has stopped due to priority try starting again
					if(mbPrioRemove && i == (int)eSoundEntityType_Main)
					{
						if(mbStopped==false)
						{
							mbStarted = false;
							mfSleepCount = 0.3f;
						}
					}
					//If the sound has already started, stop it.
					else if((mpData->GetInterval()==0 || mpData->GetLoop()==false) && 
							mbStarted)
					{
						mbStopped = true;
					}
				}
				else
				{
					if(mbPrioRemove)
					{
						//Might wanna do some ouput here since it is not good..
					}
					
					if(mvSoundEntries[i] && !mvSoundEntries[i]->GetChannel()->GetPositionIsRelative())
					{
						mvSoundEntries[i]->GetChannel()->SetPosition(GetWorldPosition());
					}
				}
			}
		}

		//////////////////////////////////////////////
		//Check if sound is playing, and if so update it.
		if(mbStopped==false)
		{
			/////////////////////////////////////////////////////////////////////////
			//Sound is not playing, start it and since it has not been invoked by Play 
			//no need to play start sound. Only do this if interval is 0 or non looping else it might
			//be a deliberate pause.
			if(	mvSoundEntries[eSoundEntityType_Main]==NULL && 
				mvSoundEntries[eSoundEntityType_Start]==NULL && 
				(mpData->GetLoop()== false || mpData->GetInterval()==0))
			{

				if(PlaySound(eSoundEntityType_Main, mpData->GetLoop()))
				{
					mbStarted = true;
					mbPrioRemove = false;

					//Call the callbacks that the sound has started.
					tSoundEntityGlobalCallbackListIt it = mlstGobalCallbacks.begin();
					for(; it != mlstGobalCallbacks.end(); ++it)
					{
						iSoundEntityGlobalCallback *pCallback = *it;
						pCallback->OnStart(this);
					}
				}
				//If sound is not started because of priority. Sleep before trying to start it again.
				else if(mbPrioRemove)
				{
					mfSleepCount = 0.3f;
				}
				
			}

			///////////////////////////////////////////////////////////
			//Check if looping and interval is not 0.
			//then there needs to be some updating.
			if(	mpData->GetLoop() && 
				mpData->GetInterval()>0 && 
				mvSoundEntries[eSoundEntityType_Start]==NULL && 
				mvSoundEntries[eSoundEntityType_Main]==NULL)
			{
				mfIntervalCount += afTimeStep;
				
				//if the interval time has elapsed the sound might be played again.
				if(mfIntervalCount >= mpData->GetInterval())
				{
					//Check random and if rand is right play the sound.
					if(cMath::RandRectf(0,1) <= mpData->GetRandom() || 
						mpData->GetRandom()==0)
					{
						PlaySound(eSoundEntityType_Main,false);
					}
					mfIntervalCount =0;
				}
			}

			//////////////////////////////////////////////
			// If the sound is looped and out of range, stop it
			if(	mvSoundEntries[eSoundEntityType_Start]==NULL && mpData->GetLoop() && mpData->GetUse3D())
			{
				if(CheckIsOutOfRange())
				{
					//////////////////////
					//If fading out, then just stop the sound directly
					if(mbFadingOut)
					{
						Stop(false);
					}
					//////////////////////
					//If not fading, stop the sound and then set out of range as true. This will check if in range every update.
					else
					{
						if(mvSoundEntries[eSoundEntityType_Main])
						{
							if(mpSoundHandler->IsValid(mvSoundEntries[eSoundEntityType_Main], mvSoundEntryID[eSoundEntityType_Main]))
							{
								if(mbLog)Log("%d Out of range stop!\n", this);
								mvSoundEntries[eSoundEntityType_Main]->Stop();
							}
							
							mvSoundEntries[eSoundEntityType_Main] = NULL;
						}

						mbOutOfRange = true;
					}
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSoundEntity::AddGlobalCallback(iSoundEntityGlobalCallback *apCallback)
	{
		mlstGobalCallbacks.push_back(apCallback);
	}

	//-----------------------------------------------------------------------

	void cSoundEntity::RemoveGlobalCallback(iSoundEntityGlobalCallback *apCallback)
	{
		tSoundEntityGlobalCallbackListIt it = mlstGobalCallbacks.begin();
		for(; it != mlstGobalCallbacks.end(); ++it)
		{
			if(apCallback == *it){
				mlstGobalCallbacks.erase(it);
				return;
			}
		}
	}
	
	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	bool cSoundEntity::CheckIsOutOfRange()
	{
		if(mpWorld->IsSoundEmitter()==false) return true;

		float fRange = GetListenerSqrLength();
		float fMaxRange = mfMaxDistance * mfMaxDistance;
		
		return fRange >= fMaxRange;
	}

	//-----------------------------------------------------------------------

	bool cSoundEntity::PlaySound(eSoundEntityType aType,bool abLoop)
	{
		tString sSoundName = mpData->GetRandomSoundName(aType,true); //TODO: Add a var instead of null!
		if(sSoundName == "") return false;

		bool bNotEnoughChannels = false;

		if(mbForcePlayAsGUISound)
		{
			mvSoundEntries[aType] = mpSoundHandler->Play3D(
										sSoundName, abLoop,
										mfVolume, cVector3f(0,0,1),
										100.0f, 200.0f,	//Just set something so that sound is always inside min.
										eSoundEntryType_Gui,true, mpData->GetPriority(),
										mpData->GetStream(), &bNotEnoughChannels);
		}
		else
		{
			if(mpData->GetUse3D())
			{
				mvSoundEntries[aType] = mpSoundHandler->Play3D(
										sSoundName, abLoop,
										mfVolume, GetWorldPosition(),
										mfMinDistance, mfMaxDistance,
										eSoundEntryType_World,false, mpData->GetPriority(),
										mpData->GetStream(), &bNotEnoughChannels);
			}
			else
			{
				mvSoundEntries[aType] = mpSoundHandler->Play3D(
										sSoundName, abLoop,
										mfVolume, cVector3f(0,0,1),
										100.0f, 200.0f,	//Just set something so that sound is always inside min.
										eSoundEntryType_World,true, mpData->GetPriority(),
										mpData->GetStream(), &bNotEnoughChannels);
			}
		}

		/////////////////////////////////
		// Sound entry Not created!
		if(mvSoundEntries[aType]==NULL)
		{
			// Sound entry could not be loaded
			if(bNotEnoughChannels==false)
				Error("Couldn't play sound '%s' for sound entity %s\n",sSoundName.c_str(),msName.c_str());
			
			//If could not be loaded and main or not enough channels and non loop: Stop the entity!
			if(	(aType == eSoundEntityType_Main && bNotEnoughChannels==false) ||
				(bNotEnoughChannels && abLoop==false))
			{
				mbStopped = true;

				//Keep the sound, as a sound that is not removed when over should always be available!
				//if(bNotEnoughChannels==false)
				//	mbRemoveWhenOver = true;
			}
		
			return false;
		}
		/////////////////////////////////
		// Sound entry Successfully created
		else
		{
			//if(mbLog) Log("	-Setting up sound\n");

			mvSoundEntryID[aType] = mvSoundEntries[aType]->GetId();

			iSoundChannel *pChannel = mvSoundEntries[aType]->GetChannel();
			pChannel->SetBlockable(mpData->GetBlockable());
			pChannel->SetBlockVolumeMul(mpData->GetBlockVolumeMul());
			pChannel->SetPriorityModifier(mpData->GetPriority());

			if(aType==eSoundEntityType_Main)
			{
				mvSoundEntries[aType]->SetCallBack(mpSoundCallback);
			}
		}

		return true;

		//for(int i=0; i<3; ++i) mvSoundEntries[i] = NULL;
	}

	//-----------------------------------------------------------------------

	float cSoundEntity::GetListenerSqrLength()
	{
		cVector3f vListenerPos = mpWorld->GetSound()->GetLowLevel()->GetListenerPosition();
		return cMath::Vector3DistSqr(vListenerPos,GetWorldPosition());
	}

	//-----------------------------------------------------------------------

}
