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

#ifndef ED_WINDOW_COMBINE
#define ED_WINDOW_COMBINE

//---------------------------------------------------------

#include "../Common/EdWindowHandler.h"
#include "../Common/EdEditModeCombine.h"

//---------------------------------------------------------

class cLevelEditorStaticObjectCombo;

/////////////////////////////////////////////////////////////
// cEdWindowCombine
//	Window belonging to EditModeCombine. Holds inputs to
//	manage the EditMode operation
class cEdWindowCombine : public iEdEditModePane
{
public:
	cEdWindowCombine(iEdEditMode*);

	/**
	 * Returns combo currently selected in the ComboBox
	 */
	cLevelEditorStaticObjectCombo* GetCurrentCombo();

	/** 
	 * Returns selected operation mode (add or remove objects)
	 */
	eCombinePickMode GetPickMode();

	/**
	 * Returns if all combos should be displayed
	 */
	bool GetShowAllCombos() { return mpInpShowAll->GetValue(); }

protected:
	void OnCreateLayout();
	void OnUpdate();

	bool ButtonPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ButtonPressed);

	bool WindowSpecificInputCallback(iEdInput* apInput);

	//////////////////////////////////////
	// Layout stuff
	cWidgetButton* mpBAddComb;
	cWidgetButton* mpBRemComb;

	cEdInputBool* mpInpShowAll;

	cEdInputEnum* mpInpCombinations;
	cEdInputBool* mvInpPickModes[3];
	cEdInputColor* mpInpCombColor;

	cWidgetListBox* mpLBStaticObjects;
	bool mbAddedNewCombo;

	cLevelEditorStaticObjectCombo* mpSelectedCombo;

	// Combo extra data
	tIntVec mvComboIDs;
};

//---------------------------------------------------------

#endif // ED_WINDOW_COMBINE
