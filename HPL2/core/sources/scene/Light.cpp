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

#include "scene/Light.h"

#include "system/String.h"

#include "impl/tinyXML/tinyxml.h"


#include "system/LowLevelSystem.h"
#include "system/Platform.h"

#include "math/Math.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/FileSearcher.h"

#include "scene/ParticleSystem.h"
#include "scene/World.h"
#include "scene/BillBoard.h"
#include "scene/SoundEntity.h"
#include "scene/MeshEntity.h"
#include "scene/RenderableContainer.h"
#include "scene/Camera.h"

#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/SubMesh.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iLight::iLight(tString asName, cResources *apResources) : iRenderable(asName)
	{
		///////////////////////////////
		//Managers and highlevel init
		mpWorld = NULL;
		mpTextureManager = apResources->GetTextureManager();
		mpFileSearcher = apResources->GetFileSearcher();

		///////////////////////////////
		//Render properties
		mbApplyTransformToBV = false;

		mShadowMapResolution = eShadowMapResolution_High;
		mfShadowMapBlurAmount = 6.0f;
		mbOcclusionCullShadowCasters = false;

		mfShadowMapBiasMul = 1;
		mfShadowMapSlopeScaleBiasMul = 1;
			
		///////////////////////////////
		//Fade and flicker init
		mDiffuseColor = 0;
		mDefaultDiffuseColor = 0;
		mSpecularColor = 0;
		mbCastShadows = false;
		mlShadowCastersAffected = eObjectVariabilityFlag_All;
		mfRadius =0;
		mfSourceRadius = 0;
		mfFadeTime=0;
		mbFlickering = false;
	
		mfFlickerStateLength = 0;

		mfFadeTime =0;

		///////////////////////////////
		//Data init
		mpFalloffMap = mpTextureManager->Create1D("core_falloff_linear",false);
		mpFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);

		mpGoboTexture = NULL;


        mpVisibleNodeTracker = hplNew( cVisibleRCNodeTracker, () );
	}

	//-----------------------------------------------------------------------

	iLight::~iLight()
	{
		if(mpVisibleNodeTracker) hplDelete(mpVisibleNodeTracker);
		if(mpFalloffMap) mpTextureManager->Destroy(mpFalloffMap);
		if(mpGoboTexture) mpTextureManager->Destroy(mpGoboTexture);
	}	

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void iLight::OnChangeVisible()
	{ 
		for(size_t i =0; i<mvBillboards.size(); ++i)
		{
			mvBillboards[i].mpBillboard->SetVisible(mbIsVisible);
		}
	}

	bool iLight::IsVisible()
	{ 
		if(mDiffuseColor.r <=0 && mDiffuseColor.g <=0 && mDiffuseColor.b <=0 && mDiffuseColor.a <=0) 
			return false;
		if(mfRadius <= 0) return false;

		return mbIsVisible; 
	}

	//-----------------------------------------------------------------------


	void iLight::SetDiffuseColor(cColor aColor)
	{
		bool bWasVisble = (mDiffuseColor.r >0 || mDiffuseColor.g >0 || mDiffuseColor.b >0 || mDiffuseColor.a >0);
		
		mDiffuseColor = aColor;

		bool bVisible = (mDiffuseColor.r >0 || mDiffuseColor.g >0 || mDiffuseColor.b >0 || mDiffuseColor.a >0);
		
		//Check if the light changed its visibility
		if(mbIsVisible && bVisible != bWasVisble && mpRenderCallback)
		{
			mpRenderCallback->OnVisibleChange(this); 
		}

		OnSetDiffuse();
	}

	//-----------------------------------------------------------------------

	void iLight::UpdateLight(float afTimeStep)
	{	
		/////////////////////////////////////////////
		// Fade
		if(mfFadeTime>0)
		{
			//Log("Fading: %f / %f\n",afTimeStep,mfFadeTime);

			float fNewRadius = mfRadius + mfRadiusAdd*afTimeStep;
			SetRadius(fNewRadius);
			
			mDiffuseColor.r += mColAdd.r*afTimeStep;
			mDiffuseColor.g += mColAdd.g*afTimeStep;
			mDiffuseColor.b += mColAdd.b*afTimeStep;
			mDiffuseColor.a += mColAdd.a*afTimeStep;
			SetDiffuseColor(mDiffuseColor);

			mfFadeTime-=afTimeStep;

			//Set the dest values.
			if(mfFadeTime<=0)
			{
				mfFadeTime =0;
				SetDiffuseColor(mDestCol);
				mfRadius = mfDestRadius;
			}
		}

		/////////////////////////////////////////////
		// Flickering
		if(mbFlickering && mfFadeTime<=0)
		{	
			//////////////////////
			//On
			if(mbFlickerOn)
			{
				if(mfFlickerTime >= mfFlickerStateLength)
				{
					mbFlickerOn = false;
					if(!mbFlickerFade)
					{
						SetDiffuseColor(mFlickerOffColor);
						SetRadius(mfFlickerOffRadius);
					}
					else
					{
						FadeTo(mFlickerOffColor,mfFlickerOffRadius, cMath::RandRectf(mfFlickerOffFadeMinLength, mfFlickerOffFadeMaxLength));
					}
					//Sound
					if(msFlickerOffSound!=""){
						cSoundEntity *pSound = mpWorld->CreateSoundEntity("FlickerOff",
																			msFlickerOffSound,true);
						if(pSound)
						{
							pSound->SetIsSaved(false);
							pSound->SetPosition(GetWorldPosition());
						}
					}

					OnFlickerOff();

					mfFlickerTime =0;
					mfFlickerStateLength = cMath::RandRectf(mfFlickerOffMinLength,mfFlickerOffMaxLength);
				}
			}
			//////////////////////
			//Off
			else 
			{
				if(mfFlickerTime >= mfFlickerStateLength)
				{
					mbFlickerOn = true;
					if(!mbFlickerFade)
					{
						SetDiffuseColor(mFlickerOnColor);
						SetRadius(mfFlickerOnRadius);
					}
					else
					{
						FadeTo(mFlickerOnColor,mfFlickerOnRadius,cMath::RandRectf(mfFlickerOnFadeMinLength, mfFlickerOnFadeMaxLength));
					}
					if(msFlickerOnSound!=""){
						cSoundEntity *pSound = mpWorld->CreateSoundEntity("FlickerOn", msFlickerOnSound,true);
						if(pSound)
						{
							pSound->SetIsSaved(false);
							pSound->SetPosition(GetWorldPosition());
						}
					}

					OnFlickerOn();

					mfFlickerTime =0;
					mfFlickerStateLength = cMath::RandRectf(mfFlickerOnMinLength,mfFlickerOnMaxLength);
				}
			}

			mfFlickerTime += afTimeStep;
		}

		/*Log("Time: %f Length: %f FadeTime: %f Color: (%f %f %f %f)\n",mfFlickerTime, mfFlickerStateLength,
		mfFadeTime,
		mDiffuseColor.r,mDiffuseColor.g,
		mDiffuseColor.b,mDiffuseColor.a);*/
	}

	//-----------------------------------------------------------------------

	void iLight::FadeTo(const cColor& aCol, float afRadius, float afTime)
	{
		if(afTime<=0) afTime = 0.0001f;

		mfFadeTime = afTime;

		mColAdd.r = (aCol.r - mDiffuseColor.r)/afTime;
		mColAdd.g = (aCol.g - mDiffuseColor.g)/afTime;
		mColAdd.b = (aCol.b - mDiffuseColor.b)/afTime;
		mColAdd.a = (aCol.a - mDiffuseColor.a)/afTime;

		mfRadiusAdd = (afRadius - mfRadius)/afTime;

		mfDestRadius = afRadius;
		mDestCol = aCol;
	}

	void iLight::StopFading()
	{
		mfFadeTime =0;
	}

	bool iLight::IsFading()
	{
		return mfFadeTime != 0;
	}

	//-----------------------------------------------------------------------

	void iLight::SetFlickerActive(bool abX)
	{
		mbFlickering = abX;
	}

	void iLight::SetFlicker(const cColor& aOffCol, float afOffRadius,
		float afOnMinLength, float afOnMaxLength,const tString &asOnSound,const tString &asOnPS,
		float afOffMinLength, float afOffMaxLength,const tString &asOffSound,const tString &asOffPS,
		bool abFade,	float afOnFadeMinLength, float afOnFadeMaxLength, 
						float afOffFadeMinLength, float afOffFadeMaxLength)
	{
		mFlickerOffColor = aOffCol;
		mfFlickerOffRadius = afOffRadius;

		mfFlickerOnMinLength = afOnMinLength;
		mfFlickerOnMaxLength = afOnMaxLength;
		msFlickerOnSound = asOnSound;
		msFlickerOnPS = asOnPS;

		mfFlickerOffMinLength = afOffMinLength;
		mfFlickerOffMaxLength = afOffMaxLength;
		msFlickerOffSound = asOffSound;
		msFlickerOffPS = asOffPS;

		mbFlickerFade = abFade;

		mfFlickerOnFadeMinLength = afOnFadeMinLength;
		mfFlickerOnFadeMaxLength = afOnFadeMaxLength;
		mfFlickerOffFadeMinLength = afOffFadeMinLength;
		mfFlickerOffFadeMaxLength = afOffFadeMaxLength;

		mFlickerOnColor = mDiffuseColor;
		mfFlickerOnRadius = mfRadius;

		mbFlickerOn = true;
		mfFlickerTime =0;

		mfFadeTime =0;

		mfFlickerStateLength = cMath::RandRectf(mfFlickerOnMinLength,mfFlickerOnMaxLength);
	}

	//-----------------------------------------------------------------------
	
	bool iLight::CheckObjectIntersection(iRenderable *apObject)
	{
		//Log("------ Checking %s with light %s -----\n",apObject->GetName().c_str(), GetName().c_str());
		//Log(" BV: min: %s max: %s\n",	apObject->GetBoundingVolume()->GetMin().ToString().c_str(),
		//								apObject->GetBoundingVolume()->GetMax().ToString().c_str());
		
		//////////////////////////////////////////////////////////////
		// If the lights cast shadows, cull objects that are in shadow
		if(mbCastShadows)
		{
			return CollidesWithBV(apObject->GetBoundingVolume());
		}
		/////////////////////////////////////////////////
		//Light is not in shadow, do not do any culling
		else
		{
			//Log("No shadow, using BV\n");
			return CollidesWithBV(apObject->GetBoundingVolume());
		}

				
	}

	//-----------------------------------------------------------------------

	void iLight::SetRadius(float afX)
	{ 
		if(mfRadius == afX) return;

		mfRadius = afX;

		mbUpdateBoundingVolume = true;
		
		//This is so that the render container is updated.
		SetTransformUpdated();
	}
	//-----------------------------------------------------------------------

	void iLight::UpdateLogic(float afTimeStep)
	{
		UpdateLight(afTimeStep);
		if(mfFadeTime>0 || mbFlickering)
		{
			mbUpdateBoundingVolume = true;
			
			//This is so that the render container is updated.
			//SetTransformUpdated();
		}
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* iLight::GetBoundingVolume()
	{
		if(mbUpdateBoundingVolume)
		{
			UpdateBoundingVolume();
			mbUpdateBoundingVolume = false;
		}

		return &mBoundingVolume;
	}
	
	//-----------------------------------------------------------------------

	cMatrixf* iLight::GetModelMatrix(cFrustum* apFrustum)
	{
		return &GetWorldMatrix();
	}
	
	//-----------------------------------------------------------------------
	
	iTexture *iLight::GetFalloffMap()
	{
		return mpFalloffMap;
	}

	void iLight::SetFalloffMap(iTexture* apTexture)
	{
		if(mpFalloffMap) mpTextureManager->Destroy(mpFalloffMap);

		mpFalloffMap = apTexture;
		mpFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);
		
	}

	//-----------------------------------------------------------------------

	void iLight::SetGoboTexture(iTexture *apTexture)
	{
		//Destroy any old texture.
		if(mpGoboTexture) mpTextureManager->Destroy(mpGoboTexture);

		mpGoboTexture = apTexture;
		if(mpGoboTexture) mpGoboTexture->SetWrapSTR(eTextureWrap_ClampToEdge);
	}

	//-----------------------------------------------------------------------

	iTexture* iLight::GetGoboTexture()
	{
		return mpGoboTexture;	
	}

	//-----------------------------------------------------------------------

	void iLight::AddShadowCaster(iRenderable *apObject)
	{
		m_mapShadowCasterCache.insert(tShadowCasterCacheMap::value_type(apObject, apObject->GetTransformUpdateCount()));
	}
	
	bool iLight::ShadowCasterIsValid(iRenderable *apObject)
	{
		tShadowCasterCacheMapIt it = m_mapShadowCasterCache.find(apObject);
		if(it == m_mapShadowCasterCache.end()) return false;

		return it->second == apObject->GetTransformUpdateCount();
	}
	
	bool iLight::ShadowCastersAreUnchanged(const tRenderableVec &avObjects)
	{
		size_t lDynObjectCount=0;
		for(size_t i=0; i<avObjects.size(); ++i)
		{
			iRenderable *pObject = avObjects[i];
			if(pObject->IsStatic()==false)
			{
				lDynObjectCount++;
				if(ShadowCasterIsValid(avObjects[i])==false)
				{
					return false;
				}
			}
		}
		
		if(lDynObjectCount != m_mapShadowCasterCache.size()) return false;
		
		return true;
	}

	void iLight::SetShadowCasterCacheFromVec(const tRenderableVec &avObjects)
	{
		m_mapShadowCasterCache.clear();

		for(size_t i=0; i<avObjects.size(); ++i)
		{
			if(avObjects[i]->IsStatic()==false) AddShadowCaster(avObjects[i]);
		}
	}
	
	void iLight::ClearShadowCasterCache()
	{
		m_mapShadowCasterCache.clear();
	}

	//-----------------------------------------------------------------------

	void iLight::LoadXMLProperties(const tString asFile)
	{
		tWString sPath = mpFileSearcher->GetFilePath(asFile);
		if(sPath != _W(""))
		{
			FILE *pFile = cPlatform::OpenFile(sPath, _W("rb"));
			if(pFile==NULL) return;

			TiXmlDocument *pDoc = hplNew( TiXmlDocument,() );
			if(pDoc->LoadFile(pFile))
			{
				TiXmlElement *pRootElem = pDoc->RootElement();
				
                TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
				if(pMainElem!=NULL)
				{
					mbCastShadows = cString::ToBool(pMainElem->Attribute("CastsShadows"),mbCastShadows);

					mDiffuseColor.a = cString::ToFloat(pMainElem->Attribute("Specular"),mDiffuseColor.a);
					
					tString sFalloffImage = cString::ToString(pMainElem->Attribute("FalloffImage"),"");
					iTexture *pTexture = mpTextureManager->Create1D(sFalloffImage,false);
					if(pTexture) SetFalloffMap(pTexture);

					ExtraXMLProperties(pMainElem);
				}
				else
				{
					Error("Cannot find main element in %s\n",asFile.c_str());
				}
			}
			else
			{
				Error("Couldn't load file '%s'\n",asFile.c_str());
			}
			if(pFile) fclose(pFile);
			hplDelete(pDoc);
		}
		else
		{
			Error("Couldn't find file '%s'\n",asFile.c_str());
		}

	}

	//-----------------------------------------------------------------------

	void iLight::AttachBillboard(cBillboard *apBillboard, const cColor &aBaseColor)
	{
		cLightBillboardConnection bbConnection;

		bbConnection.mpBillboard = apBillboard;
		bbConnection.mBaseColor = aBaseColor;

		apBillboard->SetColor(aBaseColor * cColor(mDiffuseColor.r,mDiffuseColor.g,mDiffuseColor.b,1));
		apBillboard->SetVisible(IsVisible());

		mvBillboards.push_back(bbConnection);
	}

	//-----------------------------------------------------------------------

	void iLight::RemoveBillboard(cBillboard *apBillboard)
	{
		std::vector<cLightBillboardConnection>::iterator it = mvBillboards.begin();
		for(; it != mvBillboards.end(); ++it)
		{
			cLightBillboardConnection &bbConnection = *it;
			if(bbConnection.mpBillboard == apBillboard)
			{
				mvBillboards.erase(it);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------


	
	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iLight::RenderShadow(iRenderable *apObject,cRenderSettings *apRenderSettings,
				iLowLevelGraphics *apLowLevelGraphics)
	{
		
	}


	//-----------------------------------------------------------------------
	
	void iLight::OnFlickerOff()
	{
		//Particle system
		if(msFlickerOffPS!=""){
			cParticleSystem *pPS = mpWorld->CreateParticleSystem(GetName() + "_PS", msFlickerOffPS, cVector3f(1,1,1));
			if(pPS) pPS->SetMatrix(GetWorldMatrix());
		}
	}

	//-----------------------------------------------------------------------

	void iLight::OnFlickerOn()
	{
		//Particle system
		if(msFlickerOnPS!=""){
			cParticleSystem *pPS = mpWorld->CreateParticleSystem(GetName() + "_PS", msFlickerOnPS, cVector3f(1,1,1));
			if(pPS) pPS->SetMatrix(GetWorldMatrix());
		}

	}

	//-----------------------------------------------------------------------

	void iLight::OnSetDiffuse()
	{
		for(size_t i =0; i<mvBillboards.size(); ++i)
		{
			mvBillboards[i].mpBillboard->SetColor( mvBillboards[i].mBaseColor * cColor(mDiffuseColor.r,mDiffuseColor.g,mDiffuseColor.b,1));
		}
	}

	
	//-----------------------------------------------------------------------

}
