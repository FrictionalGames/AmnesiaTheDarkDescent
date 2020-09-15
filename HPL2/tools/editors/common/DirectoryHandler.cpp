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


#include "DirectoryHandler.h"

#include "EditorBaseClasses.h"

tWString gsNullStringW = _W("");

//-------------------------------------------------------------------------

#include "../../LuxBasePersonal.h"

//-------------------------------------------------------------------------

cLookupDirectory::cLookupDirectory(const tWString& asDir, bool abAddSubDirs)
{
	msDir = asDir;
	mbAddSubDirs = abAddSubDirs;
	mbIsUpdated = true;
}

//-------------------------------------------------------------------------
#ifdef __APPLE__
namespace hpl {
	extern tString FindGameResources();
}
#endif

cDirectoryHandler::cDirectoryHandler(iEditorBase* apEditor)
{
	mpEditor = apEditor;

	msHomeDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);
	msHomeDir = cString::AddSlashAtEndW(msHomeDir);
	msHomeDir = cString::ReplaceCharToW(msHomeDir, _W("\\"), _W("/"));

#ifdef __APPLE__
	// this is ONLY for OS X
	// Save off current working directory
	msEditorDir = cPlatform::GetWorkingDir();
	msEditorDir = cString::AddSlashAtEndW(msEditorDir);
	msEditorDir = cString::ReplaceCharToW(msEditorDir, _W("\\"), _W("/"));

	// Now find and switch to game directory
	tString gameDir = FindGameResources();
	if (gameDir.empty())
	{
		exit(1);
	}
#endif

	msWorkingDir = cPlatform::GetWorkingDir();
	msWorkingDir = cString::AddSlashAtEndW(msWorkingDir);
	msWorkingDir = cString::ReplaceCharToW(msWorkingDir, _W("\\"), _W("/"));
	tWString sSep = _W("/");  
	cString::GetStringVecW(msWorkingDir, mvWorkingDirPathSteps, &sSep);

	// Create the base personal folders
	tWStringVec vDirs;
#ifdef USERDIR_RESOURCES
	SetupBaseDirs(vDirs, _W("HPL2"), _W(""), true);
#else
	SetupBaseDirs(vDirs);
#endif
	CreateBaseDirs(vDirs, msHomeDir);
}

//-------------------------------------------------------------------------

void cDirectoryHandler::OnLoadGlobalConfig(cConfigFile* apCfg)
{
	////////////////////////////////////////////////
	// Properly name folders
	tWString sPersonalDir = msHomeDir;
#ifdef USERDIR_RESOURCES
	tWString sUserParentDir = sPersonalDir + PERSONAL_RELATIVEROOT
			+ apCfg->GetStringW("Directories", "GameHomeDir", PERSONAL_RELATIVEGAME_PARENT);
	msUserResourceDir = sPersonalDir + PERSONAL_RELATIVEROOT
			+ apCfg->GetStringW("Directories", "GameHomeDir", PERSONAL_RELATIVEGAME_PARENT)
			+ PERSONAL_RESOURCES;
	msUserResourceDir = cString::AddSlashAtEndW(msUserResourceDir);
#endif

	msHomeDir = sPersonalDir + PERSONAL_RELATIVEROOT + apCfg->GetStringW("Directories", "EditorHomeDir", _W("HPL2"));
	msHomeDir = cString::AddSlashAtEndW(msHomeDir);

	msTempDir = msHomeDir + apCfg->GetStringW("Directories", "EditorTempDir", _W("Temp"));
	msTempDir = cString::AddSlashAtEndW(msTempDir);

	msThumbnailDir = msHomeDir + apCfg->GetStringW("Directories", "ThumbnailsDir", _W("Thumbnails"));
	msThumbnailDir = cString::AddSlashAtEndW(msThumbnailDir);

	/////////////////////////////////////////////////
	// Create folders if they don't exist
	tWString vDirs[] = { msHomeDir, msTempDir, msThumbnailDir,
#ifdef USERDIR_RESOURCES
		sUserParentDir, msUserResourceDir,
#endif
		_W("") };
	for(int i=0; vDirs[i]!=_W(""); ++i)
	{
		if(cPlatform::FolderExists(vDirs[i])==false)
			cPlatform::CreateFolder(vDirs[i]);
	}
}

//-------------------------------------------------------------------------

bool cDirectoryHandler::AddLookUpDir(int alCategory, const tWString& asDir,
									  bool abAddSubdirs, bool abUpdateResources)
{
	if(cPlatform::FolderExists(asDir)==false)
	{
		cPlatform::CreateMessageBox(_W("Error"), _W("Could not find lookup folder: %ls"), 
										cString::ReplaceCharToW(asDir,_W("\\"),_W("/")).c_str());
		return false;
	}

	tLookupDirVecMap::iterator it = mmapDirs.find(alCategory);
	if(it==mmapDirs.end())
	{
		tLookupDirVec vDirs = tLookupDirVec(1, cLookupDirectory(asDir, abAddSubdirs));
		mmapDirs.insert(std::pair<int, tLookupDirVec>(alCategory, vDirs));
	}
	else
	{
		bool bAdded = false;
		tLookupDirVec& vDirs = it->second;

		for(int i=0;i<(int)vDirs.size(); ++i)
		{
			cLookupDirectory& dir = (cLookupDirectory&) vDirs[i];

			if(IsSameDir(dir.msDir, asDir))
			{
				dir.mbAddSubDirs = abAddSubdirs;
				dir.mbIsUpdated = true;

				bAdded = true;

				break;
			}
		}
		if(bAdded==false)
			vDirs.push_back(cLookupDirectory(asDir, abAddSubdirs));
	}

	if(abUpdateResources)
		RefreshLookupDirs();

	return true;
}

//-------------------------------------------------------------------------

void cDirectoryHandler::RefreshLookupDirs()
{
	tLookupDirVecMap::iterator it = mmapDirs.begin();
	for(;it!=mmapDirs.end();++it)
	{
		tLookupDirVec& vDirs = it->second;
		for(int i=0;i<(int)vDirs.size();++i)
		{
			cLookupDirectory& lookupDir = vDirs[i];

			if(lookupDir.mbIsUpdated)
			{
				mpEditor->GetEngine()->GetResources()->AddResourceDir(lookupDir.msDir, lookupDir.mbAddSubDirs);
				lookupDir.mbIsUpdated = false;
			}
		}
	}
}

//-------------------------------------------------------------------------

const tWString& cDirectoryHandler::GetMainLookUpDir(int alCategory)
{
	tLookupDirVecMap::iterator it = mmapDirs.find(alCategory);

	if(it==mmapDirs.end())
		return gsNullStringW;

	tLookupDirVec& vDirs = it->second;
	if(vDirs.empty())
		return gsNullStringW;

	return vDirs[0].msDir;
}

//-------------------------------------------------------------------------

tWStringVec cDirectoryHandler::GetLookUpDirs(int alCategory)
{
	tLookupDirVecMap::const_iterator it = mmapDirs.find(alCategory);

	if(it==mmapDirs.end())
		return tWStringVec();

	tWStringVec vDirNames;
	const tLookupDirVec& vLookUpDirs = it->second;
	for(int i=0;i<(int)vLookUpDirs.size();++i)
		vDirNames.push_back(vLookUpDirs[i].msDir);

	return vDirNames;
}

//-------------------------------------------------------------------------

tWString cDirectoryHandler::GetPathRelToWD(const tWString& asPath)
{
	tWString sSep = _W("/");

	tWString sRelPath;

	tWString sPath = cString::GetFilePathW(asPath);
	tWString sFile = cString::GetFileNameW(asPath);

	tWStringVec vPathSteps;
	cString::GetStringVecW(sPath, vPathSteps, &sSep);

	int i;
	for(i=0; i<(int)mvWorkingDirPathSteps.size() && i<(int)vPathSteps.size(); ++i)
	{
		const tWString& sWDStep = mvWorkingDirPathSteps[i];
		const tWString& sPathStep = vPathSteps[i];

		if(IsSameDir(sPathStep,sWDStep)==false)	break;
	}

	if(i < (int)mvWorkingDirPathSteps.size())
	{
		for(int j=i; j<(int)mvWorkingDirPathSteps.size(); ++j)
			sRelPath += cString::AddSlashAtEndW(_W(".."));
	}

	for(int j=i; j<(int)vPathSteps.size(); ++j)
		sRelPath += cString::AddSlashAtEndW(vPathSteps[j]);

	sRelPath += sFile;

	return sRelPath;
}

//----------------------------------------------------------------------------

bool cDirectoryHandler::IsSameDir(const tWString& asDir1, const tWString& asDir2)
{
	tWString sDir1 = cString::ReplaceCharToW(asDir1, _W("\\"), _W("/"));
	tWString sDir2 = cString::ReplaceCharToW(asDir2, _W("\\"), _W("/"));

#ifdef WIN32
	sDir1 = cString::ToLowerCaseW(sDir1);
	sDir2 = cString::ToLowerCaseW(sDir2);
#endif

	return sDir1==sDir2;
}

//----------------------------------------------------------------------------
