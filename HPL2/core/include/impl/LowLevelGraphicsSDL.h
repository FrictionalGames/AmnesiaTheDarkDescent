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

#ifndef HPL_LOWLEVELGRAPHICS_SDL_H
#define HPL_LOWLEVELGRAPHICS_SDL_H

#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif

#ifdef WITH_CG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

#if USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

// Include these AFTER SDL
#ifdef __linux__
#include <GL/glx.h>
#endif

#include "graphics/LowLevelGraphics.h"
#include "math/MathTypes.h"


namespace hpl {

#ifdef __linux__
	typedef int (*GLXSWAPINTERVALPROC)(int interval);
#endif
	//-------------------------------------------------

	//////////////////////////////////////////
	// Global Functions
	//////////////////////////////////////////

	GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType);
	GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType);

	GLenum GetGLStencilFuncEnum(eStencilFunc aType);
	GLenum GetGLStencilOpEnum(eStencilOp aType);

	GLenum GetGLTextureParamEnum(eTextureParam aType);
	GLenum GetGLTextureOpEnum(eTextureOp aType);
	GLenum GetGLTextureFuncEnum(eTextureFunc aType);
	GLenum GetGLTextureSourceEnum(eTextureSource aType);

	GLenum GetGLBlendEnum(eBlendFunc aType);

	GLenum PixelFormatToGLFormat(ePixelFormat aFormat);
	GLenum PixelFormatToGLInternalFormat(ePixelFormat aFormat);
	GLenum GetGLCompressionFormatFromPixelFormat(ePixelFormat aFormat);

	GLenum TextureTypeToGLTarget(eTextureType aType);
    
	GLenum GetGLTextureTargetEnum(eTextureType aType);

	GLenum GetGLTextureCompareMode(eTextureCompareMode aMode);
	GLenum GetGLTextureCompareFunc(eTextureCompareFunc aFunc);

	GLenum GetGLWrapEnum(eTextureWrap aMode);
	
	//-------------------------------------------------	

	class cColorWriteDL
	{
	public:
		bool r;
		bool g;
		bool b;
		bool a;
	};

	//-------------------------------------------------

	class cLowLevelGraphicsSDL : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsSDL();
		~cLowLevelGraphicsSDL();

		/////////////////////////////////////////////////////
		/////////////// GENERAL SETUP ///////////////////////
		/////////////////////////////////////////////////////

		bool Init(	int alWidth, int alHeight, int alDisplay, int alBpp, int abFullscreen, int alMultisampling,
					eGpuProgramFormat aGpuProgramFormat,const tString& asWindowCaption,
					const cVector2l &avWindowPos);

		eGpuProgramFormat GetGpuProgramFormat(){ return mGpuProgramFormat;}

		int GetCaps(eGraphicCaps aType);

		void ShowCursor(bool abX);

        void SetWindowGrab(bool abX);

        void SetRelativeMouse(bool abX);

        void SetWindowCaption(const tString &asName);

        bool GetWindowMouseFocus();
        
        bool GetWindowInputFocus();
        
        bool GetWindowIsVisible();

		bool GetFullscreenModeActive() { return mbFullscreen; }

		void SetVsyncActive(bool abX, bool abAdaptive);

		void SetMultisamplingActive(bool abX);

		void SetGammaCorrection(float afX);
		float GetGammaCorrection();

		int GetMultisampling(){ return mlMultisampling;}

		cVector2f GetScreenSizeFloat();
		const cVector2l& GetScreenSizeInt();
		
		/////////////////////////////////////////////////////
		/////////////// DATA CREATION //////////////////////
		/////////////////////////////////////////////////////

		iFontData* CreateFontData(const tString &asName);

		iTexture* CreateTexture(const tString &asName, eTextureType aType, eTextureUsage aUsage);

		iVertexBuffer* CreateVertexBuffer(	eVertexBufferType aType,
											eVertexBufferDrawType aDrawType,
											eVertexBufferUsageType aUsageType,
											int alReserveVtxSize=0,int alReserveIdxSize=0);

		iGpuProgram* CreateGpuProgram(const tString& asName);
		iGpuShader* CreateGpuShader(const tString& asName, eGpuShaderType aType);

		iFrameBuffer* CreateFrameBuffer(const tString& asName);
		iDepthStencilBuffer* CreateDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits);

		iOcclusionQuery* CreateOcclusionQuery();
		
		/////////////////////////////////////////////////////
		/////////// FRAME BUFFER OPERATIONS ///////
		/////////////////////////////////////////////////////
	
		void ClearFrameBuffer(tClearFrameBufferFlag aFlags);

		void SetClearColor(const cColor& aCol);
		void SetClearDepth(float afDepth);
		void SetClearStencil(int alVal);
		
		void CopyFrameBufferToTexure(	iTexture* apTex, const cVector2l &avPos,
									const cVector2l &avSize, const cVector2l &avTexOffset=0);
		cBitmap* CopyFrameBufferToBitmap(const cVector2l &avScreenPos=0, const cVector2l &avScreenSize=-1);
		
		void WaitAndFinishRendering();
		void FlushRendering();
		void SwapBuffers();
		
		void SetCurrentFrameBuffer(iFrameBuffer* apFrameBuffer, const cVector2l &avPos = 0, const cVector2l& avSize = -1);
		iFrameBuffer* GetCurrentFrameBuffer() { return mpFrameBuffer; }

		void SetFrameBufferDrawTargets(int *apTargets, int alNumOfTargets);
		
		/////////////////////////////////////////////////////
		/////////// RENDER STATE ////////////////////////////
		/////////////////////////////////////////////////////

		void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA);
		void SetDepthWriteActive(bool abX);

		void SetCullActive(bool abX);
		void SetCullMode(eCullMode aMode);

		void SetDepthTestActive(bool abX);
		void SetDepthTestFunc(eDepthTestFunc aFunc);

		void SetAlphaTestActive(bool abX);
		void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef);

		void SetStencilActive(bool abX);
		void SetStencilWriteMask(unsigned int alMask);
		void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
						eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp);
		void SetStencilTwoSide(	eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
								int alRef, unsigned int aMask,
								eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
								eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp);
		
		void SetScissorActive(bool abX);
		void SetScissorRect(const cVector2l& avPos, const cVector2l& avSize);

		void SetClipPlane(int alIdx, const cPlanef& aPlane);
		cPlanef GetClipPlane(int alIdx);
		void SetClipPlaneActive(int alIdx, bool abX);

		void SetColor(const cColor &aColor);

		void SetBlendActive(bool abX);
		void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor);
		void SetBlendFuncSeparate(	eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
									eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha);

		void SetPolygonOffsetActive(bool abX);
		void SetPolygonOffset(float afBias,float afSlopeScaleBias);

		/////////////////////////////////////////////////////
		/////////// MATRIX //////////////////////////////////
		/////////////////////////////////////////////////////

		void PushMatrix(eMatrix aMtxType);
		void PopMatrix(eMatrix aMtxType);
		void SetIdentityMatrix(eMatrix aMtxType);

		void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA);

		void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax);
		void SetOrthoProjection(const cVector3f& avMin, const cVector3f& avMax);

		/////////////////////////////////////////////////////
		/////////// TEXTURE OPERATIONS ///////////////////////
		/////////////////////////////////////////////////////

		void SetTexture(unsigned int alUnit,iTexture* apTex);
		void SetActiveTextureUnit(unsigned int alUnit);
		void SetTextureEnv(eTextureParam aParam, int alVal);
		void SetTextureConstantColor(const cColor &aColor);

		
		/////////////////////////////////////////////////////
		/////////// DRAWING ///////////////////////////////
		/////////////////////////////////////////////////////

		void DrawTriangle(tVertexVec& avVtx);

		void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize, const cColor& aColor=cColor(1,1));
		void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize,
						const cVector2f &avMinTexCoord,const cVector2f &avMaxTexCoord,
						const cColor& aColor=cColor(1,1));
		void DrawQuad(	const cVector3f &avPos,const cVector2f &avSize,
						const cVector2f &avMinTexCoord0,const cVector2f &avMaxTexCoord0,
						const cVector2f &avMinTexCoord1,const cVector2f &avMaxTexCoord1,
						const cColor& aColor=cColor(1,1));
		
		void DrawQuad(const tVertexVec &avVtx);
		void DrawQuad(const tVertexVec &avVtx, const cColor aCol);
		void DrawQuad(const tVertexVec &avVtx,const float afZ);
		void DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol);
		void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs);

		void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol);
		void DrawLine(const cVector3f& avBegin, const cColor& aBeginCol, const cVector3f& avEnd, const cColor& aEndCol);

		void DrawBoxMinMax(const cVector3f& avMin, const cVector3f& avMax, cColor aCol);
		void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol);
		void DrawSphere(const cVector3f& avPos, float afRadius, cColor aColX, cColor aColY, cColor aColZ);

		void DrawLineQuad(const cRect2f& aRect, float afZ, cColor aCol);
		void DrawLineQuad(const cVector3f &avPos,const cVector2f &avSize, cColor aCol);
				
		/////////////////////////////////////////////////////
		/////////// VERTEX BATCHING /////////////////////////
		/////////////////////////////////////////////////////

		void AddVertexToBatch(const cVertex *apVtx);
		void AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform);
		void AddVertexToBatch(const cVertex *apVtx, const cMatrixf* aMtx);

		void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
										const cColor* apCol,const float& mfW, const float& mfH);

		void AddVertexToBatch_Raw(	const cVector3f& avPos, const cColor &aColor,
									const cVector3f& avTex);


		void AddTexCoordToBatch(unsigned int alUnit,const cVector3f *apCoord);
		void SetBatchTextureUnitActive(unsigned int alUnit,bool abActive);

		void AddIndexToBatch(int alIndex);

		void FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true);
		void FlushQuadBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true);
		void ClearBatch();
		
		/////////////////////////////////////////////////////
		/////////// IMPLEMENTION SPECIFICS /////////////////
		/////////////////////////////////////////////////////

#ifdef WITH_CG
		CGcontext GetGC_Context(){ return mCG_Context;}
#endif
		void SetupGL();

	private:
        cVector2l mvScreenSize;
        int mlDisplay;
		int mlMultisampling;
		int mlBpp;
		bool mbFullscreen;
		eGpuProgramFormat mGpuProgramFormat;

		//////////////////////////////////////
		//Windows stuff
		#if defined(WIN32) && !SDL_VERSION_ATLEAST(2,0,0)
			HGLRC mGLContext;
			HDC   mDeviceContext;
			HINSTANCE mhKeyTrapper;
		#endif

		bool mbInitHasBeenRun;

		//////////////////////////////////////
		//Render state settings
		cColorWriteDL mColorWrite;
		bool mbDepthWrite;
		
		bool mbCullActive;
		eCullMode mCullMode;
		
		bool mbDepthTestActive;
		eDepthTestFunc mDepthTestFunc;

		bool mbAlphaTestActive;
		eAlphaTestFunc mAlphaTestFunc;
		float mfAlphaTestFuncRef;
		
		bool mbScissorActive;
		cVector2l mvScissorPos;
		cVector2l mvScissorSize;
		
		bool mbBlendActive;

		iFrameBuffer* mpFrameBuffer;
		cVector2l mvFrameBufferPos;
		cVector2l mvFrameBufferSize;
		cVector2l mvFrameBufferTotalSize;

		
		//////////////////////////////////////
		//Gamma
		Uint16 mvStartGammaArray[3][256];
		float mfGammaCorrection;

		//////////////////////////////////////
		//Clipping
		cPlanef mvClipPlanes[kMaxClipPlanes];


		//////////////////////////////////////
		//SDL Variables
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_Window *mpScreen;
        SDL_GLContext mGLContext;
#else
		SDL_Surface *mpScreen;
#endif
        bool mbGrab;
		
		//////////////////////////////////////
		//Vertex Array variables
		//The vertex arrays used:
		float* mpVertexArray;
		unsigned int mlVertexCount;
		unsigned int* mpIndexArray;
		unsigned int mlIndexCount;

		unsigned int mlBatchStride;

		float *mpTexCoordArray[kMaxTextureUnits];
		bool mbTexCoordArrayActive[kMaxTextureUnits];
		unsigned int mlTexCoordArrayCount[kMaxTextureUnits];

		unsigned int mlBatchArraySize;

		//////////////////////////////////////
		//Texture
		GLenum mvCurrentTextureTarget[kMaxTextureUnits];
		
#ifdef WITH_CG
		//////////////////////////////////////
		//CG Compiler Variables
		CGcontext mCG_Context;
#endif

		//////////////////////////////////////
		//Multisample
		void CheckMultisampleCaps();

		//////////////////////////////////////
		//Double sided stencil
		bool mbDoubleSidedStencilIsSet;

#ifdef WITH_CG
		//////////////////////////////////////
		//CG Helper
		void InitCG();
		void ExitCG();
#endif
		
		//////////////////////////////////////
		//Matrix helper
		void SetMatrixMode(eMatrix mType);

		//////////////////////////////////////
		//Batch helper
		void SetUpBatchArrays();
	
		//////////////////////////////////////
		//Vtx helper
		void SetVtxBatchStates(tVtxBatchFlag aFlags);
	};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
