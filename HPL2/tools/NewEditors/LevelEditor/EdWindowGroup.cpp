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

#include "../LevelEditor/EdWindowGroup.h"

#include "../Common/Editor.h"

#include "../Common/EdSelection.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdEditModeHandler.h"
#include "../Common/EdEditModeSelect.h"

#include "../LevelEditor/HPLMap.h"

#include <algorithm>

//---------------------------------------------------------------


//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cEdWindowGroup::cEdWindowGroup(iEditor* apEditor) : iEdPopUp(apEditor, _W("Group Window"))
{
	mbMustUpdateGroupList = true;
	mbUpdating = false;
}

//---------------------------------------------------------------

cEdWindowGroup::~cEdWindowGroup()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

bool cEdWindowGroup::MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdAction* pAction =NULL;

	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());
	cEdEditModeSelect* pSelect = static_cast<cEdEditModeSelect*>(mpEditor->GetEditModeHandler()->GetMode(_W("Select")));
	cEdSelection* pSelection = pSelect->GetSelection();

	cWidgetItem* pSelectedItem = mpListGroups->GetItem(mpListGroups->GetSelectedItem());
	cEdGroup* pSelectedGroup = NULL;
	if(pSelectedItem) pSelectedGroup = static_cast<cEdGroup*>(pSelectedItem->GetUserData());

	if(mpMainMenuNew==apWidget)
	{
		pAction = pMap->CreateAddGroupAction(); //->Create,(mpEditor, cLevelEditorGroup::GetGroupCount(), "Group_"+cString::ToString((int)cLevelEditorGroup::GetGroupCount())));
		mbMustUpdateGroupList = true;
	}
	else if(mpMainMenuDelete==apWidget)
	{
		if(pSelectedGroup)
		{
			pAction = pMap->CreateRemoveGroupAction(pSelectedGroup->GetID());//(cLevelEditorActionGroupDelete,(mpEditor, lSelectedGroupID));
			mbMustUpdateGroupList = true;
		}
	}
	else if(mpMainMenuAddSelected==apWidget)
	{
		if(pSelectedGroup)
		{
			pAction = pSelectedGroup->CreateAddObjectsAction(pSelection->GetComponents()); //hplNew(cLevelEditorActionGroupAddSelected,(mpEditor, lSelectedGroupID));
			mbMustUpdateList = true;
		}
	}
	else if(mpMainMenuRemoveSelected==apWidget)
	{
		if(pSelectedGroup)
		{
			pAction = pSelectedGroup->CreateRemoveObjectsAction(pSelection->GetComponents());
			//pAction = pSelectedGroup->CreateRemoveObjectsFromSelectionAction(pSelection->GetComponents());//hplNew(cLevelEditorActionGroupRemoveSelected,(mpEditor, lSelectedGroupID));
			mbMustUpdateList = true;
		}
	}
	else if(mpMainMenuSelect==apWidget)
	{
		if(pSelectedGroup)
			pAction = pSelectedGroup->CreateAddObjectsToSelectionAction();//hplNew(cLevelEditorActionGroupSelectEntities,(mpEditor, lSelectedGroupID));
	}
	else if(mpMainMenuDeselect==apWidget)
	{
		if(pSelectedGroup)
			pAction = pSelectedGroup->CreateRemoveObjectsFromSelectionAction();//hplNew(cLevelEditorActionGroupDeselectEntities,(mpEditor, lSelectedGroupID));
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowGroup, MainMenu_ItemClick);

//---------------------------------------------------------------

bool cEdWindowGroup::GroupList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mbMustUpdateList = true;

	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowGroup, GroupList_OnSelectionChange);

//---------------------------------------------------------------

bool cEdWindowGroup::EntityList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	/*
	tIntList lstEntityIDs;
	for(int i=0;i<mpListGroupEntities->GetMultiSelectionNum();++i)
	{
		cWidgetItem* pItem = mpListGroupEntities->GetItem(mpListGroupEntities->GetMultiSelection(i));

		iEntityWrapper* pEnt = (iEntityWrapper*)pItem->GetUserData();
		int lEntityID = pEnt->GetID();
		lstEntityIDs.push_back(lEntityID);
	}
	cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)mpEditor->GetEditMode("Select");
	iEditorAction* pAction = hplNew(cEditorActionEntitySelect,(pEditMode,lstEntityIDs, eSelectActionType_Select));
	mpEditor->AddAction(pAction);
	*/

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowGroup, EntityList_OnSelectionChange);

//---------------------------------------------------------------

bool cEdWindowGroup::WindowSpecificInputCallback(iEdInput* apInput)
{
	cWidgetItem* pItem = mpListGroups->GetItem(mpListGroups->GetSelectedItem());
	cEdGroup* pGroup = static_cast<cEdGroup*>(pItem->GetUserData());

	iEdAction* pAction = NULL;
	if(apInput==mpInpName)
	{
		if(pGroup) pAction = pGroup->CreateSetNameAction(mpInpName->GetValue8Char());
	}
	else if(apInput==mpInpVisible)
	{
		if(pGroup) pAction = pGroup->CreateSetVisibleAction(mpInpVisible->GetValue());
	}

	return true;
}

//---------------------------------------------------------------

void cEdWindowGroup::OnCreateLayout()
{
	//////////////////////////////
	// Set up layout
	iEdPopUp::OnCreateLayout();
	GetBG()->SetSize(cVector2f(640, 480));
	GetBG()->SetText(_W("Groups"));

	cVector2f vTextSize = cVector2f(11);

	mpMainMenu = mpSet->CreateWidgetMainMenu(GetBG());
	cWidgetMenuItem* pItem = mpMainMenu->AddMenuItem(_W("Edit"));
	mpMainMenuNew = pItem->AddMenuItem(_W("New"));
	mpMainMenuNew->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuDelete = pItem->AddMenuItem(_W("Delete"));
	mpMainMenuDelete->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	pItem->AddSeparator();
	mpMainMenuAddSelected = pItem->AddMenuItem(_W("Add selected objects"));
	mpMainMenuAddSelected->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuRemoveSelected = pItem->AddMenuItem(_W("Remove selected objects"));
	mpMainMenuRemoveSelected->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	pItem->AddSeparator();
	mpMainMenuSelect = pItem->AddMenuItem(_W("Select objects in group"));
	mpMainMenuSelect->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuDeselect = pItem->AddMenuItem(_W("Deselect objects in group"));
	mpMainMenuDeselect->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));


	mpListGroups = mpSet->CreateWidgetListBox(cVector3f(10,50,1), cVector2f(250,340), GetBG());
	mpListGroups->SetDefaultFontSize(vTextSize);
	mpListGroups->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(GroupList_OnSelectionChange));

	mpListGroupEntities = mpSet->CreateWidgetListBox(cVector3f(270,50,1), cVector2f(340,340), GetBG());
	mpListGroupEntities->SetDefaultFontSize(vTextSize);
	mpListGroupEntities->SetAllowMultiSelection(true);
	mpListGroupEntities->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(EntityList_OnSelectionChange));
	
	cWidgetLabel* pLabelParams = mpSet->CreateWidgetLabel(cVector3f(10,400,0.1f), 0, _W("Group parameters"), GetBG());
	pLabelParams->SetDefaultFontSize(vTextSize);

	mpInpName = CreateInputString(cVector3f(10, 420, 0.1f), _W("Name"), GetBG());
	mpInpVisible = CreateInputBool(cVector3f(10, 450, 0.1f), _W("Visible"), GetBG());
}

//---------------------------------------------------------------

void cEdWindowGroup::UpdateGroupList()
{
	if(mbMustUpdateGroupList==false)
		return;

	mbMustUpdateGroupList = false;


	cWidgetItem* pItem = mpListGroups->GetItem(mpListGroups->GetSelectedItem());
	cEdGroup* pCurrentGroup = NULL;
	if(pItem) pCurrentGroup = static_cast<cEdGroup*>(pItem->GetUserData());

	cHplMap* pMap = static_cast<cHplMap*>(mpEditor->GetWorld());

	mpListGroups->ClearItems();
	const tGroupList& lstGroups = pMap->GetGroups();
	tGroupList::const_iterator it = lstGroups.begin();

	int lSelectedIdx = -1;

	///////////////////////////////////////////////
	// Always add null group first
	cEdGroup* pNullGroup = pMap->GetNullGroup();
	if(pNullGroup==pCurrentGroup)
		lSelectedIdx = mpListGroups->GetItemNum();

	cWidgetItem* pNullGroupItem = mpListGroups->AddItem(pNullGroup->GetName());
	pNullGroupItem->SetUserData(pNullGroup);
	for(;it!=lstGroups.end();++it)
	{
		cEdGroup* pGroup = *it;

		if(pGroup==pCurrentGroup)
			lSelectedIdx = mpListGroups->GetItemNum();

		cWidgetItem* pGroupItem = mpListGroups->AddItem(pGroup->GetName());
		pGroupItem->SetUserData(pGroup);
	}

	mpListGroups->SetSelectedItem(lSelectedIdx, false, mbUpdating==false);
}

//---------------------------------------------------------------

void cEdWindowGroup::UpdateEntityList()
{
	if(mbMustUpdateList==false)
		return;

	mbMustUpdateList = false;

	int lCursorPos = mpListGroupEntities->GetCursorPos();
	mpListGroupEntities->ClearItems();

	cWidgetItem* pItem = mpListGroups->GetItem(mpListGroups->GetSelectedItem());
	if(pItem)
	{
		cEdGroup* pGroup = static_cast<cEdGroup*>(pItem->GetUserData());
		
		const tScnObjList& lstObjects = pGroup->GetObjects();
		tScnObjList::const_iterator it = lstObjects.begin();

		for(;it!=lstObjects.end();++it)
		{
			iEdScnObject* pObj =*it;
			cWidgetItem* pItem = mpListGroupEntities->AddItem(pObj->GetName());
			pItem->SetUserData(pObj);
		}
	}

	mpListGroupEntities->SetCursorPos(lCursorPos, true, true);
}

//---------------------------------------------------------------

void cEdWindowGroup::UpdateGroupParams()
{
	cWidgetItem* pItem = mpListGroups->GetItem(mpListGroups->GetSelectedItem());
	if(pItem)
	{
		cEdGroup* pGroup = static_cast<cEdGroup*>(pItem->GetUserData());

		mpInpName->SetValue(cString::To16Char(pGroup->GetName()), false);
		mpInpVisible->SetValue(pGroup->IsVisible(), false);

		mpInpName->GetHandle()->SetEnabled(false);
		mpInpVisible->GetHandle()->SetEnabled(false);
	}
	else
	{
		mpInpName->SetValue(_W(""), false);
		mpInpName->GetHandle()->SetEnabled(false);
		mpInpVisible->GetHandle()->SetEnabled(false);
	}
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------

void cEdWindowGroup::OnWorldParamsChange()
{
	mbMustUpdateGroupList = true;
	mbMustUpdateList = true;

	SetUpdated();
}

void cEdWindowGroup::OnWorldAddRemObject()
{
	mbMustUpdateList = true;

	SetUpdated();
}

//---------------------------------------------------------------

void cEdWindowGroup::OnUpdate()
{
	iEdPopUp::OnUpdate();

	mbUpdating = true;

	UpdateGroupList();
	UpdateGroupParams();
	UpdateEntityList();

	mpListGroupEntities->ClearSelection();
	for(int i=0;i<mpListGroupEntities->GetItemNum();++i)
	{
		cWidgetItem* pItem = mpListGroupEntities->GetItem(i);
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pItem->GetUserData());

		if(pObj->IsSelected())
			pItem->SetSelected(true);
	}

	mbUpdating = false;
}

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------
