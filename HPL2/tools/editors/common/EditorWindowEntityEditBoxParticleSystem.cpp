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

#include "EditorWindowEntityEditBoxParticleSystem.h"
#include "EntityWrapperParticleSystem.h"
#include "EditorEditModeSelect.h"

#include "EditorWorld.h"
#include "EditorInput.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxParticleSystem::cEditorWindowEntityEditBoxParticleSystem(cEditorEditModeSelect* apEditMode, cEntityWrapperParticleSystem* apPS) : cEditorWindowEntityEditBox(apEditMode,apPS)
{
	mpEntity = apPS;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxParticleSystem::~cEditorWindowEntityEditBoxParticleSystem()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxParticleSystem::Create()
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
	AddPropertySetPS(mpTabPS);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxParticleSystem::AddPropertySetPS(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);
	float fAddNear = 18;
	float fAddFar = 35;

	mpLabelFile = mpSet->CreateWidgetLabel(vPos, 0, _W("Particle System file"), apParentTab);
	vPos.y += fAddNear;

	mpInputFile = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab);
	mpInputFile->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	mpButtonPSBrowse = mpSet->CreateWidgetButton(vPos + cVector3f(125,0,0), cVector2f(20), _W("..."), apParentTab);
	mpButtonPSBrowse->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(BrowseButton_OnPressed));

	vPos.y += fAddFar;

	mpInpColor = CreateInputColorFrame(vPos, _W("Color"), "", apParentTab);
	vPos.y += fAddFar;

	mpInpFadeAtDistance = CreateInputBool(vPos, _W("Fade at distance"), "", apParentTab);
	vPos.y += mpInpFadeAtDistance->GetSize().y;
	mpInpMinFadeDistanceEnd = CreateInputNumber(vPos, _W("Min End"), "", apParentTab);
	vPos.y += mpInpMinFadeDistanceEnd->GetSize().y;
	mpInpMinFadeDistanceStart = CreateInputNumber(vPos, _W("Min Start"), "", apParentTab);
	vPos.y += mpInpMinFadeDistanceStart->GetSize().y;
	mpInpMaxFadeDistanceStart = CreateInputNumber(vPos, _W("Max Start"), "", apParentTab);
	vPos.y += mpInpMaxFadeDistanceStart->GetSize().y;
	mpInpMaxFadeDistanceEnd = CreateInputNumber(vPos, _W("Max End"), "", apParentTab);
}


//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxParticleSystem::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	if(mpInputFile) mpInputFile->SetText(cString::To16Char(mpEntity->GetFile()));

	if(mpInpFadeAtDistance) mpInpFadeAtDistance->SetValue(mpEntity->GetFadeAtDistance(), false);
	if(mpInpColor) mpInpColor->SetValue(mpEntity->GetColor(), false);
	if(mpInpMinFadeDistanceStart) mpInpMinFadeDistanceStart->SetValue(mpEntity->GetMinFadeDistanceStart(), false);
	if(mpInpMinFadeDistanceEnd) mpInpMinFadeDistanceEnd->SetValue(mpEntity->GetMinFadeDistanceEnd(), false);
	if(mpInpMaxFadeDistanceStart) mpInpMaxFadeDistanceStart->SetValue(mpEntity->GetMaxFadeDistanceStart(), false);
	if(mpInpMaxFadeDistanceEnd) mpInpMaxFadeDistanceEnd->SetValue(mpEntity->GetMaxFadeDistanceEnd(), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxParticleSystem::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pEditorWorld = mpEditor->GetEditorWorld();
	
	if(apWidget==mpInputFile)
	{
		tString sPSFile = cString::To8Char(mpInputFile->GetText());

		cParticleSystem* pPS = pEditorWorld->GetWorld()->CreateParticleSystem("Temp", sPSFile,1);

		if(sPSFile!="" && pPS==NULL)
		{
			mpInputFile->SetText(_W(""));
		}
		else
		{
			pAction = mpEntity->CreateSetPropertyActionString(eParticleSystemStr_File, sPSFile);
			//pAction = hplNew( cEditorActionParticleSystemSetStringProperty,(lID, pEditorWorld, eParticleSystemStringProperty_File, sPSFile));
			if(pPS) pEditorWorld->GetWorld()->DestroyParticleSystem(pPS);
		}
	}
	
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxParticleSystem, InputCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxParticleSystem::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput)==true)
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apInput==mpInpFadeAtDistance)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eParticleSystemBool_FadeAtDistance, mpInpFadeAtDistance->GetValue());
	}
	else if(apInput==mpInpMinFadeDistanceEnd)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eParticleSystemFloat_MinFadeDistanceEnd, mpInpMinFadeDistanceEnd->GetValue());
	}
	else if(apInput==mpInpMinFadeDistanceStart)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eParticleSystemFloat_MinFadeDistanceStart, mpInpMinFadeDistanceStart->GetValue());
	}
	else if(apInput==mpInpMaxFadeDistanceStart)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eParticleSystemFloat_MaxFadeDistanceStart, mpInpMaxFadeDistanceStart->GetValue());
	}
	else if(apInput==mpInpMaxFadeDistanceEnd)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eParticleSystemFloat_MaxFadeDistanceEnd, mpInpMaxFadeDistanceEnd->GetValue());
	}
	else if(apInput==mpInpColor)
	{
		pAction = mpEntity->CreateSetPropertyActionColor(eParticleSystemCol_Color, mpInpColor->GetValue());
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxParticleSystem::BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget == mpButtonPSBrowse)
	{
		mpEditor->ShowLoadFilePicker(mvLoadedFiles, msLastPath,this,kGuiCallback(FilePSCallback), _W("Particle Systems"), tWStringList(1,_W("*.ps")));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxParticleSystem, BrowseButton_OnPressed);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxParticleSystem::FilePSCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpInputFile->SetText(sFilename);
		InputCallback(mpInputFile, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxParticleSystem, FilePSCallback);


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
