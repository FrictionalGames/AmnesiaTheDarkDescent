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

#include "../Common/EdSelection.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdSelectActions.h"
#include "../Common/EdObjectActions.h"
#include "../Common/EdEditModeSelect.h"

#include "../Common/EdObjCompound.h"

#include <algorithm>

//----------------------------------------------------------------------

/*
float cEdSelection::mfScaleSnap = 0.25f;
float cEdSelection::mfRotateSnap = kPi2f/6;

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetIntAction(const tString& asPropName, int alX)
{
	cEdActionSetInt* pAction = hplNew(cEdActionSetInt,(GetWorld(), tIntList(1,GetID()), 
														asPropName, alX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetFloatAction(const tString& asPropName, float afX)
{
	cEdActionSetFloat* pAction = hplNew(cEdActionSetFloat,(GetWorld(), tIntList(1,GetID()), 
															asPropName, afX));

	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetBoolAction(const tString& asPropName, bool abX)
{
	cEdActionSetBool* pAction = hplNew(cEdActionSetBool,(GetWorld(), tIntList(1,GetID()), 
														asPropName, abX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetStringAction(const tString& asPropName, const tString& asX)
{
	cEdActionSetString* pAction = hplNew(cEdActionSetString,(GetWorld(), tIntList(1,GetID()), 
															asPropName, asX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetVector2fAction(const tString& asPropName, const cVector2f& avX)
{
	cEdActionSetVec2f* pAction = hplNew(cEdActionSetVec2f,(GetWorld(), tIntList(1,GetID()), 
															asPropName, avX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetVector3fAction(const tString& asPropName, const cVector3f& avX)
{
	cEdActionSetVec3f* pAction = hplNew(cEdActionSetVec3f,(GetWorld(), tIntList(1,GetID()), 
															asPropName, avX));
	return SetUpAction(pAction);
}

//-----------------------------------------------------------------------

iEdAction* iEdObject::CreateSetColorAction(const tString& asPropName, const cColor& aX)
{
	cEdActionSetColor* pAction = hplNew(cEdActionSetColor,(GetWorld(), tIntList(1,GetID()), 
															asPropName, aX));
	return SetUpAction(pAction);
}

*/

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdSelectionType::cEdSelectionType(cEdSelection* apSel) : iBaseAggregateType(_W("Selection"), "", "")
{
	mpSelection = apSel;
	mpWorld = iEditor::GetInstance()->GetWorld();
	mpMostRestrictiveType = NULL;
}

int cEdSelectionType::GetNumManipulators()
{
	if(mpSelection->GetNumComponents()==1)
	{
		iEdScnObjType* pType = static_cast<iEdScnObjType*>(mpSelection->GetComponent(0)->GetType());
		return pType->GetNumManipulators();
	}

	return iEdScnObjType::GetNumManipulators();
}

iScnObjManipulator* cEdSelectionType::GetManipulator(int alIdx)
{
	if(mpSelection->GetNumComponents()==1)
	{
		iEdScnObjType* pType = static_cast<iEdScnObjType*>(mpSelection->GetComponent(0)->GetType());
		return pType->GetManipulator(alIdx);
	}

	return iEdScnObjType::GetManipulator(alIdx);
}

//----------------------------------------------------------------------

void cEdSelectionType::UpdateType()
{
	mbCloneable = true;
	mbDeletable = true;
	mbRenameable = true;

	mbTranslateable = true;
	mbRotateable = true;
	mbScalable = true;

	mpMostRestrictiveType = NULL;
	for(int i=0; i<mpSelection->GetNumComponents(); ++i)
	{
		iEdScnObject* pObj = mpSelection->GetComponent(i);
		iEdObjectType* pType = pObj->GetType();

		if(mpMostRestrictiveType==NULL)
		{
			mpMostRestrictiveType = static_cast<iEdScnObjType*>(pType);
		}
		else
		{
			mpMostRestrictiveType = static_cast<iEdScnObjType*>(mpMostRestrictiveType->GetMostRestrictiveType(pType));
		}

		// Set up restrictions
		mbCloneable = mbCloneable && mpMostRestrictiveType->IsCloneable();
		mbDeletable = mbDeletable && mpMostRestrictiveType->IsDeletable();
		mbRenameable = mbRenameable && mpMostRestrictiveType->IsRenameable();

		mbTranslateable = mbTranslateable && mpMostRestrictiveType->IsTranslateable() && pObj->IsTranslateable();
		mbRotateable = mbRotateable && mpMostRestrictiveType->IsRotateable() && pObj->IsRotateable();
		mbScalable = mbScalable && mpMostRestrictiveType->IsScalable() && pObj->IsScalable();
	}

	mvManipulators.clear();
	SetUpManipulators();
}

//----------------------------------------------------------------------

iEdEditPane* cEdSelectionType::CreateGroupEditPane(iEdScnObject* apObj)
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(apObj);

	if(mpMostRestrictiveType==NULL || pSelection->GetNumComponents()<=1)
		return NULL;

	return mpMostRestrictiveType->CreateGroupEditPane(pSelection);
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdSelection::cEdSelection(cEdEditModeSelect* apEditMode) : iBaseAggregateObject(NULL)
{
	mpEditMode = apEditMode;
	mpType = hplNew(cEdSelectionType,(this));

	mbBVUpdated = false;

	mbGlobalTransform = false;
	mbKeepTransform = false;
}

cEdSelection::~cEdSelection()
{
	hplDelete(mpType);
	mpType=NULL;
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEdSelection::Clear()
{
	tScnObjList lstComponentsCopy = mlstComponents;
	tScnObjListIt it = lstComponentsCopy.begin();
	for(;it!=lstComponentsCopy.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		RemoveComponent(pObj);
		//if(GetWorld()->HasObject(pObj))
		//	pObj->SetSelected(false);
	}

	mlstComponents.clear();
	mlstObjectIDs.clear();

	SetTranslation(0);
	SetRotation(0);
	SetScale(1);

	mpEditMode->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldSelectChange, NULL);
}

//----------------------------------------------------------------------

bool cEdSelection::HasObject(int alID)
{
	tIntListIt it= find(mlstObjectIDs.begin(), mlstObjectIDs.end(), alID);

    return (it!=mlstObjectIDs.end());
}

bool cEdSelection::HasObject(iEdScnObject* apObj)
{
	if(apObj==NULL) return false;

	return HasObject(apObj->GetID());
}

//----------------------------------------------------------------------

void cEdSelection::CheckObjectsAreValid()
{
	/*
	iEdWorld* pWorld = GetWorld();
	for(int i=0; i<GetNumComponents(); ++i)
	{
		iEdScnObject* pObj = GetComponent(i);
	}
	*/
}

void cEdSelection::SetGlobalTransform(bool abX)
{
	if(mbGlobalTransform==abX) return;

	mbGlobalTransform = abX;
	
	SetComponentsUpdated();
	SetUpdated();

	mbBVUpdated = true;

	mbKeepTransform = true;
	mvStoredRotate = GetRotation();
	mvStoredScale = GetScale();
}

//----------------------------------------------------------------------

void cEdSelection::SetTranslation(const cVector3f& avX)
{
	mbBypassComponentsTransform = mbGlobalTransform==false;

	iBaseAggregateObject::SetTranslation(avX);

	mbBVUpdated = true;

	if(mbBypassComponentsTransform)
	{
		tScnObjListIt it = mlstComponents.begin();
		tMatrixfListIt itMat = mlstRelTranslationMatrices.begin();
		for(;it!=mlstComponents.end() && itMat!=mlstRelTranslationMatrices.end(); ++it, ++itMat)
		{
			iEdScnObject* pObj = *it;
			const cMatrixf& matrix = *itMat;
			pObj->SetTranslationMatrix(cMath::MatrixMul(GetTranslationMatrix(), matrix));
		}
	}
}

void cEdSelection::SetRotation(const cVector3f& avX)
{
	mbBypassComponentsTransform = mbGlobalTransform==false;

	iBaseAggregateObject::SetRotation(avX);

	mbBVUpdated = true;

	if(mbBypassComponentsTransform)
	{
		tScnObjListIt it = mlstComponents.begin();
		tMatrixfListIt itMat = mlstRelRotationMatrices.begin();

		for(;it!=mlstComponents.end() && itMat!=mlstRelRotationMatrices.end(); ++it, ++itMat)
		{
			iEdScnObject* pObj = *it;
			const cMatrixf& matrix = *itMat;

			pObj->SetRotationMatrix(cMath::MatrixMul(GetRotationMatrix(), matrix));
		}
	}
}

void cEdSelection::SetScale(const cVector3f& avX)
{
	mbBypassComponentsTransform = mbGlobalTransform==false;

	iBaseAggregateObject::SetScale(avX);

	mbBVUpdated = true;

	if(mbBypassComponentsTransform)
	{
		tScnObjListIt it = mlstComponents.begin();
		tMatrixfListIt itMat = mlstRelScaleMatrices.begin();
	
		for(;it!=mlstComponents.end() && itMat!=mlstRelScaleMatrices.end(); ++it, ++itMat)
		{
			iEdScnObject* pObj = *it;
			const cMatrixf& matrix = *itMat;

			pObj->SetScaleMatrix(cMath::MatrixMul(GetScaleMatrix(), matrix));
		}
	}
}
/*

void cEdSelection::SetWorldMatrix(const cMatrixf& amtxX)
{
	iBaseAggregateObject::SetWorldMatrix(amtxX);

	if(mbUpdatingComponents==false)
	{
		tScnObjListIt it = mlstComponents.begin();
		tMatrixfListIt itWorld = mlstRelWorldMatrices.begin();
		for(;it!=mlstComponents.end(); ++it, ++itWorld)
		{
			iEdScnObject* pObj = *it;
			const cMatrixf& mtxRelWorld = *itWorld;
			pObj->SetWorldMatrix(cMath::MatrixMul(GetWorldMatrix(), mtxRelWorld));
		}
	}
}
*/


//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateSelectObjectsAction(const tScnObjList& alstObjects)
{
	return hplNew(cEdActionSelectObjects,(this, alstObjects));
}

iEdAction* cEdSelection::CreateDeselectObjectsAction(const tScnObjList& alstObjects)
{
	return hplNew(cEdActionDeselectObjects, (this, alstObjects));
}

iEdAction* cEdSelection::CreateToggleObjSelectionAction(const tScnObjList& alstObjects)
{
	return hplNew(cEdActionToggleObjSelection, (this, alstObjects));
}

//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateDeleteObjectsAction()
{
	tEdObjectList lstObjects(mlstComponents.begin(), mlstComponents.end());

	return hplNew(cEdActionDeleteObjects,(GetWorld(), lstObjects));
}

//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateCloneObjectsAction(bool abSelectClones)
{
	cEdActionCompound* pAction = hplNew(cEdActionCompound,(_W("Clone")));

	tEdObjectList lstObjects(mlstComponents.begin(), mlstComponents.end());
	cEdActionCloneObjects* pActionClone = hplNew(cEdActionCloneObjects,(GetWorld(), lstObjects));

	pAction->AddAction(pActionClone);

	if(abSelectClones)
	{
		pAction->StepForward();
		const tEdObjectList& lstClones = pActionClone->GetClones();
		tEdObjectList::const_iterator it = lstClones.begin();
		tScnObjList lstClonesCopy;

		for(;it!=lstClones.end(); ++it)
		{
			iEdScnObject* pObj = static_cast<iEdScnObject*>(*it);
			lstClonesCopy.push_back(pObj);
		}
		pAction->AddAction(CreateSelectObjectsAction(lstClonesCopy));
	}

	return pAction;
}

//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateCompoundObjectAction()
{
	iEdWorld* pWorld = GetWorld();
	iEdObjectType* pTypeCompound = pWorld->GetTypeByName(_W("Compound"));

	if(pTypeCompound==NULL)
		return NULL;

	iEdAction* pAction = NULL;

	std::set<iEdScnObject*> lstFoundCompounds;
	int lNumObjectsBelongingToCompounds = 0;
	tScnObjList lstObjectsInNewCompound;

	tScnObjList::const_iterator it = mlstComponents.begin();
	for(;it!=mlstComponents.end();++it)
	{
		iEdScnObject* pObj = *it;

		if(pObj->GetType()==pTypeCompound)
		{
			cEdObjCompound* pCompound = static_cast<cEdObjCompound*>(pObj);

			const tScnObjList& lstComponents = pCompound->GetComponents();
			lstObjectsInNewCompound.insert(lstObjectsInNewCompound.end(), lstComponents.begin(), lstComponents.end());
		}
		else
		{
			if(pObj->GetCompoundObject()!=NULL)
			{
				lstFoundCompounds.insert(pObj->GetCompoundObject());
				++lNumObjectsBelongingToCompounds;
			}

			lstObjectsInNewCompound.push_back(pObj);
		}
	}

	if(lstObjectsInNewCompound.empty() ||
		lNumObjectsBelongingToCompounds==GetNumComponents() && lstFoundCompounds.size()==1)
		return NULL;

	{
		cEdActionCompound* pCompoundAction = hplNew(cEdActionCompound,(_W("Create compound object")));

		iEdObjectData* pData = pTypeCompound->CreateData();
		pData->SetName(pWorld->GenerateValidName(pData->GetName(), true));

		pCompoundAction->AddAction(pWorld->CreateActionCreateObject(pData));
		pCompoundAction->StepForward();

		iBaseAggregateObject* pNewCompound = static_cast<iBaseAggregateObject*>(pWorld->GetObject(pData->GetID()));
		if(pNewCompound==NULL)
		{
			hplDelete(pCompoundAction);
			pCompoundAction = NULL;
		}
		else
		{
			pCompoundAction->AddAction(pNewCompound->CreateAddComponentsAction(mlstComponents));
			pCompoundAction->AddAction(CreateSelectObjectsAction(tScnObjList(1, pNewCompound)));
		}
		
		pAction = pCompoundAction;
	}

	return pAction;
}


//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateSetIntAction(const tString& asPropName, int alX)
{
	return hplNew(cEdActionGroupSetInt,(GetWorld(), mlstObjectIDs, asPropName, alX));
}

iEdAction* cEdSelection::CreateSetFloatAction(const tString& asPropName, float afX)
{
	return hplNew(cEdActionGroupSetFloat,(GetWorld(), mlstObjectIDs, asPropName, afX));
}

iEdAction* cEdSelection::CreateSetStringAction(const tString& asPropName, const tString& asX)
{
	return hplNew(cEdActionGroupSetString,(GetWorld(), mlstObjectIDs, asPropName, asX));
}

iEdAction* cEdSelection::CreateSetVector2fAction(const tString& asPropName, const cVector2f& avX)
{
	return hplNew(cEdActionGroupSetVec2f,(GetWorld(), mlstObjectIDs, asPropName, avX));
}

iEdAction* cEdSelection::CreateSetVector3fAction(const tString& asPropName, const cVector3f& avX)
{
	return hplNew(cEdActionGroupSetVec3f,(GetWorld(), mlstObjectIDs, asPropName, avX));
}

iEdAction* cEdSelection::CreateSetColorAction(const tString& asPropName, const cColor& aX)
{
	return hplNew(cEdActionGroupSetColor,(GetWorld(), mlstObjectIDs, asPropName, aX));
}

//----------------------------------------------------------------------

iEdAction* cEdSelection::CreateSetVector3fAction(int alPropID, const cVector3f& avX)
{
	iEdAction* pAction = NULL;

	switch(alPropID)
	{
	case eScnVec3f_Translation:
		pAction = hplNew(cEdActionTranslateSelection,(this, mbGlobalTransform, avX));	break;
	case eScnVec3f_Rotation:
		pAction = hplNew(cEdActionRotateSelection,(this, mbGlobalTransform, avX));		break;
	case eScnVec3f_Scale:
		pAction = hplNew(cEdActionScaleSelection,(this, mbGlobalTransform, avX));		break;
	}

	return pAction;
}

//----------------------------------------------------------------------

iEdEditPane* cEdSelection::CreateEditPane()
{
	if(HasComponents()==false)
		return NULL;

	if(GetNumComponents()>1)
		return static_cast<cEdSelectionType*>(mpType)->CreateGroupEditPane(this);
	else
		return mlstComponents.front()->CreateEditPane();
}

//----------------------------------------------------------------------

cBoundingVolume* cEdSelection::GetBoundingVolume()
{
	if(mbBVUpdated)
	{
		mbBVUpdated = false;
		UpdateBV();
	}

	return &mBV;
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

bool cEdSelection::OnUpdate(bool abForceUpdate)
{
	//Log("Updating EdSelection - ");
	if(iBaseAggregateObject::OnUpdate(abForceUpdate)==false)
	{
		//Log("Base aggregate update failed\n");
		return false;
	}

	mBV.SetTransform(GetWorldMatrix());
	mBV.SetPosition(0);
	//Log("Base aggregate update Success\n");

	/*tScnObjListIt it = mlstComponents.begin();
	tMatrixfListIt itTranslations = mlstRelTranslationMatrices.begin();
	for(;it!=mlstComponents.end(); ++it, ++itTranslations)
	{
		iEdScnObject* pObj = *it;
		const cMatrixf& mtxRelTranslation = *itTranslations;

		pObj->SetTranslationMatrix(cMath::MatrixMul(GetTranslationMatrix(), mtxRelTranslation));
	}
	*/

	return true;
}

void cEdSelection::OnDraw(const cModuleDrawData& aData)
{
}

void cEdSelection::OnAddComponent(iEdScnObject* apObj)
{
	mlstObjectIDs.push_back(apObj->GetID());
	apObj->SetSelected(true);

	if(mbChangeEditMode)
	{
		iEditor* pEditor = mpEditMode->GetEditor();
		pEditor->SetActiveEditMode(mpEditMode);
		pEditor->BroadcastModuleMsg(eEdModuleMsg_WorldSelectChange, NULL);
	}

	static_cast<cEdSelectionType*>(mpType)->UpdateType();
}

void cEdSelection::OnRemoveComponent(iEdScnObject* apObj)
{
	mlstObjectIDs.remove(apObj->GetID());
	apObj->SetSelected(false);

	if(mbChangeEditMode)
	{
		iEditor* pEditor = mpEditMode->GetEditor();
		pEditor->SetActiveEditMode(mpEditMode);
		pEditor->BroadcastModuleMsg(eEdModuleMsg_WorldSelectChange, NULL);
	}

	static_cast<cEdSelectionType*>(mpType)->UpdateType();
}

//----------------------------------------------------------------------

void cEdSelection::UpdateComponents()
{
	mbUpdatingComponents = true;

	mlstRelTranslationMatrices.clear();
	mlstRelRotationMatrices.clear();
	mlstRelScaleMatrices.clear();
	mlstRelWorldMatrices.clear();

	cVector3f vMax = -999999999.9f;
	cVector3f vMin = 999999999.9f;

	tScnObjListIt it = mlstComponents.begin();
	for(;it!=mlstComponents.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		cMath::ExpandAABB(vMin, vMax, pObj->GetTranslation(), pObj->GetTranslation());
	}

	SetTranslation((vMin+vMax)*0.5f);
	
	if(GetNumComponents()==1)
	{
		iEdScnObject* pObj = GetComponent(0);
		SetRotation(pObj->GetRotation());
		SetScale(pObj->GetScale());
	}
	else
	{
		if(mbKeepTransform)
		{
			mbKeepTransform = false;
			SetRotation(mvStoredRotate);
			SetScale(mvStoredScale);
		}
		else
		{
			SetRotation(0);
			SetScale(1);
		}
	}

	mmtxInvTranslateMatrix = cMath::MatrixInverse(GetTranslationMatrix());
	mmtxInvRotateMatrix = cMath::MatrixInverse(GetRotationMatrix());
	mmtxInvScaleMatrix  = cMath::MatrixInverse(GetScaleMatrix());
	mmtxInvWorldMatrix	= cMath::MatrixInverse(GetWorldMatrix());

	it = mlstComponents.begin();
	for(;it!=mlstComponents.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		mlstRelTranslationMatrices.push_back(pObj->GetRelativeTranslationMatrix(mmtxInvTranslateMatrix));
		mlstRelRotationMatrices.push_back(pObj->GetRelativeRotationMatrix(mmtxInvRotateMatrix));
		mlstRelScaleMatrices.push_back(pObj->GetRelativeScaleMatrix(mmtxInvScaleMatrix));
		mlstRelWorldMatrices.push_back(pObj->GetRelativeWorldMatrix(mmtxInvWorldMatrix));
	}

	mbUpdatingComponents = false;
}

//----------------------------------------------------------------------

void cEdSelection::UpdateBV()
{
	cVector3f vMax = -999999999.9f;
	cVector3f vMin = 999999999.9f;

	tScnObjListIt it = mlstComponents.begin();
	for(;it!=mlstComponents.end(); ++it)
	{
		iEdScnObject* pObj = *it;

		cMath::ExpandAABB(vMin, vMax, pObj->GetTranslation(), pObj->GetTranslation());
	}

	cVector3f vPos = (vMin+vMax)*0.5f;
	mBV.SetLocalMinMax(vMin-vPos, vMax-vPos);
	mBV.SetPosition(vPos);
}

/*
void cEdSelection::AddEntitiesByID(const tIntList& alstIDs)
{
	iEdWorld* pWorld = mpEditor->GetEdWorld();

	ClearEntities();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEdObject* pEnt = pWorld->GetObject(*it);
		AddEntity(pEnt);
	}
}

void cEdSelection::RemoveEntitiesByID(const tIntList& alstIDs)
{
	iEdWorld* pWorld = mpEditor->GetEdWorld();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEdObject* pEnt = pWorld->GetObject(*it);
		RemoveEntity(pEnt);
	}
}

void cEdSelection::ToggleEntitySelectionByID(const tIntList& alstIDs)
{
	iEdWorld* pWorld = mpEditor->GetEdWorld();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEdObject* pEnt = pWorld->GetObject(*it);
		if(HasEntity(pEnt))
			RemoveEntity(pEnt);
		else
            AddEntity(pEnt);
	}
}
*/

//----------------------------------------------------------------------
