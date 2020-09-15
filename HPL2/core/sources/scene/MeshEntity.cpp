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

#include "scene/MeshEntity.h"

#include "resources/Resources.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/AnimationManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"

#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "graphics/BoneState.h"

#include "scene/AnimationState.h"
#include "scene/NodeState.h"
#include "scene/Scene.h"
#include "scene/World.h"
#include "scene/SoundEntity.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsWorld.h"

#include "math/Math.h"

#include "engine/Engine.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshEntity::cMeshEntity(const tString asName,cMesh* apMesh, cMaterialManager* apMaterialManager,
							cMeshManager* apMeshManager, cAnimationManager *apAnimationManager) : 
							iEntity3D(asName)
	{
		mpMaterialManager = apMaterialManager;
		mpMeshManager = apMeshManager;
		mpAnimationManager = apAnimationManager;

		mpWorld = NULL;

		mpCallback = NULL;

		mpMesh = apMesh;

		mpBoneStateRoot = NULL;
		
		mpBody=NULL;

		mbApplyTransformToBV = false;

		mbIsVisible = true;
		mfIlluminationAmount = 1.0f;
		mfCoverageAmount = 1.0f;
		mlRenderFlags =0;

		mlInvWorldMatrixTransformCount = -1;
		mlBoneMatricesTransformCount = -1;

		mbBoneMatricesNeedUpdate = true;

		mbStatic = false;

		mbSkeletonPhysics = false;
		mfSkeletonPhysicsWeight = 1.0f;
		mbSkeletonPhysicsFading = false;
		mfSkeletonPhysicsFadeSpeed = 1.0f;
		
		mbSkeletonPhysicsSleeping = false;
		mbSkeletonPhysicsCanSleep = true;
		
		mbSkeletonColliders = false;

		mbNormalizeAnimationWeights = true;

		mbUpdatedBones = false;
		mbHasUpdatedAnimation = true;

		////////////////////////////////////////////////
		//Create sub entities
		for(int i=0;i<mpMesh->GetSubMeshNum();i++)
		{
			cSubMesh *pSubMesh = mpMesh->GetSubMesh(i);
			cSubMeshEntity* pSub = hplNew( cSubMeshEntity, (asName + "_" + pSubMesh->GetName(),this,pSubMesh,mpMaterialManager) );

			mvSubMeshes.push_back(pSub);
			m_mapSubMeshes.insert(tSubMeshEntityMap::value_type(mpMesh->GetSubMesh(i)->GetName(), pSub));
			
			iVertexBuffer *pVtxBuffer = mpMesh->GetSubMesh(i)->GetVertexBuffer();
			
			pSub->mBoundingVolume.AddArrayPoints(pVtxBuffer->GetFloatArray(eVertexBufferElement_Position), pVtxBuffer->GetVertexNum());
			pSub->mBoundingVolume.CreateFromPoints(pVtxBuffer->GetElementNum(eVertexBufferElement_Position));
		}
				
		////////////////////////////////////////////////
		//Create animation states from mesh.
		mvAnimationStates.reserve( mpMesh->GetAnimationNum());
		for(int i=0; i<  mpMesh->GetAnimationNum(); i++)
		{
			cAnimation* pAnimation = mpMesh->GetAnimation(i);
			
			cAnimationState* pAnimState = hplNew( cAnimationState, (pAnimation,pAnimation->GetName(),NULL) );

			mvAnimationStates.push_back(pAnimState);

			tAnimationStateIndexMap::value_type value(pAnimState->GetName(), (int)mvAnimationStates.size()-1);
			m_mapAnimationStateIndices.insert(value);
		}

		
		////////////////////////////////////////////////
		// Create Skeleton
		if(mpMesh->GetSkeleton())
		{
			//Use bones in skeleton to create nodes
			cSkeleton *pSkeleton = mpMesh->GetSkeleton();

			//Create the root node and attach all node without parents to this.
			mpBoneStateRoot = hplNew( cNode3D, ("BoneStateRoot",false) );
				
			//Create the root callback
			AddNodeChild(mpBoneStateRoot);
			
			//Reserve space for bones to be added
			mvBoneStates.reserve(pSkeleton->GetBoneNum());
            
			//////////////////////////////////
			//Fill the state array with the bones so
			//that each state has the same index as the bones.
			for(int i=0; i< pSkeleton->GetBoneNum(); i++)
			{
				cBone* pBone = pSkeleton->GetBoneByIndex(i);

				cBoneState *pState = hplNew( cBoneState,(pBone->GetName(), false) );
				pState->SetMatrix(pBone->GetLocalTransform());

				//Add bone to array and add it's index to the map.
				mvBoneStates.push_back(pState);
				m_mapBoneStateIndices.insert(tBoneIdxNameMap::value_type(pState->GetName(),i));
			}

			//////////////////////////////////
			//Set parents and children of the nodes in the array
			for(int i=0; i< (int)mvBoneStates.size();i++)
			{
				cNode3D* pState = mvBoneStates[i];
				cBone* pBone = pSkeleton->GetBoneByIndex(i);

				//Set the parent if there is one
				//TODO: Perhaps this should be removed.
				if(pBone->GetParent())
				{
					cNode3D *pParentState = GetBoneStateFromName(pBone->GetParent()->GetName());
					if(pParentState)
						pState->SetParent(pParentState);
					else
						pState->SetParent(mpBoneStateRoot);
				}
				else
				{
					pState->SetParent(mpBoneStateRoot);
				}
			}

			//Create an array to fill with bone matrices
			mvBoneMatrices.resize(pSkeleton->GetBoneNum());

			//////////////////////////////////
			//Reset all bones states
			for(size_t i=0;i < mvBoneStates.size(); i++)
			{
				cNode3D *pState = mvBoneStates[i];
				cBone* pBone = mpMesh->GetSkeleton()->GetBoneByIndex((int)i);

				pState->SetMatrix(pBone->GetLocalTransform());
			}

			//////////////////////////////////
			//Create temp bone nodes
			mvTempBoneStates.resize(mvBoneStates.size());
			for(size_t i=0;i < mvTempBoneStates.size(); i++)
			{
				mvTempBoneStates[i] = hplNew( cBoneState,(mvBoneStates[i]->GetName(),false));
			}

			//////////////////////////////////
			//Iterate and attach the sub meshes
			for(size_t i=0; i< mvSubMeshes.size(); ++i)
			{
				cSubMeshEntity *pSubEnt = mvSubMeshes[i];

				AddChild(pSubEnt);
			}

		}
		////////////////////////////////////////////////
		// No skeleton but animations. Create nodes and attach submeshes to them.
		else if(mvAnimationStates.empty()==false)
		{
			CreateNodes();	
		}
		////////////////////////////////////////////////
		// No skeleton or animation. Attach sub meshes directly!
		else
		{
			//Iterate and attach the sub meshes
			for(size_t i=0; i< mvSubMeshes.size(); ++i)
			{
				cSubMesh *pSubMesh = mpMesh->GetSubMesh((int)i);
				cSubMeshEntity *pSubEnt = mvSubMeshes[i];
				
				//If the mesh has nodes, search for right one and set matrix of submesh to it
				if(mpMesh->GetNodeNum() >0)
				{
					cNode3D* pNode = mpMesh->GetNodeByName(pSubMesh->GetName());
					if(pNode)
						pSubEnt->SetMatrix(pNode->GetWorldMatrix());
					else
						Error("Cannot find node '%s' in mesh '%s'\n",pSubMesh->GetName().c_str(), mpMesh->GetName().c_str());
				}
				//Attach the submesh
				AddChild(pSubEnt);
			}
		}
	}

	//-----------------------------------------------------------------------

	cMeshEntity::~cMeshEntity()
	{
		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			cSubMeshEntity *pSub = mvSubMeshes[i];

			//Remove from parent
			iEntity3D *pParent = pSub->GetEntityParent();
			if(pParent) pParent->RemoveChild(pSub);

			hplDelete(pSub);
		}

		if(mpBoneStateRoot)
		{
			//RemoveNodeChild(mpBoneStateRoot);
			hplDelete(mpBoneStateRoot);
		}
		
		mpMeshManager->Destroy(mpMesh);
		
		STLDeleteAll(mvNodeStates);
		STLDeleteAll(mvBoneStates);
		
		STLDeleteAll(mvTempBoneStates);

		STLDeleteAll(mvAnimationStates);
	}

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	
	//-----------------------------------------------------------------------
	
	//This functions sets the matrices of the bones according the matrices of
	//the body that is attached to each bone.
	//To get max speed all this is done recursivly from the root and then down.
	//To use function, iterate notes of root bone and call this function for each.
	void cMeshEntity::SetBoneMatrixFromBodyRec(const cMatrixf& a_mtxParentWorld,cBoneState *apBoneState)
	{
		iPhysicsBody *pBody = apBoneState->GetBody();
		
		if(pBody)
		{
			cMatrixf mtxBoneWorld = cMath::MatrixMul(pBody->GetWorldMatrix(),apBoneState->GetInvBodyMatrix());
			cMatrixf mtxParentInv = cMath::MatrixInverse(a_mtxParentWorld);

			apBoneState->SetMatrix(cMath::MatrixMul(mtxParentInv,mtxBoneWorld),false);

			cNode3DIterator BoneIt = apBoneState->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cBoneState *pBoneState = static_cast<cBoneState*>(BoneIt.Next());
				SetBoneMatrixFromBodyRec(mtxBoneWorld, pBoneState);
			}
		}
		else
		{
			apBoneState->UpdateWorldTransform();
			const cMatrixf &mtxBoneWorld = apBoneState->GetWorldMatrix();
			
			cNode3DIterator BoneIt = apBoneState->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cBoneState *pBoneState = static_cast<cBoneState*>(BoneIt.Next());

				SetBoneMatrixFromBodyRec(mtxBoneWorld, pBoneState);
			}
		}
	}

	//-----------------------------------------------------------------------
	

	/**
	 * This function will make less work be made in UpdateMatrix(...), else that might demand alot!
	 */
	void cMeshEntity::UpdateNodeMatrixRec(cNode3D *apNode)
	{
		if(apNode->IsActive())
		{
			apNode->UpdateMatrix(false);
		}

		apNode->ApplyPreAnimTransform(false);
		apNode->ApplyPostAnimTransform(false);
		

		//Should not be needed at all:
		//apNode->UpdateWorldTransform();
		//const cMatrixf& mtxWorld = apNode->GetWorldMatrix();

		cNode3DIterator NodeIt = apNode->GetChildIterator();
		while(NodeIt.HasNext())
		{
			cNode3D *pChildNode = static_cast<cNode3D*>(NodeIt.Next());

			UpdateNodeMatrixRec(pChildNode);
		}
	}
	
	//-----------------------------------------------------------------------
	
	void cMeshEntity::UpdateLogic(float afTimeStep)
	{	
		if(mbStatic) return; //No update on static models

		/////////////////////////////////////////////
		//Update the skeleton physics fade
		if(mbSkeletonPhysicsFading && mbSkeletonPhysics)
		{
			mfSkeletonPhysicsWeight -= afTimeStep * mfSkeletonPhysicsFadeSpeed;
			if(mfSkeletonPhysicsWeight <= 0)
			{
				mbSkeletonPhysicsFading = false;
				mfSkeletonPhysicsWeight = 1.0f;
				SetSkeletonPhysicsActive(false);
			}
		}
		
		/////////////////////////////////////////////
		//Check if all bodies connected to the skeleton is at rest,
		//If so we can skip skinning the body and simply just use the mesh as is.
		//(has some problems so turned off at the moment)
		mbSkeletonPhysicsSleeping = false;
		if(mbSkeletonPhysics && mfSkeletonPhysicsWeight==1.0f && mbSkeletonPhysicsCanSleep)
		{
			bool bEnabled = false;
			for(int bone =0; bone< GetBoneStateNum(); ++bone)
			{
				cBoneState *pState = GetBoneState(bone);
				iPhysicsBody *pBody = pState->GetBody();
				
				if(pBody && pBody->GetEnabled()){
					bEnabled = true;
					break;
				}
			}
			if(bEnabled == false){
				//mbSkeletonPhysicsSleeping = true;
			}
		}
		/////////////////////////////////////////////
		//Update animations and skeleton physics
		if(mvAnimationStates.empty()==false || mbSkeletonPhysics)
		{
			////////////////////////
			//Check if it is animated
			bool bAnimationActive = false;
			for(size_t i=0; i< mvAnimationStates.size(); i++)
			{
				if(mvAnimationStates[i]->IsActive()){
					bAnimationActive = true;
					break;
				}
			}

			//////////////////////////////////////
			// SKELETON
			if(mpMesh->GetSkeleton())
			{
				//If transform needs to be updated.
				bool bUpdateTransform = false;

				//////////
				//Reset all bones states
				if(	bAnimationActive || mbUpdatedBones == false ||
					(mbSkeletonPhysics && !mbSkeletonPhysicsSleeping))
				{
					for(size_t i=0;i < mvBoneStates.size(); i++)
					{
						cNode3D *pState = mvBoneStates[i];
						cBone* pBone = mpMesh->GetSkeleton()->GetBoneByIndex((int)i);

						if(pState->IsActive())
						{
							pState->SetMatrix(pBone->GetLocalTransform(),false);
						}
						
						//can optimize this by doing it in the order of the tree
						//and using recursive. (should be enough as is...)
						if(mbSkeletonPhysics && mfSkeletonPhysicsWeight!=1.0f)
						{
							mvTempBoneStates[i]->SetMatrix(pBone->GetLocalTransform(),false);
						}
					}

					bUpdateTransform = true;
				}

				///////////////////////////
				// Update skeleton physics
				if(	mbSkeletonPhysics && (!mbSkeletonPhysicsSleeping || mbUpdatedBones==false))
				{
					mbUpdatedBones = true;
					cNode3DIterator BoneIt = mpBoneStateRoot->GetChildIterator();
					while(BoneIt.HasNext())
					{
						cBoneState *pBoneState = static_cast<cBoneState*>(BoneIt.Next());
	                    
						SetBoneMatrixFromBodyRec(mpBoneStateRoot->GetWorldMatrix(),pBoneState);
					}
					
					//Interpolate matrices
					if(mfSkeletonPhysicsWeight!=1.0f)
					{	
						for(size_t i=0;i < mvBoneStates.size(); i++)
						{
							cMatrixf mtxMixLocal = cMath::MatrixSlerp(	mfSkeletonPhysicsWeight,
																		mvTempBoneStates[i]->GetLocalMatrix(),
																		mvBoneStates[i]->GetLocalMatrix(),
																		true);
							
							mvBoneStates[i]->SetMatrix(mtxMixLocal, false);
						}
					}
				}

				//////////////////////////////////
				//Go the weight mul (in case weights are normalized!)
				float fAnimationWeightMul = GetAnimationWeightMul();

				//////////////////////////////////
				//Go through all animations states and update the bones 
				for(size_t i=0; i< mvAnimationStates.size(); i++)
				{
					cAnimationState *pAnimState = mvAnimationStates[i];

					if(pAnimState->IsActive())
					{
						cAnimation *pAnim = pAnimState->GetAnimation();

						/////////////////////////////////////
						//Go through all tracks in animation and apply to nodes
						for(int i=0; i<pAnim->GetTrackNum(); i++)
						{
							cAnimationTrack *pTrack = pAnim->GetTrack(i);
							
							///////////////////////////////////
							//If index not yet set, get it!
							if(pTrack->GetNodeIndex()==-1)
							{
								int lBoneIdx = mpMesh->GetSkeleton()->GetBoneIndexByName(pTrack->GetName());
								if(lBoneIdx==-1)
								{
									// XXX: This line is commented to avoid log clutter 
									//Error("Track '%s' in '%s' does not have a corresponding bone! Skeleton bone name mismatch?\n", pTrack->GetName().c_str(), mpMesh->GetName().c_str());
									pTrack->SetNodeIndex(-2);
								}
								else
									pTrack->SetNodeIndex(lBoneIdx);
							}
							
							cNode3D* pState = GetBoneState(pTrack->GetNodeIndex());
							
							///////////////////////////////////
							//Apply the animation track to node.
							if(pState && pState->IsActive())
							{
								pTrack->ApplyToNode(pState,pAnimState->GetTimePosition(),pAnimState->GetWeight() * fAnimationWeightMul, pAnimState->IsLooping());
							}
						}

					
						pAnimState->Update(afTimeStep);
					}
				}
				
				//////////////////////////////////
				//Go through all states and update the matrices (and thereby adding the animations together).
				if(bAnimationActive)
				{
					cNode3DIterator NodeIt = mpBoneStateRoot->GetChildIterator();
					while(NodeIt.HasNext())
					{
						cNode3D *pBoneState = static_cast<cNode3D*>(NodeIt.Next());
						UpdateNodeMatrixRec(pBoneState);
					}

					//Entities are updated after BV is calculated, as the entity has the rootnode attached to it.
				}

				////////////////////////////
				//Update attached entities
				if(bAnimationActive || mbSkeletonPhysics)
				{
					for(size_t i=0;i < mvBoneStates.size(); i++)
					{
						mvBoneStates[i]->UpdateEntityChildren();
					}
				}
				
				//////////////////////////////////
				//Update the colliders if they are active
				//Note this must be done after all bone states are updated.
				if(mbSkeletonColliders && mbSkeletonPhysics==false)
				{
					for(size_t i=0;i < mvBoneStates.size(); i++)
					{
						cBoneState *pState = mvBoneStates[i];
						iPhysicsBody *pColliderBody = pState->GetColliderBody();

						if(pColliderBody)
						{
							cMatrixf mtxBody = cMath::MatrixMul(pState->GetWorldMatrix(), pState->GetBodyMatrix());
							pColliderBody->SetMatrix(mtxBody);
						}
					}
				}

				/////////////////////////////////////
				//Update the sub entity transform, so that they are updated in the renderable container.
				if(bUpdateTransform)
				{
					for(size_t i=0; i<mvSubMeshes.size(); ++i)
					{
						mvSubMeshes[i]->SetTransformUpdated(true);
					}
				}
				
			}
			//////////////////////////
			// NODES
			else
			{
				//////////////////////////////
				//Animation is being played
				if(bAnimationActive)
				{
					//Reset all state matrices
					for(size_t i=0;i < mvNodeStates.size(); i++)
					{
						cNode3D *pState = mvNodeStates[i];
						if(pState->IsActive()) pState->SetMatrix(cMatrixf::Identity);
					}

					//////////////////////////////////
					//Go the weight mul (in case weights are normalized!)
					float fAnimationWeightMul = GetAnimationWeightMul();

					/////////////////////////
					//Go through all animations states and set the node's
					for(size_t i=0; i< mvAnimationStates.size(); i++)
					{
						cAnimationState *pAnimState = mvAnimationStates[i];
						if(pAnimState->IsActive())
						{
							cAnimation *pAnim = pAnimState->GetAnimation();

							for(int i=0; i<pAnim->GetTrackNum(); i++)
							{
								cAnimationTrack *pTrack = pAnim->GetTrack(i);

								if(pTrack->GetNodeIndex()<0)
								{
									pTrack->SetNodeIndex(GetNodeStateIndex(pTrack->GetName()));
								}
								cNode3D* pNodeState = GetNodeState(pTrack->GetNodeIndex());

								if(pNodeState->IsActive()) 
									pTrack->ApplyToNode(pNodeState,pAnimState->GetTimePosition(),pAnimState->GetWeight() * fAnimationWeightMul);
							}

							pAnimState->Update(afTimeStep);
						}
					}

					//////////////////////
					//Go through all states and update the matrices (and thereby adding the animations together).
				
					tNode3DListIt nodeIt = mlstNodeChildren.begin();
					for(; nodeIt != mlstNodeChildren.end(); ++nodeIt)
					{
						cNode3D *pNodeState = *nodeIt;

						UpdateNodeMatrixRec(pNodeState);
					}

					mbHasUpdatedAnimation = true;
				}
				//////////////////////////////
				//No animation is played, only do this if an animation has been played.
				else if(mbHasUpdatedAnimation)
				{
					//Reset all state matrices
					for(size_t i=0;i < mvNodeStates.size(); i++)
					{
						cNode3D *pState = mvNodeStates[i];
						cNode3D* pMeshNode = mpMesh->GetNode((int)i);
						if(pState->IsActive()) pState->SetMatrix(pMeshNode->GetLocalMatrix());
					}
					mbHasUpdatedAnimation = false;
				}
			}
			
			//////////////////
			//Call callback
			if(mpCallback )mpCallback->AfterAnimationUpdate(this,afTimeStep);
		}
				
		/////////////////////////////////////////
		/// Update Sub Entities
		for(size_t i=0; i<mvSubMeshes.size(); ++i)
		{
			cSubMeshEntity *pSub = mvSubMeshes[i];

			pSub->UpdateLogic(afTimeStep);
		}
		
		/////////////////////////////////////////
		/// Update animation events
		if(mvAnimationStates.empty()==false)
		{
			for(size_t i=0; i< mvAnimationStates.size(); ++i)
			{
				cAnimationState *pState = mvAnimationStates[i];

				if(pState->IsActive()==false || pState->IsPaused()) continue;

				for(int j=0; j < pState->GetEventNum(); ++j)
				{
					cAnimationEvent *pEvent = pState->GetEvent(j);

					if(	pEvent->mfTime >= pState->GetPreviousTimePosition() &&
						pEvent->mfTime < pState->GetTimePosition())
					{
						HandleAnimationEvent(pEvent);
					}
				}
			}
		}

		/////////////////////////////////////////
		/// Final things
		if(mpMesh->GetSkeleton()) mbBoneMatricesNeedUpdate = true;
	}

	//-----------------------------------------------------------------------

	cAnimationState* cMeshEntity::AddAnimation(cAnimation *apAnimation,const tString &asName, float afBaseSpeed)
	{
		/////////////////////////////
		// Create nodes. Will only be done once
		CreateNodes();

		/////////////////////////////
		// Create and set up animation state
		cAnimationState* pAnimState = hplNew( cAnimationState, (apAnimation,asName,mpAnimationManager) );
		
		pAnimState->SetBaseSpeed(afBaseSpeed);
		
		mvAnimationStates.push_back(pAnimState);

		tAnimationStateIndexMap::value_type value(pAnimState->GetName(), (int)mvAnimationStates.size()-1);
		m_mapAnimationStateIndices.insert(value);

		return pAnimState;
	}

	void cMeshEntity::ClearAnimations()
	{
		STLDeleteAll(mvAnimationStates);
	}

	//-----------------------------------------------------------------------

	cAnimationState* cMeshEntity::GetAnimationState(int alIndex)
	{
		return mvAnimationStates[alIndex];
	}
	int cMeshEntity::GetAnimationStateIndex(const tString &asName)
	{
		tAnimationStateIndexMapIt it = m_mapAnimationStateIndices.find(asName);
		if(it != m_mapAnimationStateIndices.end())
		{
			return it->second;
		}
		else
		{
			return -1;
		}
	}
	cAnimationState* cMeshEntity::GetAnimationStateFromName(const tString &asName)
	{
		int lIdx = GetAnimationStateIndex(asName);
		if(lIdx >= 0)
		{
			return mvAnimationStates[lIdx];
		}
		else
		{
			return NULL;
		}
	}
	int cMeshEntity::GetAnimationStateNum()
	{
		return (int)mvAnimationStates.size();
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::Play(int alIndex,bool abLoop, bool bStopPrev)
	{
		if(bStopPrev) Stop();
		if(alIndex >= (int)mvAnimationStates.size()) return;
		
		mvAnimationStates[alIndex]->SetActive(true);
		mvAnimationStates[alIndex]->SetTimePosition(0);
		mvAnimationStates[alIndex]->SetLoop(abLoop);
		mvAnimationStates[alIndex]->SetWeight(1);
	}

	void cMeshEntity::PlayName(const tString &asName,bool abLoop, bool bStopPrev)
	{
		int lIdx = GetAnimationStateIndex(asName);
		if(lIdx>=0)
		{
			Play(lIdx,abLoop,bStopPrev);
		}
		else
		{
			Warning("Can not find animation '%s' in meshentity '%s'\n",asName.c_str(),
																	msName.c_str());
		}
	}
	
	//-----------------------------------------------------------------------

	void cMeshEntity::PlayFadeTo(int alIndex,bool abLoop, float afTime)
	{
		///////////////////////
		// Fade out previous
		for(size_t i=0; i< mvAnimationStates.size(); i++)
		{
			cAnimationState *pAnim = mvAnimationStates[i];
			if(pAnim->IsActive()) pAnim->FadeOut(afTime);
		}

		///////////////////////
		// Fade in new
		cAnimationState *pAnim = mvAnimationStates[alIndex];
		if(pAnim->IsActive()==false)
		{
			pAnim->SetWeight(0);
		}

		pAnim->SetActive(true);
		pAnim->SetTimePosition(0);
		pAnim->SetLoop(abLoop);
		pAnim->FadeIn(afTime);
	}
	
	void cMeshEntity::PlayFadeToName(const tString &asName,bool abLoop, float afTime)
	{
		int lIdx = GetAnimationStateIndex(asName);
		if(lIdx>=0)
		{
			PlayFadeTo(lIdx, abLoop, afTime);
		}
		else
		{
			Warning("Can not find animation '%s' in mesh entity '%s'\n",asName.c_str(), msName.c_str());
		}
	}

	//-----------------------------------------------------------------------

	
	void cMeshEntity::Stop()
	{
		for(size_t i=0; i< mvAnimationStates.size(); i++)
		{
			mvAnimationStates[i]->SetActive(false);
			mvAnimationStates[i]->SetTimePosition(0);
		}
	}

	//-----------------------------------------------------------------------

	bool cMeshEntity::AnimationIsOver(const tString &asName)
	{
		int lIdx = GetAnimationStateIndex(asName);
		if(lIdx>=0)
		{
			return mvAnimationStates[lIdx]->IsOver();
		}
		else
		{
			Warning("Can not find animation '%s' in mesh entity '%s'\n",asName.c_str(), msName.c_str());
			return false;
		}
	}
	
	//-----------------------------------------------------------------------

	cBoneState* cMeshEntity::GetBoneState(int alIndex)
	{
		if ( alIndex < 0 || alIndex >= mvBoneStates.size() )
		{
			return NULL;
		}
		return mvBoneStates[alIndex];
	}

	int cMeshEntity::GetBoneStateIndex(const tString &asName)
	{
		tNodeStateIndexMapIt it = m_mapBoneStateIndices.find(asName);
		if(it != m_mapBoneStateIndices.end())
		{
			return it->second;
		}
		else
		{
			return -1;
		}
	}

	int cMeshEntity::GetBoneStateIndexFromPtr(cBoneState* apBoneState)
	{
		for(size_t i=0; i<mvBoneStates.size(); ++i)
		{
			if(apBoneState == mvBoneStates[i]) return (int)i;
		}

		return -1;
	}

	cBoneState* cMeshEntity::GetBoneStateFromName(const tString &asName)
	{
		int lIdx = GetBoneStateIndex(asName);
		if(lIdx >= 0)
		{
			return mvBoneStates[lIdx];
		}
		else
		{
			return NULL;
		}
	}
	int cMeshEntity::GetBoneStateNum()
	{
		return (int)mvBoneStates.size();
	}

	//----------------------------------------------------------------------
	
	void cMeshEntity::SetSkeletonPhysicsActive(bool abX)
	{
		mbSkeletonPhysics = abX;

		mbUpdatedBones = false;
		ResetGraphicsUpdated();

		mbSkeletonPhysicsFading = false;
		mfSkeletonPhysicsWeight = 1.0f;

		for(int bone =0; bone< GetBoneStateNum(); ++bone)
		{
			cBoneState *pState = GetBoneState(bone);
			iPhysicsBody *pBody = pState->GetBody();
			iPhysicsBody *pColliderBody = pState->GetColliderBody();

			if(pBody)
			{
				pBody->SetActive(abX);
				pBody->Enable();

				if(abX==false)
				{
					pBody->SetLinearVelocity(0);
					pBody->SetAngularVelocity(0);
				}

				if(mbSkeletonColliders)
				{
					pColliderBody->SetActive(!abX);
				}
			}
		}
	}

	bool cMeshEntity::GetSkeletonPhysicsActive()
	{
		return mbSkeletonPhysics;
	}

	//----------------------------------------------------------------------

	void cMeshEntity::FadeSkeletonPhysicsWeight(float afTime)
	{
		if(mbSkeletonPhysics)
		{
			mbSkeletonPhysicsFading = true;
			mfSkeletonPhysicsFadeSpeed = 1.0f / afTime;

			for(int bone =0; bone< GetBoneStateNum(); ++bone)
			{
				cBoneState *pState = GetBoneState(bone);
				iPhysicsBody *pBody = pState->GetBody();
				iPhysicsBody *pColliderBody = pState->GetColliderBody();

				if(pBody) pBody->SetActive(false);
			}
		}
	}

	//----------------------------------------------------------------------


	float cMeshEntity::GetSkeletonPhysicsWeight()
	{
		return mfSkeletonPhysicsWeight;
	}
	void cMeshEntity::SetSkeletonPhysicsWeight(float afX)
	{
		mfSkeletonPhysicsWeight = afX;
	}

	//----------------------------------------------------------------------

	void cMeshEntity::SetSkeletonCollidersActive(bool abX)
	{		
		mbSkeletonColliders = abX;

		//Set active to the correct state.
		for(int bone =0; bone< GetBoneStateNum(); ++bone)
		{
			cBoneState *pState = GetBoneState(bone);
			iPhysicsBody *pColliderBody = pState->GetColliderBody();

			if(pColliderBody)
			{
				if(abX && !mbSkeletonPhysics) pColliderBody->SetActive(true);
				else if(!abX) pColliderBody->SetActive(false);
			}
		}
	}
	bool cMeshEntity::GetSkeletonCollidersActive()
	{
		return mbSkeletonColliders;
	}

	//----------------------------------------------------------------------

	void cMeshEntity::AlignBodiesToSkeleton(bool abCalculateSpeed)
	{
		for(int bone =0; bone< GetBoneStateNum(); ++bone)
		{
			cBoneState *pState = GetBoneState(bone);
			iPhysicsBody *pBody = pState->GetBody();

			if(pBody)
			{
				cMatrixf mtxBody = cMath::MatrixMul(pState->GetWorldMatrix(), pState->GetBodyMatrix());
				pBody->SetMatrix(mtxBody);

				if(abCalculateSpeed)
				{
					//TODO: calculate speed based on the previous frame of the animation.
				}
			}
		}
	}

	//----------------------------------------------------------------------

	cMatrixf cMeshEntity::CalculateTransformFromSkeleton(cVector3f *apPostion,cVector3f *apAngles)
	{
		//Root bone
		cNode3DIterator StateIt = mpBoneStateRoot->GetChildIterator();
		cBoneState *pBoneState = static_cast<cBoneState*>(StateIt.Next());

		//Get the root bone (should only be one)
		cBoneIterator BoneIt = GetMesh()->GetSkeleton()->GetRootBone()->GetChildIterator();
		cBone* pBone = BoneIt.Next();

		//Rotation and position
		cMatrixf mtxInvBind = pBone->GetInvWorldTransform();
		cMatrixf mtxInvBone = cMath::MatrixInverse(pBoneState->GetWorldMatrix());
		cVector3f vStateForward = mtxInvBone.GetForward();
		cVector3f vBindForward = mtxInvBind.GetForward();

		float fBindYAngle = -cMath::GetAngleFromPoints2D(0,cVector2f(-vBindForward.x,-vBindForward.z));
		float fStateYAngle = -cMath::GetAngleFromPoints2D(0,cVector2f(-vStateForward.x,-vStateForward.z));
		float fYAngle = fStateYAngle - fBindYAngle;

		cMatrixf mtxTransform = cMath::MatrixRotateY(fYAngle);

		cVector3f vRootBoneOffset = pBone->GetLocalTransform().GetTranslation();
		vRootBoneOffset.y =0;
		vRootBoneOffset = cMath::MatrixMul(mtxTransform,vRootBoneOffset);

		mtxTransform.SetTranslation(pBoneState->GetWorldPosition());// - vRootBoneOffset);
		
		if(apPostion) *apPostion = pBoneState->GetWorldPosition();// - vRootBoneOffset;
		if(apAngles) *apAngles = cVector3f(0,fYAngle,0);

		return mtxTransform;
	}

	//----------------------------------------------------------------------

	bool cMeshEntity::CheckColliderShapeCollision(iPhysicsWorld *apWorld,
						iCollideShape *apShape, const cMatrixf &a_mtxShape,
						tVector3fList *apPosList, tIntList *apNumList)
	{
		bool bCollision = false;
		cCollideData collideData;
		collideData.SetMaxSize(1);

		for(size_t i=0; i< mvBoneStates.size(); ++i)
		{
			cBoneState *pState = mvBoneStates[i];
			iPhysicsBody *pBody = pState->GetColliderBody();
			if(pBody==NULL) continue;

			cMatrixf mtxBody = cMath::MatrixMul(pState->GetWorldMatrix(), pState->GetBodyMatrix());
			pBody->SetMatrix(mtxBody);
			
			bool bRet = apWorld->CheckShapeCollision(pBody->GetShape(),
												pBody->GetLocalMatrix(),
												apShape,a_mtxShape,
												collideData,1, false);

			if(bRet)
			{
				bCollision = true;
				if(!apPosList && !apNumList) break;

				if(apPosList) apPosList->push_back(collideData.mvContactPoints[0].mvPoint);
				if(apNumList) apNumList->push_back((int)i);
			}
		}
		
		return bCollision;
	}

	//----------------------------------------------------------------------

	void cMeshEntity::ResetGraphicsUpdated()
	{
		for(size_t i=0; i< mvSubMeshes.size(); ++i)
		{
			mvSubMeshes[i]->mbGraphicsUpdated = false;
		}
		mbUpdatedBones = false;
	}

	//----------------------------------------------------------------------

	cNode3D* cMeshEntity::GetNodeState(int alIndex)
	{
		return mvNodeStates[alIndex];
	}

	int cMeshEntity::GetNodeStateIndex(const tString &asName)
	{
		tNodeStateIndexMapIt it = m_mapNodeStateIndices.find(asName);
		if(it != m_mapNodeStateIndices.end())
		{
			return it->second;
		}
		else
		{
			return -1;
		}
	}

	cNode3D* cMeshEntity::GetNodeStateFromName(const tString &asName)
	{
		int lIdx = GetNodeStateIndex(asName);
		if(lIdx >= 0)
		{
			return mvNodeStates[lIdx];
		}
		else
		{
			return NULL;
		}
	}
	int cMeshEntity::GetNodeStateNum()
	{
		return (int)mvNodeStates.size();
	}

	//-----------------------------------------------------------------------
	
	cSubMeshEntity* cMeshEntity::GetSubMeshEntity(unsigned int alIdx)
	{
		if(alIdx >= mvSubMeshes.size()) return NULL;

		return mvSubMeshes[alIdx];
	}

	cSubMeshEntity* cMeshEntity::GetSubMeshEntityName(const tString &asName)
	{
		tSubMeshEntityMapIt it = m_mapSubMeshes.find(asName);
		if(it == m_mapSubMeshes.end())return NULL;

		return it->second;
	}

	int cMeshEntity::GetSubMeshEntityNum()
	{
		return (int)mvSubMeshes.size();
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::UpdateGraphicsForFrame(float afFrameTime)
	{
		//////////////////////////////////////////
		//Check so update is needed
		if(	mbBoneMatricesNeedUpdate == false &&
			mlBoneMatricesTransformCount == GetTransformUpdateCount())
		{
			return;
		}

		mlBoneMatricesTransformCount = GetTransformUpdateCount();
		mbBoneMatricesNeedUpdate = false;

		///////////////////////////////////
		//Update the bone matrices
		cSkeleton *pSkeleton = mpMesh->GetSkeleton();
		if(pSkeleton)
		{
			if(mlInvWorldMatrixTransformCount != GetTransformUpdateCount())
			{
				mlInvWorldMatrixTransformCount = GetTransformUpdateCount();
				m_mtxInvWorldMatrix = cMath::MatrixInverse(GetWorldMatrix());
			}
			
			for(int i=0; i< pSkeleton->GetBoneNum(); i++)
			{
				cBone *pBone = pSkeleton->GetBoneByIndex(i);
				cNode3D* pState = mvBoneStates[i];
                
				//Transform the movement of the bone into the
				//Bind pose's local space.
				cMatrixf mtxLocal = cMath::MatrixMul(m_mtxInvWorldMatrix,pState->GetWorldMatrix());
				
				mvBoneMatrices[i] = cMath::MatrixMul(mtxLocal,pBone->GetInvWorldTransform());
			}
		}
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::SetVisible(bool abX)
	{
		if(abX == mbIsVisible) return;

		mbIsVisible = abX;
		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			mvSubMeshes[i]->SetVisible(abX);
		}
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cMeshEntity::GetBoundingVolume()
	{
		///////////////////////////////////////////////
		// Update the BV from bones
		if(mpMesh->GetSkeleton())
		{
			if(mbUpdateBoundingVolume)
			{
				UpdateBVFromSkeleton();
				mbUpdateBoundingVolume = false;
			}
		}
		///////////////////////////////////////////////
		// Update the BV from the sub meshes
		else
		{
			if(mbUpdateBoundingVolume)
			{
				mbUpdateBoundingVolume = false;

				cBoundingVolume *pBV = mvSubMeshes[0]->GetBoundingVolume(); 
				cVector3f vFinalMin = pBV->GetMin();
				cVector3f vFinalMax = pBV->GetMax();

				for(int i=1; i< (int)mvSubMeshes.size(); i++)
				{
					cBoundingVolume *pBV = mvSubMeshes[i]->GetBoundingVolume(); 

					cVector3f vMin = pBV->GetMin();
					cVector3f vMax = pBV->GetMax();

					if(vFinalMin.x > vMin.x)vFinalMin.x = vMin.x;
					if(vFinalMax.x < vMax.x)vFinalMax.x = vMax.x;

					if(vFinalMin.y > vMin.y)vFinalMin.y = vMin.y;
					if(vFinalMax.y < vMax.y)vFinalMax.y = vMax.y;

					if(vFinalMin.z > vMin.z)vFinalMin.z = vMin.z;
					if(vFinalMax.z < vMax.z)vFinalMax.z = vMax.z;
				}

				mBoundingVolume.SetLocalMinMax(vFinalMin,vFinalMax);
			}
		}
	
		//Return the bounding volume.
		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::SetStatic(bool abX)
	{
		if(abX == mbStatic) return;

		mbStatic = abX;
		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			mvSubMeshes[i]->SetStatic(abX);
		}
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::SetRenderFlagBit(tRenderableFlag alFlagBit, bool abSet)
	{
		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			mvSubMeshes[i]->SetRenderFlagBit(alFlagBit, abSet);
		}
	}

	void cMeshEntity::SetIlluminationAmount(float afX)
	{
		if(mfIlluminationAmount == afX) return;

		mfIlluminationAmount = afX;

		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			mvSubMeshes[i]->SetIlluminationAmount(mfIlluminationAmount);
		}
	}

	void cMeshEntity::SetCoverageAmount(float afX)
	{
		if(mfCoverageAmount == afX) return;

		mfCoverageAmount = afX;

		for(int i=0;i<(int)mvSubMeshes.size();i++)
		{
			mvSubMeshes[i]->SetCoverageAmount(mfCoverageAmount);
		}
	}

	
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIAVTE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	float cMeshEntity::GetAnimationWeightMul()
	{
		if(mbNormalizeAnimationWeights==false) return 1.0f;
		
		float fAnimNum =0;
		float fTotalAnimWeight =0;
		for(size_t i=0; i< mvAnimationStates.size(); i++)
		{
			cAnimationState *pAnimState = mvAnimationStates[i];

			if(pAnimState->IsActive())
			{
				fAnimNum++;
				fTotalAnimWeight += pAnimState->GetWeight();
			}
		}

		//Calculate the mul
		if(fAnimNum >0 && fTotalAnimWeight >0)
		{
			return 1.0f / fTotalAnimWeight;
		}
		
		return 1.0f;
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::CreateNodes()
	{
		/////////////////////////////////
		//Check so it is okay to add nodes
		if(mvNodeStates.empty() ==false) return;
		if(mpMesh->GetSkeleton()) return;
		if(mpMesh->GetNodeNum()==0) return;
		
		////////////////////////
		//Make sure that the submeshes are not added to t
		for(size_t i=0; i<mvSubMeshes.size(); ++i)
		{
			cSubMeshEntity *pSubEnt = mvSubMeshes[i];

            RemoveChild(pSubEnt);
			pSubEnt->SetMatrix(cMatrixf::Identity);
		}
		

		////////////////////////////////
		// Set up data

		//Reserve size for nodes to be added
		mvNodeStates.reserve(mpMesh->GetNodeNum());
		
		////////////////////////////////
		//Create the nodes and attach sub meshes
		for(int i=0; i< mpMesh->GetNodeNum(); i++)
		{
			cNode3D* pMeshNode = mpMesh->GetNode(i);

			cBoneState *pNode = hplNew( cBoneState, (pMeshNode->GetName(), false) );
			pNode->SetMatrix(pMeshNode->GetLocalMatrix());

			//Add node to array and add it's index to the map.
			mvNodeStates.push_back(pNode);
			m_mapNodeStateIndices.insert(tNodeStateIndexMap::value_type(pNode->GetName(),i));

			//Connect with sub mesh entity
			cSubMeshEntity *pSubEntity = GetSubMeshEntityName(pMeshNode->GetName());
			if(pSubEntity)
			{
				pSubEntity->SetLocalNode(pNode);
			}
		}

		////////////////////////////////
		//Set up hierarchy
		for(int i=0; i< (int)mvNodeStates.size();i++)
		{
			cNode3D* pStateNode = mvNodeStates[i];
			cNode3D* pMeshNode = mpMesh->GetNode(i);

			//Set the parent if there is one
			if(pMeshNode->GetParent())
			{
				cNode3D* pParentNode = GetNodeStateFromName(pMeshNode->GetParent()->GetName());
				if(pParentNode)
				{
					pStateNode->SetParent(pParentNode);
				}
				else 
				{
					AddNodeChild(pStateNode);
				}
			}
			//If not set root node as parent.
			else
			{
				AddNodeChild(pStateNode);
			}

			//Add children if there are any.
			cNode3DIterator it = pMeshNode->GetChildIterator();
			while(it.HasNext())
			{
				cNode3D* pChildNode = static_cast<cNode3D*>(it.Next());
				pStateNode->AddChild(GetNodeStateFromName(pChildNode->GetName()));
			}
		}

		//Make sure everything gets updated!
		SetTransformUpdated(true);
	}
	
	//-----------------------------------------------------------------------

	void cMeshEntity::HandleAnimationEvent(cAnimationEvent *apEvent)
	{
		if(apEvent->msValue == "") return;

		switch(apEvent->mType)
		{
		case eAnimationEventType_PlaySound:
			{
				cSoundEntity *pSound = mpWorld->CreateSoundEntity("AnimEvent",apEvent->msValue,true);
				if(pSound)
				{
					pSound->SetIsSaved(false);
					cNode3DIterator nodeIt = mpBoneStateRoot->GetChildIterator();
					if(nodeIt.HasNext())
					{
						cNode3D *pNode = nodeIt.Next();
						pNode->AddEntity(pSound);
					}
					else
					{
						pSound->SetPosition(mBoundingVolume.GetWorldCenter());
					}
				}
				break;
			}
		}
	}
	
	//-----------------------------------------------------------------------
	
	void cMeshEntity::UpdateBVFromSkeleton()
	{
		if(mpMesh->GetSkeleton()==NULL) return;
		
		if(mvBoneStates.empty())
		{
			////////////////////////////////
			//Using vertices

			//Go through all the sub meshes and build BV from vertices.
			for(int i=0;i<GetSubMeshEntityNum();i++)
			{
				cSubMeshEntity* pSub = GetSubMeshEntity(i);

				iVertexBuffer *pVtxBuffer = pSub->GetVertexBuffer();

				mBoundingVolume.AddArrayPoints(pVtxBuffer->GetFloatArray(eVertexBufferElement_Position), 
																pVtxBuffer->GetVertexNum());
			}
			if(GetSubMeshEntityNum()>0)
				mBoundingVolume.CreateFromPoints(GetSubMeshEntity(0)->GetVertexBuffer()->GetElementNum(eVertexBufferElement_Position));
		}
		else
		{
			////////////////////////////////
			//Using bones
			cVector3f vMin,vMax;
			GetAABBFromBones(vMin, vMax);
			
			mBoundingVolume.SetTransform(cMatrixf::Identity);
			mBoundingVolume.SetLocalMinMax(vMin, vMax);
		}
	}

	//-----------------------------------------------------------------------

	void cMeshEntity::GetAABBFromBones(cVector3f &avMin, cVector3f &avMax)
	{
		avMin = mvBoneStates[0]->GetWorldPosition();
		avMax = avMin;

		for(size_t i=1; i< mvBoneStates.size(); ++i)
		{
			cBoneState *pState = mvBoneStates[i];
			float fBoundingRadius = mpMesh->GetBoneBoundingRadius((int)i);
			cVector3f vMaxPos = pState->GetWorldPosition() + cVector3f(fBoundingRadius);
			cVector3f vMinPos = pState->GetWorldPosition() - cVector3f(fBoundingRadius);

			if(avMax.x < vMaxPos.x) avMax.x = vMaxPos.x;
			if(avMax.y < vMaxPos.y) avMax.y = vMaxPos.y;
			if(avMax.z < vMaxPos.z) avMax.z = vMaxPos.z;

			if(avMin.x > vMinPos.x) avMin.x = vMinPos.x;
			if(avMin.y > vMinPos.y) avMin.y = vMinPos.y;
			if(avMin.z > vMinPos.z) avMin.z = vMinPos.z;
		}
	}
	
	//-----------------------------------------------------------------------
}
