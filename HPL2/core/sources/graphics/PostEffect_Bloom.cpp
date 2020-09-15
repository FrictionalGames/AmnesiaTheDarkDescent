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

#include "graphics/PostEffect_Bloom.h"

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

	#define kVar_avRgbToIntensity	0

	#define kVar_afBlurSize			2
	


	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT BASE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cPostEffectType_Bloom::cPostEffectType_Bloom(cGraphics *apGraphics, cResources *apResources) : iPostEffectType("Bloom",apGraphics,apResources)
	{
		///////////////////////////
		// Load programs
		for(int i=0; i<2; ++i)
		{
			cParserVarContainer vars;
			if(i==1) vars.Add("BlurHorisontal");
			mpBlurProgram[i] = mpGraphics->CreateGpuProgramFromShaders("BloomBlur",	"posteffect_bloom_blur_vtx.glsl",
																					"posteffect_bloom_blur_frag.glsl", &vars);

			if(mpBlurProgram[i])
			{
				mpBlurProgram[i]->GetVariableAsId("afBlurSize",kVar_afBlurSize);
			}
		}

		cParserVarContainer vars;
		vars.Add("UseUv");
		vars.Add("UseUvCoord1");
		mpBloomProgram = mpGraphics->CreateGpuProgramFromShaders(	"BloomBlur",	"deferred_base_vtx.glsl",
																					"posteffect_bloom_add_frag.glsl", &vars);
		if(mpBloomProgram)
		{
			mpBloomProgram->GetVariableAsId("avRgbToIntensity",kVar_avRgbToIntensity);
		}
	}
	
	//-----------------------------------------------------------------------

	cPostEffectType_Bloom::~cPostEffectType_Bloom()
	{

	}

	//-----------------------------------------------------------------------

	iPostEffect * cPostEffectType_Bloom::CreatePostEffect(iPostEffectParams *apParams)
	{
		cPostEffect_Bloom *pEffect = hplNew(cPostEffect_Bloom, (mpGraphics,mpResources,this));
		cPostEffectParams_Bloom *pBloomParams = static_cast<cPostEffectParams_Bloom*>(apParams);

		return pEffect;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPostEffect_Bloom::cPostEffect_Bloom(cGraphics *apGraphics, cResources *apResources, iPostEffectType *apType) : iPostEffect(apGraphics,apResources,apType)
	{
		cVector2l vSize = mpLowLevelGraphics->GetScreenSizeInt();
		
		for(int i=0;i<2; ++i)
		{
			mpBlurBuffer[i] = mpGraphics->GetTempFrameBuffer(vSize/4,ePixelFormat_RGBA,i);
			if(mpBlurBuffer[i])
				mpBlurTexture[i] = mpBlurBuffer[i]->GetColorBuffer(0)->ToTexture();
		}

		mpBloomType = static_cast<cPostEffectType_Bloom*>(mpType);
	}

	//-----------------------------------------------------------------------

	cPostEffect_Bloom::~cPostEffect_Bloom()
	{

	}

	//-----------------------------------------------------------------------

	void cPostEffect_Bloom::OnSetParams()
	{
		
	}

	//-----------------------------------------------------------------------


	iTexture* cPostEffect_Bloom::RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)
	{
		/////////////////////////
		// Init render states
		mpCurrentComposite->SetFlatProjection();
		mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
		mpCurrentComposite->SetChannelMode(eMaterialChannelMode_RGBA);

		for(int i=0; i<2; ++i)
		{
			//Reverse order so blur program is not set unneeded times
			if(mpBloomType->mpBlurProgram[1-i])
			{
				mpBloomType->mpBlurProgram[1-i]->SetFloat(kVar_afBlurSize, mParams.mfBlurSize);	
			}
		}
		
		/////////////////////////
		// Render blur
		RenderBlur(apInputTexture);
		for(int i=1; i<mParams.mlBlurIterations;++i)
		{
			RenderBlur(mpBlurTexture[1]);
		}
		

		/////////////////////////
		// Render the input and blur as bloom onto the final buffer.
		// This function sets to frame buffer is post effect is last!
        SetFinalFrameBuffer(apFinalTempBuffer);

		mpCurrentComposite->SetTexture(0, mpBlurTexture[1]);
		mpCurrentComposite->SetTexture(1, apInputTexture);
		
		mpCurrentComposite->SetProgram(mpBloomType->mpBloomProgram);
		if(mpBloomType->mpBloomProgram)
		{
			mpBloomType->mpBloomProgram->SetVec3f(kVar_avRgbToIntensity, mParams.mvRgbToIntensity);
		}
		
		DrawQuad(0,1,mpBlurTexture[1], apInputTexture, true, true);
		
		return apFinalTempBuffer->GetColorBuffer(0)->ToTexture();
	}

	//-----------------------------------------------------------------------

	void cPostEffect_Bloom::RenderBlur(iTexture *apInputTex)
	{
		SetFrameBuffer(mpBlurBuffer[0]);
		mpCurrentComposite->SetProgram(mpBloomType->mpBlurProgram[0]);
		mpCurrentComposite->SetTexture(0, apInputTex);
		DrawQuad(0,1,apInputTex,true);

		SetFrameBuffer(mpBlurBuffer[1]);
		mpCurrentComposite->SetProgram(mpBloomType->mpBlurProgram[1]);
		mpCurrentComposite->SetTexture(0, mpBlurTexture[0]);
		DrawQuad(0,1,mpBlurTexture[0],true);
	}

	//-----------------------------------------------------------------------

}
