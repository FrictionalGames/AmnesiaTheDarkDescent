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

#include "../Common/EdEditPaneCompound.h"

#include "../Common/EdObjCompound.h"

//#include "EditorEditModeSelect.h"

//#include "EditorActionEntity.h"
//#include "EditorHelper.h"

//#include "EditorInput.h"

#include <algorithm>

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneCompound::cEdEditPaneCompound(iEdObject* apObj) : iEdScnObjEditPane(apObj)
{
}

//----------------------------------------------------------------------------

cEdEditPaneCompound::~cEdEditPaneCompound()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneCompound::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));

	iEdScnObject* pObj = static_cast<iEdScnObject*>(mpObject);

	////////////////////////////////////////
	// Properties in Tab 'General'
	cVector3f vPos = cVector3f(10,10,0.1f);
	AddPropertyName(mpTabGeneral);
	
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;

	if(pObj->IsTranslateable())
	{
		AddPropertyPosition(mpTabGeneral);
		mpInpPosition->SetPosition(vPos);
		vPos.y += mpInpPosition->GetSize().y+5;
	}
	if(pObj->IsRotateable())
	{
		AddPropertyRotation(mpTabGeneral);
		mpInpRotation->SetPosition(vPos);
		vPos.y += mpInpRotation->GetSize().y+5;
	}
	if(pObj->IsScalable())
	{
		AddPropertyScale(mpTabGeneral);
		mpInpScale->SetPosition(vPos);
		vPos.y += mpInpScale->GetSize().y+5;
	}

	mpBDestroyCompound = mpSet->CreateWidgetButton(vPos, cVector2f(mpTabGeneral->GetSize().x-20, 25), _W("Destroy Compound"), mpTabGeneral);
	mpBDestroyCompound->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(DestroyCompound_OnPressed));
	AddShortcut(mpBDestroyCompound->AddShortcut(0, eKey_B));


    ////////////////////////////////////////
	// Properties in Tab 'Specific'
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void cEdEditPaneCompound::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();
}

//----------------------------------------------------------------------------

bool cEdEditPaneCompound::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneCompound,InputCallback);

//----------------------------------------------------------------------------

bool cEdEditPaneCompound::DestroyCompound_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdObjCompound* pCompound = static_cast<cEdObjCompound*>(mpObject);
	iEdAction* pAction = pCompound->CreateRemoveComponentsAction(pCompound->GetComponents());
	this->AddAction(pAction);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneCompound,DestroyCompound_OnPressed);

//----------------------------------------------------------------------------

bool cEdEditPaneCompound::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(iEdScnObjEditPane::WindowSpecificInputCallback(apInput))
		return true;

	return true;
}

//----------------------------------------------------------------------------
