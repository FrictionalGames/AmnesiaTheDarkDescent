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

#include "../Common/EdIndex.h"

#include "../Common/Editor.h"
#include "../Common/EdThumbnailBuilder.h"

#include <algorithm>

//-------------------------------------------------------------------

static tString gsIndexVersion = "3";

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// INDEX CLASS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectIndex::iEdObjectIndex(iEditor* apEditor, 
									   const tWString& asBaseFolder, 
									   const tWString& asIndexExt, 
									   const tString& asIndexTypeName, 
									   const tString& asEntryTypeName,
									   bool abCreateSubDirs)
{
	mpEditor = apEditor;
	msBaseFolderFullPath = cString::ReplaceCharToW(asBaseFolder, _W("\\"), _W("/"));
	msIndexExt = asIndexExt;
	msIndexTypeName = asIndexTypeName;
	msEntryTypeName = asEntryTypeName;

	mpRootDir = NULL;
	
	int lLastSlashPos = cString::GetLastStringPosW(msBaseFolderFullPath, _W("/"));
	if(lLastSlashPos==msBaseFolderFullPath.size()-1)
	{
		msBaseFolderFullPath = cString::SubW(msBaseFolderFullPath, 0, lLastSlashPos);

		lLastSlashPos = cString::GetLastStringPosW(msBaseFolderFullPath, _W("/"));
	}

	msBaseFolder = cString::SubW(msBaseFolderFullPath, lLastSlashPos+1);
	
	msFilename = mpEditor->GetTempDir() + msBaseFolder + _W(".") + msIndexExt;

	mbUpdated = false;
	mbRefreshThumbnails = false;

	mbCreateSubCategories = abCreateSubDirs;
}

//-------------------------------------------------------------------

iEdObjectIndex::~iEdObjectIndex()
{
	if(mpRootDir)
		hplDelete(mpRootDir);
}

//-------------------------------------------------------------------

void iEdObjectIndex::Create()
{
	if(mpRootDir)
		hplDelete(mpRootDir);

	mpRootDir = CreateDir();

	Refresh();
}

//-------------------------------------------------------------------

bool iEdObjectIndex::Refresh()
{
	cEdThumbnailBuilder* pBuilder = mpEditor->GetThumbnailBuilder();
	//mpEditor->ClearTempEntities();
	pBuilder->PreBuild();

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	if(pDoc->CreateFromFile(msFilename))
	{
		if(pDoc->GetAttributeString("Version")!=gsIndexVersion)
		{
			mbUpdated = true;
			if(mpRootDir)
				mpRootDir->CreateFromDir(NULL, _W(""), mbCreateSubCategories);
		}
		else
		{
			cXmlElement* pXmlRootDir = pDoc->GetFirstElement("RootDir");

			if(mpRootDir && pXmlRootDir)
				mpRootDir->Refresh(pXmlRootDir, mbCreateSubCategories);
		}
	}
	else
	{
		mbUpdated = true;
		if(mpRootDir)
			mpRootDir->CreateFromDir(NULL, _W(""), mbCreateSubCategories);
	}
		
	if(mbUpdated)
	{
		mbUpdated=false;
		Save(pDoc);
	}

	/////////////////////////
	// Reload thumbnail directory
	if(mbRefreshThumbnails)
	{
		mbRefreshThumbnails = false;
		mpEditor->GetEngine()->GetResources()->AddResourceDir(mpEditor->GetThumbnailDir(), false);
		
		pBuilder->CleanUp();
	}

	pRes->DestroyXmlDocument(pDoc);

	pBuilder->PostBuild();


	return true;
}

//-------------------------------------------------------------------

void iEdObjectIndex::AddFilesInDir(tWStringList& asFileList, const tWString& asFolder, const tWString& asLocalPath, bool abAddFilesInSubDirs)
{
	/////////////////////////////////////////////////////////////
	// Scan given dir for files using the filters 
	tWStringList lstFiles;
	for(int i=0;i<(int)mvFileFilters.size();++i)
	{
		tWString sFilter = mvFileFilters[i];
		cPlatform::FindFilesInDir(lstFiles, asFolder, mvFileFilters[i]); 
	}
	lstFiles.sort();

	/////////////////////////////////////////////////////////////
	// Add to file list
	tWStringListIt it = lstFiles.begin();
	for(;it!=lstFiles.end();++it)
	{
		tWString sFile = *it;
		asFileList.push_back(sFile);
	}
}

//-------------------------------------------------------------------

void iEdObjectIndex::Save(iXmlDocument* apDoc)
{
	apDoc->DestroyChildren();
	apDoc->SetValue(msIndexTypeName);
	apDoc->SetAttributeString("Version", gsIndexVersion);

	if(mpRootDir)
	{
		cXmlElement* pElement = apDoc->CreateChildElement("RootDir");

		mpRootDir->Save(pElement, mbCreateSubCategories);
	}

	apDoc->SaveToFile(msFilename);
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// INDEX DIR
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectIndexDir::iEdObjectIndexDir(iEdObjectIndex* apIndex, iEdObjectIndexDir* apParent)
{
	mpIndex = apIndex;
	mpParentDir = apParent;

	mbExpanded = true;
}

//----------------------------------------------------------

iEdObjectIndexDir::~iEdObjectIndexDir()
{
	Clear();
}

//----------------------------------------------------------

void iEdObjectIndexDir::Clear()
{
	STLMapDeleteAll(mmapEntries);
	STLMapDeleteAll(mmapSubDirs);
}

bool iEdObjectIndexDir::CreateFromDir(iEdObjectIndexDir* apParentDir, const tWString& asRelPath, bool abCreateSubDirs)
{
	if(cString::GetFirstStringPosW(asRelPath, _W("."))==0)
		return false;

	mpParentDir = apParentDir;
	Clear();	

	msDirName = asRelPath;

    tWStringList lstFiles;
	const tWStringVec& vFilters = mpIndex->GetFileFilters();

	tWString sDir = GetFullPath();
	mpIndex->GetEditor()->GetEngine()->GetResources()->AddResourceDir(sDir, false);

	mpIndex->AddFilesInDir(lstFiles, sDir, _W(""), false);

	tWStringListIt itFiles = lstFiles.begin();
	for(;itFiles!=lstFiles.end();++itFiles)
	{
		iEdObjectIndexEntry* pEntry = CreateEntry();
		if(pEntry->CreateFromFile(*itFiles))
			mmapEntries.insert(pair<tString, iEdObjectIndexEntry*>(pEntry->GetEntryName(), pEntry));
		else
			hplDelete(pEntry);
	}

	if(abCreateSubDirs)
	{
		tWStringList lstDirs;

		cPlatform::FindFoldersInDir(lstDirs, sDir, false, false);

		tWStringListIt itDirs = lstDirs.begin();
		for(;itDirs!=lstDirs.end();++itDirs)
		{
			tWString sRelPath = *itDirs;
			iEdObjectIndexDir* pDir = mpIndex->CreateDir();
			if(pDir->CreateFromDir(this, sRelPath, true))
				mmapSubDirs.insert(std::pair<tWString,iEdObjectIndexDir*>(sRelPath,pDir));
			else
				hplDelete(pDir);
		}
	}

	return true;
}

//-------------------------------------------------------------------

const tWString& iEdObjectIndexDir::GetRelPath()
{
	if(msRelPath==_W(""))
	{
		tWString sTemp = (mpParentDir?mpParentDir->GetRelPath():_W(""));

		msRelPath = (sTemp==_W("")?sTemp:sTemp+_W("/")) + msDirName;
	}

	return msRelPath;
}

//-------------------------------------------------------------------

const tWString& iEdObjectIndexDir::GetFullPath()
{
	if(msFullPath==_W(""))
		msFullPath = mpIndex->GetBaseFolderFullPath() + _W("/") + GetRelPath();

	return msFullPath;
}

//-------------------------------------------------------------------

bool iEdObjectIndexDir::Refresh(cXmlElement* apElement, bool abAddSubDirs)
{
	bool bMapEmpty = mmapEntries.empty();
	bool bUpdated = false;

	Clear();
	
	cResources* pRes = mpIndex->GetEditor()->GetEngine()->GetResources();
	tWStringList lstFiles;
	tWStringList lstDirs;

	msDirName = cString::To16Char(apElement->GetAttributeString("RelPath"));

	const tWString& sFullPath = GetFullPath();

	pRes->AddResourceDir(sFullPath, false);

	mpIndex->AddFilesInDir(lstFiles, sFullPath, _W(""), false);

	cXmlElement* pEntries = apElement->GetFirstElement("Entries");
	cXmlElement* pDirs = apElement->GetFirstElement("SubDirs");

	if(pEntries)
	{
		cXmlNodeListIterator itEntries = pEntries->GetChildIterator();
		while(itEntries.HasNext())
		{
			iEdObjectIndexEntry* pEntry = CreateEntry();
			cXmlElement* pXmlEntry = itEntries.Next()->ToElement();

			if(pEntry->CreateFromXmlElement(pXmlEntry))
			{
				tString sEntryName = pEntry->GetEntryName();
				bool bEntryIsUpdated = pEntry->IsUpdated();

				if(bEntryIsUpdated)
					bUpdated = true;

				if(bMapEmpty)
					mmapEntries[sEntryName] = pEntry;
				else
				{
					tIndexEntryMapIt itFoundEntry = mmapEntries.find(sEntryName);
					if(itFoundEntry != mmapEntries.end())
					{
						if(bEntryIsUpdated)
						{
							hplDelete(itFoundEntry->second);
							itFoundEntry->second = pEntry;
						}
						else
							hplDelete(pEntry);
					}
					else
						mmapEntries[sEntryName] = pEntry;
				}

				lstFiles.erase(find(lstFiles.begin(),lstFiles.end(), cString::To16Char(cString::GetFileName(pEntry->GetFileName()))));
			}
			else
			{
				hplDelete(pEntry);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	// Create the entries not listed in the XML but present in disk
	if(lstFiles.empty()==false)
	{
		bUpdated = true;
		tWStringListIt itFiles = lstFiles.begin();
		for(;itFiles!=lstFiles.end();++itFiles)
		{
			iEdObjectIndexEntry* pEntry = CreateEntry();
			if(pEntry->CreateFromFile(*itFiles))
			{
				mmapEntries[pEntry->GetEntryName()] = pEntry;
			}
			else
				hplDelete(pEntry);
		}
	}

	if(abAddSubDirs)
	{
		cPlatform::FindFoldersInDir(lstDirs, sFullPath, false, false);
		if(pDirs)
		{
			cXmlNodeListIterator itDirs = pDirs->GetChildIterator();
			while(itDirs.HasNext())
			{
				iEdObjectIndexDir* pDir = mpIndex->CreateDir(this);
				cXmlElement* pXmlDir = itDirs.Next()->ToElement();

				if(pDir->Refresh(pXmlDir, abAddSubDirs))
				{
					const tWString& sRelPath = pDir->GetRelPath();
					Log("sRelPath: %ls\n", sRelPath.c_str());
					mmapSubDirs.insert(std::pair<tWString,iEdObjectIndexDir*>(sRelPath, pDir));
					tWStringListIt it = find(lstDirs.begin(),lstDirs.end(), sRelPath);
					if (it == lstDirs.end()) {
						Warning("Error finding Directory %ls in list for path %ls\n",sRelPath.c_str(), sFullPath.c_str());
					} else {						
						lstDirs.erase(it);
					}
				}
				else
				{
					hplDelete(pDir);
				}
			}
		}
	

		if(lstDirs.empty()==false)
		{
			bUpdated = true;
			tWStringListIt itDirs = lstDirs.begin();
			for(;itDirs!=lstDirs.end();++itDirs)
			{
				iEdObjectIndexDir* pDir = mpIndex->CreateDir();
				if(pDir->CreateFromDir(this, *itDirs, false))
				{
					mmapSubDirs.insert(std::pair<tWString, iEdObjectIndexDir*>(*itDirs, pDir));
				}
				else
					hplDelete(pDir);
			}
		}
	}


	mpIndex->SetUpdated(bUpdated);

	

	return true;
}

//-------------------------------------------------------------------

iEdObjectIndexEntry* iEdObjectIndexDir::GetEntry(const tString& asIndex)
{
	tIndexEntryMapIt it = mmapEntries.find(asIndex);
	if(it!=mmapEntries.end())
		return it->second;
	
	return NULL;
}

//-------------------------------------------------------------------

void iEdObjectIndexDir::Save(cXmlElement* apIndexElement, bool abSaveSubDirs)
{
	apIndexElement->SetAttributeString("RelPath", cString::To8Char(msDirName));
	if(mmapEntries.empty()==false)
	{
		cXmlElement* pEntries = apIndexElement->CreateChildElement("Entries");
		tIndexEntryMapConstIt itEntries = mmapEntries.begin();
		for(;itEntries!=mmapEntries.end();++itEntries)
		{
			cXmlElement* pXmlEntry = pEntries->CreateChildElement();

			iEdObjectIndexEntry* pEntry = itEntries->second;
			pEntry->Save(pXmlEntry);
		}
	}

	if(abSaveSubDirs && mmapSubDirs.empty()==false)
	{
		cXmlElement* pDirs = apIndexElement->CreateChildElement("SubDirs");
		tIndexDirMapIt itSubDirs = mmapSubDirs.begin();
		for(;itSubDirs!=mmapSubDirs.end();++itSubDirs)
		{
			iEdObjectIndexDir* pDir = itSubDirs->second;

			cXmlElement* pXmlDir = pDirs->CreateChildElement("Dir");
			
			pDir->Save(pXmlDir, abSaveSubDirs);
		}
	}
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// INDEX ENTRY
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectIndexEntry::iEdObjectIndexEntry(iEdObjectIndexDir* apParentDir)
{
	mpParentDir = apParentDir;
	
	mbRemoved = false;
	mbUpdated = false;
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntry::CreateFromFile(const tWString& asFilename)
{
	msFileName = cString::To8Char(asFilename);
	if(CheckFileExists()==false)
		return false;

	BuildEntryName(msEntryName);
	msDateModified = cPlatform::FileModifiedDate(GetFileNameFullPath()).ToString();

	BuildThumbnail();

	return true;
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntry::CreateFromXmlElement(cXmlElement* apElement)
{
	msFileName = apElement->GetAttributeString("Filename");
	msEntryName = apElement->GetAttributeString("EntryName");
	msDateModified = apElement->GetAttributeString("DateModified");

	if(CheckFileExists()==false)
		return false;

	if(CheckNeedsUpdate())
	{
		mbUpdated = true;

		return CreateFromFile(cString::To16Char(msFileName));
	}

	BuildThumbnail();

	return true;
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntry::CheckFileExists()
{
	return cPlatform::FileExists(GetFileNameFullPath());
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntry::CheckNeedsUpdate()
{
	return (msDateModified != cPlatform::FileModifiedDate(GetFileNameFullPath()).ToString());
}

//-------------------------------------------------------------------

tWString iEdObjectIndexEntry::GetFileNameRelPath()
{
	tWString sRelPath = mpParentDir->GetRelPath();
	tWString sFilenameRelPath = cString::AddSlashAtEndW(sRelPath) + cString::To16Char(msFileName);

	return sFilenameRelPath;
}

//-------------------------------------------------------------------

tWString iEdObjectIndexEntry::GetFileNameFullPath()
{
	tWString sFilenameFullPath = cString::AddSlashAtEndW(mpParentDir->GetFullPath()) + cString::To16Char(msFileName);

	return sFilenameFullPath;
}

//-------------------------------------------------------------------

const tWString& iEdObjectIndexEntry::GetThumbnailFilename()
{
	if(msThumbnailFilename==_W(""))
	{
		cEdThumbnailBuilder* pTmbBuilder = mpParentDir->GetIndex()->GetEditor()->GetThumbnailBuilder();
		msThumbnailFilename = pTmbBuilder->GetThumbnailNameFromFileW(GetFileNameFullPath());
	}

	return msThumbnailFilename;
}

//-------------------------------------------------------------------

void iEdObjectIndexEntry::Save(cXmlElement* apElement)
{
	apElement->SetValue(mpParentDir->GetIndex()->GetEntryTypeName());

	apElement->SetAttributeString("Filename", msFileName);
	apElement->SetAttributeString("EntryName", msEntryName);
	apElement->SetAttributeString("DateModified", msDateModified);
}

//-------------------------------------------------------------------

void iEdObjectIndexEntry::BuildEntryName(tString& asEntryName)
{
	asEntryName = msFileName;
	int lDotPos = cString::GetLastStringPos(asEntryName, ".");
	asEntryName = cString::Sub(asEntryName,0,lDotPos);
	asEntryName = cString::GetFileName(asEntryName);
	
	/*
	int lBaseFolderLength = 0;
	iEdObjectIndex* pIndex = mpParentDir->GetIndex();

	if(cString::GetFirstStringPos(msFileName, cString::To8Char(pIndex->GetBaseFolder()))==0 )
		lBaseFolderLength = (int)pIndex->GetBaseFolder().size() + 1;

	int lLastDotPosition = cString::GetLastStringPos(msFileName, ".");
	asEntryName = cString::Sub(msFileName,lBaseFolderLength, lLastDotPosition-lBaseFolderLength);*/
}
