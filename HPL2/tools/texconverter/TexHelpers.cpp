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

#include "hpl.h"

#include <Windows.h>

using namespace hpl;

extern bool RunProgram(const tWString& sPath, const tWString &sArg)
{
	STARTUPINFOW siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo); 

	//Build command line
	tWString sCommandLine = _W("")+sPath+_W(" ")+sArg;

	//Get the argument string
	wchar_t *pStr = new wchar_t[sCommandLine.size()];
	memcpy(pStr, sCommandLine.c_str(),(sCommandLine.size() + 1)*sizeof(wchar_t)); 

	BOOL lRet = CreateProcessW(0, pStr, 0,0,false,  CREATE_DEFAULT_ERROR_MODE,0,0,&siStartupInfo, &piProcessInfo);
	if(lRet==0)
	{
		return false;
	}

	//Wait for program to get finished
	int lExitCode = WaitForSingleObject(piProcessInfo.hProcess, (2 * 1000)); 
	while(lExitCode == WAIT_TIMEOUT)
	{
		lExitCode = WaitForSingleObject(piProcessInfo.hProcess, (2 * 1000)); 
	}
	
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread); 	
	
	delete [] pStr;

	return true;
}
