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

#include "../Common/EdEditPaneSound.h"
#include "../Common/EdObjSound.h"

#include "../Common/Editor.h"
//#include "EdEditModeSelect.h"
//#include "EditorHelper.h"

//----------------------------------------------------------------------------

tWString cEdEditPaneSound::msLastPath = _W("");

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneSound::cEdEditPaneSound(iEdScnObject* apObj) : iEdScnObjEditPane(apObj)
{
	if(msLastPath==_W(""))
		msLastPath = mpEditor->GetMainLookUpDir(eDir_Sounds);
}

//----------------------------------------------------------------------------

cEdEditPaneSound::~cEdEditPaneSound()
{
	msLastPath = cString::GetFilePathW(mpInpSoundEntity->GetFullPath());
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneSound::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabSound = mpTabs->AddTab(_W("Sound"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyActive(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpActive->SetPosition(vPos);
	vPos.y += mpInpActive->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddSoundProps(mpTabSound);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneSound::AddSoundProps(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);

	mpInpSoundEntity = CreateInputSound(vPos, _W("Sound Entity file"), apParentTab);
	mpInpSoundEntity->SetInitialPath(msLastPath);

	vPos.y += mpInpSoundEntity->GetSize().y+10;

	mpInpUseDefault = CreateInputBool(vPos, _W("Use Default"),  apParentTab);

	vPos.y += mpInpUseDefault->GetSize().y+10;

	tWStringList vMinMax;
	vMinMax.push_back(_W("Min"));
	vMinMax.push_back(_W("Max"));
	mpInpDistance = CreateInputVec2(vPos, _W("Distance"),  apParentTab, 50, vMinMax, eEdInpStyle_RowLabelOnTop, 0.5f);
	mpInpDistance->SetLowerBound(true, 0);

	vPos.y += mpInpDistance->GetSize().y+10;

	mpInpVolume = CreateInputNumber(vPos, _W("Volume"),  apParentTab, 50, 0.1f);
	mpInpVolume->SetLowerBound(true, 0);
}


//----------------------------------------------------------------------------

void cEdEditPaneSound::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjSound* pSound = static_cast<cEdObjSound*>(mpObject);

	mpInpSoundEntity->SetValue(cString::To16Char(pSound->GetSoundEntityFile()),false);
	mpInpUseDefault->SetValue(pSound->GetUseDefault(),false);
	mpInpDistance->SetValue(cVector2f(pSound->GetMinDistance(), pSound->GetMaxDistance()), false);
	mpInpVolume->SetValue(pSound->GetVolume(), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneSound::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;

	//////////////////////////////////////////////////////////////////////
	// Sound file
	if(apInput==mpInpSoundEntity)
	{
		pAction = mpObject->CreateSetStringAction(eSoundStr_Filename, mpInpSoundEntity->GetValue8Char());
	}

	//////////////////////////////////////////////////////////////////////
	// Use default
	else if(apInput==mpInpUseDefault)
	{
		pAction = mpObject->CreateSetBoolAction(eSoundBool_UseDefault, mpInpUseDefault->GetValue());
	}

	//////////////////////////////////////////////////////////////////////
	// Distance
	else if(apInput==mpInpDistance)
	{
		int lTBIdx = mpInpDistance->GetModifiedTextBoxId();
		float fValue = mpInpDistance->GetValue().v[lTBIdx];
		int vProps[] = 
		{
			eSoundFloat_MinDist,
			eSoundFloat_MaxDist
		};

		pAction = mpObject->CreateSetFloatAction(vProps[lTBIdx], fValue);
	}

	//////////////////////////////////////////////////////////////////////
	// Volume
	else if(apInput==mpInpVolume)
	{
		pAction = mpObject->CreateSetFloatAction(eSoundFloat_Volume, mpInpVolume->GetValue());
	}

	mpEditor->AddAction(pAction);

	return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);
}

//----------------------------------------------------------------------------
