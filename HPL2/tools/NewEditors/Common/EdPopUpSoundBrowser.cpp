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

#include "../Common/EdPopUpSoundBrowser.h"
#include "../Common/EdWorld.h"

#include "../Common/Editor.h"

#include <algorithm>

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

cEdPopUpSoundBrowser::cEdPopUpSoundBrowser(iEditor* apEditor, 
										   const tWString& asStartDir, 
										   tWString& asFilename, 
										   void* apCallbackObject, tGuiCallbackFunc apCallback) : iEdPopUp(apEditor, _W("Sound Browser")), //,true,true,true, cVector2f(570,430)),
																									iFileBrowser(asFilename.empty()?asStartDir : cString::GetFilePathW(asFilename)),
																									msFilename(asFilename)
{
	mpCallbackObject = apCallbackObject;
	mpCallback = apCallback;
	mpTestSound = NULL;
	mpSoundEntity = NULL;

	AddCategory(_W("Sounds"), _W("*.snt"));
}

//----------------------------------------------------------------------------------------

cEdPopUpSoundBrowser::~cEdPopUpSoundBrowser()
{
	mpEditor->GetWorld()->GetEngWorld()->DestroySoundEntity(mpSoundEntity);
}

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	SetSize(cVector2f(570,430));

	cWidgetWindow* pWin = static_cast<cWidgetWindow*>(GetBG());
	pWin->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(Window_OnUpdate));

	cVector3f vPos = cVector3f(15,30,0.1f);
	
	mpCBCurrentDirectory = mpSet->CreateWidgetComboBox(vPos-cVector3f(1,0,0), cVector2f(200,25), _W(""), pWin);
	mpCBCurrentDirectory->SetCanEdit(false);
	mpCBCurrentDirectory->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(CurrentDirectory_OnSelectionChange));

	vPos.y += mpCBCurrentDirectory->GetSize().y + 5;

	// File ListBox        
	mpLBFiles = mpSet->CreateWidgetMultiPropertyListBox(vPos,cVector2f(540,300),pWin);

	mpLBFiles->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(FileList_OnClick)); 
	mpLBFiles->AddCallback(eGuiMessage_SelectionDoubleClick, this, kGuiCallback(FileList_OnDblClick)); 
	mpLBFiles->SetBackgroundZ(-0.01f);
	mpLBFiles->AddColumn("",0);
	mpLBFiles->SetColumnWidth(0,24);
	mpLBFiles->AddColumn("Name",1);
	mpLBFiles->SetColumnWidth(1,300);
	mpLBFiles->AddColumn("Size",2);
	mpLBFiles->SetColumnWidth(2,75);
	mpLBFiles->AddColumn("Date modified",3);
	mpLBFiles->SetColumnWidth(3,125);

	vPos.y += mpLBFiles->GetSize().y+10;

	mpBPlayStop = mpSet->CreateWidgetButton(vPos, cVector2f(22), _W(""), pWin);

	mpBPlayStop->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PlayStop_OnClick));
	vPos.x += mpBPlayStop->GetSize().x+2;
	mpTBCurrentFileName = mpSet->CreateWidgetTextBox(vPos, cVector2f(300,25), _W(""), pWin);

	mpTBCurrentFileName->SetForceCallBackOnEnter(true);
	mpTBCurrentFileName->SetCallbackOnLostFocus(false);
	mpTBCurrentFileName->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(Button_OnPressed));

	vPos.x = mpLBFiles->GetLocalPosition().x+mpLBFiles->GetSize().x-70;

	for(int i=1; i>=0; --i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(vPos,cVector2f(70,25), _W(""), pWin);

		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));

		vPos.x -= mvButtons[i]->GetSize().x+5;
	}
	mvButtons[0]->SetText(_W("Load"));
	mvButtons[1]->SetText(_W("Cancel"));
	
	cGuiSkin* pSkin = mpSet->GetSkin();
	mvGfxTypes[0] = pSkin->GetGfx(eGuiSkinGfx_FilePickerIconFolder);
	mvGfxTypes[1] = pSkin->GetGfx(eGuiSkinGfx_FilePickerIconSounds);

	tString vIconNames[] = { "gui_icon_play.tga", "gui_icon_stop.tga" };
	for(int i=0;i<2;++i)
		mvGfxButtons[i] = mpSet->GetGui()->CreateGfxImage(vIconNames[i], eGuiMaterial_Alpha);

	mpBPlayStop->SetImage(mvGfxButtons[0]);
}

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::OnNavigate()
{
	UpdateCurrentDirectory();
	mpTBCurrentFileName->SetText(_W(""));
	PopulateFileList();
}

//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);

	if(abX)
		InitBrowser();
}

//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::UpdateCurrentDirectory()
{
	mpCBCurrentDirectory->ClearItems();
	mpCBCurrentDirectory->AddItem(_W("<System Root>"));
	for(int i=0;i<(int)mvCurrentDirFullPath.size();++i)
	{
		mpCBCurrentDirectory->AddItem(mvCurrentDirFullPath[i]);
	}
	mpCBCurrentDirectory->SetSelectedItem(mpCBCurrentDirectory->GetItemNum()-1);

	mpEditor->GetEngine()->GetResources()->AddResourceDir(GetCurrentFullPath(), false, "*.snt");
}

//----------------------------------------------------------------------------------------

bool cEdPopUpSoundBrowser::Window_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpSoundEntity)
	{
		if(mpSoundEntity->IsStopped())
			mpBPlayStop->SetImage(mvGfxButtons[0]);
		else
			mpBPlayStop->SetImage(mvGfxButtons[1]);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, Window_OnUpdate);

//----------------------------------------------------------------------------------------

bool cEdPopUpSoundBrowser::CurrentDirectory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lNumItems = mpCBCurrentDirectory->GetItemNum();
	int lSelection = mpCBCurrentDirectory->GetSelectedItem();

	tWString sDir;
	
	////////////////////////////////////////
	// If selection is not last item, remove items from selection onwards, then navigate to selection
	if(lSelection != lNumItems-1)
	{
		sDir = mpCBCurrentDirectory->GetItemText(lSelection);

		for(int i=lSelection-1;i<lNumItems-1; ++i)
			mvCurrentDirFullPath.pop_back();

		NavigateTo(sDir);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, CurrentDirectory_OnSelectionChange);

//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::PopulateFileList()
{
	//////////////////////////////////////
	// Reset Items
	mpLBFiles->ClearItems();

	tWString sCurrentFullPath = GetCurrentFullPath();
	///////////////////////////////////////
	// If we are at the root, set up special folders
	if(sCurrentFullPath==_W(""))
	{
		for(int i=0;i<(int)mvSystemRootFolders.size();++i)
		{
			cWidgetItem* pItem = mpLBFiles->AddItem();

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
	GetFilesAndFoldersInCurrentPath(0, lstFilesAndFolders);
	tWStringListIt it = lstFilesAndFolders.begin();

	int lSelectedItem=-1;
	////////////////////////////////////////
	// Add them as items to the file list
	for(int i=0;it!=lstFilesAndFolders.end(); ++it, ++i)
	{
		tWString sFilename = *it;
		tWString sFilenameFullPath = sCurrentFullPath + sFilename;

		cWidgetItem* pItem = mpLBFiles->AddItem();
		if(sFilenameFullPath==msFilename)
			lSelectedItem = i;
        
		eFileBrowserFileType type = GetFileTypeByName(sFilename);
		cGuiGfxElement* pGfx = NULL;
		tWString sFileDate = cString::To16Char(cPlatform::FileModifiedDate(sFilenameFullPath).ToString());
		tWString sFileSize;
		if(type==eFileBrowserFileType_Folder)
		{
			pGfx = mvGfxTypes[0];
		}
		else
		{
			pGfx = mvGfxTypes[1];
			sFileSize = GetHumanReadableSize(cPlatform::GetFileSize(sFilenameFullPath));
		}

		pItem->AddProperty(pGfx);
		pItem->AddProperty(sFilename);
		pItem->AddProperty(sFileSize);
		pItem->AddProperty(sFileDate);
	}
	if(lSelectedItem!=-1)
		mpLBFiles->SetSelectedItem(lSelectedItem, true, true);
		
}

//----------------------------------------------------------------------------------------

bool cEdPopUpSoundBrowser::FileList_OnClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	cWorld* pWorld = mpEditor->GetWorld()->GetEngWorld();
	///////////////////////////////////////
	// Else, check if selected item is a folder, if not, add file name to current filename textbox
	int lSelection = mpLBFiles->GetSelectedItem();

	tWString sPath = _W("");
	if(lSelection != -1) 
	{
		cWidgetItem* pItem = (cWidgetItem*)mpLBFiles->GetItem(lSelection);
		
		if(pItem)
			// Property 1 : file name
			sPath = pItem->GetProperty(1)->GetText();
	}

	if(mpSoundEntity)
	{
		pWorld->DestroySoundEntity(mpSoundEntity);
		mpSoundEntity = NULL;
	}

	if(sPath!=_W(""))
	{
		if(cPlatform::FolderExists(GetCurrentFullPath()+sPath)==false)
		{
			cSoundManager* pMgr = mpEditor->GetEngine()->GetResources()->GetSoundManager();
			mpSoundEntity = pWorld->CreateSoundEntity("", cString::To8Char(sPath), false);
			mpSoundEntity->Stop();
			mpSoundEntity->SetForcePlayAsGUISound(true);
		}
		else
			sPath = _W("");

		mpTBCurrentFileName->SetText(sPath);
	}

	mpBPlayStop->SetEnabled(mpSoundEntity!=NULL);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, FileList_OnClick);

bool cEdPopUpSoundBrowser::FileList_OnDblClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	/////////////////////////////////////
	// Get path item which got the double click
	tWString sPath = mpLBFiles->GetItem(mpLBFiles->GetSelectedItem())->GetProperty(1)->GetText();

	// Try to navigate to path (if folder)
	if(NavigateTo(sPath)==false)
	{
		// Simulate Load/Save Button press
		Button_OnPressed(mvButtons[0], cGuiMessageData());
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, FileList_OnDblClick);

//----------------------------------------------------------------------------------------

bool cEdPopUpSoundBrowser::PlayStop_OnClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpSoundEntity==NULL)
		return true;

	if(mpSoundEntity->IsStopped())
	{
		mpSoundEntity->Play();
	}
	else
	{
		mpSoundEntity->Stop();
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, PlayStop_OnClick);

//----------------------------------------------------------------------------------------

bool cEdPopUpSoundBrowser::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mvButtons[0])
	{
		tWString sFilenameFullPath = GetCurrentFullPath() + mpTBCurrentFileName->GetText();
		bool bFileExists = cPlatform::FileExists(sFilenameFullPath);

		if(bFileExists)
			msFilename = sFilenameFullPath;

		Close(bFileExists);
	}
	else if(apWidget==mvButtons[1])
	{
		Close(false);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpSoundBrowser, Button_OnPressed);

//----------------------------------------------------------------------------------------

void cEdPopUpSoundBrowser::Close(bool abReturnFile)
{
	int lIntMessageData = (int)(abReturnFile);
	if(mpCallback && mpCallbackObject)
		mpCallback(mpCallbackObject, NULL, cGuiMessageData(lIntMessageData));

	SetActive(false);
}

//----------------------------------------------------------------------------------------

