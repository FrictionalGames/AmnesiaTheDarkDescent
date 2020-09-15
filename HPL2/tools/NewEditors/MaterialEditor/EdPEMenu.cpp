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

	cWidgetMenuItem* pView = AddItem(_W("View"));
	mpShowFloor = AddItem(_W("Show Floor"), pView, this, kGuiCallback(ToggleFloor));
	mpShowWalls = AddItem(_W("Show Walls"), pView, this, kGuiCallback(ToggleWalls));
}

void cEdPEMenu::OnWorldParamsChange()
{
	cHplParticleSystem* pPS = static_cast<cHplParticleSystem*>(mpEditor->GetWorld());
	mpShowFloor->SetChecked(pPS->GetShowFloor());
	mpShowWalls->SetChecked(pPS->GetShowWalls());
}

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