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

#include "LevelEditorWindowGroup.h"

#include "LevelEditor.h"

#include "LevelEditorActions.h"
#include "LevelEditorWorld.h"
#include "../common/EditorActionEntity.h"

#include "../common/EditorSelection.h"
#include "../common/EntityWrapper.h"

#include "../common/EditorEditModeSelect.h"

#include <algorithm>

//---------------------------------------------------------------


//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cLevelEditorWindowGroup::cLevelEditorWindowGroup(cLevelEditor* apEditor) : iEditorWindowPopUp(apEditor, "Group Window", false, false, 
																							  true, cVector2f(640,480))
{
	mbMustUpdateGroupList = true;
}

//---------------------------------------------------------------

cLevelEditorWindowGroup::~cLevelEditorWindowGroup()
{
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

bool cLevelEditorWindowGroup::MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction =NULL;

	int lSelectedItem = mpListGroups->GetSelectedItem();
	int lSelectedGroupID = -1;

	if(lSelectedItem!=-1)
		lSelectedGroupID = mvGroupIDs[lSelectedItem];
	

	if(mpMainMenuNew==apWidget)
	{
		pAction = hplNew(cLevelEditorActionGroupAdd,(mpEditor, cLevelEditorGroup::GetGroupCount(), "Group_"+cString::ToString((int)cLevelEditorGroup::GetGroupCount())));
		cLevelEditorGroup::IncGroupCount();

		mbMustUpdateGroupList = true;
	}
	else if(mpMainMenuDelete==apWidget)
	{
		if(lSelectedGroupID>0)
		{
			pAction = hplNew(cLevelEditorActionGroupDelete,(mpEditor, lSelectedGroupID));
			mbMustUpdateGroupList = true;
		}
	}
	else if(mpMainMenuAddSelected==apWidget)
	{
		if(lSelectedGroupID>=0)
		{
			pAction = hplNew(cLevelEditorActionGroupAddSelected,(mpEditor, lSelectedGroupID));
			mbMustUpdateList = true;
		}
	}
	else if(mpMainMenuRemoveSelected==apWidget)
	{
		if(lSelectedGroupID>=0)
		{
			pAction = hplNew(cLevelEditorActionGroupRemoveSelected,(mpEditor, lSelectedGroupID));
			mbMustUpdateList = true;
		}
	}
	else if(mpMainMenuSelect==apWidget)
	{
		if(lSelectedGroupID>=0)
			pAction = hplNew(cLevelEditorActionGroupSelectEntities,(mpEditor, lSelectedGroupID));
	}
	else if(mpMainMenuDeselect==apWidget)
	{
		if(lSelectedGroupID>=0)
			pAction = hplNew(cLevelEditorActionGroupDeselectEntities,(mpEditor, lSelectedGroupID));
	}

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowGroup, MainMenu_ItemClick);

//---------------------------------------------------------------

bool cLevelEditorWindowGroup::GroupList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mbMustUpdateList = true;

	Update(0);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowGroup, GroupList_OnSelectionChange);

//---------------------------------------------------------------

bool cLevelEditorWindowGroup::EntityList_OnSelectionChange(iWidget* apWidget, const cGuiMessageData& aData)
{
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

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowGroup, EntityList_OnSelectionChange);

//---------------------------------------------------------------

bool cLevelEditorWindowGroup::VisibilityCheckBox_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lSelectedItem = mpListGroups->GetSelectedItem();
	int lSelectedGroupID;
	if(lSelectedItem!=-1)
	{
		lSelectedGroupID = mvGroupIDs[lSelectedItem];

		iEditorAction* pAction = hplNew(cLevelEditorActionGroupSetVisibility,(mpEditor,lSelectedGroupID, mpCheckBoxGroupVisibility->IsChecked()));
		mpEditor->AddAction(pAction);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowGroup, VisibilityCheckBox_OnChange);

//---------------------------------------------------------------

bool cLevelEditorWindowGroup::Name_OnEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lSelectedItem = mpListGroups->GetSelectedItem();
	int lSelectedGroupID;
	if(lSelectedItem!=-1)
	{
		lSelectedGroupID = mvGroupIDs[lSelectedItem];

		iEditorAction* pAction = hplNew(cLevelEditorActionGroupSetName,(mpEditor,lSelectedGroupID, cString::To8Char(mpInputName->GetText())));
		mpEditor->AddAction(pAction);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditorWindowGroup, Name_OnEnter);

//---------------------------------------------------------------

void cLevelEditorWindowGroup::OnInitLayout()
{
	//////////////////////////////
	// Set up layout
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Groups"));

	cVector2f vTextSize = cVector2f(11);

	mpMainMenu = mpSet->CreateWidgetMainMenu(mpWindow);
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


	mpListGroups = mpSet->CreateWidgetListBox(cVector3f(10,50,1), cVector2f(250,340), mpWindow);
	mpListGroups->SetDefaultFontSize(vTextSize);
	mpListGroups->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(GroupList_OnSelectionChange));

	mpListGroupEntities = mpSet->CreateWidgetMultiPropertyListBox(cVector3f(270,50,1), cVector2f(340,340), mpWindow);
	mpListGroupEntities->SetDefaultFontSize(vTextSize);
	mpListGroupEntities->SetAllowMultiSelection(true);
	mpListGroupEntities->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(EntityList_OnSelectionChange));
	mpListGroupEntities->AddColumn("Name", 0);
	mpListGroupEntities->SetColumnWidth(0,340);

	cWidgetLabel* pLabelParams = mpSet->CreateWidgetLabel(cVector3f(10,400,0.1f), 0, _W("Group parameters"), mpWindow);
	pLabelParams->SetDefaultFontSize(vTextSize);

	cWidgetLabel* pLabelName = mpSet->CreateWidgetLabel(cVector3f(10,420,0.1f), 0,_W("Name"),mpWindow);
	pLabelName->SetDefaultFontSize(vTextSize);
	mpInputName = mpSet->CreateWidgetTextBox(cVector3f(60,417,0.1f),cVector2f(200,25), _W(""), mpWindow);
	mpInputName->SetDefaultFontSize(vTextSize);
	mpInputName->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(Name_OnEnter));

	mpCheckBoxGroupVisibility = mpSet->CreateWidgetCheckBox(cVector3f(10,450,0.1f), 0, _W("Visible"),mpWindow);
	mpCheckBoxGroupVisibility->SetDefaultFontSize(vTextSize);
	mpCheckBoxGroupVisibility->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(VisibilityCheckBox_OnChange));

	mpEditor->SetLayoutNeedsUpdate(true);
}

//---------------------------------------------------------------

void cLevelEditorWindowGroup::UpdateGroupList()
{
	if(mbMustUpdateGroupList==false)
		return;

	mbMustUpdateGroupList = false;

	int lSelected = mpListGroups->GetSelectedItem();

	mvGroupIDs.clear();
	mpListGroups->ClearItems();

	tGroupMap& mapGroups = ((cLevelEditor*)mpEditor)->GetGroups();

	tGroupMapIt it = mapGroups.begin();

	for(;it!=mapGroups.end();++it)
	{
		cLevelEditorGroup& pGroup = it->second;

		mvGroupIDs.push_back(pGroup.GetID());
		mpListGroups->AddItem(cString::To16Char(pGroup.GetName()));
	}

	if(lSelected >= mpListGroups->GetItemNum())
		lSelected = mpListGroups->GetItemNum()-1;

	mpListGroups->SetSelectedItem(lSelected);
}

//---------------------------------------------------------------

void cLevelEditorWindowGroup::UpdateEntityList()
{
	if(mbMustUpdateList==false)
		return;

	mbMustUpdateList = false;

	int lCursorPos = mpListGroupEntities->GetCursorPos();
	mpListGroupEntities->ClearItems();

	int lSelectedItem = mpListGroups->GetSelectedItem();
	int lSelectedGroupID;
	if(lSelectedItem!=-1)
	{
		lSelectedGroupID = mvGroupIDs[lSelectedItem];
		tEntityWrapperMap& mapEntities = mpEditor->GetEditorWorld()->GetEntities();
		tEntityWrapperMapIt it = mapEntities.begin();

		cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)mpEditor->GetEditMode("Select");

		for(;it!=mapEntities.end();++it)
		{
			iEntityWrapper* pEnt = it->second;
			cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();


			if(pData->mlGroupID==lSelectedGroupID)
			{
				cWidgetItem* pItem = mpListGroupEntities->AddItem();

				pItem->AddProperty(cString::To16Char(pEnt->GetName()));
				pItem->SetUserData(pEnt);
			}
		}
	}

	mpListGroupEntities->SetCursorPos(lCursorPos, true, true);
}

//---------------------------------------------------------------

void cLevelEditorWindowGroup::UpdateGroupParams()
{
	int lSelectedItem = mpListGroups->GetSelectedItem();
	if(lSelectedItem>-1)
	{
		int lSelectedGroupID = mvGroupIDs[lSelectedItem];
		cLevelEditorGroup& group =  ((cLevelEditor*)mpEditor)->GetGroup(lSelectedGroupID);

		mpInputName->SetText(cString::To16Char(group.GetName()));
		mpCheckBoxGroupVisibility->SetChecked(group.GetVisibility(),false);
		mpCheckBoxGroupVisibility->SetEnabled(true);

		mpInputName->SetCanEdit( lSelectedGroupID!=0 );
		mpInputName->SetEnabled(true);
	}
	else
	{
		mpInputName->SetText(_W(""));
		mpInputName->SetEnabled(false);
		mpCheckBoxGroupVisibility->SetEnabled(false);
	}
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------

void cLevelEditorWindowGroup::OnWorldModify()
{
	mbMustUpdateGroupList = true;
	mbMustUpdateList = true;
}

//---------------------------------------------------------------

void cLevelEditorWindowGroup::OnUpdate(float afTimeStep)
{
	UpdateGroupList();
	UpdateGroupParams();
	UpdateEntityList();

	mpListGroupEntities->ClearSelection();
	for(int i=0;i<mpListGroupEntities->GetItemNum();++i)
	{
		cWidgetItem* pItem = mpListGroupEntities->GetItem(i);
		iEntityWrapper* pEnt = (iEntityWrapper*)pItem->GetUserData();

		if(pEnt->IsSelected())
			pItem->SetSelected(true);
	}
}

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------

//---------------------------------------------------------------
