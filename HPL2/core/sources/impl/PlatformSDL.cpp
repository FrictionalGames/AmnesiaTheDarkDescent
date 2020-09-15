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

#include "system/Platform.h"

#include "system/String.h"

#include "system/LowLevelSystem.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include "impl/TimerSDL.h"
#include "impl/ThreadSDL.h"
#include "impl/MutexSDL.h"

#include <set>
#include <algorithm>

namespace hpl {
	//////////////////////////////////////////////////////////////////////////
	// APPLICATION
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	unsigned long cPlatform::GetApplicationTime()
	{
		return SDL_GetTicks();
	}

	//-----------------------------------------------------------------------

	void cPlatform::Sleep ( const unsigned int alMillisecs )
	{
		SDL_Delay ( alMillisecs );
	}

	void cPlatform::CopyTextToClipboard(const tWString &asText)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
        tString tstr = cString::S16BitToUTF8(asText);
        SDL_SetClipboardText(tstr.c_str());
#endif
	}

	//-----------------------------------------------------------------------

	tWString cPlatform::LoadTextFromClipboard()
	{
        tWString tstr;
#if SDL_VERSION_ATLEAST(2, 0, 0)
        if (SDL_HasClipboardText()) {
            // Gets utf8 encoded text
            char * clip = SDL_GetClipboardText();
            if (clip) {
                tstr = cString::UTF8ToWChar(clip);
                SDL_free(clip);
            }
        }
#endif
		return tstr;
	}

	void cPlatform::GetDisplayResolution(int alDisplay, int& alWidth, int& alHeight)
	{
	#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_DisplayMode desktop;
		SDL_GetDesktopDisplayMode(alDisplay, &desktop);
		alWidth = desktop.w;
		alHeight = desktop.h;
	#else
		alWidth = 1024;
		alHeight = 768;
	#endif
	}

	//-----------------------------------------------------------------------
	void cPlatform::GetAvailableVideoModes(tVideoModeVec& avDestVidModes, int alMinBpp, int alMinRefreshRate)
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
        int ndisplays = SDL_GetNumVideoDisplays();

        std::set<cVideoMode, VideoComp> uniqVideoModes;

        for (int d=0; d<ndisplays; ++d)
        {
            SDL_DisplayMode desktop;
            SDL_GetDesktopDisplayMode(d, &desktop);

            int nmodes = SDL_GetNumDisplayModes(d);
            for (int m = 0; m < nmodes; ++m)
            {
                SDL_DisplayMode mode;
                SDL_GetDisplayMode(d, m, &mode);

                if (SDL_BITSPERPIXEL(desktop.format) != (int)SDL_BITSPERPIXEL(mode.format))
                {
                    continue;
                }
                cVideoMode vidMode(
                                   d,
                                   cVector2l(mode.w, mode.h),
                                   SDL_BITSPERPIXEL(mode.format),
                                   1
                                   );
                uniqVideoModes.insert(vidMode);
            }
            // Add fullscreen desktop mode
            uniqVideoModes.insert(cVideoMode(d, cVector2l(0,0), SDL_BITSPERPIXEL(desktop.format), 1));
        }

        avDestVidModes.assign(uniqVideoModes.begin(), uniqVideoModes.end());
#else
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
        if (!info) return;
		SDL_Rect **modes = SDL_ListModes(info->vfmt, SDL_OPENGL | SDL_FULLSCREEN);
		avDestVidModes.clear();
		if (modes == NULL) return;
		for (int i=0; modes[i]; i++)
		{
			avDestVidModes.push_back(cVideoMode(cVector2l(modes[i]->w, modes[i]->h),info->vfmt->BitsPerPixel,1));
		}
		sort(avDestVidModes.begin(), avDestVidModes.end(), VideoComp());
#endif
	}

    tWString cPlatform::GetDisplayName(int alDisplay)
    {
        return cString::To16Char(SDL_GetDisplayName(alDisplay));
    }

#ifndef HPL_MINIMAL
	//-----------------------------------------------------------------------

	iTimer * cPlatform::CreateTimer()
	{
		return hplNew(cTimerSDL, () );
	}


	//////////////////////////////////////////////////////////////////////////
	// THREADING
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	iThread* cPlatform::CreateThread(iThreadClass* apThreadClass)
	{
		iThread* pThread = hplNew(cThreadSDL, ());
		pThread->SetThreadClass(apThreadClass);

		return pThread;
	}

	//-----------------------------------------------------------------------

	iMutex* cPlatform::CreateMutEx()
	{
		return hplNew(cMutexSDL, ());
	}
#endif
}
