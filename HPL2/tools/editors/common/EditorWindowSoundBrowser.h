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

#ifndef HPLEDITOR_EDITOR_WINDOW_SOUND_BROWSER_H
#define HPLEDITOR_EDITOR_WINDOW_SOUND_BROWSER_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorWindow.h"

//--------------------------------------------------------------------

class iEditorBase;

//--------------------------------------------------------------------

class cEditorWindowSoundBrowser : public iEditorWindowPopUp, public iFileBrowser
{
public:
	cEditorWindowSoundBrowser(iEditorBase* apEditor, 
								const tWString& asStartDir, 
								tWString& asFilename, 
								void* apCallbackObject, tGuiCallbackFunc apCallback);
    ~cEditorWindowSoundBrowser();

	void Close(bool abX);

protected:
	void OnInitLayout();
	void OnSetActive(bool abX);

	void OnNavigate();
	void OnAddFilter(){}

	void UpdateCurrentDirectory();
	void PopulateDirList();
	void PopulateFileList();

	bool Window_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Window_OnUpdate);
	bool CurrentDirectory_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(CurrentDirectory_OnSelectionChange);
	bool FileList_OnClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileList_OnClick);
	bool FileList_OnDblClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FileList_OnDblClick);
	bool PlayStop_OnClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PlayStop_OnClick);
	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);

	tWString msCurrentDirectory;
	tWString& msFilename;

	void *mpCallbackObject;
	tGuiCallbackFunc mpCallback;

	cWidgetMultiPropertyListBox* mpLBFiles;

	cWidgetComboBox* mpCBCurrentDirectory;
	cWidgetTextBox* mpTBCurrentFileName;

	cGuiGfxElement* mvGfxTypes[2];
	cGuiGfxElement* mvGfxButtons[2];

	cWidgetButton* mpBPlayStop;
	cWidgetButton* mvButtons[2];

	iSoundData* mpTestSound;
	cSoundEntity* mpSoundEntity;
};

//---------------------------------------------------------

#endif //HPLEDITOR_EDITOR_WINDOW_SOUND_BROWSER_H

