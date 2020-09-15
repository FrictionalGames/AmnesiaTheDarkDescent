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

#ifndef MODEL_EDITOR_WINDOW_USER_SETTINGS_H
#define MODEL_EDITOR_WINDOW_USER_SETTINGS_H

#include "../common/EditorWindow.h"
#include "../common/EditorVar.h"

#include "ModelEditorTypes.h"

//--------------------------------------------------------------------------------

class cModelEditor;
class cModelEditorWindowUserSettings;
class cEntityCustomVar;
class iCustomVarInput;

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// USER SETTINGS WINDOW
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cModelEditorWindowUserSettings : public iEditorWindowPopUp
{
public:
	cModelEditorWindowUserSettings(cModelEditor* apEditor);
	~cModelEditorWindowUserSettings();

protected:
	void OnSetActive(bool abX);
	void OnInitLayout();
	void OnUpdate(float afTimeStep);

	bool TypeList_OnChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(TypeList_OnChange);
	
	bool SubTypeList_OnChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SubTypeList_OnChange);

	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);

	void PopulateTypeList();
	void PopulateSubTypeList();
	void PopulateVarList();

	void OrganizeVarInputs();

	void SetTypeSubTypeCombo(const tString& asType, const tString& asSubType);

	///////////////////////////////////////////////////
	// Data
	cModelEditor* mpEditor;

	cWidgetLabel* mpLabelType;
	cWidgetComboBox* mpComboBoxType;
	cWidgetLabel* mpLabelSubType;
	cWidgetComboBox* mpComboBoxSubType;

	cWidgetFrame* mpFrameVars;
	cEditorVarInputPanel* mpInputPanel;
	

	cWidgetButton* mvButtons[2];
};

//--------------------------------------------------------------------------------

#endif // MODEL_EDITOR_WINDOW_USER_SETTINGS_H
