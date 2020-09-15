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

#ifndef HPL_POSTEFFECT_BLOOM_H
#define HPL_POSTEFFECT_BLOOM_H

#include "graphics/PostEffect.h"

namespace hpl {

	//------------------------------------------
	
	class cPostEffectParams_Bloom : public iPostEffectParams
	{
	public:
		cPostEffectParams_Bloom() : iPostEffectParams("Bloom"),  
			mvRgbToIntensity(0.3f, 0.58f, 0.12f),
			mlBlurIterations(2),
			mfBlurSize(1.0f)
		{}
		
		kPostEffectParamsClassInit(cPostEffectParams_Bloom)

        cVector3f mvRgbToIntensity;

		float mfBlurSize;
		int mlBlurIterations;
	};

	//------------------------------------------
	
	class cPostEffectType_Bloom : public iPostEffectType
	{
	friend class cPostEffect_Bloom;
	public:
		cPostEffectType_Bloom(cGraphics *apGraphics, cResources *apResources);
		virtual ~cPostEffectType_Bloom();

		iPostEffect *CreatePostEffect(iPostEffectParams *apParams);
	
	private:
		iGpuProgram *mpBlurProgram[2];
		iGpuProgram *mpBloomProgram;
	};
	
	//------------------------------------------

	class cPostEffect_Bloom : public iPostEffect
	{
	public:
		cPostEffect_Bloom(cGraphics *apGraphics,cResources *apResources, iPostEffectType *apType);
		~cPostEffect_Bloom();

	private:
		void OnSetParams();
		iPostEffectParams *GetTypeSpecificParams() { return &mParams; }
		
		iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

		void RenderBlur(iTexture *apInputTex);

		iFrameBuffer *mpBlurBuffer[2];
		iTexture *mpBlurTexture[2];
		
		cPostEffectType_Bloom *mpBloomType;

		cPostEffectParams_Bloom mParams;
	};

	//------------------------------------------

};
#endif // HPL_POSTEFFECT_BLOOM_H
