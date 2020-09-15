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

#include "LuxPlayerState_InteractPush.h"

#include "LuxPlayer.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState_Normal.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_InteractPush::cLuxPlayerState_InteractPush(cLuxPlayer *apPlayer) : iLuxPlayerState_Interact(apPlayer, eLuxPlayerState_InteractPush)
{
	mfWalkMaxSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushMaxSpeedWalk",0);
	mfWalkForce = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushForceWalk",0);
	
	mfRunMaxSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushMaxSpeedRun",0);
	mfRunForce = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushForceRun",0);
	
	mfCrouchMaxSpeed = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushMaxSpeedCrouch",0);
	mfCrouchForce = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushForceCrouch",0);
	
	mfYawRange = cMath::ToRad(gpBase->mpGameCfg->GetFloat("Player_Interaction","PushYawRange",0));
	mfPitchMinRange = cMath::ToRad(gpBase->mpGameCfg->GetFloat("Player_Interaction","PushPitchMinRange",0));
	mfPitchMaxRange = cMath::ToRad(gpBase->mpGameCfg->GetFloat("Player_Interaction","PushPitchMaxRange",0));
	
	mfMaxForce = gpBase->mpGameCfg->GetFloat("Player_Interaction","PushMaxForce",0);

	mVelocityPid.SetErrorNum(20);
	mVelocityPid.p = 15;
	mVelocityPid.i = 0;
	mVelocityPid.d = 0.1f;

	mStopVelocityPid.SetErrorNum(20);
	mStopVelocityPid.p = 10;
	mStopVelocityPid.i = 0;
	mStopVelocityPid.d = 0.1f;

}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractPush::~cLuxPlayerState_InteractPush()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::OnEnterState(eLuxPlayerState aPrevState)
{
	/////////////////////////////////
	//Get the static variables
	SetupInteractVars();
	
	cVector3f vInteractPos = cLuxPlayerStateVars::mvInteractPos;

	mpPushData = mpCurrentProp->GetPushData();
	
	/////////////////////////////////
	//Reset variables
	ClearMoveVars();

	mVelocityPid.Reset();
	mStopVelocityPid.Reset();
	
	mvPushDir =0;
	
	/////////////////////////////////
	//Set up character body
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
	pCharBody->SetDisableDiagSpeedBoost(false);

	/////////////////////////////////
	//Set up camera
	cCamera *pCam = mpPlayer->GetCamera();
	mfPrevMinPitchLimit = pCam->GetPitchMinLimit();
	mfPrevMaxPitchLimit = pCam->GetPitchMaxLimit();

	pCam->SetPitchLimits(pCam->GetPitch() - mfPitchMinRange, pCam->GetPitch() + mfPitchMaxRange);
	pCam->SetYawLimits(pCam->GetYaw() - mfYawRange, pCam->GetYaw() + mfYawRange);

	
	/////////////////////////////////
	//Calculate the local attach pos (if in attach at point)
	if(mpPushData->mbPushAtPoint)
	{
		cMatrixf mtxInvWorld = cMath::MatrixInverse(mpCurrentBody->GetLocalMatrix());
		mvLocalAttachPos = cMath::MatrixMul(mtxInvWorld, vInteractPos);
	}

	/////////////////////////////////
	//Get the maximum distance
	if(mpPushData->mbPushAtPoint)
	{
		mfMaxAttachPointDist = cMath::Vector3Dist(pCharBody->GetFeetPosition(), vInteractPos);
	}
	else
	{
		mfMaxAttachPointDist = cMath::Vector3Dist(pCharBody->GetFeetPosition(), mpCurrentBody->GetLocalPosition());
	}
	mfMaxAttachPointDist = mfMaxAttachPointDist*1.2f + 0.3f;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::OnLeaveState(eLuxPlayerState aNewState)
{
	if(mpPlayer->GetCharacterBody())
	{
		mpPlayer->GetCharacterBody()->SetDisableDiagSpeedBoost(true);
	}

	mpPlayer->GetCamera()->SetPitchLimits(mfPrevMinPitchLimit, mfPrevMaxPitchLimit);
	mpPlayer->GetCamera()->SetYawLimits(0, 0);

	ResetInteractVars();
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::Update(float afTimeStep)
{
	iPhysicsWorld *pPhysicsWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld()->GetPhysicsWorld();
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	///////////////////////////////////////
	// Get the attach point (skip if not at point)
	cVector3f vAttachPos(0);
	if(mpPushData->mbPushAtPoint)
	{
		vAttachPos = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mvLocalAttachPos);
	}

	///////////////////////////////////////
	// Check so not too far away from body
	float fDistance = mpPushData->mbPushAtPoint ?	cMath::Vector3Dist(pCharBody->GetFeetPosition(), vAttachPos) :
													cMath::Vector3Dist(pCharBody->GetFeetPosition(), mpCurrentBody->GetLocalPosition());
	if(fDistance > mfMaxAttachPointDist)
	{
		mpPlayer->ChangeState(mPreviousState);
		return;
	}

	///////////////////////////////////////
	// Get the direction that to move in
	cVector3f vDirAdd[2] = {pCharBody->GetForward(),pCharBody->GetRight()};
	mvPushDir =0;
	for(int i=0; i<2; ++i)
	{
		if(mbMoving[i]) mvPushDir += vDirAdd[i] * mfTotalMoveMul[i];
	}
	
	///////////////////////////////////////
	// Check if there was any movement
    if(mvPushDir == 0)
	{
	
		cVector3f vBodyVel = mpPushData->mbPushAtPoint ?  mpCurrentBody->GetVelocityAtPosition(vAttachPos) : mpCurrentBody->GetLinearVelocity();
		cVector3f vVelError = cVector3f(0) - vBodyVel; 
		vVelError.y =0; //Skip fixing y and concentrate on ground plane.
		cVector3f vForce = mStopVelocityPid.Output(vVelError,afTimeStep) * mpCurrentBody->GetMass();

		vForce = cMath::Vector3MaxLength(vForce, mfMaxForce);
		
		if(mpPushData->mbPushAtPoint)
			mpCurrentBody->AddForceAtPosition(vForce,vAttachPos);    
		else
			mpCurrentBody->AddForce(vForce); 
		
		return;
	}

	mvPushDir.Normalize();
	
	///////////////////////////////////////
	// Update speed so it only travels in the correct direction!
	cVector3f vBodyVel = mpPushData->mbPushAtPoint ?  mpCurrentBody->GetVelocityAtPosition(vAttachPos) : mpCurrentBody->GetLinearVelocity();
	cVector3f vVelInDir = mvPushDir * cMath::Vector3Dot(mvPushDir, vBodyVel);
	
	mVelocityPid.p = 12;
	mVelocityPid.i = 0;
	mVelocityPid.d = 0.2f;
	cVector3f vVelError = vVelInDir - vBodyVel;
	vVelError.y =0; //Skip fixing y and concentrate on ground plane.
	cVector3f vCorrectForce = mVelocityPid.Output(vVelError,afTimeStep) * mpCurrentBody->GetMass();

	vCorrectForce = cMath::Vector3MaxLength(vCorrectForce, mfMaxForce);

	if(mpPushData->mbPushAtPoint)
		mpCurrentBody->AddForceAtPosition(vCorrectForce,vAttachPos);    
	else
		mpCurrentBody->AddForce(vCorrectForce); 

	
	///////////////////////////////////////
	// GetMax Speed and force
	float fMaxSpeed = mfWalkMaxSpeed;
	float fPushForce = mfWalkForce;
	if(mpPlayer->GetCurrentMoveState() == eLuxMoveState_Normal)
	{
		cLuxMoveState_Normal *pNormalMove = static_cast<cLuxMoveState_Normal*>(mpPlayer->GetMoveStateData(eLuxMoveState_Normal));
		
		if(pNormalMove->IsRunning())
		{
			fMaxSpeed = mfRunMaxSpeed;
			fPushForce = mfRunForce;
		}
		else if(pNormalMove->IsCrouching())
		{
			fMaxSpeed = mfCrouchMaxSpeed;
			fPushForce = mfCrouchForce;
		}
	}
	
	
	///////////////////////////////////////
	// Update the box force
	if(vVelInDir.Length() < fMaxSpeed || cMath::Vector3Dot(vVelInDir,mvPushDir)<0)
	{
		cVector3f vForce = mvPushDir * fPushForce * mpPushData->mfPushForceMul;

		vForce = cMath::Vector3MaxLength(vForce, mfMaxForce);

		//Attach at point
		if(mpPushData->mbPushAtPoint)
		{
			mpCurrentBody->AddForceAtPosition(vForce,vAttachPos);
		}
		//Attach at center
		else
		{
			mpCurrentBody->AddForce(vForce);
		}
	}

	
	///////////////////////////////////////////
	//Get the max speed the player may have
	cVector3f vMaxPlayerVel = 0;
	
	//Make sure box is moving in right direction.
	if(cMath::Vector3Dot(vVelInDir,mvPushDir)>0)
	{
		vMaxPlayerVel = vVelInDir;
	}

	///////////////////////////////////////
	// Check if the body will collide with the player!
	cCollideData collideData;collideData.SetMaxSize(1);
	cMatrixf mtxNextTransform = cMath::MatrixMul(cMath::MatrixTranslate(mvPushDir * 0.1f), mpCurrentBody->GetLocalMatrix());
    
	bool bWillCollideWithPlayer = pPhysicsWorld->CheckShapeCollision(
										pCharBody->GetCurrentShape(), cMath::MatrixTranslate(pCharBody->GetPosition()),
										mpCurrentBody->GetShape(), mtxNextTransform,
										collideData,1,false);

	///////////////////////////////////////
	// Get the max right and forward speeds allowed.
	// Skip it if the body is about to collide with player!
	// Note: This will only work if move state is update before the player state!
	if(bWillCollideWithPlayer==false)
	{
		for(int i=0; i<2; ++i)
		{
			float fMaxSpeed = cMath::Abs( cMath::Vector3Dot(vDirAdd[i],vMaxPlayerVel) );
			pCharBody->SetMaxPositiveMoveSpeed((eCharDir)i,fMaxSpeed);
			pCharBody->SetMaxNegativeMoveSpeed((eCharDir)i,-fMaxSpeed);
		}
	}

	///////////////////////////////////////
	// Update player movement
	for(int i=0; i<2; ++i)
	{
		if(mbMoving[i])
			pCharBody->Move((eCharDir)i, mfTotalMoveMul[i]);
	}
	
}

void cLuxPlayerState_InteractPush::PostUpdate(float afTimeStep)
{
	ClearMoveVars();
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractPush::OnDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	////////////////////////////
	// Interact
	if(aAction == eLuxPlayerAction_Interact)
	{
		// Released
		if(abPressed==false)
		{
			mpPlayer->ChangeState(mPreviousState);

			return false;
		}
	}
	////////////////////////////
	// Attack
	else if(aAction == eLuxPlayerAction_Attack)
	{
		// Pressed
		if(abPressed)
		{
			iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
			if(mpPushData->mbPushAtPoint)
			{
				cVector3f vAttachPos = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mvLocalAttachPos);
				mpCurrentBody->AddImpulseAtPosition(mvPushDir * mpPushData->mfPushImpulse,vAttachPos);
			}
			else
			{
				mpCurrentBody->AddImpulse(mvPushDir * mpPushData->mfPushImpulse);
			}

			mpPlayer->ChangeState(mPreviousState);	
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::OnScroll(float afAmount)
{
	
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractPush::OnMove(eCharDir aDir, float afMul)
{
	mfTotalMoveMul[aDir] += afMul;
	mbMoving[aDir] = true;

	return false;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_InteractPush::OnAddYaw(float afAmount)
{
	return true;
}

bool cLuxPlayerState_InteractPush::OnAddPitch(float afAmount)
{
	return true;
}

//-----------------------------------------------------------------------

cGuiGfxElement* cLuxPlayerState_InteractPush::GetCrosshair()
{
	return NULL;
}

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractPush::DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	cVector3f vBodyVel = mpCurrentBody->GetLinearVelocity(); //TODO: Get vel at point if pick at point!
	cVector3f vVelInDir = mvPushDir * cMath::Vector3Dot(mvPushDir, vBodyVel);
	
	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("CurrentVel: %ls"), cString::To16Char(vBodyVel.ToString()).c_str());
	afStartY += 13.0f;
	
	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("WantedVel: %ls"), cString::To16Char(vVelInDir.ToString()).c_str());
	afStartY += 13.0f;

	return afStartY;	
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::ClearMoveVars()
{
	for(int i=0; i<2; ++i)
	{
		mfTotalMoveMul[i] = 0;
		mbMoving[i] = false;
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractPush_SaveData, iLuxPlayerState_Interact_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractPush::CreateSaveData()
{
	return hplNew(cLuxPlayerState_InteractPush_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractPush::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_InteractPush_SaveData *pData = static_cast<cLuxPlayerState_InteractPush_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_InteractPush_SaveData *pData = static_cast<cLuxPlayerState_InteractPush_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractPush::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_InteractPush_SaveData *pData = static_cast<cLuxPlayerState_InteractPush_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------





