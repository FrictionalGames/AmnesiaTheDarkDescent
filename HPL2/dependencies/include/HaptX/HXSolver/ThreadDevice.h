
//! File: <ThreadDevice.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_THREADDEVICE_H_
#define _HAPTX_THREADDEVICE_H_

#include "Device.h"
#include "Thread.h"

namespace HaptX 
{
	class HAPTX_SOLVER_SPEC ThreadDevice : public Device 
	{
	public:
		ThreadDevice();											// Constructor
		virtual ~ThreadDevice();								// Destructor
		
		virtual void	ThreadFunction();						// The ThreadFunction is called from the thread, override this function

		BOOL			Start(BOOL bSuspended = FALSE);			// Starts the thread
		void			End	();									// Ends the thread
		BOOL			SetPriority(int nPriority);				// Set priority of the thread
		void			Resume();
		BOOL			IsRunning();							// Check if the thread shall be terminated
		
	private:
		static void CALLBACK thread_function(CThread* pThread, VOID* pArg)
		{
			ThreadDevice* pThreadDevice = static_cast<ThreadDevice*>(pArg);
			pThreadDevice->ThreadFunction();
		}

	private:
		CThread m_thread;
	};
}

#endif
