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

#include "../Common/EdWindowSelect.h"

#include "../Common/EdSelection.h"

#include "../Common/Editor.h"
#include "../Common/EdEditModeSelect.h"
#include "../Common/EdWorld.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdScnObjEditPane.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdWindowSelect::cEdWindowSelect(iEdEditMode* apMode) : iEdEditModePane(apMode)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdWindowSelect::PositionEditPane(iEdScnObjEditPane* apPane)
{
	if(apPane==NULL) return;

	iWidget* pBG = GetBG();
	const cVector3f& vGlobalPos = pBG->GetGlobalPosition();
	const cVector2f& vSize = pBG->GetSize();
	//pBG->AttachChild(apPane->GetBG()); // TODO: RESEARCH WHY THIS DOES NOT WORK!!
	apPane->GetBG()->SetGlobalPosition(vGlobalPos + cVector3f(0, vSize.y, 5));
	apPane->SetWidth(vSize.x);
	apPane->SetHeight(mpSet->GetVirtualSize().y-vSize.y);
}

//---------------------------------------------------------------------------
/*
void cEdWindowSelect::SetSelectableTools(bool abTranslate, bool abRotate, bool abScale)
{
	/*
	mvButtons[0]->SetEnabled(abTranslate);
	mvButtons[1]->SetEnabled(abRotate);
	mvButtons[2]->SetEnabled(abScale);
	
	eSelectToolMode oldMode = mToolMode;
	if(mToolMode==eSelectToolMode_LastEnum)
		mToolMode = eSelectToolMode_Translate;

	if(abRotate==false)
	{
		if(mToolMode==eSelectToolMode_Rotate)
			mToolMode = eSelectToolMode_Translate;
	}
	if(abScale==false)
	{
		if(mToolMode==eSelectToolMode_Scale)
			mToolMode = eSelectToolMode_Translate;
	}
	if(abTranslate==false)
		if(mToolMode==eSelectToolMode_Translate)
			mToolMode = eSelectToolMode_LastEnum;
	
	//static_cast<cEdEditModeSelect*>(mpEditMode)->SetSelectToolMode(mToolMode);
	
	UpdateButtonHighLight();
	
}
*/

//---------------------------------------------------------------------------

void cEdWindowSelect::AddFilter(iEdObjectType* apType)
{
	if(apType==NULL)
		return;

	////////////////////////////////////////////////
	// First check if the filter is already added
	tWidgetListIt it = mlstBFilters.begin();
	for(;it!=mlstBFilters.end();++it)
	{
		iWidget* pButton = *it;
		if(pButton->GetUserData()==apType)
			return;
		//if(pButton->GetUserValue()==apType->GetTopID())
		//	return;
	}

	cVector2f vButtonSize = cVector2f(17);
	cWidgetButton* pButton = mpSet->CreateWidgetButton(0, vButtonSize, _W(""), mpObjectTypeGroup, true);
	pButton->SetUserData(apType);
	//pButton->SetUserValue(apType->GetTopID());
	pButton->SetDefaultFontSize(cVector2f(10));

	pButton->SetToolTipEnabled(true);
	pButton->SetToolTip(apType->GetName());

	pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(FilterButtonPressed));
	mlstBFilters.push_back(pButton);

	UpdateFilterButtons();
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEdWindowSelect::UpdateButtonHighLight()
{
	cColor normalColor = cColor(0,1);
	cColor highlightColor = cColor(1,0,0,1.0f);

	/*
	for(int i=eSelectToolMode_Translate;i<3;++i)
	{
		bool bPressed = (i==(int)mToolMode);
		mvButtons[i]->SetPressed(bPressed, false);
	}
	*/
}

//---------------------------------------------------------------------------

void cEdWindowSelect::UpdateFilterButtons()
{
	////////////////////////////////////////////////
	// Position filter buttons according to the order they were added
	cVector3f vPos = cVector3f(10,25,0.1f);
	tWidgetListIt it = mlstBFilters.begin();

	for(;it!=mlstBFilters.end();++it)
	{
		cWidgetButton* pB = static_cast<cWidgetButton*>(*it);
		if(pB==NULL) continue;

		pB->SetPosition(vPos);

		float fButtonWidth = pB->GetSize().x;
		if(vPos.x+24+fButtonWidth < mpObjectTypeGroup->GetSize().x)
		{
            vPos.x += pB->GetSize().x + 4;
		}
		else
		{
			vPos.x = 10;
			vPos.y += pB->GetSize().y + 4;
		}
	}
}

//---------------------------------------------------------------------------

bool cEdWindowSelect::TypeChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowSelect, TypeChange);

//---------------------------------------------------------------------------

bool cEdWindowSelect::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iScnObjManipulator* pManipulator = static_cast<iScnObjManipulator*>(apWidget->GetUserData());

	//static_cast<cEdEditModeSelect*>(mpEditMode);
	/*
	if(apWidget==mvButtons[0])
	{
		mToolMode = eSelectToolMode_Translate;
	}
	else if(apWidget==mvButtons[1])
	{
		mToolMode = eSelectToolMode_Rotate;
	}
	else if(apWidget==mvButtons[2])
	{
		mToolMode = eSelectToolMode_Scale;
	}
	*/

	//UpdateButtonHighLight();

	static_cast<cEdEditModeSelect*>(mpEditMode)->SetCurrentManipulator(pManipulator);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowSelect, ButtonPressed);

//---------------------------------------------------------------------------

bool cEdWindowSelect::FilterButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdEditModeSelect* pEditMode = static_cast<cEdEditModeSelect*>(mpEditMode);
	cUIPickFilter* pFilter = pEditMode->GetFilter();

	if(apWidget==mpCBSelectAll)
	{
		bool bChecked = mpCBSelectAll->IsChecked();
		pFilter->SetAllPass(mpCBSelectAll->IsChecked());

		if(bChecked)
			mpCBSelectMultipleTypes->SetChecked(false);
	}
	else if(apWidget==mpCBSelectMultipleTypes)
	{
		if(mpCBSelectMultipleTypes->IsChecked())
			mpCBSelectAll->SetChecked(false);
	}
	else
	{
		tWidgetListIt it = mlstBFilters.begin();
		for(;it!=mlstBFilters.end();++it)
		{
			cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
			iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());
			
			bool bFilterSet = false;
			if(mpCBSelectMultipleTypes->IsChecked())
			{
				if(apWidget==pButton)
					pFilter->SetTypeFilter(pType, pButton->IsPressed());
			}
			else
			{
				if(apWidget==pButton)
					bFilterSet = true;
				else
					bFilterSet = false;

				pFilter->SetTypeFilter(pType, bFilterSet);
			}
		}
	}

	SetUpdated();

	//mpEditor->SetLayoutNeedsUpdate(true);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowSelect, FilterButtonPressed);

//---------------------------------------------------------------------------

void cEdWindowSelect::OnCreateLayout()
{	
	iEdPane::OnCreateLayout();
	SetHeight(95.0f);

	mpObjectTypeGroup = mpSet->CreateWidgetGroup(cVector3f(5,8,0.1f), cVector2f(160,80), _W("Select Object Type"), mpBGWidget);
	mpObjectTypeGroup->SetDefaultFontSize(cVector2f(11.5f));

	mpCBSelectAll = mpSet->CreateWidgetCheckBox(cVector3f(10,10,0.1f),0,_W("All"), mpObjectTypeGroup);
	mpCBSelectAll->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(FilterButtonPressed));
	mpCBSelectAll->SetDefaultFontSize(cVector2f(10));

	mpCBSelectMultipleTypes = mpSet->CreateWidgetCheckBox(cVector3f(40,10,0.1f),0,_W("MultiSelect"), mpObjectTypeGroup);
	mpCBSelectMultipleTypes->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(FilterButtonPressed));
	mpCBSelectMultipleTypes->SetDefaultFontSize(cVector2f(10));

    //tString sButtonText[] = { "Translate", "Rotate", "Scale" };
	//eKey vButtonKey[] = { eKey_Q, eKey_W, eKey_E };

	//cVector3f vPos = cVector3f(170,8,0.1f);
	//for(int i=0;i<3;++i)
	/*
	{
		
		cGuiGfxElement* pGfx = mpSet->GetGui()->CreateGfxImage("editmode_select_"+cString::ToLowerCase(sButtonText[i])+".tga", eGuiMaterial_Alpha);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, cVector2f(25),_W(""), mpBGWidget);

		pButton->SetToolTipEnabled(true);
		pButton->SetToolTip(cString::To16Char(sButtonText[i]));
		pButton->SetToggleable(true);
		pButton->SetImage(pGfx);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));
		mvButtons[i] = pButton;
		AddShortcut(mpSet->AddGlobalShortcut(0, vButtonKey[i], pButton, eGuiMessage_ButtonPressed, false, false));

		vPos.y += 28;
	}
	*/


	iEdObjectType* pWorldRoot = mpEditor->GetWorld()->GetRootType();
	for(int i=0; i<pWorldRoot->GetNumChildren(); ++i)
	{
		iEdObjectType* pType = pWorldRoot->GetChild(i);
		AddFilter(pType);
	}
}

//---------------------------------------------------------------------------

void cEdWindowSelect::OnUpdate()
{
	iEdEditModePane::OnUpdate();

	cEdEditModeSelect* pEditMode = static_cast<cEdEditModeSelect*>(mpEditMode);
	cUIPickFilter* pFilter = pEditMode->GetFilter();

	bool bAllPass = pFilter->GetAllPass();
	mpCBSelectAll->SetChecked(bAllPass, false);

	pFilter->SetAllPass(false);

	tWidgetListIt it = mlstBFilters.begin();
	for(;it!=mlstBFilters.end();++it)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
		iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());
		pButton->SetPressed(pFilter->GetTypeFilter(pType), false);
		pButton->SetEnabled(bAllPass==false);
	}

	pFilter->SetAllPass(bAllPass);
}

//---------------------------------------------------------------------------
