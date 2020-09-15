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

#include "LuxBase.h"

//---------------------------------------
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//---------------------------------------

cLuxBase* gpBase = NULL;

//---------------------------------------

int hplMain(const tString &asCommandline)
{
	//////////////////////////
	// Init BlackBox
	#ifdef WIN32
		HINSTANCE hBlackBoxLib = LoadLibrary( "BlackBox.dll" );
	#endif


	//////////////////////////
	// Game creation and exit
	gpBase = hplNew( cLuxBase, ());

	//Init and run if all okay
	if(gpBase->Init(asCommandline))
	{
		gpBase->Run();
		gpBase->Exit();
	}
	//Error occurred
	else
	{
		if(gpBase->msErrorMessage==_W(""))
			gpBase->msErrorMessage = _W("Error occured");

		cPlatform::CreateMessageBox(_W("Error!"),gpBase->msErrorMessage.c_str());
		//No Exit, since it was not sure everything was created as it should.
	}

	hplDelete(gpBase);

	cMemoryManager::LogResults();

	//////////////////////////
	// Exit BlackBox
	#ifdef WIN32
			if(hBlackBoxLib) FreeLibrary(hBlackBoxLib);
	#endif

	return 0;
}
