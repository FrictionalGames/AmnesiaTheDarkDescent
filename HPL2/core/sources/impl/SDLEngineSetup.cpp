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

#include "impl/SDLEngineSetup.h"

#include "system/System.h"
#include "input/Input.h"
#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "scene/Scene.h"
#include "sound/Sound.h"
#include "physics/Physics.h"
#include "ai/AI.h"
#include "haptic/Haptic.h"

#include "impl/KeyboardSDL.h"
#include "impl/MouseSDL.h"
#include "impl/LowLevelGraphicsSDL.h"
#include "impl/LowLevelResourcesSDL.h"
#include "impl/LowLevelSystemSDL.h"
#include "impl/LowLevelInputSDL.h"
#include "impl/LowLevelSoundFmod.h"
#include "impl/LowLevelSoundOpenAL.h"
#include "impl/LowLevelPhysicsNewton.h"

#ifdef INCLUDE_HAPTIC 
	#include "impl/LowLevelHapticHaptX.h"
#endif

#if USE_SDL2
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#else
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#endif
#ifdef WIN32
#include "Windows.h"
#include "Dbt.h"
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLEngineSetup::cSDLEngineSetup(tFlag alHplSetupFlags)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0");
#endif
		if(alHplSetupFlags & (eHplSetup_Screen | eHplSetup_Video))
		{
			if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0) {
				FatalError("Error Initializing Display: %s",SDL_GetError()); 
				exit(1);
			}
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_DisableScreenSaver();
#elif defined WIN32 // only on SDL1.2
			// Set up device notifications!
			// This is bad, cos it is actually Windows specific code, should not be here. TODO: move it, obviously
			SDL_SysWMinfo info;
			SDL_VERSION(&info.version);
			if(SDL_GetWMInfo(&info))
			{
				DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
				ZeroMemory(&notificationFilter, sizeof(notificationFilter));
 
				// set up filtering, so we only get notified of input device changes
				notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
				static const GUID GuidDevInterfaceHID = {0x745a17a0, 0x74d3, 0x11d0,
															{ 0xb6, 0xfe, 0x00, 0xa0, 0xc9, 0x0f, 0x57, 0xda }};
				notificationFilter.dbcc_classguid = GuidDevInterfaceHID;

				notificationFilter.dbcc_size = sizeof(notificationFilter);
 
				HDEVNOTIFY hDevNotify;
				hDevNotify = RegisterDeviceNotification(info.window, &notificationFilter,
					DEVICE_NOTIFY_WINDOW_HANDLE |
					DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
 
				if(hDevNotify == NULL) {
				}
			}
#endif // WIN32
		}
		else
		{
			SDL_Init( SDL_INIT_TIMER );
		}
		
		//////////////////////////
		// System
		mpLowLevelSystem = hplNew( cLowLevelSystemSDL, () );
		
		//////////////////////////
		// Graphics
		mpLowLevelGraphics = hplNew( cLowLevelGraphicsSDL,() );
		
		//////////////////////////
		// Input
		mpLowLevelInput = hplNew( cLowLevelInputSDL,(mpLowLevelGraphics) );
		
		//////////////////////////
		// Resources
		mpLowLevelResources = hplNew( cLowLevelResourcesSDL,(mpLowLevelGraphics) );
		
		//////////////////////////
		// Sound
		mpLowLevelSound	= hplNew( cLowLevelSoundOpenAL,() );
		
		//////////////////////////
		// Physics
		mpLowLevelPhysics = hplNew( cLowLevelPhysicsNewton,() );
		
		//////////////////////////
		// Haptic
#ifdef INCLUDE_HAPTIC 
		mpLowLevelHaptic = hplNew( cLowLevelHapticHaptX,() );
#else 
		mpLowLevelHaptic = NULL;
#endif
		
	}

	//-----------------------------------------------------------------------

	cSDLEngineSetup::~cSDLEngineSetup()
	{
		Log("- Deleting lowlevel stuff.\n");
		
		Log("  Physics\n");
		hplDelete(mpLowLevelPhysics);
		Log("  Sound\n");
		hplDelete(mpLowLevelSound);
		Log("  Input\n");
		hplDelete(mpLowLevelInput);
		Log("  Resources\n");
		hplDelete(mpLowLevelResources);
		Log("  System\n");
		hplDelete(mpLowLevelSystem);
		Log("  Graphics\n");
		hplDelete(mpLowLevelGraphics);
		Log("  Haptic\n");
#ifdef INCLUDE_HAPTIC 	
		hplDelete(mpLowLevelHaptic);
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_EnableScreenSaver();
#endif
		SDL_Quit();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cScene* cSDLEngineSetup::CreateScene(cGraphics* apGraphics, cResources *apResources, cSound* apSound,
										cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cGui *apGui,
										cHaptic *apHaptic)
	{
		cScene *pScene = hplNew( cScene, (apGraphics,apResources, apSound,apPhysics, apSystem,apAI,apGui,apHaptic) );
		return pScene;
	}

	//-----------------------------------------------------------------------

	
	/**
	 * \todo Lowlevelresource and resource both use lowlevel graphics. Can this be fixed??
	 * \param apGraphics 
	 * \return 
	 */
	cResources* cSDLEngineSetup::CreateResources(cGraphics* apGraphics)
	{
		cResources *pResources = hplNew( cResources, (mpLowLevelResources,mpLowLevelGraphics) );
		return pResources;
	}
	
	//-----------------------------------------------------------------------

	cInput* cSDLEngineSetup::CreateInput(cGraphics* apGraphics)
	{
		cInput *pInput = hplNew( cInput, (mpLowLevelInput) );
		return pInput;
	}
	
	//-----------------------------------------------------------------------

	cSystem* cSDLEngineSetup::CreateSystem()
	{
		cSystem *pSystem = hplNew( cSystem, (mpLowLevelSystem) );
		return pSystem;
	}
	
	//-----------------------------------------------------------------------

	cGraphics* cSDLEngineSetup::CreateGraphics()
	{
		cGraphics *pGraphics = hplNew( cGraphics, (mpLowLevelGraphics,mpLowLevelResources) );
		return pGraphics;
	}
	//-----------------------------------------------------------------------
	
	cSound* cSDLEngineSetup::CreateSound()
	{
		cSound *pSound = hplNew( cSound, (mpLowLevelSound) );
		return pSound;
	}
	
	//-----------------------------------------------------------------------
	
	cPhysics* cSDLEngineSetup::CreatePhysics()
	{
		cPhysics *pPhysics = hplNew( cPhysics, (mpLowLevelPhysics) );
		return pPhysics;
	}

	//-----------------------------------------------------------------------

	cAI* cSDLEngineSetup::CreateAI()
	{
		cAI *pAI = hplNew( cAI,() );
		return pAI;
	}

	//-----------------------------------------------------------------------

	cHaptic* cSDLEngineSetup::CreateHaptic()
	{
		cHaptic *pHaptic = hplNew( cHaptic, (mpLowLevelHaptic) );
		return pHaptic;
	}

	//-----------------------------------------------------------------------

}
