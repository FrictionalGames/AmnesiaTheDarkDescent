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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITIES_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITIES_H

#include "EditorWindowObjectBrowser.h"

//----------------------------------------------------------

class cEditorEditModeEntities;

class cEditorObjectIndexEntities;
class cEditorObjectIndexDirEntities;
class cEditorObjectIndexEntryEntity;


//----------------------------------------------------------

class cEditorObjectIndexEntryEntity : public iEditorObjectIndexEntryMeshObject
{
public:
	cEditorObjectIndexEntryEntity(cEditorObjectIndexDirEntities* apDir);

	//tWString& GetMeshFileName();

	cMeshEntity* CreateTempEntity(cWorld* apWorld);
};

class cEditorObjectIndexDirEntities : public iEditorObjectIndexDir
{
public:
	cEditorObjectIndexDirEntities(cEditorObjectIndexEntities* apIndex, cEditorObjectIndexDirEntities* apParent=NULL);

	iEditorObjectIndexEntry* CreateEntry();
};

class cEditorObjectIndexEntities : public iEditorObjectIndex
{
public:
	cEditorObjectIndexEntities(iEditorBase* apEditor, const tWString& asBaseFolder);

	iEditorObjectIndexDir* CreateDir(iEditorObjectIndexDir* apParent=NULL);
};

//----------------------------------------------------------

class cEditorWindowEntities : public cEditorWindowObjectBrowser
{
public:
	cEditorWindowEntities(cEditorEditModeEntities* apEditMode, const tWStringVec& avBaseDirs);
	~cEditorWindowEntities();

	void Reset();
protected:
	iEditorObjectIndex* CreateSpecificIndex(iEditorBase* apEditorBase, const tWString& asFolder);

	void OnInit(){}
	void OnInitLayout();
	void OnLoadLevel(){}
	void OnUpdate(float afTimeStep){}
	void OnSetActive(bool abX);
	
	bool Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Input_OnTextBoxEnter);

	void ResetInputs();

	cMeshEntity* CreatePreviewEntity(iEditorObjectIndexEntryMeshObject* apEntry);

	////////////////////////////////////////////////////
	// Data
};

//----------------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_STATIC_OBJECTS_H
