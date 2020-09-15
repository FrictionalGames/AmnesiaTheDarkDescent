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

#include "ParticleEditor.h"
#include "BuildID_ParticleEditor.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


//////////////////////////////////////////////////////////////////////////
// APP ENTRY POINT
//////////////////////////////////////////////////////////////////////////


int hplMain(const tString& asCommandLine)
{
	//////////////////////////
	// Init BlackBox
	#ifdef WIN32
		HINSTANCE hBlackBoxLib = LoadLibrary( "BlackBox.dll" );
	#endif

	cParticleEditor* pEditor = hplNew(cParticleEditor, ());
	cEngine* pEngine = pEditor->Init(NULL, "ParticleEditor", GetBuildID_ParticleEditor(), false);

	pEngine->Run();

	hplDelete(pEditor);
	hplDelete(pEngine);

	cMemoryManager::LogResults();

	//////////////////////////
	// Exit BlackBox
	#ifdef WIN32
			if(hBlackBoxLib) FreeLibrary(hBlackBoxLib);
	#endif

	return 0;
}

