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

#ifndef LUX_PROP_CHEST_H
#define LUX_PROP_CHEST_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Chest_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_Chest_SaveData)
public:
	bool mbLocked;
	
	int mlCoinsNeeded;
};

//----------------------------------------------

class cLuxChestJointData
{
public:
	iPhysicsJointHinge *mpHingeJoint;
	iPhysicsBody *mpChildBody;

    float mfMaxAngle;
};


//----------------------------------------------

class cLuxProp_Chest;

class cLuxChestMessageCallback : public iLuxMessageCallback
{
public:
	cLuxChestMessageCallback(cLuxProp_Chest *apChest);

	void OnPress(bool abYes);

private:
	cLuxProp_Chest *mpChest;
};

//----------------------------------------------


class cLuxProp_Chest : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Chest;
friend class cLuxChestMessageCallback;
public:	
	cLuxProp_Chest(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_Chest();

	//////////////////////
	//General
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);
	tWString GetFocusText();

	void ImplementedOnSetActive(bool abX);

	void OnDamage(float afAmount, int alStrength);

	void InFocusDraw(cGuiSet *apGuiSet,float afFrameTime);

	//////////////////////
	//Properties
	iLuxInteractData_RotateBase* GetMoveBaseData(){ return &mSwingDoorData;}
	
	void SetLocked(bool abLocked, bool abEffects);
	bool GetLocked(){ return mbLocked;}
	
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
	cLuxChestJointData* GetJointDataFromBody(iPhysicsBody *apBody);
	cLuxChestJointData* GetJointDataFromJoint(iPhysicsJoint *apJoint);

	//////////////////////
	// Data
	std::vector<cLuxChestJointData> mvJointData;

	cLuxInteractData_SwingDoor mSwingDoorData;

	cLuxChestMessageCallback *mpMessageCallback;

	//////////////////////
	// Variables
	bool mbLocked;
	
	int mlCoinsNeeded;

};

//----------------------------------------------

class cLuxPropLoader_Chest : public iLuxPropLoader
{
public:
	cLuxPropLoader_Chest(const tString& asName);
	virtual ~cLuxPropLoader_Chest(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_CHEST_H
