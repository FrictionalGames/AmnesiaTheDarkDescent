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

#include "../ParticleEditor/EdPEMenu.h"
#include "../ParticleEditor/HPLParticleSystem.h"
#include "../ParticleEditor/EdPEViewport.h"

#include "../Common/Editor.h"
#include "../Common/EdWindowHandler.h"

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

cEdPEMenu::cEdPEMenu(iEdWindow* apWin) : cEdGenericMenu(apWin)
{
}

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

void cEdPEMenu::OnCreateLayout()
{
	cEdGenericMenu::OnCreateLayout();

	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem = NULL;

	pItem = AddItem(_W("File"));
	AddNew(pItem);
	pItem->AddSeparator();
	AddOpen(pItem);
	AddSave(pItem);
	AddSaveAs(pItem);
	pItem->AddSeparator();
	AddRecentFiles(pItem);
	pItem->AddSeparator();
	AddExit(pItem);

	pItem = AddItem(_W("View"));

	mpShowAxes = AddItem(_W("Show Axes"), pItem, this, kGuiCallback(ToggleAxes), eKeyModifier_None, eKey_A);
	mpShowGrid = AddItem(_W("Show Grid"), pItem, this, kGuiCallback(ToggleGrid), eKeyModifier_None, eKey_G);
	
	pSubItem = AddItem(_W("Room"), pItem);

    mpShowFloor = AddItem(_W("Show Floor"), pSubItem, this, kGuiCallback(ToggleFloor), eKeyModifier_None, eKey_F);
	mpShowWalls = AddItem(_W("Show Walls"), pSubItem, this, kGuiCallback(ToggleWalls), eKeyModifier_None, eKey_W);

	pSubItem = AddItem(_W("BG Color"), pItem);
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpEditor->GetWorld());
	for(int i=0; i<pPS->GetSelectableBGColorNum(); ++i)
	{
		const tWString& sLabel = pPS->GetSelectableBGColorName(i);
		cWidgetMenuItem* pColorItem = AddItem(sLabel, pSubItem, this, kGuiCallback(SetBGColor), eKeyModifier_None, eKey_None, i);

		mvSetColor.push_back(pColorItem);
	}
}

void cEdPEMenu::OnWorldParamsChange()
{
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpEditor->GetWorld());
	cEdPEViewport* pViewport = static_cast<cEdPEViewport*>(mpEditor->GetFocusedViewport());

	mpShowAxes->SetChecked(pViewport->GetDrawAxes());
	mpShowGrid->SetChecked(pViewport->GetDrawGrid());
	
	mpShowFloor->SetChecked(pPS->GetShowFloor());
	mpShowWalls->SetChecked(pPS->GetShowWalls());

	const cColor& selectedCol = pPS->GetBGColor();
	for(int i=0; i<pPS->GetSelectableBGColorNum(); ++i)
	{
		const cColor& color = pPS->GetSelectableBGColor(i);
		cWidgetMenuItem* pColorItem = mvSetColor[i];

		bool bChecked = color==selectedCol;
		pColorItem->SetChecked(bChecked);
	}
}

bool cEdPEMenu::ToggleAxes(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bToggle = !pItem->IsChecked();

	cEdPEViewport* pViewport = static_cast<cEdPEViewport*>(mpEditor->GetFocusedViewport());
	pViewport->SetDrawAxes(bToggle);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPEMenu, ToggleAxes);

bool cEdPEMenu::ToggleGrid(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bToggle = !pItem->IsChecked();

	cEdPEViewport* pViewport = static_cast<cEdPEViewport*>(mpEditor->GetFocusedViewport());
	pViewport->SetDrawGrid(bToggle);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPEMenu, ToggleGrid);

bool cEdPEMenu::ToggleFloor(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bToggle = !pItem->IsChecked();

	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpWindow->GetEditor()->GetWorld());
	pPS->SetShowFloor(bToggle);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPEMenu, ToggleFloor);

bool cEdPEMenu::ToggleWalls(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWidgetMenuItem* pItem = static_cast<cWidgetMenuItem*>(apWidget);
	bool bToggle = !pItem->IsChecked();

	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpWindow->GetEditor()->GetWorld());
	pPS->SetShowWalls(bToggle);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPEMenu, ToggleWalls);

bool cEdPEMenu::SetBGColor(iWidget* apWidget, const cGuiMessageData& aData)
{
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpEditor->GetWorld());
	pPS->SetSelectableBGColor(apWidget->GetUserValue());

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPEMenu, SetBGColor);
