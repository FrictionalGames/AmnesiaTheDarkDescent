
//! File: <Util.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_UTIL_H_
#define _HAPTX_UTIL_H_

#include <vector>

#include "HXSolver.h"
#include "MgMath.h"

namespace HaptX 
{
	// We define a Util nested namespace so that it is clear where utility 
	// constants are coming from when used elsewhere, e g Util::pi
	namespace Util 
	{
		static const mgFloat pi         = 3.141592654f;
		static const mgFloat two_pi     = 6.28318530718f;
		static const mgFloat half_pi    = 1.57079632679f;
		static const mgFloat third_pi   = 1.0471975512f;
		static const mgFloat quarter_pi = 0.785398163397f;
		static const mgFloat sixth_pi   = 0.523598775598f;
		static const mgFloat epsilon    = 1e-10;
		static const mgFloat huge_value = 1e10;
	}
}

#endif
