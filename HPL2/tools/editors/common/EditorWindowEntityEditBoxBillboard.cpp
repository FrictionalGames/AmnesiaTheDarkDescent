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

#include "EditorWindowEntityEditBoxBillboard.h"

#include "EntityWrapperBillboard.h"
#include "EditorEditModeSelect.h"

//#include "EditorActionsBillboard.h"

#include "EditorHelper.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBillboard::cEditorWindowEntityEditBoxBillboard(cEditorEditModeSelect* apEditMode, cEntityWrapperBillboard* apBillboard) : cEditorWindowEntityEditBox(apEditMode,apBillboard)
{
	mpEntity = apBillboard;
	mpLightHighlighter = NULL;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBillboard::~cEditorWindowEntityEditBoxBillboard()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBillboard::Create()
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
	AddPropertySetBillboard(mpTabBillboard);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBillboard::AddPropertySetBillboard(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);

	mpInpMaterial = CreateInputFile(vPos, _W("Material file"), "", apParentTab);
	mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
	mpInpMaterial->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Billboards));

	vPos.y += mpInpMaterial->GetSize().y+10;

	mpInpType = CreateInputEnum(vPos, _W("Billboard type"), "", tWStringList(), apParentTab);
	mpInpType->AddValue(_W("Point"));
	mpInpType->AddValue(_W("Axis"));
	mpInpType->AddValue(_W("FixedAxis"));

	vPos.y += mpInpType->GetSize().y+10;

	mpInpOffset = CreateInputNumber(vPos, _W("Offset"), "", apParentTab, 50, 0.5f);

	vPos.y += mpInpOffset->GetSize().y+10;

	mpInpSize = CreateInputVec2(vPos, _W("Size"), "", apParentTab, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.5f);

	vPos.y += mpInpSize->GetSize().y+10;

	mpInpColor = CreateInputColorFrame(vPos, _W("Color"), "", apParentTab);
	
	vPos.y += mpInpColor->GetSize().y+10;

	mpInpConnectedLight = CreateInputString(vPos, _W("Connected light name"), "", apParentTab);
	mpInpConnectedLight->SetCanEdit(false);
	mpBConnectLight = mpSet->CreateWidgetButton(vPos + cVector3f(mpInpConnectedLight->GetSize().x+5,12,0), cVector2f(50,20), _W("Connect"), apParentTab);

	mpBConnectLight->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

	vPos.y += mpInpConnectedLight->GetSize().y + 20;
	mpInpHalo = CreateInputBool(vPos, _W("Halo"), "", apParentTab);
	vPos.y += mpInpHalo->GetSize().y + 5;
	mpInpHaloSourceSize = CreateInputVec3(vPos, _W("Halo Source Size"), "", apParentTab, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.1f);
}


//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBillboard::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	mpInpMaterial->SetValue(cString::To16Char(mpEntity->GetMaterialFile()),false);
	((iEditorInput*)mpInpType)->SetValue(cString::To16Char(mpEntity->GetBillboardType()), false);
	mpInpOffset->SetValue(mpEntity->GetBillboardOffset(), false);
	mpInpSize->SetValue(mpEntity->GetBillboardSize(), false);
	mpInpColor->SetValue(mpEntity->GetBillboardColor(), false);

	mpInpConnectedLight->SetValue(cString::To16Char(mpEntity->GetConnectedLightName()),false);

	mpInpHalo->SetValue(mpEntity->GetIsHalo(), false);
	mpInpHaloSourceSize->SetValue(mpEntity->GetHaloSourceSize(), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBillboard::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	
	cResources* pRes = mpEditor->GetEngine()->GetResources();

	if(apWidget==mpBConnectLight)
	{
		mpLightHighlighter = hplNew( cEntitySelectorHighlighter,(mpEditMode));
		mpLightHighlighter->GetFilter()->SetTypeFilter(eEditorEntityType_Light, true);
		mpLightHighlighter->SetMouseUpCallback(this, kGuiCallback(HighlighterCallback));

		iEntityWrapper* pLight = pWorld->GetEntityByName(mpEntity->GetConnectedLightName());
		if(pLight && pLight->GetTypeID()==eEditorEntityType_Light)
			mpLightHighlighter->SetHighlightedEntities(tEntityWrapperList(1, pLight));

		mpEditMode->SetCurrentEntitySelector(mpLightHighlighter);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxBillboard, InputCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBillboard::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apInput==mpInpMaterial)
	{
		tString sMatFile = cString::To8Char(mpInpMaterial->GetValue());

		if(sMatFile!="" && cEditorHelper::LoadResourceFile(eEditorResourceType_Material, sMatFile)==false)
		{
			mpInpMaterial->SetValue(_W(""), false);
		}
		else
		{
			pAction = mpEntity->CreateSetPropertyActionString(eBillboardStr_Material, sMatFile);
		}
	}
	else if(apInput==mpInpType)
	{
		tString sType = cString::To8Char(mpInpType->GetEnumValue(mpInpType->GetValue()));

		pAction = mpEntity->CreateSetPropertyActionString(eBillboardStr_Type, sType);
	}
	else if(apInput==mpInpOffset)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBillboardFloat_Offset, mpInpOffset->GetValue());
	}
	else if(apInput==mpInpSize)
	{
		pAction = mpEntity->CreateSetPropertyActionVector2f(eBillboardVec2f_Size, mpInpSize->GetValue());
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eBillboardCol_Color, mpInpColor->GetValue());
	}
	else if(apInput==mpInpConnectedLight)
	{
		//pAction = mpEntity->CreateSetPropertyActionString(eBillboardStr_ConnectedLight, cString::To8Char(mpInpConnectedLight->GetValue()));
	}
	else if(apInput==mpInpHalo)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBillboardBool_IsHalo, mpInpHalo->GetValue());
	}
	else if(apInput==mpInpHaloSourceSize)
	{
		pAction = mpEntity->CreateSetPropertyActionVector3f(eBillboardVec3f_HaloSourceSize, mpInpHaloSourceSize->GetValue());
	}

	mpEditor->AddAction(pAction);
	
	return true;
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBillboard::HighlighterCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpLightHighlighter==NULL)
		return false;

	iEntityWrapper* pLight = mpLightHighlighter->GetPickedEntity();
	tString sName = pLight?pLight->GetName():"";
	if(mpEntity->GetConnectedLightName()!=sName)
	{
		iEditorAction* pAction = mpEntity->CreateSetPropertyActionString(eBillboardStr_ConnectedLight, sName);
		mpEditor->AddAction(pAction);
	}

	hplDelete(mpLightHighlighter);
	mpLightHighlighter = NULL;

	mpEditMode->SetCurrentEntitySelector(NULL);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxBillboard, HighlighterCallback);

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
