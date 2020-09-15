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

#include "ModelEditorEditModeToolbox.h"

#include "ModelEditor.h"

#include "../common/EditorEditMode.h"


//----------------------------------------------------------------

cModelEditorEditModeToolbox::cModelEditorEditModeToolbox(cModelEditor* apEditor) : iEditorWindow(apEditor,"Edit Mode ToolBox")
{
	mpEditor = apEditor;
}

//----------------------------------------------------------------

cModelEditorEditModeToolbox::~cModelEditorEditModeToolbox()
{
}

//----------------------------------------------------------------

void cModelEditorEditModeToolbox::OnInitLayout()
{
	SetSize(cVector2f(50,370));
	
	cVector3f vPos = cVector3f(5,5,0.1f);

	tEditorEditModeVec& vEditModes = mpEditor->GetEditModes();
	for(int i=0;i<(int)vEditModes.size();++i)
	{
		iEditorEditMode* pEditMode = vEditModes[i];

		mvButtons[i] = mpSet->CreateWidgetButton(vPos, 40, cString::To16Char(pEditMode->GetName()), mpBGFrame);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		mvButtons[i]->SetUserData(pEditMode);

		vPos.y += 45;

		AddWidget(mvButtons[i]);
	}

	/*mvButtons[eModelEditorEditMode_Select]->SetText(_W("Select"));
	mvButtons[eModelEditorEditMode_Bodies]->SetText(_W("Shapes"));
	mvButtons[eModelEditorEditMode_Joints]->SetText(_W("Joints"));
	mvButtons[eModelEditorEditMode_Lights]->SetText(_W("Lights"));
	mvButtons[eModelEditorEditMode_Billboards]->SetText(_W("Bboards"));
	mvButtons[eModelEditorEditMode_ParticleSystems]->SetText(_W("PS"));
	mvButtons[eModelEditorEditMode_Sounds]->SetText(_W("Sounds"));*/

}

//----------------------------------------------------------------

bool cModelEditorEditModeToolbox::Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpEditor->SetCurrentEditMode((iEditorEditMode*)apWidget->GetUserData());
	/*eModelEditorEditMode desiredEditMode = eModelEditorEditMode_Select;
	for(int i=0;i<7; ++i)
		if(apWidget==mvButtons[i]) desiredEditMode = (eModelEditorEditMode)i;
	
	mpEditor->SetModelEditorEditMode(desiredEditMode);*/
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorEditModeToolbox, Button_Pressed);

//----------------------------------------------------------------

