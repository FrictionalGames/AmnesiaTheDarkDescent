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

#ifndef ED_WINDOW_OUTLINE_H
#define ED_WINDOW_OUTLINE_H

//--------------------------------------------------------------------

#include "../Common/EdWindowHandler.h"
#include "../Common/EdScnObject.h"

class iEdScnObject;
class cEdWindowOutline;

//--------------------------------------------------------------------

class cUIPickFilter;

/*
class cEdWindowOutlineHelper : public iEdPopUp
{
public:
	cEdWindowOutlineHelper(cEdWindowOutline* apWindow, tIntList& alstIDs);
	~cEdWindowOutlineHelper();

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

	void OnCreateLayout();
	void OnSetActive(bool abX);

	cEdWindowOutline* mpOutline;
	cWidgetListBox* mpListObjects;
	cWidgetButton* mvButtons[2];
	
	void* mpCallbackObject;
	tGuiCallbackFunc mpCallback;

	tIntList mlstExcludedIDs;
	
	tIntVec mvIDList;
	tIntList& mlstTempIDs;

	cUIPickFilter* mpFilter;
};
*/

//--------------------------------------------------------------------

class cEdWindowOutline : public iEdPopUp
{
public:
	cEdWindowOutline(iEditor* apEditor);

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
	
	void OnCreateLayout();
	void OnSetActive(bool abX);
	void OnUpdate();

	void OnWorldAddRemObject();
	void OnWorldObjModify();
	void OnWorldSelectChange();

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool AttachParentBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(AttachParentBodyCallback);
	bool AttachChildBodyCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(AttachChildBodyCallback);
	bool SetAttachmentsCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SetAttachmentsCallback);

	bool HelperButton_OnPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(HelperButton_OnPressed);

	iEdScnObject* GetSelectedObject();

	void PushEditButtonsState();
	void PopEditButtonsState();
	void SetEditButtonsEnabled(bool);

	void ShowHelperPane(const tScnObjTypeVec& avAttachableTypes, const tScnObjList& alstObjects, tGuiCallbackFunc apCallback, bool abMulti=false);
	void HideHelperPane();

	void PopulateHelperList();

	bool WindowSpecificInputCallback(iEdInput*);

	tScnObjList mlstAddedChildren;

	cWidgetListBox* mpListObjects;
	cEdInputBool*	mpInpHideShapes;

	cWidgetDummy* mpGroupPanel;
	cWidgetButton* mpBDetach;
	cWidgetButton* mpBAttachParent;
	cWidgetButton* mpBDetachParent;
	cWidgetButton* mpBAttachChild;
	cWidgetButton* mpBDetachChild;
	cWidgetButton* mpBEditAttachments;

	bool mbDetachEnabled;
	bool mbAttachParentEnabled;
	bool mbAttachChildEnabled;
	bool mbDetachParentEnabled;
	bool mbDetachChildEnabled;
	bool mbEditAttachmentsEnabled;


	cWidgetDummy* mpHelperPane;
	cWidgetListBox* mpHelperList;
	cWidgetButton* mvHelperButtons[2];
	cUIPickFilter* mpFilter;

	tScnObjList mlstHelperObjects;

	tGuiCallbackFunc mpCallback;

	bool mbHideHelperPane;

	cVector2f mvSizeNormal;
	cVector2f mvSizeOpenPane;
};

//--------------------------------------------------------------------

#endif // MODEL_EDITOR_WINDOW_OUTLINE_H

