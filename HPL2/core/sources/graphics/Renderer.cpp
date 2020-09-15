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

#include "graphics/Renderer.h"

#include "math/Math.h"
#include "math/BoundingVolume.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"
#include "system/String.h"

#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/MeshCreator.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Renderable.h"
#include "graphics/RenderList.h"
#include "graphics/FrameBuffer.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/ProgramComboManager.h"
#include "graphics/GPUProgram.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/OcclusionQuery.h"

#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/TextureManager.h"
#include "resources/GpuShaderManager.h"
#include "resources/MeshManager.h"

#include "scene/Camera.h"
#include "scene/World.h"
#include "scene/RenderableContainer.h"
#include "scene/LightSpot.h"
#include "scene/LightPoint.h"
#include "scene/LightBox.h"
#include "scene/SubMeshEntity.h"
#include "scene/FogArea.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// STATIC VARAIBLES
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	eShadowMapQuality iRenderer::mShadowMapQuality	=		eShadowMapQuality_Medium;
	eShadowMapResolution iRenderer::mShadowMapResolution	=	eShadowMapResolution_High;
	eParallaxQuality iRenderer::mParallaxQuality = eParallaxQuality_Low;
	bool iRenderer::mbParallaxEnabled=true;
	int iRenderer::mlReflectionSizeDiv = 2;
	bool iRenderer::mbRefractionEnabled=true;

	//-----------------------------------------------------------------------

	int iRenderer::mlRenderFrameCount =0;
	
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// RENDERER SORTING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cRendererNodeSortFunc::operator()(iRenderableContainerNode* apNodeA, iRenderableContainerNode* apNodeB)
	{
		if(apNodeA->IsInsideView() != apNodeB->IsInsideView())
		{
			return apNodeA->IsInsideView() > apNodeB->IsInsideView();
		}

		return apNodeA->GetViewDistance() >	apNodeB->GetViewDistance();
	}

	//-----------------------------------------------------------------------
		
	//////////////////////////////////////////////////////////////////////////
	// RENDER SETTINGS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderSettings::cRenderSettings(bool abIsReflection)
	{
		////////////////////////
		// Create data
		mpRenderList = hplNew( cRenderList, () );
	
		mpVisibleNodeTracker = hplNew( cVisibleRCNodeTracker, () );

		////////////////////////
		// Setup data
		mlCurrentOcclusionObject = 0;
		
		////////////////////////
		// Set up General Variables
		mbIsReflection = abIsReflection;
		mbLog = false;

		mClearColor = cColor(0,0);

		////////////////////////
		// Set up Render Variables
		//Change this later I assume:
		mlMinimumObjectsBeforeOcclusionTesting = 0;//8;//8 should be good default, giving a good amount of colliders, or? Clarifiction: Minium num of object rendered until node visibility tests start!
		mlSampleVisiblilityLimit = 3;

		mbUseCallbacks = true;

		mbUseEdgeSmooth = false;

		mbUseOcclusionCulling = true;

		mMaxShadowMapResolution = eShadowMapResolution_High;
		if(mbIsReflection)
			mMaxShadowMapResolution = eShadowMapResolution_Medium;

		mbClipReflectionScreenRect = true;

		mbUseScissorRect = false;

		mbRenderWorldReflection = true;

		////////////////////////
		// Set up Shadow Variables
		mbRenderShadows = true;
		mfShadowMapBias = 4;			//The constant bias
		mfShadowMapSlopeScaleBias = 2;	//The bias based on sloping of depth.

		////////////////////////////
		//Light settings
		mbSSAOActive = mbIsReflection ? false : true;
		
		////////////////////////
		// Set up Output Variables
		mlNumberOfLightsRendered =0;
		mlNumberOfOcclusionQueries =0;

		////////////////////////
		// Set up Private Variables


		////////////////////////
		// Create Reflection settings
		mpReflectionSettings = NULL;
		if(mbIsReflection == false)
		{
			mpReflectionSettings = hplNew(cRenderSettings, (true) );
		}

	}

	cRenderSettings::~cRenderSettings()
	{
		hplDelete(mpRenderList);
		hplDelete(mpVisibleNodeTracker);
		
		STLDeleteAll(mvOcclusionObjectPool);

		for(size_t i=0; i<mvLightOcclusionPairs.size(); ++i)
		{
			if(mvLightOcclusionPairs[i].mpQuery)
				hplDelete(mvLightOcclusionPairs[i].mpQuery);
		}

		if(mpReflectionSettings) hplDelete(mpReflectionSettings);
	}

	//-----------------------------------------------------------------------

	void cRenderSettings::ResetVariables()
	{
		//return;
		mpVisibleNodeTracker->Reset();

        if(mpReflectionSettings) mpReflectionSettings->ResetVariables();		
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////
	//The render settings will use the default setup, except for the variables below
	// This means SSAO, edgesmooth, etc are always off for reflections.
	#define RenderSettingsCopy(aVar) mpReflectionSettings->aVar = aVar
	void cRenderSettings::SetupReflectionSettings()
	{
		if(mpReflectionSettings==NULL) return;
		RenderSettingsCopy(mbLog);

		RenderSettingsCopy(mClearColor);

		////////////////////////////
		//Render settings
		RenderSettingsCopy(mlMinimumObjectsBeforeOcclusionTesting);
		RenderSettingsCopy(mlSampleVisiblilityLimit);

		mpReflectionSettings->mbUseScissorRect = false;
		
		////////////////////////////
		//Shadow settings
		RenderSettingsCopy(mbRenderShadows);
		RenderSettingsCopy(mfShadowMapBias);
		RenderSettingsCopy(mfShadowMapSlopeScaleBias);

		////////////////////////////
		//Light settings
		//RenderSettingsCopy(mbSSAOActive);

		////////////////////////////
		//Output
		RenderSettingsCopy(mlNumberOfLightsRendered);
		RenderSettingsCopy(mlNumberOfOcclusionQueries);
	}

	//-----------------------------------------------------------------------

	void cRenderSettings::AddOcclusionPlane(const cPlanef &aPlane)
	{
		mvOcclusionPlanes.push_back(aPlane);
	}
	
	//-----------------------------------------------------------------------

	void cRenderSettings::ResetOcclusionPlanes()
	{
		mvOcclusionPlanes.clear();
	}


	//-----------------------------------------------------------------------

	void cRenderSettings::AssignOcclusionObject(iRenderer *apRenderer, void *apSource, int alCustomIndex, iVertexBuffer *apVtxBuffer, cMatrixf *apMatrix, bool abDepthTest)
	{
		if(mlCurrentOcclusionObject == mvOcclusionObjectPool.size())
		{
			mvOcclusionObjectPool.push_back(hplNew(cOcclusionQueryObject, () ));
		}

		cOcclusionQueryObject *pObject = mvOcclusionObjectPool[mlCurrentOcclusionObject];

		pObject->mlCustomID = alCustomIndex;
		pObject->mpQuery = apRenderer->GetOcclusionQuery();
		pObject->mpVtxBuffer = apVtxBuffer;
		pObject->mpMatrix = apMatrix;
		pObject->mbDepthTest = abDepthTest;

		m_setOcclusionObjects.insert(tOcclusionQueryObjectMap::value_type(apSource, pObject));

		++mlCurrentOcclusionObject;
	}
	
	//-----------------------------------------------------------------------


	int cRenderSettings::RetrieveOcclusionObjectSamples(iRenderer *apRenderer, void *apSource, int alCustomIndex)
	{
		tOcclusionQueryObjectMapIt it = m_setOcclusionObjects.find(apSource);
		if(it == m_setOcclusionObjects.end()){
			if(mbLog) Log(" Could not find source %d custom index %d in occlusion objects set!\n", apSource, alCustomIndex);
			return 0;
		}

		//////////////////////////////////////
		//Get the number of objects with key and get the one with right custom ID
		size_t lCount = m_setOcclusionObjects.count(apSource);
		cOcclusionQueryObject* pObject = NULL;
		for(size_t i=0; i < lCount; ++i)
		{
			cOcclusionQueryObject* pTestObject = it->second;
			if(pTestObject->mlCustomID == alCustomIndex)
			{
				pObject = pTestObject;
				break;
			}
			it++;
		}
		if(pObject==NULL)
		{
			if(mbLog) Log(" Found source %d but could NOT find custom index %d in occlusion objects set!\n", apSource, alCustomIndex);
			return 0;
		}

		//////////////////////////////////////
		// Get the query and wait (if needed) for result to come in.
		iOcclusionQuery *pQuery = pObject->mpQuery;
		
		//If query is null, then samples have already been retrieved.
		if(pQuery==NULL) return pObject->mlSampleResults;
		
		while(pQuery->FetchResults()==false);

		pObject->mlSampleResults = pQuery->GetSampleCount();
		pObject->mpQuery = NULL;
		apRenderer->ReleaseOcclusionQuery(pQuery);

		return pObject->mlSampleResults;
	}

	//-----------------------------------------------------------------------

	void cRenderSettings::WaitAndRetrieveAllOcclusionQueries(iRenderer *apRenderer)
	{
		for(int i=0; i<mlCurrentOcclusionObject; ++i)
		{
			iOcclusionQuery *pQuery = mvOcclusionObjectPool[i]->mpQuery;
			if(pQuery==NULL) continue;
			
            while(pQuery->FetchResults()==false);

			mvOcclusionObjectPool[i]->mlSampleResults = pQuery->GetSampleCount();
			mvOcclusionObjectPool[i]->mpQuery = NULL;
			apRenderer->ReleaseOcclusionQuery(pQuery);
		}
	}

	//-----------------------------------------------------------------------

	void cRenderSettings::ClearOcclusionObjects(iRenderer *apRenderer)
	{
		if(mbLog) Log(" Clearing occlusion queries i settings!\n");
		m_setOcclusionObjects.clear();
		for(int i=0; i<mlCurrentOcclusionObject; ++i)
		{
			iOcclusionQuery *pQuery = mvOcclusionObjectPool[i]->mpQuery;
			if(pQuery==NULL) continue;

			apRenderer->ReleaseOcclusionQuery(pQuery);
			mvOcclusionObjectPool[i]->mpQuery = NULL;
		}

		mlCurrentOcclusionObject = 0;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SHADOWMAP CACHE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cShadowMapLightCache::SetFromLight(iLight* apLight)
	{
		mpLight = apLight;
		mlTransformCount = apLight->GetTransformUpdateCount();
		mfRadius = apLight->GetRadius();
		
		if(apLight->GetLightType() == eLightType_Spot)
		{
			cLightSpot *pSpotLight = static_cast<cLightSpot*>(apLight);
			mfAspect = pSpotLight->GetAspect();
			mfFOV = pSpotLight->GetFOV();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iRenderer::iRenderer(const tString& asName, cGraphics *apGraphics,cResources* apResources, int alNumOfProgramComboModes)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		//////////////
		//Set variables from arguments
		msName = asName;

		mpShaderManager = mpResources->GetGpuShaderManager();

		/////////////////////////////////
		//Set up the render functions
		SetupRenderFunctions(mpGraphics->GetLowLevel());

		////////////////////////
		// Debug Variables
		mbOnlyRenderPrevVisibleOcclusionObjects = false;
		mlOnlyRenderPrevVisibleOcclusionObjectsFrameCount =0;

		//////////////
		//Create data classes
		mpProgramManager  = hplNew(	cProgramComboManager, (msName, mpGraphics, mpResources, alNumOfProgramComboModes));

		//////////////
		//Init variables
		mfDefaultAlphaLimit = 0.5f;

		mbSetFrameBufferAtBeginRendering = true;
		mbClearFrameBufferAtBeginRendering = true;
		mbSetupOcclusionPlaneForFog = false;

		mfScissorLastFov =0;
		mfScissorLastTanHalfFov =0;

		mpCallbackFunctions = hplNew( cRendererCallbackFunctions, (this) );

		mfTimeCount =0;

		mlActiveOcclusionQueryNum =0;

		//////////////
		// Create programs
		cParserVarContainer vars;
		
		mpDepthOnlyProgram = mpProgramManager->CreateProgramFromShaders("DepthOnly",
											"deferred_base_vtx.glsl",
											"deferred_depthonly_frag.glsl",
											&vars,false);

		
		////////////
		// Create shapes
		//  Color and Texture because Geforce cards fail without it, no idea why...
		mpShapeBox = LoadVertexBufferFromMesh("core_box.dae", eVertexElementFlag_Position | eVertexElementFlag_Texture0| eVertexElementFlag_Color0);
	}

	//-----------------------------------------------------------------------

	iRenderer::~iRenderer()
	{
		DestroyShadowMaps();

		STLDeleteAll(mvOcclusionQueryPool);

		if(mpShapeBox) hplDelete(mpShapeBox);

		hplDelete(mpCallbackFunctions);
		hplDelete(mpProgramManager);
		
		hplDelete(mpDepthOnlyProgram);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iRenderer::Render(float afFrameTime,cFrustum *apFrustum, cWorld *apWorld, cRenderSettings *apSettings, cRenderTarget *apRenderTarget,
							bool abSendFrameBufferToPostEffects,tRendererCallbackList *apCallbackList)
	{
		BeginRendering(afFrameTime,apFrustum, apWorld, apSettings,apRenderTarget,abSendFrameBufferToPostEffects,apCallbackList);

		SetupRenderList();
        RenderObjects();
		
        EndRendering();
	}

	//-----------------------------------------------------------------------

	void iRenderer::Update(float afTimeStep)
	{
		mfTimeCount += afTimeStep;
	}

	//-----------------------------------------------------------------------

	iTexture* iRenderer::GetPostEffectTexture()
	{
		if(mpCurrentRenderTarget->mpFrameBuffer)
		{
			iFrameBufferAttachment *pAttachement = mpCurrentRenderTarget->mpFrameBuffer->GetColorBuffer(0);
            iTexture *pTexture = static_cast<iTexture*>(pAttachement);
			
			return pTexture;
		}
		else
		{
			//TODO: Copy from frambuffer to texture. But what texture?
			return NULL;
		}
	}

	//-----------------------------------------------------------------------

	void iRenderer::AssignOcclusionObject(void *apSource, int alCustomIndex, iVertexBuffer *apVtxBuffer, cMatrixf *apMatrix, bool abDepthTest)
	{
		if(mbLog) Log("  Creating occlusion object from source %d with custom ID: %d . VtxBuffer: %d Matrix: %d Depthtest: %d\n", apSource, alCustomIndex, apVtxBuffer, apMatrix, abDepthTest);

		mpCurrentSettings->AssignOcclusionObject(this, apSource, alCustomIndex, apVtxBuffer, apMatrix, abDepthTest);
	}
	
	int iRenderer::RetrieveOcclusionObjectSamples(void *apSource, int alCustomIndex)
	{
		int lSamples = mpCurrentSettings->RetrieveOcclusionObjectSamples(this, apSource, alCustomIndex);	

		if(mbLog) Log("  Retrieved %d samples from occlusion object with source %d and custom ID: %d.\n",lSamples,apSource, alCustomIndex);

		return lSamples;
	}

	void iRenderer::WaitAndRetrieveAllOcclusionQueries()
	{
		if(mbLog) Log("  Retrieving sample count for all active occlusion queries.\n");

		mpCurrentSettings->WaitAndRetrieveAllOcclusionQueries(this);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iRenderer::BeginRendering(	float afFrameTime,cFrustum *apFrustum, cWorld *apWorld, cRenderSettings *apSettings, cRenderTarget *apRenderTarget,
									bool abSendFrameBufferToPostEffects,tRendererCallbackList *apCallbackList, bool abAtStartOfRendering)
	{
		if(apSettings->mbLog)
		{
			Log("Start Rendering Frustum: %d, World: '%s' TargetBuffer %d\n", apFrustum, apWorld->GetName().c_str(), apRenderTarget->mpFrameBuffer);
			Log("-----------------  START -------------------------\n");
		}

		//////////////////////////////////////////
		//Set up variables
		mfCurrentFrameTime = afFrameTime;
		mpCurrentWorld = apWorld;
		mpCurrentSettings = apSettings;
		mpCurrentRenderList = apSettings->mpRenderList;

		mbSendFrameBufferToPostEffects = abSendFrameBufferToPostEffects;
		mpCallbackList = apCallbackList;

		mbOcclusionPlanesActive = true;

		////////////////////////////////
		//Initialize render functions
		InitAndResetRenderFunctions(apFrustum, apRenderTarget, apSettings->mbLog, 
									apSettings->mbUseScissorRect, apSettings->mvScissorRectPos, apSettings->mvScissorRectSize);


		////////////////////////////////
		//Set up near plane variables

		//Calculate radius for near plane so that it is always inside it.
		float fTanHalfFOV = tan(mpCurrentFrustum->GetFOV()*0.5f);

		float fNearPlane = mpCurrentFrustum->GetNearPlane();
		mfCurrentNearPlaneTop =  fTanHalfFOV * fNearPlane;
		mfCurrentNearPlaneRight = mpCurrentFrustum->GetAspect() * mfCurrentNearPlaneTop;


		/////////////////////////////////////////////
		// Setup occlusion planes
		mvCurrentOcclusionPlanes.resize(0);

		//User clip planes
		for(size_t i=0; i<apSettings->mvOcclusionPlanes.size(); ++i)
			mvCurrentOcclusionPlanes.push_back(apSettings->mvOcclusionPlanes[i]);

        //Fog
		if(mbSetupOcclusionPlaneForFog && apWorld->GetFogActive() && apWorld->GetFogColor().a >= 1.0f && apWorld->GetFogCulling())
		{
			cPlanef fogPlane;
			fogPlane.FromNormalPoint(	apFrustum->GetForward(), 
										apFrustum->GetOrigin() + apFrustum->GetForward()*-apWorld->GetFogEnd());
			mvCurrentOcclusionPlanes.push_back(fogPlane);
		}

		/////////////////////////////////////////////
		// Init rendering

			//////////////////////////////
			//Render Target
			if(mbSetFrameBufferAtBeginRendering && abAtStartOfRendering)
			{
				SetFrameBuffer(mpCurrentRenderTarget->mpFrameBuffer, true, true);
			}

			/////////////////////////
			// General states
			mpLowLevelGraphics->SetClearColor(mpCurrentSettings->mClearColor);
			mpLowLevelGraphics->SetClearDepth(1);
			
			/////////////////////////
			// Render states
			mpLowLevelGraphics->SetColorWriteActive(true, true, true, true);
			
			mpLowLevelGraphics->SetCullActive(true);
			mpLowLevelGraphics->SetCullMode(eCullMode_CounterClockwise);
			
			mpLowLevelGraphics->SetDepthTestActive(true);
			mpLowLevelGraphics->SetDepthWriteActive(true);
			mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
			
			mpLowLevelGraphics->SetColor(cColor(1,1,1,1));

			for(int i=0; i<kMaxTextureUnits; ++i)
				mpLowLevelGraphics->SetTexture(i, NULL);
		
			//////////////////////////////
			//Clear screen
			if(mbClearFrameBufferAtBeginRendering && abAtStartOfRendering)
			{
				ClearFrameBuffer(eClearFrameBufferFlag_Depth | eClearFrameBufferFlag_Color, true);
			}

			
			//////////////////////////////////////////////////
			// Projection matrix
			SetNormalFrustumProjection();

			/////////////////////////////////////////////
			// Clear Render list
			if(abAtStartOfRendering)
				mpCurrentRenderList->Clear();
	}

	//-----------------------------------------------------------------------
	
	void iRenderer::EndRendering(bool abAtEndOfRendering)
	{
		///////////////////////////////
		//Clear all occlusion objects
		if(abAtEndOfRendering)
			mpCurrentSettings->ClearOcclusionObjects(this);

		/////////////////////////////////////////////
		// If no post effects, make sure rendering is copied to frame buffer.
		if(abAtEndOfRendering && mbSendFrameBufferToPostEffects==false)
		{
			CopyToFrameBuffer();
		}

		/////////////////////////////////////////////
		// Reset all rendering states
		SetBlendMode(eMaterialBlendMode_None);
		SetChannelMode(eMaterialChannelMode_RGBA);
		SetAlphaMode(eMaterialAlphaMode_Solid);

		/////////////////////////////////////////////
		// Reset the cull mode setup
		mbInvertCullMode = false;
		mpLowLevelGraphics->SetCullMode(eCullMode_CounterClockwise);
		
		/////////////////////////////////////////////
		// Unbind all rendering data
		for(int i=0; i<kMaxTextureUnits; ++i)
		{
			if(mvCurrentTexture[i]) mpLowLevelGraphics->SetTexture(i, NULL);
		}
		
		if(mpCurrentProgram) mpCurrentProgram->UnBind();
		if(mpCurrentVtxBuffer) mpCurrentVtxBuffer->UnBind();

		/////////////////////////////////////////////
		// Clean up render functions
		ExitAndCleanUpRenderFunctions();

		if(abAtEndOfRendering && mbLog)
		{
			Log("----------\n");
			Log("Frame stats:\n");
			Log("----------\n");
			Log(" Number of Solid Objects: %d\n", mpCurrentRenderList->GetSolidObjectNum());
			Log(" Number of Trans Objects: %d\n", mpCurrentRenderList->GetTransObjectNum());
			Log(" Number of Lights: %d\n", mpCurrentRenderList->GetLightNum());
			Log("-----------------  END -------------------------\n");
		}

	}

	//-----------------------------------------------------------------------

	void iRenderer::CreateAndAddShadowMap(eShadowMapResolution aResolution, const cVector3l &avSize, ePixelFormat aFormat)
	{
		tString sName = "ShadowMap"+cString::ToString(avSize.x)+"x"+cString::ToString(avSize.y)+"_"+
						cString::ToString((int)mvShadowMapData[aResolution].size());
		cShadowMapData *pData = hplNew(cShadowMapData, ());
		pData->mlFrameCount = -1;

		pData->mpTexture = mpGraphics->CreateTexture(sName+"_Texture",eTextureType_2D, eTextureUsage_RenderTarget);
		pData->mpTexture->CreateFromRawData(avSize, aFormat, NULL);
		pData->mpTexture->SetCompareMode(eTextureCompareMode_RToTexture);
		pData->mpTexture->SetCompareFunc(eTextureCompareFunc_LessOrEqual);
		pData->mpTexture->SetFilter(eTextureFilter_Nearest);	
		pData->mpTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

		//Hack to avoid ATI drier failure:
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_OGL_ATIFragmentShader))
		{
			pData->mpTempDiffTexture = mpGraphics->CreateTexture(sName+"_TempDiff",eTextureType_2D, eTextureUsage_RenderTarget);
			pData->mpTempDiffTexture->CreateFromRawData(avSize, ePixelFormat_Alpha, NULL);
		}
		else
		{
			pData->mpTempDiffTexture = NULL;
		}

		pData->mpBuffer = mpGraphics->CreateFrameBuffer(sName+"_Buffer");
		if(pData->mpTempDiffTexture) pData->mpBuffer->SetTexture2D(0, pData->mpTempDiffTexture);
		pData->mpBuffer->SetDepthTexture2D(pData->mpTexture);

		pData->mpBuffer->CompileAndValidate();

		mvShadowMapData[aResolution].push_back(pData);
	}

	//-----------------------------------------------------------------------


    
	cShadowMapData* iRenderer::GetShadowMapData(eShadowMapResolution aResolution, iLight *apLight)
	{
		////////////////////////////
		//If size is 1, then just return that one
		if(mvShadowMapData[aResolution].size()==1) return mvShadowMapData[aResolution][0];

		//////////////////////////
		//Set up variables
		cShadowMapData *pBestData = NULL;  
		int lMaxFrameDist = -1;

		////////////////////////////
		//Iterate the shadow map array looking for shadow map already used by light
		//Else find the one with the largest frame length.
		for(size_t i=0; i<mvShadowMapData[aResolution].size(); ++i)
		{
			cShadowMapData *pData = mvShadowMapData[aResolution][i];
			
            if(pData->mCache.mpLight == apLight)
			{
				pBestData = pData;
				break;
			}
			else 
			{
				int lFrameDist = cMath::Abs(pData->mlFrameCount- mlRenderFrameCount);
				if(lFrameDist > lMaxFrameDist)
				{
					lMaxFrameDist = lFrameDist;
					pBestData = pData;	
				}
			}
		}

		////////////////////////////
		//Update the usage count
		if(pBestData)
		{
			pBestData->mlFrameCount = mlRenderFrameCount;
		}
		
		
		return pBestData;
	}

	//-----------------------------------------------------------------------


	bool iRenderer::ShadowMapNeedsUpdate(iLight *apLight, cShadowMapData *apShadowData)
	{
		//Occlusion culling must always be updated!
		if(apLight->GetOcclusionCullShadowCasters()) return true;

		cShadowMapLightCache& cacheData = apShadowData->mCache;
		
		///////////////////////////
		// Check if texture map and light are valid
		bool bValid =	cacheData.mpLight == apLight &&	cacheData.mlTransformCount == apLight->GetTransformUpdateCount() &&
						cacheData.mfRadius == apLight->GetRadius();

		/////////////////////////////
		// Spotlight specific
		if(bValid && apLight->GetLightType() == eLightType_Spot)
		{
			cLightSpot *pSpotLight = static_cast<cLightSpot*>(apLight);
			bValid = pSpotLight->GetAspect() == cacheData.mfAspect && pSpotLight->GetFOV() == cacheData.mfFOV;
		}

		/////////////////////////////
		// Shadow casters
		if(bValid)
		{
			bValid = apLight->ShadowCastersAreUnchanged(mvShadowCasters);
		}

		/////////////////////////////
		// If not valid, update data
		if(bValid == false)
		{
			cacheData.SetFromLight(apLight);
			apLight->SetShadowCasterCacheFromVec(mvShadowCasters);
		}
		
		return bValid ? false : true;
	}
	
	//-----------------------------------------------------------------------

	void iRenderer::DestroyShadowMaps()
	{
		for(int res=0; res < eShadowMapResolution_LastEnum; ++res)
		{
			for(size_t i=0; i<mvShadowMapData[res].size(); ++i)
			{
				cShadowMapData *pData = mvShadowMapData[res][i];

				mpGraphics->DestroyFrameBuffer(pData->mpBuffer);
				mpGraphics->DestroyTexture(pData->mpTexture);
				if(pData->mpTempDiffTexture) mpGraphics->DestroyTexture(pData->mpTempDiffTexture);
			}
			STLDeleteAll(mvShadowMapData[res]);
		}
		
	}

	//-----------------------------------------------------------------------

	void iRenderer::RenderZObject(iRenderable *apObject, cFrustum *apCustomFrustum)
	{
		cMaterial *pMaterial = apObject->GetMaterial();

		eMaterialRenderMode renderMode = apObject->GetCoverageAmount()>=1 ? eMaterialRenderMode_Z : eMaterialRenderMode_Z_Dissolve;

		////////////////////////
		//Set up render modes
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaLimit(mfDefaultAlphaLimit);
		SetAlphaMode( pMaterial->GetTexture(eMaterialTexture_Alpha) || renderMode==eMaterialRenderMode_Z_Dissolve ? 
						eMaterialAlphaMode_Trans : eMaterialAlphaMode_Solid);

		////////////////////////
		//Set up textures
        SetMaterialTextures(renderMode, pMaterial);
		
        ////////////////////////
		//Set up program
		SetMaterialProgram(renderMode,pMaterial);
			

		////////////////////////
		//Matrix
		if(apCustomFrustum)
			SetMatrix(apObject->GetModelMatrix(apCustomFrustum));
		else
			SetMatrix(apObject->GetModelMatrixPtr());

		////////////////////////
		//Vertex buffer
		SetVertexBuffer(apObject->GetVertexBuffer());

		////////////////////////
		//Draw
		DrawCurrentMaterial(renderMode, apObject);
	}
	
	//-----------------------------------------------------------------------

	void iRenderer::CheckNodesAndAddToListIterative(iRenderableContainerNode *apNode, tRenderableFlag alNeededFlags)
	{
		///////////////////////////////////////
		//Make sure node is updated
		apNode->UpdateBeforeUse();

		///////////////////////////////////////
		//Get frustum collision, if previous was inside, then this is too!
		eCollision frustumCollision = mpCurrentFrustum->CollideNode(apNode);
		
		////////////////////////////////
		//Do a visible check but always iterate the root node!	
		if(apNode->GetParent())
		{
			if(	frustumCollision == eCollision_Outside) return;
			if(CheckNodeIsVisible(apNode)==false)		return;
		}

		////////////////////////
		//Iterate children
		if(apNode->HasChildNodes())
		{
			tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
			for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
			{
				iRenderableContainerNode *pChildNode = *childIt;
				CheckNodesAndAddToListIterative(pChildNode, alNeededFlags);
			}
		}

		/////////////////////////////
		//Iterate objects
		if(apNode->HasObjects())
		{
			tRenderableListIt it = apNode->GetObjectList()->begin();
			for(; it != apNode->GetObjectList()->end(); ++it)
			{
				iRenderable *pObject = *it;
				if(CheckObjectIsVisible(pObject, alNeededFlags)==false) continue;

			    if(	frustumCollision == eCollision_Inside ||
					pObject->CollidesWithFrustum(mpCurrentFrustum))
				{
					mpCurrentRenderList->AddObject(pObject);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderer::CheckForVisibleAndAddToList(iRenderableContainer *apContainer, tRenderableFlag alNeededFlags)
	{
		apContainer->UpdateBeforeRendering();

		CheckNodesAndAddToListIterative(apContainer->GetRoot(), alNeededFlags);
	}

	//-----------------------------------------------------------------------

	/**
	* Inserts the child nodes in apNode in a_setNodeStack.
	*/
	void iRenderer::PushNodeChildrenToStack(tRendererSortedNodeSet& a_setNodeStack, iRenderableContainerNode *apNode, int alNeededFlags)
	{
		if(apNode->HasChildNodes()==false) return;

		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			iRenderableContainerNode *pChildNode = *childIt;

			///////////////////////
			// Make sure node is update
			pChildNode->UpdateBeforeUse();

			///////////////////////////////////////////////////
			// Check node has object and needed flags
			if(	pChildNode->UsesFlagsAndVisibility() &&
				(pChildNode->HasVisibleObjects()==false || (pChildNode->GetRenderFlags() & alNeededFlags) != alNeededFlags))
			{
				continue;
			}

			///////////////////////////////////////////////////
			// Check if inside frustum, skipping test if parent was inside
			eCollision frustumCollision =	apNode->GetPrevFrustumCollision() == eCollision_Inside ?
													eCollision_Inside : mpCurrentFrustum->CollideNode(pChildNode);

			if(frustumCollision == eCollision_Outside) continue;
			if(CheckNodeIsVisible(pChildNode)==false) continue;

            pChildNode->SetPrevFrustumCollision(frustumCollision);


			////////////////////////////////////////////////////////
			//Check if the frustum origin is inside the node AABB.
			// If so no intersection test is needed
			if(mpCurrentFrustum->CheckAABBNearPlaneIntersection(pChildNode->GetMin(), pChildNode->GetMax()))
			{
				cVector3f vViewSpacePos = cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), pChildNode->GetCenter());
				pChildNode->SetViewDistance(vViewSpacePos.z);
				pChildNode->SetInsideView(true);
			}
			////////////////////////////////////////////////////////
			// Frustum origin is outside of node. Do intersection test.
			else
			{	
				cVector3f vIntersection;
				cMath::CheckAABBLineIntersection(pChildNode->GetMin(), pChildNode->GetMax(), 
												mpCurrentFrustum->GetOrigin(), pChildNode->GetCenter(),
												&vIntersection, NULL);
				cVector3f vViewSpacePos = cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), vIntersection);
				pChildNode->SetViewDistance(vViewSpacePos.z);
				pChildNode->SetInsideView(false);
			}
			
			////////////////////////////////////////////////////////
			// Add to sorted stack.
			a_setNodeStack.insert(pChildNode);
		}
	}

	//-----------------------------------------------------------------------

	void iRenderer::AddAndRenderNodeOcclusionQuery(tNodeOcclusionPairList *apList, iRenderableContainerNode *apNode, bool abObjectsRendered)
	{
		//DEBUG
		// Skip using a query and just render the node.
		if(	mbOnlyRenderPrevVisibleOcclusionObjects && 
			mlOnlyRenderPrevVisibleOcclusionObjectsFrameCount >= 1)
		{
			RenderNodeBoundingBox(apNode,NULL);
			return;
		}

		//////////////////////
		//Create the pair
		cNodeOcclusionPair noPair;
		noPair.mpNode = apNode;
		noPair.mpQuery = GetOcclusionQuery();
		noPair.mbObjectsRendered = abObjectsRendered;

		if(mbLog)Log("CHC: Testing query %d on node: %d\n",noPair.mpQuery, apNode);

		/////////////////////
		// Render node AABB
		RenderNodeBoundingBox(apNode, noPair.mpQuery);		
		
		
		/////////////////////
		// Add to list
		apList->push_back(noPair);
	}

	//-----------------------------------------------------------------------

	void iRenderer::RenderNodeBoundingBox(iRenderableContainerNode *apNode, iOcclusionQuery *apQuery)
	{
		////////////////////////
		//States
		SetDepthWrite(false);
		SetAlphaMode(eMaterialAlphaMode_Solid);

		/////////////////////////
		//Program
		SetProgram(mpDepthOnlyProgram);

		/////////////////////////
		//Texture
		SetTexture(0,NULL);

		/////////////////////////
		//Matrix
		cVector3f vSize = apNode->GetMax() - apNode->GetMin();
		cMatrixf mtxBox = cMath::MatrixScale(vSize);
		mtxBox.SetTranslation(apNode->GetCenter());
		SetModelViewMatrix( cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), mtxBox) );

		/////////////////////////
		//Vertex buffer
		SetVertexBuffer(mpShapeBox);
		

		/////////////////////////
		//Draw
		if(apQuery) apQuery->Begin();
		DrawCurrent();
		if(apQuery) apQuery->End();

		mpCurrentSettings->mlNumberOfOcclusionQueries++;
		
		//Debug:
		//mpCurrentSettings->mlstRenderedNodes.push_back(apNode);
	}

	//-----------------------------------------------------------------------

	bool iRenderer::RenderObjectZAndAddToRenderListStaticCallback(iRenderer *apRenderer, iRenderable *apObject)
	{
		return apRenderer->RenderObjectZAndAddToRenderList(apObject);
	}
	
	bool iRenderer::RenderObjectZAndAddToRenderList(iRenderable *apObject)
	{
		cMaterial *pMaterial = apObject->GetMaterial();

		/////////////////////////////
		// Add to render list
		mpCurrentRenderList->AddObject(apObject);
		
		////////////////////////////
		//Check if object is translucent or has no material
		// If so, do not render it.
		if( pMaterial==NULL || pMaterial->GetType()->IsTranslucent())
		{
			return false;
		}

		////////////////////////
		//Set up states
		SetDepthWrite(true);

		RenderZObject(apObject, NULL);

		return true;
	}

	//-----------------------------------------------------------------------

	static std::vector<iRenderable*> gvSortedObjects;
	int iRenderer::RenderAndAddNodeObjects(iRenderableContainerNode *apNode, tRenderCHCObjectCallbackFunc apRenderCallback, tRenderableFlag alNeededFlags)
	{
		if(apNode->HasObjects()==false) return 0;

		int lRenderedObjects = 0;

		////////////////////////////////////
		//Iterate sorted objects and render
		for(tRenderableListIt it = apNode->GetObjectList()->begin(); it != apNode->GetObjectList()->end(); ++it)
		{
			iRenderable *pObject = *it;
			
			//////////////////////
			// Check if object is visible
			if(CheckObjectIsVisible(pObject, alNeededFlags)==false) continue;

			/////////////////////////////
			//Check if inside frustum, skip test node was inside
			if(	apNode->GetPrevFrustumCollision() != eCollision_Inside && 
				pObject->CollidesWithFrustum(mpCurrentFrustum)==false)
			{
				continue;
			}

			if(apRenderCallback(this,pObject))
			{
				++lRenderedObjects;
			}
		}

		return lRenderedObjects;
	}
	
	//-----------------------------------------------------------------------
	
	cVisibleRCNodeTracker *gpCurrentVisibleNodeTracker = NULL;
    void iRenderer::PushUpVisibility(iRenderableContainerNode *apNode)
	{
		gpCurrentVisibleNodeTracker->SetNodeVisible(apNode);
		
		if(apNode->GetParent()) PushUpVisibility(apNode->GetParent());
	}

	//-----------------------------------------------------------------------
	
	static void PrintNodeDebugContents(tRendererSortedNodeSet &a_setNodes)
	{
		Log("  Nodes:\n");
		tRendererSortedNodeSetIt it = a_setNodes.begin();
		for(; it != a_setNodes.end(); ++it)
		{
			iRenderableContainerNode *pNode = *it;
            
			Log("   Node: %d, inside: %d, children %d, objects: %d\n", pNode, pNode->IsInsideView(), 
																	pNode->GetChildNodeList()->size(),
																	pNode->GetObjectNum());
		}
	}

	//-----------------------------------------------------------------------


	void iRenderer::CheckForVisibleObjectsAddToListAndRenderZ(	cVisibleRCNodeTracker *apVisibleNodeTracker,
																tObjectVariabilityFlag alObjectTypes, tRenderableFlag alNeededFlags,
																bool abSetupRenderStates,
																tRenderCHCObjectCallbackFunc apRenderObjectCallback)
	{
		START_RENDER_PASS(CHC_Culling);

		////////////////////////////
		//Set up rendering
		if(abSetupRenderStates)
		{
			SetDepthTest(true);
			SetDepthWrite(false);
			SetBlendMode(eMaterialBlendMode_None);
			SetAlphaMode(eMaterialAlphaMode_Solid);
			SetChannelMode(eMaterialChannelMode_None);

			SetTextureRange(NULL,0);
		}

		////////////////////////////
		//Set up stack and other variables
		const tFlag vContainerFlags[2] = { eObjectVariabilityFlag_Static, eObjectVariabilityFlag_Dynamic};

		iRenderableContainer *pContainers[2];
		pContainers[0] = mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Static);
		pContainers[1] = mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Dynamic);

		tNodeOcclusionPairList lstNodeOcclusionPairs;

		// Set up output variables
		mpCurrentSettings->mlNumberOfOcclusionQueries =0;
		
		// Switch sets, so that the last frames set is not current.
		apVisibleNodeTracker->SwitchAndClearVisibleNodeSet();

		tRenderCHCObjectCallbackFunc pRenderCallback;
		if(apRenderObjectCallback)	pRenderCallback = apRenderObjectCallback;
		else						pRenderCallback = RenderObjectZAndAddToRenderListStaticCallback;


		// Setup the container before rendering
		for(int i=0;i<2; ++i)
		{
			if(vContainerFlags[i] & alObjectTypes)
			{
				pContainers[i]->UpdateBeforeRendering();
			}
		}

		// Temp variable used when pushing visibility
		gpCurrentVisibleNodeTracker = apVisibleNodeTracker;

		//Debug:
		//mpCurrentSettings->mlstRenderedNodes.clear();

		//Add Root nodes to stack
		tRendererSortedNodeSet setNodeStack;
		for(int i=0;i<2; ++i)
		{
			if(vContainerFlags[i] & alObjectTypes)
			{
				iRenderableContainerNode *pNode = pContainers[i]->GetRoot();
				pNode->UpdateBeforeUse();	//Make sure node is updated.	
				pNode->SetInsideView(true);	//We never want to check root! Assume player is inside.
				setNodeStack.insert(pNode);
			}
		}


		//Render at least X objects without rendering nodes, to some occluders
		int lMinRenderedObjects = mpCurrentSettings->mlMinimumObjectsBeforeOcclusionTesting;	
				
		////////////////////////////
		//Iterate the nodes on the stack.
		while(setNodeStack.empty()==false || lstNodeOcclusionPairs.empty()==false)
		{
			//if(mbLog) PrintNodeDebugContents(setNodeStack);

			///////////////////////////
			//If node stack not empty, pop the first node on the stack
			if(setNodeStack.empty()==false)
			{
				tRendererSortedNodeSetIt firstIt = setNodeStack.begin(); //Might be slow...
				iRenderableContainerNode *pNode = *firstIt;
				setNodeStack.erase(firstIt); //This is most likely very slow... use list?

				//////////////////////////
				// Check if node is a leaf  
				bool bNodeIsLeaf = pNode->HasChildNodes()==false;

				//////////////////////////
				// Check if near plane is inside node AABB
				bool bNearPlaneInsideNode = pNode->IsInsideView();

				//////////////////////////
				// Check if node was visible
				bool bWasVisible = apVisibleNodeTracker->WasNodeVisible(pNode);

				//////////////////////////
				//Render nodes and queue queries
				// Only do this if:
				// - Near plane is not inside node AABB 
				// - All of the closest objects have been rendered (so there are some blockers)
				// - Node was not visible or node is leaf (always draw leaves!)
				if(	bNearPlaneInsideNode==false && lMinRenderedObjects<=0 && 
					(bWasVisible==false || bNodeIsLeaf) )
				{
					////////////////
					//If node is leaf and was visible render objects directly.
					// Rendering directly can be good as it may result in extra blocker for next node test.
					bool bRenderObjects = false;
					if(bNodeIsLeaf && bWasVisible) bRenderObjects = true;
															
					/////////////////////////////////////
					// Render AABB and add query to list
					AddAndRenderNodeOcclusionQuery(&lstNodeOcclusionPairs, pNode, bRenderObjects);
					
					//////////////////////////
					//Render node objects after AABB so that an object does not occlude its own node.
					if(bRenderObjects) RenderAndAddNodeObjects(pNode,pRenderCallback, alNeededFlags);

					//Debug:
					// Skipping any queries, so must push up visible if this node was visible
					if(mbOnlyRenderPrevVisibleOcclusionObjects && 
						mlOnlyRenderPrevVisibleOcclusionObjectsFrameCount >= 1 &&
						bRenderObjects && bWasVisible)
					{
						PushUpVisibility(pNode);
					}
				}
				//////////////////////////
				//If previous test failed, push children to stack and render objects (if any) directly.
				else
				{
					if(mbLog)
					{
						if(lMinRenderedObjects>0)
							Log("CHC: Rendered objects left: %d node %d, pushing children and rendering nodes!\n",lMinRenderedObjects, pNode);
						else
							Log("CHC: Near plane inside node %d, pushing children and rendering nodes!\n",pNode);
					}
					
					////////////////
					//Add child nodes to stack if any (also checks if they have needed flags are in frustum)
					PushNodeChildrenToStack(setNodeStack, pNode, alNeededFlags);

					////////////////
					//Render objects if any
					int lObjectsRendered = RenderAndAddNodeObjects(pNode,pRenderCallback, alNeededFlags);
					lMinRenderedObjects -= lObjectsRendered;
				}

				////////////////////
				// If node stack is empty, it is a good time to flush!
				if(setNodeStack.empty())
				{
					mpLowLevelGraphics->FlushRendering();
				}
			}

			///////////////////////////
			//If node-query list is not empty, check if the first query is ready
			if(lstNodeOcclusionPairs.empty()==false)
			{
				cNodeOcclusionPair& noPair = lstNodeOcclusionPairs.front();

							
				//////////////////////////////////////
				//Check if the query is done
				if(noPair.mpQuery->FetchResults()==false)
				{
					//Do nothing for now....
				}
				///////////////////////
				// Query is ready
				else
				{
					////////////
					//Get data, release query and pop front
					bool bObjectsRendered = noPair.mbObjectsRendered;
					iRenderableContainerNode *pNode = noPair.mpNode;
					
					int lSampleCount = noPair.mpQuery->GetSampleCount();
					ReleaseOcclusionQuery(noPair.mpQuery);
					
					lstNodeOcclusionPairs.pop_front(); //Makes noPair invalid

					if(mbLog)Log("CHC: Fetching query %d on node: %d, samples: %d\n",noPair.mpQuery, pNode, lSampleCount);

					//////////
					// Check if node is visible and if so handle children and objects
					if(lSampleCount > mpCurrentSettings->mlSampleVisiblilityLimit)
					{
						if(mbLog)Log("CHC: Rendering objects in node %d\n", pNode);

						////////////////
						//Set node and all of its parents as visible.
						PushUpVisibility(pNode);
						
						////////////////
						//Add child nodes to stack if any (also checks if they have needed flags are in frustum)
						PushNodeChildrenToStack(setNodeStack, pNode, alNeededFlags);

						////////////////
						//Render objects if any and not already rendered.
						if(bObjectsRendered==false){
							RenderAndAddNodeObjects(pNode,pRenderCallback, alNeededFlags);
						}
					}
					else
					{
						if(mbLog) Log("CHC: Skipping node %d\n", pNode);

					}
				}
			}

		}

		//Debug:
		if(mbOnlyRenderPrevVisibleOcclusionObjects)
		{
			mlOnlyRenderPrevVisibleOcclusionObjectsFrameCount++;
		}

		END_RENDER_PASS();
	}
	
	//-----------------------------------------------------------------------

	static tRenderableVec* gpLightShadowCasterVec=NULL;
	static cFrustum *gpLightFrustum=NULL;
	static cFrustum *gpViewFrustum=NULL;
	static eFrustumPlane gvBeyondLightAndViewPlanes[6]; //Use to check if an caster is not going to affect view.
	static int glBeyondLightAndViewPlaneNum;
	static bool gbLightBehindNearPlane;

	//-----------------------------------------------------------------------
	
	static bool BoxIntersectOrInsidePlane(const cPlanef& aPlane,cVector3f *apCornerVec)
	{
		for(int corner =0; corner < 8; ++corner)
		{
			//Check if point is inside, if so skip plane...
			if(cMath::PlaneToPointDist(aPlane, apCornerVec[corner]) > 0)
			{
				return true;
			}
		}

		return false;
	}

	static bool BoxInsidePlane(const cPlanef& aPlane,cVector3f *apCornerVec)
	{
		for(int corner =0; corner < 8; ++corner)
		{
			//Check if point is inside, if so skip plane...
			if(cMath::PlaneToPointDist(aPlane, apCornerVec[corner]) < 0)
			{
				return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderer::CheckShadowCasterContributesToView(iRenderable *apObject)
	{
		//This should be always true since the shadow map might be saved and will then end up faulty if some objects have been dismissed!
		return true;


		cBoundingVolume *pBV = apObject->GetBoundingVolume();

		//Log("---- Checking %s --- \n",apObject->GetName().c_str());

		//Log("1\n");
		//////////////////////////////////////
		//If light is behind near plane, one must check if object in front of near plane
		bool bObjectMightOnNearPlane=false;
		if(gbLightBehindNearPlane)
		{
			return true;	//Temp, until I can come up with a qay to resolve the issues.

			const cPlanef& nearPlane = gpViewFrustum->GetPlane(eFrustumPlane_Near);
			float fDist = cMath::PlaneToPointDist(nearPlane, pBV->GetWorldCenter());
			
			//Object is outside of near plane. 
			if(fDist < -pBV->GetRadius()) return true; 

			//Object is not really inside near plane, test more accurately later
			if(fDist < pBV->GetRadius())
			{
				bObjectMightOnNearPlane = true;	//Do not test yet
			}
		}

		eCollision frustumCollision = eCollision_Inside;
		
		//Log("2\n");
		////////////////////////////
		// Sphere test
		int lOutsideCount=0;
		int lInsideCount=0;
		for(int i=0; i<glBeyondLightAndViewPlaneNum; ++i)
		{
			eFrustumPlane frustumPlane = gvBeyondLightAndViewPlanes[i];
			const cPlanef& cameraPlane = gpViewFrustum->GetPlane(frustumPlane);
			
			float fDist = cMath::PlaneToPointDist(cameraPlane, pBV->GetWorldCenter());
			
			//Test if inside of plane
			if(fDist > pBV->GetRadius())
			{
				lInsideCount++;
			}
			else if(fDist < -pBV->GetRadius())
			{
				lOutsideCount++;
				break;
			}
		}
		
		//Log("3\n");
		// If all was inside, then we are sure it contributes
		if(lInsideCount == glBeyondLightAndViewPlaneNum) return true;

		//Log("4\n");
		// If any was outside, we are sure it does NOT contribute
		if(lOutsideCount > 0 && bObjectMightOnNearPlane==false) return false;
				
		////////////////////
		// Create corners
		const cVector3f& vMax = pBV->GetMax();
		const cVector3f& vMin = pBV->GetMin();

		cVector3f vCorners[8] ={
			cVector3f(vMax.x,vMax.y,vMax.z), cVector3f(vMax.x,vMax.y,vMin.z),
				cVector3f(vMax.x,vMin.y,vMax.z), cVector3f(vMax.x,vMin.y,vMin.z),

				cVector3f(vMin.x,vMax.y,vMax.z), cVector3f(vMin.x,vMax.y,vMin.z),
				cVector3f(vMin.x,vMin.y,vMax.z), cVector3f(vMin.x,vMin.y,vMin.z),
		};
		
		//Log("5\n");
		////////////////////////////
		// Check near plane intersection
		if(lOutsideCount > 0 && bObjectMightOnNearPlane)
		{
			//If object is not fully inside, it contributes
			if(BoxInsidePlane(gpViewFrustum->GetPlane(eFrustumPlane_Near),vCorners)==false)
			{
				return true;
			}
			bObjectMightOnNearPlane = false;//Reset so it is not tested again.
		}
		
		////////////////////////////
		// Box Test
		
		//Log("6\n");
		//If needed check so that it is inside the near plane
		if(bObjectMightOnNearPlane)
		{
			//If object is not fully inside, it contributes
			if(BoxInsidePlane(gpViewFrustum->GetPlane(eFrustumPlane_Near),vCorners)==false)
			{
				return true;
			}
		}
		
		//Log("7\n");
		//Iterate all planes separating between contributing and not.
		bool bContributes;
		for(int plane=0; plane<glBeyondLightAndViewPlaneNum; ++plane)
		{
			eFrustumPlane frustumPlane = gvBeyondLightAndViewPlanes[plane];
			const cPlanef& cameraPlane = gpViewFrustum->GetPlane(frustumPlane);
			
			bContributes = BoxIntersectOrInsidePlane(cameraPlane, vCorners);

			//Log("plane %d contribute: %d\n",frustumPlane, bContributes);

			if(bContributes==false) break;
		}
		
		return bContributes;
	}

	//-----------------------------------------------------------------------


	void iRenderer::GetShadowCastersIterative(iRenderableContainerNode *apNode, eCollision aPrevCollision)
	{
		///////////////////////////////////////
		//Make sure node is updated
		apNode->UpdateBeforeUse();

		///////////////////////////////////////
		//Get frustum collision, if previous was inside, then this is too!
		eCollision frustumCollision = aPrevCollision == eCollision_Inside ? aPrevCollision : gpLightFrustum->CollideNode(apNode);
		
		///////////////////////////////////
		//Check if visible but always iterate the root node!	
		if(apNode->GetParent()) 
		{
			if(frustumCollision == eCollision_Outside) return;
			if(CheckNodeIsVisible(apNode)==false) return;
		}

		////////////////////////
		//Iterate children
		if(apNode->HasChildNodes())
		{
			for(tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin(); childIt != apNode->GetChildNodeList()->end(); ++childIt)
			{
				iRenderableContainerNode *pChildNode = *childIt;
				GetShadowCastersIterative(pChildNode, frustumCollision);
			}
		}

		/////////////////////////////
		//Iterate objects
		if(apNode->HasObjects())
		{
			for(tRenderableListIt it = apNode->GetObjectList()->begin(); it != apNode->GetObjectList()->end(); ++it)
			{
				iRenderable *pObject = *it;
				
				/////////
				//Check so visible and shadow caster
				if(	CheckObjectIsVisible(pObject, eRenderableFlag_ShadowCaster)==false ||
					pObject->GetMaterial() == NULL ||
					pObject->GetMaterial()->GetType()->IsTranslucent())
				{
					continue;
				}
				
				/////////
				//Check if in frustum
				if(	frustumCollision != eCollision_Inside &&
					gpLightFrustum->CollideBoundingVolume(pObject->GetBoundingVolume()) == eCollision_Outside)
				{
					continue;
				}
				
				/////////
				// Check if it contributes to scene
				if(CheckShadowCasterContributesToView(pObject)==false) continue;

				
				///////////////////////////////
				// Add object!
				
				//Calculate the view space Z (just a squared distance)
				pObject->SetViewSpaceZ(cMath::Vector3DistSqr(pObject->GetBoundingVolume()->GetWorldCenter(), 
															gpLightFrustum->GetOrigin()));

				//Add to list
				gpLightShadowCasterVec->push_back(pObject);				
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderer::GetShadowCasters(iRenderableContainer *apContainer, tRenderableVec& avObjectVec, cFrustum *apLightFrustum)
	{
		apContainer->UpdateBeforeRendering();

		gpLightFrustum = apLightFrustum;
		gpLightShadowCasterVec = &avObjectVec;

		GetShadowCastersIterative(apContainer->GetRoot(), eCollision_Outside);
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_ShadowCasters(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		cMaterial *pMatA = apObjectA->GetMaterial();
		cMaterial *pMatB = apObjectB->GetMaterial();

		//////////////////////////
		//Alpha mode
		if(pMatA->GetAlphaMode() != pMatB->GetAlphaMode())
		{
			return pMatA->GetAlphaMode() < pMatB->GetAlphaMode();
		}

		//////////////////////////
		//If alpha, sort by texture (we know alpha is same for both materials, so can just test one)
		if(	pMatA->GetAlphaMode() == eMaterialAlphaMode_Trans )
		{
			if(pMatA->GetProgram(0,eMaterialRenderMode_Z) != pMatB->GetProgram(0,eMaterialRenderMode_Z))
			{
				return pMatA->GetProgram(0,eMaterialRenderMode_Z) < pMatB->GetProgram(0,eMaterialRenderMode_Z);
			}

			if(pMatA->GetTexture(eMaterialTexture_Diffuse) != pMatB->GetTexture(eMaterialTexture_Diffuse))
			{
				return pMatA->GetTexture(eMaterialTexture_Diffuse) < pMatB->GetTexture(eMaterialTexture_Diffuse);
			}
		}

		//////////////////////////
		//View space depth, no need to test further since Z should almost never be the same for two objects.
		//View space z is really just BB dist dis squared, so use "<"
		return apObjectA->GetViewSpaceZ() < apObjectB->GetViewSpaceZ();
	}

	//-----------------------------------------------------------------------

	bool iRenderer::SetupShadowMapRendering(iLight *apLight)
	{
		/////////////////////////
		// Get light data
		if(apLight->GetLightType() != eLightType_Spot) return false; //Only support spot lights for now...

		cLightSpot *pSpotLight = static_cast<cLightSpot*>(apLight);
		cFrustum *pLightFrustum = pSpotLight->GetFrustum();

		//Set the view frustum, needed in some functions cause the current is set for the light during rendering.
		gpViewFrustum = mpCurrentFrustum;
		
		/////////////////////////
		// Get the camera planes that face away from the light
		glBeyondLightAndViewPlaneNum =0;
		cVector3f vLightForward = pLightFrustum->GetForward();
		for(int i=0; i<eFrustumPlane_LastEnum; ++i)
		{
			const cPlanef& cameraPlane = gpViewFrustum->GetPlane((eFrustumPlane)i);

			//Check so plane is facing the light
			// Above 0 because GetForward from frustum is inverted.
			// Note that this is not optimal, but good enough for now. Should have some other way of choosing to make it better.
			cVector3f vPlaneNormal = cameraPlane.GetNormal();
			if(cMath::Vector3Dot(vPlaneNormal, vLightForward) > 0)	
			{
				gvBeyondLightAndViewPlanes[glBeyondLightAndViewPlaneNum] = (eFrustumPlane)i;
				++glBeyondLightAndViewPlaneNum;
			}
		}
		
		/////////////////////////
		// See if light is behind near plane
		if(cMath::PlaneToPointDist(gpViewFrustum->GetPlane(eFrustumPlane_Near), pLightFrustum->GetOrigin()) < 0)
		{
			gbLightBehindNearPlane = true;	
		}
		else
		{
			gbLightBehindNearPlane = false;
		}

		/////////////////////////
		// If culling by occlusion, skip rest of function
		if(apLight->GetOcclusionCullShadowCasters())
		{
			//mvShadowCasters.resize(0); //Debug reasons only, remove later!
			return true;
		}


		/////////////////////////
		// Get objects to render

		//Clear list
		mvShadowCasters.resize(0); //No clear, so we keep all in memory.

		//Get the objects
		if(apLight->GetShadowCastersAffected() & eObjectVariabilityFlag_Dynamic)
			GetShadowCasters(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Dynamic),mvShadowCasters, pSpotLight->GetFrustum());

		if(apLight->GetShadowCastersAffected() & eObjectVariabilityFlag_Static)
			GetShadowCasters(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Static),mvShadowCasters, pSpotLight->GetFrustum());

		//See if any objects where added.
		if(mvShadowCasters.empty()) return false;

		//Sort the list
		std::sort(mvShadowCasters.begin(), mvShadowCasters.end(), SortFunc_ShadowCasters);
		
		return true;
	}

	//-----------------------------------------------------------------------

	static cFrustum * gpTempLightFrustum=NULL;
	bool iRenderer::RenderShadowCasterCHCStaticCallback(iRenderer *apRenderer, iRenderable *apObject)
	{
		return apRenderer->RenderShadowCasterCHC(apObject);
	}
	
	bool iRenderer::RenderShadowCasterCHC(iRenderable *apObject)
	{
		cMaterial *pMaterial = apObject->GetMaterial();

		//Check so it is a solid object
		if( pMaterial==NULL || pMaterial->GetType()->IsTranslucent())
		{
			return false;
		}

		//Check so it affects the view frustum
		if(CheckShadowCasterContributesToView(apObject)==false) return false;

		//mvShadowCasters.push_back(apObject); //Debug. Only to see what object are rendered.
		
		//Render the object
		RenderShadowCaster(apObject,gpTempLightFrustum);

		return true;
	}

	//-----------------------------------------------------------------------

	void iRenderer::RenderShadowCaster(iRenderable *apObject, cFrustum *apLightFrustum)
	{
		RenderZObject(apObject, apLightFrustum);
	}

	//-----------------------------------------------------------------------


	void iRenderer::RenderShadowCastersNormal(cFrustum *apLightFrustum)
	{
		////////////////////////////////
		// Iterate the objects to be rendered 
		for(size_t i=0; i<mvShadowCasters.size(); ++i)
		{
			RenderShadowCaster(mvShadowCasters[i], apLightFrustum);
		}
	}
	//-----------------------------------------------------------------------

	void iRenderer::RenderShadowMap(iLight *apLight, iFrameBuffer *apShadowBuffer)
	{
		if(mbLog){ 
			Log("---\nBegin Rendering Shadow Map for light '%s' / %d to buffer %d\n",apLight->GetName().c_str(), apLight, apShadowBuffer);
		}
		/////////////////////////
		// Get light data
		if(apLight->GetLightType() != eLightType_Spot) return; //Only support spot lights for now...

        cLightSpot *pSpotLight = static_cast<cLightSpot*>(apLight);
		cFrustum *pLightFrustum = pSpotLight->GetFrustum();
	
       		
		/////////////////////////
		// Setup render states
		SetDepthTest(true);
		SetDepthWrite(true);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetAlphaLimit(mfDefaultAlphaLimit);
		SetChannelMode(eMaterialChannelMode_None);

		SetTextureRange(NULL,0);

		//Do not use any custom occlusion for shadows!
		SetOcclusionPlanesActive(false);

		mpLowLevelGraphics->SetPolygonOffsetActive(true);
		mpLowLevelGraphics->SetPolygonOffset(mpCurrentSettings->mfShadowMapBias * apLight->GetShadowMapBiasMul(), 
											 mpCurrentSettings->mfShadowMapSlopeScaleBias * apLight->GetShadowMapSlopeScaleBiasMul());
		
		/////////////////////////
		// Setup render target
		SetFrameBuffer(apShadowBuffer,false, false);

		mpLowLevelGraphics->SetClearDepth(1);
		ClearFrameBuffer(eClearFrameBufferFlag_Depth, false);
		
		/////////////////////////
		// Setup projection
		cFrustum *pLastFrustum = mpCurrentFrustum;
		mpCurrentFrustum = pLightFrustum;	//Is this a little too hackish? Not sure...
		SetFrustumProjection(pLightFrustum);
		
		/////////////////////////
		// Render the shadow casters
		if(apLight->GetOcclusionCullShadowCasters())
		{
			gpTempLightFrustum = pLightFrustum;
			CheckForVisibleObjectsAddToListAndRenderZ(	apLight->GetVisibleNodeTracker(),
														apLight->GetShadowCastersAffected(),
														eRenderableFlag_ShadowCaster, 
														false, 
														RenderShadowCasterCHCStaticCallback);
		}
		else
		{
			RenderShadowCastersNormal(pLightFrustum);
		}
		
        
		/////////////////////////
		// Reset states
		SetTexture(0,NULL);

		SetOcclusionPlanesActive(true);

		mpLowLevelGraphics->SetPolygonOffsetActive(false);

		/////////////////////////
		// Reset projection
		mpCurrentFrustum = pLastFrustum;
		SetNormalFrustumProjection();

        
		if(mbLog) Log("End Rendering Shadow Map\n---\n");
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_OcclusionObject(cOcclusionQueryObject* apObjectA, cOcclusionQueryObject *apObjectB)
	{
		//////////////////////////
		//Vertex buffer
		if(apObjectA->mpVtxBuffer != apObjectB->mpVtxBuffer)
		{
			return apObjectA->mpVtxBuffer < apObjectB->mpVtxBuffer;
		}

		//////////////////////////
		//Depth
		if(apObjectA->mbDepthTest != apObjectB->mbDepthTest)
		{
			return apObjectA->mbDepthTest;
		}

		//////////////////////////
		//Matrix
		return apObjectA->mpMatrix < apObjectB->mpMatrix;
	}

	void iRenderer::AssignAndRenderOcclusionQueryObjects(bool abSetFrameBuffer, iFrameBuffer *apFrameBuffer, bool abUsePosAndSize)
	{
		cRenderList *pRenderList = mpCurrentSettings->mpRenderList;

		///////////////////////////////////
		// Get and use any previous occlusion queries
		cRenderableVecIterator objIt = pRenderList->GetOcclusionQueryObjectIterator();
		while(objIt.HasNext())
		{
			iRenderable *pObject = objIt.Next();
			pObject->AssignOcclusionQuery(this);
		}

		/////////////////////////////////////
		//If no queries added, then skip any rendering
		if(mpCurrentSettings->mlCurrentOcclusionObject <=0) return;
		
		START_RENDER_PASS(OcclusionObjects);

		////////////////////////////////////
		// Copying queries to new array
		mvSortedOcclusionObjects.resize(mpCurrentSettings->mlCurrentOcclusionObject);
		for(int i=0; i<mpCurrentSettings->mlCurrentOcclusionObject; ++i)
			mvSortedOcclusionObjects[i] = mpCurrentSettings->mvOcclusionObjectPool[i];

		////////////////////////////////////
		// Sort the queries
		std::sort(mvSortedOcclusionObjects.begin(), mvSortedOcclusionObjects.end(), SortFunc_OcclusionObject);

		///////////////////////////////////
		// Set up frame buffer
		if(abSetFrameBuffer)
		{
			SetFrameBuffer(apFrameBuffer, abUsePosAndSize);
		}

		///////////////////////////////////
		// Set up rendering
		SetDepthTest(true);
		SetDepthWrite(false);
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_None);

		SetTextureRange(NULL,0);
		SetProgram(NULL);

		///////////////////////////////////
		// Render the queries
		for(size_t i=0; i<mvSortedOcclusionObjects.size(); ++i)
		{
			cOcclusionQueryObject *pObject = mvSortedOcclusionObjects[i];

			if(pObject->mbDepthTest)
				SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
			else
				SetDepthTestFunc(eDepthTestFunc_Always);

			SetMatrix(pObject->mpMatrix);


			SetVertexBuffer(pObject->mpVtxBuffer);

			pObject->mpQuery->Begin();
			DrawCurrent();
			pObject->mpQuery->End();
		}


		//Make sure rendering is on its way!
		mpLowLevelGraphics->FlushRendering();

		///////////////////////////////////
		// Reset some settings
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
		SetChannelMode(eMaterialChannelMode_RGBA);


		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void iRenderer::RetrieveAllLightOcclusionPair(bool abWaitForResult)
	{
		for(size_t i=0; i<mpCurrentSettings->mvLightOcclusionPairs.size(); ++i)
		{
			cLightOcclusionPair &loPair = mpCurrentSettings->mvLightOcclusionPairs[i];
			iOcclusionQuery *pQuery = loPair.mpQuery;

			if(pQuery == NULL) continue;

			//Wait for results if specfied.
			if(abWaitForResult)
			{
				while(pQuery->FetchResults());
			}

			//Get result and set resulting samples
			if(pQuery->FetchResults())
			{
				loPair.mlSampleResults = pQuery->GetSampleCount();
			}
			else
			{
				loPair.mlSampleResults =0;
			}

			loPair.mpQuery = NULL;
			ReleaseOcclusionQuery(pQuery);
		}
	}
	
	//-----------------------------------------------------------------------

	void iRenderer::RenderBasicSkyBox()
	{
		if(mpCurrentWorld==NULL || mpCurrentWorld->GetSkyBoxActive()==false) return;
		START_RENDER_PASS(BasicSkyBox);

		SetDepthTest(true);
		SetDepthWrite(false);

		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);
				
		/////////////////////////
		//Calculate and set matrix
		m_mtxSkyBox = cMatrixf::Identity;

		float fFarClip = mpCurrentFrustum->GetFarPlane();

		float fSide = sqrt((fFarClip*fFarClip) / 3) *0.95f;
		m_mtxSkyBox.m[0][0] = fSide;
		m_mtxSkyBox.m[1][1] = fSide;
		m_mtxSkyBox.m[2][2] = fSide;

		m_mtxSkyBox.SetTranslation(mpCurrentFrustum->GetOrigin());
		
		SetMatrix(&m_mtxSkyBox);

		/////////////////////////
		//Program
        SetProgram(NULL);
		
		/////////////////////////
		//Texture and vertex buffer
		SetTexture(0,mpCurrentWorld->GetSkyBoxTexture());
		SetTextureRange(NULL,1);
        
		SetVertexBuffer(mpCurrentWorld->GetSkyBoxVertexBuffer());

		DrawCurrent();

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	bool iRenderer::SetupLightScissorRect(iLight *apLight, cMatrixf *apViewSpaceMatrix)
	{
		if(mfScissorLastFov != mpCurrentFrustum->GetFOV())
		{
			mfScissorLastTanHalfFov = tan(mpCurrentFrustum->GetFOV()*0.5f);
		}
	
		/*cMath::GetClipRectFromBV(mTempClipRect,*apLight->GetBoundingVolume(),mpCurrentFrustum,
								mvScreenSize, mfScissorLastTanHalfFov);*/
		

		mTempClipRect = cMath::GetClipRectFromSphere(apViewSpaceMatrix->GetTranslation(),
													apLight->GetRadius(), mpCurrentFrustum,
													mvRenderTargetSize,true,mfScissorLastTanHalfFov);
		
		return SetScissorRect(mTempClipRect, true);
	}

	//-----------------------------------------------------------------------

	
	void iRenderer::SetMaterialProgram(eMaterialRenderMode aRenderMode, cMaterial *apMaterial)
	{
		iMaterialType *pMatType = apMaterial->GetType();
		iGpuProgram *pProgram = apMaterial->GetProgram(0,aRenderMode);
		
		///////////////////////////////////////
		// Check if program is set
		bool bNewProgramSet = false;
		if(mpCurrentProgram != pProgram)
		{
			bNewProgramSet = true;
			if(pProgram)
			{
				if(mbLog) Log("  Setting gpu program %d : '%s'\n", pProgram, pProgram->GetName().c_str());
				pProgram->Bind();	
			}
			else
			{
				if(mbLog) Log("  Setting gpu program NULL\n");
				mpCurrentProgram->UnBind();
			}
			
			mpCurrentProgram = pProgram;
		}

		///////////////////////////////////////
		// A program is currently set, check if variables need updating
		if(mpCurrentProgram)
		{
			//////////////////////
			//Check if Type specific changes are needed
			if(pMatType->HasTypeSpecifics(aRenderMode) && (bNewProgramSet || mpCurrentMaterialType != pMatType) )
			{
				if(mbLog) Log("  Setting up type specific program vars for material type %d/'%s'\n",pMatType,pMatType->GetName().c_str());
				pMatType->SetupTypeSpecificData(aRenderMode,pProgram,this);
				mpCurrentMaterialType = pMatType;
			}
			//////////////////////
			//Check if Material specific changes are needed
			if(apMaterial->GetHasSpecificSettings(aRenderMode) && (bNewProgramSet || mpCurrentMaterial != apMaterial) )
			{
				if(mbLog) Log("  Setting up material specific program vars for material %d/'%s'\n",apMaterial,apMaterial->GetName().c_str());
				pMatType->SetupMaterialSpecificData(aRenderMode,pProgram,apMaterial,this);
				mpCurrentMaterial = apMaterial;
			}
		}

		
		
		//TODO: If Cg is to be used, the worldviewproj matrix need to be setup!
	}

	//-----------------------------------------------------------------------

	void iRenderer::SetMaterialTextures(eMaterialRenderMode aRenderMode, cMaterial *apMaterial)
	{
		iMaterialType *pType = apMaterial->GetType();
		
		for(int i=0; i<kMaxTextureUnits; ++i)
		{
			//Set texture, if special textures are used, check for those too!
			iTexture *pTexture = apMaterial->GetTextureInUnit(aRenderMode,i);
			
			if(mvCurrentTexture[i] != pTexture)
			{
				if(mbLog) {
					if(pTexture)
						Log("  Setting texture unit: %d, %d/'%s'\n",i,pTexture,pTexture->GetName().c_str());
					else
						Log("  Setting texture unit: %d, 'NULL\n",i);
				}

				mpLowLevelGraphics->SetTexture(i, pTexture);
				mvCurrentTexture[i] = pTexture;
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderer::DrawCurrentMaterial(eMaterialRenderMode aRenderMode, iRenderable *apObject)
	{
		if(apObject)
		{
			cMaterial *pMaterial = apObject->GetMaterial();
			iMaterialType *pMatType = pMaterial->GetType();

			if(pMaterial->HasObjectSpecificsSettings(aRenderMode))
			{
				pMatType->SetupObjectSpecificData(aRenderMode,mpCurrentProgram,apObject,this);
			}
		}

		DrawCurrent();
	}
	
	//-----------------------------------------------------------------------

	bool iRenderer::CheckRenderablePlaneIsVisible(iRenderable *apObject, cFrustum *apFrustum)
	{
		if(apObject->GetRenderType() != eRenderableType_SubMesh) return true;
		
		cSubMeshEntity *pSubMeshEnt = static_cast<cSubMeshEntity*>(apObject);
		cSubMesh *pSubMesh = pSubMeshEnt->GetSubMesh();
		
		if(pSubMesh->GetIsOneSided()==false) return true;

		cVector3f vNormal = cMath::MatrixMul3x3(apObject->GetWorldMatrix(), pSubMesh->GetOneSidedNormal());
		cVector3f vPos = cMath::MatrixMul(apObject->GetWorldMatrix(), pSubMesh->GetOneSidedPoint());

		float fDot = cMath::Vector3Dot(vPos - apFrustum->GetOrigin(), vNormal);

		return fDot < 0;		
	}

	//-----------------------------------------------------------------------

	cRect2l iRenderer::GetClipRectFromObject(iRenderable *apObject, float afPaddingPercent, cFrustum *apFrustum, const cVector2l &avScreenSize, float afHalfFovTan)
	{
		cBoundingVolume *pBV = apObject->GetBoundingVolume();

		cRect2l clipRect;
		if(afHalfFovTan ==0)	
			afHalfFovTan = tan(apFrustum->GetFOV()*0.5f);
		cMath::GetClipRectFromBV(clipRect, *pBV, apFrustum, avScreenSize, afHalfFovTan);

		//Add 20% padding on clip rect
		int lXInc = (int)((float)clipRect.w*afHalfFovTan);
		int lYInc = (int)((float)clipRect.h*afHalfFovTan);

		clipRect.x = cMath::Max(clipRect.x-lXInc, 0);
		clipRect.y = cMath::Max(clipRect.y-lYInc, 0);
		clipRect.w = cMath::Min(clipRect.w+lXInc*2, avScreenSize.x-clipRect.x);
		clipRect.h = cMath::Min(clipRect.h+lYInc*2, avScreenSize.y-clipRect.y);

		return clipRect;
	}

	
	//-----------------------------------------------------------------------

	bool iRenderer::CheckObjectIsVisible(iRenderable *apObject, tRenderableFlag alNeededFlags)
	{
		/////////////////////////////
		// Is Visible var
		if(apObject->IsVisible()==false) return false;

		/////////////////////////////
		// Check flags
		if((apObject->GetRenderFlags() & alNeededFlags) != alNeededFlags) return false;

		/////////////////////////////
		// Clip plane check.
		// NOTE: This shall always be the user clip planes! Since we wanna cull stuff like nodes where clip planes are not active, etc.
		if(mvCurrentOcclusionPlanes.empty()==false && mbOcclusionPlanesActive)
		{
			cBoundingVolume *pBV = apObject->GetBoundingVolume();
			for(size_t i=0; i<mvCurrentOcclusionPlanes.size(); ++i)
			{
				cPlanef& plane = mvCurrentOcclusionPlanes[i];
				
				if(cMath::CheckPlaneBVCollision(plane, *pBV)==eCollision_Outside) return false;
			}
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderer::CheckNodeIsVisible(iRenderableContainerNode *apNode)
	{
		if(mbOcclusionPlanesActive==false || mvCurrentOcclusionPlanes.empty()) return true;

		// NOTE: This shall always be the user clip planes! The render function ones might not be active when culling is needed and so on.
		for(size_t i=0; i<mvCurrentOcclusionPlanes.size(); ++i)
		{
			cPlanef& plane = mvCurrentOcclusionPlanes[i];

			if(cMath::CheckPlaneAABBCollision(	plane, apNode->GetMin(), apNode->GetMax(), 
												apNode->GetCenter(), apNode->GetRadius())==eCollision_Outside)
			{
				return false;
			}
		}

        return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderer::CheckFogAreaInsideNearPlane(cMatrixf &a_mtxInvBoxModelMatrix)
	{
		cPlanef boxspaceNearPlane = cMath::TransformPlane(a_mtxInvBoxModelMatrix, mpCurrentFrustum->GetPlane(eFrustumPlane_Near));
		cVector3f vNearPlaneVtx[4];
		for(int i=0; i<4; ++i)
		{
			vNearPlaneVtx[i] = cMath::MatrixMul(a_mtxInvBoxModelMatrix, mpCurrentFrustum->GetVertex(i));
		}

		cVector3f vMin(-0.5f);
		cVector3f vMax(0.5f);

		//////////////////////////////
		// AABB vs Near Plane
		/*if(cMath::CheckPlaneAABBCollision(boxspaceNearPlane, vMin, vMax)!= eCollision_Intersect)
		{
			//return false;
		}*/

		//////////////////////////////
		// Near plane points vs AABB
		for(int i=0; i<4; ++i)
		{
			if(cMath::CheckPointInAABBIntersection(vNearPlaneVtx[i], vMin, vMax)) return true;
		}

		//////////////////////////////
		// Check if near plane points intersect with box
		if(cMath::CheckPointsAABBPlanesCollision(vNearPlaneVtx, 4, vMin, vMax)!=eCollision_Outside) return true;

		return false;
	}

	//-----------------------------------------------------------------------
	
	static const cVector3f gvFogBoxPlaneNormals[6] = {
			cVector3f(-1,0,0), //Left
			cVector3f(1,0,0), //Right

			cVector3f(0,-1,0), //Bottom	
			cVector3f(0,1,0), //Top

			cVector3f(0,0,-1), //Back
			cVector3f(0,0,1), //Front	
	};
	static const cVector3f gvFogBoxCompareSize = cVector3f(0.5001f);

	bool iRenderer::CheckFogAreaRayIntersection(cMatrixf &a_mtxInvBoxModelMatrix, const cVector3f &avBoxSpaceRayStart, const cVector3f& avRayDir,
												float &afEntryDist, float &afExitDist)
	{
		cVector3f vBoxSpaceDir = cMath::MatrixMul3x3(a_mtxInvBoxModelMatrix, avRayDir);

		bool bFoundIntersection=false;
		afExitDist = 0;

		///////////////////////////////////
		// Iterate the sides of the cube
		for(int i=0; i<6; ++i)
		{
			const cVector3f& vPlaneNormal = gvFogBoxPlaneNormals[i];

			///////////////////////////////////
			// Calculate plane intersection
			float fMul = cMath::Vector3Dot(vPlaneNormal, vBoxSpaceDir);
			if(fabs(fMul)<0.0001f) continue;
			float fNegDist = -(cMath::Vector3Dot(vPlaneNormal, avBoxSpaceRayStart)+0.5f);

			float fT = fNegDist / fMul;
			if(fT <0) continue;
			cVector3f vAbsNrmIntersect = cMath::Vector3Abs(vBoxSpaceDir*fT + avBoxSpaceRayStart);
			
			///////////////////////////////////
			// Check if the intersection is inside the cube
			if(cMath::Vector3LessEqual(vAbsNrmIntersect,  gvFogBoxCompareSize))
			{
				//////////////////////
				// First intersection
				if(bFoundIntersection==false)
				{
					afEntryDist = fT;
					afExitDist = fT;
					bFoundIntersection = true;
				}
				//////////////////////
				// There has already been a intersection.
				else
				{
					afEntryDist = cMath::Min(afEntryDist, fT);
					afExitDist = cMath::Max(afExitDist, fT);
				}
			}
		}

		if(afExitDist<0) return false;
		
		return bFoundIntersection;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_FogAreaData(const cFogAreaRenderData& aFogDataA, const cFogAreaRenderData& aFogDataB)
	{
		return aFogDataA.mpFogArea->GetViewSpaceZ() < aFogDataB.mpFogArea->GetViewSpaceZ();		
	}


	void iRenderer::SetupFogRenderDataArray(bool abSort)
	{
		mpCurrentSettings->mvFogRenderData.resize(0);
		for(int i=0; i<mpCurrentRenderList->GetFogAreaNum(); ++i)
		{
			cFogArea *pFogArea = mpCurrentRenderList->GetFogArea(i);
			cFogAreaRenderData fogData;

			fogData.mpFogArea = pFogArea;
			fogData.m_mtxInvBoxSpace = cMath::MatrixInverse( *pFogArea->GetModelMatrixPtr() );
			fogData.mbInsideNearFrustum = CheckFogAreaInsideNearPlane(fogData.m_mtxInvBoxSpace);
			fogData.mvBoxSpaceFrustumOrigin = cMath::MatrixMul(fogData.m_mtxInvBoxSpace, mpCurrentFrustum->GetOrigin());

			mpCurrentSettings->mvFogRenderData.push_back(fogData);
		}

		if(abSort && mpCurrentSettings->mvFogRenderData.empty()==false)
		{
			std::sort(mpCurrentSettings->mvFogRenderData.begin(), mpCurrentSettings->mvFogRenderData.end(), SortFunc_FogAreaData);
		}
	}

	//-----------------------------------------------------------------------

	float iRenderer::GetFogAreaVisibilityForObject(cFogAreaRenderData *apFogData, iRenderable *apObject)
	{
		cFogArea *pFogArea = apFogData->mpFogArea;

		cVector3f vObjectPos = apObject->GetBoundingVolume()->GetWorldCenter();
		cVector3f vRayDir = vObjectPos - mpCurrentFrustum->GetOrigin();
		float fCameraDistance = vRayDir.Length();
		vRayDir = vRayDir / fCameraDistance;

        float fEntryDist, fExitDist;
		if(CheckFogAreaRayIntersection(apFogData->m_mtxInvBoxSpace,apFogData->mvBoxSpaceFrustumOrigin, vRayDir, fEntryDist, fExitDist)==false)
		{
			return 1.0f;
		}
		
		if(apFogData->mbInsideNearFrustum==false && fCameraDistance < fEntryDist)
		{
			return 1.0f;
		}

		//////////////////////////////
		//Calculate the distance the ray travels in the fog
		float fFogDist;
		if(apFogData->mbInsideNearFrustum)
		{
			if(pFogArea->GetShowBacksideWhenInside())
				fFogDist = cMath::Min(fExitDist, fCameraDistance);
			else
				fFogDist = fCameraDistance;
		}
		else
		{
			if(pFogArea->GetShowBacksideWhenOutside())
				fFogDist = cMath::Min(fExitDist - fEntryDist, fCameraDistance - fEntryDist);
			else
				fFogDist = fCameraDistance - fEntryDist;
		}

		//////////////////////////////
		//Calculate the alpha
		if(fFogDist <=0) return 1.0f;
		
		float fFogStart = pFogArea->GetStart();
		float fFogEnd = pFogArea->GetEnd();
		float fFogAlpha = 1 - pFogArea->GetColor().a;

		if(fFogDist < fFogStart) return 1.0f;
		
		if(fFogDist > fFogEnd) return fFogAlpha;

		float fAlpha = (fFogDist - fFogStart) / (fFogEnd - fFogStart);
		if(pFogArea->GetFalloffExp()!=1) 
			fAlpha = powf(fAlpha, pFogArea->GetFalloffExp());

		return (1.0f-fAlpha) + fFogAlpha * fAlpha;
	}


	//-----------------------------------------------------------------------

	void iRenderer::SetOcclusionPlanesActive(bool abX)
	{
		mbOcclusionPlanesActive = abX;
	}
	
	//-----------------------------------------------------------------------

	iVertexBuffer* iRenderer::CreateQuadVertexBuffer(	eVertexBufferType aType,
		const cVector3f& avPos, const cVector2f& avSize, 
		const cVector2f& avMinUV, const cVector2f& avMaxUV,
		bool abInvertY)
	{
		iVertexBuffer *pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(	aType,
			eVertexBufferDrawType_Quad,
			eVertexBufferUsageType_Dynamic);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		for(int i=0; i<4; ++i) pVtxBuffer->AddIndex(i);

		float fMinUV_Y = abInvertY ? avMaxUV.y : avMinUV.y;
		float fMaxUV_Y = abInvertY ? avMinUV.y : avMaxUV.y;

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(avMinUV.x, fMinUV_Y,0)); 
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(avPos.x, avPos.y,avPos.z));

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(avMaxUV.x, fMinUV_Y,0)); 
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position,  cVector3f(avPos.x+avSize.x, avPos.y,avPos.z));

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(avMaxUV.x, fMaxUV_Y,0)); 
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position,  cVector3f(avPos.x+avSize.x, avPos.y+avSize.y,avPos.z));

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(avMinUV.x, fMaxUV_Y,0)); 
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position,  cVector3f(avPos.x, avPos.y+avSize.y,avPos.z));

		pVtxBuffer->Compile(0);

		return pVtxBuffer;
	}

	//-----------------------------------------------------------------------

	void iRenderer::UpdateqQuadVertexPostion(iVertexBuffer *apVtxBuffer,const cVector3f& avPos, const cVector2f& avSize, bool abCallUpdate)
	{
		int lVtxStride = apVtxBuffer->GetElementNum(eVertexBufferElement_Position);
		float *pPos = apVtxBuffer->GetFloatArray(eVertexBufferElement_Position);

		//0
		pPos[0*lVtxStride +0] = avPos.x;
		pPos[0*lVtxStride +1] = avPos.y;
		pPos[0*lVtxStride +2] = avPos.z;

		//1
		pPos[1*lVtxStride +0] = avPos.x+avSize.x;
		pPos[1*lVtxStride +1] = avPos.y;
		pPos[1*lVtxStride +2] = avPos.z;

		//2
		pPos[2*lVtxStride +0] = avPos.x+avSize.x;
		pPos[2*lVtxStride +1] = avPos.y+avSize.y;
		pPos[2*lVtxStride +2] = avPos.z;

		//3
		pPos[3*lVtxStride +0] = avPos.x;
		pPos[3*lVtxStride +1] = avPos.y+avSize.y;
		pPos[3*lVtxStride +2] = avPos.z;

		if(abCallUpdate) apVtxBuffer->UpdateData(eVertexElementFlag_Position,false);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* iRenderer::LoadVertexBufferFromMesh(const tString& asMeshName, tVertexElementFlag alVtxToCopy)
	{
		iVertexBuffer *pVtxBuffer = mpResources->GetMeshManager()-> CreateVertexBufferFromMesh(asMeshName, alVtxToCopy);
		if(pVtxBuffer==NULL) FatalError("Could not load vertex buffer from mesh '%s'\n",asMeshName.c_str());

		return pVtxBuffer;

		/*cMesh *pMesh = mpResources->GetMeshManager()->CreateMesh(asMeshName);
		if(pMesh==NULL)FatalError("Could not load mesh '%s'\n",asMeshName.c_str());

		iVertexBuffer *pVtxBuffer = pMesh->GetSubMesh(0)->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware,
																						eVertexBufferUsageType_Static,
																						alVtxToCopy);
		mpResources->GetMeshManager()->Destroy(pMesh);
		return pVtxBuffer;*/
	}

	//-----------------------------------------------------------------------

	void iRenderer::RunCallback(eRendererMessage aMessage)
	{
		if(mpCallbackList == NULL || mpCurrentSettings->mbUseCallbacks==false) return;

		tRendererCallbackListIt it = mpCallbackList->begin();
        for(; it != mpCallbackList->end(); ++it)
		{
			iRendererCallback *pCallback = *it;

            pCallback->RunMessage(aMessage, mpCallbackFunctions);
		}
	}

	//-----------------------------------------------------------------------

	eShadowMapResolution iRenderer::GetShadowMapResolution(eShadowMapResolution aWanted, eShadowMapResolution aMax)
	{
		if(aMax == eShadowMapResolution_High)
		{
			return aWanted;
		}
		else if(aMax == eShadowMapResolution_Medium)
		{
			return aWanted == eShadowMapResolution_High ? eShadowMapResolution_Medium : aWanted;
		}
		else
		{
			return eShadowMapResolution_Low;
		}
	}

	//-----------------------------------------------------------------------
	
	iOcclusionQuery *iRenderer::GetOcclusionQuery()
	{
		iOcclusionQuery *pOcclusionQuery = NULL;

		if(mvOcclusionQueryPool.empty())
		{
			pOcclusionQuery = mpLowLevelGraphics->CreateOcclusionQuery();
		}
		else
		{
			pOcclusionQuery = mvOcclusionQueryPool.back();
			mvOcclusionQueryPool.resize(mvOcclusionQueryPool.size()-1);
		}

		mlActiveOcclusionQueryNum++;

		return pOcclusionQuery;
	}
	
	void iRenderer::ReleaseOcclusionQuery(iOcclusionQuery * apQuery)
	{
		mlActiveOcclusionQueryNum--;

		mvOcclusionQueryPool.push_back(apQuery);
	}
	
	//-----------------------------------------------------------------------


}
