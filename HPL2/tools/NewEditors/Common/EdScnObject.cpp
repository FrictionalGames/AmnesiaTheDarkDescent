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

#include "../Common/EdScnObject.h"

#include "../Common/Editor.h"
#include "../Common/EdScnWorld.h"

#include "../Common/EdEngineObject.h"
#include "../Common/EdObjectIcon.h"

#include "../Common/EdAction.h"

#include "../Common/EdVar.h"
#include "../Common/EdUserClassDefinition.h"

#include "../Common/EdViewport.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"

#include "../Common/EdScnObjectActions.h"

#include "../Common/EdEditModeSelectToolTranslate.h"
#include "../Common/EdEditModeSelectToolRotate.h"
#include "../Common/EdEditModeSelectToolScale.h"

#include "../Common/EdObjCompound.h"

#include "../Common/EdMultiScnObjEditPane.h"

#include "../Common/EdSurfacePicker.h"

#include <algorithm>


//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE TYPE - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

tManipulatorMap iEdScnObjType::mmapManipulatorPool = tManipulatorMap();

//--------------------------------------------------------------

iEdScnObjType::iEdScnObjType(const tWString& asName, const tString& asXmlElement) : iEdObjectType(asName, asXmlElement)
{
	mbEnabled = true;
	mbVisible = true;

	mbTranslateable = true;
	mbRotateable = true;
	mbScalable = true;

	mbAttachable = true;

	mpCategory = NULL;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE TYPE - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

bool iEdScnObjType::Init()
{
	if(iEdObjectType::Init()==false)
		return false;

	SetUpManipulators();

	return true;
}

//--------------------------------------------------------------

void iEdScnObjType::SetCategory(cEdScnObjCategory* apCat)
{
	mpCategory = apCat;
}

cEdScnObjCategory* iEdScnObjType::GetCategory()
{
	if(mpCategory==NULL)
	{
		iEdScnObjType* pParent = static_cast<iEdScnObjType*>(mpParentType);
		return pParent->GetCategory();
	}

	return mpCategory;
}

//--------------------------------------------------------------

bool iEdScnObjType::IsVisible()
{
	iEdScnObjType* pParentType = static_cast<iEdScnObjType*>(mpParentType);
	if(pParentType)
	{
		if(pParentType->IsVisible()) return mbVisible;
		else return false;
	}

	return mbVisible;
}

void iEdScnObjType::SetVisible(bool abX)
{
	mbVisible = abX;

	SetChildObjectsUpdated();
}

//--------------------------------------------------------------

bool iEdScnObjType::IsEnabled()
{
	iEdScnObjType* pParentType = static_cast<iEdScnObjType*>(mpParentType);
	if(pParentType)
	{
		if(pParentType->IsEnabled()) return mbEnabled;
		else return false;
	}

	return mbEnabled;
}

void iEdScnObjType::SetEnabled(bool abX)
{
	mbEnabled = abX;

	SetChildObjectsUpdated();
}

//--------------------------------------------------------------

void iEdScnObjType::AddAttachableType(const tWString& asName)
{
	if(asName.empty())
		return;

	mlstAttachableTypeNames.push_back(asName);
}

void iEdScnObjType::AddManipulator(iScnObjManipulator* apManipulator)
{
	if(apManipulator==NULL) return;

	iScnObjManipulator* pManipulator = NULL;

	tManipulatorMap::iterator it = mmapManipulatorPool.find(apManipulator->GetName());

	if(it==mmapManipulatorPool.end())
	{
		mmapManipulatorPool.insert(tManipulatorMap::value_type(apManipulator->GetName(), apManipulator));
		pManipulator = apManipulator;
		pManipulator->Init();
	}
	else
	{
		hplDelete(apManipulator);
		pManipulator = it->second;
	}

	mvManipulators.push_back(pManipulator);
}

bool iEdScnObjType::PostCreateSetUp()
{
	if(mpParentType)
	{
		iEdScnObjType* pParent = static_cast<iEdScnObjType*>(mpParentType);
		const tScnObjTypeVec& vParentAttachableTypes = pParent->GetAttachableTypes();
		mvAttachableTypes.insert(mvAttachableTypes.end(), vParentAttachableTypes.begin(), vParentAttachableTypes.end());
	}

	tWStringListIt it = mlstAttachableTypeNames.begin();
	for(;it!=mlstAttachableTypeNames.end(); ++it)
	{
		const tWString& sTypeName = *it;

		iEdScnObjType* pType = static_cast<iEdScnObjType*>(mpWorld->GetTypeByName(sTypeName));

		if(pType) mvAttachableTypes.push_back(pType);
		else
		{
			FatalError("Type '%ls': attachable type '%ls' not found!", GetFullName().c_str(), sTypeName.c_str());
			return false;
		}
	}

	return iEdObjectType::PostCreateSetUp();
}

//--------------------------------------------------------------

void iEdScnObjType::SetUpManipulators()
{
	if(mbTranslateable)
		AddManipulator(hplNew(cTranslateManipulator,()));
	if(mbRotateable)
		AddManipulator(hplNew(cRotateManipulator,()));
	if(mbScalable)
		AddManipulator(hplNew(cScaleManipulator,()));

	Log("Added %d manipulators to type %ls\n", (int)mvManipulators.size(), this->msName.c_str());
}

int iEdScnObjType::GetNumManipulators()
{
	return (int)mvManipulators.size();
}

iScnObjManipulator* iEdScnObjType::GetManipulator(int alX)
{
	return mvManipulators[alX];
}

iEdEditPane* iEdScnObjType::CreateGroupEditPane(iEdScnObject* apObj)
{
	return hplNew(cEdMultiScnObjEditPane,(apObj));
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE TYPE - PROTECTED METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT DATA - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iEdScnObjData::iEdScnObjData(iEdObjectType* apType) : iEdObjectData(apType)
{
	mbSelected = false;

	mlParentID = -1;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT DATA - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void iEdScnObjData::CopyFromObject(iEdObject* apObj)
{
	iEdObjectData::CopyFromObject(apObj);
	iEdScnObject* pObj = static_cast<iEdScnObject*>(apObj);

	mbSelected = pObj->IsSelected();

	if(pObj->GetParent())
		mlParentID = pObj->GetParent()->GetID();

	mlstChildIDs.clear();
	const tScnObjList& lstChildren = pObj->GetChildren();
	tScnObjList::const_iterator itChildren = lstChildren.begin();
	for(;itChildren!=lstChildren.end(); ++itChildren)
	{
		iEdScnObject* pChild = *itChildren;

		mlstChildIDs.push_back(pChild->GetID());
	}
}

void iEdScnObjData::CopyToObject(iEdObject* apObj, ePropStep aStep)
{
	iEdObjectData::CopyToObject(apObj, aStep);
	iEdScnObject* pObj = static_cast<iEdScnObject*>(apObj);
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpType->GetWorld());

	if(aStep==ePropStep_PostDeployAll)
	{
		iEdScnObject* pParent = pWorld->GetScnObject(mlParentID);
		if(pParent && pParent->HasChild(pObj)==false)
			pParent->AddChild(pObj);

		tIntListIt itChildren = mlstChildIDs.begin();
		for(;itChildren!=mlstChildIDs.end(); ++itChildren)
		{
			int lChildID = *itChildren;

			iEdScnObject* pChild = pWorld->GetScnObject(lChildID);
			if(pChild)
				pObj->AddChild(pChild);
		}
	}
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT DATA - PROTECTED METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iEdObject* iEdScnObjData::CreateTypeSpecificObject()
{
    return hplNew(iEdScnObject,(this));
}

//--------------------------------------------------------------

bool iEdScnObjData::LoadSpecificData(cXmlElement* apElem)
{
	cXmlElement* pChildrenElement = apElem->GetFirstElement("Children");
	if(pChildrenElement)
	{
		cXmlNodeListIterator itChildren = pChildrenElement->GetChildIterator();
		while(itChildren.HasNext())
		{
			cXmlElement* pChildElem = itChildren.Next()->ToElement();

			int lChildID = pChildElem->GetAttributeInt("ID", -1);
			if(lChildID!=-1)
				mlstChildIDs.push_back(lChildID);
		}
	}
	return true;
}

bool iEdScnObjData::SaveSpecificData(cXmlElement* apElem)
{
	if(mlstChildIDs.empty()==false)
	{
		cXmlElement* pChildrenElem = apElem->CreateChildElement("Children");
		tIntListIt itChildID = mlstChildIDs.begin();
		for(;itChildID!=mlstChildIDs.end();++itChildID)
		{
			int lChildID = *itChildID;

			cXmlElement* pChildElem = pChildrenElem->CreateChildElement("Child");
			pChildElem->SetAttributeInt("ID", lChildID);
		}
	}

	return true;
}


//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iEdScnObject::iEdScnObject(iEdObjectData* apData) : iEdObject(apData)
{
	mpEngObject = NULL;
	mpIcon = NULL;

	mbSelected = false;

	mbTranslationUpdated = false;
	mbRotationUpdated = false;
	mbScaleUpdated = false;

	mbTransformUpdated = false;

	mvTranslation = 0;
	mvRotation = 0;
	mvScale = 1;

	mmtxTranslation = cMatrixf::Identity;
	mmtxRotation = cMatrixf::Identity;
	mmtxScale = cMatrixf::Identity;

	mmtxTransform = cMatrixf::Identity;

	mpCompound = NULL;

	mpParent = NULL;

	mbEnabled = true;
	mbVisible = true;

	tWString sType = mpType? mpType->GetFullName(): _W("(type name unavailable at creation)");

	mbActive = true;
	//Log("ScnObject of type %ls created - %p\n", sType.c_str(), this);
}

iEdScnObject::~iEdScnObject()
{
	if(mpIcon) hplDelete(mpIcon);
	if(mpEngObject) hplDelete(mpEngObject);

	if(mpType && GetWorld()->IsClearing()==false && mpParent) mpParent->RemChild(this);
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

bool iEdScnObject::CheckPicked(cUIPickFilter* apFilter, cUIPickMethod* apMethod, iEdScnObject** apPickedObject)
{
	bool bPicked = false;
	bool bCompoundCheck = (mpCompound!=NULL) && apFilter->GetTypeFilter(mpCompound->GetMainType());
	if(bCompoundCheck || apFilter->GetTypeFilter(GetMainType()))
		bPicked = apMethod->Picks(this);

	if(bPicked && apPickedObject)
	{
		if(bCompoundCheck)
			*apPickedObject = mpCompound;
		else
			*apPickedObject = this;
	}

	return bPicked;
}

bool iEdScnObject::PassesFilter(cUIPickFilter* apFilter)
{
	return apFilter->GetTypeFilter(GetMainType());
}

bool iEdScnObject::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float *apT)
{
	if(mpEngObject) return mpEngObject->CheckRayIntersect(apViewport, apPos, apTriangle, apT);
	if(mpIcon) return mpIcon->CheckRayIntersect(apViewport, apPos, apTriangle, apT);

	return false;
}

//--------------------------------------------------------------

bool iEdScnObject::Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox, cVector3f* apPos)
{
	if(mpEngObject) return mpEngObject->Check2DBoxIntersect(apViewport, aBox);
	if(mpIcon) return mpIcon->Check2DBoxIntersect(apViewport, aBox);

	return false;
}

//--------------------------------------------------------------

void iEdScnObject::SetTranslation(const cVector3f& avX)
{
	if(mvTranslation==avX)
		return;

	mvTranslation = avX;
	mbTranslationUpdated = true;
	SetTransformUpdated();

	SetUpdated();
}

//--------------------------------------------------------------

void iEdScnObject::SetRotation(const cVector3f& avX)
{
	if(mvRotation==avX)
		return;

	mvRotation = avX;
	mbRotationUpdated = true;
	SetTransformUpdated();

	SetUpdated();
}

//--------------------------------------------------------------

void iEdScnObject::SetScale(const cVector3f& avX)
{
	if(mvScale==avX)
		return;

	mvScale = avX;
	mbScaleUpdated = true;
	SetTransformUpdated();

	SetUpdated();
}

//--------------------------------------------------------------

void iEdScnObject::SetTranslationMatrix(const cMatrixf& amtxX)
{
	if(mmtxTranslation==amtxX)
		return;

	mmtxTranslation = amtxX;
	SetTranslation(mmtxTranslation.GetTranslation());
	mbTranslationUpdated = false;
}

void iEdScnObject::SetRotationMatrix(const cMatrixf& amtxX)
{
	if(mmtxRotation==amtxX)
		return;

	mmtxRotation = amtxX;
	SetRotation(cMath::MatrixToEulerAngles(amtxX, eEulerRotationOrder_XYZ));
	mbRotationUpdated = false;
}

void iEdScnObject::SetScaleMatrix(const cMatrixf& amtxX)
{
	if(mmtxScale==amtxX)
		return;

	mmtxScale = amtxX;
	cMatrixf mtxInvMatrix = cMath::MatrixInverse(amtxX);
	cVector3f vScale = cVector3f(1.0f/mtxInvMatrix.GetRight().Length(),
								 1.0f/mtxInvMatrix.GetUp().Length(),
								 1.0f/mtxInvMatrix.GetForward().Length());
	SetScale(vScale);
	mbScaleUpdated = false;
}

//--------------------------------------------------------------

const cMatrixf& iEdScnObject::GetTranslationMatrix(bool abForceUpdate)
{
	if(mbTranslationUpdated || abForceUpdate)
	{
		mmtxTranslation = cMath::MatrixTranslate(mvTranslation);
		mbTranslationUpdated = false;
	}

	return mmtxTranslation;
}

const cMatrixf& iEdScnObject::GetRotationMatrix(bool abForceUpdate)
{
	if(mbRotationUpdated || abForceUpdate)
	{
		mmtxRotation = cMath::MatrixRotate(mvRotation, eEulerRotationOrder_XYZ);
		mbRotationUpdated = false;
	}

	return mmtxRotation;
}

const cMatrixf& iEdScnObject::GetScaleMatrix(bool abForceUpdate)
{
	if(mbScaleUpdated || abForceUpdate)
	{
		mmtxScale = cMath::MatrixScale(mvScale);
		mbScaleUpdated = false;
	}

	return mmtxScale;
}

const cMatrixf& iEdScnObject::GetWorldMatrix(bool abForceUpdate)
{
	if(mbTransformUpdated || abForceUpdate)
	{
		mmtxTransform = cMath::MatrixMul(GetTranslationMatrix(), 
										 cMath::MatrixMul(GetRotationMatrix(), GetScaleMatrix()));
	}

	return mmtxTransform;
}

void iEdScnObject::SetWorldMatrix(const cMatrixf& amtxWorldMatrix)
{
	cMatrixf mtxInvMatrix = cMath::MatrixInverse(amtxWorldMatrix);
	cVector3f vScale = cVector3f(1.0f/mtxInvMatrix.GetRight().Length(),
									1.0f/mtxInvMatrix.GetUp().Length(),
									1.0f/mtxInvMatrix.GetForward().Length());

	SetScale(vScale);

	cMatrixf mtxRotate = cMath::MatrixMul(amtxWorldMatrix.GetRotation(), cMath::MatrixInverse(GetScaleMatrix()));
	SetRotationMatrix(mtxRotate);

	SetTranslation(amtxWorldMatrix.GetTranslation());
}

//--------------------------------------------------------------

cMatrixf iEdScnObject::GetRelativeTranslationMatrix(const cMatrixf& amtxInvParent)
{
	return cMath::MatrixMul(amtxInvParent, GetTranslationMatrix());
}

cMatrixf iEdScnObject::GetRelativeRotationMatrix(const cMatrixf& amtxInvParent)
{
	return cMath::MatrixMul(amtxInvParent, GetRotationMatrix());
}

cMatrixf iEdScnObject::GetRelativeScaleMatrix(const cMatrixf& amtxInvParent)
{
	return cMath::MatrixMul(amtxInvParent, GetScaleMatrix());
}

cMatrixf iEdScnObject::GetRelativeWorldMatrix(const cMatrixf& amtxInvParent)
{
	return cMath::MatrixMul(amtxInvParent, GetWorldMatrix());
}

bool iEdScnObject::IsAffectedByDecal(cEdSurfacePicker* apPicker)
{
	return apPicker->Affects(this);
}


void iEdScnObject::SetCompoundObject(cEdObjCompound* apObj, bool abDetachFromOld)
{
	if(apObj==mpCompound)
		return;

	cEdObjCompound* pOldObject = mpCompound;
	if(pOldObject && abDetachFromOld)
		pOldObject->RemoveComponent(this);

	mpCompound = apObj;
}


//--------------------------------------------------------------

bool iEdScnObject::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eScnBool_Active:
		SetActive(abX);
		break;
	default:
		return false;
	}

	return true;
}

bool iEdScnObject::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case eScnVec3f_Translation:
        SetTranslation(avX); break;
	case eScnVec3f_Rotation:
		SetRotation(avX); break;
	case eScnVec3f_Scale:
		SetScale(avX); break;
	default:
		return false;
	}

	return true;
}

//--------------------------------------------------------------

bool iEdScnObject::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eScnBool_Active:
		abX = mbEnabled;
		break;
	default:
		return false;
	}

	return true;
}

bool iEdScnObject::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case eScnVec3f_Translation:
		avX = GetTranslation(); break;
	case eScnVec3f_Rotation:
		avX = GetRotation(); break;
	case eScnVec3f_Scale:
		avX = GetScale(); break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------

iEdAction* iEdScnObject::CreateSetTranslationMatrixAction(const cMatrixf& amtxX)
{
	return hplNew(cEdActionSetTranslationMatrix,(GetWorld(), this, amtxX));
}

iEdAction* iEdScnObject::CreateSetRotationMatrixAction(const cMatrixf& amtxX)
{
	return hplNew(cEdActionSetRotationMatrix,(GetWorld(), this, amtxX));
}

iEdAction* iEdScnObject::CreateSetScaleMatrixAction(const cMatrixf& amtxX)
{
	return hplNew(cEdActionSetScaleMatrix,(GetWorld(), this, amtxX));
}

iEdAction* iEdScnObject::CreateSetWorldMatrixAction(const cMatrixf& amtxX)
{
    return hplNew(cEdActionSetWorldMatrix,(GetWorld(), this, amtxX));
}

//------------------------------------------------------------------

//------------------------------------------------------------------

//------------------------------------------------------------------

/*
iEdAction* iEdScnObject::CreateSetIntAction(int alPropID, int alX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Int, alPropID);
	cEdActionSetInt* pAction = hplNew(cEdActionSetInt,(GetWorld(), tIntList(1,GetID()), pProp, alX));
	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetFloatAction(int alPropID, float afX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Float, alPropID);
	cEdActionSetFloat* pAction = hplNew(cEdActionSetFloat,(GetWorld(), tIntList(1,GetID()), pProp, afX));

	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetBoolAction(int alPropID, bool abX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Bool, alPropID);
	cEdActionSetBool* pAction = hplNew(cEdActionSetBool,(GetWorld(), tIntList(1,GetID()), 
																							pProp, abX));
	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetStringAction(int alPropID, const tString& asX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_String, alPropID);
	cEdActionSetString* pAction = hplNew(cEdActionSetString,(GetWorld(), tIntList(1,GetID()), 
																							pProp, asX));
	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetVector2fAction(int alPropID, const cVector2f& avX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Vec2, alPropID);
	cEdActionSetVec2f* pAction = hplNew(cEdActionSetVec2f,(GetWorld(), tIntList(1,GetID()), 
																							pProp, avX));
	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetVector3fAction(int alPropID, const cVector3f& avX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Vec3, alPropID);
	cEdActionSetVec3f* pAction = hplNew(cEdActionSetVec3f,(GetWorld(), tIntList(1,GetID()), 
																							pProp, avX));
	return SetUpAction(pAction);
}

iEdAction* iEdScnObject::CreateSetColorAction(int alPropID, const cColor& aX)
{
	iProp* pProp = mpType->GetPropByTypeAndID(eVariableType_Color, alPropID);
	cEdActionSetColor* pAction = hplNew(cEdActionSetColor,(GetWorld(), tIntList(1,GetID()), 
																							pProp, aX));
	return SetUpAction(pAction);
}
*/
//--------------------------------------------------------------

void iEdScnObject::SetSelected(bool abX)
{
	if(mbSelected==abX)
		return;

	mbSelected=abX;
	OnSetSelected(abX);
}

void iEdScnObject::AddChild(iEdScnObject* apObj)
{
	if(apObj==NULL || HasChild(apObj))
		return;

	apObj->SetParent(this);
	mlstChildren.push_back(apObj);
}

void iEdScnObject::RemChild(iEdScnObject* apObj)
{
	if(apObj==NULL || HasChild(apObj)==false)
		return;

	apObj->SetParent(NULL, false);
	mlstChildren.remove(apObj);
}

void iEdScnObject::ClearChildren()
{
	tScnObjList lstChildrenCopy = mlstChildren;
	tScnObjListIt it = lstChildrenCopy.begin();

	for(;it!=lstChildrenCopy.end();++it)
	{
		iEdScnObject* pChild = *it;

		RemChild(pChild);
	}
}

bool iEdScnObject::HasChild(iEdScnObject* apObj)
{
	return apObj && apObj->GetParent()==this;
}

void iEdScnObject::SetParent(iEdScnObject* apObj, bool abRemoveFromOld)
{
	if(mpParent==apObj) return;

	if(abRemoveFromOld && mpParent)
		mpParent->RemChild(this);

	mpParent = apObj;
}

//--------------------------------------------------------------

iEdAction* iEdScnObject::CreateAddChildrenAction(tScnObjList& alstChildren)
{
	return hplNew(cEdActionScnObjSetChildren,(this, alstChildren));
}

iEdAction* iEdScnObject::CreateRemChildrenAction(tScnObjList& alstChildren)
{
	return NULL;
}

//--------------------------------------------------------------

bool iEdScnObject::SkipAddingToOutline(int alLevel, const tScnObjList& alstAddedChildren)
{
	if(alLevel==0 && GetParent()!=NULL)
		return true;

	tScnObjList::const_iterator it = find(alstAddedChildren.begin(), alstAddedChildren.end(), this);
	return it!=alstAddedChildren.end();
}

void iEdScnObject::AddToOutline(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren)
{
	if(SkipAddingToOutline(alLevel, alstAddedChildren)) return;

	tWString sEntryText;
	for(int i=0;i<alLevel;++i)
		sEntryText += _W("   ");
	sEntryText += GetName();
		
	cWidgetItem* pItem = apList->AddItem(sEntryText);
	pItem->SetUserData(this);
	alstAddedChildren.push_back(this);

	AddToOutlineSpecific(apList, alLevel, alstAddedChildren);

	tScnObjListIt it = mlstChildren.begin();
	for(;it!=mlstChildren.end();++it)
	{
		iEdScnObject* pChild = *it;
		pChild->AddToOutline(apList, alLevel+1, alstAddedChildren);
	}
}

//--------------------------------------------------------------

void iEdScnObject::DrawObject(const cModuleDrawData& aData, bool abHighlight, const cColor& aHighlightCol)
{
	if(mpIcon)
		mpIcon->DrawIcon(aData.mpViewport, aData.mpFunctions, abHighlight, IsEnabled(), cColor(0.8f,1));
	else if(mpEngObject) 
		mpEngObject->Draw(aData.mpViewport, aData.mpFunctions, abHighlight, true, aHighlightCol);
}

void iEdScnObject::DrawLinks(const cModuleDrawData& aData)
{
	if(mpParent)
	{
		aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mpParent->GetTranslation(), cColor(1,0,0,1));
	}

	tScnObjListIt it = mlstChildren.begin();
	for(;it!=mlstChildren.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), pObj->GetTranslation(), cColor(0,1,0,1));
	}
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE OBJECT - PROTECTED METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void iEdScnObject::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	bool bIsCompound = mpCompound!=NULL;
	bool bHighlight = mbSelected || bIsCompound && mpCompound->IsSelected();

	cColor col = bIsCompound?cColor(1,0,0,0.5f):cColor(1, 0.5f);

	DrawObject(aData, bHighlight, col);

	if(bIsCompound==false && mbSelected)
	{
		DrawLinks(aData);
	}
}

//--------------------------------------------------------------

bool iEdScnObject::OnCreate()
{
	mpIcon = CreateIcon();

	bool bValid = SetUpEngineObject() || mpIcon!=NULL;

	return bValid;
}

void iEdScnObject::OnPostDeployAll(bool)
{
	iEdScnObjData* pData = static_cast<iEdScnObjData*>(mpData);
	if(pData && pData->mbSelected)
	{
		cEdEditModeSelect* pMode = static_cast<cEdEditModeSelect*>(GetWorld()->GetEditor()->GetEditModeHandler()->GetMode(_W("Select")));
		pMode->GetSelection()->AddComponent(this);
	}
}

//--------------------------------------------------------------

bool iEdScnObject::OnUpdate(bool abForceUpdate)
{
	if(mpEngObject)
	{
		const cMatrixf& mtxWorld = GetWorldMatrix(abForceUpdate);
		//Log("Object %ls matrix : %s\n", GetName().c_str(), mtxWorld.ToFileString().c_str());
		mpEngObject->Update();
		mpEngObject->UpdateVisibility();
		mpEngObject->SetMatrix(mtxWorld);
		return true;
	}
	
	return static_cast<iEdScnObjType*>(mpType)->AllowNullEngineObject();
}

//--------------------------------------------------------------

void iEdScnObject::OnDestroy()
{
	iEdWorld* pWorld = GetWorld();
	iEditor* pEditor = pWorld->GetEditor();
	if(mbSelected && pWorld->IsClearing()==false)
	{
		cEdEditModeSelect* pMode = static_cast<cEdEditModeSelect*>(pEditor->GetEditModeHandler()->GetMode(_W("Select")));
		pMode->GetSelection()->RemoveComponent(this);
	}
}

//--------------------------------------------------------------

bool iEdScnObject::SetUpEngineObject()
{
	if(mpEngObject)
	{
		hplDelete(mpEngObject);
		mpEngObject = NULL;
	}

	mpEngObject = CreateEngineObject();
	if(mpEngObject && mpEngObject->Create(cString::ToString(mlID))==false)
	{
		hplDelete(mpEngObject);
		mpEngObject = NULL;
	}

	bool bNullAllowed = static_cast<iEdScnObjType*>(mpType)->AllowNullEngineObject();

	return mpEngObject!=NULL || bNullAllowed;
}

//--------------------------------------------------------------

bool iEdScnObject::SetUpIcon()
{
	if(mpIcon)
	{
		hplDelete(mpIcon);
		mpIcon = NULL;
	}

	mpIcon = CreateIcon();

	return mpIcon!=NULL;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER TYPE ROOT - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iUserTypeRoot::iUserTypeRoot(const tWString& asName, const tString& asXmlElement, const tString& asDefFile) : iEdScnObjType(asName, asXmlElement)
{
	mpUserClassDef = hplNew(cEdUserClassDefinition,(asDefFile));

	mpDefaultType = NULL;
}

iUserTypeRoot::~iUserTypeRoot()
{
	hplDelete(mpUserClassDef);
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER TYPE ROOT - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void iUserTypeRoot::RegisterVarCatName(eEdVarCategory aCat, const tString& asName)
{
	mpUserClassDef->RegisterVarCatName(aCat, asName);
}

//--------------------------------------------------------------

bool iUserTypeRoot::Init()
{
	if(mpUserClassDef->Create()==false)
		return false;

	for(int i=0; i<mpUserClassDef->GetNumClasses(); ++i)
	{
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mpUserClassDef->GetClass(i));
		
		iUserTypeNode* pType = CreateSpecificUserTypeNode(pClass);
		if(AddChildType(pType))
		{
			if(mpDefaultType==NULL || mpUserClassDef->GetDefaultClass()==pClass)
				mpDefaultType = pType;
		}
	}

	return true;
}


iEdObjectType* iUserTypeRoot::GetTypeByName(const tWString& asName)
{
	iEdObjectType* pType = iEdObjectType::GetTypeByName(asName);

	if(pType && pType!=this && pType->GetNumChildren()==0)
		return pType;

	return GetDefaultType();
}

iUserTypeNode* iUserTypeRoot::GetDefaultType()
{
	if(mpDefaultType)
		return mpDefaultType->GetDefaultType();

	return NULL;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER TYPE ROOT - PROTECTED METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER TYPE NODE - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iUserTypeNode::iUserTypeNode(iUserTypeRoot* apRoot, iEdClass* apClass) : iEdScnObjType(apClass->GetName(), apRoot->GetXmlElementName())
{
	mpRoot = apRoot;
	mpClass = apClass;

	mpDefaultType = NULL;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER TYPE NODE - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

bool iUserTypeNode::Init()
{
	if(mpClass==NULL)
	{
		Error("Class in type %ls is NULL\n", GetName().c_str());
		return false;
	}

	bool bInit = iEdScnObjType::Init();
	for(int i=0; i<mpClass->GetNumChildren(); ++i)
	{
		cEdUserClass* pChildClass = static_cast<cEdUserClass*>(mpClass->GetChild(i));

		iUserTypeNode* pSubType = mpRoot->CreateSpecificUserTypeNode(pChildClass);

		bInit = bInit && AddChildType(pSubType);
		if(bInit)
		{
			if(mpDefaultType==NULL || 
				static_cast<cEdUserClass*>(mpClass)->GetDefaultChildClass()==pChildClass)
				mpDefaultType = pSubType;
		}
		else
        	break;
	}

	return bInit;
}

//--------------------------------------------------------------

iEdObjectType* iUserTypeNode::GetTypeByName(const tWString& asName)
{
	iEdObjectType* pType = iEdObjectType::GetTypeByName(asName);

	if(pType)
		return pType;

	return GetDefaultType();
}

iUserTypeNode* iUserTypeNode::GetDefaultType()
{
	if(mpDefaultType)
		return mpDefaultType->GetDefaultType();

	return this;
}

//--------------------------------------------------------------

cEdClassInstance* iUserTypeNode::CreateClassInstance()
{
	return static_cast<cEdUserClass*>(mpClass)->CreateInstance(eEdVarCategory_Instance);
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER OBJ DATA - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iUserObjData::iUserObjData(iEdObjectType* apType) : iEdScnObjData(apType)
{
	iUserTypeNode* pType = static_cast<iUserTypeNode*>(apType);

	mpClassInstance = pType->CreateClassInstance();
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER OBJ DATA - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void iUserObjData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);
	iUserObject* pObj = static_cast<iUserObject*>(apObj);

	for(int i=0; i<mpClassInstance->GetVarInstanceNum(); ++i)
	{
		cEdVarInstance* pVar = mpClassInstance->GetVarInstance(i);
		cEdVarInstance* pObjVar = pObj->GetVar(pVar->GetName());

		if(pObjVar)	mpClassInstance->SetVarValue(pVar->GetName(), pObjVar->GetValue());
	}
}

void iUserObjData::CopyToObject(iEdObject* apObj, ePropStep aStep)
{
	iEdScnObjData::CopyToObject(apObj, aStep);
	iUserObject* pObj = static_cast<iUserObject*>(apObj);

	if(aStep==ePropStep_PostCreate)
	{
		for(int i=0; i<mpClassInstance->GetVarInstanceNum(); ++i)
		{
			cEdVarInstance* pVar = mpClassInstance->GetVarInstance(i);
			pObj->SetVar(pVar->GetName(), pVar->GetValue());
		}
	}
}

//--------------------------------------------------------------

bool iUserObjData::LoadSpecificData(cXmlElement* apElem)
{
	if(iEdScnObjData::LoadSpecificData(apElem)==false)
		return false;

	if(mpClassInstance)
	{
		cXmlElement* pVarElem = apElem->GetFirstElement("UserVariables");
		if(pVarElem) mpClassInstance->Load(pVarElem);
	}

	return true;
}

bool iUserObjData::SaveSpecificData(cXmlElement* apElem)
{
	if(iEdScnObjData::SaveSpecificData(apElem)==false)
		return false;

	if(mpClassInstance)
	{
		cXmlElement* pVarElem = apElem->CreateChildElement("UserVariables");
		if(pVarElem) mpClassInstance->Save(pVarElem);
	}

	return true;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER OBJECT - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iUserObject::iUserObject(iEdObjectData* apData) : iEdScnObject(apData)
{
	mpClassInstance = NULL;

	iUserTypeNode* pType = static_cast<iUserTypeNode*>(mpType);
	SetType(pType, true);
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER OBJECT - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

void iUserObject::SetType(iUserTypeNode* apType, bool abForce)
{
	iUserTypeNode* pOldType = static_cast<iUserTypeNode*>(mpType);

	if(abForce==false && (apType==NULL || pOldType==apType || 
		pOldType->GetUserTypeRoot()!=apType->GetUserTypeRoot()))
		return;

	pOldType->RemoveObject(this);

	tVarValueMap mapValues;
	if(mpClassInstance)
	{
		mpClassInstance->SaveValuesToMap(mapValues);

		hplDelete(mpClassInstance);
	}

	mpType = apType;
	mpType->AddObject(this);

	mpClassInstance = apType->CreateClassInstance();
	mpClassInstance->LoadValuesFromMap(mapValues);

	SetTypeChanged(pOldType!=mpType);
}

//--------------------------------------------------------------

cEdVarInstance* iUserObject::GetVar(const tWString& asName)
{
	return mpClassInstance->GetVarInstance(asName);
}

void iUserObject::SetVar(const tWString& asName, const tWString& asValue)
{
	if(mpClassInstance->SetVarValue(asName, asValue))
		OnSetVar(asName, asValue);
}

//--------------------------------------------------------------

iEdAction* iUserObject::CreateSetUserTypeAction(iUserTypeNode* apType)
{
	return hplNew(cEdActionSetObjectUserType,(this, apType));
}


//--------------------------------------------------------------

iEdAction* iUserObject::CreateSetUserValueAction(const tWString& asName, const tWString& asValue)
{
	return hplNew(cEdActionSetObjectVar,(this, asName, asValue));
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// USER OBJECT - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

iBaseAggregateType::iBaseAggregateType(const tWString& asName, const tString& asXmlElement, const tString& asXmlComponent) : iEdScnObjType(asName, asXmlElement)
{
	msXmlComponentName = asXmlComponent;
}

//--------------------------------------------------------------

//--------------------------------------------------------------

iBaseAggregateObjData::iBaseAggregateObjData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

void iBaseAggregateObjData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);

	mvComponentData.clear();

	iBaseAggregateObject* pObj = static_cast<iBaseAggregateObject*>(apObj);
	const tScnObjList& lstComponents = pObj->GetComponents();
	tScnObjList::const_iterator it = lstComponents.begin();
	for(;it!=lstComponents.end(); ++it)
	{
		iEdScnObject* pComponent = *it;

		mvComponentData.push_back(pComponent->CreateCopyData());
	}
}

void iBaseAggregateObjData::CopyToObject(iEdObject* apObj, ePropStep aStep)
{
	iEdScnObjData::CopyToObject(apObj, aStep);

	if(aStep==ePropStep_PostDeployAll)
	{
		iBaseAggregateObject* pObj = static_cast<iBaseAggregateObject*>(apObj);
		iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpType->GetWorld());

		if(mvComponentData.empty())
		{
			for(size_t i=0;i<mvComponentIDs.size(); ++i)
			{
				int lID = mvComponentIDs[i];
				iEdObject* pComponent = pWorld->GetObject(lID);

				mvComponentData.push_back(pComponent->CreateCopyData());
			}
		}

		for(size_t i=0;i<mvComponentData.size();++i)
		{
			iEdObjectData* pData = mvComponentData[i];
			
			iEdScnObject* pComponent = pWorld->GetScnObject(pData->GetID());
			if(pComponent==NULL)
				pComponent = static_cast<iEdScnObject*>(pWorld->CreateObjFromData(pData));

			pObj->AddComponent(pComponent);
		}
	}
}

//--------------------------------------------------------------

bool iBaseAggregateObjData::LoadSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::LoadSpecificData(apElement)==false)
		return false;

	const tString& sComponentName = static_cast<iBaseAggregateType*>(mpType)->GetComponentElementName();
	iEdWorld* pWorld = mpType->GetWorld();

	cXmlNodeListIterator itComponents = apElement->GetChildIterator();
	while(itComponents.HasNext())
	{
		cXmlElement* pCompElem = itComponents.Next()->ToElement();
		if(pCompElem->GetValue()!=sComponentName) continue;

		int lCompID = pCompElem->GetAttributeInt("ID", -1);
		if(lCompID!=-1)
            mvComponentIDs.push_back(lCompID);
	}

	return true;
}

//--------------------------------------------------------------

bool iBaseAggregateObjData::SaveSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecificData(apElement)==false)
		return false;

	const tString& sComponentName = static_cast<iBaseAggregateType*>(mpType)->GetComponentElementName();

	for(size_t i=0; i<mvComponentData.size();++i)
	{
		iEdObjectData* pData = mvComponentData[i];
		apElement->CreateChildElement(sComponentName)->SetAttributeInt("ID", pData->GetID());
	}

	return true;
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE AGGREGATE OBJECT - CONSTRUCTORS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iBaseAggregateObject::iBaseAggregateObject(iEdObjectData* apData) : iEdScnObject(apData)
{
	mbComponentsUpdated = false;
	mbUpdatingComponents = false;
	mbBypassComponentsTransform = false;
}

iBaseAggregateObject::~iBaseAggregateObject()
{
}

//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// BASE AGGREGATE OBJECT - PUBLIC METHODS
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

tIntVec iBaseAggregateObject::GetCloneIDs()
{
	tIntVec vIDs;

	iEdWorld* pWorld = GetWorld();
	for(int i=0;i<(int)mlstComponents.size()+1;++i)
		vIDs.push_back(pWorld->GetFreeID());

	return vIDs;
}

void iBaseAggregateObject::SetUpCloneData(iEdObjectData* apData, const tIntVec& avIDs)
{
	iEdWorld* pWorld = GetWorld();
	iBaseAggregateObjData* pCloneData = static_cast<iBaseAggregateObjData*>(apData);

	tEdObjDataVec vComponentsData;
	tScnObjList lstComponentClones;
	tScnObjListIt itComponents = mlstComponents.begin();
	for(int i=0; itComponents!=mlstComponents.end(); ++itComponents, ++i)
	{
		iEdScnObject* pComponent = *itComponents;
		iEdObjectData* pComponentData = pComponent->CreateCopyData();

		//pComponentData->SetCompoundID(-1);
		pComponentData->SetID(avIDs[i]);
		pComponentData->SetName(pWorld->GenerateValidName(pComponentData->GetName(), false));

		vComponentsData.push_back(pComponentData);

		iEdScnObject* pComponentClone = static_cast<iEdScnObject*>(pWorld->CreateObjFromData(pComponentData));
		lstComponentClones.push_back(pComponentClone);
	}
	
	itComponents = lstComponentClones.begin();
	for(;itComponents!=lstComponentClones.end();++itComponents)
	{
		iEdObject* pComponent = *itComponents;
		pComponent->PostDeployAll(false);
	}
	mbComponentsUpdated = false;

	//pCloneData->SetComponentsData(vComponentsData);
}

//-------------------------------------------------------------------

bool iBaseAggregateObject::IsCulledByFrustum(cCamera* apCamera)
{
	return false;
}


bool iBaseAggregateObject::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	////////////////////////////////////////////////////
	// Check intersection against all component shapes
	bool bIntersect = false;

	//float fMinSqrDistance = 999999999.0f;
	cVector3f vIntersectionPos;
	cVector3f vMinIntersectionPos;
	float fMinT = 99999.0f;
	const cVector3f& vRayStart = apViewport->GetUnprojectedStart();

	// Iterate through components
	tScnObjListIt it = mlstComponents.begin();
	for(;it!=mlstComponents.end();++it)
	{
		iEdScnObject* pObj = *it;
		//if(pObj->IsCulledByFrustum(apViewport->GetCamera()))
		//	continue;

		float fT;
		// Check intersection with each
		if(pObj->CheckRayIntersect(apViewport, &vIntersectionPos, NULL, &fT))
		{
			bIntersect = true;
			if(fT<fMinT)
			{
				fMinT = fT;
				vMinIntersectionPos = vIntersectionPos;
			}
		}
	}

	// If there is intersection, return minimum distance intersection (if possible)
	if(bIntersect)
	{
		if(apPos) *apPos = vMinIntersectionPos;
		if(apT) *apT = fMinT;
	}

	return bIntersect;
}


//-------------------------------------------------------------------

bool iBaseAggregateObject::Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox, cVector3f* apPos)
{
	////////////////////////////////////////////////////
	// Check intersection against all component shapes
	bool bIntersect = false;

	float fMinDistance = 99999999.0f;
	cVector3f vMinIntersectionPos;
	const cVector3f& vRayStart = apViewport->GetUnprojectedStart();

	//////////////////////////////////////////////////////
	// Iterate through components
	tScnObjListIt it = mlstComponents.begin();
	for(;it!=mlstComponents.end();++it)
	{
		iEdScnObject* pObj = *it;
		//////////////////////////////////////////
		// Check if outside the screen
		//if(pObj->IsCulledByFrustum(apViewport->GetCamera()))
		//	continue;

		//////////////////////////////////////////
		// Check intersection with each
		cVector3f vIntersectionPos;
		if(pObj->Check2DBoxIntersect(apViewport, aBox, &vIntersectionPos))
		{
			bIntersect = true;

			////////////////////////////////////////
			// Update min distance
			float fDistance = cMath::Vector3DistSqr(vRayStart, vIntersectionPos);
			if(fDistance < fMinDistance)
			{
				fMinDistance = fDistance;
				vMinIntersectionPos = vIntersectionPos;
			}
		}
	}

	///////////////////////////////////////////////////////////
	// If there is intersection, return minimum distance intersection (if possible)
	if(bIntersect && apPos)
		*apPos = vMinIntersectionPos;

	return bIntersect;
}

//-------------------------------------------------------------------

void iBaseAggregateObject::AddComponent(iEdScnObject* apObj)
{
	if(apObj==NULL)
		return;

	tScnObjListIt it = find(mlstComponents.begin(), mlstComponents.end(), apObj);
	if(it!=mlstComponents.end())
		return;
	
	mlstComponents.push_back(apObj);
	OnAddComponent(apObj);

	SetComponentsUpdated();
}

//-------------------------------------------------------------------

void iBaseAggregateObject::RemoveComponent(iEdScnObject* apObj)
{
	if(apObj==NULL)
		return;

	tScnObjListIt it = find(mlstComponents.begin(), mlstComponents.end(), apObj);
	if(it==mlstComponents.end())
		return;

	OnRemoveComponent(apObj);
	mlstComponents.erase(it);

	SetComponentsUpdated();
}

//-------------------------------------------------------------------

bool iBaseAggregateObject::HasComponents()
{
	return mlstComponents.empty()==false;
}

int iBaseAggregateObject::GetNumComponents()
{
	return (int)mlstComponents.size();
}

iEdScnObject* iBaseAggregateObject::GetComponent(int alIdx)
{
	if(alIdx<0 || alIdx>=GetNumComponents())
		return NULL;

	tScnObjListIt it = mlstComponents.begin();
	for(int i=0;it!=mlstComponents.end(); ++i, ++it)
	{
        if(i==alIdx)
			return *it;
	}

	return NULL;
}

//-------------------------------------------------------------------

/*
void iBaseAggregateObject::OnWorldPostTransDraw(const cModuleDrawData& aData)
{
	for(tEntityWrapperListIt it = mlstEntities.begin();it!=mlstEntities.end();++it)
	{
		iEdObject* pEnt = *it;
		cBoundingVolume* pBV = pEnt->GetPickBV(NULL);

		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1,0,0,1));
	}
}
*/

//-------------------------------------------------------------------

void iBaseAggregateObject::SetTranslation(const cVector3f& avPosition)
{
	iEdScnObject::SetTranslation(avPosition);

	mbComponentsUpdated = false;

	if(mbBypassComponentsTransform==false)
	{
		ApplyTransformToComponents();
	}
}

//-------------------------------------------------------------------

void iBaseAggregateObject::SetRotation(const cVector3f& avRotation)
{
	iEdScnObject::SetRotation(avRotation);

	mbComponentsUpdated = false;

	if(mbBypassComponentsTransform==false)
		ApplyTransformToComponents();
}

//-------------------------------------------------------------------

void iBaseAggregateObject::SetScale(const cVector3f& avScale)
{
	iEdScnObject::SetScale(avScale);

	mbComponentsUpdated = false;

	if(mbBypassComponentsTransform==false)
		ApplyTransformToComponents();
}

//-------------------------------------------------------------------

bool iBaseAggregateObject::OnUpdate(bool abForce)
{
	if(iEdScnObject::OnUpdate(abForce)==false)
		return false;

	if(mbComponentsUpdated)
	{
		mbComponentsUpdated = false;
		UpdateComponents();
	}

	//Log("BaseAggregateObject %p mbUpdated %d\n", this, mbUpdated);

	return true;
}

//-------------------------------------------------------------------

void iBaseAggregateObject::OnDestroy()
{
	iEdScnObject::OnDestroy();
	
	iEdWorld* pWorld = GetWorld();
	//iEntityWrapperTypeAggregate* pType = (iEntityWrapperTypeAggregate*)mpType;
	if(pWorld->IsClearing()==false)
	{
		tScnObjList lstComponentsCopy = mlstComponents;
		tScnObjListIt it = lstComponentsCopy.begin();
		for(;it!=lstComponentsCopy.end();++it)
		{
			iEdScnObject* pComponent = *it;
			//if(pType->GetDestroyComponentsOnDeletion())	pWorld->DestroyEntityWrapper(pEnt);
			//else RemoveComponent(pEnt);

			RemoveComponent(pComponent);
		}
	}
}

//-------------------------------------------------------------------

void iBaseAggregateObject::ApplyTransformToComponents()
{
	tScnObjListIt it = mlstComponents.begin();
	tMatrixfListIt itMat = mlstRelWorldMatrices.begin();

	const cMatrixf& mmtxParentWorldMatrix = GetWorldMatrix();
	
	for(;it!=mlstComponents.end() && itMat!=mlstRelWorldMatrices.end(); ++it, ++itMat)
	{
		iEdScnObject* pObj = *it;
		const cMatrixf& mtxChildRelMatrix= *itMat;

		pObj->SetWorldMatrix(cMath::MatrixMul(mmtxParentWorldMatrix, mtxChildRelMatrix));
	}
}



//-------------------------------------------------------------------
//-------------------------------------------------------------------

cEdScnObjRoot::cEdScnObjRoot() : iEdScnObjType(_W("Object"), "")
{
	AddVec3f(eScnVec3f_Translation,	"WorldPos", 0, ePropStep_PreCreate);
	AddVec3f(eScnVec3f_Rotation,	"Rotation", 0, ePropStep_PreCreate);
	AddVec3f(eScnVec3f_Scale,		"Scale",	1, ePropStep_PreCreate);
}

