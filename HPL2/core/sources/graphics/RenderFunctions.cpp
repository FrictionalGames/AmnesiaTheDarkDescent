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

#include "graphics/RenderFunctions.h"

#include "graphics/RenderList.h"
#include "graphics/FrameBuffer.h"
#include "graphics/GPUProgram.h"
#include "graphics/VertexBuffer.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Texture.h"
#include "graphics/Graphics.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"
#include "math/Frustum.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------------

	void iRenderFunctions::SetupRenderFunctions(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		mvScreenSize = mpLowLevelGraphics->GetScreenSizeInt();
		mvScreenSizeFloat = mpLowLevelGraphics->GetScreenSizeFloat();
	}

	//-----------------------------------------------------------------------
	
	void iRenderFunctions::InitAndResetRenderFunctions(	cFrustum *apFrustum, cRenderTarget *apRenderTarget, bool abLog,
														bool abUseGlobalScissorRect, 
														const cVector2l& avGlobalScissorRectPos, const cVector2l& avGlobalScissorRectSize)
	{
		mpCurrentFrustum = apFrustum;
		mpCurrentRenderTarget = apRenderTarget;
		mbLog = abLog;

		////////////////////////////////
		//Set up variables
		mpCurrentProjectionMatrix = NULL;
		mbInvertCullMode = false;
		mbCurrentDepthTest = true;
		mbCurrentDepthWrite = true;
		mbCurrentStencilActive = false;
		mvCurrentScissorRectPos =0;
		mvCurrentScissorRectSize = -1;
		mbCurrentScissorActive = false;
		mCurrentDepthTestFunc = eDepthTestFunc_LessOrEqual;
		mbCurrentCullActive = true;
		mCurrentCullMode = eCullMode_CounterClockwise;
		mCurrentChannelMode = eMaterialChannelMode_RGBA;
		mCurrentAlphaMode = eMaterialAlphaMode_LastEnum;
		mfCurrentAlphaLimit = -1;
		mCurrentBlendMode = eMaterialBlendMode_LastEnum;
		mpCurrentProgram = NULL;
		mpCurrentVtxBuffer = NULL;
		mpCurrentMatrix = &m_mtxNULL;
		mpCurrentMaterial = NULL;
		mpCurrentMaterialType = NULL;

		for(int i=0; i<kMaxTextureUnits; ++i) mvCurrentTexture[i] = NULL;

		////////////////////////////////
		//Get size of render target
		cVector2l vFrameBufferSize = mpCurrentRenderTarget->mpFrameBuffer ? mpCurrentRenderTarget->mpFrameBuffer->GetSize() : mvScreenSize;
		mvRenderTargetSize.x = mpCurrentRenderTarget->mvSize.x <0 ? vFrameBufferSize.x : mpCurrentRenderTarget->mvSize.x;
		mvRenderTargetSize.y = mpCurrentRenderTarget->mvSize.y <0 ? vFrameBufferSize.y : mpCurrentRenderTarget->mvSize.y;

		mvCurrentFrameBufferSize = vFrameBufferSize;

		////////////////////////////////
		//Set up scissor rect
		mbUseGlobalScissorRect = abUseGlobalScissorRect;
		mvGlobalScissorRectPos = avGlobalScissorRectPos;
		mvGlobalScissorRectSize = avGlobalScissorRectSize;
		mbGlobalScissorRectActive = false;
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::ExitAndCleanUpRenderFunctions()
	{
		////////////////////////////////
		//Clean up scissor rect
		if(mbUseGlobalScissorRect)
		{
			mbUseGlobalScissorRect = false;
			
			if(mbLog) Log("  Setting scissor active: 0\n");
			mpLowLevelGraphics->SetScissorActive(false);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void iRenderFunctions::SetFlatProjection(const cVector2f &avSize,float afMin,float afMax)
	{
		if(mbLog) Log(" Setting Ortho Projection size: %fx%f, minz: %f maxz: %f\n",avSize.x, avSize.y, afMin,afMax);

		mpLowLevelGraphics->SetOrthoProjection(avSize,afMin,afMax);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);
		mpCurrentMatrix = &m_mtxNULL;
		mpCurrentProjectionMatrix = NULL;

		SetInvertCullMode(false);
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetFlatProjectionMinMax(const cVector3f &avMin,const cVector3f &avMax)
	{
		if(mbLog) Log(" Setting Ortho Projection min: %s, max: %s\n",avMin.ToString().c_str(), avMax.ToString().c_str());

		mpLowLevelGraphics->SetOrthoProjection(avMin,avMax);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);
		mpCurrentMatrix = &m_mtxNULL;
		mpCurrentProjectionMatrix = NULL;

		SetInvertCullMode(false);
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetNormalFrustumProjection()
	{
		SetFrustumProjection(mpCurrentFrustum);
	}

	void iRenderFunctions::SetFrustumProjection(cFrustum *apFrustum)
	{
		SetProjectionMatrix(&apFrustum->GetProjectionMatrix());
		
		SetInvertCullMode(mpCurrentFrustum->GetInvertsCullMode());
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetProjectionMatrix(const cMatrixf *apProjMatrix)
	{
		if(mpCurrentProjectionMatrix == apProjMatrix) return;

		if(mbLog) Log(" Setting projection matrix: %d  %s.\n",apProjMatrix, apProjMatrix->ToString().c_str());

		mpLowLevelGraphics->SetMatrix(eMatrix_Projection, *apProjMatrix);
		mpCurrentMatrix = &m_mtxNULL;
		mpCurrentProjectionMatrix = apProjMatrix;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetDepthTest(bool abX)
	{
		if(mbCurrentDepthTest == abX) return false;

		if(mbLog) Log("  Setting depth test active: %d\n", abX);

		mpLowLevelGraphics->SetDepthTestActive(abX);
		mbCurrentDepthTest = abX;

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetDepthWrite(bool abX)
	{
		if(mbCurrentDepthWrite == abX) return false;

		if(mbLog) Log("  Setting depth write active: %d\n", abX);

		mpLowLevelGraphics->SetDepthWriteActive(abX);
		mbCurrentDepthWrite = abX;

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetDepthTestFunc(eDepthTestFunc aFunc)
	{
		if(mCurrentDepthTestFunc == aFunc) return false;

		if(mbLog) 
		{
			tString sFunc="Unknown";
			switch(aFunc)
			{
			case eDepthTestFunc_Never:			sFunc = "Never"; break;
			case eDepthTestFunc_Less:			sFunc = "Less"; break;
			case eDepthTestFunc_LessOrEqual:	sFunc = "LessOrEqual"; break;
			case eDepthTestFunc_Greater:		sFunc = "Greater"; break;
			case eDepthTestFunc_GreaterOrEqual:	sFunc = "GreaterOrEqual"; break;
			case eDepthTestFunc_Equal:			sFunc = "Equal"; break;
			case eDepthTestFunc_NotEqual:		sFunc = "NotEqual"; break;
			case eDepthTestFunc_Always:			sFunc = "Always"; break;
			}
			Log("  Setting depth func: %s\n", sFunc.c_str());
		}

		mCurrentDepthTestFunc = aFunc;
		mpLowLevelGraphics->SetDepthTestFunc(aFunc);

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetCullActive(bool abX)
	{
		if(mbCurrentCullActive == abX) return false;

		if(mbLog) Log("  Setting cull active: %d\n",abX);

		mbCurrentCullActive = abX;
		mpLowLevelGraphics->SetCullActive(abX);

		return true;
	}

	//-----------------------------------------------------------------------


	bool iRenderFunctions::SetCullMode(eCullMode aMode, bool abCheckIfInverted)
	{
		if(abCheckIfInverted && mbInvertCullMode)
			aMode = aMode==eCullMode_Clockwise ? eCullMode_CounterClockwise : eCullMode_Clockwise;

		if(mCurrentCullMode == aMode) return false;

		if(mbLog)
		{
			tString sMode="Unknown";
			switch(aMode)
			{
			case eCullMode_Clockwise:			sMode = "Clockwise"; break;
			case eCullMode_CounterClockwise:	sMode = "CounterClockwise"; break;
			}
			Log("  Setting cull mode: %s\n", sMode.c_str());
		}

		mCurrentCullMode = aMode;
		mpLowLevelGraphics->SetCullMode(aMode);

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetStencilActive(bool abX)
	{
		if(mbCurrentStencilActive == abX) return false;

		if(mbLog) Log("  Setting stencil active: %d\n",abX);

		mpLowLevelGraphics->SetStencilActive(abX);
		mbCurrentStencilActive = abX;

		return true;
	}


	//-----------------------------------------------------------------------

	void iRenderFunctions::SetStencilWriteMask(unsigned int alMask)
	{
		mpLowLevelGraphics->SetStencilWriteMask(alMask);
	}
	
	void iRenderFunctions::SetStencil(	eStencilFunc aFunc,int alRef, unsigned int aMask,
										eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)
	{
		mpLowLevelGraphics->SetStencil(aFunc, alRef, aMask, aFailOp, aZFailOp, aZPassOp);
	}
	
	void iRenderFunctions::SetStencilTwoSide(	eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
												int alRef, unsigned int aMask,
												eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
												eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)
	{
		mpLowLevelGraphics->SetStencilTwoSide(	aFrontFunc, aBackFunc, 
												alRef, aMask,
												aFrontFailOp, aFrontZFailOp, aFrontZPassOp,
												aBackFailOp, aBackZFailOp, aBackZPassOp);
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetScissorActive(bool abX)
	{
		if(mbCurrentScissorActive == abX) return false;

		////////////////////////
		// Change scissor rect
		if(mbUseGlobalScissorRect)
		{
			if(mbCurrentScissorActive)
			{
				if(mvCurrentScissorRectSize.x > -1)
					SetScissorRect(mvCurrentScissorRectPos, mvCurrentScissorRectSize, false);
			}
			else
			{
				SetScissorRect(mvGlobalScissorRectPos, mvGlobalScissorRectSize, false);
			}

			return false;
		}
		////////////////////////
		// Set scissor on / off
		else
		{
			if(mbLog) Log("  Setting scissor active: %d\n",abX);

			mpLowLevelGraphics->SetScissorActive(abX);
			mbCurrentScissorActive = abX;
		}
		
		return true;
	}
	
	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetScissorRect(const cRect2l& aClipRect, bool abAutoEnabling)
	{
		return SetScissorRect(cVector2l(aClipRect.x, aClipRect.y), cVector2l(aClipRect.w, aClipRect.h), abAutoEnabling);
	}

	bool iRenderFunctions::SetScissorRect(const cVector2l& avPos, const cVector2l& avSize, bool abAutoEnabling)
	{
		/////////////////////////////
		// Clip the rect
		cVector2l vFinalPos=avPos + mpCurrentRenderTarget->mvPos;
		cVector2l vFinalSize = avSize;

		cVector2l vMin = mbUseGlobalScissorRect ? mvGlobalScissorRectPos : mpCurrentRenderTarget->mvPos;
		cVector2l vMax = mbUseGlobalScissorRect ? mvGlobalScissorRectSize+vMin : mvRenderTargetSize+vMin;

		if(vFinalPos.x < vMin.x) vFinalPos.x = vMin.x;
		if(vFinalPos.y < vMin.y) vFinalPos.y = vMin.y;
		
		if(vFinalPos.x+vFinalSize.x > vMax.x) vFinalSize.x = vMax.x - vFinalPos.x;
		if(vFinalPos.y+vFinalSize.y > vMax.y) vFinalSize.y = vMax.y - vFinalPos.y;

		/////////////////////////////
		// Check if already set
		if(mvCurrentScissorRectPos == vFinalPos && mvCurrentScissorRectSize == vFinalSize) return false;

		mvCurrentScissorRectPos = vFinalPos;
		mvCurrentScissorRectSize = vFinalSize;

		/////////////////////////////
		// Set the clip rect
		if(	mbUseGlobalScissorRect == false &&
			vFinalPos.x == 0 && vFinalPos.y ==0 &&
			vFinalSize.x == mvRenderTargetSize.x && vFinalSize.y == mvRenderTargetSize.y)
		{
			if(abAutoEnabling)SetScissorActive(false);
			return false;
		}
		else
		{
			if(abAutoEnabling) SetScissorActive(true);
			
			if(mbLog)
				Log("  Setting scissor rect: %d, %d, %dx%d\n",	vFinalPos.x, vFinalPos.y, vFinalSize.x,vFinalSize.y);
			
			mpLowLevelGraphics->SetScissorRect(mvCurrentScissorRectPos, mvCurrentScissorRectSize);
			return true;
		}
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetChannelMode(eMaterialChannelMode aMode)
	{
		if(mCurrentChannelMode == aMode) return false;

		switch(aMode)
		{
		case eMaterialChannelMode_RGBA:
			if(mbLog) Log("  Setting channel mode: RGBA\n");
			mpLowLevelGraphics->SetColorWriteActive(true, true, true, true);
			break;
		case eMaterialChannelMode_RGB:
			if(mbLog) Log("  Setting channel mode: RGB\n");
			mpLowLevelGraphics->SetColorWriteActive(true, true, true, false);
			break;
		case eMaterialChannelMode_A:
			if(mbLog) Log("  Setting channel mode: A\n");
			mpLowLevelGraphics->SetColorWriteActive(false, false, false, true);
			break;
		case eMaterialChannelMode_None:
			if(mbLog) Log("  Setting channel mode: None\n");
			mpLowLevelGraphics->SetColorWriteActive(false, false, false, false);
			break;
		}

		mCurrentChannelMode = aMode;

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetAlphaMode(eMaterialAlphaMode aMode)
	{
		if(mCurrentAlphaMode == aMode) return false;

		if(aMode == eMaterialAlphaMode_Solid)
		{
			if(mbLog) Log("  Setting alpha mode: Solid\n");
			mpLowLevelGraphics->SetAlphaTestActive(false);
		}
		else
		{
			if(mbLog) Log("  Setting alpha mode: Trans\n");
			mpLowLevelGraphics->SetAlphaTestActive(true);
		}
		mCurrentAlphaMode = aMode;

		return true;
	}

	bool iRenderFunctions::SetAlphaLimit(float afLimit)
	{
		if(afLimit == mfCurrentAlphaLimit) return false;

		mfCurrentAlphaLimit = afLimit;

        if(mbLog) Log("  Setting alpha limit: %f\n", mfCurrentAlphaLimit);

		mpLowLevelGraphics->SetAlphaTestFunc(eAlphaTestFunc_GreaterOrEqual, mfCurrentAlphaLimit);

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetBlendMode(eMaterialBlendMode aMode)
	{
		if(mCurrentBlendMode == aMode) return false;

		///////////////////////////
		// Blend off
		if(aMode == eMaterialBlendMode_None)
		{
			if(mbLog) Log("  Setting blend mode: None\n");
			mpLowLevelGraphics->SetBlendActive(false);
		}
		///////////////////////////
		// Blend on
		else
		{
			mpLowLevelGraphics->SetBlendActive(true);

			switch(aMode)
			{
			case eMaterialBlendMode_Add: 
				if(mbLog) Log("  Setting blend mode: Add\n");
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One,eBlendFunc_One);
				break;
			case eMaterialBlendMode_Mul:
				if(mbLog) Log("  Setting blend mode: Mul\n");
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_Zero,eBlendFunc_SrcColor);
				break;
			case eMaterialBlendMode_MulX2:
				if(mbLog) Log("  Setting blend mode: MulX2\n");
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_DestColor,eBlendFunc_SrcColor);
				break;
			case eMaterialBlendMode_Alpha:
				if(mbLog) Log("  Setting blend mode: Alpha\n");
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha,eBlendFunc_OneMinusSrcAlpha);
				break;
			case eMaterialBlendMode_PremulAlpha:
				if(mbLog) Log("  Setting blend mode: PremulAlpha\n");
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One,eBlendFunc_OneMinusSrcAlpha);
				break;
			}
		}

		mCurrentBlendMode = aMode;

		return true;
	}

	//-----------------------------------------------------------------------

	bool iRenderFunctions::SetProgram(iGpuProgram *apProgram)
	{
		if(mpCurrentProgram == apProgram) return false;

		if(apProgram)
		{
			if(mbLog) Log("  Setting gpu program %d : '%s'\n", apProgram, apProgram->GetName().c_str());
			apProgram->Bind();	
		}
		else
		{
			if(mbLog) Log("  Setting gpu program NULL\n");
			mpCurrentProgram->UnBind();
		}

		mpCurrentProgram = apProgram;

		//Setting material this way will always NULL current material and material type.
		mpCurrentMaterialType = NULL;
		mpCurrentMaterial = NULL;

		return true;
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetTexture(int alUnit, iTexture *apTexture)
	{
		if(mvCurrentTexture[alUnit] == apTexture) return;

		if(mbLog) {
			if(apTexture)
				Log("  Setting texture unit: %d, %d/'%s'\n",alUnit,apTexture,apTexture->GetName().c_str());
			else
				Log("  Setting texture unit: %d, 'NULL\n",alUnit);
		}
		mpLowLevelGraphics->SetTexture(alUnit, apTexture);

		mvCurrentTexture[alUnit] = apTexture;
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetTextureRange(iTexture *apTexture, int alFirstUnit, int alLastUnit)
	{
		for(int i=alFirstUnit; i<= alLastUnit; ++i)
		{
			if(mvCurrentTexture[i] != apTexture)
			{
				if(mbLog) {
					if(apTexture)
						Log("  Setting texture unit: %d, %d/'%s'\n",i,apTexture,apTexture->GetName().c_str());
					else
						Log("  Setting texture unit: %d, 'NULL\n",i);
				}

				mpLowLevelGraphics->SetTexture(i, apTexture);
				mvCurrentTexture[i] = apTexture;
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetVertexBuffer(iVertexBuffer *apVtxBuffer)
	{
		if(mpCurrentVtxBuffer == apVtxBuffer) return;

		if(mbLog) {
			if(apVtxBuffer)
				Log("  Setting vertex buffer: %d\n",apVtxBuffer);
			else
				Log("  Setting vertex buffer: NULL\n");
		}

		if(mpCurrentVtxBuffer) mpCurrentVtxBuffer->UnBind();
		if(apVtxBuffer) apVtxBuffer->Bind();

		mpCurrentVtxBuffer = apVtxBuffer;
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetMatrix(cMatrixf *apMatrix)
	{
		//////////////////////////
		//Set matrix
		if(mpCurrentMatrix != apMatrix)
		{
			//Normal matrix
			if(apMatrix)
			{
				if(mbLog) Log("  Setting model matrix: %d / %s\n",apMatrix, apMatrix->ToString().c_str());

				cMatrixf mtxModel = cMath::MatrixMul(mpCurrentFrustum->GetViewMatrix(), *apMatrix);

				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,mtxModel);
			}
			//NULL matrix
			else
			{
				if(mbLog) Log("  Setting model matrix: NULL\n");

				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,mpCurrentFrustum->GetViewMatrix());
			}

			mpCurrentMatrix = apMatrix;
		}

		//////////////////////////
		// Setup program

		//TODO: Only needed when using Cg
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetModelViewMatrix(const cMatrixf& a_mtxModelView)
	{
		//No test here...
		if(mbLog) Log("  Setting view matrix: %s\n",cMath::MatrixToChar(a_mtxModelView));

		mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,a_mtxModelView);
		mpCurrentMatrix = &m_mtxNULL;

		//////////////////////////
		// Setup program

		//TODO: Only needed when using Cg
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	void iRenderFunctions::SetInvertCullMode(bool abX)
	{
		if(mbInvertCullMode == abX) return;

		mbInvertCullMode = abX;
		if(mbLog) Log(" Inverting Cull Modes!\n");
		
		SetCullMode(mCurrentCullMode == eCullMode_Clockwise ? eCullMode_CounterClockwise : eCullMode_Clockwise, false);
	}
	
	//-----------------------------------------------------------------------

	void iRenderFunctions::SetFrameBuffer(iFrameBuffer *apFrameBuffer, bool abUsePosAndSize, bool abUseGlobalScissor)
	{
		if(abUsePosAndSize)
		{
			if(mbLog){
				Log(" Setting FrameBuffer %d '%s', pos: %d,%d size: %dx%d\n",	apFrameBuffer, apFrameBuffer ? apFrameBuffer->GetName().c_str() : "",
					mpCurrentRenderTarget->mvPos.x,
					mpCurrentRenderTarget->mvPos.y,
					mvRenderTargetSize.x,
					mvRenderTargetSize.y);
			}
			mpLowLevelGraphics->SetCurrentFrameBuffer(	apFrameBuffer,mpCurrentRenderTarget->mvPos,	mvRenderTargetSize);
		}
		else
		{
			if(mbLog){
				Log(" Setting FrameBuffer %d '%s', default size\n", apFrameBuffer, apFrameBuffer ? apFrameBuffer->GetName().c_str() : "");
			}
			mpLowLevelGraphics->SetCurrentFrameBuffer(apFrameBuffer);
		}

		////////////////////////////////
		//Set new frame buffer size
		cVector2l vPrevFrameBufferSize = mvCurrentFrameBufferSize;
		mvCurrentFrameBufferSize = apFrameBuffer ? apFrameBuffer->GetSize() : mvScreenSize;

		////////////////////////////////////
		//Setup the scissor rect
		if(mbUseGlobalScissorRect)
		{
			////////////////////////////////////
			//Global scissor Activated
			if(abUseGlobalScissor)
			{
				////////////////////////////////////
				//Set Active
				if(mbGlobalScissorRectActive==false)
				{
					if(mbLog)
					{
						Log("  Setting scissor active: 1\n");
						
					}
				
					mpLowLevelGraphics->SetScissorActive(true);
				}
				
				////////////////////////////////////
				//Setup rect
				if(mbGlobalScissorRectActive==false || mvCurrentFrameBufferSize != vPrevFrameBufferSize)
				{
					if(mbLog)
					{
						Log("  Setting scissor rect: %d, %d, %dx%d\n",	mvGlobalScissorRectPos.x, mvGlobalScissorRectPos.y, 
																		mvGlobalScissorRectSize.x,mvGlobalScissorRectSize.y);
					}

					mpLowLevelGraphics->SetScissorRect(mvGlobalScissorRectPos, mvGlobalScissorRectSize);
				}
				
				mbGlobalScissorRectActive = true;
			}
			////////////////////////////////////
			//Global scissor not active
			else
			{
				if(mbGlobalScissorRectActive)
				{
					if(mbLog) Log("  Setting scissor active: 0\n");
				
					mpLowLevelGraphics->SetScissorActive(false);
					mbGlobalScissorRectActive = false;
				}
			}
		}
	}
	//-----------------------------------------------------------------------

	void iRenderFunctions::ClearFrameBuffer(tClearFrameBufferFlag aFlags, bool abUsePosAndSize)
	{
		bool bScissorUsed=false;
		if(	abUsePosAndSize &&
			(mpCurrentRenderTarget->mvPos != cVector2l(0,0) || mvRenderTargetSize != mvCurrentFrameBufferSize || mbUseGlobalScissorRect) )
		{
			/////////////////////////////////
			// Global scissor rect (do no setup and do not turn off scissor at the end!)
			if(mbUseGlobalScissorRect)
			{
				if(mbLog) {
					Log(" Clearing portion of frame buffer. Pos: %d,%d Size: %dx%d\n",
						mvGlobalScissorRectPos.x, mvGlobalScissorRectPos.y,
						mvGlobalScissorRectSize.x, mvGlobalScissorRectSize.y);
				}
			}
			/////////////////////////////////
			// Render target size
			else
			{
				if(mbLog) {
					Log(" Clearing portion of frame buffer. Pos: %d,%d Size: %dx%d\n",
						mpCurrentRenderTarget->mvPos.x,	mpCurrentRenderTarget->mvPos.y,
						mvRenderTargetSize.x, mvRenderTargetSize.y);
				}

				mpLowLevelGraphics->SetScissorActive(true);
				mpLowLevelGraphics->SetScissorRect(mpCurrentRenderTarget->mvPos, mvRenderTargetSize);
				bScissorUsed = true;
			}
		}
		else
		{
			if(mbLog)
			{
				Log(" Clearing entire frame buffer\n");
			}
		}

		/////////////////////////////////
		// Clear the buffer
		mpLowLevelGraphics->ClearFrameBuffer(aFlags);

		if(bScissorUsed)
			mpLowLevelGraphics->SetScissorActive(false);
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::DrawQuad(	const cVector3f& aPos, const cVector2f& avSize, const cVector2f& avMinUV, const cVector2f& avMaxUV,
										bool abInvertY, const cColor& aColor)
	{
		if(mbLog){
			Log("   Drawing quad to: pos (%s) size (%s) uv: (%s)-(%s)\n",	aPos.ToString().c_str(), 
				avSize.ToString().c_str(),
				avMinUV.ToString().c_str(),
				avMaxUV.ToString().c_str());
		}
		if(abInvertY)
		{
			mpLowLevelGraphics->DrawQuad(	aPos, avSize,cVector2f(avMinUV.x, avMaxUV.y),
				cVector2f(avMaxUV.x, avMinUV.y),aColor);
		}
		else
		{
			mpLowLevelGraphics->DrawQuad(aPos, avSize,avMinUV,avMaxUV,aColor);
		}
	}
	
	//-----------------------------------------------------------------------

	void iRenderFunctions::DrawQuad(	const cVector3f& aPos, const cVector2f& avSize, 
										const cVector2f& avMinUV0, const cVector2f& avMaxUV0, const cVector2f& avMinUV1, const cVector2f& avMaxUV1,
										bool abInvertY0,bool abInvertY1, const cColor& aColor)
	{
		if(mbLog){
			Log("   Drawing quad to: pos (%s) size (%s) uv0: (%s)-(%s) uv1: (%s)-(%s)\n",	aPos.ToString().c_str(), 
				avSize.ToString().c_str(),
				avMinUV0.ToString().c_str(),
				avMaxUV0.ToString().c_str(),
				avMinUV1.ToString().c_str(),
				avMaxUV0.ToString().c_str());
		}
		cVector2f vFinalMinUv0 = avMinUV0;
		cVector2f vFinalMaxUv0 = avMaxUV0;
		if(abInvertY0){
			vFinalMaxUv0.y = avMinUV0.y;
			vFinalMinUv0.y = avMaxUV0.y;
		}

		cVector2f vFinalMinUv1 = avMinUV1;
		cVector2f vFinalMaxUv1 = avMaxUV1;
		if(abInvertY0){
			vFinalMaxUv1.y = avMinUV1.y;
			vFinalMinUv1.y = avMaxUV1.y;
		}

		mpLowLevelGraphics->DrawQuad(aPos, avSize,vFinalMinUv0,vFinalMaxUv0,vFinalMinUv1,vFinalMaxUv1,aColor);
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::DrawCurrent(eVertexBufferDrawType aDrawType)
	{
		if(mbLog) Log("   Drawing vertex buffer\n");

		if(mpCurrentVtxBuffer) mpCurrentVtxBuffer->Draw(aDrawType);

	}
	
	//-----------------------------------------------------------------------

	void iRenderFunctions::DrawWireFrame(iVertexBuffer *apVtxBuffer, const cColor &aColor)
	{
		///////////////////////////////////////
		//Set up variables
		int lIndexNum = apVtxBuffer->GetElementNum();
		if(lIndexNum<0) lIndexNum = apVtxBuffer->GetIndexNum();
		unsigned int* pIndexArray = apVtxBuffer->GetIndices();

		float *pVertexArray = apVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
		int lVertexStride = apVtxBuffer->GetElementNum(eVertexBufferElement_Position);

		cVector3f vTriPos[3];
	
		///////////////////////////////////////
		//Iterate through each triangle and draw it as 3 lines
		for(int tri = 0; tri < lIndexNum; tri+=3)
		{
			////////////////////////
			//Set the vector with positions of the lines
			for(int idx =0; idx < 3; idx++)
			{
				int lVtx = pIndexArray[tri + 2-idx]*lVertexStride;

				vTriPos[idx].x = pVertexArray[lVtx + 0];
				vTriPos[idx].y = pVertexArray[lVtx + 1];
				vTriPos[idx].z = pVertexArray[lVtx + 2];
			}

			////////////////////////
			//Draw the three lines
			for(int i=0; i<3; ++i)
			{
				int lNext = i==2 ? 0 : i+1;
				mpLowLevelGraphics->DrawLine(vTriPos[i],vTriPos[lNext], aColor);
			}
		}

	}
	
	//-----------------------------------------------------------------------

	iTexture* iRenderFunctions::CreateRenderTexture(const tString& asName, const cVector2l& avSize, ePixelFormat aPixelFormat,
													eTextureFilter aFilter, eTextureType aType)
	{
		iTexture *pTexture =NULL;
		pTexture = mpGraphics->CreateTexture(asName,aType,eTextureUsage_RenderTarget);
		if(pTexture->CreateFromRawData(cVector3l(avSize.x, avSize.y,0),aPixelFormat, NULL)==false)
		{
			Error("Could not create texture '%s'\n", asName.c_str());
			return pTexture;
		}
				
		pTexture->SetWrapSTR(eTextureWrap_ClampToEdge);
		pTexture->SetFilter(aFilter);

		return pTexture;
	}

	//-----------------------------------------------------------------------

	void iRenderFunctions::CopyFrameBufferToTexure(	iTexture *apTexture, const cVector2l& avPos, const cVector2l& avSize, const cVector2l& avTextureOffset,
													bool abTextureOffsetUsesRenderTargetPos)
	{
		if(mbLog)
		{
			if(abTextureOffsetUsesRenderTargetPos)
				Log(" Copying current to texture '%s' (%d). Pos: %d:%d Size: %dx%d TextureOffset: %d:%d\n",
								apTexture->GetName().c_str(), apTexture,
								avPos.x, avPos.y, avSize.x, avSize.y, 
								avTextureOffset.x+mpCurrentRenderTarget->mvPos.x, 
								avTextureOffset.y+mpCurrentRenderTarget->mvPos.y);
			else
				Log(" Copying current to texture '%s' (%d). Pos: %d:%d Size: %dx%d TextureOffset: %d:%d\n",
								apTexture->GetName().c_str(), apTexture,
								avPos.x, avPos.y, avSize.x, avSize.y, avTextureOffset.x, avTextureOffset.y);
								

		}
		if(abTextureOffsetUsesRenderTargetPos)
			mpLowLevelGraphics->CopyFrameBufferToTexure(apTexture, avPos, avSize, avTextureOffset + mpCurrentRenderTarget->mvPos);
		else
			mpLowLevelGraphics->CopyFrameBufferToTexure(apTexture, avPos, avSize, avTextureOffset);
	}

	//-----------------------------------------------------------------------

}
