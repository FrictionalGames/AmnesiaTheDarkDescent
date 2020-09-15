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

#include "LuxInteractConnections.h"

#include "LuxProp_Wheel.h"
#include "LuxProp_SwingDoor.h"
#include "LuxProp_MoveObject.h"
#include "LuxMap.h"


//////////////////////////////////////////////////////////////////////////
// BASE INTERFACE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxInteractConnection_SaveData::FromConnection(iLuxInteractConnection* apConnection)
{
	kCopyFromVar(apConnection, msName);
	kCopyFromVar(apConnection, mbInteractionOnly);
	kCopyFromVar(apConnection, mbInvert);
	kCopyFromVar(apConnection, mlStateUsed);

	mlPropId = apConnection->mpProp->GetID();
}

//-----------------------------------------------------------------------

kBeginSerializeBaseVirtual(iLuxInteractConnection_SaveData)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(mbInteractionOnly,eSerializeType_Bool)
kSerializeVar(mlPropId,eSerializeType_Int32)
kSerializeVar(mbInvert,eSerializeType_Bool)
kSerializeVar(mlStateUsed,eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ROPE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInteractConnection_Rope::cLuxInteractConnection_Rope(const tString& asName, iLuxProp *apProp,  iPhysicsRope *apRope, float afSpeedMul, 
														float afMinSpeed, float afMaxSpeed,bool abInvert, int alStatesUsed)
	: iLuxInteractConnection(asName,apProp, abInvert, alStatesUsed)
{
	mpRope = apRope;

	mfSpeedMul = afSpeedMul;
	mfMinSpeed = afMinSpeed;
	mfMaxSpeed = afMaxSpeed;
}

cLuxInteractConnection_Rope::~cLuxInteractConnection_Rope()
{

}

//-----------------------------------------------------------------------

void cLuxInteractConnection_Rope::Update(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

void cLuxInteractConnection_Rope::UpdateProp(float afTimeStep)
{
	if(mpProp->IsInteractedWith()) return;
	if(mbInteractionOnly==false) return;

	float fMin = mpRope->GetMinTotalLength();
	float fMax = mpRope->GetMaxTotalLength();
	float fLength = mpRope->GetTotalLength();

	float fT = (fLength-fMin) / (fMax - fMin);

	//Invert if var is set
	if(mbInvert) fT = 1 - fT;

	////////////////////////
	// Update wheel
	if(mpProp->GetPropType() == eLuxPropType_Wheel)
	{
		cLuxProp_Wheel *pWheel = static_cast<cLuxProp_Wheel*>(mpProp);
		
		float fAngle = pWheel->GetMinLimit() + fT * (pWheel->GetMaxLimit() - pWheel->GetMinLimit());
		
		pWheel->SetAngle(fAngle, true);
	}

}

//-----------------------------------------------------------------------

void cLuxInteractConnection_Rope::OnTurn(float afAngleAdd, float afT)
{
	//Invert if var is set
	if(mbInvert) afT = 1 - afT;

	float fMin = mpRope->GetMinTotalLength();
	float fMax = mpRope->GetMaxTotalLength();

	float fWantedLength = fMin + (fMax-fMin)*afT;

	mpRope->SetMotorActive(true);
	mpRope->SetMotorWantedLength(fWantedLength);
	mpRope->SetMotorSpeedMul(mfSpeedMul);
	mpRope->SetMotorMinSpeed(mfMinSpeed);
	mpRope->SetMotorMaxSpeed(mfMaxSpeed);
}

//-----------------------------------------------------------------------

void cLuxInteractConnection_Rope::OnLimit(int alState)
{
	if(mpProp->GetPropType() == eLuxPropType_Wheel) return;
	if(alState==0) return;
	//Check if this state is used.
	if(mlStateUsed != 0 && mlStateUsed != alState) return;

	//Invert state if var is set.
	if(mbInvert) alState = -alState;

	float fWantedLength =0;
	if(alState == 1)
	{
		fWantedLength = mpRope->GetMaxTotalLength();	
	}
	else if(alState == -1)
	{
		fWantedLength = mpRope->GetMinTotalLength();	
	}

	mpRope->SetMotorActive(true);
	mpRope->SetMotorWantedLength(fWantedLength);
	mpRope->SetMotorSpeedMul(mfSpeedMul);
	mpRope->SetMotorMinSpeed(mfMinSpeed);
	mpRope->SetMotorMaxSpeed(mfMaxSpeed);
}

//-----------------------------------------------------------------------

iLuxInteractConnection_SaveData* cLuxInteractConnection_Rope::CreateSaveData()
{
	return hplNew(cLuxInteractConnection_Rope_SaveData, ());
}

//-----------------------------------------------------------------------

iLuxInteractConnection* cLuxInteractConnection_Rope_SaveData::CreateConnection(cLuxMap *apMap)
{
	//Get the rope!
	iPhysicsRope *pRope = apMap->GetPhysicsWorld()->GetRopeFromUniqueID(mlRopeId);
	if(pRope==NULL){
		Error("Rope with ID %d was not found when creating saved connection '%s'!\n", mlRopeId, msName.c_str());
		return NULL;
	}

	//Get the prop
	iLuxProp *pProp = static_cast<iLuxProp*>(apMap->GetEntityByID(mlPropId, eLuxEntityType_Prop));
	if(pProp==NULL){
		Error("Prop with ID %d was not found when creating saved connection '%s'!\n", mlPropId, msName.c_str());
		return NULL;
	}

    cLuxInteractConnection_Rope *pConnection = hplNew( cLuxInteractConnection_Rope,(msName, pProp, pRope, mfSpeedMul, mfMinSpeed, mfMaxSpeed, mbInvert, mlStateUsed) );
	pConnection->SetInteractionOnly(mbInteractionOnly);
	return pConnection;
}

//-----------------------------------------------------------------------

void cLuxInteractConnection_Rope_SaveData::FromConnection(iLuxInteractConnection* apConnection)
{
	iLuxInteractConnection_SaveData::FromConnection(apConnection);
	cLuxInteractConnection_Rope *pData = static_cast<cLuxInteractConnection_Rope*>(apConnection);
	
	mlRopeId = pData->mpRope->GetUniqueID();

	kCopyFromVar(pData, mfSpeedMul);
	kCopyFromVar(pData, mfMinSpeed);
	kCopyFromVar(pData, mfMaxSpeed);
}

//-----------------------------------------------------------------------

kBeginSerialize(cLuxInteractConnection_Rope_SaveData, iLuxInteractConnection_SaveData)
kSerializeVar(mlRopeId, eSerializeType_Int32)
kSerializeVar(mfSpeedMul, eSerializeType_Float32)
kSerializeVar(mfMinSpeed, eSerializeType_Float32)
kSerializeVar(mfMaxSpeed, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// MOVE OBJECT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInteractConnection_MoveObject::cLuxInteractConnection_MoveObject(const tString& asName, iLuxProp *apProp, cLuxProp_MoveObject *apMoveObject,
																	bool abInvert,  int alStatesUsed)
: iLuxInteractConnection(asName,apProp, abInvert, alStatesUsed)
{
	mpMoveObject = apMoveObject;
	
	mbPropNeedsUpdate = true; //Used to make sure that the prop gets update at the start.
}

//-----------------------------------------------------------------------

cLuxInteractConnection_MoveObject::~cLuxInteractConnection_MoveObject()
{

}

//-----------------------------------------------------------------------

void cLuxInteractConnection_MoveObject::Update(float afTimeStep)
{

}

//-----------------------------------------------------------------------

void cLuxInteractConnection_MoveObject::UpdateProp(float afTimeStep)
{
	if(	mbInteractionOnly==false || 
		(mpMoveObject->IsMoving()==false && mbPropNeedsUpdate==false))
	{
		return;
	}

	mbPropNeedsUpdate = false;

	if(mpProp->GetPropType() != eLuxPropType_Wheel) return;

	float fT = mpMoveObject->GetMoveState();
	if(mbInvert) fT = 1 - fT;
	
	////////////////////////
	// Update wheel
	cLuxProp_Wheel *pWheel = static_cast<cLuxProp_Wheel*>(mpProp);
	float fAngle = pWheel->GetMinLimit() + fT * (pWheel->GetMaxLimit() - pWheel->GetMinLimit());
	
	if(mpProp->IsInteractedWith())
	{
		float fLimitAdd = (pWheel->GetMaxLimit() - pWheel->GetMinLimit()) * 0.2f;
		float fMinLimit = fAngle - fLimitAdd;
		float fMaxLimit = fAngle + fLimitAdd;

		//TODO: Can this be fixed? Right now it messes up stuff? Is there another
		//Way to stop the wheel when the connected moveobject has collided and is stuck?
		//Log("Adjusting: Angle: %f, Min: %f Max: %f\n", pWheel->GetAngle(), fMinLimit, fMaxLimit);
		//if(pWheel->GetAngle() < fMinLimit) pWheel->SetAngle(fMinLimit, true);
		//if(pWheel->GetAngle() > fMaxLimit) pWheel->SetAngle(fMaxLimit, true);
	}
	else
	{
		pWheel->SetAngle(fAngle, true);
	}
}

//-----------------------------------------------------------------------


void cLuxInteractConnection_MoveObject::OnTurn(float afAngleAdd, float afT)
{
	if(mbInvert) afT = 1 - afT;

	mpMoveObject->MoveToState(afT, 10.0f, 5, 0.01f, false);
}

//-----------------------------------------------------------------------

void cLuxInteractConnection_MoveObject::OnLimit(int alState)
{
	if(mpProp->GetPropType() == eLuxPropType_Wheel) return;

	if(alState==0) return;
	if(mlStateUsed != 0 && mlStateUsed != alState) return;

	if(mbInvert) alState = -alState;

	mpMoveObject->MoveToState(alState >0 ? 1.0f : 0.0f);
}

//-----------------------------------------------------------------------

iLuxInteractConnection_SaveData* cLuxInteractConnection_MoveObject::CreateSaveData()
{
	return hplNew(cLuxInteractConnection_MoveObject_SaveData, ());
}

//-----------------------------------------------------------------------

iLuxInteractConnection* cLuxInteractConnection_MoveObject_SaveData::CreateConnection(cLuxMap *apMap)
{
	//////////////////
	//Get the move object
	cLuxProp_MoveObject *pMoveObject = static_cast<cLuxProp_MoveObject*>(apMap->GetEntityByID(mlMoveObjectId,eLuxEntityType_Prop, eLuxPropType_MoveObject));
	if(pMoveObject==NULL)
	{
		Error("MoveObject with ID %d was not found when creating saved connection '%s'!\n", mlMoveObjectId, msName.c_str());
		return NULL;
	}
	
	//////////////////
	//Get the prop
	iLuxProp *pProp = static_cast<iLuxProp*>(apMap->GetEntityByID(mlPropId,eLuxEntityType_Prop));
	if(pProp==NULL){
		Error("Prop with ID %d was not found when creating saved connection '%s'!\n", mlPropId, msName.c_str());
		return NULL;
	}

	cLuxInteractConnection_MoveObject *pConn = hplNew( cLuxInteractConnection_MoveObject,(msName, pProp, pMoveObject, mbInvert, mlStateUsed) );
	pConn->SetInteractionOnly(mbInteractionOnly);
	return pConn;
	
	return pConn;
}

//-----------------------------------------------------------------------

void cLuxInteractConnection_MoveObject_SaveData::FromConnection(iLuxInteractConnection* apConnection)
{
	iLuxInteractConnection_SaveData::FromConnection(apConnection);
	cLuxInteractConnection_MoveObject *pData = static_cast<cLuxInteractConnection_MoveObject*>(apConnection);

	mlMoveObjectId = pData->mpMoveObject->GetID();
}

//-----------------------------------------------------------------------

kBeginSerialize(cLuxInteractConnection_MoveObject_SaveData, iLuxInteractConnection_SaveData)
kSerializeVar(mlMoveObjectId, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------
