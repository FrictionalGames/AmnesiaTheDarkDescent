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

#include "../Common/EdGenericMenu.h"
#include "../Common/Editor.h"

#include "../Common/EdActionHandler.h"

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

cEdGenericMenu::cEdGenericMenu(iEdWindow* apWindow) : iEdMenu(apWindow, _W("MainMenu"))
{
	mpRecent = NULL;
	mpUndo = NULL;
	mpRedo = NULL;
}

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

void cEdGenericMenu::AddNew(cWidgetMenuItem* apItem, int alMod, eKey aKey)
{
	AddItem(_W("New"), apItem, this, kGuiCallback(New), alMod, aKey);
}

void cEdGenericMenu::AddOpen(cWidgetMenuItem* apItem, int alMod, eKey aKey)
{
	AddItem(_W("Open"), apItem, this, kGuiCallback(Open), alMod, aKey);
}

void cEdGenericMenu::AddSave(cWidgetMenuItem* apItem, int alMod, eKey aKey)
{
	AddItem(_W("Save"), apItem, this, kGuiCallback(Save), alMod, aKey);
}

void cEdGenericMenu::AddSaveAs(cWidgetMenuItem* apItem, int alMod, eKey aKey)
{
	AddItem(_W("Save As"), apItem, this, kGuiCallback(SaveAs), alMod, aKey);
}

void cEdGenericMenu::AddRecentFiles(cWidgetMenuItem* apItem)
{
	mpRecent = AddItem(_W("Recent Files"), apItem);
	UpdateRecentFiles();
}

void cEdGenericMenu::AddExit(cWidgetMenuItem* apItem, int alMod, eKey aKey)
{
	AddItem(_W("Quit"), apItem, this, kGuiCallback(Quit), alMod, aKey);
}

void cEdGenericMenu::AddActionControls(cWidgetMenuItem* apItem)
{
	mpUndo = AddItem(_W("Undo"), apItem, this, kGuiCallback(Undo), eKeyModifier_Ctrl, eKey_Z);
	mpRedo = AddItem(_W("Redo"), apItem, this, kGuiCallback(Redo), eKeyModifier_Ctrl, eKey_Y);

	UpdateActionControls();
}

void cEdGenericMenu::AddPreferences(cWidgetMenuItem* apItem)
{
	AddItem(_W("Preferences"), apItem, this, kGuiCallback(Preferences));
}

void cEdGenericMenu::OnActionExec()
{
	UpdateActionControls();
}

void cEdGenericMenu::OnWorldLoad()
{
	UpdateRecentFiles();
}

void cEdGenericMenu::OnWorldSave()
{
	UpdateRecentFiles();
}

//----------------------------------------------------------

void cEdGenericMenu::UpdateActionControls()
{
	if(mpUndo) mpUndo->SetEnabled(mpEditor->GetActionHandler()->HasDoneActions());
	if(mpRedo) mpRedo->SetEnabled(mpEditor->GetActionHandler()->HasUndoneActions());
}

void cEdGenericMenu::UpdateRecentFiles()
{
	if(mpRecent)
	{
		mpRecent->ClearMenuItems();

		const tWStringList& lstRecentFiles = mpEditor->GetWorld()->GetRecentFiles();
		tWStringList::const_iterator it = lstRecentFiles.begin();
		for(;it!=lstRecentFiles.end(); ++it)
		{
			const tWString& sFile = *it;
			AddItem(sFile, mpRecent, this, kGuiCallback(OpenRecentFile));
		}
	}
}

//----------------------------------------------------------

bool cEdGenericMenu::New(iWidget* apWidget, const cGuiMessageData& aData)
{
	New();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, New);

bool cEdGenericMenu::Open(iWidget* apWidget, const cGuiMessageData& aData)
{
	Open();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Open);


bool cEdGenericMenu::Save(iWidget* apWidget, const cGuiMessageData& aData)
{
	Save();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Save);


bool cEdGenericMenu::SaveAs(iWidget* apWidget, const cGuiMessageData& aData)
{
	SaveAs();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, SaveAs);


bool cEdGenericMenu::Quit(iWidget* apWidget, const cGuiMessageData& aData)
{
	Quit();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Quit);

bool cEdGenericMenu::OpenRecentFile(iWidget* apWidget, const cGuiMessageData& aData)
{
	const tWString& sFile = apWidget->GetText();
	iEdWorld* pWorld = mpEditor->GetWorld();

	pWorld->SetFileToLoadOnUpdate(sFile);
	pWorld->RemRecentFile(sFile);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, OpenRecentFile);

bool cEdGenericMenu::Undo(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->GetActionHandler()->Undo();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Undo);


bool cEdGenericMenu::Redo(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->GetActionHandler()->Redo();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Redo);

bool cEdGenericMenu::Preferences(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->ShowPreferencesDialog();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdGenericMenu, Preferences);

void cEdGenericMenu::New()
{
	mpEditor->Reset();
}

void cEdGenericMenu::Open()
{
	mpEditor->GetWorld()->CreateLoadWorldDialog();
}

void cEdGenericMenu::Save()
{
	mpEditor->GetWorld()->Save();
}

void cEdGenericMenu::SaveAs()
{
	mpEditor->GetWorld()->CreateSaveWorldDialog();
}

void cEdGenericMenu::Quit()
{
	mpEditor->GetEngine()->Exit();
}
