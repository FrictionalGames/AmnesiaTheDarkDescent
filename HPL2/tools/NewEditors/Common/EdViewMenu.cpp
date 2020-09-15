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

#include "../Common/EdViewMenu.h"

#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdCamera.h"

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cEdViewMenu::cEdViewMenu(iEdWindow* apWindow) : iEdMenu(apWindow, _W("View"))
{
}

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

//------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

void cEdViewMenu::OnCreateLayout()
{
	iEdMenu::OnCreateLayout();

	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);

	cWidgetMenuItem* pViewSubMenu = AddItem(_W("View"));

	cWidgetMenuItem* pCamModeItem = AddItem(_W("Camera Mode"), pViewSubMenu);
	for(int i=0; i<pViewport->GetCamera()->GetNumModes(); ++i)
	{
		iCamMode* pMode = pViewport->GetCamera()->GetMode(i);

		mlstModes.push_back(AddItem(pMode->GetName(), pCamModeItem, 
									this, kGuiCallback(SetCamMode), 
									eKeyModifier_None, eKey_None, 
									pMode->GetID()));
	}

	mpSubMenuRenderer = AddItem(_W("Render Mode"), pViewSubMenu);
	mpSubMenuCamControl = AddItem(_W("Controller"), pViewSubMenu);

	mpToggleGrid = AddItem(_W("Show Grid"), pViewSubMenu, this, kGuiCallback(ToggleGrid), eKeyModifier_None, eKey_G);
	mpToggleAxes = AddItem(_W("Show Axes"), pViewSubMenu, this, kGuiCallback(ToggleAxes), eKeyModifier_None, eKey_A);
}

//------------------------------------------------------------------

bool cEdViewMenu::SetCamMode(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);
	pViewport->GetCamera()->SetCameraMode(apWidget->GetUserValue());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdViewMenu, SetCamMode);

//------------------------------------------------------------------

bool cEdViewMenu::SetCamController(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);
	pViewport->GetCamera()->GetCurrentCamMode()->SetController(apWidget->GetUserValue());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdViewMenu, SetCamController);

//------------------------------------------------------------------

bool cEdViewMenu::SetRenderer(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);
	pViewport->SetRenderMode((eRenderer)apWidget->GetUserValue());

	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdViewMenu, SetRenderer);

//------------------------------------------------------------------

bool cEdViewMenu::ToggleGrid(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bActive = pItem->IsChecked();

	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);
	pViewport->SetDrawGrid(!bActive);

	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdViewMenu, ToggleGrid);

//------------------------------------------------------------------

bool cEdViewMenu::ToggleAxes(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bActive = pItem->IsChecked();

	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);
	pViewport->SetDrawAxes(!bActive);

	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdViewMenu, ToggleAxes);

//------------------------------------------------------------------

void cEdViewMenu::UpdateMenu()
{
	cEdWindowViewport* pViewport = static_cast<cEdWindowViewport*>(mpWindow);

	mpToggleGrid->SetChecked(pViewport->GetDrawGrid());
	mpToggleAxes->SetChecked(pViewport->GetDrawAxes());

	cEdCamera* pCam = pViewport->GetCamera();
	tWidgetListIt it = mlstModes.begin();
	iCamMode* pCurrentMode = pCam->GetCurrentCamMode();
	for(int i=0; i<pCam->GetNumModes(); ++i, ++it)
	{
		iCamMode* pMode = pCam->GetMode(i);
		bool bCurrent = pCurrentMode==pMode;

		cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(*it);
		pItem->SetChecked(bCurrent);
	}


	iCamController* pCurrentController = pCurrentMode->GetCurrentController();
	mpSubMenuCamControl->ClearMenuItems();
	for(int i=0; i<pCurrentMode->GetNumControllers(); ++i)
	{
		iCamController* pController = pCurrentMode->GetController(i);
		bool bCurrent = (pCurrentController==pController);

		cWidgetMenuItem* pItem = AddItem(pController->GetName(), mpSubMenuCamControl, this, kGuiCallback(SetCamController), eKeyModifier_None, eKey_None, i); 
		pItem->SetChecked(bCurrent);
	}

	cGraphics* pGfx = mpEditor->GetEngine()->GetGraphics();
	iRenderer* pCurrentRenderer = pViewport->GetEngineViewport()->GetRenderer();
	mpSubMenuRenderer->ClearMenuItems();
	for(int i=0; i<pCurrentMode->GetNumRenderers(); ++i)
	{
		cEdCamRenderer* pCamRenderer = pCurrentMode->GetRenderer(i);
		
		iRenderer* pRenderer = pGfx->GetRenderer(pCamRenderer->mRenderer);
		bool bCurrent = (pCurrentRenderer==pRenderer);

		cWidgetMenuItem* pItem = AddItem(pCamRenderer->msName, mpSubMenuRenderer, this, kGuiCallback(SetRenderer), eKeyModifier_None, eKey_None, pCamRenderer->mRenderer); 
		pItem->SetChecked(bCurrent);
	}
}


//------------------------------------------------------------------
