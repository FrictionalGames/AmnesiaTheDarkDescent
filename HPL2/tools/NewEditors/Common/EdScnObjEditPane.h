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

#ifndef ED_SCN_OBJ_EDIT_PANE_H
#define ED_SCN_OBJ_EDIT_PANE_H

//------------------------------------------------------------------------------

#include "../Common/StdAfx.h"
using namespace hpl;

#include "../Common/EdEditPane.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdEditModeSelect.h"

#include "../Common/EdVar.h"

//------------------------------------------------------------------------------

class cEdEditModeSelect;

//------------------------------------------------------------------------------

class iEdScnObjEditPane : public iEdEditPane
{
	friend class cEdEditModeSelect;
public:
	iEdScnObjEditPane(iEdObject*);
	virtual ~iEdScnObjEditPane();

	virtual void Create()=0;

	void AddAttachedChild(iEdScnObject*);
	void RemoveAttachedChild(iEdScnObject*);

protected:
	void OnCreateLayout();
	void OnSetActive(bool);

	virtual void OnUpdate();

	virtual void SetUpGeneralTab();

	void AddManipulatorButtons(const cVector3f&, iWidget** apCurrentManipulatorButton);

	void AddCompoundDetachButton(cWidgetTab* apParentTab);
	void AddPropertyName(cWidgetTab* apParentTab);
	void AddPropertyPosition(cWidgetTab* apParentTab);
	void AddPropertyScale(cWidgetTab* apParentTab);
	void AddPropertyRotation(cWidgetTab* apParentTab);
	void AddPropertyActive(cWidgetTab* apParentTab);

	void AddPropertySetAttachments(cWidgetTab* apParentTab);

	bool SetManipulator(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SetManipulator);

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool FilterButton_OnPressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(FilterButton_OnPressed);

	bool Child_OnPick(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Child_OnPick);

	bool WindowSpecificInputCallback(iEdInput*);

	///////////////////////////////////////
	// Data
	cWidgetTabFrame* mpTabs;

	cEdInputText* mpInpName;
	cEdInputText* mpInpTag;
	cEdInputBool* mpInpActive;

	cEdInputVec3* mpInpPosition;
	cEdInputVec3* mpInpScale;
	cEdInputVec3* mpInpRotation;

	cWidgetButton* mpBRemoveFromCompound;

	//Attachments
	tWidgetList mlstBAttachFilters;
	cWidgetButton* mpBEditAttachments;
	cWidgetButton* mpBClearAttachments;

	cEdSelectorHighlighter* mpChildHighlighter;
	tScnObjList mlstAttachedChildren;

	tWidgetList mlstManipulatorButtons;


	static cEdEditModeSelect* mpSelect;
	////////////////////////////////////////////
	// Last opened tab
	static int mLastEditedType;
	static int mCurrentEditedType;
	static int mlLastTabOpened;

	static iScnObjManipulator* mpLastActiveManipulator;
};

//------------------------------------------------------------------------------

class iEdUserObjEditPane : public iEdScnObjEditPane
{
public:
	iEdUserObjEditPane(iEdScnObject*);
	virtual ~iEdUserObjEditPane();

	bool VarInputCallback(iEdVarInput*);
	static bool VarInputCallbackStaticHelper(void*, iEdVarInput*);

	void SetRefreshInputs() { mbRefreshInputs = true; }
protected:
	void AddInputs(cWidgetTab*, const cVector3f&);

	void OnWorldObjModify();

	void OnUpdate();

	cWidgetFrame* mpFVars;
	iEdVarInputPanel* mpInputPanel;

	bool mbRefreshInputs;
};

//------------------------------------------------------------------------------

#endif // ED_SCN_OBJ_EDIT_PANE_H
