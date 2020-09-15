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

#ifndef LUX_AREA_SIGN_H
#define LUX_AREA_SIGN_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Sign_SaveData : public iLuxArea_SaveData
{
	kSerializableClassInit(cLuxArea_Sign_SaveData)
public:
	iLuxArea* CreateArea(cLuxMap *apMap);

	tString msTextCat;
	tString msTextEntry;
};

//----------------------------------------------

class cLuxArea_Sign : public iLuxArea
{
typedef iLuxArea super_class;
friend class cLuxAreaLoader_Sign;
public:	
	cLuxArea_Sign(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxArea_Sign();

	//////////////////////
	//General
	void SetupAfterLoad(cWorld *apWorld);

	void OnUpdate(float afTimeStep);

	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);
	tWString GetFocusText();

	//////////////////////
	//Properties
	
	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}

	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);
protected:

private:
	cVector3f GetStartPosition();

	/////////////////////////
	// Data
	tString msTextCat;
	tString msTextEntry;
};

//----------------------------------------------

class cLuxAreaLoader_Sign : public iLuxAreaLoader
{
public:
	cLuxAreaLoader_Sign(const tString& asName);
	~cLuxAreaLoader_Sign();

	iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);
	
	void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
	void SetupArea(iLuxArea *apArea, cWorld *apWorld);
	
};

//----------------------------------------------


#endif // LUX_AREA_SIGN_H
