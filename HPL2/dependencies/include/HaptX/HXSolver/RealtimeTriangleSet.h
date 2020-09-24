
//! File: <RealtimeTriangleSet.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMETRIANGLESET_H_
#define _HAPTX_REALTIMETRIANGLESET_H_

#include <map>

#include "RealtimeTriangle.h"
#include "Octree.h"

namespace HaptX
{
	namespace RTSInternal
	{
		void HAPTX_SOLVER_SPEC		SetEPD(mgFloat v);
		mgFloat HAPTX_SOLVER_SPEC	GetEPD();
	}

	class HAPTX_SOLVER_SPEC RealtimeTriangleSet : public RealtimeGeometry
	{
	public:	
		inline RealtimeTriangleSet(const bool& _one_sided = false, const bool& _convex = false); // Constructor.
		
		virtual ~RealtimeTriangleSet()
		{
			delete m_octreeRoot;
		}

		inline void					AddTriangle(const int id, const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& normal);
		inline void					DeleteTriangles();

		virtual void				Exclude(const Vec3f& p);													// See RealtimeGeometry::Exclude
		virtual void				Transform(const Matrix4f& frw, const Matrix4f& inv);						// See RealtimeGeometry::Transform
		virtual void				TransformTextures(const Matrix4f& xf);										// See RealtimeGeometry::TransformTextures
		virtual void				GetTextureCoords(const ExclusionPlane& ep, const Vec3f& u, const Vec3f& v);	// See RealtimeGeometry::GetTextureCoords

		void						InitOctree(const HaptX::Vectors::Vec3f* vertices, int numVertices, int maxLevels, int maxTris, bool leftHanded);
		void						InitOctree(const HaptX::Vectors::Vec3f* vertices, const int* faces, int numVertices, int numFaces, int maxLevels, int maxTris, bool leftHanded);
		int							InitOctree(void* Buffer);
		void						OctreeTraverse(std::vector<int>& face_num);
		int							OctTreeCalcCacheSize();
		int							OctreeToCache(void* Buffer, long MaxSize, long& ActSize);

		bool						IsOneSided();																// Returns true if the polygons are one sided

	protected:
		
		typedef RealtimeTriangle::TriState TriState;					// The triangles.
		
		inline virtual TriState		GetTriState(int id);				// Returns the TriState for the triangle with the given id.
		inline virtual int			GetTriangleIndex(int id);
		inline void					DoEP(const Vec3f& p, const Vec3f& np, const mgFloat& max_ep_d2, const int id);

		struct HAPTX_SOLVER_SPEC Tri
		{
			inline					Tri(const int _id, const Vec3f& _a, const Vec3f& _b, const Vec3f& _c, const Vec3f& _normal) : id(_id),a(_a), b(_b), c(_c), normal(_normal) {} // constructor.
			
			Vec3f					a;					// The first vertex.
			Vec3f					b;					// The second vertex.
			Vec3f					c;					// The third vertex.
			Vec3f					normal;				// The normal.
			int						id;					// The original index of the triangle, i.e. the index of the triangle as specified in the index list.
			TriState				state;				// The distance-minimization state of the triangle.
		};

	protected:
		Octree*						m_octreeRoot;		// A pointer to the octree root.
		Vec3f						m_octreeP;
		Matrix4f					m_octreeM;
		bool						m_oneSided;			// Specifies if the triangles should be one-sided.
		bool						m_convex;
		std::vector<Tri>			m_tris;
		std::vector<Tri>			m_originalTris;
	};

	//! Constructor
	inline RealtimeTriangleSet::RealtimeTriangleSet(const bool& _one_sided, const bool& _convex) : m_oneSided(_one_sided), m_convex(_convex)
	{
		m_octreeRoot = 0;
		m_octreeP = HaptX::Vectors::Vec3f(0.0,0.0,0.0);
		m_octreeM.SetToIdentity();
	}

	inline void RealtimeTriangleSet::AddTriangle(const int id, const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& normal)
	{
		m_originalTris.push_back(Tri(id, a, b, c, normal));
		m_tris.push_back(Tri(id, a, b, c, normal));
	}

	inline void RealtimeTriangleSet::DeleteTriangles()
	{
		m_tris.clear();
		m_originalTris.clear();
	}

	inline  RealtimeTriangle::TriState RealtimeTriangleSet::GetTriState(int id)
	{
		return m_tris[id].state;
	}

	inline int RealtimeTriangleSet::GetTriangleIndex(int id)
	{
		return m_tris[id].id;
	}
}

#endif
