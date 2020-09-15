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

#ifndef ED_WINDOW_ENTITY_CLASS_H
#define ED_WINDOW_ENTITY_CLASS_H

#include "../Common/EdWindowHandler.h"
#include "../Common/EdVar.h"

//#include "ModelEditorTypes.h"

//--------------------------------------------------------------------------------

//class cModelEditor;
//class cEdWindowEntityClass;
//class cEntityCustomVar;
//class iCustomVarInput;

class cEdUserClass;

//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////
// USER SETTINGS WINDOW
//////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

class cEdWindowEntityClass : public iEdPopUp
{
public:
	cEdWindowEntityClass(iEditor*);
	~cEdWindowEntityClass();

protected:
	void OnSetActive(bool abX);
	void OnCreateLayout();
	void OnUpdate();

	void OnActionExec();
	void OnWorldParamsChange();

	bool WindowSpecificInputCallback(iEdInput*);
	
	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);

	void PopulateTypes();
	void PopulateTypesHelper(cEdUserClass*);

	void PopulateVariables();
	//void PopulateTypeList();
	//void PopulateSubTypeList();
	//void PopulateVarList();

	//void OrganizeVarInputs();

	//void SetTypeSubTypeCombo(const tString& asType, const tString& asSubType);

	///////////////////////////////////////////////////
	// Data
	//cWidgetLabel* mpLabelType;
	//cWidgetComboBox* mpComboBoxType;
	//cWidgetLabel* mpLabelSubType;
	//cWidgetComboBox* mpComboBoxSubType;

	cEdInputEnum* mpInpType;
	bool mbTypeChanged;

	cWidgetFrame* mpFrameVars;
	iEdVarInputPanel* mpInputPanel;

	cEdUserClass* mpBackupClass;
	tVarValueMap mmapBackupValues;
	
	cWidgetButton* mvButtons[2];
};

//--------------------------------------------------------------------------------

#endif // ED_WINDOW_ENTITY_CLASS_H
