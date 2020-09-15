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

#include "../Common/DirectoryHandler.h"

#include "../Common/Editor.h"
#include "../Common/EditorTypes.h"

//-------------------------------------------------------------------------

// Lovely magic from the "LuxBasePersonal.h" in game code.  We should maybe make this into a engine function or something
#if defined(WIN32)
#define PERSONAL_RELATIVEROOT _W("")
#define PERSONAL_RELATIVEPIECES _W(""),
#define PERSONAL_RELATIVEPIECES_COUNT 0
#elif defined(__linux__)
#define PERSONAL_RELATIVEROOT _W(".frictionalgames/")
#define PERSONAL_RELATIVEPIECES _W(""), _W(".frictionalgames"),
#define PERSONAL_RELATIVEPIECES_COUNT 1
#else
#define PERSONAL_RELATIVEROOT _W("Library/Application Support/Frictional Games/")
#define PERSONAL_RELATIVEPIECES _W(""), _W("Library"), _W("Library/Application Support"), _W("Library/Application Support/Frictional Games"),
#define PERSONAL_RELATIVEPIECES_COUNT 3
#endif

//-------------------------------------------------------------------------

cLookupDirectory::cLookupDirectory(const tWString& asDir, bool abAddSubDirs)
{
	msDir = asDir;
	mbAddSubDirs = abAddSubDirs;
	mbIsUpdated = true;
}

//-------------------------------------------------------------------------

cEdDirectoryHandler::cEdDirectoryHandler(iEditor* apEditor) : iEdModule(apEditor, _W("DirectoryHandler"))
{
	msHomeDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);
	msHomeDir = cString::AddSlashAtEndW(msHomeDir);
	msHomeDir = cString::ReplaceCharToW(msHomeDir, _W("\\"), _W("/"));

	msWorkingDir = cPlatform::GetWorkingDir();
	msWorkingDir = cString::AddSlashAtEndW(msWorkingDir);
	msWorkingDir = cString::ReplaceCharToW(msWorkingDir, _W("\\"), _W("/"));
	tWString sSep = _W("/");  
	cString::GetStringVecW(msWorkingDir, mvWorkingDirPathSteps, &sSep);

	// Evil kludge here to work around VS C++ not liking an empty array initializer.
	// So we skip the first "dummy" element by doing i+1 in the loop
	tWString vDirs[] = { PERSONAL_RELATIVEPIECES };
	for(int i=0; i<PERSONAL_RELATIVEPIECES_COUNT; ++i) 
	{
		tWString sDir = msHomeDir + vDirs[i+1];
		if(cPlatform::FolderExists(sDir)==false)
			cPlatform::CreateFolder(sDir);
	}

	AddLookUpDir(-1, GetWorkingDir() + _W("editor"), true);
}

//-------------------------------------------------------------------------

void cEdDirectoryHandler::OnLoadGlobalConfig(cConfigFile* apCfg)
{
	////////////////////////////////////////////////
	// Properly name folders
	msHomeDirTail = apCfg->GetStringW("Directories", "HomeDir", _W("HPL2"));
	msHomeDir = msHomeDir + PERSONAL_RELATIVEROOT + msHomeDirTail;
	msHomeDir = cString::AddSlashAtEndW(msHomeDir);

	msTempDirTail = apCfg->GetStringW("Directories", "TempDir", _W("Temp"));
	msTempDir = msHomeDir + msTempDirTail;
	msTempDir = cString::AddSlashAtEndW(msTempDir);

	msThumbnailDirTail = apCfg->GetStringW("Directories", "ThumbnailsDir", _W("Thumbnails"));
	msThumbnailDir = msHomeDir + msThumbnailDirTail;
	msThumbnailDir = cString::AddSlashAtEndW(msThumbnailDir);

	/////////////////////////////////////////////////
	// Create folders if they don't exist
	tWString vDirs[] = { msHomeDir, msTempDir, msThumbnailDir, _W("") };
	for(int i=0; vDirs[i]!=_W(""); ++i)
	{
		if(cPlatform::FolderExists(vDirs[i])==false)
			cPlatform::CreateFolder(vDirs[i]);
	}
}

void cEdDirectoryHandler::OnSaveGlobalConfig(cConfigFile* apCfg)
{
	apCfg->SetString("Directories", "HomeDir", cString::To8Char(msHomeDirTail));
	apCfg->SetString("Directories", "TempDir", cString::To8Char(msTempDirTail));
	apCfg->SetString("Directories", "ThumbnailDir", cString::To8Char(msThumbnailDirTail));
}

//-------------------------------------------------------------------------

bool cEdDirectoryHandler::AddLookUpDir(int alCategory, const tWString& asDir,
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

void cEdDirectoryHandler::RefreshLookupDirs()
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
				AddDirToResources(lookupDir.msDir, lookupDir.mbAddSubDirs);
				lookupDir.mbIsUpdated = false;
			}
		}
	}
}

void cEdDirectoryHandler::AddDirToResources(const tWString& asDir, bool abAddSubdirs)
{
	mpEditor->GetEngine()->GetResources()->AddResourceDir(asDir, abAddSubdirs);
}

//-------------------------------------------------------------------------

const tWString& cEdDirectoryHandler::GetMainLookUpDir(int alCategory)
{
	tLookupDirVecMap::iterator it = mmapDirs.find(alCategory);

	if(it==mmapDirs.end())
		return gsEmptyWString;

	tLookupDirVec& vDirs = it->second;
	if(vDirs.empty())
		return gsEmptyWString;

	return vDirs[0].msDir;
}

//-------------------------------------------------------------------------

tWStringVec cEdDirectoryHandler::GetLookUpDirs(int alCategory)
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

tWString cEdDirectoryHandler::GetPathRelToWD(const tWString& asPath)
{
	if(asPath.empty())
		return asPath;

	tWString sSep = _W("/");

	tWString sRelPath;

	tWString sPath = cString::GetFilePathW(asPath);
	tWString sFile = cString::GetFileNameW(asPath);

	tWStringVec vPathSteps;
	cString::GetStringVecW(sPath, vPathSteps, &sSep);

	size_t i;
	for(i=0; i<mvWorkingDirPathSteps.size() && i<(int)vPathSteps.size(); ++i)
	{
		const tWString& sWDStep = mvWorkingDirPathSteps[i];
		const tWString& sPathStep = vPathSteps[i];

		if(IsSameDir(sPathStep,sWDStep)==false)	break;
	}

	if(i <mvWorkingDirPathSteps.size())
	{
		for(size_t j=i; j<mvWorkingDirPathSteps.size(); ++j)
			sRelPath += cString::AddSlashAtEndW(_W(".."));
	}

	for(size_t j=i; j<(int)vPathSteps.size(); ++j)
		sRelPath += cString::AddSlashAtEndW(vPathSteps[j]);

	sRelPath += sFile;

	return sRelPath;
}

//----------------------------------------------------------------------------

void cEdDirectoryHandler::OnInit()
{
	AddDirToResources(msThumbnailDir,false);
	RefreshLookupDirs();

	Log("DirectoryHandler init:\n");
	Log("\tWorking Dir: %ls\n", msWorkingDir.c_str());
	Log("\tHome Dir: %ls\n", msHomeDir.c_str());
	Log("\tTemp Dir: %ls\n", msTempDir.c_str());
	Log("\tThumbnail Dir: %ls\n", msThumbnailDir.c_str());
	Log("\tLookup Dirs:\n");
	tLookupDirVecMap::iterator it = mmapDirs.begin();
	for(;it!=mmapDirs.end(); ++it)
	{
		Log("\tCat %d:\n", it->first);
		tLookupDirVec& vDirs = it->second;
		for(size_t i=0; i<vDirs.size(); ++i)
		{
			cLookupDirectory& dir = vDirs[i];
			Log("\t\t%ls\n", dir.msDir.c_str());
		}
	}
}

//----------------------------------------------------------------------------

bool cEdDirectoryHandler::IsSameDir(const tWString& asDir1, const tWString& asDir2)
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
