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

#include "EditorWindowSelect.h"
#include "EditorEditModeSelect.h"
#include "EditorWorld.h"
#include "EntityWrapper.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEditorWindowSelect::cEditorWindowSelect(cEditorEditModeSelect* apEditMode) : iEditModeWindow(apEditMode)
{
	mToolMode = eSelectToolMode_Translate;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

void cEditorWindowSelect::SetSelectableTools(bool abTranslate, bool abRotate, bool abScale)
{
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
	
	((cEditorEditModeSelect*)mpEditMode)->SetSelectToolMode(mToolMode);
	
	UpdateButtonHighLight();
}

//---------------------------------------------------------------------------

void cEditorWindowSelect::AddFilter(eEditorEntityType aType)
{
	////////////////////////////////////////////////
	// First check if the filter is already added
	std::list<cWidgetButton*>::iterator it = mlstBFilters.begin();
	for(;it!=mlstBFilters.end();++it)
	{
		cWidgetButton* pButton = *it;
		if(pButton->GetUserValue()==aType)
			return;
	}

	// Then add the button if the given type exists in the editor
	iEntityWrapperType* pType = mpEditor->GetEditorWorld()->GetEntityTypeByID(aType);
	if(pType==NULL)
		return;

	const tWString& sTypeName = pType->GetName();
	cWidgetButton* pButton = mpSet->CreateWidgetButton(0, cVector2f(17), cString::SubW(sTypeName,0,2), mpObjectTypeGroup, true);
	pButton->SetUserValue(aType);
	pButton->SetDefaultFontSize(cVector2f(10));
	pButton->SetToolTipEnabled(true);
	pButton->SetToolTip(sTypeName);
	pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(FilterButtonPressed));
	mlstBFilters.push_back(pButton);

	UpdateFilterButtons();
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cEditorWindowSelect::UpdateButtonHighLight()
{
	cColor normalColor = cColor(0,1);
	cColor highlightColor = cColor(1,0,0,1.0f);

	for(int i=eSelectToolMode_Translate;i<3;++i)
	{
		bool bPressed = (i==(int)mToolMode);
		mvButtons[i]->SetPressed(bPressed, false);
	}
}

//---------------------------------------------------------------------------

void cEditorWindowSelect::UpdateFilterButtons()
{
	////////////////////////////////////////////////
	// Position filter buttons according to the order they were added
	cVector3f vPos = cVector3f(10,25,0.1f);
	std::list<cWidgetButton*>::iterator it = mlstBFilters.begin();

	for(;it!=mlstBFilters.end();++it)
	{
		cWidgetButton* pB = *it;
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

bool cEditorWindowSelect::TypeChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowSelect, TypeChange);

//---------------------------------------------------------------------------

bool cEditorWindowSelect::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
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

	UpdateButtonHighLight();

	((cEditorEditModeSelect*)mpEditMode)->SetSelectToolMode(mToolMode);
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowSelect, ButtonPressed);

//---------------------------------------------------------------------------

bool cEditorWindowSelect::FilterButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)mpEditMode;
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
		std::list<cWidgetButton*>::iterator it = mlstBFilters.begin();
		for(;it!=mlstBFilters.end();++it)
		{
			cWidgetButton* pButton = *it;
			int type = pButton->GetUserValue();

			bool bFilterSet = false;
			if(mpCBSelectMultipleTypes->IsChecked())
			{
				if(apWidget==pButton)
					pFilter->SetTypeFilter(type, pButton->IsPressed());
			}
			else
			{
				if(apWidget==pButton)
					bFilterSet = true;
				else
					bFilterSet = false;

				pFilter->SetTypeFilter(type, bFilterSet);
			}
		}
	}

	mpEditor->SetLayoutNeedsUpdate(true);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowSelect, FilterButtonPressed);

//---------------------------------------------------------------------------

void cEditorWindowSelect::OnInitLayout()
{	
	mpBGFrame->SetSize(cVector2f(200,95));
	mpBGFrame->SetClipActive(false);

	mpObjectTypeGroup = mpSet->CreateWidgetGroup(cVector3f(5,8,0.1f), cVector2f(160,80), _W("Select Object Type"), mpBGFrame);
	mpObjectTypeGroup->SetDefaultFontSize(cVector2f(11.5f));

	mpCBSelectAll = mpSet->CreateWidgetCheckBox(cVector3f(10,10,0.1f),0,_W("All"), mpObjectTypeGroup);
	mpCBSelectAll->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(FilterButtonPressed));
	mpCBSelectAll->SetDefaultFontSize(cVector2f(10));

	mpCBSelectMultipleTypes = mpSet->CreateWidgetCheckBox(cVector3f(40,10,0.1f),0,_W("MultiSelect"), mpObjectTypeGroup);
	mpCBSelectMultipleTypes->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(FilterButtonPressed));
	mpCBSelectMultipleTypes->SetDefaultFontSize(cVector2f(10));

    tString sButtonText[] = { "Translate", "Rotate", "Scale" };
	eKey vButtonKey[] = { eKey_Q, eKey_W, eKey_E };

	cVector3f vPos = cVector3f(170,8,0.1f);
	for(int i=0;i<3;++i)
	{
		cGuiGfxElement* pGfx = mpSet->GetGui()->CreateGfxImage("editmode_select_"+cString::ToLowerCase(sButtonText[i])+".tga", eGuiMaterial_Alpha);
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, cVector2f(25),_W(""), mpBGFrame);

		pButton->SetToolTipEnabled(true);
		pButton->SetToolTip(cString::To16Char(sButtonText[i]));
		pButton->SetToggleable(true);
		pButton->SetImage(pGfx);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));
		mvButtons[i] = pButton;
		mvShortcuts[i] = mpSet->AddGlobalShortcut(0, vButtonKey[i], pButton, eGuiMessage_ButtonPressed, false, false);

		vPos.y += 28;
	}

	tIntList& lstTypes = ((cEditorEditModeSelect*)mpEditMode)->GetCurrentFilteredTypes();
	tIntListIt it = lstTypes.begin();
	for(;it!=lstTypes.end();++it)
	{
		AddFilter((eEditorEntityType)*it);
	}
}

//---------------------------------------------------------------------------

void cEditorWindowSelect::OnUpdate(float afTimeStep)
{
	iEditorWindow::OnUpdate(afTimeStep);

	cEditorEditModeSelect* pEditMode = ((cEditorEditModeSelect*)mpEditMode);
	cUIPickFilter* pFilter = pEditMode->GetFilter();

	bool bAllPass = pFilter->GetAllPass();
	mpCBSelectAll->SetChecked(bAllPass, false);

	std::list<cWidgetButton*>::iterator it = mlstBFilters.begin();
	for(;it!=mlstBFilters.end();++it)
	{
		cWidgetButton* pButton = *it;
		pButton->SetPressed(pFilter->GetTypeFilter(pButton->GetUserValue()), false);
		pButton->SetEnabled(bAllPass==false);
	}
}

//---------------------------------------------------------------------------

void cEditorWindowSelect::OnSetActive(bool abX)
{
	for(int i=0;i<3;++i)
		mvShortcuts[i]->SetEnabled(abX);
}

//---------------------------------------------------------------------------
