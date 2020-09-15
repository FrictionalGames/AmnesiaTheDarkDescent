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

#include "../LevelEditor/HPLMapActions.h"

#include "../LevelEditor/HPLMap.h"
#include "../Common/EdScnObject.h"

//#include "../common/EditorEditModeSelect.h"
//#include "../common/EntityWrapper.h"

//#include "../common/EditorWorld.h"
//#include "../common/EditorSelection.h"

//#include "../common/EntityWrapperDecal.h"
//#include "../common/EntityWrapperStaticObject.h"

//--------------------------------------------------------------------------------------

cEdActionMapAddGroup::cEdActionMapAddGroup(cHplMap* apMap) : iEdActionWorldModifier(_W("Add group"), apMap)
{
	mlID = apMap->GetFreeGroupID();
	msName = "Group " + cString::ToString((int)mlID);
	/*tEntityWrapperList& lstEnts = mpEditor->GetSelection()->GetEntities();
	tEntityWrapperListIt it = lstEnts.begin();
	for(;it!=lstEnts.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		mvEntityIDs.push_back(pEnt->GetID());
		mvEntityGroups.push_back(pData->mlGroupID);
	}
	*/
}

//--------------------------------------------------------------------------------------

void cEdActionMapAddGroup::DoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->AddGroup(mlID);
	pGroup->SetName(msName);

	/*((cLevelEditor*)mpEditor)->AddGroup(mlID, msName);
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();
		
		pData->mlGroupID = mlID;
	}

	mpEditor->GetEditorWorld()->IncModifications();
	*/
}

//--------------------------------------------------------------------------------------

void cEdActionMapAddGroup::UndoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	pMap->RemoveGroup(mlID);
	
	/*
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mvEntityGroups[i];
	}
	((cLevelEditor*)mpEditor)->RemoveGroup(mlID);
	
	mpEditor->GetEditorWorld()->DecModifications();
	*/
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionGroupAddObjects::cEdActionGroupAddObjects(cEdGroup* apGroup, const tScnObjList& alstObjects) : iEdActionWorldModifier(_W("Add selected entities to group"), apGroup->GetWorld())
{
	mlID = apGroup->GetID();

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end(); ++it)
	{
		iEdScnObject* pObj = *it;
		mvObjectIDs.push_back(pObj->GetID());
		// retrieve current group from extension data

	}
}

//--------------------------------------------------------------------------------------

void cEdActionGroupAddObjects::DoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->GetGroupByID(mlID);

	for(size_t i=0; i<mvObjectIDs.size(); ++i)
	{
		iEdScnObject* pObj = pMap->GetScnObject(mvObjectIDs[i]);

		pGroup->AddObject(pObj);
	}
}

//--------------------------------------------------------------------------------------

void cEdActionGroupAddObjects::UndoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	for(size_t i=0;i<mvObjectIDs.size();++i)
	{
		iEdScnObject* pObj = pMap->GetScnObject(mvObjectIDs[i]);
		cEdGroup* pOldGroup = pMap->GetGroupByID(mvObjectOldGroups[i]);

		pOldGroup->AddObject(pObj);
		//iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		//cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		//pData->mlGroupID = mvEntityGroups[i];
	}
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionMapDeleteGroup::cEdActionMapDeleteGroup(cHplMap* apMap, unsigned int alID) : iEdActionWorldModifier(_W("Delete group"), apMap)
{
	mlID = alID;

	cEdGroup* pGroup = apMap->GetGroupByID(mlID);
	if(pGroup)
	{
		msName = pGroup->GetName();
		mbVisible = pGroup->IsVisible();

		const tScnObjList& lstObjects = pGroup->GetObjects();
		tScnObjList::const_iterator it = lstObjects.begin();
		for(;it!=lstObjects.end(); ++it)
		{
			iEdScnObject* pObj = *it;
			mvObjectIDs.push_back(pObj->GetID());
		}
	}
}

//--------------------------------------------------------------------------------------

void cEdActionMapDeleteGroup::DoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	pMap->RemoveGroup(mlID);
}

//--------------------------------------------------------------------------------------

void cEdActionMapDeleteGroup::UndoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->AddGroup(mlID);
	if(pGroup)
	{
		for(size_t i=0; i<mvObjectIDs.size(); ++i)
		{
			iEdScnObject* pObj = pMap->GetScnObject(mvObjectIDs[i]);
			pGroup->AddObject(pObj);
		}
		pGroup->SetName(msName);
		pGroup->SetVisible(mbVisible);
	}
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
/*
cLevelEditorActionGroupDeselectEntities::cLevelEditorActionGroupDeselectEntities(iEditorBase* apEditor, unsigned int alID) : iEditorAction("Remove group entities from selection")
{
	mpEditor = apEditor;

	mlID = alID;

	mpEditMode = (cEditorEditModeSelect*) mpEditor->GetEditMode("Select");

	tEntityWrapperMapIt it = mpEditor->GetEditorWorld()->GetEntities().begin();

	for(;it!=mpEditor->GetEditorWorld()->GetEntities().end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		if(pData->mlGroupID==mlID)
			mvEntityIDs.push_back(pEnt->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupDeselectEntities::Do()
{
	if(mpEditor->GetCurrentEditMode()!=mpEditMode)
		mpEditor->SetCurrentEditMode(mpEditMode);

	cEditorSelection* pSelection = mpEditor->GetSelection();
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		pSelection->RemoveEntity(pEnt);
	}

	mpEditor->GetEditorWorld()->IncModifications();

	mpEditMode->SetEditBoxNeedsRefresh(true);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupDeselectEntities::Undo()
{
	if(mpEditor->GetCurrentEditMode()!=mpEditMode)
		mpEditor->SetCurrentEditMode(mpEditMode);

	cEditorSelection* pSelection = mpEditor->GetSelection();
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		pSelection->AddEntity(pEnt);
	}

	mpEditor->GetEditorWorld()->DecModifications();

	mpEditMode->SetEditBoxNeedsRefresh(true);
}
*/

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionGroupRemoveObjects::cEdActionGroupRemoveObjects(cEdGroup* apGroup, const tScnObjList& alstObjects) : iEdActionWorldModifier(_W("Remove selected entities from group"), apGroup->GetWorld())
{
	mlID = apGroup->GetID();

	tScnObjList::const_iterator it = alstObjects.begin();
	for(;it!=alstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;
		mvObjectIDs.push_back(pObj->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cEdActionGroupRemoveObjects::DoModify()
{
	/*
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = 0;
	}

	mpEditor->GetEditorWorld()->IncModifications();
	*/
}

//--------------------------------------------------------------------------------------

void cEdActionGroupRemoveObjects::UndoModify()
{
	/*
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mlID;
	}

	mpEditor->GetEditorWorld()->DecModifications();
	*/
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

/*
cLevelEditorActionGroupSelectEntities::cLevelEditorActionGroupSelectEntities(iEditorBase* apEditor, unsigned int alID) : iEditorAction("Add selected entities to group")
{
	mpEditor = apEditor;

	mlID = alID;

	mpEditMode = (cEditorEditModeSelect*) mpEditor->GetEditMode("Select");

	tEntityWrapperMapIt it = mpEditor->GetEditorWorld()->GetEntities().begin();

	for(;it!=mpEditor->GetEditorWorld()->GetEntities().end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		if(pData->mlGroupID==mlID)
			mvEntityIDs.push_back(pEnt->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSelectEntities::Do()
{
	if(mpEditor->GetCurrentEditMode()!=mpEditMode)
		mpEditor->SetCurrentEditMode(mpEditMode);

	cEditorSelection* pSelection = mpEditor->GetSelection();
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		pSelection->AddEntity(pEnt);
	}

	mpEditMode->SetEditBoxNeedsRefresh(true);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSelectEntities::Undo()
{
	if(mpEditor->GetCurrentEditMode()!=mpEditMode)
		mpEditor->SetCurrentEditMode(mpEditMode);

	cEditorSelection* pSelection = mpEditor->GetSelection();
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		pSelection->RemoveEntity(pEnt);
	}

	mpEditMode->SetEditBoxNeedsRefresh(true);
}
*/


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


cEdActionGroupSetName::cEdActionGroupSetName(cEdGroup* apGroup, const tString& asName) : iEdActionWorldModifier(_W("Add group"), apGroup->GetWorld())
{
	mlID = apGroup->GetID();
 
	msNewName = asName;
	msOldName = apGroup->GetName();
}

//--------------------------------------------------------------------------------------

void cEdActionGroupSetName::DoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->GetGroupByID(mlID);
	pGroup->SetName(msNewName);
}

//--------------------------------------------------------------------------------------

void cEdActionGroupSetName::UndoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->GetGroupByID(mlID);
	pGroup->SetName(msOldName);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionGroupSetVisible::cEdActionGroupSetVisible(cEdGroup* apGroup, bool abNewValue) : iEdActionWorldModifier(_W("Add group"), apGroup->GetWorld())
{
	mlID = apGroup->GetID();
 
	mbNewValue = abNewValue;
	mbOldValue = apGroup->IsVisible();
}

//--------------------------------------------------------------------------------------

void cEdActionGroupSetVisible::DoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->GetGroupByID(mlID);
	pGroup->SetVisible(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionGroupSetVisible::UndoModify()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	cEdGroup* pGroup = pMap->GetGroupByID(mlID);
	pGroup->SetVisible(mbOldValue);
}

//--------------------------------------------------------------------------------------
/*
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetGlobalMaxDecalTris::cLevelEditorActionSetGlobalMaxDecalTris(iEdWorld* apWorld, int alMaxTris) : iEdActionWorldModifier("Set Max triangles per decal", apWorld)
{
	mOldValue = cEntityWrapperTypeDecal::GetGlobalMaxTriangles();
	mNewValue = alMaxTris;
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetGlobalMaxDecalTris::DoModify()
{
	Apply(mNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetGlobalMaxDecalTris::UndoModify()
{
	Apply(mOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetGlobalMaxDecalTris::Apply(const int& aX)
{
	cEntityWrapperTypeDecal::SetGlobalMaxTriangles(aX);
	cEntityWrapperTypeDecal::SetForcingUpdate(true);

	tEntityWrapperMap& mapEnts = mpEditorWorld->GetEntities();
	tEntityWrapperMapIt it = mapEnts.begin();
	for(;it!=mapEnts.end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		if(pEnt->GetTypeID()==eEditorEntityType_Decal)
			pEnt->UpdateEntity();
	}

	cEntityWrapperTypeDecal::SetForcingUpdate(false);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
*/
cEdActionMapAddStaticObjectCombo::cEdActionMapAddStaticObjectCombo(iEdWorld* apWorld) : iEdActionWorldModifier(_W("Add static object combo"), apWorld)
{
	cHplMap* pMap = static_cast<cHplMap*>(apWorld);

	mlNewComboID = pMap->GetFreeComboID();
}

//--------------------------------------------------------------------------------------

void cEdActionMapAddStaticObjectCombo::DoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);

	cLevelEditorStaticObjectCombo* pCombo = pWorld->CreateStaticObjectCombo(mlNewComboID);
	pWorld->AddStaticObjectCombo(pCombo);
}

//--------------------------------------------------------------------------------------

void cEdActionMapAddStaticObjectCombo::UndoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);
	pWorld->RemoveStaticObjectCombo(mlNewComboID);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionMapRemoveStaticObjectCombo::cEdActionMapRemoveStaticObjectCombo(iEdWorld* apWorld, int alComboID) : iEdActionWorldModifier(_W("Remove combo"), apWorld)
{
	cHplMap* pWorld = static_cast<cHplMap*>(apWorld);

	mlComboID = -1;
	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(alComboID);
	if(pCombo==NULL)
		return;

	mlComboID = alComboID;
	mColor = pCombo->GetColor();
	const tScnObjList& lstObjs = pCombo->GetObjects();
	tScnObjList::const_iterator itObjs = lstObjs.begin();
	for(;itObjs!=lstObjs.end();++itObjs)
	{
		iEdScnObject* pObj = *itObjs;

		mvObjIds.push_back(pObj->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cEdActionMapRemoveStaticObjectCombo::DoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);

	pWorld->RemoveStaticObjectCombo(mlComboID);
}

//--------------------------------------------------------------------------------------

void cEdActionMapRemoveStaticObjectCombo::UndoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);

	cLevelEditorStaticObjectCombo* pCombo = pWorld->CreateStaticObjectCombo(mlComboID);
    if(pCombo==NULL)
		return;

	pCombo->SetColor(mColor);
	for(size_t i=0;i<mvObjIds.size();++i)
	{
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pWorld->GetObject(mvObjIds[i]));

		pCombo->AddObject(pObj);
	}

	pWorld->AddStaticObjectCombo(pCombo);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionComboAddObject::cEdActionComboAddObject(cLevelEditorStaticObjectCombo* apCombo, iEdScnObject* apObj) : iEdActionWorldModifier(_W("Add object to combo"), apCombo->GetWorld())
{
	////////////////////////////////////////////////////////////
	// Condition for the action to be invalid is objectID==-1
	// if no object, or already in combo, invalidate!
	if(apObj==NULL || apCombo->HasObject(apObj))
	{
		mlObjectID = -1;
		return;
	}

	mlObjectID = apObj->GetID();
	mlNewComboID = apCombo->GetID();
	mlOldComboID = -1;

	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);

	const tStaticObjectComboList& lstCombos = pWorld->GetStaticObjectCombos();
	tStaticObjectComboList::const_iterator it = lstCombos.begin();
	for(;it!=lstCombos.end();++it)
	{
		cLevelEditorStaticObjectCombo* pCombo = *it;
		if(pCombo->HasObject(apObj))
		{
			mlOldComboID = pCombo->GetID();
			break;
		}
	}
}

//--------------------------------------------------------------------------------------

void cEdActionComboAddObject::DoModify()
{
	Apply(mlOldComboID, mlNewComboID);
}

//--------------------------------------------------------------------------------------

void cEdActionComboAddObject::UndoModify()
{
	Apply(mlNewComboID, mlOldComboID);
}

//--------------------------------------------------------------------------------------

void cEdActionComboAddObject::Apply(int alComboToRemove, int alComboToAdd)
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);
	iEdScnObject* pObject = static_cast<iEdScnObject*>(pWorld->GetObject(mlObjectID));

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(alComboToRemove);
	if(pCombo) pCombo->RemoveObject(pObject);

	pCombo = pWorld->GetStaticObjectCombo(alComboToAdd);
	if(pCombo) pCombo->AddObject(pObject);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionComboRemoveObject::cEdActionComboRemoveObject(cLevelEditorStaticObjectCombo* apCombo, iEdScnObject* apObj) : iEdActionWorldModifier(_W("Add object to combo"), apCombo->GetWorld())
{
	////////////////////////////////////////////////////////////
	// Condition for the action to be invalid is objectID==-1
	// if no object, or already in combo, invalidate!
	if(apObj==NULL || apCombo->HasObject(apObj)==false)
	{
		mlObjectID = -1;
		return;
	}

	mlObjectID = apObj->GetID();
	mlComboID = apCombo->GetID();
}

//--------------------------------------------------------------------------------------

void cEdActionComboRemoveObject::DoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);
	iEdScnObject* pObject = static_cast<iEdScnObject*>(pWorld->GetObject(mlObjectID));

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	if(pCombo) pCombo->RemoveObject(pObject);
}

//--------------------------------------------------------------------------------------

void cEdActionComboRemoveObject::UndoModify()
{
	cHplMap* pWorld = static_cast<cHplMap*>(mpWorld);
	iEdScnObject* pObject = static_cast<iEdScnObject*>(pWorld->GetObject(mlObjectID));

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	if(pCombo) pCombo->AddObject(pObject);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionComboSetColor::cEdActionComboSetColor(cLevelEditorStaticObjectCombo* apCombo, const cColor& aCol) : iEdActionWorldModifier(_W("Set combo color"), apCombo->GetWorld())
{
	cHplMap* pWorld = apCombo->GetWorld();

	mlComboID = apCombo->GetID();
	mNewColor = aCol;

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	mOldColor = pCombo->GetColor();
}

//--------------------------------------------------------------------------------------

void cEdActionComboSetColor::DoModify()
{
	Apply(mNewColor);
}

//--------------------------------------------------------------------------------------

void cEdActionComboSetColor::UndoModify()
{
	Apply(mOldColor);
}

//--------------------------------------------------------------------------------------

void cEdActionComboSetColor::Apply(const cColor& aX)
{
	cHplMap* pMap= static_cast<cHplMap*>(mpWorld);
	cLevelEditorStaticObjectCombo* pCombo = pMap->GetStaticObjectCombo(mlComboID);

	if(pCombo) pCombo->SetColor(aX);
}

//--------------------------------------------------------------------------------------


