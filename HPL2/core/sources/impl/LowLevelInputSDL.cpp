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

#include "impl/LowLevelInputSDL.h"

#include "impl/MouseSDL.h"
#include "impl/KeyboardSDL.h"
#include "impl/GamepadSDL.h"
#include "impl/GamepadSDL2.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

#include "engine/Engine.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#else
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#endif

#if defined WIN32 && !SDL_VERSION_ATLEAST(2,0,0)
#include <Windows.h>
#include <Dbt.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelInputSDL::cLowLevelInputSDL(iLowLevelGraphics *apLowLevelGraphics)
        : mpLowLevelGraphics(apLowLevelGraphics), mbQuitMessagePosted(false)
	{
		LockInput(true);
		RelativeMouse(false);
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
#else
//		mlConnectedDevices = 0;
//		mlCheckDeviceChange = 0;
//		mbDirtyGamepads = true;
//
		SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#endif
	}

	//-----------------------------------------------------------------------

	cLowLevelInputSDL::~cLowLevelInputSDL()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cLowLevelInputSDL::LockInput(bool abX)
	{
		mpLowLevelGraphics->SetWindowGrab(abX);
	}

	void cLowLevelInputSDL::RelativeMouse(bool abX)
	{
		mpLowLevelGraphics->SetRelativeMouse(abX);
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::BeginInputUpdate()
	{
		SDL_Event sdlEvent;

		mlstEvents.clear();
		while(SDL_PollEvent(&sdlEvent)!=0)
		{
#if defined WIN32 && !SDL_VERSION_ATLEAST(2,0,0)
			if(sdlEvent.type==SDL_SYSWMEVENT)
			{
				SDL_SysWMmsg* pMsg = sdlEvent.syswm.msg;
				
				// This is bad, cos it is actually Windows specific code, should not be here. TODO: move it, obviously
				if(pMsg->msg==WM_DEVICECHANGE)
				{
					if(pMsg->wParam==DBT_DEVICEARRIVAL)
					{
						cEngine::SetDeviceWasPlugged();
					}
					else if(pMsg->wParam==DBT_DEVICEREMOVECOMPLETE)
					{
						cEngine::SetDeviceWasRemoved();
					}
				}
			}
			else
#endif //WIN32
#if SDL_VERSION_ATLEAST(2, 0, 0)
            // built-in SDL2 gamepad hotplug code
            // this whole contract should be rewritten to allow clean adding/removing
            // of controllers, instead of brute force rescanning
            if (sdlEvent.type==SDL_CONTROLLERDEVICEADDED)
            {
                // sdlEvent.cdevice.which is the device #
                cEngine::SetDeviceWasPlugged();
            } else if (sdlEvent.type==SDL_CONTROLLERDEVICEREMOVED)
            {
                // sdlEvent.cdevice.which is the instance # (not device #).
                // instance # increases as devices are plugged and unplugged.
                cEngine::SetDeviceWasRemoved();
            }
#endif
#if defined (__APPLE__)
            if (sdlEvent.type==SDL_KEYDOWN)
            {
                if (sdlEvent.key.keysym.sym == SDLK_q && sdlEvent.key.keysym.mod & KMOD_GUI) {
                    mbQuitMessagePosted = true;
                } else {
                    mlstEvents.push_back(sdlEvent);
                }
            } else
#endif
            if (sdlEvent.type==SDL_QUIT)
            {
                mbQuitMessagePosted = true;
            } else
				mlstEvents.push_back(sdlEvent);
		}
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::EndInputUpdate()
	{
		
	}

	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::InitGamepadSupport()
	{
#if !SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);
#endif
	}

	void cLowLevelInputSDL::DropGamepadSupport()
	{
#if !SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
#endif
	}

	int cLowLevelInputSDL::GetPluggedGamepadNum()
	{
#if USE_XINPUT
		return cGamepadXInput::GetNumConnected();
#else
		return SDL_NumJoysticks();
#endif
	}

	//-----------------------------------------------------------------------
	
	iMouse* cLowLevelInputSDL::CreateMouse()
	{
		return hplNew( cMouseSDL,(this));
	}
	
	//-----------------------------------------------------------------------
	
	iKeyboard* cLowLevelInputSDL::CreateKeyboard()
	{
		return hplNew( cKeyboardSDL,(this) );
	}

	//-----------------------------------------------------------------------

	iGamepad* cLowLevelInputSDL::CreateGamepad(int alIndex)
	{
#if USE_SDL2
		return hplNew( cGamepadSDL2, (this, alIndex) );
#else
		return hplNew( cGamepadSDL, (this, alIndex) );
#endif
	}
	
	//-----------------------------------------------------------------------

    bool cLowLevelInputSDL::isQuitMessagePosted()
    {
        return mbQuitMessagePosted;
    }
    
    void cLowLevelInputSDL::resetQuitMessagePosted()
    {
        mbQuitMessagePosted = false;
    }

	//-----------------------------------------------------------------------
    
}
