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

#include "EditorEditModeAreas.h"

#include "EditorBaseClasses.h"

#include "EditorWindowAreas.h"

#include "EditorActionsArea.h"
#include "EntityWrapperArea.h"
#include "EditorWorld.h"

#include "BoxCreator.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeAreas::cEditorEditModeAreas(iEditorBase* apEditor,
										   iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Areas", apEditorWorld)
{
	mpBoxCreator = hplNew(cBoxCreator,(this));
}

//-----------------------------------------------------------------

cEditorEditModeAreas::~cEditorEditModeAreas()
{
	hplDelete(mpBoxCreator);
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEditorEditModeAreas::OnViewportMouseDown(int alButtons)
{
	cEntityWrapperTypeArea* pType = (cEntityWrapperTypeArea*)GetType();
	mpBoxCreator->SetDefaultSize(pType->GetDefaultSize());
	mpBoxCreator->SetDraggingEnabled(pType->GetDrawAsSphere()==false && pType->IsScalable()==true);

	mpBoxCreator->OnViewportMouseDown(alButtons);
}

//-----------------------------------------------------------------

void cEditorEditModeAreas::OnViewportMouseUp(int alButtons)
{
	mpBoxCreator->OnViewportMouseUp(alButtons);

	if(mpBoxCreator->IsDoneCreating())
	{
		iEditorAction* pAction = CreateObject(CreateObjectData());

		mpEditor->AddAction(pAction);

		mpBoxCreator->Reset();
	}
}

//-----------------------------------------------------------------

void cEditorEditModeAreas::OnEditorUpdate(float afTimeStep)
{
	mpBoxCreator->OnEditorUpdate();
}

//-----------------------------------------------------------------

void cEditorEditModeAreas::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	iEditorEditModeObjectCreator::DrawPostGrid(apViewport, apFunctions, avPos);
	mpBoxCreator->Draw(apViewport, apFunctions);
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeAreas::CreateSpecificWindow()
{
	return hplNew(cEditorWindowAreas,(this));
}

//-----------------------------------------------------------------

bool cEditorEditModeAreas::SetUpCreationData(iEntityWrapperData* apData)
{
	cEditorWindowAreas* pWin = (cEditorWindowAreas*)mpWindow;
	iEditorEditModeObjectCreator::SetUpCreationData(apData);
	cEntityWrapperDataArea* pData = (cEntityWrapperDataArea*)apData;

	pData->SetVec3f(eObjVec3f_Position, mpBoxCreator->GetBoxCenter());
	pData->SetVec3f(eObjVec3f_Scale,mpBoxCreator->GetBoxSize());

	return true;
}

//-----------------------------------------------------------------

void cEditorEditModeAreas::CreateTypes()
{
	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Area);
	for(int i=0;i<pDef->GetTypeNum();++i)
	{
		// Area types do not have subtypes.
		cEditorUserClassSubType* pType = pDef->GetType(i)->GetSubType(0);
		mvTypes.push_back(hplNew(cEntityWrapperTypeArea, (pType)));
	}
}

//-----------------------------------------------------------------

void cEditorEditModeAreas::OnSetCurrent(bool abX)
{
	mpBoxCreator->Reset();
}

//-----------------------------------------------------------------


