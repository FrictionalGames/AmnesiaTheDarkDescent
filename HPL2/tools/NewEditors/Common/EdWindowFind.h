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

#ifndef ED_WINDOW_FIND_H
#define ED_WINDOW_FIND_H

#include "../Common/EdWindowHandler.h"
#include "../Common/EdScnObject.h"

class cEdEditModeSelect;

class cEdWindowFind : public iEdPopUp
{
public:
	cEdWindowFind(iEditor* apEditor);
	~cEdWindowFind();


protected:
	bool SearchParam_OnChange(iWidget* apWidget,const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SearchParam_OnChange);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool StringLocation_OnCheckChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(StringLocation_OnCheckChange);

	void UpdateFoundEntities();
	void UpdateItemHighlight();

	void OnCreateLayout();
	void OnUpdate();

	void OnWorldAddRemObject();
	void OnWorldObjModify();
	void OnWorldSelectChange();

	void OnSetActive(bool abX);

	////////////////////////////////////
	// Data

	cEdEditModeSelect* mpEditMode;
	tScnObjList mlstFoundObjects;
	
	bool mbStartingWithLocation;
	bool mbMustUpdateList;
	bool mbUpdateHighlights;

	////////////////////////
	// Layout stuff

	cWidgetLabel* mpLabelSearch;
	cWidgetTextBox* mpInputSearch;

	cWidgetLabel* mpLabelInside;
	cWidgetComboBox* mpComboBoxInside;
	cWidgetMultiPropertyListBox* mpListFoundEntities;
	cWidgetButton* mvButtons[4];
};

#endif // ED_WINDOW_FIND_H 
