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

#include "LuxEnemyMover.h"

#include "LuxEnemy.h"
#include "LuxMap.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemyMover::cLuxEnemyMover(iLuxEnemy *apEnemy, iCharacterBody *apCharBody)
{
	mpEnemy = apEnemy;
	mpCharBody = apCharBody;

	mbTurning = false;
	mfTurnGoalAngle =0;
	mfTurnSpeed =0;
	mfTurnBreakAcc =0;

	mfStuckLimit = 0.3f;
	mfMaxStuckCounter = 2.0f;

	mfStuckCounter =0;

	mMoveState = eLuxEnemyMoveState_LastEnum;
	mbOverideMoveState = false;
}

//-----------------------------------------------------------------------

cLuxEnemyMover::~cLuxEnemyMover()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemyMover::SetupAfterLoad(cWorld *apWorld)
{
}
//-----------------------------------------------------------------------

void cLuxEnemyMover::OnUpdate(float afTimeStep)
{
	UpdateStuckCounter(afTimeStep);
	UpdateTurning(afTimeStep);	
	UpdateMoveAnimation(afTimeStep);
	UpdateStepEffects(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxEnemyMover::MoveToPos(const cVector3f& avFeetPos)
{
	TurnToPos(avFeetPos);
	mpCharBody->Move(eCharDir_Forward,1.0f);
}

//-----------------------------------------------------------------------

void cLuxEnemyMover::TurnToPos(const cVector3f& avFeetPos)
{
	cVector3f vStartPos = mpCharBody->GetPosition(); 

	float fGoalAngle = -cMath::GetAngleFromPoints2D(cVector2f(vStartPos.x, vStartPos.z), 
													cVector2f(avFeetPos.x, avFeetPos.z));
	TurnToAngle(fGoalAngle);
}

void cLuxEnemyMover::TurnToAngle(float afAngle)
{
	mbTurning = true;

	mfTurnGoalAngle = afAngle;
}

//-----------------------------------------------------------------------

void cLuxEnemyMover::UseMoveStateAnimations()
{
	if(mbOverideMoveState==false) return;
	
	mbOverideMoveState = false;
	mMoveState = eLuxEnemyMoveState_LastEnum;
}

//-----------------------------------------------------------------------

float cLuxEnemyMover::CalculateSpeedMul(float afTimeStep)
{
	float fMul = 1.0f;

	//////////////////////
	//Break when making sharp turns
	if(mbTurning)
	{
		float fAngleDist = std::fabs(cMath::GetAngleDistanceRad(mpCharBody->GetYaw(), mfTurnGoalAngle));

		if(fAngleDist >= mpEnemy->mfTurnMinBreakAngle && mpCharBody->GetMoveSpeed(eCharDir_Forward)>0.15f)
		{
			fMul -= mpEnemy->mfTurnBreakMul * fAngleDist;
			if(fMul<0) fMul =0;
		}

		//Log("%p: %f, %f, %f\n", this, cMath::ToDeg(fAngleDist), fMul, mpCharBody->GetMoveSpeed(eCharDir_Forward));
	}

	return fMul;
}

//-----------------------------------------------------------------------

float cLuxEnemyMover::GetMoveSpeed()
{
	cVector3f vVel = mpCharBody->GetVelocity(gpBase->mpEngine->GetStepSize());
	vVel.y =0;
	return vVel.Length();
}
//-----------------------------------------------------------------------

float cLuxEnemyMover::GetWantedSpeedAmount()
{
	float fWantedSpeed = mpCharBody->GetMoveSpeed(eCharDir_Forward);
	float fRealSpeed = GetMoveSpeed();
	
	if(fabs(fWantedSpeed) < 0.001f) return 1;

	return fRealSpeed / fWantedSpeed;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemyMover::UpdateStuckCounter(float afTimeStep)
{
	///////////////////////
	// Get the wanted speed and the real speed
	float fWantedSpeed = mpCharBody->GetMoveSpeed(eCharDir_Forward);
	float fRealSpeed = cMath::Vector3Dist(mpCharBody->GetPosition(), mpCharBody->GetLastPosition());
	fRealSpeed = fRealSpeed / afTimeStep;

	///////////////////////
	// Get the wanted direction and real direction
	cVector3f vWantedDir = mpCharBody->GetForward();
	cVector3f vRealDir = mpCharBody->GetPosition() - mpCharBody->GetLastPosition();
	vRealDir.Normalize();

	float fCos = cMath::Vector3Dot(vWantedDir,vRealDir);

	///////////////////////
	// Calculate 
	if( fRealSpeed/fWantedSpeed < mfStuckLimit || (std::fabs(fCos) < 0.3f && fWantedSpeed > 0.001f) )
	{
		mfStuckCounter += afTimeStep ;
		if(mfStuckCounter > mfMaxStuckCounter) mfStuckCounter = mfMaxStuckCounter;
	}
	else
	{
		mfStuckCounter -= afTimeStep*0.8f;
		if(mfStuckCounter<0)mfStuckCounter =0;
	}
}

//-----------------------------------------------------------------------

void cLuxEnemyMover::UpdateTurning(float afTimeStep)
{
	if(mbTurning==false) return;

	//////////////////////////////
	//Get distance to goal angle
	float fAngleDist = cMath::GetAngleDistanceRad(mpCharBody->GetYaw(), mfTurnGoalAngle);

	/////////////////
	//Check if close enough to goal
	if(std::fabs(fAngleDist) < 0.001f)
	{
		mbTurning = false;
		mfTurnSpeed =0;
		mfTurnBreakAcc =0;
		return;
	}

	//////////////////////
	//Calculate the turn speed
	mfTurnSpeed = cMath::Min(mpEnemy->mfTurnSpeedMul * std::fabs(fAngleDist), mpEnemy->mfTurnMaxSpeed);

	if(fAngleDist < 0)	mpCharBody->AddYaw(-mfTurnSpeed * afTimeStep);
	else				mpCharBody->AddYaw(mfTurnSpeed * afTimeStep);
}
//-----------------------------------------------------------------------

void cLuxEnemyMover::UpdateMoveAnimation(float afTimeStep)
{
	if(mbOverideMoveState) return;

	float fSpeed = GetMoveSpeed();
	if(mpCharBody->GetMoveSpeed(eCharDir_Forward) <0) fSpeed = -fSpeed;

	//Log("Update move anim. Speed: %f Current: %d\n", fSpeed, mMoveState);
	
	eLuxEnemyPoseType pose = mpEnemy->mCurrentPose;
	
	////////////////////////////////
	// Move state animation
	eLuxEnemyMoveState prevMoveState = mMoveState;

	switch(mMoveState)
	{
	/////////////////
	// Backward
	case eLuxEnemyMoveState_Backward:
		if(fSpeed >= 0)
			mMoveState = eLuxEnemyMoveState_Stopped;

		break;

	/////////////////
	// Stopped State
	case eLuxEnemyMoveState_Stopped:
		if(fSpeed < -0.05f)
			mMoveState = eLuxEnemyMoveState_Backward;
		else if(fSpeed >= mpEnemy->mfStoppedToWalkSpeed[pose])
			mMoveState = eLuxEnemyMoveState_Walking;
		else if(std::fabs(mfTurnSpeed) > 0.07f && mpCharBody->GetMoveDelay()<=0)
			mMoveState = eLuxEnemyMoveState_Walking;

		break;

	/////////////////
	// Walking State
	case eLuxEnemyMoveState_Walking:
		if(fSpeed >= mpEnemy->mfWalkToRunSpeed[pose])
			mMoveState = eLuxEnemyMoveState_Running;
		else if(fSpeed <= mpEnemy->mfWalkToStoppedSpeed[pose])
		{
			if(std::fabs(mfTurnSpeed) < 0.03f) mMoveState = eLuxEnemyMoveState_Stopped;
		}

		break;

	/////////////////
	// Running State
	case eLuxEnemyMoveState_Running:
		if(fSpeed <= mpEnemy->mfRunToWalkSpeed[pose])
			mMoveState = eLuxEnemyMoveState_Walking;

		break;

	/////////////////
	// NULL
	case eLuxEnemyMoveState_LastEnum:
		mMoveState = eLuxEnemyMoveState_Stopped;
		break;
	}
	
	//////////////////////////////////////////////
	//If move state has changed, change animation
	if(prevMoveState != mMoveState)
	{
		//Backward
		if(mMoveState == eLuxEnemyMoveState_Backward)
		{
			//Log(" To Backward\n");
			mpEnemy->PlayAnim(mpEnemy->GetBackwardAnimationName(), true, 0.4f, true, mpEnemy->mfMoveSpeedAnimMul, false, false);
		}
		//Stopped
		else if(mMoveState == eLuxEnemyMoveState_Stopped)
		{
			//Log(" To Stop\n");
			mpEnemy->PlayAnim(mpEnemy->GetIdleAnimationName(),true,0.7f,false,1.0f,false,false);
		}
		//Walking
		else if(mMoveState == eLuxEnemyMoveState_Walking)
		{
			bool bSync = prevMoveState == eLuxEnemyMoveState_Running ? true : false;
			//Log(" To Walk. Synch: %d\n", bSync);

			mpEnemy->PlayAnim(mpEnemy->GetWalkAnimationName(),true, 0.5f, true, mpEnemy->mfMoveSpeedAnimMul, bSync, false);
		}
		//Running
		else if(mMoveState == eLuxEnemyMoveState_Running)
		{
			bool bSync = prevMoveState == eLuxEnemyMoveState_Walking ? true : false;
			//Log(" To Run. Synch: %d\n", bSync);
			
          mpEnemy->PlayAnim(mpEnemy->GetRunAnimationName(),true, 0.5f, true, mpEnemy->mfMoveSpeedAnimMul, bSync, false);
		}
	}

	/////////////////////////////////
	//Update animation speed
	if(mpEnemy->mpCurrentAnimation && mMoveState != eLuxEnemyMoveState_Stopped)
	{
		if(std::fabs(fSpeed) > 0.05f)
		{
			//Log(" Anim speed: %f\n", std::fabs(fSpeed) * mpEnemy->mfMoveSpeedAnimMul);
			mpEnemy->mpCurrentAnimation->SetSpeed(std::fabs(fSpeed) * mpEnemy->mfMoveSpeedAnimMul);
		}
		else
		{
			//Log(" Anim speed: %f\n", std::fabs(mfTurnSpeed) * mpEnemy->mfMoveSpeedAnimMul * 2);
			mpEnemy->mpCurrentAnimation->SetSpeed(std::fabs(mfTurnSpeed) * mpEnemy->mfMoveSpeedAnimMul * 2);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxEnemyMover::UpdateStepEffects(float afTimeStep)
{
	if(mpEnemy->IsInWater()==false || mpEnemy->GetWaterSurfaceData()==NULL) return;

	cAnimationState *pAnim = mpEnemy->GetCurrentAnimation();
	if(pAnim==NULL) return;

	///////////////////////////////////////
	// Check if a step is occuring!
	bool bStep = false;
	for(int i=0; i<pAnim->GetEventNum(); ++i)
	{
		cAnimationEvent *pEvent = pAnim->GetEvent(i);
        if(pEvent->mType != eAnimationEventType_Step) continue;

		if(	pEvent->mfTime >= pAnim->GetPreviousTimePosition() &&
			pEvent->mfTime < pAnim->GetTimePosition())
		{
			bStep = true;
			break;
		}
	}
	if(bStep==false) return;
	
	///////////////////////////////////////
	// Do the liquid effect
	cSurfaceData *pSurface = mpEnemy->GetWaterSurfaceData();
	
	//Get the speed
	float fSpeed = mpEnemy->mfWaterStepSpeedMisc;
	if(mMoveState == eLuxEnemyMoveState_Walking || mMoveState == eLuxEnemyMoveState_Backward)
	{
		fSpeed = mpEnemy->mfWaterStepSpeedWalk;
	}
	else if(mMoveState == eLuxEnemyMoveState_Running)
	{
		fSpeed = mpEnemy->mfWaterStepSpeedWalk;
	}

	//Get impact effect and coordinate
	cSurfaceImpactData *pImpact = pSurface->GetImpactDataFromSpeed(3.0f);
	cVector3f vEffectPos = mpCharBody->GetPosition();
	vEffectPos.y =mpEnemy->GetWaterSurfaceY()+0.01f;

	//Create sounda and ps for effect	
	cWorld *pWorld = mpEnemy->GetMap()->GetWorld();

	if(pImpact->GetPSName() != "")
	{
		cParticleSystem *pPS = pWorld->CreateParticleSystem("Splash", pImpact->GetPSName(),1);
		if(pPS) pPS->SetPosition(vEffectPos);
	}

	if(pImpact->GetSoundName() != "")
	{
		cSoundEntity *pSound = pWorld->CreateSoundEntity("Splash",pImpact->GetSoundName(),true);
		if(pSound) pSound->SetPosition(vEffectPos);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxEnemyMover_SaveData)

kSerializeVar(mbTurning, eSerializeType_Bool)
kSerializeVar(mfTurnGoalAngle, eSerializeType_Float32)
kSerializeVar(mfTurnSpeed, eSerializeType_Float32)
kSerializeVar(mfTurnBreakAcc, eSerializeType_Float32)

kSerializeVar(mfStuckCounter, eSerializeType_Float32)

kSerializeVar(mlMoveState, eSerializeType_Int32)
kSerializeVar(mbOverideMoveState, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

void cLuxEnemyMover_SaveData::FromMover(cLuxEnemyMover *apMover)
{	
	mbTurning = apMover->mbTurning;
	mfTurnGoalAngle = apMover->mfTurnGoalAngle;
	mfTurnSpeed = apMover->mfTurnSpeed;
	mfTurnBreakAcc = apMover->mfTurnBreakAcc;

	mfStuckCounter = apMover->mfStuckCounter;

	mlMoveState = apMover->mMoveState;
	mbOverideMoveState = apMover->mbOverideMoveState;
}

//-----------------------------------------------------------------------

void cLuxEnemyMover_SaveData::ToMover(cLuxEnemyMover *apMover)
{
	apMover->mbTurning = mbTurning;
	apMover->mfTurnGoalAngle = mfTurnGoalAngle;
	apMover->mfTurnSpeed = mfTurnSpeed;
	apMover->mfTurnBreakAcc = mfTurnBreakAcc;

	apMover->mfStuckCounter = mfStuckCounter;

	apMover->mMoveState = (eLuxEnemyMoveState)mlMoveState;
	apMover->mbOverideMoveState = mbOverideMoveState;
}

//-----------------------------------------------------------------------
