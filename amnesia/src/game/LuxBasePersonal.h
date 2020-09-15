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

/*
 *  LusBasePersonal.h
 *  Lux
 *
 *  Created by Edward Rudd on 8/27/10.
 *  Copyright 2010 Frictional Games All rights reserved.
 *
 */

/////////////////////////
// Multi platform personal directory specifics.
#if defined(WIN32)
#define PERSONAL_RELATIVEROOT _W("")
#define PERSONAL_RELATIVEPIECES
#define PERSONAL_RELATIVEPIECES_COUNT 0
#elif defined(__linux__)
#define PERSONAL_RELATIVEROOT _W(".frictionalgames/")
#define PERSONAL_RELATIVEPIECES _W(".frictionalgames"),
#define PERSONAL_RELATIVEPIECES_COUNT 1
#else
#define PERSONAL_RELATIVEROOT _W("Library/Application Support/Frictional Games/")
#define PERSONAL_RELATIVEPIECES _W("Library"), _W("Library/Application Support"), _W("Library/Application Support/Frictional Games"),
#define PERSONAL_RELATIVEPIECES_COUNT 3
#endif
#define PERSONAL_RELATIVEGAME_PARENT _W("Amnesia/")
#define PERSONAL_RESOURCES _W("local_resources/")
namespace hpl {
inline void SetupBaseDirs(tWStringVec& vDirs, const tWString& asRelativeParent = _W(""), const tWString& asMainFolder = _W(""),
                                        bool userDir = false, const tWString& asCustomStoryPath = _W(""))
{
    vDirs.clear();
#if PERSONAL_RELATIVEPIECES_COUNT > 0
    tWString aDirs[] = { PERSONAL_RELATIVEPIECES };
    for (int i = 0; i < PERSONAL_RELATIVEPIECES_COUNT; ++i) {
        vDirs.push_back(aDirs[i]);
    }
#endif
    if (asRelativeParent.length()) {
        vDirs.push_back(PERSONAL_RELATIVEROOT + asRelativeParent);
        if (asMainFolder.length()) {
            vDirs.push_back(PERSONAL_RELATIVEROOT + asRelativeParent + asMainFolder + _W("/"));
        }
    }
    vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT);
#ifndef HPL_MINIMAL
    iFileBrowser::msGameDir = cPlatform::GetWorkingDir();
    iFileBrowser::msPersonalDir = PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT;
#endif
    if (userDir) {
        vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES);
        if (asCustomStoryPath.length()) {
            vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES
                            + asCustomStoryPath + _W("/"));
        }
    }
}

inline void CreateBaseDirs(const tWStringVec& vDirs, const tWString& asRoot)
{
	//Check if directories exist and if not create
    for(tWStringVec::const_iterator it = vDirs.begin(); it != vDirs.end(); ++it)
	{
		tWString sDir = asRoot + (*it);
		if(cPlatform::FolderExists(sDir)) continue;

		cPlatform::CreateFolder(sDir);
	}
}
}
