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

#include "ModelEditorWindowUserSettings.h"
#include "ModelEditor.h"
#include "ModelEditorWorld.h"
#include "../common/EditorUserClassDefinitionManager.h"

#include <algorithm>

//-------------------------------------------------------------------
//-------------------------------------------------------------------

cModelEditorWindowUserSettings::cModelEditorWindowUserSettings(cModelEditor* apEditor) : iEditorWindowPopUp(apEditor, "User Settings Window", true, true, false, cVector2f(700,500))
{
	mpEditor = apEditor;
	mpInputPanel = NULL;
}

//-------------------------------------------------------------------

cModelEditorWindowUserSettings::~cModelEditorWindowUserSettings()
{
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::OnSetActive(bool abX)
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// TODO: The var loading definitely can be done in a better way, check when there's enough time
	iEditorWindowPopUp::OnSetActive(abX);


	if(abX)
	{
		////////////////////////////////////////////////////////////////////
		// Sets up custom variables according to the type and subtype
		cModelEditorWorld* pEntity = (cModelEditorWorld*)mpEditor->GetEditorWorld();
		cEditorClassInstance* pClass = pEntity->GetClass();
		if(pClass)
		{
			cEditorUserClassSubType* pType = (cEditorUserClassSubType*)pClass->GetClass();
            mpComboBoxType->SetSelectedItem(pType->GetParent()->GetIndex());
			mpComboBoxSubType->SetSelectedItem(pType->GetIndex());
		}
		else
		{
			mpComboBoxType->SetSelectedItem(0);
			mpEditor->ShowMessageBox(_W("Warning"), _W("Model has no type, falling back to defaults"), _W("OK"), _W(""), NULL, NULL);
		}
	}
	else
	{
	}
}

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::OnInitLayout()
{
	iEditorWindowPopUp::OnInitLayout();
	mpWindow->SetText(_W("User Defined Variables"));

	cVector3f vPos = cVector3f(25,30,0.1f);

	mpLabelType = mpSet->CreateWidgetLabel(vPos,0, _W("Type"), mpWindow);

	vPos.x += 50;
	// Type ComboBox
	mpComboBoxType = mpSet->CreateWidgetComboBox(vPos, cVector2f(150,25), _W(""), mpWindow);

	mpComboBoxType->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(TypeList_OnChange));

	vPos.x += 160;

	mpLabelSubType = mpSet->CreateWidgetLabel(vPos,0,_W("SubType"), mpWindow);


	vPos.x += 70;

	mpComboBoxSubType = mpSet->CreateWidgetComboBox(vPos, cVector2f(150,25), _W(""), mpWindow);

	mpComboBoxSubType->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(SubTypeList_OnChange));

	vPos.x = 15;
	vPos.y += 35; 

	mpFrameVars = mpSet->CreateWidgetFrame(cVector3f(mpWindow->GetSize().x*0.025f, vPos.y, vPos.z), cVector2f(mpWindow->GetSize().x*0.95f,mpWindow->GetSize().y*0.8f), false, mpWindow, true, true);


	vPos.x = mpFrameVars->GetSize().x*0.5f-72.5f;
	vPos.y += mpFrameVars->GetSize().y + 5;

	PopulateTypeList();
}

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::OnUpdate(float afTimeStep)
{
}

//-------------------------------------------------------------------

bool cModelEditorWindowUserSettings::TypeList_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	PopulateSubTypeList();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowUserSettings,TypeList_OnChange);

//-------------------------------------------------------------------

bool cModelEditorWindowUserSettings::SubTypeList_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lTypeIdx = mpComboBoxType->GetSelectedItem();
	int lSubTypeIdx = mpComboBoxSubType->GetSelectedItem();

	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);	
	cEditorUserClassSubType* pClass = pDef->GetType(lTypeIdx)->GetSubType(lSubTypeIdx);

	if(pClass==NULL)
		return true;

	cModelEditorWorld* pEntity = (cModelEditorWorld*)mpEditor->GetEditorWorld();
	pEntity->SetType(pClass);

	PopulateVarList();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowUserSettings,SubTypeList_OnChange);

//-------------------------------------------------------------------

bool cModelEditorWindowUserSettings::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mvButtons[0])
	{
	}
	else if(apWidget==mvButtons[1])
	{
	}

	SetActive(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditorWindowUserSettings,Button_OnPressed);

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::PopulateTypeList()
{
	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);

	mpComboBoxType->ClearItems();

	for(int i=0;i<pDef->GetTypeNum();++i)
	{
		cEditorUserClassType* pType = pDef->GetType(i);
		mpComboBoxType->AddItem(pType->GetName());
	}
}


//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::PopulateSubTypeList()
{
	mpLabelSubType->SetEnabled(true);
	mpComboBoxSubType->SetEnabled(true);

	mpComboBoxSubType->ClearItems();

	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);
	cEditorUserClassType* pType = pDef->GetType(mpComboBoxType->GetSelectedItem());
	for(int i=0;i<pType->GetSubTypeNum();++i)
	{
		cEditorUserClassSubType* pSubType = pType->GetSubType(i);
		mpComboBoxSubType->AddItem(pSubType->GetName());
	}
	bool bEnabled = (mpComboBoxSubType->GetItemNum()>1);

	mpComboBoxSubType->SetSelectedItem(pType->GetDefaultSubTypeIndex());
	
	mpLabelSubType->SetEnabled(bEnabled);
	mpComboBoxSubType->SetEnabled(bEnabled);
}

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::PopulateVarList()
{
	cModelEditorWorld* pEntity = (cModelEditorWorld*)mpEditor->GetEditorWorld();
	cEditorClassInstance* pClass = pEntity->GetClass();

	if(mpInputPanel)
		hplDelete(mpInputPanel);

	if(pClass)
	{
		mpInputPanel = pClass->CreateInputPanel(this, mpFrameVars, false);
		mpInputPanel->Update();
	}
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::OrganizeVarInputs()
{
}

//-------------------------------------------------------------------

void cModelEditorWindowUserSettings::SetTypeSubTypeCombo(const tString& asType, const tString& asSubType)
{
}

//-------------------------------------------------------------------
