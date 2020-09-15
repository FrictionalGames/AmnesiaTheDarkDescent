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

#include "LuxPlayerState_Ladder.h"

#include "LuxPlayer.h"
#include "LuxArea_Ladder.h"
#include "LuxMoveState_Normal.h"
#include "LuxHelpFuncs.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxPlayerHelpers.h"


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_Ladder::cLuxPlayerState_Ladder(cLuxPlayer *apPlayer) : iLuxPlayerState(apPlayer, eLuxPlayerState_Ladder)
{
	mpLadder = NULL;

	mfUpSpeed = gpBase->mpGameCfg->GetFloat("Player_Ladder","UpSpeed",0);
	mfDownSpeed = gpBase->mpGameCfg->GetFloat("Player_Ladder","DownSpeed",0);

	mfStepLength = gpBase->mpGameCfg->GetFloat("Player_Ladder","StepLength",0);

	mfStepCount =0;

	mlState =0;
}

//-----------------------------------------------------------------------

cLuxPlayerState_Ladder::~cLuxPlayerState_Ladder()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::OnEnterState(eLuxPlayerState aPrevState)
{
	
	/////////////////////////////////////
	// Load the data
	mpLadder = cLuxPlayerStateVars::mpLadder;
	mvStartPosition = cLuxPlayerStateVars::mvLadderStartPos;

	/////////////////////////////////////
	// Get data
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
	
	/////////////////////////////////////
	// Set up player
	mpPlayer->ChangeMoveState(eLuxMoveState_Normal);
	pCharBody->SetGravityActive(false);
	pCharBody->SetTestCollision(false);
	pCharBody->StopMovement();
	pCharBody->ResetClimbing();

    cLuxMoveState_Normal *pMoveState = static_cast<cLuxMoveState_Normal*>(mpPlayer->GetMoveStateData(eLuxMoveState_Normal));
	pMoveState->SetCrouch(false);
	pMoveState->ResetJumping();

	/////////////////////////////////////
	// Holster lantern
	mbLanternDrawn = mpPlayer->GetHelperLantern()->IsActive();
	mpPlayer->GetHelperLantern()->SetActive(false,true,true, false);

	///////////////////////////////////////
	// Set up variables
	mlState =0;
	mfStepCount =0;
	mbPlayedSound = false;
	mfMoveMul =0;

	///////////////////////////////////////
	// Save pitch limits
	mfPitchMinLimit = mpPlayer->GetCamera()->GetPitchMinLimit();
	mfPitchMaxLimit = mpPlayer->GetCamera()->GetPitchMaxLimit();

	/////////////////////////////////////		
	//Different time if player is above the ladder.
	float fTime = 0.5f;
	if(mpPlayer->GetCharacterBody()->GetPosition().y > mpLadder->GetMaxY()) fTime = 1.2f;

	mfTimeCount = fTime;
	
	/////////////////////////////////////
	//Get the goal position and then add used to get there
	mvGoalPos = mvStartPosition;
	mvPosAdd = (mvGoalPos - mpPlayer->GetCharacterBody()->GetPosition()) / fTime;

	mvCharPosition = mpPlayer->GetCharacterBody()->GetPosition();

	/////////////////////////////////////
	//Get the goal rotation and the add to get there.
	mvGoalRot.x = 0;
	mvGoalRot.y = mpLadder->GetStartRotation().y;

	cVector3f vStartRot;
	vStartRot.x = mpPlayer->GetCamera()->GetPitch();
	vStartRot.y = mpPlayer->GetCamera()->GetYaw();

	mvRotAdd.x = cMath::GetAngleDistance(vStartRot.x, mvGoalRot.x,k2Pif) / fTime;
	mvRotAdd.y = cMath::GetAngleDistance(vStartRot.y, mvGoalRot.y,k2Pif) / fTime;

    /////////////////////////////////////
	// Play attach sound
	PlaySound("attach");
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::OnLeaveState(eLuxPlayerState aNewState)
{
	//////////////////////////////
	//Reset variables
	// Need to so pointer exists! (as this might be on leave!)
	if(mpPlayer->GetCharacterBody())
	{
		mpPlayer->GetCharacterBody()->SetGravityActive(true);
		mpPlayer->GetCharacterBody()->SetTestCollision(true);
	}

	mpPlayer->GetCamera()->SetPitchLimits(mfPitchMinLimit, mfPitchMaxLimit);
	mpPlayer->GetCamera()->SetYawLimits(0, 0);

	/////////////////////////////////////
	// Draw lantern if it was before
	if(mbLanternDrawn)
        mpPlayer->GetHelperLantern()->SetActive(true, true, true, false);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::Update(float afTimeStep)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
	cCamera *pCam = mpPlayer->GetCamera();

	//////////////////////////////////
	// Attach To Ladder
	if(mlState ==0)
	{
		mfTimeCount -= afTimeStep;

		mvCharPosition += mvPosAdd*afTimeStep;
		pCharBody->SetPosition(mvCharPosition);
		mvCharPosition = pCharBody->GetPosition();

		pCam->AddPitch(mvRotAdd.x *afTimeStep);
		pCam->AddYaw(mvRotAdd.y *afTimeStep);
		pCharBody->SetYaw(pCam->GetYaw());

		if(mfTimeCount<=0)
		{
			mpPlayer->GetCharacterBody()->SetTestCollision(true);

			////////////////////////////
			//Set turn head limits
			SetupHeadTurnLimits();

			mlState++;
		}
	}
	//////////////////////////////////
	// Move On Ladder
	else if(mlState == 1)
	{
		///////////////////////////////////
		// Update position
		mvCharPosition = pCharBody->GetPosition();

		//Up
		if(mfMoveMul > 0)
		{
			mvCharPosition.y += mfMoveMul*mfUpSpeed* afTimeStep;

			if(mfStepCount<0)mfStepCount=0;
		}
		//Down
		else if(mfMoveMul < 0)
		{
			mvCharPosition.y += mfMoveMul*mfDownSpeed*afTimeStep;

			if(mfStepCount>0)mfStepCount=0;
		}
		//Still
		else
		{
			if(!mbPlayedSound)
			{
				if(mfStepCount>0) PlaySound("up");
				if(mfStepCount<0) PlaySound("down");
			}

			mfStepCount =0;
			mbPlayedSound = false;
		}

		/////////////////////////////////
		//Check collision
		if(pCharBody->CheckCharacterFits(mvCharPosition,false,0,NULL,kEpsilonf)==false)
		{
			mfMoveMul =0;
			return;	
		}

		/////////////////////////////////
		//Check if climb sound should be played.
		if(mfMoveMul > 0)
		{
			mfStepCount += mfMoveMul*mfUpSpeed*afTimeStep;
			if(mfStepCount >= mfStepLength)
			{
				mfStepCount =0;
				PlaySound("up");
				mbPlayedSound = true;
			}
		}
		else if(mfMoveMul < 0)
		{
			mfStepCount += mfMoveMul*mfDownSpeed*afTimeStep;
			if(mfStepCount <= -mfStepLength)
			{
				mfStepCount =0;
				PlaySound("down");
				mbPlayedSound = true;
			}
		}
		
		/////////////////////////////////
		//Update character body
		pCharBody->SetPosition(mvCharPosition);


		/////////////////////////////////
		//Get the rays that will be tested to see if something solid was hit when clibming down.
        cVector3f vGroundRayStart1 = pCharBody->GetFeetPosition() + cVector3f(0,0.05f,0) + 
									mpLadder->GetForward()*pCharBody->GetSize().x*0.7f;
		cVector3f vGroundRayEnd1 = vGroundRayStart1 - cVector3f(0,0.1f,0);

		cVector3f vGroundRayStart2 = pCharBody->GetFeetPosition() + cVector3f(0,0.05f,0);
		cVector3f vGroundRayEnd2 = vGroundRayStart2 - cVector3f(0,0.1f,0);

		///////////////////////////
		//Check if at top
		if(mfMoveMul > 0 && (mvCharPosition.y - pCharBody->GetSize().y*0.5f) > mpLadder->GetMaxY())
		{
			mlState = 2;
			mfLeaveAtTopCount = 2;
			PlaySound("dettach");
		}
		///////////////////////////
		//Check if at bottom
		else if(	mfMoveMul < 0 && 
					(  (mvCharPosition.y - pCharBody->GetSize().y/2) < mpLadder->GetMinY() ||
					    pCharBody->CheckRayIntersection(vGroundRayStart1, vGroundRayEnd1,NULL,NULL) ||
						pCharBody->CheckRayIntersection(vGroundRayStart2, vGroundRayEnd2,NULL,NULL) 
					)
				)
		{
			mpPlayer->ChangeState(eLuxPlayerState_Normal);
		}

		mfMoveMul =0;
	}
	//////////////////////////////////
	// On the top of the ladder
	else if(mlState == 2)
	{
		//mfLeaveAtTopCount -= afTimeStep;
		pCharBody->Move(eCharDir_Forward,1);

		cVector3f vRayStart = pCharBody->GetFeetPosition() + cVector3f(0,0.1f,0);
		cVector3f vRayEnd = pCharBody->GetFeetPosition() - cVector3f(0,0.3f,0);

		mfLeaveAtTopCount -= afTimeStep;
		if( pCharBody->CheckRayIntersection(vRayStart, vRayEnd,NULL,NULL))
		{
			mpPlayer->ChangeState(eLuxPlayerState_Normal);
		}
		else if(mfLeaveAtTopCount <=0)
		{
			mpPlayer->ChangeState(eLuxPlayerState_Normal);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::PostUpdate(float afTimeStep)
{

}

//-----------------------------------------------------------------------


void cLuxPlayerState_Ladder::OnDraw(cGuiSet *apGuiSet ,float afFrameTime)
{

}

//-----------------------------------------------------------------------


cGuiGfxElement* cLuxPlayerState_Ladder::GetCrosshair()
{
	return NULL;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnMove(eCharDir aDir, float afMul)
{
	////////////////////////////////////
	//If in final states, do not move.
	if(mlState==0 || mlState==2) return false;
	if(aDir == eCharDir_Right) return false;

	mfMoveMul = afMul;

	return false;	
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnAddYaw(float afAmount)
{
	//Do not move camera when attaching
	if(mlState==0) 	return false;
	else			return true;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnAddPitch(float afAmount)
{
	//Do not move camera when attaching
	if(mlState==0) 	return false;
	else			return true;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnRun(bool abPressed)
{
	return false;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnJump(bool abPressed)
{
    if(abPressed)
	{
		mpPlayer->ChangeState(eLuxPlayerState_Normal);
		cLuxMoveState_Normal *pMoveState = static_cast<cLuxMoveState_Normal*>(mpPlayer->GetMoveStateData(eLuxMoveState_Normal));
		pMoveState->Jump();
	}

	return false;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnCrouch(bool abPressed)
{
	return false;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_Ladder::OnDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	if(abPressed)
	{
		////////////////////////////////
		//Release grip from ladder
		if(	aAction == eLuxPlayerAction_Interact ||
			aAction == eLuxPlayerAction_Attack)
		{
			mpPlayer->ChangeState(eLuxPlayerState_Normal);
		}
	}

	////////////////////////////////
	//Lantern shall still work!
	if( aAction == eLuxPlayerAction_Lantern)
	{
		return true;
	}
	
	return false;
}

//-----------------------------------------------------------------------


float cLuxPlayerState_Ladder::DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	return afStartY;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::PlaySound(const tString &asType)
{
	tString sSound = "player_ladder_"+mpLadder->GetMaterial()+"_"+asType;
	gpBase->mpHelpFuncs->PlayGuiSoundData(sSound,eSoundEntryType_World);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::SetupHeadTurnLimits()
{
	cCamera *pCam = mpPlayer->GetCamera();

	cVector2f vMaxHeadLimits = cVector2f(cMath::ToRad(120),cMath::ToRad(79));
	cVector2f vMinHeadLimits = cVector2f(cMath::ToRad(-120),cMath::ToRad(-60));
	float fXmax = pCam->GetYaw() + vMaxHeadLimits.x;
	float fYmax = pCam->GetPitch() + vMaxHeadLimits.y;

	float fXmin = pCam->GetYaw() + vMinHeadLimits.x;
	float fYmin = pCam->GetPitch() + vMinHeadLimits.y;

	pCam->SetPitchLimits(fYmin, fYmax);
	pCam->SetYawLimits(fXmin, fXmax);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_Ladder_SaveData, iLuxPlayerState_DefaultBase_SaveData)
kSerializeVar(mlState, eSerializeType_Int32)
kSerializeVar(mvStartPosition, eSerializeType_Vector3f)

kSerializeVar(mlLadderID, eSerializeType_Int32)

kSerializeVar(mfTimeCount, eSerializeType_Float32)

kSerializeVar(mvCharPosition, eSerializeType_Vector3f)

kSerializeVar(mvGoalPos, eSerializeType_Vector3f)
kSerializeVar(mvGoalRot, eSerializeType_Vector3f)

kSerializeVar(mvPosAdd, eSerializeType_Vector3f)
kSerializeVar(mvRotAdd, eSerializeType_Vector3f)

kSerializeVar(mfMoveMul, eSerializeType_Float32)

kSerializeVar(mfUpSpeed, eSerializeType_Float32)
kSerializeVar(mfDownSpeed, eSerializeType_Float32)

kSerializeVar(mfStepLength, eSerializeType_Float32)
kSerializeVar(mfStepCount, eSerializeType_Float32)
kSerializeVar(mbPlayedSound, eSerializeType_Bool)

kSerializeVar(mfPitchMaxLimit, eSerializeType_Float32)
kSerializeVar(mfPitchMinLimit, eSerializeType_Float32)

kSerializeVar(mbLanternDrawn, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_Ladder::CreateSaveData()
{
	return hplNew(cLuxPlayerState_Ladder_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_Ladder::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_Ladder_SaveData *pData = static_cast<cLuxPlayerState_Ladder_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
	if(mpLadder)
		pData->mlLadderID = mpLadder->GetID();
	else
		pData->mlLadderID = -1;
	
	kCopyToVar(pData, mlState);
	kCopyToVar(pData, mvStartPosition);

	kCopyToVar(pData, mfTimeCount);

	kCopyToVar(pData, mvCharPosition);

	kCopyToVar(pData, mvGoalPos);
	kCopyToVar(pData, mvGoalRot);

	kCopyToVar(pData, mvPosAdd);
	kCopyToVar(pData, mvRotAdd);

	kCopyToVar(pData, mfMoveMul);

	kCopyToVar(pData, mfUpSpeed);
	kCopyToVar(pData, mfDownSpeed);

	kCopyToVar(pData, mfStepLength);
	kCopyToVar(pData, mfStepCount);
	kCopyToVar(pData, mbPlayedSound);

	kCopyToVar(pData, mfPitchMaxLimit);
	kCopyToVar(pData, mfPitchMinLimit);

	kCopyToVar(pData, mfLeaveAtTopCount);

	kCopyToVar(pData, mbLanternDrawn);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_Ladder_SaveData *pData = static_cast<cLuxPlayerState_Ladder_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
	iLuxEntity *pEntity = gpBase->mpMapHandler->GetCurrentMap()->GetEntityByID(pData->mlLadderID,eLuxEntityType_Area, eLuxAreaType_Ladder);
	if(pEntity)
	{
		mpLadder = static_cast<cLuxArea_Ladder*>(pEntity);
	}
	
	kCopyFromVar(pData, mvStartPosition);

	cLuxPlayerStateVars::SetupLadder(mpLadder, mvStartPosition);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Ladder::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_Ladder_SaveData *pData = static_cast<cLuxPlayerState_Ladder_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
	kCopyFromVar(pData, mlState);
	
	kCopyFromVar(pData, mfTimeCount);

	kCopyFromVar(pData, mvCharPosition);

	kCopyFromVar(pData, mvGoalPos);
	kCopyFromVar(pData, mvGoalRot);

	kCopyFromVar(pData, mvPosAdd);
	kCopyFromVar(pData, mvRotAdd);

	kCopyFromVar(pData, mfMoveMul);

	kCopyFromVar(pData, mfUpSpeed);
	kCopyFromVar(pData, mfDownSpeed);

	kCopyFromVar(pData, mfStepLength);
	kCopyFromVar(pData, mfStepCount);
	kCopyFromVar(pData, mbPlayedSound);

	kCopyFromVar(pData, mfPitchMaxLimit);
	kCopyFromVar(pData, mfPitchMinLimit);

	kCopyFromVar(pData, mfLeaveAtTopCount);

	kCopyFromVar(pData, mbLanternDrawn);

	if(mlState>0)
	{
		SetupHeadTurnLimits();
	}
}

//-----------------------------------------------------------------------


