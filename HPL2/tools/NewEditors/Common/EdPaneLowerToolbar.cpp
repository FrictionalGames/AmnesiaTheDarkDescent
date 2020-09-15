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

#include "../Common/EdPaneLowerToolbar.h"

#include "../Common/Editor.h"

#include "../Common/EdActionHandler.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdSelection.h"
#include "../Common/EdGrid.h"
#include "../Common/EdClipPlane.h"
#include "../Common/EdWorld.h"

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

iEdPaneLowerToolbar::iEdPaneLowerToolbar(iEditor* apEditor) : iEdPane(apEditor, _W("Lower Toolbar"))
{	
	mpGridControlsGroup = NULL;
	mpBPlaneSwitch = NULL;
	mpBSnap = NULL;
	mpGridControlsGroup = NULL;
	mpInpPlaneHeight = NULL;
	mpInpSnapSep = NULL;
	
	mpHandleViewportControls = NULL;
	mpBEnlargeViewport = NULL;

	mpHandleLighting =NULL;
	mpBGlobalAmbientLight = NULL;
	mpBGlobalPointLight = NULL;

	mpHandleCamera = NULL;
	mpBCameraLockToGrid = NULL;
}

//---------------------------------------------------------------

iEdPaneLowerToolbar::~iEdPaneLowerToolbar()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void iEdPaneLowerToolbar::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
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

iWidget* iEdPaneLowerToolbar::AddGridControls()
{
	// Grid Controls group
	mpGridControlsGroup = mpSet->CreateWidgetGroup(0,cVector2f(200,43),_W("Grid Controls"), GetBG());
	mpGridControlsGroup->SetDefaultFontSize(iEdInput::GetFontSize());

	// Buttons

	// Plane switch
	mpBPlaneSwitch = mpSet->CreateWidgetButton(cVector3f(5,10,0.1f), 25, _W(""), mpGridControlsGroup);
	mpBPlaneSwitch->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBPlaneSwitch->SetToolTip(_W("Cycle through grid planes"));
	mpBPlaneSwitch->SetToolTipEnabled(true);

	////////////////////
	// Snap controls
	// Active
	cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("button_snap.tga", eGuiMaterial_Alpha);
	mpBSnap = mpSet->CreateWidgetButton(cVector3f(33, 10, 0.1f), 25, _W(""), mpGridControlsGroup);
	mpBSnap->SetToggleable(true);
	mpBSnap->SetImage(pImg);
	mpBSnap->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBSnap->SetToolTip(_W("Toggle grid snapping"));
	mpBSnap->SetToolTipEnabled(true);

	// Height and sep.
	mpInpPlaneHeight = CreateInputNumber(cVector3f(65,5,0.1f), _W("Height"),  mpGridControlsGroup, 50, 0.25f);
	mpInpSnapSep = CreateInputNumber(cVector3f(120,5,0.1f), _W("Snap sep."),  mpGridControlsGroup, 50, 0.25f); 

	return mpGridControlsGroup;
}

iWidget* iEdPaneLowerToolbar::AddLightingControls()
{
	mpHandleLighting = mpSet->CreateWidgetDummy(0, GetBG());

	mpBGlobalAmbientLight = mpSet->CreateWidgetButton(cVector3f(0,0,0.1f), 19, _W("A"), mpHandleLighting);
	mpBGlobalAmbientLight->SetToolTip(_W("Toggle global ambient light"));
	mpBGlobalAmbientLight->SetToolTipEnabled(true);
	mpBGlobalAmbientLight->SetToggleable(true);
	mpBGlobalAmbientLight->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	mpBGlobalPointLight = mpSet->CreateWidgetButton(cVector3f(0,21,0.1f), 19, _W("P"), mpHandleLighting);
	mpBGlobalPointLight->SetToolTip(_W("Toggle global point light"));
	mpBGlobalPointLight->SetToolTipEnabled(true);
	mpBGlobalPointLight->SetToggleable(true);
	mpBGlobalPointLight->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	return mpHandleLighting;
}

iWidget* iEdPaneLowerToolbar::AddViewportControls()
{
	mpHandleViewportControls = mpSet->CreateWidgetDummy(0, GetBG());

	cGuiGfxElement* pImg = mpSet->GetGui()->CreateGfxImage("button_enlarge.tga", eGuiMaterial_Alpha);
	mpBEnlargeViewport = mpSet->CreateWidgetButton(0, 40, _W(""), mpHandleViewportControls);
	mpBEnlargeViewport->SetToggleable(true);
	mpBEnlargeViewport->SetImage(pImg);
	mpBEnlargeViewport->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(InputCallback));
	mpBEnlargeViewport->SetToolTip(_W("Enlarge current viewport"));
	mpBEnlargeViewport->SetToolTipEnabled(true);

	cVector3f vPos = mpBEnlargeViewport->GetLocalPosition() + cVector3f(mpBEnlargeViewport->GetSize().x, 0, 0);

	mpBCameraLockToGrid = mpSet->CreateWidgetButton(vPos, 19, _W("LT"), mpHandleViewportControls);
	mpBCameraLockToGrid->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBCameraLockToGrid->SetToggleable(true);
	mpBCameraLockToGrid->SetToolTip(_W("Lock camera tracking to grid"));
	mpBCameraLockToGrid->SetToolTipEnabled(true);

	mpBCameraFocusOnSelection = mpSet->CreateWidgetButton(vPos + cVector3f(0,21,0), 19, _W("F"), mpHandleViewportControls);
	mpBCameraFocusOnSelection->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
	mpBCameraFocusOnSelection->SetToolTip(_W("Focus on selection"));
	mpBCameraFocusOnSelection->SetToolTipEnabled(true);
	mpBCameraFocusOnSelection->AddShortcut(0, eKey_F, eGuiMessage_ButtonPressed);

	AddShortcut(mpBEnlargeViewport->AddShortcut(0, eKey_Space, eGuiMessage_ButtonPressed));
	
	return mpHandleViewportControls;
}

iWidget* iEdPaneLowerToolbar::AddCameraControls()
{
	mpHandleCamera = mpSet->CreateWidgetDummy(0, GetBG());


	return mpHandleCamera;
}

//---------------------------------------------------------------

//---------------------------------------------------------------

void iEdPaneLowerToolbar::OnUpdate()
{
	iEdWorld* pWorld = mpEditor->GetWorld();
	iEdViewport* pViewport = mpEditor->GetFocusedViewport();
	cEdGrid* pGrid = pViewport->GetGrid();

	iCamMode* pMode = pViewport->GetCamera()->GetCurrentCamMode();

	//bool bCameraIsOrtho = pViewport->GetVCamera()->IsOrtho();
    
	float fHeight = pGrid->GetHeight();
	float fSnapSeparation = pGrid->GetSnapSeparation();

	if(mpGridControlsGroup)
	{
		mpBPlaneSwitch->SetText(pGrid->GetPlaneString());
		mpBPlaneSwitch->SetEnabled(pMode->AllowGridPlaneSwitch());

		mpInpPlaneHeight->SetValue(fHeight, false);
		mpInpSnapSep->SetValue(fSnapSeparation, false);
		mpBSnap->SetPressed(pGrid->GetSnapToGrid(), false);
	}

	if(mpHandleLighting)
	{
		mpBGlobalAmbientLight->SetPressed(pWorld->GetAmbientLightEnabled(), false);
		mpBGlobalPointLight->SetPressed(pWorld->GetPointLightEnabled(), false);
	}

	if(mpHandleViewportControls)
	{
		mpBEnlargeViewport->SetPressed(mpEditor->IsViewportEnlarged(), false);
		mpBCameraLockToGrid->SetEnabled(pMode->AllowLockToGridSwitch());
		mpBCameraLockToGrid->SetPressed(pMode->GetCurrentController()->IsLockedToGrid());
	}

	if(mpHandleCamera)
	{
		//mpBCameraLockToGrid->SetEnabled(bCameraIsOrtho==false);
		//mpBCameraLockToGrid->SetPressed(mpEditor->GetFocusedViewport()->GetVCamera()->IsLockedToGrid(), false);
	}
}

//---------------------------------------------------------------

void iEdPaneLowerToolbar::OnViewportFocusChange(iEdViewport* apViewport)
{
	SetUpdated();
}

void iEdPaneLowerToolbar::OnViewportParamsChange(iEdViewport* apViewport)
{
	SetUpdated();
}

//---------------------------------------------------------------

void iEdPaneLowerToolbar::OnWorldParamsChange()
{
	SetUpdated();
}

//---------------------------------------------------------------

bool iEdPaneLowerToolbar::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdWorld* pWorld = mpEditor->GetWorld();

	cEdGrid* pGrid = mpEditor->GetFocusedViewport()->GetGrid();
	ePlaneNormal plane = pGrid->GetPlaneNormal();
	iEdAction* pAction = NULL;

	if(apInput==mpInpPlaneHeight)
	{
		cVector3f vHeight = pGrid->GetHeights();
		vHeight.v[pGrid->GetPlaneNormal()] = mpInpPlaneHeight->GetValue();
		pAction = pGrid->CreateSetHeightsAction(vHeight);
	}
	else if(apInput==mpInpSnapSep)
	{
		pAction = pGrid->CreateSetSnapSeparationAction(mpInpSnapSep->GetValue());
	}
	
	mpEditor->AddAction(pAction);

	return pAction!=NULL;
}

bool iEdPaneLowerToolbar::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdWorld* pWorld = mpEditor->GetWorld();

	cEdGrid* pGrid = mpEditor->GetFocusedViewport()->GetGrid();
	float fSnap = pGrid->GetSnapSeparation();
	float fHeight = pGrid->GetHeight();

	iEdAction* pAction = NULL;

	///////////////////////////
	// Enlarge Viewport
	if(apWidget==mpBEnlargeViewport)
	{
		bool bEnlarged = mpEditor->IsViewportEnlarged();
		mpEditor->SetViewportEnlarge(!bEnlarged);
	}
	///////////////////////////
	// GridPlane switch button
	else if(apWidget==mpBPlaneSwitch)
	{
		int currentPlane = (int)pGrid->GetPlaneNormal();

		pAction = pGrid->CreateSetNormalAction(++currentPlane);
	}
	///////////////////////////
	// Grid snap button
	else if(apWidget==mpBSnap)
		pGrid->SetSnapToGrid(mpBSnap->IsPressed());
	///////////////////////////
	// Ambient light button
	else if(apWidget==mpBGlobalAmbientLight)
		pWorld->SetAmbientLightEnabled(mpBGlobalAmbientLight->IsPressed());
	///////////////////////////
	// Point light button
	else if(apWidget==mpBGlobalPointLight)
		pWorld->SetPointLightEnabled(mpBGlobalPointLight->IsPressed());
	///////////////////////////
	// Camera lock to grid button
	else if(apWidget==mpBCameraLockToGrid)
	{
		mpEditor->GetFocusedViewport()->GetCamera()->GetCurrentController()->LockToGrid(mpBCameraLockToGrid->IsPressed());
	}
	else if(apWidget==mpBCameraFocusOnSelection)
	{
		cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpEditor->GetFocusedViewport());
		pAction = pViewport->CreateFocusAction();
	}
	else
	{
		return false;
	}
	
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEdPaneLowerToolbar,InputCallback);

//---------------------------------------------------------------
