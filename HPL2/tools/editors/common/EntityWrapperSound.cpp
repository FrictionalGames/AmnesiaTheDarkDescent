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

#include "EntityWrapperSound.h"

#include "EntityIcon.h"

#include "EditorWorld.h"
#include "EditorBaseClasses.h"

#include "EditorWindowViewport.h"

#include "EditorWindowEntityEditBoxSound.h"

//---------------------------------------------------------------------------

cEntityWrapperTypeSound::cEntityWrapperTypeSound() : iEntityWrapperType(eEditorEntityType_Sound, _W("Sound"), "Sound")
{
	AddBool(eSoundBool_UseDefault, "UseDefault");
	AddString(eSoundStr_Filename, "SoundEntityFile", "", ePropCopyStep_PreEnt);
	AddFloat(eSoundFloat_MinDist, "MinDistance", 1);
	AddFloat(eSoundFloat_MaxDist, "MaxDistance", 5);
	AddFloat(eSoundFloat_Volume, "Volume", 1);
}

iEntityWrapperData* cEntityWrapperTypeSound::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataSound,(this));
}

//---------------------------------------------------------------------------

cEntityWrapperDataSound::cEntityWrapperDataSound(iEntityWrapperType* apType) :iEntityWrapperData(apType)
{
	SetName("Sound");
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataSound::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperSound,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperSound::cEntityWrapperSound(iEntityWrapperData* apData) : iEntityWrapper(apData)
                                                                                           
{
	mpSoundEntityData = NULL;
}

//------------------------------------------------------------------------------

cEntityWrapperSound::~cEntityWrapperSound()
{
	if(mpSoundEntityData)
	{
		cSoundEntityManager* pManager = GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetSoundEntityManager();
		pManager->Destroy(mpSoundEntityData);
	}
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityIcon* cEntityWrapperSound::CreateIcon()
{
	return hplNew( cEntityIcon,(this, "Sound"));
}

//------------------------------------------------------------------------------

bool cEntityWrapperSound::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundBool_UseDefault:
		abX = GetUseDefault();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperSound::GetProperty(int alPropID, float& afX)
{
	if(iEntityWrapper::GetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundFloat_MinDist:
		afX = GetMinDistance();
		break;
	case eSoundFloat_MaxDist:
		afX = GetMaxDistance();
		break;
	case eSoundFloat_Volume:
		afX = GetVolume();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperSound::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundStr_Filename:
		asX = GetSoundEntityFile();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperSound::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundBool_UseDefault:
		SetUseDefault(abX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperSound::SetProperty(int alPropID, const float& afX)
{
	if(iEntityWrapper::SetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundFloat_MinDist:
		SetMinDistance(afX);
		break;
	case eSoundFloat_MaxDist:
		SetMaxDistance(afX);
		break;
	case eSoundFloat_Volume:
		SetVolume(afX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperSound::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eSoundStr_Filename:
		SetSoundEntityFile(asX);
		break;
	default:
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperSound::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode,
								bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);

	if(abIsSelected)
	{
		float fMin = mfMinDistance;
		float fMax = mfMaxDistance;

		if(mbUseDefault && mpSoundEntityData)
		{
			fMin = mpSoundEntityData->GetMinDistance();
			fMax = mpSoundEntityData->GetMaxDistance();
		}

		apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, fMin, cColor(0.6f, 1));
		apFunctions->GetLowLevelGfx()->DrawSphere(mvPosition, fMax, cColor(0.4f, 1));
	}

	apFunctions->SetMatrix(NULL);
}


//------------------------------------------------------------------------------
/*
void cEntityWrapperSound::SaveToElement(cXmlElement* apElement)
{
	iEntityWrapper::SaveToElement(apElement);
	apElement->SetValue("Sound");

	apElement->SetAttributeString("SoundEntityFile", mpEditorWorld->GetEditor()->GetFilePathRelativeToWorkingDir(msSoundEntityFile));

	apElement->SetAttributeBool("UseDefault", mbUseDefault);

	apElement->SetAttributeFloat("MinDistance", mfMinDistance);
	apElement->SetAttributeFloat("MaxDistance", mfMaxDistance);

	apElement->SetAttributeFloat("Volume", mfVolume);
}
*/
//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperSound::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxSound,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------------

void cEntityWrapperSound::SetSoundEntityFile(const tString& asFile)
{
	iEditorBase* pEditor = GetEditorWorld()->GetEditor();
	cSoundEntityManager* pManager = pEditor->GetEngine()->GetResources()->GetSoundEntityManager();

	if(mpSoundEntityData)
		pManager->Destroy(mpSoundEntityData);

	msSoundEntityFile = cString::To8Char(pEditor->GetPathRelToWD(asFile));
	mpSoundEntityData = pManager->CreateSoundEntity(asFile);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

