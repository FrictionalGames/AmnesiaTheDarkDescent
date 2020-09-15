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

#include "graphics/MaterialType_BasicSolid.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"

#include "math/Frustum.h"
#include "math/Math.h"

#include "graphics/Graphics.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/ProgramComboManager.h"
#include "graphics/Renderable.h"
#include "graphics/RendererDeferred.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DEFINES
	//////////////////////////////////////////////////////////////////////////
	
	//------------------------------
	// Variables
	//------------------------------
	#define kVar_afInvFarPlane					0
	#define kVar_avHeightMapScaleAndBias		1
	#define kVar_a_mtxUV						2	
	#define kVar_afColorMul						3
	#define kVar_afDissolveAmount				4
	#define kVar_avFrenselBiasPow				5
	#define kVar_a_mtxInvViewRotation			6


	//------------------------------
	//Diffuse Features and data
	//------------------------------
	#define eFeature_Diffuse_NormalMaps		eFlagBit_0
	#define eFeature_Diffuse_Specular		eFlagBit_1
	#define eFeature_Diffuse_Parallax		eFlagBit_2
	#define eFeature_Diffuse_UvAnimation	eFlagBit_3
	#define eFeature_Diffuse_Skeleton		eFlagBit_4
	#define eFeature_Diffuse_EnvMap			eFlagBit_5
	#define eFeature_Diffuse_CubeMapAlpha	eFlagBit_6
		
	#define kDiffuseFeatureNum 7

	static cProgramComboFeature vDiffuseFeatureVec[] =
	{
		cProgramComboFeature("UseNormalMapping", kPC_VertexBit | kPC_FragmentBit),
		cProgramComboFeature("UseSpecular", kPC_FragmentBit),		
		cProgramComboFeature("UseParallax", kPC_VertexBit | kPC_FragmentBit, eFeature_Diffuse_NormalMaps),							
		cProgramComboFeature("UseUvAnimation", kPC_VertexBit),							
		cProgramComboFeature("UseSkeleton",	kPC_VertexBit),	
		cProgramComboFeature("UseEnvMap", kPC_VertexBit | kPC_FragmentBit),
		cProgramComboFeature("UseCubeMapAlpha", kPC_FragmentBit),
	};

	//------------------------------
	//Illumination Features and data
	//------------------------------
	#define eFeature_Illum_UvAnimation	eFlagBit_0
	#define eFeature_Illum_Skeleton		eFlagBit_1

	#define kIllumFeatureNum 2

	cProgramComboFeature vIllumFeatureVec[] =
	{
		cProgramComboFeature("UseUvAnimation", kPC_VertexBit),							
		cProgramComboFeature("UseSkeleton",	kPC_VertexBit),							
	};

	//------------------------------
	//Z Features and data
	//------------------------------
	#define eFeature_Z_UseAlpha					eFlagBit_0
	#define eFeature_Z_UvAnimation				eFlagBit_1
	#define eFeature_Z_Dissolve					eFlagBit_2
	#define eFeature_Z_DissolveAlpha			eFlagBit_3
	#define eFeature_Z_UseAlphaDissolveFilter	eFlagBit_4
	
	#define kZFeatureNum 5

	cProgramComboFeature vZFeatureVec[] =
	{
			cProgramComboFeature("UseAlphaMap",					kPC_FragmentBit),
			cProgramComboFeature("UseUvAnimation",				kPC_VertexBit),
			cProgramComboFeature("UseDissolve",					kPC_FragmentBit),
			cProgramComboFeature("UseDissolveAlphaMap",			kPC_FragmentBit),
			cProgramComboFeature("UseAlphaUseDissolveFilter",	kPC_FragmentBit)
	};

	//------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// STATIC OBJECTS
	//////////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------------------------

	bool iMaterialType_SolidBase::mbGlobalDataCreated = false;
	cProgramComboManager* iMaterialType_SolidBase::mpGlobalProgramManager;

	float iMaterialType_SolidBase::mfVirtualPositionAddScale = 0.03f;

	//--------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SOLID BASE
	//////////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------------------------

	iMaterialType_SolidBase::iMaterialType_SolidBase(cGraphics *apGraphics, cResources *apResources) : iMaterialType(apGraphics,apResources)
	{
		mbIsGlobalDataCreator = false;
	}

	//--------------------------------------------------------------------------

	iMaterialType_SolidBase::~iMaterialType_SolidBase()
	{

	}

	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton)
	{
		/////////////////////////////
		// Remove from global manager
		if(aRenderMode == eMaterialRenderMode_Z || aRenderMode ==  eMaterialRenderMode_Z_Dissolve)
		{
			mpGlobalProgramManager->DestroyGeneratedProgram(eMaterialRenderMode_Z, apProgram);
		}
		/////////////////////////////
		// Remove from normal manager
		else
		{
			mpProgramManager->DestroyGeneratedProgram(aRenderMode, apProgram);
		}
	}
	
	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::CreateGlobalPrograms()
	{
		if(mbGlobalDataCreated) return;

		mbGlobalDataCreated = true;
		mbIsGlobalDataCreator = true;

		/////////////////////////////
		//Load programs
		//This makes this material's program manager responsible for managing the global programs!
		cParserVarContainer defaultVars;
		defaultVars.Add("UseUv");
		
		mpProgramManager->SetupGenerateProgramData(	eMaterialRenderMode_Z,"Z","deferred_base_vtx.glsl", "deferred_base_frag.glsl", 
													vZFeatureVec,kZFeatureNum, defaultVars);
		
		mpProgramManager->AddGenerateProgramVariableId("a_mtxUV",kVar_a_mtxUV,eMaterialRenderMode_Z);
		mpProgramManager->AddGenerateProgramVariableId("afDissolveAmount",kVar_afDissolveAmount,eMaterialRenderMode_Z);

		mpGlobalProgramManager = mpProgramManager;
	}

	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::LoadData()
	{
		/////////////////////////////
		//Global data init (that is shared between Solid materials)
		CreateGlobalPrograms();

		//////////////
		// Create textures
		mpDissolveTexture = mpResources->GetTextureManager()->Create2D("core_dissolve.tga",true);


		LoadSpecificData();
	}

	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::DestroyData()
	{
		if(mpDissolveTexture) mpResources->GetTextureManager()->Destroy(mpDissolveTexture);

		//If this instace was global data creator, then it needs to be recreated.
		if(mbIsGlobalDataCreator)
		{
			mbGlobalDataCreated = false;
		}
		mpProgramManager->DestroyShadersAndPrograms();
	}

	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars)
	{

	}

	void iMaterialType_SolidBase::GetVariableValues(cMaterial *apMaterial, cResourceVarsObject *apVars)
	{

	}

	//--------------------------------------------------------------------------

	void iMaterialType_SolidBase::CompileMaterialSpecifics(cMaterial *apMaterial)
	{
		////////////////////////
		//If there is an alpha texture, set alpha mode to trans, else solid.
		if(apMaterial->GetTexture(eMaterialTexture_Alpha))
		{
			apMaterial->SetAlphaMode(eMaterialAlphaMode_Trans);
		}
		else
		{
			apMaterial->SetAlphaMode(eMaterialAlphaMode_Solid);
		}

		CompileSolidSpecifics(apMaterial);
	}

	//--------------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// SOLID DIFFUSE
	//////////////////////////////////////////////////////////////////////////
	
	//--------------------------------------------------------------------------
	
	cMaterialType_SolidDiffuse::cMaterialType_SolidDiffuse(cGraphics *apGraphics, cResources *apResources) : iMaterialType_SolidBase(apGraphics, apResources)
	{
		AddUsedTexture(eMaterialTexture_Diffuse);
		AddUsedTexture(eMaterialTexture_NMap);
		AddUsedTexture(eMaterialTexture_Alpha);
		AddUsedTexture(eMaterialTexture_Specular);
		AddUsedTexture(eMaterialTexture_Height);
		AddUsedTexture(eMaterialTexture_Illumination);
		AddUsedTexture(eMaterialTexture_DissolveAlpha);
		AddUsedTexture(eMaterialTexture_CubeMap);
		AddUsedTexture(eMaterialTexture_CubeMapAlpha);

		mbHasTypeSpecifics[eMaterialRenderMode_Diffuse] = true;

		AddVarFloat("HeightMapScale", 0.05f, "");
		AddVarFloat("HeightMapBias", 0, "");
		AddVarFloat("FrenselBias", 0.2f, "Bias for Fresnel term. values: 0-1. Higher means that more of reflection is seen when looking straight at object.");
		AddVarFloat("FrenselPow", 8.0f, "The higher the 'sharper' the reflection is, meaning that it is only clearly seen at sharp angles.");
		AddVarBool("AlphaDissolveFilter", false, "If alpha values between 0 and 1 should be used and dissolve the texture. This can be useful for things like hair.");
	}
	
	//--------------------------------------------------------------------------

	cMaterialType_SolidDiffuse::~cMaterialType_SolidDiffuse()
	{
	}

	//--------------------------------------------------------------------------


	void cMaterialType_SolidDiffuse::LoadSpecificData()
	{
		/////////////////////////////
		//Load Diffuse programs
		cParserVarContainer defaultVars;
		defaultVars.Add("UseUv");
		defaultVars.Add("UseNormals");
		defaultVars.Add("UseDepth");
		defaultVars.Add("VirtualPositionAddScale",mfVirtualPositionAddScale);
		
		//Get the G-buffer type
		if(cRendererDeferred::GetGBufferType() == eDeferredGBuffer_32Bit)	defaultVars.Add("Deferred_32bit");
		else																defaultVars.Add("Deferred_64bit");

		//Set up number of gbuffer textures used
		if(cRendererDeferred::GetNumOfGBufferTextures() == 4)	defaultVars.Add("RenderTargets_4");
		else													defaultVars.Add("RenderTargets_3");

		//Set up relief mapping method
		if(	iRenderer::GetParallaxQuality() != eParallaxQuality_Low &&
			mpGraphics->GetLowLevel()->GetCaps(eGraphicCaps_ShaderModel_3)!=0) 
		{
			defaultVars.Add("ParallaxMethod_Relief");
		}
		else														
		{
			defaultVars.Add("ParallaxMethod_Simple");
		}

		
		
		mpProgramManager->SetupGenerateProgramData(	eMaterialRenderMode_Diffuse,"Diffuse","deferred_base_vtx.glsl", "deferred_gbuffer_solid_frag.glsl", 
													vDiffuseFeatureVec,kDiffuseFeatureNum, defaultVars);

		/////////////////////////////
		//Load Illumination programs
		defaultVars.Clear();
		defaultVars.Add("UseUv");
		mpProgramManager->SetupGenerateProgramData(	eMaterialRenderMode_Illumination,"Illum","deferred_base_vtx.glsl", "deferred_illumination_frag.glsl", 
													vIllumFeatureVec,kIllumFeatureNum, defaultVars);

		
		////////////////////////////////
		//Set up variable ids
		mpProgramManager->AddGenerateProgramVariableId("afInvFarPlane",kVar_afInvFarPlane,eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avHeightMapScaleAndBias",kVar_avHeightMapScaleAndBias, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("a_mtxUV",kVar_a_mtxUV,eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avFrenselBiasPow", kVar_avFrenselBiasPow,eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("a_mtxInvViewRotation", kVar_a_mtxInvViewRotation,eMaterialRenderMode_Diffuse);

		mpProgramManager->AddGenerateProgramVariableId("a_mtxUV",kVar_a_mtxUV,eMaterialRenderMode_Illumination);
		mpProgramManager->AddGenerateProgramVariableId("afColorMul",kVar_afColorMul,eMaterialRenderMode_Illumination);
	}

	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::CompileSolidSpecifics(cMaterial *apMaterial)
	{
		cMaterialType_SolidDiffuse_Vars *pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();

		//////////////////////////////////
		//Z specifics
		apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_Z_Dissolve,true);
		apMaterial->SetUseAlphaDissolveFilter(pVars->mbAlphaDissolveFilter);
		
		//////////////////////////////////
		//Normal map and height specifics
		if(apMaterial->GetTexture(eMaterialTexture_NMap))
		{
			if(apMaterial->GetTexture(eMaterialTexture_Height))
			{
				apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Diffuse,true);
			}
		}

		//////////////////////////////////
		//Uv animation specifics
		if(apMaterial->HasUvAnimation())
		{
			apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Z,true);
			apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Diffuse,true);
			apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Illumination,true);
		}

		//////////////////////////////////
		//Cubemap
		if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
		{
			apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Diffuse,true);
		}

		//////////////////////////////////
		//Illuminations specifics
		if(apMaterial->GetTexture(eMaterialTexture_Illumination))
		{
			apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_Illumination,true);
		}
	}

	//--------------------------------------------------------------------------

	
	iTexture* cMaterialType_SolidDiffuse::GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit)
	{
		cMaterialType_SolidDiffuse_Vars *pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();

		////////////////////////////
		//Z
		if(aRenderMode == eMaterialRenderMode_Z)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Alpha);
			case 1: return mpDissolveTexture;
			}
		}
		////////////////////////////
		//Z Dissolve
		else if(aRenderMode == eMaterialRenderMode_Z_Dissolve)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Alpha);
			case 1: return mpDissolveTexture;
			case 2: return apMaterial->GetTexture(eMaterialTexture_DissolveAlpha);
			}
		}
		////////////////////////////
		//Diffuse
		else if(aRenderMode == eMaterialRenderMode_Diffuse)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Diffuse);
			case 1: return apMaterial->GetTexture(eMaterialTexture_NMap);
			case 2: return apMaterial->GetTexture(eMaterialTexture_Specular);
			case 3: return apMaterial->GetTexture(eMaterialTexture_Height);
			case 4: return apMaterial->GetTexture(eMaterialTexture_CubeMap);
			case 5: return apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha);
			}
		}
		////////////////////////////
		//Illumination
		else if(aRenderMode == eMaterialRenderMode_Illumination)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Illumination);
			}
		}

		return NULL;
	}
	//--------------------------------------------------------------------------

	iTexture* cMaterialType_SolidDiffuse::GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit)
	{
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	
	iGpuProgram* cMaterialType_SolidDiffuse::GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton)
	{
		cMaterialType_SolidDiffuse_Vars *pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();

		////////////////////////////
		//Z
		if(aRenderMode == eMaterialRenderMode_Z)
		{
			tFlag lFlags =0;
			if(apMaterial->GetTexture(eMaterialTexture_Alpha))	lFlags |= eFeature_Z_UseAlpha;
			if(apMaterial->HasUvAnimation())					lFlags |= eFeature_Z_UvAnimation;
			if(pVars->mbAlphaDissolveFilter)					lFlags |= eFeature_Z_UseAlphaDissolveFilter;

			return mpGlobalProgramManager->GenerateProgram(eMaterialRenderMode_Z, lFlags);
		}
		////////////////////////////
		//Z Dissolve
		else if(aRenderMode == eMaterialRenderMode_Z_Dissolve)
		{
			tFlag lFlags =0;
			lFlags |= eFeature_Z_Dissolve;
			if(apMaterial->GetTexture(eMaterialTexture_Alpha))			lFlags |= eFeature_Z_UseAlpha;
			if(apMaterial->GetTexture(eMaterialTexture_DissolveAlpha))	lFlags |= eFeature_Z_DissolveAlpha;
			if(apMaterial->HasUvAnimation())							lFlags |= eFeature_Z_UvAnimation;
			if(pVars->mbAlphaDissolveFilter)							lFlags |= eFeature_Z_UseAlphaDissolveFilter;

			return mpGlobalProgramManager->GenerateProgram(eMaterialRenderMode_Z, lFlags);
		}
		////////////////////////////
		//Diffuse
		else if(aRenderMode == eMaterialRenderMode_Diffuse)
		{
			tFlag lFlags =0;
			if(apMaterial->GetTexture(eMaterialTexture_NMap))			lFlags |= eFeature_Diffuse_NormalMaps;
			if(apMaterial->GetTexture(eMaterialTexture_Specular))		lFlags |= eFeature_Diffuse_Specular;
			if(	apMaterial->GetTexture(eMaterialTexture_Height) && 
				iRenderer::GetParallaxEnabled())			    		lFlags |= eFeature_Diffuse_Parallax;
			if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
			{	
				lFlags |= eFeature_Diffuse_EnvMap;
				if(apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha))	lFlags |= eFeature_Diffuse_CubeMapAlpha;
			}
			if(apMaterial->HasUvAnimation())							lFlags |= eFeature_Diffuse_UvAnimation;
			

			return mpProgramManager->GenerateProgram(aRenderMode,lFlags);
		}
		////////////////////////////
		//Illumination
		else if(aRenderMode == eMaterialRenderMode_Illumination)
		{
			tFlag lFlags =0;
			if(apMaterial->HasUvAnimation())	lFlags |= eFeature_Illum_UvAnimation;

			return mpProgramManager->GenerateProgram(aRenderMode,lFlags);
		}

		return NULL;
	}

	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderer *apRenderer)
	{
		////////////////////////////
		//Diffuse
		if(aRenderMode == eMaterialRenderMode_Diffuse)
		{
			cFrustum *pFrustum = apRenderer->GetCurrentFrustum();

			apProgram->SetFloat(kVar_afInvFarPlane, 1.0f/pFrustum->GetFarPlane());
		}
		
	}

	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::SetupMaterialSpecificData(	eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,
																iRenderer *apRenderer)
	{
		if(	aRenderMode == eMaterialRenderMode_Diffuse || 
			aRenderMode == eMaterialRenderMode_Z || 
			aRenderMode == eMaterialRenderMode_Z_Dissolve || 
			aRenderMode == eMaterialRenderMode_Illumination)
		{
			/////////////////////////
			//UV Animation
			if(apMaterial->HasUvAnimation())
			{
				apProgram->SetMatrixf(kVar_a_mtxUV, apMaterial->GetUvMatrix());
			}
			
			if(aRenderMode == eMaterialRenderMode_Diffuse)
			{
				/////////////////////////
				//Parallax
				cMaterialType_SolidDiffuse_Vars* pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();
				if(apMaterial->GetTexture(eMaterialTexture_Height) && iRenderer::GetParallaxEnabled())
				{
					apProgram->SetVec2f(kVar_avHeightMapScaleAndBias, pVars->mfHeightMapScale, pVars->mfHeightMapBias);
				}

				/////////////////////////
				//Cube Map
				if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
				{
					apProgram->SetVec2f(kVar_avFrenselBiasPow, pVars->mfFrenselBias, pVars->mfFrenselPow);
					
					cMatrixf mtxInvView = apRenderer->GetCurrentFrustum()->GetViewMatrix().GetTranspose();
					apProgram->SetMatrixf(kVar_a_mtxInvViewRotation, mtxInvView.GetRotation());
				}
			}
		}
	}
	
	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::SetupObjectSpecificData(	eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,
																iRenderer *apRenderer)
	{
		
		////////////////////////////
		//Z Dissolve
		if(aRenderMode == eMaterialRenderMode_Z_Dissolve)
		{
			bool bRet = apProgram->SetFloat(kVar_afDissolveAmount, apObject->GetCoverageAmount());
			if(bRet==false)Error("Could not set variable!\n");
		}
		////////////////////////////
		//Illumination
		else if(aRenderMode == eMaterialRenderMode_Illumination)
		{
			bool bRet = apProgram->SetFloat(kVar_afColorMul, apObject->GetIlluminationAmount());
		}
	}


	//--------------------------------------------------------------------------

	iMaterialVars* cMaterialType_SolidDiffuse::CreateSpecificVariables()
	{
		return hplNew(cMaterialType_SolidDiffuse_Vars,());
	}

	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::LoadVariables(cMaterial* apMaterial, cResourceVarsObject *apVars)
	{
		cMaterialType_SolidDiffuse_Vars *pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();
		if(pVars==NULL)
		{
			pVars = (cMaterialType_SolidDiffuse_Vars*)CreateSpecificVariables();
			apMaterial->SetVars(pVars);
		}
				
		pVars->mfHeightMapScale = apVars->GetVarFloat("HeightMapScale", 0.1f);
		pVars->mfHeightMapBias = apVars->GetVarFloat("HeightMapBias", 0);
		pVars->mfFrenselBias = apVars->GetVarFloat("FrenselBias", 0.2f);
		pVars->mfFrenselPow = apVars->GetVarFloat("FrenselPow", 8.0f);
		pVars->mbAlphaDissolveFilter = apVars->GetVarBool("AlphaDissolveFilter", false);

	}

	//--------------------------------------------------------------------------

	void cMaterialType_SolidDiffuse::GetVariableValues(cMaterial* apMaterial, cResourceVarsObject* apVars)
	{
		cMaterialType_SolidDiffuse_Vars* pVars = (cMaterialType_SolidDiffuse_Vars*)apMaterial->GetVars();

		apVars->AddVarFloat("HeightMapScale", pVars->mfHeightMapScale);
		apVars->AddVarFloat("HeightMapBias", pVars->mfHeightMapBias);
		apVars->AddVarFloat("FrenselBias", pVars->mfFrenselBias);
		apVars->AddVarFloat("FrenselPow", pVars->mfFrenselPow);
		apVars->AddVarBool("AlphaDissolveFilter", pVars->mbAlphaDissolveFilter);
	}

	//--------------------------------------------------------------------------
}
