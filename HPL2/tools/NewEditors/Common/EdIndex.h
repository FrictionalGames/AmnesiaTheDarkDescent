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

#ifndef ED_INDEX_H
#define ED_INDEX_H

#include "../Common/StdAfx.h"

#include "../Common/Editor.h"

class iEditor;
class iEdObjectIndex;
class iEdObjectIndexDir;
class iEdObjectIndexEntry;

//----------------------------------------------------------

typedef std::map<tWString, iEdObjectIndexDir*> tIndexDirMap;
typedef tIndexDirMap::iterator tIndexDirMapIt;

typedef std::vector<iEdObjectIndexDir*> tIndexDirVec;
typedef tIndexDirVec::iterator tIndexDirVecIt;

typedef std::map<tString, iEdObjectIndexEntry*> tIndexEntryMap;
typedef tIndexEntryMap::iterator tIndexEntryMapIt;
typedef tIndexEntryMap::const_iterator tIndexEntryMapConstIt;

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// BASE INDEX CLASS
////////////////////////////////////////////////////////////

//----------------------------------------------------------

class iEdObjectIndex
{
public:
	iEdObjectIndex(iEditor* apEditor, 
					   const tWString& asBaseFolder, 
					   const tWString& asIndexExt, 
					   const tString& asIndexTypeName, 
					   const tString& asEntryTypeName,
					   bool abCreateSubCategories);
	virtual ~iEdObjectIndex();

	iEditor* GetEditor() { return mpEditor; }

	void AddFileFilter(const tWString& asFilter) { mvFileFilters.push_back(asFilter); }
	const tWStringVec& GetFileFilters() { return mvFileFilters; }

	virtual void Save(iXmlDocument* apDoc);

	virtual void Create();
	virtual bool Refresh();

	void AddFilesInDir(tWStringList& asFileList, const tWString& asFolder, const tWString& sLocalPath, bool abAddFilesInSubDirs);

	virtual bool CheckValidFilename(const tString& asFilename) { return true; }

	virtual iEdObjectIndexDir* CreateDir(iEdObjectIndexDir* apParent=NULL)=0;
	
	const tWString& GetBaseFolder() { return msBaseFolder; }
	const tWString& GetBaseFolderFullPath() { return msBaseFolderFullPath; }

	const tString& GetEntryTypeName() { return msEntryTypeName; }

	void SetUpdated(bool abX) { mbUpdated = true; }
	void SetRefreshThumbnails(bool abX) { mbRefreshThumbnails=abX; }

	iEdObjectIndexDir* GetRootDir() { return mpRootDir; }

protected:
	///////////////////////////////////
	// Data
	iEditor* mpEditor;

	tString msIndexTypeName;
	tString msEntryTypeName;
	tString msVersion;

	tWString msBaseFolder;
	tWString msBaseFolderFullPath;
	tWString msIndexExt;
	tWStringVec mvFileFilters;

	tWString msFilename;

	bool mbCreateSubCategories;

	bool mbUpdated;
	bool mbRefreshThumbnails;

	iEdObjectIndexDir* mpRootDir;
};

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// BASE INDEX DIR
////////////////////////////////////////////////////////////

//----------------------------------------------------------

class iEdObjectIndexDir
{
public:
	iEdObjectIndexDir(iEdObjectIndex* apIndex, iEdObjectIndexDir* apParent);
	virtual ~iEdObjectIndexDir();

	void Clear();

	iEdObjectIndex* GetIndex() { return mpIndex; }
	iEdObjectIndexDir* GetParentDir() { return mpParentDir; }
	const tIndexDirMap& GetSubDirs() { return mmapSubDirs; }

	iEdObjectIndexEntry* GetEntry(int alIdx);
	iEdObjectIndexEntry* GetEntry(const tString& asIndex);
	int GetEntryNum() { return (int)mmapEntries.size(); }
	tIndexEntryMap& GetEntries() { return mmapEntries; }

	const tWString& GetDirName() { return msDirName; }
	const tWString& GetRelPath();
	const tWString& GetFullPath();

	bool Refresh(cXmlElement* apElement, bool abAddSubDirs);
	
	virtual bool CreateFromDir(iEdObjectIndexDir* apParentDir, const tWString& asRelPath, bool abCreateSubCategories);

	virtual iEdObjectIndexEntry* CreateEntry()=0;

	virtual void Save(cXmlElement* apIndexElement, bool abSaveSubDirs);

	void SetExpanded(bool abX) { mbExpanded = abX; }
	bool IsExpanded() { return mbExpanded; }

protected:
	tWString msDirName;
	tWString msRelPath;
	tWString msFullPath;

	tString msName;
	bool mbExpanded;

	iEdObjectIndex* mpIndex;
	iEdObjectIndexDir* mpParentDir;
	tIndexDirMap mmapSubDirs;
	tIndexEntryMap mmapEntries;
};


//----------------------------------------------------------


//----------------------------------------------------------

//----------------------------------------------------------

//----------------------------------------------------------

////////////////////////////////////////////////////////////
// BASE INDEX ENTRY
////////////////////////////////////////////////////////////

//----------------------------------------------------------

class iEdObjectIndexEntry
{
public:
	iEdObjectIndexEntry(iEdObjectIndexDir* apDir);
	virtual ~iEdObjectIndexEntry(){}

	virtual bool CreateFromFile(const tWString& asFilename);
	virtual bool CreateFromXmlElement(cXmlElement* apElement);

	bool CheckFileExists();
	bool CheckNeedsUpdate();

	virtual void Save(cXmlElement* apElement);

	const tString& GetEntryName() { return msEntryName; }
	virtual const tString& GetFileName() { return msFileName; }
	const tString& GetDateModified() { return msDateModified; }

	tWString GetFileNameRelPath();
	tWString GetFileNameFullPath();
	
	const tWString& GetThumbnailFilename();

	bool IsUpdated() { return mbUpdated; }

	iEdObjectIndexDir* GetDir() { return mpParentDir; }

protected:
	virtual void BuildEntryName(tString& asEntryName);
	virtual void BuildThumbnail()=0;

	iEdObjectIndexDir* mpParentDir;
	tString msEntryName;
	tString msFileName;
	tString msDateModified;

	tWString msRelPath;
	tWString msFullPath;

	tWString msThumbnailFilename;

	bool mbRemoved;
	bool mbUpdated;
};


#endif // ED_INDEX_H
