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

#include "LuxPlayerState_InteractLever.h"

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

cLuxPlayerState_InteractLever::cLuxPlayerState_InteractLever(cLuxPlayer *apPlayer) : iLuxPlayerState_InteractRotateBase(apPlayer, eLuxPlayerState_InteractLever)
{
	
}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractLever::~cLuxPlayerState_InteractLever()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractLever::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	return;

	cCamera *pCam = mpPlayer->GetCamera();

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetProgram(NULL);
	apFunctions->SetMatrix(NULL);

	cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());
	//cVector3f vJointMoveDir = cMath::Vector3Cross(vJointToBody, mpCurrentJoint->GetPinDir());

	cVector3f vUp = pCam->GetUp();
	cVector3f vRight = pCam->GetRight();
	cVector3f vForward = pCam->GetForward();

	cVector3f vUpJointForward = cMath::Vector3Cross(vUp, mpCurrentJoint->GetPinDir());
	cVector3f vUpAdd = vUpJointForward * cMath::Vector3Dot(vUp, vJointToBody);
	vUp = cMath::Vector3Normalize(vUpAdd + vUp);

	cVector3f vRightJointForward = cMath::Vector3Cross(vRight, mpCurrentJoint->GetPinDir());
	cVector3f vRightAdd = vRightJointForward * cMath::Vector3Dot(vRight, vJointToBody);
	vRight = cMath::Vector3Normalize(vRightAdd + vRight);

	cVector3f vPushAmount = vUp * mvLastMouseAdd.y + vRight * -mvLastMouseAdd.x;
	cVector3f vPushRotateDir = cMath::Vector3Cross(vJointToBody, vPushAmount);

	cVector3f vPivot = mpCurrentJoint->GetPivotPoint();
	apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+vPushAmount*30, cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+vUpJointForward, cColor(1,0,1,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+vJointToBody, cColor(0,1,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+mpCurrentJoint->GetPinDir(), cColor(0,0,1,1));

}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractLever::GetSpeedAdd(cCamera *apCam)
{
	cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());

	cVector3f vUp = apCam->GetUp();
	cVector3f vRight = apCam->GetRight();
	cVector3f vForward = apCam->GetForward();
	
	cVector3f vUpJointForward = cMath::Vector3Cross(vUp, mpCurrentJoint->GetPinDir());
	cVector3f vUpAdd = vUpJointForward * cMath::Vector3Dot(vUp, vJointToBody);
	vUp = cMath::Vector3Normalize(vUpAdd + vUp);

	cVector3f vRightJointForward = cMath::Vector3Cross(vRight, mpCurrentJoint->GetPinDir());
	cVector3f vRightAdd = vRightJointForward * cMath::Vector3Dot(vRight, vJointToBody);
	vRight = cMath::Vector3Normalize(vRightAdd + vRight);
	
	cVector3f vPushAmount = (vUp) * mvMouseAdd.y + (vRight) * -mvMouseAdd.x;
	cVector3f vPushRotateDir = cMath::Vector3Cross(vJointToBody, vPushAmount);

	float fSpeedAdd = cMath::Vector3Dot(vPushRotateDir, mpCurrentJoint->GetPinDir());

	return fSpeedAdd;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractLever::OnThrow()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractLever_SaveData, iLuxPlayerState_InteractRotateBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractLever::CreateSaveData()
{
	return hplNew(cLuxPlayerState_InteractLever_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractLever::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_InteractLever_SaveData *pData = static_cast<cLuxPlayerState_InteractLever_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractLever::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_InteractLever_SaveData *pData = static_cast<cLuxPlayerState_InteractLever_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractLever::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_InteractLever_SaveData *pData = static_cast<cLuxPlayerState_InteractLever_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------





