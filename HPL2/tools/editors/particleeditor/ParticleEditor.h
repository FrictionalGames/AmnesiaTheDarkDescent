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

#include "ParticleEditorTypes.h"

//--------------------------------------------------------------------

class iEditorWindow;

class iEditorEditMode;
class cEditorWindowViewport;
class cEditorActionHandler;

class cParticleEditor;
class cEditorGrid;
class iEditorWorld;

class cEditorWindowTextureBrowser;
class cEditorWindowOptions;

class cParticleEditorWindowEmitters;


//--------------------------------------------------------------------
//--------------------------------------------------------------------

class cParticleEditor : public iEditorBase
{
public:
	cParticleEditor();
	~cParticleEditor();

	void ViewportMouseDown(cEditorWindowViewport* apViewport, int alButtons){}
	void ViewportMouseUp(cEditorWindowViewport* apViewport, int alButtons){}

protected:
	///////////////////////////
	// Own functions
	bool MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MainMenu_ItemClick);

	void OnNew();
	void OnPostWorldLoad();
	void AppSpecificReset();
	void AppSpecificLoad(iXmlDocument* apDoc);
	void AppSpecificSave(iXmlDocument* apDoc);

	void UpdateMenu();
	void UpdateEditMenu();

	///////////////////////////
	// Implemented functions
	iEditorWorld* CreateSpecificWorld();
	iEditorWindowLowerToolbar* CreateSpecificLowerToolbar() { return NULL; }
	iEditorWindowEditModeSidebar* CreateEditModeSidebar() { return NULL; }

	cWidgetMainMenu* CreateMainMenu();
	void SetUpWindowAreas();
	void CreateViewports();

	void OnSetUpDirectories();

	void OnUpdate(float afTimeStep);
	void OnSetSelectedViewport(){}

	void OnPostUpdateLayout();

	void OnInit();
	void OnInitInput();
	void OnInitLayout();

	void OnLoadConfig();
	void OnSaveConfig();

	void SetUpClassDefinitions(cEditorUserClassDefinitionManager* apManager) {}

	///////////////////////////
	// Data
	
	// File menu
	cWidgetMenuItem* mpMainMenuNew;
	cWidgetMenuItem* mpMainMenuSave;
	cWidgetMenuItem* mpMainMenuSaveAs;
	cWidgetMenuItem* mpMainMenuLoad;
	cWidgetMenuItem* mpMainMenuRecent;
	cWidgetMenuItem* mpMainMenuExit;

	// Edit menu
	cWidgetMenuItem* mpMainMenuUndo;
	cWidgetMenuItem* mpMainMenuRedo;
	cWidgetMenuItem* mpMainMenuOptions;

	// View menu
	cWidgetMenuItem* mpMIShowFloor;
	cWidgetMenuItem* mpMIShowWalls;
	cWidgetMenuItem* mpMISetColor[3];

	////////////////////////////////
	// Some windows
	cEditorWindowOptions* mpWindowOptions;
	cParticleEditorWindowEmitters* mpWEmitters;

	///////////////////////
	// Config stuff
	cConfigFile* mpLocalConfig;
};

//----------------------------------------------------------

#endif //LEVEL_EDITOR_H

