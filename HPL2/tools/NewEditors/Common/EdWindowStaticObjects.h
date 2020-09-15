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

#ifndef ED_WINDOW_STATIC_OBJECTS_H
#define ED_WINDOW_STATIC_OBJECTS_H

#include "../Common/EdWindowObjectBrowser.h"

//----------------------------------------------------------

class cEdEditModeStaticObjects;

class cEdObjectIndexStaticObjects;
class cEdObjectIndexDirStaticObjects;
class cEdObjectIndexEntryStaticObject;


//----------------------------------------------------------

class cEdObjectIndexEntryStaticObject : public iEdObjectIndexEntryMeshObject
{
public:
	cEdObjectIndexEntryStaticObject(cEdObjectIndexDirStaticObjects* apDir);
};

class cEdObjectIndexDirStaticObjects : public iEdObjectIndexDir
{
public:
	cEdObjectIndexDirStaticObjects(cEdObjectIndexStaticObjects* apIndex, cEdObjectIndexDirStaticObjects* apParent=NULL);

	iEdObjectIndexEntry* CreateEntry();
};

class cEdObjectIndexStaticObjects : public iEdObjectIndex
{
public:
	cEdObjectIndexStaticObjects(iEditor* apEditor, const tWString& asBaseFolder);

	iEdObjectIndexDir* CreateDir(iEdObjectIndexDir* apParent=NULL);
};

//----------------------------------------------------------

class cEdWindowStaticObjects : public cEdWindowObjectBrowser
{
public:
	cEdWindowStaticObjects(iEdEditMode*, const tWStringVec& avBaseDirs);
	~cEdWindowStaticObjects();

	bool SetUpCreationData(iEdObjectData*);

protected:
	iEdObjectIndex* CreateSpecificIndex(iEditor* apEditor, const tWString& asFolder);

	void OnInit(){}
	void OnCreateLayout();
	void OnLoadLevel(){}
	void OnUpdate(){}
	//void OnSetActive(bool abX);

	void ResetInputs();

	cMeshEntity* CreatePreviewEntity(iEdObjectIndexEntryMeshObject* apEntry);


	////////////////////////////////////////////////////
	// Data
	tWString msBaseDir;
	tWStringVec mvDirectories;
};

//----------------------------------------------------------

#endif // ED_WINDOW_STATIC_OBJECTS_H
