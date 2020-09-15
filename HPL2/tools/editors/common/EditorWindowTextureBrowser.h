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

#ifndef HPLEDITOR_EDITOR_WINDOW_TEXTURE_BROWSER_H
#define HPLEDITOR_EDITOR_WINDOW_TEXTURE_BROWSER_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorWindow.h"

#include "EditorIndex.h"

//--------------------------------------------------------------------

class iEditorBase;

class cEditorObjectIndexTextures;
class cEditorWindowTextureBrowser;

//--------------------------------------------------------------------

class cEditorObjectIndexEntryTexture : public iEditorObjectIndexEntry
{
public:
	cEditorObjectIndexEntryTexture(cEditorObjectIndexTextures* apIndex);

	bool CreateFromFile(const tWString& asFilename);
	bool CreateFromXmlElement(cXmlElement* apElement);

	tString GetTextureFile();
	tString GetTextureFileFullPath();
	const cVector3l& GetTextureSize() { return mvSize; }
	tWString GetTextureTypeString();
	eEditorTextureResourceType GetTextureType() { return mType; }

	void Save(cXmlElement* apElement);

protected:
	void BuildEntryName(tString& asEntryName);
	void BuildThumbnail();

	eEditorTextureResourceType GetTextureTypeFromTypeString(const tString& asType);

	tString GetDiffuseFromMatFile(const tString& asFileName);

	tString msTextureFile;
	cVector3l mvSize;
	eEditorTextureResourceType mType;
};

//--------------------------------------------------------------------

class cEditorObjectIndexDirTextures : public iEditorObjectIndexDir
{
public:
	cEditorObjectIndexDirTextures(cEditorObjectIndexTextures* apIndex);

	iEditorObjectIndexEntry* CreateEntry();

protected:
};

//--------------------------------------------------------------------

class cEditorObjectIndexTextures : public iEditorObjectIndex
{
public:
	cEditorObjectIndexTextures(iEditorBase* apEditor, const tWString& asBaseDir);

	iEditorObjectIndexDir* CreateDir(iEditorObjectIndexDir* apParent=NULL);
};

//--------------------------------------------------------------------

class cTextureDescriptor
{
public:
	tString msFilename;
	cVector3l mvSize;
	eEditorTextureType mType;
	tString msDate;
};

//--------------------------------------------------------------------

class cTextureBrowserIcon
{
public:
	cTextureBrowserIcon(cEditorWindowTextureBrowser* apBrowser,
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

	cEditorWindowTextureBrowser* mpBrowser;
	cGuiSet* mpSet;
	cWidgetFrame* mpIconBG;
	cWidgetImage* mpTexture;
	cWidgetLabel* mpLabelName;
	cWidgetLabel* mpLabelSize;
	cWidgetLabel* mpLabelType;

	cEditorObjectIndexEntryTexture* mpEntry;
};


//--------------------------------------------------------------------

class cEditorWindowTextureBrowser : public iEditorWindowPopUp, public iFileBrowser
{
public:
	cEditorWindowTextureBrowser(iEditorBase* apEditor, 
								eEditorTextureResourceType aType,
								const tWString& asStartDir, 
								tWString& asTextureFilename, 
								void* apCallbackObject, tGuiCallbackFunc apCallback,
								const tWStringList& alstFilters=tWStringList());
    ~cEditorWindowTextureBrowser();

	//void Open(eEditorTextureType aType, const tWString& asDirectory, tWString* apTextureFilename, void* apCallbackObject,tGuiCallbackFunc apCallback);

	void Close(bool abX);

	void SetSelectedIcon(cTextureBrowserIcon* apIcon);

	//void MoveSlider(int alMouseButton);

	iEditorObjectIndex* CreateIndex(const tWString& asFolder);
protected:
	void OnInitLayout();
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

	eEditorTextureResourceType mType;
	tWStringList mlstFilters;

	cWidgetButton* mvButtons[2];

	tWString& msTextureFilename;

	void * mpCallbackObject;
	tGuiCallbackFunc mpCallback;

	std::map<tWString, iEditorObjectIndex*> mmapTextureIndices;
	iEditorObjectIndex* mpCurrentIndex;
};

//---------------------------------------------------------

#endif //HPLEDITOR_EDITOR_WINDOW_TEXTURE_BROWSER_H

