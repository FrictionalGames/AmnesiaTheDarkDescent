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

#include "../Common/EdPopUpTextureBrowser.h"
//#include "EditorWindowLoaderStatus.h"

#include "../Common/EdThumbnailBuilder.h"

#include <algorithm>


//----------------------------------------------------------------------------------------

cEditorObjectIndexEntryTexture::cEditorObjectIndexEntryTexture(cEditorObjectIndexTextures* apIndex) : iEdObjectIndexEntry(apIndex->GetRootDir())
{

}

//----------------------------------------------------------------------------------------

bool cEditorObjectIndexEntryTexture::CreateFromFile(const tWString& asFilename)
{
	if(iEdObjectIndexEntry::CreateFromFile(asFilename)==false)
		return false;

	mbMaterial = cString::GetFileExtW(asFilename)==_W("mat");

	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	tWString sPath = pIndex->GetBaseFolderFullPath();

	// Create list entry for each texture
	cResources* pRes = pIndex->GetEditor()->GetEngine()->GetResources();
	tString sFile = GetTextureFileFullPath();

	cBitmap* pBmp = pRes->GetBitmapLoaderHandler()->LoadBitmap(cString::To16Char(sFile),0);
	if(pBmp==NULL)
		return false;

	mvSize = pBmp->GetSize();
	mType = GetTextureTypeFromBitmap(pBmp);

	hplDelete(pBmp);

	BuildEntryName(msFileName);

	return true;
}

//----------------------------------------------------------------------------------------

bool cEditorObjectIndexEntryTexture::CreateFromXmlElement(cXmlElement* apElement)
{
	if(iEdObjectIndexEntry::CreateFromXmlElement(apElement)==false)
		return false;

	mType = GetTextureTypeFromTypeString(apElement->GetAttributeString("Type"));
	cVector3f vTemp = apElement->GetAttributeVector3f("Size");
	for(int i=0;i<3;++i)
		mvSize.v[i] = (int)vTemp.v[i];

	return true;
}

//----------------------------------------------------------------------------------------

bool cEditorObjectIndexEntryTexture::IsMaterial()
{
	//if(mbUpdated)
		mbMaterial = cString::GetFileExt(msFileName)=="mat";

	return mbMaterial;
}

//----------------------------------------------------------------------------------------

void cEditorObjectIndexEntryTexture::Save(cXmlElement* apElement)
{
	iEdObjectIndexEntry::Save(apElement);

	apElement->SetAttributeString("Type", cString::To8Char(GetTextureTypeString()));
	cVector3f vTemp;
	for(int i=0;i<3;++i)
		vTemp.v[i] = (float)mvSize.v[i];
	apElement->SetAttributeVector3f("Size", vTemp);
}

//----------------------------------------------------------------------------------------

tWString cEditorObjectIndexEntryTexture::GetTextureTypeString()
{
	tWString sType;
	switch(mType)
	{
	case eEdTexture_1D:
		sType = _W("1D texture");
		break;
	case eEdTexture_CubeMap:
		sType = _W("CubeMap");
		break;
	default:
		sType = _W("2D texture");
		break;
	}

	return sType;
}

//----------------------------------------------------------------------------------------

void cEditorObjectIndexEntryTexture::BuildEntryName(tString& asEntryName)
{
	msEntryName = cString::GetFileName(asEntryName);
}

//----------------------------------------------------------------------------------------

void cEditorObjectIndexEntryTexture::BuildThumbnail()
{
	bool bIsUpdated = IsUpdated();
	
	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditor* pEditor = pIndex->GetEditor();
	cEdThumbnailBuilder* pTmbBuilder = pEditor->GetThumbnailBuilder();
	cResources* pRes = pEditor->GetEngine()->GetResources();
	////////////////////////////////////////////////////////////////
	// Create thumbnail if it doesn't exist or the object was updated
	tWString sFilename = cString::ReplaceCharToW(cString::To16Char(GetFileName()), _W("/"), _W("_"));
	//tString sFilename = GetTextureFile();
	tWString sFilenameFullPath = cString::To16Char(GetTextureFileFullPath());
	tWString sThumbnailFilename = pTmbBuilder->GetThumbnailNameFromFileW(sFilenameFullPath);

	if(bIsUpdated || cPlatform::FileExists(sThumbnailFilename)==false)
	{
		//pEditor->ShowLoadingWindow(_W("Loading"), _W("Creating Thumbnail"));
		pEditor->GetThumbnailBuilder()->BuildThumbnailFromImage(sFilenameFullPath, _W(""));

		if(bIsUpdated==false)
			pIndex->SetRefreshThumbnails(true);
	}	
}

//----------------------------------------------------------------------------------------

eEdTexture cEditorObjectIndexEntryTexture::GetTextureTypeFromBitmap(cBitmap* apBmp)
{
	eEdTexture type;
	if(apBmp->GetNumOfImages()==6)
		type = eEdTexture_CubeMap;
	else
	{
		if(apBmp->GetHeight()==1)
			type = eEdTexture_1D;
		else
			type = eEdTexture_2D;
	}

	return type;
}

eEdTexture cEditorObjectIndexEntryTexture::GetTextureTypeFromTypeString(const tString& asType)
{
	if(asType=="1D texture")
		return eEdTexture_1D;
	if(asType=="CubeMap")
		return eEdTexture_CubeMap;

	return eEdTexture_2D;
}

//----------------------------------------------------------------------------------------

tString cEditorObjectIndexEntryTexture::GetTextureFile()
{
	tString sFile;
	////////////////////////////////////////////////////////////////////
	// Check if file is a material, and get diffuse source from file
	if(IsMaterial())
		sFile = GetDiffuseFromMatFile(msFileName);
	else
		sFile = msFileName;

	return sFile;
}

//----------------------------------------------------------------------------------------

tString cEditorObjectIndexEntryTexture::GetTextureFileFullPath()
{
	tString sFile;
	cFileSearcher* pSearcher = mpParentDir->GetIndex()->GetEditor()->GetEngine()->GetResources()->GetFileSearcher();
	////////////////////////////////////////////////////////////////////
	// Check if file is a material, and get diffuse source from file
	if(IsMaterial())
	{
		tWString sWFile = pSearcher->GetFilePath(cString::GetFileName(GetDiffuseFromMatFile(msFileName)));
		sFile = cString::To8Char(sWFile);
	}
	else
	{
		sFile = cString::To8Char(mpParentDir->GetFullPath()) + msFileName;
	}

	return sFile;
}

//----------------------------------------------------------------------------------------

tString cEditorObjectIndexEntryTexture::GetDiffuseFromMatFile(const tString& asFileName)
{
	tString sTexFile;
	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	cResources* pRes = pIndex->GetEditor()->GetEngine()->GetResources();
	cBitmapLoaderHandler* pHandler = pRes->GetBitmapLoaderHandler();

	iXmlDocument* pTempMatDoc = pRes->LoadXmlDocument(msFileName);
	if(pTempMatDoc)
	{
		cXmlElement* pXmlUnits = pTempMatDoc->GetFirstElement("TextureUnits");
		cXmlNodeListIterator itUnits = pXmlUnits->GetChildIterator();
		while(itUnits.HasNext())
		{
			cXmlElement* pUnit = itUnits.Next()->ToElement();
			sTexFile = pUnit->GetAttributeString("File");
			if(cString::GetFileExt(sTexFile)=="")
			{
				tStringVec* pvSupportedExts = pHandler->GetSupportedTypes();
				for(int i=0; i<(int)pvSupportedExts->size(); ++i)
				{
					tString sFileExt = cString::SetFileExt(sTexFile, (*pvSupportedExts)[i]);
					if(pRes->GetFileSearcher()->GetFilePath(sFileExt)!=_W(""))
					{
						sTexFile = sFileExt;
					}
				}
			}
			break;
		}
	}
	pRes->DestroyXmlDocument(pTempMatDoc);

	return sTexFile;
}

//----------------------------------------------------------------------------------------

cEditorObjectIndexDirTextures::cEditorObjectIndexDirTextures(cEditorObjectIndexTextures* apIndex) : iEdObjectIndexDir(apIndex, NULL)
{
}

//----------------------------------------------------------------------------------------

iEdObjectIndexEntry* cEditorObjectIndexDirTextures::CreateEntry()
{
	return hplNew(cEditorObjectIndexEntryTexture,((cEditorObjectIndexTextures*)mpIndex));
}

//----------------------------------------------------------------------------------------

cEditorObjectIndexTextures::cEditorObjectIndexTextures(iEditor* apEditor, const tWString& asBaseDir) : iEdObjectIndex(apEditor, 
																														asBaseDir, 
																														_W("tls"), 
																														"Textures", 
																														"Texture",
																														false)
{
	cBitmapLoaderHandler* pHandler = mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler();
	tStringVec* pFilters = pHandler->GetSupportedTypes();
	tWString sWildcard = _W("*.");
	for(int i=0;i<(int)pFilters->size();++i)
		AddFileFilter(sWildcard + cString::To16Char((*pFilters)[i]));

	AddFileFilter(_W("*.mat"));
}


//----------------------------------------------------------------------------------------

iEdObjectIndexDir* cEditorObjectIndexTextures::CreateDir(iEdObjectIndexDir* apParent)
{
	return hplNew(cEditorObjectIndexDirTextures,(this));
}

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// TEXTURE ICON - CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

cGuiSet* cTextureBrowserIcon::mpSet = NULL;

//----------------------------------------------------------------------------------------

cTextureBrowserIcon::cTextureBrowserIcon(cEdPopUpTextureBrowser* apBrowser,iWidget* apWidget, 
										 cEditorObjectIndexEntryTexture* apEntry)
{
	if(mpSet==NULL)	mpSet = apBrowser->GetGuiSet();
	mpBrowser = apBrowser;
	mpEntry = apEntry;

	iEditor* pEditor = mpBrowser->GetEditor();
	cResources* pRes = pEditor->GetEngine()->GetResources();
	cGraphics* pGfx = pEditor->GetEngine()->GetGraphics();
	
	
	int lWidth = mpEntry->GetTextureSize().x;
	int lHeight = mpEntry->GetTextureSize().y;

	///////////////////////////
	// Background
    mpIconBG = mpSet->CreateWidgetFrame(cVector3f(10,10,40), cVector2f(130,175), false, apWidget);
	mpIconBG->SetClipActive(false);
	mpIconBG->AddCallback(eGuiMessage_MouseUp, this, kGuiCallback(Frame_OnMouseUp));
	mpIconBG->AddCallback(eGuiMessage_MouseDoubleClick, this, kGuiCallback(Frame_OnDoubleClick));
	mpIconBG->SetBackgroundZ(0);
	mpIconBG->SetBackGroundColor(cColor(1,1));
	
	///////////////////////////
	// File name label
	float fFrameMiddle = mpIconBG->GetSize().x*0.5f;
	mpLabelName = mpSet->CreateWidgetLabel(cVector3f(fFrameMiddle, 130, 0.1f), -1, _W(""), mpIconBG);
	mpLabelName->SetDefaultFontSize(11.5f);
	mpLabelName->SetTextAlign(eFontAlign_Center);

	tWString sFullFilename = cString::To16Char(mpEntry->GetEntryName());
	//Log("\tShowing texture %ls in browser\n", sFullFilename.c_str());
	tWString sShortenedFilename;
	if(mpLabelName->GetDefaultFontType()->GetLength(cVector2f(11.5f),sFullFilename.c_str()) > 125.0f)
		sShortenedFilename = cString::SubW(sFullFilename,0, 10) + _W("...") + cString::GetFileExtW(sFullFilename);
	else
		sShortenedFilename = sFullFilename;
	mpLabelName->SetText(sShortenedFilename);

	//////////////////////////
	// Size label
	mpLabelSize = mpSet->CreateWidgetLabel(cVector3f(fFrameMiddle,145,0.1f), -1, _W(""), mpIconBG);
	mpLabelSize->SetDefaultFontSize(10);
	mpLabelSize->SetTextAlign(eFontAlign_Center);
	mpLabelSize->SetText(cString::ToStringW(lWidth) + _W("x") + cString::ToStringW(lHeight));

	/////////////////////////
	// Type label
	mpLabelType = mpSet->CreateWidgetLabel(cVector3f(fFrameMiddle,160,0.1f), -1, _W(""), mpIconBG);
	mpLabelType->SetDefaultFontSize(10);
	mpLabelType->SetTextAlign(eFontAlign_Center);
	tWString sLabel = mpEntry->GetTextureTypeString();
	mpLabelType->SetText(sLabel);


	//////////////////////////////////////////
	// Texture image
	float fScale = 1;

	if(lWidth == 1)
		lWidth = 128;
	if(lHeight == 1)
		lHeight = 128;

	if(lWidth > lHeight)
	{
		if(lWidth > 128)
			fScale = 128.0f/lWidth;
	}
	else
	{
		if(lHeight > 128)
			fScale = 128.0f/lHeight;
	}

	mpTexture = mpSet->CreateWidgetImage("", 
										 cVector3f(65,65,0.1f) - cVector3f(lWidth*fScale,lHeight*fScale,0)*0.5f, 
										 cVector2f(lWidth*fScale, lHeight*fScale),
										 eGuiMaterial_Diffuse, 
										 false, 
										 mpIconBG);

	cEdThumbnailBuilder* pThbBuilder = pEditor->GetThumbnailBuilder();
	tWString sThumbFile = pThbBuilder->GetThumbnailNameFromFileW(cString::To16Char(apEntry->GetTextureFileFullPath()));
	iTexture* pTex = pGfx->CreateTexture("", eTextureType_2D, eTextureUsage_Normal);
	cBitmap* pBmp = pRes->GetBitmapLoaderHandler()->LoadBitmap(sThumbFile, 0);

	if(pTex && pTex->CreateFromBitmap(pBmp))
	{
		cGuiGfxElement* pGfxElem = mpSet->GetGui()->CreateGfxTexture(pTex, false, eGuiMaterial_Diffuse);
		mpTexture->SetImage(pGfxElem);
	}

	hplDelete(pBmp);

	mpTexture->AddCallback(eGuiMessage_MouseUp, this, kGuiCallback(Frame_OnMouseUp));
	mpTexture->AddCallback(eGuiMessage_MouseDoubleClick, this, kGuiCallback(Frame_OnDoubleClick));

	mpTexture->SetToolTipEnabled(true);
	mpTexture->SetToolTip(sFullFilename);
	mpLabelName->SetToolTipEnabled(true);
	mpLabelName->SetToolTip(sFullFilename);
	mpIconBG->SetToolTipEnabled(true);
	mpIconBG->SetToolTip(sFullFilename);
}

//----------------------------------------------------------------------------------------

cTextureBrowserIcon::~cTextureBrowserIcon()
{
	if(mpSet->IsDestroyingSet()==false)
	{
		if(mpTexture)
		{
			mpBrowser->GetEditor()->GetEngine()->GetGraphics()->DestroyTexture(mpTexture->GetImage()->GetTexture(0));
			mpSet->DestroyWidget(mpTexture);
		}
		if(mpLabelSize) mpSet->DestroyWidget(mpLabelSize);
		if(mpLabelType) mpSet->DestroyWidget(mpLabelType);
		if(mpLabelName) mpSet->DestroyWidget(mpLabelName);
		if(mpIconBG) mpSet->DestroyWidget(mpIconBG);
	}
}

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// TEXTURE ICON - PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

void cTextureBrowserIcon::SetPosition(const cVector3f& avPos)
{
	mvPos = avPos;
	if(mpIconBG)
		mpIconBG->SetPosition(avPos);
}

//----------------------------------------------------------------------------------------

void cTextureBrowserIcon::SetSliderPosition(const cVector3f& avPos)
{
	if(mpIconBG)
		mpIconBG->SetPosition(mvPos - avPos);
}

//----------------------------------------------------------------------------------------

void cTextureBrowserIcon::SetSelected(bool abX)
{
	mpIconBG->SetDrawBackground(abX);
}

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// TEXTURE ICON - PROTECTED METHODS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

bool cTextureBrowserIcon::Frame_OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
		mpBrowser->SetSelectedIcon(this);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cTextureBrowserIcon,Frame_OnMouseUp);

//----------------------------------------------------------------------------------------

bool cTextureBrowserIcon::Frame_OnDoubleClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		mpBrowser->Close(true);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cTextureBrowserIcon,Frame_OnDoubleClick);

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// TEXTURE BROWSER
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

cEdPopUpTextureBrowser::cEdPopUpTextureBrowser(iEditor* apEditor,
											   bool abMaterialBrowser,
											   eEdTexture aType,
											   const tWString& asStartDir,
											   tWString& asTextureFilename, 
											   void* apCallbackObject, tGuiCallbackFunc apCallback,
											   const tWStringList& alstFilters) : iEdPopUp(apEditor, _W("Texture Browser")), //true,true, cVector2f(800,550)),
																				  iFileBrowser(asTextureFilename.empty()?asStartDir:cString::GetFilePathW(asTextureFilename)),
																							msTextureFilename(asTextureFilename)
{
	mpSelectedIcon = NULL;

	mpCallbackObject = apCallbackObject;
	mpCallback = apCallback;
	mlstFilters = alstFilters;

	mbMaterialBrowser = abMaterialBrowser;
	mType=eEdTexture_LastEnum;
	SetType(aType);

	SetDestroyOnSetInactive(true);
}

//----------------------------------------------------------------------------------------

cEdPopUpTextureBrowser::~cEdPopUpTextureBrowser()
{
	STLDeleteAll(mvIcons);
	STLMapDeleteAll(mmapTextureIndices);
}

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::SetSelectedIcon(cTextureBrowserIcon* apIcon)
{
	if(mpSelectedIcon==apIcon)
		return;

	tWString sFile;

	if(mpSelectedIcon)
		mpSelectedIcon->SetSelected(false);
	mpSelectedIcon = apIcon;
	if(mpSelectedIcon)
	{
		sFile = mpSelectedIcon->GetEntry()->GetFileNameFullPath();

		mpSelectedIcon->SetSelected(true);
	}
	else
	{
		sFile = _W("");
	}

	msTextureFilename = sFile;
	mpLabelTextureFilename->SetText(cString::GetFileNameW(sFile));
}

//----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------

bool cEdPopUpTextureBrowser::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bReturnFile = (apWidget==mvButtons[0]);

	Close(bReturnFile);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpTextureBrowser,Button_OnPressed);

//----------------------------------------------------------------------------------------

bool cEdPopUpTextureBrowser::DirList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpTextureBrowser,DirList_OnSelectionChange);

//----------------------------------------------------------------------------------------

bool cEdPopUpTextureBrowser::Frame_OnClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal & eGuiMouseButton_Left)
		SetSelectedIcon(NULL);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpTextureBrowser,Frame_OnClick);

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	SetSize(cVector2f(800,550));
	
	mpComboBoxCurrentDirectory = mpSet->CreateWidgetComboBox(cVector3f(5,30,10), cVector2f(200,25), _W(""), GetBG());
	mpComboBoxCurrentDirectory->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(CurrentDirectory_OnSelectionChange));

	mpListDirectories = mpSet->CreateWidgetListBox(cVector3f(10,60,10),cVector2f(150,425), GetBG());
	mpListDirectories->AddCallback(eGuiMessage_SelectionDoubleClick, this, kGuiCallback(DirectoryList_OnSelectionDblClick));

	mpFrameTextures = mpSet->CreateWidgetFrame(cVector3f(175,60,1),cVector2f(600,425), true, GetBG(), false, true);
	mpFrameTextures->SetBackgroundZ(0);
	mpFrameTextures->SetBackGroundColor(cColor(0.4f,1));
	mpFrameTextures->SetDrawBackground(true);
	mpFrameTextures->AddCallback(eGuiMessage_MouseUp,this,kGuiCallback(Frame_OnClick));

	for(int i=0;i<2;++i)
	{
		mvButtons[i] = mpSet->CreateWidgetButton(0,cVector2f(70,25), _W(""), GetBG());
		mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));
	}
	mvButtons[0]->SetPosition(cVector3f(650,490,10));
	mvButtons[0]->SetText(_W("OK"));
	mvButtons[1]->SetPosition(cVector3f(725,490,10));
	mvButtons[1]->SetText(_W("Cancel"));

	mpLabelTextureFilename = mpSet->CreateWidgetTextBox(cVector3f(175,490,0.1f), cVector2f(375,25), _W(""), GetBG());
}

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::PopulateDirList()
{
	mpListDirectories->ClearItems();

	tWString sCurrentPath = GetCurrentFullPath();
	if(sCurrentPath==_W(""))
	{
		tWString sCurrentFullPath = GetCurrentFullPath();
		///////////////////////////////////////
		// If we are at the root, set up special folders
		if(sCurrentFullPath==_W(""))
		{
			for(int i=0;i<(int)mvSystemRootFolders.size();++i)
				mpListDirectories->AddItem(mvSystemRootFolders[i]);
		}
		return;
	}

	tWStringList lstDirectories;
	cPlatform::FindFoldersInDir(lstDirectories, sCurrentPath, false, true);

	tWStringListIt it = lstDirectories.begin();
	for(;it!=lstDirectories.end();++it)
	{
		tWString sDirName = *it;

		mpListDirectories->AddItem(sDirName);
	}
}

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::PopulateTextureList()
{
	//mpEditor->ShowLoadingWindow(_W("Loading"), _W("Loading textures"));

	iEdObjectIndex* pIndex = CreateIndex(GetCurrentFullPath());

	//tWString sIndexFilename = mpEditor->GetTempDir() + pIndex->GetRootDir()->GetRelPath() + _W(".tls");
	tWString sThumbnailsDir = mpEditor->GetThumbnailDir();
	iXmlDocument *pDoc = mpEditor->GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument();

	//////////////////////////////////////////
	// Clear current icons and selection
	STLDeleteAll(mvIcons);
	mpSelectedIcon = NULL;
    
	//////////////////////////////
	// Check if index exists
	//bool bContentFilePresent = pDoc->CreateFromFile(sIndexFilename);

	//if(bContentFilePresent==false)
	//{
		//BuildContentFile(sIndexFilename);
	//	pDoc->CreateFromFile(sIndexFilename);
	//}

	cVector3f vPos = cVector3f(10,10,0.1f);
	cVector2f vSelectedIconPos = 0;
	const tIndexEntryMap& mapEntries = pIndex->GetRootDir()->GetEntries();
	tIndexEntryMapConstIt itEntries = mapEntries.begin();
	for(;itEntries!=mapEntries.end();++itEntries)
	{
		cEditorObjectIndexEntryTexture* pEntry = static_cast<cEditorObjectIndexEntryTexture*>(itEntries->second);
		if(pEntry->IsMaterial()!=mbMaterialBrowser)
			continue;

		if(mType!=eEdTexture_LastEnum && pEntry->GetTextureType()!=mType)
			continue;

		/*
		bool bValid = false;
		if(mlstFilters.empty())
			bValid = true;
		else
		{
			tWStringListIt itFilters = mlstFilters.begin();
			for(;itFilters!=mlstFilters.end();++itFilters)
			{
				if(cString::ToLowerCaseW(cString::GetFileExtW(cString::To16Char(pEntry->GetFileName()))) == *itFilters)
				{
					bValid = true;
					break;
				}
			}
		}

		if(bValid==false)
			continue;
			*/

		cTextureBrowserIcon* pIcon = hplNew(cTextureBrowserIcon,(this, mpFrameTextures, (cEditorObjectIndexEntryTexture*)pEntry));
		pIcon->SetPosition(vPos);
		mvIcons.push_back(pIcon);
		if(pIcon->GetEntry()->GetFileNameFullPath()==msTextureFilename)
		{
			vSelectedIconPos = cVector2f(vPos.x,vPos.y);
			SetSelectedIcon(pIcon);
		}

		vPos.x += 180;
		if((vPos.x + 175) > 600)
		{
			vPos.x = 10;
			vPos.y += 180;
		}
	}

	mpFrameTextures->ScrollToPosition(vSelectedIconPos);
}

//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------

bool cEdPopUpTextureBrowser::CurrentDirectory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lNumItems = mpComboBoxCurrentDirectory->GetItemNum();
	int lSelection = mpComboBoxCurrentDirectory->GetSelectedItem();

	tWString sDir;
	
	////////////////////////////////////////
	// If selection is not last item, remove items from selection onwards, then navigate to selection
	if(lSelection != lNumItems-1)
	{
		sDir = mpComboBoxCurrentDirectory->GetItemText(lSelection);

		for(int i=lSelection-1;i<lNumItems-1; ++i)
		{
			mvCurrentDirFullPath.pop_back();
		}

		NavigateTo(sDir);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpTextureBrowser, CurrentDirectory_OnSelectionChange);

//----------------------------------------------------------------------------------------

bool cEdPopUpTextureBrowser::DirectoryList_OnSelectionDblClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	/////////////////////////////////////
	// Get path item which got the double click
	tWString sPath = mpListDirectories->GetItemText(mpListDirectories->GetSelectedItem());

	// Try to navigate to path (if folder)
	NavigateTo(sPath);

	UpdateCurrentDirectory();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdPopUpTextureBrowser, DirectoryList_OnSelectionDblClick);

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::UpdateCurrentDirectory()
{
	mpComboBoxCurrentDirectory->ClearItems();
	mpComboBoxCurrentDirectory->AddItem(_W("<System Root>"));
	for(int i=0;i<(int)mvCurrentDirFullPath.size();++i)
	{
		mpComboBoxCurrentDirectory->AddItem(mvCurrentDirFullPath[i]);
	}
	mpComboBoxCurrentDirectory->SetSelectedItem(mpComboBoxCurrentDirectory->GetItemNum()-1);

	PopulateDirList();
}

//----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::OnNavigate()
{
	UpdateCurrentDirectory();
	// and repopulate the file list
	PopulateTextureList();
}

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);

	if(abX)
		InitBrowser();
}

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::Close(bool abReturnFile)
{
	int lIntMessageData = (int)(abReturnFile);
	if(mpCallback && mpCallbackObject)
		mpCallback(mpCallbackObject, NULL, cGuiMessageData(lIntMessageData));

	SetActive(false);
}

//----------------------------------------------------------------------------------------

iEdObjectIndex* cEdPopUpTextureBrowser::CreateIndex(const tWString& asFolder)
{
	std::map<tWString, iEdObjectIndex*>::iterator it = mmapTextureIndices.find(asFolder);
	if(it!=mmapTextureIndices.end())
	{
		iEdObjectIndex* pIndex = it->second;
		pIndex->Refresh();
		return pIndex;
	}

	//mpEditor->ShowLoadingWindow(_W("Loading"), _W("Loading"));

	iEdObjectIndex* pIndex = hplNew(cEditorObjectIndexTextures,(mpEditor, asFolder));
	if(pIndex)
	{
		mmapTextureIndices[asFolder] = pIndex;
		pIndex->Create();
	}

	return pIndex;
}

//----------------------------------------------------------------------------------------

void cEdPopUpTextureBrowser::SetType(eEdTexture aType)
{
	if(mType == aType) return;
	mType = aType;
	/*

	if(aType==eEdTexture_CubeMap)
	{
		AddCategory(_W("CubeMaps"), _W("dds"));
	}
	//if(mType==4)
	//{
	//	AddCategory(_W("Materials"), _W("mat"));
	//}
	else
	{
		int lID = AddCategory(_W("Textures"));
		tStringVec* pvStrings = mpEditor->GetEngine()->GetResources()->GetBitmapLoaderHandler()->GetSupportedTypes();
		for(size_t i=0; i<pvStrings->size(); ++i)
			AddFilter(lID, cString::To16Char(pvStrings->at(i)));
	}
	*/
}

//----------------------------------------------------------------------------------------


