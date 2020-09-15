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

#include "../Common/GenericBG.h"

#include "../Common/Editor.h"

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGenericBG::cGenericBG(iEditor* apEditor) : iEdPane(apEditor, _W("MainBackground"))
{
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGenericBG::OnCreateLayout()
{
	iEdPane::OnCreateLayout();

	mpBGWidget->SetGlobalKeyPressListener(true);
	mpBGWidget->AddCallback(eGuiMessage_KeyPress, this, kGuiCallback(KeypressCallback));

	cWidgetFrame* pFrame = static_cast<cWidgetFrame*>(mpBGWidget);
	pFrame->SetBackGroundColor(cColor(0.5f,0.5f,0.5f,1));
}
//-----------------------------------------------------------------------

void cGenericBG::OnActionExec()
{
	SetUpdated();
}

void cGenericBG::OnWorldLoad()
{
}

void cGenericBG::OnWorldSave()
{
}

//-----------------------------------------------------------------------

bool cGenericBG::KeypressCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mKeyPress.mKey == eKey_Escape)
		mpEditor->GetEngine()->Exit();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cGenericBG, KeypressCallback);

//-----------------------------------------------------------------------
