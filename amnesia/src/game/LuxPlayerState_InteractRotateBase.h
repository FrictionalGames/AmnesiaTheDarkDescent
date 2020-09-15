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

#ifndef LUX_PLAYER_STATE_INTERACT_ROTATE_BASE_H
#define LUX_PLAYER_STATE_INTERACT_ROTATE_BASE_H

//----------------------------------------------

#include "LuxPlayerState_Interact.h"

//----------------------------------------------

class iLuxInteractData_RotateBase;

//------------------------------------

class iLuxPlayerState_InteractRotateBase_SaveData : public iLuxPlayerState_Interact_SaveData
{
	kSerializableClassInit(iLuxPlayerState_InteractRotateBase_SaveData)
public:

};
//----------------------------------------------

class iLuxPlayerState_InteractRotateBase : public iLuxPlayerState_Interact
{
typedef iLuxPlayerState_Interact super_class;
public:	
	iLuxPlayerState_InteractRotateBase(cLuxPlayer *apPlayer, eLuxPlayerState aState);
	virtual ~iLuxPlayerState_InteractRotateBase();

	void OnEnterState(eLuxPlayerState aPrevState);
	void OnLeaveState(eLuxPlayerState aNewState);

	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);

	bool OnDoAction(eLuxPlayerAction aAction,bool abPressed);

	void OnScroll(float afAmount);

	bool OnMove(eCharDir aDir, float afMul);

	bool OnAddYaw(float afAmount);
	bool OnAddPitch(float afAmount);

	bool OnRun(bool abPressed){ return true;}
	bool OnJump(bool abPressed){ return false;}

	cGuiGfxElement* GetCrosshair();

	void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter){}

	float DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY);
	
	virtual void RenderSolid(cRendererCallbackFunctions* apFunctions);

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return false; }
	
	virtual void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);


protected:
	virtual void EnterRotateBase(eLuxPlayerState aPrevState){}
	virtual void LeaveRotateBase(eLuxPlayerState aNewState){}

	virtual float GetSpeedAdd(cCamera *apCam)=0;
	virtual void OnThrow()=0;

	void SetupForceAxes();

	iLuxInteractData_RotateBase *mpMoveBaseData;

	iPhysicsJoint *mpCurrentJoint;

	float mfMoveToMouseAddFactor;
	float mfMaxTorque;

	bool mbHasGravity;
	
	float mfMaxDistance;

	cVector3f mvJointRight;
	cVector3f mvJointForward;
	
	cVector3f mvForceAxis[2];
	
	cVector2f mvMouseAdd;
	cVector2f mvLastMouseAdd;
	
	float mfRotSpeed;
	
	cPidController<cVector3f> mRotatePid;

	cVector3f mvCamDir[3];
	float mfAxisCos[2];
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_ROTATE_BASE_H
