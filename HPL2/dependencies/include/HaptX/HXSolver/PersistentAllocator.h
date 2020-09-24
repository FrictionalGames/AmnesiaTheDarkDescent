
//! File: <PersistentAllocator.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_PERSISTENTALLOCATOR_H_
#define _HAPTX_PERSISTENTALLOCATOR_H_

#include "HXSolver.h"

namespace HaptX
{
	struct HAPTX_SOLVER_SPEC PersistentAllocator
	{
		void* operator new(size_t a);
		void operator delete(void*, size_t);
	};

	HAPTX_SOLVER_SPEC void ClearAndFreeObjectPool();
}

#endif
