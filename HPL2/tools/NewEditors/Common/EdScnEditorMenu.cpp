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

#include "../Common/EdScnEditorMenu.h"
#include "../Common/Editor.h"
#include "../Common/EdWindowHandler.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdSelection.h"

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

cEdScnEditorMenu::cEdScnEditorMenu(iEdWindow* apWindow) : cEdGenericMenu(apWindow)
{
	mpClone = NULL;
	mpDelete = NULL;
}

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

void cEdScnEditorMenu::AddEditControls(cWidgetMenuItem* apItem)
{
	mpClone = AddItem(_W("Duplicate"), apItem, this, kGuiCallback(DuplicateObject), eKeyModifier_Ctrl, eKey_D);
	mpDelete = AddItem(_W("Delete"), apItem, this, kGuiCallback(DeleteObject), eKeyModifier_None, eKey_Delete);

	cEdEditModeSelect* pMode = static_cast<cEdEditModeSelect*>(mpWindow->GetEditor()->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pMode->GetSelection();
	mpClone->SetUserData(pSelection);
	mpDelete->SetUserData(pSelection);

	UpdateEditControls();
}

void cEdScnEditorMenu::OnWorldSelectChange()
{
	UpdateEditControls();
}

//----------------------------------------------------------

void cEdScnEditorMenu::UpdateEditControls()
{
	if(mpClone==NULL && mpDelete==NULL) return;

	cEdSelection* pSelection = static_cast<cEdSelection*>(mpClone->GetUserData());
	bool bHasObjects = pSelection->HasComponents();

	mpClone->SetEnabled(bHasObjects && pSelection->IsCloneable());
	mpDelete->SetEnabled(bHasObjects && pSelection->IsDeletable());
}

//----------------------------------------------------------

bool cEdScnEditorMenu::DuplicateObject(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(apWidget->GetUserData());
	mpWindow->GetEditor()->AddAction(pSelection->CreateCloneObjectsAction());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdScnEditorMenu, DuplicateObject);

//----------------------------------------------------------

bool cEdScnEditorMenu::DeleteObject(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(apWidget->GetUserData());
	mpWindow->GetEditor()->AddAction(pSelection->CreateDeleteObjectsAction());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdScnEditorMenu, DeleteObject);

//----------------------------------------------------------
