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

#include "impl/PBuffer.h"
#include "impl/LowLevelGraphicsSDL.h"
#include "system/LowLevelSystem.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * All pBuffers must be created before any of them are set as a render target!
	 *
	 */
	cPBuffer::cPBuffer(iLowLevelGraphics* apLowLevelGraphics,bool abShareObjects,bool abUseMipMaps, bool abUseDepth,bool abUseStencil)
	{
		#ifdef WIN32
		mDeviceContext = 0;
		mGLContext =0;

		mbShareObjects = abShareObjects;
		mpLowLevelGraphics = apLowLevelGraphics;

		//Now we chose the pixel format attributes:
		//Format:
		mvAttribFormat.push_back(WGL_DRAW_TO_PBUFFER_ARB);
		mvAttribFormat.push_back(true);
		
		mvAttribFormat.push_back(WGL_BIND_TO_TEXTURE_RGBA_ARB);
		mvAttribFormat.push_back(true);
		//Use this if the texture is a rectangle: (nVidia only!)
		//mvAttribFormat.push_back(WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV);
		//mvAttribFormat.push_back(true);

		mvAttribFormat.push_back(WGL_DEPTH_BITS_ARB);
		mvAttribFormat.push_back(24);
		mvAttribFormat.push_back(WGL_STENCIL_BITS_ARB);
		mvAttribFormat.push_back(8);
		mvAttribFormat.push_back(WGL_COLOR_BITS_ARB);
		mvAttribFormat.push_back(32);
		
		//Buffer:
		mvAttribBuffer.push_back(WGL_PBUFFER_LARGEST_ARB);
		mvAttribBuffer.push_back(true);
		mvAttribBuffer.push_back(WGL_TEXTURE_FORMAT_ARB);
		mvAttribBuffer.push_back(WGL_TEXTURE_RGBA_ARB);
		
		mvAttribBuffer.push_back(WGL_TEXTURE_TARGET_ARB);
		mvAttribBuffer.push_back(WGL_TEXTURE_2D_ARB);
		//Use this if the texture is a rectangle: (nVidia only!)
		//mvAttribBuffer.push_back(WGL_TEXTURE_RECTANGLE_NV);
		
		
		mvAttribBuffer.push_back(WGL_MIPMAP_TEXTURE_ARB);
		mvAttribBuffer.push_back(false);

		//Use this if you have mipmaps:
		/*mvAttribBuffer.push_back(WGL_MIPMAP_LEVEL_ARB);
		mvAttribBuffer.push_back(0);*/

		//Set end of attributes vectors
		mvAttribFormat.push_back(0);
		mvAttribBuffer.push_back(0);
		#elif defined(__linux__)
		#endif
	}

	cPBuffer::~cPBuffer()
	{
		#ifdef WIN32
		//This should not be deleted if it is shared.
		wglDeleteContext(mGLContext);				
		wglReleasePbufferDCARB(mPBuffer, mDeviceContext);
		wglDestroyPbufferARB(mPBuffer);
		#elif defined(__linux__)
		#endif
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cPBuffer::Init(unsigned int alWidth,unsigned int alHeight, cColor aCol)
	{
		#ifdef WIN32
		unsigned int lFormatNum=0;
		int lFormat=0;

		mlWidth = alWidth;
		mlHeight = alHeight;

		HDC CurrentHdc = wglGetCurrentDC();
		HGLRC CurrentGGlRc = wglGetCurrentContext();

		//Set the pixel format:
		wglChoosePixelFormatARB(CurrentHdc, &mvAttribFormat[0], NULL, 1, &lFormat, &lFormatNum);
		if(lFormatNum==0){
			Error("Couldn't find any pixel format!\n");
			return false;
		}
				
		//Create the buffer
		mPBuffer = wglCreatePbufferARB(CurrentHdc, lFormat, mlWidth, mlHeight, &mvAttribBuffer[0]);
		if (!mPBuffer)
		{
			int err = GetLastError();
			Error("pbuffer creation error!\n");
			return false;
		}

		//Get the Device Context
		mDeviceContext = wglGetPbufferDCARB(mPBuffer);
		if(!mDeviceContext){
			Error("Some DC error!\n");
		}

		//Get the GL Context
		mGLContext = wglCreateContext(mDeviceContext);
		if(!mGLContext){
			Error("Some GLRC error!\n");
		}

		if(mbShareObjects){
			if(!wglShareLists(CurrentGGlRc, mGLContext))
			{
				Error("Error sharing lists.\n");
			}
		}
		
		//Check the real dimensions of the PBuffer
		wglQueryPbufferARB(mPBuffer, WGL_PBUFFER_WIDTH_ARB, &mlWidth);
		wglQueryPbufferARB(mPBuffer, WGL_PBUFFER_HEIGHT_ARB, &mlHeight);
			
		//Init some GL stuff with the Buffer.
		#ifdef WIN32
		HDC OldHDC = wglGetCurrentDC();
		HGLRC OldGLRC = wglGetCurrentContext();
		#endif
		MakeCurrentContext();
				
		if(mbShareObjects)
		{
			cLowLevelGraphicsSDL* pSDLGfx = static_cast<cLowLevelGraphicsSDL*>(mpLowLevelGraphics);
			pSDLGfx->SetupGL();
		}
		
		mpLowLevelGraphics->SetClearColor(aCol);
		mpLowLevelGraphics->ClearFrameBuffer(eClearFrameBufferFlag_Color);

		wglMakeCurrent(OldHDC,OldGLRC);
		#elif defined(__linux__)
		return false;
		#endif
	
		return true;
	}

	//-----------------------------------------------------------------------

	int cPBuffer::MakeCurrentContext()
	{
		#ifdef WIN32
		return wglMakeCurrent(mDeviceContext, mGLContext);
		#else
		return 0;
		//return glXMakeCurrent(dpy, gPBuffer, glCtx);
		#endif
	}
	
	//-----------------------------------------------------------------------
	
	void cPBuffer::Bind()
	{
		#ifdef WIN32
		if(!wglBindTexImageARB(mPBuffer, WGL_FRONT_LEFT_ARB))
		{
			Error("Error Binding pbuffer...\n");
		}
		#elif defined(__linux__)
		#endif		
	}
	
	//-----------------------------------------------------------------------
	
	void cPBuffer::UnBind()
	{
		#ifdef WIN32
		if(wglReleaseTexImageARB(mPBuffer, WGL_FRONT_LEFT_ARB)==false)
		{
			Error("Error UnBinding pbuffer...\n");
		}
		#elif defined(__linux__)
		#endif
	}


	//-----------------------------------------------------------------------
}
