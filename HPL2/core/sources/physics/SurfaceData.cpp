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

#include "physics/SurfaceData.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "scene/ParticleSystem.h"

#include "scene/World.h"
#include "scene/SoundEntity.h"
#include "sound/Sound.h"
#include "sound/SoundChannel.h"
#include "sound/SoundHandler.h"

#include "resources/Resources.h"
#include "resources/SoundEntityManager.h"
#include "resources/ParticleManager.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSurfaceData::cSurfaceData(const tString &asName, cPhysics *apPhysics,cResources *apResources)
	{
		msName = asName;
		mpPhysics = apPhysics;
		mpResources = apResources;

		//Setup default properties
		mFrictionMode = ePhysicsMaterialCombMode_Average;
		mElasticityMode  = ePhysicsMaterialCombMode_Average;

		mfElasticity = 0.5f;
		mfStaticFriction = 0.3f;
		mfKineticFriction = 0.3f;

		mlPriority = 0;
		
		mfMinScrapeSpeed = 0.6f;
		mfMinScrapeFreq = 0.7f;
		mfMinScrapeFreqSpeed = 1; 
		mfMaxScrapeFreq = 2;
		mfMaxScrapeFreqSpeed = 3;
		mfMiddleScrapeSpeed = 2;
		msScrapeSoundName = "";

		mpHapticSurface = NULL;
	}

	//-----------------------------------------------------------------------

	cSurfaceData::~cSurfaceData()
	{
		STLDeleteAll(mvImpactData);
		STLDeleteAll(mvHitData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cSurfaceData::OnImpact(float afSpeed,const cVector3f &avPos,int alContacts, iPhysicsBody *apBody)
	{
		if(mpPhysics->CanPlayImpact()==false)
		{
			return;
		}

		cWorld *pWorld = apBody->GetWorld()->GetWorld();
		if(pWorld==NULL){
			return;
		}

		cSurfaceImpactData *pData = NULL;
		for(size_t i=0; i< mvImpactData.size(); i++)
		{
			if(mvImpactData[i]->GetMinSpeed() <= afSpeed)
			{
				pData = mvImpactData[i];
				break;
			}
		}

		if(pData == NULL){
			return;
		}

		if(pData->GetSoundName()!="")
		{
			mpPhysics->AddImpact();
			
			cSoundEntity *pEntity = pWorld->CreateSoundEntity("Impact",pData->GetSoundName(),true);
			if(pEntity)
			{
				pEntity->SetIsSaved(false);
				//TODO: Offset the sound a bit so that it is not played inside a static object.
				pEntity->SetPosition(avPos);
			}
		}
	}
	
	//-----------------------------------------------------------------------

	void cSurfaceData::OnSlide(float afSpeed,const cVector3f &avPos,int alContacts, iPhysicsBody *apBody,
								iPhysicsBody *apSlideAgainstBody)
	{
		if(alContacts < mlMinScrapeContacts) return;
		
		//Make sure that only one body can update the scrape.
		if(	apBody->GetScrapeBody() && apSlideAgainstBody != apBody->GetScrapeBody()) 
		{
			return;
		}

		cWorld *pWorld = apBody->GetWorld()->GetWorld();
		if(pWorld==NULL) return;

		cSoundHandler *pSoundHandler = pWorld->GetSound()->GetSoundHandler();
		if(pSoundHandler->GetSilent()) return;

		/////////////////////////////////////
		//Check if sound exist in world.
		if(apBody->GetScrapeSoundEntity() && pWorld->SoundEntityExists(apBody->GetScrapeSoundEntity(), apBody->GetScrapeSoundEntityID())==false)
		{
			//Log("rEmove scrape (%d)! Does not exist!\n", apBody->GetScrapeSoundEntity());
			apBody->SetScrapeSoundEntity(NULL);
		}

		///////////////////////////////////////////////
		//Check if body is still, in that case stop set speed to 0
		if(apBody->GetMass() != 0)
		{
			if(apBody->GetPreveScrapeMatrix() == apBody->GetLocalMatrix())
			{
				afSpeed =0;
			}
			apBody->SetPreveScrapeMatrix(apBody->GetLocalMatrix());
		}


		///////////////////////////////////////////////
		//Update current scrape sound
		if(apBody->GetScrapeSoundEntity()!=NULL)
		{
			//check if the sound should be stopped
			float fMin = cMath::Max(mfMinScrapeSpeed-0.7f, 0.02f);
			if(std::abs(afSpeed) < fMin)
			{
				//Log("STOP scrape (%d)! Scrape speed too low!\n", apBody->GetScrapeSoundEntity());

				apBody->GetScrapeSoundEntity()->FadeOut(4.3f);
				apBody->SetScrapeSoundEntity(NULL);
				apBody->SetScrapeBody(NULL);
				//Log("Stopped scrape '%s' %d on body '%s' IN SURFACEDATA!\n",msScrapeSoundName.c_str(),
				//	(size_t)apBody->GetScrapeSoundEntity(),
				//	apBody->GetName().c_str());
			}
			else
			{
				apBody->SetHasSlide(true);

				//Change frequency according to speed.
                float fAbsSpeed = std::abs(afSpeed);
				float fFreq = 1;

				//Higher than middle
				if(fAbsSpeed >= mfMiddleScrapeSpeed)
				{
					if(fAbsSpeed >= mfMaxScrapeFreqSpeed)
					{
						fFreq = mfMaxScrapeFreq;
					}
					else
					{
						//Calculate how close the speed is to max.
						float fT = (fAbsSpeed-mfMiddleScrapeSpeed) / 
									(mfMaxScrapeFreqSpeed-mfMiddleScrapeSpeed);

						fFreq = (1 - fT) + fT * mfMaxScrapeFreq;
					}
				}
				//Below middle
				else
				{
					if(fAbsSpeed <= mfMinScrapeFreqSpeed)
					{
						fFreq = mfMinScrapeFreq;
					}
					else
					{
						//Calculate how close the speed is to max.
						float fT = (mfMiddleScrapeSpeed - fAbsSpeed) / 
							(mfMiddleScrapeSpeed - mfMinScrapeFreqSpeed);

						fFreq = (1 - fT) + fT * mfMinScrapeFreq;
					}
				}

				//Log("Speed: %f Freq: %f\n",fAbsSpeed,fFreq);
				cSoundEntry *pEntry = apBody->GetScrapeSoundEntity()->GetSoundEntry(eSoundEntityType_Main, true);
				if(pEntry)
				{
					pEntry->SetSpeedMul(fFreq);
					apBody->GetScrapeSoundEntity()->SetPosition(avPos);
				}
			}
		}
		///////////////////////////////////////////////
		//Create scrape sound
		else
		{
			if(mfMinScrapeSpeed <= std::abs(afSpeed) && msScrapeSoundName!="")
			{
				apBody->SetHasSlide(true);

				cSoundEntity *pEntity = pWorld->CreateSoundEntity("Scrape", msScrapeSoundName,true);
				if(pEntity)
				{
					pEntity->FadeIn(3.3f);
					pEntity->SetPosition(avPos);

					pEntity->SetIsSaved(false);

					apBody->SetScrapeSoundEntity(pEntity);
					apBody->SetScrapeBody(apSlideAgainstBody);

					//Log("Starting scrape '%s'(%d)(%d) on body '%s'(%d)\n",msScrapeSoundName.c_str(),
					//											(size_t)pEntity, pEntity->GetSoundEntry(eSoundEntityType_Main)->GetChannel(),
					//											apBody->GetName().c_str(), apBody);
				}
			}
		}

		///////////////////////////////////////////////
		//If body is static and got scrape sound, add to update list
		if(apBody->GetMass()==0 && apBody->HasSlide() && apBody->IsInUpdateList()==false)
		{
			apBody->GetWorld()->AddBodyToUpdateList(apBody);
		}
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::CreateImpactEffect(	float afSpeed,const cVector3f &avPos,int alContacts, 
											cSurfaceData *apSecondSurface, iPhysicsWorld *apPhysicsWorld)
	{
		if(afSpeed == 0) return;

		cSurfaceImpactData *pDataA = NULL;
		cSurfaceImpactData *pDataB = NULL;

		cWorld *pWorld = apPhysicsWorld->GetWorld();
		if(pWorld==NULL){
			return;
		}

		cSoundHandler *pSoundHandler = pWorld->GetSound()->GetSoundHandler();
		if(pSoundHandler->GetSilent()) return;
		
		/////////////////////////////
        //Get first surface
		for(size_t i=0; i< mvImpactData.size(); i++)
		{
			if(mvImpactData[i]->GetMinSpeed() <= afSpeed)
			{
				pDataA = mvImpactData[i];
				break;
			}
		}

		/////////////////////////////
		//Get second surface
		if(apSecondSurface != this && apSecondSurface != NULL)
		{
			for(size_t i=0; i< apSecondSurface->mvImpactData.size(); i++)
			{
				if(apSecondSurface->mvImpactData[i]->GetMinSpeed() <= afSpeed)
				{
					pDataB = apSecondSurface->mvImpactData[i];
					break;
				}
			}
		}
		
		tString sPS = "";

		if(pDataA && !pDataB)
		{
			sPS = pDataA->GetPSName();
		}
		else if(!pDataA && pDataB)
		{
			sPS = pDataB->GetPSName();
		}
		else if(pDataA && pDataB)
		{
			if(pDataA->GetPSPrio() >= pDataB->GetPSPrio())
				sPS = pDataA->GetPSName();
			else
				sPS = pDataB->GetPSName();
		}

		if(sPS != "")
		{
            cMatrixf mtxPos = cMath::MatrixTranslate(avPos);
			cParticleSystem *pPS = pWorld->CreateParticleSystem("ImpactPS",sPS,1);
			if(pPS) pPS->SetMatrix(mtxPos);


			//Log("Mat1: '%s' Mat2: '%s' Speed %f particle system '%s' pos: %s\n",
			//	GetName().c_str(),
			//	apSecondSurface->GetName().c_str(),
			//	afSpeed,sPS.c_str(),
			//	mtxPos.GetTranslation().ToString().c_str());
		}
	}


	//-----------------------------------------------------------------------

	void cSurfaceData::UpdateRollEffect(iPhysicsBody *apBody)
	{
		if(apBody->GetUseSurfaceEffects()==false) return;
		if(msRollSoundName == "" || mRollAxisFlags ==0) return;

		/////////////////////////////////
		//Get the max angular speed
		cVector3f vAngularSpeed = cMath::MatrixMul(apBody->GetLocalMatrix().GetRotation(),
												apBody->GetAngularVelocity());
		float fRollingSpeed = 0;
		//X
		if(mRollAxisFlags & eRollAxisFlag_X) 
			fRollingSpeed = std::abs(vAngularSpeed.x);
		//Y
		if(mRollAxisFlags & eRollAxisFlag_Y) 
			if(fRollingSpeed < std::abs(vAngularSpeed.y)) fRollingSpeed = std::abs(vAngularSpeed.y);
		//Z
		if(mRollAxisFlags & eRollAxisFlag_Z) 
			if(fRollingSpeed < std::abs(vAngularSpeed.z)) fRollingSpeed = std::abs(vAngularSpeed.z);

		//Log("Rollspeed: %f\n",fRollingSpeed);
		
		if(fRollingSpeed==0 && apBody->GetRollSoundEntity()==NULL) return;
	
		/////////////////////////////////
		//Update roll sound
		cWorld *pWorld = apBody->GetWorld()->GetWorld();
		if(pWorld==NULL) return;

		cSoundHandler *pSoundHandler = pWorld->GetSound()->GetSoundHandler();
		if(pSoundHandler->GetSilent()) return;
		
		//Check if sound exist in world.
		if(pWorld->SoundEntityExists(apBody->GetRollSoundEntity(), apBody->GetRollSoundEntityID())==false)
		{
			apBody->SetRollSoundEntity(NULL);
		}

		//If the body all ready has a Roll sound
		if(apBody->GetRollSoundEntity()!=NULL)
		{
			//check if the sound should be stopped
			float fMin = cMath::Max(mfMinRollSpeed-0.7f, 0.02f);
			if(	fRollingSpeed < fMin ||	apBody->HasCollision()==false)
			{
				apBody->GetRollSoundEntity()->FadeOut(4.3f);
				apBody->SetRollSoundEntity(NULL);
				//Log("Stopped Roll '%s' on body '%s'\n",msRollSoundName.c_str(), 
				//														apBody->GetName().c_str());
			}
			else
			{
				//Change frequency according to speed.
				float fAbsSpeed = fRollingSpeed;
				float fFreq = 1;
				float fVolume =1;
				
				//Higher than middle
				if(fAbsSpeed >= mfMiddleRollSpeed)
				{
					if(fAbsSpeed >= mfMaxRollFreqSpeed)
					{
						fFreq = mfMaxRollFreq;
						fVolume = mfMaxRollVolume;
					}
					else
					{
						//Calculate how close the speed is to max.
						float fT = (fAbsSpeed-mfMiddleRollSpeed) / 
							(mfMaxRollFreqSpeed-mfMiddleRollSpeed);

						fFreq = (1 - fT) + fT * mfMaxRollFreq;
						fVolume = (1 - fT) + fT * mfMaxRollVolume;
					}
				}
				//Below middle
				else
				{
					if(fAbsSpeed <= mfMinRollFreqSpeed)
					{
						fFreq = mfMinRollFreq;
						fVolume = mfMinRollVolume;
					}
					else
					{
						//Calculate how close the speed is to max.
						float fT = (mfMiddleRollSpeed - fAbsSpeed) / 
							(mfMiddleRollSpeed - mfMinRollFreqSpeed);

						fFreq = (1 - fT) + fT * mfMinRollFreq;
						fVolume = (1 - fT) + fT * mfMinRollVolume;
					}
				}

				//Log("Speed: %f Freq: %f\n",fAbsSpeed,fFreq);
				cSoundEntity *pSound = apBody->GetRollSoundEntity();
				cSoundEntry *pEntry = pSound->GetSoundEntry(eSoundEntityType_Main, true);
				if(pEntry)
				{
					pEntry->SetSpeedMul(fFreq);
					pEntry->FadeVolumeMulTo(cMath::Min(fVolume * pSound->GetVolume(),1.0f), 4.0f);
					
					apBody->GetRollSoundEntity()->SetPosition(apBody->GetWorldPosition());

					//Log("Updated Roll on body '%s' w speed %f to f: %f v: %f\n",apBody->GetName().c_str(),
					//						fRollingSpeed,fFreq,fVolume);
				}
				else
				{

				}
			}
		}
		else
		{
			if(mfMinRollSpeed <= fRollingSpeed && apBody->HasCollision())
			{
				cSoundEntity *pEntity = pWorld->CreateSoundEntity("Roll",msRollSoundName,true);
				if(pEntity)
				{
					pEntity->FadeIn(3.3f);
					pEntity->SetPosition(apBody->GetWorldPosition());
					pEntity->SetIsSaved(false);

					apBody->SetRollSoundEntity(pEntity);
					
					//Log("Starting Roll '%s' on body '%s'\n",msRollSoundName.c_str(), 
					//											apBody->GetName().c_str());
				}
			}
		}
		
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetElasticity(float afElasticity)
	{
		mfElasticity = afElasticity;
	}

	float cSurfaceData::GetElasticity() const
	{
		return mfElasticity;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetStaticFriction(float afElasticity)
	{
		mfStaticFriction = afElasticity;
	}

	float cSurfaceData::GetStaticFriction() const
	{
		return mfStaticFriction;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetKineticFriction(float afElasticity)
	{
		mfKineticFriction = afElasticity;
	}

	float cSurfaceData::GetKineticFriction() const
	{
		return mfKineticFriction;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetPriority(int alPriority)
	{
		mlPriority = alPriority;
	}

	int cSurfaceData::GetPriority() const
	{
		return mlPriority;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetFrictionCombMode(ePhysicsMaterialCombMode aMode)
	{
		mFrictionMode = aMode;
	}

	ePhysicsMaterialCombMode cSurfaceData::GetFrictionCombMode() const
	{
		return mFrictionMode;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::SetElasticityCombMode(ePhysicsMaterialCombMode aMode)
	{
		mElasticityMode = aMode;
	}

	//-----------------------------------------------------------------------

	ePhysicsMaterialCombMode cSurfaceData::GetElasticityCombMode() const
	{
		return mElasticityMode;
	}

	//-----------------------------------------------------------------------

	void cSurfaceData::PreloadData()
	{
		if(msRollSoundName!="")mpResources->GetSoundEntityManager()->Preload(msRollSoundName);
		if(msScrapeSoundName!="")mpResources->GetSoundEntityManager()->Preload(msScrapeSoundName);
		
		for(size_t i=0; i< mvImpactData.size(); ++i)
		{
			if(mvImpactData[i]->msSoundName!="") 
			{
				mpResources->GetSoundEntityManager()->Preload(mvImpactData[i]->msSoundName);
			}
			if(mvImpactData[i]->msPSName!="")
			{
				mpResources->GetParticleManager()->Preload(mvImpactData[i]->msPSName);
			}
		}
		for(size_t i=0; i< mvHitData.size(); ++i)
		{
			if(mvHitData[i]->msSoundName!="") 
				mpResources->GetSoundEntityManager()->Preload(mvHitData[i]->msSoundName);
			if(mvHitData[i]->msPSName!="")
				mpResources->GetParticleManager()->Preload(mvHitData[i]->msPSName);
		}
	}
	
	//-----------------------------------------------------------------------

	iPhysicsMaterial* cSurfaceData::ToMaterial(iPhysicsWorld *apWorld)
	{
		iPhysicsMaterial *pMat=NULL;

		pMat = apWorld->GetMaterialFromName(msName);
		
		if(pMat==NULL)
		{
			pMat = apWorld->CreateMaterial(msName);
		}
		
		pMat->SetElasticity(mfElasticity);
		pMat->SetKineticFriction(mfKineticFriction);
		pMat->SetStaticFriction(mfStaticFriction);

		pMat->SetElasticityCombMode(mElasticityMode);
		pMat->SetFrictionCombMode(mFrictionMode);

		pMat->SetSurfaceData(this);

		return pMat;
	}

	//-----------------------------------------------------------------------

	cSurfaceImpactData* cSurfaceData::CreateImpactData(float afMinSpeed)
	{
		cSurfaceImpactData *pData = hplNew( cSurfaceImpactData, () );
		pData->mfMinSpeed = afMinSpeed;

		mvImpactData.push_back(pData);

		return pData;
	}

	cSurfaceImpactData* cSurfaceData::GetImpactData(int alIdx)
	{
		return mvImpactData[alIdx];
	}

	int cSurfaceData::GetImpactDataNum()
	{
		return (int)mvImpactData.size();
	}

	cSurfaceImpactData* cSurfaceData::GetImpactDataFromSpeed(float afSpeed)
	{	
		for(size_t i=0; i< mvImpactData.size(); ++i)
		{
			if(afSpeed >= mvImpactData[i]->GetMinSpeed())
			{
				return mvImpactData[i];
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	cSurfaceImpactData* cSurfaceData::CreateHitData(float afMinSpeed)
	{
		cSurfaceImpactData *pData = hplNew( cSurfaceImpactData, () );
		pData->mfMinSpeed = afMinSpeed;

		mvHitData.push_back(pData);

		return pData;
	}

	cSurfaceImpactData* cSurfaceData::GetHitData(int alIdx)
	{
		return mvHitData[alIdx];
	}

	int cSurfaceData::GetHitDataNum()
	{
		return (int)mvHitData.size();
	}

	cSurfaceImpactData* cSurfaceData::GetHitDataFromSpeed(float afSpeed)
	{	
		for(size_t i=0; i< mvHitData.size(); ++i)
		{
			if(afSpeed >= mvHitData[i]->GetMinSpeed())
			{
				return mvHitData[i];
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}
