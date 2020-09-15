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

#include "impl/ThreadSDL.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#include <typeinfo>
#else
#include "SDL/SDL.h"
#include <pthread.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cThreadSDL::cThreadSDL()
	{
		mpThreadHandle = NULL;
	}

	//-----------------------------------------------------------------------

	cThreadSDL::~cThreadSDL()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cThreadSDL::Start()
	{
		mbThreadActive = true;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        mpThreadHandle = SDL_CreateThread(iThread::MainThreadFunc, typeid(this).name(), this);
#else
		mpThreadHandle = SDL_CreateThread(iThread::MainThreadFunc, this);
#endif
	}

	//-----------------------------------------------------------------------
	
	void cThreadSDL::Stop()
	{
		mbThreadActive = false;
		SDL_WaitThread(mpThreadHandle,0);
		mpThreadHandle = NULL;
	}

	//-----------------------------------------------------------------------

	void cThreadSDL::Sleep(unsigned int alSleepTime)
	{
		SDL_Delay(alSleepTime);
	}

	//-----------------------------------------------------------------------

	void cThreadSDL::SetPriority(eThreadPrio aPrio)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
        int prio = TranslateEnginePrio(aPrio);
        SDL_SetThreadPriority((SDL_ThreadPriority)prio);
#else
		/** investigate further on other linux distros and mac os x
		 * As right now Priority range seems to be 0 .. 0 in linux */
#   ifdef __linux__
		struct sched_param param;
		param.sched_priority = TranslateEnginePrio(aPrio);
		int ret = pthread_setschedparam(SDL_GetThreadID(mpThreadHandle),SCHED_OTHER,&param);
#   endif
#endif
		iThread::SetPriority(aPrio);
	}

	//-----------------------------------------------------------------------

	int cThreadSDL::TranslateEnginePrio(eThreadPrio aPrio)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
        switch (aPrio) {
            case eThreadPrio_Low:
                return SDL_THREAD_PRIORITY_LOW;
            case eThreadPrio_High:
                return SDL_THREAD_PRIORITY_HIGH;
            case eThreadPrio_Normal:
            default:
                return SDL_THREAD_PRIORITY_NORMAL;
		}
#else
		switch (aPrio) {
		case eThreadPrio_Low:
		case eThreadPrio_Normal:
		case eThreadPrio_High:
		default:
			return 0;
		}
#endif
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

}

