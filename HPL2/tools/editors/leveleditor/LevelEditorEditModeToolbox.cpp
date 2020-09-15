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

#include "LevelEditorEditModeToolbox.h"

#include "LevelEditor.h"

#include "../common/EditorEditMode.h"

//----------------------------------------------------------------

cLevelEditorEditModeToolbox::cLevelEditorEditModeToolbox(cLevelEditor* apEditor) : iLevelEditorWindow(apEditor,"Edit Mode ToolBox")
{
}

//----------------------------------------------------------------

cLevelEditorEditModeToolbox::~cLevelEditorEditModeToolbox()
{
}

//----------------------------------------------------------------

void cLevelEditorEditModeToolbox::OnInitLayout()
{
	SetSize(cVector2f(50,500));
	cVector3f vPos = cVector3f(5,5,0.1f);

	tEditorEditModeVec& vEditModes = mpEditor->GetEditModes();
	for(int i=0;i<(int)vEditModes.size();++i)
	{
		iEditorEditMode* pEditMode = vEditModes[i];
		tString sFilename = cString::ToLowerCase(cString::ReplaceCharTo(pEditMode->GetName(), " ", ""));
		cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("editmode_" + sFilename + ".tga", eGuiMaterial_Alpha);

		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, 40, _W(""), mpBGFrame, true);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		pButton->SetUserData(pEditMode);
		pButton->SetImage(pImg);
		pButton->SetToolTip(cString::To16Char(pEditMode->GetName()));
		pButton->SetToolTipEnabled(true);

		vPos.y += 45;

		AddWidget(pButton);
		mvButtons.push_back(pButton);
	}
}

//----------------------------------------------------------------

bool cLevelEditorEditModeToolbox::Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->SetCurrentEditMode((iEditorEditMode*)apWidget->GetUserData());
	mpEditor->SetLayoutNeedsUpdate(true);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorEditModeToolbox, Button_Pressed);

//----------------------------------------------------------------

void cLevelEditorEditModeToolbox::OnUpdate(float afTimeStep)
{
	iEditorEditMode* pEditMode = mpEditor->GetCurrentEditMode();
	for(int i=0; i<(int)mvButtons.size(); ++i)
	{
		bool bPressed = (pEditMode==(iEditorEditMode*)mvButtons[i]->GetUserData());

		mvButtons[i]->SetPressed(bPressed);
	}
}


//----------------------------------------------------------------
