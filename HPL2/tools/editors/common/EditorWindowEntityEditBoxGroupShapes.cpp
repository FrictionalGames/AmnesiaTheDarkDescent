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

#include "EditorWindowEntityEditBoxGroupShapes.h"

#include "EntityWrapper.h"

#include "EditorActionEntity.h"
#include "EditorActionsBodies.h"


#include "EntityWrapperBody.h"
#include "EntityWrapperBodyShape.h"

#include "EditorWorld.h"

#include "EditorHelper.h"


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEditorWindowEntityEditBoxGroupShapes::cEditorWindowEntityEditBoxGroupShapes(cEditorEditModeSelect* apEditMode, const tEntityWrapperList& alstEntities) : cEditorWindowEntityEditBoxGroup(apEditMode, alstEntities)
{
}

cEditorWindowEntityEditBoxGroupShapes::~cEditorWindowEntityEditBoxGroupShapes()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEditorWindowEntityEditBoxGroupShapes::Create()
{
	cWidgetButton** vButtons[2] = { &mpBCreateBody, &mpBDetachFromBody };

	cVector3f vPos = cVector3f(15,20,0.1f);
	for(int i=0;i<2;++i)
	{
		*vButtons[i] = mpSet->CreateWidgetButton(vPos + cVector3f(0,(float)i*30,0), cVector2f(160,25), _W(""), mpBGFrame);
		(*vButtons[i])->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}

	mpBCreateBody->SetText(_W("Create Body"));
	mpBDetachFromBody->SetText(_W("Detach from Body"));
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxGroupShapes::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	tIntList lstEntityIDs;
	cEditorHelper::GetIDsFromEntityList(mlstEntities, lstEntityIDs);
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	iEditorAction* pAction = NULL;

	cEntityWrapperTypeBody* pType = (cEntityWrapperTypeBody*)pWorld->GetEntityTypeByID(eEditorEntityType_Body);

	if(apWidget==mpBCreateBody)
	{
		//cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)this->mlstEntities.front();
		pAction = cEntityWrapperBodyShape::CreateActionCreateBodyFromShapes(pWorld, lstEntityIDs);
		
		/*
		tEntityDataVec vShapeData;
		tIntListIt it = lstEntityIDs.begin();
		for(;it!=lstEntityIDs.end();++it)
		{
			iEntityWrapper* pShape = pWorld->GetEntity(*it);
			vShapeData.push_back(pShape->CreateCopyData());
		}
		pData->SetComponentsData(vShapeData);

		pAction = hplNew(cEditorActionObjectCreate,(pWorld, pData));
		*/
	}
	else if(apWidget==mpBDetachFromBody)
	{
		pAction = hplNew(cEditorActionBodyRemoveShapes,(mpEditMode->GetEditorWorld(), lstEntityIDs));
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxGroupShapes,Button_OnPressed);


//--------------------------------------------------------------------------
