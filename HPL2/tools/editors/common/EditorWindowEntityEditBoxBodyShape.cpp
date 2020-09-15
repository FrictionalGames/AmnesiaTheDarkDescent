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

#include "EditorWindowEntityEditBoxBodyShape.h"

#include "EntityWrapperBodyShape.h"
#include "EntityWrapperBody.h"


#include "EditorEditModeSelect.h"
#include "EditorSelection.h"

#include "EditorWorld.h"
#include "EditorActionEntity.h"
#include "EditorActionsBodies.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBodyShape::cEditorWindowEntityEditBoxBodyShape(cEditorEditModeSelect* apEditMode, cEntityWrapperBodyShape* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBodyShape::~cEditorWindowEntityEditBoxBodyShape()
{
	
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBodyShape::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabBodyShape = mpTabs->AddTab(_W("Shape"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y;

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddButtonBody(mpTabBodyShape);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBodyShape::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBodyShape::AddButtonBody(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,30,0.1f);
	for(int i=0; i<2; ++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(vPos + cVector3f(0,(float)i*30,0),cVector2f(160,25), _W(""), apParentTab);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}

	mvButtons[0]->SetText(_W("Create Body"));
	mvButtons[1]->SetText(_W("Detach from body"));
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBodyShape::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEntityWrapperBody* pParentBody = mpEntity->GetParentBody();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	tIntList& lstShapeIDs = mpEditor->GetSelection()->GetEntityIDs();

	iEditorAction* pAction = NULL;

	if(apWidget==mvButtons[0])
	{
		if(pParentBody!=NULL)
			return true;

		pAction = mpEntity->CreateActionCreateBody();

		/*
		cEntityWrapperDataBody* pData = (cEntityWrapperDataBody*)pWorld->GetEntityTypeByID(eEditorEntityType_Body)->CreateData();
		pData->SetName(pWorld->GenerateName(pData->GetName()));

		tIntList& lstShapeIDs = mpEditor->GetSelection()->GetEntityIDs();
		tIntVec vShapeIDs;
		vShapeIDs.insert(vShapeIDs.begin(), lstShapeIDs.begin(), lstShapeIDs.end());
		pData->SetShapeIDs(vShapeIDs);
		//lstShapeIDs.insert(lstShapeIDs.begin(), pData->mvShapeIDs.begin(), pData->mvShapeIDs.end());

		pAction = hplNew(cEditorActionObjectCreate,(pWorld, pData));
		*/
	}
	else if(apWidget==mvButtons[1])
	{
		if(pParentBody==NULL)
			return true;

		pAction = hplNew(cEditorActionBodyRemoveShapes,(pWorld, lstShapeIDs));
	}

	mpEditor->AddAction(pAction);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxBodyShape,Button_OnPressed);

//----------------------------------------------------------------------------
