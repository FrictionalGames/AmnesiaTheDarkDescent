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

#ifndef ED_POPUP_TEXTURE_BROWSER_H
#define ED_POPUP_TEXTURE_BROWSER_H

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdWindowHandler.h"
#include "../Common/EdIndex.h"

//--------------------------------------------------------------------

class iEditor;

class cEditorObjectIndexTextures;
class cEdPopUpTextureBrowser;

//--------------------------------------------------------------------

class cEditorObjectIndexEntryTexture : public iEdObjectIndexEntry
{
public:
	cEditorObjectIndexEntryTexture(cEditorObjectIndexTextures* apIndex);

	bool CreateFromFile(const tWString& asFilename);
	bool CreateFromXmlElement(cXmlElement* apElement);

	bool IsMaterial();

	tString GetTextureFile();
	tString GetTextureFileFullPath();
	const cVector3l& GetTextureSize() { return mvSize; }
	tWString GetTextureTypeString();
	eEdTexture GetTextureType() { return mType; }

	void Save(cXmlElement* apElement);

protected:
	void BuildEntryName(tString& asEntryName);
	void BuildThumbnail();

	eEdTexture GetTextureTypeFromBitmap(cBitmap*);
	eEdTexture GetTextureTypeFromTypeString(const tString& asType);

	tString GetDiffuseFromMatFile(const tString& asFileName);

	bool mbMaterial;

	tString msTextureFile;
	cVector3l mvSize;
	eEdTexture mType;
};

//--------------------------------------------------------------------

class cEditorObjectIndexDirTextures : public iEdObjectIndexDir
{
public:
	cEditorObjectIndexDirTextures(cEditorObjectIndexTextures* apIndex);

	iEdObjectIndexEntry* CreateEntry();

protected:
};

//--------------------------------------------------------------------

class cEditorObjectIndexTextures : public iEdObjectIndex
{
public:
	cEditorObjectIndexTextures(iEditor* apEditor, const tWString& asBaseDir);

	iEdObjectIndexDir* CreateDir(iEdObjectIndexDir* apParent=NULL);
};

//--------------------------------------------------------------------

class cTextureDescriptor
{
public:
	tString msFilename;
	cVector3l mvSize;
	eEdTexture mType;
	tString msDate;
};

//--------------------------------------------------------------------

class cTextureBrowserIcon
{
public:
	cTextureBrowserIcon(cEdPopUpTextureBrowser* apBrowser,
						iWidget* apWidget, 
						cEditorObjectIndexEntryTexture* apEntry);
	~cTextureBrowserIcon();

	cEditorObjectIndexEntryTexture* GetEntry() { return mpEntry; }

	void SetPosition(const cVector3f& avPos);
	cVector3f& GetPosition() { return mvPos; }

	void SetSliderPosition(const cVector3f& avPos);
	
	void SetSelected(bool abX);
protected:
	bool Frame_OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Frame_OnMouseUp);

	bool Frame_OnDoubleClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Frame_OnDoubleClick);

	cVector3f mvPos;

	cEdPopUpTextureBrowser* mpBrowser;
	cWidgetFrame* mpIconBG;
	cWidgetImage* mpTexture;
	cWidgetLabel* mpLabelName;
	cWidgetLabel* mpLabelSize;
	cWidgetLabel* mpLabelType;

	cEditorObjectIndexEntryTexture* mpEntry;

	static cGuiSet* mpSet;
};


//--------------------------------------------------------------------

class cEdPopUpTextureBrowser : public iEdPopUp, public iFileBrowser
{
public:
	cEdPopUpTextureBrowser(iEditor* apEditor,
							bool abMaterialBrowser,
							eEdTexture aType,
							const tWString& asStartDir, 
							tWString& asTextureFilename, 
							void* apCallbackObject, tGuiCallbackFunc apCallback,
							const tWStringList& alstFilters=tWStringList());
    ~cEdPopUpTextureBrowser();

	void Close(bool abX);

	void SetSelectedIcon(cTextureBrowserIcon* apIcon);

	//void MoveSlider(int alMouseButton);

	iEdObjectIndex* CreateIndex(const tWString& asFolder);
protected:
	void SetType(eEdTexture);

	void OnCreateLayout();
	void OnSetActive(bool abX);

	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);
	bool DirList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(DirList_OnSelectionChange);
	bool Frame_OnClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Frame_OnClick);
	/*bool FrameSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FrameSlider_OnMove);*/
	bool CurrentDirectory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(CurrentDirectory_OnSelectionChange);
	bool DirectoryList_OnSelectionDblClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(DirectoryList_OnSelectionDblClick);

	void OnNavigate();
	void OnAddFilter(){}

	void UpdateCurrentDirectory();
	void PopulateDirList();
	void PopulateTextureList();
	void BuildContentFile(const tWString& asFilename);

	tWString msCurrentDirectory;

	std::vector<cTextureBrowserIcon*> mvIcons;
	cTextureBrowserIcon* mpSelectedIcon;
	
	cWidgetComboBox* mpComboBoxCurrentDirectory;
	cWidgetListBox* mpListDirectories;
	cWidgetFrame* mpFrameTextures;
	cWidgetTextBox* mpLabelTextureFilename;

	bool mbMaterialBrowser;
	eEdTexture mType;
	tWStringList mlstFilters;

	cWidgetButton* mvButtons[2];

	tWString& msTextureFilename;

	void * mpCallbackObject;
	tGuiCallbackFunc mpCallback;

	std::map<tWString, iEdObjectIndex*> mmapTextureIndices;
	iEdObjectIndex* mpCurrentIndex;
};

//---------------------------------------------------------

#endif //ED_POPUP_TEXTURE_BROWSER_H

