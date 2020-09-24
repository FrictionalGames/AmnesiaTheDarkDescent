
//! File: <CollisionCallback.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _COLLISIONCALLBACK_H_
#define _COLLISIONCALLBACK_H_

#include "LinearAlgebra.h"
#include "HaptXDefines.h"

namespace HaptX
{
	class Surface;

	struct collisionResult_t
	{
		Surface* surface;			// the surface type

		// The ds and dt arguments are the derivatives of the texture coordinates with respect to the u and v axes of
		// the surface. Hence the s texture coordinate of the finger is given by
		// st_origin.x + ds.x * finger.x + ds.y * finger.y
		// and the t coordinate by
		// st_origin.y + dt.x * finger.x + dt.y * finger.y
		// Similarly the change in texture coordinate suggested by the proxy_movement will be
		// Vec2f(proxy_movement * ds, proxy_movement * dt)
		Vectors::Vec2f	ds;
		Vectors::Vec2f	dt;
		Vectors::Vec2f	st_origin;
		
		Vectors::Vec3f contactPoint;		// surface contact point
		Vectors::Vec3f normal;				// surface normal

		void operator = (const collisionResult_t& c)
		{
			surface			= c.surface;
			ds				= c.ds;
			dt				= c.dt;
			st_origin		= c.st_origin;
			contactPoint	= c.contactPoint;
			normal			= c.normal;
		}

		bool operator == (const collisionResult_t& c)
		{
			if (surface != c.surface)			return false;
			if (ds != c.ds)						return false;
			if (dt != c.dt)						return false;
			if (st_origin != c.st_origin)		return false;
			if (contactPoint != c.contactPoint)	return false;
			if (normal != c.normal)				return false;

			return true;
		}

		bool operator != (const collisionResult_t& c)
		{
			if (surface != c.surface)			return true;
			if (ds != c.ds)						return true;
			if (dt != c.dt)						return true;
			if (st_origin != c.st_origin)		return true;
			if (contactPoint != c.contactPoint)	return true;
			if (normal != c.normal)				return true;

			return false;
		}
	};

	// return value should be "true" if there is a collision else the function should return false
	// when there is a collision then the "ret" variable should be set
	typedef bool (*pfnCollisionCallbackFunc)(collisionResult_t& ret, const Vectors::Vec3f unconstrainedPosition, const Vectors::Vec3f constrainedPosition, const mgFloat radius);
};

#endif
