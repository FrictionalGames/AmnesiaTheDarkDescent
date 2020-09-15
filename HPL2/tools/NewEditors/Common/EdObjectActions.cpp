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

#include "../Common/EdObjectActions.h"
#include "../Common/EdWorld.h"

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CREATE OBJECT - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cEdActionCreateObject::cEdActionCreateObject(iEdWorld* apWorld, iEdObjectData* apData) : iEdActionWorldModifier(_W("Create object: ") + (apData? apData->GetName() : _W("NULL Data")), 
																												apWorld)
{
	mpNewObjData = apData;
}

cEdActionCreateObject::~cEdActionCreateObject()
{
	if(mpNewObjData) hplDelete(mpNewObjData);
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CREATE OBJECT - PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cEdActionCreateObject::Init()
{
	return (mpNewObjData!=NULL);
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CREATE OBJECT - PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

void cEdActionCreateObject::DoModify()
{
	iEdObject* pNewObj = mpWorld->CreateObjFromData(mpNewObjData);
	if(pNewObj) pNewObj->PostDeployAll(false);
}

//------------------------------------------------------------------

void cEdActionCreateObject::UndoModify()
{
	iEdObject* pNewObject = mpWorld->GetObject(mpNewObjData->GetID());
	if(pNewObject) mpWorld->DestroyObj(pNewObject);
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// DELETE OBJECTS - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cEdActionDeleteObjects::cEdActionDeleteObjects(iEdWorld* apWorld, const tEdObjectList& alstObjects) : iEdActionWorldModifier(_W("Delete objects"), apWorld)
{
	tEdObjectList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdObject* pObj = *it;

		mvBackupObjData.push_back(pObj->CreateCopyData());
	}

}

//------------------------------------------------------------------

cEdActionDeleteObjects::~cEdActionDeleteObjects()
{
	STLDeleteAll(mvBackupObjData);
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// DELETE OBJECTS - PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cEdActionDeleteObjects::Init()
{
	return mvBackupObjData.empty()==false;
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// DELETE OBJECTS - PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

void cEdActionDeleteObjects::DoModify()
{
	for(size_t i=0; i<mvBackupObjData.size(); ++i)
	{
		iEdObjectData* pData = mvBackupObjData[i];
		iEdObject* pObj = mpWorld->GetObject(pData->GetID());

		//mpSelection->RemoveEntity(pEnt);
		mpWorld->DestroyObj(pObj);
	}
}

//------------------------------------------------------------------

void cEdActionDeleteObjects::UndoModify()
{
	tEdObjectList lstRestoredObjects;

	for(size_t i=0; i<mvBackupObjData.size(); ++i)
	{
		iEdObjectData* pData = mvBackupObjData[i];

		iEdObject* pObj = mpWorld->CreateObjFromData(pData);
		if(pObj) lstRestoredObjects.push_back(pObj);
	}

	tEdObjectListIt itRestoredObjs = lstRestoredObjects.begin();
	for(;itRestoredObjs!=lstRestoredObjects.end();++itRestoredObjs)
	{
		iEdObject* pObj = *itRestoredObjs;
		pObj->PostDeployAll(false);
	}
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CLONE OBJECTS - CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cEdActionCloneObjects::cEdActionCloneObjects(iEdWorld* apWorld, const tEdObjectList& alstObjects, const tWString& asPrefix, const tWString& asSuffix) : iEdActionWorldModifier(_W("Delete objects"), apWorld)
{
	tEdObjectList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdObject* pObj = *it;

		mvObjectIDs.push_back(pObj->GetID());
		mvClonedEntityIDs.push_back(pObj->GetCloneIDs());
	}

	msNamePrefix = asPrefix;
	msNameSuffix = asSuffix;
}

//------------------------------------------------------------------

cEdActionCloneObjects::~cEdActionCloneObjects()
{
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CLONE OBJECTS - PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cEdActionCloneObjects::Init()
{
	return mvObjectIDs.empty()==false;
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CLONE OBJECTS - PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

void cEdActionCloneObjects::DoModify()
{
	mlstClones.clear();
	bool bSaveCloneIDs = mvCloneIDs.empty();

	for(size_t i=0; i<mvObjectIDs.size(); ++i)
	{
		iEdObject* pObj = mpWorld->GetObject(mvObjectIDs[i]);
		const tIntVec& vCloneIDs = mvClonedEntityIDs[i];

		iEdObject* pClone = pObj->Clone(vCloneIDs, false, true, msNamePrefix, msNameSuffix);
		mlstClones.push_back(pClone);
		if(bSaveCloneIDs) mvCloneIDs.push_back(pClone->GetID());
	}
	tEdObjectListIt it = mlstClones.begin();
	for(;it!=mlstClones.end(); ++it)
	{
		iEdObject* pObj = *it;
		pObj->PostDeployAll(true);
	}
}

//------------------------------------------------------------------

void cEdActionCloneObjects::UndoModify()
{
	for(size_t i=0; i<mvCloneIDs.size(); ++i)
	{
		int lID = mvCloneIDs[i];

		iEdObject* pClone = mpWorld->GetObject(lID);
		mpWorld->DestroyObj(pClone);
	}
}

//------------------------------------------------------------------
