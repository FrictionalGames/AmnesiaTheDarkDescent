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

#include "../ModelEditor/EdEditPaneShapes.h"
#include "../Common/Editor.h"

#include "../Common/EdSelection.h"
#include "../ModelEditor/EdObjShape.h"

//#include "EntityWrapper.h"

//#include "EditorActionEntity.h"
//#include "EditorActionsBodies.h"


//#include "EntityWrapperBody.h"
//#include "EntityWrapperBodyShape.h"

//#include "EditorWorld.h"

//#include "EditorHelper.h"


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdEditPaneShapes::cEdEditPaneShapes(iEdObject* apObj) : cEdMultiScnObjEditPane(apObj)
{
	// XXX
}

cEdEditPaneShapes::~cEdEditPaneShapes()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdEditPaneShapes::Create()
{
	cEdMultiScnObjEditPane::Create();
	
	cWidgetTab* pTab = mpTabs->AddTab(_W("Shapes"));
	cVector3f vPos = cVector3f(15,20,0.1f);
	mpBCreateBody = mpSet->CreateWidgetButton(vPos, cVector2f(160,25), _W("Create Body"), pTab);
	mpBCreateBody->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	vPos.y += 30;
	//mpBDetachFromBody = mpSet->CreateWidgetButton(vPos, cVector2f(160,25), _W("Detach from Body"), pTab);
	//mpBDetachFromBody->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------

bool cEdEditPaneShapes::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	tIntList lstEntityIDs;
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpObject);

	//cEditorHelper::GetIDsFromEntityList(mlstEntities, lstEntityIDs);
	iEdWorld* pWorld = mpEditor->GetWorld();

	iEdAction* pAction = NULL;

	//cTypeBody* pType = (cTypeBody*)pWorld->GetObjectTypeByID(eEditorEntityType_Body);

	if(apWidget==mpBCreateBody)
	{
		iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

		pAction = pShape->CreateActionCreateBodyFromShapes(pWorld, pSelection->GetComponents());

		//cEdObjShape* pShape = (cEdObjShape*)this->mlstEntities.front();
		//pAction = cEdObjShape::CreateActionCreateBodyFromShapes(pWorld, lstEntityIDs);
		
		/*
		tEntityDataVec vShapeData;
		tIntListIt it = lstEntityIDs.begin();
		for(;it!=lstEntityIDs.end();++it)
		{
			iEdScnObject* pShape = pWorld->GetObject(*it);
			vShapeData.push_back(pShape->CreateCopyData());
		}
		pData->SetComponentsData(vShapeData);

		pAction = hplNew(cEdActionObjectCreate,(pWorld, pData));
		*/
	}
	else if(apWidget==mpBDetachFromBody)
	{
		//pAction = hplNew(cEdActionBodyRemoveShapes,(mpEditMode->GetWorld(), lstEntityIDs));
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneShapes,Button_OnPressed);


//--------------------------------------------------------------------------
