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

#include "../Common/EdEditPaneBillboard.h"

#include "../Common/Editor.h"

#include "../Common/EdObjBillboard.h"
#include "../Common/EdEditModeSelect.h"

#include "../Common/EdResource.h"

//#include "EditorActionsBillboard.h"

//#include "EditorHelper.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneBillboard::cEdEditPaneBillboard(iEdScnObject* apObj) : iEdScnObjEditPane(apObj)
{
	mpLightHighlighter = NULL;
}

//----------------------------------------------------------------------------

cEdEditPaneBillboard::~cEdEditPaneBillboard()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneBillboard::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabBillboard = mpTabs->AddTab(_W("Billboard"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpActive->SetPosition(vPos);
	vPos.y += mpInpActive->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddBillboardProps(mpTabBillboard);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneBillboard::AddBillboardProps(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);

	mpInpMaterial = CreateInputMaterial(vPos, _W("Material file"), apParentTab, 100, mpEditor->GetMainLookUpDir(eDir_Billboards));

	vPos.y += mpInpMaterial->GetSize().y+10;

	mpInpType = CreateInputEnum(vPos, _W("Billboard type"), apParentTab);
	mpInpType->AddValue(_W("Point"));
	mpInpType->AddValue(_W("Axis"));
	mpInpType->AddValue(_W("FixedAxis"));

	vPos.y += mpInpType->GetSize().y+10;

	mpInpOffset = CreateInputNumber(vPos, _W("Offset"),  apParentTab, 50, 0.5f);

	vPos.y += mpInpOffset->GetSize().y+10;

	mpInpSize = CreateInputVec2(vPos, _W("Size"),  apParentTab, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.5f);

	vPos.y += mpInpSize->GetSize().y+10;

	mpInpColor = CreateInputColor(vPos, _W("Color"),  apParentTab);
	
	vPos.y += mpInpColor->GetSize().y+10;

	mpInpConnectedLight = CreateInputString(vPos, _W("Connected light name"),  apParentTab);
	mpInpConnectedLight->SetCanEdit(false);
	mpBConnectLight = mpSet->CreateWidgetButton(vPos + cVector3f(mpInpConnectedLight->GetSize().x+5,12,0), cVector2f(50,20), _W("Connect"), apParentTab);

	mpBConnectLight->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	vPos.y += mpInpConnectedLight->GetSize().y + 20;
	mpInpHalo = CreateInputBool(vPos, _W("Halo"),  apParentTab);
	vPos.y += mpInpHalo->GetSize().y + 5;
	mpInpHaloSourceSize = CreateInputVec3(vPos, _W("Halo Source Size"),  apParentTab, 50, tWStringList(), eEdInpStyle_RowLabelOnTop, 0.1f);
}


//----------------------------------------------------------------------------

void cEdEditPaneBillboard::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjBillboard* pBB = static_cast<cEdObjBillboard*>(mpObject);

	mpInpMaterial->SetValue(cString::To16Char(pBB->GetMaterialFile()),false);
	mpInpType->SetValue(cString::To16Char(pBB->GetBillboardType()), false);
	mpInpOffset->SetValue(pBB->GetBillboardOffset(), false);
	mpInpSize->SetValue(pBB->GetBillboardSize(), false);
	mpInpColor->SetValue(pBB->GetBillboardColor(), false);

	mpInpConnectedLight->SetValue(cString::To16Char(pBB->GetConnectedLightName()),false);

	mpInpHalo->SetValue(pBB->GetIsHalo(), false);
	mpInpHaloSourceSize->SetValue(pBB->GetHaloSourceSize(), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneBillboard::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdAction* pAction = NULL;
	int lID = mpObject->GetID();
	iEdWorld* pWorld = mpEditor->GetWorld();
	
	cResources* pRes = mpEditor->GetEngine()->GetResources();

	if(apWidget==mpBConnectLight)
	{
		mpLightHighlighter = hplNew( cEdSelectorHighlighter,(mpSelect));
		iEdObjectType* pType = mpEditor->GetWorld()->GetTypeByName(_W("Light"));
		mpLightHighlighter->GetFilter()->SetTypeFilter(pType, true);
		mpLightHighlighter->SetMouseUpCallback(this, kGuiCallback(HighlighterCallback));

		cEdObjBillboard* pBB = static_cast<cEdObjBillboard*>(mpObject);

		iEdScnObject* pLight = static_cast<iEdScnObject*>(pWorld->GetObject(cString::To16Char(pBB->GetConnectedLightName())));
		if(pLight && pLight->GetMainType()->GetName()==_W("Light"))
			mpLightHighlighter->SetHighlightedEntities(tScnObjList(1, pLight));

		mpSelect->SetCurrentSelector(mpLightHighlighter);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneBillboard, InputCallback);

//----------------------------------------------------------------------------

bool cEdEditPaneBillboard::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;
	int lID = mpObject->GetID();
	iEdWorld* pWorld = mpEditor->GetWorld();

	if(apInput==mpInpMaterial)
	{
		pAction = mpObject->CreateSetStringAction(eBillboardStr_Material, mpInpMaterial->GetValue8Char());
		/*tString sMatFile = cString::To8Char(mpInpMaterial->GetValue());
		if(sMatFile.empty()==false && cEdLoader::LoadMaterial(sMatFile)==false)
			mpInpMaterial->SetValue(_W(""), false);
		else
			pAction = mpObject->CreateSetStringAction(eBillboardStr_Material, sMatFile);

		/*if(sMatFile!="" && cEdHelper::LoadResourceFile(eEdResourceType_Material, sMatFile)==false)
		{
			mpInpMaterial->SetValue(_W(""), false);
		}
		else
		{
			pAction = mpObject->CreateSetStringAction(eBillboardStr_Material, sMatFile);
		}
		*/
	}
	else if(apInput==mpInpType)
	{
		tString sType = cString::To8Char(mpInpType->GetEnumValue(mpInpType->GetValue()));

		pAction = mpObject->CreateSetStringAction(eBillboardStr_Type, sType);
	}
	else if(apInput==mpInpOffset)
	{
		pAction = mpObject->CreateSetFloatAction(eBillboardFloat_Offset, mpInpOffset->GetValue());
	}
	else if(apInput==mpInpSize)
	{
		pAction = mpObject->CreateSetVector2fAction(eBillboardVec2f_Size, mpInpSize->GetValue());
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpObject->CreateSetColorAction(eBillboardCol_Color, mpInpColor->GetValue());
	}
	else if(apInput==mpInpConnectedLight)
	{
		//pAction = mpObject->CreateSetStringAction(eBillboardStr_ConnectedLight, cString::To8Char(mpInpConnectedLight->GetValue()));
	}
	else if(apInput==mpInpHalo)
	{
		pAction = mpObject->CreateSetBoolAction(eBillboardBool_IsHalo, mpInpHalo->GetValue());
	}
	else if(apInput==mpInpHaloSourceSize)
	{
		pAction = mpObject->CreateSetVector3fAction(eBillboardVec3f_HaloSourceSize, mpInpHaloSourceSize->GetValue());
	}

	mpEditor->AddAction(pAction);
	
	return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);
}

//----------------------------------------------------------------------------

bool cEdEditPaneBillboard::HighlighterCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpLightHighlighter==NULL)
		return false;

	cEdObjBillboard* pBB = static_cast<cEdObjBillboard*>(mpObject);
	iEdObject* pLight = mpLightHighlighter->GetPickedEntity();
	tString sName = pLight?cString::To8Char(pLight->GetName()):"";
	if(pBB->GetConnectedLightName()!=sName)
	{
		iEdAction* pAction = mpObject->CreateSetStringAction(eBillboardStr_ConnectedLight, sName);
		mpEditor->AddAction(pAction);
	}

	mpSelect->SetCurrentSelector(NULL);

	hplDelete(mpLightHighlighter);
	mpLightHighlighter = NULL;
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneBillboard, HighlighterCallback);

//----------------------------------------------------------------------------
