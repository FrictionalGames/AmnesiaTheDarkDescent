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

#include "EditorWindowEditModeSidebar.h"
#include "EditorBaseClasses.h"
#include "EditorEditMode.h"

//----------------------------------------------------------------

iEditorWindowEditModeSidebar::iEditorWindowEditModeSidebar(iEditorBase* apEditor) : iEditorWindow(apEditor,"Edit Mode Sidebar")
{
}

//----------------------------------------------------------------

iEditorWindowEditModeSidebar::~iEditorWindowEditModeSidebar()
{
}

//----------------------------------------------------------------

void iEditorWindowEditModeSidebar::OnInitLayout()
{
	cVector3f vPos = cVector3f(3,5,0.1f);
	float fButtonSize = 25.0f;
	eKey key = eKey_1;
	int lKeyMod = 0;
	tWString sModStrings[] = { _W("Ctrl"), _W("Shift"), _W("Alt") };

	tEditorEditModeVec& vEditModes = mpEditor->GetEditModes();
	for(int i=0;i<(int)vEditModes.size();++i)
	{
		iEditorEditMode* pEditMode = vEditModes[i];
		tString sFilename = cString::ToLowerCase(cString::ReplaceCharTo(pEditMode->GetName(), " ", ""));
		cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("editmode_" + sFilename + ".tga", eGuiMaterial_Alpha);

		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, fButtonSize, _W(""), mpBGFrame, true);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		mpSet->AddGlobalShortcut(lKeyMod, key, pButton, eGuiMessage_ButtonPressed);
		pButton->SetUserData(pEditMode);
		pButton->SetImage(pImg);

		tWString sTooltip = cString::ToStringW(key-eKey_0);
		if(lKeyMod!=0)
			sTooltip = sModStrings[lKeyMod-1] + _W("+") + sTooltip;
		sTooltip = cString::To16Char(pEditMode->GetName()) + _W(" (") + sTooltip + _W(")");
		pButton->SetToolTip(sTooltip);

		mvEditModeButtons.push_back(pButton);

		vPos.y += fButtonSize+2.0f;

		////////////////
		// Assign key
		key = (eKey)(key+1);
		if(key==eKey_1 && lKeyMod==0)
			lKeyMod = 1;
		if(key>eKey_9)
		{
			lKeyMod = lKeyMod<<1;
			key=eKey_0;
		}
	}

	SetSize(cVector2f(fButtonSize+6,vPos.y));
}

//----------------------------------------------------------------

bool iEditorWindowEditModeSidebar::Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->SetCurrentEditMode((iEditorEditMode*)apWidget->GetUserData());
	mpEditor->SetLayoutNeedsUpdate(true);

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorWindowEditModeSidebar, Button_Pressed);

//----------------------------------------------------------------

void iEditorWindowEditModeSidebar::OnUpdate(float afTimeStep)
{
	iEditorEditMode* pEditMode = mpEditor->GetCurrentEditMode();
	for(int i=0; i<(int)mvEditModeButtons.size(); ++i)
	{
		bool bPressed = (pEditMode==(iEditorEditMode*)mvEditModeButtons[i]->GetUserData());

		mvEditModeButtons[i]->SetPressed(bPressed, false);
	}
}


//----------------------------------------------------------------
