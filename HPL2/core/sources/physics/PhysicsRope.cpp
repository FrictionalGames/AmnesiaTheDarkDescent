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

#include "physics/PhysicsRope.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"

#include "scene/World.h"
#include "scene/SoundEntity.h"

#include "sound/Sound.h"
#include "sound/SoundHandler.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPhysicsRope::iPhysicsRope(const tString &asName,iPhysicsWorld *apWorld, const cVector3f &avStartPos , const cVector3f &avEndPos) 
		: iVerletParticleContainer(asName, apWorld)
	{
		//Add start and end particles.
		mlstParticles.push_front(	hplNew( cVerletParticle, (this,avStartPos, 0) ) );
		mlstParticles.push_back(	hplNew( cVerletParticle, (this,avEndPos, 1) ) );

		for(int i=0; i< 2;++i)
		{
			mvAttachedBody[i].mpBody = NULL;
			mForcePid[i].SetErrorNum(10);
		}

		mfTotalLength = cMath::Vector3Dist(avStartPos, avEndPos);
		mfSegmentLength = 0; //0 = no segments!

		mbMotorActive = false;
		mfMotorWantedLength = mfTotalLength;
		mfMotorSpeedMul = 1.0f;
		mfMotorMinSpeed = -1.0f;
		mfMotorMaxSpeed = 1.0f;

		msMotorSound = "";
		mfMotorSoundStartSpeed = 0.1f;
		mfMotorSoundStopSpeed = 0.05f;
		mpMotorSoundEntity = NULL;
		mlMotorSoundEntityID = -1;

		mbAutoMoveActive = false;
		mfAutoMoveAcc = 1.0f;
		mfAutoMoveMaxSpeed = 1.0f;
		mfAutoMoveSpeed =0;

		mfMinTotalLength = mfTotalLength;
		mfMaxTotalLength = mfTotalLength;

		mbCollideAttachments = false;
		mbCollide = false;

		mfStrength = 100;
		mfStiffness = 1;

		mfFirstSegmentLength = mfTotalLength;

		mlMaxIterations = 3;
		
		mbHasUpdated = false;
	}


	//-----------------------------------------------------------------------

	iPhysicsRope::~iPhysicsRope()
	{
		for(size_t i=0;i<2;++i)
		{
			if(mvAttachedBody[i].mpBody)
			{
				mvAttachedBody[i].mpBody->RemoveAttachedVerletContainer(this);
			}
		}

		STLDeleteAll(mlstParticles);
		hplDelete(mpRayParticleCallback);
	}

	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iPhysicsRope::UpdateBeforeSimulate(float afTimeStep)
	{
		mbHasUpdated = true;
		
		PreUpdate(afTimeStep);
		if(mbSleeping)
		{
			return;
		}

		UpdateMotorAndAutoMove(afTimeStep);
		UpdateMovement(afTimeStep);

		UpdateAttachedParticlePositions(afTimeStep);

		for(int i=0; i<mlMaxIterations; ++i)
		{
			UpdateConstraints(afTimeStep);
		}

		UpdateAttachedBodies(afTimeStep);
	}
	
	//-----------------------------------------------------------------------
	
	void iPhysicsRope::UpdateAfterSimulate(float afTimeStep)
	{
		if(mbSleeping) return;

		CalculateSmoothPositions(afTimeStep);
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsRope::SetMotorActive(bool abX)
	{
		if(mbMotorActive == abX) return;

		mbMotorActive = abX;

		if(mbMotorActive)
		{
			mfAutoMoveSpeed = 0;
			SetSleeping(false);
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::SetAutoMoveActive(bool abX)
	{
		mbAutoMoveActive = abX;

        if(mbAutoMoveActive)
		{
			SetSleeping(false);
		}
	}

	bool iPhysicsRope::GetAutoMoveActive()
	{
		return mbAutoMoveActive;
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::RemoveAttachedBody(iPhysicsBody *apBody, bool abRemoveContainerFromBody)
	{
		for(int i=0; i<2; ++i)
		{
			if(mvAttachedBody[i].mpBody == apBody)
			{
				mvAttachedBody[i].mpBody = NULL;
			}
		}
		if(abRemoveContainerFromBody)
			apBody->RemoveAttachedVerletContainer(this);
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsRope::SetAttachedStartBody(iPhysicsBody *apBody)
	{
		SetAttachedBody(0, GetStartParticle(), apBody);

		//TODO: Need to change inverse mass of start particle, if body mass > 0?
	}
	
	void iPhysicsRope::SetAttachedEndBody(iPhysicsBody *apBody)
	{
		SetAttachedBody(1, GetEndParticle(), apBody);
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsRope::SetTotalLength(float afX)
	{
		if(mfTotalLength == afX) return;

		mfTotalLength = afX;

		BuildRopeParticles();
		SetSleeping(false);
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::SetMinTotalLength(float afX)
	{
		mfMinTotalLength = afX;
	}

	void iPhysicsRope::SetMaxTotalLength(float afX)
	{
		mfMaxTotalLength = afX;
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::SetSegmentLength(float afX)
	{
		if(afX<0 || mfSegmentLength == afX) return;

        mfSegmentLength = afX;

		BuildRopeParticles();
		SetSleeping(false);
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsRope::RenderDebug(iLowLevelGraphics *apLowLevel)
	{
		cVector3f vPrevPos =0;
		bool bFirst=true;
		int lCount =0;
		for(tVerletParticleListIt it = mlstParticles.begin(); it != mlstParticles.end(); ++it, ++lCount)
		{
			cVerletParticle *pPart = *it;

			apLowLevel->DrawSphere(pPart->GetPosition(), mfParticleRadius,cColor(1,0,0,1));

			if(bFirst==false)
			{
				apLowLevel->DrawLine(vPrevPos,pPart->GetPosition(),cColor(1,1));
			}

			bFirst=false;
			vPrevPos = pPart->GetPosition();
		}
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	bool iPhysicsRope::CheckParticleBodyCollision(iPhysicsBody *apBody)
	{
		if(mbCollideAttachments) return true;

        bool bAttachment=false;
		for(int i=0; i<2; ++i)
		{
			if(mvAttachedBody[i].mpBody == apBody) return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool iPhysicsRope::CheckSpecificDataSleeping()
	{
		if(mbMotorActive) return false;

		for(int i=0; i<2; ++i)
		{
			iPhysicsBody *pBody = mvAttachedBody[i].mpBody;
			if(pBody)
			{
				if(pBody->GetAngularVelocity().SqrLength() > 0.001f*0.001f) return false;
				if(pBody->GetLinearVelocity().SqrLength() > 0.001f*0.001f) return false;
			}
		}

		return true;
	}

	bool iPhysicsRope::CheckSpecificDataAwake()
	{	
		if(mbMotorActive) return true;

		for(int i=0; i<2; ++i)
		{
			iPhysicsBody *pBody = mvAttachedBody[i].mpBody;
			if(pBody)
			{
				if(pBody->GetEnabled()) return true;
			}
		}
		return false;
	}

	void iPhysicsRope::SetSpecificDataSleeping(bool abSleeping)
	{
		for(int i=0; i<2; ++i)
		{
			iPhysicsBody *pBody = mvAttachedBody[i].mpBody;
			if(pBody)
			{
				if(abSleeping==false) pBody->Enable();
			}
		}
	}
	
	//-----------------------------------------------------------------------
	
	void iPhysicsRope::UpdateMovement(float afTimeStep)
	{
		for(tVerletParticleListIt it = mlstParticles.begin(); it != mlstParticles.end(); ++it)
		{
			cVerletParticle *pPart = *it;

			pPart->UpdateMovement(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::UpdateMotorAndAutoMove(float afTimeStep)
	{
		float fVel =0;
		
		//////////////////
		// Update Motor
		if(mbMotorActive)
		{
			float fTotalLength =  GetTotalLength();
			float fDir = mfMotorWantedLength - GetTotalLength();
			fVel = fDir * mfMotorSpeedMul;

			if(fVel > mfMotorMaxSpeed)	fVel = mfMotorMaxSpeed;
			if(fVel < mfMotorMinSpeed)	fVel = mfMotorMinSpeed;

			fTotalLength += fVel * afTimeStep;

			SetTotalLength(fTotalLength);
			
			if(cMath::Abs(mfMotorWantedLength - fTotalLength) < 0.02f)
			{
				mbMotorActive = false;
			}
		}
		//////////////////
		// Update Auto move
		else if(mbAutoMoveActive)
		{
			float fTotalLength =  GetTotalLength();
			mfAutoMoveSpeed += mfAutoMoveAcc*afTimeStep;
            
			if(mfAutoMoveSpeed > mfAutoMoveMaxSpeed)	mfAutoMoveSpeed = mfAutoMoveMaxSpeed;
			if(mfAutoMoveSpeed < -mfAutoMoveMaxSpeed)	mfAutoMoveSpeed = -mfAutoMoveMaxSpeed;

			fTotalLength += mfAutoMoveSpeed * afTimeStep;
			
			bool bStop = false;
			if(fTotalLength > mfMaxTotalLength){
				fTotalLength = mfMaxTotalLength;
				bStop = true;
			}
			if(fTotalLength < mfMinTotalLength){
				fTotalLength = mfMinTotalLength;
				bStop = true;
			}

			if(bStop) mfAutoMoveSpeed =0;
			fVel = mfAutoMoveSpeed;

			//Log("Speed: %f Length: %f\n", mfAutoMoveSpeed, fTotalLength);

			SetTotalLength(fTotalLength);
		}
		
		//////////////////
		// Update sound
		if(mpMotorSoundEntity==NULL && fVel==0) return;

		cWorld *pNormalWorld = mpWorld->GetWorld();
		cSoundHandler *pSoundHandler = pNormalWorld->GetSound()->GetSoundHandler();
		if(pSoundHandler->GetSilent()) return;

		if(mpMotorSoundEntity && pNormalWorld->SoundEntityExists(mpMotorSoundEntity, mlMotorSoundEntityID)==false)
		{
			mpMotorSoundEntity = NULL;	
		}
		
		///////////////////////
		//Sound is playing. Check if speed is under the stop speed
		if(mpMotorSoundEntity && cMath::Abs(fVel) < mfMotorSoundStopSpeed)
		{
			mpMotorSoundEntity->FadeOut(10.0f);
			mpMotorSoundEntity = NULL;
		}
		///////////////////////
		//Sound is NOT playing. See if one should be started.
		else if(msMotorSound != "" && mpMotorSoundEntity==NULL && cMath::Abs(fVel) > mfMotorSoundStartSpeed)
		{
			mpMotorSoundEntity = pNormalWorld->CreateSoundEntity(msName+"_MotorSound",msMotorSound,true);
			if(mpMotorSoundEntity)
			{
				mlMotorSoundEntityID = mpMotorSoundEntity->GetCreationID();

				mpMotorSoundEntity->SetIsSaved(false);
				mpMotorSoundEntity->SetPosition(GetStartParticle()->GetPosition());
				mpMotorSoundEntity->FadeIn(5.0f);
			}
		}
	}

	
	//-----------------------------------------------------------------------

	void iPhysicsRope::UpdateAttachedParticlePositions(float afTimeStep)
	{
		for(int i=0; i<2; ++i)
		{
			if(mvAttachedBody[i].mpBody==NULL) continue;
            
			cVector3f vPos = cMath::MatrixMul(mvAttachedBody[i].mpBody->GetLocalMatrix(), mvAttachedBody[i].mvBodyLocalPos);

			mvAttachedBody[i].mpParticle->SetPosition(vPos, false);
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::UpdateAttachedBodies(float afTimeStep)
	{
		for(int i=0; i<2; ++i)
		{
			if(mvAttachedBody[i].mpBody==NULL) continue;

			//Log("%s Updating body %d\n",msName.c_str() ,i);

			iPhysicsBody *pBody = mvAttachedBody[i].mpBody;
			
			cVector3f vWantedPos = mvAttachedBody[i].mpParticle->GetPosition();
			cVector3f vCurrentPos = cMath::MatrixMul(pBody->GetLocalMatrix(), mvAttachedBody[i].mvBodyLocalPos);
			cVector3f vError = vWantedPos - vCurrentPos;

			mForcePid[i].p = mfStrength;
			mForcePid[i].i = 0;
			mForcePid[i].d = mfStrength * 0.1f * mfStiffness;
			
			cVector3f vForce = mForcePid[i].Output(vError, afTimeStep);

			cVector3f vLocalPos = vCurrentPos - pBody->GetLocalPosition();
			cVector3f vMassCentre = pBody->GetMassCentre();
			if(vMassCentre != cVector3f(0,0,0))
			{
				vMassCentre = cMath::MatrixMul(pBody->GetLocalMatrix().GetRotation(),vMassCentre);
				vLocalPos -= vMassCentre;
			}

			cVector3f vTorque = cMath::Vector3Cross(vLocalPos, vForce);

			//Log("  torque (%s)\n", cMath::MatrixMul(pBody->GetInertiaMatrix(), vTorque).ToString().c_str());
			pBody->AddTorque(cMath::MatrixMul(pBody->GetInertiaMatrix(), vTorque) );
			//Log("  force (%s)\n", (vForce * pBody->GetMass()).ToString().c_str());
			pBody->AddForce(vForce * pBody->GetMass());
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::UpdateConstraints(float afTimeStep)
	{
		cVerletParticle *pPrevPart = NULL;
		tVerletParticleListIt it = mlstParticles.begin();
		for(int lCount = 0; it != mlstParticles.end(); ++it, ++lCount)
		{
			cVerletParticle *pPart = *it;
			if(lCount >0)
			{
				float fLength = lCount==1 ? mfFirstSegmentLength : mfSegmentLength;

				//Log("Updating %d, pos1: (%s) pos2: (%s)\n", lCount, pPrevPart->mvPosition.ToString().c_str(), pPart->mvPosition.ToString().c_str());

				UpdateLengthConstraint(pPrevPart, pPart, fLength);
			}
						
			pPrevPart = pPart;
		}

		//Skip collision testing for now!
		return;
		it = mlstParticles.begin();
		for(int lCount = 0; it != mlstParticles.end(); ++it, ++lCount)
		{	
			cVerletParticle *pPart = *it;

			if(mbCollideAttachments==false && (pPart == GetStartParticle() || pPart == GetEndParticle()))
			{
				continue;
			}
			
			if(pPart->GetInvMass() != 0) UpdateParticleCollisionConstraint(pPart, pPart->GetPrevPosition(), mfParticleRadius);
		}
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsRope::CalculateSmoothPositions(float afTimeStep)
	{
		cVector3f vBasePos[2] = { GetStartParticle()->GetPosition(), GetEndParticle()->GetPosition()};
		cVector3f vAddPos[2] ={0, 0};
		for(int i=0; i<2; ++i)
		{
			if(mvAttachedBody[i].mpBody)
			{
				cVector3f vWantedPos = cMath::MatrixMul(mvAttachedBody[i].mpBody->GetLocalMatrix(), mvAttachedBody[i].mvBodyLocalPos);
				vAddPos[i] = vWantedPos - vBasePos[i];
			}
		}

		float fMaxCount = (float) mlstParticles.size() - 1;

		tVerletParticleListIt it = mlstParticles.begin();
		for(float fCount = 0; it != mlstParticles.end(); ++it, ++fCount)
		{
			cVerletParticle *pPart = *it;
			
			float fT = fCount / fMaxCount;

			cVector3f vAdd = vAddPos[0]*(1-fT) + vAddPos[1]*fT;

			pPart->SetSmoothPosition(pPart->GetPosition() + vAdd);
		}
	}

	//-----------------------------------------------------------------------
	
	void iPhysicsRope::BuildRopeParticles()
	{
		////////////////////////
		//If not updated, clear data
		if(mbHasUpdated==false)
		{
			int lRemoveCount = (int)mlstParticles.size() -2;
			tVerletParticleListIt it = mlstParticles.begin(); it++;
			for(; lRemoveCount>0; --lRemoveCount)
			{
				cVerletParticle* pPart = *it;
				it = mlstParticles.erase(it);
				hplDelete(pPart);
			}
		}

		////////////////////////
		//Set up
		int lParticleNum = (int)mlstParticles.size() - 2;

		////////////////////////
		//Calculate number of particles needed and update vars
		int lWantedNum = 0;
		if(mfSegmentLength > 0)
		{
			lWantedNum = (int)((mfTotalLength / mfSegmentLength)+0.999999f) - 1;

			mfFirstSegmentLength = cMath::Modulus(mfTotalLength, mfSegmentLength);
			if(mfFirstSegmentLength == 0) mfFirstSegmentLength = mfSegmentLength;
		}
		else
		{
			mfFirstSegmentLength = mfTotalLength;
		}
		
		//Log("Wanted num: %d | total len: %f | first set len: %f | seg len: %f \n", lWantedNum, mfTotalLength, mfFirstSegmentLength, mfSegmentLength);

		if(lWantedNum == lParticleNum) return;

		////////////////////////
		//Remove particles if needed
		if(lWantedNum < lParticleNum)
		{
			tVerletParticleListIt it = mlstParticles.begin();
			it++;
			int lRemoveCount= lParticleNum - lWantedNum;
			for(; lRemoveCount>0; --lRemoveCount)
			{
				cVerletParticle* pPart = *it;
				it = mlstParticles.erase(it);
				hplDelete(pPart);
			}
		}
		////////////////////////
		//Add particles if needed
		else if(lWantedNum > lParticleNum)
		{
			cVector3f vDir = GetStartDirection();
			int lAddCount= lWantedNum - lParticleNum;

			cVector3f vStartPos = GetStartParticle()->GetPosition();

			float fLength = mfFirstSegmentLength + mfSegmentLength*((float)lAddCount-1);

			tVerletParticleListIt it = mlstParticles.begin(); 
			it++;

			for(; lAddCount>0; --lAddCount)
			{
				cVector3f vPos = vStartPos + vDir*fLength;
				//Log("Adding %d pos: %s length: %f\n", lAddCount, vPos.ToString().c_str(), fLength);
				fLength -= mfSegmentLength;

				it = mlstParticles.insert(it, hplNew(cVerletParticle, (this,vPos, 1)) );
			}
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsRope::SetAttachedBody(int alIdx, cVerletParticle *apParticle, iPhysicsBody *apBody)
	{
		mForcePid[alIdx].Reset();
		
		if(mvAttachedBody[alIdx].mpBody)
		{
			mvAttachedBody[alIdx].mpBody->RemoveAttachedVerletContainer(this);
		}
		
		apBody->AddAttachedVerletContainer(this);

		mvAttachedBody[alIdx].mpBody = apBody;
		mvAttachedBody[alIdx].mpParticle = apParticle;

		cMatrixf mtxInvBody = cMath::MatrixInverse(apBody->GetLocalMatrix());
		mvAttachedBody[alIdx].mvBodyLocalPos = cMath::MatrixMul(mtxInvBody, apParticle->GetPosition());
	}
	
	//-----------------------------------------------------------------------

	cVector3f iPhysicsRope::GetStartDirection()
	{
		tVerletParticleListIt it = mlstParticles.begin();
		cVerletParticle *pP1 = *it;
		it++;
		cVerletParticle *pP2 = *it;

		return cMath::Vector3Normalize(pP2->GetPosition() - pP1->GetPosition());
	}

	//-----------------------------------------------------------------------
}
