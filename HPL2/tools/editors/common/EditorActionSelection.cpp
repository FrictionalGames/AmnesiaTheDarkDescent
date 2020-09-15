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

#include "EditorActionSelection.h"

#include "EditorBaseClasses.h"

#include "EditorEditModeSelect.h"
#include "EditorWorld.h"
#include "EditorSelection.h"
#include "EntityWrapper.h"

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------

cEditorActionSelectionTranslate::cEditorActionSelectionTranslate(cEditorEditModeSelect* apEditMode,
																 const cVector3f& avTranslate,
																 bool abUseSnap) : iEditorActionWorldModifier("Translate selection",apEditMode->GetEditorWorld())
{
	mpEditMode = apEditMode;
	mpEditorWorld = mpEditMode->GetEditorWorld();
	mbUseSnap = abUseSnap;

	mpSelection = mpEditorWorld->GetEditor()->GetSelection();

	mvTranslate = avTranslate;

	mpSelection->FallToReferenceTransforms();
	tEntityWrapperListIt it = mpSelection->GetEntities().begin();
	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		mvEntityOldPos.push_back(pEnt->GetPosition());
	}
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionTranslate::DoModify()
{
	mpSelection->SetRelativeTranslation(mvTranslate, mbUseSnap);
	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionTranslate::UndoModify()
{
	tEntityWrapperListIt it = mpSelection->GetEntities().begin();

	int i=0;
	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		pEnt->SetAbsPosition(mvEntityOldPos[i]);
		pEnt->UpdateEntity();
		++i;
	}
	mpSelection->UpdateCenter();
	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------


cEditorActionSelectionRotate::cEditorActionSelectionRotate(cEditorEditModeSelect* apEditMode,
														   iEditorWorld* apEditorWorld,
														   const cVector3f& avRotate,
														   bool abUseSnap,
														   bool abRelativeTransform) : iEditorActionWorldModifier("Rotate selection", apEditorWorld)
{
	mpEditMode = apEditMode;
	mpEditorWorld = apEditorWorld;
	mvRotate = avRotate;
	mbUseSnap = abUseSnap;
	mbRelativeTransform = abRelativeTransform;

	mpSelection = apEditMode->GetEditor()->GetSelection();
	mvOldRotation = mpSelection->GetCenterOldRotation();

	mpSelection->FallToReferenceTransforms();

	tEntityWrapperListIt it = mpSelection->GetEntities().begin();

	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		mvEntityOldRot.push_back(pEnt->GetRotation());
	}
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionRotate::DoModify()
{
	if(mbRelativeTransform)
		mpSelection->SetRelativeRotation(mvRotate,mbUseSnap);
	else
	{
		tEntityWrapperListIt it = mpSelection->GetEntities().begin();
		for(;it!=mpSelection->GetEntities().end();++it)
		{
			iEntityWrapper* pEnt = *it;

			pEnt->SetAbsRotation(mvRotate);
			pEnt->UpdateEntity();
		}
		mpSelection->SetCenterRotation(mvRotate);
	}

	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionRotate::UndoModify()
{
	tEntityWrapperListIt it = mpSelection->GetEntities().begin();

	int i=0;
	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		pEnt->SetAbsRotation(mvEntityOldRot[i]);
		pEnt->UpdateEntity();
		++i;
	}
	mpSelection->UpdateCenter();
	mpSelection->SetCenterRotation(mvOldRotation);

	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

cEditorActionSelectionScale::cEditorActionSelectionScale(cEditorEditModeSelect* apEditMode,
														 iEditorWorld* apEditorWorld,
														 const cVector3f& avScale,
														 bool abUseSnap) : iEditorActionWorldModifier("Scale selection", apEditorWorld)
{
	mpEditMode = apEditMode;
	mpEditorWorld = apEditorWorld;
	mvScale = avScale;
	mbUseSnap = abUseSnap;

	mpSelection = apEditorWorld->GetEditor()->GetSelection();
	mpSelection->FallToReferenceTransforms();

	tEntityWrapperListIt it = mpSelection->GetEntities().begin();

	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		mvEntityOldScale.push_back(pEnt->GetScale());
	}
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionScale::DoModify()
{
	mpSelection->SetRelativeScale(mvScale, mbUseSnap);
	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------

void cEditorActionSelectionScale::UndoModify()
{
	tEntityWrapperListIt it = mpSelection->GetEntities().begin();

	int i=0;
	for(;it!=mpSelection->GetEntities().end();++it)
	{
        iEntityWrapper* pEnt = *it;

		pEnt->SetAbsScale(mvEntityOldScale[i]);
		pEnt->UpdateEntity();
		++i;
	}
	mpSelection->UpdateCenter();
	mpSelection->UpdateReferenceTransforms();
}

//-------------------------------------------------------------------------------------
