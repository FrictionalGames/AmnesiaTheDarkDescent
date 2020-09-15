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

#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

//---------------------------------------------------------

#include "../Common/Editor.h"

class cEdWindowFind;
class cEdWindowGroup;

//---------------------------------------------------------

enum eDirExtension
{
	eDir_StaticObjects = eDir_LastEnum,
	eDir_Entities,
	eDir_Decals
};

enum eEdObjID_Extended
{
	eEdObjID_StaticObject,
};

enum eLayoutBlock_Extended
{
	eLayoutBlock_EditModeSidebar = eLayoutBlock_Editor_LastEnum,
	eLayoutBlock_LowerToolbar,
};

//---------------------------------------------------------

class cLevelEditor : public iEditor
{
public:
	cLevelEditor();

	void ShowFindDialog();
	void ShowGroupDialog();
	void ShowLevelSettingsDialog();

protected:
	void SetUpDirectories();
	void SetUpEditModes();

	iEdWindow* CreatePreferencesDialog();

	iEdMenu*	CreateMenu();
    iEdWorld*	CreateWorld();

	void CreateAppSpecificLayout();

	cEdWindowFind* mpWindowFind;
	cEdWindowGroup* mpWindowGroup;
};

//---------------------------------------------------------

#endif // LEVEL_EDITOR_H
