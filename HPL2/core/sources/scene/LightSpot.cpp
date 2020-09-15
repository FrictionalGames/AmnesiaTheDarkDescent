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

#include "scene/LightSpot.h"

#include "impl/tinyXML/tinyxml.h"
#include "math/Math.h"
#include "math/Frustum.h"
#include "resources/TextureManager.h"
#include "resources/Resources.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/Camera.h"

#include "scene/World.h"
#include "scene/Scene.h"
#include "engine/Engine.h"

#include "system/String.h"

namespace hpl {
	
	static const cMatrixf g_mtxTextureUnitFix(	0.5f,0,   0,   0.5f,
												0,   0.5f,0,   0.5f,
												0,   0,   0.5f,0.5f,
												0,   0,   0,   1.0f
												);
			
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cLightSpot::cLightSpot(tString asName, cResources *apResources) : iLight(asName,apResources)
	{
		mbProjectionUpdated = true;
		mbViewProjUpdated = true;
		mbFrustumUpdated = true;
		
        mLightType = eLightType_Spot;

		mpFrustum = hplNew( cFrustum, () );

		mlViewProjMatrixCount =-1;
		mlViewMatrixCount =-1;
		mlFrustumMatrixCount =-1;

		mfFOV = cMath::ToRad(60.0f);
		mfAspect = 1.0f;
		mfNearClipPlane = 0.1f;
		mfRadius = 100.0f;

		mfTanHalfFOV = tan(mfFOV*0.5f);
		mfCosHalfFOV = cos(mfFOV*0.5f);

		mbFovUpdated = true;
		
		m_mtxView = cMatrixf::Identity;
		m_mtxViewProj = cMatrixf::Identity;
		m_mtxProjection = cMatrixf::Identity;

		mpSpotFalloffMap = mpTextureManager->Create1D("core_falloff_linear",false);
		mpSpotFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpSpotFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);

		UpdateBoundingVolume();
	}
	
	cLightSpot::~cLightSpot()
	{
		if(mpSpotFalloffMap) mpTextureManager->Destroy(mpSpotFalloffMap);

		hplDelete(mpFrustum);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cLightSpot::SetRadius(float afX)
	{ 
		mfRadius = afX;

		UpdateBoundingVolume();

		//This is so that the render container is updated.
		SetTransformUpdated();
		mbProjectionUpdated = true;
	}

	void cLightSpot::SetFOV(float afAngle)
	{ 
		mfFOV = afAngle;
		mbProjectionUpdated = true;

		mfTanHalfFOV = tan(mfFOV*0.5f);
		mfCosHalfFOV = cos(mfFOV*0.5f);
	}
	
	//-----------------------------------------------------------------------
	
	const cMatrixf& cLightSpot::GetViewMatrix()
	{
		if(mlViewMatrixCount != GetTransformUpdateCount())
		{
			mlViewMatrixCount = GetTransformUpdateCount();
			m_mtxView = cMath::MatrixInverse(GetWorldMatrix());
		}

		return m_mtxView;
	}

	//-----------------------------------------------------------------------


	const cMatrixf& cLightSpot::GetProjectionMatrix()
	{
		if(mbProjectionUpdated)
		{
			float fFar = mfRadius;
			float fNear = mfNearClipPlane;
			float fTop = tan(mfFOV*0.5f) * fNear;
			float fBottom = -fTop;
			float fRight = mfAspect * fTop;
			float fLeft = mfAspect * fBottom;

			float A = (2.0f*fNear) / (fRight - fLeft);
			float B = (2.0f*fNear) / (fTop - fBottom);
			float D = -1.0f;
			float C = -(2.0f*fFar*fNear) / (fFar - fNear);
			float Z = -(fFar + fNear)/(fFar - fNear);
			
			float X = 0;
			float Y = 0;
			
			m_mtxProjection = cMatrixf(
				A,0,X,0,
				0,B,Y,0,
				0,0,Z,C,
				0,0,D,0);

			mbProjectionUpdated = false;	
			mbViewProjUpdated = true;
			mbFrustumUpdated = true;
		}

		return m_mtxProjection;
	}

	//-----------------------------------------------------------------------
	
	const cMatrixf& cLightSpot::GetViewProjMatrix()
	{
		if(mlViewProjMatrixCount != GetTransformUpdateCount() || mbViewProjUpdated || mbProjectionUpdated)
		{
			m_mtxViewProj = cMath::MatrixMul(GetProjectionMatrix(),GetViewMatrix());
			m_mtxViewProj = cMath::MatrixMul(g_mtxTextureUnitFix, m_mtxViewProj);
			
			mlViewProjMatrixCount = GetTransformUpdateCount();
			mbViewProjUpdated = false;
		}

		return m_mtxViewProj;
	}

	//-----------------------------------------------------------------------

	cFrustum* cLightSpot::GetFrustum()
	{
		if(mlFrustumMatrixCount != GetTransformUpdateCount() || mbFrustumUpdated || mbProjectionUpdated)
		{
			mpFrustum->SetupPerspectiveProj(GetProjectionMatrix(),
											GetViewMatrix(),
											mfRadius,mfNearClipPlane,
											mfFOV,mfAspect,GetWorldPosition(),false);
			mbFrustumUpdated = false;
			mlFrustumMatrixCount = GetTransformUpdateCount();
		}

		return mpFrustum;
	}

	//-----------------------------------------------------------------------

	iTexture *cLightSpot::GetSpotFalloffMap()
	{
		return mpSpotFalloffMap;
	}

	void cLightSpot::SetSpotFalloffMap(iTexture* apTexture)
	{
		if(mpSpotFalloffMap) mpTextureManager->Destroy(mpSpotFalloffMap);

		mpSpotFalloffMap = apTexture;
		if(mpSpotFalloffMap)
		{
			mpSpotFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
			mpSpotFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);
		}
	}

	//-----------------------------------------------------------------------


	bool cLightSpot::CollidesWithBV(cBoundingVolume *apBV)
	{
		if(cMath::CheckBVIntersection(*GetBoundingVolume(), *apBV)==false) return false;

		return GetFrustum()->CollideBoundingVolume(apBV)!= eCollision_Outside;
	}
	
	//-----------------------------------------------------------------------

	bool cLightSpot::CollidesWithFrustum(cFrustum *apFrustum)
	{
		return apFrustum->CollideFrustum(GetFrustum())!=eCollision_Outside;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	
	static eTextureAnimMode GetAnimMode(const tString& asType)
	{
		if(cString::ToLowerCase(asType) == "none") return eTextureAnimMode_None;
		else if(cString::ToLowerCase(asType) == "loop") return eTextureAnimMode_Loop;
		else if(cString::ToLowerCase(asType) == "oscillate") return eTextureAnimMode_Oscillate;

		return eTextureAnimMode_None;
	}

	void cLightSpot::ExtraXMLProperties(TiXmlElement *apMainElem)
	{
		tString sTexture = cString::ToString(apMainElem->Attribute("ProjectionImage"),"");

		eTextureAnimMode animMode = GetAnimMode(cString::ToString(apMainElem->Attribute("ProjectionAnimMode"),"None"));
		float fFrameTime = cString::ToFloat(apMainElem->Attribute("ProjectionFrameTime"),1.0f);
		iTexture *pTex = NULL;

        if(animMode != eTextureAnimMode_None)
		{
			pTex = mpTextureManager->CreateAnim(sTexture,true,eTextureType_2D);
			pTex->SetAnimMode(animMode);
			pTex->SetFrameTime(fFrameTime);
		}
		else
		{
			pTex = mpTextureManager->Create2D(sTexture,true);
		}
		
		
		if(pTex)
		{
			SetGoboTexture(pTex);
		}

		mfAspect = cString::ToFloat(apMainElem->Attribute("Aspect"),mfAspect);

		mfNearClipPlane = cString::ToFloat(apMainElem->Attribute("NearClipPlane"),mfNearClipPlane);
	}

	//-----------------------------------------------------------------------

	void cLightSpot::UpdateBoundingVolume()
	{
		mBoundingVolume = GetFrustum()->GetBoundingVolume();
	}

	//-----------------------------------------------------------------------
	
}
