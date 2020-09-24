
//! File: <RealtimeTriangleSetTPV.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMETRIANGLESETTPV_H_
#define _HAPTX_REALTIMETRIANGLESETTPV_H_

#include "RealtimeTriangleSet.h"

namespace HaptX
{
	// RealtimeTriangleSetTPV is a RealtimeTriangleSet that is Textured
	// Per Vertex.
	class HAPTX_SOLVER_SPEC RealtimeTriangleSetTPV : public RealtimeTriangleSet
	{
	public:
		inline RealtimeTriangleSetTPV(const bool& _one_sided = false, const bool& _convex = false) : RealtimeTriangleSet(_one_sided, _convex) {} // Constructor.

		virtual void		TransformTextures(const Matrix4f& xf);											// See RealtimeGeometry::TransformTextures
		virtual void		GetTextureCoords(const ExclusionPlane& ep, const Vec3f& u, const Vec3f& v);		// See RealtimeGeometry::GetTextureCoords
		inline void			AddTriangle(const int id, const Vec3f& a, const Vec3f& at, const Vec3f& b, const Vec3f& bt, const Vec3f& c, const Vec3f& ct, const Vec3f& normal);

		void				CopyTriangles(RealtimeTriangleSetTPV* dest);

	protected:
		struct HAPTX_SOLVER_SPEC TexTri
		{
			inline TexTri(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c) : a(_a), b(_b), c(_c) {}
			
			Vec3f a;			// The texture coordinate for the first vertex
			Vec3f b;			// The texture coordinate for the second vertex
			Vec3f c;			// The texture coordinate for the third vertex
		};

		std::vector<TexTri> m_texTris;
		std::vector<TexTri> m_originalTexTris;
	};

	inline void RealtimeTriangleSetTPV::AddTriangle(const int id, const Vec3f& a, const Vec3f& at, const Vec3f& b, const Vec3f& bt, const Vec3f& c, const Vec3f& ct, const Vec3f& normal)
	{
		m_tris.push_back(Tri(id,a,b,c,normal));
		m_originalTris.push_back(Tri(id,a,b,c,normal));
		m_texTris.push_back(TexTri(at,bt,ct));
		m_originalTexTris.push_back(TexTri(at,bt,ct));
	}
}

#endif
