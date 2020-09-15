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

#include "LevelEditorLowerToolbar.h"

#include "LevelEditor.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorWindowViewport.h"
#include "../common/EditorGrid.h"
#include "../common/EditorWorld.h"
#include "../common/EditorActionMisc.h"



//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cLevelEditorLowerToolbar::cLevelEditorLowerToolbar(cLevelEditor* apEditor) : iLevelEditorWindow(apEditor, "Lower Toolbar" )
{
}

//---------------------------------------------------------------

cLevelEditorLowerToolbar::~cLevelEditorLowerToolbar()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cLevelEditorLowerToolbar::OnInitLayout()
{
	//////////////////////////////
	// Set up layout
	SetSize(cVector2f(670,50));

	// Grid Controls group
	mpGridControlsGroup = mpSet->CreateWidgetGroup(cVector3f(5,5,0.01f),cVector2f(440,43),_W("Grid Controls"), mpBGFrame);
	AddWidget(mpGridControlsGroup);

	// Buttons
	cVector3f vPos = cVector3f(5,10,0.1f);
	for(int i=0;i<3;++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(0, 25,_W(""), mpGridControlsGroup);
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(ButtonPressed));
		mvButtons[i]->SetPosition(vPos);
		vPos.x+=30;

		AddWidget(mvButtons[i]);
	}

	mvButtons[0]->SetText(_W("XZ"));
	mvButtons[1]->SetText(_W("XY"));
	mvButtons[2]->SetText(_W("YZ"));

	// Grid parameters
	for(int i=0;i<2;++i)
	{
		mvLabels[i] = mpSet->CreateWidgetLabel(0,75,_W(""),mpGridControlsGroup);
		mvTextBoxes[i] = mpSet->CreateWidgetTextBox(0,75,_W(""), mpGridControlsGroup, eWidgetTextBoxInputType_Numeric,0.5f);
		mvTextBoxes[i]->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(TextBoxChange));

		AddWidget(mvLabels[i]);
		AddWidget(mvTextBoxes[i]);
	}

	mvLabels[0]->SetPosition(cVector3f(105,15,0.1f));
	mvLabels[0]->SetText(_W("Height"));
	mvTextBoxes[0]->SetPosition(cVector3f(150,10,0.1f));
	mvTextBoxes[0]->SetNumericAdd(0.25f);

	mvLabels[1]->SetPosition(cVector3f(240,15,0.1f));
	mvLabels[1]->SetText(_W("Snap Sep."));
	mvTextBoxes[1]->SetPosition(cVector3f(305,10,0.1f));
	mvTextBoxes[1]->SetNumericAdd(0.25f);

	mpSnapToGridCheckBox = mpSet->CreateWidgetCheckBox(cVector3f(385,10,0.1f), cVector2f(40,20), _W("Snap"), mpGridControlsGroup);
	mpSnapToGridCheckBox->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(CheckBoxChange));
	AddWidget(mpSnapToGridCheckBox);

	// Enlarge viewport button
	cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("button_enlarge.tga", eGuiMaterial_Alpha);
	mvButtons[3] = mpSet->CreateWidgetButton(cVector3f(450, 5, 0.1f), 40,_W(""), mpBGFrame);
	mvButtons[3]->SetToggleable(true);
	mvButtons[3]->SetImage(pImg);
	mvButtons[3]->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(ButtonPressed));
	mvButtons[3]->SetToolTip(_W("Enlarge current viewport"));
	mvButtons[3]->SetToolTipEnabled(true);
	AddWidget(mvButtons[3]);
	mpSet->AddGlobalShortcut(0, eKey_Space, mvButtons[3], eGuiMessage_ButtonPressed);

	mpCheckBoxGlobalAmbientLight = mpSet->CreateWidgetCheckBox(cVector3f(510,10,0.1f), 0,_W("Global Ambient Light"), mpBGFrame);
	mpCheckBoxGlobalAmbientLight->AddCallback(eGuiMessage_CheckChange,this,kGuiCallback(CheckBoxChange));
	AddWidget(mpCheckBoxGlobalAmbientLight);
	
	
	mpCheckBoxGlobalPointLight = mpSet->CreateWidgetCheckBox(cVector3f(510,30,0.1f), 0,_W("Global Point Light"), mpBGFrame);
	mpCheckBoxGlobalPointLight->AddCallback(eGuiMessage_CheckChange,this,kGuiCallback(CheckBoxChange));
	AddWidget(mpCheckBoxGlobalPointLight);
	
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

bool cLevelEditorLowerToolbar::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEditorGrid* pGrid = mpEditor->GetFocusedViewport()->GetGrid();
	float fSnap = pGrid->GetSnapSeparation();
	float fHeight = pGrid->GetHeight();
	iEditorAction* pAction = NULL;

	///////////////////////////
	// GridPlane XZ button
	if(apWidget==mvButtons[0])
	{
		pAction = hplNew(cEditorActionGridChangeProperties,(pGrid, eGridPlane_XZ, fHeight, fSnap));
	}
	///////////////////////////
	// GridPlane XY button
	if(apWidget==mvButtons[1])
	{
		pAction = hplNew(cEditorActionGridChangeProperties,(pGrid, eGridPlane_XY, fHeight, fSnap));
	}
	///////////////////////////
	// GridPlane YZ button
	if(apWidget==mvButtons[2])
	{
		pAction = hplNew(cEditorActionGridChangeProperties,(pGrid, eGridPlane_YZ, fHeight, fSnap));
	}
	///////////////////////////
	// Enlarge Viewport
	if(apWidget==mvButtons[3])
	{
		bool bEnlarged = mpEditor->GetFlags(eEditorFlag_ViewportEnlarged);
		mpEditor->SetFlags(eEditorFlag_ViewportEnlarged, !bEnlarged);
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorLowerToolbar, ButtonPressed);

//---------------------------------------------------------------

bool cLevelEditorLowerToolbar::TextBoxChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEditorGrid* pGrid = mpEditor->GetFocusedViewport()->GetGrid();
	cWidgetTextBox* pBox = (cWidgetTextBox*)apWidget;
	
	float fValue = pBox->GetNumericValue();
	eGridPlane plane = pGrid->GetPlaneAxis();

	////////////////////////////////////
	// Grid Height TextBox
	if(apWidget == mvTextBoxes[0])
	{
		if(pGrid)
		{
			float fHeight = fValue;
			float fSnapSeparation = pGrid->GetSnapSeparation();

			cEditorActionGridChangeProperties* pAction = hplNew(cEditorActionGridChangeProperties,
																(pGrid, plane, fHeight, fSnapSeparation));
			mpEditor->AddAction(pAction);
		}

	}

    ////////////////////////////////////
	// Grid Snap Separation TextBox
	if(apWidget == mvTextBoxes[1])
	{
		float fOldValue = pGrid->GetSnapSeparation();

		if(fValue<=0)
		{
			fValue = fOldValue; 
			if(pGrid) pGrid->SetSnapSeparation(fValue);
		}
		else
		{
			float fHeight = pGrid->GetHeight();
			float fSnapSeparation = fValue;

			cEditorActionGridChangeProperties* pAction = hplNew(cEditorActionGridChangeProperties,
																(pGrid, plane,fHeight, fSnapSeparation));
			mpEditor->AddAction(pAction);
		}
	}

	((cWidgetTextBox*)apWidget)->SetNumericValue(fValue);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorLowerToolbar,TextBoxChange);

//---------------------------------------------------------------

bool cLevelEditorLowerToolbar::CheckBoxChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	if(apWidget==mpSnapToGridCheckBox)
	{
		//////////////////////////////////////
		// Change grid settings and recalculate position on grid, the latter done to avoid a lil artifact
		cEditorGrid::SetSnapToGrid(mpSnapToGridCheckBox->IsChecked());
		mpEditor->SetPosOnGrid(mpEditor->GetPosOnGridFromMousePos());
	}
	else if(apWidget==this->mpCheckBoxGlobalAmbientLight)
	{
		pWorld->SetGlobalAmbientLightEnabled(mpCheckBoxGlobalAmbientLight->IsChecked());
	}
	else if(apWidget==this->mpCheckBoxGlobalPointLight)
	{
		pWorld->SetGlobalPointLightEnabled(mpCheckBoxGlobalPointLight->IsChecked());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorLowerToolbar,CheckBoxChange);

//---------------------------------------------------------------

void cLevelEditorLowerToolbar::OnUpdate(float afTimeStep)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	cEditorWindowViewport* pViewport = mpEditor->GetFocusedViewport();
	cEditorGrid* pGrid = pViewport->GetGrid();

	bool bPlaneChangeActive = (pViewport->GetVCamera()->IsOrtho()==false);
    
	float fHeight = pGrid->GetHeight();
	float fSnapSeparation = pGrid->GetSnapSeparation();

	for(int i=0;i<3;++i)
	{
		mvButtons[i]->SetEnabled(bPlaneChangeActive);
	}

	mvTextBoxes[0]->SetNumericValue(fHeight);
	mvTextBoxes[1]->SetNumericValue(fSnapSeparation);

	mpSnapToGridCheckBox->SetChecked(pGrid->GetSnapToGrid());

	mpCheckBoxGlobalAmbientLight->SetChecked(pWorld->GetGlobalAmbientLightEnabled());
	mpCheckBoxGlobalPointLight->SetChecked(pWorld->GetGlobalPointLightEnabled());

	mvButtons[3]->SetPressed(mpEditor->GetFlags(eEditorFlag_ViewportEnlarged));
}

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------
