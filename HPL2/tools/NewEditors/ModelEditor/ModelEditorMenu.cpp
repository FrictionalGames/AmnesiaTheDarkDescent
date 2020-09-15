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

#include "../ModelEditor/ModelEditorMenu.h"
#include "../ModelEditor/ModelEditor.h"
#include "../ModelEditor/HPLEntity.h"

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

cModelEditorMenu::cModelEditorMenu(iEdWindow* apWin) : cEdScnEditorMenu(apWin)
{
}

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

void cModelEditorMenu::OnCreateLayout()
{
	cEdScnEditorMenu::OnCreateLayout();

	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem = NULL;

	pItem = AddItem(_W("File"));
	AddNew(pItem);
	pItem->AddSeparator();
	AddOpen(pItem);
	AddSave(pItem);
	AddSaveAs(pItem);
	pItem->AddSeparator();
	AddRecentFiles(pItem);
	pItem->AddSeparator();
	AddItem(_W("Import Mesh"), pItem, this, kGuiCallback(ImportMesh));
	pItem->AddSeparator();
	AddExit(pItem);

	pItem = AddItem(_W("Edit"));
	AddActionControls(pItem);
	pItem->AddSeparator();
	AddEditControls(pItem);
	pItem->AddSeparator();
	AddItem(_W("Model Outline"), pItem, this, kGuiCallback(ShowOutlineWindow), eKeyModifier_None, eKey_O);
	pItem->AddSeparator();
	AddPreferences(pItem);

	pItem = AddItem(_W("Entity settings"));
	AddItem(_W("Class"), pItem, this, kGuiCallback(ShowClassWindow));
	AddItem(_W("Animations"), pItem, this, kGuiCallback(ShowAnimationsWindow));
}

void cModelEditorMenu::OnWorldParamsChange()
{
	cEdScnEditorMenu::OnWorldParamsChange();
}

bool cModelEditorMenu::ImportMesh(iWidget* apWidget, const cGuiMessageData& aData)
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	pEnt->CreateImportMeshDialog();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorMenu, ImportMesh);

bool cModelEditorMenu::ShowOutlineWindow(iWidget* apWidget, const cGuiMessageData& aData)
{
	cModelEditor* pEditor = static_cast<cModelEditor*>(mpEditor);
	pEditor->ShowOutlineDialog();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorMenu, ShowOutlineWindow);

bool cModelEditorMenu::ShowClassWindow(iWidget* apWidget, const cGuiMessageData& aData)
{
	cModelEditor* pEditor = static_cast<cModelEditor*>(mpEditor);
	pEditor->ShowEntityClassDialog();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorMenu, ShowClassWindow);

bool cModelEditorMenu::ShowAnimationsWindow(iWidget* apWidget, const cGuiMessageData& aData)
{
	cModelEditor* pEditor = static_cast<cModelEditor*>(mpEditor);
	pEditor->ShowAnimationsDialog();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorMenu, ShowAnimationsWindow);