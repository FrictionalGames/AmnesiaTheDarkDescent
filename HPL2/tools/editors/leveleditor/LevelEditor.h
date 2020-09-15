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

#include "hpl.h"

using namespace hpl;

#include "../common/EditorBaseClasses.h"
#include "../common/EditorTypes.h"
#include "LevelEditorTypes.h"

//--------------------------------------------------------------------

class iEditorWindow;

class iEditorEditMode;
class cEditorWindowViewport;
class cEditorActionHandler;

class cLevelEditor;
class cEditorGrid;
class iEditorWorld;

class cEditorWindowEntitySearch;
class cLevelEditorWindowGroup;

class iEditorWindowLowerToolbar;
class iEditorWindowEditModeSidebar;

class cEditorWindowTextureBrowser;
class cEditorWindowOptions;

//--------------------------------------------------------------------

enum eDirExt
{
	eDir_Maps = eDir_LastEnum,
	eDir_StaticObjects,
	eDir_Entities,

	eDirExt_LastEnum
};


//--------------------------------------------------------------------

////////////////////////////////////
// Level Editor Group
//  Used to group entities int the level editor. Entities keep ID for a group stored.
//
class cLevelEditorGroup
{
public:
	cLevelEditorGroup();
	cLevelEditorGroup(cLevelEditor* apEditor,unsigned int alID, const tString& asName);

	/**
	 * Iterates all entities to find those of correct ID and set visibility
	 */
	void SetVisibility(bool abX);
	bool GetVisibility() { return mbVisibility; }

	void SetName(const tString& asName) { msName = asName; }
	const tString& GetName() { return msName; }

	unsigned int GetID() { return mlID; }

	static unsigned int GetGroupCount() { return mlGroupCounter; }
	static void IncGroupCount() { ++mlGroupCounter; }

protected:
	cLevelEditor* mpEditor;
	unsigned int mlID;
	tString msName;
	bool mbVisibility;


	static unsigned int mlGroupCounter;
};


//--------------------------------------------------------------------

////////////////////////////////////
// Level Editor
//  Base class for the level editor. Contains global functions specific to the level editor. 
//  It is also responsible for (by using functionality from iEditorBase) created all needed GUI
//  and structures needed when Engine calls the Run command.
class cLevelEditor : public iEditorBase
{
public:
	cLevelEditor();
	~cLevelEditor();

	void LookAtEntity(int alEntityID);

	// Open up a file picker for importing the contents of a map file into the current map. 
	void Command_Import();
	
	
	// Open up a file picker for exporting all currently selected entities into a map file.
	void Command_Export();

	///////////////////////////////
	// Group Tool stuff
	void AddGroup(unsigned int alID, const tString& asName);
	void RemoveGroup(unsigned int alID);

	cLevelEditorGroup& GetGroup(unsigned int alID);
	tGroupMap& GetGroups() { return mmapGroups; }

	void SetUpClassDefinitions(cEditorUserClassDefinitionManager* apManager);

	const tWStringVec& GetStaticObjectExtraDirs() { return mvExtraSODirs; }
	const tWStringVec& GetEntityExtraDirs() { return mvExtraEntDirs; }

protected:
	///////////////////////////
	// Own functions
	bool MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MainMenu_ItemClick);

	bool MainMenu_UndoRedo(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MainMenu_UndoRedo);

	void AppSpecificReset();
	//void AppSpecificLoad(iXmlDocument* apDoc);
	//void AppSpecificSave(iXmlDocument* apDoc);

	void LoadEditorSession(iXmlDocument* apDoc, cXmlElement** apElement);
	void SaveEditorSession(iXmlDocument* apDoc, cXmlElement** apElement);

	bool ImportFileCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ImportFileCallback);
	bool ExportFileCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ExportFileCallback);

	iEditorWorld* CreateSpecificWorld();

	cWidgetMainMenu* CreateMainMenu();
	void UpdateEditMenu();

	///////////////////////////
	// Implemented functions
	void OnSetUpDirectories();

	void OnUpdate(float afTimeStep);
	void OnSetSelectedViewport(){}

	void OnPostUpdateLayout();

	void OnInit();
	void OnInitInput();
	void OnInitLayout();

	void OnLoadConfig();
	void OnSaveConfig();

	///////////////////////////
	// Data
	tWString msLastImportFile;	//Last file path used for an import operation. Default = ??
	tWString msLastImportPath;	//Last file path used for an export operation. Default = ??

	tWStringVec mvExtraSODirs;
	tWStringVec mvExtraEntDirs;

	/////////////////////////////
	// Main menu
	cWidgetMainMenu* mpMainMenu;
	
	// File menu
	cWidgetMenuItem* mpMainMenuNew;
	cWidgetMenuItem* mpMainMenuSave;
	cWidgetMenuItem* mpMainMenuSaveAs;
	cWidgetMenuItem* mpMainMenuLoad;
	cWidgetMenuItem* mpMainMenuRecent;
	cWidgetMenuItem* mpMainMenuImport;
	cWidgetMenuItem* mpMainMenuExport;
	cWidgetMenuItem* mpMainMenuExit;

	// Edit menu
	cWidgetMenuItem* mpMainMenuUndo;
	cWidgetMenuItem* mpMainMenuRedo;
	cWidgetMenuItem* mpMainMenuDelete;
	cWidgetMenuItem* mpMainMenuClone;
	cWidgetMenuItem* mpMainMenuSearch;
	cWidgetMenuItem* mpMainMenuGroup;
	cWidgetMenuItem* mpMainMenuLevelSettings;
	cWidgetMenuItem* mpMainMenuOptions;
	cWidgetMenuItem* mpMainMenuCompound;


	
	////////////////////////////////
	// Some windows
	cEditorWindowOptions* mpWindowOptions;
	cEditorWindowEntitySearch* mpWindowSearch;
	cEditorWindowTextureBrowser* mpWindowTextureBrowser;

	cLevelEditorWindowGroup* mpWindowGroup;


	////////////////////////////////
	// Groups
	tGroupMap mmapGroups;	//Map for all entity groups currently created. (map specific)

	///////////////////////
	// Config stuff
	cConfigFile* mpLocalConfig;
};

//----------------------------------------------------------

#endif //LEVEL_EDITOR_H

