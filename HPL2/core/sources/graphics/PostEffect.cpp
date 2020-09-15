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

#include "graphics/PostEffect.h"

#include "graphics/Graphics.h"

#include "graphics/PostEffectComposite.h"
#include "graphics/FrameBuffer.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Texture.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT BASE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	iPostEffectType::iPostEffectType(const tString& asName, cGraphics *apGraphics, cResources *apResources)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		msName = asName;
	}
	
	//-----------------------------------------------------------------------

	iPostEffectType::~iPostEffectType()
	{

	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// POST EFFECT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPostEffect::iPostEffect(cGraphics *apGraphics,cResources *apResources, iPostEffectType *apType)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
		mpType = apType;

		mbFinalFrameBufferUsed = false;

		mpLowLevelGraphics = mpGraphics->GetLowLevel();

		mbActive = true;
		mbDisabled = false;
	}

	//-----------------------------------------------------------------------

	iPostEffect::~iPostEffect()
	{
	}

	//-----------------------------------------------------------------------

	iTexture* iPostEffect::Render(cPostEffectComposite *apComposite, iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer, bool abLastEffect)
	{
		///////////////////////////
		// Set up variables and data
		mpCurrentComposite = apComposite;
		mbIsLastEffect = abLastEffect;

		mbFinalFrameBufferUsed = false;

		///////////////////////////
		// Render
		iTexture *pOutputTex = RenderEffect(apInputTexture, apFinalTempBuffer);
		
		//////////////////////////
		// If last effect and final frame buffer has not been called, copy o rendertarget
		if(mbIsLastEffect && mbFinalFrameBufferUsed == false)
		{
			mpCurrentComposite->SetProgram(NULL);
			mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);

			cRenderTarget *pRenderTarget = mpCurrentComposite->GetCurrentRenderTarget();
			mpCurrentComposite->SetFrameBuffer(pRenderTarget->mpFrameBuffer, true);

			mpCurrentComposite->SetTexture(0, pOutputTex);

			DrawQuad(0, 1, pOutputTex, true);
		}

		return pOutputTex;
	}
	
	//-----------------------------------------------------------------------

	void iPostEffect::SetActive(bool abX)
	{
		if(mbActive == abX) return;

		mbActive = abX;

		OnSetActive(abX);
	}
	
	//-----------------------------------------------------------------------

	void iPostEffect::SetParams(iPostEffectParams *apSrcParams)
	{
		if(mpType==NULL) return;

		//Make sure the type is correct!
		if(apSrcParams->GetName() != mpType->GetName()) return;

        GetTypeSpecificParams()->LoadFrom(apSrcParams);
		OnSetParams();
	}
	
	void iPostEffect::GetParams(iPostEffectParams *apDestParams)
	{
		if(mpType==NULL) return;

		//Make sure the type is correct!
		if(apDestParams->GetName() != mpType->GetName()) return;

		GetTypeSpecificParams()->CopyTo(apDestParams);
	}

	//-----------------------------------------------------------------------

	void iPostEffect::SetFinalFrameBuffer(iFrameBuffer *apOutputBuffer)
	{
		mbFinalFrameBufferUsed = true;

		///////////////////////
		// Set the frame buffer
		if(mbIsLastEffect)
		{
			cRenderTarget *pRenderTarget = mpCurrentComposite->GetCurrentRenderTarget();
			mpCurrentComposite->SetFrameBuffer(pRenderTarget->mpFrameBuffer, true);
		}
		else
		{
			mpCurrentComposite->SetFrameBuffer(apOutputBuffer, true);
		}
	}

	//-----------------------------------------------------------------------

	void iPostEffect::GetTextureUvPosAndSize(const cVector2f& avTexSize, cVector2f& avUvPos,  cVector2f& avUvSize)
	{
		cVector2f vScreenSizeFloat = mpLowLevelGraphics->GetScreenSizeFloat();
		cRenderTarget *pRenderTarget = mpCurrentComposite->GetCurrentRenderTarget();
		cVector2l vRenderTargetSize = mpCurrentComposite->GetRenderTargetSize();
		cVector2f vViewportSize((float)vRenderTargetSize.x, (float)vRenderTargetSize.y);
		cVector2f vViewportPos((float)pRenderTarget->mvPos.x, (float)pRenderTarget->mvPos.y);

		cVector2f vRelPos = vViewportPos / vScreenSizeFloat;
		cVector2f vRelSize = vViewportSize / vScreenSizeFloat;

		avUvPos = vRelPos * avTexSize;
		avUvSize = vRelSize * avTexSize;
	}

	//-----------------------------------------------------------------------

	void iPostEffect::SetFrameBuffer(iFrameBuffer *apFrameBuffer)
	{
		iTexture *pTex = apFrameBuffer->GetColorBuffer(0)->ToTexture();

		/////////////////////
		//Check if texture is same size as screen, if so no need do any extra calcs
		if(pTex->GetSizeInt2D() == mpLowLevelGraphics->GetScreenSizeInt())
		{
			mpCurrentComposite->SetFrameBuffer(apFrameBuffer,true);
		}
		/////////////////////
		//Texture does not have same size as screen, need to do extra calculations
		else
		{
			cVector2f vTexSize = pTex->GetSizeFloat2D();
			cVector2f vUvPos, vUvSize;
			GetTextureUvPosAndSize(vTexSize,vUvPos, vUvSize);

			cVector2l vTargetPos((int)(vUvPos.x+0.5f), (int)(vUvPos.y+0.5f));		
			cVector2l vTargetSize((int)(vUvSize.x+0.5f), (int)(vUvSize.y+0.5f));

			mpLowLevelGraphics->SetCurrentFrameBuffer(apFrameBuffer, vTargetPos, vTargetSize);
		}
	}

	//-----------------------------------------------------------------------

	void iPostEffect::DrawQuad(const cVector3f& avPos,  const cVector2f& avSize, iTexture *apTexture, bool abFlipY)
	{
		cVector2f vTexSize = apTexture->GetSizeFloat2D();
		cVector2f vUvPos, vUvSize;
		GetTextureUvPosAndSize(vTexSize,vUvPos,vUvSize);

		mpCurrentComposite->DrawQuad(avPos,avSize,
									cVector2f(vUvPos.x, (vTexSize.y - vUvSize.y)-vUvPos.y), 
									cVector2f(vUvPos.x + vUvSize.x,vTexSize.y - vUvPos.y),
									abFlipY);
	}

	//-----------------------------------------------------------------------

	void iPostEffect::DrawQuad(	const cVector3f& avPos,  const cVector2f& avSize, iTexture *apTexture0, iTexture *apTexture1, 
								bool abFlipY0,bool abFlipY1)
	{
		cVector2f vTexSize[2] = {apTexture0->GetSizeFloat2D(), apTexture1->GetSizeFloat2D()};
		cVector2f vTexMin[2], vTexMax[2];
		
		for(int i=0; i<2; ++i)
		{
			cVector2f vUvPos, vUvSize;
			GetTextureUvPosAndSize(vTexSize[i],vUvPos,vUvSize);
			vTexMin[i] = cVector2f(vUvPos.x, (vTexSize[i].y - vUvSize.y)-vUvPos.y); 
			vTexMax[i] = cVector2f(vUvPos.x + vUvSize.x,vTexSize[i].y - vUvPos.y);
		}

		mpCurrentComposite->DrawQuad(cVector2f(0,0),1,vTexMin[0],vTexMax[0],vTexMin[1],vTexMax[1],abFlipY0,abFlipY1);
	}

	//-----------------------------------------------------------------------

}
