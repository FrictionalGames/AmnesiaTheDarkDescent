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

#include "../Common/EdObjSound.h"

#include "../Common/EdObjectIcon.h"

#include "../Common/EdWorld.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/EdEditPaneSound.h"

#include "../Common/EdResource.h"

//---------------------------------------------------------------------------

cTypeSound::cTypeSound() : iEdScnObjType(_W("Sound"), "Sound")
{
	AddBool(eSoundBool_UseDefault, "UseDefault");
	AddString(eSoundStr_Filename, "SoundEntityFile", "", ePropStep_PreCreate);
	AddFloat(eSoundFloat_MinDist, "MinDistance", 1);
	AddFloat(eSoundFloat_MaxDist, "MaxDistance", 5);
	AddFloat(eSoundFloat_Volume, "Volume", 1);

	mbRotateable = false;
	mbScalable = false;
}

iEdObjectData* cTypeSound::CreateTypeSpecificData()
{
	return hplNew(cEdObjDataSound,(this));
}

//---------------------------------------------------------------------------

cEdObjDataSound::cEdObjDataSound(iEdObjectType* apType) : iEdScnObjData(apType)
{
	SetName(_W("Sound"));
}

//---------------------------------------------------------------------------

iEdObject* cEdObjDataSound::CreateTypeSpecificObject()
{
	return hplNew(cEdObjSound,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdObjSound::cEdObjSound(iEdObjectData* apData) : iEdScnObject(apData)
                                                                                           
{
	mpSoundEntityData = NULL;
}

//------------------------------------------------------------------------------

cEdObjSound::~cEdObjSound()
{
	if(mpSoundEntityData)
	{
		cSoundEntityManager* pManager = GetWorld()->GetEditor()->GetEngine()->GetResources()->GetSoundEntityManager();
		pManager->Destroy(mpSoundEntityData);
	}
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cObjectIcon* cEdObjSound::CreateIcon()
{
	return hplNew( cObjectIcon,(this, "Sound"));
}

//------------------------------------------------------------------------------

bool cEdObjSound::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eSoundBool_UseDefault:
		abX = GetUseDefault();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjSound::GetProperty(int alPropID, float& afX)
{
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
		return iEdScnObject::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjSound::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eSoundStr_Filename:
		asX = GetSoundEntityFile();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjSound::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eSoundBool_UseDefault:
		SetUseDefault(abX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjSound::SetProperty(int alPropID, const float afX)
{
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
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjSound::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eSoundStr_Filename:
		SetSoundEntityFile(asX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEdObjSound::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);

	if(mbSelected==false)
		return;
	
	float fMin = mfMinDistance;
	float fMax = mfMaxDistance;

	if(mbUseDefault && mpSoundEntityData)
	{
		fMin = mpSoundEntityData->GetMinDistance();
		fMax = mpSoundEntityData->GetMaxDistance();
	}

	aData.mpFunctions->GetLowLevelGfx()->DrawSphere(GetTranslation(), fMin, cColor(0.6f, 1));
	aData.mpFunctions->GetLowLevelGfx()->DrawSphere(GetTranslation(), fMax, cColor(0.4f, 1));
}


//------------------------------------------------------------------------------

iEdEditPane* cEdObjSound::CreateEditPane()
{
	return hplNew(cEdEditPaneSound,(this));
}

//------------------------------------------------------------------------------

void cEdObjSound::SetSoundEntityFile(const tString& asFile)
{
	iEditor* pEditor = GetWorld()->GetEditor();
	cSoundEntityManager* pManager = pEditor->GetEngine()->GetResources()->GetSoundEntityManager();

	if(mpSoundEntityData)
		pManager->Destroy(mpSoundEntityData);

	cEdLoader::LoadSound(asFile, &mpSoundEntityData);
	msSoundEntityFile = cString::To8Char(pEditor->GetPathRelToWD(cString::To16Char(asFile)));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

