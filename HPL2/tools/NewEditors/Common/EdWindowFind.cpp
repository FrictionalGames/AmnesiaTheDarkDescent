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

#include "../Common/EdWindowFind.h"

#include "../Common/EdEditModeSelect.h"
#include "../Common/EdWorld.h"
#include "../Common/EdSelection.h"

#include "../Common/Editor.h"
#include "../Common/EdEditModeHandler.h"

#include "../Common/EdScnObject.h"
#include "../Common/EdObjectActions.h"

#include <algorithm>

//-------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

cEdWindowFind::cEdWindowFind(iEditor* apEditor) : iEdPopUp(apEditor, _W("Find Objects"))//apEditor,"Search Window", false, false, 
																			//false, cVector2f(640,410))
{
	mpEditMode = (cEdEditModeSelect*)mpEditor->GetEditModeHandler()->GetMode(_W("Select"));
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


cEdWindowFind::~cEdWindowFind()
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

bool cEdWindowFind::SearchParam_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	OnWorldAddRemObject();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowFind,SearchParam_OnChange);

//-------------------------------------------------------------------------------

bool cEdWindowFind::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	tScnObjList lstObjects;
	for(int i=0;i<mpListFoundEntities->GetMultiSelectionNum();++i)
	{
		int lItemIdx = mpListFoundEntities->GetMultiSelection(i);

		cWidgetItem* pItem = mpListFoundEntities->GetItem(lItemIdx);
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pItem->GetUserData());
		lstObjects.push_back(pObj);
	}

	iEdAction* pAction = NULL;

	if(apWidget==mpListFoundEntities)
	{
		pAction = mpEditMode->GetSelection()->CreateSelectObjectsAction(lstObjects);
	}
	/*
	else if(apWidget == mvButtons[0])
	{
		pAction = mpEditor->CreateFocusOnSelectionAction();
	}
	*/
	else if(apWidget == mvButtons[1])
	{
		pAction = mpEditMode->GetSelection()->CreateDeleteObjectsAction();
	}

	mpEditor->AddAction(pAction);


	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowFind,InputCallback);

//-------------------------------------------------------------------------------

bool cEdWindowFind::StringLocation_OnCheckChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowFind,StringLocation_OnCheckChange);

//-------------------------------------------------------------------------------

void cEdWindowFind::UpdateFoundEntities()
{
	if(mbMustUpdateList==false)
		return;

	mbMustUpdateList = false;

	int lCursorPos = mpListFoundEntities->GetCursorPos();
	mpListFoundEntities->ClearItems();

	tWString sSearchString = cString::ToLowerCaseW(mpInputSearch->GetText());
	int lSearchIn = mpComboBoxInside->GetSelectedItem();

	iEdWorld* pWorld = mpEditor->GetWorld();

	const tEdObjectMap& mapObjects = pWorld->GetObjects();
	tEdObjectMapConstIt it = mapObjects.begin();
	for(;it!=mapObjects.end(); ++it)
	{
		iEdObject* pObj = it->second;
		tWString sObjString;

		switch(lSearchIn)
		{
		case 0:	// Name
			sObjString = pObj->GetName();		break;
		case 1:
			sObjString = pObj->GetTypeName();	break;
		}

		sObjString = cString::ToLowerCaseW(sObjString);
		int lPos = cString::GetFirstStringPosW(sObjString, sSearchString);

		if(sSearchString.empty() ||
			lPos>=0)
		{
			cWidgetItem* pItem = mpListFoundEntities->AddItem();

			pItem->AddProperty(cString::ToStringW(pObj->GetID()));
			pItem->AddProperty(pObj->GetName());
			pItem->AddProperty(pObj->GetTypeName());

			pItem->SetUserData(pObj);
		}
	}

		

	mpListFoundEntities->SetCursorPos(lCursorPos, true, true);
	mbUpdateHighlights=true;
	UpdateItemHighlight();
}

//-------------------------------------------------------------------------------

void cEdWindowFind::UpdateItemHighlight()
{
	if(mbUpdateHighlights==false)
		return;

	mbUpdateHighlights = false;

	mpListFoundEntities->ClearSelection();
	for(int i=0;i<mpListFoundEntities->GetItemNum();++i)
	{
		cWidgetItem* pItem = mpListFoundEntities->GetItem(i);
		iEdScnObject* pObj = static_cast<iEdScnObject*>(pItem->GetUserData());

		pItem->SetSelected(pObj->IsSelected());
	}
}


//-------------------------------------------------------------------------------

void cEdWindowFind::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	SetSize(cVector2f(640, 410));
	GetBG()->SetText(_W("Find Objects"));

	cVector2f vTextSize = cVector2f(11);
	cVector3f vPos = cVector3f(20,35,0.1f);

	/////////////////////////////////////////////
	// Search Text
	mpLabelSearch = mpSet->CreateWidgetLabel(vPos, -1, _W("Search Text"), GetBG());
	mpLabelSearch->SetDefaultFontSize(vTextSize);
	vPos.x += mpLabelSearch->GetSize().x+5;
	mpInputSearch = mpSet->CreateWidgetTextBox(vPos+cVector3f(0,-3,0), cVector2f(200,0), _W(""), GetBG(), eWidgetTextBoxInputType_Normal);
	mpInputSearch->AddCallback(eGuiMessage_TextChange, this, kGuiCallback(SearchParam_OnChange));
	mpInputSearch->SetDefaultFontSize(vTextSize);

	vPos.x += mpInputSearch->GetSize().x + 15;

	/////////////////////////////////////////////
	// Text filter
	mpLabelInside = mpSet->CreateWidgetLabel(vPos, -1, _W("In"), GetBG());
	mpLabelInside->SetDefaultFontSize(vTextSize);
	mpComboBoxInside = mpSet->CreateWidgetComboBox(vPos+cVector3f(mpLabelInside->GetSize().x+5,-3,0), cVector2f(100,0), _W(""), GetBG());
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
	mpListFoundEntities = mpSet->CreateWidgetMultiPropertyListBox(vPos, cVector2f(600,300), GetBG());
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
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos+cVector3f(i*60.0f,0,0),cVector2f(50,20),vButtonStrings[i], GetBG());

		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));
		mvButtons[i] = pButton;
	}
}

//-------------------------------------------------------------------------------

void cEdWindowFind::OnUpdate()
{
	UpdateFoundEntities();
	UpdateItemHighlight();
}

//-------------------------------------------------------------------------------

void cEdWindowFind::OnWorldAddRemObject()
{
	mbMustUpdateList = true;
	SetUpdated();
}

//-------------------------------------------------------------------------------

void cEdWindowFind::OnWorldObjModify()
{
	OnWorldAddRemObject();
}

//-------------------------------------------------------------------------------

void cEdWindowFind::OnWorldSelectChange()
{
	mbUpdateHighlights = true;
	SetUpdated();
}

//-------------------------------------------------------------------------------

void cEdWindowFind::OnSetActive(bool abX)
{
	iEdPopUp::OnSetActive(abX);
	
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
