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

#include "EditorWindowInputWorldName.h"
#include "EditorWorld.h"

//----------------------------------------------------------------------

cEditorWindowInputWorldName::cEditorWindowInputWorldName(iEditorBase* apEditor) : iEditorWindowPopUp(apEditor, 
																									 "Input name window", 
																									 true, true, false,
																									 cVector2f(200,130))
{
}

//----------------------------------------------------------------------

cEditorWindowInputWorldName::~cEditorWindowInputWorldName()
{
}

//----------------------------------------------------------------------

void cEditorWindowInputWorldName::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Input name"));

	mpLabelName = mpSet->CreateWidgetLabel(cVector3f(10,30,0.1f), 0, _W("Name"), mpWindow);
	AddWidget(mpLabelName);

	mpInputName = mpSet->CreateWidgetTextBox(cVector3f(10,50,0.1f), cVector2f(180, 25), _W(""), mpWindow);
	AddWidget(mpInputName);

	for(int i=0;i<2;++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(cVector3f(10 + (float)i*45,80,0.1f), cVector2f(40,25), _W(""), mpWindow);
		AddWidget(mvButtons[i]);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	}
	mvButtons[0]->SetText(_W("OK"));
	mvButtons[1]->SetText(_W("Cancel"));
}

//----------------------------------------------------------------------

void cEditorWindowInputWorldName::OnUpdate()
{
	iEditorWindowPopUp::OnUpdate();
}

//----------------------------------------------------------------------

bool cEditorWindowInputWorldName::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mvButtons[0])
	{
		iEditorWorld* pWorld = mpEditor->GetEditorWorld();
		tString sName = cString::To8Char(mpInputName->GetText());

		pWorld->SetName(sName);
	}
	else if(apWidget==mvButtons[1])
	{
	}

	mpEditor->SetLayoutNeedsUpdate(true);
	SetActive(false);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowInputWorldName, InputCallback);

//----------------------------------------------------------------------
