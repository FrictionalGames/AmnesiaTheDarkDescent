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

#include "LuxPlayerState_InteractRotateBase.h"

#include "LuxPlayer.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState_Normal.h"
#include "LuxInputHandler.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPlayerState_InteractRotateBase::iLuxPlayerState_InteractRotateBase(cLuxPlayer *apPlayer, eLuxPlayerState aState) 
: iLuxPlayerState_Interact(apPlayer, aState)
{
	mfMoveToMouseAddFactor = gpBase->mpGameCfg->GetFloat("Player_Interaction","MoveToMouseAddFactor", 0.01f);
	mfMaxTorque = gpBase->mpGameCfg->GetFloat("Player_Interaction","MoveMaxForce", 1000.0f);


	mRotatePid.SetErrorNum(10);
	mRotatePid.p = 10.0f;
	mRotatePid.i = 0.0f;
	mRotatePid.d = 1.0f;
}

//-----------------------------------------------------------------------

iLuxPlayerState_InteractRotateBase::~iLuxPlayerState_InteractRotateBase()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::OnEnterState(eLuxPlayerState aPrevState)
{
	//Log("---- Interact Start -------\n");

	/////////////////////////////////
	//Clear smoothing
	if(gpBase->mpInputHandler->GetSmoothMouse()==false)
		gpBase->mpInputHandler->ResetSmoothMousePos();

	/////////////////////////////////
	//Get the static variables
	SetupInteractVars();

	mpMoveBaseData = mpCurrentProp->GetMoveBaseData();
	
    mpCurrentJoint = mpCurrentBody->GetJoint(0);
	
	/////////////////////////////////
	//Reset data
	mRotatePid.Reset();
	mfRotSpeed =0;
	mvMouseAdd =0;

	/////////////////////////////////
	//Setup body
	mbHasGravity = mpCurrentBody->GetGravity();
	
	mpCurrentBody->SetGravity(false);//TODO: User var!
	
	/////////////////////////////////
	// Setup vectors
	/*cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());
	mvJointForward = cMath::Vector3Cross(mpCurrentJoint->GetPinDir(), vJointToBody);
	*/

	/////////////////////////////////
	//Get Max distance
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
	mfMaxDistance = cMath::Vector3Dist(pCharBody->GetFeetPosition(), mpCurrentJoint->GetPivotPoint());
	mfMaxDistance = mfMaxDistance * 1.2f + 0.5f;

	/////////////////////////////////
	//Callback
	EnterRotateBase(aPrevState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::OnLeaveState(eLuxPlayerState aNewState)
{
	//Log("---- Interact End -------\n");
	/////////////////////////////////
	//Reset body
	mpCurrentBody->SetGravity(mbHasGravity);

	ResetInteractVars();

	LeaveRotateBase(aNewState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::SetupForceAxes()
{
	/////////////////////////////
	//Set up variables
	cCamera *pCam = mpPlayer->GetCamera();

	cVector3f vCamDir[3];
	vCamDir[1] = cVector3f(0,1,0);
	vCamDir[0] = cMath::Vector3Cross(pCam->GetForward(), vCamDir[1]);
	vCamDir[2] = cMath::Vector3Cross(vCamDir[1], vCamDir[0]);

	for(int i=0; i<3; ++i) mvCamDir[i] = vCamDir[i];
	
	cVector3f vForceDir[2];
	float fMaxCos[2] = {0,0};
	int lMaxCam[2] = {-1,-1};
	int lForceAxis[2] = {-1,-1};

	/////////////////////////////
	//Get the directions
	vForceDir[0] = cMath::Vector3Cross(pCam->GetForward(), mpCurrentJoint->GetPinDir());
	vForceDir[1] = cMath::Vector3Cross(mpCurrentJoint->GetPinDir(),vForceDir[0]);

	/////////////////////////////
	//Determine the cam direction closes to each force direction
	for(int force=0; force < 2; ++force)
	for(int cam=0; cam < 3; ++cam)
	{
		float fCos = cMath::Vector3Dot(vForceDir[force], vCamDir[cam]);
		if(cMath::Abs(fCos) > cMath::Abs(fMaxCos[force]))
		{
			fMaxCos[force] = fCos;
			lMaxCam[force] = cam;
		}
	}

	/////////////////////////////
	//Determine what axis to place each force dir
	for(int force=0; force < 2; ++force)
	{
		int lOther = force==0?1:0;
		if(lMaxCam[force]==0)
		{
			lForceAxis[force] =0;
			lForceAxis[lOther] = 1;
			break;
		}
		if(lMaxCam[force]==1)
		{
			lForceAxis[force] = 1;
			lForceAxis[lOther] = 0;
			break;
		}
	}

	/////////////////////////////
	//Set the forces axes
	for(int force=0; force < 2; ++force)
	{
		float fMul = 1.0f;
		if(fMaxCos[force] < 0) fMul = -1.0f;
		mvForceAxis[lForceAxis[force]] = vForceDir[force] * fMul;
		
		//debug:
		mfAxisCos[lForceAxis[force]] = fMaxCos[force];
	}

	/////////////////////////////
	//If right force is close to forward, turn up a little bit to compensate.
	float fFwdRightDot = cMath::Vector3Dot(vCamDir[2], mvForceAxis[0]);
	if(fFwdRightDot > 0)
	{
		mvForceAxis[1] += mvForceAxis[0]*fFwdRightDot;
		mvForceAxis[1].Normalize();
	}
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::Update(float afTimeStep)
{
	//////////////////////////////
	//Update move add
	
	if(gpBase->mpInputHandler->GetSmoothMouse()==false)
		mvMouseAdd = gpBase->mpInputHandler->GetSmoothMousePos(mvMouseAdd);

	//////////////////////////////
	//Set up variables
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
	cCamera *pCam = mpPlayer->GetCamera();
	iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(mpCurrentJoint);
	
	//////////////////////////////
	//Check if out of range
	float fDistance = cMath::Vector3Dist(pCharBody->GetFeetPosition(), mpCurrentJoint->GetPivotPoint());
	if(fDistance > mfMaxDistance)
	{
		mpPlayer->ChangeState(mPreviousState);
	}

	
	///////////////////////////////////
	//Slow down the speed based on the current speed
	float fSlowDownSpeed = cMath::Abs(mfRotSpeed) * mpMoveBaseData->mfMoveSlowDownFactor;
	if(mfRotSpeed > 0) 
	{
		mfRotSpeed -= afTimeStep * fSlowDownSpeed;
		if(mfRotSpeed < 0) mfRotSpeed = 0;
	}
	if(mfRotSpeed < 0) 
	{
		mfRotSpeed += afTimeStep * fSlowDownSpeed;
		if(mfRotSpeed > 0) mfRotSpeed = 0;
	}
	
	if(mvMouseAdd == 0)
	{
		mfRotSpeed = 0;
	}
	
	///////////////////////////////////
	//Get vectors and get move speed
	float fSpeedAdd = GetSpeedAdd(pCam);

	///////////////////////////////////
	//Change the speed and cap it o max if needed
	
	mfRotSpeed +=  fSpeedAdd * 3000.0f * mpMoveBaseData->mfMoveSpeedFactor * afTimeStep;	
	if(mfRotSpeed > mpMoveBaseData->mfMoveMaxSpeed)	mfRotSpeed = mpMoveBaseData->mfMoveMaxSpeed;
	if(mfRotSpeed < -mpMoveBaseData->mfMoveMaxSpeed)	mfRotSpeed = -mpMoveBaseData->mfMoveMaxSpeed;

	///////////////////////////////////
	//Calculate torque
	cVector3f vRotateDir = mpCurrentJoint->GetPinDir();
	cVector3f vBodyVel = mpCurrentBody->GetAngularVelocity();
	cVector3f vHingeVel = vRotateDir * cMath::Vector3Dot(vRotateDir, vBodyVel);
	cVector3f vWantedVel = vRotateDir * mfRotSpeed;

	cVector3f vTorque = mRotatePid.Output(vWantedVel - vHingeVel, afTimeStep);
	vTorque = cMath::MatrixMul(mpCurrentBody->GetInertiaMatrix(), vTorque);
	
	vTorque = cMath::Vector3MaxLength(vTorque, mfMaxTorque);
		
	mpCurrentBody->AddTorque(vTorque);

	mvLastMouseAdd = mvMouseAdd;
	mvMouseAdd = 0;
}

void iLuxPlayerState_InteractRotateBase::PostUpdate(float afTimeStep)
{
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_InteractRotateBase::OnDoAction(eLuxPlayerAction aAction,bool abPressed)
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
		//Pressed
		if(abPressed)
		{
			OnThrow();

			mpPlayer->ChangeState(mPreviousState);	

			return false;
		}		
	}

	return true;
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::OnScroll(float afAmount)
{
	
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_InteractRotateBase::OnMove(eCharDir aDir, float afMul)
{
	if(aDir == eCharDir_Forward)	mvMouseAdd.y += afMul * mfMoveToMouseAddFactor;
	else							mvMouseAdd.x += afMul * -mfMoveToMouseAddFactor;

	return true;
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_InteractRotateBase::OnAddYaw(float afAmount)
{
	mvMouseAdd.x += afAmount;

	return false;
}

bool iLuxPlayerState_InteractRotateBase::OnAddPitch(float afAmount)
{
#ifdef USE_GAMEPAD
	////////////////
	// We have both gamepad and mouse connected?
	if(gpBase->mpInputHandler->IsGamepadPresent())
	{
		/////////////
		// Check which one of them was used for the input this frame
#if USE_SDL2
		if(cMath::Abs(gpBase->mpInputHandler->GetGamepad()->GetAxisValue(eGamepadAxis_RightY)) > 0.0f)
#else
		if(cMath::Abs(gpBase->mpInputHandler->GetGamepad()->GetAxisValue(eGamepadAxis_3)) > 0.0f)
#endif
		{
			//Gamepad was used
			if(gpBase->mpInputHandler->GetInvertGamepadLook()) afAmount = -afAmount;
		}
		else if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
	}
	else if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
#else
	if(gpBase->mpInputHandler->GetInvertMouse()) afAmount = -afAmount;
#endif
	mvMouseAdd.y += afAmount;

	return false;
}

//-----------------------------------------------------------------------

cGuiGfxElement* iLuxPlayerState_InteractRotateBase::GetCrosshair()
{
	return NULL;
}

//-----------------------------------------------------------------------

float iLuxPlayerState_InteractRotateBase::DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("Speed: %f"), mfRotSpeed);//mSmoothSpeed.GetOutput());
	afStartY += 13.0f;

	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("Angle: %f"),  cMath::ToDeg(mpCurrentJoint->GetAngle()));
	afStartY += 13.0f;

	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("AngularVel: (%ls)"),cString::To16Char(mpCurrentBody->GetAngularVelocity().ToString()).c_str());
	afStartY += 13.0f;

	
	//apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("YForce: (%ls) YCam: (%ls)"),  
	//									cString::To16Char(mvForceAxis[1].ToString()).c_str(),
	//									cString::To16Char(mpPlayer->GetCamera()->GetUp().ToString()).c_str());
	//afStartY += 13.0f;


	afStartY = mpCurrentProp->OnInteractDebugDraw(apSet,apFont,afStartY);

	return afStartY;	
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	/*cCamera *pCam = mpPlayer->GetCamera();
	cVector3f vJointPos = mpCurrentBody->GetLocalPosition();//mpCurrentJoint->GetPivotPoint();

	cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());

	cVector3f vUp = pCam->GetUp();
	cVector3f vRight = pCam->GetRight();
	cVector3f vForward = pCam->GetForward();

	cVector3f vPushAmount = (vUp + vForward) * 1 + vRight * -1;
	cVector3f vPushRotateDir = cMath::Vector3Cross(vJointToBody, vPushAmount);

	float fSpeedAdd = cMath::Vector3Dot(vPushRotateDir, mpCurrentJoint->GetPinDir());

	//apFunctions->GetLowLevelGfx()->DrawLine(vJointPos, vJointPos+cMath::Vector3Normalize(vPushAmount), cColor(1,0,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(vJointPos, vJointPos+cMath::Vector3Normalize(vPushRotateDir), cColor(0,1,0,1));

	apFunctions->GetLowLevelGfx()->DrawLine(vJointPos, vJointPos+mvForceAxis[0], cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vJointPos, vJointPos+mvForceAxis[1], cColor(0,1,0,1));

	//float fAdd = GetSpeedAdd(mpPlayer->GetCamera());
	//apFunctions->GetLowLevelGfx()->DrawLine(vJointPos, vJointPos + mpCurrentJoint->GetPinDir()*fAdd*10.0f, cColor(0,0,1,1));*/
}


//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxPlayerState_InteractRotateBase_SaveData, iLuxPlayerState_Interact_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------


void iLuxPlayerState_InteractRotateBase::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	iLuxPlayerState_InteractRotateBase_SaveData *pData = static_cast<iLuxPlayerState_InteractRotateBase_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	iLuxPlayerState_InteractRotateBase_SaveData *pData = static_cast<iLuxPlayerState_InteractRotateBase_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_InteractRotateBase::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	iLuxPlayerState_InteractRotateBase_SaveData *pData = static_cast<iLuxPlayerState_InteractRotateBase_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------





