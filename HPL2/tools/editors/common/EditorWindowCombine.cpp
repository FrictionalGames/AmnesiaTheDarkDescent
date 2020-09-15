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

#include "EditorWindowCombine.h"
#include "EditorEditModeCombine.h"
#include "../leveleditor/LevelEditorWorld.h"
#include "../common/EntityWrapper.h"

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEditorWindowCombine::cEditorWindowCombine(iEditorEditMode* apEditMode) : iEditModeWindow(apEditMode)
{
	mbAddedNewCombo = false;
}


//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cLevelEditorStaticObjectCombo* cEditorWindowCombine::GetCurrentCombo()
{
	cEditorEditModeCombine* pEditMode = (cEditorEditModeCombine*)mpEditMode;
	int lSelectedItem = mpInpCombinations->GetValue();
	int lComboID = -1;
	if(lSelectedItem>=0 && lSelectedItem<(int)mvComboIDs.size())
		lComboID = mvComboIDs[lSelectedItem];
		
	return pEditMode->GetCombo(lComboID);
}

//-------------------------------------------------------------

eCombinePickMode cEditorWindowCombine::GetPickMode()
{
	for(int i=0;i<3;++i)
		if(mvInpPickModes[i]->GetValue()==true)
			return (eCombinePickMode)i;
	
	return eCombinePickMode_Add;
}


//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEditorWindowCombine::OnInitLayout()
{
	////////////////////////////////////////////
	// Create layout
	mpBGFrame->SetSize(cVector2f(200,600));
	mpBGFrame->SetClipActive(false);

	cVector3f vPos = cVector3f(10, 10, 0.1f);

	/////////////////////////////////////////////
	// CheckBox to set if all combos should be displayed
	mpInpShowAll = CreateInputBool(vPos, _W("Show all combos"), "", mpBGFrame);

	vPos.y += mpInpShowAll->GetSize().y + 5;

	/////////////////////////////////////////////
	// ComboBox to select current combination
	mpInpCombinations = CreateInputEnum(vPos, _W("Combinations"), "", tWStringList(), mpBGFrame);

	vPos.y += 11;
	vPos.x += mpInpCombinations->GetSize().x + 5;

	// Add / remove combo buttons
	mpBAddComb = mpSet->CreateWidgetButton(vPos, cVector2f(40,20), _W("Add"), mpBGFrame);
	mpBAddComb->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));

	mpBRemComb = mpSet->CreateWidgetButton(vPos + cVector3f(42,0,0), cVector2f(40,20), _W("Rem"), mpBGFrame);
	mpBRemComb->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));

	vPos.x = 10;
	vPos.y += mpInpCombinations->GetSize().y + 10;

	// Add / Remove object to combo operation mode
	tWString vLabels[] = { _W("Add"), _W("Remove"), _W("Toggle") };
	for(int i=0;i<3;++i)
	{
		mvInpPickModes[i] = CreateInputBool(vPos, vLabels[i], "", mpBGFrame);
		vPos.y += mvInpPickModes[i]->GetSize().y + 4;
	}
	mvInpPickModes[0]->SetValue(true, false);

	vPos.y += 6;
    
	// Combo color
	mpInpCombColor = CreateInputColorFrame(vPos, _W("Color"), "", mpBGFrame);

	vPos.y += mpInpCombColor->GetSize().y + 10;

	// List all static objects combined here
	mpLBStaticObjects = mpSet->CreateWidgetListBox(vPos, cVector2f(160, 300), mpBGFrame);
}

//-------------------------------------------------------------

void cEditorWindowCombine::OnUpdate(float afTimeStep)
{
	//////////////////////////////////////////
	// Update combinations list
	int lCurrentCombo = mpInpCombinations->GetValue();

	mpInpCombinations->ClearValues();
	mvComboIDs.clear();

	cEditorEditModeCombine* pEditMode = (cEditorEditModeCombine*)mpEditMode;
	tStaticObjectComboList lstCombos = pEditMode->GetCombos();
	tStaticObjectComboList::const_iterator itCombos = lstCombos.begin();
	for(;itCombos!=lstCombos.end();++itCombos)
	{
		cLevelEditorStaticObjectCombo* pCombo = *itCombos;

		int lID = pCombo->GetID();
		mvComboIDs.push_back(lID);
		mpInpCombinations->AddValue(_W("Combo_") + cString::ToStringW(lID, 2));
	}

	if(mbAddedNewCombo)
	{
		mbAddedNewCombo = false;
		lCurrentCombo = mpInpCombinations->GetNumValues()-1;
	}

	////////////////////////////////////////////
	// Doing this will update rest of inputs
	mpInpCombinations->SetValue(lCurrentCombo);
}

//-------------------------------------------------------------

bool cEditorWindowCombine::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEditorEditModeCombine* pEditMode = (cEditorEditModeCombine*)mpEditMode;
	if(apWidget==mpBAddComb)
	{
		pEditMode->AddCombo();
		mbAddedNewCombo = true;
	}
	else if(apWidget==mpBRemComb)
	{
		pEditMode->RemoveCombo(mvComboIDs[mpInpCombinations->GetValue()]);
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowCombine, ButtonPressed);

//-------------------------------------------------------------

bool cEditorWindowCombine::WindowSpecificInputCallback(iEditorInput* apInput)
{
	cLevelEditorStaticObjectCombo* pCombo = GetCurrentCombo();

	if(apInput==mpInpCombinations)
	{
		bool bComboNotNull = (pCombo!=NULL);

		mpInpCombColor->GetHandle()->SetEnabled(bComboNotNull);
		mpLBStaticObjects->SetEnabled(bComboNotNull);

		mpLBStaticObjects->ClearItems();

		if(bComboNotNull)
		{
			mpInpCombColor->SetValue(pCombo->GetColor(), false);

			const tEntityWrapperList& lstObjects = pCombo->GetObjects();
			tEntityWrapperList::const_iterator itObjects = lstObjects.begin();

			for(;itObjects!=lstObjects.end();++itObjects)
			{
				iEntityWrapper* pEnt = *itObjects;

				cWidgetItem* pItem = mpLBStaticObjects->AddItem(pEnt->GetName());
				pItem->SetUserData(pEnt);
			}
		}
		else
		{
			mpInpCombColor->SetValue(cColor(0,1), false);
		}
	}
	else if(apInput==mpInpCombColor)
	{
		if(pCombo!=NULL)
		{
			mpEditor->AddAction(pCombo->CreateActionSetColor(mpInpCombColor->GetValue()));
		}
	}
	else if(apInput==mpInpShowAll)
	{
	}
	else
	{
		for(int i=0;i<(int)3;++i)
		{
			if(apInput!=mvInpPickModes[i])
				mvInpPickModes[i]->SetValue(false, false);
			else
				mvInpPickModes[i]->SetValue(true, false);
		}
	}

	return true;
}

//-------------------------------------------------------------
