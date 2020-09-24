
// File: <MgTime.h>
// Copyright (c) 1997-2007, Reachin Technologies AB.
// All rights reserved.
// The copyright to the computer program herein is the property of Reachin
// Technologies AB, Sweden. The program may be used and/or copied only with the
// written permission of Reachin Technologies AB or in accordance with the terms
// stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MGTIME_H_
#define _HAPTX_MGTIME_H_

#include <stdlib.h>
#include <time.h>
#include <sys\timeb.h>
#include <math.h>

#include "HXSolver.h"

namespace HaptX
{
	// Time represents a double-precision floating point value specifying the time in 
	// seconds since the application started to a precision of around 10ms (but to an 
	// unspecified accuracy). Time uses the Windows' QueryPerformanceCounter() function.
	class HAPTX_SOLVER_SPEC Time
	{
	public:
		inline Time()							{ SetToNow(); }
		inline Time(double time) : t(time)		{}

		inline void SetToNow()					{ t = GetSecondsSinceEpoch(); }			
		inline bool InPast() const				{ return t < GetSecondsSinceEpoch(); }	
		inline bool InFuture() const			{ return t > GetSecondsSinceEpoch(); }	
		inline bool IsZero() const				{ return (abs(t) < 0.000001); }			// Indicates whether this time is equal to zero or not.
    
		inline operator double() const			{ return t; }							// Cast to double.

		bool operator < (const Time& rh) const	{ return t < rh.t; }
		bool operator > (const Time& rh) const	{ return t > rh.t; }
		bool operator <= (const Time& rh) const { return t <= rh.t; }
		bool operator >= (const Time& rh) const { return t >= rh.t; }
		bool operator == (const Time& rh) const { return (abs((t - rh.t)) < 0.000001); }
		bool operator != (const Time& rh) const { return (t != rh.t); }

	protected:
		double GetSecondsSinceEpoch() const;

	protected:
		double t;		// The time since the application started in seconds, to a precision of around 10ms.

		static bool				m_doneInit;
		static long double		ms_startTime;
		static LARGE_INTEGER	ms_lpCount;
		static LARGE_INTEGER	ms_startlpCount;
		static LARGE_INTEGER	ms_lpFreq;
	};
}

#endif
