
//! File: <RefCounter.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REFCOUNTER_H_
#define _HAPTX_REFCOUNTER_H_

#include "HXSolver.h"
#include "Lock.h"

namespace HaptX
{
	static Lock refLock;
	
	// RefCounter counts references from the HaptX loop.
	class HAPTX_SOLVER_SPEC RefCounter
	{
	public:
		RefCounter()
		{
			m_hapticReferences = 0;
		}

		virtual ~RefCounter()
		{
		}

		inline void IncreaseHapticRef()
		{
			refLock.Enter();
			m_hapticReferences++;
			refLock.Leave();
		}

		inline void DecreaseHapticRef()
		{
			refLock.Enter();
			m_hapticReferences--;
			refLock.Leave();	
		}

		inline int GetHapticRef()
		{
			return m_hapticReferences;
		}

	protected:
		int m_hapticReferences;
	};
}

#endif
