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

#include "../Common/EdScnObjectActions.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdObjCompound.h"
#include "../Common/EdScnWorld.h"

#include "../Common/Editor.h"

#include <algorithm>

iEdActionSetObjMatrix::iEdActionSetObjMatrix(const tWString& asName, 
											 iEdWorld* apWorld, 
											 iEdScnObject* apObj, const cMatrixf& amtxX) : iEdActionWorldModifier(asName, apWorld)
{
	mpObject = apObj;
	
	mmtxNewMatrix = amtxX;
}

bool iEdActionSetObjMatrix::Init()
{
	if(mpObject==NULL)
		return false;
	mlID = mpObject->GetID();
	mmtxOldMatrix = GetMatrix(mpObject);

	if(mmtxOldMatrix==mmtxNewMatrix)
		return false;

	return true;
}

void iEdActionSetObjMatrix::DoModify()
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mlID));
	SetMatrix(pObj, mmtxNewMatrix);
}

void iEdActionSetObjMatrix::UndoModify()
{
	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mlID));
	SetMatrix(pObj, mmtxOldMatrix);
}

cEdActionSetTranslationMatrix::cEdActionSetTranslationMatrix(iEdWorld* apWorld, 
															 iEdScnObject* apObj, const cMatrixf& amtxX) : iEdActionSetObjMatrix(_W("Translation"), apWorld, apObj, amtxX)
{
}

const cMatrixf& cEdActionSetTranslationMatrix::GetMatrix(iEdScnObject* apObj)
{
	return apObj->GetTranslationMatrix();
}

void cEdActionSetTranslationMatrix::SetMatrix(iEdScnObject* apObj, const cMatrixf& amtxX)
{
	apObj->SetTranslationMatrix(amtxX);
}


cEdActionSetRotationMatrix::cEdActionSetRotationMatrix(iEdWorld* apWorld, 
															 iEdScnObject* apObj, const cMatrixf& amtxX) : iEdActionSetObjMatrix(_W("Rotation"), apWorld, apObj, amtxX)
{
}

const cMatrixf& cEdActionSetRotationMatrix::GetMatrix(iEdScnObject* apObj)
{
	return apObj->GetRotationMatrix();
}

void cEdActionSetRotationMatrix::SetMatrix(iEdScnObject* apObj, const cMatrixf& amtxX)
{
	apObj->SetRotationMatrix(amtxX);
}

cEdActionSetScaleMatrix::cEdActionSetScaleMatrix(iEdWorld* apWorld, 
															 iEdScnObject* apObj, const cMatrixf& amtxX) : iEdActionSetObjMatrix(_W("Scale"), apWorld, apObj, amtxX)
{
}

const cMatrixf& cEdActionSetScaleMatrix::GetMatrix(iEdScnObject* apObj)
{
	return apObj->GetScaleMatrix();
}

void cEdActionSetScaleMatrix::SetMatrix(iEdScnObject* apObj, const cMatrixf& amtxX)
{
	apObj->SetRotationMatrix(amtxX);
}

cEdActionSetWorldMatrix::cEdActionSetWorldMatrix(iEdWorld* apWorld, 
															 iEdScnObject* apObj, const cMatrixf& amtxX) : iEdActionSetObjMatrix(_W("World"), apWorld, apObj, amtxX)
{
}

const cMatrixf& cEdActionSetWorldMatrix::GetMatrix(iEdScnObject* apObj)
{
	return apObj->GetWorldMatrix();
}

void cEdActionSetWorldMatrix::SetMatrix(iEdScnObject* apObj, const cMatrixf& amtxX)
{
	apObj->SetWorldMatrix(amtxX);
}


//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

iEdActionAggregateAddComponents::iEdActionAggregateAddComponents(iBaseAggregateObject* apObject, const tScnObjList& alstComponents) : iEdActionWorldModifier(_W("Add entities to Compound Object"), apObject->GetWorld())
{
	mlID = apObject->GetID();
	tScnObjList::const_iterator it = alstComponents.begin();
	for(; it!=alstComponents.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		mvComponentIDs.push_back(pObj->GetID());
	}
}

//---------------------------------------------------------------------

iEdActionAggregateAddComponents::~iEdActionAggregateAddComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEdActionAggregateAddComponents::Init()
{
	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		int lID = mvComponentIDs[i];
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(lID));
		iEdObjectData* pData = NULL;
		iBaseAggregateObject* pAggregate = GetAggregateFromObject(pObj);
		if(pAggregate)
			pData = pAggregate->CreateCopyData();

		mvOldAggregateData.push_back(pData);
	}

	return true;
}

//---------------------------------------------------------------------

void iEdActionAggregateAddComponents::DoModify()
{
	iBaseAggregateObject* pAggregate = static_cast<iBaseAggregateObject*>(mpWorld->GetObject(mlID));
	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mvComponentIDs[i]));
		iBaseAggregateObject* pOldAggregate = GetAggregateFromObject(pObj);
		pAggregate->AddComponent(pObj);

		if(pOldAggregate && pOldAggregate->HasComponents()==false)
			mpWorld->DestroyObj(pOldAggregate);
	}
	pAggregate->SetUpdated();
}

//---------------------------------------------------------------------

void iEdActionAggregateAddComponents::UndoModify()
{
	iBaseAggregateObject* pAggregate = static_cast<iBaseAggregateObject*>(mpWorld->GetObject(mlID));
	tScnObjList lstOldAggregates;
	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mvComponentIDs[i]));
		iBaseAggregateObject* pOldAggregate = NULL;
		iEdObjectData* pOldAggregateData = mvOldAggregateData[i];
		if(pOldAggregateData)
			pOldAggregate = static_cast<iBaseAggregateObject*>(mpWorld->GetObject(pOldAggregateData->GetID()));

		pAggregate->RemoveComponent(pObj);

		if(pOldAggregate==NULL)
			pOldAggregate = static_cast<iBaseAggregateObject*>(mpWorld->CreateObjFromData(pOldAggregateData));

		if(pOldAggregate)
		{
			pOldAggregate->AddComponent(pObj);
			lstOldAggregates.push_back(pOldAggregate);
		}
	}

	tScnObjListIt itAggregate = lstOldAggregates.begin();
	for(;itAggregate!=lstOldAggregates.end();++itAggregate)
	{
		iEdScnObject* pOldAggregate = *itAggregate;
		pOldAggregate->PostDeployAll(false);
		pOldAggregate->SetUpdated();
	}

	if(pAggregate->HasComponents()==false)
		mpWorld->DestroyObj(pAggregate);
	else
		pAggregate->SetUpdated();
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------



iEdActionAggregateRemoveComponents::iEdActionAggregateRemoveComponents(const tScnObjList& alstObjects) : iEdActionWorldModifier(_W("Detach entities from aggregate"), alstObjects.front()->GetWorld())
{
	tScnObjList::const_iterator it = alstObjects.begin();
	for(; it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		mvComponentIDs.push_back(pObj->GetID());
	}
}

//---------------------------------------------------------------------

iEdActionAggregateRemoveComponents::~iEdActionAggregateRemoveComponents()
{
	STLDeleteAll(mvOldAggregateData);
}

//---------------------------------------------------------------------

bool iEdActionAggregateRemoveComponents::Init()
{
	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mvComponentIDs[i]));

		iBaseAggregateObject* pAggregate = GetAggregateFromObject(pObj);
		iEdObjectData* pData = NULL;
		if(pAggregate)
			pData = pAggregate->CreateCopyData();

		mvOldAggregateData.push_back(pData);
	}
	
	return true;
}
//---------------------------------------------------------------------

void iEdActionAggregateRemoveComponents::DoModify()
{
	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mvComponentIDs[i]));
		iBaseAggregateObject* pAggregate = GetAggregateFromObject(pObj);
		if(pAggregate)
		{
			//pSelection->RemoveEntity(pAggregate);

			pAggregate->RemoveComponent(pObj);
			if(pAggregate->HasComponents()==false)
				mpWorld->DestroyObj(pAggregate);

			//pSelection->AddEntity(pEnt);
		}
	}
}

//---------------------------------------------------------------------

void iEdActionAggregateRemoveComponents::UndoModify()
{
	//cEditorSelection* pSelection = mpEditorWorld->GetEditor()->GetSelection();
	//pSelection->ClearEntities();

	for(size_t i=0;i<mvComponentIDs.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(mvComponentIDs[i]));
		iEdObjectData* pData = mvOldAggregateData[i];
		iBaseAggregateObject* pAggregate = static_cast<iBaseAggregateObject*>(mpWorld->GetObject(pData->GetID()));
		if(pAggregate==NULL)
			pAggregate = static_cast<iBaseAggregateObject*>(mpWorld->CreateObjFromData(pData));

		pAggregate->AddComponent(pObj);

//		pSelection->AddEntity(pAggregate);
	}
}

//---------------------------------------------------------------------

cEdActionCompoundAddComponents::cEdActionCompoundAddComponents(iBaseAggregateObject* apObj, 
															   const tScnObjList& alstObjects) : iEdActionAggregateAddComponents(apObj, alstObjects)
{
}

iBaseAggregateObject* cEdActionCompoundAddComponents::GetAggregateFromObject(iEdScnObject* apObj)
{
	if(apObj) return apObj->GetCompoundObject();

	return NULL;
}

//---------------------------------------------------------------------

cEdActionCompoundRemoveComponents::cEdActionCompoundRemoveComponents(const tScnObjList& alstObjects) : iEdActionAggregateRemoveComponents(alstObjects)
{
}

iBaseAggregateObject* cEdActionCompoundRemoveComponents::GetAggregateFromObject(iEdScnObject* apObj)
{
	if(apObj) return apObj->GetCompoundObject();

	return NULL;
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------

cEdActionScnObjSetChildren::cEdActionScnObjSetChildren(iEdScnObject* apObj, const tScnObjList& alstChildren) : iEdActionWorldModifier(_W("Set Children"), apObj->GetWorld())
{
	mlID = apObj->GetID();
	mlstNewChildren = alstChildren;
	mlstOldChildren = apObj->GetChildren();
}

bool cEdActionScnObjSetChildren::Init()
{
	if(mlstOldChildren.size()!=mlstNewChildren.size())
		return true;

	tScnObjListIt it = mlstOldChildren.begin();
	for(;it!=mlstOldChildren.end();++it)
	{
		iEdScnObject* pObj = *it;

		tScnObjListIt it = find(mlstNewChildren.begin(), mlstNewChildren.end(), pObj);
		if(it==mlstNewChildren.end())
			return true;
	}

	return false;
}

void cEdActionScnObjSetChildren::DoModify()
{
	Apply(mlstNewChildren);
}

void cEdActionScnObjSetChildren::UndoModify()
{
	Apply(mlstOldChildren);
}

void cEdActionScnObjSetChildren::Apply(const tScnObjList& alstObjects)
{
	iEdScnObject* pObj = static_cast<iEdScnWorld*>(mpWorld)->GetScnObject(mlID);
	pObj->ClearChildren();

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end();++it)
	{
		iEdScnObject* pChild = *it;

		pObj->AddChild(pChild);
	}

	mpWorld->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldObjModify, NULL);
}


//---------------------------------------------------------------------

//---------------------------------------------------------------------

//---------------------------------------------------------------------

cEdActionSetObjectVar::cEdActionSetObjectVar(iUserObject* apObj,
											 const tWString& asName, const tWString& asValue) : iEdActionWorldModifier(_W("Set Object Var"), apObj->GetWorld())
{
	mlID = apObj->GetID();
	msVarName = asName;
	msNewValue = asValue;

	cEdVarInstance* pVar = apObj->GetVar(msVarName);
	if(pVar)
		msOldValue = pVar->GetValue();
}

bool cEdActionSetObjectVar::Init()
{
	return msOldValue!=msNewValue;
}

//---------------------------------------------------------------------

void cEdActionSetObjectVar::DoModify()
{
	Apply(msNewValue);
}

//---------------------------------------------------------------------

void cEdActionSetObjectVar::UndoModify()
{
	Apply(msOldValue);
}

//---------------------------------------------------------------------

void cEdActionSetObjectVar::Apply(const tWString& asX)
{
	iUserObject* pObj = static_cast<iUserObject*>(mpWorld->GetObject(mlID));
	pObj->SetVar(msVarName, asX);

	mpWorld->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldObjModify, NULL);
}

//---------------------------------------------------------------------

//---------------------------------------------------------------------

cEdActionSetObjectUserType::cEdActionSetObjectUserType(iUserObject* apObj, iUserTypeNode* apType) : iEdActionWorldModifier(_W("Set User Type"), apObj->GetWorld())
{
	mlID = apObj->GetID();
	mpNewType = apType;

	mpOldType = static_cast<iUserTypeNode*>(apObj->GetType());
}

bool cEdActionSetObjectUserType::Init()
{
	return mpNewType && mpNewType!=mpOldType && mpOldType->GetUserTypeRoot()==mpNewType->GetUserTypeRoot();
}

void cEdActionSetObjectUserType::DoModify()
{
	Apply(mpNewType);
}

void cEdActionSetObjectUserType::UndoModify()
{
	Apply(mpOldType);
}

void cEdActionSetObjectUserType::Apply(iUserTypeNode* apType)
{
	iUserObject* pObj = static_cast<iUserObject*>(mpWorld->GetObject(mlID));
	pObj->SetType(apType);
}
