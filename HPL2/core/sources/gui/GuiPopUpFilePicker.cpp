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

#include "gui/GuiPopUpFilePicker.h"

#include "gui/GuiTypes.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "math/Math.h"

#include "resources/Resources.h"

#include "graphics/FontData.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"

#include "gui/WidgetTextBox.h"
#include "gui/WidgetMultiPropertyListBox.h"
#include "gui/WidgetWindow.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetComboBox.h"

namespace hpl {

	/////////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	cGuiPopUpFilePicker::cGuiPopUpFilePicker(cGuiSet* apSet, cGuiSkin* apSkin,
											 eFilePickerType aType, 
											 const tWString &asStartPath, bool abShowHiddenFiles,
											 void *apCallbackObject, tGuiCallbackFunc apCallback, 
											 const tWString& asStartFilename) : iGuiPopUp(apSet,false,cVector2f(600,500)), 
																				iFileBrowser(asStartPath, abShowHiddenFiles)
	{
		mpSkin = apSkin;

		mPickerType = aType;

		mpCallbackObject = apCallbackObject;
		mpCallback = apCallback;

		msStartFilename = cString::GetFileNameW(asStartFilename);
		if(msStartFilename.empty())
			msStartFilename = cString::GetFileNameW(asStartPath);

		if(msStartFilename==asStartPath)
			msStartFilename.clear();

		mpSaveFileDest = NULL;
		mpLoadFileListDest = NULL;
	}

    //-------------------------------------------------------------------------------

	cGuiPopUpFilePicker::~cGuiPopUpFilePicker()
	{
		for(int i=0;i<3;++i) if(mvButtons[i]) mpSet->DestroyWidget(mvButtons[i]);
		if(mpCurrentDirectory) mpSet->DestroyWidget(mpCurrentDirectory);
		if(mpCurrentFileName) mpSet->DestroyWidget(mpCurrentFileName);
		if(mpFileList) mpSet->DestroyWidget(mpFileList);
		if(mpFilterList) mpSet->DestroyWidget(mpFilterList);
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	void cGuiPopUpFilePicker::SetSaveFileDest( tWString &asFileName )
	{
		mpSaveFileDest = &asFileName;

		mpCurrentFileName->SetText(cString::GetFileNameW(asFileName));
	}

	//-------------------------------------------------------------------------------

	void cGuiPopUpFilePicker::SetLoadFileListDest( tWStringVec &asFileList )
	{
		mpLoadFileListDest = &asFileList;
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
	{
		/////////////////////////////
		// Go Up one folder button
		if(apWidget == mvButtons[0])
		{
			NavigateUp();			
		}
		/////////////////////////////
		// Save/Load button
		else if(apWidget == mvButtons[1] ||
				apWidget == mpCurrentFileName)
		{
			if(mpCurrentFileName->GetText() == _W(""))
				mpSet->CreatePopUpMessageBox(	_W("Error"), 
												_W("File name must not be empty"), 
												_W("OK"), 
												_W(""),
												NULL,NULL);
			else
			{
				tWString sFilenameFullPath = GetCurrentFullPath() + mpCurrentFileName->GetText();
				bool bFileExists = cPlatform::FileExists(sFilenameFullPath);

				switch(mPickerType)
				{
				case eFilePickerType_Save:

					if(cString::GetFileExtW(sFilenameFullPath)!=cString::SubW(mvCategories[0]->mvFilters[0],2))
					{
						tWString sExt = cString::SubW(mvCategories[0]->mvFilters[0],1);
						mpCurrentFileName->SetText(mpCurrentFileName->GetText() + sExt);
						sFilenameFullPath += sExt;

						bFileExists = bFileExists || cPlatform::FileExists(sFilenameFullPath);
					}
					
					if(bFileExists)
					{
						mpSet->CreatePopUpMessageBox(	_W("Warning"), 
														_W("File already exists. Overwrite?"),
														_W("Yes"), _W("No"), 
														this, kGuiCallback(MessageBoxReturnCallback));
					}
					else
					{
						BuildSaveFileName();
						ClosePopUp(true);
					}

					break;
				case eFilePickerType_Load:
					if(bFileExists==false)
					{
						mpSet->CreatePopUpMessageBox(	_W("Error"), 
														_W("File does not exist. Please choose a valid file."), 
														_W("OK"), _W(""), 
														NULL, NULL);
					}
					else
					{
						BuildLoadFileList();
						ClosePopUp(true);
					}
					break;
				default:
					break;
				}
			}
		}
		/////////////////////////////
		// Cancel button
		else if(apWidget == mvButtons[2])
		{
			ClosePopUp(false);
		}
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, Button_Pressed);

	//-------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::MessageBoxReturnCallback( iWidget* apWidget, const cGuiMessageData& aData)
	{
		/////////////////////////
		// If button pressed is 0 (Yes)
		if(aData.mlVal == 0)
		{
			switch(mPickerType)
			{
			case eFilePickerType_Load:
				break;
			case eFilePickerType_Save:
				BuildSaveFileName();
				ClosePopUp(true);
				break;
			}
		}
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, MessageBoxReturnCallback);

	//-------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::Directory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
	{
		cWidgetComboBox* pBox = (cWidgetComboBox*)apWidget;
		int lNumItems = pBox->GetItemNum();
		int lSelection = pBox->GetSelectedItem();

		tWString sDir;
		
		////////////////////////////////////////
        // If the selection is a system item (e.g. begins with "<") handle it differently
        if(lSelection < (lNumItems - mvCurrentDirFullPath.size())) {
            mvCurrentDirFullPath.clear();
            NavigateTo(pBox->GetItemText(lSelection));
        }
        // If selection is not last item, remove items from selection onwards, then navigate to selection
        else if(lSelection != lNumItems-1)
		{
			sDir = pBox->GetItemText(lSelection);

			for(int i=lNumItems; i>lSelection; --i)
			{
				mvCurrentDirFullPath.pop_back();
			}

			NavigateTo(sDir);
		}
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, Directory_OnSelectionChange);

	//-------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::FileList_OnSelectionDblClick(iWidget* apWidget, const cGuiMessageData& aData)
	{
		/////////////////////////////////////
		// Get path item which got the double click
		tWString sPath = mpFileList->GetItem(mpFileList->GetSelectedItem())->GetProperty(1)->GetText();

		// Try to navigate to path (if folder)
		if(NavigateTo(sPath)==false)
		{
			// Simulate Load/Save Button press
			Button_Pressed(mvButtons[1], cGuiMessageData());
		}

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, FileList_OnSelectionDblClick);

	//-------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::FileList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
	{
		int lFolder = mpCurrentDirectory->GetSelectedItem();

		///////////////////////////////////////
		// If checking special folders, do nothing
		if(lFolder == 0) return true;

		///////////////////////////////////////
		// Else, check if selected item is a folder, if not, add file name to current filename textbox
		int lSelection = mpFileList->GetSelectedItem();

		tWString sPath = _W("");

		if(lSelection != -1) 
		{
			cWidgetItem* pItem = (cWidgetItem*)mpFileList->GetItem(lSelection);
			
			if(pItem)
				// Property 1 : file name
				sPath = pItem->GetProperty(1)->GetText();
		}

		if(sPath != _W("") && cPlatform::FolderExists(GetCurrentFullPath() + sPath)==false)
			mpCurrentFileName->SetText( sPath );

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, FileList_OnSelectionChange);

	//-------------------------------------------------------------------------

	bool cGuiPopUpFilePicker::FilterList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
	{
		////////////////////////////
		// If filter has changed, repopulate
		PopulateFileList();
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpFilePicker, FilterList_OnSelectionChange);

	//-------------------------------------------------------------------------

	void cGuiPopUpFilePicker::BuildSaveFileName()
	{
        *mpSaveFileDest = GetCurrentFullPath() + mpCurrentFileName->GetText();
	}

	//-------------------------------------------------------------------------

	void cGuiPopUpFilePicker::BuildLoadFileList()
	{
		// TODO : Build load file list
		mpLoadFileListDest->clear();

		mpLoadFileListDest->push_back(GetCurrentFullPath() + mpCurrentFileName->GetText());
	}

	//-------------------------------------------------------------------------

    /**	This method clears the current filter combobox,
	 *	and then repopulates it with the new filter values:
	 *	it adds an item per category, and writes the filters on the item text
	 */
	void cGuiPopUpFilePicker::OnAddFilter()
	{
		////////////////////////////////////////////////
		// This method clears the current filter combobox,
		// and then repopulates it with the new filter values:
		// it adds an item per category, and writes the filters on the item text
		mpFilterList->ClearItems();
		
		for(int i=0;i<(int)mvCategories.size();++i)
		{
			cFileBrowserCategory* pCat = mvCategories[i];

			tWString sFilterText = pCat->msName + _W(" (");

			for(int j=0;j <(int)pCat->mvFilters.size();++j)
			{
				if(j!=0) sFilterText += _W(", ");

				sFilterText += pCat->mvFilters[j];
			}

			sFilterText += _W(")");

			mpFilterList->AddItem(sFilterText);
		}

		mpFilterList->SetSelectedItem(0);
	}

	//-------------------------------------------------------------------------

	void cGuiPopUpFilePicker::OnNavigate()
	{
		mpCurrentDirectory->ClearItems();
		mpCurrentDirectory->AddItem(_W("<System Root>"));
        if (iFileBrowser::msPersonalDir.length()) {
            mpCurrentDirectory->AddItem(_W("<Personal Folder>"));
        }
        if (iFileBrowser::msGameDir.length()) {
            mpCurrentDirectory->AddItem(_W("<Game Folder>"));
        }

		for(int i=0;i<(int)mvCurrentDirFullPath.size();++i)
		{
			mpCurrentDirectory->AddItem(mvCurrentDirFullPath[i]);
		}
		mpCurrentDirectory->SetText(GetCurrentFullPath());

		PopulateFileList();
	}

	//-------------------------------------------------------------------------

	/** This method populates the File list according to the current directory and
	 *	selected filters. If current directory is empty, populate the list with
	 *	special folders (Windows only)
	 */
	void cGuiPopUpFilePicker::PopulateFileList()
	{
		int lSelectedCategoryIndex = mpFilterList->GetSelectedItem();

		if(lSelectedCategoryIndex<0) return;

		//////////////////////////////////////
		// Reset Items
		mpFileList->ClearItems();

		tWString sCurrentFullPath = GetCurrentFullPath();
		///////////////////////////////////////
		// If we are at the root, set up special folders
		if(sCurrentFullPath==_W(""))
		{
			for(int i=0;i<(int)mvSystemRootFolders.size();++i)
			{
				cWidgetItem* pItem = mpFileList->AddItem();

				pItem->AddProperty(NULL);
				pItem->AddProperty(mvSystemRootFolders[i]);
				pItem->AddProperty(_W(""));
				pItem->AddProperty(_W(""));
			}
			return;
		}

		///////////////////////////////////////
		// Else, scan current directory
		tWStringList lstFilesAndFolders;
		GetFilesAndFoldersInCurrentPath(lSelectedCategoryIndex, lstFilesAndFolders);
		tWStringListIt it = lstFilesAndFolders.begin();

		////////////////////////////////////////
		// Add them as items to the file list
		for(;it!=lstFilesAndFolders.end(); ++it)
		{
			tWString sFilename = *it;

			cWidgetItem* pItem = mpFileList->AddItem();
            
			cGuiGfxElement* pGfx = mvGfxFileTypeIcons[GetFileTypeByName(sFilename)];
			tWString sFileDate = cString::To16Char(cPlatform::FileModifiedDate(sCurrentFullPath + sFilename).ToString());
			tWString sFileSize = GetHumanReadableSize(cPlatform::GetFileSize(sCurrentFullPath + sFilename));

			pItem->AddProperty(pGfx);
			pItem->AddProperty(sFilename);
			pItem->AddProperty(sFileSize);
			pItem->AddProperty(sFileDate);
		}
		
	}


	//-------------------------------------------------------------------------
	
	/** Builds the window and sets up stuff
	 *
	 */
	void cGuiPopUpFilePicker::Init()
	{
		tWString sCaption;

		switch(mPickerType)
		{
		case eFilePickerType_Load:
			sCaption = _W("Load");
			break;
		case eFilePickerType_Save:
			sCaption = _W("Save");
			break;
		default:
			break;
		}
		sCaption += _W(" file");

		////////////////////////////////
		// Set up graphics
		mpGfxUpButton = mpSkin->GetGfx(eGuiSkinGfx_FilePickerUpButton);

		for(int i=0; i<6; ++i)
		{
			mvGfxFileTypeIcons[i] = mpSkin->GetGfx((eGuiSkinGfx) (i + eGuiSkinGfx_FilePickerUpButton+1));
		}


		/////////////////////////////////
		// Init widgets
		
		// Main window
		mpWindow->SetText(sCaption);
		
		// Current directory TextBox
		mpCurrentDirectory = mpSet->CreateWidgetComboBox(cVector3f(30,60,0.1f),
														cVector2f(400, mpWindow->GetDefaultFontSize().y+2),
														_W(""), 
														mpWindow);
		mpCurrentDirectory->SetCanEdit(false);
		mpCurrentDirectory->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(Directory_OnSelectionChange));

		// Buttons
		for(int i=0; i<3; ++i)
		{
			mvButtons[i] = mpSet->CreateWidgetButton(	0,
														0,
														_W(""), 
														mpWindow);

			mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		}

        if(mpGfxUpButton) mvButtons[0]->SetImage( mpGfxUpButton, false);
		mvButtons[0]->SetPosition(cVector3f(435,60,0.1f));
		mvButtons[0]->SetSize(23);
		
		mvButtons[1]->SetText(sCaption);
		mvButtons[1]->SetPosition(cVector3f(500,425, 0.5f));
		mvButtons[1]->SetSize(cVector2f(70,20));
		
		mvButtons[2]->SetText(_W("Cancel"));
		mvButtons[2]->SetPosition(cVector3f(500,450, 0.5f));
		mvButtons[2]->SetSize(cVector2f(70,20));

		// Current file name TextBox
		mpCurrentFileName = mpSet->CreateWidgetTextBox(	cVector3f(30,425,0.5f),
														cVector2f(400,mpWindow->GetDefaultFontSize().y+2),
														msStartFilename, 
														mpWindow);
		mpCurrentFileName->SetForceCallBackOnEnter(true);
		mpCurrentFileName->SetCallbackOnLostFocus(false);
		mpCurrentFileName->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(Button_Pressed));

		mpSet->SetFocusedWidget(mpCurrentFileName);
		mpCurrentFileName->SetSelectedText();
		
		// Filters ComboBox
		mpFilterList = mpSet->CreateWidgetComboBox(	cVector3f(30, 450, 0.5f), 
													cVector2f(300, mpWindow->GetDefaultFontSize().y+2),
													_W(""), 
													mpWindow);
		mpFilterList->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(FilterList_OnSelectionChange));

		// File ListBox        
		mpFileList = mpSet->CreateWidgetMultiPropertyListBox(cVector3f(30,90,1),
															 cVector2f(540,300),
															 mpWindow);
		mpFileList->AddColumn("",0);
		mpFileList->SetColumnWidth(0,24);
		mpFileList->AddColumn("Name",1);
		mpFileList->SetColumnWidth(1,300);
		mpFileList->AddColumn("Size",2, eFontAlign_Right);
		mpFileList->SetColumnWidth(2,75);
		mpFileList->AddColumn("Date modified",3);
		mpFileList->SetColumnWidth(3,125);

		mpFileList->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(FileList_OnSelectionChange));
		mpFileList->AddCallback(eGuiMessage_SelectionDoubleClick, this, kGuiCallback(FileList_OnSelectionDblClick));

		InitBrowser();
	}

	//-------------------------------------------------------------------------

	void cGuiPopUpFilePicker::ClosePopUp(bool abPickedFile)
	{
		if(abPickedFile)
			mpSet->GetResources()->AddResourceDir(GetCurrentFullPath(), false);

		int lIntMessageData = (int)abPickedFile;
		RunCallback(mpCallbackObject, mpCallback, NULL, cGuiMessageData(lIntMessageData), true);

		SelfDestruct();
	}
	//-------------------------------------------------------------------------

};




