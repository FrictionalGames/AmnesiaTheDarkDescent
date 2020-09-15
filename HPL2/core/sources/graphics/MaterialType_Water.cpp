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

#include "graphics/MaterialType_Water.h"

#include "system/LowLevelSystem.h"
#include "system/PreprocessParser.h"

#include "resources/Resources.h"

#include "math/Math.h"
#include "math/Frustum.h"

#include "scene/World.h"

#include "graphics/Graphics.h"
#include "graphics/Material.h"
#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/ProgramComboManager.h"
#include "graphics/Renderable.h"
#include "graphics/Renderer.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DEFINES
	//////////////////////////////////////////////////////////////////////////
	
	//------------------------------
	// Variables
	//------------------------------
	#define kVar_afT								0
	#define kVar_afRefractionScale					1
	#define kVar_a_mtxInvViewRotation				2
	#define kVar_avReflectionMapSizeMul				3
	#define kVar_avFrenselBiasPow					4
	#define kVar_avReflectionFadeStartAndLength		5
	#define kVar_afWaveAmplitude					6
	#define kVar_afWaveFreq							7
	#define kVar_avFogStartAndLength				8
	#define kVar_avFogColor							9
	#define kVar_afFalloffExp						10

	//------------------------------
	//Diffuse Features and data
	//------------------------------
	#define eFeature_Diffuse_Reflection				eFlagBit_0
	#define eFeature_Diffuse_CubeMapReflection		eFlagBit_1
	#define eFeature_Diffuse_ReflectionFading		eFlagBit_2
	#define eFeature_Diffuse_Fog					eFlagBit_3
	
	#define kDiffuseFeatureNum 4

	static cProgramComboFeature vDiffuseFeatureVec[] =
	{
		cProgramComboFeature("UseReflection", kPC_FragmentBit),
		cProgramComboFeature("UseCubeMapReflection", kPC_FragmentBit, eFeature_Diffuse_Reflection),
		cProgramComboFeature("UseReflectionFading", kPC_FragmentBit, eFeature_Diffuse_Reflection),
		cProgramComboFeature("UseFog", kPC_FragmentBit | kPC_VertexBit),
	};

	//////////////////////////////////////////////////////////////////////////
	// TRANSLUCENT
	//////////////////////////////////////////////////////////////////////////
	
	//--------------------------------------------------------------------------
	
	cMaterialType_Water::cMaterialType_Water(cGraphics *apGraphics, cResources *apResources) : iMaterialType(apGraphics, apResources)
	{
		mbIsTranslucent = true;

		AddUsedTexture(eMaterialTexture_Diffuse);
		AddUsedTexture(eMaterialTexture_NMap);
		AddUsedTexture(eMaterialTexture_CubeMap);

		
		AddVarFloat("RefractionScale", 0.1f, "The amount reflection and refraction is offset by ripples in water.");
		AddVarFloat("FrenselBias", 0.2f, "Bias for Fresnel term. values: 0-1. Higher means that more of reflection is seen when looking straight at water.");
		AddVarFloat("FrenselPow", 8.0f, "The higher the 'sharper' the reflection is, meaning that it is only clearly seen at sharp angles.");
		AddVarFloat("WaveSpeed", 1.0f, "The speed of the waves.");
		AddVarFloat("WaveAmplitude", 1.0f, "The size of the waves.");
		AddVarFloat("WaveFreq", 1.0f, "The frequency of the waves.");
		AddVarFloat("ReflectionFadeStart",0,"Where the reflection starts fading.");
		AddVarFloat("ReflectionFadeEnd",0,"Where the reflection stops fading. 0 or less means no fading.");

		AddVarBool("HasReflection", true, "If a reflection should be shown or not.");
		AddVarBool("OcclusionCullWorldReflection", true, "If occlusion culling should be used on reflection.");
		AddVarBool("LargeSurface", true, "If the water will cover a large surface and will need special sorting when rendering other transperant objects.");
		
		mbHasTypeSpecifics[eMaterialRenderMode_Diffuse] = true;
		mbHasTypeSpecifics[eMaterialRenderMode_DiffuseFog] = true;
	}

	cMaterialType_Water::~cMaterialType_Water()
	{
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton)
	{
		mpProgramManager->DestroyGeneratedProgram(eMaterialRenderMode_Diffuse, apProgram);
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::LoadData()
	{
		/////////////////////////////
		// Load Diffuse programs
		cParserVarContainer defaultVars;
		defaultVars.Add("UseUv");
		defaultVars.Add("UseNormals");
		defaultVars.Add("UseVertexPosition");
		defaultVars.Add("UseRefractionEdgeCheck");
		defaultVars.Add("UseNormals");
		defaultVars.Add("UseNormalMapping");
		if(iRenderer::GetRefractionEnabled())	defaultVars.Add("UseRefraction");
        				
		mpProgramManager->SetupGenerateProgramData(	eMaterialRenderMode_Diffuse,"Diffuse","deferred_base_vtx.glsl", "water_surface_frag.glsl", 
											vDiffuseFeatureVec,kDiffuseFeatureNum, defaultVars);
		

		////////////////////////////////
		//Set up variable ids
		mpProgramManager->AddGenerateProgramVariableId("afT",kVar_afT,eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("afRefractionScale",kVar_afRefractionScale,eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("a_mtxInvViewRotation",kVar_a_mtxInvViewRotation, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avReflectionMapSizeMul", kVar_avReflectionMapSizeMul, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avFrenselBiasPow", kVar_avFrenselBiasPow, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avReflectionFadeStartAndLength", kVar_avReflectionFadeStartAndLength, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("afWaveAmplitude", kVar_afWaveAmplitude, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("afWaveFreq", kVar_afWaveFreq, eMaterialRenderMode_Diffuse);

		mpProgramManager->AddGenerateProgramVariableId("avFogStartAndLength",kVar_avFogStartAndLength, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("avFogColor",kVar_avFogColor, eMaterialRenderMode_Diffuse);
		mpProgramManager->AddGenerateProgramVariableId("afFalloffExp",kVar_afFalloffExp	, eMaterialRenderMode_Diffuse);

	}
	void cMaterialType_Water::DestroyData()
	{
		mpProgramManager->DestroyShadersAndPrograms();
	}

	//--------------------------------------------------------------------------

	iTexture* cMaterialType_Water::GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit)
	{
		////////////////////////////
		//Z
		if(aRenderMode == eMaterialRenderMode_Z)
		{
			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Diffuse);
			}
		}
		////////////////////////////
		//Diffuse
		else if(aRenderMode == eMaterialRenderMode_Diffuse || aRenderMode == eMaterialRenderMode_DiffuseFog)
		{
			cMaterialType_Water_Vars *pVars = static_cast<cMaterialType_Water_Vars*>(apMaterial->GetVars());

			switch(alUnit)
			{
			case 0: return apMaterial->GetTexture(eMaterialTexture_Diffuse);
			case 1: return apMaterial->GetTexture(eMaterialTexture_NMap);
			case 2: 
					if(iRenderer::GetRefractionEnabled())
						return mpGraphics->GetRenderer(eRenderer_Main)->GetRefractionTexture();
					else
						return NULL;
			case 3: 
					if(iRenderer::GetRefractionEnabled())
					{
						if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
							return apMaterial->GetTexture(eMaterialTexture_CubeMap);
						else
							return mpGraphics->GetRenderer(eRenderer_Main)->GetReflectionTexture();
					}
					else
					{
						return NULL;
					}
			}
		}

		return NULL;
	}

	//--------------------------------------------------------------------------

	iTexture* cMaterialType_Water::GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit)
	{
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	
	iGpuProgram* cMaterialType_Water::GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton)
	{
		////////////////////////////
		//Z
		if(aRenderMode == eMaterialRenderMode_Z)
		{
			return NULL;
		}
		////////////////////////////
		//Diffuse
		else if(aRenderMode == eMaterialRenderMode_Diffuse || aRenderMode == eMaterialRenderMode_DiffuseFog)
		{
			cMaterialType_Water_Vars *pVars = static_cast<cMaterialType_Water_Vars*>(apMaterial->GetVars());

			tFlag lFlags =0;
			
			if(iRenderer::GetRefractionEnabled())
			{
				if(pVars->mbHasReflection)								lFlags |= eFeature_Diffuse_Reflection;
				if(apMaterial->GetTexture(eMaterialTexture_CubeMap))	lFlags |= eFeature_Diffuse_CubeMapReflection;
			}

			if(pVars->mfReflectionFadeEnd>0)						lFlags |= eFeature_Diffuse_ReflectionFading;
			if(aRenderMode == eMaterialRenderMode_DiffuseFog)		lFlags |= eFeature_Diffuse_Fog;
			
			return mpProgramManager->GenerateProgram(eMaterialRenderMode_Diffuse,lFlags);
		}

		return NULL;
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram,iRenderer *apRenderer)
	{
		////////////////////////////
		//Diffuse
		if(aRenderMode == eMaterialRenderMode_Diffuse)
		{
			
		}
	}
	
	//--------------------------------------------------------------------------

	void cMaterialType_Water::SetupMaterialSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,iRenderer *apRenderer)
	{
		////////////////////////////
		//Diffuse
		if(aRenderMode == eMaterialRenderMode_Diffuse || aRenderMode == eMaterialRenderMode_DiffuseFog)
		{
			cMaterialType_Water_Vars *pVars = static_cast<cMaterialType_Water_Vars*>(apMaterial->GetVars());

			////////////////////////////
			// General
			apProgram->SetFloat(kVar_afT, apRenderer->GetTimeCount() * pVars->mfWaveSpeed);
			apProgram->SetFloat(kVar_afRefractionScale, pVars->mfRefractionScale * (float)apRenderer->GetRenderTargetSize().x);
			apProgram->SetFloat(kVar_afWaveAmplitude, pVars->mfWaveAmplitude * 0.04f);
			apProgram->SetFloat(kVar_afWaveFreq	, pVars->mfWaveFreq * 10.0f);

			////////////////////////////
			// Fog
			cWorld *pWorld = apRenderer->GetCurrentWorld();

			apProgram->SetVec2f(kVar_avFogStartAndLength, cVector2f(pWorld->GetFogStart(), pWorld->GetFogEnd() - pWorld->GetFogStart()));
			apProgram->SetColor4f(kVar_avFogColor, pWorld->GetFogColor());
			apProgram->SetFloat(kVar_afFalloffExp, pWorld->GetFogFalloffExp());

			//////////////////////////////
			//Reflection
			if(pVars->mbHasReflection && iRenderer::GetRefractionEnabled())
			{
				apProgram->SetVec2f(kVar_avFrenselBiasPow, cVector2f(pVars->mfFrenselBias, pVars->mfFrenselPow));

				//////////////////////////////
				//Cube map reflection
				if(apMaterial->GetTexture(eMaterialTexture_CubeMap))
				{
					cMatrixf mtxInvView = apRenderer->GetCurrentFrustum()->GetViewMatrix().GetTranspose();
					apProgram->SetMatrixf(kVar_a_mtxInvViewRotation, mtxInvView.GetRotation());
				}
				//////////////////////////////
				//World reflection
				else
				{
					apProgram->SetVec2f(kVar_avReflectionMapSizeMul, cVector2f(1.0f / (float)iRenderer::GetReflectionSizeDiv()) );
				}
				
				//////////////////////////////
				//Reflection fading
				if(pVars->mfReflectionFadeEnd > 0)
				{
					float fLength = pVars->mfReflectionFadeEnd - pVars->mfReflectionFadeStart;
					apProgram->SetVec2f(kVar_avReflectionFadeStartAndLength, cVector2f(-pVars->mfReflectionFadeStart, -fLength) );
				}
			}
		}
	}
	
	//--------------------------------------------------------------------------

	void cMaterialType_Water::SetupObjectSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,iRenderer *apRenderer)
	{
	}


	//--------------------------------------------------------------------------

	iMaterialVars* cMaterialType_Water::CreateSpecificVariables()
	{
		cMaterialType_Water_Vars* pVars = hplNew(cMaterialType_Water_Vars,());

		pVars->mbHasReflection = true;
		pVars->mfRefractionScale = 0.1f;
		pVars->mfFrenselBias = 0.2f;
		pVars->mfFrenselPow = 8.0f;

		return pVars;
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars)
	{
		cMaterialType_Water_Vars *pVars = (cMaterialType_Water_Vars*)apMaterial->GetVars();
		if(pVars==NULL)
		{
			pVars = (cMaterialType_Water_Vars*)CreateSpecificVariables();
			apMaterial->SetVars(pVars);
		}
				
		pVars->mbHasReflection = apVars->GetVarBool("HasReflection", true);
		pVars->mfRefractionScale = apVars->GetVarFloat("RefractionScale", 0.1f);
		pVars->mfFrenselBias = apVars->GetVarFloat("FrenselBias", 0.2f);
		pVars->mfFrenselPow = apVars->GetVarFloat("FrenselPow", 8.0f);
		pVars->mfReflectionFadeStart = apVars->GetVarFloat("ReflectionFadeStart", 0);
		pVars->mfReflectionFadeEnd = apVars->GetVarFloat("ReflectionFadeEnd", 0);
		pVars->mfWaveSpeed = apVars->GetVarFloat("WaveSpeed", 1.0f);
		pVars->mfWaveAmplitude = apVars->GetVarFloat("WaveAmplitude", 1.0f);
		pVars->mfWaveFreq = apVars->GetVarFloat("WaveFreq", 1.0f);

		apMaterial->SetWorldReflectionOcclusionTest( apVars->GetVarBool("OcclusionCullWorldReflection", true));
		apMaterial->SetMaxReflectionDistance( apVars->GetVarFloat("ReflectionFadeEnd", 0.0f));
		apMaterial->SetLargeTransperantSurface( apVars->GetVarBool("LargeSurface", false));
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::GetVariableValues(cMaterial* apMaterial, cResourceVarsObject* apVars)
	{
		cMaterialType_Water_Vars* pVars = (cMaterialType_Water_Vars*)apMaterial->GetVars();

		apVars->AddVarBool("HasReflection", pVars->mbHasReflection);
		apVars->AddVarFloat("RefractionScale", pVars->mfRefractionScale);
		apVars->AddVarFloat("FrenselBias", pVars->mfFrenselBias);
		apVars->AddVarFloat("FrenselPow", pVars->mfFrenselPow);
		apVars->AddVarFloat("ReflectionFadeStart", pVars->mfReflectionFadeStart);
		apVars->AddVarFloat("ReflectionFadeEnd", pVars->mfReflectionFadeEnd);
		apVars->AddVarFloat("WaveSpeed",pVars->mfWaveSpeed);
		apVars->AddVarFloat("WaveAmplitude",pVars->mfWaveAmplitude);
		apVars->AddVarFloat("WaveFreq",pVars->mfWaveFreq);

		apVars->AddVarBool("OcclusionCullWorldReflection", apMaterial->GetWorldReflectionOcclusionTest());
		apVars->AddVarBool("LargeSurface", apMaterial->GetLargeTransperantSurface());
	}

	//--------------------------------------------------------------------------

	void cMaterialType_Water::CompileMaterialSpecifics(cMaterial *apMaterial)
	{
		cMaterialType_Water_Vars *pVars = static_cast<cMaterialType_Water_Vars*>(apMaterial->GetVars());

		/////////////////////////////////////
		//Set if has specific variables
		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_Diffuse, true);
		apMaterial->SetHasSpecificSettings(eMaterialRenderMode_DiffuseFog, true);
		
		
		/////////////////////////////////////
		//Set up the blend mode
		if(iRenderer::GetRefractionEnabled())
			apMaterial->SetBlendMode(eMaterialBlendMode_None);
		else
			apMaterial->SetBlendMode(eMaterialBlendMode_Mul);

		/////////////////////////////////////
		//Set up the refraction
		if(iRenderer::GetRefractionEnabled())
		{
			apMaterial->SetHasRefraction(true);
			apMaterial->SetUseRefractionEdgeCheck(true);
		}

		///////////////////////////
		//Set up reflection
		if(pVars->mbHasReflection && iRenderer::GetRefractionEnabled())
		{
			if(apMaterial->GetTexture(eMaterialTexture_CubeMap)==NULL)
			{
				apMaterial->SetHasWorldReflection(true);
			}
		}
	}
	
	//--------------------------------------------------------------------------


}
