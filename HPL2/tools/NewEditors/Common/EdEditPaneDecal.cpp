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

#include "../Common/EdEditPaneDecal.h"

#include "../Common/EdObjDecal.h"
#include "../Common/EdResource.h"
//#include "EditorHelper.h"
//#include "EdEditModeSelect.h"

#include "../Common/EdSurfacePicker.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneDecal::cEdEditPaneDecal(iEdScnObject* apObject) : iEdScnObjEditPane(apObject)
{
	mbClearingSubDivValues = false;
}

//----------------------------------------------------------------------------

cEdEditPaneDecal::~cEdEditPaneDecal()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneDecal::Create()
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
	AddDecalProps(mpTabDecal);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneDecal::AddDecalProps(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	cEdObjDecal* pDecal = static_cast<cEdObjDecal*>(mpObject);
	const tTypeFilterMap& mapAffectedSurfaces = pDecal->GetAffectedTypes();
	tTypeFilterMap::const_iterator it = mapAffectedSurfaces.begin();
	for(;it!=mapAffectedSurfaces.end();++it)
	{
		iEdObjectType* pType = it->first;
		tWString sTypeName = cString::SubW(pType->GetName(), 0, 2);

		cWidgetButton* pButton = mpSet->CreateWidgetButton(vPos, cVector2f(20), sTypeName, apParentTab);
		pButton->SetToggleable(true);
		pButton->SetUserData(pType);
		pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(InputCallback));

		mlstBSurfaceTypes.push_back(pButton);
		vPos.x += 25;
	}

	vPos.x = 10;
	vPos.y += 25;

	mpInpMaterial = CreateInputMaterial(vPos, _W("Material"), apParentTab);
	vPos.y += mpInpMaterial->GetSize().y+10;

	mpInpColor = CreateInputColor(vPos, _W("Color"),  apParentTab);

	vPos.y += mpInpColor->GetSize().y+10;

	mpInpSubDivisions = CreateInputEnum(vPos, _W("UV SubDivisions"), apParentTab,50);
	mpInpSubDivisions->AddValue(_W("1x1"));
	mpInpSubDivisions->AddValue(_W("2x2"));
	mpInpSubDivisions->AddValue(_W("3x3"));
	mpInpSubDivisions->AddValue(_W("4x4"));
	vPos.x += mpInpSubDivisions->GetSize().x + 10;

	mpInpCurrentSubDiv = CreateInputEnum(vPos, _W("Current SubDiv"), apParentTab, 80);
	mpInpCurrentSubDiv->SetMaxShownItems(4);

	vPos.x = 10;
	vPos.y += mpInpSubDivisions->GetSize().y + 10;

	mpInpOffset = CreateInputNumber(vPos, _W("Decal offset"),  apParentTab, 80, 0.000001f);
	mpInpOffset->SetLowerBound(true, cTypeDecal::GetDecalOffsetMin());
	mpInpOffset->SetUpperBound(true, cTypeDecal::GetDecalOffsetMax());
	mpInpOffset->SetDecimals(6);

	vPos.y += mpInpOffset->GetSize().y + 10;

	mpInpMaxTris = CreateInputNumber(vPos ,_W("Max triangles"),  apParentTab, 70, 10);
	mpInpMaxTris->SetLowerBound(true, 1);
	mpInpMaxTris->SetDecimals(0);
}

//----------------------------------------------------------------------------

void cEdEditPaneDecal::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjDecal* pDecal = static_cast<cEdObjDecal*>(mpObject);
	const tTypeFilterMap& affectedTypes = pDecal->GetAffectedTypes();

	tWidgetListIt it = mlstBSurfaceTypes.begin();
	for(; it!=mlstBSurfaceTypes.end(); ++it)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
		iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());

		pButton->SetPressed(affectedTypes.find(pType)->second, false);
	}

	cIndexedFile* pMat = pDecal->GetMatFile();
	mpInpMaterial->SetValue(cString::To16Char(pDecal->GetMaterial()),false);
	mpInpColor->SetValue(pDecal->GetColor(), false);
	mpInpSubDivisions->SetValue(pDecal->GetUVSubDivisions().x-1, false);

	int lValue = mpInpSubDivisions->GetValue()+1;
	int lSubDivAmount = lValue*lValue;
	mbClearingSubDivValues = true;
	mpInpCurrentSubDiv->ClearValues();
	for(int i=0;i<lSubDivAmount;++i)
		mpInpCurrentSubDiv->AddValue(cString::ToStringW(i));
	mbClearingSubDivValues = false;

    mpInpCurrentSubDiv->SetValue(pDecal->GetCurrentSubDiv(), false);
	mpInpOffset->SetValue(pDecal->GetOffset(), false);
	mpInpMaxTris->SetValue((float)pDecal->GetMaxTriangles(), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneDecal::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	cEdObjDecal* pDecal = static_cast<cEdObjDecal*>(mpObject);

	tTypeFilterMap affectedTypes = pDecal->GetAffectedTypes();
	tWidgetListIt it = mlstBSurfaceTypes.begin();
	for(; it!=mlstBSurfaceTypes.end(); ++it)
	{
		cWidgetButton* pButton = static_cast<cWidgetButton*>(*it);
		if(apWidget==pButton)
		{
			iEdObjectType* pType = static_cast<iEdObjectType*>(pButton->GetUserData());

			affectedTypes[pType] = pButton->IsPressed();
			pDecal->SetAffectedTypes(affectedTypes);
		}
	}

	// 14.40

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneDecal,InputCallback);

//----------------------------------------------------------------------------

bool cEdEditPaneDecal::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	if(apInput==mpInpMaterial)
	{
		tString sMat = mpInpMaterial->GetValue8Char();
		if(sMat.empty() || cEdLoader::LoadMaterial(sMat))
			pAction = mpObject->CreateSetStringAction(eDecalStr_Material, sMat);
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpObject->CreateSetColorAction(eDecalCol_Color, mpInpColor->GetValue());
	}	
	else if(apInput==mpInpSubDivisions)
	{
		pAction = mpObject->CreateSetVector2fAction(eDecalVec2f_SubDivs, cVector2f((float)mpInpSubDivisions->GetValue()+1));
	}
	else if(apInput==mpInpCurrentSubDiv)
	{
		//////////////////////////////////////
		// NOTE: this is really hackish... how to fix this?
		if(mbClearingSubDivValues==false)
			pAction = mpObject->CreateSetIntAction(eDecalInt_CurrentSubDiv, mpInpCurrentSubDiv->GetValue());
	}
	else if(apInput==mpInpOffset)
	{
		pAction = mpObject->CreateSetFloatAction(eDecalFloat_Offset, mpInpOffset->GetValue());
	}
	else if(apInput==mpInpMaxTris)
	{
		pAction = mpObject->CreateSetIntAction(eDecalInt_MaxTris, (int)mpInpMaxTris->GetValue());
	}
	else
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);

	AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
