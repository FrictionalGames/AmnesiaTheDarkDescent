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

#include "../Common/EdWindowCombine.h"
#include "../Common/EdEditModeCombine.h"
#include "../LevelEditor/HPLMap.h"
#include "../Common/Editor.h"
//#include "../common/EntityWrapper.h"

//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEdWindowCombine::cEdWindowCombine(iEdEditMode* apMode) : iEdEditModePane(apMode)
{
	mbAddedNewCombo = false;
}


//-------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------

cLevelEditorStaticObjectCombo* cEdWindowCombine::GetCurrentCombo()
{
	return mpSelectedCombo;
	cEdEditModeCombine* pEditMode = static_cast<cEdEditModeCombine*>(mpEditMode);
	int lSelectedItem = mpInpCombinations->GetValue();
	int lComboID = -1;
	if(lSelectedItem>=0 && lSelectedItem<(int)mvComboIDs.size())
		lComboID = mvComboIDs[lSelectedItem];
		
	return pEditMode->GetCombo(lComboID);
}

//-------------------------------------------------------------

eCombinePickMode cEdWindowCombine::GetPickMode()
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

void cEdWindowCombine::OnCreateLayout()
{
	iEdPane::OnCreateLayout();
	SetHeight(600);
	////////////////////////////////////////////
	// Create layout
	
	cVector3f vPos = cVector3f(10, 10, 0.1f);

	/////////////////////////////////////////////
	// CheckBox to set if all combos should be displayed
	mpInpShowAll = CreateInputBool(vPos, _W("Show all combos"),  GetBG());

	vPos.y += mpInpShowAll->GetSize().y + 5;

	/////////////////////////////////////////////
	// ComboBox to select current combination
	mpInpCombinations = CreateInputEnum(vPos, _W("Combinations"), GetBG());

	vPos.y += 11;
	vPos.x += mpInpCombinations->GetSize().x + 5;

	// Add / remove combo buttons
	mpBAddComb = mpSet->CreateWidgetButton(vPos, cVector2f(40,20), _W("Add"), GetBG());
	mpBAddComb->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));

	mpBRemComb = mpSet->CreateWidgetButton(vPos + cVector3f(42,0,0), cVector2f(40,20), _W("Rem"), GetBG());
	mpBRemComb->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPressed));

	vPos.x = 10;
	vPos.y += mpInpCombinations->GetSize().y + 10;

	// Add / Remove object to combo operation mode
	tWString vLabels[] = { _W("Add"), _W("Remove"), _W("Toggle") };
	for(int i=0;i<3;++i)
	{
		mvInpPickModes[i] = CreateInputBool(vPos, vLabels[i],  GetBG());
		vPos.y += mvInpPickModes[i]->GetSize().y + 4;
	}
	mvInpPickModes[0]->SetValue(true, false);

	vPos.y += 6;
    
	// Combo color
	mpInpCombColor = CreateInputColor(vPos, _W("Color"),  GetBG());

	vPos.y += mpInpCombColor->GetSize().y + 10;

	// List all static objects combined here
	mpLBStaticObjects = mpSet->CreateWidgetListBox(vPos, cVector2f(160, 300), GetBG());
}

//-------------------------------------------------------------

void cEdWindowCombine::OnUpdate()
{
	//////////////////////////////////////////
	// Update combinations list
	int lCurrentCombo = mpInpCombinations->GetValue();

	mpInpCombinations->ClearValues();
	mvComboIDs.clear();

	cEdEditModeCombine* pEditMode = (cEdEditModeCombine*)mpEditMode;
	tStaticObjectComboList lstCombos = pEditMode->GetCombos();
	tStaticObjectComboList::const_iterator itCombos = lstCombos.begin();
	for(;itCombos!=lstCombos.end();++itCombos)
	{
		cLevelEditorStaticObjectCombo* pCombo = *itCombos;

		if(pCombo==mpSelectedCombo)
			lCurrentCombo = mpInpCombinations->GetNumValues();

		int lID = pCombo->GetID();
		mpInpCombinations->AddValue(_W("Combo_") + cString::ToStringW(lID, 2), pCombo);
	}

	if(mbAddedNewCombo)
	{
		mbAddedNewCombo = false;
		//lCurrentCombo = mpInpCombinations->GetNumValues()-1;
	}

	if(lCurrentCombo>=mpInpCombinations->GetNumValues())
		lCurrentCombo = mpInpCombinations->GetNumValues()-1;

	////////////////////////////////////////////
	// Doing this will update rest of inputs
	mpInpCombinations->SetValue(lCurrentCombo);
}

//-------------------------------------------------------------

bool cEdWindowCombine::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdEditModeCombine* pEditMode = static_cast<cEdEditModeCombine*>(mpEditMode);
	if(apWidget==mpBAddComb)
	{
		pEditMode->AddCombo();
		mbAddedNewCombo = true;

		mpSelectedCombo = static_cast<cHplMap*>(mpEditor->GetWorld())->GetLastAddedCombo();
	}
	else if(apWidget==mpBRemComb)
	{
		int lSelectedID = mpInpCombinations->GetValue()-1;
		pEditMode->RemoveCombo(mpSelectedCombo->GetID());
	}

	SetUpdated();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowCombine, ButtonPressed);

//-------------------------------------------------------------

bool cEdWindowCombine::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpCombinations)
	{
		mpSelectedCombo = static_cast<cLevelEditorStaticObjectCombo*>(mpInpCombinations->GetSelectedUserData());
		bool bComboNotNull = (mpSelectedCombo!=NULL);

		mpInpCombColor->GetHandle()->SetEnabled(bComboNotNull);
		mpLBStaticObjects->SetEnabled(bComboNotNull);

		mpLBStaticObjects->ClearItems();

		if(bComboNotNull)
		{
			mpInpCombColor->SetValue(mpSelectedCombo->GetColor(), false);

			const tScnObjList& lstObjects = mpSelectedCombo->GetObjects();
			tScnObjList::const_iterator itObjects = lstObjects.begin();

			for(;itObjects!=lstObjects.end();++itObjects)
			{
				iEdScnObject* pEnt = *itObjects;

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
		if(mpSelectedCombo!=NULL)
		{
			mpEditor->AddAction(mpSelectedCombo->CreateActionSetColor(mpInpCombColor->GetValue()));
		}
	}
	else if(apInput==mpInpShowAll)
	{
	}
	else
	{
		for(int i=0;i<3;++i)
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
