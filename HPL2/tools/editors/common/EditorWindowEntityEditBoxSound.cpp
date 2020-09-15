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

#include "EditorWindowEntityEditBoxSound.h"
#include "EntityWrapperSound.h"

#include "EditorEditModeSelect.h"
#include "EditorHelper.h"

//----------------------------------------------------------------------------

tWString cEditorWindowEntityEditBoxSound::msLastPath = _W("");

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxSound::cEditorWindowEntityEditBoxSound(cEditorEditModeSelect* apEditMode, cEntityWrapperSound* apSound) : cEditorWindowEntityEditBox(apEditMode,apSound)
{
	mpEntity = apSound;
	if(msLastPath==_W(""))
		msLastPath = mpEditor->GetMainLookUpDir(eDir_Sounds);
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxSound::~cEditorWindowEntityEditBoxSound()
{
	msLastPath = cString::GetFilePathW(mpInpSoundEntity->GetFullPath());
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxSound::Create()
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
	AddPropertySetSound(mpTabSound);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxSound::AddPropertySetSound(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(15,35,0.1f);

	mpInpSoundEntity = CreateInputFile(vPos, _W("Sound Entity file"), "", apParentTab);
	mpInpSoundEntity->SetBrowserType(eEditorResourceType_Sound);
	mpInpSoundEntity->SetInitialPath(msLastPath);

	vPos.y += mpInpSoundEntity->GetSize().y+10;

	mpInpUseDefault = CreateInputBool(vPos, _W("Use Default"), "", apParentTab);

	vPos.y += mpInpUseDefault->GetSize().y+10;

	tWStringList vMinMax;
	vMinMax.push_back(_W("Min"));
	vMinMax.push_back(_W("Max"));
	mpInpDistance = CreateInputVec2(vPos, _W("Distance"), "", apParentTab, 50, vMinMax, eEditorInputLayoutStyle_RowLabelOnTop, 0.5f);

	vPos.y += mpInpDistance->GetSize().y+10;

	mpInpVolume = CreateInputNumber(vPos, _W("Volume"), "", apParentTab, 50, 0.1f);
}


//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxSound::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	mpInpSoundEntity->SetValue(cString::To16Char(mpEntity->GetSoundEntityFile()),false);
	mpInpUseDefault->SetValue(mpEntity->GetUseDefault(),false);
	mpInpDistance->SetValue(cVector2f(mpEntity->GetMinDistance(), mpEntity->GetMaxDistance()), false);
	mpInpVolume->SetValue(mpEntity->GetVolume(), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxSound::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();

	if(apInput==mpInpSoundEntity)
	{
		tString sSoundEntFile = cString::To8Char(mpInpSoundEntity->GetValue());

		if(cEditorHelper::LoadResourceFile(eEditorResourceType_Sound, sSoundEntFile)==false)
		{
			sSoundEntFile = "";
		}

		pAction = mpEntity->CreateSetPropertyActionString( eSoundStr_Filename, sSoundEntFile);
	}
	else if(apInput==mpInpUseDefault)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eSoundBool_UseDefault, mpInpUseDefault->GetValue());
	}
	else if(apInput==mpInpDistance)
	{
		int lTBIdx = mpInpDistance->GetModifiedTextBoxId();
		float fValue = mpInpDistance->GetValue().v[lTBIdx];
		int vProps[] = {
			eSoundFloat_MinDist,
			eSoundFloat_MaxDist
		};

		pAction = mpEntity->CreateSetPropertyActionFloat(vProps[lTBIdx], fValue);
	}
	else if(apInput==mpInpVolume)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eSoundFloat_Volume, mpInpVolume->GetValue());
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
