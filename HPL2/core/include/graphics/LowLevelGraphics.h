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

#ifndef HPL_LOWLEVELGRAPHICS_H
#define HPL_LOWLEVELGRAPHICS_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {
	
	class iFontData;
	class iOcclusionQuery;
	class iGpuShader;
	class iGpuProgram;
	class iTexture;
	class iVertexBuffer;
	class cBitmap;
	class iFrameBuffer;
	class iDepthStencilBuffer;
	class iMutex;

	//----------------------------------------
		
	class iLowLevelGraphics
	{
	public:
		virtual ~iLowLevelGraphics(){}
		

		/////////////////////////////////////////////////////
		/////////////// GENERAL SETUP ///////////////////////
		/////////////////////////////////////////////////////

		virtual bool Init(	int alWidth, int alHeight, int alDisplay, int alBpp, int abFullscreen, int alMultisampling,
							eGpuProgramFormat aGpuProgramFormat, const tString& asWindowCaption,
							const cVector2l &avWindowPos)=0;

		virtual eGpuProgramFormat GetGpuProgramFormat()=0;
		
		/**
		 * Get the capabilities of the graphics. Th return value depends on the capability
		 * \return 
		 */
		virtual int GetCaps(eGraphicCaps aType)=0;
		
		/**
		 * Show the cursor or not. Default is false
		 */
		virtual void ShowCursor(bool abX)=0;

		virtual void SetWindowGrab(bool abX)=0;

		virtual void SetRelativeMouse(bool abX)=0;

        virtual void SetWindowCaption(const tString& asName)=0;

        virtual bool GetWindowMouseFocus()=0;

        virtual bool GetWindowInputFocus()=0;

        virtual bool GetWindowIsVisible()=0;

		virtual int GetMultisampling()=0;
		
		/**
		 * Get Size of screen
		 */
		virtual cVector2f GetScreenSizeFloat()=0;
		virtual const cVector2l& GetScreenSizeInt()=0;

		/*
		* Get fullscreen mode
		*/
		virtual bool GetFullscreenModeActive() = 0;
		
		/**
		 * Set if vsync should be active
		 */
		virtual void SetVsyncActive(bool abX, bool abAdaptive = false)=0;

		virtual void SetMultisamplingActive(bool abX)=0;

		virtual void SetGammaCorrection(float afX)=0;
		virtual float GetGammaCorrection()=0;

		/////////////////////////////////////////////////////
		/////////////// DATA CREATION ///////////////////////
		/////////////////////////////////////////////////////

		virtual iFontData* CreateFontData(const tString &asName)=0;

		virtual iGpuProgram* CreateGpuProgram(const tString& asName)=0;
		virtual iGpuShader* CreateGpuShader(const tString& asName, eGpuShaderType aType)=0;
		
		virtual iTexture* CreateTexture(const tString &asName,eTextureType aType,  eTextureUsage aUsage)=0;
		
		virtual iVertexBuffer* CreateVertexBuffer(	eVertexBufferType aType,
													eVertexBufferDrawType aDrawType,
													eVertexBufferUsageType aUsageType,
													int alReserveVtxSize=0,int alReserveIdxSize=0)=0;

		virtual iFrameBuffer* CreateFrameBuffer(const tString& asName)=0;
		virtual iDepthStencilBuffer* CreateDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits)=0;

		virtual iOcclusionQuery* CreateOcclusionQuery()=0;
		
		/////////////////////////////////////////////////////
		/////////// FRAME BUFFER OPERATIONS ///////
		/////////////////////////////////////////////////////

		virtual void ClearFrameBuffer(tClearFrameBufferFlag aFlags)=0;

		virtual void SetClearColor(const cColor& aCol)=0;
		virtual void SetClearDepth(float afDepth)=0;
		virtual void SetClearStencil(int alVal)=0;
		
		virtual void FlushRendering()=0;
		virtual void WaitAndFinishRendering()=0;
		virtual void SwapBuffers()=0;

		/**
		* Copies the current frame buffer to a texture.
		* \param apTex The texture the context is copied to.
		* \param &avPos The position in conext
		* \param &avSize The size of the screen.
		* \param &avTexOffset The position on the texture.
		*/
		virtual void CopyFrameBufferToTexure(	iTexture* apTex, const cVector2l &avPos, 
												const cVector2l &avSize, const cVector2l &avTexOffset=0)=0;

		/**
		 * Copies a portion of the screen to the full size of a bitmap.
		 * \param &avScreenPos The position on the screen
		 * \param &avScreenSize The size of the portion of the screen. -1 = use full screen size.
		 * \return a bitmap that contains the data. The bitmap is created by the method and must be deleted by the user.
		 */
		virtual cBitmap* CopyFrameBufferToBitmap(const cVector2l &avScreenPos=0, const cVector2l &avScreenSize=-1)=0;

		/**
		* All further drawing operations are rendered to this texture.
		* \param pTex Texture to render to. NULL = screen (frame buffer)
		*/
		virtual void SetCurrentFrameBuffer(iFrameBuffer* apFrameBuffer, const cVector2l &avPos = 0, const cVector2l& avSize = -1)=0;
		
		/**
		* Returns current FrameBuffer
		* 
		*/
		virtual iFrameBuffer* GetCurrentFrameBuffer()=0;

		/**
		 * Permanently sets a different setting of draw targets to current frame buffer. 
		 * \param *apTargets a vector of integers, each corresponding to a draw buffer in the current 
		 * \param alNumOfTargets 
		 */
		virtual void SetFrameBufferDrawTargets(int *apTargets, int alNumOfTargets)=0;
		
		/////////////////////////////////////////////////////
		/////////// RENDER STATE ////////////////////////////
		/////////////////////////////////////////////////////

		virtual void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)=0;
		virtual void SetDepthWriteActive(bool abX)=0;

		virtual void SetCullActive(bool abX)=0;
		virtual void SetCullMode(eCullMode aMode)=0;

		virtual void SetDepthTestActive(bool abX)=0;
		virtual void SetDepthTestFunc(eDepthTestFunc aFunc)=0;

		virtual void SetAlphaTestActive(bool abX)=0;
		virtual void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)=0;

		virtual void SetStencilActive(bool abX)=0;
		virtual void SetStencilWriteMask(unsigned int alMask)=0;
		virtual void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
								eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)=0;
		virtual void SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
										int alRef, unsigned int aMask,
										eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
										eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)=0;
		
		virtual void SetScissorActive(bool abX)=0;
		virtual void SetScissorRect(const cVector2l& avPos, const cVector2l& avSize)=0;

		virtual void SetClipPlane(int alIdx, const cPlanef& aPlane)=0;
		virtual cPlanef GetClipPlane(int alIdx)=0;
		virtual void SetClipPlaneActive(int alIdx, bool abX)=0;

		virtual void SetColor(const cColor &aColor)=0;

		virtual void SetBlendActive(bool abX)=0;
		virtual void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)=0;
		virtual void SetBlendFuncSeparate(	eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
											eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)=0;

		virtual void SetPolygonOffsetActive(bool abX)=0;
		virtual void SetPolygonOffset(float afBias, float afSlopeScaleBias)=0;

		
		/////////////////////////////////////////////////////
		/////////// MATRIX //////////////////////////////////
		/////////////////////////////////////////////////////

		virtual void PushMatrix(eMatrix aMtxType)=0;
		virtual void PopMatrix(eMatrix aMtxType)=0;
		virtual void SetIdentityMatrix(eMatrix aMtxType)=0;

		virtual void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)=0;
		
		virtual void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)=0;
		virtual void SetOrthoProjection(const cVector3f& avMin, const cVector3f& avMax)=0;
				
		/////////////////////////////////////////////////////
		/////////// TEXTURE OPERATIONS ///////////////////////
		/////////////////////////////////////////////////////

		virtual void SetTexture(unsigned int alUnit,iTexture* apTex)=0;
		virtual void SetActiveTextureUnit(unsigned int alUnit)=0;
		virtual void SetTextureEnv(eTextureParam aParam, int alVal)=0;
		virtual void SetTextureConstantColor(const cColor &aColor)=0;


		/////////////////////////////////////////////////////
		/////////// DRAWING ///////////////////////////////
		/////////////////////////////////////////////////////

		virtual void DrawTriangle(tVertexVec& avVtx)=0;
			
		virtual void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize, const cColor& aColor=cColor(1,1))=0;
		virtual void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize,
								const cVector2f &avMinTexCoord,const cVector2f &avMaxTexCoord,
								const cColor& aColor=cColor(1,1))=0;
		virtual void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize,
								const cVector2f &avMinTexCoord0,const cVector2f &avMaxTexCoord0,
								const cVector2f &avMinTexCoord1,const cVector2f &avMaxTexCoord1,
								const cColor& aColor=cColor(1,1))=0;

		virtual void DrawQuad(const tVertexVec &avVtx)=0;
		virtual void DrawQuad(const tVertexVec &avVtx, const cColor aCol)=0;
		virtual void DrawQuad(const tVertexVec &avVtx,const float afZ)=0;
		virtual void DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol)=0;
		virtual void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs)=0;

		virtual void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol)=0;
		virtual void DrawLine(const cVector3f& avBegin, const cColor& aBeginCol, const cVector3f& avEnd, const cColor& aEndCol)=0;
		virtual void DrawBoxMinMax(const cVector3f& avMin, const cVector3f& avMax, cColor aCol)=0;
		virtual void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol)=0;
		virtual void DrawSphere(const cVector3f& avPos, float afRadius, cColor aColX, cColor aColY, cColor aColZ)=0;

		virtual void DrawLineQuad(const cRect2f& aRect, float afZ, cColor aCol)=0;
		virtual void DrawLineQuad(const cVector3f &avPos,const cVector2f &avSize, cColor aCol)=0;
		
		/////////////////////////////////////////////////////
		/////////// VERTEX BATCHING /////////////////////////
		/////////////////////////////////////////////////////
		
		virtual void AddVertexToBatch(const cVertex *apVtx)=0;
		virtual void AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform)=0;
		virtual void AddVertexToBatch(const cVertex *apVtx, const cMatrixf* aMtx)=0;

		virtual void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
									const cColor* apCol,const float& mfW, const float& mfH)=0;
		
		virtual void AddVertexToBatch_Raw(	const cVector3f& avPos, const cColor &aColor,
											const cVector3f& avTex)=0;
											

		virtual void AddIndexToBatch(int alIndex)=0;

		virtual void AddTexCoordToBatch(unsigned int alUnit,const cVector3f *apCoord)=0;
		virtual void SetBatchTextureUnitActive(unsigned int alUnit,bool abActive)=0;
		
		virtual void FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true)=0;
		virtual void FlushQuadBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true)=0;
		virtual void ClearBatch()=0;

		/////////////////////////////////////////////////////
		/////////// STATIC VARIABLES /////////////////////////
		/////////////////////////////////////////////////////

		static void SetForceShaderModel3And4Off(bool abX){ mbForceShaderModel3And4Off = abX;}
		static bool GetForceShaderModel3And4Off(){	return mbForceShaderModel3And4Off;}
	
	protected:
		static bool mbForceShaderModel3And4Off;
	};
};
#endif // HPL_LOWLEVELGRAPHICS_H
