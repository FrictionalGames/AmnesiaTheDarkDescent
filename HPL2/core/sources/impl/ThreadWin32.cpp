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

#include "impl/ThreadWin32.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cThreadWin32::cThreadWin32()
	{
		mpThreadHandle = NULL;
	}

	//-----------------------------------------------------------------------

	cThreadWin32::~cThreadWin32()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cThreadWin32::Start()
	{
		mbThreadActive = true;
		mpThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)iThread::MainThreadFunc, (LPVOID)this, 0, NULL);
	}
	
	void cThreadWin32::Stop()
	{
		mbThreadActive = false;
		WaitForSingleObject(mpThreadHandle,INFINITE);
		mpThreadHandle = NULL;
	}

	void cThreadWin32::Sleep(unsigned int alSleepTime)
	{
		WaitForSingleObject(mpThreadHandle, alSleepTime);
	}

	void cThreadWin32::SetPriority(eThreadPrio aPrio)
	{
		iThread::SetPriority(aPrio);

		SetThreadPriority(mpThreadHandle, TranslateEnginePrio(aPrio));
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int cThreadWin32::TranslateEnginePrio(eThreadPrio aPrio)
	{
		int lPrio = THREAD_PRIORITY_NORMAL;
		switch(aPrio)
		{
		case eThreadPrio_Low:
			lPrio = THREAD_PRIORITY_LOWEST;
			break;
		case eThreadPrio_High:
			lPrio = THREAD_PRIORITY_HIGHEST;
			break;
		}

		return lPrio;
	}


	//-----------------------------------------------------------------------

}

