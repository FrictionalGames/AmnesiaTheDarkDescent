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

#include "gui/WidgetBaseClasses.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"

#include "math/Math.h"

#include "gui/Widget.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"


namespace hpl {

	static tWString gsNullText(_W(""));
    
    tWString iFileBrowser::msGameDir = _W("");
    tWString iFileBrowser::msPersonalDir = _W("");

	tWStringVec iFileBrowser::mvSizeStrings = tWStringVec();
	
	//////////////////////////////////////////////////////////////////////////
	// ITEM PROPERTY
	//////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------

	cWidgetItemProperty::cWidgetItemProperty(const tWString& asText)
	{
		mType = eItemPropertyType_String;
		msText = asText;
		mpGfx = NULL;
	}

	//------------------------------------------------------------------------

	cWidgetItemProperty::cWidgetItemProperty(const cGuiGfxElement* apGfx)
	{
		mType = eItemPropertyType_Image;
		mpGfx = (cGuiGfxElement*)apGfx;
	}

	//------------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// ITEM CLASS
	//////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------

	cWidgetItem::cWidgetItem(iWidgetItemContainer* apCont)
	{
		mpCont = apCont;
		mbSelectable = true;
		mbSelected = false;

		mpUserData = NULL;
	}

	//------------------------------------------------------------------------

	cWidgetItem::~cWidgetItem()
	{
		STLDeleteAll(mvProperties);
	}

	//------------------------------------------------------------------------

	cWidgetItemProperty* cWidgetItem::GetProperty(int alIdx) const
	{
		if(alIdx>=0 && alIdx<(int)mvProperties.size())
		{
			return mvProperties[alIdx];
		}
		return NULL;
	}

	//------------------------------------------------------------------------

	void cWidgetItem::AddProperty(const tWString& asText)
	{
		cWidgetItemProperty* pProp = hplNew(cWidgetItemProperty,(asText));

		mvProperties.push_back(pProp);
	}

	//------------------------------------------------------------------------

	void cWidgetItem::AddProperty(const cGuiGfxElement* apGfx)
	{
		cWidgetItemProperty* pProp = hplNew(cWidgetItemProperty, (apGfx));

		mvProperties.push_back(pProp);
	}

	//------------------------------------------------------------------------

	int cWidgetItem::GetPropertyNum()
	{
		return (int)mvProperties.size();
	}

	//------------------------------------------------------------------------

	void cWidgetItem::SetSelected(bool abX)
	{
		if(mbSelected==abX)
			return;

		mbSelected = abX;
		if(mbSelected)
			mpCont->AddItemToSelection(this);
		else
			mpCont->RemoveItemFromSelection(this);
	}

	void cWidgetItem::SetText(const tWString& asText)
	{
		msText = asText;
	}

	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ITEM CONTAINER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iWidgetItemContainer::~iWidgetItemContainer()
	{
		STLDeleteAll(mvItems);
	}

	//-----------------------------------------------------------------------

	cWidgetItem* iWidgetItemContainer::AddItem(const tString& asText)
	{
		return AddItem(cString::To16Char(asText));
	}

	//-----------------------------------------------------------------------

	cWidgetItem* iWidgetItemContainer::AddItem(const tWString &asText)
	{
		cWidgetItem* pItem = hplNew(cWidgetItem, (this));

		pItem->SetIndex(GetItemNum());
		pItem->SetText(asText);

		mvItems.push_back(pItem);

		UpdateProperties();

		return pItem;
	}

	//-----------------------------------------------------------------------

	cWidgetItem* iWidgetItemContainer::AddItem()
	{
		return AddItem(_W(""));
	}

	//-----------------------------------------------------------------------

	void iWidgetItemContainer::RemoveItem(int alX)
	{
		int lCount =0;
		tWidgetItemVecIt it = mvItems.begin();
		for(; it != mvItems.end(); ++it,++lCount)
		{
			if(lCount == alX) 
			{
				hplDelete(*it);
				mvItems.erase(it);
				break;
			}
		}

		UpdateProperties();
	}

	//-----------------------------------------------------------------------

	void iWidgetItemContainer::RemoveItem(const tWString &asItem)
	{
		tWidgetItemVecIt it = mvItems.begin();
		for(; it != mvItems.end(); ++it)
		{
			if((*it)->GetText() == asItem) 
			{
				hplDelete(*it);
				mvItems.erase(it);
				break;
			}
		}

		UpdateProperties();
	}

	//-----------------------------------------------------------------------

	void iWidgetItemContainer::ClearItems()
	{
		STLDeleteAll(mvItems);

		UpdateProperties();
	}
	
	//-----------------------------------------------------------------------

	cWidgetItem* iWidgetItemContainer::GetItem(int alX) const
	{
		if(alX>=0 && alX<(int)mvItems.size())
			return mvItems[alX];
		else
			return NULL;
	}
	const tWString& iWidgetItemContainer::GetItemText(int alX) const
	{
		if(alX>=0 && alX<(int)mvItems.size())
			return mvItems[alX]->GetText();
		else
			return gsNullText;
	}
	void iWidgetItemContainer::SetItemText(int alX, const tWString& asText)
	{
		if(alX>=0 && alX<(int)mvItems.size())
			mvItems[alX]->SetText(asText);
	}
	int iWidgetItemContainer::GetItemNum() const
	{
		return (int)mvItems.size();
	}

	bool iWidgetItemContainer::HasItem(const tWString &asItem)
	{
		tWidgetItemVecIt it = mvItems.begin();
		for(; it != mvItems.end(); ++it)
		{
			if((*it)->GetText() == asItem) return true;
		}
		return false;
	}

	bool iWidgetItemContainer::HasItem(const tString &asItem)
	{
		return HasItem(cString::To16Char(asItem));
	}

	//-----------------------------------------------------------------------

	
	//-----------------------------------------------------------------------

	iFileBrowser::iFileBrowser(const tWString& asStartPath, bool abAddHiddenFiles)
	{
		if(mvSizeStrings.empty())
		{
			mvSizeStrings.push_back(_W("bytes"));
			mvSizeStrings.push_back(_W("KB"));
			mvSizeStrings.push_back(_W("MB"));
			mvSizeStrings.push_back(_W("GB"));
		}
		////////////////////////
		// Build up special folder list
		mvSystemRootFolders.push_back(_W("<Personal Folder>"));

		unsigned long lAvailableDrives = cPlatform::GetSystemAvailableDrives();
		tWString sDrive = _W("<Disk Drive> A:");

		while(lAvailableDrives)
		{
			if(lAvailableDrives & 1)
				mvSystemRootFolders.push_back(sDrive);
			sDrive[13]+=1;
			lAvailableDrives >>= 1;
		}

		msStartPath = cString::GetFilePathW(asStartPath);

		mbAddHiddenFiles = abAddHiddenFiles;

		////////////////////////////////////////////////////////////////////
		// TEMP Initialization
		mvFileTypeExtensions.resize(eFileBrowserFileType_Other);
		mvFileTypeExtensions[eFileBrowserFileType_Graphics].push_back(_W("bmp"));
		mvFileTypeExtensions[eFileBrowserFileType_Graphics].push_back(_W("tga"));
		mvFileTypeExtensions[eFileBrowserFileType_Graphics].push_back(_W("jpg"));

		mvFileTypeExtensions[eFileBrowserFileType_Models].push_back(_W("dae"));
		mvFileTypeExtensions[eFileBrowserFileType_Models].push_back(_W("ent"));

		mvFileTypeExtensions[eFileBrowserFileType_Sounds].push_back(_W("ogg"));
		mvFileTypeExtensions[eFileBrowserFileType_Sounds].push_back(_W("wav"));

		mvFileTypeExtensions[eFileBrowserFileType_Text].push_back(_W("txt"));
		mvFileTypeExtensions[eFileBrowserFileType_Text].push_back(_W("hps"));
		mvFileTypeExtensions[eFileBrowserFileType_Text].push_back(_W("log"));
	}

	//-----------------------------------------------------------------------

	iFileBrowser::~iFileBrowser()
	{
		STLDeleteAll(mvCategories);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iFileBrowser::InitBrowser()
	{
		if(NavigateTo(msStartPath)==false)
            NavigateTo(_W("<System Root>"));
	}

	//-----------------------------------------------------------------------

	int iFileBrowser::AddCategory(const tWString &asCategory, const tWString &asFilter)
	{
		cFileBrowserCategory* pCat = hplNew(cFileBrowserCategory, ());
		
		pCat->msName = asCategory;

		mvCategories.push_back(pCat);

		int lIndex = (int)mvCategories.size()-1;
		if(asFilter!=_W(""))
			AddFilter(lIndex,asFilter);

		return lIndex;
	}

	//-----------------------------------------------------------------------

	void iFileBrowser::AddFilter(const int alCategoryId, const tWString &asFilter)
	{
		if(alCategoryId>=0 && alCategoryId<(int)mvCategories.size())
		{
			mvCategories[alCategoryId]->mvFilters.push_back(asFilter);
			
			OnAddFilter();
		}
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iFileBrowser::GetFilesAndFoldersInCurrentPath(int alCategoryIndex, tWStringList& alstDestination)
	{
		if(alCategoryIndex<0 || 
		   alCategoryIndex>=(int)mvCategories.size()) return;

		alstDestination.clear();

		cFileBrowserCategory* pCat = mvCategories[alCategoryIndex];

		tWStringList lstFolders;
		tWStringList lstFiles;

		tWString sCurrentFullPath = GetCurrentFullPath();

		////////////////////////////////////////
		// Get folders and files in current path
		cPlatform::FindFoldersInDir(lstFolders, sCurrentFullPath , mbAddHiddenFiles, true);

		for(int i=0; i<(int)pCat->mvFilters.size();++i)
		{
			cPlatform::FindFilesInDir(lstFiles, sCurrentFullPath , pCat->mvFilters[i], mbAddHiddenFiles);
		}
		lstFiles.sort();

		alstDestination.insert(alstDestination.end(),lstFolders.begin(), lstFolders.end());
		alstDestination.insert(alstDestination.end(),lstFiles.begin(), lstFiles.end());
		
	}

	//-------------------------------------------------------------------------


	/** This method returns the file type
	 *
	 * \param &asFilename 
	 * \return 
	 */
	eFileBrowserFileType iFileBrowser::GetFileTypeByName(const tWString &asFilename)
	{
		if(cPlatform::FolderExists( GetCurrentFullPath() + asFilename))
			return eFileBrowserFileType_Folder;

		////////////////////////////////////////////////////////////////////

		tWString sExt = cString::GetFileExtW(asFilename);
		sExt = cString::ToLowerCaseW(sExt);

		for(int i=0;i<(int)mvFileTypeExtensions.size();++i)
		{
			tWStringVec& vFileExtensions = mvFileTypeExtensions[i];

			for(int j=0;j<(int)vFileExtensions.size();++j)
			{
				if(sExt == vFileExtensions[j])
					return (eFileBrowserFileType)i;
			}
		}

		return eFileBrowserFileType_Other;		
	}

	//-------------------------------------------------------------------------

	

	//-------------------------------------------------------------------------

	/** Go to the specified path, be it a special folder, a child dir from current dir, 
	 *	a dir relative to the working directory, or a valid absolute path.
	 * \param asPath Path to navigate to
	 * \return true if success (valid path provided)
	 */
	bool iFileBrowser::NavigateTo(const tWString& asPath)
	{
		if(asPath==_W("<System Root>"))
		{
            mvCurrentDirFullPath.clear();
		}
        else if(asPath==_W("<Game Folder>"))
        {
            SetPathFromString(iFileBrowser::msGameDir);
        }
		else if(asPath==_W("<Personal Folder>"))
		{
			SetPathFromString(cPlatform::GetSystemSpecialPath(eSystemPath_Personal) + iFileBrowser::msPersonalDir);
		}
		else if(cString::SubW(asPath,0,12) == _W("<Disk Drive>"))
		{
			return NavigateTo(cString::SubW(asPath,13));
		}
		else if(asPath==_W(".."))
		{
            if (!mvCurrentDirFullPath.empty()) {
                mvCurrentDirFullPath.pop_back();
            }
		}
		else if(asPath==_W("."))
		{
		}
		else
		{
			if(mvCurrentDirFullPath.empty()==false && cPlatform::FolderExists( GetCurrentFullPath() + asPath ))
			{
				mvCurrentDirFullPath.push_back(asPath);
			}
#ifndef WIN32
            // Unix does the whole "/" thing as the system root.. (No drive letters!!)
            else if (mvCurrentDirFullPath.empty() && asPath.length() > 0 && asPath[0] != _W('/')
                     && cPlatform::FolderExists( GetCurrentFullPath() + asPath )) {
                mvCurrentDirFullPath.push_back(asPath);
            }
#endif
			else if(cPlatform::FolderExists(cString::AddSlashAtEndW(cPlatform::GetWorkingDir()) + asPath) )
			{
				SetPathFromString(cString::AddSlashAtEndW(cPlatform::GetWorkingDir()) + asPath);
			}
			else if(cPlatform::FolderExists(asPath))
			{
				SetPathFromString(asPath);
			}
			else
				return false;
		}
		OnNavigate();

		return true;
	}
    
	//-------------------------------------------------------------------------

	/**This method makes the current directory go up one level
	 * Top == special folders 
	 * \return true on success
	 */
	bool iFileBrowser::NavigateUp()
	{
		return NavigateTo(_W(".."));
	}

	//-------------------------------------------------------------------------

	/** Sets current path based on a string
	 *
	 * \param &asPath 
	 */
	void iFileBrowser::SetPathFromString(const tWString &asPath)
	{
		tWString sPath = cString::ReplaceCharToW(asPath, _W("\\"), _W("/"));

		/////////////////////////////////////////////////
		// Check if path is valid, and if so, store the
		// directories that conform the full path separately (without slashes)
		if(cPlatform::FolderExists(sPath) || sPath==_W(""))
		{
			mvCurrentDirFullPath.clear();

			tWStringVec vInputPath;
			tWString t = tWString(_W("/"));
			cString::GetStringWVec(sPath, vInputPath, &t);

			for(int i=0;i<(int)vInputPath.size();++i)
			{
				const tWString& sPathPiece = vInputPath[i];

				if(sPathPiece==_W(".") || sPathPiece==_W(""))
					;
				else if(sPathPiece==_W(".."))
					mvCurrentDirFullPath.pop_back();
				else
					mvCurrentDirFullPath.push_back(sPathPiece);
			}
		}
	}

	//-------------------------------------------------------------------------

	/** Returns file size in an eye friendly format
	 *
	 * \param alSize 
	 * \return 
	 */
	tWString iFileBrowser::GetHumanReadableSize(unsigned long alSize)
	{
		int i=0;
		for(; i<4; ++i)
		{
			if(alSize > 1024)
				alSize >>= 10;
			else
				break;
		}

		return cString::ToStringW(alSize) + _W(" ") + mvSizeStrings[i];
	}

	//-------------------------------------------------------------------------

	/** GetCurrentFullPath : returns current full path
	 *
	 * \return current full path with '/' separators
	 */
	tWString iFileBrowser::GetCurrentFullPath()
	{
		tWString sPath;
#ifndef WIN32
		sPath = _W("/");
#endif

		for(int i=0; i<(int)mvCurrentDirFullPath.size();++i)
			sPath += cString::AddSlashAtEndW(mvCurrentDirFullPath[i]);
		
		return sPath;
	}

};


