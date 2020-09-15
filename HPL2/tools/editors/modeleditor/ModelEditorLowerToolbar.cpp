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

#include "ModelEditorLowerToolbar.h"

#include "ModelEditor.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorWindowViewport.h"
#include "../common/EditorGrid.h"
#include "../common/EditorActionMisc.h"

#include "ModelEditorWindowPhysicsTest.h"



//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cModelEditorLowerToolbar::cModelEditorLowerToolbar(iEditorBase* apEditor) : iEditorWindowLowerToolbar(apEditor)
{
	mpTestWindow = NULL;
}

//---------------------------------------------------------------

cModelEditorLowerToolbar::~cModelEditorLowerToolbar()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cModelEditorLowerToolbar::OnInitLayout()
{
	//Set up size of window
	SetSize(cVector2f(mpEditor->GetLayoutVec2f(eLayoutVec2_ViewportAreaSize).x,50));

	cVector3f vPos = cVector3f(5,5,0.1f);

	iWidget* pHandle = AddGridControls();
	pHandle->SetPosition(vPos);
	vPos.x += pHandle->GetSize().x+10;
	pHandle = AddViewportControls();
	pHandle->SetPosition(vPos);
	vPos.x += pHandle->GetSize().x+5;
	
	cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("button_test.tga", eGuiMaterial_Alpha);
	mpButtonTest = mpSet->CreateWidgetButton(vPos, 40, _W(""), mpBGFrame,true);
	mpButtonTest->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));
	mpButtonTest->SetImage(pImg);
	mpButtonTest->SetToolTip(_W("Start model test"));
	mpButtonTest->SetToolTipEnabled(true);

	pHandle = mpButtonTest;
	vPos.x += pHandle->GetSize().x+10;
	pHandle = AddLightingControls();
	pHandle->SetPosition(vPos);
	vPos.x += pHandle->GetSize().x+5;
	pHandle = AddCameraControls();
	pHandle->SetPosition(vPos);

}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

bool cModelEditorLowerToolbar::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mpButtonTest)
	{
		bool bTestActive = mpEditor->GetFlags(eModelEditorFlag_TestWindowActive);
		mpEditor->SetFlags(eModelEditorFlag_TestWindowActive, !bTestActive);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorLowerToolbar, ButtonPressed);

//---------------------------------------------------------------

void cModelEditorLowerToolbar::OnUpdate(float afTimeStep)
{
	iEditorWindowLowerToolbar::OnUpdate(afTimeStep);

	mpButtonTest->SetPressed(mpEditor->GetFlags(eModelEditorFlag_TestWindowActive), false);
}


//---------------------------------------------------------------
