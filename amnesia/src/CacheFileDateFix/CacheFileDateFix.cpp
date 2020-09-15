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

#include "hpl.h"

#include <sys/stat.h>
#include <fstream>
#include <stdio.h>

using namespace hpl;

cEngine *gpEngine=NULL;
int glNumOfProblems = 0;
int glNumOfFiles = 0;
int glNumOfSkippedFiles = 0;

//------------------------------------------------

bool IsCorrectType(const tWString & asFileName)
{
	tWString sExt = cString::ToLowerCaseW(cString::GetFileExtW(asFileName));
	if(sExt  == _W("map_cache")) return true;
	if(sExt  == _W("nodes")) return true;
	if(sExt  == _W("msh")) return true;
	if(sExt  == _W("anm")) return true;
	
	return false;
}

tWString GetParentExt(const tWString & asFileName)
{
	tWString sExt = cString::ToLowerCaseW(cString::GetFileExtW(asFileName));
	if(sExt  == _W("map_cache")) return _W("map");
	if(sExt  == _W("nodes")) return _W("");
	if(sExt  == _W("msh")) return _W("dae");
	if(sExt  == _W("anm")) return _W("dae_anim");

	return false;
}


void IterateFiles(const tWString & asDir)
{
	
	////////////////////////////////
	// Iterate files
	tWStringList lstFiles;
	cPlatform::FindFilesInDir(lstFiles,asDir, _W("*.*"));

	for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
	{	
		const tWString &sFileName = *it;
		if(IsCorrectType(sFileName)==false) continue;

		tWString sPath = cString::SetFilePathW(sFileName, asDir);
		
		if(cPlatform::FileExists(sPath)==false)
		{
			Warning("%s does not exist!\n", cString::To8Char(sPath).c_str());
			glNumOfProblems++;
			continue;
		}

		///////////////////////////////
		//Check with parent map
		if(GetParentExt(sFileName) != _W(""))
		{	
			tWString sParentPath = cString::SetFileExtW(sPath, GetParentExt(sFileName));

			if(cPlatform::FileExists(sParentPath)==false)
			{
				Warning("%s does not exist!\n", cString::To8Char(sParentPath).c_str());
				glNumOfProblems++;
				continue;
			}
		
			if(cPlatform::FileModifiedDate(sPath) > cPlatform::FileModifiedDate(sParentPath))
			{
				glNumOfSkippedFiles++;
				continue;
			}
		}
        
		unsigned long lFileSize = cPlatform::GetFileSize(sPath);
		if(lFileSize <= 0) 
		{
			Warning("%s has zero size!\n", cString::To8Char(sPath).c_str());
			glNumOfProblems++;
			continue;
		}

		
		char *pBuffer = (char*) hplMalloc(lFileSize);
		cPlatform::CopyFileToBuffer(sPath,pBuffer, lFileSize);

		FILE *pFile = cPlatform::OpenFile(sPath, _W("wb"));
		if(pFile)
		{
			fwrite(pBuffer,1, lFileSize, pFile);
			fclose(pFile);

			printf("Updated '%s'\n", cString::To8Char(sPath).c_str());
			glNumOfFiles++;
		}
		else
		{	
			Warning("%s could not be opened!\n", cString::To8Char(sPath).c_str());
			glNumOfProblems++;
		}

		hplFree(pFile);
	}

	////////////////////////////////
	// Iterate folders
	tWStringList lstFolders;
	cPlatform::FindFoldersInDir(lstFolders, asDir, false);
	for(tWStringListIt it = lstFolders.begin(); it != lstFolders.end(); ++it)
	{
		//printf("\n");
		IterateFiles(cString::SetFilePathW(*it, asDir));
	}
}

//------------------------------------------------


int main(int argc, const char* argv[])
{
	SetLogFile(_W("CacheFileDateFix.log"));

	cEngineInitVars vars;
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, 0, &vars);
	
	IterateFiles(_W("./"));

	Log("Number of files fixed: %d\n", glNumOfFiles);
	Log("Number of files skipped: %d\n", glNumOfSkippedFiles);
	Log("Number of problems: %d\n", glNumOfProblems);
	
	DestroyHPLEngine(gpEngine);
	
	return 0;
}
int hplMain(const tString &asCommandline){ return -1;}

