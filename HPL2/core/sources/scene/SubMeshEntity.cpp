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

#include "scene/SubMeshEntity.h"

#include "scene/MeshEntity.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"

#include "scene/AnimationState.h"
#include "scene/NodeState.h"

#include "physics/PhysicsBody.h"

#include "math/Math.h"

namespace hpl {
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	cSubMeshEntity::cSubMeshEntity(const tString &asName, cMeshEntity *apMeshEntity, cSubMesh * apSubMesh,
								cMaterialManager* apMaterialManager) : iRenderable(asName)
	{
		mpMeshEntity = apMeshEntity;
		mpSubMesh = apSubMesh;

		mbIsOneSided = mpSubMesh->GetIsOneSided();
		mvOneSidedNormal = mpSubMesh->GetOneSidedNormal();

		mpMaterialManager = apMaterialManager;

		mbGraphicsUpdated = false;

		if(mpMeshEntity->GetMesh()->GetSkeleton())
		{
			mpDynVtxBuffer = mpSubMesh->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware,eVertexBufferUsageType_Dynamic,eFlagBit_All);
			mvDynTriangles = *mpSubMesh->GetTriangleVecPtr();
		}
		else
		{
			mpDynVtxBuffer = NULL;
		}

		mpLocalNode = NULL;

		mpEntityCallback = hplNew( cSubMeshEntityBodyUpdate, () );
		mbUpdateBody = false;

		mpMaterial = NULL;

		mpUserData = NULL;

		//This is used to see if null should be returned.
		// 0 = no check made, test if matrix is identity
		// -1 = Matrix was not identity
		// 1 = matrix was identiy
		mlStaticNullMatrixCount =0;
	}

	cSubMeshEntity::~cSubMeshEntity()
	{
		hplDelete(mpEntityCallback);

		if(mpDynVtxBuffer) hplDelete(mpDynVtxBuffer);

		/* Clear any custom textures here*/	
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BODY CALLBACK
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	void cSubMeshEntityBodyUpdate::OnTransformUpdate(iEntity3D* apEntity)
	{
		/*cSubMeshEntity *pSubEntity = static_cast<cSubMeshEntity*>(apEntity);

		if(pSubEntity->GetBody())
		{
			if(apEntity->GetWorldMatrix() != pSubEntity->GetBody()->GetLocalMatrix())
			{
				Log("Setting matrix on %s from\n",pSubEntity->GetBody()->GetName().c_str());
				Log("  %s\n",apEntity->GetWorldMatrix().ToString().c_str());
				Log("  %s\n",pSubEntity->GetBody()->GetLocalMatrix().ToString().c_str());

				pSubEntity->GetBody()->SetMatrix(apEntity->GetWorldMatrix());
			}
		}*/
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSubMeshEntity::UpdateLogic(float afTimeStep)
	{
		
	}

	//-----------------------------------------------------------------------


	cMaterial* cSubMeshEntity::GetMaterial()
	{
		if(mpMaterial==NULL && mpSubMesh->GetMaterial()==NULL)
		{
			//Error("Materials for sub entity %s are NULL!\n",GetName().c_str());
		}

		if(mpMaterial) 
			return mpMaterial;
		else 
			return mpSubMesh->GetMaterial();
	}

	//-----------------------------------------------------------------------

	// Set Src as private variable to give this a little boost! Or?
	static inline void MatrixFloatTransformSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
		pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
		pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
	}

	static inline void MatrixFloatRotateSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
		pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
		pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
	}

	//-----------------------------------------------------------------------

	// Set Src as private variable to give this a little boost!Or?
	static inline void MatrixFloatTransformAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
		pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
		pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
	}

	static inline void MatrixFloatRotateAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
		pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
		pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::UpdateGraphicsForFrame(float afFrameTime)
	{
		////////////////////////////////////
		//Update things in parent first.
		mpMeshEntity->UpdateGraphicsForFrame(afFrameTime);

		////////////////////////////////////
		// If it has dynamic mesh, update it.
		if(mpDynVtxBuffer)
		{
			if(mpMeshEntity->mbSkeletonPhysicsSleeping && mbGraphicsUpdated)
			{
				return;
			}
			
			mbGraphicsUpdated = true;

			const float *pBindPos = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Position);
			const float *pBindNormal = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Normal);
			const float *pBindTangent = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Texture1Tangent);

			float *pSkinPos = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
			float *pSkinNormal = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Normal);
			float *pSkinTangent = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Texture1Tangent);

			const int lVtxStride = mpDynVtxBuffer->GetElementNum(eVertexBufferElement_Position);
			const int lVtxNum = mpDynVtxBuffer->GetVertexNum();

			for(int vtx=0; vtx < lVtxNum; vtx++)
			{
				//To count the bone bindings
				int lCount = 0;
				//Get pointer to weights and bone index.
				const float *pWeight = &mpSubMesh->mpVertexWeights[vtx*4];
				if(*pWeight==0) continue;

				const unsigned char *pBoneIdx = &mpSubMesh->mpVertexBones[vtx*4];

				const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];
				
				
				MatrixFloatTransformSet(pSkinPos,mtxTransform, pBindPos, *pWeight);

				MatrixFloatRotateSet(pSkinNormal,mtxTransform, pBindNormal, *pWeight);

				MatrixFloatRotateSet(pSkinTangent,mtxTransform, pBindTangent, *pWeight);

				++pWeight; ++pBoneIdx; ++lCount;

				//Iterate weights until 0 is found or count < 4
				while(*pWeight != 0 && lCount < 4)
				{
					//Log("Boneidx: %d Count %d Weight: %f\n",(int)*pBoneIdx,lCount, *pWeight);				
					const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];

					//Transform with the local movement of the bone.
					MatrixFloatTransformAdd(pSkinPos,mtxTransform, pBindPos, *pWeight);

					MatrixFloatRotateAdd(pSkinNormal,mtxTransform, pBindNormal, *pWeight);

					MatrixFloatRotateAdd(pSkinTangent,mtxTransform, pBindTangent, *pWeight);

					++pWeight; ++pBoneIdx; ++lCount;
				}

				pBindPos += lVtxStride;
				pSkinPos += lVtxStride;

				pBindNormal += 3;
				pSkinNormal += 3;

				pBindTangent += 4;
				pSkinTangent += 4;
			}

			//No stencil shadows:
			/*float *pSkinPosArray = mpDynVtxBuffer->GetArray(eVertexElementFlag_Position);
			if(mpMeshEntity->GetRenderFlagBit(eRenderableFlag_ShadowCaster))
			{
				//Update the shadow double
				memcpy(&pSkinPosArray[lVtxStride*lVtxNum],pSkinPosArray,sizeof(float)*lVtxStride*lVtxNum);
				for(int vtx=lVtxStride*lVtxNum + lVtxStride-1; vtx < lVtxStride*lVtxNum*2; vtx+=lVtxStride)
				{
					pSkinPosArray[vtx] = 0;
				}
			}*/

			//Update buffer
			mpDynVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Normal | eVertexElementFlag_Texture1,false);
			
			//No stencil shadows:
			/*if(mpMeshEntity->GetRenderFlagBit(eRenderableFlag_ShadowCaster))
			{
				//Update triangles
				cMath::CreateTriangleData(mvDynTriangles,
					mpDynVtxBuffer->GetIndices(), mpDynVtxBuffer->GetIndexNum(),
					pSkinPosArray, lVtxStride, lVtxNum);
			}*/
		}
		
	}

	//-----------------------------------------------------------------------


	iVertexBuffer* cSubMeshEntity::GetVertexBuffer()
	{
		if(mpDynVtxBuffer)
		{
			return mpDynVtxBuffer;
		}
		else
		{
			return mpSubMesh->GetVertexBuffer();
		}
	}

	//-----------------------------------------------------------------------


	cBoundingVolume* cSubMeshEntity::GetBoundingVolume()
	{
		if(mpMeshEntity->GetMesh()->GetSkeleton())
		{
			return mpMeshEntity->GetBoundingVolume();
		}	
		else
		{
			if(mbUpdateBoundingVolume)
			{
				mBoundingVolume.SetTransform(GetWorldMatrix());
				mbUpdateBoundingVolume = false;
			}

			return &mBoundingVolume;
		}
	}

	//-----------------------------------------------------------------------


	int cSubMeshEntity::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//-----------------------------------------------------------------------

	cMatrixf* cSubMeshEntity::GetModelMatrix(cFrustum *apFrustum)
	{
		////////////////////////
		// Static entity
		if(IsStatic() && mlStaticNullMatrixCount>=0)
		{
			if(mlStaticNullMatrixCount==0)
			{
				if(GetWorldMatrix() == cMatrixf::Identity) 	mlStaticNullMatrixCount = 1;
				else										mlStaticNullMatrixCount = -1;

				if(mlStaticNullMatrixCount == 1)return NULL;
				else							return &GetWorldMatrix();
					
			}
			else
			{
				return NULL;
			}
		}
		////////////////////////
		// Dynamic
		else
		{
			return &GetWorldMatrix();
		}
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SetLocalNode(cNode3D *apNode)
	{
		mpLocalNode = apNode;

		mpLocalNode->AddEntity(this);
	}

	//-----------------------------------------------------------------------

	cNode3D* cSubMeshEntity::GetLocalNode()
	{
		return mpLocalNode;
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SetUpdateBody(bool abX)
	{
		mbUpdateBody = abX;

		/*if(mbUpdateBody)
		{
			AddCallback(mpEntityCallback);
		}
		else
		{
			RemoveCallback(mpEntityCallback);
		}*/
	}
	
	bool cSubMeshEntity::GetUpdateBody()
	{
		return mbUpdateBody;
	}

	//-----------------------------------------------------------------------

	cTriangleData& cSubMeshEntity::GetTriangle(int alIndex)
	{ 
		if(mpDynVtxBuffer)
			return mvDynTriangles[alIndex];
		else
			return (*mpSubMesh->GetTriangleVecPtr())[alIndex];
	}
	int cSubMeshEntity::GetTriangleNum()
	{ 
		if(mpDynVtxBuffer)
			return (int)mvDynTriangles.size();
		else
			return (int)mpSubMesh->GetTriangleVecPtr()->size();
	}

	tTriangleDataVec* cSubMeshEntity::GetTriangleVecPtr()
	{ 
		if(mpDynVtxBuffer)
			return &mvDynTriangles;
		else
			return mpSubMesh->GetTriangleVecPtr();
	}
	
	//-----------------------------------------------------------------------
	
	void cSubMeshEntity::SetCustomMaterial(cMaterial *apMaterial, bool abDestroyOldCustom)
	{
		if(abDestroyOldCustom)
		{
			if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		}

        mpMaterial = apMaterial;
	}
	
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSubMeshEntity::OnTransformUpdated()
	{
		mpMeshEntity->mbUpdateBoundingVolume = true;
	}

	//-----------------------------------------------------------------------

}
