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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorWindow.h"

#include "EditorEditModeSelect.h"

#include "EditorVar.h"

//------------------------------------------------------------------------------

class iEntityWrapper;
class iEntityWrapperCustomVarEntity;

class cEditorEditModeSelect;

//------------------------------------------------------------------------------

class cEditorWindowEntityEditBox : public iEditModeWindow
{
public:
	cEditorWindowEntityEditBox(cEditorEditModeSelect* apEditMode,iEntityWrapper* apEntity);
	virtual ~cEditorWindowEntityEditBox();

	virtual void Create()=0;

	void AddAttachedChild(iEntityWrapper* apChild);
	void RemoveAttachedChild(iEntityWrapper* apChild);

protected:
	void OnInitLayout();
	void OnSetActive(bool abX);

	virtual void OnUpdate(float afTimeStep);

	virtual void SetUpGeneralTab();

	void AddPropertyName(cWidgetTab* apParentTab);
	void AddPropertyPosition(cWidgetTab* apParentTab);
	void AddPropertyScale(cWidgetTab* apParentTab);
	void AddPropertyRotation(cWidgetTab* apParentTab);
	void AddPropertyActive(cWidgetTab* apParentTab);

	void AddPropertySetAttachments(cWidgetTab* apParentTab);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool FilterButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FilterButton_OnPressed);

	bool Child_OnPick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Child_OnPick);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	///////////////////////////////////////
	// Data
	cEditorEditModeSelect* mpEditMode;
	iEntityWrapper* mpEntity;

	cWidgetTabFrame* mpTabs;

	cEditorInputText* mpInpName;
	cEditorInputText* mpInpTag;
	cEditorInputBool* mpInpActive;

	cEditorInputVec3* mpInpPosition;
	cEditorInputVec3* mpInpScale;
	cEditorInputVec3* mpInpRotation;

	//Attachments
	std::vector<cWidgetButton*> mvBAttachFilters;
	cWidgetButton* mpBEditAttachments;
	cWidgetButton* mpBClearAttachments;
	cEntitySelectorHighlighter* mpChildHighlighter;
	tEntityWrapperList mlstAttachedChildren;

	////////////////////////////////////////////
	// Last opened tab
	static int mLastEditedType;
	static int mCurrentEditedType;
	static int mlLastTabOpened;
};

//------------------------------------------------------------------------------

class cEditorWindowEntityEditBoxUserDefinedEntity : public cEditorWindowEntityEditBox
{
public:
	cEditorWindowEntityEditBoxUserDefinedEntity(cEditorEditModeSelect* apEditMode,iEntityWrapperUserDefinedEntity* apVarEntity);
	~cEditorWindowEntityEditBoxUserDefinedEntity();

	bool VarInputCallback(iEditorVarInput* apInput);
	static bool VarInputCallbackStaticHelper(void* apWin, iEditorVarInput* apInput);

	void SetRefreshInputs() { mbRefreshInputs = true; }
protected:
	void AddInputs(cWidgetTab* apParentTab, const cVector3f& avStartPos);

	void OnUpdate(float afTimeStep);

	iEntityWrapperUserDefinedEntity* mpEntity;
	cWidgetFrame* mpFVars;
	cEditorVarInputPanel* mpInputPanel;

	bool mbRefreshInputs;
};

//------------------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_H
