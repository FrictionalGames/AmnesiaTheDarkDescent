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

#include "../Common/EdObjParticleSystem.h"

#include "../Common/EdWorld.h"
#include "../Common/EdClipPlane.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"

#include "../Common/EdEditPaneParticleSystem.h"

//#include "EditorHelper.h"

//---------------------------------------------------------------------------

cIconEntityPS::cIconEntityPS(iEdScnObject* apParent) : iIconObject(apParent, "PS")
{
}

cIconEntityPS::~cIconEntityPS()
{
	DestroyPS();
}

void cIconEntityPS::Update()
{
	cEdObjParticleSystem* pParent = (cEdObjParticleSystem*)mpParent;
	if(pParent->mbTypeUpdated)
	{
		const tString& sFile = pParent->GetFile();
		if(ReCreatePS(sFile))
			pParent->mbTypeUpdated = false;
	}
	if(mpObject) 
	{
		cParticleSystem* pPS = (cParticleSystem*)mpObject;

		//pPS->SetVisible(pParent->IsEnabled() && pParent->GetType()->IsEnabled());
	
		if(pParent->mbDataUpdated)
		{
			pParent->mbDataUpdated = false;
			pPS->SetColor(pParent->GetColor());
			pPS->SetFadeAtDistance(pParent->GetFadeAtDistance());
			pPS->SetMinFadeDistanceStart(pParent->GetMinFadeDistanceStart());
			pPS->SetMinFadeDistanceEnd(pParent->GetMinFadeDistanceEnd());
			pPS->SetMaxFadeDistanceStart(pParent->GetMaxFadeDistanceStart());
			pPS->SetMaxFadeDistanceEnd(pParent->GetMaxFadeDistanceEnd());
		}
	}
}

bool cIconEntityPS::ReCreatePS(const tString& asFile)
{
	DestroyPS();
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	mpObject = pWorld->CreateParticleSystem(cString::To8Char(mpParent->GetName()), asFile, mpParent->GetScale(), false);

	return IsCreated();
}

void cIconEntityPS::DestroyPS()
{
	if(mpObject)
	{
		cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
		pWorld->DestroyParticleSystem((cParticleSystem*)mpObject);

		mpObject = NULL;
	}
}

//---------------------------------------------------------------------------

cTypeParticleSystem::cTypeParticleSystem() : iEdScnObjType(_W("Particle System"), "ParticleSystem")
{
	AddString(eParticleSystemStr_File, "File", "", ePropStep_PreCreate);
	
	AddColor(eParticleSystemCol_Color, "Color");
	AddBool(eParticleSystemBool_FadeAtDistance, "FadeAtDistance");

	AddFloat(eParticleSystemFloat_MinFadeDistanceStart, "MinFadeDistanceStart", 2);
	AddFloat(eParticleSystemFloat_MaxFadeDistanceStart, "MaxFadeDistanceStart", 100);
	AddFloat(eParticleSystemFloat_MinFadeDistanceEnd, "MinFadeDistanceEnd", 1);
	AddFloat(eParticleSystemFloat_MaxFadeDistanceEnd, "MaxFadeDistanceEnd", 110);
}

iEdObjectData* cTypeParticleSystem::CreateTypeSpecificData()
{
	return hplNew( cEdObjDataParticleSystem, (this));
}

//---------------------------------------------------------------------------

cEdObjDataParticleSystem::cEdObjDataParticleSystem(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

iEdObject* cEdObjDataParticleSystem::CreateTypeSpecificObject()
{
	return hplNew(cEdObjParticleSystem,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdObjParticleSystem::cEdObjParticleSystem(iEdObjectData* apData) : iEdScnObject(apData)
{
}

//------------------------------------------------------------------------------

cEdObjParticleSystem::~cEdObjParticleSystem()
{
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEdObjParticleSystem::GetProperty(int alPropID, cColor& aX)
{
	if(iEdScnObject::GetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemCol_Color:
		aX = GetColor();
		break;
	default:
		return false;
	}

	return true;
}

bool cEdObjParticleSystem::GetProperty(int alPropID, bool& abX)
{
	if(iEdScnObject::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemBool_FadeAtDistance:
		abX = GetFadeAtDistance();
		break;
	default:
		return false;
	}

	return true;
}

bool cEdObjParticleSystem::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eParticleSystemFloat_MinFadeDistanceStart:
		afX = GetMinFadeDistanceStart();
		break;
	case eParticleSystemFloat_MaxFadeDistanceStart:
		afX = GetMaxFadeDistanceStart();
		break;
	case eParticleSystemFloat_MinFadeDistanceEnd:
		afX = GetMinFadeDistanceEnd();
		break;
	case eParticleSystemFloat_MaxFadeDistanceEnd:
		afX = GetMaxFadeDistanceEnd();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjParticleSystem::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eParticleSystemStr_File:
		asX = GetFile();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjParticleSystem::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eParticleSystemCol_Color:
		SetColor(aX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, aX);
	}

	return true;
}

bool cEdObjParticleSystem::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eParticleSystemBool_FadeAtDistance:
		SetFadeAtDistance(abX);
	
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjParticleSystem::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eParticleSystemFloat_MinFadeDistanceStart:
		SetMinFadeDistanceStart(afX);
		break;
	case eParticleSystemFloat_MaxFadeDistanceStart:
		SetMaxFadeDistanceStart(afX);
		break;
	case eParticleSystemFloat_MinFadeDistanceEnd:
		SetMinFadeDistanceEnd(afX);
		break;
	case eParticleSystemFloat_MaxFadeDistanceEnd:
		SetMaxFadeDistanceEnd(afX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjParticleSystem::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eParticleSystemStr_File:
		SetFile(asX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


void cEdObjParticleSystem::SetVisible(bool abX)
{
	//iEdObject::SetVisible(abX);

	//((cParticleSystem*)mpEngineEntity)->SetVisible(abX);
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::OnSetCulled(bool abX)
{
	//iEdObject::OnSetCulled(abX);

	//((cParticleSystem*)mpEngineEntity)->SetVisible(abX==false);
}

//------------------------------------------------------------------------------

bool cEdObjParticleSystem::EntitySpecificCheckCulled(cEdClipPlane* apPlane)
{
	return true;
	//return apPlane->PointIsOnCullingSide(GetTranslation());
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);

	if(mbSelected)
	{
		aData.mpFunctions->SetDepthTest(true);
		aData.mpFunctions->SetDepthWrite(false);
		
		aData.mpViewport->DrawArrow(aData.mpFunctions, GetWorldMatrix(), 1, true, cVector2f(0.05f, 0.4f), cColor(1,1));
		
		aData.mpFunctions->SetMatrix(NULL);
	}
}

//------------------------------------------------------------------------------


iEdEditPane* cEdObjParticleSystem::CreateEditPane()
{
	return hplNew(cEdEditPaneParticleSystem,(this));
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetFile(const tString& asFile)
{
	if(msFile==asFile)
		return;
	mbTypeUpdated = true;
	msFile = asFile;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetColor(const cColor& aCol)
{
	 mColor = aCol;
	 mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetFadeAtDistance(bool abX)
{
	mbFadeAtDistance = abX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetMinFadeDistanceStart(float afX)
{
	mfMinFadeDistanceStart = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetMinFadeDistanceEnd(float afX)
{
	mfMinFadeDistanceEnd = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetMaxFadeDistanceStart(float afX)
{
	mfMaxFadeDistanceStart = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEdObjParticleSystem::SetMaxFadeDistanceEnd(float afX)
{
	mfMaxFadeDistanceEnd = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEngineObject* cEdObjParticleSystem::CreateEngineObject()
{
	return hplNew(cIconEntityPS,(this));
}
