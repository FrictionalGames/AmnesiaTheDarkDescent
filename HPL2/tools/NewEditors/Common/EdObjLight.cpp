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

#include "../Common/EdObjLight.h"

#include "../Common/EdWorld.h"
#include "../Common/EdClipPlane.h"
#include "../Common/EdResource.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"
#include "../Common/ShapeCreator.h"

#include "../Common/EdObjLightBox.h"
#include "../Common/EdObjLightSpot.h"
#include "../Common/EdObjLightPoint.h"

#include "../Common/EdEditPaneLight.h"

#include "../Common/EdObjBillboard.h"

#include <algorithm>

//------------------------------------------------------------------------------

iIconObjectLight::iIconObjectLight(iEdScnObject* apParent, const tString& asIconFile) : iIconObject(apParent, "Light" + asIconFile)
{
}

iIconObjectLight::~iIconObjectLight()
{
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	if(mpObject)
		pWorld->DestroyLight(static_cast<iLight*>(mpObject));
}

//------------------------------------------------------------------------------

void iIconObjectLight::SetUpLight(iLight* apLight)
{
	//apLight->SetRenderableUserData(mpParent);
	mpObject = apLight;
}

//------------------------------------------------------------------------------

void iIconObjectLight::Update()
{
	iEdWorld* pWorld = mpParent->GetWorld();
	iLight* pLight = static_cast<iLight*>(mpObject);
	pLight->SetVisible(mpParent->IsVisible() && mpParent->IsEnabled());// && mpParent->GetType()->I());
	
	iEdObjLight* pParent = static_cast<iEdObjLight*>(mpParent);
	pParent->UpdateFlickerParams();

	tObjectVariabilityFlag lFlags =0;
	if(pParent->GetShadowsAffectDynamic())	lFlags |= eObjectVariabilityFlag_Dynamic;
	if(pParent->GetShadowsAffectStatic())	lFlags |= eObjectVariabilityFlag_Static;
	pLight->SetShadowCastersAffected(lFlags);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT TYPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeLight::cTypeLight() : iEdScnObjType(_W("Light"), "")
{
	AddBool(eLightBool_CastShadows, "CastShadows", false);
	AddString(eLightStr_ShadowResolution, "ShadowResolution", "High");
	AddBool(eLightBool_ShadowsAffectStatic, "ShadowsAffectStatic");
	AddBool(eLightBool_ShadowsAffectDynamic, "ShadowsAffectDynamic");

	AddFloat(eLightFloat_Radius, "Radius", 1.0f);
	AddString(eLightStr_FalloffMap, "FalloffMap");
	AddString(eLightStr_Gobo, "Gobo");
	AddString(eLightStr_GoboAnimMode, "GoboAnimMode", "None");
	AddFloat(eLightFloat_GoboAnimFrameTime, "GoboAnimFrameTime");
	AddColor(eLightCol_Diffuse, "DiffuseColor", cColor(1));

	AddBool(eLightBool_FlickerActive, "FlickerActive", false);
	AddFloat(eLightFloat_FlickerOnMinLength, "FlickerOnMinLength");
	AddFloat(eLightFloat_FlickerOnMaxLength, "FlickerOnMaxLength");
	AddString(eLightStr_FlickerOnPS, "FlickerOnPS");
	AddString(eLightStr_FlickerOnSound, "FlickerOnSound");

	AddFloat(eLightFloat_FlickerOffMinLength, "FlickerOffMinLength");
	AddFloat(eLightFloat_FlickerOffMaxLength, "FlickerOffMaxLength");
	AddString(eLightStr_FlickerOffPS, "FlickerOffPS");
	AddString(eLightStr_FlickerOffSound, "FlickerOffSound");
	AddColor(eLightCol_FlickerOff, "FlickerOffColor", cColor(0));
	AddFloat(eLightFloat_FlickerOffRadius, "FlickerOffRadius");

	AddBool(eLightBool_FlickerFade, "FlickerFade", false);
	AddFloat(eLightFloat_FlickerOnFadeMinLength, "FlickerOnFadeMinLength");
	AddFloat(eLightFloat_FlickerOnFadeMaxLength, "FlickerOnFadeMaxLength");
	AddFloat(eLightFloat_FlickerOffFadeMinLength, "FlickerOffFadeMinLength");
	AddFloat(eLightFloat_FlickerOffFadeMaxLength, "FlickerOffFadeMaxLength");
}


bool cTypeLight::Init()
{
	AddChildType(hplNew(cTypeBoxLight,()));
	AddChildType(hplNew(cTypePointLight,()));
	AddChildType(hplNew(cTypeSpotLight,()));

	return true;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT SUBTYPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iLightSubType::iLightSubType(const tWString& asName, const tString& asXmlElementName, 
							 iShapeCreator* apShapeCreator) : iEdScnObjType(asName, asXmlElementName)
{
	mpShapeCreator = apShapeCreator;
}

iLightSubType::~iLightSubType()
{
	if(mpShapeCreator) hplDelete(mpShapeCreator);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// LIGHT DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjLightData::iEdObjLightData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//------------------------------------------------------------------------------


void iEdObjLightData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);
	iEdObjLight* pLight = static_cast<iEdObjLight*>(apObj);

	mlstConnectedBBIds.clear();
	std::list<cEdObjBillboard*>& lstConnectedBillboards = pLight->GetConnectedBillboards();
	std::list<cEdObjBillboard*>::iterator it = lstConnectedBillboards.begin();
	for(;it!=lstConnectedBillboards.end();++it)
	{
		cEdObjBillboard* pBB = *it;
		mlstConnectedBBIds.push_back(pBB->GetID());
	}
}


//------------------------------------------------------------------------------

void iEdObjLightData::CopyToObject(iEdObject* apObj, ePropStep aOrder)
{
	iEdObjectData::CopyToObject(apObj,aOrder);
	iEdObjLight* pLight = static_cast<iEdObjLight*>(apObj);

	iEdWorld* pWorld = mpType->GetWorld();

	if(aOrder==ePropStep_PostDeployAll)
	{
		tIntListIt it = mlstConnectedBBIds.begin();
		for(;it!=mlstConnectedBBIds.end();++it)
		{
			int lID = *it;
			cEdObjBillboard* pBB = static_cast<cEdObjBillboard*>(pWorld->GetObject(lID));

			pLight->AddConnectedBillboard(pBB);
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// LIGHT OBJECT - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEdObjLight::iEdObjLight(iEdObjectData* apData) : iEdScnObject(apData)
{
}

//------------------------------------------------------------------------------

iEdObjLight::~iEdObjLight()
{
	if(GetWorld()->IsClearing()==false)
	{
		std::list<cEdObjBillboard*>::iterator it = mlstConnectedBBs.begin();
		for(;it!=mlstConnectedBBs.end();++it)
		{
			cEdObjBillboard* pBB = *it;
			if(GetWorld()->HasObject(pBB))
			{
				pBB->SetConnectedLightName("");
				pBB->SetUpdated();
			}
		}
		mlstConnectedBBs.clear();
	}
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// LIGHT OBJECT - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool iEdObjLight::GetProperty(int alPropID, cColor& aX)
{
	if(iEdScnObject::GetProperty(alPropID, aX))
		return true;

	switch(alPropID)
	{
	case eLightCol_Diffuse:
		aX = GetDiffuseColor();
		break;
	case eLightCol_FlickerOff:
		aX = GetFlickerOffColor();
		break;
	default:
		return false;
	}

	return true;
}

bool iEdObjLight::GetProperty(int alPropID, float& afX)
{
	if(iEdScnObject::GetProperty(alPropID, afX))
		return true;

	switch(alPropID)
	{
	case eLightFloat_Radius:
		afX = GetRadius();
		break;
	case eLightFloat_GoboAnimFrameTime:
		afX = GetGoboAnimFrameTime();
		break;
	case eLightFloat_FlickerOnMinLength:
		afX = GetFlickerOnMinLength();
		break;
	case eLightFloat_FlickerOnMaxLength:
		afX = GetFlickerOnMaxLength();
		break;
	case eLightFloat_FlickerOffMinLength:
		afX = GetFlickerOffMinLength();
		break;
	case eLightFloat_FlickerOffMaxLength:
		afX = GetFlickerOffMaxLength();
		break;
	case eLightFloat_FlickerOffRadius:
		afX = GetFlickerOffRadius();
		break;
	case eLightFloat_FlickerOnFadeMinLength:
		afX = GetFlickerOnFadeMinLength();
		break;
	case eLightFloat_FlickerOnFadeMaxLength:
		afX = GetFlickerOnFadeMaxLength();
		break;
	case eLightFloat_FlickerOffFadeMinLength:
		afX = GetFlickerOffFadeMinLength();
		break;
	case eLightFloat_FlickerOffFadeMaxLength:
		afX = GetFlickerOffFadeMaxLength();
		break;
	default:
		return false;
	}


	return true;
}

bool iEdObjLight::GetProperty(int alPropID, int& alX)
{
	if(iEdScnObject::GetProperty(alPropID, alX))
		return true;

	return true;
}

bool iEdObjLight::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eLightStr_Gobo:
		asX = GetGoboFilename();
		break;
	case eLightStr_GoboAnimMode:
		asX = GetGoboAnimMode();
		break;
	case eLightStr_FalloffMap:
		asX = GetFalloffMap();
		break;
	case eLightStr_FlickerOnSound:
		asX = GetFlickerOnSound();
		break;
	case eLightStr_FlickerOffSound:
		asX = GetFlickerOffSound();
		break;
	case eLightStr_FlickerOnPS:
		asX = GetFlickerOnPS();
		break;
	case eLightStr_FlickerOffPS:
		asX = GetFlickerOffPS();
		break;
	case eLightStr_ShadowResolution:
		asX = GetShadowResolution();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool iEdObjLight::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eLightBool_FlickerActive:
		abX = GetFlickerActive();
		break;
	case eLightBool_FlickerFade:
		abX = GetFlickerFade();
		break;
	case eLightBool_CastShadows:
		abX = GetCastShadows();
		break;
	case eLightBool_ShadowsAffectStatic:
		abX = GetShadowsAffectStatic();
	case eLightBool_ShadowsAffectDynamic:
		abX = GetShadowsAffectDynamic();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool iEdObjLight::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eLightCol_Diffuse:
		SetDiffuseColor(aX);
		break;
	case eLightCol_FlickerOff:
		SetFlickerOffColor(aX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, aX);
	}

	return true;
}

bool iEdObjLight::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eLightFloat_Radius:
		SetRadius(afX);
		break;
	case eLightFloat_GoboAnimFrameTime:
		SetGoboAnimFrameTime(afX);
		break;
	case eLightFloat_FlickerOnMinLength:
		SetFlickerOnMinLength(afX);
		break;
	case eLightFloat_FlickerOnMaxLength:
		SetFlickerOnMaxLength(afX);
		break;
	case eLightFloat_FlickerOffMinLength:
		SetFlickerOffMinLength(afX);
		break;
	case eLightFloat_FlickerOffMaxLength:
		SetFlickerOffMaxLength(afX);
		break;
	case eLightFloat_FlickerOffRadius:
		SetFlickerOffRadius(afX);
		break;
	case eLightFloat_FlickerOnFadeMinLength:
		SetFlickerOnFadeMinLength(afX);
		break;
	case eLightFloat_FlickerOnFadeMaxLength:
		SetFlickerOnFadeMaxLength(afX);
		break;
	case eLightFloat_FlickerOffFadeMinLength:
		SetFlickerOffFadeMinLength(afX);
		break;
	case eLightFloat_FlickerOffFadeMaxLength:
		SetFlickerOffFadeMaxLength(afX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool iEdObjLight::SetProperty(int alPropID, const int aX)
{
	if(iEdScnObject::SetProperty(alPropID, aX))
		return true;

	return true;
}

bool iEdObjLight::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eLightStr_Gobo:
		this->SetGobo(asX);
		break;
	case eLightStr_GoboAnimMode:
		SetGoboAnimMode(asX);
		break;
	case eLightStr_FalloffMap:
		SetFalloffMap(asX);
		break;
	case eLightStr_FlickerOnSound:
		SetFlickerOnSound(asX);
		break;
	case eLightStr_FlickerOffSound:
		SetFlickerOffSound(asX);
		break;
	case eLightStr_FlickerOnPS:
		SetFlickerOnPS(asX);
		break;
	case eLightStr_FlickerOffPS:
		SetFlickerOffPS(asX);
		break;
	case eLightStr_ShadowResolution:
		SetShadowResolution(asX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}


	return true;
}

bool iEdObjLight::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eLightBool_FlickerActive:
		SetFlickerActive(abX);
		break;
	case eLightBool_FlickerFade:
		SetFlickerFade(abX);
		break;
	case eLightBool_CastShadows:
		SetCastShadows(abX);
		break;
	case eLightBool_ShadowsAffectStatic:
		SetShadowsAffectStatic(abX);
		break;
	case eLightBool_ShadowsAffectDynamic:
		SetShadowsAffectDynamic(abX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

//------------------------------------------------------------------------------

void iEdObjLight::SetCastShadows(bool abX)
{
	mbCastShadows = abX;

	static_cast<iLight*>(mpEngObject->GetObject())->SetCastShadows(abX);
}

//------------------------------------------------------------------------------

void iEdObjLight::SetShadowsAffectStatic(bool abX)
{
	mbShadowsAffectStatic = abX;
}

//------------------------------------------------------------------------------

void iEdObjLight::SetShadowsAffectDynamic(bool abX)
{
	mbShadowsAffectDynamic = abX;
}

//------------------------------------------------------------------------------

void iEdObjLight::SetRadius(float afRadius)
{
	mfRadius = afRadius;

	static_cast<iLight*>(mpEngObject->GetObject())->SetRadius(mfRadius);
}


//------------------------------------------------------------------------------

void iEdObjLight::SetDiffuseColor(const cColor& aDiffuseColor)
{
	mcolDiffuseColor =  aDiffuseColor;

	static_cast<iLight*>(mpEngObject->GetObject())->SetDiffuseColor(mcolDiffuseColor);
}

//------------------------------------------------------------------------------

void iEdObjLight::SetFlickerActive(bool abX)
{
	mbFlickerActive = abX;

	mbFlickerUpdated = true;
}

//------------------------------------------------------------------------------

void iEdObjLight::SetFlickerOnMinLength(float afX)
{
	mfFlickerOnMinLength = afX;
	if(mfFlickerOnMaxLength<afX)
		SetFlickerOnMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOnMaxLength(float afX)
{
	mfFlickerOnMaxLength = afX;
	if(mfFlickerOnMinLength>afX)
		SetFlickerOnMinLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffMinLength(float afX)
{
	mfFlickerOffMinLength = afX;
	if(mfFlickerOffMaxLength<afX)
		SetFlickerOffMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffMaxLength(float afX)
{
	mfFlickerOffMaxLength = afX;
	if(mfFlickerOffMinLength>afX)
		SetFlickerOffMinLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffRadius(float afX)
{
	mfFlickerOffRadius = afX;

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerFade(bool abX)
{
	mbFlickerFade = abX;

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOnFadeMinLength(float afX)
{
	mfFlickerOnFadeMinLength = afX;
	if(mfFlickerOnFadeMaxLength<afX)
		SetFlickerOnFadeMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOnFadeMaxLength(float afX)
{
	mfFlickerOnFadeMaxLength = afX;
	if(mfFlickerOnFadeMinLength>afX)
		SetFlickerOnFadeMinLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffFadeMinLength(float afX)
{
	mfFlickerOffFadeMinLength = afX;
	if(mfFlickerOffFadeMaxLength<afX)
		SetFlickerOffFadeMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffFadeMaxLength(float afX)
{
	mfFlickerOffFadeMaxLength = afX;
	if(mfFlickerOffFadeMinLength>afX)
		SetFlickerOffFadeMinLength(afX);

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffColor(const cColor& aCol)
{
	mcolFlickerOffColor = aCol;

	mbFlickerUpdated = true;
}


void iEdObjLight::SetFlickerOnSound(const tString& asStr)
{
	msFlickerOnSound = asStr;

	mbFlickerUpdated=true;
}

void iEdObjLight::SetFlickerOnPS(const tString& asStr)
{
	msFlickerOnPS = asStr;

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffSound(const tString& asStr)
{
	msFlickerOffSound = asStr;

	mbFlickerUpdated = true;
}

void iEdObjLight::SetFlickerOffPS(const tString& asStr)
{
	msFlickerOffPS = asStr;

	mbFlickerUpdated = true;
}

//------------------------------------------------------------------------------

void iEdObjLight::UpdateFlickerParams()
{
	if(mbFlickerUpdated==false)
		return;

	mbFlickerUpdated = false;
	/////////////////////////////////////////////
	// Reset radius and color to normal values
	SetRadius(mfRadius);
	SetDiffuseColor(mcolDiffuseColor);

	iLight* pLight = static_cast<iLight*>(mpEngObject->GetObject());

    pLight->SetFlickerActive(mbFlickerActive);
	pLight->SetFlicker(mcolFlickerOffColor, mfFlickerOffRadius,
							mfFlickerOnMinLength, mfFlickerOnMaxLength, msFlickerOnSound, msFlickerOnPS,
							mfFlickerOffMinLength, mfFlickerOffMaxLength, msFlickerOffSound, msFlickerOffPS,
							mbFlickerFade, 
							mfFlickerOnFadeMinLength, mfFlickerOnFadeMaxLength, mfFlickerOffFadeMinLength, mfFlickerOffFadeMaxLength);
}

//------------------------------------------------------------------------------

void iEdObjLight::AddConnectedBillboard(cEdObjBillboard* apBB)
{
	if(apBB==NULL || apBB->GetEngObject()==NULL)return;

	std::list<cEdObjBillboard*>::iterator it = find(mlstConnectedBBs.begin(), mlstConnectedBBs.end(), apBB);
	if(it!=mlstConnectedBBs.end()) return;

	mlstPendingConnectedBBs.push_back(apBB);
	apBB->SetConnectedLight(this);
}

void iEdObjLight::RemoveConnectedBillboard(cEdObjBillboard* apBB)
{
	if(apBB==NULL)return;
	
	mlstConnectedBBs.remove(apBB);
	iLight* pLight = static_cast<iLight*>(mpEngObject->GetObject());
	cBillboard* pBB = static_cast<cBillboard*>(apBB->GetEngObject()->GetObject());
	pLight->RemoveBillboard(pBB);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void iEdObjLight::SetFalloffMap(const tString& asFalloffMap)
{
	iTexture* pTex = NULL;

	if(cEdLoader::LoadTexture1D(asFalloffMap, &pTex))
		msFalloffMap = asFalloffMap;
	else
	{
		cEdLoader::LoadTexture1D("core_falloff_linear", &pTex);
		msFalloffMap.clear();
	}

	static_cast<iLight*>(mpEngObject->GetObject())->SetFalloffMap(pTex);
}

//------------------------------------------------------------------------------

void iEdObjLight::SetGobo(const tString& asGoboFilename)
{
	iTexture* pTex = NULL;

	if(cEdLoader::LoadTexture2D(asGoboFilename, &pTex))
		msGoboFilename = asGoboFilename;
	else
		msGoboFilename.clear();

	static_cast<iLight*>(mpEngObject->GetObject())->SetGoboTexture(pTex);	
}

//------------------------------------------------------------------------------

void iEdObjLight::SetGoboAnimMode(const tString& asX)
{
	msGoboAnimMode = asX;

	SetGobo(msGoboFilename);
}

//------------------------------------------------------------------------------

void iEdObjLight::SetGoboAnimFrameTime(float afX)
{
	mfGoboAnimFrameTime = afX;

	SetGobo(msGoboFilename);
}

//------------------------------------------------------------------------------

/*
bool iEdObjLight::EntitySpecificCheckCulled(cEdClipPlane* apPlane)
{
	cBoundingVolume* pBV = mpEngObject->GetRenderBV();

	return apPlane->PointIsOnCullingSide(pBV->GetMin()) && apPlane->PointIsOnCullingSide(pBV->GetMax());
}
*/

//------------------------------------------------------------------------------

void iEdObjLight::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);

	if(mbSelected && IsEnabled())
	{
		aData.mpFunctions->SetDepthTest(true);
		aData.mpFunctions->SetDepthWrite(false);

		DrawLightTypeSpecific(aData);
	}
}

//------------------------------------------------------------------------------

iEdEditPane* iEdObjLight::CreateEditPane()
{
	return hplNew(cEdEditPaneLight,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// LIGHT OBJECT - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void iEdObjLight::OnSetActive(bool abX)
{
	mpEngObject->Update();
}

bool iEdObjLight::OnUpdate(bool abX)
{
	if(iEdScnObject::OnUpdate(abX)==false)
		return false;

	if(mlstPendingConnectedBBs.empty()==false)
	{
		std::list<cEdObjBillboard*>::iterator it = mlstPendingConnectedBBs.begin();
		for(;it!=mlstPendingConnectedBBs.end();)
		{
			cEdObjBillboard* pBB = static_cast<cEdObjBillboard*>(*it);
			iLight* pLight = static_cast<iLight*>(mpEngObject->GetObject());
			cBillboard* pEngBB = static_cast<cBillboard*>(pBB->GetEngObject()->GetObject());

			if(pEngBB)
			{
				pLight->AttachBillboard(pEngBB, pBB->GetBillboardColor());
				mlstConnectedBBs.push_back(pBB);
				it = mlstPendingConnectedBBs.erase(it);
			}
			else
				++it;
		}
	}

	return mlstPendingConnectedBBs.empty();
}

//------------------------------------------------------------------------------
