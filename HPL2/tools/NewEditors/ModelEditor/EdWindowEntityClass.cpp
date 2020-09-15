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

#include "../ModelEditor/EdWindowEntityClass.h"

#include "../Common/EdUserClassDefinition.h"
#include "../Common/Editor.h"
#include "../ModelEditor/HplEntity.h"

#include <algorithm>

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// ENTITY CLASS WINDOW - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

static bool VarInputValueEnterCallback(void*, iEdVarInput*);

//-------------------------------------------------------------------

cEdWindowEntityClass::cEdWindowEntityClass(iEditor* apEditor) : iEdPopUp(apEditor, _W("EntityClass"))//, true, true, false, cVector2f(700,500))
{
	mpInputPanel = NULL;
	mpBackupClass = NULL;

	mbTypeChanged = false;
}

//-------------------------------------------------------------------

cEdWindowEntityClass::~cEdWindowEntityClass()
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// ENTITY CLASS WINDOW - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEdWindowEntityClass::OnSetActive(bool abX)
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// TODO: The var loading definitely can be done in a better way, check when there's enough time
	iEdPopUp::OnSetActive(abX);
	if(abX)
	{
		mmapBackupValues.clear();
		////////////////////////////////////////////////////////////////////
		// Sets up custom variables according to the type
		cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
		cEdClassInstance* pEntClassInstance = pEnt->GetClassInstance();

		mpBackupClass = static_cast<cEdUserClass*>(pEntClassInstance->GetClass());
		pEntClassInstance->SaveValuesToMap(mmapBackupValues);

		mbTypeChanged = true;
	}
}

//-------------------------------------------------------------------

void cEdWindowEntityClass::OnCreateLayout()
{
	iEdPopUp::OnCreateLayout();
	SetSize(cVector2f(640, 480));
	//mpWindow->SetText(_W("User Defined Variables"));

	const cVector2f& vSize = GetBG()->GetSize();

	cVector3f vPos = cVector3f(25,30,0.1f);

	mpInpType = CreateInputEnum(vPos, _W("Class"), GetBG(), 350);

	vPos.y += 35; 

	mpFrameVars = mpSet->CreateWidgetFrame(cVector3f(vSize.x*0.025f, vPos.y, vPos.z), cVector2f(vSize.x*0.95f,vSize.y*0.8f), false, GetBG(), true, true);

	vPos.x = mpFrameVars->GetSize().x*0.5f-72.5f;
	vPos.y += mpFrameVars->GetSize().y + 5;

	PopulateTypes();
}

//-------------------------------------------------------------------

void cEdWindowEntityClass::OnUpdate()
{
	iEdPopUp::OnUpdate();
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	cEdUserClass* pClass = static_cast<cEdUserClass*>(pEnt->GetClassInstance()->GetClass());
	
	mpInpType->SetValue(pClass->GetFullNameNoBaseClass(), false);

	if(mbTypeChanged)
	{
		mbTypeChanged = false;
		PopulateVariables();
	}
	if(mpInputPanel) mpInputPanel->Update();
}

//-------------------------------------------------------------------

void cEdWindowEntityClass::OnActionExec()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	cEdUserClass* pOldClass = static_cast<cEdUserClass*>(pEnt->GetClassInstance()->GetClass());
	cEdUserClass* pClass = static_cast<cEdUserClass*>(mpInpType->GetSelectedUserData());
	if(pClass!=pOldClass)
	{
		mbTypeChanged = true;
	}

	SetUpdated();
}

void cEdWindowEntityClass::OnWorldParamsChange()
{
	SetUpdated();
}

//-------------------------------------------------------------------

bool cEdWindowEntityClass::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpType)
	{
		cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
		cEdUserClass* pOldClass = static_cast<cEdUserClass*>(pEnt->GetClassInstance()->GetClass());
		cEdUserClass* pClass = static_cast<cEdUserClass*>(mpInpType->GetSelectedUserData());
        if(pClass!=pOldClass)
		{
			mpEditor->AddAction(pEnt->CreateSetClassAction(pClass));
			mbTypeChanged = true;
		}
	}
	else
		return iEdPopUp::WindowSpecificInputCallback(apInput);

	return true;
}

//-------------------------------------------------------------------

/*
bool cEdWindowEntityClass::SubTypeList_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lTypeIdx = mpComboBoxType->GetSelectedItem();
	int lSubTypeIdx = mpComboBoxSubType->GetSelectedItem();

	cEditorUserClassDefinition* pDef = mpEditor->GetClassDefinitionManager()->GetDefinition(eUserClassDefinition_Entity);	
	cEditorUserClassSubType* pClass = pDef->GetType(lTypeIdx)->GetSubType(lSubTypeIdx);

	if(pClass==NULL)
		return true;

	cModelEditorWorld* pEntity = (cModelEditorWorld*)mpEditor->GetWorld();
	pEntity->SetType(pClass);

	PopulateVarList();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowEntityClass,SubTypeList_OnChange);
*/

//-------------------------------------------------------------------

bool cEdWindowEntityClass::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
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
kGuiCallbackDeclaredFuncEnd(cEdWindowEntityClass,Button_OnPressed);

//-------------------------------------------------------------------

void cEdWindowEntityClass::PopulateTypes()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	cEdUserClassDefinition* pDef = pEnt->GetClassDefinition();

	mpInpType->ClearValues();
	for(int i=0; i<pDef->GetNumClasses(); ++i)
	{
		cEdUserClass* pClass = static_cast<cEdUserClass*>(pDef->GetClass(i));

		PopulateTypesHelper(pClass);
	}
}

//-------------------------------------------------------------------

void cEdWindowEntityClass::PopulateTypesHelper(cEdUserClass* apClass)
{
	int lChildren = apClass->GetNumChildren();

	if(lChildren==0)
		mpInpType->AddValue(apClass->GetFullNameNoBaseClass(), apClass);
	else
	{
		for(int i=0; i<apClass->GetNumChildren(); ++i)
		{
			cEdUserClass* pClass = static_cast<cEdUserClass*>(apClass->GetChild(i));
			PopulateTypesHelper(pClass);
		}
	}
}

//-------------------------------------------------------------------

void cEdWindowEntityClass::PopulateVariables()
{
	cHplEntity* pEnt = static_cast<cHplEntity*>(mpEditor->GetWorld());
	cEdClassInstance* pClassInstance = pEnt->GetClassInstance();

	if(mpInputPanel)
		hplDelete(mpInputPanel);

	mpInputPanel = pClassInstance->CreateInputPanel(this, mpFrameVars, false);
	if(mpInputPanel)
	{
		mpInputPanel->SetCallback(this, VarInputValueEnterCallback);
	}
}

//-------------------------------------------------------------------

bool VarInputValueEnterCallback(void* apObj, iEdVarInput* apInput)
{
	cEdWindowEntityClass* pWindow = static_cast<cEdWindowEntityClass*>(apObj);
	iEditor* pEditor = pWindow->GetEditor();

	cHplEntity* pEnt = static_cast<cHplEntity*>(pEditor->GetWorld());

	const tWString& sVarName = apInput->GetVar()->GetName();
	const tWString& sVarValue = apInput->GetInput()->GetValue();

	pEditor->AddAction(pEnt->CreateSetVariableValueAction(sVarName, sVarValue));

	return true;
}

//-------------------------------------------------------------------

/*
void cEdWindowEntityClass::OrganizeVarInputs()
{
}
*/


//-------------------------------------------------------------------
