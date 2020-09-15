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

#include "EditorActionEntity.h"

#include "EditorAction.h"

#include "EditorWorld.h"

#include "EditorGrid.h"

#include "EntityWrapper.h"

#include "EditorBaseClasses.h"
#include "EditorEditModeSelect.h"
#include "EditorSelection.h"

#include "EditorHelper.h"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY CREATE ACTION
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionObjectCreate::cEditorActionObjectCreate(iEditorWorld* apEditorWorld, iEntityWrapperData* apData) : iEditorActionWorldModifier("Create",apEditorWorld)
{
	mpNewObjectData = apData;
}

cEditorActionObjectCreate::~cEditorActionObjectCreate()
{
	hplDelete(mpNewObjectData);
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionObjectCreate::DoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->CreateEntityWrapperFromData(mpNewObjectData);
	if(pEnt) pEnt->OnPostDeployAll(false);
}

//---------------------------------------------------------------------------------------

void cEditorActionObjectCreate::UndoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mpNewObjectData->GetInt(eObjInt_ID));
	mpEditorWorld->DestroyEntityWrapper(pEnt);
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY DELETE ACTION
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityDelete::cEditorActionEntityDelete(iEditorWorld* apEditorWorld, cEditorSelection* apSelection, const tIntList& alstSelectedIDs) : iEditorActionWorldModifier("Delete",apEditorWorld)
{
	mpSelection = apSelection;
	
	tIntList::const_iterator it = alstSelectedIDs.begin();

	for(;it!=alstSelectedIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		iEntityWrapperData* pData = pEnt->CreateCopyData();
	
		mvEntityData.push_back(pData);
	}
}

cEditorActionEntityDelete::~cEditorActionEntityDelete()
{
	if(mvEntityData.empty()==false)
		STLDeleteAll(mvEntityData);
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityDelete::DoModify()
{
	for(int i=0;i<(int)mvEntityData.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvEntityData[i]->GetInt(eObjInt_ID));

		mpSelection->RemoveEntity(pEnt);
		mpEditorWorld->DestroyEntityWrapper(pEnt);
	}
}

//---------------------------------------------------------------------------------------

void cEditorActionEntityDelete::UndoModify()
{
	tEntityWrapperList lstEnts;

	for(int i=0;i<(int)mvEntityData.size();++i)
	{
		iEntityWrapperData* pData = mvEntityData[i];

		iEntityWrapper* pEnt = mpEditorWorld->CreateEntityWrapperFromData(pData);
		lstEnts.push_back(pEnt);

		mpSelection->AddEntity(pEnt);
	}

	tEntityWrapperListIt it = lstEnts.begin();
	for(;it!=lstEnts.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		pEnt->OnPostDeployAll(false);
	}
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY CLONE ACTION 
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityClone::cEditorActionEntityClone(iEditorWorld* apEditorWorld, cEditorSelection* apSelection, const tIntList& alstSelectedIDs) : iEditorActionWorldModifier("Clone",apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mpSelection = apSelection;

	tIntList::const_iterator it = alstSelectedIDs.begin();
	for(;it!=alstSelectedIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);

		if(pEnt)
		{
			mvEntityIDs.push_back(*it);
			mvClonedEntityIDs.push_back(pEnt->GetCloneIDs());
		}
	}
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityClone::DoModify()
{
	tEntityWrapperList lstClones;
	mpSelection->ClearEntities();
	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mvEntityIDs[i]);
		iEntityWrapper* pClone = pEnt->Clone(mvClonedEntityIDs[i]);

		mpSelection->AddEntity(pClone);
		lstClones.push_back(pClone);
	}
	
	tEntityWrapperListIt it = lstClones.begin();
	for(;it!=lstClones.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		pEnt->OnPostDeployAll(false);
	}
}

//----------------------------------------------------------------------

void cEditorActionEntityClone::UndoModify()
{
	mpSelection->ClearEntities();
    for(int i=0;i<(int)mvClonedEntityIDs.size();++i)
	{
		tIntVec& vIDs = mvClonedEntityIDs[i];
		for(int j=0;j<(int)vIDs.size();++j)
		{
			int lID = vIDs[j];
			iEntityWrapper* pEnt = mpEditorWorld->GetEntity(lID);
			mpEditorWorld->DestroyEntityWrapper(pEnt);
		}
	}

	for(int i=0;i<(int)mvEntityIDs.size();++i)
	{
		iEntityWrapper* pEnt = pEnt = mpEditorWorld->GetEntity(mvEntityIDs[i]);
		mpSelection->AddEntity(pEnt);
	}
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY SELECT ACTION 
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntitySelect::cEditorActionEntitySelect(cEditorEditModeSelect* apEditMode,
													 const tIntList& alstEntityIDs, eSelectActionType aType) : iEditorAction("Select")
{
	mpEditMode = apEditMode;
	mpSelection = mpEditMode->GetEditor()->GetSelection();
	mpEditorWorld = mpEditMode->GetEditorWorld();

	mType = aType;

	mlstNewSelectedEntityIDs = alstEntityIDs;

	mlstOldSelectedEntityIDs = mpSelection->GetEntityIDs();
}

///---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntitySelect::Do()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();

	if(pEditor->GetCurrentEditMode()!=mpEditMode)
		pEditor->SetCurrentEditMode(mpEditMode);

	switch(mType)
	{
	case eSelectActionType_Clear:
		mpSelection->ClearEntities();
		break;
	case eSelectActionType_Select:
		mpSelection->AddEntitiesByID(mlstNewSelectedEntityIDs);
		break;
	case eSelectActionType_Deselect:
		mpSelection->RemoveEntitiesByID(mlstNewSelectedEntityIDs);
		break;
	case eSelectActionType_Toggle:
		mpSelection->ToggleEntitySelectionByID(mlstNewSelectedEntityIDs);
		break;
	}
}

//--------------------------------------------------------------

void cEditorActionEntitySelect::Undo()
{
	iEditorBase* pEditor = mpEditMode->GetEditor();

	if(pEditor->GetCurrentEditMode()!=mpEditMode)
		pEditor->SetCurrentEditMode(mpEditMode);
	
	mpSelection->AddEntitiesByID(mlstOldSelectedEntityIDs);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY TRANSLATE ACTION 
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityTranslate::cEditorActionEntityTranslate(iEditorWorld* apEditorWorld, 
														   tIntList& alstEntityIDs, 
														   const cVector3f& avTranslate, 
														   bool abUseSnap, bool abRelativeTransform) : iEditorActionWorldModifier("Translate", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;

	mlstEntityIDs = alstEntityIDs;
	
	tIntListIt it = mlstEntityIDs.begin();

	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vOldPos = pEnt->GetPosition();
		cVector3f vNewPos = avTranslate;

		if(abRelativeTransform)
			vNewPos += vOldPos;
		//if(abUseSnap && mpGrid)
		//	vNewPos = mpGrid->GetSnappedPosition(vNewPos,true,true);

		mlstOldTranslations.push_back(vOldPos);
		mlstNewTranslations.push_back(vNewPos);
	}
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityTranslate::Do()
{
	Apply(mlstNewTranslations);

	iEditorActionWorldModifier::Do();
}

void cEditorActionEntityTranslate::Undo()
{
	iEditorActionWorldModifier::Undo();

	Apply(mlstOldTranslations);
}

void cEditorActionEntityTranslate::Apply(tVector3fList &alstParam)
{
	tIntListIt it = mlstEntityIDs.begin();
	tVector3fListIt itPos = alstParam.begin();

	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vPos = (*itPos);

		pEnt->SetAbsPosition(vPos);
		pEnt->UpdateEntity();
		++itPos;
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY ROTATE ACTION 
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityRotate::cEditorActionEntityRotate(iEditorWorld* apEditorWorld, 
													 tIntList& alstEntityIDs, 
													 const cVector3f& avRotate, 
													 bool abRelativeTransform) : iEditorActionWorldModifier("Rotate", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	
	mlstEntityIDs = alstEntityIDs;
	
	tIntListIt it = mlstEntityIDs.begin();
	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vOldRot = pEnt->GetRotation();
		cVector3f vNewRot = avRotate;

		if(abRelativeTransform)
			vNewRot += vOldRot;

		mlstOldRotations.push_back(vOldRot);
		mlstNewRotations.push_back(vNewRot);
	}
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityRotate::Do()
{
	Apply(mlstNewRotations);

	iEditorActionWorldModifier::Do();
}

void cEditorActionEntityRotate::Undo()
{
	iEditorActionWorldModifier::Undo();

	Apply(mlstOldRotations);
}

void cEditorActionEntityRotate::Apply(tVector3fList &alstParam)
{
	tIntListIt it = mlstEntityIDs.begin();
	tVector3fListIt itRot = alstParam.begin();

	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vRot = (*itRot);

		pEnt->SetAbsRotation(vRot);
		pEnt->UpdateEntity();
		++itRot;
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY SCALE ACTION
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityScale::cEditorActionEntityScale(iEditorWorld* apEditorWorld, 
														   tIntList& alstEntityIDs, 
														   const cVector3f& avScale, 
														   bool abRelativeTransform) : iEditorActionWorldModifier("Scale", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;

	mlstEntityIDs = alstEntityIDs;
	
	tIntListIt it = mlstEntityIDs.begin();

	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vOldScale = pEnt->GetScale();
		cVector3f vNewScale = avScale;

		if(abRelativeTransform)
			vNewScale += vOldScale;

		mlstOldScale.push_back(vOldScale);
		mlstNewScale.push_back(vNewScale);
	}
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityScale::Do()
{
	iEditorActionWorldModifier::Do();

	Apply(mlstNewScale);
}

void cEditorActionEntityScale::Undo()
{
	iEditorActionWorldModifier::Undo();

	Apply(mlstOldScale);
}

void cEditorActionEntityScale::Apply(tVector3fList &alstParam)
{
	tIntListIt it = mlstEntityIDs.begin();
	tVector3fListIt itScale = alstParam.begin();

	for(;it!=mlstEntityIDs.end();++it)
	{
		iEntityWrapper* pEnt = mpEditorWorld->GetEntity(*it);
		cVector3f vScale = (*itScale);

		pEnt->SetAbsScale(vScale);
		pEnt->UpdateEntity();
		++itScale;
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY ATTACH CHILDREN ACTION
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityAttachChildren::cEditorActionEntityAttachChildren(iEditorWorld* apEditorWorld, int alID, const tIntList& alstChildIDs) : iEditorActionWorldModifier("Attach Children", apEditorWorld)
{
	mlID = alID;

	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mlID);

	tEntityWrapperList& lstChildren = pEnt->GetChildren();
	cEditorHelper::GetIDsFromEntityList(lstChildren, mlstOldChildIDs);

	mlstChildIDs = alstChildIDs;
	tIntListIt it = mlstChildIDs.begin();
	for(;it!=mlstChildIDs.end();++it)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*it);
		iEntityWrapper* pOldParent = pChild->GetParent();

		mlstChildOldParentIDs.push_back(pOldParent?pOldParent->GetID():-1);
	}
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityAttachChildren::DoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mlID);
	pEnt->ClearChildren();
	tIntListIt it = mlstChildIDs.begin();
	for(;it!=mlstChildIDs.end();++it)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*it);
		pEnt->AttachChild(pChild);
	}
}

//---------------------------------------------------------------------------------------

void cEditorActionEntityAttachChildren::UndoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mlID);
	tIntListIt itChildren = mlstChildIDs.begin();
	tIntListIt itOldParents = mlstChildOldParentIDs.begin();
	for(;itChildren!=mlstChildIDs.end();++itChildren)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*itChildren);
		pEnt->DetachChild(pChild);

		iEntityWrapper* pOldParent = mpEditorWorld->GetEntity(*itOldParents);
		if(pOldParent) pOldParent->AttachChild(pChild);

		++itOldParents;
	}

	pEnt->ClearChildren();
	tIntListIt itOldChildren = mlstOldChildIDs.begin();
	for(;itOldChildren!=mlstOldChildIDs.end();++itOldChildren)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*itOldChildren);
		pEnt->AttachChild(pChild);
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////
// ENTITY DETACH CHILDREN ACTION
/////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

cEditorActionEntityDetachChildren::cEditorActionEntityDetachChildren(iEditorWorld* apEditorWorld, int alID, tIntList& alstChildIDs) : iEditorActionWorldModifier("Detach Children", apEditorWorld)
{
	mlID = alID;

	mlstChildIDs = alstChildIDs;
	tIntListIt it = mlstChildIDs.begin();
}

//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------

void cEditorActionEntityDetachChildren::DoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mlID);
	tIntListIt it = mlstChildIDs.begin();
	for(;it!=mlstChildIDs.end();++it)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*it);
		pEnt->DetachChild(pChild);
	}
}

//---------------------------------------------------------------------------------------

void cEditorActionEntityDetachChildren::UndoModify()
{
	iEntityWrapper* pEnt = mpEditorWorld->GetEntity(mlID);
	tIntListIt it = mlstChildIDs.begin();
	for(;it!=mlstChildIDs.end();++it)
	{
		iEntityWrapper* pChild = mpEditorWorld->GetEntity(*it);
		pEnt->AttachChild(pChild);
	}
}

//---------------------------------------------------------------------------------------


