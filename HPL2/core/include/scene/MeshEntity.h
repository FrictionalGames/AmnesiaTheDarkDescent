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

#ifndef HPL_MESH_ENTITY_H
#define HPL_MESH_ENTITY_H

#include <vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"
#include "math/MeshTypes.h"

#include "scene/SubMeshEntity.h"

#include "scene/AnimationState.h"

namespace hpl {

	class cMaterialManager;
	class cMeshManager;
	class cAnimationManager;
	class cMesh;
	class cSubMesh;
	class cMeshEntity;
	class cAnimation;
	class cAnimationState;
	class cNodeState;
	class cBone;
	class cNode3D;
	class iCollideShape;
	class iPhysicsBody;
	class iPhysicsWorld;
	class cWorld;

	//-----------------------------------------------------------------------

	class cMeshEntityRootNodeUpdate : public iEntityCallback
	{
	public:

		void OnTransformUpdate(iEntity3D * apEntity);
	};

	//------------------------------------------

	class cMeshEntityCallback
	{
	public:
		virtual void AfterAnimationUpdate(cMeshEntity *apMeshEntity, float afTimeStep)=0;
	};

	//------------------------------------------

	class cMeshEntity : public iEntity3D
	{
	#ifdef __GNUC__
		typedef iRenderable __super;
	#endif
	friend class cSubMeshEntity;
	friend class cMeshEntityRootNodeUpdate;
	friend class cMesh;
	public:
		cMeshEntity(const tString asName,cMesh* apMesh, cMaterialManager* apMaterialManager,
					cMeshManager* apMeshManager, cAnimationManager *apAnimationManager);
		~cMeshEntity();
	
		void SetWorld(cWorld *apWorld){ mpWorld = apWorld;}
		cWorld* GetWorld(){ return mpWorld;}

		cMesh* GetMesh(){ return mpMesh;}

		void SetCallback(cMeshEntityCallback *apCallback){mpCallback = apCallback;}

		//Sub mesh entities
		cSubMeshEntity* GetSubMeshEntity(unsigned int alIdx);
		cSubMeshEntity* GetSubMeshEntityName(const tString &asName);
		int GetSubMeshEntityNum();

		//Animation states
		cAnimationState* AddAnimation(cAnimation *apAnimation,const tString &asName, float afBaseSpeed);
		void ClearAnimations();

		cAnimationState* GetAnimationState(int alIndex);
		int GetAnimationStateIndex(const tString &asName);
		cAnimationState* GetAnimationStateFromName(const tString &asName);
		int GetAnimationStateNum();

		//Animation controller
		void Play(int alIndex,bool abLoop, bool bStopPrev);
		void PlayName(const tString &asName,bool abLoop, bool bStopPrev);
		void Stop();

		void PlayFadeTo(int alIndex,bool abLoop, float afTime);
		void PlayFadeToName(const tString &asName,bool abLoop, float afTime);

		bool AnimationIsOver(const tString &asName);

		void SetNormalizeAnimationWeights(bool abX){ mbNormalizeAnimationWeights = abX;}
		bool GetNormalizeAnimationWeights(){  return mbNormalizeAnimationWeights;}

		//Bone states
		cNode3D* GetBoneStateRoot(){ return mpBoneStateRoot;}
		
		cBoneState* GetBoneState(int alIndex);
		int GetBoneStateIndex(const tString &asName);
		int GetBoneStateIndexFromPtr(cBoneState* apBoneState);
		cBoneState* GetBoneStateFromName(const tString &asName);
		int GetBoneStateNum();

		//Skeleton physics
		void SetSkeletonPhysicsActive(bool abX);
		bool GetSkeletonPhysicsActive();

		void SetSkeletonPhysicsCanSleep(bool abX){mbSkeletonPhysicsCanSleep = abX;}
		bool GetSkeletonPhysicsCanSleep(){ return mbSkeletonPhysicsCanSleep;}

		float GetSkeletonPhysicsWeight();
		void SetSkeletonPhysicsWeight(float afX);

		void FadeSkeletonPhysicsWeight(float afTime);
		
		void SetSkeletonCollidersActive(bool abX);
		bool GetSkeletonCollidersActive();

		void AlignBodiesToSkeleton(bool abCalculateSpeed);

		
		/**
		 * Calculates the transform (and angles and postion if wanted) of a mesh based on the postion of the root bone.
		 * This is useful when going from rag doll to mesh.
		 * \param *apPostion Can be NULL, the postion
		 * \param *apAngles Can be NULL, the angles.
		 */
		cMatrixf CalculateTransformFromSkeleton(cVector3f *apPostion,cVector3f *apAngles);

		/**
		 * Checks collision with the skeletons collider boides
		 * \param *apWorld Physics world
		 * \param *apShape The shape
		 * \param &a_mtxShape The shapes matrix
		 * \param *apPosList A list of positions that all contact points are stored in. can be NULL.
		 * \param *apNumList A list of ints of number of the bone state body hit. can be NULL.
		 */
		bool CheckColliderShapeCollision(iPhysicsWorld *apWorld,
										iCollideShape *apShape, const cMatrixf &a_mtxShape,
										tVector3fList *apPosList, tIntList *apNumList);

		void ResetGraphicsUpdated();

		//Node states
		cNode3D* GetNodeState(int alIndex);
		int GetNodeStateIndex(const tString &asName);
		cNode3D* GetNodeStateFromName(const tString &asName);
		int GetNodeStateNum();
		
		//Entity implementation
		tString GetEntityType(){ return "MeshEntity";}
		bool IsVisible(){ return mbIsVisible; }
		void SetVisible(bool abVisible);

		cBoundingVolume* GetBoundingVolume();

		void SetStatic(bool abX);
		bool IsStatic(){ return mbStatic;}

		void SetRenderFlagBit(tRenderableFlag alFlagBit, bool abSet);
		//bool GetRenderFlagBit(tRenderableFlag alFlagBit){ return (mlRenderFlags & alFlagBit)!=0;} 
		//inline tRenderableFlag GetRenderFlags() const { return mlRenderFlags;}

		void SetIlluminationAmount(float afX);
		float GetIlluminationAmount(){ return mfIlluminationAmount; }

		void SetCoverageAmount(float afX);
		float GetCoverageAmount(){ return mfCoverageAmount;}

		void UpdateLogic(float afTimeStep);

		void UpdateGraphicsForFrame(float afFrameTime);

		void SetBody(iPhysicsBody* apBody){ mpBody = apBody;}
		iPhysicsBody* GetBody(){ return mpBody;}

		void SetUserData(void* apData) { mpUserData = apData; }
		void* GetUserData() { return mpUserData; }

	private:
		float GetAnimationWeightMul();

		void CreateNodes();

		void UpdateNodeMatrixRec(cNode3D *apNode);

		void HandleAnimationEvent(cAnimationEvent *apEvent);

		void SetBoneMatrixFromBodyRec(const cMatrixf& a_mtxParentWorld,cBoneState *apBoneState);

		void UpdateBVFromSkeleton();
		void GetAABBFromBones(cVector3f &avMin, cVector3f &avMax);

		void BuildBoneStatesRec(cBone *apBone, cNode3D *apParent);

		cMaterialManager* mpMaterialManager;
		cMeshManager* mpMeshManager;
		cAnimationManager *mpAnimationManager;

		cWorld *mpWorld;

		bool mbIsVisible;
		float mfIlluminationAmount;
		float mfCoverageAmount;
		tRenderableFlag mlRenderFlags;
		
		bool mbBoneMatricesNeedUpdate;
		int mlBoneMatricesTransformCount;

		cMatrixf m_mtxInvWorldMatrix;
		int mlInvWorldMatrixTransformCount;

		bool mbStatic;

		tSubMeshEntityVec mvSubMeshes;
		tSubMeshEntityMap m_mapSubMeshes;

		tAnimationStateVec mvAnimationStates;
		tAnimationStateIndexMap m_mapAnimationStateIndices;

		cNode3D *mpBoneStateRoot;

		tNodeStateVec mvBoneStates;
		tNodeStateIndexMap m_mapBoneStateIndices;
		tNodeStateVec mvTempBoneStates;

		std::vector<cMatrixf> mvBoneMatrices;

		bool mbSkeletonPhysics;
		bool mbSkeletonPhysicsFading;
		float mfSkeletonPhysicsFadeSpeed;
		float mfSkeletonPhysicsWeight;

		bool mbSkeletonPhysicsSleeping;
		bool mbSkeletonPhysicsCanSleep;
		
		bool mbSkeletonColliders;

		bool mbNormalizeAnimationWeights;

		bool mbUpdatedBones;
		bool mbHasUpdatedAnimation;

		tNodeStateVec mvNodeStates;
		tNodeStateIndexMap m_mapNodeStateIndices;

		cMesh* mpMesh;
		
		cMeshEntityCallback *mpCallback;

		iPhysicsBody *mpBody;

		void* mpUserData;
	};

	//-----------------------------------------------------------------------


};
#endif // HPL_MESH_ENTITY_H
