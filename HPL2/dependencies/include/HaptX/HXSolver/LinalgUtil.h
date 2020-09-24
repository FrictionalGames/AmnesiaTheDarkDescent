
//! File: <LinalgUtil.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

//! general linear algebra utilities.

#ifndef _HAPTX_LINALGUTIL_H_
#define _HAPTX_LINALGUTIL_H_

#include "LinearAlgebra.h"
#include "Error.h"

namespace HaptX
{
	// svdDecompose calculates a singular value decomposition of the given matrix a.
	// template parameter TT must be indexable with a[i] producing a value of type T.
	// template parameter T must be indexable with a[i][j] producing an mgFloat lvalue 
	// (i is the row and j is the column).
	SVDDecomposition HAPTX_SOLVER_SPEC SvdDecompose(const Matrix3f& m) throw(...);
	
	// orthogonal returns an arbitrary vector orthogonal to the
	// given one. We require that the given vector is non-zero. 
	inline Vec3f Orthogonal(const Vec3f& v)
	{
		if (mgAbs(v.x) < mgAbs(v.y))
		{
			if (mgAbs(v.x) < mgAbs(v.z)) return Vec3f(1.0,0.0,0.0) % v;
			return Vec3f(0.0,0.0,1.0) % v;
		} 
		else
		{
			if (mgAbs(v.y) < mgAbs(v.z)) return Vec3f(0.0,1.0,0.0) % v;
			else return Vec3f(0.0,0.0,1.0) % v;
		}
	}

	// if two vectors are provided we produce a vector orthogonal to
	// both. the given vectors must be non-zero and non-colinear.
	inline Vec3f Orthogonal(const Vec3f& v1, const Vec3f& v2)
	{
		return v1 % v2;
	}
}

#endif
