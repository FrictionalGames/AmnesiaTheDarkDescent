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

#ifndef LUX_INTERACT_CONNECTIONS_H
#define LUX_INTERACT_CONNECTIONS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEntity;
class iLuxProp;
class cLuxProp_SwingDoor;
class cLuxProp_MoveObject;
class cLuxMap;

//----------------------------------------------

class iLuxInteractConnection;

class iLuxInteractConnection_SaveData : public iSerializable
{
	kSerializableClassInit(iLuxInteractConnection_SaveData)
public:
	virtual iLuxInteractConnection* CreateConnection(cLuxMap *apMap)=0;
	virtual void FromConnection(iLuxInteractConnection* apConnection);

	tString msName;
	bool mbInteractionOnly;
	int mlPropId;
	bool mbInvert;
	int mlStateUsed;

protected:
	iLuxProp *GetProp(int alId, cLuxMap *apMap);
};

//----------------------------------------------

class iLuxInteractConnection
{
friend class iLuxInteractConnection_SaveData;
public:
	iLuxInteractConnection(const tString& asName, iLuxProp *apProp,bool abInvert, int alStatesUsed) 
		: msName(asName), mpProp(apProp), mbInteractionOnly(true), mbInvert(abInvert), mlStateUsed(alStatesUsed) {}
	virtual ~iLuxInteractConnection(){}

	///////////////
	//General
	virtual void Update(float afTimeStep)=0;
	virtual void UpdateProp(float afTimeStep)=0;

	///////////////
	//Callbacks
	virtual void OnTurn(float afAngleAdd, float afT)=0;
	virtual void OnLimit(int alState)=0;

	///////////////
	//Properties
	const tString& GetName(){ return msName; }

	void SetInteractionOnly(bool abX){ mbInteractionOnly = abX;}
	bool GetInteractionOnly(){ return mbInteractionOnly;}

	///////////////
	//Save
	virtual iLuxInteractConnection_SaveData* CreateSaveData()=0;
	
protected:
	tString msName;
	bool mbInteractionOnly;
	iLuxProp* mpProp;

	bool mbInvert;
	int mlStateUsed;
};

//----------------------------------------------

class cLuxInteractConnection_Rope_SaveData :  public iLuxInteractConnection_SaveData
{
	kSerializableClassInit(cLuxInteractConnection_Rope_SaveData)
public:
	iLuxInteractConnection* CreateConnection(cLuxMap *apMap);
	void FromConnection(iLuxInteractConnection* apConnection);

	int mlRopeId;
	float mfSpeedMul;
	float mfMinSpeed;
	float mfMaxSpeed;
};

//----------------------------------------------

class cLuxInteractConnection_Rope : public iLuxInteractConnection
{
friend class cLuxInteractConnection_Rope_SaveData;
public:
	cLuxInteractConnection_Rope(const tString& asName, iLuxProp *apProp,  iPhysicsRope *apRope, float afSpeedMul, float afMinSpeed, float afMaxSpeed,
								bool abInvert, int alStatesUsed);
	~cLuxInteractConnection_Rope();

	///////////////
	//General
	void Update(float afTimeStep);
	void UpdateProp(float afTimeStep);

	///////////////
	//Callbacks
	void OnTurn(float afAngleAdd, float afT);
	void OnLimit(int alState);


	///////////////
	//Script
	iLuxInteractConnection_SaveData* CreateSaveData();

private:
    iPhysicsRope *mpRope;

	float mfSpeedMul;
	float mfMinSpeed;
	float mfMaxSpeed;
};


//----------------------------------------------

class cLuxInteractConnection_MoveObject_SaveData :  public iLuxInteractConnection_SaveData
{
	kSerializableClassInit(cLuxInteractConnection_MoveObject_SaveData)
public:
	iLuxInteractConnection* CreateConnection(cLuxMap *apMap);
	void FromConnection(iLuxInteractConnection* apConnection);

	int mlMoveObjectId;
};
//----------------------------------------------

class cLuxInteractConnection_MoveObject : public iLuxInteractConnection
{
friend class cLuxInteractConnection_MoveObject_SaveData;
public:

	/**
	* lStatesUsed, 0=max and min, 1=max only, -1=min only.
	*/
	cLuxInteractConnection_MoveObject(const tString& asName, iLuxProp *apProp, cLuxProp_MoveObject *apMoveObject, bool abInvert, int alStatesUsed);
	~cLuxInteractConnection_MoveObject();

	///////////////
	//General
	void Update(float afTimeStep);
	void UpdateProp(float afTimeStep);

	///////////////
	//Callbacks
	void OnTurn(float afAngleAdd, float afT);
	void OnLimit(int alState);

	///////////////
	//Script
	iLuxInteractConnection_SaveData* CreateSaveData();

private:
	cLuxProp_MoveObject *mpMoveObject;
	bool mbPropNeedsUpdate;
};

//----------------------------------------------


#endif // LUX_INTERACT_CONNECTIONS_H
