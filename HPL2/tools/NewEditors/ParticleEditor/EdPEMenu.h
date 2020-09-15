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

#ifndef ED_PE_MENU_H
#define ED_PE_MENU_H

#include "../Common/EdGenericMenu.h"

class cEdPEMenu : public cEdGenericMenu
{
public:
	cEdPEMenu(iEdWindow*);

protected:
	void OnCreateLayout();
	void CreateEditSubMenu(){}

	void OnWorldParamsChange();

	bool ToggleAxes(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleAxes);
	bool ToggleGrid(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleGrid);
	bool ToggleFloor(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleFloor);
	bool ToggleWalls(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleWalls);
	bool SetBGColor(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SetBGColor);

	// View menu
	cWidgetMenuItem* mpShowAxes;
	cWidgetMenuItem* mpShowGrid;
	cWidgetMenuItem* mpShowFloor;
	cWidgetMenuItem* mpShowWalls;
	std::vector<cWidgetMenuItem*>	mvSetColor;
};

#endif // ED_PE_MENU_H
