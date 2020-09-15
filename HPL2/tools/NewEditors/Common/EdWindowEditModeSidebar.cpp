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

#include "../Common/EdWindowEditModeSidebar.h"
#include "../Common/Editor.h"
#include "../Common/EdEditMode.h"

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEdWindowEditModeSidebar::cEdWindowEditModeSidebar(iEditor* apEditor) : iEdPane(apEditor, _W("EditMode Sidebar"))
{
}

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

void cEdWindowEditModeSidebar::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	iKeyboard* pKB = mpEditor->GetEngine()->GetInput()->GetKeyboard();

	cVector3f vPos = cVector3f(3,5,0.1f);
	float fButtonSize = 25.0f;

	eKey key = eKey_1;
	eKeyModifier keyMod = eKeyModifier_None;

	tString sModifierText;
	
	const tEditModeList& lstModes = mpEditor->GetEditModes();
	tEditModeList::const_iterator it = lstModes.begin();
	for(; it!=lstModes.end(); ++it)
	{
		iEdEditMode* pMode = *it;

		tString sTooltip = " (" + sModifierText + pKB->KeyToString(key) + ")";

		// Create button
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, fButtonSize, _W(""), GetBG(), true);
		mlstModeButtons.push_back(pButton);

		// Link button to EditMode
		pButton->SetUserData(pMode);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		pButton->SetToolTip(pMode->GetName() + cString::To16Char(sTooltip));
		pButton->SetImage(pMode->GetIcon());

		vPos.y += fButtonSize+2.0f;

		////////////////////////////////////////////////////////////////////////
		// Assign shortcut (Assuming we are not gonna have more than 29 edit modes
		AddShortcut(pButton->AddShortcut(keyMod, key));

		key = (eKey)(key+1);
		if(key>eKey_9)
		{
			if(keyMod==eKeyModifier_None) keyMod = eKeyModifier_Ctrl;
			else keyMod = (eKeyModifier) (keyMod<<1);
			key=eKey_0;

			sModifierText = pKB->ModifierKeyToString(keyMod) + "+";
		}
	}

	SetSize(cVector2f(fButtonSize +3 +3,vPos.y));
}

//----------------------------------------------------------------------

void cEdWindowEditModeSidebar::OnUpdate()
{
	tWidgetListIt it = mlstModeButtons.begin();
	for(; it!=mlstModeButtons.end(); ++it)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
		iEdEditMode* pMode = static_cast<iEdEditMode*>(pButton->GetUserData());

		bool bPressed = pMode->IsActive();
		
		pButton->SetPressed(bPressed, false);
	}
}

//----------------------------------------------------------------------

void cEdWindowEditModeSidebar::OnWorldSelectChange()
{
	SetUpdated();
}

//----------------------------------------------------------------------

bool cEdWindowEditModeSidebar::Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdEditMode* pMode = static_cast<iEdEditMode*>(apWidget->GetUserData());
	mpEditor->SetActiveEditMode(pMode);
		
	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowEditModeSidebar, Button_Pressed);

//----------------------------------------------------------------------
