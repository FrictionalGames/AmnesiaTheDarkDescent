
//! File: <MgMath.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MATH_H_
#define _HAPTX_MATH_H_

#include <float.h>

#define _USE_MATH_DEFINES 
#include <math.h>

namespace HaptX 
{
	// <NO_DOC>
	// The HaptX floating-point type. 
	typedef double mgFloat;

	inline mgFloat mgAtof(const char* a)
	{
		return atof(a);
	}

	inline mgFloat mgSqrt(const mgFloat& x)
	{
		return sqrt(x);
	}

	inline mgFloat mgAbs(const mgFloat& x)
	{
		return fabs(x);
	}

	inline mgFloat mgCos(const mgFloat& x) 
	{
		return cos(x);
	}

	inline mgFloat mgAcos(const mgFloat& x)
	{
		return acos(x);
	}

	inline mgFloat mgSin(const mgFloat& x)
	{
		return sin(x);
	}

	inline mgFloat mgAsin(const mgFloat& x)
	{
		return asin(x);
	}

	inline mgFloat mgTan(const mgFloat& x)
	{
		return tan(x);
	}

	inline mgFloat mgAtan(const mgFloat& x)
	{
		return atan(x);
	}

	inline mgFloat mgAtan2(const mgFloat& y, const mgFloat& x)
	{
		if (y == 0.0 && x == 0.0) return 0.0;
		return atan2(y,x);
	}

	inline mgFloat mgMod(const mgFloat& x, const mgFloat& y)
	{
		return fmod(x,y);
	}

	inline mgFloat mgFloor(const mgFloat& x)
	{
		return floor(x);
	}

	inline mgFloat mgCeil(const mgFloat& x)
	{
		return ceil(x);
	}

	inline mgFloat mgExp(const mgFloat& x)
	{
		return exp(x);
	}

	inline mgFloat mgLog(const mgFloat& x)
	{
		return log(x);
	}

	inline mgFloat mgPow(const mgFloat& x, const mgFloat& y)
	{
		return pow(x,y);
	}

	inline bool mgIsNan(const mgFloat& x)
	{
		return ((_isnan(x) != 0)? true : false);
	}
	// </NO_DOC>
}

#endif
