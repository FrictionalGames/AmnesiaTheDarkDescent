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

#ifndef LEVEL_EDITOR_WINDOW_GROUP_H
#define LEVEL_EDITOR_WINDOW_GROUP_H

#include "hpl.h"
using namespace hpl;

#include "LevelEditorWindow.h"

#include "LevelEditorTypes.h"

//---------------------------------------------------------------

class cLevelEditor;


//---------------------------------------------------------------

class cLevelEditorWindowGroup : public iEditorWindowPopUp
{
public:
	cLevelEditorWindowGroup(cLevelEditor* apEditor);
	~cLevelEditorWindowGroup();


protected:
	////////////////////////////////
	// Own functions
	bool MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MainMenu_ItemClick);

	bool GroupList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(GroupList_OnSelectionChange);

	bool EntityList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(EntityList_OnSelectionChange);

	bool VisibilityCheckBox_OnChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(VisibilityCheckBox_OnChange);

	bool Name_OnEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Name_OnEnter);

	void UpdateGroupList();
	void UpdateEntityList();
	void UpdateGroupParams();
	////////////////////////////////
	// Implemented functions
	void OnInitLayout();

	void OnWorldModify();

	void OnUpdate(float afTimeStep);

	////////////////////////////////
	// Data
	tGroupMap* mpGroupMap;
	tIntVec		mvGroupIDs;

	bool mbMustUpdateGroupList;
	bool mbMustUpdateList;

	cWidgetMainMenu* mpMainMenu;
	cWidgetMenuItem* mpMainMenuNew;
	cWidgetMenuItem* mpMainMenuDelete;
	cWidgetMenuItem* mpMainMenuAddSelected;
	cWidgetMenuItem* mpMainMenuRemoveSelected;
	cWidgetMenuItem* mpMainMenuSelect;
	cWidgetMenuItem* mpMainMenuDeselect;
	cWidgetMenuItem* mpMainMenuExit;

	cWidgetTextBox* mpInputName;
	cWidgetListBox* mpListGroups;
	cWidgetMultiPropertyListBox* mpListGroupEntities;
	cWidgetCheckBox* mpCheckBoxGroupVisibility;

};

//--------------------------------------------------------------------

#endif //LEVEL_EDITOR_WINDOW_GROUP_H
