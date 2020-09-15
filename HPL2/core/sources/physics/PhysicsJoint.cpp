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

#include "physics/PhysicsJoint.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsController.h"
#include "system/LowLevelSystem.h"

#include "sound/Sound.h"
#include "scene/SoundEntity.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"

#include "scene/World.h"
#include "scene/Scene.h"

#include "engine/Engine.h"

#include "math/Math.h"

#include "engine/ScriptFuncs.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPhysicsJoint::iPhysicsJoint(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
		iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f &avPinDir) 
		: msName(asName), mpParentBody(apParentBody), mpChildBody(apChildBody), mpWorld(apWorld)
	{
		mlUniqueID = -1;

		mMaxLimit.msSound = "";
		mMinLimit.msSound = "";

		if(apParentBody) {
			apParentBody->AddJoint(this);
			m_mtxParentBodySetup = apParentBody->GetLocalMatrix();
		}
		else{
			m_mtxParentBodySetup = cMatrixf::Identity;
		}

		m_mtxPrevChild = cMatrixf::Identity;
		m_mtxPrevParent = cMatrixf::Identity;
		
		apChildBody->AddJoint(this);
		m_mtxChildBodySetup = apChildBody->GetLocalMatrix();

		mvPivotPoint = avPivotPoint;
		mvStartPivotPoint = avPivotPoint;
		
		mvPinDir = avPinDir;
		mvStartPinDir = avPinDir;
		
		if(mpParentBody)
		{
			cMatrixf m_mtxInvParent = cMath::MatrixInverse(mpParentBody->GetLocalMatrix());
			mvLocalPivot = cMath::MatrixMul(m_mtxInvParent,avPivotPoint);
			mvLocalPinDir = cMath::MatrixMul(m_mtxInvParent.GetRotation(),avPinDir);
		}
		else
		{
			mvLocalPivot = avPivotPoint;
			mvLocalPinDir = avPinDir;
		}

		msMoveSound = "";

		mbHasCollided = false;
		mpSound = NULL;
		mlSoundID = -1;
		
		mpCallback = NULL;

		mbAutoDeleteCallback = false;

		mpUserData = NULL;

		mbBreakable = false;
		mfBreakForce = 0;
		msBreakSound = "";
		mbBroken = false;

		mbStickyMinLimit = false;
		mbStickyMaxLimit = false;

		mlLimitStepCount =0;

		mlSpeedCount =0;

		mbLimitAutoSleep = false;
		mfLimitAutoSleepDist = 0.02f;
		mlLimitAutoSleepNumSteps = 10;

		//Log("Created joint '%s'\n",msName.c_str());
	}
	
	iPhysicsJoint::~iPhysicsJoint()
	{
		if(mbAutoDeleteCallback && mpCallback) hplDelete(mpCallback);

		//Destroy all controllers.
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for(; it != m_mapControllers.end(); ++it)
		{
			mpWorld->DestroyController(it->second);
		}

		if(mpChildBody) mpChildBody->RemoveJoint(this);
		if(mpParentBody) mpParentBody->RemoveJoint(this);

		if(mpSound) mpWorld->GetWorld()->DestroySoundEntity(mpSound);

		//Log("Deleted joint '%s'\n",msName.c_str());
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	/**
	 * This should only be used by PhysicsBody.
	 */
	void iPhysicsJoint::RemoveBody(iPhysicsBody *apBody)
	{
		if(mpParentBody == apBody) mpParentBody = NULL;
		if(mpChildBody == apBody) mpChildBody = NULL;
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsJoint::AddController(iPhysicsController *apController)
	{
		
		//Add controller top map
		m_mapControllers.insert(tPhysicsControllerMap::value_type(apController->GetName(), apController));

		//Set properties
		apController->SetBody(mpChildBody);
		apController->SetJoint(this);
	}

	//-----------------------------------------------------------------------

	iPhysicsController* iPhysicsJoint::GetController(const tString &asName)
	{
		tPhysicsControllerMapIt it = m_mapControllers.find(asName);
		if(it == m_mapControllers.end()) return NULL;

		return it->second;
	}

	//-----------------------------------------------------------------------

	bool iPhysicsJoint::ChangeController(const tString &asName)
	{
		iPhysicsController *pNewCtrl = GetController(asName);
		if(pNewCtrl==NULL) return false;
		
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for(; it != m_mapControllers.end(); ++it)
		{
			iPhysicsController *pCtrl = it->second;

			if(pCtrl == pNewCtrl)
			{
				pCtrl->SetActive(true);	
			}
			else
			{
				pCtrl->SetActive(false);
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	void iPhysicsJoint::SetAllControllersPaused(bool abX)
	{
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for(; it != m_mapControllers.end(); ++it)
		{
			iPhysicsController *pCtrl = it->second;
			
			pCtrl->SetPaused(abX);
		}
	}

	//-----------------------------------------------------------------------

	cPhysicsControllerIterator iPhysicsJoint::GetControllerIterator()
	{
		return cPhysicsControllerIterator(&m_mapControllers);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iPhysicsJoint::OnMaxLimit()
	{
		if(mbHasCollided==false && mpCallback)
		{
			mpCallback->OnMaxLimit(this);
		}
		
		//////////////////////////////////////////////////
		// Check if any of the controllers has a OnMax end.
		if(mbHasCollided==false)
		{
			//Log("OnMax!\n");
			tPhysicsControllerMapIt it = m_mapControllers.begin();
			for(; it != m_mapControllers.end(); ++it)
			{
				iPhysicsController *pCtrl = it->second;

				//Log("Ctrl %s: %d\n",pCtrl->GetName().c_str(),(int)pCtrl->GetEndType());
				
				if(pCtrl->IsActive() && pCtrl->GetEndType() == ePhysicsControllerEnd_OnMax)
				{
					pCtrl->SetActive(false);
					iPhysicsController *pNextCtrl = GetController(pCtrl->GetNextController());
					if(pNextCtrl) pNextCtrl->SetActive(true);
					else Warning("Controller '%s' does not exist in joint '%s'\n",pCtrl->GetNextController().c_str(),msName.c_str());
				}
			}
		}

		LimitEffect(&mMaxLimit);
	}

	//-----------------------------------------------------------------------

	void iPhysicsJoint::OnMinLimit()
	{
		if(mbHasCollided==false && mpCallback)
		{
			mpCallback->OnMinLimit(this);

			
		}
		//////////////////////////////////////////////////
		// Check if any of the controllers has a OnMin end.
		if(mbHasCollided==false)
		{
			//Log("OnMin!\n");

			tPhysicsControllerMapIt it = m_mapControllers.begin();
			for(; it != m_mapControllers.end(); ++it)
			{
				iPhysicsController *pCtrl = it->second;

				if(pCtrl->IsActive() && pCtrl->GetEndType() == ePhysicsControllerEnd_OnMin)
				{
					pCtrl->SetActive(false);
					iPhysicsController *pNextCtrl = GetController(pCtrl->GetNextController());
					if(pNextCtrl) pNextCtrl->SetActive(true);
					else Warning("Controller '%s' does not exist in joint '%s'\n",pCtrl->GetNextController().c_str(),msName.c_str());
				}
			}
		}

		LimitEffect(&mMinLimit);
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsJoint::CalcSoundFreq(float afSpeed,float *apFreq, float *apVol)
	{
		float fAbsSpeed = std::abs(afSpeed);
		float fFreq = 1;
		float fVolume = 1;

		//Higher than middle
		if(fAbsSpeed >= mfMiddleMoveSpeed)
		{
			if(fAbsSpeed >= mfMaxMoveFreqSpeed)
			{
				fFreq = mfMaxMoveFreq;
				fVolume = mfMaxMoveVolume;
			}
			else
			{
				//Calculate how close the speed is to max.
				float fT = (fAbsSpeed-mfMiddleMoveSpeed) / 
					(mfMaxMoveFreqSpeed-mfMiddleMoveSpeed);

				fFreq = (1 - fT) + fT * mfMaxMoveFreq;
				fVolume = mfMiddleMoveVolume*(1 - fT) + fT * mfMaxMoveVolume;
			}
		}
		//Below middle
		else
		{
			if(fAbsSpeed <= mfMinMoveFreqSpeed)
			{
				fFreq = mfMinMoveFreq;
				fVolume = mfMinMoveVolume;
			}
			else
			{
				//Calculate how close the speed is to max.
				float fT = (mfMiddleMoveSpeed - fAbsSpeed) / 
					(mfMiddleMoveSpeed - mfMinMoveFreqSpeed);

				fFreq = (1 - fT) + fT * mfMinMoveFreq;
				fVolume = mfMiddleMoveVolume*(1 - fT) + fT * mfMinMoveVolume;
			}
		}

		*apFreq = fFreq;
		*apVol = fVolume;
	}

	//-----------------------------------------------------------------------
	
	bool iPhysicsJoint::OnPhysicsUpdate()
	{
		bool bFrozen = true;
        if(mpParentBody && mpParentBody->GetEnabled()) 
			bFrozen = false;
		else if(mpChildBody->GetEnabled()) 
			bFrozen = false;
		
		if(bFrozen && mpSound==NULL) return false;

		//Get the pivot point, if there is no parent, it is stuck.
		if(mpParentBody)
		{
			mvPivotPoint = cMath::MatrixMul(mpParentBody->GetLocalMatrix(),mvLocalPivot);
			mvPinDir = cMath::MatrixMul3x3(mpParentBody->GetLocalMatrix(),mvLocalPinDir);
		}
				
		cWorld *pWorld = mpWorld->GetWorld();
		if(pWorld == NULL) return true;
		if(msMoveSound == "") return true;

		if(mpWorld->GetWorld()->GetSound()->GetSoundHandler()->GetSilent()) return true;

		//////////////////////////////////////
		//Get the speed
		cVector3f vVel(0,0,0);
		//Linear
		if(mMoveSpeedType== ePhysicsJointSpeed_Linear)
		{
			if(mpParentBody)
			{
				vVel = mpChildBody->GetLinearVelocity() - mpParentBody->GetLinearVelocity();
			}
			else
			{
				vVel = mpChildBody->GetLinearVelocity();
			}
		}
		//Angular
		else
		{
			if(mpParentBody)
			{
				vVel = mpChildBody->GetAngularVelocity() - mpParentBody->GetAngularVelocity();
			}
			else
			{
				vVel = mpChildBody->GetAngularVelocity();	
			}
		}

		//Check so the body is not still
		if(mpParentBody)
		{
			if(	m_mtxPrevChild == mpChildBody->GetLocalMatrix() &&
				m_mtxPrevParent == mpParentBody->GetLocalMatrix())
			{
				vVel =0;
			}
			m_mtxPrevChild = mpChildBody->GetLocalMatrix();
			m_mtxPrevParent = mpParentBody->GetLocalMatrix();
		}
		else
		{
			if(m_mtxPrevChild == mpChildBody->GetLocalMatrix())
			{
				vVel =0;
			}
			m_mtxPrevChild = mpChildBody->GetLocalMatrix();
		}

		float fSpeed = vVel.Length();

		if(pWorld->SoundEntityExists(mpSound, mlSoundID)==false)
		{
			mpSound = NULL;
		}
		
		//////////////////////////////////////
		// Create and update sound if speed is high enough
		// Joint has sound
		if(mpSound)
		{
			//Log("Updating %s\n",mpSound->GetName().c_str());
			float fMin = cMath::Max(mfMinMoveSpeed-0.2f, 0.1f);
			if(fSpeed <= fMin)
			{
				mpSound->FadeOut(4.3f);
				mpSound	= NULL;
			}
			else 
			{
				//Log("Getting entry!\n");
				cSoundEntry *pEntry = mpSound->GetSoundEntry(eSoundEntityType_Main, true);
				if(pEntry)
				{
					//Log("Update entry!\n");
					float fFreq, fVolume;
					CalcSoundFreq(fSpeed, &fFreq,&fVolume);
					
					pEntry->SetSpeedMul(fFreq);
					pEntry->SetVolumeMul(fVolume);

					//Log("Speed: %f Vol: %f Freq: %f\n",fSpeed,fVolume,fFreq);
				}
				else
				{
					//Log("Null entry!\n");
				}


				mpSound->SetPosition(mvPivotPoint);
			}
		}
		//////////////////////
		// Joint has no sound
		else 
		{
			/////////////////////////////
			// Speed is over limit
			if(fSpeed > mfMinMoveSpeed)
			{
				if(mlSpeedCount >= 3)
				{
					mlSpeedCount =0;
					mpSound = pWorld->CreateSoundEntity("MoveSound",msMoveSound, true);
					if(mpSound)	{
						mlSoundID = mpSound->GetCreationID();
						mpSound->SetIsSaved(false);
						mpSound->FadeIn(3.3f);
						mpSound->SetPosition(mvPivotPoint);
					}
					//Log("Starting!\n");
				}
				else
				{
					mlSpeedCount++;
				}
			}
			/////////////////////////////
			// Speed is under limit
			else
			{
				mlSpeedCount =0;
			}
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	void iPhysicsJoint::SetSound(cSoundEntity *apSound)
	{
		mpSound = apSound;
        if(apSound)
			mlSoundID = apSound->GetCreationID();
		else
			mlSoundID = -1;
	}
	
	//-----------------------------------------------------------------------
	
	void iPhysicsJoint::LimitEffect(cJointLimitEffect *pEffect)
	{
		cWorld *pWorld = mpWorld->GetWorld();

		if(pWorld && pEffect->msSound!="")
		{
			cVector3f vVel(0,0,0);
			if(mpParentBody)
				vVel = mpChildBody->GetLinearVelocity() - mpParentBody->GetLinearVelocity();
			else
				vVel = mpChildBody->GetLinearVelocity();

			float fSpeed = vVel.Length();
			if(fSpeed > pEffect->mfMaxSpeed) fSpeed = pEffect->mfMaxSpeed;

			//Log("Speed: %f\n",fSpeed);

			if(fSpeed >= pEffect->mfMinSpeed && mbHasCollided==false && pEffect->msSound != "")
			{
				float fVolume = (fSpeed - pEffect->mfMinSpeed) / (pEffect->mfMaxSpeed - pEffect->mfMinSpeed);
                
				cSoundEntity *pSound = pWorld->CreateSoundEntity("LimitSound", pEffect->msSound,true);
				if(pSound)
				{
					pSound->SetIsSaved(false);
					pSound->SetVolume(fVolume);
					pSound->SetPosition(mpChildBody->GetLocalPosition());
				}
			}
		}

		mbHasCollided = true;
	}

	//-----------------------------------------------------------------------

	void iPhysicsJoint::OnNoLimit()
	{
		mbHasCollided = false;
	}

	//-----------------------------------------------------------------------

	void iPhysicsJoint::Break()
	{
		mbBroken = true;
		mbBreakable = true;
	}

	//-----------------------------------------------------------------------

	bool iPhysicsJoint::CheckBreakage()
	{
		if(mbBreakable==false) return false;

		float fForcesSize = GetForceSize();
		
		if(fForcesSize >= mfBreakForce || mbBroken)
		{
			if(msBreakSound != "")
			{
				cWorld *pWorld = mpWorld->GetWorld();
				cSoundEntity *pSound = pWorld->CreateSoundEntity("BreakSound", msBreakSound,true);
				if(pSound)
				{
					pSound->SetIsSaved(false);
					pSound->SetPosition(mvPivotPoint);
				}
			}
			return true;
		}

		return false;
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsJoint::CheckLimitAutoSleep(iPhysicsJoint *apJoint, 
											const float afMin, const float afMax,
											const float afDist)
	{
		if(apJoint->mbLimitAutoSleep)
		{
			float fMinDiff = std::abs(afMin - afDist);
			float fMaxDiff = std::abs(afMax - afDist);

			if(	fMaxDiff < apJoint->mfLimitAutoSleepDist || 
				fMinDiff < apJoint->mfLimitAutoSleepDist)
			{
				if(apJoint->mlLimitStepCount >= apJoint->mlLimitAutoSleepNumSteps)
					apJoint->mpChildBody->DisableAfterSimulation();
				else
					apJoint->mlLimitStepCount++;
			}
			else
			{
				apJoint->mlLimitStepCount =0;
			}
		}
	}

	//-----------------------------------------------------------------------
}
