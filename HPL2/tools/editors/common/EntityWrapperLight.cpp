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

#include "EntityWrapperLight.h"

#include "EditorWorld.h"
#include "EditorClipPlane.h"
#include "EditorHelper.h"
#include "EditorBaseClasses.h"

#include "EditorWindowViewport.h"

#include "EditorWindowEntityEditBoxLight.h"

#include "EntityWrapperBillboard.h"

#include <algorithm>

//------------------------------------------------------------------------------

bool iEntityWrapperTypeLight::mbLightsVisible = true; 
bool iEntityWrapperTypeLight::mbLightsActive = true; 

//------------------------------------------------------------------------------

iIconEntityLight::iIconEntityLight(iEntityWrapper* apParent, const tString& asIconFile) : iIconEntity(apParent, "Light" + asIconFile)
{
}

iIconEntityLight::~iIconEntityLight()
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	if(mpEntity)
		pWorld->DestroyLight((iLight*)mpEntity);
}

//------------------------------------------------------------------------------

void iIconEntityLight::Update()
{
	iEditorWorld* pWorld = mpParent->GetEditorWorld();
	iLight* pLight = (iLight*)mpEntity;
	pLight->SetVisible(mpParent->IsVisible() && mpParent->IsActive() && mpParent->GetType()->IsActive());
	
	iEntityWrapperLight* pParent = (iEntityWrapperLight*)mpParent;
	pParent->UpdateFlickerParams();

	tObjectVariabilityFlag lFlags =0;
	if(pParent->GetShadowsAffectDynamic())	lFlags |= eObjectVariabilityFlag_Dynamic;
	if(pParent->GetShadowsAffectStatic())	lFlags |= eObjectVariabilityFlag_Static;
	pLight->SetShadowCastersAffected(lFlags);
}

//------------------------------------------------------------------------------

void iEntityWrapperTypeLight::SetVisible(bool abX)
{
	if(mbLightsVisible==abX)
		return; 

	mbLightsVisible = abX; 
	mpWorld->SetVisibilityUpdated(); 
}

//------------------------------------------------------------------------------

void iEntityWrapperTypeLight::SetActive(bool abX)
{
	if(mbLightsActive==abX)
		return; 

	mbLightsActive = abX; 
	mpWorld->SetVisibilityUpdated(); 
}

//------------------------------------------------------------------------------

iEntityWrapperDataLight::iEntityWrapperDataLight(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//------------------------------------------------------------------------------


void iEntityWrapperDataLight::CopyFromEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyFromEntity(apEntity);
	iEntityWrapperLight* pLight = (iEntityWrapperLight*)apEntity;

	tEntityWrapperList lstBBs;
	std::list<cEntityWrapperBillboard*>& lstConnectedBillboards = pLight->GetConnectedBillboards();
	lstBBs.insert(lstBBs.end(), lstConnectedBillboards.begin(), lstConnectedBillboards.end());

	cEditorHelper::GetIDsFromEntityList(lstBBs, mlstConnectedBBIds);
}


//------------------------------------------------------------------------------

void iEntityWrapperDataLight::CopyToEntity(iEntityWrapper* apEntity, int alCopyFlags)
{
	iEntityWrapperData::CopyToEntity(apEntity,alCopyFlags);
	iEntityWrapperLight* pLight = (iEntityWrapperLight*)apEntity;

	if(alCopyFlags==ePropCopyStep_PostDeployAll)
	{
		tIntListIt it = mlstConnectedBBIds.begin();
		for(;it!=mlstConnectedBBIds.end();++it)
		{
			int lID = *it;
			pLight->AddConnectedBillboard((cEntityWrapperBillboard*)mpType->GetWorld()->GetEntity(lID));
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEntityWrapperLight::iEntityWrapperLight(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
}

//------------------------------------------------------------------------------

iEntityWrapperLight::~iEntityWrapperLight()
{
	if(GetEditorWorld()->IsClearingEntities()==false)
	{
		std::list<cEntityWrapperBillboard*>::iterator it = mlstConnectedBBs.begin();
		for(;it!=mlstConnectedBBs.end();++it)
		{
			cEntityWrapperBillboard* pBB = *it;
			if(GetEditorWorld()->HasEntity(pBB))
			{
				pBB->SetConnectedLightName("");
				pBB->UpdateEntity();
			}
		}
		mlstConnectedBBs.clear();
	}

}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool iEntityWrapperLight::GetProperty(int alPropID, cColor& aX)
{
	if(iEntityWrapper::GetProperty(alPropID, aX))
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

bool iEntityWrapperLight::GetProperty(int alPropID, float& afX)
{
	if(iEntityWrapper::GetProperty(alPropID, afX))
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

bool iEntityWrapperLight::GetProperty(int alPropID, int& alX)
{
	if(iEntityWrapper::GetProperty(alPropID, alX))
		return true;

	return true;
}

bool iEntityWrapperLight::GetProperty(int alPropID, tString& asX)
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
		return iEntityWrapper::GetProperty(alPropID, asX);
	}

	return true;
}

bool iEntityWrapperLight::GetProperty(int alPropID, bool& abX)
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
		return iEntityWrapper::GetProperty(alPropID, abX);
	}

	return true;
}

bool iEntityWrapperLight::SetProperty(int alPropID, const cColor& aX)
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
		return iEntityWrapper::SetProperty(alPropID, aX);
	}

	return true;
}

bool iEntityWrapperLight::SetProperty(int alPropID, const float& afX)
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
		return iEntityWrapper::SetProperty(alPropID, afX);
	}

	return true;
}

bool iEntityWrapperLight::SetProperty(int alPropID, const int& aX)
{
	if(iEntityWrapper::SetProperty(alPropID, aX))
		return true;

	return true;
}

bool iEntityWrapperLight::SetProperty(int alPropID, const tString& asX)
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
		return iEntityWrapper::SetProperty(alPropID, asX);
	}


	return true;
}

bool iEntityWrapperLight::SetProperty(int alPropID, const bool& abX)
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
		return iEntityWrapper::SetProperty(alPropID, abX);
	}

	return true;
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetCastShadows(bool abX)
{
	mbCastShadows = abX;

	((iLight*)mpEngineEntity->GetEntity())->SetCastShadows(abX);
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetShadowsAffectStatic(bool abX)
{
	mbShadowsAffectStatic = abX;
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetShadowsAffectDynamic(bool abX)
{
	mbShadowsAffectDynamic = abX;
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetRadius(float afRadius)
{
	mfRadius = afRadius;

	((iLight*)mpEngineEntity->GetEntity())->SetRadius(mfRadius);
}


//------------------------------------------------------------------------------

void iEntityWrapperLight::SetDiffuseColor(const cColor& aDiffuseColor)
{
	mcolDiffuseColor =  aDiffuseColor;

	((iLight*)mpEngineEntity->GetEntity())->SetDiffuseColor(mcolDiffuseColor);
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetFlickerActive(bool abX)
{
	mbFlickerActive = abX;

	mbFlickerUpdated = true;
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetFlickerOnMinLength(float afX)
{
	mfFlickerOnMinLength = afX;
	if(mfFlickerOnMaxLength<afX)
		SetFlickerOnMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOnMaxLength(float afX)
{
	mfFlickerOnMaxLength = afX;
	if(mfFlickerOnMinLength>afX)
		SetFlickerOnMinLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffMinLength(float afX)
{
	mfFlickerOffMinLength = afX;
	if(mfFlickerOffMaxLength<afX)
		SetFlickerOffMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffMaxLength(float afX)
{
	mfFlickerOffMaxLength = afX;
	if(mfFlickerOffMinLength>afX)
		SetFlickerOffMinLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffRadius(float afX)
{
	mfFlickerOffRadius = afX;

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerFade(bool abX)
{
	mbFlickerFade = abX;

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOnFadeMinLength(float afX)
{
	mfFlickerOnFadeMinLength = afX;
	if(mfFlickerOnFadeMaxLength<afX)
		SetFlickerOnFadeMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOnFadeMaxLength(float afX)
{
	mfFlickerOnFadeMaxLength = afX;
	if(mfFlickerOnFadeMinLength>afX)
		SetFlickerOnFadeMinLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffFadeMinLength(float afX)
{
	mfFlickerOffFadeMinLength = afX;
	if(mfFlickerOffFadeMaxLength<afX)
		SetFlickerOffFadeMaxLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffFadeMaxLength(float afX)
{
	mfFlickerOffFadeMaxLength = afX;
	if(mfFlickerOffFadeMinLength>afX)
		SetFlickerOffFadeMinLength(afX);

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffColor(const cColor& aCol)
{
	mcolFlickerOffColor = aCol;

	mbFlickerUpdated = true;
}


void iEntityWrapperLight::SetFlickerOnSound(const tString& asStr)
{
	msFlickerOnSound = asStr;

	mbFlickerUpdated=true;
}

void iEntityWrapperLight::SetFlickerOnPS(const tString& asStr)
{
	msFlickerOnPS = asStr;

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffSound(const tString& asStr)
{
	msFlickerOffSound = asStr;

	mbFlickerUpdated = true;
}

void iEntityWrapperLight::SetFlickerOffPS(const tString& asStr)
{
	msFlickerOffPS = asStr;

	mbFlickerUpdated = true;
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::UpdateFlickerParams()
{
	if(mbFlickerUpdated==false)
		return;

	mbFlickerUpdated = false;
	/////////////////////////////////////////////
	// Reset radius and color to normal values
	SetRadius(mfRadius);
	SetDiffuseColor(mcolDiffuseColor);

	iLight* pLight = (iLight*)mpEngineEntity->GetEntity();

    pLight->SetFlickerActive(mbFlickerActive);
	pLight->SetFlicker(mcolFlickerOffColor, mfFlickerOffRadius,
							mfFlickerOnMinLength, mfFlickerOnMaxLength, msFlickerOnSound, msFlickerOnPS,
							mfFlickerOffMinLength, mfFlickerOffMaxLength, msFlickerOffSound, msFlickerOffPS,
							mbFlickerFade, mfFlickerOnFadeMinLength, mfFlickerOnFadeMaxLength, mfFlickerOffFadeMinLength, mfFlickerOffFadeMaxLength);
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::AddConnectedBillboard(cEntityWrapperBillboard* apBB)
{
	if(apBB==NULL || apBB->GetEngineEntity()==NULL)return;

	std::list<cEntityWrapperBillboard*>::iterator it = find(mlstConnectedBBs.begin(), mlstConnectedBBs.end(), apBB);
	if(it!=mlstConnectedBBs.end()) return;

	mlstConnectedBBs.push_back(apBB);
	apBB->SetConnectedLight(this);

	((iLight*)mpEngineEntity->GetEntity())->AttachBillboard((cBillboard*)apBB->GetEngineEntity()->GetEntity(), apBB->GetBillboardColor());
}

void iEntityWrapperLight::RemoveConnectedBillboard(cEntityWrapperBillboard* apBB)
{
	if(apBB==NULL)return;
	
	mlstConnectedBBs.remove(apBB);
	((iLight*)mpEngineEntity->GetEntity())->RemoveBillboard((cBillboard*)apBB->GetEngineEntity()->GetEntity());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetFalloffMap(const tString& asFalloffMap)
{
	iTexture* pTex = NULL;

	cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, asFalloffMap, &pTex);
	if(pTex)
		msFalloffMap = cString::To8Char(GetEditorWorld()->GetEditor()->GetPathRelToWD(asFalloffMap));
	else
	{
		cEditorHelper::LoadTextureResource(eEditorTextureResourceType_1D, "core_falloff_linear", &pTex);
		msFalloffMap = "";
	}

	((iLight*)mpEngineEntity->GetEntity())->SetFalloffMap(pTex);	
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetGobo(const tString& asGoboFilename)
{
	iTexture* pTex = NULL;

	cEditorHelper::LoadTextureResource(eEditorTextureResourceType_2D, asGoboFilename, &pTex, msGoboAnimMode, mfGoboAnimFrameTime);
	if(pTex)
		msGoboFilename = asGoboFilename;
	else
		msGoboFilename = "";

	((iLight*)mpEngineEntity->GetEntity())->SetGoboTexture(pTex);	
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetGoboAnimMode(const tString& asX)
{
	msGoboAnimMode = asX;

	SetGobo(msGoboFilename);
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::SetGoboAnimFrameTime(float afX)
{
	mfGoboAnimFrameTime = afX;

	SetGobo(msGoboFilename);
}

//------------------------------------------------------------------------------

bool iEntityWrapperLight::EntitySpecificCheckCulled(cEditorClipPlane* apPlane)
{
	cBoundingVolume* pBV = mpEngineEntity->GetRenderBV();

	return apPlane->PointIsOnCullingSide(pBV->GetMin()) && apPlane->PointIsOnCullingSide(pBV->GetMax());
}

//------------------------------------------------------------------------------

void iEntityWrapperLight::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected, aHighlightCol);

	if(IsActive() && abIsSelected)
	{
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(false);
		DrawLightTypeSpecific(apViewport, apFunctions, apEditMode, abIsSelected);
	}
}

//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* iEntityWrapperLight::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxLight,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void iEntityWrapperLight::OnSetActive(bool abX)
{
	mpEngineEntity->Update();
}
