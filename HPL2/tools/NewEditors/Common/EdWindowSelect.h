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

#ifndef ED_WINDOW_SELECT_H
#define ED_WINDOW_SELECT_H

#include "../Common/StdAfx.h"

using namespace hpl;

//---------------------------------------------------

#include "../Common/EdWindowHandler.h"
#include "../Common/EdEditModeSelect.h"

//---------------------------------------------------

class cEdWindowSelect : public iEdEditModePane
{
public:
	cEdWindowSelect(iEdEditMode*);

	void PositionEditPane(iEdScnObjEditPane*);
protected:
	bool TypeChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TypeChange);

	bool ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ButtonPressed);

	bool FilterButtonPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FilterButtonPressed);

	bool WindowSpecificInputCallback(iEdInput*) { return false; }

	void UpdateButtonHighLight();
	void UpdateFilterButtons();

	void OnCreateLayout();
	void OnUpdate();

	/**
	 * Adds a button to be able to filter the given type
	 */
	void AddFilter(iEdObjectType*);

	/////////////////////////////////////////////////////////////////
	// Data
	//eSelectToolMode mToolMode;

	cWidgetGroup* mpObjectTypeGroup;
	tWidgetList mlstBFilters;

	cWidgetCheckBox* mpCBSelectAll;
	cWidgetCheckBox* mpCBSelectMultipleTypes;

	cWidgetGroup* mpToolSelectGroup;
	cWidgetButton* mvButtons[3];

	tWidgetList mlstManipulatorButtons;
};

//---------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_SELECT_H
