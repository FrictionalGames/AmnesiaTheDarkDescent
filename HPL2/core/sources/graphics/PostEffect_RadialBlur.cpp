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

#include "graphics/PostEffect_RadialBlur.h"

#include "graphics/Graphics.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/PostEffectComposite.h"
#include "graphics/FrameBuffer.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/GPUShader.h"

#include "system/PreprocessParser.h"

namespace hpl {
	
	//////////////////////////////////////////////////////////////////////////
	// PROGRAM VARS
	//////////////////////////////////////////////////////////////////////////

	#define kVar_afSize				0
	#define kVar_avHalfScreenSize	1
	#define kVar_afBlurStartDist	2

	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT BASE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cPostEffectType_RadialBlur::cPostEffectType_RadialBlur(cGraphics *apGraphics, cResources *apResources) : iPostEffectType("RadialBlur",apGraphics,apResources)
	{
		cParserVarContainer vars;
		mpProgram = mpGraphics->CreateGpuProgramFromShaders("RadialBlur","deferred_base_vtx.glsl", "posteffect_radial_blur_frag.glsl", &vars);
		if(mpProgram)
		{
			mpProgram->GetVariableAsId("afSize",kVar_afSize);
			mpProgram->GetVariableAsId("avHalfScreenSize",kVar_avHalfScreenSize);
			mpProgram->GetVariableAsId("afBlurStartDist", kVar_afBlurStartDist);
		}
	}
	
	//-----------------------------------------------------------------------

	cPostEffectType_RadialBlur::~cPostEffectType_RadialBlur()
	{

	}

	//-----------------------------------------------------------------------

	iPostEffect * cPostEffectType_RadialBlur::CreatePostEffect(iPostEffectParams *apParams)
	{
		cPostEffect_RadialBlur *pEffect = hplNew(cPostEffect_RadialBlur, (mpGraphics,mpResources,this));
		cPostEffectParams_RadialBlur *pRadialBlurParams = static_cast<cPostEffectParams_RadialBlur*>(apParams);

		return pEffect;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPostEffect_RadialBlur::cPostEffect_RadialBlur(cGraphics *apGraphics, cResources *apResources, iPostEffectType *apType) : iPostEffect(apGraphics,apResources,apType)
	{
		cVector2l vSize = mpLowLevelGraphics->GetScreenSizeInt();
		
		mpRadialBlurType = static_cast<cPostEffectType_RadialBlur*>(mpType);
	}

	//-----------------------------------------------------------------------

	cPostEffect_RadialBlur::~cPostEffect_RadialBlur()
	{

	}

	//-----------------------------------------------------------------------

	void cPostEffect_RadialBlur::Reset()
	{
	}

	//-----------------------------------------------------------------------

	void cPostEffect_RadialBlur::OnSetParams()
	{	
	}
	
	
	//-----------------------------------------------------------------------


	iTexture* cPostEffect_RadialBlur::RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)
	{
		/////////////////////////
		// Init render states
		mpCurrentComposite->SetFlatProjection();
		mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
		mpCurrentComposite->SetChannelMode(eMaterialChannelMode_RGBA);
		mpCurrentComposite->SetTextureRange(NULL,1);

		cVector2l vRenderTargetSize = mpCurrentComposite->GetRenderTargetSize();
		cVector2f vRenderTargetSizeFloat((float)vRenderTargetSize.x, (float)vRenderTargetSize.y);

		/////////////////////////
		// Render to accum buffer
		// This function sets to frame buffer is post effect is last!
		SetFinalFrameBuffer(apFinalTempBuffer);

		mpCurrentComposite->SetProgram(mpRadialBlurType->mpProgram);
		
		if(mpRadialBlurType->mpProgram)
		{
			mpRadialBlurType->mpProgram->SetFloat(kVar_afSize, mParams.mfSize*vRenderTargetSizeFloat.x);
			mpRadialBlurType->mpProgram->SetFloat(kVar_afBlurStartDist, mParams.mfBlurStartDist);
			mpRadialBlurType->mpProgram->SetVec2f(kVar_avHalfScreenSize,  vRenderTargetSizeFloat*0.5f);
		}
				
		mpCurrentComposite->SetTexture(0, apInputTexture);
					
				
		DrawQuad(0, 1, apInputTexture, true);

		mpCurrentComposite->SetProgram(NULL);
		mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
		
		return apFinalTempBuffer->GetColorBuffer(0)->ToTexture();
	}

	//-----------------------------------------------------------------------

}
