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

#include "../Common/EdEditPaneParticleSystem.h"
#include "../Common/EdObjParticleSystem.h"
//#include "EdEditModeSelect.h"
#include "../Common/EdResource.h"

#include "../Common/Editor.h"

#include "../Common/EdWorld.h"
#include "../Common/EdInput.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneParticleSystem::cEdEditPaneParticleSystem(iEdScnObject* apObj) : iEdScnObjEditPane(apObj)
{
}

//----------------------------------------------------------------------------

cEdEditPaneParticleSystem::~cEdEditPaneParticleSystem()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneParticleSystem::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabPS = mpTabs->AddTab(_W("Particle System"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

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

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPSProps(mpTabPS);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneParticleSystem::AddPSProps(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);
	float fAddNear = 18;
	float fAddFar = 35;

	mpInpFile = CreateInputParticleSystem(vPos, _W("Particle System file"), apParentTab);
	vPos.y += mpInpFile->GetSize().y + 10;

	mpInpColor = CreateInputColor(vPos, _W("Color"),  apParentTab);
	vPos.y += fAddFar;

	mpInpFadeAtDistance = CreateInputBool(vPos, _W("Fade at distance"),  apParentTab);
	vPos.y += mpInpFadeAtDistance->GetSize().y;
	mpInpMinFadeDistanceEnd = CreateInputNumber(vPos, _W("Min End"),  apParentTab);
	vPos.y += mpInpMinFadeDistanceEnd->GetSize().y;
	mpInpMinFadeDistanceStart = CreateInputNumber(vPos, _W("Min Start"),  apParentTab);
	vPos.y += mpInpMinFadeDistanceStart->GetSize().y;
	mpInpMaxFadeDistanceStart = CreateInputNumber(vPos, _W("Max Start"),  apParentTab);
	vPos.y += mpInpMaxFadeDistanceStart->GetSize().y;
	mpInpMaxFadeDistanceEnd = CreateInputNumber(vPos, _W("Max End"),  apParentTab);
}


//----------------------------------------------------------------------------

void cEdEditPaneParticleSystem::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjParticleSystem* pPS = static_cast<cEdObjParticleSystem*>(mpObject);

	if(mpInpFile) mpInpFile->SetValue(cString::To16Char(pPS->GetFile()), false);

	if(mpInpFadeAtDistance) mpInpFadeAtDistance->SetValue(pPS->GetFadeAtDistance(), false);
	if(mpInpColor) mpInpColor->SetValue(pPS->GetColor(), false);
	if(mpInpMinFadeDistanceStart) mpInpMinFadeDistanceStart->SetValue(pPS->GetMinFadeDistanceStart(), false);
	if(mpInpMinFadeDistanceEnd) mpInpMinFadeDistanceEnd->SetValue(pPS->GetMinFadeDistanceEnd(), false);
	if(mpInpMaxFadeDistanceStart) mpInpMaxFadeDistanceStart->SetValue(pPS->GetMaxFadeDistanceStart(), false);
	if(mpInpMaxFadeDistanceEnd) mpInpMaxFadeDistanceEnd->SetValue(pPS->GetMaxFadeDistanceEnd(), false);
}

//-----------------------------------------------------------------------------

bool cEdEditPaneParticleSystem::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	if(apInput==mpInpFile)
	{
		pAction = mpObject->CreateSetStringAction(eParticleSystemStr_File, mpInpFile->GetValue8Char());
	}
	else if(apInput==mpInpFadeAtDistance)
	{
		pAction = mpObject->CreateSetBoolAction(eParticleSystemBool_FadeAtDistance, mpInpFadeAtDistance->GetValue());
	}
	else if(apInput==mpInpMinFadeDistanceEnd)
	{
		pAction = mpObject->CreateSetFloatAction(eParticleSystemFloat_MinFadeDistanceEnd, mpInpMinFadeDistanceEnd->GetValue());
	}
	else if(apInput==mpInpMinFadeDistanceStart)
	{
		pAction = mpObject->CreateSetFloatAction(eParticleSystemFloat_MinFadeDistanceStart, mpInpMinFadeDistanceStart->GetValue());
	}
	else if(apInput==mpInpMaxFadeDistanceStart)
	{
		pAction = mpObject->CreateSetFloatAction(eParticleSystemFloat_MaxFadeDistanceStart, mpInpMaxFadeDistanceStart->GetValue());
	}
	else if(apInput==mpInpMaxFadeDistanceEnd)
	{
		pAction = mpObject->CreateSetFloatAction(eParticleSystemFloat_MaxFadeDistanceEnd, mpInpMaxFadeDistanceEnd->GetValue());
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpObject->CreateSetColorAction(eParticleSystemCol_Color, mpInpColor->GetValue());
	}
	else 
	{
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

