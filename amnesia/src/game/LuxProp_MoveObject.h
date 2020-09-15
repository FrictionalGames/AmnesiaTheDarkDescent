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

#ifndef LUX_PROP_MOVE_OBJECT_H
#define LUX_PROP_MOVE_OBJECT_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_MoveObject_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_MoveObject_SaveData)
public:
	cVector3f mvAngularOffsetPos;
	bool mbUseAngularLocalOffset;

	bool mbAutoMoveReachedGoal;
};

//----------------------------------------------

class cLuxProp_MoveObject : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_MoveObject;
public:	
	cLuxProp_MoveObject(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_MoveObject();

	//////////////////////
	//Genera
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Actions
	void MoveToState(float afState, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed);
	void MoveToState(float afState);

	//////////////////////
	//Properties
	void SetAngularOffsetPos(const cVector3f& avWorldPos);

	const cMatrixf& GetClosedTransform(){ return m_mtxClosedTransform;}
	const cMatrixf& GetOpenTransform(){ return m_mtxOpenTransform;}
	
	float GetMoveState();
		
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
	void UpdateAutoMove(float afTimeStep);

	void OnStartMove();
	void CalculateOpenRotateMatrix();
	
	//Vars
	cVector3f mvAngularOffsetPos;
	bool mbUseAngularLocalOffset;
	bool mbAutoMoveReachedGoal;

	tString msAngularOffsetArea;

	//Data
	eLuxMoveObjectType mMoveObjectType;

	float mfOpenAmount;
	eLuxAxis mMoveAxis;

	float mfOpenAcc;
	float mfOpenSpeed;
	
	float mfCloseAcc;
	float mfCloseSpeed;
	
	bool mbAutoMove;
	float mfAutoMoveStateGoal;
	float mfAutoMoveAcc;
	float mfAutoMoveSpeed;
	float mfAutoMoveSlowdownDist;

	cMatrixf m_mtxClosedTransform;
	cMatrixf m_mtxOpenTransform;
};

//----------------------------------------------

class cLuxPropLoader_MoveObject : public iLuxPropLoader
{
public:
	cLuxPropLoader_MoveObject(const tString& asName);
	virtual ~cLuxPropLoader_MoveObject(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
	eLuxMoveObjectType ToMoveObjectType(const tString& asType);
};

//----------------------------------------------


#endif // LUX_PROP_SWING_DOOR_H
