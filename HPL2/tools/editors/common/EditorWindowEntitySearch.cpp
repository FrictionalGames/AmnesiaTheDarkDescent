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

#include "EditorWindowEntitySearch.h"

#include "EditorEditModeSelect.h"
#include "EditorWorld.h"
#include "EditorSelection.h"

#include "EntityWrapper.h"
#include "EditorActionEntity.h"

#include <algorithm>

//-------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

cEditorWindowEntitySearch::cEditorWindowEntitySearch(iEditorBase* apEditor) : iEditorWindowPopUp(apEditor,"Search Window", false, false, 
																								 false, cVector2f(640,410))
{
	mpEditMode = (cEditorEditModeSelect*)mpEditor->GetEditMode("Select");
	mbStartingWithLocation = true;
	mbMustUpdateList = true;
	mbUpdateHighlights = true;

	for(int i=0;i<3;++i)
		mvButtons[i] = NULL;

	mpLabelSearch = NULL;
	mpInputSearch = NULL;

	mpLabelInside = NULL;
	mpComboBoxInside = NULL;

	mpListFoundEntities = NULL;
}


cEditorWindowEntitySearch::~cEditorWindowEntitySearch()
{
}

//-------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

bool cEditorWindowEntitySearch::SearchParam_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mbMustUpdateList = true;
	UpdateFoundEntities();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntitySearch,SearchParam_OnChange);

//-------------------------------------------------------------------------------

bool cEditorWindowEntitySearch::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	tIntList lstEntityIDs;
	for(int i=0;i<mpListFoundEntities->GetMultiSelectionNum();++i)
	{
		cWidgetItem* pItem = mpListFoundEntities->GetItem(mpListFoundEntities->GetMultiSelection(i));
		iEntityWrapper* pEnt = (iEntityWrapper*)pItem->GetUserData();
		int lEntityID = pEnt->GetID();
		lstEntityIDs.push_back(lEntityID);
	}

	iEditorAction* pAction = NULL;

	if(apWidget==mpListFoundEntities)
	{
		pAction = mpEditMode->CreateSelectEntityAction(lstEntityIDs,eSelectActionType_Select);
	}
	else if(apWidget == mvButtons[0])
	{
		pAction = mpEditor->CreateFocusOnSelectionAction();
	}
	else if(apWidget == mvButtons[1])
	{
		pAction = mpEditMode->CreateDeleteEntitiesAction(lstEntityIDs);
	}

	mpEditor->AddAction(pAction);


	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntitySearch,InputCallback);

//-------------------------------------------------------------------------------

bool cEditorWindowEntitySearch::StringLocation_OnCheckChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntitySearch,StringLocation_OnCheckChange);

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::UpdateFoundEntities()
{
	if(mbMustUpdateList==false)
		return;

	mbMustUpdateList = false;

	int lCursorPos = mpListFoundEntities->GetCursorPos();
	mpListFoundEntities->ClearItems();

	tString sSearchString = cString::ToLowerCase(cString::To8Char(mpInputSearch->GetText()));
	int lSearchIn = mpComboBoxInside->GetSelectedItem();

	tEntityWrapperMap& mapEntities = mpEditMode->GetEditorWorld()->GetEntities();
	tEntityWrapperMapIt it = mapEntities.begin();
	for(;it!=mapEntities.end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		tString sObjString;
		const tWString& sTypeString = pEnt->GetTypeName();

		switch(lSearchIn)
		{
		case 0:		// Object Name
			sObjString = pEnt->GetName();
			break;
		//case 1:		// Object Tag
		//	sObjString = pEnt->GetTag();
		//	break;
		case 1:		// Object Type
			sObjString = cString::To8Char(sTypeString);
			break;
		default:
			sObjString = pEnt->GetName();
			break;
		}

		sObjString = cString::ToLowerCase(sObjString);

		int lPos = cString::GetFirstStringPos(sObjString, sSearchString);

		bool bAddItem = sSearchString=="" || lPos>=0 ;
		if(bAddItem)
		{
			cWidgetItem* pItem = mpListFoundEntities->AddItem();

			pItem->AddProperty(cString::ToStringW(pEnt->GetID()));
			pItem->AddProperty(cString::To16Char(pEnt->GetName()));
			//pItem->AddProperty(cString::To16Char(pEnt->GetTag()));
			pItem->AddProperty(sTypeString);
			pItem->SetUserData(pEnt);
		}
	}

	mpListFoundEntities->SetCursorPos(lCursorPos, true, true);
	mbUpdateHighlights=true;
	UpdateItemHighlight();
}

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::UpdateItemHighlight()
{
	if(mbUpdateHighlights==false)
		return;

	mbUpdateHighlights = false;
	cEditorSelection* pSelection = mpEditor->GetSelection();
	tEntityWrapperList& lstEntities = pSelection->GetEntities();

	mpListFoundEntities->ClearSelection();
	for(int i=0;i<mpListFoundEntities->GetItemNum();++i)
	{
		cWidgetItem* pItem = mpListFoundEntities->GetItem(i);
		iEntityWrapper* pEnt = (iEntityWrapper*) pItem->GetUserData();

		if(find(lstEntities.begin(), lstEntities.end(), pEnt)!=lstEntities.end())
			pItem->SetSelected(true);
	}
}


//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("Find Objects"));

	cVector2f vTextSize = cVector2f(11);
	cVector3f vPos = cVector3f(20,35,0.1f);

	/////////////////////////////////////////////
	// Search Text
	mpLabelSearch = mpSet->CreateWidgetLabel(vPos, -1, _W("Search Text"), mpWindow);
	mpLabelSearch->SetDefaultFontSize(vTextSize);
	vPos.x += mpLabelSearch->GetSize().x+5;
	mpInputSearch = mpSet->CreateWidgetTextBox(vPos+cVector3f(0,-3,0), cVector2f(200,0), _W(""), mpWindow, eWidgetTextBoxInputType_Normal);
	mpInputSearch->AddCallback(eGuiMessage_TextChange, this, kGuiCallback(SearchParam_OnChange));
	mpInputSearch->SetDefaultFontSize(vTextSize);

	vPos.x += mpInputSearch->GetSize().x + 15;

	/////////////////////////////////////////////
	// Text filter
	mpLabelInside = mpSet->CreateWidgetLabel(vPos, -1, _W("In"), mpWindow);
	mpLabelInside->SetDefaultFontSize(vTextSize);
	mpComboBoxInside = mpSet->CreateWidgetComboBox(vPos+cVector3f(mpLabelInside->GetSize().x+5,-3,0), cVector2f(100,0), _W(""), mpWindow);
	mpComboBoxInside->SetDefaultFontSize(vTextSize);
	mpComboBoxInside->AddItem(_W("Name"));
	//mpComboBoxInside->AddItem(_W("Tag"));
	mpComboBoxInside->AddItem(_W("Type"));
	mpComboBoxInside->SetSelectedItem(0);
	mpComboBoxInside->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(SearchParam_OnChange));

	vPos.x = 20;
	vPos.y += mpComboBoxInside->GetSize().y+10;

	//////////////////////////////////////////
	// Found Entities List
	mpListFoundEntities = mpSet->CreateWidgetMultiPropertyListBox(vPos, cVector2f(600,300), mpWindow);
	mpListFoundEntities->SetBackgroundZ(-0.01f);
	mpListFoundEntities->SetAllowMultiSelection(true);
	mpListFoundEntities->SetDefaultFontSize(vTextSize);
	mpListFoundEntities->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));

	mpListFoundEntities->AddColumn("ID", 0);
	mpListFoundEntities->SetColumnWidth(0, 50);

	mpListFoundEntities->AddColumn("Name", 1);
	mpListFoundEntities->SetColumnWidth(1, 350);

	//mpListFoundEntities->AddColumn("Tag", 2);
	//mpListFoundEntities->SetColumnWidth(2, 200);

	mpListFoundEntities->AddColumn("Type",2);
	mpListFoundEntities->SetColumnWidth(2,200);

	vPos.y += mpListFoundEntities->GetSize().y+10;
	vPos.x = 500;

	////////////////////////////////////////////
	// Buttons
	tWString vButtonStrings[] = {
		_W("Focus"),
		_W("Delete"),
	};
	for(int i=0;i<2;++i)
	{
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos+cVector3f(i*60.0f,0,0),cVector2f(50,20),vButtonStrings[i], mpWindow);

		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
		mvButtons[i] = pButton;
	}
}

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::OnUpdate(float afTimeStep)
{
	if(IsActive())
	{
		UpdateFoundEntities();
		UpdateItemHighlight();
	}
}

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::OnWorldModify()
{
	mbMustUpdateList = true;
}

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::OnSelectionChange()
{
	mbUpdateHighlights = true;
}

//-------------------------------------------------------------------------------

void cEditorWindowEntitySearch::OnSetActive(bool abX)
{
	iEditorWindowPopUp::OnSetActive(abX);
	
	if(abX)
	{
		mbMustUpdateList = true;
		mbUpdateHighlights = true;

		UpdateFoundEntities();

		mpSet->SetFocusedWidget(mpInputSearch);
		mpInputSearch->SetSelectedText(0);
	}
}

//-------------------------------------------------------------------------------
