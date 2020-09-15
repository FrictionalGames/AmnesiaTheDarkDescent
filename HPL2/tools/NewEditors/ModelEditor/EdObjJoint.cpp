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

#include "../ModelEditor/EdObjJoint.h"

#include "../ModelEditor/EdObjBody.h"

#include "../ModelEditor/EdEditPaneJoint.h"

#include "../Common/EdViewport.h"

#include "../ModelEditor/EdObjJointBall.h"
#include "../ModelEditor/EdObjJointHinge.h"
#include "../ModelEditor/EdObjJointScrew.h"
#include "../ModelEditor/EdObjJointSlider.h"

#include "../Common/EdObjectIcon.h"

//-------------------------------------------------------------------

cTypeJoint::cTypeJoint() : iEdScnObjType(_W("Joint"), "")
{
	AddInt(eJointInt_LimitAutoSleepNumSteps, "LimitStepCount", 0);
	AddInt(eJointInt_ParentID, "ConnectedParentBodyID", -1, ePropStep_PostDeployAll);
	AddInt(eJointInt_ChildID, "ConnectedChildBodyID", -1, ePropStep_PostDeployAll);

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

	AddAttachableType(_W("Body"));
}

//-------------------------------------------------------------------

bool cTypeJoint::Init()
{
	bool bSuccess = true;
	bSuccess = bSuccess && AddChildType(hplNew(cTypeJointBall,()));
	bSuccess = bSuccess && AddChildType(hplNew(cTypeJointHinge,()));
	bSuccess = bSuccess && AddChildType(hplNew(cTypeJointSlider,()));
	bSuccess = bSuccess && AddChildType(hplNew(cTypeJointScrew,()));

	return bSuccess;
}

//-------------------------------------------------------------------

iEdObjJointData::iEdObjJointData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//-------------------------------------------------------------------

/*
bool iEdObjJointData::SaveSpecific(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecific(apElement)==false)
		return false;

	cMatrixf mtxOrientation = cMath::MatrixRotate(GetVec3f(eScnVec3f_Rotation), eEulerRotationOrder_XYZ).GetTranspose();

	apElement->SetAttributeVector3f("PinDir", mtxOrientation.GetUp());

	return true;
}
*/

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

iEdObjJoint::iEdObjJoint(iEdObjectData* apData) : iEdScnObject(apData)
{
	mpParentBody = NULL;
	mpChildBody = NULL;
}

//-------------------------------------------------------------------

iEdObjJoint::~iEdObjJoint()
{
	if(GetWorld()->IsClearing()==false)
	{
		SetParentBody(NULL);
		SetChildBody(NULL);
	}
}

//-------------------------------------------------------------------

bool iEdObjJoint::GetProperty(int alPropID, int& alX)
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
		return iEdScnObject::GetProperty(alPropID, alX);
	}

	return true;
}

bool iEdObjJoint::GetProperty(int alPropID, float& afX)
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
		return iEdScnObject::GetProperty(alPropID, afX);
	}
	
	return true;
}

bool iEdObjJoint::GetProperty(int alPropID, bool& abX)
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
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool iEdObjJoint::GetProperty(int alPropID, tString& asX)
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
		return iEdScnObject::GetProperty(alPropID, asX);
	}
	
	return true;
}

//-------------------------------------------------------------------

bool iEdObjJoint::SetProperty(int alPropID, const int alX)
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
		return iEdScnObject::SetProperty(alPropID, alX);
	}

	return true;
}

bool iEdObjJoint::SetProperty(int alPropID, const float afX)
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
		return iEdScnObject::SetProperty(alPropID, afX);
	}
	
	return true;
}

bool iEdObjJoint::SetProperty(int alPropID, const bool abX)
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
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool iEdObjJoint::SetProperty(int alPropID, const tString& asX)
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
		return iEdScnObject::SetProperty(alPropID, asX);
	}
	
	return true;
}

//-------------------------------------------------------------------

void iEdObjJoint::SetParentBody(cEdObjBody* apBody, bool abRemoveFromOld)
{
	if(mpParentBody==apBody)
		return;

	if(mpParentBody && abRemoveFromOld)
		mpParentBody->RemoveJoint(this, false);

	mpParentBody = apBody;

	if(mpParentBody)
		mpParentBody->AddChildJoint(this);
}

void iEdObjJoint::SetChildBody(cEdObjBody* apBody, bool abRemoveFromOld)
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

void iEdObjJoint::SetParentBodyByID(int alX)
{
	iEdObject* pBody = GetWorld()->GetObject(alX);
	if(pBody==NULL || pBody->GetTypeName()==_W("Body"))
		SetParentBody(static_cast<cEdObjBody*>(pBody));
}

void iEdObjJoint::SetChildBodyByID(int alX)
{
	iEdObject* pBody = GetWorld()->GetObject(alX);
	if(pBody==NULL || pBody->GetTypeName()==_W("Body"))
		SetChildBody(static_cast<cEdObjBody*>(pBody));
}

int iEdObjJoint::GetParentBodyID()
{
	int lID = -1;
	if(mpParentBody) lID = mpParentBody->GetID();
	
	return lID;
}

int iEdObjJoint::GetChildBodyID()
{
	int lID = -1;
	if(mpChildBody) lID = mpChildBody->GetID();
	
	return lID;
}

//-------------------------------------------------------------------

iEdEditPane* iEdObjJoint::CreateEditPane()
{
	return hplNew( cEdEditPaneJoint,(this));
}

//------------------------------------------------------------------------------

iEdAction* iEdObjJoint::CreateSetParentBodyAction(cEdObjBody* apBody)
{
	int lID = apBody?apBody->GetID():-1;
	return CreateSetIntAction(eJointInt_ParentID, lID);
}

iEdAction* iEdObjJoint::CreateSetChildBodyAction(cEdObjBody* apBody)
{
	int lID = apBody?apBody->GetID():-1;
	return CreateSetIntAction(eJointInt_ChildID, lID);
}

//------------------------------------------------------------------------------

void iEdObjJoint::AddToOutlineSpecific(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren)
{
	cEdObjBody* vBodies[] = { GetParentBody(), GetChildBody() };
	
	tWString sJointConnectedEntryPrefix[] = { _W("Parent"), _W("Child") };

    for(int i=0;i<2;++i)
	{
		tWString sEntryText;
		for(int j=0;j<=alLevel;++j)
			sEntryText = _W("   ") + sEntryText;

		cEdObjBody* pBody = vBodies[i];

		tWString sBodyName = pBody?pBody->GetName() : _W("None");

		sEntryText += sJointConnectedEntryPrefix[i] + _W(": ") + sBodyName;
		cWidgetItem* pItem = apList->AddItem(sEntryText);
		pItem->SetUserData(pBody);
	}
}

//-------------------------------------------------------------------

cObjectIcon* iEdObjJoint::CreateIcon()
{
	return hplNew(cObjectIcon,(this, mpType->GetXmlElementName()));
}

//-------------------------------------------------------------------

void iEdObjJoint::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	iEdScnObject::OnDraw(aData);

	if(mbSelected==false) return;

	if(mpParentBody)
		aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mpParentBody->GetTranslation(), cColor(1,0,0,1));
	if(mpChildBody)
		aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mpChildBody->GetTranslation(), cColor(0,1,0,1));

	cMatrixf mtxTransRot = cMath::MatrixMul(GetTranslationMatrix(), GetRotationMatrix());

	aData.mpViewport->DrawArrow(aData.mpFunctions, mtxTransRot, 1, true, cVector2f(0.05f, 0.2f), cColor(1));
}


//-------------------------------------------------------------------
