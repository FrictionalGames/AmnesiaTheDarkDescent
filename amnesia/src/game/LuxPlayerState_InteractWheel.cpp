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

#include "LuxPlayerState_InteractWheel.h"

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

cLuxPlayerState_InteractWheel::cLuxPlayerState_InteractWheel(cLuxPlayer *apPlayer) : iLuxPlayerState_InteractRotateBase(apPlayer, eLuxPlayerState_InteractWheel)
{
	mRotatePid.p = 5.0f;
	mRotatePid.i = 0.0f;
	mRotatePid.d = 0.1f;
}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractWheel::~cLuxPlayerState_InteractWheel()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	return;
	cCamera *pCam = mpPlayer->GetCamera();

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetProgram(NULL);
	apFunctions->SetMatrix(NULL);


	cVector3f vAxes(1,1,1);
	vAxes = cMath::MatrixMul(mpCurrentBody->GetInertiaMatrix(), vAxes);

	cVector3f vPinDir = mpCurrentJoint->GetPinDir();
	vPinDir = cMath::MatrixMul(mpCurrentBody->GetInertiaMatrix(), vPinDir);
	
	cVector3f vPivot = mpCurrentJoint->GetPivotPoint();
	//apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+cVector3f(1,0,0)*vAxes.x*10, cColor(1,0,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+cVector3f(0,1,0)*vAxes.y*10, cColor(0,1,0,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+cVector3f(0,0,1)*vAxes.z*-10, cColor(0,0,1,1));

	apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+vPinDir*10, cColor(0,0,1,1));
	
	//apFunctions->GetLowLevelGfx()->DrawLine(vPivot, vPivot+mpCurrentJoint->GetPinDir(), cColor(0,0,1,1));
	//apFunctions->GetLowLevelGfx()->DrawLine(mpCurrentBody->GetLocalPosition(), 
	//										mpCurrentBody->GetLocalPosition() + mpCurrentBody->GetAngularVelocity(), 
	//										cColor(0,0,1,1));

}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::EnterRotateBase(eLuxPlayerState aPrevState)
{
	mlstMoveAdds.clear();
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::LeaveRotateBase(eLuxPlayerState aNewState)
{

}

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractWheel::GetSpeedAdd(cCamera *apCam)
{
	mlstMoveAdds.push_back(mvMouseAdd);

	size_t lMaxMoveAdds = 10;
	if(mlstMoveAdds.size() > lMaxMoveAdds) mlstMoveAdds.pop_front();

	if(mlstMoveAdds.size() < 4) return 0;

	///////////////////////////////
	//Calculate the tangents
	cVector2f vPrevPos =0;
	bool bFirst = true;
	float fProperPosCount =0;

	cVector3f vTanCenterVec[2]={0,0};
	
	//Iterate the latest 10 moveadds, get tanget (both dirs) and for each tangent dir calculate
	//prev_pos + tan and add to that tan center vec accu,
	for(std::list<cVector2f>::iterator it = mlstMoveAdds.begin(); it != mlstMoveAdds.end(); ++it)
	{
		cVector2f vNormalPos = *it;
		vNormalPos.Normalize();

		if(bFirst)
		{
			bFirst = false;
			vPrevPos = vNormalPos;
			continue;
		}

        cVector2f vDiff = vNormalPos - vPrevPos;
		if(vDiff.Length() < 0.00001f) continue;

		cVector2f vTangent[2] = { cVector2f(vDiff.y, -vDiff.x), cVector2f(-vDiff.y, vDiff.x) };
		for(int i=0; i<2; ++i)
		{
			cVector2f vTan = vTangent[i];
			vTan.Normalize();

			vTanCenterVec[i] += vPrevPos + vTan;
		}

		fProperPosCount += 1;
	}
	
	//The the median tan center vec
	float fTanCenterDist[2]={0,0};
	for(int i=0; i<2; ++i)
	{
		fTanCenterDist[i] = (vTanCenterVec[i] / fProperPosCount).Length();	
	}

	//Get the direction of the rotation and also make sure there is some rotation going on.
    float fDirMul = 1;
	float fMinDist = fTanCenterDist[1];
	if(fTanCenterDist[0] < fTanCenterDist[1]){
		fDirMul = -1;
		fMinDist = fTanCenterDist[0];
	}

	//If too far from center, there is no real rotation made, so skip the movement.
    if(fMinDist > 0.75f) return 0;
	
	///////////////////////////////
	//Get the rotate direction based on the pin.
	cCamera *pCam = mpPlayer->GetCamera();
	
	cVector3f vFwd = pCam->GetForward();
	cVector3f vPinDir = mpCurrentJoint->GetPinDir();
	
	if(cMath::Vector3Dot(vPinDir, vFwd) < 0) fDirMul = -fDirMul;


	///////////////////////////////
	//Calculate the speed
	float fLength = mvMouseAdd.Length();
	
	return fDirMul *fLength* 1.0f;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::OnThrow()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractWheel_SaveData, iLuxPlayerState_InteractRotateBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractWheel::CreateSaveData()
{
	return hplNew(cLuxPlayerState_InteractWheel_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractWheel::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_InteractWheel_SaveData *pData = static_cast<cLuxPlayerState_InteractWheel_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_InteractWheel_SaveData *pData = static_cast<cLuxPlayerState_InteractWheel_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractWheel::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_InteractWheel_SaveData *pData = static_cast<cLuxPlayerState_InteractWheel_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------





