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

#ifndef LUX_PROP_LAMP_H
#define LUX_PROP_LAMP_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Lamp_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_Lamp_SaveData)
public:
	bool mbLit;
	bool mbLightConnectionSetup;

	tString msConnectionLight;
	float mfConnectionLightAmount;
	bool mbConnectionLightUseOnColor;
	bool mbConnectionLightUseSpec;
};


//----------------------------------------------

class cLuxProp_Lamp;

class cLuxLampLightConnection_Lamp
{
public:
	cLuxProp_Lamp *mpLamp;
	float mfAmount;
	bool mbUseLightOnColor;
	bool mbUseLightSpec;
};

typedef std::list<cLuxLampLightConnection_Lamp*> tLuxLampLightConnection_LampList;
typedef tLuxLampLightConnection_LampList::iterator tLuxLampLightConnection_LampListIt;

class cLuxLampLightConnection
{
public:
	cLuxLampLightConnection(iLight *apLight);
	~cLuxLampLightConnection();

	void Update(float afTimeStep);
	
	iLight* GetLight(){ return mpLight;}
	void AddLamp(cLuxProp_Lamp *apLamp, float afAmount, bool abUseOnColor, bool abUseSpec);
	void RemoveLamp(cLuxProp_Lamp *apLamp);

private:
    iLight *mpLight;   
	tLuxLampLightConnection_LampList mlstLamps; 
};

//----------------------------------------------

class cLuxProp_Lamp : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Lamp;
friend class cLuxLampLightConnection;
public:	
	cLuxProp_Lamp(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_Lamp();

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
	

	//////////////////////
	//Properties
	cLuxInteractData_Grab* GetGrabData(){ return &mGrabData;}
	
	void SetLit(bool abX, bool abUseEffects);
	bool GetLit(){ return mbLit; }

	bool CanBeIgnitByPlayer();

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
	void SetupLampLightConnection();

	//Vars
	bool mbLit;

	bool mbLightConnectionSetup;

	cLuxLampLightConnection *mpLightConnection;
	tString msConnectionLight;
	float mfConnectionLightAmount;
	bool mbConnectionLightUseOnColor;
	bool mbConnectionLightUseSpec;

	
	//Data
	bool mbCanBeLitByPlayer;
	bool mbCanBeGrabbed;

	
	cLuxInteractData_Grab mGrabData;
};

//----------------------------------------------

class cLuxPropLoader_Lamp : public iLuxPropLoader
{
public:
	cLuxPropLoader_Lamp(const tString& asName);
	virtual ~cLuxPropLoader_Lamp(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_H
