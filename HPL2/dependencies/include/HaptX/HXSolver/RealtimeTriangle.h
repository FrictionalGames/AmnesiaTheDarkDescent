
//! File: <RealtimeTriangle.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMETRIANGLE_H_
#define _HAPTX_REALTIMETRIANGLE_H_

#include "RealtimeGeometry.h"

namespace HaptX
{  
	class RealtimeTriangleSet;
	class RealtimeMesh;

	// RealtimeTriangle is a textured triangle where the texture
	// coordinates are provided for each vertex.
	class HAPTX_SOLVER_SPEC RealtimeTriangle : public RealtimeGeometry 
	{
	public:
		RealtimeTriangle(const Vec3f& _a, const Vec3f& _at, const Vec3f& _b, const Vec3f& _bt, const Vec3f& _c, const Vec3f& _ct);// constructor.

		virtual void	Exclude(const Vec3f& p);	// Exclude determines local distance minima to the given point. These minima must be registered with a call to AddExclusionPlane.
		virtual void	Transform(const Matrix4f& frw, const Matrix4f& inv); // See RealtimeGeometry::Transform
		virtual void	TransformTextures(const Matrix4f& xf); // See RealtimeGeometry::TransformTextures
		virtual void	GetTextureCoords(const ExclusionPlane& ep, const Vec3f& u, const Vec3f& v); // See RealtimeGeometry::GetTextureCoords

	protected:
		Vec3f			a;		// The first vertex of the triangle.
		Vec3f			b;		// The second vertex of the triangle.
		Vec3f			c;		// The third vertex of the triangle.
		Vec3f			at;		// The texture coodinate for the first vertex.
		Vec3f			bt;		// The texture coodinate for the second vertex.
		Vec3f			ct;		// The texture coodinate for the third vertex.

		class HAPTX_SOLVER_SPEC TriState
		{
		public:
			inline TriState() : m_state(INIT) {}
			
			void		Init(const Vec3f& a, const Vec3f& b, const Vec3f& c);
			void		InitGeom(const Vec3f& a, const Vec3f& b, const Vec3f& c);
			void		Minimise(const Vec3f& a, const Vec3f& b, const Vec3f& c, Vec3f& q);

		public:
			enum state_e 
			{
				INIT = 0,
				S0	 = 1, 
				S1	 = 2, 
				S2	 = 3, 
				S3	 = 4, 
				S12	 = 5,
				S23	 = 6,
				S31	 = 7
			};
			
			state_e		m_state;

			mgFloat		uu;
			mgFloat		uv;
			mgFloat		vv;
			mgFloat		denom;
			mgFloat		u;
			mgFloat		v;
		};

		TriState tri;

		friend class RealtimeTriangleSet;
		friend class RealtimeMesh;
	};

	//! Constructor.
	inline RealtimeTriangle::RealtimeTriangle(const Vec3f& _a, const Vec3f& _at, const Vec3f& _b, const Vec3f& _bt, const Vec3f& _c, const Vec3f& _ct) :
	a(_a), at(_at), b(_b), bt(_bt), c(_c), ct(_ct) {}
}

#endif
