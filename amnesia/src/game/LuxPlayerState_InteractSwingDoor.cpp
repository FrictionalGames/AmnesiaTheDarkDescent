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

#include "LuxPlayerState_InteractSwingDoor.h"

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

cLuxPlayerState_InteractSwingDoor::cLuxPlayerState_InteractSwingDoor(cLuxPlayer *apPlayer) : iLuxPlayerState_InteractRotateBase(apPlayer, eLuxPlayerState_InteractSwingDoor)
{
	
}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractSwingDoor::~cLuxPlayerState_InteractSwingDoor()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractSwingDoor::GetSpeedAdd(cCamera *apCam)
{
	cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());

	cVector3f vUp = apCam->GetUp();
	cVector3f vRight = apCam->GetRight();
	cVector3f vForward = apCam->GetForward();

	cVector3f vPushAmount = (vUp + vForward) * mvMouseAdd.y + vRight * -mvMouseAdd.x;
	cVector3f vPushRotateDir = cMath::Vector3Cross(vJointToBody, vPushAmount);

	float fSpeedAdd = cMath::Vector3Dot(vPushRotateDir, mpCurrentJoint->GetPinDir());
	
	return fSpeedAdd;
}

void cLuxPlayerState_InteractSwingDoor::OnThrow()
{
	cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
	cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());
	mvJointForward = cMath::Vector3Cross(mpCurrentJoint->GetPinDir(), vJointToBody);

	cVector3f vImpulse = mvJointForward * mpMoveBaseData->mfMoveThrowImpulse;
	float fForwardDot = cMath::Vector3Dot(mvJointForward, mpPlayer->GetCamera()->GetForward());
	if(fForwardDot < 0) vImpulse = vImpulse * -1;

	mpCurrentBody->AddImpulse(vImpulse);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractSwingDoor_SaveData, iLuxPlayerState_InteractRotateBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractSwingDoor::CreateSaveData()
{
	return hplNew(cLuxPlayerState_InteractSwingDoor_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractSwingDoor::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractSwingDoor::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractSwingDoor::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------





