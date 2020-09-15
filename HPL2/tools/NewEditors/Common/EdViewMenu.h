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

#ifndef ED_VIEW_MENU_H
#define ED_VIEW_MENU_H

//---------------------------------------------------------

#include "../Common/EdMenu.h"

//---------------------------------------------------------

class cEdViewMenu : public iEdMenu
{
public:
	cEdViewMenu(iEdWindow*);

protected:
	void OnCreateLayout();

	bool SetCamMode(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SetCamMode);
	bool SetCamController(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SetCamController);
	bool SetRenderer(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SetRenderer);
	bool ToggleGrid(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleGrid);
	bool ToggleAxes(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ToggleAxes);

	void UpdateMenu();

	cWidgetMenuItem* mpSubMenuRenderer;
	cWidgetMenuItem* mpSubMenuCamControl;

	tWidgetList mlstModes;

	cWidgetMenuItem* mpToggleGrid;
	cWidgetMenuItem* mpToggleAxes;
};

//---------------------------------------------------------

#endif // ED_VIEW_MENU_H
