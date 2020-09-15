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

#include "graphics/Material.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "resources/GpuShaderManager.h"

#include "graphics/GPUShader.h"
#include "graphics/GPUProgram.h"
#include "graphics/MaterialType.h"

#include "math/Math.h"


namespace hpl {

	bool cMaterial::mbDestroyTypeSpecifics = true;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial::cMaterial(const tString& asName, const tWString& asFullPath, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType) 
		: iResourceBase(asName, asFullPath, 0)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		mpType = NULL;
		mpVars = NULL;
		SetType(apType);

		mbAutoDestroyTextures = true;

		mlRenderFrameCount = -1;

		mbHasRefraction = false;
		mlRefractionTextureUnit =0;
		mbUseRefractionEdgeCheck = false;

		mbHasWorldReflection = false;
		mlWorldReflectionTextureUnit =0;
		mbWorldReflectionOcclusionTest = true;
		mfMaxReflectionDistance = 0;

		mbHasTranslucentIllumination = false; //If the material is translucent and also need an extra additive pass.

		mbLargeTransperantSurface = false;

		mbUseAlphaDissolveFilter = false;

		mbAffectedByFog = true;
		
		for(int i=0; i<eMaterialRenderMode_LastEnum; ++i)
		{
			mbHasSpecificSettings[i] = false;
			mbHasObjectSpecificsSettings[i] = false;
		}

		mfAnimTime = 0;
		m_mtxUV = cMatrixf::Identity;
		mbHasUvAnimation = false;

		////////////////////////
		// Set up variables
		for(int i=0;i<eMaterialTexture_LastEnum; ++i) 
		{
			mvTextures[i] = NULL;
		}
		for(int j=0; j<2; ++j)
		for(int i=0;i<eMaterialRenderMode_LastEnum; ++i) 
		{
			mvPrograms[j][i] = NULL;
		}
		for(int i=0; i<eMaterialRenderMode_LastEnum;++i)
		for(int j=0; j<kMaxTextureUnits; ++j)
		{
			mvTextureInUnit[i][j] = NULL;
		}
		

		///////////////////////
		//Set up depending in type
		if(mpType->IsTranslucent())
		{
			mBlendMode = eMaterialBlendMode_Add;
		}
		else
		{
			mBlendMode = eMaterialBlendMode_None;
		}
		mAlphaMode = eMaterialAlphaMode_Solid;
		mbDepthTest = true;

	}

	//-----------------------------------------------------------------------
	
	cMaterial::~cMaterial()
	{
		if(mpVars) hplDelete(mpVars);

		if(mbDestroyTypeSpecifics && mpType)
		{
			// Destroy all programs
			for(int i=0;i<eMaterialRenderMode_LastEnum; ++i) 
			for(int j=0;j<2; ++j)
			{
				if(mvPrograms[j][i])
				{
					mpType->DestroyProgram(this, (eMaterialRenderMode)i,mvPrograms[j][i], j);
				}
			}
		}

		////////////////////////
		// Destroy all textures
		if(mbAutoDestroyTextures)
		{
			for(int i=0;i<eMaterialTexture_LastEnum; ++i) 
			{
				if(mvTextures[i]) mpResources->GetTextureManager()->Destroy(mvTextures[i]);
			}
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial::SetType(iMaterialType* apType)
	{
		if(mpType==apType) return;

		mpType = apType;

		if(mpVars) hplDelete(mpVars);
		if(mpType) mpVars = mpType->CreateSpecificVariables();
	}

	//-----------------------------------------------------------------------

	void cMaterial::Compile()
	{
		////////////////////////
		//Reset some settings before compiling
		for(int i=0; i<eMaterialRenderMode_LastEnum; ++i)
		{
			mbHasSpecificSettings[i] = false;
			mbHasObjectSpecificsSettings[i] = false;
		}

		///////////////////
		// Get the programs
		for(int i=0;i<eMaterialRenderMode_LastEnum; ++i) 
		for(int j=0;j<2; ++j)
		{
			iGpuProgram *pPrevProg = mvPrograms[j][i];
			mvPrograms[j][i] = mpType->GetGpuProgram(this, (eMaterialRenderMode)i, j);

			//Destroy any previous program (this is so recompilations work with program count!)
			if(pPrevProg) mpType->DestroyProgram(this, (eMaterialRenderMode)i,pPrevProg, j);
		}

		///////////////////
		// Compile texture lookup
		for(int i=0;i<eMaterialRenderMode_LastEnum; ++i) 
			for(int j=0; j<kMaxTextureUnits; ++j)
			{
				mvTextureInUnit[i][j] = mpType->GetTextureForUnit(this, (eMaterialRenderMode)i, j);
			}
		
		///////////////////
		// Type specifics
		mpType->CompileMaterialSpecifics(this);
	}
	
	//-----------------------------------------------------------------------
	
	void cMaterial::SetTexture(eMaterialTexture aType, iTexture *apTexture)
	{
		mvTextures[aType] = apTexture;
	}
	
	//-----------------------------------------------------------------------

	iTexture *cMaterial::GetTexture(eMaterialTexture aType)
	{
		return mvTextures[aType];
	}

	//-----------------------------------------------------------------------

	cResourceVarsObject* cMaterial::GetVarsObject()
	{
		cResourceVarsObject* pVarsObject = hplNew(cResourceVarsObject,());
		mpType->GetVariableValues(this, pVarsObject);

		return pVarsObject;
	}

	//-----------------------------------------------------------------------

	void cMaterial::LoadVariablesFromVarsObject(cResourceVarsObject* apVarsObject)
	{
		mpType->LoadVariables(this, apVarsObject);		
	}

	//-----------------------------------------------------------------------

	void cMaterial::SetBlendMode(eMaterialBlendMode aBlendMode)
	{
		if(mpType->IsTranslucent()==false) return;

		mBlendMode = aBlendMode;
	}
	
	void cMaterial::SetAlphaMode(eMaterialAlphaMode aAlphaMode)
	{
		//if(mpType->IsTranslucent()) return;

		mAlphaMode = aAlphaMode;
	}

	void cMaterial::SetDepthTest(bool abDepthTest)
	{
		if(mpType->IsTranslucent()==false) return;

		mbDepthTest = abDepthTest;
	}

	//-----------------------------------------------------------------------

	void cMaterial::UpdateBeforeRendering(float afTimeStep)
	{
		if(mbHasUvAnimation) UpdateAnimations(afTimeStep);
	}

	//-----------------------------------------------------------------------

	void cMaterial::AddUvAnimation(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis)
	{
		mvUvAnimations.push_back(cMaterialUvAnimation(aType, afSpeed, afAmp, aAxis));

		mbHasUvAnimation = true;
	}

	//-----------------------------------------------------------------------

	void cMaterial::ClearUvAnimations()
	{
		mvUvAnimations.clear();

		mbHasUvAnimation = false;

		m_mtxUV = cMatrixf::Identity;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static cVector3f GetAxisVector(eMaterialAnimationAxis aAxis)
	{
		switch(aAxis)
		{
		case eMaterialAnimationAxis_X: return cVector3f(1,0,0);
		case eMaterialAnimationAxis_Y: return cVector3f(0,1,0);
		case eMaterialAnimationAxis_Z: return cVector3f(0,0,1);
		}
		return 0;
	}

	//-----------------------------------------------------------------------
	
	void cMaterial::UpdateAnimations(float afTimeStep)
	{
		m_mtxUV = cMatrixf::Identity;

        for(size_t i=0; i<mvUvAnimations.size(); ++i)
		{
			cMaterialUvAnimation *pAnim = &mvUvAnimations[i];

			///////////////////////////
			// Translate
			if(pAnim->mType == eMaterialUvAnimation_Translate)
			{
				cVector3f vDir = GetAxisVector(pAnim->mAxis);

				cMatrixf mtxAdd = cMath::MatrixTranslate(vDir * pAnim->mfSpeed * mfAnimTime);
				m_mtxUV = cMath::MatrixMul(m_mtxUV, mtxAdd);
			}
			///////////////////////////
			// Sin
			else if(pAnim->mType == eMaterialUvAnimation_Sin)
			{
				cVector3f vDir = GetAxisVector(pAnim->mAxis);
				
				cMatrixf mtxAdd = cMath::MatrixTranslate(vDir * sin(mfAnimTime * pAnim->mfSpeed) * pAnim->mfAmp);
				m_mtxUV = cMath::MatrixMul(m_mtxUV, mtxAdd);
			}
			///////////////////////////
			// Rotate
			else if(pAnim->mType == eMaterialUvAnimation_Rotate)
			{
				cVector3f vDir = GetAxisVector(pAnim->mAxis);

				cMatrixf mtxRot = cMath::MatrixRotate(vDir * pAnim->mfSpeed * mfAnimTime,eEulerRotationOrder_XYZ);
				m_mtxUV = cMath::MatrixMul(m_mtxUV, mtxRot);
			}
		}

		mfAnimTime += afTimeStep;
	}

	//-----------------------------------------------------------------------

}
