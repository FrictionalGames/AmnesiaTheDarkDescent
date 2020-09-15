/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_SUB_MESH_H
#define HPL_SUB_MESH_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "math/MeshTypes.h"
#include "physics/PhysicsTypes.h"

namespace hpl {

	class cMaterial;
	class iVertexBuffer;

	class cMesh;
	class iPhysicsWorld;
	class iCollideShape;

	class cMaterialManager;

	//--------------------------------------------------

	class cMeshCollider
	{
	public:
		tString msGroup; // Only used as temp var when loading!

		eCollideShapeType mType;
		cVector3f mvSize;
		cMatrixf m_mtxOffset;
		bool mbCharCollider;
	};

	typedef std::vector<cMeshCollider*> tMeshColliderVec;
	typedef tMeshColliderVec::iterator tMeshColliderVecIt;

	//--------------------------------------------------

	class cSubMesh
	{
	friend class cMesh;
	friend class cSubMeshEntity;
	public:
		cSubMesh(const tString &asName,cMaterialManager* apMaterialManager);
		~cSubMesh();

		void SetMaterial(cMaterial* apMaterial);
		void SetVertexBuffer(iVertexBuffer* apVtxBuffer);

		//Renderable implementation.
		cMaterial *GetMaterial();
		iVertexBuffer* GetVertexBuffer();

		const tString& GetName(){ return msName;}

		//Vertex-Bone pairs
		void ResizeVertexBonePairs(int alSize);
		int GetVertexBonePairNum();
		cVertexBonePair& GetVertexBonePair(int alNum);

		void AddVertexBonePair(const cVertexBonePair &aPair);
		void ClearVertexBonePairs();

		//Colliders
		cMeshCollider* CreateCollider(eCollideShapeType aType);
		cMeshCollider* GetCollider(int alIdx);
		int GetColliderNum();
		iCollideShape* CreateCollideShape(iPhysicsWorld *apWorld);
		static iCollideShape* CreateCollideShapeFromCollider(cMeshCollider *pCollider, iPhysicsWorld *apWorld, const cVector3f& avSizeMul, cMatrixf *apMtxOffset);

		void SetIsCollideShape(bool abX){mbCollideShape = abX;}
		bool IsCollideShape(){ return mbCollideShape;}

		const cTriEdge& GetEdge(int alIndex) const{ return mvEdges[alIndex];}
		int GetEdgeNum(){ return (int)mvEdges.size();}

		tTriEdgeVec* GetEdgeVecPtr(){ return &mvEdges;}

		tTriangleDataVec* GetTriangleVecPtr(){ return &mvTriangles;}

		void SetDoubleSided(bool abX){ mbDoubleSided = abX;}
		bool GetDoubleSided(){ return mbDoubleSided;}

		void SetModelScale(const cVector3f& avScale){ mvModelScale = avScale;}
		cVector3f GetModelScale(){ return mvModelScale;}

		const cMatrixf& GetLocalTransform(){ return m_mtxLocalTransform;}
		void SetLocalTransform(const cMatrixf& a_mtxTrans){ m_mtxLocalTransform = a_mtxTrans;}

		bool GetIsOneSided(){ return mbIsOneSided;}
		const cVector3f& GetOneSidedNormal(){ return mvOneSidedNormal;}
		const cVector3f& GetOneSidedPoint(){ return mvOneSidedPoint;}

		void SetMaterialName(const tString& asName){msMaterialName =asName;}
		const tString& GetMaterialName(){ return msMaterialName;}
		
		void Compile();
	private:
		void CheckOneSided();
		void CompileBonePairs();

		tString msName;
		
		tString msMaterialName;
		cMaterial* mpMaterial;
		iVertexBuffer* mpVtxBuffer;

		cMatrixf m_mtxLocalTransform;

		tVertexBonePairVec mvVtxBonePairs;

		tMeshColliderVec mvColliders;

		float *mpVertexWeights;
		unsigned char *mpVertexBones;

		tTriEdgeVec mvEdges;
		tTriangleDataVec mvTriangles;

		cVector3f mvModelScale;

		bool mbDoubleSided;

		bool mbCollideShape;

		bool mbIsOneSided;
		cVector3f mvOneSidedNormal;
		cVector3f mvOneSidedPoint;

		cMaterialManager* mpMaterialManager;

		cMesh* mpParent;
	};

};
#endif // HPL_SUB_MESH_H
