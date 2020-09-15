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

#ifndef LUX_PROP_SWING_DOOR_H
#define LUX_PROP_SWING_DOOR_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_SwingDoor_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_SwingDoor_SaveData)
public:
	bool mbLocked;
	bool mbBroken;
	bool mbClosed;

	int mlBrokenEntityID;

	bool mbDisableBreakable;
	bool mbDisableAutoClose;

	int mlCurrentMeshEntity;


    cEngineMeshEntity_SaveData mDamageMesh1;	
	cEngineMeshEntity_SaveData mDamageMesh2;
};

//----------------------------------------------

class cLuxSwingDoorJointData
{
public:
	iPhysicsJointHinge *mpHingeJoint;
	iPhysicsBody *mpChildBody;

    float mfMaxAngle;
};


//----------------------------------------------


class cLuxProp_SwingDoor : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_SwingDoor;
public:	
	cLuxProp_SwingDoor(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_SwingDoor();

	//////////////////////
	//Genera
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	void ImplementedOnSetActive(bool abX);

	void OnHealthChange();
	void OnDamage(float afAmount, int alStrength);

	//////////////////////
	//Properties
	iLuxInteractData_RotateBase* GetMoveBaseData(){ return &mSwingDoorData;}
	
	void SetClosed(bool abClosed, bool abEffects);
	bool GetClosed(){ return mbClosed;}

	/**
	 * -1=closed 0=between 1=open
	 */
	int GetDoorState();

	void SetLocked(bool abLocked, bool abEffects);
	bool GetLocked(){ return mbLocked;}

	void SetDisableAutoClose(bool abX){ mbDisableAutoClose=abX;}
	bool GetDisableAutoClose(){ return mbDisableAutoClose;}

	void SetCurrentDamageLevel(int alX);

	cMeshEntity* GetEffectMeshEntity();

	bool IsBroken(){ return mbBroken;}

	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState);


	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
	void SetupDoorPhysics(float afOpenAmount);

	cLuxSwingDoorJointData* GetJointDataFromBody(iPhysicsBody *apBody);
	cLuxSwingDoorJointData* GetJointDataFromJoint(iPhysicsJoint *apJoint);

	//////////////////////
	// Data
	std::vector<cLuxSwingDoorJointData> mvJointData;

	cLuxInteractData_SwingDoor mSwingDoorData;

	bool mbCanInteractWithStaticBody;
	tString msCloseOnSound;
	tString msCloseOffSound;
	tString msLockOnSound;
	tString msLockOffSound;
	tString msInteractLockedSound;
	
	bool mbDisableBreakable;
	bool mbBreakable;

	tString msDamageMesh[2];
	tString msBrokenEntity;

	tString msDamageSound;
	tString msDamagePS;

	tString msBreakSound;
	tString msBreakPS;
	float mfBreakImpulse;

	float mfHealthDamage[2];

	cMeshEntity* mpDamageMeshEntity[3];

	//////////////////////
	// Variables
	bool mbLocked;
	bool mbClosed;

	int mlBrokenEntityID;

	bool mbDisableAutoClose;

	int mlCurrentMeshEntity;
	bool mbBroken;

	float mfInteractSoundCount;


};

//----------------------------------------------

class cLuxPropLoader_SwingDoor : public iLuxPropLoader
{
public:
	cLuxPropLoader_SwingDoor(const tString& asName);
	virtual ~cLuxPropLoader_SwingDoor(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_SWING_DOOR_H
