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

#ifdef WIN32
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
//#pragma comment(lib, "GLaux.lib")
#pragma comment(lib, "Cg.lib")
#pragma comment(lib, "CgGL.lib")
//#pragma comment(lib, "SDL_ttf.lib")
#pragma comment(lib, "TaskKeyHook.lib")
#endif

#include <assert.h>
#include <stdlib.h>

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/Mutex.h"

#include "impl/LowLevelGraphicsSDL.h"
#include "impl/SDLFontData.h"
#include "impl/SDLTexture.h"
//#include "impl/CGShader.h"
//#include "impl/CGProgram.h"
#include "impl/GLSLShader.h"
#include "impl/GLSLProgram.h"
#include "impl/VertexBufferOGL_Array.h"
#include "impl/VertexBufferOGL_VBO.h"
#include "impl/FrameBufferGL.h"
#include "impl/OcclusionQueryOGL.h"

#include "graphics/Bitmap.h"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

#if USE_SDL2
#include "SDL2/SDL_syswm.h"
#else
#include "SDL/SDL_syswm.h"
#endif

#ifdef WIN32
#include "impl/TaskKeyHook.h"
#endif

#ifndef WIN32
	#if defined __ppc__ || defined(__LP64__)
		#define CALLBACK
	#else
		#define CALLBACK __attribute__ ((__stdcall__))
	#endif
#endif

namespace hpl {

	#ifdef HPL_OGL_THREADSAFE
		iMutex *gpLowlevelGfxMutex=NULL;

		cLowlevelGfxMutex::cLowlevelGfxMutex(){ if(gpLowlevelGfxMutex) gpLowlevelGfxMutex->Lock(); }
		cLowlevelGfxMutex::~cLowlevelGfxMutex(){ if(gpLowlevelGfxMutex) gpLowlevelGfxMutex->Unlock(); }
	#endif

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::cLowLevelGraphicsSDL()
	{
		mlBatchArraySize = 20000;
		mlVertexCount = 0;
		mlIndexCount =0;
		mlMultisampling =0;
        mpScreen = 0;
        mbGrab = false;

		mbDoubleSidedStencilIsSet = false;

#if defined(WIN32) && !SDL_VERSION_ATLEAST(2,0,0)
		mhKeyTrapper = NULL;
#endif

		mpFrameBuffer = NULL;

		for(int i=0;i<kMaxTextureUnits;i++)
			mvCurrentTextureTarget[i] = 0;

		//Create the batch arrays:
		mlBatchStride = 13;
		//3 Pos floats, 4 color floats, 3 Tex coord floats .
		mpVertexArray = (float*)hplMalloc(sizeof(float) * mlBatchStride * mlBatchArraySize); 
		mpIndexArray = (unsigned int*)hplMalloc(sizeof(unsigned int) * mlBatchArraySize); //Index is one int.

		for(int i=0;i<kMaxTextureUnits;i++)
		{
			mpTexCoordArray[i] = (float*)hplMalloc(sizeof(float) * 3 * mlBatchArraySize);
			mbTexCoordArrayActive[i] = false;
			mlTexCoordArrayCount[i]=0;
		}

		mbInitHasBeenRun = false;

		//Init extra stuff
#ifdef WITH_CG
		if(mGpuProgramFormat == eGpuProgramFormat_CG)
		{
			InitCG();
		}
#endif

		//TTF_Init();
	}

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::~cLowLevelGraphicsSDL()
	{
		//#ifdef WIN32
		//	if(mhKeyTrapper) FreeLibrary(mhKeyTrapper);
		//#endif

		if(mbInitHasBeenRun)
		{
#if !SDL_VERSION_ATLEAST(2, 0, 0)
			SDL_SetGammaRamp(mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);
#endif
		}

		hplFree(mpVertexArray);
		hplFree(mpIndexArray);
		for(int i=0;i<kMaxTextureUnits;i++)	hplFree(mpTexCoordArray[i]);

		//Exit extra stuff
#ifdef WITH_CG
		ExitCG();
#endif
		//TTF_Quit();
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_DestroyWindow(mpScreen);
#endif
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// GENERAL SETUP
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void CALLBACK OGLDebugOutputCallback(GLenum alSource, GLenum alType, GLuint alID, GLenum alSeverity, GLsizei alLength, const GLchar* apMessage, GLvoid* apUserParam)
	{
		Log("Source: %d Type: %d Id: %d Severity: %d '%s'\n", alSource, alType, alID, alSeverity, apMessage);
	}

	//-----------------------------------------------------------------------

	bool cLowLevelGraphicsSDL::Init(int alWidth, int alHeight, int alDisplay, int alBpp, int abFullscreen, 
		int alMultisampling, eGpuProgramFormat aGpuProgramFormat,const tString& asWindowCaption,
		const cVector2l &avWindowPos)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;
        mlDisplay = alDisplay;
		mlBpp = alBpp;
		mbFullscreen = abFullscreen;

		mlMultisampling = alMultisampling;
		
		mGpuProgramFormat = aGpuProgramFormat;
		if(mGpuProgramFormat == eGpuProgramFormat_LastEnum) mGpuProgramFormat = eGpuProgramFormat_GLSL;

		//Set some GL Attributes
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Multisampling
		if(mlMultisampling > 0)
		{
			if(SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1)==-1)
			{
				Error("Multisample buffers not supported!\n");
			}
			else
			{
				if(SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, mlMultisampling)==-1)
				{
					Error("Couldn't set multisampling samples to %d\n",mlMultisampling);
				}
			}
		}

#if SDL_VERSION_ATLEAST(2, 0, 0)
        unsigned int mlFlags = SDL_WINDOW_OPENGL;
        if (alWidth == 0 && alHeight == 0) {
            mvScreenSize = cVector2l(800,600);
            mlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else if (abFullscreen) {
            mlFlags |= SDL_WINDOW_FULLSCREEN;
        }


        Log(" Setting video mode: %d x %d - %d bpp\n",alWidth, alHeight, alBpp);
        mpScreen = SDL_CreateWindow(asWindowCaption.c_str(),
                                    SDL_WINDOWPOS_CENTERED_DISPLAY(mlDisplay), SDL_WINDOWPOS_CENTERED_DISPLAY(mlDisplay),
                                    mvScreenSize.x, mvScreenSize.y, mlFlags);
		if(mpScreen==NULL)
        {
            // try disabling FSAA
			Error("Could not set display mode setting a lower one! %s\n", SDL_GetError());
			mvScreenSize = cVector2l(640,480);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            mpScreen = SDL_CreateWindow(asWindowCaption.c_str(),
                                        SDL_WINDOWPOS_CENTERED_DISPLAY(mlDisplay), SDL_WINDOWPOS_CENTERED_DISPLAY(mlDisplay),
                                        mvScreenSize.x, mvScreenSize.y, mlFlags);
            if(mpScreen==NULL)
            {
                FatalError("Unable to initialize display! %s\n", SDL_GetError());
                return false;
            }
            else
            {
                cPlatform::CreateMessageBox(_W("Warning!"),_W("Could not set displaymode and 640x480 is used instead!\n"));
            }
        }
        {
            // update with the screen size ACTUALLY obtained
            int w,h;
            SDL_GetWindowSize(mpScreen, &w, &h);
            mvScreenSize = cVector2l(w, h);
        }
        mGLContext = SDL_GL_CreateContext(mpScreen);
#else
		unsigned int mlFlags = SDL_OPENGL;

		if(abFullscreen) mlFlags |= SDL_FULLSCREEN;

		// If caption set before engine creation, no chance for the "SDL_App" to appear for even a msec
		SetWindowCaption(asWindowCaption);

		Log(" Setting video mode: %d x %d - %d bpp\n",alWidth, alHeight, alBpp);
		mpScreen = SDL_SetVideoMode( alWidth, alHeight, alBpp, mlFlags);
		if(mpScreen==NULL){
			Error("Could not set display mode setting a lower one!\n");
			mvScreenSize = cVector2l(640,480);

			mpScreen = SDL_SetVideoMode( mvScreenSize.x, mvScreenSize.y, alBpp, mlFlags);
			if(mpScreen==NULL)
			{
				FatalError("Unable to initialize display!\n");
				return false;
			}
			else
			{
				//SetWindowCaption(asWindowCaption);
				cPlatform::CreateMessageBox(_W("Warning!"),_W("Could not set displaymode and 640x480 is used instead!\n"));
			}
		}
		else
		{
			//SetWindowCaption(asWindowCaption);
		}
        // update with the screen size ACTUALLY obtained
        mvScreenSize = cVector2l(mpScreen->w, mpScreen->h);
#   ifdef WIN32
		//////////////////////////////
		// Set up window position
		if(abFullscreen==false)
		{
			SDL_SysWMinfo pInfo;
			SDL_VERSION(&pInfo.version);
			SDL_GetWMInfo(&pInfo);

			RECT r;
			GetWindowRect(pInfo.window, &r);
			
			if(avWindowPos.x >=0 && avWindowPos.y >=0)
			{
				SetWindowPos(pInfo.window, HWND_TOP, avWindowPos.x, avWindowPos.y, 0, 0,  SWP_NOSIZE);
			}
		}
#   endif
#endif
        if (mbGrab) {
            SetWindowGrab(true);
        }

		//Trap Alt tab if in fullscreen
#if defined(WIN32) && !SDL_VERSION_ATLEAST(2,0,0)
		if(abFullscreen)
		{
			//mhKeyTrapper = LoadLibrary( "keyhook.dll" );
			//::DisableTaskKeys(true,false);
		}
#endif //WIN32

		Log(" Init Glew...");
		if(glewInit() == GLEW_OK)
		{
			Log("OK\n");
		}
		else
		{
			Error(" Couldn't init glew!\n");
		}

		///Setup up windows specifc context:
#if defined(WIN32) && !SDL_VERSION_ATLEAST(2,0,0)
		mGLContext = wglGetCurrentContext();
		mDeviceContext = wglGetCurrentDC();
#endif

		//Check Multisample properties
		CheckMultisampleCaps();

		//Turn off cursor as default
		ShowCursor(false);

		//Gamma
		mfGammaCorrection = 1.0f;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetWindowBrightness(mpScreen, mfGammaCorrection);
#else
		SDL_GetGammaRamp(mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);

		SDL_SetGamma(mfGammaCorrection,mfGammaCorrection,mfGammaCorrection);
#endif

		//GL
		Log(" Setting up OpenGL\n");
		SetupGL();

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_GL_SwapWindow(mpScreen);
#else
		//Set the clear color
		SDL_GL_SwapBuffers();
#endif

		mbInitHasBeenRun = true;


		/*if(GLEW_ARB_debug_output)
		{
			glDebugMessageCallbackARB(&OGLDebugOutputCallback, NULL);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}
		else
		{	
			Warning("OGL debug output not supported!\n");
		}*/


		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::CheckMultisampleCaps()
	{

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetupGL()
	{
		///////////////////////////////
		// Setup variables
		mColorWrite.r = true; mColorWrite.g = true;
		mColorWrite.b = true; mColorWrite.a = true;
		mbDepthWrite = true;

		mbCullActive = true;
		mCullMode = eCullMode_CounterClockwise;

		mbDepthTestActive = true;
		mDepthTestFunc = eDepthTestFunc_LessOrEqual;

		mbAlphaTestActive = false;
		mAlphaTestFunc = eAlphaTestFunc_GreaterOrEqual;
		mfAlphaTestFuncRef = 0.6f;

		mbScissorActive = false;
		mvScissorPos =0;
		mvScissorSize = mvScreenSize;

		mbBlendActive = false;

		mpFrameBuffer = NULL;
		mvFrameBufferPos =0;
		mvFrameBufferSize = mvScreenSize;
		mvFrameBufferTotalSize = mvScreenSize;


		///////////////////////////////
		//Inits GL stuff
		//Set Shade model and clear color.
		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		///////////////////////////////
		//Depth Test setup
		glClearDepth(1.0f);//VAlues buffer is cleared with
		glEnable(GL_DEPTH_TEST); //enable depth testing
		glDepthFunc(GL_LEQUAL); //function to do depth test with
		glDisable(GL_ALPHA_TEST);
		glDepthMask(true);

		///////////////////////////////
		//Render Settings
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GetGLDepthTestFuncEnum(mDepthTestFunc));
		glAlphaFunc(GetGLAlphaTestFuncEnum(mAlphaTestFunc),mfAlphaTestFuncRef);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		glDisable(GL_SCISSOR_TEST);
		glScissor(mvScissorPos.x, (mvScissorSize.y - mvScissorPos.y - 1)-mvScissorSize.y, mvScissorSize.x, mvScissorSize.y);
		glDisable(GL_BLEND);

		///////////////////////////////
		//Set best perspective correction
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//int lStencilBits=-1;
		//glGetIntegerv(GL_STENCIL_BITS,&lStencilBits);
		//Log(" Stencil bits: %d\n",lStencilBits);

		///////////////////////////////
		//Stencil setup
		glClearStencil(0);

		///////////////////////////////
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);


		/////  BEGIN BATCH ARRAY STUFF ///////////////

		//Enable all the vertex arrays that are used:
		glEnableClientState(GL_VERTEX_ARRAY ); //The positions
		glEnableClientState(GL_COLOR_ARRAY ); //The color
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); //Tex coords
		glDisableClientState(GL_NORMAL_ARRAY);
		//Disable the once not used.
		glDisableClientState(GL_INDEX_ARRAY); //color index
		glDisableClientState(GL_EDGE_FLAG_ARRAY);

		///// END BATCH ARRAY STUFF ///////////////


		//Show some info
		Log("  Vendor: %s\n", glGetString(GL_VENDOR));
		Log("  Renderer: %s\n", glGetString(GL_RENDERER));
		Log("  Version: %s\n", glGetString(GL_VERSION));
		Log("  Max texture image units: %d\n",GetCaps(eGraphicCaps_MaxTextureImageUnits));
		Log("  Max texture coord units: %d\n",GetCaps(eGraphicCaps_MaxTextureCoordUnits));
		Log("  Max user clip planes: %d\n",GetCaps(eGraphicCaps_MaxUserClipPlanes));
		Log("  Two sided stencil: %d\n",GetCaps(eGraphicCaps_TwoSideStencil));
		Log("  Vertex Buffer Object: %d\n",GetCaps(eGraphicCaps_VertexBufferObject));

		Log("  Anisotropic filtering: %d\n",GetCaps(eGraphicCaps_AnisotropicFiltering));
		if(GetCaps(eGraphicCaps_AnisotropicFiltering))
			Log("  Max Anisotropic degree: %d\n",GetCaps(eGraphicCaps_MaxAnisotropicFiltering));

		Log("  Multisampling: %d\n",GetCaps(eGraphicCaps_Multisampling));

		Log("  Texture compression: %d\n",GetCaps(eGraphicCaps_TextureCompression));
		Log("  Texture compression S3TC: %d\n",GetCaps(eGraphicCaps_TextureCompression_DXTC));

		Log("  Auto generate MipMaps: %d\n",GetCaps(eGraphicCaps_AutoGenerateMipMaps));

		Log("  Render to texture: %d\n",GetCaps(eGraphicCaps_RenderToTexture));
		Log("  Max draw buffers: %d\n",GetCaps(eGraphicCaps_MaxDrawBuffers));
		Log("  Max color render targets: %d\n",GetCaps(eGraphicCaps_MaxColorRenderTargets));

		Log("  Packed depth-stencil: %d\n",GetCaps(eGraphicCaps_PackedDepthStencil));

		Log("  Texture float: %d\n",GetCaps(eGraphicCaps_TextureFloat));

		Log("  GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		Log("  ShaderModel 2: %d\n",GetCaps(eGraphicCaps_ShaderModel_2));
		Log("  ShaderModel 3: %d\n",GetCaps(eGraphicCaps_ShaderModel_3));
		Log("  ShaderModel 4: %d\n",GetCaps(eGraphicCaps_ShaderModel_4));

		Log("  OGL ATIFragmentShader: %d\n",GetCaps(eGraphicCaps_OGL_ATIFragmentShader));

	}
	//-----------------------------------------------------------------------

	int cLowLevelGraphicsSDL::GetCaps(eGraphicCaps aType)
	{
		;

		switch(aType)
		{
		case eGraphicCaps_TextureTargetRectangle:	return 1;//GLEW_ARB_texture_rectangle?1:0;
		
		case eGraphicCaps_VertexBufferObject:		return GLEW_ARB_vertex_buffer_object?1:0;
		case eGraphicCaps_TwoSideStencil:			
			{
				if(GLEW_EXT_stencil_two_side) return 1;
				else if(GLEW_ATI_separate_stencil) return 1;
				else return 0;
			}

		case eGraphicCaps_MaxTextureImageUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,(GLint *)&lUnits);
				return lUnits;
			}

		case eGraphicCaps_MaxTextureCoordUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,(GLint *)&lUnits);
				return lUnits;
			}
		case eGraphicCaps_MaxUserClipPlanes:
			{
				int lClipPlanes;
				glGetIntegerv( GL_MAX_CLIP_PLANES,(GLint *)&lClipPlanes);
				return lClipPlanes;	
			}

		case eGraphicCaps_AnisotropicFiltering:		return GLEW_EXT_texture_filter_anisotropic ? 1 : 0;
		
		case eGraphicCaps_MaxAnisotropicFiltering:
			{
				if(!GLEW_EXT_texture_filter_anisotropic) return 0;

				float fMax;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&fMax);
				return (int)fMax;
			}

		case eGraphicCaps_Multisampling: return GLEW_ARB_multisample ? 1: 0;
		
		case eGraphicCaps_TextureCompression:		return GLEW_ARB_texture_compression  ? 1 : 0;
		case eGraphicCaps_TextureCompression_DXTC:	return GLEW_EXT_texture_compression_s3tc ? 1 : 0;
		
		case eGraphicCaps_AutoGenerateMipMaps:		return GLEW_SGIS_generate_mipmap ? 1 : 0;
	
		case eGraphicCaps_RenderToTexture:			return GLEW_EXT_framebuffer_object ? 1: 0;
		
		case eGraphicCaps_MaxDrawBuffers:
			{
				GLint lMaxbuffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS, &lMaxbuffers);
				return lMaxbuffers;
			}
		case eGraphicCaps_PackedDepthStencil:	return GLEW_EXT_packed_depth_stencil ? 1: 0;		
		case eGraphicCaps_TextureFloat:			return GLEW_ARB_texture_float ? 1: 0;

		case eGraphicCaps_PolygonOffset:		return 1;	//OpenGL always support it!

		case eGraphicCaps_ShaderModel_2:		return (GLEW_ARB_fragment_program || GLEW_ARB_fragment_shader) ? 1 : 0;	//Mac always support this, so not a good test.
#ifdef __APPLE__
		case eGraphicCaps_ShaderModel_3:		return 0; // Force return false for OS X as dynamic branching doesn't work well (it's slow)
		case eGraphicCaps_ShaderModel_4:		return 0;
#else
		case eGraphicCaps_ShaderModel_3:
			{
				if(mbForceShaderModel3And4Off)
					return 0;
				else
					return  (GLEW_NV_vertex_program3 || GLEW_ATI_shader_texture_lod) ? 1 : 0;
			}
		case eGraphicCaps_ShaderModel_4:
			{
				if(mbForceShaderModel3And4Off)
					return 0;
				else
					return  GLEW_EXT_gpu_shader4 ? 1 : 0;
			}
#endif
		
		case eGraphicCaps_OGL_ATIFragmentShader: return  GLEW_ATI_fragment_shader ? 1 : 0;

		case eGraphicCaps_MaxColorRenderTargets:
			{
				GLint lMax;
				glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &lMax);
				return lMax;
			}
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ShowCursor(bool abX)
	{
		;

		if(abX)
			SDL_ShowCursor(SDL_ENABLE);
		else
			SDL_ShowCursor(SDL_DISABLE);
	}
    
	//-----------------------------------------------------------------------

    void cLowLevelGraphicsSDL::SetWindowGrab(bool abX)
    {
        mbGrab = abX;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        if (mpScreen) {
            SDL_SetWindowGrab(mpScreen, abX ? SDL_TRUE : SDL_FALSE);
        }
#else
		SDL_WM_GrabInput(abX ? SDL_GRAB_ON : SDL_GRAB_OFF);
#endif
    }

	void cLowLevelGraphicsSDL::SetRelativeMouse(bool abX)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_SetRelativeMouseMode(abX ? SDL_TRUE : SDL_FALSE);
#endif
	}

    void cLowLevelGraphicsSDL::SetWindowCaption(const tString &asName)
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetWindowTitle(mpScreen, asName.c_str());
#else
        SDL_WM_SetCaption(asName.c_str(), "");
#endif
    }

    bool cLowLevelGraphicsSDL::GetWindowMouseFocus()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return (SDL_GetWindowFlags(mpScreen) & SDL_WINDOW_MOUSE_FOCUS) != 0;
#else
        return (SDL_GetAppState() & SDL_APPMOUSEFOCUS) !=0;
#endif
    }

    bool cLowLevelGraphicsSDL::GetWindowInputFocus()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return (SDL_GetWindowFlags(mpScreen) & SDL_WINDOW_INPUT_FOCUS) != 0;
#else
        return (SDL_GetAppState() & SDL_APPINPUTFOCUS) !=0;
#endif
    }

    bool cLowLevelGraphicsSDL::GetWindowIsVisible()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return (SDL_GetWindowFlags(mpScreen) & SDL_WINDOW_SHOWN) != 0;
#else
        return (SDL_GetAppState() & SDL_APPACTIVE) !=0;
#endif
    }

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVsyncActive(bool abX, bool abAdaptive)
	{
        ;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_GL_SetSwapInterval(abX ? (abAdaptive ? -1 : 1) : 0);
#elif defined(WIN32)
		if(WGLEW_EXT_swap_control)
		{
			wglSwapIntervalEXT(abX ? (abAdaptive ? -1 : 1) : 0);
		}
#elif defined(__linux__)
		if (GLX_SGI_swap_control)
		{
			GLXSWAPINTERVALPROC glXSwapInterval = (GLXSWAPINTERVALPROC)glXGetProcAddress((GLubyte*)"glXSwapIntervalSGI");
			glXSwapInterval(abX ? (abAdaptive ? -1 : 1) : 0);
		}
		else if (GLX_MESA_swap_control)
		{
			GLXSWAPINTERVALPROC glXSwapInterval = (GLXSWAPINTERVALPROC)glXGetProcAddress((GLubyte*)"glXSwapIntervalMESA");
			glXSwapInterval(abX ? (abAdaptive ? -1 : 1) : 0);
		}
#elif defined(__APPLE__)
		CGLContextObj ctx = CGLGetCurrentContext();
		GLint swap = abX ? 1 : 0;
		CGLSetParameter(ctx, kCGLCPSwapInterval, &swap);
#endif
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMultisamplingActive(bool abX)
	{	
		;

		if(!GLEW_ARB_multisample || mlMultisampling<=0) return;

		if(abX)
			glEnable(GL_MULTISAMPLE_ARB);
		else
			glDisable(GL_MULTISAMPLE_ARB);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetGammaCorrection(float afX)
	{
		;

		mfGammaCorrection = afX;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetWindowBrightness(mpScreen, mfGammaCorrection);
#else
		SDL_SetGamma(mfGammaCorrection,mfGammaCorrection,mfGammaCorrection);
#endif
	}

	float cLowLevelGraphicsSDL::GetGammaCorrection()
	{
		;

		return mfGammaCorrection;
	}

	//-----------------------------------------------------------------------

	cVector2f cLowLevelGraphicsSDL::GetScreenSizeFloat()
	{
		;

		return cVector2f((float)mvScreenSize.x, (float)mvScreenSize.y);
	}

	const cVector2l& cLowLevelGraphicsSDL::GetScreenSizeInt()
	{
		;

		return mvScreenSize;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// DATA CREATION
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iFontData* cLowLevelGraphicsSDL::CreateFontData(const tString &asName)
	{
		;

		return hplNew( cSDLFontData, (asName, this) );
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cLowLevelGraphicsSDL::CreateGpuProgram(const tString& asName)
	{
		;

		return hplNew( cGLSLProgram, (asName) );
		//return hplNew( cCGProgram, () ); 
	}

	iGpuShader* cLowLevelGraphicsSDL::CreateGpuShader(const tString& asName, eGpuShaderType aType)
	{
		;

		return hplNew( cGLSLShader, (asName,aType, this) );
		//return hplNew( cCGShader, (asName,mCG_Context, aType) );
	}

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(const tString &asName,eTextureType aType,   eTextureUsage aUsage)
	{
		;

		cSDLTexture *pTexture = hplNew( cSDLTexture, (asName,aType, aUsage, this) );

		return pTexture;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cLowLevelGraphicsSDL::CreateVertexBuffer(eVertexBufferType aType,
															eVertexBufferDrawType aDrawType,
															eVertexBufferUsageType aUsageType, 
															int alReserveVtxSize,int alReserveIdxSize)
	{
		;

		//return hplNew( cVertexBufferVBO,(this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
		//return hplNew( cVertexBufferOGL, (this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );

		if(GetCaps(eGraphicCaps_VertexBufferObject) && aType == eVertexBufferType_Hardware)
		{
			return hplNew( cVertexBufferOGL_VBO, (this, aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
		}
		else
		{
			//Error("VBO is not supported, using Vertex array!\n");
			return hplNew( cVertexBufferOGL_Array, (this, aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
		}
	}

	//-----------------------------------------------------------------------

	iFrameBuffer* cLowLevelGraphicsSDL::CreateFrameBuffer(const tString& asName)
	{
		;

		if(GetCaps(eGraphicCaps_RenderToTexture)==0) return NULL;

		return hplNew(cFrameBufferGL,(asName, this));
	}

	//-----------------------------------------------------------------------

	iDepthStencilBuffer* cLowLevelGraphicsSDL::CreateDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits)
	{
		;

		if(GetCaps(eGraphicCaps_RenderToTexture)==0) return NULL;

		return hplNew(cDepthStencilBufferGL,(avSize, alDepthBits,alStencilBits));
	}

	//-----------------------------------------------------------------------

	iOcclusionQuery* cLowLevelGraphicsSDL::CreateOcclusionQuery()
	{
		;

		return hplNew(cOcclusionQueryOGL, () );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FRAME BUFFER OPERATIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearFrameBuffer(tClearFrameBufferFlag aFlags)
	{
		;

		GLbitfield bitmask=0;

		if(aFlags & eClearFrameBufferFlag_Color)	bitmask |= GL_COLOR_BUFFER_BIT;
		if(aFlags & eClearFrameBufferFlag_Depth)	bitmask |= GL_DEPTH_BUFFER_BIT;
		if(aFlags & eClearFrameBufferFlag_Stencil)	bitmask |= GL_STENCIL_BUFFER_BIT;

		glClear(bitmask);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetClearColor(const cColor& aCol){
		;

		glClearColor(aCol.r, aCol.g, aCol.b, aCol.a);
	}
	void cLowLevelGraphicsSDL::SetClearDepth(float afDepth){
		;

		glClearDepth(afDepth);
	}
	void cLowLevelGraphicsSDL::SetClearStencil(int alVal){
		;

		glClearStencil(alVal);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::CopyFrameBufferToTexure(iTexture* apTex, const cVector2l &avPos, 
		const cVector2l &avSize, const cVector2l &avTexOffset)
	{
		;

		if(apTex==NULL)return;

		cVector2l vSize = avSize;
		if(vSize.x <= 0) vSize.x = mvFrameBufferSize.x;
		if(vSize.y <= 0) vSize.y = mvFrameBufferSize.y;

		cVector2l vPos = mvFrameBufferPos + avPos;
		vPos.y = (mvFrameBufferTotalSize.y - vSize.y)-vPos.y;

		cVector2l vTexPos = avTexOffset;
		vTexPos.y = (apTex->GetHeight() - vSize.y)-vTexPos.y;

		//Log(" Copying current to texture Pos: %d:%d Size: %dx%d TextureOffset: %d:%d\n",
		//	vPos.x, vPos.y, vSize.x, vSize.y, vTexPos.x, vTexPos.y);

		SetTexture(0, apTex);
		glCopyTexSubImage2D(GetGLTextureTargetEnum(apTex->GetType()),0,	vTexPos.x, vTexPos.y,
							vPos.x, vPos.y, vSize.x, vSize.y);
	}

	//-----------------------------------------------------------------------

	cBitmap* cLowLevelGraphicsSDL::CopyFrameBufferToBitmap(	const cVector2l &avScreenPos,const cVector2l &avScreenSize)
	{
		;

		cVector2l vSize = avScreenSize;
		if(vSize.x <= 0) vSize.x = mvFrameBufferSize.x;
		if(vSize.y <= 0) vSize.y = mvFrameBufferSize.y;

		cVector2l vPos = mvFrameBufferPos + avScreenPos;
		vPos.y = (mvFrameBufferTotalSize.y - vSize.y)-vPos.y;

		cBitmap *pBitmap = hplNew(cBitmap, () );

		///////////////////////////////////
		//Get the correct pixel format
        ePixelFormat pixelFormat = ePixelFormat_RGBA;
		if(mpFrameBuffer)
		{
			iFrameBufferAttachment *pColorBuffer = mpFrameBuffer->GetColorBuffer(0);
			if(pColorBuffer && pColorBuffer->GetFrameBufferAttachmentType() == eFrameBufferAttachment_Texture)
			{
				iTexture *pTexture = static_cast<iTexture*>(pColorBuffer);
				pixelFormat = pTexture->GetPixelFormat();
			}
		}

		//////////////////////////////
		//Get create the bitmap data
		pBitmap->CreateData(cVector3l(vSize.x, vSize.y,1),pixelFormat,0,0);

		//////////////////////////////
		//Copy pixels
		glReadPixels(vPos.x, vPos.y, vSize.x, vSize.y,PixelFormatToGLFormat(pixelFormat), GL_UNSIGNED_BYTE, pBitmap->GetData(0,0)->mpData);

		//TODO: Flip, not needed?

		return pBitmap;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetCurrentFrameBuffer(iFrameBuffer* apFrameBuffer, const cVector2l &avPos, const cVector2l& avSize)
	{
		;

		///////////////////////////////////////////////////////
		//Set frame buffer 
		if(true)//mpFrameBuffer != apFrameBuffer)
		{
			iFrameBuffer *pPrevFameBuffer = mpFrameBuffer;
			mpFrameBuffer = apFrameBuffer;

			if(mpFrameBuffer)
			{
				cFrameBufferGL *pFrameBufferGL = static_cast<cFrameBufferGL*>(mpFrameBuffer);

				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,pFrameBufferGL->GetHandle());
			}
			else
			{
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			}

			// Post tasks like generating mipmaps
			if(pPrevFameBuffer)
			{
				pPrevFameBuffer->PostBindUpdate();
			}
		}

		/////////////////////////////////////
		//Get framebuffer size
		if(mpFrameBuffer)	mvFrameBufferTotalSize = mpFrameBuffer->GetSize();
		else				mvFrameBufferTotalSize = mvScreenSize;
		
		cVector2l vFrameBufferSize = avSize;
		if(vFrameBufferSize.x <0 || vFrameBufferSize.y<0)
		{
			vFrameBufferSize = mvFrameBufferTotalSize;
		}
		cVector2l vFrameBufferPos = avPos;

		///////////////////////////////////////////////////////
		//Set Viewport
		if(true)//mvFrameBufferPos != vFrameBufferPos || mvFrameBufferSize != vFrameBufferSize)
		{
			mvFrameBufferSize = vFrameBufferSize;
			mvFrameBufferPos = vFrameBufferPos;

			if(mpFrameBuffer)
			{
				vFrameBufferPos.y = (mpFrameBuffer->GetSize().y - vFrameBufferSize.y)-vFrameBufferPos.y;
			}
			else
			{
				vFrameBufferPos.y = (mvScreenSize.y - vFrameBufferSize.y)-vFrameBufferPos.y;
			}
			glViewport(vFrameBufferPos.x,vFrameBufferPos.y,vFrameBufferSize.x, vFrameBufferSize.y);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetFrameBufferDrawTargets(int *apTargets, int alNumOfTargets)
	{
		;

		std::vector<GLenum> vAttachmentVec;
		for(int i=0; i<alNumOfTargets; ++i)
		{
			vAttachmentVec.push_back(GL_COLOR_ATTACHMENT0_EXT + apTargets[i]);
		}
		glDrawBuffers(alNumOfTargets, &vAttachmentVec[0]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::WaitAndFinishRendering()
	{
		;

		glFinish();
		//dont use this any more, SwapBuffers() takes care of it
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::FlushRendering()
	{
		;

		glFlush();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SwapBuffers()
	{
		;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_GL_SwapWindow(mpScreen);
#else
		SDL_GL_SwapBuffers();
#endif
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// RENDER STATE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)
	{
		;

		if( mColorWrite.r == abR &&
			mColorWrite.g == abG &&
			mColorWrite.b == abB &&
			mColorWrite.a == abA)
		{
			return;
		}
		mColorWrite.r = abR;
		mColorWrite.g = abG;
		mColorWrite.b = abB;
		mColorWrite.a = abA;

		glColorMask(abR,abG,abB,abA);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthWriteActive(bool abX)
	{
		;

		if(mbDepthWrite == abX) return;

		mbDepthWrite = abX;

		glDepthMask(abX);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthTestActive(bool abX)
	{
		;

		if(mbDepthTestActive == abX) return;

		mbDepthTestActive = abX;

		if(abX) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthTestFunc(eDepthTestFunc aFunc)
	{
		;

		if(mDepthTestFunc == aFunc) return;
		mDepthTestFunc = aFunc;

		glDepthFunc(GetGLDepthTestFuncEnum(aFunc));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetAlphaTestActive(bool abX)
	{
		;

		if(mbAlphaTestActive == abX) return;

		mbAlphaTestActive = abX;

		if(abX) glEnable(GL_ALPHA_TEST);
		else glDisable(GL_ALPHA_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)
	{
		;

		if(mAlphaTestFunc == aFunc && mfAlphaTestFuncRef == afRef) return;

		mAlphaTestFunc = aFunc;
		mfAlphaTestFuncRef = afRef;

		glAlphaFunc(GetGLAlphaTestFuncEnum(aFunc),afRef);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencilActive(bool abX)
	{
		;

		if(abX)
		{
			//DO this check, so you can setup stencil and then turn on / off afterwards
			//and separate will still remain.
			if(mbDoubleSidedStencilIsSet && GLEW_EXT_stencil_two_side)
			{
				glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			}
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			glDisable(GL_STENCIL_TEST);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencilWriteMask(unsigned int alMask)
	{
		;

		glStencilMask(alMask);
	}
	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
		eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)
	{
		;

		mbDoubleSidedStencilIsSet = false;
		if(GLEW_EXT_stencil_two_side)
		{
			glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			glActiveStencilFaceEXT(GL_FRONT);
		}
		glStencilFunc(GetGLStencilFuncEnum(aFunc), alRef, aMask);

		glStencilOp(GetGLStencilOpEnum(aFailOp), GetGLStencilOpEnum(aZFailOp),
			GetGLStencilOpEnum(aZPassOp));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
		int alRef, unsigned int aMask,
		eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
		eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)
	{
		;

		mbDoubleSidedStencilIsSet = true;

		//Nvidia implementation
		if(GLEW_EXT_stencil_two_side)
		{
			glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

			//Front
			glActiveStencilFaceEXT(GL_FRONT);
			glStencilFunc(GetGLStencilFuncEnum(aFrontFunc), alRef, aMask);

			glStencilOp(GetGLStencilOpEnum(aFrontFailOp), GetGLStencilOpEnum(aFrontZFailOp),
				GetGLStencilOpEnum(aFrontZPassOp));
			//Back
			glActiveStencilFaceEXT(GL_BACK);
			glStencilFunc(GetGLStencilFuncEnum(aBackFunc), alRef, aMask);

			glStencilOp(GetGLStencilOpEnum(aBackFailOp), GetGLStencilOpEnum(aBackZFailOp),
				GetGLStencilOpEnum(aBackZPassOp));
		}
		//Ati implementation
		else if(GLEW_ATI_separate_stencil)
		{
			//Front
			glStencilOpSeparateATI( GL_FRONT, GetGLStencilOpEnum(aFrontFailOp), 
				GetGLStencilOpEnum(aFrontZFailOp), 
				GetGLStencilOpEnum(aFrontZPassOp));
			//Back
			glStencilOpSeparateATI( GL_BACK, GetGLStencilOpEnum(aBackFailOp), 
				GetGLStencilOpEnum(aBackZFailOp), 
				GetGLStencilOpEnum(aBackZPassOp));

			//Front and Back function
			glStencilFuncSeparateATI(GetGLStencilFuncEnum(aFrontFunc),
				GetGLStencilFuncEnum(aBackFunc), 
				alRef, aMask);
		}
		else
		{
			FatalError("Only single sided stencil supported!\n");
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetCullActive(bool abX)
	{
		;

		//if(mbCullActive == abX) return;
		mbCullActive = abX;

		if(abX) glEnable(GL_CULL_FACE);
		else	glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	void cLowLevelGraphicsSDL::SetCullMode(eCullMode aMode)
	{
		//if(mCullMode == aMode) return;
		mCullMode = aMode;

		glCullFace(GL_BACK);
		if(aMode == eCullMode_Clockwise)	glFrontFace(GL_CCW);
		else								glFrontFace(GL_CW);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetScissorActive(bool abX)
	{
		;

		if(mbScissorActive == abX) return;

		mbScissorActive = abX;

		if(abX) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetScissorRect(const cVector2l& avPos, const cVector2l& avSize)
	{
		;

		cVector2l vFrameBufferSize;
		if(mpFrameBuffer)	vFrameBufferSize = mpFrameBuffer->GetSize();
		else				vFrameBufferSize = mvScreenSize;

		glScissor(avPos.x, (vFrameBufferSize.y - avPos.y)-avSize.y, avSize.x, avSize.y);
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetClipPlane(int alIdx, const cPlanef& aPlane)
	{
		;

		mvClipPlanes[alIdx] = aPlane;

		double vPlane[4];
		vPlane[0] = aPlane.a;
		vPlane[1] = aPlane.b;
		vPlane[2] = aPlane.c;
		vPlane[3] = aPlane.d;
		glClipPlane(GL_CLIP_PLANE0 + alIdx,vPlane);
	}
	cPlanef cLowLevelGraphicsSDL::GetClipPlane(int alIdx)
	{
		;

		return mvClipPlanes[alIdx];
	}
	void cLowLevelGraphicsSDL::SetClipPlaneActive(int alIdx, bool abX)
	{
		;

		if(abX) glEnable(GL_CLIP_PLANE0 + alIdx);
		else	glDisable(GL_CLIP_PLANE0 + alIdx);
	}


	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetBlendActive(bool abX)
	{
		;

		if(mbBlendActive == abX) return;
		mbBlendActive = abX;

		if(abX)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)
	{
		;

		glBlendFunc(GetGLBlendEnum(aSrcFactor),GetGLBlendEnum(aDestFactor));
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
		eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)
	{
		;

		if(GLEW_EXT_blend_func_separate)
		{
			glBlendFuncSeparateEXT(GetGLBlendEnum(aSrcFactorColor),
				GetGLBlendEnum(aDestFactorColor),
				GetGLBlendEnum(aSrcFactorAlpha),
				GetGLBlendEnum(aDestFactorAlpha));
		}
		else
		{
			glBlendFunc(GetGLBlendEnum(aSrcFactorColor),GetGLBlendEnum(aDestFactorColor));
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetPolygonOffsetActive(bool abX)
	{
		;

		if(abX)	glEnable(GL_POLYGON_OFFSET_FILL);
		else	glDisable(GL_POLYGON_OFFSET_FILL);

	}

	void cLowLevelGraphicsSDL::SetPolygonOffset(float afBias, float afSlopeScaleBias)
	{
		;

		glPolygonOffset(afSlopeScaleBias, afBias);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// MATRIX
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::PushMatrix(eMatrix aMtxType)
	{
		;

		SetMatrixMode(aMtxType);
		glPushMatrix();
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::PopMatrix(eMatrix aMtxType)
	{
		;

		SetMatrixMode(aMtxType);
		glPopMatrix();
	}
	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)
	{
		;

		SetMatrixMode(aMtxType);
		glLoadIdentity();
		cMatrixf mtxTranpose = a_mtxA.GetTranspose();
		glLoadMatrixf(mtxTranpose.v);
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetIdentityMatrix(eMatrix aMtxType)
	{
		;

		SetMatrixMode(aMtxType);
		glLoadIdentity();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)
	{
		;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,avSize.x,avSize.y,0,afMin,afMax);
	}

	void cLowLevelGraphicsSDL::SetOrthoProjection(const cVector3f& avMin, const cVector3f& avMax)
	{
		;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(avMin.x,avMax.x,avMax.y,avMin.y,avMin.z,avMax.z);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// TEXTURE OPERATIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetTexture(unsigned int alUnit,iTexture* apTex)
	{
		;

		GLenum NewTarget=0;
		if(apTex)	NewTarget = GetGLTextureTargetEnum(apTex->GetType());
		
		GLenum LastTarget = mvCurrentTextureTarget[alUnit];
		
		//Check if multi texturing is supported.
		if(GLEW_ARB_multitexture){
			glActiveTextureARB(GL_TEXTURE0_ARB + alUnit);
		}
		
		//Disable this unit if NULL
		if(apTex == NULL)
		{
			if(LastTarget!=0)
				glDisable(LastTarget);
			
			//glBindTexture(LastTarget,0);
		}
		//Enable the unit, set the texture handle and bind the pbuffer
		else
		{
			if(LastTarget!=0 && NewTarget != LastTarget)
			{
				glDisable(LastTarget);
			}

			cSDLTexture *pSDLTex = static_cast<cSDLTexture*> (apTex);

			glBindTexture(NewTarget, pSDLTex->GetTextureHandle());
			glEnable(NewTarget);

			//if it is a render target we need to do some more binding.
			if(pSDLTex->GetUsage() == eTextureUsage_RenderTarget)
			{
				//TODO: Do something else?
			}
		}

		mvCurrentTextureTarget[alUnit] = NewTarget;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetActiveTextureUnit(unsigned int alUnit)
	{
		;

		glActiveTextureARB(GL_TEXTURE0_ARB + alUnit);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTextureEnv(eTextureParam aParam, int alVal)
	{
		;

		GLenum lParam = GetGLTextureParamEnum(aParam);

		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_ARB);

		if(aParam==eTextureParam_ColorFunc || aParam==eTextureParam_AlphaFunc){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureFuncEnum((eTextureFunc)alVal));
		}
		else if(aParam>=eTextureParam_ColorSource0 && aParam<=eTextureParam_AlphaSource2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureSourceEnum((eTextureSource)alVal));
		}
		else if(aParam>=eTextureParam_ColorOp0 && aParam<=eTextureParam_AlphaOp2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureOpEnum((eTextureOp)alVal));
		}
		else {
			glTexEnvi(GL_TEXTURE_ENV,lParam,alVal);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTextureConstantColor(const cColor &aColor)
	{
		;

		float vColor[4] = {	aColor.r, aColor.g, aColor.b, aColor.a	};

		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &vColor[0]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetColor(const cColor &aColor)
	{
		;

		glColor4f(aColor.r, aColor.g, aColor.b, aColor.a);
	}

	//-----------------------------------------------------------------------



	//////////////////////////////////////////////////////////////////////////
	// DRAWING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawTriangle(tVertexVec& avVtx)
	{
		;

		assert(avVtx.size()==3);

		glBegin(GL_TRIANGLES);
		{
			for(int i=0;i<3;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const cVector3f &avPos,const cVector2f &avSize,const cColor& aColor)
	{
		;

		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0, 0.0); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y,avPos.z);

			glTexCoord2f(1.0, 0.0); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y,avPos.z);

			glTexCoord2f(1.0, 1.0); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y+avSize.y,avPos.z);

			glTexCoord2f(0.0, 1.0); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y+avSize.y,avPos.z);

		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuad(const cVector3f &avPos,const cVector2f &avSize,
		const cVector2f &avMinTexCoord,const cVector2f &avMaxTexCoord,
		const cColor& aColor)
	{
		;

		glBegin(GL_QUADS);
		{
			glTexCoord2f(avMinTexCoord.x, avMinTexCoord.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y,avPos.z);

			glTexCoord2f(avMaxTexCoord.x, avMinTexCoord.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y,avPos.z);

			glTexCoord2f(avMaxTexCoord.x, avMaxTexCoord.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y+avSize.y,avPos.z);

			glTexCoord2f(avMinTexCoord.x, avMaxTexCoord.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y+avSize.y,avPos.z);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuad(const cVector3f &avPos,const cVector2f &avSize,
		const cVector2f &avMinTexCoord0,const cVector2f &avMaxTexCoord0,
		const cVector2f &avMinTexCoord1,const cVector2f &avMaxTexCoord1,
		const cColor& aColor)
	{
		;

		glBegin(GL_QUADS);
		{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,avMinTexCoord0.x, avMinTexCoord0.y); 
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,avMinTexCoord1.x, avMinTexCoord1.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y,avPos.z);

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,avMaxTexCoord0.x, avMinTexCoord0.y); 
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,avMaxTexCoord1.x, avMinTexCoord1.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y,avPos.z);

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,avMaxTexCoord0.x, avMaxTexCoord0.y); 
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,avMaxTexCoord1.x, avMaxTexCoord1.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x+avSize.x, avPos.y+avSize.y,avPos.z);

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,avMinTexCoord0.x, avMaxTexCoord0.y); 
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,avMinTexCoord1.x, avMaxTexCoord1.y); 
			glColor4f(aColor.r,aColor.g,aColor.b,aColor.a);
			glVertex3f(avPos.x, avPos.y+avSize.y,avPos.z);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx)
	{
		;

		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs)
	{
		;

		int lExtraUnits = (int)avExtraUvs.size()/4;
		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++)
			{
				glMultiTexCoord3fARB(GL_TEXTURE0_ARB,avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);

				for(int unit=0; unit<lExtraUnits; ++unit)
				{
					glMultiTexCoord3fARB(GL_TEXTURE0_ARB + unit + 1,
						avExtraUvs[unit*4 + i].x, avExtraUvs[unit*4 + i].y, avExtraUvs[unit*4 + i].z);
				}

				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();

	}


	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx, const cColor aCol)
	{
		;

		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ)
	{
		;

		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol)
	{
		;

		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol)
	{
		;

		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			glVertex3f(avBegin.x,avBegin.y,avBegin.z);
			glVertex3f(avEnd.x,avEnd.y,avEnd.z);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawLine(const cVector3f& avBegin, const cColor& aBeginCol, const cVector3f& avEnd, const cColor& aEndCol)
	{
		;

		glBegin(GL_LINES);
		{
			glColor4f(aBeginCol.r,aBeginCol.g,aBeginCol.b,aBeginCol.a);
			glVertex3f(avBegin.x,avBegin.y,avBegin.z);

			glColor4f(aEndCol.r,aEndCol.g,aEndCol.b,aEndCol.a);
			glVertex3f(avEnd.x,avEnd.y,avEnd.z);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawBoxMinMax(const cVector3f& avMin, const cVector3f& avMax, cColor aCol)
	{
		;

		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			//Pos Z Quad
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMax.z);

			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			//Neg Z Quad
			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			//Lines between
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMax.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);
		}
		glEnd();

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol)
	{
		;

		int alSegments = 32;
		float afAngleStep = k2Pif /(float)alSegments;

		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			//X Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x, avPos.y + sin(a)*afRadius, 
					avPos.z + cos(a)*afRadius);

				glVertex3f(avPos.x, avPos.y + sin(a+afAngleStep)*afRadius, 
					avPos.z + cos(a+afAngleStep)*afRadius);
			}

			//Y Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y, 
					avPos.z + sin(a)*afRadius);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius, avPos.y , 
					avPos.z+ sin(a+afAngleStep)*afRadius);
			}

			//Z Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y + sin(a)*afRadius, avPos.z);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius, 
					avPos.y + sin(a+afAngleStep)*afRadius, 
					avPos.z);
			}

		}
		glEnd();
	}

	//-----------------------------------------------------------------------
	
	void cLowLevelGraphicsSDL::DrawSphere(const cVector3f& avPos, float afRadius, cColor aColX, cColor aColY, cColor aColZ)
	{
		;

		int alSegments = 32;
		float afAngleStep = k2Pif /(float)alSegments;

		SetTexture(0,NULL);
		SetBlendActive(false);
		glBegin(GL_LINES);
		{
			//X Circle:
			glColor4f(aColX.r,aColX.g,aColX.b,aColX.a);
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x, avPos.y + sin(a)*afRadius, 
					avPos.z + cos(a)*afRadius);

				glVertex3f(avPos.x, avPos.y + sin(a+afAngleStep)*afRadius, 
					avPos.z + cos(a+afAngleStep)*afRadius);
			}

			//Y Circle:
			glColor4f(aColY.r,aColY.g,aColY.b,aColY.a);
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y, 
					avPos.z + sin(a)*afRadius);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius, avPos.y , 
					avPos.z+ sin(a+afAngleStep)*afRadius);
			}

			//Z Circle:
			glColor4f(aColZ.r,aColZ.g,aColZ.b,aColZ.a);
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y + sin(a)*afRadius, avPos.z);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius, 
					avPos.y + sin(a+afAngleStep)*afRadius, 
					avPos.z);
			}

		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLineQuad(const cRect2f& aRect, float afZ, cColor aCol)
	{
		;

		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINE_STRIP);
		{
			glVertex3f(aRect.x,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y,afZ);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawLineQuad(const cVector3f &avPos,const cVector2f &avSize, cColor aCol)
	{
		;

		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINE_STRIP);
		{
			glVertex3f(avPos.x,avPos.y,avPos.z);
			glVertex3f(avPos.x+avSize.x,avPos.y,avPos.z);
			glVertex3f(avPos.x+avSize.x,avPos.y+avSize.y,avPos.z);
			glVertex3f(avPos.x,avPos.y+avSize.y,avPos.z);
			glVertex3f(avPos.x,avPos.y,avPos.z);
		}
		glEnd();
	}



	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// VERTEX BATCHING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch(const cVertex *apVtx)
	{
		;

		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;
		//Normal coord
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount = mlVertexCount + mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform)
	{
		;

		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x+avTransform->x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y+avTransform->y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z+avTransform->z;

		/*Log("Trans: %s\n",avTransform->ToString().c_str());
		Log("Adding: %f:%f:%f\n",mpVertexArray[mlVertexCount + 0],
		mpVertexArray[mlVertexCount + 1],
		mpVertexArray[mlVertexCount + 2]);*/
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;

		/*Log("Tex: %f:%f:%f\n",mpVertexArray[mlVertexCount + 7],
		mpVertexArray[mlVertexCount + 8],
		mpVertexArray[mlVertexCount + 9]);*/

		//Normal coord
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount = mlVertexCount + mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch(const cVertex *apVtx, const cMatrixf* aMtx)
	{
		;

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
		const cColor* apCol,const float& mfW, const float& mfH)
	{
		;

		//Coord
		mpVertexArray[mlVertexCount + 0] =	avTransform->x + mfW;
		mpVertexArray[mlVertexCount + 1] =	avTransform->y + mfH;
		mpVertexArray[mlVertexCount + 2] =	avTransform->z;

		//Color
		mpVertexArray[mlVertexCount + 3] =	apCol->r;
		mpVertexArray[mlVertexCount + 4] =	apCol->g;
		mpVertexArray[mlVertexCount + 5] =	apCol->b;
		mpVertexArray[mlVertexCount + 6] =	apCol->a;

		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;


		mlVertexCount = mlVertexCount + mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch_Raw(	const cVector3f& avPos, const cColor &aColor,
		const cVector3f& avTex)
	{
		;

		//Coord
		mpVertexArray[mlVertexCount + 0] =	avPos.x;
		mpVertexArray[mlVertexCount + 1] =	avPos.y;
		mpVertexArray[mlVertexCount + 2] =	avPos.z;

		//Color
		mpVertexArray[mlVertexCount + 3] =	aColor.r;
		mpVertexArray[mlVertexCount + 4] =	aColor.g;
		mpVertexArray[mlVertexCount + 5] =	aColor.b;
		mpVertexArray[mlVertexCount + 6] =	aColor.a;

		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	avTex.x;
		mpVertexArray[mlVertexCount + 8] =	avTex.y;
		mpVertexArray[mlVertexCount + 9] =	avTex.z;


		mlVertexCount = mlVertexCount + mlBatchStride;
	}


	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddIndexToBatch(int alIndex)
	{
		;

		mpIndexArray[mlIndexCount] = alIndex;
		mlIndexCount++;

		if(mlIndexCount>=mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddTexCoordToBatch(unsigned int alUnit,const cVector3f *apCoord)
	{
		;

		unsigned int lCount = mlTexCoordArrayCount[alUnit];

		mpTexCoordArray[alUnit][lCount+0] = apCoord->x;
		mpTexCoordArray[alUnit][lCount+1] = apCoord->y;
		mpTexCoordArray[alUnit][lCount+2] = apCoord->z;

		mlTexCoordArrayCount[alUnit]+=3;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetBatchTextureUnitActive(unsigned int alUnit,bool abActive)
	{
		;

		glClientActiveTextureARB(GL_TEXTURE0_ARB+alUnit);

		if(abActive==false){
			glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
		}
		else {
			glTexCoordPointer(3,GL_FLOAT,0, &mpTexCoordArray[alUnit][0]);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear)
	{
		;

		SetVtxBatchStates(aTypeFlags);
		SetUpBatchArrays();

		glDrawElements(GL_TRIANGLES,mlIndexCount,GL_UNSIGNED_INT, mpIndexArray);

		if(abAutoClear){
			mlIndexCount = 0;
			mlVertexCount = 0;
			for(int i=0;i<kMaxTextureUnits;i++)
				mlTexCoordArrayCount[i]=0;
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushQuadBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear)
	{
		;

		SetVtxBatchStates(aTypeFlags);
		SetUpBatchArrays();

		glDrawElements(GL_QUADS,mlIndexCount,GL_UNSIGNED_INT, mpIndexArray);

		if(abAutoClear){
			mlIndexCount = 0;
			mlVertexCount = 0;
			for(int i=0;i<kMaxTextureUnits;i++)
				mlTexCoordArrayCount[i]=0;
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearBatch()
	{
		mlIndexCount = 0;
		mlVertexCount = 0;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// IMPLEMENTAION SPECIFICS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetUpBatchArrays()
	{
		;

		//Set the arrays
		glVertexPointer(3,GL_FLOAT, sizeof(float)*mlBatchStride, mpVertexArray);
		glColorPointer(4,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[3]);
		glNormalPointer(GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[10]);

		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
		glClientActiveTextureARB(GL_TEXTURE2_ARB);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVtxBatchStates(tVtxBatchFlag aFlags)
	{
		;

		if(aFlags & eVtxBatchFlag_Position)	glEnableClientState(GL_VERTEX_ARRAY );
		else glDisableClientState(GL_VERTEX_ARRAY );

		if(aFlags & eVtxBatchFlag_Color0) glEnableClientState(GL_COLOR_ARRAY );
		else glDisableClientState(GL_COLOR_ARRAY );

		if(aFlags & eVtxBatchFlag_Normal) glEnableClientState(GL_NORMAL_ARRAY );
		else glDisableClientState(GL_NORMAL_ARRAY );


		if(aFlags & eVtxBatchFlag_Texture0){ 
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		if(aFlags & eVtxBatchFlag_Texture1){ 
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		if(aFlags & eVtxBatchFlag_Texture2){ 
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}


	}
#ifdef WITH_CG
	void cLowLevelGraphicsSDL::InitCG()
	{
		mCG_Context = cgCreateContext();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ExitCG()
	{
		cgDestroyContext(mCG_Context);
	}
#endif

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMatrixMode(eMatrix mType)
	{
		;

		switch(mType)
		{
		case eMatrix_ModelView: glMatrixMode(GL_MODELVIEW);break;
		case eMatrix_Projection: glMatrixMode(GL_PROJECTION); break;
		case eMatrix_Texture: glMatrixMode(GL_TEXTURE); break;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	GLenum GetGLWrapEnum(eTextureWrap aMode)
	{
		;

		switch(aMode)
		{
		case eTextureWrap_Clamp: return GL_CLAMP;
		case eTextureWrap_Repeat: return GL_REPEAT;
		case eTextureWrap_ClampToEdge: return GL_CLAMP_TO_EDGE;
		case eTextureWrap_ClampToBorder: return GL_CLAMP_TO_BORDER;
		}

		return GL_REPEAT;
	}


	//-----------------------------------------------------------------------

	GLenum PixelFormatToGLFormat(ePixelFormat aFormat)
	{
		;

		switch(aFormat)
		{
		case ePixelFormat_Alpha:			return GL_ALPHA;
		case ePixelFormat_Luminance:		return GL_LUMINANCE;
		case ePixelFormat_LuminanceAlpha:	return GL_LUMINANCE_ALPHA;
		case ePixelFormat_RGB:				return GL_RGB;
		case ePixelFormat_RGBA:				return GL_RGBA;
		case ePixelFormat_BGR:				return GL_BGR_EXT;
		case ePixelFormat_BGRA:				return GL_BGRA_EXT;
		case ePixelFormat_Depth16:			return GL_DEPTH_COMPONENT;
		case ePixelFormat_Depth24:			return GL_DEPTH_COMPONENT;
		case ePixelFormat_Depth32:			return GL_DEPTH_COMPONENT;
		case ePixelFormat_Alpha16:			return GL_ALPHA;
		case ePixelFormat_Luminance16:		return GL_LUMINANCE;
		case ePixelFormat_LuminanceAlpha16:	return GL_LUMINANCE_ALPHA;
		case ePixelFormat_RGB16:			return GL_RGB;
		case ePixelFormat_RGBA16:			return GL_RGBA;
		case ePixelFormat_Alpha32:			return GL_ALPHA;
		case ePixelFormat_Luminance32:		return GL_LUMINANCE;
		case ePixelFormat_LuminanceAlpha32: return GL_LUMINANCE_ALPHA;
		case ePixelFormat_RGB32:			return GL_RGB;
		case ePixelFormat_RGBA32:			return GL_RGBA;
		};

		return 0;
	}

	//-------------------------------------------------

	GLenum PixelFormatToGLInternalFormat(ePixelFormat aFormat)
	{
		;

		switch(aFormat)
		{
		case ePixelFormat_Alpha:			return GL_ALPHA;
		case ePixelFormat_Luminance:		return GL_LUMINANCE;
		case ePixelFormat_LuminanceAlpha:	return GL_LUMINANCE_ALPHA;
		case ePixelFormat_RGB:				return GL_RGB;
		case ePixelFormat_RGBA:				return GL_RGBA;
		case ePixelFormat_BGR:				return GL_RGB;
		case ePixelFormat_BGRA:				return GL_RGBA;
		case ePixelFormat_Depth16:			return GL_DEPTH_COMPONENT16;
		case ePixelFormat_Depth24:			return GL_DEPTH_COMPONENT24;
		case ePixelFormat_Depth32:			return GL_DEPTH_COMPONENT32;
		case ePixelFormat_Alpha16:			return GL_ALPHA16F_ARB;
		case ePixelFormat_Luminance16:		return GL_LUMINANCE16F_ARB;
		case ePixelFormat_LuminanceAlpha16:	return GL_LUMINANCE_ALPHA16F_ARB;
		case ePixelFormat_RGB16:			return GL_RGB16F_ARB;
		case ePixelFormat_RGBA16:			return GL_RGBA16F_ARB;
		case ePixelFormat_Alpha32:			return GL_ALPHA32F_ARB;
		case ePixelFormat_Luminance32:		return GL_LUMINANCE32F_ARB;
		case ePixelFormat_LuminanceAlpha32: return GL_LUMINANCE_ALPHA32F_ARB;
		case ePixelFormat_RGB32:			return GL_RGB32F_ARB;
		case ePixelFormat_RGBA32:			return GL_RGBA32F_ARB;
		};

		return 0;
	}

	//-------------------------------------------------

	GLenum GetGLCompressionFormatFromPixelFormat(ePixelFormat aFormat)
	{
		;

		switch(aFormat)
		{
		case ePixelFormat_DXT1:				return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		case ePixelFormat_DXT3:				return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case ePixelFormat_DXT5:				return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		return 0;
	}

	//-------------------------------------------------

	GLenum TextureTypeToGLTarget(eTextureType aType)
	{
		switch(aType)
		{
		case eTextureType_1D:		return GL_TEXTURE_1D;	
		case eTextureType_2D:		return GL_TEXTURE_2D;
		case eTextureType_Rect:		return GL_TEXTURE_RECTANGLE_NV;
		case eTextureType_CubeMap:	return GL_TEXTURE_CUBE_MAP_ARB;
		case eTextureType_3D:		return GL_TEXTURE_3D;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLBlendEnum(eBlendFunc aType)
	{
		;

		switch(aType)
		{
		case eBlendFunc_Zero:					return GL_ZERO;
		case eBlendFunc_One:					return GL_ONE;
		case eBlendFunc_SrcColor:				return GL_SRC_COLOR;
		case eBlendFunc_OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
		case eBlendFunc_DestColor:				return GL_DST_COLOR;
		case eBlendFunc_OneMinusDestColor:		return GL_ONE_MINUS_DST_COLOR;
		case eBlendFunc_SrcAlpha:				return GL_SRC_ALPHA;
		case eBlendFunc_OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case eBlendFunc_DestAlpha:				return GL_DST_ALPHA;
		case eBlendFunc_OneMinusDestAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case eBlendFunc_SrcAlphaSaturate:		return GL_SRC_ALPHA_SATURATE;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureParamEnum(eTextureParam aType)
	{
		;

		switch(aType)
		{
		case eTextureParam_ColorFunc:		return GL_COMBINE_RGB_ARB;
		case eTextureParam_AlphaFunc:		return GL_COMBINE_ALPHA_ARB;
		case eTextureParam_ColorSource0:	return GL_SOURCE0_RGB_ARB;
		case eTextureParam_ColorSource1:	return GL_SOURCE1_RGB_ARB;
		case eTextureParam_ColorSource2:	return GL_SOURCE2_RGB_ARB;
		case eTextureParam_AlphaSource0:	return GL_SOURCE0_ALPHA_ARB;
		case eTextureParam_AlphaSource1:	return GL_SOURCE1_ALPHA_ARB;
		case eTextureParam_AlphaSource2:	return GL_SOURCE2_ALPHA_ARB;
		case eTextureParam_ColorOp0:		return GL_OPERAND0_RGB_ARB;
		case eTextureParam_ColorOp1:		return GL_OPERAND1_RGB_ARB;
		case eTextureParam_ColorOp2:		return GL_OPERAND2_RGB_ARB;
		case eTextureParam_AlphaOp0:		return GL_OPERAND0_ALPHA_ARB;
		case eTextureParam_AlphaOp1:		return GL_OPERAND1_ALPHA_ARB;
		case eTextureParam_AlphaOp2:		return GL_OPERAND2_ALPHA_ARB;
		case eTextureParam_ColorScale:		return GL_RGB_SCALE_ARB;
		case eTextureParam_AlphaScale:		return GL_ALPHA_SCALE;	
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureOpEnum(eTextureOp aType)
	{
		;

		switch(aType)
		{
		case eTextureOp_Color:			return GL_SRC_COLOR;
		case eTextureOp_OneMinusColor:	return GL_ONE_MINUS_SRC_COLOR;
		case eTextureOp_Alpha:			return GL_SRC_ALPHA;
		case eTextureOp_OneMinusAlpha:	return GL_ONE_MINUS_SRC_ALPHA;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureSourceEnum(eTextureSource aType)
	{
		;

		switch(aType)
		{
		case eTextureSource_Texture:	return GL_TEXTURE;
		case eTextureSource_Constant:	return GL_CONSTANT_ARB;
		case eTextureSource_Primary:	return GL_PRIMARY_COLOR_ARB;
		case eTextureSource_Previous:	return GL_PREVIOUS_ARB;
		}
		return 0;
	}
	//-----------------------------------------------------------------------

	GLenum GetGLTextureTargetEnum(eTextureType aType)
	{
		;

		switch(aType)
		{
		case eTextureType_1D:		return GL_TEXTURE_1D;	
		case eTextureType_2D:		return GL_TEXTURE_2D;
		case eTextureType_Rect:
			{
				return GL_TEXTURE_RECTANGLE_NV;
			}
		case eTextureType_CubeMap:	return GL_TEXTURE_CUBE_MAP_ARB;
		case eTextureType_3D:		return GL_TEXTURE_3D;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureCompareMode(eTextureCompareMode aMode)
	{
		;

		switch(aMode)
		{
		case eTextureCompareMode_None:			return GL_NONE;
		case eTextureCompareMode_RToTexture:	return GL_COMPARE_R_TO_TEXTURE;
		}
		return 0;
	}
	
	GLenum GetGLTextureCompareFunc(eTextureCompareFunc aFunc)
	{
		switch(aFunc)
		{
		case eTextureCompareFunc_LessOrEqual:		return GL_LEQUAL;
		case eTextureCompareFunc_GreaterOrEqual:	return GL_GEQUAL;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureFuncEnum(eTextureFunc aType)
	{
		;

		switch(aType)
		{
		case eTextureFunc_Modulate:		return GL_MODULATE;
		case eTextureFunc_Replace:		return GL_REPLACE;
		case eTextureFunc_Add:			return GL_ADD;
		case eTextureFunc_Substract:	return GL_SUBTRACT_ARB;
		case eTextureFunc_AddSigned:	return GL_ADD_SIGNED_ARB;
		case eTextureFunc_Interpolate:	return GL_INTERPOLATE_ARB;
		case eTextureFunc_Dot3RGB:		return GL_DOT3_RGB_ARB;
		case eTextureFunc_Dot3RGBA:		return GL_DOT3_RGBA_ARB;
		}
		return 0;
	}

	//-----------------------------------------------------------------------
	GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType)
	{
		;

		switch(aType)
		{
		case eDepthTestFunc_Never:			return GL_NEVER;
		case eDepthTestFunc_Less:				return GL_LESS;
		case eDepthTestFunc_LessOrEqual:		return GL_LEQUAL;
		case eDepthTestFunc_Greater:			return GL_GREATER; 
		case eDepthTestFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eDepthTestFunc_Equal:			return GL_EQUAL;
		case eDepthTestFunc_NotEqual:			return GL_NOTEQUAL;
		case eDepthTestFunc_Always:			return GL_ALWAYS; 
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType)
	{
		;

		switch(aType)
		{
		case eAlphaTestFunc_Never:			return GL_NEVER;
		case eAlphaTestFunc_Less:				return GL_LESS;
		case eAlphaTestFunc_LessOrEqual:		return GL_LEQUAL;
		case eAlphaTestFunc_Greater:			return GL_GREATER; 
		case eAlphaTestFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eAlphaTestFunc_Equal:			return GL_EQUAL;
		case eAlphaTestFunc_NotEqual:			return GL_NOTEQUAL;
		case eAlphaTestFunc_Always:			return GL_ALWAYS; 
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLStencilFuncEnum(eStencilFunc aType)
	{
		;

		switch(aType)
		{
		case eStencilFunc_Never:			return GL_NEVER;
		case eStencilFunc_Less:				return GL_LESS;
		case eStencilFunc_LessOrEqual:		return GL_LEQUAL;
		case eStencilFunc_Greater:			return GL_GREATER; 
		case eStencilFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eStencilFunc_Equal:			return GL_EQUAL;
		case eStencilFunc_NotEqual:			return GL_NOTEQUAL;
		case eStencilFunc_Always:			return GL_ALWAYS; 
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLStencilOpEnum(eStencilOp aType)
	{
		;

		switch(aType)
		{
		case eStencilOp_Keep:			return GL_KEEP; 
		case eStencilOp_Zero:			return GL_ZERO; 
		case eStencilOp_Replace:		return GL_REPLACE; 
		case eStencilOp_Increment:		return GL_INCR; 
		case eStencilOp_Decrement:		return GL_DECR;
		case eStencilOp_Invert:			return GL_INVERT;
		case eStencilOp_IncrementWrap:	return GL_INCR_WRAP_EXT;
		case eStencilOp_DecrementWrap:	return GL_DECR_WRAP_EXT;
		}
		return 0;
	}

	//-----------------------------------------------------------------------



}
