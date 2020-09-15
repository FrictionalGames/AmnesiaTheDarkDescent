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

#include "../ModelEditor/EdLowerToolbarModel.h"

#include "../Common/Editor.h"

#include "../Common/EdActionHandler.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdSelection.h"
#include "../Common/EdGrid.h"

#include "../ModelEditor/HPLEntity.h"
//#include "../Common/EditorActionMisc.h"

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cEdLowerToolbarModel::cEdLowerToolbarModel(iEditor* apEditor) : iEdPaneLowerToolbar(apEditor)
{	
}

//---------------------------------------------------------------

cEdLowerToolbarModel::~cEdLowerToolbarModel()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEdLowerToolbarModel::OnCreateLayout()
{
	iEdPaneLowerToolbar::OnCreateLayout();

	cVector3f vPos = cVector3f(5,5,0.1f);

	iWidget* pHandle = AddGridControls();
	pHandle->SetPosition(vPos);

	vPos.x += pHandle->GetSize().x;
	
	pHandle = AddLightingControls();
	pHandle->SetPosition(vPos);
	
	vPos.x += pHandle->GetSize().x;

	pHandle = AddViewportControls();
	pHandle->SetPosition(vPos);

	vPos.x += pHandle->GetSize().x;

	cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("button_test.tga", eGuiMaterial_Alpha);
	mpBTest = mpSet->CreateWidgetButton(vPos, 40, _W(""), GetBG(), true);
	mpBTest->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ShowPhysicsTest));
	mpBTest->SetImage(pImg);
	mpBTest->SetToolTip(_W("Start model test"));
	mpBTest->SetToolTipEnabled(true);

	//////////////////////////////
	// Set up layout
	//mpEditor->GetWindowHandler()->GetLayoutBlock(eLayoutBlock_ViewportArea);
	//SetSize(cVector2f(mpEditor->GetLayoutVec2f(eLayoutVec2_ViewportAreaSize).x-4,50));
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEdLowerToolbarModel::OnUpdate()
{
	iEdPaneLowerToolbar::OnUpdate();

	//mpButtonTest->SetPressed(mpEditor->GetFlags(eModelEditorFlag_TestWindowActive), false);
}

//---------------------------------------------------------------

void cEdLowerToolbarModel::OnWorldParamsChange()
{
	iEdPaneLowerToolbar::OnWorldParamsChange();
}

//---------------------------------------------------------------

bool cEdLowerToolbarModel::WindowSpecificInputCallback(iEdInput* apInput)
{
	//iEdWorld* pWorld = mpEditor->GetWorld();

	//iEdAction* pAction = NULL;

	//mpEditor->AddAction(pAction);

	return iEdPaneLowerToolbar::WindowSpecificInputCallback(apInput);
}

bool cEdLowerToolbarModel::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(iEdPaneLowerToolbar::InputCallback(apWidget, aData)==true)
		return true;

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdLowerToolbarModel,InputCallback);

//---------------------------------------------------------------

bool cEdLowerToolbarModel::ShowPhysicsTest(iWidget* apWidget, const cGuiMessageData& aData)
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());

	bool bWindowActive = pEnt->ToggleTestWindow();
	mpBTest->SetPressed(bWindowActive, false);

	mpGridControlsGroup->SetEnabled(!bWindowActive);
	mpHandleViewportControls->SetEnabled(!bWindowActive);
	mpHandleLighting->SetEnabled(!bWindowActive);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdLowerToolbarModel,ShowPhysicsTest);

//---------------------------------------------------------------

