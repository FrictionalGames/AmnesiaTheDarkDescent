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

#ifndef HPLEDITOR_DIRECTORY_HANDLER_H
#define HPLEDITOR_DIRECTORY_HANDLER_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorTypes.h"

#include "../common/EdModule.h"

//-------------------------------------------------------------------------

class iEditorBase;

//-------------------------------------------------------------------------

class cLookupDirectory
{
public:
	cLookupDirectory(const tWString& asDir, bool abAddSubDirs);

	void Update();

	tWString msDir;

	bool mbAddSubDirs;
	bool mbIsUpdated;
};

//-------------------------------------------------------------------------

typedef std::vector<cLookupDirectory> tLookupDirVec;

typedef std::map<int, tLookupDirVec> tLookupDirVecMap;

//-------------------------------------------------------------------------

class cDirectoryHandler
{
public:
	cDirectoryHandler(iEditorBase* apEditor);

	void OnLoadGlobalConfig(cConfigFile* apFile);

	bool AddLookUpDir(int alCategory, const tWString& asDir, 
						bool abAddSubDirsToResources = false, bool abUpdateResources = false);

	void RefreshLookupDirs();

	const tWString& GetWorkingDir() { return msWorkingDir; }
	const tWString& GetHomeDir() { return msHomeDir; }
	const tWString& GetTempDir() { return msTempDir; }
	const tWString& GetThumbnailDir() { return msThumbnailDir; }
#ifdef USERDIR_RESOURCES
	const tWString& GetUserResourceDir() { return msUserResourceDir; }
#endif
#ifdef __APPLE__
	const tWString& GetEditorDir() { return msEditorDir; }
#endif

	// Lookup Dirs
	const tWString& GetMainLookUpDir(int alCategory);
	tWStringVec GetLookUpDirs(int alCategory);

	tWString GetPathRelToWD(const tWString& asPath);

protected:

	bool IsSameDir(const tWString& asDir1, const tWString& asDir2);

	iEditorBase* mpEditor;

	tWString msWorkingDir;
	tWStringVec mvWorkingDirPathSteps;

	tWString msHomeDir;
	tWString msTempDir;
	tWString msThumbnailDir;
#ifdef USERDIR_RESOURCES
	tWString msUserResourceDir;
#endif
#ifdef __APPLE__
	tWString msEditorDir;
#endif

	tLookupDirVecMap mmapDirs;
};

//-------------------------------------------------------------------------

#endif // HPLEDITOR_DIRECTORY_HANDLER_H
              
