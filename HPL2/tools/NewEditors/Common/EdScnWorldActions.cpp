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

#include "../Common/EdScnWorldActions.h"

#include "../Common/EdScnWorld.h"
#include "../Common/EdScnObject.h"

//#include "../common/EditorEditModeSelect.h"
//#include "../common/EntityWrapper.h"

//#include "../common/EditorWorld.h"
//#include "../common/EditorSelection.h"

//#include "../common/EntityWrapperDecal.h"
//#include "../common/EntityWrapperStaticObject.h"

//#include "LevelEditor.h"
//#include "LevelEditorWorld.h"

//--------------------------------------------------------------------------------------

/*
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
*/
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetSkyboxActive::cEdActionWorldSetSkyboxActive(iEdWorld* apWorld, bool abX) : iEdActionWorldModifier(_W("Set Skybox active"), apWorld)
{
	mbOldValue = static_cast<cHplMap*>(mpWorld)->GetSkyboxActive();
	mbNewValue = abX;
}


//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxActive::DoModify()
{
	Apply(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxActive::UndoModify()
{
	Apply(mbOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxActive::Apply(bool abX)
{
	static_cast<cHplMap*>(mpWorld)->SetSkyboxActive(abX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetSkyboxTexture::cEdActionWorldSetSkyboxTexture(iEdWorld* apWorld, const tString& asX) : iEdActionWorldModifier(_W("Set Skybox texture"), apWorld)
{
	msOldValue = static_cast<cHplMap*>(mpWorld)->GetSkyboxTexture();
	msNewValue = asX;
}


//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxTexture::DoModify()
{
	Apply(msNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxTexture::UndoModify()
{
	Apply(msOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxTexture::Apply(const tString& asX)
{
	static_cast<cHplMap*>(mpWorld)->SetSkyboxTexture(asX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetSkyboxColor::cEdActionWorldSetSkyboxColor(iEdWorld* apWorld, const cColor& aX) : iEdActionWorldModifier(_W("Set Skybox color"), apWorld)
{
	mOldValue = static_cast<cHplMap*>(mpWorld)->GetSkyboxColor();
	mNewValue = aX;
}


//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxColor::DoModify()
{
	Apply(mNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxColor::UndoModify()
{
	Apply(mOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetSkyboxColor::Apply(const cColor& aX)
{
	static_cast<cHplMap*>(mpWorld)->SetSkyboxColor(aX);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetFogBool::cEdActionWorldSetFogBool(iEdWorld* apWorld, eFogBoolProp aProp, bool abX) : iEdActionWorldModifier(_W("Set Skybox active"), apWorld)
{
	mProp = aProp;

	mbOldValue = GetOldValue();
	mbNewValue = abX;
}


//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogBool::DoModify()
{
	Apply(mbNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogBool::UndoModify()
{
	Apply(mbOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogBool::Apply(bool abX)
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	switch(mProp)
	{
	case eFogBoolProp_Active:
        pMap->SetFogActive(abX);
		break;
	case eFogBoolProp_Culling:
		pMap->SetFogCulling(abX);
		break;
	}
}

bool cEdActionWorldSetFogBool::GetOldValue()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	switch(mProp)
	{
	case eFogBoolProp_Active:
        return pMap->GetFogActive();
	case eFogBoolProp_Culling:
		return pMap->GetFogCulling();
	}

	return false;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetFogFloat::cEdActionWorldSetFogFloat(iEdWorld* apWorld, eFogFloatProp aProp, float afX) : iEdActionWorldModifier(_W("Set Fog float"), apWorld)
{
	mProp = aProp;

	mfOldValue = GetOldValue();
	mfNewValue = afX;
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogFloat::DoModify()
{
	Apply(mfNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogFloat::UndoModify()
{
	Apply(mfOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogFloat::Apply(float afX)
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	switch(mProp)
	{
	case eFogFloatProp_Start:
        pMap->SetFogStart(afX);
		break;
	case eFogFloatProp_End:
		pMap->SetFogEnd(afX);
		break;
	case eFogFloatProp_FalloffExp:
		pMap->SetFogFalloffExp(afX);
		break;
	}
}

float cEdActionWorldSetFogFloat::GetOldValue()
{
	cHplMap* pMap = static_cast<cHplMap*>(mpWorld);
	switch(mProp)
	{
	case eFogFloatProp_Start:
        return pMap->GetFogStart();
	case eFogFloatProp_End:
		return pMap->GetFogEnd();
	case eFogFloatProp_FalloffExp:
		return pMap->GetFogFalloffExp();
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cEdActionWorldSetFogColor::cEdActionWorldSetFogColor(iEdWorld* apWorld, const cColor& aX) : iEdActionWorldModifier(_W("Set Fog color"), apWorld)
{
	mOldValue = static_cast<cHplMap*>(mpWorld)->GetFogColor();
	mNewValue = aX;
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogColor::DoModify()
{
	Apply(mNewValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogColor::UndoModify()
{
	Apply(mOldValue);
}

//--------------------------------------------------------------------------------------

void cEdActionWorldSetFogColor::Apply(const cColor& aX)
{
	static_cast<cHplMap*>(mpWorld)->SetFogColor(aX);
}

/*
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

cWorldEditorActionSetGlobalMaxDecalTris::cLevelEditorActionSetGlobalMaxDecalTris(iEdWorld* apWorld, int alMaxTris) : iEdActionWorldModifier("Set Max triangles per decal", apWorld)
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

//--------------------------------------------------------------------------------------


