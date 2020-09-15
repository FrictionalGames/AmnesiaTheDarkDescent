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

#include "EntityWrapperJoint.h"

#include "EntityWrapperBody.h"

#include "EditorWindowEntityEditBoxJoint.h"

#include "EditorWorld.h"

#include "EntityWrapperJointBall.h"
#include "EntityWrapperJointHinge.h"
#include "EntityWrapperJointScrew.h"
#include "EntityWrapperJointSlider.h"

#include "EntityIcon.h"

//-------------------------------------------------------------------

iEntityWrapperTypeJoint::iEntityWrapperTypeJoint(eEditorEntityJointType aSubType, const tString& asElementName) : iEntityWrapperType(eEditorEntityType_Joint, _W("Joint"), asElementName)
{
	mJointType = aSubType;

	AddInt(eJointInt_LimitAutoSleepNumSteps, "LimitStepCount", 0);
	AddInt(eJointInt_ParentID, "ConnectedParentBodyID", -1, ePropCopyStep_PostDeployAll);
	AddInt(eJointInt_ChildID, "ConnectedChildBodyID", -1, ePropCopyStep_PostDeployAll);

	AddFloat(eJointFloat_Stiffness, "Stiffness");

	AddFloat(eJointFloat_MinMoveSpeed, "MinMoveSpeed", 0.1f);
	AddFloat(eJointFloat_MinMoveFreq, "MinMoveFreq", 0.95f);
	AddFloat(eJointFloat_MinMoveFreqSpeed, "MinMoveFreqSpeed", 0.2f);
	AddFloat(eJointFloat_MinMoveVolume, "MinMoveVolume", 0.01f);

	AddFloat(eJointFloat_MaxMoveSpeed, "MaxMoveSpeed", 2);
	AddFloat(eJointFloat_MaxMoveFreq, "MaxMoveFreq", 1.1f);
	AddFloat(eJointFloat_MaxMoveFreqSpeed, "MaxMoveFreqSpeed", 0.7f);
	AddFloat(eJointFloat_MaxMoveVolume, "MaxMoveVolume", 0.8f);
	
	AddFloat(eJointFloat_MiddleMoveSpeed, "MiddleMoveSpeed", 0.5f);
	AddFloat(eJointFloat_MiddleMoveVolume, "MiddleMoveVolume", 0.5f);
	
	AddString(eJointStr_MinLimitSound, "MinLimitSound");
	AddFloat(eJointFloat_MinLimitMinSpeed, "MinLimitMinSpeed");
	AddFloat(eJointFloat_MinLimitMaxSpeed, "MinLimitMaxSpeed");

	AddString(eJointStr_MaxLimitSound, "MaxLimitSound");
	AddFloat(eJointFloat_MaxLimitMinSpeed, "MaxLimitMinSpeed");
	AddFloat(eJointFloat_MaxLimitMaxSpeed, "MaxLimitMaxSpeed");

	AddBool(eJointBool_Breakable, "Breakable", false);
	AddFloat(eJointFloat_BreakForce, "BreakForce");
	AddString(eJointStr_BreakSound, "BreakSound");

	AddBool(eJointBool_CollideBodies, "CollideBodies");
	AddBool(eJointBool_StickyMinLimit, "StickyMinLimit", false);
	AddBool(eJointBool_StickyMaxLimit, "StickyMaxLimit", false);

	AddString(eJointStr_MoveSound, "MoveSound");
	AddString(eJointStr_MoveType, "MoveType", "Linear");

	mvAttachableTypes.push_back(eEditorEntityType_Body);
}

//-------------------------------------------------------------------

iEntityWrapperDataJoint::iEntityWrapperDataJoint(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
	/*mJointType = aSubType;

	mbStickyMinLimit = false;
	mbStickyMaxLimit = false;

	mfStiffness = 0;

	mlLimitAutoSleepNumSteps = 0;

	msMoveSound="";
	msMoveType="Linear";

	mfMinMoveSpeed = 0.1f;
	mfMinMoveFreq = 0.95f;
	mfMinMoveVolume = 0.01f;
	mfMinMoveFreqSpeed = 0.2f;
	
	mfMaxMoveSpeed = 2;
	mfMaxMoveFreq = 1.1f;
	mfMaxMoveVolume = 0.8f;
	mfMaxMoveFreqSpeed = 0.7f;
	
	mfMiddleMoveSpeed = 0.5f;
	mfMiddleMoveVolume = 0.5f;

	msMinLimitSound="";
	mfMinLimitMinSpeed=0;
	mfMinLimitMaxSpeed=0;

	msMaxLimitSound="";
	mfMaxLimitMinSpeed=0;
	mfMaxLimitMaxSpeed=0;

	mbBreakable = false;
	mfBreakForce = 0;
	msBreakSound = "";

	mbCollideBodies = true;

	mlParentBodyID = -1;
	mlChildBodyID = -1;
	*/
}

//-------------------------------------------------------------------

bool iEntityWrapperDataJoint::SaveSpecific(cXmlElement* apElement)
{
	if(iEntityWrapperData::SaveSpecific(apElement)==false)
		return false;

	cMatrixf mtxOrientation = cMath::MatrixRotate(GetVec3f(eObjVec3f_Rotation), eEulerRotationOrder_XYZ).GetTranspose();

	apElement->SetAttributeVector3f("PinDir", mtxOrientation.GetUp());

	return true;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

iEntityWrapperJoint::iEntityWrapperJoint(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	mpParentBody = NULL;
	mpChildBody = NULL;

	mbStickyMinLimit = false;
	mbStickyMaxLimit = false;

	mfStiffness = 0;

	mlLimitAutoSleepNumSteps = 0;

	msMoveSound="";

	mfMinMoveSpeed = 0.1f;
	mfMinMoveFreq = 0.95f;
	mfMinMoveVolume = 0.01f;
	mfMinMoveFreqSpeed = 0.2f;
	
	mfMaxMoveSpeed = 2;
	mfMaxMoveFreq = 1.1f;
	mfMaxMoveVolume = 0.8f;
	mfMaxMoveFreqSpeed = 0.7f;
	
	mfMiddleMoveSpeed = 0.5f;
	mfMiddleMoveVolume = 0.5f;

	msMinLimitSound="";
	mfMinLimitMinSpeed=0;
	mfMinLimitMaxSpeed=0;

	msMaxLimitSound="";
	mfMaxLimitMinSpeed=0;
	mfMaxLimitMaxSpeed=0;

	mbBreakable = false;
	mfBreakForce = 0;
	msBreakSound = "";

	mbCollideBodies = true;
}

//-------------------------------------------------------------------

iEntityWrapperJoint::~iEntityWrapperJoint()
{
	if(GetEditorWorld()->IsClearingEntities()==false)
	{
		SetParentBody(NULL);
		SetChildBody(NULL);
	}
}

//-------------------------------------------------------------------

int iEntityWrapperJoint::GetJointType()
{
	return ((iEntityWrapperTypeJoint*)mpType)->GetJointType();
}


//-------------------------------------------------------------------

bool iEntityWrapperJoint::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eJointInt_LimitAutoSleepNumSteps:
		alX = GetLimitStepCount();
		break;
	case eJointInt_ParentID:
		alX = GetParentBodyID();
		break;
	case eJointInt_ChildID:
		alX = GetChildBodyID();
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, alX);
	}

	return true;
}

bool iEntityWrapperJoint::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointFloat_Stiffness:
		afX = GetStiffness();			break;
	case eJointFloat_MinMoveSpeed:
		afX = GetMinMoveSpeed();		break;
	case eJointFloat_MinMoveFreq:
		afX = GetMinMoveFreq();		break;
	case eJointFloat_MinMoveFreqSpeed:
		afX = GetMinMoveFreqSpeed();	break;
	case eJointFloat_MinMoveVolume:
		afX = GetMinMoveVolume();		break;
	case eJointFloat_MaxMoveSpeed:
		afX = GetMaxMoveSpeed();		break;
	case eJointFloat_MaxMoveFreq:
		afX = GetMaxMoveFreq();		break;
	case eJointFloat_MaxMoveFreqSpeed:
		afX = GetMaxMoveFreqSpeed();	break;
	case eJointFloat_MaxMoveVolume:
		afX = GetMaxMoveVolume();		break;
	case eJointFloat_MiddleMoveSpeed:
		afX = GetMiddleMoveSpeed();	break;
	case eJointFloat_MiddleMoveVolume:
		afX = GetMiddleMoveVolume();	break;
	case eJointFloat_MinLimitMinSpeed:
		afX = GetMinLimitMinSpeed();	break;
	case eJointFloat_MinLimitMaxSpeed:
		afX = GetMinLimitMaxSpeed();	break;
	case eJointFloat_MaxLimitMinSpeed:
		afX = GetMaxLimitMinSpeed();	break;
	case eJointFloat_MaxLimitMaxSpeed:
		afX = GetMaxLimitMaxSpeed();	break;
	case eJointFloat_BreakForce:
		afX = GetBreakForce(); break;
	default:
		return iEntityWrapper::GetProperty(alPropID, afX);
	}
	
	return true;
}

bool iEntityWrapperJoint::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eJointBool_CollideBodies:
		abX = GetCollideBodies();	break;
	case eJointBool_StickyMinLimit:
		abX = GetStickyMinLimit();	break;
	case eJointBool_StickyMaxLimit:
		abX = GetStickyMaxLimit(); break;
	case eJointBool_Breakable:
		abX = IsBreakable();		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, abX);
	}

	return true;
}

bool iEntityWrapperJoint::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eJointStr_MoveSound:
		asX = GetMoveSound();	break;
	case eJointStr_MoveType:
		asX = GetMoveType();	break;
	case eJointStr_MinLimitSound:
		asX = GetMinLimitSound(); break;
	case eJointStr_MaxLimitSound:
		asX = GetMaxLimitSound();	break;
	case eJointStr_BreakSound:
		asX = GetBreakSound();		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, asX);
	}
	
	return true;
}

//-------------------------------------------------------------------

bool iEntityWrapperJoint::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case eJointInt_LimitAutoSleepNumSteps:
		SetLimitStepCount(alX);
		break;
	case eJointInt_ParentID:
		SetParentBodyByID(alX);
		break;
	case eJointInt_ChildID:
		SetChildBodyByID(alX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, alX);
	}

	return true;
}

bool iEntityWrapperJoint::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eJointFloat_Stiffness:
		SetStiffness(afX);			break;
	case eJointFloat_MinMoveSpeed:
		SetMinMoveSpeed(afX);		break;
	case eJointFloat_MinMoveFreq:
		SetMinMoveFreq(afX);		break;
	case eJointFloat_MinMoveFreqSpeed:
		SetMinMoveFreqSpeed(afX);	break;
	case eJointFloat_MinMoveVolume:
		SetMinMoveVolume(afX);		break;
	case eJointFloat_MaxMoveSpeed:
		SetMaxMoveSpeed(afX);		break;
	case eJointFloat_MaxMoveFreq:
		SetMaxMoveFreq(afX);		break;
	case eJointFloat_MaxMoveFreqSpeed:
		SetMaxMoveFreqSpeed(afX);	break;
	case eJointFloat_MaxMoveVolume:
		SetMaxMoveVolume(afX);		break;
	case eJointFloat_MiddleMoveSpeed:
		SetMiddleMoveSpeed(afX);	break;
	case eJointFloat_MiddleMoveVolume:
		SetMiddleMoveVolume(afX);	break;
	case eJointFloat_MinLimitMinSpeed:
		SetMinLimitMinSpeed(afX);	break;
	case eJointFloat_MinLimitMaxSpeed:
		SetMinLimitMaxSpeed(afX);	break;
	case eJointFloat_MaxLimitMinSpeed:
		SetMaxLimitMinSpeed(afX);	break;
	case eJointFloat_MaxLimitMaxSpeed:
		SetMaxLimitMaxSpeed(afX);	break;
	case eJointFloat_BreakForce:
		SetBreakForce(afX); break;
	default:
		return iEntityWrapper::SetProperty(alPropID, afX);
	}
	
	return true;
}

bool iEntityWrapperJoint::SetProperty(int alPropID, const bool& abX)
{
	switch(alPropID)
	{
	case eJointBool_CollideBodies:
		SetCollideBodies(abX);	break;
	case eJointBool_StickyMinLimit:
		SetStickyMinLimit(abX);	break;
	case eJointBool_StickyMaxLimit:
		SetStickyMaxLimit(abX); break;
	case eJointBool_Breakable:
		SetBreakable(abX);		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, abX);
	}

	return true;
}

bool iEntityWrapperJoint::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eJointStr_MoveSound:
		SetMoveSound(asX);	break;
	case eJointStr_MoveType:
		SetMoveType(asX);	break;
	case eJointStr_MinLimitSound:
		SetMinLimitSound(asX); break;
	case eJointStr_MaxLimitSound:
		SetMaxLimitSound(asX);	break;
	case eJointStr_BreakSound:
		SetBreakSound(asX);		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, asX);
	}
	
	return true;
}

//-------------------------------------------------------------------

void iEntityWrapperJoint::SetParentBody(cEntityWrapperBody* apBody, bool abRemoveFromOld)
{
	if(mpParentBody==apBody)
		return;

	if(mpParentBody && abRemoveFromOld)
		mpParentBody->RemoveJoint(this, false);

	mpParentBody = apBody;

	if(mpParentBody)
		mpParentBody->AddChildJoint(this);
}

void iEntityWrapperJoint::SetChildBody(cEntityWrapperBody* apBody, bool abRemoveFromOld)
{
	if(mpChildBody==apBody)
		return;

	if(mpChildBody && abRemoveFromOld)
		mpChildBody->RemoveJoint(this, false);

	mpChildBody = apBody;

	if(mpChildBody)
		mpChildBody->AddParentJoint(this);
}

//-------------------------------------------------------------------

void iEntityWrapperJoint::SetParentBodyByID(int alX)
{
	iEntityWrapper* pBody = GetEditorWorld()->GetEntity(alX);
	if(pBody==NULL || pBody->GetTypeID()==eEditorEntityType_Body)
		SetParentBody((cEntityWrapperBody*)pBody);
}

void iEntityWrapperJoint::SetChildBodyByID(int alX)
{
	iEntityWrapper* pBody = GetEditorWorld()->GetEntity(alX);
	if(pBody==NULL || pBody->GetTypeID()==eEditorEntityType_Body)
		SetChildBody((cEntityWrapperBody*)pBody);
}

int iEntityWrapperJoint::GetParentBodyID()
{
	int lID = -1;
	if(mpParentBody) lID = mpParentBody->GetID();
	
	return lID;
}

int iEntityWrapperJoint::GetChildBodyID()
{
	int lID = -1;
	if(mpChildBody) lID = mpChildBody->GetID();
	
	return lID;
}

//-------------------------------------------------------------------

cEditorWindowEntityEditBox* iEntityWrapperJoint::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pWindow = hplNew( cEditorWindowEntityEditBoxJoint,(apEditMode, this));

	return pWindow;
}

//------------------------------------------------------------------------------

void iEntityWrapperJoint::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	//DrawIcon(apViewport, apFunctions, apEditMode, abIsSelected, mvPosition);

	//cMatrixf mtxTrans = cMath::MatrixTranslate(mvPosition);
	//cMatrixf mtxRot = cMath::MatrixRotate(mvRotation,eEulerRotationOrder_XYZ);
	cMatrixf mtxTransRot = cMath::MatrixMul(mmtxTranslate,mmtxRotate);

	//apFunctions->SetMatrix(&mtxTransRot);

	DrawArrow(apViewport, apFunctions, mtxTransRot, 1, true, cVector2f(0.05f, 0.2f), cColor(1));

	apFunctions->SetDepthTest(false);
	apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, 0.01f, cColor(1,1,0,1));
	apFunctions->SetDepthTest(true);
	
	if(mpParentBody)
		apFunctions->GetLowLevelGfx()->DrawLine(mvPosition, mpParentBody->GetPosition(), cColor(0,1,0,1));
	if(mpChildBody)
		apFunctions->GetLowLevelGfx()->DrawLine(mvPosition, mpChildBody->GetPosition(), cColor(0,0,1,1));
}

//-------------------------------------------------------------------

cEntityIcon* iEntityWrapperJoint::CreateIcon()
{
	return hplNew(cEntityIcon,(this, mpType->GetXmlElementName()));
}

//-------------------------------------------------------------------
