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

#include "../ModelEditor/EdEditPaneShape.h"

#include "../ModelEditor/EdObjShape.h"
#include "../ModelEditor/EdObjBody.h"
#include "../Common/Editor.h"
#include "../Common/EdAction.h"
#include "../Common/EdSelection.h"

//#include "EdEditModeSelect.h"
//#include "EditorSelection.h"

//#include "EditorWorld.h"
//#include "EditorActionEntity.h"
//#include "EditorActionsBodies.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneShape::cEdEditPaneShape(iEdObject* apObject) : iEdScnObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneShape::~cEdEditPaneShape()
{
	
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneShape::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabBodyShape = mpTabs->AddTab(_W("Shape"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y;

	tWStringList lstLabels;
	lstLabels.push_back(_W("X"));
	lstLabels.push_back(_W("Y"));
	lstLabels.push_back(_W("Z"));
	mpInpSize = CreateInputVec3(vPos, _W("Size"), mpTabGeneral, 50, lstLabels, eEdInpStyle_ColumnLabelOnTop, 0.25f);
	cVector3f vSize;
	mpObject->GetProperty(eShapeVec3f_Size, vSize);
	for(int i=0;i<3;++i)
	{
		if(vSize.v[i]==0)
			mpInpSize->GetInputWidget(i)->SetEnabled(false);
	}
	
    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddButtonBody(mpTabBodyShape);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneShape::AddButtonBody(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(5,10,0.1f);
	cVector2f vSize = apParentTab->GetSize();
	vSize.x -= 20;
	vSize.y = 25;

	iEdObjShape* pShape = static_cast<iEdObjShape*>(mpObject);

	if(pShape->HasParentBody())
	{
		mpBBodyActions = mpSet->CreateWidgetButton(vPos, vSize, _W("Detach from Body"), apParentTab);
		mpBBodyActions->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(DetachFromBody));
	}
	else
	{
		mpBBodyActions = mpSet->CreateWidgetButton(vPos, vSize, _W("Create Body"), apParentTab);
		mpBBodyActions->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(CreateBody));
	}
}

//----------------------------------------------------------------------------

bool cEdEditPaneShape::CreateBody(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdSelection* pSelection = mpSelect->GetSelection();
	iEdObjShape* pShape = static_cast<iEdObjShape*>(mpObject);
	cEdActionCompound* pAction = static_cast<cEdActionCompound*>(pShape->CreateActionCreateBody());
	pAction->StepForward();

	pAction->AddAction(pSelection->CreateSelectObjectsAction(tScnObjList(1,pShape->GetParentBody())));
	
	AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneShape,CreateBody);

//----------------------------------------------------------------------------

bool cEdEditPaneShape::DetachFromBody(iWidget* apWidget, const cGuiMessageData& aData)
{ 
	iEdObjShape* pShape = static_cast<iEdObjShape*>(mpObject);

	/*
	cEdObjBody* pParentBody = pShape->GetParentBody();
	iEdWorld* pWorld = mpEditor->GetWorld();
	//tIntList& lstShapeIDs = mpEditor->GetSelection()->GetObjectIDs();

	iEdAction* pAction = NULL;

	if(apWidget==mvButtons[0])
	{
		if(pParentBody!=NULL)
			return true;

		//pAction = mpEntity->CreateActionCreateBody();

		/*
		cEdObjBodyData* pData = (cEdObjBodyData*)pWorld->GetObjectTypeByID(eEditorEntityType_Body)->CreateData();
		pData->SetName(pWorld->GenerateName(pData->GetName()));

		tIntList& lstShapeIDs = mpEditor->GetSelection()->GetObjectIDs();
		tIntVec vShapeIDs;
		vShapeIDs.insert(vShapeIDs.begin(), lstShapeIDs.begin(), lstShapeIDs.end());
		pData->SetShapeIDs(vShapeIDs);
		//lstShapeIDs.insert(lstShapeIDs.begin(), pData->mvShapeIDs.begin(), pData->mvShapeIDs.end());

		pAction = hplNew(cEdActionObjectCreate,(pWorld, pData));

	}
	else if(apWidget==mvButtons[1])
	{
		if(pParentBody==NULL)
			return true;

		//pAction = hplNew(cEdActionBodyRemoveShapes,(pWorld, lstShapeIDs));
	}

	mpEditor->AddAction(pAction);
	*/
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneShape,DetachFromBody);

//----------------------------------------------------------------------------

void cEdEditPaneShape::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	iEdObjShape* pShape = static_cast<iEdObjShape*>(mpObject);

	mpInpSize->SetValue(pShape->GetSize(), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneShape::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpSize)
	{
		AddAction(mpObject->CreateSetVector3fAction(eShapeVec3f_Size, mpInpSize->GetValue()));
	}
	else
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);

	return true;
}

//----------------------------------------------------------------------------
