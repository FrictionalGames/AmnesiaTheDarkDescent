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

#include "LuxProp_MoveObject.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxArea.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_MoveObject::cLuxPropLoader_MoveObject(const tString& asName) : iLuxPropLoader(asName)
{
	mbForceFullGameSave = true;
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_MoveObject::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_MoveObject, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_MoveObject::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_MoveObject  *pMoveObject = static_cast<cLuxProp_MoveObject*>(apProp);

	///////////////////////////
	// General
	pMoveObject->mMoveObjectType = ToMoveObjectType(GetVarString("MoveObjectType","Linear"));

	pMoveObject->mfOpenAmount = GetVarFloat("OpenAmount", 1.0f);
	pMoveObject->mMoveAxis = StringToAxis(GetVarString("MoveAxis", "y"));

	pMoveObject->mfOpenAcc = GetVarFloat("OpenAcc", 1.0f);
	pMoveObject->mfOpenSpeed = GetVarFloat("OpenSpeed", 0.8f);

	pMoveObject->mfCloseAcc = GetVarFloat("CloseAcc", 9.0f);
	pMoveObject->mfCloseSpeed = GetVarFloat("CloseSpeed", 5.0f);

	pMoveObject->mbAutoMove = GetVarBool("AutoMove", false);
	pMoveObject->mfAutoMoveStateGoal = GetVarFloat("AutoMoveStateGoal", 0.0f);
	pMoveObject->mfAutoMoveAcc = GetVarFloat("AutoMoveAcc", 9.0f);
	pMoveObject->mfAutoMoveSpeed = GetVarFloat("AutoMoveSpeed", 4.0f);
	pMoveObject->mfAutoMoveSlowdownDist = GetVarFloat("AutoMoveSlowdownDist", 0.1f);
}

//-----------------------------------------------------------------------

eLuxMoveObjectType cLuxPropLoader_MoveObject::ToMoveObjectType(const tString& asType)
{
	tString sLowType = cString::ToLowerCase(asType);

    if(sLowType == "linear")	return eLuxMoveObjectType_Linear;
	if(sLowType == "angular")	return eLuxMoveObjectType_Angular;

	Error("MoveObjectType '%s' does not exist!\n", asType.c_str());

	return eLuxMoveObjectType_Linear;
}

//-----------------------------------------------------------------------

void cLuxPropLoader_MoveObject::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_MoveObject  *pMoveObject = static_cast<cLuxProp_MoveObject*>(apProp);
	
	//This varaible will only be used until the first update and then set to "" again.
	// so it is only temp and used to make sure that the area is loaded.
	pMoveObject->msAngularOffsetArea = apInstanceVars->GetVarString("AngularOffsetArea", "");
}
//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_MoveObject::cLuxProp_MoveObject(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_MoveObject)
{
	mvAngularOffsetPos = 0;
	mbUseAngularLocalOffset = false;
	mbAutoMoveReachedGoal = false;
}

//-----------------------------------------------------------------------

cLuxProp_MoveObject::~cLuxProp_MoveObject()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_MoveObject::CanInteract(iPhysicsBody *apBody)
{
	return msInteractCallback!="";
}

//-----------------------------------------------------------------------

bool cLuxProp_MoveObject::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return msInteractCallback!="";
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::OnSetupAfterLoad(cWorld *apWorld)
{
	///////////////////
	// Get closed and open transforms
	iPhysicsBody *pBody = GetMainBody();

	m_mtxClosedTransform = pBody->GetLocalMatrix();
	
	cVector3f vMul[3] = {cVector3f(1,0,0), cVector3f(0,1,0), cVector3f(0,0,1) };

	///////////////////
	// Linear
	if(mMoveObjectType == eLuxMoveObjectType_Linear)
	{
		cVector3f vLocalSize = pBody->GetBoundingVolume()->GetLocalMax() - pBody->GetBoundingVolume()->GetLocalMin();
		cVector3f vAdd = vLocalSize * vMul[mMoveAxis] * mfOpenAmount;

		m_mtxOpenTransform = m_mtxClosedTransform.GetRotation();
		m_mtxOpenTransform.SetTranslation(m_mtxClosedTransform.GetTranslation() + vAdd);
	}
	///////////////////
	// Angular
	else
	{
		CalculateOpenRotateMatrix();
	}
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::OnResetProperties()
{

}
//-----------------------------------------------------------------------

void cLuxProp_MoveObject::UpdatePropSpecific(float afTimeStep)
{
	//Log("Body pos: (%s) Size: (%s)\n", mvBodies[0]->GetWorldPosition().ToString().c_str(), mvBodies[0]->GetShape()->GetSize().ToString().c_str());

	//////////////////////
	//Check the anglular offset area
	if(msAngularOffsetArea != "")
	{
		iLuxArea *pArea = static_cast<iLuxArea*>(mpMap->GetEntityByName(msAngularOffsetArea));
		if(pArea)
		{
			SetAngularOffsetPos(pArea->GetPosition());
		}
		else
		{
			Error("Angular offset area '%s' does not exist!\n", msAngularOffsetArea.c_str());
		}
		msAngularOffsetArea = "";
	}

	
	//////////////////////
	//Update Automove
	UpdateAutoMove(afTimeStep);
	
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_MoveObject::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(msInteractCallback != "")
		return eLuxFocusCrosshair_Grab;
	else
		return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::MoveToState(float afState, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed)
{
	///////////////////
	// Linear
	if(mMoveObjectType == eLuxMoveObjectType_Linear)
	{
		cVector3f vWanted = m_mtxClosedTransform.GetTranslation() * (1-afState) + 
							m_mtxOpenTransform.GetTranslation() * afState;
		
		MoveLinearTo(vWanted, afAcc, afMaxSpeed,afSlowdownDist,abResetSpeed);
	}
	///////////////////
	// Angular
	else
	{
		//afState = cMath::Clamp(afState,0,1);	
		
		cMatrixf mtxWanted = cMath::MatrixSlerp(afState, m_mtxClosedTransform,m_mtxOpenTransform, true);

		cMatrixf mtxInvClose = cMath::MatrixInverse(m_mtxClosedTransform);
		cVector3f vLocalOffset = cMath::MatrixMul(mtxInvClose, mvAngularOffsetPos);

		if(mbUseAngularLocalOffset)
			MoveAngularTo(mtxWanted, afAcc, afMaxSpeed,afSlowdownDist,abResetSpeed, true, mvAngularOffsetPos ,vLocalOffset);
		else
			MoveAngularTo(mtxWanted, afAcc, afMaxSpeed,afSlowdownDist,abResetSpeed, false, 0 ,0);
	}
}

void cLuxProp_MoveObject::MoveToState(float afState)
{
	float fCurrent = GetMoveState();
	//if(fCurrent == afState) return;z

	if(afState < fCurrent)
		MoveToState(afState, mfCloseAcc, mfCloseSpeed, 0.3f, true);
	else
		MoveToState(afState, mfOpenAcc, mfOpenSpeed, 0.3f, true);
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::SetAngularOffsetPos(const cVector3f& avWorldPos)
{
	mvAngularOffsetPos = avWorldPos;
	mbUseAngularLocalOffset = true;

	if(mMoveObjectType == eLuxMoveObjectType_Angular)
	{
		CalculateOpenRotateMatrix();
	}
}

//-----------------------------------------------------------------------

float cLuxProp_MoveObject::GetMoveState()
{
	cVector3f vCurrent, vClosed, vOpen, vDelta, vT;

	///////////////////
	// Linear
	if(mMoveObjectType == eLuxMoveObjectType_Linear)
	{
		vCurrent = GetMainBody()->GetLocalPosition();
		vClosed = m_mtxClosedTransform.GetTranslation();
		vOpen = m_mtxOpenTransform.GetTranslation();

		vDelta = vCurrent - vClosed;
		vT = vDelta / (vOpen - vClosed);
	}
	///////////////////
	// Angular
	else
	{
		vDelta = cMath::MatrixEulerAngleDistance(m_mtxClosedTransform, GetMainBody()->GetLocalMatrix());
		vT =	vDelta / cMath::MatrixEulerAngleDistance(m_mtxClosedTransform, m_mtxOpenTransform);
	}

	float fT = 0; 
	if(vDelta.x != 0)		fT = vT.x;
	else if(vDelta.y != 0)	fT = vT.y;
	else if(vDelta.z != 0)	fT = vT.z;
	
	return fT;
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	if(alState > 0)	MoveToState(1.0f);
	if(alState < 0)	MoveToState(0.0f);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::UpdateAutoMove(float afTimeStep)
{
	///////////////////////
	//Skip update if update is off, the entity is moving or the goal is reached.
	if(mbAutoMove==false || mbMoving || mbAutoMoveReachedGoal) return;
	
	///////////////////////
	//Check if the auto move goal has been reached
	if(cMath::Abs(GetMoveState() - mfAutoMoveStateGoal) < 0.001f)
	{
		mbAutoMoveReachedGoal = true;
	}
	///////////////////////
	//Goal not yet reached, do some
	else
	{
		MoveToState(mfAutoMoveStateGoal, mfAutoMoveAcc, mfAutoMoveSpeed, mfAutoMoveSlowdownDist, true);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::OnStartMove()
{
	mbAutoMoveReachedGoal = false;
}

void cLuxProp_MoveObject::CalculateOpenRotateMatrix()
{
	cVector3f vMul[3] = {cVector3f(1,0,0), cVector3f(0,1,0), cVector3f(0,0,1) };

	m_mtxOpenTransform = m_mtxClosedTransform.GetRotation();

	cMatrixf mtxRot = cMath::MatrixRotate(vMul[mMoveAxis] * kPi2f * mfOpenAmount,eEulerRotationOrder_XYZ);
	m_mtxOpenTransform = cMath::MatrixMul(m_mtxOpenTransform, mtxRot);

	m_mtxOpenTransform.SetTranslation(m_mtxClosedTransform.GetTranslation());
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_MoveObject_SaveData, iLuxProp_SaveData)

kSerializeVar(mvAngularOffsetPos, eSerializeType_Vector3f)
kSerializeVar(mbUseAngularLocalOffset, eSerializeType_Bool)

kSerializeVar(mbAutoMoveReachedGoal, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_MoveObject::CreateSaveData()
{
	return hplNew(cLuxProp_MoveObject_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_MoveObject_SaveData *pData = static_cast<cLuxProp_MoveObject_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, mvAngularOffsetPos);
	kCopyToVar(pData, mbUseAngularLocalOffset);
	kCopyToVar(pData, mbAutoMoveReachedGoal);
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_MoveObject_SaveData *pData = static_cast<cLuxProp_MoveObject_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, mvAngularOffsetPos);
	kCopyFromVar(pData, mbUseAngularLocalOffset);
	kCopyFromVar(pData, mbAutoMoveReachedGoal);

	SetAngularOffsetPos(mvAngularOffsetPos);
}

//-----------------------------------------------------------------------

void cLuxProp_MoveObject::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
