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

#ifndef LUX_AREA_STICKY_H
#define LUX_AREA_STICKY_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Sticky_SaveData : public iLuxArea_SaveData
{
	kSerializableClassInit(cLuxArea_Sticky_SaveData)
public:
	iLuxArea* CreateArea(cLuxMap *apMap);
	
	tString msAttachFunction;
	tString msDetachFunction;

	tString msAttachSound;
	tString msDetachSound;

	tString msAttachPS;
	tString msDetachPS;

	bool mbCanDetach;

	float mfPoseTime;

	bool mbCheckCenterInArea;

	tString msAttachableBodyName;

	int mlAttachedEntityID;
	int mlAttachedBodyID;
	
	float mfAttachedBodyMass;
	bool mbAttachedBodyGravity;
	bool mbAttachedEntityFullGameSaved;

	float mfSetMtxTime;
};

//----------------------------------------------

class cLuxArea_Sticky : public iLuxArea
{
typedef iLuxArea super_class;
friend class cLuxAreaLoader_Sticky;
public:	
	cLuxArea_Sticky(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxArea_Sticky();

	//////////////////////
	//General
	void SetupAfterLoad(cWorld *apWorld);
	void OnUpdate(float afTimeStep);

	//////////////////////
	//Actions
	void AttachBody(iPhysicsBody *apBody);
	void DetachBody();

	//////////////////////
	//Properties
	static void SetAllowAttachment(bool abX){ mbAllowAttachment = abX;}

	iPhysicsBody *GetAttachedBody(){ return mpAttachedBody;}
	bool CanDetach(){ return mbCanDetach && mfSetMtxTime>=1;}

	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}

	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
	void UpdateAttachBody(float afTimeStep);
	void UpdateCollision(float afTimeStep);

	
	tString GetCallbackFunc(const tString &asFunc,iPhysicsBody *apBody);
	
	/////////////////////////
	// Data
	tString msAttachFunction;
	tString msDetachFunction;

	tString msAttachSound;
	tString msDetachSound;

	tString msAttachPS;
	tString msDetachPS;

	tString msAttachableBodyName;

	bool mbMoveBody;
	bool mbRotateBody;

	bool mbCheckCenterInArea;

	bool mbCanDetach;

	float mfPoseTime;

	/////////////////////////
	// Variables
	bool mbAttachedBodyGravity;
	float mfAttachedBodyMass;

	bool mbAttachedEntityFullGameSaved;

	iPhysicsBody *mpAttachedBody;
	iPhysicsBody *mpLastAttachedBody;

	float mfSetMtxTime;
	cMatrixf mtxAttachedStart;

	static bool mbAllowAttachment;
};

//----------------------------------------------

class cLuxAreaLoader_Sticky : public iLuxAreaLoader
{
public:
	cLuxAreaLoader_Sticky(const tString& asName);
	~cLuxAreaLoader_Sticky();

	iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);
	
	void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
	void SetupArea(iLuxArea *apArea, cWorld *apWorld);
	
};

//----------------------------------------------


#endif // LUX_AREA_STICKY_H
