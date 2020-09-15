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

#include "impl/MeshLoaderMSH.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"
#include "resources/BinaryBuffer.h"

#include "graphics/Mesh.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "graphics/Material.h"

#include "scene/Node3D.h"

#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC DATA
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	bool gbLogMSHLoad = false;
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshLoaderMSH::cMeshLoaderMSH(iLowLevelGraphics *apLowLevelGraphics) : iMeshLoader(apLowLevelGraphics)
	{
			AddSupportedExtension("msh");
			AddSupportedExtension("anm");
	}

	//-----------------------------------------------------------------------

	cMeshLoaderMSH::~cMeshLoaderMSH()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWorld* cMeshLoaderMSH::LoadWorld(const tWString& asFile, cScene* apScene,tWorldLoadFlag aFlags)
	{
		return NULL;	//Not used!
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshLoaderMSH::LoadMesh(const tWString& asFile,tMeshLoadFlag aFlags)
	{
		if(gbLogMSHLoad) Log("------ Loading ---------\n");
		/////////////////////////////////////////////////
		// Load file
		cBinaryBuffer binBuff(asFile);
		if(binBuff.Load()==false)
		{
			Error("Could not load file '%s' in MSH loader.", cString::To8Char(asFile).c_str());
			return NULL;
		}

		/////////////////////////////////////////////////
		// Header
		int lMagicNum = binBuff.GetInt32();
		int lVersion = binBuff.GetInt32();
		
		//Check magic number
		if(lMagicNum != MSH_FORMAT_MAGIC_NUMBER)
		{
			Error("File '%s' does not have right MSH magic number! Invalid header!\n", cString::To8Char(asFile).c_str());
			return NULL;
		}

		//Check so file has he right version
		if(lVersion != MSH_FORMAT_VERSION)
		{
			Error("File '%s' does not have right MSH version!\n", cString::To8Char(asFile).c_str());
			return NULL;
		}

		/////////////////////////////////////////////////
		// General properties
		int lSubMeshNum = binBuff.GetInt32();
		int lAnimationNum = binBuff.GetInt32();
		bool bSkeleton = binBuff.GetBool();

		if(gbLogMSHLoad) Log("General: %d %d %d\n", lSubMeshNum, lAnimationNum, bSkeleton);
		
		// Create the mesh
		tString sMeshName = cString::GetFileName(cString::To8Char(asFile));
		cMesh* pMesh = hplNew( cMesh, (sMeshName, asFile,mpMaterialManager,mpAnimationManager) );

		////////////////////////////
		// Skeleton
		if(bSkeleton)
		{
			cSkeleton* pSkeleton = hplNew( cSkeleton, () );
            
			int lRootChildNum = binBuff.GetInt32();
			for(int i=0; i<lRootChildNum; ++i)
			{
				GetBoneFromBuffer(pSkeleton->GetRootBone(), &binBuff, 0);
			}

			pMesh->SetSkeleton(pSkeleton);
		}

		////////////////////////////
		// Nodes
		{
			cNode3D *pRootNode = pMesh->GetRootNode();
			
			int lRootChildNum = binBuff.GetInt32();
			if(gbLogMSHLoad) Log("Nodes num: %d\n", lRootChildNum);

			for(int i=0; i<lRootChildNum; ++i)
			{
				GetNodeFromBuffer(pRootNode, pMesh, &binBuff, 0);
			}
		}
		
		/////////////////////////////////////////////////
		// Sub Meshes
		for(int sub=0; sub<lSubMeshNum; ++sub)
		{

			/////////////////////////
			// General Data
			tString sName, sMaterial;
			binBuff.GetString(&sName);
			binBuff.GetString(&sMaterial);

			cMatrixf mtxTransform; cVector3f vModelScale;
            binBuff.GetMatrixf(&mtxTransform);
			binBuff.GetVector3f(&vModelScale);
			bool bCollideShape = binBuff.GetBool();
						
			//Create sub mesh and set up data
			cSubMesh *pSubMesh = pMesh->CreateSubMesh(sName);

			pSubMesh->SetLocalTransform(mtxTransform);
			pSubMesh->SetModelScale(vModelScale);
			pSubMesh->SetIsCollideShape(bCollideShape);

			//////////////////
			//Set material
			pSubMesh->SetMaterialName(sMaterial);
			if(sMaterial != "")
			{
				//Use fast load material if set.
				if(	mpMeshManager->GetUseFastloadMaterial() &&	mpMeshManager->GetFastloadMaterial() != "")
				{
					sMaterial = mpMeshManager->GetFastloadMaterial();
				}

				cMaterial *pMaterial = mpMaterialManager->CreateMaterial(sMaterial);
				pSubMesh->SetMaterial(pMaterial);
			}

			if(gbLogMSHLoad) Log("Submesh %d: '%s' '%s'\n", sub, sName.c_str(), sMaterial.c_str());

			////////////////////
			// Colliders
			{
				int lColliderNum = binBuff.GetInt32();

				if(gbLogMSHLoad) Log(" Colliders: %d\n",lColliderNum);
				
				for(int i=0; i<lColliderNum; ++i)
				{
					eCollideShapeType type = (eCollideShapeType)binBuff.GetShort16();

					cMeshCollider *pCollider = pSubMesh->CreateCollider(type);
					binBuff.GetMatrixf(&pCollider->m_mtxOffset);
					binBuff.GetVector3f(&pCollider->mvSize);
					pCollider->mbCharCollider = binBuff.GetBool();

					if(gbLogMSHLoad) Log("  Collider%d: %d %s %s %d\n",i,	type, pCollider->m_mtxOffset.ToString().c_str(), 
																			pCollider->mvSize.ToString().c_str(), pCollider->mbCharCollider);
				}
			}

			////////////////////
			// Vertex Bone Pairs
			{
				int lVtxBonePairNum = binBuff.GetInt32();
				pSubMesh->ResizeVertexBonePairs(lVtxBonePairNum);

				if(gbLogMSHLoad) Log(" VertexBonePairs: %d\n",pSubMesh->GetVertexBonePairNum(), lVtxBonePairNum);

				for(int i=0; i<pSubMesh->GetVertexBonePairNum(); ++i)
				{
					cVertexBonePair& vtxBonePair = pSubMesh->GetVertexBonePair(i);
					vtxBonePair.vtxIdx = binBuff.GetInt32();
					vtxBonePair.boneIdx = binBuff.GetInt32();
					vtxBonePair.weight = binBuff.GetFloat32();
				}
			}

						
			////////////////////
			// Vertex data
			iVertexBuffer* pVtxBuff = mpLowLevelGraphics->CreateVertexBuffer(	eVertexBufferType_Hardware, eVertexBufferDrawType_Tri,
																				eVertexBufferUsageType_Static, 0, 0);
			{
				int lVtxNum = binBuff.GetInt32();
				int lVtxTypeNum = binBuff.GetInt32();
				
				if(gbLogMSHLoad) Log(" VertexBuffers num: %d typenum: %d\n",lVtxNum, lVtxTypeNum);
				
				////////////////////
				// Get vertex arrays
				for(int i=0; i< lVtxTypeNum; ++i)
				{
					//Get the settings
					eVertexBufferElement arrayType = (eVertexBufferElement)binBuff.GetShort16();
					eVertexBufferElementFormat elementFormat = (eVertexBufferElementFormat)binBuff.GetShort16();
					int lProgramVarIndex = binBuff.GetInt32();
					int lElementNum = binBuff.GetInt32();

					if(gbLogMSHLoad) Log("   Vtx %d: %d %d %d\n", i, arrayType, lProgramVarIndex, lElementNum);

					//Create the array
					pVtxBuff->CreateElementArray(arrayType, elementFormat, lElementNum, lProgramVarIndex);
					pVtxBuff->ResizeArray(arrayType, lVtxNum * lElementNum);
					
					//Get and fill the array data
					void *pData = GetVertexBufferWithFormat(pVtxBuff, arrayType, elementFormat);
					GetBinaryBufferDataWithFormat(&binBuff, pData, (size_t)(lVtxNum * lElementNum), elementFormat);
				}
			}

			////////////////////
			//Get Indices
			{
				int lIdxNum =  binBuff.GetInt32();

				if(gbLogMSHLoad) Log("Indices: %d\n", lIdxNum);

				pVtxBuff->ResizeIndices(lIdxNum);
				binBuff.GetInt32Array((int*)pVtxBuff->GetIndices(), lIdxNum);
			}

			
			///////////////////
			//Compile vertex buffer and set to submesh
			pVtxBuff->Compile(0);

			pSubMesh->SetVertexBuffer(pVtxBuff);
			pSubMesh->Compile();
		}

		////////////////////////////
		// Compile Mesh
		pMesh->CompileBonesAndSubMeshes();


		////////////////////////////
		// Animations
		{
            int lAnimationNum = binBuff.GetInt32();
			if(gbLogMSHLoad) Log(" Animation num: %d\n",lAnimationNum);

			for(int i=0; i<lAnimationNum; ++i)
			{
				cAnimation *pAnim = GetAnimation(&binBuff, asFile);
				pMesh->AddAnimation(pAnim);
			}
		}
	

		return pMesh;
	}

	//-----------------------------------------------------------------------

	bool cMeshLoaderMSH::SaveMesh(cMesh* apMesh, const tWString& asFile)
	{
		cBinaryBuffer binBuff(asFile);

		if(gbLogMSHLoad) Log("------ Saving ---------\n");

		////////////////////////////////////////
		// Header
        binBuff.AddInt32(MSH_FORMAT_MAGIC_NUMBER);
		binBuff.AddInt32(MSH_FORMAT_VERSION);
		
		////////////////////////////////////////
		// General Data
		binBuff.AddInt32(apMesh->GetSubMeshNum());
		binBuff.AddInt32(apMesh->GetAnimationNum());
		binBuff.AddBool(apMesh->GetSkeleton() != NULL); //TODO: Check for skeleton!

		////////////////////////////
		// Skeleton
		if(apMesh->GetSkeleton() != NULL)
		{
			cBone *pRootBone = apMesh->GetSkeleton()->GetRootBone();
			binBuff.AddInt32((int)pRootBone->GetChildList()->size());

			//Iterate bones
			cBoneIterator boneIt = pRootBone->GetChildIterator();
			while(boneIt.HasNext())
			{
				AddBoneToBuffer(boneIt.Next(), &binBuff, 0);
			}
        }

		////////////////////////////
		// Nodes
		{
			cNode3D *pRootNode = apMesh->GetRootNode();
			binBuff.AddInt32((int)pRootNode->GetChildList()->size());

			if(gbLogMSHLoad) Log("Nodes num: %d\n", pRootNode->GetChildList()->size());

			//Iterate child nodes
			cNode3DIterator nodeIt = pRootNode->GetChildIterator();
			while(nodeIt.HasNext())
			{
				AddNodeToBuffer(nodeIt.Next(), &binBuff, 0);
			}
		}

		////////////////////////////////////////
		// Sub Meshes
		for(int sub=0; sub< apMesh->GetSubMeshNum(); sub++)
		{
			cSubMesh* pSubMesh = apMesh->GetSubMesh(sub);
			iVertexBuffer *pVtxBuff = pSubMesh->GetVertexBuffer();
			
			////////////////////////////
			//Add variables
			binBuff.AddString(pSubMesh->GetName());
			
			binBuff.AddString(pSubMesh->GetMaterialName());
			binBuff.AddMatrixf(pSubMesh->GetLocalTransform());
			binBuff.AddVector3f(pSubMesh->GetModelScale());
			binBuff.AddBool(pSubMesh->IsCollideShape());


			if(gbLogMSHLoad) Log("Submesh %d: '%s' '%s'\n", sub, pSubMesh->GetName().c_str(), pSubMesh->GetMaterialName().c_str());

			////////////////////
			// Colliders
			{
				binBuff.AddInt32(pSubMesh->GetColliderNum());

				if(gbLogMSHLoad) Log(" Colliders: %d\n",pSubMesh->GetColliderNum());

				for(int i=0; i<pSubMesh->GetColliderNum(); ++i)
				{
					cMeshCollider *pCollider = pSubMesh->GetCollider(i);

					binBuff.AddShort16(pCollider->mType);
					binBuff.AddMatrixf(pCollider->m_mtxOffset);
					binBuff.AddVector3f(pCollider->mvSize);
					binBuff.AddBool(pCollider->mbCharCollider);

					if(gbLogMSHLoad) Log("  Collider%d: %d %s %s %d\n",i, pCollider->mType, pCollider->m_mtxOffset.ToString().c_str(), 
																		pCollider->mvSize.ToString().c_str(), pCollider->mbCharCollider);
				}
			}

			////////////////////
			// Vertex Bone Pairs
			{
				if(gbLogMSHLoad) Log(" VertexBonePairs: %d\n",pSubMesh->GetVertexBonePairNum());

				binBuff.AddInt32(pSubMesh->GetVertexBonePairNum());

				for(int i=0; i<pSubMesh->GetVertexBonePairNum(); ++i)
				{
					cVertexBonePair& vtxBonePair = pSubMesh->GetVertexBonePair(i);
					binBuff.AddInt32(vtxBonePair.vtxIdx);
					binBuff.AddInt32(vtxBonePair.boneIdx);
					binBuff.AddFloat32(vtxBonePair.weight);
				}
			}

			////////////////////////////
			//Add Vertices
			{
				int lVtxNum =  pVtxBuff->GetVertexNum();
				binBuff.AddInt32(lVtxNum);

				//////////////////////////////
				// Calculate the number of vertex buffer types
				int lVtxTypeNum =  0;
				for(int i=0; i < eVertexBufferElement_LastEnum;i++)
				{
					if(pVtxBuff->GetElementNum((eVertexBufferElement)i) > 0) ++lVtxTypeNum;
				}
				binBuff.AddInt32(lVtxTypeNum);
				
				if(gbLogMSHLoad) Log(" VertexBuffers num: %d typenum: %d\n",lVtxNum, lVtxTypeNum);
			
				//////////////////////////////
				// Iterate the Vertices
				for(int i=0; i < eVertexBufferElement_LastEnum;i++)
				{
					eVertexBufferElement arrayType = (eVertexBufferElement)i;

					if(pVtxBuff->GetElementNum(arrayType) <= 0) continue;

					int lElementNum = pVtxBuff->GetElementNum(arrayType);
					eVertexBufferElementFormat elementFormat = pVtxBuff->GetElementFormat(arrayType);
					
					binBuff.AddShort16(arrayType);
					binBuff.AddShort16(elementFormat);
					binBuff.AddInt32(pVtxBuff->GetElementProgramVarIndex(arrayType));
					binBuff.AddInt32(lElementNum);

					if(gbLogMSHLoad) Log("  Vtx %d: %d %d %d\n", i, arrayType, pVtxBuff->GetElementProgramVarIndex(arrayType), lElementNum);

					void *pData = GetVertexBufferWithFormat(pVtxBuff, arrayType, elementFormat);
					AddBinaryBufferDataWithFormat(&binBuff, pData, (size_t)(lVtxNum * lElementNum), elementFormat);
				}
			}

			////////////////////////////
			//Add Indices
			{
				int lIdxNum =  pVtxBuff->GetIndexNum();

				if(gbLogMSHLoad) Log("Indices: %d\n", lIdxNum);

				binBuff.AddInt32(lIdxNum);
				binBuff.AddInt32Array((int*)pVtxBuff->GetIndices(), lIdxNum);
			}
		}

		
		////////////////////////////
		// Animations
		{
			binBuff.AddInt32(apMesh->GetAnimationNum());
			if(gbLogMSHLoad) Log(" Animation num: %d\n",apMesh->GetAnimationNum());

            for(int i=0; i<apMesh->GetAnimationNum(); ++i)
			{
				AddAnimation(apMesh->GetAnimation(i), &binBuff);
			}
		}
		
		////////////////////////////
		// Save data
		bool bRet = binBuff.Save();
		if(bRet==false) 	Error("Couldn't save mesh to '%s'", cString::To8Char(asFile).c_str());
		
		return bRet;
	}
	
	//-----------------------------------------------------------------------

	cAnimation* cMeshLoaderMSH::LoadAnimation(const tWString& asFile)
	{
		if(gbLogMSHLoad) Log("------ Loading Anim ---------\n");
		/////////////////////////////////////////////////
		// Load file
		cBinaryBuffer binBuff(asFile);
		if(binBuff.Load()==false)
		{
			Error("Could not load file '%s' in MSH loader.", cString::To8Char(asFile).c_str());
			return NULL;
		}

		/////////////////////////////////////////////////
		// Header
		int lMagicNum = binBuff.GetInt32();
		int lVersion = binBuff.GetInt32();

		//Check magic number
		if(lMagicNum != MSH_FORMAT_MAGIC_NUMBER)
		{
			Error("File '%s' does not have right MSH magic number! Invalid header!\n", cString::To8Char(asFile).c_str());
			return NULL;
		}

		//Check so file has he right version
		if(lVersion != MSH_FORMAT_VERSION)
		{
			Warning("File '%s' does not have right MSH version!\n", cString::To8Char(asFile).c_str());
			return NULL;
		}

		/////////////////////////////////////////////////
		// Animation
		cAnimation *pAnimation = GetAnimation(&binBuff, asFile);
		
		return pAnimation;
	}
	
	//-----------------------------------------------------------------------

	bool cMeshLoaderMSH::SaveAnimation(cAnimation* apAnimation, const tWString& asFile)
	{
		cBinaryBuffer binBuff(asFile);

		if(gbLogMSHLoad) Log("------ Saving ---------\n");

		////////////////////////////////////////
		// Header
		binBuff.AddInt32(MSH_FORMAT_MAGIC_NUMBER);
		binBuff.AddInt32(MSH_FORMAT_VERSION);

		////////////////////////////////////////
		// Animation
        AddAnimation(apAnimation, &binBuff);

		////////////////////////////
		// Save data
		bool bRet = binBuff.Save();
		if(bRet==false) 	Error("Couldn't save animation to '%s'", asFile.c_str());

		return bRet;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::AddAnimation(cAnimation *apAnimation, cBinaryBuffer* apBuffer)
	{
		////////////////////////
		// General properties
		apBuffer->AddString(apAnimation->GetName());
		apBuffer->AddFloat32(apAnimation->GetLength());
		apBuffer->AddInt32(apAnimation->GetTrackNum());

		if(gbLogMSHLoad) Log(" Animation %s: %f %d\n", apAnimation->GetName().c_str(), apAnimation->GetLength(), apAnimation->GetTrackNum());

		////////////////////////
		// Tracks
        for(int track=0;track<apAnimation->GetTrackNum(); ++track)
		{
			cAnimationTrack *pTrack = apAnimation->GetTrack(track);

			/////////////////////////
			// General Properties
            apBuffer->AddString(pTrack->GetName());
			apBuffer->AddShort16(pTrack->GetTransformFlags());
			apBuffer->AddInt32(pTrack->GetKeyFrameNum());

			if(gbLogMSHLoad) Log("  Track %d %s: %d %d\n", track, pTrack->GetName().c_str(), pTrack->GetTransformFlags(), pTrack->GetKeyFrameNum());

			/////////////////////////
			// Key Frames
			for(int frame=0; frame<pTrack->GetKeyFrameNum(); ++frame)
			{
				cKeyFrame *pFrame = pTrack->GetKeyFrame(frame);

				//if(gbLogMSHLoad) Log("   Frame%d (%s) (%s, %f) %f\n",	frame, pFrame->trans.ToString().c_str(), 
				//														pFrame->rotation.v.ToString().c_str(), pFrame->rotation.w, pFrame->time);

                apBuffer->AddFloat32(pFrame->time);
				apBuffer->AddVector3f(pFrame->trans);
				apBuffer->AddQuaternion(pFrame->rotation);
			}
		}
	}
	
	//-----------------------------------------------------------------------

	cAnimation* cMeshLoaderMSH::GetAnimation(cBinaryBuffer* apBuffer, const tWString &asFullPath)
	{
		/////////////////////////
		// General Properties
		tString sAnimName;
		apBuffer->GetString(&sAnimName);
		float fLength = apBuffer->GetFloat32();
		int lTrackNum = apBuffer->GetInt32();
		
		cAnimation *pAnimation = hplNew(cAnimation, (cString::To8Char(asFullPath), asFullPath, cString::GetFileName(cString::To8Char(asFullPath))));
		pAnimation->SetLength(fLength);

		if(gbLogMSHLoad) Log(" Animation %s: %f %d\n", pAnimation->GetName().c_str(), pAnimation->GetLength(), lTrackNum);

		////////////////////////
		// Tracks
		pAnimation->ReserveTrackNum(lTrackNum);
		for(int track=0;track<lTrackNum; ++track)
		{
			/////////////////////////
			// General Properties
			tString sTrackName;
			apBuffer->GetString(&sTrackName);
			tAnimTransformFlag transFlag = apBuffer->GetShort16();
			int lFrameNum = apBuffer->GetInt32();

			cAnimationTrack *pTrack = pAnimation->CreateTrack(sTrackName,transFlag);
			
			if(gbLogMSHLoad) Log("  Track %d %s: %d %d\n", track, pTrack->GetName().c_str(), pTrack->GetTransformFlags(), lFrameNum);

			/////////////////////////
			// Key Frames
			for(int frame=0; frame<lFrameNum; ++frame)
			{
				cKeyFrame *pFrame = pTrack->CreateKeyFrame(apBuffer->GetFloat32());

				apBuffer->GetVector3f(&pFrame->trans);
				apBuffer->GetQuaternion(&pFrame->rotation);
				
				//if(gbLogMSHLoad) Log("   Frame%d (%s) (%s, %f) %f\n",	frame, pFrame->trans.ToString().c_str(), 
				//														pFrame->rotation.v.ToString().c_str(), pFrame->rotation.w, pFrame->time);

			}
		}

		return pAnimation;
	}

	//-----------------------------------------------------------------------
	
	static tString gsLevelTemp = "";
	const char* GetLevelSpaces(int alLevel)
	{
			gsLevelTemp ="";
			for(int i=0; i<alLevel; ++i) gsLevelTemp += " ";
			return gsLevelTemp.c_str();
	}

	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::AddNodeToBuffer(cNode3D *apNode, cBinaryBuffer* apBuffer, int alLevel)
	{
		apBuffer->AddString(apNode->GetName());
		apBuffer->AddMatrixf(apNode->GetLocalMatrix());
		apBuffer->AddInt32(apNode->GetCustomFlags());
		
		apBuffer->AddInt32((int)apNode->GetChildList()->size());
		
		if(gbLogMSHLoad) Log("%s Node '%s' %d %d (%s)\n",	GetLevelSpaces(alLevel), apNode->GetName().c_str(),apNode->GetCustomFlags(), 
															apNode->GetChildList()->size(), apNode->GetLocalMatrix().ToString().c_str());

		cNode3DIterator nodeIt = apNode->GetChildIterator();
		while(nodeIt.HasNext())
		{
			AddNodeToBuffer(nodeIt.Next(), apBuffer, alLevel+1);
		}
	}

	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::GetNodeFromBuffer(cNode3D *apParentNode, cMesh *apMesh, cBinaryBuffer* apBuffer, int alLevel)
	{
		///////////////////////////////
		// Get properties
		tString sName;
		cMatrixf mtxTransform;

		apBuffer->GetString(&sName);
		apBuffer->GetMatrixf(&mtxTransform);
		int lCustomFlags = apBuffer->GetInt32();
		int lChildNum = apBuffer->GetInt32();

		///////////////////////////////
		// Create node
		cNode3D *pNode = apParentNode->CreateChild(sName);
		apMesh->AddNode(pNode);
		pNode->SetMatrix(mtxTransform);
		pNode->SetCustomFlags(lCustomFlags);

		if(gbLogMSHLoad) Log("%s Node '%s' %d %d (%s)\n",	GetLevelSpaces(alLevel), pNode->GetName().c_str(), pNode->GetCustomFlags(), 
															lChildNum, pNode->GetLocalMatrix().ToString().c_str());

		///////////////////////////////
		// Iterate children
		for(int i=0; i<lChildNum; ++i)
		{
			GetNodeFromBuffer(pNode,apMesh, apBuffer, alLevel+1);
		}
	}
	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::AddBoneToBuffer(cBone *apBone, cBinaryBuffer* apBuffer, int alLevel)
	{
		apBuffer->AddString(apBone->GetName());
		apBuffer->AddString(apBone->GetSid());
		apBuffer->AddMatrixf(apBone->GetLocalTransform());
		apBuffer->AddInt32((int)apBone->GetChildList()->size());

		if(gbLogMSHLoad) Log("%s Bone '%s' '%s' %d (%s)\n",	GetLevelSpaces(alLevel), apBone->GetName().c_str(), apBone->GetSid().c_str(), 
															apBone->GetChildList()->size(), apBone->GetLocalTransform().ToString().c_str());

		cBoneIterator boneIt = apBone->GetChildIterator();
		while(boneIt.HasNext())
		{
			AddBoneToBuffer(boneIt.Next(), apBuffer, alLevel+1);
		}
	}
	
	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::GetBoneFromBuffer(cBone *apParentBone, cBinaryBuffer* apBuffer, int alLevel)
	{
		tString sName, sSid;
		cMatrixf mtxTransform;

		apBuffer->GetString(&sName);
		apBuffer->GetString(&sSid);
		apBuffer->GetMatrixf(&mtxTransform);
		int lChildNum = apBuffer->GetInt32();

		cBone *pBone = apParentBone->CreateChildBone(sName, sSid);
		pBone->SetTransform(mtxTransform);
		
		if(gbLogMSHLoad) Log("%s Bone '%s' '%s' %d (%s)\n",	GetLevelSpaces(alLevel), pBone->GetName().c_str(), pBone->GetSid().c_str(), 
															lChildNum, pBone->GetLocalTransform().ToString().c_str());

        for(int i=0; i<lChildNum; ++i)
		{
			GetBoneFromBuffer(pBone, apBuffer, alLevel+1);
		}
	}

	//-----------------------------------------------------------------------

	void* cMeshLoaderMSH::GetVertexBufferWithFormat(iVertexBuffer *apVtxBuffer, eVertexBufferElement aElement, eVertexBufferElementFormat aFormat)
	{
		switch(aFormat)
		{
		case eVertexBufferElementFormat_Int:		return (void*)apVtxBuffer->GetIntArray(aElement);
		case eVertexBufferElementFormat_Float:		return (void*)apVtxBuffer->GetFloatArray(aElement);
		case eVertexBufferElementFormat_Byte:		return (void*)apVtxBuffer->GetByteArray(aElement);
		}
		
		Error("Vertex buffer has incorrect format when getting vertexbuffer data during loading of MSH file!\n");
		return NULL;
	}

	//-----------------------------------------------------------------------
	
	void cMeshLoaderMSH::AddBinaryBufferDataWithFormat(cBinaryBuffer* apBuffer, void *apSrcData, size_t alSize, eVertexBufferElementFormat aFormat)
	{
		switch(aFormat)
		{
		case eVertexBufferElementFormat_Int:
			apBuffer->AddInt32Array((int*)apSrcData, alSize);
			break;
		case eVertexBufferElementFormat_Float:
			apBuffer->AddFloat32Array((float*)apSrcData, alSize);
			break;
		case eVertexBufferElementFormat_Byte:		
			apBuffer->AddCharArray((char*)apSrcData, alSize);
			break;
		default:
			Error("Vertex buffer has incorrect format when adding binary data during loading of MSH file!\n");
			break;
		}
	}
	
	//-----------------------------------------------------------------------

	void cMeshLoaderMSH::GetBinaryBufferDataWithFormat(cBinaryBuffer* apBuffer, void *apDestData, size_t alSize, eVertexBufferElementFormat aFormat)
	{
		switch(aFormat)
		{
		case eVertexBufferElementFormat_Int:
			apBuffer->GetInt32Array((int*)apDestData, alSize);
			break;
		case eVertexBufferElementFormat_Float:
			apBuffer->GetFloat32Array((float*)apDestData, alSize);
			break;
		case eVertexBufferElementFormat_Byte:		
			apBuffer->GetCharArray((char*)apDestData, alSize);
			break;
		default:
			Error("Vertex buffer has incorrect format when getting binary data during loading of MSH file!\n");
			break;
		}
	}

	//-----------------------------------------------------------------------


}
