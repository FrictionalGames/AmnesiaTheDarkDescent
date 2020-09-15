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

#include "graphics/RendererDeferred.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/PreprocessParser.h"

#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Renderable.h"
#include "graphics/RenderList.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/FrameBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/ProgramComboManager.h"
#include "graphics/OcclusionQuery.h"
#include "graphics/TextureCreator.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/GpuShaderManager.h"
#include "resources/MeshManager.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"

#include "scene/Camera.h"
#include "scene/World.h"
#include "scene/RenderableContainer.h"
#include "scene/Light.h"
#include "scene/LightSpot.h"
#include "scene/LightBox.h"
#include "scene/FogArea.h"
#include "scene/MeshEntity.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// STATIC VARIABLES
	//////////////////////////////////////////////////////////////////////////

	eDeferredGBuffer cRendererDeferred::mGBufferType = eDeferredGBuffer_32Bit;
	int cRendererDeferred::mlNumOfGBufferTextures = 4;
	bool cRendererDeferred::mbDepthCullLights = true;

	bool cRendererDeferred::mbSSAOLoaded = false;
	int cRendererDeferred::mlSSAONumOfSamples = 8;
	int cRendererDeferred::mlSSAOBufferSizeDiv = 2;
	float cRendererDeferred::mfSSAOScatterLengthMul = 0.2f;
	float cRendererDeferred::mfSSAOScatterLengthMin = 0.015f;
	float cRendererDeferred::mfSSAOScatterLengthMax = 0.13f;
	float cRendererDeferred::mfSSAODepthDiffMul = 1.5f;
	float cRendererDeferred::mfSSAOSkipEdgeLimit = 3.0f;
	eDeferredSSAO cRendererDeferred::mSSAOType = eDeferredSSAO_OnColorBuffer;
	bool cRendererDeferred::mbEdgeSmoothLoaded = false;
	
	//debug
	bool cRendererDeferred::mbOcclusionTestLargeLights = true;
	bool cRendererDeferred::mbDebugRenderFrameBuffers = false;


	//////////////////////////////////////////////////////////////////////////
	// DEBUG DEFINES
	//////////////////////////////////////////////////////////////////////////

	//#define kDebug_RenderLightData

	//////////////////////////////////////////////////////////////////////////
	// LIGHT PROGRAM COMBOS
	//////////////////////////////////////////////////////////////////////////

	enum eDefferredProgramMode
	{
		eDefferredProgramMode_Lights,
		eDefferredProgramMode_Misc,
		eDefferredProgramMode_LastEnum
	};

	#define eFeature_Light_LightShapes		eFlagBit_0
	#define eFeature_Light_Batching			eFlagBit_1
	#define eFeature_Light_Specular			eFlagBit_2
	#define eFeature_Light_SpotLight		eFlagBit_3
	#define eFeature_Light_Gobo				eFlagBit_4
	#define eFeature_Light_DivideInFrag		eFlagBit_5
	#define eFeature_Light_ShadowMap		eFlagBit_6
	
	#define kLightFeatureNum 7

	cProgramComboFeature gvLightFeatureVec[] =
	{
		cProgramComboFeature("UseDeferredLightShapes", kPC_VertexBit),
		cProgramComboFeature("UseBatching", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseSpecular", kPC_FragmentBit),
		cProgramComboFeature("LightType_Spot", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseGobo", kPC_FragmentBit),
		cProgramComboFeature("DivideInFrag", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseShadowMap", kPC_FragmentBit, eFeature_Light_SpotLight),
	};

	//////////////////////////////////////////////////////////////////////////
	// FOG PROGRAM COMBOS
	//////////////////////////////////////////////////////////////////////////
	
	#define eFeature_FogArea_OutsideBox		eFlagBit_0
	#define eFeature_FogArea_Backside		eFlagBit_1
	
	#define kFogAreaFeatureNum 2

	cProgramComboFeature gvFogAreaFeatureVec[] =
	{
		cProgramComboFeature("OutsideBox", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseBackside", kPC_FragmentBit | kPC_VertexBit),
	};

	//////////////////////////////////////////////////////////////////////////
	// PROGRAM VARAIBLES
	//////////////////////////////////////////////////////////////////////////

	#define kVar_avLightPos							0
	#define kVar_avLightColor						1
	#define kVar_afInvLightRadius					2
	#define kVar_afNegFarPlane						3
	#define kVar_afOneMinusCosHalfSpotFOV			4
	#define kVar_avLightForward						5
	#define kVar_a_mtxSpotViewProj					6
	#define kVar_a_mtxInvViewRotation				7
	#define kVar_avShadowMapOffsetMul				8
	#define kVar_afNegInvFarPlane					9
	#define kVar_afFarPlane							10
	#define kVar_afScatterLengthMul					11
	#define kVar_avScreenSize						12
	#define kVar_avScatterLengthLimits				13
	#define kVar_avFogStartAndLength				14
	#define kVar_avFogColor							15
	#define kVar_avRayCastStart						16
	#define kVar_a_mtxBoxInvViewModelRotation		17
	#define kVar_avNegPlaneDistNeg					18
	#define kVar_avNegPlaneDistPos					19
	#define kVar_afFalloffExp						20
	#define kVar_afDepthDiffMul						21
	#define kVar_afSkipEdgeLimit					22


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRendererDeferred::cRendererDeferred(cGraphics *apGraphics,cResources* apResources) 
		: iRenderer("Deferred",apGraphics, apResources,eDefferredProgramMode_LastEnum)
	{
		////////////////////////////////////
		// Set up render specific things
		mbSetFrameBufferAtBeginRendering = false;		//Not using the input frame buffer for any rendering. Only doing copy at the end!
		mbClearFrameBufferAtBeginRendering = false;
		mbSetupOcclusionPlaneForFog = true;

		////////////////////////////////////
		// Set up variables
		mfLastFrustumFOV = -1;
		mfLastFrustumFarPlane = -1;
		
		mfMinLargeLightNormalizedArea = 0.2f*0.2f;
		mfMinRenderReflectionNormilzedLength = 0.15f;

		mfShadowDistanceMedium = 10;
		mfShadowDistanceLow = 20;
		mfShadowDistanceNone = 40;

		mlMaxBatchLights = 100;

		mbReflectionTextureCleared = false;
	}

	//-----------------------------------------------------------------------

	cRendererDeferred::~cRendererDeferred()
	{
		STLDeleteAll(mvTempDeferredLights);	
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cRendererDeferred::LoadData()
	{
		cVector2l vRelfectionSize = cVector2l(mvScreenSize.x/mlReflectionSizeDiv, mvScreenSize.y/mlReflectionSizeDiv);

		Log("Setting up G-Bugger: type: %d texturenum: %d\n", mGBufferType, mlNumOfGBufferTextures);
		////////////////////////////////////
		//Create G-Buffer textures
		for(int i=0; i<mlNumOfGBufferTextures; ++i)
		{
			ePixelFormat pixelFormat = mGBufferType == eDeferredGBuffer_32Bit ? ePixelFormat_RGBA : ePixelFormat_RGBA16;
			//ePixelFormat pixelFormat = ePixelFormat_RGBA16;

			tString sName = "G-BufferTexure"+cString::ToString(i);
			mpGBufferTexture[0][i] = CreateRenderTexture(sName, mvScreenSize,pixelFormat,eTextureFilter_Nearest);
			mpGBufferTexture[1][i] = CreateRenderTexture(sName+"_Reflection", vRelfectionSize, pixelFormat,eTextureFilter_Nearest);
		}

		////////////////////////////////////
		//Create Depth and stencil
		mpDepthStencil[0] = mpGraphics->CreateDepthStencilBuffer(mvScreenSize,24,8, false);
		mpDepthStencil[1] = mpGraphics->CreateDepthStencilBuffer(vRelfectionSize, 24,8, false);
		
		////////////////////////////////////
		//Create Frame buffers
		for(int type=0; type<2; ++type)
		{
			mpGBuffer[type][eGBufferComponents_Full] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_Main");
			for(int i=0;i<mlNumOfGBufferTextures; ++i) 
			{
				mpGBuffer[type][eGBufferComponents_Full]->SetTexture2D(i,mpGBufferTexture[type][i]);
			}
			mpGBuffer[type][eGBufferComponents_Full]->SetDepthStencilBuffer(mpDepthStencil[type]);
			if(mpGBuffer[type][eGBufferComponents_Full]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer for Deferred renderer type: %d!\n", type);
				return false;
			}

			/////////////////////////
			//Frame buffer with only color
			mpGBuffer[type][eGBufferComponents_Color] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_ColorDepth");
			mpGBuffer[type][eGBufferComponents_Color]->SetTexture2D(0,mpGBufferTexture[type][0]);
			if(mpGBuffer[type][eGBufferComponents_Color]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer with only color Deferred renderer type: %d!\n", type);
				return false;
			}

			/////////////////////////
			//Frame buffer with only depth
			mpGBuffer[type][eGBufferComponents_Depth] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_Depth");
			mpGBuffer[type][eGBufferComponents_Depth]->SetDepthStencilBuffer(mpDepthStencil[type]);
			if(mpGBuffer[type][eGBufferComponents_Depth]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer with only depth for Deferred renderer type: %d!\n", type);
				return false;
			}
			
			/////////////////////////
			//Frame buffer with only depth and color
			mpGBuffer[type][eGBufferComponents_ColorAndDepth] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_ColorDepth");
			mpGBuffer[type][eGBufferComponents_ColorAndDepth]->SetTexture2D(0,mpGBufferTexture[type][0]);
			mpGBuffer[type][eGBufferComponents_ColorAndDepth]->SetDepthStencilBuffer(mpDepthStencil[type]);
			if(mpGBuffer[type][eGBufferComponents_ColorAndDepth]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer with only color and depth for Deferred renderer type: %d!\n", type);
				return false;
			}

			/////////////////////////
			//Frame buffer with only normals
			mpGBuffer[type][eGBufferComponents_Normals] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_Normals");
			mpGBuffer[type][eGBufferComponents_Normals]->SetTexture2D(0,mpGBufferTexture[type][1]);
			if(mpGBuffer[type][eGBufferComponents_Normals]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer with only normals for Deferred renderer type: %d!\n", type);
				return false;
			}

			/////////////////////////
			//Frame buffer with only linear depth
			mpGBuffer[type][eGBufferComponents_LinearDepth] = mpGraphics->CreateFrameBuffer("Deferred_GBuffer_LinearDepth");
			mpGBuffer[type][eGBufferComponents_LinearDepth]->SetTexture2D(0,mpGBufferTexture[type][2]);
			if(mpGBuffer[type][eGBufferComponents_LinearDepth]->CompileAndValidate()==false)
			{
				Error("Could not create frame buffer with only linear depth for Deferred renderer type: %d!\n", type);
				return false;
			}
		}
		
		////////////////////////////////////
		//Create Accumulation texture
		mpAccumBufferTexture = mpGraphics->CreateTexture("AccumBiffer",eTextureType_Rect,eTextureUsage_RenderTarget);
		mpAccumBufferTexture->CreateFromRawData(cVector3l(mvScreenSize.x, mvScreenSize.y,0),ePixelFormat_RGBA, NULL);
		mpAccumBufferTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

		////////////////////////////////////
		//Create Accumulation buffer
		mpAccumBuffer = mpGraphics->CreateFrameBuffer("Deferred_Accumulation");
		mpAccumBuffer->SetTexture2D(0,mpAccumBufferTexture);
		mpAccumBuffer->SetDepthStencilBuffer(mpDepthStencil[0]);

		mpAccumBuffer->CompileAndValidate();

		////////////////////////////////////
		//Create Refraction texture
		mpRefractionTexture = mpGraphics->GetTempFrameBuffer(mvScreenSize,ePixelFormat_RGBA,0)->GetColorBuffer(0)->ToTexture();
		mpRefractionTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

		////////////////////////////////////
		//Create Reflection texture
		mpReflectionTexture = CreateRenderTexture("ReflectionTexture",vRelfectionSize,ePixelFormat_RGBA);
		
		////////////////////////////////////
		//Create Reflection buffer
		mpReflectionBuffer = mpGraphics->CreateFrameBuffer("Deferred_Reflection");
		mpReflectionBuffer->SetTexture2D(0,mpReflectionTexture);
		mpReflectionBuffer->SetDepthStencilBuffer(mpDepthStencil[1]);
		mpReflectionBuffer->CompileAndValidate();
		
		
		////////////////////////////////////
		//Create Shadow Textures
		cVector3l vShadowSize[] = {
									cVector3l(128, 128,1),
									cVector3l(256, 256,1),
									cVector3l(256, 256,1),
									cVector3l(512, 512,1),
									cVector3l(1024, 1024,1)
		};
		int lStartSize = 2;
		if(mShadowMapResolution == eShadowMapResolution_Medium)		lStartSize = 1;
		else if(mShadowMapResolution == eShadowMapResolution_Low)	lStartSize = 0;

        for(int i=0; i<1; ++i)
			CreateAndAddShadowMap(eShadowMapResolution_High, vShadowSize[lStartSize + eShadowMapResolution_High],ePixelFormat_Depth16);
		for(int i=0; i<4; ++i)
			CreateAndAddShadowMap(eShadowMapResolution_Medium, vShadowSize[lStartSize + eShadowMapResolution_Medium],ePixelFormat_Depth16);
		for(int i=0; i<6; ++i)
			CreateAndAddShadowMap(eShadowMapResolution_Low, vShadowSize[lStartSize + eShadowMapResolution_Low],ePixelFormat_Depth16);
		
		
		// Select samples depending quality and shader model (if dynamic branching is supported)
        if(mpLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_4))
		{
			//High
			if(mShadowMapQuality == eShadowMapQuality_High)	{
				mlShadowJitterSize = 32;
				mlShadowJitterSamples = 32;	//64 here instead? I mean, ATI has to deal with medium has max? or different max for ATI?
			}
			//Medium
			else if(mShadowMapQuality == eShadowMapQuality_Medium) {
				mlShadowJitterSize = 32;
				mlShadowJitterSamples = 16;
			}
			//Low
			else {
				mlShadowJitterSize = 0;
				mlShadowJitterSamples = 0;
			}
		}
		//No dynamic branching
		else
		{
			//High
			if(mShadowMapQuality == eShadowMapQuality_High)	{
				mlShadowJitterSize = 32;
				mlShadowJitterSamples = 16;
			}
			//Medium
			else if(mShadowMapQuality == eShadowMapQuality_Medium)	{
				mlShadowJitterSize = 32;
				mlShadowJitterSamples = 4;
			}
			//Low
			else {
				mlShadowJitterSize = 0;
				mlShadowJitterSamples = 0;
			}
		}
		
		if(mShadowMapQuality != eShadowMapQuality_Low)
		{
			mpShadowJitterTexture = mpGraphics->CreateTexture("ShadowOffset", eTextureType_2D, eTextureUsage_Normal);
			mpGraphics->GetTextureCreator()->GenerateScatterDiskMap2D(mpShadowJitterTexture,mlShadowJitterSize,mlShadowJitterSamples, true);
		}
		else
		{
			mpShadowJitterTexture = NULL;
		}

		////////////////////////////////////
		//Create Sky box program
		{
			cParserVarContainer vars;
			vars.Add("UseUv");
			iGpuShader *pVtxShader = mpShaderManager->CreateShader("deferred_base_vtx.glsl",eGpuShaderType_Vertex,&vars);
			iGpuShader *pFragShader = mpShaderManager->CreateShader("deferred_gbuffer_skybox_frag.glsl", eGpuShaderType_Fragment,&vars);

			mpSkyBoxProgram = mpGraphics->CreateGpuProgram("DeferredSkyBox");
			mpSkyBoxProgram->SetShader(eGpuShaderType_Vertex, pVtxShader);
			mpSkyBoxProgram->SetShader(eGpuShaderType_Fragment, pFragShader);
			mpSkyBoxProgram->Link();
		}
		
		
		////////////////////////////////////
		//Create Fog program
		{
			cParserVarContainer vars;
			if(GetGBufferType() == eDeferredGBuffer_32Bit)	vars.Add("PackedDepth");
			
			mpFogProgramManager = hplNew(	cProgramComboManager, ("FogArea", mpGraphics, mpResources, 1));

			mpFogProgramManager->SetupGenerateProgramData(0,"Fog","deferred_fog_vtx.glsl","deferred_fog_frag.glsl",gvFogAreaFeatureVec,kFogAreaFeatureNum,vars);

			if(GetGBufferType() == eDeferredGBuffer_32Bit)
					mpFogProgramManager->AddGenerateProgramVariableId("afNegFarPlane", kVar_afNegFarPlane,0);
			mpFogProgramManager->AddGenerateProgramVariableId("avFogStartAndLength", kVar_avFogStartAndLength,0);
			mpFogProgramManager->AddGenerateProgramVariableId("avFogColor", kVar_avFogColor,0);
			mpFogProgramManager->AddGenerateProgramVariableId("avRayCastStart", kVar_avRayCastStart,0);
			mpFogProgramManager->AddGenerateProgramVariableId("a_mtxBoxInvViewModelRotation", kVar_a_mtxBoxInvViewModelRotation,0);
			mpFogProgramManager->AddGenerateProgramVariableId("avNegPlaneDistNeg",kVar_avNegPlaneDistNeg,0);
			mpFogProgramManager->AddGenerateProgramVariableId("avNegPlaneDistPos",kVar_avNegPlaneDistPos,0);
			mpFogProgramManager->AddGenerateProgramVariableId("afFalloffExp",kVar_afFalloffExp,0);
		}
		
		////////////////////////////////////
		//Create Light programs
		{
			/////////////////////////////
			//Misc
			{
				cParserVarContainer vars;

				//////////////
				//Light Stencil
				mpLightStencilProgram = mpProgramManager->CreateProgramFromShaders("LightStencil",
																					"deferred_base_vtx.glsl",
																					"deferred_base_frag.glsl",
																					&vars,true);

				//////////////
				//Light box
				for(int i=0;i<2; ++i)
				{
					if(i==1) vars.Add("UseSSAO");
					mpLightBoxProgram[i] = mpProgramManager->CreateProgramFromShaders("LightBoxNormal",
																					"deferred_base_vtx.glsl",
																					"deferred_light_box_frag.glsl",
																					&vars,true);

					vars.Clear();
					if(mpLightBoxProgram[i])
					{
						mpLightBoxProgram[i]->GetVariableAsId("avLightColor",kVar_avLightColor);
					}
				}
			}
			
			/////////////////////////////
			//Lights
			{
				cParserVarContainer defaultVars;
				
				//Shadow variables
				defaultVars.Add("ShadowJitterLookupMul",1.0f / (float)mlShadowJitterSize);
				defaultVars.Add("ShadowJitterSamplesDiv2",mlShadowJitterSamples / 2);
				defaultVars.Add("ShadowJitterSamples", mlShadowJitterSamples);

				if(mShadowMapQuality == eShadowMapQuality_High)		defaultVars.Add("ShadowMapQuality_High");
				if(mShadowMapQuality == eShadowMapQuality_Medium)	defaultVars.Add("ShadowMapQuality_Medium");
				if(mShadowMapQuality == eShadowMapQuality_Low)		defaultVars.Add("ShadowMapQuality_Low");

				//Vertex shader will handles deferred lights
				defaultVars.Add("DeferredLight");

				//Deferred renderer type
				if(mGBufferType == eDeferredGBuffer_32Bit)	defaultVars.Add("Deferred_32bit","");
				else										defaultVars.Add("Deferred_64bit","");
				
				if(mlNumOfGBufferTextures == 4)				defaultVars.Add("RenderTargets_4","");
				else										defaultVars.Add("RenderTargets_3","");

				mpProgramManager->SetupGenerateProgramData(	eDefferredProgramMode_Lights,"Lights", "deferred_base_vtx.glsl", "deferred_light_frag.glsl",gvLightFeatureVec,
													kLightFeatureNum,defaultVars);
													//1, defaultVars);

				
				mpProgramManager->AddGenerateProgramVariableId("avLightPos",	kVar_avLightPos, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("avLightColor",	kVar_avLightColor, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("afInvLightRadius",	kVar_afInvLightRadius, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("afNegFarPlane",	kVar_afNegFarPlane, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("afOneMinusCosHalfSpotFOV",	kVar_afOneMinusCosHalfSpotFOV, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("avLightForward", kVar_avLightForward, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("a_mtxSpotViewProj", kVar_a_mtxSpotViewProj, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("a_mtxInvViewRotation", kVar_a_mtxInvViewRotation, eDefferredProgramMode_Lights);
				mpProgramManager->AddGenerateProgramVariableId("avShadowMapOffsetMul", kVar_avShadowMapOffsetMul, eDefferredProgramMode_Lights);
			}

			//////////////////////////////
			// Generate some light programs
			//for(i=0; i<128; ++i)
			//	mpProgramManager->GenerateProgram(eDefferredProgramMode_Lights, i);
		}

		////////////////////////////////////
		//Create SSAO programs and textures
		if(mbSSAOLoaded && mpLowLevelGraphics->GetCaps(eGraphicCaps_TextureFloat)==0)
		{
			mbSSAOLoaded = false;
			Warning("System does not support float textures! SSAO is disabled.\n");
		}
		if(mbSSAOLoaded)
		{
			cVector2l vSSAOSize = mvScreenSize / mlSSAOBufferSizeDiv;
			
			/////////////////////////////////////
			// Textures and frame buffers

			// Textures
			mpLinearDepthTexture = CreateRenderTexture("LinearDepth", vSSAOSize,ePixelFormat_RGB16);
			mpSSAOTexture = CreateRenderTexture("SSAO", vSSAOSize,ePixelFormat_RGB);
			mpSSAOBlurTexture = CreateRenderTexture("SSAOBlur", vSSAOSize,ePixelFormat_RGB);

			//Frame buffers
			mpLinearDepthBuffer = mpGraphics->CreateFrameBuffer("LinearDepth");
			mpLinearDepthBuffer->SetTexture2D(0,mpLinearDepthTexture);
			mpLinearDepthBuffer->CompileAndValidate();

			mpSSAOBuffer = mpGraphics->CreateFrameBuffer("SSAO");
			mpSSAOBuffer->SetTexture2D(0,mpSSAOTexture);
			mpSSAOBuffer->CompileAndValidate();

			mpSSAOBlurBuffer = mpGraphics->CreateFrameBuffer("SSAOBlur");
			mpSSAOBlurBuffer->SetTexture2D(0,mpSSAOBlurTexture);
			mpSSAOBlurBuffer->CompileAndValidate();

			//Scatter disk
			mpSSAOScatterDisk = mpGraphics->CreateTexture("SSAOScatterDisk", eTextureType_2D,eTextureUsage_Normal);
			mpGraphics->GetTextureCreator()->GenerateScatterDiskMap2D(mpSSAOScatterDisk,4, mlSSAONumOfSamples, false);
			
			
			/////////////////////////////////////
			// Programs
			cParserVarContainer programVars;

			//Program for unpacking depth to a lower resolution texture
			if(mGBufferType == eDeferredGBuffer_32Bit)	programVars.Add("Deferred_32bit");
			else										programVars.Add("Deferred_64bit");
			programVars.Add("UseUv");
			mpUnpackDepthProgram = mpGraphics->CreateGpuProgramFromShaders("UnpackDepth","deferred_base_vtx.glsl", "deferred_unpack_depth_frag.glsl",&programVars);
			if(mpUnpackDepthProgram)
			{
				mpUnpackDepthProgram->GetVariableAsId("afNegInvFarPlane",kVar_afNegInvFarPlane);
			}
			programVars.Clear();
			
			//SSAO Blur programs (vertical and horizontal)
			programVars.Add("BlurHorisontal");
			mpSSAOBlurProgram[0] = mpGraphics->CreateGpuProgramFromShaders("SSAOBlurHori","deferred_ssao_blur_vtx.glsl", "deferred_ssao_blur_frag.glsl",&programVars);
			programVars.Clear();
			mpSSAOBlurProgram[1] = mpGraphics->CreateGpuProgramFromShaders("SSAOBlurVert","deferred_ssao_blur_vtx.glsl", "deferred_ssao_blur_frag.glsl",&programVars);

			for(int i=0; i<2; ++i)
			{
				if(mpSSAOBlurProgram[i])
				{
					mpSSAOBlurProgram[i]->GetVariableAsId("afFarPlane",kVar_afFarPlane);
				}
			}
			
			//SSAO Rendering
			programVars.Add("SampleNumDiv2", mlSSAONumOfSamples / 2);
			mpSSAORenderProgram = mpGraphics->CreateGpuProgramFromShaders(	"SSAORender","deferred_ssao_render_vtx.glsl", 
																			"deferred_ssao_render_frag.glsl",&programVars);
			if(mpSSAORenderProgram)
			{
				mpSSAORenderProgram->GetVariableAsId("afFarPlane",kVar_afFarPlane);
				mpSSAORenderProgram->GetVariableAsId("afScatterLengthMul", kVar_afScatterLengthMul);
				mpSSAORenderProgram->GetVariableAsId("avScatterLengthLimits", kVar_avScatterLengthLimits);
				mpSSAORenderProgram->GetVariableAsId("avScreenSize", kVar_avScreenSize);
				mpSSAORenderProgram->GetVariableAsId("afDepthDiffMul", kVar_afDepthDiffMul);
				mpSSAORenderProgram->GetVariableAsId("afSkipEdgeLimit", kVar_afSkipEdgeLimit);
			}
		}

		////////////////////////////////////
		//Create Smooth Edge and textures
		if(mbEdgeSmoothLoaded && mpLowLevelGraphics->GetCaps(eGraphicCaps_TextureFloat)==0)
		{
			mbEdgeSmoothLoaded = false;
			Warning("System does not support float textures! Edge smooth is disabled.\n");
		}
		if(mbEdgeSmoothLoaded)
		{
			/////////////////////////////////////
			// Textures and frame buffers

			// Textures
			mpEdgeSmooth_LinearDepthTexture = CreateRenderTexture("EdgeSmoothLinearDepth", mvScreenSize,ePixelFormat_RGB16);
			mpEdgeSmooth_TempAccum = mpGraphics->GetTempFrameBuffer(mvScreenSize,ePixelFormat_RGBA,0)->GetColorBuffer(0)->ToTexture();

			//Frame buffers
			mpEdgeSmooth_LinearDepthBuffer = mpGraphics->CreateFrameBuffer("EdgeSmoothLinearDepth");
			mpEdgeSmooth_LinearDepthBuffer->SetTexture2D(0,mpEdgeSmooth_LinearDepthTexture);
			mpEdgeSmooth_LinearDepthBuffer->CompileAndValidate();

			/////////////////////////////////////
			// Programs

			cParserVarContainer programVars;

			//Program for unpacking depth
			if(mGBufferType == eDeferredGBuffer_32Bit)	programVars.Add("Deferred_32bit");
			else										programVars.Add("Deferred_64bit");
			programVars.Add("UseUv");
			mpEdgeSmooth_UnpackDepthProgram = mpGraphics->CreateGpuProgramFromShaders("EdgeSmoothUnpackDepth","deferred_base_vtx.glsl", "deferred_unpack_depth_frag.glsl",&programVars);
			if(mpEdgeSmooth_UnpackDepthProgram)
			{
				mpEdgeSmooth_UnpackDepthProgram->GetVariableAsId("afNegInvFarPlane",kVar_afNegInvFarPlane);
			}
			programVars.Clear();

			//Program for edge smoothing
			programVars.Add("UseUv");
			mpEdgeSmooth_RenderProgram =  mpGraphics->CreateGpuProgramFromShaders("EdgeSmoothRender","deferred_base_vtx.glsl", "deferred_edge_smooth_frag.glsl",&programVars);
			if(mpEdgeSmooth_RenderProgram)
			{
				mpEdgeSmooth_RenderProgram->GetVariableAsId("afFarPlane",kVar_afFarPlane);
			}
		}

		////////////////////////////////////
		//Create light shapes
		tFlag lVtxFlag = eVertexElementFlag_Position | eVertexElementFlag_Color0 | eVertexElementFlag_Texture0;
		mpShapeSphere[eDeferredShapeQuality_High] = LoadVertexBufferFromMesh("core_12_12_sphere.dae",lVtxFlag);	
		mpShapeSphere[eDeferredShapeQuality_Medium] = LoadVertexBufferFromMesh("core_7_7_sphere.dae",lVtxFlag);
		mpShapeSphere[eDeferredShapeQuality_Low] = LoadVertexBufferFromMesh("core_5_5_sphere.dae",lVtxFlag);

		mpShapePyramid = LoadVertexBufferFromMesh("core_pyramid.dae",lVtxFlag);
		
		////////////////////////////////////
		//Quad used when rendering light.
		mpFullscreenLightQuad = CreateQuadVertexBuffer(eVertexBufferType_Software,0,1,0,mvScreenSizeFloat, true);
		
		////////////////////////////////////
		//Batch vertex buffer
		mlMaxBatchVertices = mpShapeSphere[eDeferredShapeQuality_Low]->GetVertexNum() * mlMaxBatchLights;
		mlMaxBatchIndices = mpShapeSphere[eDeferredShapeQuality_Low]->GetIndexNum() * mlMaxBatchLights;
		mpBatchBuffer = mpLowLevelGraphics->CreateVertexBuffer(	eVertexBufferType_Software,
																eVertexBufferDrawType_Quad, eVertexBufferUsageType_Stream,
																mlMaxBatchVertices,mlMaxBatchIndices);
		mpBatchBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		mpBatchBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		mpBatchBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Byte,4);
		mpBatchBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);
		mpBatchBuffer->CreateElementArray(eVertexBufferElement_Texture1,eVertexBufferElementFormat_Float,3);

		mpBatchBuffer->Compile(0);
        
		return true;
	}

	//-----------------------------------------------------------------------

	
	void cRendererDeferred::DestroyData()
	{
		/////////////////////////
		//Vertex buffers
		if(mpBatchBuffer) hplDelete(mpBatchBuffer);

		if(mpFullscreenLightQuad) hplDelete(mpFullscreenLightQuad);

		for(int i=0;i< eDeferredShapeQuality_LastEnum; ++i)
		{
			if(mpShapeSphere[i]) hplDelete(mpShapeSphere[i]);
		}
		if(mpShapePyramid) hplDelete(mpShapePyramid);
		
		
		/////////////////////////
		//G-Buffer and Accum buffer
		for(int type=0; type<2; ++type)
		{
			for(int j=0; j<eGBufferComponents_LastEnum; ++j)
			{
				mpGraphics->DestroyFrameBuffer(mpGBuffer[type][j]);	
			}
			
			for(int j=0; j<mlNumOfGBufferTextures; ++j)
			{
				mpGraphics->DestroyTexture(mpGBufferTexture[type][j]);	
			}

			mpGraphics->DestoroyDepthStencilBuffer(mpDepthStencil[type]);
		}
		
		mpGraphics->DestroyFrameBuffer(mpAccumBuffer);	
		mpGraphics->DestroyFrameBuffer(mpReflectionBuffer);
		
		mpGraphics->DestroyTexture(mpAccumBufferTexture);
		mpGraphics->DestroyTexture(mpReflectionTexture);
		
		
		/////////////////////////
		//Shadow textures
		DestroyShadowMaps();
		
		if(mpShadowJitterTexture) mpGraphics->DestroyTexture(mpShadowJitterTexture);

		/////////////////////////
		//Fog stuff
		hplDelete(mpFogProgramManager);

		/////////////////////////
		//SSAO textures and programs
		if(mbSSAOLoaded)
		{
			mpGraphics->DestroyTexture(mpLinearDepthTexture);
			mpGraphics->DestroyTexture(mpSSAOTexture);
			mpGraphics->DestroyTexture(mpSSAOBlurTexture);
			mpGraphics->DestroyTexture(mpSSAOScatterDisk);

			mpGraphics->DestroyFrameBuffer(mpLinearDepthBuffer);
			mpGraphics->DestroyFrameBuffer(mpSSAOBuffer);
			mpGraphics->DestroyFrameBuffer(mpSSAOBlurBuffer);

			mpGraphics->DestroyGpuProgram(mpUnpackDepthProgram);
			for(int i=0;i<2; ++i)
				mpGraphics->DestroyGpuProgram(mpSSAOBlurProgram[i]);
			mpGraphics->DestroyGpuProgram(mpSSAORenderProgram);
		}

		/////////////////////////////
		// Edge smooth
		if(mbEdgeSmoothLoaded)
		{
			mpGraphics->DestroyTexture(mpEdgeSmooth_LinearDepthTexture);
			mpGraphics->DestroyFrameBuffer(mpEdgeSmooth_LinearDepthBuffer);
			
			mpGraphics->DestroyGpuProgram(mpEdgeSmooth_UnpackDepthProgram);
			mpGraphics->DestroyGpuProgram(mpEdgeSmooth_RenderProgram);
		}
		
		/////////////////////////
		//Gpu programs
		mpGraphics->DestroyGpuProgram(mpSkyBoxProgram);

		mpProgramManager->DestroyShadersAndPrograms();
	}

	//-----------------------------------------------------------------------

	iTexture* cRendererDeferred::GetPostEffectTexture()
	{
		return mpAccumBufferTexture;

		//This should never be needed since if a post effect is used, 
		/*if(mpCurrentRenderTarget->mpFrameBuffer==NULL)
		{
			return mpAccumBuffer;
		}
		else
		{
			return mpCurrentRenderTarget->mpFrameBuffer;
		}*/
	}

	iTexture* cRendererDeferred::GetGbufferTexture(int alIdx)
	{ 
		int lType = mpCurrentSettings->mbIsReflection ? 1 : 0;
		return mpGBufferTexture[lType][alIdx];
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRendererDeferred::CopyToFrameBuffer()
	{
		if(mpCurrentSettings->mbIsReflection) return;

		START_RENDER_PASS(CopyToFrameBuffer);

		SetDepthTest(false);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);
		
		SetFrameBuffer(mpCurrentRenderTarget->mpFrameBuffer,true);

		SetFlatProjection();

		SetProgram(NULL);
		SetTexture(0,mpAccumBufferTexture);
		SetTextureRange(NULL, 1);

		////////////////////////////////////
		//Draw the accumulation buffer to the current frame buffer
		//Since the texture v coordinate is reversed, need to do some math.
		cVector2f vViewportPos((float)mpCurrentRenderTarget->mvPos.x, (float)mpCurrentRenderTarget->mvPos.y);
		cVector2f vViewportSize((float)mvRenderTargetSize.x, (float)mvRenderTargetSize.y);
		DrawQuad(	cVector2f(0,0),1,
					cVector2f(vViewportPos.x, (mvScreenSizeFloat.y - vViewportSize.y)-vViewportPos.y ), 
					cVector2f(vViewportPos.x + vViewportSize.x,mvScreenSizeFloat.y - vViewportPos.y),
					true);

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------
	
	void cRendererDeferred::SetupRenderList()
	{
		mpCurrentRenderList->Setup(mfCurrentFrameTime,mpCurrentFrustum);
	}

		//-----------------------------------------------------------------------
	
	void cRendererDeferred::RenderObjects()
	{
		//Set up variables used in rendering later on.
		SetupRenderVariables();
		
		//Set up the frame buffers needed for G-buffer
		SetupGBuffer();

		tRenderableFlag lVisibleFlags=0;
		if(mpCurrentSettings->mbIsReflection)	lVisibleFlags |= eRenderableFlag_VisibleInReflection;
		else									lVisibleFlags |= eRenderableFlag_VisibleInNonReflection;
		
		///////////////////////////
		//Occlusion testing
		if(mpCurrentSettings->mbUseOcclusionCulling)
		{
			CheckForVisibleObjectsAddToListAndRenderZ(	mpCurrentSettings->mpVisibleNodeTracker,eObjectVariabilityFlag_All, lVisibleFlags, 
														true, NULL);

			AssignAndRenderOcclusionQueryObjects(false, NULL, true);

			SetupLightsAndRenderQueries();

			mpCurrentRenderList->Compile(	eRenderListCompileFlag_Diffuse |
											eRenderListCompileFlag_Translucent |
											eRenderListCompileFlag_Decal |
											eRenderListCompileFlag_Illumination);
			if(mbLog)mpCurrentRenderList->PrintAllObjects();
			//RenderDynamicZTemp();

		}
		///////////////////////////
		//Brute force
		else
		{
			CheckForVisibleAndAddToList(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Static), lVisibleFlags);
			CheckForVisibleAndAddToList(mpCurrentWorld->GetRenderableContainer(eWorldContainerType_Dynamic), lVisibleFlags);
			
			mpCurrentRenderList->Compile(	eRenderListCompileFlag_Z |
											eRenderListCompileFlag_Diffuse |
											eRenderListCompileFlag_Translucent |
											eRenderListCompileFlag_Decal |
											eRenderListCompileFlag_Illumination);
			if(mbLog)mpCurrentRenderList->PrintAllObjects();
			RenderZ(); 

			AssignAndRenderOcclusionQueryObjects(false, NULL, true);

			SetupLightsAndRenderQueries();
		}
		
		RenderGbuffer();
		if(mbDebugRenderFrameBuffers)
		{
			RenderGbufferContent();
			return;
		}
		
		RenderDecals();

		RunCallback(eRendererMessage_PostGBuffer);

		//RenderDeferredSkyBox();

		/*if(mpCurrentSettings->mbIsReflection)
		{
			RenderGbufferContent(); //Debug, used to see what gbuffers contain
			return;
		}*/
		RenderLights();
		
		//Debug:
		//RenderSSAO();
		//return;


		RenderIllumination();

		RenderFog();
		RenderFullScreenFog();

		RenderEdgeSmooth();

		#ifndef kDebug_RenderLightData
		RenderBasicSkyBox();
		#endif

		RunCallback(eRendererMessage_PostSolid);
		
		RenderTranslucent();

		RunCallback(eRendererMessage_PostTranslucent);

		if(mbOcclusionTestLargeLights)
			RetrieveAllLightOcclusionPair(false); //false = we do not stop and wait.

		//Debug for testing reflection!
		/*if(mpCurrentSettings->mbIsReflection==false)
		{
			RenderReflectionContent();
			return;
		}*/
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::SetupGBuffer()
	{
		START_RENDER_PASS(GBufferSetup);
		/////////////////////////////
		// Set G-Buffer as frame buffer (need to set something with color else textures will not be used and alpha will not work!)
		SetGBuffer(eGBufferComponents_Full);

		/////////////////////////////
		// Clear depth (no need to clear any of the textures!)
		
		mpLowLevelGraphics->SetClearDepth(1);
		ClearFrameBuffer(eClearFrameBufferFlag_Depth, true);
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::SetupRenderVariables()
	{
		//////////////////////////////
		//Setup far plane coordinates
		mfFarPlane = mpCurrentFrustum->GetFarPlane();
		mfFarTop = -tan(mpCurrentFrustum->GetFOV()*0.5f) * mfFarPlane;
		mfFarBottom = -mfFarTop;
		mfFarRight = mfFarBottom * mpCurrentFrustum->GetAspect(); 
		mfFarLeft = -mfFarRight;
	}
	
	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderZ()
	{
		START_RENDER_PASS(EarlyZ);

		SetDepthTest(true);
		SetDepthWrite(true);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_None);

		SetTextureRange(NULL,0);

		cRenderableVecIterator zIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Z);
		while(zIt.HasNext())
		{
			iRenderable *pObject = zIt.Next();
			RenderZObject(pObject, NULL);
		}

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderDynamicZTemp()
	{
		START_RENDER_PASS(EarlyZ);

		SetDepthTest(true);
		SetDepthWrite(true);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_None);

		SetTextureRange(NULL,0);

		for(int i=0; i<mpCurrentRenderList->GetSolidObjectNum(); ++i)
		{
			iRenderable *pObject = mpCurrentRenderList->GetSolidObject(i);
			if(pObject->IsStatic()) continue;

			cMaterial *pMaterial = pObject->GetMaterial();

			SetMaterialProgram(eMaterialRenderMode_Z,pMaterial);

			if(pMaterial->GetTexture(eMaterialTexture_Alpha))
			{
				SetTexture(0,pMaterial->GetTexture(eMaterialTexture_Alpha));
				SetAlphaMode(eMaterialAlphaMode_Trans);
			}
			else
			{
				SetTexture(0,NULL);
				SetAlphaMode(eMaterialAlphaMode_Solid);
			}

			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrentMaterial(eMaterialRenderMode_Z, pObject);
		}
		END_RENDER_PASS();
	}

	
	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderGbuffer()
	{
		START_RENDER_PASS(GBuffer);

		SetDepthTestFunc(eDepthTestFunc_Equal);
		SetDepthTest(true);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		
		////////////////////////////////////
		//Iterate renderable objects and render to G-Buffer
		cRenderableVecIterator diffuseIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Diffuse);
		while(diffuseIt.HasNext())
		{
			iRenderable *pObject = diffuseIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			SetMaterialProgram(eMaterialRenderMode_Diffuse,pMaterial);

			SetMaterialTextures(eMaterialRenderMode_Diffuse, pMaterial);

			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrentMaterial(eMaterialRenderMode_Diffuse, pObject);
		}
		
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderSSAO()
	{
		//If not active or loaded, return.
		if(mbSSAOLoaded==false || mpCurrentSettings->mbSSAOActive==false) return;
		
		//Check so any box lights are to be rendered, else return
		if(mSSAOType == eDeferredSSAO_InBoxLight)
		{
			if(	mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack].empty() &&
				mvSortedLights[eDeferredLightList_Box_RenderBack].empty())
			{
				return;
			}
		}

		START_RENDER_PASS(SSAO);

		//////////////////////////////
		// Set up variables
		cVector3f vQuadPos = cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane);
		cVector2f vQuadSize = cVector2f(mfFarRight*2,mfFarTop*2);

		iTexture *pGBufferDepthTexture = GetBufferTexture(2);
		
		cVector2f vGBufferDepthSize = cVector2f((float)pGBufferDepthTexture->GetWidth(), (float)pGBufferDepthTexture->GetHeight());
		cVector2f vSSAOSize = cVector2f((float)mpSSAOTexture->GetWidth(), (float)mpSSAOTexture->GetHeight());

		//////////////////////////////
		// Set up render states
		SetChannelMode(eMaterialChannelMode_RGBA);
		SetTextureRange(NULL, 1);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetBlendMode(eMaterialBlendMode_None);
		SetDepthWrite(false);
		SetDepthTest(false);
		
		SetFlatProjectionMinMax(cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane*1.5f),cVector3f(mfFarRight,mfFarTop,mfFarPlane*1.5f));
		
		
		//////////////////////////////
		// Render linear depth
		SetFrameBuffer(mpLinearDepthBuffer);

		if(mGBufferType == eDeferredGBuffer_64Bit)
		{
			if(mpUnpackDepthProgram)
				mpUnpackDepthProgram->SetFloat(kVar_afNegInvFarPlane, -1.0f / mfFarPlane);
		}
		SetProgram(mpUnpackDepthProgram);
		SetTexture(0, pGBufferDepthTexture);	//Set G-buffer depth texture
		
		DrawQuad(vQuadPos, vQuadSize,0, vGBufferDepthSize,true);


		//////////////////////////////
		// Render SSAO
		SetFrameBuffer(mpSSAOBuffer);
		SetTexture(0,mpLinearDepthTexture);
		SetTexture(1,mpSSAOScatterDisk);


		if(mpSSAORenderProgram)
		{
			mpSSAORenderProgram->SetFloat(kVar_afFarPlane, mfFarPlane);
			mpSSAORenderProgram->SetFloat(kVar_afScatterLengthMul, mfSSAOScatterLengthMul);
			mpSSAORenderProgram->SetVec2f(kVar_avScatterLengthLimits, mfSSAOScatterLengthMin,mfSSAOScatterLengthMax);
			mpSSAORenderProgram->SetVec2f(kVar_avScreenSize,vSSAOSize);
			mpSSAORenderProgram->SetFloat(kVar_afDepthDiffMul, mfSSAODepthDiffMul);
			mpSSAORenderProgram->SetFloat(kVar_afSkipEdgeLimit, mfSSAOSkipEdgeLimit);
		}

		SetProgram(mpSSAORenderProgram);

		DrawQuad(vQuadPos, vQuadSize,0, vSSAOSize,true);


		//////////////////////////////
		// Blur SSAO
		//if(false)
		{
			//Horizontal
			SetFrameBuffer(mpSSAOBlurBuffer);
			SetTexture(0,mpSSAOTexture);
			SetTexture(1,mpLinearDepthTexture);
			
			if(mpSSAOBlurProgram[0])
				mpSSAOBlurProgram[0]->SetFloat(kVar_afFarPlane, mfFarPlane);
			SetProgram(mpSSAOBlurProgram[0]);
			DrawQuad(vQuadPos, vQuadSize,0, vSSAOSize,true);
			
			//Vertical
			SetFrameBuffer(mpSSAOBuffer);
			SetTexture(0,mpSSAOBlurTexture);
			SetTexture(1,mpLinearDepthTexture);

			if(mpSSAOBlurProgram[1])
				mpSSAOBlurProgram[1]->SetFloat(kVar_afFarPlane, mfFarPlane);
			SetProgram(mpSSAOBlurProgram[1]);
			DrawQuad(vQuadPos, vQuadSize,0, vSSAOSize,true);
		}
        

		///////////////////////////////
		// Debug rendering
		if(false)
		{
			SetAccumulationBuffer();

			SetFlatProjection();
			SetProgram(NULL);

			SetTextureRange(NULL,1);

			SetTexture(0,mpSSAOTexture);
			DrawQuad(0,1, 0,vSSAOSize, true);

			SetNormalFrustumProjection();
			END_RENDER_PASS();
			return;
		}

		/////////////////////////////
		// Render SSAO to color if correct mode is set.
		if(mSSAOType == eDeferredSSAO_OnColorBuffer)
		{
			SetChannelMode(eMaterialChannelMode_RGB);
			SetGBuffer(eGBufferComponents_Color);
			
			SetFlatProjection();
			SetProgram(NULL);

			SetBlendMode(eMaterialBlendMode_Mul);

			SetTexture(0,mpSSAOTexture);
			SetTexture(1,NULL);

			DrawQuad(0,1, 0,vSSAOSize, true);
			SetChannelMode(eMaterialChannelMode_RGBA);
		}
		
		/////////////////////////////
		// Set render states back to normal
		SetNormalFrustumProjection();

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderEdgeSmooth()
	{
		if(mbEdgeSmoothLoaded==false || mpCurrentSettings->mbUseEdgeSmooth==false) return;
		//if(mbEdgeSmoothLoaded==false) return;

		START_RENDER_PASS(EdgeSmooth);

		//////////////////////////////
		// Set up variables
		cVector3f vQuadPos = cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane);
		cVector2f vQuadSize = cVector2f(mfFarRight*2,mfFarTop*2);

		iTexture *pGBufferDepthTexture = GetBufferTexture(2);
		iTexture *pGBufferNormalTexture = GetBufferTexture(1);
		
		//////////////////////////////
		// Set up render states
		SetChannelMode(eMaterialChannelMode_RGBA);
		SetTextureRange(NULL, 1);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetBlendMode(eMaterialBlendMode_None);
		SetDepthWrite(false);
		SetDepthTest(false);

		SetFlatProjectionMinMax(cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane*1.5f),cVector3f(mfFarRight,mfFarTop,mfFarPlane*1.5f));

		////////////////////////////////////////////
		// Render linear depth to texture
		SetFrameBuffer(mpEdgeSmooth_LinearDepthBuffer);

		if(mGBufferType == eDeferredGBuffer_64Bit)
		{
			mpEdgeSmooth_UnpackDepthProgram->SetFloat(kVar_afNegInvFarPlane, -1.0f / mfFarPlane);
		}
		SetProgram(mpEdgeSmooth_UnpackDepthProgram);
		SetTexture(0, pGBufferDepthTexture);	//Set G-buffer depth texture

		DrawQuad(vQuadPos, vQuadSize,0, mvScreenSizeFloat,true);

		////////////////////////////////////////////
		// Copy the screen to temp texture
		SetAccumulationBuffer();
		CopyFrameBufferToTexure(mpEdgeSmooth_TempAccum,0,mvScreenSize,0, true);

		//////////////////////////////
		// Render Edge smoothing
		SetTexture(0,mpEdgeSmooth_TempAccum);
		SetTexture(1,mpEdgeSmooth_LinearDepthTexture);
		SetTexture(2,pGBufferNormalTexture);

		SetProgram(mpEdgeSmooth_RenderProgram);
		if(mpEdgeSmooth_RenderProgram)
		{
			mpEdgeSmooth_RenderProgram->SetFloat(kVar_afFarPlane, mfFarPlane);
		}

		DrawQuad(vQuadPos, vQuadSize,0, mvScreenSizeFloat,true);

		/////////////////////////////
		// Set render states back to normal
		SetNormalFrustumProjection();
		
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------
	
	//Definitions used when rendering lights
	#define kLightRadiusMul_High		1.08f
	#define kLightRadiusMul_Medium		1.12f
	#define kLightRadiusMul_Low			1.2f
	#define kMaxStencilBitsUsed			8
	#define kStartStencilBit			0

	//-----------------------------------------------------------------------

	/**
	 * Calculates matrices for both rendering shape and the transformation
	 * \param a_mtxDestRender This has a scale based on radius and radius mul. The mul is to make sure that the shape covers the while light.
	 * \param a_mtxDestTransform A simple view space transform for the light.
	 * \param afRadiusMul 
	 */
	static inline void SetupLightMatrix(cMatrixf& a_mtxDestRender,cMatrixf& a_mtxDestTransform, iLight *apLight, cFrustum *apFrustum, float afRadiusMul)
	{
		////////////////////////////
		// Point Light
		if(apLight->GetLightType() == eLightType_Point)
		{
			a_mtxDestRender = cMath::MatrixScale(apLight->GetRadius() * afRadiusMul); //kLightRadiusMul = make sure it encapsulates the light.
			a_mtxDestTransform = cMath::MatrixMul(apFrustum->GetViewMatrix(), apLight->GetWorldMatrix());
			a_mtxDestRender = cMath::MatrixMul(a_mtxDestTransform,a_mtxDestRender);
		}
		////////////////////////////
		// Spot Light
		else if(apLight->GetLightType() == eLightType_Spot)
		{
			cLightSpot *pLightSpot = static_cast<cLightSpot*>(apLight);

			float fFarHeight = pLightSpot->GetTanHalfFOV() * pLightSpot->GetRadius() * 2.0f;
			//Note: Aspect might be wonky if there is no gobo.
			float fFarWidth = fFarHeight * pLightSpot->GetAspect(); 

			a_mtxDestRender = cMath::MatrixScale(cVector3f(fFarWidth,fFarHeight,apLight->GetRadius()) );//x and y = "far plane", z = radius 
			a_mtxDestTransform = cMath::MatrixMul(apFrustum->GetViewMatrix(), apLight->GetWorldMatrix());
			a_mtxDestRender = cMath::MatrixMul(a_mtxDestTransform,a_mtxDestRender);
		}
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::SetupLightProgramVariables(	iGpuProgram *apProgram,cDeferredLight* apLightData)
	{
		iLight *pLight = apLightData->mpLight;

		if(apProgram==NULL) return;

		///////////////////////
		// General variables
		apProgram->SetVec3f(kVar_avLightPos, apLightData->m_mtxViewSpaceRender.GetTranslation());
		apProgram->SetColor4f(kVar_avLightColor, pLight->GetDiffuseColor());
		apProgram->SetFloat(kVar_afInvLightRadius, 1.0f / pLight->GetRadius());

		////////////////////////
		// Point light specific
		if(pLight->GetLightType() == eLightType_Point)
		{
			if(pLight->GetGoboTexture())
			{
				cMatrixf mtxFinal = cMath::MatrixMul(pLight->GetWorldMatrix(),m_mtxInvView);
				apProgram->SetMatrixf(kVar_a_mtxInvViewRotation, mtxFinal.GetRotation());
			}	
		}
		////////////////////////
		// Spot light specific
		else if(pLight->GetLightType() == eLightType_Spot)
		{
			cLightSpot *pLightSpot = static_cast<cLightSpot*>(pLight);
			
			//Calculate and set the forward vector
			cVector3f vForward = cVector3f(0,0,1);
			vForward = cMath::MatrixMul3x3(apLightData->m_mtxViewSpaceTransform, vForward);
			apProgram->SetVec3f(kVar_avLightForward, vForward);

			//Get the already calculated cosine of fov
			apProgram->SetFloat(kVar_afOneMinusCosHalfSpotFOV,  1 - pLightSpot->GetCosHalfFOV());

			//If a shadow, set up shadow variables and then clear shadow map
            if(apLightData->mpShadowTexture)
			{
				cVector2f vInvShadowMapSize(1.0f / (float)apLightData->mpShadowTexture->GetWidth(),
											1.0f / (float)apLightData->mpShadowTexture->GetHeight());
				
				if(mpShadowJitterTexture)
					apProgram->SetVec2f(kVar_avShadowMapOffsetMul, vInvShadowMapSize * pLight->GetShadowMapBlurAmount());

				apLightData->mpShadowTexture = NULL;
			}
			
			if(pLight->GetGoboTexture() || apLightData->mbCastShadows)
			{
				cMatrixf mtxFinal = cMath::MatrixMul(pLightSpot->GetViewProjMatrix(), m_mtxInvView);
				apProgram->SetMatrixf(kVar_a_mtxSpotViewProj, mtxFinal);
			}
		}
	}

	//-----------------------------------------------------------------------
	
	#ifdef kDebug_RenderLightData
		
		#define RenderSingleLightDebug() \
			SetStencilActive(false); SetProgram(NULL);SetTextureRange(NULL,0); mpLowLevelGraphics->SetColor(cColor(0.25,0,0,0));\
			DrawCurrent();\
			mpLowLevelGraphics->SetColor(cColor(1,1)); for(int i=0; i<mlNumOfGBufferTextures; ++i) SetTexture(i, mpGBufferTexture[i]); SetStencilActive(true);
	#else
		#define RenderSingleLightDebug()
	#endif

	//-----------------------------------------------------------------------

	iGpuProgram* cRendererDeferred::SetupProgramAndTextures(cDeferredLight* apLightData, tFlag alExtraFlags)
	{
		iLight *pLight = apLightData->mpLight;
		eLightType lightType = pLight->GetLightType();

		/////////////////////////
		//Flag setup
		tFlag lFlags = alExtraFlags;
		if(pLight->GetDiffuseColor().a > 0)	lFlags |= eFeature_Light_Specular;
		if(pLight->GetGoboTexture())		lFlags |= eFeature_Light_Gobo;
		
		//Spotlight specifics
		if(lightType == eLightType_Spot)
		{
			lFlags |= eFeature_Light_SpotLight;
			if(lFlags & eFeature_Light_LightShapes) lFlags |= eFeature_Light_DivideInFrag;
			
			//Only spot lights can cast shadows (for now)
			if(apLightData->mbCastShadows && apLightData->mpShadowTexture)
			{
				lFlags |= eFeature_Light_ShadowMap;
			}
		}

		/////////////////////////
		//Program
		iGpuProgram *pProgram = mpProgramManager->GenerateProgram(eDefferredProgramMode_Lights, lFlags);
		SetProgram(pProgram);
		if(pProgram)
		{
			pProgram->SetFloat(kVar_afNegFarPlane, -mpCurrentFrustum->GetFarPlane());
		}
		
		/////////////////////////
		//Textures
		SetTexture(4,pLight->GetFalloffMap());
		
		if(pLight->GetGoboTexture())		SetTexture(5, pLight->GetGoboTexture());
		
		////////////////////////
		// Point light specific
		if(lightType == eLightType_Point)
		{

		}
		////////////////////////
		// Spot light specific
		else if(lightType == eLightType_Spot)
		{
			cLightSpot *pLightSpot = static_cast<cLightSpot*>(pLight);
			
			//Shadow stuff
			if(apLightData->mpShadowTexture)
			{
				SetTexture(6, apLightData->mpShadowTexture);
				
				if(mpShadowJitterTexture) SetTexture(7, mpShadowJitterTexture);
			}

			//Spot fall off
			if(pLight->GetGoboTexture()==NULL)
				SetTexture(5,pLightSpot->GetSpotFalloffMap());

		}

		return pProgram;
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderLightShadowMap(cDeferredLight* apLightData)
	{
		//Setup render states
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);

		//Render shadow map and set used shadow texture to data.
		iLight *pLight = apLightData->mpLight;
		eShadowMapResolution shadowMapRes = apLightData->mShadowResolution;

		cShadowMapData *pShadowData = GetShadowMapData(shadowMapRes, pLight);
		apLightData->mpShadowTexture = pShadowData->mpTexture;

        if(ShadowMapNeedsUpdate(pLight, pShadowData))
		{
			RenderShadowMap(pLight, pShadowData->mpBuffer);

			//Reset to previous frame buffer
			SetAccumulationBuffer();
		}

		//Set back G-buffer textures
		for(int i=0; i<mlNumOfGBufferTextures; ++i) SetTexture(i, GetBufferTexture(i));

		//Reset render states
		SetDepthTestFunc(eDepthTestFunc_GreaterOrEqual);

		SetDepthWrite(false);
		SetChannelMode(eMaterialChannelMode_RGBA);
		SetBlendMode(eMaterialBlendMode_Add);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		
	}

	//-----------------------------------------------------------------------

	
	iVertexBuffer* cRendererDeferred::GetLightShape(iLight *apLight, eDeferredShapeQuality aQuality)
	{	
		///////////////////
		//Point Light
		if(apLight->GetLightType() == eLightType_Point)
		{
			return mpShapeSphere[aQuality];
		}
		///////////////////
		// Spot Light
		else if(apLight->GetLightType() == eLightType_Spot)
		{
			return mpShapePyramid;
		}

		return NULL;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_Box(const cDeferredLight* apLightDataA, const cDeferredLight* apLightDataB)
	{
		iLight *pLightA = apLightDataA->mpLight;
		iLight *pLightB = apLightDataB->mpLight;
		
		cLightBox *pBoxLightA = static_cast<cLightBox*>(pLightA);
		cLightBox *pBoxLightB = static_cast<cLightBox*>(pLightB);

		if(pBoxLightA->GetBoxLightPrio() != pBoxLightB->GetBoxLightPrio())
			return pBoxLightA->GetBoxLightPrio() < pBoxLightB->GetBoxLightPrio();

		//////////////////////////
		//Pointer
		return pLightA < pLightB;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_Default(const cDeferredLight* apLightDataA, const cDeferredLight* apLightDataB)
	{
		iLight *pLightA = apLightDataA->mpLight;
		iLight *pLightB = apLightDataB->mpLight;

		//////////////////////////
		//Type
		if(pLightA->GetLightType() != pLightB->GetLightType())
		{
			return pLightA->GetLightType() < pLightB->GetLightType();
		}
		
		//////////////////////////
		//Specular
		int lHasSpecularA = pLightA->GetDiffuseColor().a > 0 ? 1 : 0;
		int lHasSpecularB = pLightB->GetDiffuseColor().a > 0 ? 1 : 0;
		if(lHasSpecularA != lHasSpecularB)
		{
			return lHasSpecularA < lHasSpecularB;
		}

		////////////////////////////////
		// Point inside near plane
		if(pLightA->GetLightType() == eLightType_Point)
		{
			return apLightDataA->mbInsideNearPlane < apLightDataB->mbInsideNearPlane;
		}

		//////////////////////////
		//Gobo
		if(pLightA->GetGoboTexture() != pLightB->GetGoboTexture())
		{
			return pLightA->GetGoboTexture() < pLightB->GetGoboTexture();
		}

		//////////////////////////
		//Attenuation
		if(pLightA->GetFalloffMap() != pLightB->GetFalloffMap())
		{
			return pLightA->GetFalloffMap() < pLightB->GetFalloffMap();
		}

		//////////////////////////
		//Spot falloff
		if(pLightA->GetLightType() == eLightType_Spot)
		{
			cLightSpot *pLightSpotA = static_cast<cLightSpot*>(pLightA);
			cLightSpot *pLightSpotB = static_cast<cLightSpot*>(pLightB);
			
			if(pLightSpotA->GetSpotFalloffMap() != pLightSpotB->GetSpotFalloffMap())
			{
				return pLightSpotA->GetSpotFalloffMap() < pLightSpotB->GetSpotFalloffMap();
			}
		}
		
		//////////////////////////
		//Pointer
		return pLightA < pLightB;
	}

	

	//-----------------------------------------------------------------------

	typedef bool (*tSortDeferredLightFunc)(const cDeferredLight* apLightDataA, const cDeferredLight* apLightDataB); 

	static tSortDeferredLightFunc vLightSortFunctions[eDeferredLightList_LastEnum] = {	SortFunc_Default,
																						SortFunc_Default,
																						SortFunc_Default,
																						
																						SortFunc_Default, //<- Batches, not used!
																						
																						SortFunc_Box,
																						SortFunc_Box};

	//-----------------------------------------------------------------------

	void cRendererDeferred::SetupLightsAndRenderQueries()
	{
		//////////////////////////
		// Check query results from last frame and clear list.
		tLightSet setPrevVisibleLights;
		if(mbOcclusionTestLargeLights)
		{
			for(size_t i=0; i<mpCurrentSettings->mvLightOcclusionPairs.size(); ++i)
			{
				cLightOcclusionPair &loPair = mpCurrentSettings->mvLightOcclusionPairs[i];
				
				if(loPair.mlSampleResults > mpCurrentSettings->mlSampleVisiblilityLimit)
				{
					setPrevVisibleLights.insert(loPair.mpLight);
				}
			}

			mpCurrentSettings->mvLightOcclusionPairs.resize(0);
		}
        

		//////////////////////////
		// Clear light list
		STLDeleteAll_NoClear(mvTempDeferredLights);
		mvTempDeferredLights.resize(0);

		////////////////////////////////
		// Set up variables
		float fScreenArea = (float)(mvRenderTargetSize.x*mvRenderTargetSize.y);
		
		mlMinLargeLightArea =	(int)(mfMinLargeLightNormalizedArea * fScreenArea);
		
		///////////////////////////////
		//Set up render states
		SetDepthTest(true);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_None);

		SetTextureRange(NULL,0);
		SetProgram(mpDepthOnlyProgram);

		///////////////////////////////
		//Iterate all lights in render list
		for(int i=0; i<mpCurrentRenderList->GetLightNum(); ++i)
		{
			cDeferredLight* pLightData = hplNew( cDeferredLight, () );
			iLight* pLight = mpCurrentRenderList->GetLight(i);
			eLightType lightType = pLight->GetLightType();

			pLightData->mpLight = pLight;
			pLightData->mpQuery = NULL;

			////////////////////////////////
			//Add to light list used later when rendering.
			mvTempDeferredLights.push_back(pLightData);

			////////////////////////////
			// Skip box lights
			if(lightType == eLightType_Box) continue;

			////////////////////////
			//Check if near plane is inside light volume

			//Point
			if(lightType == eLightType_Point)
			{
				pLightData->mbInsideNearPlane = mpCurrentFrustum->CheckSphereNearPlaneIntersection(	pLight->GetWorldPosition(),
																								pLight->GetRadius()*kLightRadiusMul_Low);
			}
			//Spot
			else if(lightType == eLightType_Spot)
			{
				cLightSpot *pLightSpot = static_cast<cLightSpot*>(pLight);
				pLightData->mbInsideNearPlane = mpCurrentFrustum->CheckFrustumNearPlaneIntersection(pLightSpot->GetFrustum());
			}

			////////////////////////////////////
			// Setup Light matrix
			SetupLightMatrix(	pLightData->m_mtxViewSpaceRender, pLightData->m_mtxViewSpaceTransform,
								pLight,mpCurrentFrustum,kLightRadiusMul_Medium);


			////////////////////////
			//Calculate screen clip rect and area

			//Point
			if(lightType == eLightType_Point)
			{
				pLightData->mClipRect = cMath::GetClipRectFromSphere(pLightData->m_mtxViewSpaceRender.GetTranslation(), pLight->GetRadius(), 
																	mpCurrentFrustum,mvRenderTargetSize,
																	true,mfScissorLastTanHalfFov);
			}
			//Spot
			else if(lightType == eLightType_Spot)
			{
				cMath::GetClipRectFromBV(	pLightData->mClipRect, *pLight->GetBoundingVolume(), mpCurrentFrustum,
											mvScreenSize, mfScissorLastTanHalfFov);
			}
			pLightData->mlArea = pLightData->mClipRect.w * pLightData->mClipRect.h;

			///////////////////////////
			// Setup Shadow casting variables
			if(lightType == eLightType_Spot && pLight->GetCastShadows()  && mpCurrentSettings->mbRenderShadows)
			{
				cLightSpot *pLightSpot = static_cast<cLightSpot*>(pLight);

				////////////////////////
				//Inside near plane, use max resolution
				if(pLightData->mbInsideNearPlane)
				{
					pLightData->mbCastShadows = true;
					pLightData->mShadowResolution = GetShadowMapResolution(pLight->GetShadowMapResolution(), mpCurrentSettings->mMaxShadowMapResolution);
				}
				////////////////////////
				//Outside near plane, calculate distance and chose resolution
				else
				{
					cVector3f vIntersection = pLightSpot->GetFrustum()->GetOrigin();
					pLightSpot->GetFrustum()->CheckLineIntersection(mpCurrentFrustum->GetOrigin(), pLight->GetBoundingVolume()->GetWorldCenter(),vIntersection);
					
					float fDistToLight = cMath::Vector3Dist(mpCurrentFrustum->GetOrigin(), vIntersection);
					
					pLightData->mbCastShadows = true;
					pLightData->mShadowResolution = GetShadowMapResolution(pLight->GetShadowMapResolution(), mpCurrentSettings->mMaxShadowMapResolution);
					
					///////////////////////
					//Skip shadow
					if(fDistToLight > mfShadowDistanceNone)
					{
						pLightData->mbCastShadows = false;
					}
					///////////////////////
					//Use Low
					else if(fDistToLight > mfShadowDistanceLow)
					{
						if(pLightData->mShadowResolution == eShadowMapResolution_Low)
							pLightData->mbCastShadows = false;
						pLightData->mShadowResolution = eShadowMapResolution_Low;
					}
					///////////////////////
					//Use Medium
					else if(fDistToLight > mfShadowDistanceMedium)
					{
						if(pLightData->mShadowResolution == eShadowMapResolution_High)
							pLightData->mShadowResolution = eShadowMapResolution_Medium;
						else
							pLightData->mShadowResolution = eShadowMapResolution_Low;
					}
				}
				
			}


			///////////////////////////
			// Render Query
			
			//If not doing occlusion testing on large light, might as well just skip here
			if(mbOcclusionTestLargeLights==false || mpCurrentSettings->mbUseOcclusionCulling==false) continue;

			// If inside near plane or too small on screen skip queries
			if(pLightData->mbInsideNearPlane || pLightData->mlArea < mlMinLargeLightArea) continue;
			

			///////////////////////////
			// Only check if light was invisible last frame
			if(STLObjectExists(setPrevVisibleLights,pLight)) continue;

			////////////////////////////////
			//Render light shape and make a query
			pLightData->mpQuery = GetOcclusionQuery();
			
			//Model matrix
			SetModelViewMatrix( pLightData->m_mtxViewSpaceRender );

			//Vertex buffer
			SetVertexBuffer(GetLightShape(pLight, eDeferredShapeQuality_Medium));

			//Draw and do query
			pLightData->mpQuery->Begin();
			DrawCurrent();
			pLightData->mpQuery->End();
		}

		if(mbOcclusionTestLargeLights) 
		{
			mpLowLevelGraphics->FlushRendering();
		}
	}


	//-----------------------------------------------------------------------

	void cRendererDeferred::InitLightRendering()
	{
		/////////////////////////////////
		// Get the inverse view matrix
		m_mtxInvView = cMath::MatrixInverse(mpCurrentFrustum->GetViewMatrix());


		//////////////////////////////
		//Setup quad vertex buffers
		if(mfLastFrustumFOV != mpCurrentFrustum->GetFOV() || mfLastFrustumFarPlane != mfFarPlane)
		{
			UpdateqQuadVertexPostion(	mpFullscreenLightQuad,
										cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane),cVector2f(mfFarRight*2,mfFarTop*2),
										true);
		}

		//////////////////////////////
		//Setup misc variables
		mbStencilNeedClearing = false;

		//////////////////////////////
		//Clear lists
		for(int i=0;i<eDeferredLightList_LastEnum; ++i)
		{
			mvSortedLights[i].resize(0); //No clear, keep array size data, no need to delete, same pointer in temp list
		}
		

		//////////////////////////////
		//Fill lists
		mpCurrentSettings->mlNumberOfLightsRendered =0;
		for(size_t i=0; i<mvTempDeferredLights.size(); ++i)
		{
			cDeferredLight* pLightData =  mvTempDeferredLights[i];
			iLight* pLight = pLightData->mpLight;
			eLightType lightType = pLight->GetLightType();

			////////////////////////
			//If box, we have special case...
			if(lightType == eLightType_Box)
			{
				cLightBox *pLightBox = static_cast<cLightBox*>(pLight);

				//Set up matrix
				pLightData->m_mtxViewSpaceRender = cMath::MatrixScale(pLightBox->GetSize());
				pLightData->m_mtxViewSpaceRender.SetTranslation(pLightBox->GetWorldPosition());
				pLightData->m_mtxViewSpaceRender = cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), pLightData->m_mtxViewSpaceRender);
				
				mpCurrentSettings->mlNumberOfLightsRendered++;

				//Check if near plane is inside box. If so only render back
				if( mpCurrentFrustum->CheckBVNearPlaneIntersection(pLight->GetBoundingVolume()) )
				{
					mvSortedLights[eDeferredLightList_Box_RenderBack].push_back(pLightData);
				}
				else
				{
					mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack].push_back(pLightData);
				}
				
				continue;
			}

			////////////////////////
			// Test if query has any samples. 
			//  Only check if the query is done, else skip so we do not have a stop-and-wait.
			iOcclusionQuery *pQuery = pLightData->mpQuery;
			if(pQuery)
			{
				bool bLightInvisible = false;
				if(pQuery->FetchResults())	
				{
					int lSampleCount = pQuery->GetSampleCount();
					if(lSampleCount <= mpCurrentSettings->mlSampleVisiblilityLimit)
					{
						bLightInvisible = true;
					}

					if(mbLog)
						Log(" Fetching query for light '%s'/%d. Have %d samples. Visible: %d\n", pLight->GetName().c_str(), pLight,lSampleCount, !bLightInvisible);
				}
				
				ReleaseOcclusionQuery(pQuery);
				pLightData->mpQuery = NULL;
				
				if(bLightInvisible)
				{
					continue;
				}
			}
			
			mpCurrentSettings->mlNumberOfLightsRendered++;

			////////////////////////
			//Check what list to put light in
			if(pLightData->mbInsideNearPlane)
			{
				if(lightType == eLightType_Point)
				{
					//mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad].push_back(lightData);
					mvSortedLights[eDeferredLightList_RenderBack].push_back(pLightData);
				}
				else
				{
					mvSortedLights[eDeferredLightList_RenderBack].push_back(pLightData);
				}
			}
			else
			{
				if(lightType == eLightType_Point)
				{
					if(pLightData->mlArea >= mlMinLargeLightArea)
						mvSortedLights[eDeferredLightList_StencilFront_RenderBack].push_back(pLightData);
					else
						mvSortedLights[eDeferredLightList_RenderBack].push_back(pLightData);
				}
				//Always do double passes for spotlights as they need to will get artefacts otherwise...
				//(At least with gobos)l
				else if(lightType == eLightType_Spot)
				{
					//mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad].push_back(pLightData);
					mvSortedLights[eDeferredLightList_StencilFront_RenderBack].push_back(pLightData);
				}
				
				
				
				//Skip batching for now, only speed boosts when having many small lights
				//Add later when proper test scenes exist.
				//mvSortedLights[eDeferredLightList_Batches].push_back(lightData);
			}
		}

		//Log("Near lights: %d\n",mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad].size());
		//Log("Large lights: %d\n",mvSortedLights[eDeferredLightList_StencilFront_RenderBack].size());
		//Log("Default lights: %d\n",mvSortedLights[eDeferredLightList_RenderBack].size());
		
		//////////////////////////////
		//Sort lists
		for(int i=0; i<eDeferredLightList_LastEnum; ++i)
		{
			if(mvSortedLights[i].size() > 0)
			{
				std::sort(mvSortedLights[i].begin(), mvSortedLights[i].end(), vLightSortFunctions[i]);
			}
		}
		
	}

	
	//-----------------------------------------------------------------------
	
	void cRendererDeferred::RenderLights_StencilBack_ScreenQuad()
	{
		if(mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad].empty()) return;
		if(mbLog) Log("---\nRendering Lights StencilBack_ScreenQuad Begin\n");
		
		//Check if stencil is dirty
		if(mbStencilNeedClearing)
		{
			ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			mbStencilNeedClearing = false;
		}
		
		if(mbDepthCullLights) SetStencilActive(true);
		
		///////////////////////
		// Render Inside Near Plane Lights
		int lNumOfNearPlaneLights = (int)mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad].size();
		int lStartLight = 0;
		while(lNumOfNearPlaneLights>0)
		{
			//cVector2l vMinClip(10000,10000);
			//cVector2l vMaxClip(-1000,-10000);

			int lIterations;
			if(lNumOfNearPlaneLights >kMaxStencilBitsUsed)	lIterations = kMaxStencilBitsUsed;
			else											lIterations = lNumOfNearPlaneLights;

			lNumOfNearPlaneLights -= lIterations;

			/////////////////////////////
			//Render stencil
			if(mbDepthCullLights)
				SetDepthTest(true);
			else
				SetDepthTest(false);
			SetChannelMode(eMaterialChannelMode_None);
			SetStencil(eStencilFunc_Always,0xFF,0xFF,eStencilOp_Keep,eStencilOp_Keep,eStencilOp_Replace);
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad][lStartLight + i];
				iLight *pLight = pLightData->mpLight;
				if(mbLog)Log(" Stencil light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencilWriteMask(cMath::GetFlagBit(kStartStencilBit + i));

				//Set up textures
				SetTexture(5,NULL);

				//Set up matrix
				//SetupLightMatrix(m_mtxTempLight,pLight, mpCurrentFrustum);
				SetModelViewMatrix(pLightData->m_mtxViewSpaceRender);

				//Set vertex buffer
				SetVertexBuffer(GetLightShape(pLight, eDeferredShapeQuality_Medium));

				//Set program
				SetProgram(mpLightStencilProgram);

				DrawCurrent();
			}

			/////////////////////////////
			//Render lights as quads
			SetDepthTest(false);
			SetChannelMode(eMaterialChannelMode_RGBA);
			SetFlatProjectionMinMax(cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane*1.5f),cVector3f(mfFarRight,mfFarTop,mfFarPlane*1.5f));
			SetCullMode(eCullMode_CounterClockwise);
			
			SetStencilWriteMask(0xFF);
			
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_StencilBack_ScreenQuad][lStartLight + i];
				iLight *pLight = pLightData->mpLight;

				//////////////////
				// Render shadow (if light is caster)
				if(pLightData->mbCastShadows && SetupShadowMapRendering(pLight))
				{
					if(mbDepthCullLights) 
					{
						SetStencilActive(false);
						SetDepthTest(true);
					}
					else
					{
						SetDepthTest(false);
					}

					//Render shadow map
					RenderLightShadowMap(pLightData);
					
					if(mbDepthCullLights) 
					{
						SetDepthTest(false);
						SetStencilActive(true);
					}
					else
					{
						SetDepthTest(false);
					}

					SetFlatProjectionMinMax(cVector3f(mfFarLeft,mfFarBottom,-mfFarPlane*1.5f),cVector3f(mfFarRight,mfFarTop,mfFarPlane*1.5f));
				}

				//////////////////
				// Render light
				if(mbLog)Log(" Rendering light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencil(	eStencilFunc_Equal, 0xFF, cMath::GetFlagBit(kStartStencilBit + i),
												eStencilOp_Keep, eStencilOp_Keep,eStencilOp_Keep);

				//Set vertex buffer
				SetVertexBuffer(mpFullscreenLightQuad);

				// Set up clip
				SetScissorRect(pLightData->mClipRect, true);
								
				//Debug data
				RenderSingleLightDebug();

				//Set up program and textures
				iGpuProgram *pProgram = SetupProgramAndTextures(pLightData,0);

				//Set up Light specific variables
				SetupLightProgramVariables(pProgram, pLightData);

				//Draw the light
				DrawCurrent();
				
				/////////////////////////////
				// Update min max clip rect occupied by lights 
				/*if(vMinClip.x > lightData.mClipRect.x) vMinClip.x = lightData.mClipRect.x;
				if(vMinClip.y > lightData.mClipRect.y) vMinClip.y = lightData.mClipRect.y;
				if(vMaxClip.x < lightData.mClipRect.x+lightData.mClipRect.w) vMaxClip.x = lightData.mClipRect.x+lightData.mClipRect.w;
				if(vMaxClip.y < lightData.mClipRect.y+lightData.mClipRect.h) vMaxClip.y = lightData.mClipRect.y+lightData.mClipRect.h;*/
			}
			
			/////////////////////////////
			//Go back to normal frustum projection
			SetNormalFrustumProjection();
			SetCullMode(eCullMode_Clockwise);
			SetScissorActive(false);

			/////////////////////////////
			//Prepare for next iteration (if any)
			if(lNumOfNearPlaneLights >0)
			{
				//According to test there was a slow down using this. Keeping code anyways.
				//cRect2l dirtyRect(vMinClip, vMaxClip - vMinClip);
				//SetupScissorRect(dirtyRect);
				
				lStartLight += kMaxStencilBitsUsed;
				ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			}
		}
		
		////////////////////////////////////////
		// Reset things needed
		mbStencilNeedClearing = true;
		if(mbLog) Log("Rendering Lights StencilBack_ScreenQuad End\n---\n");
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderLights_StencilFront_RenderBack()
	{
		if(mvSortedLights[eDeferredLightList_StencilFront_RenderBack].empty()) return;
		if(mbLog) Log("---\nRendering Lights StencilFront_RenderBack Begin\n");

		//Check if stencil is dirty
		if(mbStencilNeedClearing)
		{
			ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			mbStencilNeedClearing = false;
		}

		if(mbDepthCullLights) 
		{
			SetStencilActive(true);
			SetDepthTest(true);
		}
		else
		{
			SetDepthTest(false);
		}
		
		///////////////////////
		// Render Large Lights
		int lNumOfNearPlaneLights = (int)mvSortedLights[eDeferredLightList_StencilFront_RenderBack].size();
		int lStartLight = 0;
		while(lNumOfNearPlaneLights>0)
		{

			
			int lIterations;
			if(lNumOfNearPlaneLights >kMaxStencilBitsUsed)	lIterations = kMaxStencilBitsUsed;
			else											lIterations = lNumOfNearPlaneLights;

			lNumOfNearPlaneLights -= lIterations;

			/////////////////////////////
			//Render stencil using the front (back?!), set bit at zfail (since test is inverted)
			SetChannelMode(eMaterialChannelMode_None);
			SetCullMode(eCullMode_CounterClockwise);
			SetStencil(eStencilFunc_Always,0xFF,0xFF,eStencilOp_Keep,eStencilOp_Replace,eStencilOp_Keep);
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_StencilFront_RenderBack][lStartLight + i];
				iLight *pLight = pLightData->mpLight;
				if(mbLog)Log(" Stencil light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencilWriteMask(cMath::GetFlagBit(kStartStencilBit + i));

				//Set up textures
				SetTexture(5,NULL);

				//Set up matrix
				SetModelViewMatrix(pLightData->m_mtxViewSpaceRender);

				//Set vertex buffer
				//SetVertexBuffer(mpShapeSphereMedRes);
				SetVertexBuffer(GetLightShape(pLight, eDeferredShapeQuality_Medium));

				//Set program
				SetProgram(mpLightStencilProgram);

				//Draw the light
				DrawCurrent();
			}

			/////////////////////////////
			//Render back (front?!) of light geometry, checking depth and stencil
			SetChannelMode(eMaterialChannelMode_RGBA);
			SetCullMode(eCullMode_Clockwise);
			SetStencilWriteMask(0xFF);
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_StencilFront_RenderBack][lStartLight + i];
				iLight *pLight = pLightData->mpLight;
				
				//////////////////
				// Render shadow (if light is caster)
				if(pLightData->mbCastShadows && SetupShadowMapRendering(pLight))
				{
					//Setup render modes for shadow map rendering
					SetCullMode(eCullMode_CounterClockwise);
					SetStencilActive(false);

					//Render shadow map
					RenderLightShadowMap(pLightData);
					
					//Reset render settings to previous
					SetStencilActive(true);
					SetCullMode(eCullMode_Clockwise);
				}
				
				//////////////////
				// Render light
				if(mbLog)Log(" Rendering light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencil(	eStencilFunc_Equal, 0xFF, cMath::GetFlagBit(kStartStencilBit + i),
												eStencilOp_Keep, eStencilOp_Keep,eStencilOp_Keep);

				//Set up matrix
				SetModelViewMatrix(pLightData->m_mtxViewSpaceRender);

				//Set vertex buffer
				//SetVertexBuffer(mpShapeSphereHighRes);
				SetVertexBuffer(GetLightShape(pLight, eDeferredShapeQuality_High));
				
				//Set up program and textures
				iGpuProgram *pProgram = SetupProgramAndTextures(pLightData,eFeature_Light_LightShapes | eFeature_Light_DivideInFrag);

				//Set up Light specific variables
				SetupLightProgramVariables(pProgram, pLightData);

				//Draw the light
				////////////////////////
				// If lights are occlusion tested, render a query with the front and push back into Light-Occlusion pair vector
				if(mbOcclusionTestLargeLights)
				{
					cLightOcclusionPair loPair;
					loPair.mpLight = pLight;
					loPair.mpQuery = GetOcclusionQuery();

					loPair.mpQuery->Begin();
					DrawCurrent();
					loPair.mpQuery->End();

					mpCurrentSettings->mvLightOcclusionPairs.push_back(loPair);
				}
				///////////////////////////////
				//Normal rendering of front
				else
				{
					DrawCurrent();
				}
			}
			
			/////////////////////////////
			//Prepare for next iteration (if any)
			if(lNumOfNearPlaneLights >0)
			{
				lStartLight += kMaxStencilBitsUsed;
				ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			}
		}

		////////////////////////////////////////
		// Reset things needed
		mbStencilNeedClearing = true;
		if(mbLog) Log("Rendering Lights StencilFront_RenderBack End\n---\n");
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderLights_RenderBack()
	{
		if(mvSortedLights[eDeferredLightList_RenderBack].empty()) return;
		if(mbLog) Log("---\nRendering Lights RenderBack Begin\n");
		
		if(mbDepthCullLights)
		{
			SetStencilActive(false);
			SetDepthTest(true);
		}
		else
		{
			SetDepthTest(false);
		}
		
		/////////////////////////////
		//Render back of light geometry, checking depth
		SetChannelMode(eMaterialChannelMode_RGBA);
		for(size_t i=0; i<mvSortedLights[eDeferredLightList_RenderBack].size(); ++i)
		{
			cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_RenderBack][i];
			iLight *pLight = pLightData->mpLight;

			//////////////////
			// Render shadow (if light is caster)
			if(pLightData->mbCastShadows && SetupShadowMapRendering(pLight))
			{
				//Setup render modes for shadow map rendering
				SetCullMode(eCullMode_CounterClockwise);

				//Render shadow map
				RenderLightShadowMap(pLightData);

				//Reset render settings to previous
				SetCullMode(eCullMode_Clockwise);
			}

			//////////////////
			// Render light
			if(mbLog)Log(" Rendering light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

			//Set up matrix
			SetModelViewMatrix(pLightData->m_mtxViewSpaceRender);

			//Set vertex buffer
			//SetVertexBuffer(mpShapeSphereMedRes);
			SetVertexBuffer(GetLightShape(pLight, eDeferredShapeQuality_Medium));

			//Set up program and textures
			tFlag lExtraFlags = eFeature_Light_LightShapes; 
			if(pLightData->mbInsideNearPlane) lExtraFlags |= eFeature_Light_DivideInFrag;
			iGpuProgram *pProgram = SetupProgramAndTextures(pLightData,lExtraFlags);

			//Set up Light specific variables
			SetupLightProgramVariables(pProgram, pLightData);

			//Draw the light
			DrawCurrent();
		}
		if(mbLog) Log("Rendering Lights RenderBack End\n---\n");
	}

	//------------------------------------------------------------------------------

	void cRendererDeferred::RenderBoxLight(cDeferredLight* apLightData)
	{
		iLight *pLight = apLightData->mpLight;
		if(mbLog)Log(" Rendering light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

		cLightBox *pLightBox = static_cast<cLightBox*>(pLight);

		//Get correct program, depending if ssao is used or not.
		int lProgramNum =	(mbSSAOLoaded && mpCurrentSettings->mbSSAOActive && mSSAOType == eDeferredSSAO_InBoxLight) ? 1 :0;

		//Set up texture
		if(lProgramNum == 1) SetTexture(5, mpSSAOTexture);

		//Set up program
		SetProgram(mpLightBoxProgram[lProgramNum]);

		//Set up Light specific variables
		if(mpLightBoxProgram[lProgramNum])
		{
			mpLightBoxProgram[lProgramNum]->SetColor4f(kVar_avLightColor,pLight->GetDiffuseColor());
		}

		//Blend mode
		switch(pLightBox->GetBlendFunc())
		{
		case eLightBoxBlendFunc_Add:
			SetBlendMode(eMaterialBlendMode_Add); break;
		case eLightBoxBlendFunc_Replace:
			SetBlendMode(eMaterialBlendMode_None); break;
		}

		//Set up matrix
		SetModelViewMatrix(apLightData->m_mtxViewSpaceRender);

		//Draw the light
		DrawCurrent();
	}

	//------------------------------------------------------------------------------

	void cRendererDeferred::RenderLights_Box_StencilFront_RenderBack()
	{
		if(mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack].empty()) return;
		if(mbLog) Log("---\nRendering Lights Box_StencilFront_RenderBack Begin\n");

		//Check if stencil is dirty
		if(mbStencilNeedClearing)
		{
			ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			mbStencilNeedClearing = false;
		}

		if(mbDepthCullLights)
		{
			SetStencilActive(true);
			SetDepthTest(true);
		}
		else
		{
			SetDepthTest(false);
		}
		SetVertexBuffer(mpShapeBox);

		///////////////////////
		// Render Large Lights
		int lNumOfNearPlaneLights = (int)mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack].size();
		int lStartLight = 0;
		while(lNumOfNearPlaneLights>0)
		{
			int lIterations;
			if(lNumOfNearPlaneLights >kMaxStencilBitsUsed)	lIterations = kMaxStencilBitsUsed;
			else											lIterations = lNumOfNearPlaneLights;

			lNumOfNearPlaneLights -= lIterations;

			/////////////////////////////
			//Render stencil using the front, set bit at zfail (since test is inverted)
			SetChannelMode(eMaterialChannelMode_None);
			SetCullMode(eCullMode_CounterClockwise);
			SetStencil(eStencilFunc_Always,0xFF,0xFF,eStencilOp_Keep,eStencilOp_Replace,eStencilOp_Keep);
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack][lStartLight + i];
				iLight *pLight = pLightData->mpLight;
				if(mbLog)Log(" Stencil light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencilWriteMask(cMath::GetFlagBit(kStartStencilBit + i));

				//Set up matrix
				SetModelViewMatrix(pLightData->m_mtxViewSpaceRender);

				//Set program
				SetProgram(mpLightStencilProgram);

				DrawCurrent();
			}

			/////////////////////////////
			//Render back of light geometry, checking depth and stencil
			SetChannelMode(eMaterialChannelMode_RGBA);
			SetCullMode(eCullMode_Clockwise);
			SetStencilWriteMask(0xFF);
			for(int i=0; i<lIterations; ++i)
			{
				cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_Box_StencilFront_RenderBack][lStartLight + i];
				iLight *pLight = pLightData->mpLight;
				if(mbLog)Log(" Rendering light: '%s' / %d\n",pLight->GetName().c_str(), pLight);

				//Stencil
				SetStencil(	eStencilFunc_Equal, 0xFF, cMath::GetFlagBit(kStartStencilBit + i),
												eStencilOp_Keep, eStencilOp_Keep,eStencilOp_Keep);
				//SetStencil(	eStencilFunc_Always, 0xFF, cMath::GetFlagBit(kStartStencilBit + i),
				//								eStencilOp_Keep, eStencilOp_Keep,eStencilOp_Keep);
				
				RenderBoxLight(pLightData);
			}

			/////////////////////////////
			//Prepare for next iteration (if any)
			if(lNumOfNearPlaneLights >0)
			{
				lStartLight += kMaxStencilBitsUsed;
				ClearFrameBuffer(eClearFrameBufferFlag_Stencil,true);
			}
		}

		////////////////////////////////////////
		// Reset things needed
		mbStencilNeedClearing = true;
		SetBlendMode(eMaterialBlendMode_Add);
		
		if(mbLog) Log("Rendering Lights Box_StencilFront_RenderBack End\n---\n");
	}

	//------------------------------------------------------------------------------
	
	void cRendererDeferred::RenderLights_Box_RenderBack()
	{
		if(mvSortedLights[eDeferredLightList_Box_RenderBack].empty()) return;
		if(mbLog) Log("---\nRendering Lights Box_RenderBack Begin\n");

		if(mbDepthCullLights)
		{
			SetStencilActive(false);
			SetDepthTest(true);
		}
		else
		{
			SetDepthTest(false);
		}

		SetChannelMode(eMaterialChannelMode_RGBA);
		SetVertexBuffer(mpShapeBox);
		
		/////////////////////////////
		//Render back of light geometry, checking depth
		for(size_t i=0; i<mvSortedLights[eDeferredLightList_Box_RenderBack].size(); ++i)
		{
			cDeferredLight* pLightData = mvSortedLights[eDeferredLightList_Box_RenderBack][i];

			RenderBoxLight(pLightData);
		}

		//Reset what is needed.
		SetBlendMode(eMaterialBlendMode_Add);

		if(mbLog) Log("Rendering Lights Box_RenderBack End\n---\n");
	}
	
	//------------------------------------------------------------------------------

	void cRendererDeferred::RenderLights()
	{
		START_RENDER_PASS(Lights);

		/////////////////////////////////////////
		// Sort the Lights and place them in the lists
		InitLightRendering();


		/////////////////////////////////////////
		// Render SSAO (used by box lights)
		RenderSSAO();
		
		/////////////////////////////////////////
		// Set up general render states.
		SetDepthTest(true);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_Add);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		for(int i=0; i<mlNumOfGBufferTextures; ++i) SetTexture(i, GetBufferTexture(i));
		SetTextureRange(NULL, mlNumOfGBufferTextures);

		////////////////////////////////
		//Set up frame buffer
		SetAccumulationBuffer();
		ClearFrameBuffer(eClearFrameBufferFlag_Color |eClearFrameBufferFlag_Stencil,true); //Perhaps there is some ambient light that can be added instead?

		///////////////////////
		// Set up culling and depth mode
		SetCullMode(eCullMode_Clockwise);
		SetDepthTestFunc(eDepthTestFunc_GreaterOrEqual);

		///////////////////////
		// Render box lights
		RenderLights_Box_StencilFront_RenderBack();
		RenderLights_Box_RenderBack();
		
		///////////////////////
		// Render lights that are inside near plane
		RenderLights_StencilBack_ScreenQuad();

		///////////////////////
		// Render lights with large screen size or with shadows.
		RenderLights_StencilFront_RenderBack();

		///////////////////////
		// Simple rendering with no stencil
		RenderLights_RenderBack();
		
		///////////////////////
		// Batch lights and renderer several at a time.
		// Skip for now...
		//RenderLights_Batches();
				
		////////////////////////////
		//Reset settings
		SetStencilActive(false);
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
		SetCullMode(eCullMode_CounterClockwise);

		////////////////////////////
		//Debug: Draw wire frame for all lights!
		#ifdef kDebug_RenderLightData
		{
			SetDepthTest(true);
			SetDepthWrite(false);
			SetBlendMode(eMaterialBlendMode_None);
			
			SetVertexBuffer(NULL);
			SetProgram(NULL);
			SetTextureRange(NULL,0);
			SetMatrix(NULL);

			for(int i=0; i<mpRenderList->GetLightNum(); ++i)
			{
				iLight* pLight = mpRenderList->GetLight(i);

				if(pLight->GetLightType() == eLightType_Point)
				{
					mpLowLevelGraphics->DrawSphere(pLight->GetWorldPosition(),pLight->GetRadius(), cColor(1,1));
					mpLowLevelGraphics->DrawSphere(pLight->GetWorldPosition(),0.2f, cColor(0.6f,1));
				}
				else if(pLight->GetLightType() == eLightType_Spot)
				{
					cLightSpot *pLightSpot = static_cast<cLightSpot*>(pLight);
					
					pLightSpot->GetFrustum()->Draw(mpLowLevelGraphics);
				}
			}
		}
		#endif

		
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderIllumination()
	{
		if(mpCurrentRenderList->ArrayHasObjects(eRenderListType_Illumination)==false) return;

		START_RENDER_PASS(Illumination);
		
		cRenderableVecIterator illumIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Illumination);
		if(illumIt.HasNext()==false) return;

		
		SetDepthTest(true);
		SetDepthWrite(false);
		SetDepthTestFunc(eDepthTestFunc_Equal);
		SetBlendMode(eMaterialBlendMode_Add);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetTextureRange(NULL,1);
				
		while(illumIt.HasNext())
		{
			iRenderable *pObject = illumIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();
			SetMaterialProgram(eMaterialRenderMode_Illumination,pMaterial);
			
			SetTexture(0,pMaterial->GetTextureInUnit(eMaterialRenderMode_Illumination,0));
			
			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrentMaterial(eMaterialRenderMode_Illumination, pObject);
		}

		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);

		
		END_RENDER_PASS();
	}
	
	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderDecals()
	{
		if(mpCurrentRenderList->ArrayHasObjects(eRenderListType_Decal)==false) return;

		START_RENDER_PASS(Decals);

		SetGBuffer(eGBufferComponents_ColorAndDepth);

		SetDepthTest(true);
		SetDepthWrite(false);

		SetAlphaLimit(0.01f);
		SetAlphaMode(eMaterialAlphaMode_Trans);
		SetChannelMode(eMaterialChannelMode_RGBA); //RGB?
		SetDepthTestFunc(eDepthTestFunc_LessOrEqual);


		float fHalfFovTan=0;

		cRenderableVecIterator transIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Decal);
		while(transIt.HasNext())
		{
			iRenderable *pObject = transIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			SetBlendMode(pMaterial->GetBlendMode());

			SetMaterialProgram(eMaterialRenderMode_Diffuse,pMaterial);

			SetMaterialTextures(eMaterialRenderMode_Diffuse, pMaterial);

			SetMatrix(pObject->GetModelMatrixPtr());

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrent();
		}

		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetAlphaLimit(mfDefaultAlphaLimit);


		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderFullScreenFog()
	{
		if(mpCurrentWorld->GetFogActive()==false) return;

		START_RENDER_PASS(FullScreenFog);

		//////////////////////////
		// Set up rendering
		SetDepthTest(false);
		SetDepthWrite(false);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetBlendMode(eMaterialBlendMode_Alpha);

		SetTexture(0, GetGbufferTexture(2)); //depth!
		SetTextureRange(NULL, 1);

		SetMatrix(NULL);
		

		//////////////////////////
		// Set up program
		int lFlags =0;
		iGpuProgram *pProgram = mpFogProgramManager->GenerateProgram(0, lFlags);
		SetProgram(pProgram);
        
		if(pProgram)
		{
			if(GetGBufferType() == eDeferredGBuffer_32Bit)
				pProgram->SetFloat(kVar_afNegFarPlane, -mpCurrentFrustum->GetFarPlane());
			pProgram->SetVec2f(kVar_avFogStartAndLength, cVector2f(mpCurrentWorld->GetFogStart(), mpCurrentWorld->GetFogEnd() - mpCurrentWorld->GetFogStart()));
			pProgram->SetColor4f(kVar_avFogColor, mpCurrentWorld->GetFogColor());
			pProgram->SetFloat(kVar_afFalloffExp, mpCurrentWorld->GetFogFalloffExp());
		}


		//////////////////////////
		// Set up flat project
		SetFlatProjection();

		//////////////////////////
		// Render
		DrawQuad(0, 1);

		//////////////////////////
		// Reset
		SetNormalFrustumProjection();
		SetDepthTest(true);

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------


	void cRendererDeferred::RenderFog()
	{
		if(mpCurrentRenderList->GetFogAreaNum()==0)
		{
			mpCurrentSettings->mvFogRenderData.resize(0); //Make sure render data array is empty!
			return;
		}

		START_RENDER_PASS(Fog);

		SetupFogRenderDataArray(true);

		SetDepthTest(true);
		SetDepthWrite(false);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetBlendMode(eMaterialBlendMode_Alpha);
		
		SetTexture(0, GetGbufferTexture(2)); //depth!
		SetTextureRange(NULL, 1);

		
		for(size_t i=0; i<mpCurrentSettings->mvFogRenderData.size(); ++i)
		{
			cFogAreaRenderData& fogData = mpCurrentSettings->mvFogRenderData[i];
			cFogArea *pFogArea = fogData.mpFogArea;
			
			/////////////////////////////////////////////
			// Get program
			int lFlags =0;
			if(fogData.mbInsideNearFrustum)
			{
				if(pFogArea->GetShowBacksideWhenInside()) lFlags |= eFeature_FogArea_Backside;
			}
			else
			{
				if(pFogArea->GetShowBacksideWhenOutside()) lFlags |= eFeature_FogArea_Backside;
				lFlags |= eFeature_FogArea_OutsideBox;
			}
			
			iGpuProgram *pProgram = mpFogProgramManager->GenerateProgram(0, lFlags);
			if(pProgram==NULL) continue;

			/////////////////////////////////////////////
			// Setup program
			SetProgram(pProgram);
			
			if(GetGBufferType() == eDeferredGBuffer_32Bit)
					pProgram->SetFloat(kVar_afNegFarPlane, -mpCurrentFrustum->GetFarPlane());
			pProgram->SetVec2f(kVar_avFogStartAndLength, cVector2f(pFogArea->GetStart(), pFogArea->GetEnd() - pFogArea->GetStart()));
			pProgram->SetColor4f(kVar_avFogColor, pFogArea->GetColor());
			pProgram->SetFloat(kVar_afFalloffExp, pFogArea->GetFalloffExp());
		
			/////////////////////////////////////////////
			//Outside of box setup
			if(fogData.mbInsideNearFrustum==false)
			{
				cMatrixf mtxInvModelView = cMath::MatrixInverse( cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), *pFogArea->GetModelMatrixPtr()) );
				cVector3f vRayCastStart = cMath::MatrixMul(mtxInvModelView, cVector3f(0));

				
				pProgram->SetVec3f(kVar_avRayCastStart, vRayCastStart);
				pProgram->SetMatrixf(kVar_a_mtxBoxInvViewModelRotation, mtxInvModelView.GetRotation());

				cVector3f vNegPlaneDistNeg( cMath::PlaneToPointDist(cPlanef(-1,0,0,0.5f),vRayCastStart), cMath::PlaneToPointDist(cPlanef(0,-1,0,0.5f),vRayCastStart),
											cMath::PlaneToPointDist(cPlanef(0,0,-1,0.5f),vRayCastStart));
				cVector3f vNegPlaneDistPos( cMath::PlaneToPointDist(cPlanef(1,0,0,0.5f),vRayCastStart), cMath::PlaneToPointDist(cPlanef(0,1,0,0.5f),vRayCastStart),
											cMath::PlaneToPointDist(cPlanef(0,0,1,0.5f),vRayCastStart));

				pProgram->SetVec3f(kVar_avNegPlaneDistNeg, vNegPlaneDistNeg*-1);
				pProgram->SetVec3f(kVar_avNegPlaneDistPos, vNegPlaneDistPos*-1);
			}
			
			/////////////////////////////////////////////
			// Render
			SetCullMode(fogData.mbInsideNearFrustum ? eCullMode_Clockwise : eCullMode_CounterClockwise);
			SetDepthTest(!fogData.mbInsideNearFrustum);

			SetMatrix(pFogArea->GetModelMatrixPtr());

			SetVertexBuffer(mpShapeBox);

			DrawCurrent();
		}

		//////////////////////////////////
		// Reset settings
		SetCullMode(eCullMode_CounterClockwise);
		SetDepthTest(true);

		END_RENDER_PASS();
	}
	
	//-----------------------------------------------------------------------


	void cRendererDeferred::RenderTranslucent()
	{
		if(mpCurrentRenderList->ArrayHasObjects(eRenderListType_Translucent)==false) return;

		START_RENDER_PASS(Translucent);

		///////////////////////////////
		//Set up rendering
		SetDepthTest(true);
		SetDepthWrite(false);

		SetAlphaLimit(0.01f);
		SetAlphaMode(eMaterialAlphaMode_Trans);

		///////////////////////////////
		//Set up variables
		float fHalfFovTan=0;

		///////////////////////////////
		//Iterate transparent objects
		cRenderableVecIterator transIt = mpCurrentRenderList->GetArrayIterator(eRenderListType_Translucent);
		while(transIt.HasNext())
		{
			iRenderable *pObject = transIt.Next();
			cMaterial *pMaterial = pObject->GetMaterial();

			eMaterialRenderMode renderMode = mpCurrentWorld->GetFogActive() ? eMaterialRenderMode_DiffuseFog : eMaterialRenderMode_Diffuse;
			if(pMaterial->GetAffectedByFog()==false) renderMode = eMaterialRenderMode_Diffuse;

			//No world reflections in a reflection!
			if(mpCurrentSettings->mbIsReflection && pMaterial->HasWorldReflection()) continue;

			////////////////////////////////////////
			// Check the fog area alpha
			mfTempAlpha = 1;
			if(pMaterial->GetAffectedByFog())
			{
				for(size_t i=0; i<mpCurrentSettings->mvFogRenderData.size(); ++i)
				{
					mfTempAlpha *= GetFogAreaVisibilityForObject(&mpCurrentSettings->mvFogRenderData[i], pObject);
				}
			}
			
			
			////////////////////////////////////////
			// Update object, need to do this here since otherwise the reflection rendering might reset it!

			//Before viewport
			if(pObject->UpdateGraphicsForViewport(mpCurrentFrustum, mfCurrentFrameTime)==false)
			{
				continue;
			}
			
			if(pObject->RetrieveOcculsionQuery(this)==false)
			{
				continue;
			}
			
			cMatrixf *pMatrix = pObject->GetModelMatrix(mpCurrentFrustum);

			////////////////////////////////////////
			// World reflection
			if(pMaterial->HasWorldReflection() && pObject->GetRenderType() == eRenderableType_SubMesh)
			{
				if(CheckRenderablePlaneIsVisible(pObject, mpCurrentFrustum)==false) continue;

				///////////////////////////////////
				//Retrieve all occlusion queries before rendering new scene. 
				//  Otherwise it will lead to problems on some cards.
				WaitAndRetrieveAllOcclusionQueries();	//Queires for halos and such
				if(mbOcclusionTestLargeLights)
					RetrieveAllLightOcclusionPair(false);	//Queries for light visibility (false = no stop and wait!)

				///////////////////////////////////
				//Render the reflection
				cSubMeshEntity *pReflectSubMeshEnt = static_cast<cSubMeshEntity*>(pObject);
				RenderReflection(pReflectSubMeshEnt);
			}

			////////////////////////////////////////
			// Refraction set up
			if(pMaterial->HasRefraction())
			{
				if(CheckRenderablePlaneIsVisible(pObject, mpCurrentFrustum)==false) continue;
				
				////////////////////////////////////
				// Get the clip rect needed by the refraction
				cBoundingVolume *pBV = pObject->GetBoundingVolume();

				if(fHalfFovTan ==0)	
					fHalfFovTan = tan(mpCurrentFrustum->GetFOV()*0.5f);
				cRect2l clipRect = GetClipRectFromObject(pObject, 0.2f, mpCurrentFrustum, mvRenderTargetSize, fHalfFovTan);
				
				////////////////////////////////////
				// Add an extra check to make sure there is no bleeding. Draw outline of mesh to alpha!
				if(pMaterial->UseRefractionEdgeCheck())
				{
					////////////////////////////////////
					// Clear alpha
					SetFlatProjection(cVector2f((float)mvRenderTargetSize.x, (float)mvRenderTargetSize.y));					
					
					//Set up new settings
					SetDepthTest(false);
					SetProgram(NULL);
					SetBlendMode(eMaterialBlendMode_None);
					SetAlphaMode(eMaterialAlphaMode_Solid);
					SetChannelMode(eMaterialChannelMode_A);
					SetTextureRange(NULL,0);
					
					DrawQuad(	cVector2f((float)clipRect.x, (float)clipRect.y), 
								cVector2f((float)clipRect.w, (float)clipRect.h), 0, 1, false, cColor(1,0));
                    
					//Set Normal projection and depth test
					SetNormalFrustumProjection();
					SetDepthTest(true);

					////////////////////////////////////
					// Render alpha of object
					SetMatrix(pMatrix);
					SetVertexBuffer(pObject->GetVertexBuffer());

					DrawCurrent();
										
					//Set backs settings to normal.
					SetChannelMode(eMaterialChannelMode_RGBA);
					SetAlphaMode(eMaterialAlphaMode_Trans);
				}
				
				////////////////////////////////////
				// Copy frame buffer to texture
				CopyFrameBufferToTexure(mpRefractionTexture, 
										cVector2l(clipRect.x, clipRect.y), 
										cVector2l(clipRect.w, clipRect.h), 
										cVector2l(clipRect.x, clipRect.y),
										true);
				
			}
			
			////////////////////////////////////////
			// Set up and render
			if(pMaterial->HasRefraction())	SetBlendMode(eMaterialBlendMode_None); //Blending shall take place in shader!
			else							SetBlendMode(pMaterial->GetBlendMode());
			SetDepthTest(pMaterial->GetDepthTest());
			
			SetMaterialProgram(renderMode,pMaterial);
			SetMaterialTextures(renderMode, pMaterial);
			
			SetMatrix(pMatrix);

			SetVertexBuffer(pObject->GetVertexBuffer());

			DrawCurrentMaterial(renderMode, pObject);


			////////////////////////////////////////
			// Set up and render Illumination
			if(pMaterial->HasTranslucentIllumination())
			{
				renderMode = renderMode == eMaterialRenderMode_Diffuse ? eMaterialRenderMode_Illumination : eMaterialRenderMode_IlluminationFog;

				SetBlendMode(eMaterialBlendMode_Add);
				SetDepthTest(pMaterial->GetDepthTest());
				
				SetMaterialProgram(renderMode,pMaterial);
				SetMaterialTextures(renderMode, pMaterial);
				
				SetMatrix(pMatrix);

				SetVertexBuffer(pObject->GetVertexBuffer());

				DrawCurrentMaterial(renderMode, pObject);
			}
		}

		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetAlphaLimit(mfDefaultAlphaLimit);

		

		END_RENDER_PASS();
	}

    //-----------------------------------------------------------------------

	void cRendererDeferred::RenderReflection(iRenderable *apObject)
	{
		////////////////////////////////////
		//Set up variables
		cSubMeshEntity *pReflectionObject = static_cast<cSubMeshEntity*>(apObject);
		cMaterial *pRelfMaterial = pReflectionObject->GetMaterial();


		///////////////////////////
		//Check if surface is close enough for reflection!
		bool bReflectionIsInRange=true;
		if(pRelfMaterial->GetMaxReflectionDistance() > 0)
		{
			cVector3f vPoint = mpCurrentFrustum->GetOrigin() + mpCurrentFrustum->GetForward()*-1*pRelfMaterial->GetMaxReflectionDistance();
			cVector3f vNormal = mpCurrentFrustum->GetForward();

			cPlanef maxRelfctionDistPlane;
			maxRelfctionDistPlane.FromNormalPoint(vNormal, vPoint);

			if(cMath::CheckPlaneBVCollision(maxRelfctionDistPlane, *pReflectionObject->GetBoundingVolume())==eCollision_Outside)
			{
				bReflectionIsInRange = false;
			}
		}

		//////////////////////////////////////////////
		// Render the reflection!
		if(mpCurrentSettings->mbRenderWorldReflection && bReflectionIsInRange && pReflectionObject->GetIsOneSided())
		{
			RenderSubMeshEntityReflection(pReflectionObject);
		}
		//////////////////////////////////////////////
		// No reflection, just clear!
		else
		{	
			if(mbReflectionTextureCleared == false)
			{
				if(mpCurrentSettings->mbLog) Log("- Clear reflection Begin!\n");

				cRenderTarget renderTarget;
				renderTarget.mpFrameBuffer = mpReflectionBuffer;

				SetFrameBuffer(mpReflectionBuffer, false, false);

				ClearFrameBuffer(eClearFrameBufferFlag_Color, false);

				SetAccumulationBuffer();

				mbReflectionTextureCleared = true;

				if(mpCurrentSettings->mbLog) Log("- Clear reflection End!\n");
			}
		}
	}

	//-----------------------------------------------------------------------
	
	void cRendererDeferred::RenderSubMeshEntityReflection(cSubMeshEntity *pReflectionObject)
	{
		cMaterial *pRelfMaterial = pReflectionObject->GetMaterial();

		if(mbLog) Log("------------- Setting up Reflection rendering -----------\n");

		////////////////////////
		//Reset settings from normal rendering
		cFrustum *pSaved_Frustum = mpCurrentFrustum;
		cRenderSettings *pSaved_Settings = mpCurrentSettings;
		cRenderTarget *pSaved_RenderTarget = mpCurrentRenderTarget;
		bool bSaved_SendFrameBufferToPostEffects = mbSendFrameBufferToPostEffects;

		EndRendering(false);

		SetAlphaLimit(mfDefaultAlphaLimit);//Need to have the normal alpha limit!

		////////////////////////
		//Setup settings
		mpCurrentSettings->SetupReflectionSettings();

		mpCurrentSettings->mpReflectionSettings->mbUseOcclusionCulling = pRelfMaterial->GetWorldReflectionOcclusionTest();

		///////////////////////////////////
		//Reflection texture is not cleared!
		mbReflectionTextureCleared = false;

		///////////////////////////////////
		//Make render target
		cRenderTarget renderTarget;
		cVector2l vCurrentFrameBufferSize = mpCurrentRenderTarget->mpFrameBuffer ? mpCurrentRenderTarget->mpFrameBuffer->GetSize() : mvScreenSize;
		renderTarget.mpFrameBuffer = mpReflectionBuffer;
		renderTarget.mvPos = mpCurrentRenderTarget->mvPos / mlReflectionSizeDiv;
		renderTarget.mvSize.x = mpCurrentRenderTarget->mvSize.x == -1 ? vCurrentFrameBufferSize.x/mlReflectionSizeDiv : mpCurrentRenderTarget->mvSize.x/mlReflectionSizeDiv;
		renderTarget.mvSize.y = mpCurrentRenderTarget->mvSize.y == -1 ? vCurrentFrameBufferSize.y/mlReflectionSizeDiv : mpCurrentRenderTarget->mvSize.y/mlReflectionSizeDiv;

		///////////////////////////
		//Make a frustum, mirrored along the plane
		cSubMesh *pSubMesh = pReflectionObject->GetSubMesh();
		cVector3f vSurfaceNormal = cMath::Vector3Normalize(cMath::MatrixMul3x3(pReflectionObject->GetWorldMatrix(), pSubMesh->GetOneSidedNormal()));
		cVector3f vSurfacePos = cMath::MatrixMul(pReflectionObject->GetWorldMatrix(), pSubMesh->GetOneSidedPoint());

		cPlanef reflectPlane;
		reflectPlane.FromNormalPoint(vSurfaceNormal, vSurfacePos);

		cMatrixf mtxReflection = cMath::MatrixPlaneMirror(reflectPlane);
		cMatrixf mtxReflView = cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), mtxReflection);
		cVector3f vReflOrigin = cMath::MatrixMul(mtxReflection, mpCurrentFrustum->GetOrigin());


		cMatrixf mtxProj = mpCurrentFrustum->GetProjectionMatrix();

		cPlanef cameraSpaceReflPlane = cMath::TransformPlane(mtxReflView, reflectPlane);
		cMatrixf mtxReflProj = cMath::ProjectionMatrixObliqueNearClipPlane(mtxProj, cameraSpaceReflPlane);

		cFrustum reflectFrustum;
		reflectFrustum.SetupPerspectiveProj(mtxReflProj, mtxReflView, 
			mpCurrentFrustum->GetFarPlane(),mpCurrentFrustum->GetNearPlane(), 
			mpCurrentFrustum->GetFOV(), mpCurrentFrustum->GetAspect(),
			vReflOrigin,false, &mtxProj, true);
		reflectFrustum.SetInvertsCullMode(true);

		///////////////////////////
		//Reset clip planes
		mpCurrentSettings->mpReflectionSettings->ResetOcclusionPlanes();

		///////////////////////////
		//End of reflection clip plane
		if(pRelfMaterial->GetMaxReflectionDistance() > 0)
		{
			cVector3f vForward = mpCurrentFrustum->GetForward()*-1;
			float fMaxReflDist = pRelfMaterial->GetMaxReflectionDistance();
			cPlanef maxRelfctionDistPlane;

			///////////////////////////////
			//Forward and normal is aligned, the normal of plane becomes inverse forward
			float fFDotN = cMath::Vector3Dot(vForward, vSurfaceNormal);
			if(fFDotN <-0.99999f)
			{
				cVector3f vClipNormal, vClipPoint;
				vClipNormal = vForward*-1;
				vClipPoint = mpCurrentFrustum->GetOrigin() + vForward*pRelfMaterial->GetMaxReflectionDistance();

				maxRelfctionDistPlane.FromNormalPoint(vClipNormal, vClipPoint);
			}
			///////////////////////////////
			//Get the plane into camera space and then get a point where z=max reflection distance. 
			//Note: Because of test above a and b in plane cannot be 0!
			else
			{
				cPlanef cameraSpacePlane = cMath::TransformPlane(mpCurrentFrustum->GetViewMatrix(), reflectPlane);

				cVector3f vPoint1 = cVector3f(0,0, -fMaxReflDist);
				cVector3f vPoint2 = cVector3f(0,0, -fMaxReflDist);

				//Vertical row (x always same)
				if(fabs(cameraSpacePlane.b) < 0.0001f)
				{
					vPoint1.x = (-cameraSpacePlane.c*-fMaxReflDist - cameraSpacePlane.d) / cameraSpacePlane.a;
					vPoint2 = vPoint1;
					vPoint2.y+=1;
				}
				//Horizontal row (y always same)
				else if(fabs(cameraSpacePlane.a) < 0.0001f)
				{
					vPoint1.y = (-cameraSpacePlane.c*-fMaxReflDist - cameraSpacePlane.d) / cameraSpacePlane.b;
					vPoint2 = vPoint1;
					vPoint2.x+=1;
				}
				//Oblique row (x and y changes)
				else
				{
					vPoint1.x = (-cameraSpacePlane.c*-fMaxReflDist - cameraSpacePlane.d) / cameraSpacePlane.a;
					vPoint2.y = (-cameraSpacePlane.c*-fMaxReflDist - cameraSpacePlane.d) / cameraSpacePlane.b;
				}

				cMatrixf mtxInvCamera = cMath::MatrixInverse(mpCurrentFrustum->GetViewMatrix());
				vPoint1 = cMath::MatrixMul(mtxInvCamera, vPoint1);
				vPoint2 = cMath::MatrixMul(mtxInvCamera, vPoint2);

				cVector3f vNormal = cMath::Vector3Cross(vPoint1-vReflOrigin, vPoint2-vReflOrigin);
				vNormal.Normalize();
				//make sure normal has correct sign!
				if(cMath::Vector3Dot(vSurfaceNormal, vNormal)<0) vNormal = vNormal*-1;

				maxRelfctionDistPlane.FromNormalPoint(vNormal, vPoint1);
			}


			mpCurrentSettings->mpReflectionSettings->AddOcclusionPlane(maxRelfctionDistPlane);
		}

		//////////////////////////
		// Add screen rect Occlusion planes!
		if(mpCurrentSettings->mbClipReflectionScreenRect)
		{
			cVector3f vUp = reflectFrustum.GetViewMatrix().GetUp();
			cVector3f vRight = reflectFrustum.GetViewMatrix().GetRight();
			cVector3f vForward = reflectFrustum.GetViewMatrix().GetForward();
			cVector3f vOrigin = reflectFrustum.GetOrigin();

			float fNearPlane = reflectFrustum.GetNearPlane();
			float fHalfFovTan = tan(reflectFrustum.GetFOV()*0.5f);
			float fNearTop =  fHalfFovTan * fNearPlane;
			float fNearRight = reflectFrustum.GetAspect() * fNearTop;

			cVector3f vMin,vMax;
			bool bNeedsClipRect = false;
			bool bVisible = cMath::GetNormalizedClipRectFromBV(vMin, vMax, *pReflectionObject->GetBoundingVolume(), &reflectFrustum, fHalfFovTan);
			if(bVisible)
			{
				if(mbLog) Log("  Normalized Clip limits: (%s) -> (%s)\n", vMin.ToString().c_str(), vMax.ToString().c_str());
				////////////////////////////
				// Right
				if(vMax.x <1)
				{
					cVector3f vNearPlanePos = vOrigin + vRight*(vMax.x*fNearRight) + vForward*-fNearPlane;
					cPlanef rightPlane;
					rightPlane.FromPoints(vOrigin, vNearPlanePos, vNearPlanePos+vUp);
					mpCurrentSettings->mpReflectionSettings->AddOcclusionPlane(rightPlane);
					bNeedsClipRect =true;
				}

				////////////////////////////
				// Left
				if(vMin.x >-1)
				{
					cVector3f vNearPlanePos = vOrigin + vRight*(vMin.x*fNearRight) + vForward*-fNearPlane;
					cPlanef leftPlane;
					leftPlane.FromPoints(vOrigin, vNearPlanePos+vUp, vNearPlanePos);
					mpCurrentSettings->mpReflectionSettings->AddOcclusionPlane(leftPlane);
					bNeedsClipRect =true;
				}

				////////////////////////////
				// Top
				if(vMax.y <1)
				{
					cVector3f vNearPlanePos = vOrigin + vUp*(vMax.y*fNearTop) + vForward*-fNearPlane;
					cPlanef topPlane;
					topPlane.FromPoints(vOrigin, vNearPlanePos+vRight, vNearPlanePos);
					mpCurrentSettings->mpReflectionSettings->AddOcclusionPlane(topPlane);
					bNeedsClipRect = true;
				}

				////////////////////////////
				// Bottom
				if(vMin.y >-1)
				{
					cVector3f vNearPlanePos = vOrigin + vUp*(vMin.y*fNearTop) + vForward*-fNearPlane;
					cPlanef bottomPlane;
					bottomPlane.FromPoints(vOrigin, vNearPlanePos, vNearPlanePos+vRight);
					mpCurrentSettings->mpReflectionSettings->AddOcclusionPlane(bottomPlane);
					bNeedsClipRect =true;
				}

				////////////////////////////
				// Add a stencil rect!
				if(bNeedsClipRect)
				{
					cVector2l vFrameBufferSize = renderTarget.mpFrameBuffer->GetSize();
					cVector2l vRenderTargetSize;
					vRenderTargetSize.x = renderTarget.mvSize.x < 0 ? vFrameBufferSize.x : renderTarget.mvSize.x;
					vRenderTargetSize.y = renderTarget.mvSize.y < 0 ? vFrameBufferSize.y : renderTarget.mvSize.y;

					cRect2l clipRect;
					cMath::GetClipRectFromBV(clipRect, *pReflectionObject->GetBoundingVolume(), mpCurrentFrustum, vRenderTargetSize, fHalfFovTan);

					if(mbLog) Log("  Setting up scissor rect. pos: (%d, %d)  %d x %d\n", clipRect.x, clipRect.y,clipRect.w, clipRect.h);

					mpCurrentSettings->mpReflectionSettings->mbUseScissorRect = true;
					mpCurrentSettings->mpReflectionSettings->mvScissorRectPos = cVector2l(clipRect.x, clipRect.y);
					mpCurrentSettings->mpReflectionSettings->mvScissorRectSize = cVector2l(clipRect.w, clipRect.h);
				}
			}
		}

		///////////////////////////
		//Render
		if(mpCurrentSettings->mbLog)
			Log("\n==============================\n= BEGIN RENDER REFLECTION\n==============================\n\n");

		Render(mfCurrentFrameTime, &reflectFrustum, mpCurrentWorld, mpCurrentSettings->mpReflectionSettings, &renderTarget, false, mpCallbackList);				

		if(mpCurrentSettings->mbLog)
			Log("\n==============================\n= END RENDER REFLECTION\n==============================\n\n");


		///////////////////////////
		//Set back to order!
		BeginRendering(	mfCurrentFrameTime, pSaved_Frustum, mpCurrentWorld, pSaved_Settings, pSaved_RenderTarget, 
						bSaved_SendFrameBufferToPostEffects,mpCallbackList, false);


		SetAccumulationBuffer();

		//Set trans stuff
		SetDepthTest(true);
		SetDepthWrite(false);

		SetAlphaLimit(0.01f);
		SetAlphaMode(eMaterialAlphaMode_Trans);
	}
	
	//-----------------------------------------------------------------------

	void cRendererDeferred::SetAccumulationBuffer()
	{
		if(mpCurrentSettings->mbIsReflection)
			SetFrameBuffer(mpReflectionBuffer, true);
		else
			SetFrameBuffer(mpAccumBuffer, true);
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::SetGBuffer(eGBufferComponents aComponents)
	{
		int lType = mpCurrentSettings->mbIsReflection ? 1 : 0;
		SetFrameBuffer(mpGBuffer[lType][aComponents], true);
	}

	//-----------------------------------------------------------------------

	iFrameBuffer* cRendererDeferred::GetGBufferFrameBuffer(eGBufferComponents aComponents)
	{
		int lType = mpCurrentSettings->mbIsReflection ? 1 : 0;
		
		return mpGBuffer[lType][aComponents];
	}

	//-----------------------------------------------------------------------

	iTexture* cRendererDeferred::GetBufferTexture(int alIdx)
	{
		int lType = mpCurrentSettings->mbIsReflection ? 1 : 0;
		return mpGBufferTexture[lType][alIdx];
	}

	//-----------------------------------------------------------------------


	void cRendererDeferred::RenderDeferredSkyBox()
	{
		if(mpCurrentWorld==NULL || mpCurrentWorld->GetSkyBoxActive()==false) return;
		START_RENDER_PASS(DeferredSkyBox);

		SetGBuffer(eGBufferComponents_ColorAndDepth);
		

		//Debug, leave in for Luis or someone to test.
		//int lTargets1[] = {0,3};
		//mpLowLevelGraphics->SetFrameBufferDrawTargets(lTargets1,2);

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
		SetProgram(NULL);//mpSkyBoxProgram);

		/////////////////////////
		//Texture and vertex buffer
		SetTexture(0,mpCurrentWorld->GetSkyBoxTexture());
		SetTextureRange(NULL,1);

		SetVertexBuffer(mpCurrentWorld->GetSkyBoxVertexBuffer());

		DrawCurrent();

		//Debug, leave in for Luis or someone to test.
		//int lTargets2[] = {0,1,2,3};
		//mpLowLevelGraphics->SetFrameBufferDrawTargets(lTargets2,4);

		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------


	void cRendererDeferred::RenderGbufferContent()
	{
		START_RENDER_PASS(GBufferContent);

		//Pure testing below
		SetDepthTest(false);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetAccumulationBuffer();

		ClearFrameBuffer(eClearFrameBufferFlag_Depth | eClearFrameBufferFlag_Color, false);

		SetFlatProjection();
		SetProgram(NULL);

		SetTextureRange(NULL,1);

		SetTexture(0,GetBufferTexture(0));
		DrawQuad(cVector2f(0,0),cVector2f(0.5f,0.5f), 0,mvScreenSizeFloat, true);
		SetTexture(0,GetBufferTexture(1));
		DrawQuad(cVector2f(0.5f,0),cVector2f(0.5f,0.5f), 0,mvScreenSizeFloat, true);
		SetTexture(0,GetBufferTexture(2));
		DrawQuad(cVector2f(0,0.5f),cVector2f(0.5f,0.5f), 0,mvScreenSizeFloat, true);
		if(mlNumOfGBufferTextures > 3)
		{
			SetTexture(0,GetBufferTexture(3));
			DrawQuad(cVector2f(0.5f,0.5f),cVector2f(0.5f,0.5f), 0,mvScreenSizeFloat, true);
		}
		

		SetNormalFrustumProjection();
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

	void cRendererDeferred::RenderReflectionContent()
	{
		START_RENDER_PASS(ReflectionContent);

		//Pure testing below
		SetDepthTest(false);
		SetDepthWrite(false);
		SetBlendMode(eMaterialBlendMode_None);
		SetAlphaMode(eMaterialAlphaMode_Solid);
		SetChannelMode(eMaterialChannelMode_RGBA);

		SetAccumulationBuffer();

		SetFlatProjection();
		SetProgram(NULL);

		SetTextureRange(NULL,1);

		SetTexture(0,mpReflectionTexture);
		DrawQuad(cVector2f(0,0),cVector2f(1,1), 0, mvScreenSizeFloat / (float)mlReflectionSizeDiv, true);
		

		SetNormalFrustumProjection();
		END_RENDER_PASS();
	}

	//-----------------------------------------------------------------------

}
