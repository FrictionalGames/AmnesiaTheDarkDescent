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

#ifndef MODEL_EDITOR_WINDOW_OUTLINE_H
#define MODEL_EDITOR_WINDOW_OUTLINE_H

//--------------------------------------------------------------------

#include "../common/EditorWindow.h"

class cModelEditorWindowOutline;

//--------------------------------------------------------------------

class cUIPickFilter;

class cModelEditorWindowOutlineHelper : public iEditorWindowPopUp
{
public:
	cModelEditorWindowOutlineHelper(cModelEditorWindowOutline* apWindow, tIntList& alstIDs);
	~cModelEditorWindowOutlineHelper();

	void SetAllowedTypes(const tIntVec& avTypes);

	void AddExcludedID(int alID);
	void ExcludeIDs(tIntList& alstIDs);

	void SetCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);
	
	void SetMultiSelectionEnabled(bool abX);
protected:
	void PopulateList();
	void UpdateHighlightedItems();
	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);
	bool List_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(List_OnSelectionChange);

	void OnInitLayout();
	void OnSetActive(bool abX);

	cModelEditorWindowOutline* mpOutline;
	cWidgetListBox* mpListObjects;
	cWidgetButton* mvButtons[2];
	
	void* mpCallbackObject;
	tGuiCallbackFunc mpCallback;

	tIntList mlstExcludedIDs;
	
	tIntVec mvIDList;
	tIntList& mlstTempIDs;

	cUIPickFilter* mpFilter;
};

//--------------------------------------------------------------------

class cModelEditorWindowOutline : public iEditorWindowPopUp
{
public:
	cModelEditorWindowOutline(iEditorBase* apEditor);

	void AddButtonDetach();
	void AddButtonAttachParent();
	void AddButtonAttachChild();
	void AddButtonEditAttachments();
	void AddButtonDetachParent();
	void AddButtonDetachChild();

	void ClearEditPanel();

protected:
	void PopulateList();
	void UpdateHighlightedItems();
	void UpdateEditPanel();
	void AddObjectToList(iEntityWrapper* apEntity, int alLevel);

	void OnInitLayout();
	void OnSetActive(bool abX);
	void OnUpdate(float afTimeStep);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool AttachParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(AttachParentBodyCallback);
	bool AttachChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(AttachChildBodyCallback);
	bool SetAttachmentsCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SetAttachmentsCallback);

	cModelEditorWindowOutlineHelper* CreateHelperWindow(tIntList& alstIDs, tGuiCallbackFunc apCallback, bool abMulti=false);
	iEntityWrapper* GetSelectedEntity();


	bool mbHideConnectedShapes;
	tIntVec mvIDList;
	tIntList mlstAddedChildren;

	cWidgetListBox* mpListObjects;
	cWidgetCheckBox* mpCheckBoxHideShapes;

	cWidgetDummy* mpGroupPanel;
	cWidgetButton* mpButtonDetach;
	cWidgetButton* mpButtonAttachParent;
	cWidgetButton* mpButtonDetachParent;
	cWidgetButton* mpButtonAttachChild;
	cWidgetButton* mpButtonDetachChild;
	cWidgetButton* mpButtonEditAttachments;

	cModelEditorWindowOutlineHelper* mpHelperWindow;
	tIntList mlstTempIDs;

	unsigned int mlModificationStamp;
};

//--------------------------------------------------------------------

#endif // MODEL_EDITOR_WINDOW_OUTLINE_H

