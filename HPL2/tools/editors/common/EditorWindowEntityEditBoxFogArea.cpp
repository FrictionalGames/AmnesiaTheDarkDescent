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

#include "EditorWindowEntityEditBoxFogArea.h"
#include "EditorHelper.h"
#include "EntityWrapperFogArea.h"
#include "EditorInput.h"

//------------------------------------------------------------

cEditorWindowEntityEditFogArea::cEditorWindowEntityEditFogArea(cEditorEditModeSelect* apEditMode, cEntityWrapperFogArea* apArea) : cEditorWindowEntityEditBox(apEditMode,apArea)
{
	mpFogArea = apArea;
}

//------------------------------------------------------------

cEditorWindowEntityEditFogArea::~cEditorWindowEntityEditFogArea()
{
}

//------------------------------------------------------------

void cEditorWindowEntityEditFogArea::Create()
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
	mpInpColor = CreateInputColorFrame(vPos, _W("Color"),"", pTab);
	vPos.y += mpInpColor->GetSize().y;
	mpInpStart = CreateInputNumber(vPos, _W("Start"), "", pTab);
	vPos.y += mpInpStart->GetSize().y;
	mpInpEnd = CreateInputNumber(vPos, _W("End"), "", pTab);
	vPos.y += mpInpEnd->GetSize().y;
	mpInpFalloffExp = CreateInputNumber(vPos, _W("FalloffExp"), "", pTab);	
	vPos.y += mpInpFalloffExp->GetSize().y;
	mpInpShowBacksideWhenInside = CreateInputBool(vPos, _W("Show backside when inside"), "", pTab);	
	vPos.y += mpInpShowBacksideWhenInside->GetSize().y;
	mpInpShowBacksideWhenOutside = CreateInputBool(vPos, _W("Show backside when outside"), "", pTab);	
}

//------------------------------------------------------------

void cEditorWindowEntityEditFogArea::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);
	mpInpColor->SetValue(mpFogArea->GetColor(), false);
	mpInpStart->SetValue(mpFogArea->GetStart(), false);
	mpInpEnd->SetValue(mpFogArea->GetEnd(), false);
	mpInpFalloffExp->SetValue(mpFogArea->GetFalloffExp(), false);
	mpInpShowBacksideWhenInside->SetValue(mpFogArea->GetShownBacksideWhenInside(), false);
	mpInpShowBacksideWhenOutside->SetValue(mpFogArea->GetShownBacksideWhenOutside(), false);
}

//------------------------------------------------------------

bool cEditorWindowEntityEditFogArea::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	tString strFilename;

	/////////////////////////////////////////////////////
	if(apInput==mpInpColor)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eFogAreaCol_Color, mpInpColor->GetValue());
	}
	else if(apInput==mpInpStart)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eFogAreaFloat_Start, mpInpStart->GetValue());
	}
	else if(apInput==mpInpEnd)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eFogAreaFloat_End, mpInpEnd->GetValue());
	}
	else if(apInput==mpInpFalloffExp)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eFogAreaFloat_FalloffExp, mpInpFalloffExp->GetValue());
	}
	else if(apInput==mpInpShowBacksideWhenInside)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eFogAreaBool_ShownBacksideWhenInside, mpInpShowBacksideWhenInside->GetValue());
	}
	else if(apInput==mpInpShowBacksideWhenOutside)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eFogAreaBool_ShownBacksideWhenOutside, mpInpShowBacksideWhenOutside->GetValue());
	}
	else
		return false;
	
	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

