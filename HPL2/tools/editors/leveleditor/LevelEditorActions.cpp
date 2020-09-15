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

#include "LevelEditorActions.h"

#include "../common/EditorEditModeSelect.h"
#include "../common/EntityWrapper.h"

#include "../common/EditorWorld.h"
#include "../common/EditorSelection.h"

#include "../common/EntityWrapperDecal.h"
#include "../common/EntityWrapperStaticObject.h"

#include "LevelEditor.h"
#include "LevelEditorWorld.h"

//--------------------------------------------------------------------------------------

cLevelEditorActionGroupAdd::cLevelEditorActionGroupAdd(iEditorBase* apEditor, unsigned int alID, const tString& asName) : iEditorAction("Add group")
{
	mpEditor = apEditor;
	mlID = alID;
	msName = asName;

	tEntityWrapperList& lstEnts = mpEditor->GetSelection()->GetEntities();
	tEntityWrapperListIt it = lstEnts.begin();
	for(;it!=lstEnts.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		mvEntityIDs.push_back(pEnt->GetID());
		mvEntityGroups.push_back(pData->mlGroupID);
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupAdd::Do()
{
	((cLevelEditor*)mpEditor)->AddGroup(mlID, msName);
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();
		
		pData->mlGroupID = mlID;
	}

	mpEditor->GetEditorWorld()->IncModifications();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupAdd::Undo()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mvEntityGroups[i];
	}
	((cLevelEditor*)mpEditor)->RemoveGroup(mlID);
	
	mpEditor->GetEditorWorld()->DecModifications();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionGroupAddSelected::cLevelEditorActionGroupAddSelected(iEditorBase* apEditor, unsigned int alID) : iEditorAction("Add selected entities to group")
{
	mpEditor = apEditor;

	mlID = alID;

	tEntityWrapperList& lstEntities = mpEditor->GetSelection()->GetEntities();
	tEntityWrapperListIt it = lstEntities.begin();
	for(;it!=lstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		mvEntityIDs.push_back(pEnt->GetID());
		mvEntityGroups.push_back(pData->mlGroupID);
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupAddSelected::Do()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mlID;
	}

	mpEditor->GetEditorWorld()->IncModifications();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupAddSelected::Undo()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mvEntityGroups[i];
	}

	mpEditor->GetEditorWorld()->DecModifications();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionGroupDelete::cLevelEditorActionGroupDelete(iEditorBase* apEditor, unsigned int alID) : iEditorAction("Delete group")
{
	mpEditor = apEditor;

	mlID = alID;

	cLevelEditorGroup& pGroup = ((cLevelEditor*)mpEditor)->GetGroup(mlID);
	msName = pGroup.GetName();
	mbVis = pGroup.GetVisibility();

	tEntityWrapperMapIt it = mpEditor->GetEditorWorld()->GetEntities().begin();;

	for(;it!=mpEditor->GetEditorWorld()->GetEntities().end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		if(pData->mlGroupID==mlID)
		{
			mvEntityIDs.push_back(pEnt->GetID());
		}
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupDelete::Do()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = 0;
	}
	((cLevelEditor*)mpEditor)->RemoveGroup(mlID);

	mpEditor->GetEditorWorld()->IncModifications();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupDelete::Undo()
{
	((cLevelEditor*)mpEditor)->AddGroup(mlID, msName);
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mlID;
	}
    ((cLevelEditor*)mpEditor)->GetGroup(mlID).SetVisibility(mbVis);	
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


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

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionGroupRemoveSelected::cLevelEditorActionGroupRemoveSelected(iEditorBase* apEditor, unsigned int alID) : iEditorAction("Remove selected entities from group")
{
	mpEditor = apEditor;

	mlID = alID;

	cEditorSelection* pSelection = mpEditor->GetSelection();

	tEntityWrapperListIt it = pSelection->GetEntities().begin();

	for(;it!=pSelection->GetEntities().end();++it)
	{
		iEntityWrapper* pEnt = *it;
		mvEntityIDs.push_back(pEnt->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupRemoveSelected::Do()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = 0;
	}

	mpEditor->GetEditorWorld()->IncModifications();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupRemoveSelected::Undo()
{
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditor->GetEditorWorld()->GetEntity(mvEntityIDs[i]);
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();

		pData->mlGroupID = mlID;
	}

	mpEditor->GetEditorWorld()->DecModifications();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

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


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


cLevelEditorActionGroupSetName::cLevelEditorActionGroupSetName(iEditorBase* apEditor, unsigned int alID, const tString& asName) : iEditorAction("Add group")
{
	mpEditor = apEditor;

	mlID = alID;
 
	msNewName = asName;
	msOldName = ((cLevelEditor*)mpEditor)->GetGroup(mlID).GetName();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSetName::Do()
{
	((cLevelEditor*)mpEditor)->GetGroup(mlID).SetName(msNewName);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSetName::Undo()
{
	((cLevelEditor*)mpEditor)->GetGroup(mlID).SetName(msOldName);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


cLevelEditorActionGroupSetVisibility::cLevelEditorActionGroupSetVisibility(iEditorBase* apEditor, unsigned int alID, bool abNewValue) : iEditorAction("Add group")
{
	mpEditor = apEditor;

	mlID = alID;
 
	mbNewValue = abNewValue;
	mbOldValue = ((cLevelEditor*)mpEditor)->GetGroup(mlID).GetVisibility();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSetVisibility::Do()
{
	((cLevelEditor*)mpEditor)->GetGroup(mlID).SetVisibility(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionGroupSetVisibility::Undo()
{
	((cLevelEditor*)mpEditor)->GetGroup(mlID).SetVisibility(mbOldValue);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetSkyboxActive::cLevelEditorActionSetSkyboxActive(iEditorWorld* apWorld, bool abX) : iEditorActionWorldModifier("Set Skybox active", apWorld)
{
	mbOldValue = mpEditorWorld->GetSkyboxActive();
	mbNewValue = abX;
}


//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxActive::DoModify()
{
	Apply(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxActive::UndoModify()
{
	Apply(mbOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxActive::Apply(bool abX)
{
	mpEditorWorld->SetSkyboxActive(abX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetSkyboxTexture::cLevelEditorActionSetSkyboxTexture(iEditorWorld* apWorld, const tString& asX) : iEditorActionWorldModifier("Set Skybox texture", apWorld)
{
	msOldValue = mpEditorWorld->GetSkyboxTexture();
	msNewValue = asX;
}


//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxTexture::DoModify()
{
	Apply(msNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxTexture::UndoModify()
{
	Apply(msOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxTexture::Apply(const tString& asX)
{
	mpEditorWorld->SetSkyboxTexture(asX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetSkyboxColor::cLevelEditorActionSetSkyboxColor(iEditorWorld* apWorld, const cColor& aX) : iEditorActionWorldModifier("Set Skybox color", apWorld)
{
	mOldValue = mpEditorWorld->GetSkyboxColor();
	mNewValue = aX;
}


//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxColor::DoModify()
{
	Apply(mNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxColor::UndoModify()
{
	Apply(mOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetSkyboxColor::Apply(const cColor& aX)
{
	mpEditorWorld->SetSkyboxColor(aX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetFogBoolProperty::cLevelEditorActionSetFogBoolProperty(iEditorWorld* apWorld, eFogBoolProp aProp, bool abX) : iEditorActionWorldModifier("Set Skybox active", apWorld)
{
	mProp = aProp;

	mbOldValue = GetOldValue();
	mbNewValue = abX;
}


//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogBoolProperty::DoModify()
{
	Apply(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogBoolProperty::UndoModify()
{
	Apply(mbOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogBoolProperty::Apply(bool abX)
{
	switch(mProp)
	{
	case eFogBoolProp_Active:
        mpEditorWorld->SetFogActive(abX);
		break;
	case eFogBoolProp_Culling:
		mpEditorWorld->SetFogCulling(abX);
		break;
	}
}

bool cLevelEditorActionSetFogBoolProperty::GetOldValue()
{
	switch(mProp)
	{
	case eFogBoolProp_Active:
        return mpEditorWorld->GetFogActive();
	case eFogBoolProp_Culling:
		return mpEditorWorld->GetFogCulling();
	}

	return false;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetFogFloatProperty::cLevelEditorActionSetFogFloatProperty(iEditorWorld* apWorld, eFogFloatProp aProp, float afX) : iEditorActionWorldModifier("Set Fog float", apWorld)
{
	mProp = aProp;

	mfOldValue = GetOldValue();
	mfNewValue = afX;
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogFloatProperty::DoModify()
{
	Apply(mfNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogFloatProperty::UndoModify()
{
	Apply(mfOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogFloatProperty::Apply(float afX)
{
	switch(mProp)
	{
	case eFogFloatProp_Start:
        mpEditorWorld->SetFogStart(afX);
		break;
	case eFogFloatProp_End:
		mpEditorWorld->SetFogEnd(afX);
		break;
	case eFogFloatProp_FalloffExp:
		mpEditorWorld->SetFogFalloffExp(afX);
		break;
	}
}

float cLevelEditorActionSetFogFloatProperty::GetOldValue()
{
	switch(mProp)
	{
	case eFogFloatProp_Start:
        return mpEditorWorld->GetFogStart();
	case eFogFloatProp_End:
		return mpEditorWorld->GetFogEnd();
	case eFogFloatProp_FalloffExp:
		return mpEditorWorld->GetFogFalloffExp();
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetFogColor::cLevelEditorActionSetFogColor(iEditorWorld* apWorld, const cColor& aX) : iEditorActionWorldModifier("Set Fog color", apWorld)
{
	mOldValue = mpEditorWorld->GetFogColor();
	mNewValue = aX;
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogColor::DoModify()
{
	Apply(mNewValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogColor::UndoModify()
{
	Apply(mOldValue);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionSetFogColor::Apply(const cColor& aX)
{
	mpEditorWorld->SetFogColor(aX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionSetGlobalMaxDecalTris::cLevelEditorActionSetGlobalMaxDecalTris(iEditorWorld* apWorld, int alMaxTris) : iEditorActionWorldModifier("Set Max triangles per decal", apWorld)
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

cLevelEditorActionAddStaticObjectCombo::cLevelEditorActionAddStaticObjectCombo(iEditorWorld* apWorld) : iEditorActionWorldModifier("Add static object combo", apWorld)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)apWorld;

	mlNewComboID = pWorld->GetFreeComboID();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionAddStaticObjectCombo::DoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;

	cLevelEditorStaticObjectCombo* pCombo = pWorld->CreateStaticObjectCombo(mlNewComboID);
	pWorld->AddStaticObjectCombo(pCombo);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionAddStaticObjectCombo::UndoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;
	pWorld->RemoveStaticObjectCombo(mlNewComboID);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionRemoveStaticObjectCombo::cLevelEditorActionRemoveStaticObjectCombo(iEditorWorld* apWorld, int alComboID) : iEditorActionWorldModifier("Remove combo", apWorld)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;

	mlComboID = -1;
	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(alComboID);
	if(pCombo==NULL)
		return;

	mlComboID = alComboID;
	mColor = pCombo->GetColor();
	const tEntityWrapperList& lstObjs = pCombo->GetObjects();
	tEntityWrapperList::const_iterator itObjs = lstObjs.begin();
	for(;itObjs!=lstObjs.end();++itObjs)
	{
		iEntityWrapper* pObj = *itObjs;

		mvObjIds.push_back(pObj->GetID());
	}
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionRemoveStaticObjectCombo::DoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;

	pWorld->RemoveStaticObjectCombo(mlComboID);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionRemoveStaticObjectCombo::UndoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;

	cLevelEditorStaticObjectCombo* pCombo = pWorld->CreateStaticObjectCombo(mlComboID);
    if(pCombo==NULL)
		return;

	pCombo->SetColor(mColor);
	for(int i=0;i<(int)mvObjIds.size();++i)
	{
		iEntityWrapper* pObj = mpEditorWorld->GetEntity(mvObjIds[i]);

		pCombo->AddObject((cEntityWrapperStaticObject*)pObj);
	}

	pWorld->AddStaticObjectCombo(pCombo);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionAddObjectToCombo::cLevelEditorActionAddObjectToCombo(iEditorWorld* apWorld, cLevelEditorStaticObjectCombo* apCombo, cEntityWrapperStaticObject* apObj) : iEditorActionWorldModifier("Add object to combo", apWorld)
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

	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;

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

void cLevelEditorActionAddObjectToCombo::DoModify()
{
	Apply(mlOldComboID, mlNewComboID);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionAddObjectToCombo::UndoModify()
{
	Apply(mlNewComboID, mlOldComboID);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionAddObjectToCombo::Apply(int alComboToRemove, int alComboToAdd)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;
	cEntityWrapperStaticObject* pObject = (cEntityWrapperStaticObject*)pWorld->GetEntity(mlObjectID);

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(alComboToRemove);
	if(pCombo) pCombo->RemoveObject(pObject);

	pCombo = pWorld->GetStaticObjectCombo(alComboToAdd);
	if(pCombo) pCombo->AddObject(pObject);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionRemoveObjectFromCombo::cLevelEditorActionRemoveObjectFromCombo(iEditorWorld* apWorld, cLevelEditorStaticObjectCombo* apCombo, cEntityWrapperStaticObject* apObj) : iEditorActionWorldModifier("Add object to combo", apWorld)
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

void cLevelEditorActionRemoveObjectFromCombo::DoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;
	cEntityWrapperStaticObject* pObject = (cEntityWrapperStaticObject*)pWorld->GetEntity(mlObjectID);

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	if(pCombo) pCombo->RemoveObject(pObject);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionRemoveObjectFromCombo::UndoModify()
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;
	cEntityWrapperStaticObject* pObject = (cEntityWrapperStaticObject*)pWorld->GetEntity(mlObjectID);

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	if(pCombo) pCombo->AddObject(pObject);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cLevelEditorActionComboSetColor::cLevelEditorActionComboSetColor(iEditorWorld* apWorld, int alComboID, const cColor& aCol) : iEditorActionWorldModifier("Set combo color", apWorld)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)apWorld;

	mlComboID = alComboID;
	mNewColor = aCol;

	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);
	mOldColor = pCombo->GetColor();
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionComboSetColor::DoModify()
{
	Apply(mNewColor);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionComboSetColor::UndoModify()
{
	Apply(mOldColor);
}

//--------------------------------------------------------------------------------------

void cLevelEditorActionComboSetColor::Apply(const cColor& aX)
{
	cLevelEditorWorld* pWorld = (cLevelEditorWorld*)mpEditorWorld;
	cLevelEditorStaticObjectCombo* pCombo = pWorld->GetStaticObjectCombo(mlComboID);

	if(pCombo) pCombo->SetColor(aX);
}

//--------------------------------------------------------------------------------------


