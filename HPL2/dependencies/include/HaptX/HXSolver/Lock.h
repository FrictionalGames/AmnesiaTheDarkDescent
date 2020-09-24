
//! File: <Lock.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_LOCK_H_
#define _HAPTX_LOCK_H_

#include <StrStream>
#include <SStream>
#include <windows.h>

#include "HXSolver.h"

namespace HaptX
{
  	// Lock is a small wrapper class around Windows Critical Sections. 
	// It performs mutual exclusion between multiple threads.
    class HAPTX_SOLVER_SPEC Lock
	{
    public:
		Lock();						// Constructor
		Lock(const Lock& lock);		// Copy constructor. Do not copy, do same as default

        // Assignment operator. Do not copy, do same as default 
	    // Locks are not supposed to be assigned, but it might happen when 
	    // putting them in vectors
		Lock& operator = (const Lock&); 

		~Lock();					// Destructor

		// Enter critical section
		void Enter();

		// EnterNoWait attempts to lock, if the lock is not 
		// immediately available we return false without blocking, true otherwise.
		BOOL EnterNoWait();

		// Leave Critical Section
		void Leave();

    protected:
		CRITICAL_SECTION critical_section;

	private:
		static void sched_yield();
    };

  	
    // Initalizes a critical section object for thread synchronization.
    class HAPTX_SOLVER_SPEC CLockData
    {
    public:
	    CLockData();
	    ~CLockData();

	public:
	    CRITICAL_SECTION m_CS;
    };


    // Locks an CLockData object at construction
	// The lock is released upon destruction
    class HAPTX_SOLVER_SPEC CLock
    {
    public:
	    CLock(CLockData* lock);
	    ~CLock();

    private:
	    CLockData* m_pLockData;
    };
}

#endif
