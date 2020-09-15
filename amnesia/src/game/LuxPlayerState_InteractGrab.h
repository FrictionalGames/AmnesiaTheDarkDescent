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

#ifndef LUX_PLAYER_STATE_INTERACT_GRAB_H
#define LUX_PLAYER_STATE_INTERACT_GRAB_H

//----------------------------------------------

#include "LuxPlayerState_Interact.h"

//----------------------------------------------

class cLuxPlayer;

class cLuxInteractData_Grab;

//----------------------------------------------

class cGrabbedBodyProperties
{
public:
	iPhysicsBody *mpBody;
	bool mbHasGravity;
	bool mbCollideCharacter;
	float mfMass;
};

class cGrabbedSubMeshProperties
{
public:
	cMaterial *mpCustomTransMaterial;
};

//------------------------------------

class cLuxPlayerState_InteractGrab_SaveData : public iLuxPlayerState_Interact_SaveData
{
	kSerializableClassInit(cLuxPlayerState_InteractGrab_SaveData)
public:
	cMatrixf m_mtxBodyRotation;
	cVector3f mvLocalBodyOffset;
	float mfDepth;
	float mfMaxDistance;
};

//------------------------------------


class cLuxPlayerState_InteractGrab : public iLuxPlayerState_Interact
{
typedef iLuxPlayerState_Interact super_class;
public:	
	cLuxPlayerState_InteractGrab(cLuxPlayer *apPlayer);
	virtual ~cLuxPlayerState_InteractGrab();

	void OnEnterState(eLuxPlayerState aPrevState);
	void OnLeaveState(eLuxPlayerState aNewState);

	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);

	bool OnDoAction(eLuxPlayerAction aAction,bool abPressed);

	void OnScroll(float afAmount);

	bool OnAddYaw(float afAmount);
	bool OnAddPitch(float afAmount);

	cGuiGfxElement* GetCrosshair();

	void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter);
	bool AllowBuoyancy(iPhysicsBody *apBody);

	float DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY);
	
	void RenderSolid(cRendererCallbackFunctions* apFunctions);

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return true; }
	iLuxPlayerState_SaveData* CreateSaveData();

	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	void SaveBodyProperties(iPhysicsBody *apBody);
	bool BodyIsAdded(iPhysicsBody *apBody);

	cLuxInteractData_Grab *mpGrabData;

	float mfMaxForce;
	float mfMaxTorque;
	float mfMaxAngularSpeed;

	float mfMinSlowPlayerMass;
	float mfMaxSlowPlayerMass;
	float mfMinSlowPlayerMul;

	float mfMaxLeaveAngularSpeed;
	float mfMaxLeaveLinearSpeed;
	
	std::vector<cGrabbedBodyProperties> mvBodyProperties;
	std::vector<cGrabbedSubMeshProperties> mvSubMeshProperties;

	bool mbCustomMaterialsSetup;

	cVector3f mvLocalBodyOffset;
	cMatrixf m_mtxBodyRotation;

	cPidController<cVector3f> mForcePid;
	cPidController<cVector3f> mSpeedTorquePid;

	float mfDepth;
	float mfMaxDistance;
	float mfMassSum;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_GRAB_H
