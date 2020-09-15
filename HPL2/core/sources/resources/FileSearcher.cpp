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

#include "resources/FileSearcher.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "resources/LowLevelResources.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFileSearcherEntry::cFileSearcherEntry(const tWString& asPath)
	{
		msPath = asPath;
        
		tWString sSepp = _W("/\\");
		cString::GetStringVecW(msPath,mvPathDirs,&sSepp);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFileSearcher::cFileSearcher()
	{
		msNull = _W("");
	}

	//-----------------------------------------------------------------------

	cFileSearcher::~cFileSearcher()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cFileSearcher::AddDirectory(const tWString& asSearchPath, const tString &asMask, bool abAddSubDirectories)
	{
		//Make the path with only "/" and lower case.
		tWString sPath = cString::ReplaceCharToW(asSearchPath,_W("\\"),_W("/"));

		///////////////////////////////
		//Add all files in directory
		tWStringList lstFileNames;

		cPlatform::FindFilesInDir(lstFileNames,sPath, cString::To16Char(asMask));
			
		for(tWStringListIt it = lstFileNames.begin();it!=lstFileNames.end();it++)
		{
			tWString& sFile = *it;
			tString sLowFile = cString::ToLowerCase(cString::To8Char(sFile));
			tWString sFilePath = cString::ReplaceCharToW( cPlatform::GetFullFilePath( cString::SetFilePathW(sFile,sPath)), _W("\\"),_W("/"));;
			
			//Check if file and path already exist
			tFilePathMapIt pathIt = m_mapFiles.find(sLowFile);
			if(pathIt != m_mapFiles.end() && pathIt->second.msPath == sFilePath)
			{
				continue;
			}

			//Add file
			//Log("Adding lowercase file: '%s' with path: '%s'\n 8bitHash: %u 16bitHash %u\n", sLowFile.c_str(), cString::To8Char(sFilePath).c_str(),
			//	cString::GetHash(cString::To8Char(sFilePath)), cString::GetHashW(sFilePath));
			m_mapFiles.insert(tFilePathMap::value_type(sLowFile, cFileSearcherEntry(sFilePath) ));
		}
		
		//////////////////////////////////
		//Search sub directories if set.
		if(abAddSubDirectories)
		{
			tWStringList lstDirNames;
			cPlatform::FindFoldersInDir(lstDirNames,sPath,false);
			
			for(tWStringListIt it = lstDirNames.begin();it!=lstDirNames.end();it++)
			{
				tWString sNewPath = cString::SetFilePathW(*it, sPath);

				AddDirectory(sNewPath,asMask,true);
			}
		}
	}

	//-----------------------------------------------------------------------

	void cFileSearcher::ClearDirectories()
	{
		m_mapFiles.clear();
		m_setLoadedDirs.clear();
	}

	//-----------------------------------------------------------------------

	const tWString& cFileSearcher::GetFilePath(const tString& asFileNameAndPath, int *apEqualCount)
	{
		tString sFile = cString::GetFileName(asFileNameAndPath);
		tString sLowName = cString::ToLowerCase(sFile);

		//////////////////////
		//Get the iterator to path
		tFilePathMapIt it = m_mapFiles.find(sLowName);
		if(it == m_mapFiles.end())
		{
			if(apEqualCount) *apEqualCount = 0;
			return msNull;
		}
		
		//////////////////////
		//Count the number of files with same name
		//if 1, just return it.
		size_t lCount = m_mapFiles.count(sLowName);
		if(lCount==1 && apEqualCount==NULL)
		{
			return it->second.msPath;
		}

		/////////////////////////////
		//Compare paths
		tWString sWantedPath = cString::To16Char(cString::GetFilePath(asFileNameAndPath));
		if(sWantedPath == _W("")) return it->second.msPath;

		tWStringVec vWantedDirs;
		tWString sSepp =_W("/\\");
		
		int lBestEqualCount = 0;
        tFilePathMapIt bestEqualIt = it;
        
		cString::GetStringVecW(sWantedPath, vWantedDirs,&sSepp);

		//Iterate according to count and compare
		for(size_t itcount=0; itcount<lCount; ++itcount, ++it)
		{
			///////////////////////////////
			//Compare the wanted path with current, seeing how many directories are in common

			//Start with the wanted path dir
			int lEqualCount1 =0;
			int j = (int)it->second.mvPathDirs.size()-1;
            for(int i= (int)vWantedDirs.size()-1; (i>=0 && j>=0); --j)
			{
				//if equal, increase equal count and go to next wanted dir
				if(vWantedDirs[i] == it->second.mvPathDirs[j])
				{
					lEqualCount1++;
					--i;
				}
			}

			//Start with the available path dir
			int lEqualCount2 =0;
			j = (int)vWantedDirs.size()-1;
			for(int i= (int)it->second.mvPathDirs.size()-1; (i>=0 && j>=0); --j)
			{
				//if equal, increase equal count and go to next wanted dir
				if(it->second.mvPathDirs[i] == vWantedDirs[j])
				{
					lEqualCount2++;
					--i;
				}
			}

			int lMaxCount = lEqualCount1 > lEqualCount2 ? lEqualCount1 : lEqualCount2;

			/////////////////////
			//If a better equal count was found, use that.
			if(lMaxCount > lBestEqualCount)
			{
				lBestEqualCount = lMaxCount;
                bestEqualIt = it;				
			}
		}

		if(apEqualCount) *apEqualCount = lBestEqualCount;

		//Return best fit
		return bestEqualIt->second.msPath;
	}

	//-----------------------------------------------------------------------

}
