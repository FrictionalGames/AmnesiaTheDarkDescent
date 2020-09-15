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

#include "EditorWindowEntityEditBoxDecal.h"

#include "EntityWrapperDecal.h"
#include "EditorHelper.h"
#include "EditorEditModeSelect.h"

#include "SurfacePicker.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxDecal::cEditorWindowEntityEditBoxDecal(cEditorEditModeSelect* apEditMode, cEntityWrapperDecal* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
	mbClearingSubDivValues = false;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxDecal::~cEditorWindowEntityEditBoxDecal()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxDecal::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	/*
    mpGroupName->SetPosition(cVector3f(15,30,0.1f));
	mpGroupPosition->SetPosition(cVector3f(15,60,0.1f));
	mpGroupScale->SetPosition(cVector3f(15,160,0.1f));
	mpGroupRotation->SetPosition(cVector3f(15,260,0.1f));
	*/

	mpTabDecal = mpTabs->AddTab(_W("Decal"));
    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddDecalPropertySet(mpTabDecal);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxDecal::AddDecalPropertySet(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	tWString vButtonText[] = { _W("St"), _W("Pr"), _W("En") };
	for(int i=0;i<eSurfaceType_LastEnum;++i)
	{
		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos + cVector3f(25,0,0)*(float)i, cVector2f(20), vButtonText[i], apParentTab);

		pButton->SetToggleable(true);
		pButton->SetUserValue(i);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

		mvBSurfaceTypes.push_back(pButton);		
	}

	vPos.y += 25;

	mpInpMaterial = CreateInputFile(vPos, _W("Material"), "", apParentTab);
	mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
	vPos.y += mpInpMaterial->GetSize().y+10;

	mpInpColor = CreateInputColorFrame(vPos, _W("Color"), "", apParentTab);

	vPos.y += mpInpColor->GetSize().y+10;

	mpInpSubDivisions = CreateInputEnum(vPos, _W("UV SubDivisions"), "", tWStringList(), apParentTab,50);
	mpInpSubDivisions->AddValue(_W("1x1"));
	mpInpSubDivisions->AddValue(_W("2x2"));
	mpInpSubDivisions->AddValue(_W("3x3"));
	mpInpSubDivisions->AddValue(_W("4x4"));
	vPos.x += mpInpSubDivisions->GetSize().x + 10;

	mpInpCurrentSubDiv = CreateInputEnum(vPos, _W("Current SubDiv"), "", tWStringList(), apParentTab, 80);
	mpInpCurrentSubDiv->SetMaxShownItems(4);

	vPos.x = 10;
	vPos.y += mpInpSubDivisions->GetSize().y + 10;

	mpInpOffset = CreateInputNumber(vPos, _W("Decal offset"), "", apParentTab, 80, 0.000001f);
	mpInpOffset->SetLowerBound(true, cEntityWrapperTypeDecal::GetDecalOffsetMin());
	mpInpOffset->SetUpperBound(true, cEntityWrapperTypeDecal::GetDecalOffsetMax());
	mpInpOffset->SetDecimals(6);

	vPos.y += mpInpOffset->GetSize().y + 10;

	mpInpMaxTris = CreateInputNumber(vPos ,_W("Max triangles"), "", apParentTab, 70, 10);
	mpInpMaxTris->SetLowerBound(true, 1);
	mpInpMaxTris->SetDecimals(0);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxDecal::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	int lPropID[] = { eDecalBool_AffectStatic, eDecalBool_AffectPrimitive, eDecalBool_AffectEntity };
	for(int i=0;i<(int)mvBSurfaceTypes.size();++i)
	{
		bool bVal;
		mpEntity->GetProperty(lPropID[i], bVal);
        mvBSurfaceTypes[i]->SetPressed(bVal,false);
	}

	mpInpMaterial->SetValue(cString::To16Char(mpEntity->GetMaterial()),false);
	mpInpColor->SetValue(mpEntity->GetColor(), false);
	mpInpSubDivisions->SetValue(mpEntity->GetUVSubDivisions().x-1, false);

	int lValue = mpInpSubDivisions->GetValue()+1;
	int lSubDivAmount = lValue*lValue;
	mbClearingSubDivValues = true;
	mpInpCurrentSubDiv->ClearValues();
	for(int i=0;i<lSubDivAmount;++i)
		mpInpCurrentSubDiv->AddValue(cString::ToStringW(i));
	mbClearingSubDivValues = false;

    mpInpCurrentSubDiv->SetValue(mpEntity->GetCurrentSubDiv(), false);
	mpInpOffset->SetValue(mpEntity->GetOffset(), false);
	mpInpMaxTris->SetValue((float)mpEntity->GetMaxTriangles(), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxDecal::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	int lPropID[] = { eDecalBool_AffectStatic, eDecalBool_AffectPrimitive, eDecalBool_AffectEntity };
	for(int i=0;i<(int)this->mvBSurfaceTypes.size();++i)
	{
		if(apWidget==mvBSurfaceTypes[i])
		{
			mpEditor->AddAction(mpEntity->CreateSetPropertyActionBool(lPropID[apWidget->GetUserValue()], mvBSurfaceTypes[i]->IsPressed()));
		}
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxDecal,InputCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxDecal::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();

	if(apInput==mpInpMaterial)
	{
		if(mpInpMaterial->GetValue()==_W("") || 
			cEditorHelper::LoadResourceFile(eEditorResourceType_Material, cString::To8Char(mpInpMaterial->GetValue()), NULL))
			pAction = mpEntity->CreateSetPropertyActionString(5, cString::To8Char(mpInpMaterial->GetValue()));
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eDecalCol_Color, mpInpColor->GetValue());
	}	
	else if(apInput==mpInpSubDivisions)
	{
		pAction = mpEntity->CreateSetPropertyActionVector2f(eDecalVec2f_SubDivs, cVector2f((float)mpInpSubDivisions->GetValue()+1));
	}
	else if(apInput==mpInpCurrentSubDiv)
	{
		//////////////////////////////////////
		// NOTE: this is really hackish... how to fix this?
		if(mbClearingSubDivValues==false)
			pAction = mpEntity->CreateSetPropertyActionInt(eDecalInt_CurrentSubDiv, mpInpCurrentSubDiv->GetValue());
	}
	else if(apInput==mpInpOffset)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eDecalFloat_Offset, mpInpOffset->GetValue());
	}
	else if(apInput==mpInpMaxTris)
	{
		pAction = mpEntity->CreateSetPropertyActionInt(eDecalInt_MaxTris, (int)mpInpMaxTris->GetValue());
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
