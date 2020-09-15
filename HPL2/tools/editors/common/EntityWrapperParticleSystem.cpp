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

#include "EntityWrapperParticleSystem.h"

#include "EditorWorld.h"
#include "EditorClipPlane.h"
#include "EditorBaseClasses.h"

#include "EditorWindowViewport.h"

#include "EditorWindowEntityEditBoxParticleSystem.h"

#include "EditorHelper.h"

//---------------------------------------------------------------------------

cIconEntityPS::cIconEntityPS(iEntityWrapper* apParent) : iIconEntity(apParent, "PS")
{
}

cIconEntityPS::~cIconEntityPS()
{
	DestroyPS();
}

void cIconEntityPS::Update()
{
	cEntityWrapperParticleSystem* pParent = (cEntityWrapperParticleSystem*)mpParent;
	if(pParent->mbTypeUpdated)
	{
		const tString& sFile = pParent->GetFile();
		if(ReCreatePS(sFile))
		{
			pParent->mbTypeUpdated = false;
			pParent->mbEntityUpdated = true;
		}
	}
	if(mpEntity) 
	{
		cParticleSystem* pPS = (cParticleSystem*)mpEntity;

		pPS->SetVisible(pParent->IsActive() && pParent->GetType()->IsActive());
	
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
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity = pWorld->CreateParticleSystem(mpParent->GetName(), asFile, mpParent->GetScale(), false);

	return IsCreated();
}

void cIconEntityPS::DestroyPS()
{
	if(mpEntity)
	{
		cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
		pWorld->DestroyParticleSystem((cParticleSystem*)mpEntity);
		mpEntity = NULL;
	}
}

//---------------------------------------------------------------------------

cEntityWrapperTypeParticleSystem::cEntityWrapperTypeParticleSystem() : iEntityWrapperType(eEditorEntityType_ParticleSystem, _W("Particle System"), "ParticleSystem")
{
	AddString(eParticleSystemStr_File, "File", "", ePropCopyStep_PreEnt);
	
	AddColor(eParticleSystemCol_Color, "Color");
	AddBool(eParticleSystemBool_FadeAtDistance, "FadeAtDistance");

	AddFloat(eParticleSystemFloat_MinFadeDistanceStart, "MinFadeDistanceStart", 2);
	AddFloat(eParticleSystemFloat_MaxFadeDistanceStart, "MaxFadeDistanceStart", 100);
	AddFloat(eParticleSystemFloat_MinFadeDistanceEnd, "MinFadeDistanceEnd", 1);
	AddFloat(eParticleSystemFloat_MaxFadeDistanceEnd, "MaxFadeDistanceEnd", 110);
}

iEntityWrapperData* cEntityWrapperTypeParticleSystem::CreateSpecificData()
{
	return hplNew( cEntityWrapperDataParticleSystem, (this));
}

//---------------------------------------------------------------------------

cEntityWrapperDataParticleSystem::cEntityWrapperDataParticleSystem(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

iEntityWrapper* cEntityWrapperDataParticleSystem::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperParticleSystem,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperParticleSystem::cEntityWrapperParticleSystem(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
}

//------------------------------------------------------------------------------

cEntityWrapperParticleSystem::~cEntityWrapperParticleSystem()
{
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEntityWrapperParticleSystem::GetProperty(int alPropID, cColor& aX)
{
	if(iEntityWrapper::GetProperty(alPropID, aX)==true)
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

bool cEntityWrapperParticleSystem::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
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

bool cEntityWrapperParticleSystem::GetProperty(int alPropID, float& afX)
{
	if(iEntityWrapper::GetProperty(alPropID, afX)==true)
		return true;

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
		return false;
	}

	return true;
}

bool cEntityWrapperParticleSystem::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemStr_File:
		asX = GetFile();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperParticleSystem::SetProperty(int alPropID, const cColor& aX)
{
	if(iEntityWrapper::SetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemCol_Color:
		SetColor(aX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperParticleSystem::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemBool_FadeAtDistance:
		SetFadeAtDistance(abX);
	
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperParticleSystem::SetProperty(int alPropID, const float& afX)
{
	if(iEntityWrapper::SetProperty(alPropID, afX)==true)
		return true;

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
		return false;
	}

	return true;
}

bool cEntityWrapperParticleSystem::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eParticleSystemStr_File:
		SetFile(asX);
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


void cEntityWrapperParticleSystem::SetVisible(bool abX)
{
	//iEntityWrapper::SetVisible(abX);

	//((cParticleSystem*)mpEngineEntity)->SetVisible(abX);
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::OnSetCulled(bool abX)
{
	iEntityWrapper::OnSetCulled(abX);

	//((cParticleSystem*)mpEngineEntity)->SetVisible(abX==false);
}

//------------------------------------------------------------------------------

bool cEntityWrapperParticleSystem::EntitySpecificCheckCulled(cEditorClipPlane* apPlane)
{
	return apPlane->PointIsOnCullingSide(mvPosition);
}

//------------------------------------------------------------------------------


void cEntityWrapperParticleSystem::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode,
						bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport,apFunctions,apEditMode,abIsSelected, aHighlightCol);
		
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	if(abIsSelected) DrawArrow(apViewport, apFunctions, GetWorldMatrix(), 1, true, cVector2f(0.05f, 0.4f), cColor(1,1));

	apFunctions->SetMatrix(NULL);
}


//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperParticleSystem::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxParticleSystem,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetFile(const tString& asFile)
{
	if(msFile==asFile)
		return;
	mbTypeUpdated = true;
	msFile = asFile;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetColor(const cColor& aCol)
{
	 mColor = aCol;
	 mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetFadeAtDistance(bool abX)
{
	mbFadeAtDistance = abX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetMinFadeDistanceStart(float afX)
{
	mfMinFadeDistanceStart = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetMinFadeDistanceEnd(float afX)
{
	mfMinFadeDistanceEnd = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetMaxFadeDistanceStart(float afX)
{
	mfMaxFadeDistanceStart = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

void cEntityWrapperParticleSystem::SetMaxFadeDistanceEnd(float afX)
{
	mfMaxFadeDistanceEnd = afX;
	mbDataUpdated = true;
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEngineEntity* cEntityWrapperParticleSystem::CreateSpecificEngineEntity()
{
	return hplNew(cIconEntityPS,(this));
}
