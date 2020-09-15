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

#include "../Common/EdEditPaneFogArea.h"
//#include "../Common/EditorHelper.h"
#include "../Common/EdObjFogArea.h"
#include "../Common/EdInput.h"

//------------------------------------------------------------

cEdEditPaneFogArea::cEdEditPaneFogArea(iEdScnObject* apObj) : iEdScnObjEditPane(apObj)
{
}

//------------------------------------------------------------

cEdEditPaneFogArea::~cEdEditPaneFogArea()
{
}

//------------------------------------------------------------

void cEdEditPaneFogArea::Create()
{
	cWidgetTab* pTabGeneral = mpTabs->AddTab(_W("General"));
	cWidgetTab* pTab = mpTabs->AddTab(_W("Fog"));

	AddPropertyName(pTabGeneral);
	AddPropertyActive(pTabGeneral);
	AddPropertyPosition(pTabGeneral);
	AddPropertyRotation(pTabGeneral);
	AddPropertyScale(pTabGeneral);

	mpInpScale->SetLabel(_W("Size"));

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpActive->SetPosition(vPos);
	vPos.y += mpInpActive->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;
	

	vPos = cVector3f(10,10,0.1f);
	mpInpColor = CreateInputColor(vPos, _W("Color"), pTab);
	vPos.y += mpInpColor->GetSize().y;
	mpInpStart = CreateInputNumber(vPos, _W("Start"),  pTab);
	vPos.y += mpInpStart->GetSize().y;
	mpInpEnd = CreateInputNumber(vPos, _W("End"),  pTab);
	vPos.y += mpInpEnd->GetSize().y;
	mpInpFalloffExp = CreateInputNumber(vPos, _W("FalloffExp"),  pTab);	
	vPos.y += mpInpFalloffExp->GetSize().y;
	mpInpShowBacksideWhenInside = CreateInputBool(vPos, _W("Show backside when inside"),  pTab);	
	vPos.y += mpInpShowBacksideWhenInside->GetSize().y;
	mpInpShowBacksideWhenOutside = CreateInputBool(vPos, _W("Show backside when outside"),  pTab);	
}

//------------------------------------------------------------

void cEdEditPaneFogArea::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjFogArea* pFogArea = static_cast<cEdObjFogArea*>(mpObject);

	mpInpColor->SetValue(pFogArea->GetColor(), false);

	mpInpStart->SetValue(pFogArea->GetStart(), false);
	mpInpEnd->SetValue(pFogArea->GetEnd(), false);

	mpInpFalloffExp->SetValue(pFogArea->GetFalloffExp(), false);

	mpInpShowBacksideWhenInside->SetValue(pFogArea->GetShownBacksideWhenInside(), false);
	mpInpShowBacksideWhenOutside->SetValue(pFogArea->GetShownBacksideWhenOutside(), false);
}

//------------------------------------------------------------

bool cEdEditPaneFogArea::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	/////////////////////////////////////////////////////
	// Color
	if(apInput==mpInpColor)
	{
		pAction = mpObject->CreateSetColorAction(eFogAreaCol_Color, mpInpColor->GetValue());
	}

	/////////////////////////////////////////////////////
	// Start
	else if(apInput==mpInpStart)
	{
		pAction = mpObject->CreateSetFloatAction(eFogAreaFloat_Start, mpInpStart->GetValue());
	}
	/////////////////////////////////////////////////////
	// End
	else if(apInput==mpInpEnd)
	{
		pAction = mpObject->CreateSetFloatAction(eFogAreaFloat_End, mpInpEnd->GetValue());
	}

	/////////////////////////////////////////////////////
	// Falloff Exp
	else if(apInput==mpInpFalloffExp)
	{
		pAction = mpObject->CreateSetFloatAction(eFogAreaFloat_FalloffExp, mpInpFalloffExp->GetValue());
	}

	/////////////////////////////////////////////////////
	// Show backside when inside
	else if(apInput==mpInpShowBacksideWhenInside)
	{
		pAction = mpObject->CreateSetBoolAction(eFogAreaBool_ShownBacksideWhenInside, mpInpShowBacksideWhenInside->GetValue());
	}
	/////////////////////////////////////////////////////
	// Show backside when outnside
	else if(apInput==mpInpShowBacksideWhenOutside)
	{
		pAction = mpObject->CreateSetBoolAction(eFogAreaBool_ShownBacksideWhenOutside, mpInpShowBacksideWhenOutside->GetValue());
	}
	
	AddAction(pAction);

	return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);
}

//----------------------------------------------------------------------------

