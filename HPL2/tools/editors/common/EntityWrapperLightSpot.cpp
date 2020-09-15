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

#include "EntityWrapperLightSpot.h"
#include "EntityWrapperLight.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"
#include "EditorHelper.h"

#include "EditorWindowViewport.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ICON ENTITY SPOTLIGHT : CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cIconEntityLightSpot::cIconEntityLightSpot(iEntityWrapper* apParent) : iIconEntityLight(apParent, "Spot")
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ICON ENTITY SPOTLIGHT : PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cIconEntityLightSpot::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity = pWorld->CreateLightSpot(asName);

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT TYPE: CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperTypeLightSpot::cEntityWrapperTypeLightSpot() : iEntityWrapperTypeLight("SpotLight", eEditorEntityLightType_Spot)
{
	mScaleType = eScaleType_None;

	AddFloat(eLightSpotFloat_FOV, "FOV", cMath::ToRad(60));
	AddFloat(eLightSpotFloat_Aspect, "Aspect", 1);
	AddFloat(eLightSpotFloat_NearClipPlane, "NearClipPlane", 0.1f);
	AddString(eLightSpotStr_FalloffMap, "SpotFalloffMap");
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT TYPE: PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------


iEntityWrapperData* cEntityWrapperTypeLightSpot::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataLightSpot,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT DATA: CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperDataLightSpot::cEntityWrapperDataLightSpot(iEntityWrapperType *apType) : iEntityWrapperDataLight(apType)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT DATA: PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataLightSpot::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperLightSpot,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperLightSpot::cEntityWrapperLightSpot(iEntityWrapperData* apData) : iEntityWrapperLight(apData)
{
}

//---------------------------------------------------------------------------

cEntityWrapperLightSpot::~cEntityWrapperLightSpot()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperLightSpot::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eLightSpotFloat_FOV:
		SetFOV(afX);
		break;
	case eLightSpotFloat_Aspect:
		SetAspect(afX);
		break;
	case eLightSpotFloat_NearClipPlane:
		SetNearClipPlane(afX);
		break;
	default:
		return iEntityWrapperLight::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperLightSpot::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eLightSpotStr_FalloffMap:
		SetSpotFalloffMap(asX);
		break;
	default:
		return iEntityWrapperLight::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperLightSpot::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eLightSpotFloat_FOV:
		afX = GetFOV();
		break;
	case eLightSpotFloat_Aspect:
		afX = GetAspect();
		break;
	case eLightSpotFloat_NearClipPlane:
		afX = GetNearClipPlane();
		break;
	default:
		return iEntityWrapperLight::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperLightSpot::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eLightSpotStr_FalloffMap:
		asX = GetSpotFalloffMap();
		break;
	default:
		return iEntityWrapperLight::GetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::SetFOV(float afAngle)
{
	mfFOV = afAngle;

	((cLightSpot*)mpEngineEntity->GetEntity())->SetFOV(mfFOV);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::SetAspect(float afAngle)
{
	mfAspect = afAngle;

	((cLightSpot*)mpEngineEntity->GetEntity())->SetAspect(mfAspect);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::SetRadius(float afX)
{
	mfRadius = afX;

	((cLightSpot*)mpEngineEntity->GetEntity())->SetRadius(mfRadius);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::SetNearClipPlane(float afX)
{
	mfNearClipPlane = afX;

	((cLightSpot*)mpEngineEntity->GetEntity())->SetNearClipPlane(mfNearClipPlane);
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::SetSpotFalloffMap(const tString& asFalloffMap)
{
	iTexture* pTex = NULL;
	if(cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, asFalloffMap, &pTex))
	{
		msSpotFalloffMap = cString::To8Char(GetEditorWorld()->GetEditor()->GetPathRelToWD(asFalloffMap));
	}
	else
	{
		cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, "core_falloff_linear", &pTex);
		msSpotFalloffMap = "";
	}

	((cLightSpot*)mpEngineEntity->GetEntity())->SetSpotFalloffMap(pTex);	
}

//---------------------------------------------------------------------------

void cEntityWrapperLightSpot::DrawLightTypeSpecific(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
													iEditorEditMode* apEditMode, bool abIsSelected)
{
	((cLightSpot*)mpEngineEntity->GetEntity())->GetFrustum()->Draw(apFunctions->GetLowLevelGfx(), cColor(1,1));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperLightSpot::CreateSpecificEngineEntity()
{
	return hplNew(cIconEntityLightSpot,(this));
}
