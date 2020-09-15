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

#ifndef LUX_PLAYER_STATE_INTERACT_PUSH_H
#define LUX_PLAYER_STATE_INTERACT_PUSH_H

//----------------------------------------------

#include "LuxPlayerState_Interact.h"

//----------------------------------------------

class cLuxInteractData_Push;

//------------------------------------

class cLuxPlayerState_InteractPush_SaveData : public iLuxPlayerState_Interact_SaveData
{
	kSerializableClassInit(cLuxPlayerState_InteractPush_SaveData)
public:

};

//----------------------------------------------

class cLuxPlayerState_InteractPush : public iLuxPlayerState_Interact
{
typedef iLuxPlayerState_Interact super_class;
public:	
	cLuxPlayerState_InteractPush(cLuxPlayer *apPlayer);
	virtual ~cLuxPlayerState_InteractPush();

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

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return false; }
	iLuxPlayerState_SaveData* CreateSaveData();

	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	void ClearMoveVars();

	cLuxInteractData_Push *mpPushData;

	float mfTotalMoveMul[2];
	bool mbMoving[2];

	float mfMaxSpeedInDir[2];

	cPidController<cVector3f> mVelocityPid;
	cPidController<cVector3f> mStopVelocityPid;
	
	cVector3f mvLocalAttachPos;

	cVector3f mvPushDir;
	float mfMaxAttachPointDist;

	float mfWalkMaxSpeed;
	float mfWalkForce;
	float mfRunMaxSpeed;
	float mfRunForce;
	float mfCrouchMaxSpeed;
	float mfCrouchForce;

	float mfPitchMinRange;
	float mfPitchMaxRange;
	float mfYawRange;

	float mfPrevMinPitchLimit;
	float mfPrevMaxPitchLimit;

	float mfMaxForce;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_GRAB_H
