
//! File: <RealtimeTriangleSetTG.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMETRIANGLESETTG_H_
#define _HAPTX_REALTIMETRIANGLESETTG_H_

#include "RealtimeTriangleSet.h"
#include "RealtimeGeometryTG.h"

namespace HaptX
{
	// RealtimeTriangleSetTG is a RealtimeTriangleSet that is not 
	// textured. See also RealtimeGeometry
	class HAPTX_SOLVER_SPEC RealtimeTriangleSetTG : public RealtimeTriangleSet, public RealtimeGeometryTG 
	{
	public:
		RealtimeTriangleSetTG(const Vec3f& _ds, const Vec3f& _dt, const Vec3f& _dr, const Vec3f& _str_origin, const bool& _one_sided = false, const bool& _convex = false); // Constructor

		virtual void Transform(const Matrix4f& frw, const Matrix4f& inv);							// See RealtimeGeometry::Transform
		virtual void TransformTextures(const Matrix4f& xf);											// See RealtimeGeometry::TransformTextures
		virtual void GetTextureCoords(const ExclusionPlane& ep, const Vec3f& u, const Vec3f& v);	// See RealtimeGeometry::GetTextureCoords

		void CopyTriangles(RealtimeTriangleSetTG* dest);
	};
}

#endif
