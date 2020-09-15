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

#include "graphics/MaterialType_BasicTranslucent.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"
#include "system/String.h"

#include "resources/Resources.h"

#include "scene/World.h"
#include "scene/Light.h"

#include "math/Math.h"
#include "math/Frustum.h"

#include "graphics/Graphics.h"
#include "graphics/Material.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/ProgramComboManager.h"
#include "graphics/Renderable.h"
#include "graphics/Renderer.h"
#include "graphics/RenderList.h"

namespace hpl {

	//------------------------------
	// Variables
	//------------------------------
	#define kVar_afAlpha							0
	#define kVar_avFogStartAndLength				1
	#define kVar_afOneMinusFogAlpha					2
	#define kVar_afFalloffExp						3
	#define kVar_a_mtxUV							4
	#define kVar_afRefractionScale					5
	#define kVar_a_mtxInvViewRotation				6
	#define kVar_avFrenselBiasPow					7
	#define kVar_avRimLightMulPow					8
	#define kVar_afLightLevel						9
	
	
	//------------------------------
	//Diffuse Features and data
	//------------------------------
	#define eFeature_Diffuse_Fog					eFlagBit_0
	#define eFeature_Diffuse_UvAnimation			eFlagBit_1
	#define eFeature_Diffuse_UseRefraction			eFlagBit_2
	#define eFeature_Diffuse_NormalMap				eFlagBit_3
	#define eFeature_Diffuse_EnvMap					eFlagBit_4
	#define eFeature_Diffuse_DiffuseMap				eFlagBit_5
	#define eFeature_Diffuse_CubeMapAlpha			eFlagBit_6
	#define eFeature_Diffuse_UseScreenNormal		eFlagBit_7
	
	#define kDiffuseFeatureNum 8

	static cProgramComboFeature vDiffuseFeatureVec[] =
	{
		cProgramComboFeature("UseFog", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseUvAnimation", kPC_VertexBit),	
		cProgramComboFeature("UseRefraction", kPC_FragmentBit | kPC_VertexBit),	
		cProgramComboFeature("UseNormalMapping", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseEnvMap", kPC_FragmentBit | kPC_VertexBit),
		cProgramComboFeature("UseDiffuseMap", kPC_FragmentBit),
		cProgramComboFeature("UseCubeMapAlpha", kPC_FragmentBit),
		cProgramComboFeature("UseScreenNormal", kPC_FragmentBit),
	};

	//////////////////////////////////////////////////////////////////////////
	// TRANSLUCENT
	//////////////////////////////////////////////////////////////////////////
	
	//--------------------------------------------------------------------------
	
	cMaterialType_Translucent::cMaterialType_Translucent(cGraphics *apGraphics, cResources *apResources) : iMaterialType(apGraphics, apResources)
	{
		mbIsTranslucent = true;

		AddUsedTexture(eMaterialTexture_Diffuse);
		AddUsedTexture(eMaterialTexture_NMap);
		AddUsedTexture(eMaterialTexture_CubeMap);
		AddUsedTexture(eMaterialTexture_CubeMapAlpha);

		AddVarBool("Refraction", false, "If the material has refraction (distortion of bg). Uses NMap and/or normals of mesh");
		AddVarBool("RefractionEdgeCheck", true, "If true, there is no bleeding with foreground objects, but takes some extra power.");
		AddVarBool("RefractionNormals", false, "If normals should be used when refracting. If no NMap is set this is forced true!");
		AddVarBool("RefractionNormals", false, "If normals should be used when refracting. If no NMap is set this is forced true!");
		AddVarFloat("RefractionScale", 0.1f, "The amount refraction offsets the background");
		AddVarFloat("FrenselBias", 0.2f, "Bias for Fresnel term. values: 0-1. Higher means that more of reflection is seen when looking straight at the surface.");
		AddVarFloat("FrenselPow", 8.0f, "The higher the 'sharper' the reflection is, meaning that it is only clearly seen at sharp angles.");
		AddVarFloat("RimLightMul", 0.0f, "The amount of rim light based on the reflection. This gives an edge to the object. Values: 0 - inf (although 1.0f should be used for max)");
		AddVarFloat("RimLightPow", 8.0f, "The sharpness of the rim lighting.");
		AddVarBool("AffectedByLightLevel", false, "The the material alpha is affected by the light level.");
		
		for(int i=0; i<5; ++i)	
			mpBlendProgramManager[i] = hplNew( cProgramComboManager, ("Blend"+cString::ToString(i),mpGraphics, mpResources,eMaterialRenderMode_LastEnum) );

		mbHasTypeSpecifics[eMaterialRenderMode_Diffuse] = true;
		mbHasTypeSpecifics[eMaterialRenderMode_DiffuseFog] = true;
		mbHasTypeSpecifics[eMaterialRenderMode_Illumination] = true;
		mbHasTypeSpecifics[eMaterialRenderMode_IlluminationFog] = true;
	}

	cMaterialType_Translucent::~cMaterialType_Translucent()
	{
		for(int i=0; i<5; ++i)	
			hplDelete(mpBlendProgramManager[i]);
	}


	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton)
	{
		int lProgramNum = apMaterial->GetBlendMode()-1;

		//These render modes always use add!!
		if(aRenderMode == eMaterialRenderMode_Illumination || aRenderMode == eMaterialRenderMode_IlluminationFog)
		{
			lProgramNum = eMaterialBlendMode_Add -1;
		}

		//Log("Destroying mat '%s' program '%s' / %d manager num: %d\n", apMaterial->GetName().c_str(), apProgram->GetName().c_str(),apProgram, lProgramNum);
		mpBlendProgramManager[lProgramNum]->DestroyGeneratedProgram(eMaterialRenderMode_Diffuse, apProgram);
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::LoadData()
	{
		for(int i=0; i<5; ++i)
		{
			cParserVarContainer defaultVars;
			defaultVars.Add("UseUv");
			defaultVars.Add("UseNormals");
			defaultVars.Add("UseColor");
			
			if(i==0) defaultVars.Add("BlendMode_Add");
			if(i==1) defaultVars.Add("BlendMode_Mul");
			if(i==2) defaultVars.Add("BlendMode_MulX2");
			if(i==3) defaultVars.Add("BlendMode_Alpha");
			if(i==4) defaultVars.Add("BlendMode_PremulAlpha");

			mpBlendProgramManager[i]->SetupGenerateProgramData(	eMaterialRenderMode_Diffuse,"Diffuse","deferred_base_vtx.glsl", "deferred_transparent_frag.glsl", 
														vDiffuseFeatureVec,kDiffuseFeatureNum, defaultVars);

			////////////////////////////////
			//Set up variable ids
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("afAlpha",kVar_afAlpha, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("avFogStartAndLength",kVar_avFogStartAndLength, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("afOneMinusFogAlpha",kVar_afOneMinusFogAlpha, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("afFalloffExp",kVar_afFalloffExp, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("a_mtxUV",kVar_a_mtxUV, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("afRefractionScale", kVar_afRefractionScale, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("a_mtxInvViewRotation", kVar_a_mtxInvViewRotation, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("avFrenselBiasPow", kVar_avFrenselBiasPow, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("avRimLightMulPow", kVar_avRimLightMulPow, eMaterialRenderMode_Diffuse);
			mpBlendProgramManager[i]->AddGenerateProgramVariableId("afLightLevel", kVar_afLightLevel, eMaterialRenderMode_Diffuse);
		
		}
	}
	void cMaterialType_Translucent::DestroyData()
	{
		for(int i=0; i<5; ++i) mpBlendProgramManager[i]->DestroyShadersAndPrograms();
	}

	//--------------------------------------------------------------------------

	iTexture* cMaterialType_Translucent::GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit)
	{
		cMaterialType_Translucent_Vars *pVars = (cMaterialType_Translucent_Vars*)apMaterial->GetVars();

		bool bRefractionEnabled = pVars->mbRefraction && iRenderer::GetRefractionEnabled();

		////////////////////////////
		//Diffuse
		if(aRenderMode == eMaterialRenderMode_Diffuse || aRenderMode == eMaterialRenderMode_DiffuseFog)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Diffuse);
			case 1: return apMaterial->GetTexture(eMaterialTexture_NMap);
			case 2: if(bRefractionEnabled)
						return mpGraphics->GetRenderer(eRenderer_Main)->GetRefractionTexture();
					else
						return NULL;
			case 3: return apMaterial->GetTexture(eMaterialTexture_CubeMap);
			case 4: return apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha);
			}
		}
		////////////////////////////
		//Illumination
		else if(aRenderMode == eMaterialRenderMode_Illumination || aRenderMode == eMaterialRenderMode_IlluminationFog)
		{
			switch(alUnit)
			{
			case 1: return apMaterial->GetTexture(eMaterialTexture_NMap);
			case 3: return apMaterial->GetTexture(eMaterialTexture_CubeMap);
			case 4: return apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha);
			}
		}

		return NULL;
	}

	//--------------------------------------------------------------------------

	iTexture* cMaterialType_Translucent::GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit)
	{
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	
	iGpuProgram* cMaterialType_Translucent::GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton)
	{
		cMaterialType_Translucent_Vars *pVars = (cMaterialType_Translucent_Vars*)apMaterial->GetVars();

		bool bRefractionEnabled = pVars->mbRefraction && iRenderer::GetRefractionEnabled();

		////////////////////////////
		//Diffuse
		if(aRenderMode == eMaterialRenderMode_Diffuse || aRenderMode == eMaterialRenderMode_DiffuseFog)
		{
			int lProgramNum = apMaterial->GetBlendMode()-1;
			
			tFlag lFlags =0;
			if(apMaterial->GetTexture(eMaterialTexture_Diffuse))	lFlags |= eFeature_Diffuse_DiffuseMap;
			if(aRenderMode == eMaterialRenderMode_DiffuseFog)		lFlags |= eFeature_Diffuse_Fog;
			if(apMaterial->HasUvAnimation())						lFlags |= eFeature_Diffuse_UvAnimation;
			if(apMaterial->GetTexture(eMaterialTexture_NMap))		lFlags |= eFeature_Diffuse_NormalMap;
			if(bRefractionEnabled && apMaterial->GetTexture(eMaterialTexture_CubeMap))
			{
				lFlags |= eFeature_Diffuse_EnvMap;
				if(apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha))	lFlags |= eFeature_Diffuse_CubeMapAlpha;
			}
			if(bRefractionEnabled)									lFlags |= eFeature_Diffuse_UseRefraction;
			if(pVars->mbRefractionNormals && bRefractionEnabled)	lFlags |= eFeature_Diffuse_UseScreenNormal;
			
			return mpBlendProgramManager[lProgramNum]->GenerateProgram(eMaterialRenderMode_Diffuse, lFlags);
		}
		////////////////////////////
		//Illumination
		if(aRenderMode == eMaterialRenderMode_Illumination || aRenderMode == eMaterialRenderMode_IlluminationFog)
		{
			if(bRefractionEnabled==false && apMaterial->GetTexture(eMaterialTexture_CubeMap))
			{
				int lProgramNum = eMaterialBlendMode_Add - 1;
				
				tFlag lFlags =0;
				if(aRenderMode == eMaterialRenderMode_IlluminationFog)	lFlags |= eFeature_Diffuse_Fog;
				if(apMaterial->GetTexture(eMaterialTexture_NMap))		lFlags |= eFeature_Diffuse_NormalMap;
				if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
				{
					lFlags |= eFeature_Diffuse_EnvMap;
					if(apMaterial->GetTexture(eMaterialTexture_CubeMapAlpha))	lFlags |= eFeature_Diffuse_CubeMapAlpha;
				}
				
				return mpBlendProgramManager[lProgramNum]->GenerateProgram(eMaterialRenderMode_Diffuse, lFlags);
			}
		}

		return NULL;
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram,iRenderer *apRenderer)
	{
	}
	
	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::SetupMaterialSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,iRenderer *apRenderer)
	{
		cMaterialType_Translucent_Vars *pVars = (cMaterialType_Translucent_Vars*)apMaterial->GetVars();

		bool bIlluminationPass = (aRenderMode == eMaterialRenderMode_IlluminationFog || aRenderMode == eMaterialRenderMode_Illumination);
		bool bRefractionEnabled = pVars->mbRefraction && iRenderer::GetRefractionEnabled();

		/////////////////////////
		//UV Animation
		if(apMaterial->HasUvAnimation())
		{
			apProgram->SetMatrixf(kVar_a_mtxUV, apMaterial->GetUvMatrix());
		}

		////////////////////////////
		//Reflection vars
		if(apMaterial->GetTexture(eMaterialTexture_CubeMap) && 
			(bRefractionEnabled && bIlluminationPass==false) || (bRefractionEnabled==false && bIlluminationPass) )
		{
			cMatrixf mtxInvView = apRenderer->GetCurrentFrustum()->GetViewMatrix().GetTranspose();
			apProgram->SetMatrixf(kVar_a_mtxInvViewRotation, mtxInvView.GetRotation());

			apProgram->SetVec2f(kVar_avFrenselBiasPow, cVector2f(pVars->mfFrenselBias, pVars->mfFrenselPow));
			apProgram->SetVec2f(kVar_avRimLightMulPow, cVector2f(pVars->mfRimLightMul, pVars->mfRimLightPow)); 
		}

		////////////////////////////
		//Refraction vars
		if(bRefractionEnabled && (aRenderMode == eMaterialRenderMode_DiffuseFog || aRenderMode == eMaterialRenderMode_Diffuse) )
		{
			apProgram->SetFloat(kVar_afRefractionScale, pVars->mfRefractionScale * (float)apRenderer->GetRenderTargetSize().x);
		}

		////////////////////////////
		//Fog
		if(aRenderMode == eMaterialRenderMode_DiffuseFog || aRenderMode == eMaterialRenderMode_IlluminationFog)
		{
			cWorld *pWorld = apRenderer->GetCurrentWorld();

			apProgram->SetVec2f(kVar_avFogStartAndLength, cVector2f(pWorld->GetFogStart(), pWorld->GetFogEnd() - pWorld->GetFogStart()));
			apProgram->SetFloat(kVar_afOneMinusFogAlpha, 1 - pWorld->GetFogColor().a);
			apProgram->SetFloat(kVar_afFalloffExp, pWorld->GetFogFalloffExp());
		}
	}
	
	//--------------------------------------------------------------------------

	static inline float GetMaxColorValue(const cColor& aCol)
	{
		return cMath::Max(cMath::Max(aCol.r, aCol.g),aCol.b);
	}

	void cMaterialType_Translucent::SetupObjectSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,iRenderer *apRenderer)
	{
		cMaterialType_Translucent_Vars *pVars = (cMaterialType_Translucent_Vars*)apObject->GetMaterial()->GetVars();

		////////////////////////////
		//Light affects Alpha
		if(pVars->mbAffectedByLightLevel)
		{
			cVector3f vCenterPos = apObject->GetBoundingVolume()->GetWorldCenter();
            cRenderList *pRenderList = apRenderer->GetCurrentRenderList();

			float fLightAmount = 0.0f;

			////////////////////////////////////////
			//Iterate lights and add light amount
			for(int i=0; i<pRenderList->GetLightNum(); ++i)
			{
				iLight* pLight = pRenderList->GetLight(i);
				
				//Check if there is an intersection
				if(pLight->CheckObjectIntersection(apObject))
				{
					if(pLight->GetLightType() == eLightType_Box)
					{
						fLightAmount += GetMaxColorValue(pLight->GetDiffuseColor());
					}
					else
					{
						float fDist = cMath::Vector3Dist(pLight->GetWorldPosition(), vCenterPos);

						fLightAmount += GetMaxColorValue(pLight->GetDiffuseColor()) * cMath::Max(1.0f - (fDist / pLight->GetRadius()), 0.0f);
					}
					
					if(fLightAmount >= 1.0f)
					{
                        fLightAmount = 1.0f;
						break;
					}
				}
			}

			////////////////////////////////////////
			//Set up variable
			apProgram->SetFloat(kVar_afAlpha, apRenderer->GetTempAlpha());
			apProgram->SetFloat(kVar_afLightLevel, fLightAmount);
		}
		////////////////////////////
		//Normal Alpha
		else
		{
			apProgram->SetFloat(kVar_afAlpha, apRenderer->GetTempAlpha());
			apProgram->SetFloat(kVar_afLightLevel, 1.0f);
		}
	}

	//--------------------------------------------------------------------------

	iMaterialVars* cMaterialType_Translucent::CreateSpecificVariables()
	{
		cMaterialType_Translucent_Vars* pVars = hplNew(cMaterialType_Translucent_Vars,());

		return pVars;
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars)
	{
		cMaterialType_Translucent_Vars *pVars = (cMaterialType_Translucent_Vars*)apMaterial->GetVars();
		if(pVars==NULL)
		{
			pVars = (cMaterialType_Translucent_Vars*)CreateSpecificVariables();
			apMaterial->SetVars(pVars);
		}

		pVars->mbRefraction = apVars->GetVarBool("Refraction", false);
		pVars->mbRefractionEdgeCheck = apVars->GetVarBool("RefractionEdgeCheck", true);
		pVars->mbRefractionNormals = apVars->GetVarBool("RefractionNormals", true);
		pVars->mfRefractionScale  = apVars->GetVarFloat("RefractionScale", 1.0f);
		pVars->mfFrenselBias = apVars->GetVarFloat("FrenselBias", 0.2f);
		pVars->mfFrenselPow = apVars->GetVarFloat("FrenselPow", 8.0);
		pVars->mfRimLightMul =  apVars->GetVarFloat("RimLightMul", 0.0f);
		pVars->mfRimLightPow = apVars->GetVarFloat("RimLightPow", 8.0f);
		pVars->mbAffectedByLightLevel = apVars->GetVarBool("AffectedByLightLevel", false);
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::GetVariableValues(cMaterial *apMaterial, cResourceVarsObject *apVars)
	{
		cMaterialType_Translucent_Vars* pVars = (cMaterialType_Translucent_Vars*)apMaterial->GetVars();

		apVars->AddVarBool("Refraction", pVars->mbRefraction);
		apVars->AddVarBool("RefractionEdgeCheck", pVars->mbRefractionEdgeCheck);
		apVars->AddVarBool("RefractionNormals", pVars->mbRefractionNormals);
		apVars->AddVarFloat("RefractionScale", pVars->mfRefractionScale);
		apVars->AddVarFloat("FrenselBias", pVars->mfFrenselBias);
		apVars->AddVarFloat("FrenselPow",pVars->mfFrenselPow);
		apVars->AddVarFloat("RimLightMul",pVars->mfRimLightMul);
		apVars->AddVarFloat("RimLightPow",pVars->mfRimLightPow);
		apVars->AddVarBool("AffectedByLightLevel", pVars->mbAffectedByLightLevel);
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Translucent::CompileMaterialSpecifics(cMaterial *apMaterial)
	{
		cMaterialType_Translucent_Vars *pVars = static_cast<cMaterialType_Translucent_Vars*>(apMaterial->GetVars());

		/////////////////////////////////////
		//Set up specifics
		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Diffuse,true);
		apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_Diffuse,true);

		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_DiffuseFog,true);
		apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_DiffuseFog,true);

		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Illumination,true);
		apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_Illumination,true);

		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_IlluminationFog,true);
		apMaterial->SetHasObjectSpecificsSettings(eMaterialRenderMode_IlluminationFog,true);

		/////////////////////////////////////
		//Set up the refraction
		bool bRefractionEnabled = pVars->mbRefraction && iRenderer::GetRefractionEnabled();

		if(bRefractionEnabled)
		{
			apMaterial->SetHasRefraction(true);
			apMaterial->SetUseRefractionEdgeCheck(pVars->mbRefractionEdgeCheck);
			//Note: No need to set blend mode to None since rendered sets that when refraction is true!
			//		Also, this gives problems when recompiling, since the material data would not be accurate!
		}

		/////////////////////////////////////
		//Set up the reflections
		if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
		{
			if(bRefractionEnabled==false)
				apMaterial->SetHasTranslucentIllumination(true);
		}
	}
	
	//--------------------------------------------------------------------------


}
