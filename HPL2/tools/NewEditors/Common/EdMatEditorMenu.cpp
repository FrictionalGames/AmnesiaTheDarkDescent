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

#include "../Common/EdMatEditorMenu.h"
#include "../Common/EdWindowMaterialEditor.h"
#include "../Common/HPLMaterial.h"

#include "../Common/Editor.h"

cEdMatEditorMenu::cEdMatEditorMenu(iEdWindow* apWindow) : cEdGenericMenu(apWindow)
{
}

void cEdMatEditorMenu::OnCreateLayout()
{
	cEdGenericMenu::OnCreateLayout();

	cEdWindowMaterialEditor* pEditor = static_cast<cEdWindowMaterialEditor*>(mpWindow);

	cWidgetMenuItem* pItem = mpMenu->AddMenuItem(_W("File"));
	if(pEditor->IsStandalone())
	{
		AddNew(pItem);
		pItem->AddSeparator();
		AddOpen(pItem);
		AddSave(pItem);
		AddSaveAs(pItem);
		pItem->AddSeparator();
		AddRecentFiles(pItem);
		pItem->AddSeparator();
		AddExit(pItem);
	}
	else
	{
		AddNew(pItem, eKeyModifier_None, eKey_None);
		pItem->AddSeparator();
		AddOpen(pItem, eKeyModifier_None, eKey_None);
		AddSave(pItem, eKeyModifier_None, eKey_None);
		AddSaveAs(pItem, eKeyModifier_None, eKey_None);
		pItem->AddSeparator();
		AddExit(pItem, eKeyModifier_None, eKey_None);
	}
}

void cEdMatEditorMenu::New()
{
	cEdWindowMaterialEditor* pEditor = static_cast<cEdWindowMaterialEditor*>(mpWindow);
	if(pEditor->IsStandalone())
		mpEditor->Reset();
	else
        mpWindow->DispatchMessage(eEdModuleMsg_Reset, NULL);
}

void cEdMatEditorMenu::Open()
{
	cEdWindowMaterialEditor* pWin = static_cast<cEdWindowMaterialEditor*>(mpWindow);
	pWin->GetMaterial()->CreateLoadWorldDialog();
}

void cEdMatEditorMenu::Save()
{
	cEdWindowMaterialEditor* pWin = static_cast<cEdWindowMaterialEditor*>(mpWindow);
	pWin->GetMaterial()->Save();
}

void cEdMatEditorMenu::SaveAs()
{
	cEdWindowMaterialEditor* pWin = static_cast<cEdWindowMaterialEditor*>(mpWindow);
	pWin->GetMaterial()->CreateSaveWorldDialog();
}

void cEdMatEditorMenu::Quit()
{
	mpWindow->SetActive(false);
}

