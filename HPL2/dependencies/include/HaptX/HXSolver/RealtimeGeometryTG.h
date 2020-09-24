
//! File: <RealtimeGeometryTG.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMEGEOMETRYTG_H_
#define _HAPTX_REALTIMEGEOMETRYTG_H_

#include "RealtimeGeometry.h"

namespace HaptX
{
	// RealtimeGeometryTG has no transform from RG space to texture space.
	class HAPTX_SOLVER_SPEC RealtimeGeometryTG
	{
	public:
		~RealtimeGeometryTG()	{}

		void Transform(const Matrix4f& frw, const Matrix4f& inv);	// See RealtimeGeometry::Transform
		void TransformTextures(const Matrix4f& xf);					// See RealtimeGeometry::TransformTextures
		// See RealtimeGeometry::GetTextureCoords
		inline void GetTextureCoords(const Vec3f& p, const RealtimeGeometry::texCoordMode_e& mode, 
									 const Vec3f& u, const Vec3f& v, Vec3f& tex_coord,
									 Vec2f& ds, Vec2f& dt, Vec2f& dr);
	};

	inline void RealtimeGeometryTG::GetTextureCoords(const Vec3f& p, const RealtimeGeometry::texCoordMode_e& mode, 
									 const Vec3f& u, const Vec3f& v, Vec3f& tex_coord,
									 Vec2f& ds, Vec2f& dt, Vec2f& dr)
	{
		tex_coord.r = 0.0;
		tex_coord.s = 0.0;
		tex_coord.t = 0.0;
		dr = Vec2f(0.0,0.0);
		ds = Vec2f(0.0,0.0);
		dt = Vec2f(0.0,0.0);
	}
};

#endif
