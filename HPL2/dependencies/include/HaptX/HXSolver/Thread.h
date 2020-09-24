
//! File: <Thread.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_CTHREAD_H_
#define _HAPTX_CTHREAD_H_

#include <stdio.h>
#include <process.h>
#include "HXSolver.h"

namespace HaptX 
{
	class CThread;
	typedef void(CALLBACK* ThreadFunction)(CThread* pThread, VOID* lParam);

	class HAPTX_SOLVER_SPEC CThread
	{
	public:
		CThread();
		virtual ~CThread();
		
		BOOL						IsRunning();
		BOOL						StartThread(ThreadFunction address, VOID* lParam, BOOL bCreateSuspended = FALSE);
		BOOL						SetPriority(int nPriority);
		void						Resume();
		void						CloseThread();
		
	protected:
		BOOL						Init();
		static unsigned __stdcall	InternalThread(void* pArg);

	protected:
		unsigned					m_uThreadID;
		HANDLE						m_hThread;
		HANDLE						m_hClose;
		HANDLE						m_hClosed;
		VOID*						m_plParam;
		BOOL						m_bSuspended;

		ThreadFunction				m_pThreadFunc;
	};
}

#endif
