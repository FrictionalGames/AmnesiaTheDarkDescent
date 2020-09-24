
//! File: <Octree.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_OCTREE_H_
#define _HAPTX_OCTREE_H_

#include <vector>
#include "LinearAlgebra.h"

namespace HaptX
{
	// The octree is used by RealtimeTriangleSet and its sub-classes to optimise collision detection.
	class HAPTX_SOLVER_SPEC Octree
	{
	public:
		Octree();
		~Octree();

		bool		HasFaces();																													// Does this node or sub-nodes have any faces.
		void		Init(const Vec3f* vertices, int numVertices, int maxLevels, int maxTris, bool leftHanded);									// Create the octree, use maxTris = [30,50] => maxLevels = ((numVertices / 3) / (maxTris * 8)) + 1 (all faces are assumed to be triangles)
		void		Init(const Vec3f* vertices, const int* faces, int numVertices, int numFaces, int maxLevels, int maxTris, bool leftHanded);	// Create the octree, use maxTris = [30,50] => maxLevels = (numFaces / (maxTris * 8)) + 1 (all faces are assumed to be triangles)
		int			Init(void* Buffer);																											// Create the octree from a pre-generated buffer.
		int			Init(char* FileName);																										// Create the octree from a pre-generated file.
		int			SaveToBuffer(void* Buffer, long MaxLen, long& UsedLen);
		int			SaveToFile(char* FileName);
		long		CalcBufferSize();

	private:
		void		ComputeBoundingSphereAndBox(const Vec3f* vert, int num_vert);
		void		CreateNode(const Vec3f* vert, int num_vert, const int* faces, int num_faces, float* position, float* size, int* face_numbers);
		void		CreateLeafNode(bool* bleaf, const Vec3f* vert, int num_vert, const int* faces, int num_faces, float* position, float* size, int nodeID, int* face_numbers);
		float*		GetNodeCenter(float* cent, float* size, int nodeID);
		bool		TriangleNodeIntersect(float* v1, float* v2, float* v3, float* bmin, float* bmax);
		void		GetBoundingBox(float* bbox_min, float* bbox_max, float* p, float* s, int nodeID);
		int			SafeWriteMem(char** Buffer, long& MaxLen, long Size, void* Value, bool& bOverFlow);
		int			SafeReadMem	(char** Buffer, long Size, void* Value);
		int			WriteHeader(void* Buffer, long MaxLen);
		void*		FromBin(void* Buffer);
		int			ToBin(void* Buffer, long MaxLen, long& UsedLen, bool& bCountOnly);
	
	public:
		bool		m_isLeaf;				// Is this node a leaf node (the last node in the tree).
		float		m_mins[3];				// Bounding box min.
		float		m_maxs[3];				// Bounding box max.
		float		m_spherePos[3];			// Bounding sphere center.
		float		m_sphereRadius;			// Bounding sphere radius.
		float		m_newPos[3];

		int*		m_faceNumbers;			// Face numbers, note this is not the same as vertex index number.
		int			m_numFaces;				// Number of faces in this node (note that unless this is not a leaf node, n_face_numbers will be NULL).

		Octree*		m_subNodes[8];			// Sub-nodes.

	private:
		static int	ms_maxLevels;
		static int	ms_maxTris;
		static int	ms_currentLevel;
	};

	Octree* OctreeCopy(Octree* src);
	void HAPTX_SOLVER_SPEC OctreeCollisionTraverse(std::vector<int>& faceNumbers, Octree* pNode, const float* pmin, const float* pmax);				// Get all potential collision faces and put them into "faceNumbers", vertex_index_0 = faces[faceNumbers[A] * 3 + 0] ... (all faces are assumed to be triangles).
}

#endif
