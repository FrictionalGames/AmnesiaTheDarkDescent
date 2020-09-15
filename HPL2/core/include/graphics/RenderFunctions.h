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

#ifndef HPL_RENDER_FUNCTIONS_H
#define HPL_RENDER_FUNCTIONS_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

namespace hpl {

	//---------------------------------------------

	class iLowLevelGraphics;
	class iVertexBuffer;
	class iTexture;
	class iGpuProgram;
	class cFrustum;
	class cMaterial;
	class iMaterialType;
	class cGraphics;

	//---------------------------------------------
	
	class iRenderFunctions
	{
	public:
		virtual ~iRenderFunctions() { }

		/*
		 * This function must be set before the render functions can be used!
	     */
		void SetupRenderFunctions(iLowLevelGraphics *apLowLevelGraphics);
		
		
		/**
		 * This must be called every frame before any render function is called
		 */
		void InitAndResetRenderFunctions(	cFrustum *apFrustum, cRenderTarget *apRenderTarget, bool abLog,
											bool abUseGlobalScissorRect=false, 
											const cVector2l& avGlobalScissorRectPos=0, const cVector2l& avGlobalScissorRectSize=0);
		void ExitAndCleanUpRenderFunctions();

		void SetFlatProjection(const cVector2f &avSize=1,float afMin=-100,float afMax=100);
		void SetFlatProjectionMinMax(const cVector3f &avMin,const cVector3f &avMax);
		void SetNormalFrustumProjection();
		void SetFrustumProjection(cFrustum *apFrustum);
		void SetProjectionMatrix(const cMatrixf *apProjMatrix);
				
		bool SetDepthTest(bool abX);
		bool SetDepthWrite(bool abX);
		bool SetDepthTestFunc(eDepthTestFunc aFunc);
		bool SetCullActive(bool abX);
		bool SetCullMode(eCullMode aMode, bool abCheckIfInverted=true);
		bool SetStencilActive(bool abX);
		void SetStencilWriteMask(unsigned int alMask);
		void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
						eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp);
		void SetStencilTwoSide(	eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
								int alRef, unsigned int aMask,
								eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
								eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp);
		bool SetScissorActive(bool abX);
		/**
		 * When abAutoEnabling is true, it will also set to false if size = render target and pos=0
		 */
		bool SetScissorRect(const cVector2l& avPos, const cVector2l& avSize, bool abAutoEnabling);
		bool SetScissorRect(const cRect2l& aClipRect, bool abAutoEnabling);
		bool SetChannelMode(eMaterialChannelMode aMode);
		bool SetAlphaMode(eMaterialAlphaMode aMode);
		bool SetAlphaLimit(float afLimit);
		bool SetBlendMode(eMaterialBlendMode aMode);
		bool SetProgram(iGpuProgram *apProgram);
		void SetTexture(int alUnit, iTexture *apTexture);
		void SetTextureRange(iTexture *apTexture, int alFirstUnit, int alLastUnit = kMaxTextureUnits-1);
		void SetVertexBuffer(iVertexBuffer *apVtxBuffer);
		void SetMatrix(cMatrixf *apMatrix);
		void SetModelViewMatrix(const cMatrixf& a_mtxModelView);

		void SetInvertCullMode(bool abX);

		void SetFrameBuffer(iFrameBuffer *apFrameBuffer, bool abUsePosAndSize=false, bool abUseGlobalScissor=true);
		void ClearFrameBuffer(tClearFrameBufferFlag aFlags, bool abUsePosAndSize);

		void DrawQuad(	const cVector3f& aPos, const cVector2f& avSize, const cVector2f& avMinUV=0, const cVector2f& avMaxUV=1,
						bool abInvertY=false, const cColor& aColor=cColor(1,1) );

		void DrawQuad(	const cVector3f& aPos, const cVector2f& avSize, 
						const cVector2f& avMinUV0, const cVector2f& avMaxUV0,
						const cVector2f& avMinUV1, const cVector2f& avMaxUV1,
						bool abInvertY0=false,bool abInvertY1=false, const cColor& aColor=cColor(1,1) );


		void DrawCurrent(eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);

		void DrawWireFrame(iVertexBuffer *apVtxBuffer, const cColor &aColor);
		
		iTexture* CreateRenderTexture(	const tString& asName, const cVector2l& avSize, ePixelFormat aPixelFormat,
										eTextureFilter aFilter = eTextureFilter_Bilinear, eTextureType aType= eTextureType_Rect);

		void CopyFrameBufferToTexure(	iTexture *apTexture, const cVector2l& avPos, const cVector2l& avSize, const cVector2l& avTextureOffset,
										bool abTextureOffsetUsesRenderTargetPos);

		cRenderTarget* GetCurrentRenderTarget(){return mpCurrentRenderTarget;}
		const cVector2l& GetCurrentFrameBufferSize(){ return mvCurrentFrameBufferSize;}
		const cVector2l& GetRenderTargetSize(){ return mvRenderTargetSize;}

	protected:
		cGraphics *mpGraphics;
		iLowLevelGraphics *mpLowLevelGraphics;

		cFrustum *mpCurrentFrustum;
		cRenderTarget *mpCurrentRenderTarget;
		const cMatrixf *mpCurrentProjectionMatrix;

		cVector2l mvRenderTargetSize;		//Use this when ever some calculations involving the size of rendertarget is involved!
		cVector2l mvCurrentFrameBufferSize;	//This shall NOT be used in calculations and is simply as a helper

		cVector2l mvScreenSize;
		cVector2f mvScreenSizeFloat;

		bool mbLog;
		bool mbInvertCullMode;

		bool mbUseGlobalScissorRect;
		bool mbGlobalScissorRectActive;
		cVector2l mvGlobalScissorRectPos;
		cVector2l mvGlobalScissorRectSize;

		bool mbCurrentDepthTest;
		bool mbCurrentDepthWrite;
		bool mbCurrentStencilActive;
		cVector2l mvCurrentScissorRectPos;
		cVector2l mvCurrentScissorRectSize;
		bool mbCurrentScissorActive;
		eDepthTestFunc mCurrentDepthTestFunc;
		bool mbCurrentCullActive;
		eCullMode mCurrentCullMode;
		eMaterialChannelMode mCurrentChannelMode;
		eMaterialAlphaMode mCurrentAlphaMode;
		float mfCurrentAlphaLimit;
		eMaterialBlendMode mCurrentBlendMode;
		iTexture *mvCurrentTexture[kMaxTextureUnits];
		iGpuProgram* mpCurrentProgram;
		iVertexBuffer *mpCurrentVtxBuffer;
		cMatrixf *mpCurrentMatrix;

		cMaterial *mpCurrentMaterial;
		iMaterialType *mpCurrentMaterialType;
		
		cMatrixf m_mtxNULL; //used to reset current matrix pointer.
	};
	
	//---------------------------------------------

};
#endif // HPL_RENDER_FUNCTIONS_H
