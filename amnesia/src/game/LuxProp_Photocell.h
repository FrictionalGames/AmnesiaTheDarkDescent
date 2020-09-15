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

#ifndef LUX_PROP_PHOTOCELL_H
#define LUX_PROP_PHOTOCELL_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Photocell_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_Photocell_SaveData)
public:
	bool mbLit;
	float mfLightLevel;
	float mfLightLevelCheckCount;
	float mfLightLevelOnLimit;
	float mfLightLevelOffLimit;
};

//----------------------------------------------

class cLuxProp_Photocell : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Photocell;
public:	
	cLuxProp_Photocell(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_Photocell();

	//////////////////////
	//General
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Properties
	
	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}
	
	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);


private:
	void UpdateLightLevel(float afTimeStep);

	//General
	

	//Vars
	bool mbLit;
	float mfLightLevel;
	float mfLightLevelCheckCount;
	
	float mfLightLevelOnLimit;
	float mfLightLevelOffLimit;
};

//----------------------------------------------

class cLuxPropLoader_Photocell : public iLuxPropLoader
{
public:
	cLuxPropLoader_Photocell(const tString& asName);
	virtual ~cLuxPropLoader_Photocell(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_H
