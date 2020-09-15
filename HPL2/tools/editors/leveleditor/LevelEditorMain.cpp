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

#include "../common/StdAfx.h"
using namespace hpl;

#include "LevelEditor.h"
#include "BuildID_LevelEditor.h"

#ifdef WITH_BINRELOC
#include "binreloc.h"
#endif

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

	//cMemoryManager::SetLogCreation(true);

	cLevelEditor* pEditor = hplNew(cLevelEditor, ());


	cEngine* pEngine = pEditor->Init(NULL, "LevelEditor", GetBuildID_LevelEditor(), false);

	pEngine->Run();

	hplDelete(pEditor);
	DestroyHPLEngine(pEngine);
	cMemoryManager::LogResults();

	//////////////////////////
	// Exit BlackBox
	#ifdef WIN32
			if(hBlackBoxLib) FreeLibrary(hBlackBoxLib);
	#endif

	return 0;
}



//---------------------------------------
