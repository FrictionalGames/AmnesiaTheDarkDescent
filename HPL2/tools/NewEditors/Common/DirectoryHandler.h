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

#ifndef ED_DIRECTORY_HANDLER_H
#define ED_DIRECTORY_HANDLER_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "../Common/EditorTypes.h"

#include "../common/EdModule.h"

//-------------------------------------------------------------------------

class iEditor;

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

class cEdDirectoryHandler : public iEdModule
{
public:
	cEdDirectoryHandler(iEditor* apEditor);

	bool AddLookUpDir(int alCategory, const tWString& asDir, 
						bool abAddSubDirsToResources = false, bool abUpdateResources = false);

	void RefreshLookupDirs();

	void AddDirToResources(const tWString& asDir, bool abAddSubdirs=false);

	const tWString& GetWorkingDir() { return msWorkingDir; }
	const tWString& GetHomeDir() { return msHomeDir; }
	const tWString& GetTempDir() { return msTempDir; }
	const tWString& GetThumbnailDir() { return msThumbnailDir; }

	// Lookup Dirs
	const tWString& GetMainLookUpDir(int alCategory);
	tWStringVec GetLookUpDirs(int alCategory);

	tWString GetPathRelToWD(const tWString& asPath);

protected:
	void OnInit();

	void OnLoadGlobalConfig(cConfigFile* apFile);
	void OnSaveGlobalConfig(cConfigFile* apFile);
	bool IsSameDir(const tWString& asDir1, const tWString& asDir2);

	tWString msWorkingDir;
	tWStringVec mvWorkingDirPathSteps;

	tWString msHomeDirTail;
	tWString msTempDirTail;
	tWString msThumbnailDirTail;

	tWString msHomeDir;
	tWString msTempDir;
	tWString msThumbnailDir;

    tLookupDirVecMap mmapDirs;
};

//-------------------------------------------------------------------------

#endif // ED_DIRECTORY_HANDLER_H
              