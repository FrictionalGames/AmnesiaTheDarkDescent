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

#include "../Common/EdObjLightSpot.h"
#include "../Common/EdObjLight.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"
//#include "EditorHelper.h"

#include "../Common/EdWindowViewport.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ICON ENTITY SPOTLIGHT : CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cIconObjectLightSpot::cIconObjectLightSpot(iEdScnObject* apParent) : iIconObjectLight(apParent, "Spot")
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// ICON ENTITY SPOTLIGHT : PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cIconObjectLightSpot::Create(const tString& asName)
{
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	SetUpLight(pWorld->CreateLightSpot(asName));

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT TYPE: CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeSpotLight::cTypeSpotLight() : iLightSubType(_W("Spot"), "SpotLight", NULL)
{
	AddFloat(eLightSpotFloat_FOV, "FOV", cMath::ToRad(60));
	AddFloat(eLightSpotFloat_Aspect, "Aspect", 1);
	AddFloat(eLightSpotFloat_NearClipPlane, "NearClipPlane", 0.1f);
	AddString(eLightSpotStr_FalloffMap, "SpotFalloffMap");

	mbScalable = false;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT TYPE: PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------


iEdObjectData* cTypeSpotLight::CreateTypeSpecificData()
{
	return hplNew(cEdObjSpotLightData,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT DATA: CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjSpotLightData::cEdObjSpotLightData(iEdObjectType *apType) : iEdObjLightData(apType)
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT DATA: PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObject* cEdObjSpotLightData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjSpotLight,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjSpotLight::cEdObjSpotLight(iEdObjectData* apData) : iEdObjLight(apData)
{
}

//---------------------------------------------------------------------------

cEdObjSpotLight::~cEdObjSpotLight()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjSpotLight::SetProperty(int alPropID, const float afX)
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
		return iEdObjLight::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjSpotLight::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eLightSpotStr_FalloffMap:
		SetSpotFalloffMap(asX);
		break;
	default:
		return iEdObjLight::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjSpotLight::GetProperty(int alPropID, float& afX)
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
		return iEdObjLight::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjSpotLight::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eLightSpotStr_FalloffMap:
		asX = GetSpotFalloffMap();
		break;
	default:
		return iEdObjLight::GetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::SetFOV(float afAngle)
{
	mfFOV = afAngle;

	static_cast<cLightSpot*>(mpEngObject->GetObject())->SetFOV(mfFOV);
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::SetAspect(float afAngle)
{
	mfAspect = afAngle;

	static_cast<cLightSpot*>(mpEngObject->GetObject())->SetAspect(mfAspect);
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::SetRadius(float afX)
{
	mfRadius = afX;

	static_cast<cLightSpot*>(mpEngObject->GetObject())->SetRadius(mfRadius);
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::SetNearClipPlane(float afX)
{
	mfNearClipPlane = afX;

	static_cast<cLightSpot*>(mpEngObject->GetObject())->SetNearClipPlane(mfNearClipPlane);
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::SetSpotFalloffMap(const tString& asFalloffMap)
{
	iTexture* pTex = NULL;
	//if(cEdHelper::LoadTextureResource(eEdTextureResourceType_1D, asFalloffMap, &pTex))
	//{
		//msSpotFalloffMap =	GetEdWorld()->GetEditor()->GetFilePathRelativeToWorkingDir(asFalloffMap);
	//}
	//else
	//{
	//	cEdHelper::LoadTextureResource(eEdTextureResourceType_1D, "core_falloff_linear", &pTex);
	//	msSpotFalloffMap = "";
	//}

	static_cast<cLightSpot*>(mpEngObject->GetObject())->SetSpotFalloffMap(pTex);	
}

//---------------------------------------------------------------------------

void cEdObjSpotLight::DrawLightTypeSpecific(const cModuleDrawData& aData)
{
	static_cast<cLightSpot*>(mpEngObject->GetObject())->GetFrustum()->Draw(aData.mpFunctions->GetLowLevelGfx(), cColor(1,1));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SPOTLIGHT : PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEngineObject* cEdObjSpotLight::CreateEngineObject()
{
	return hplNew(cIconObjectLightSpot,(this));
}
