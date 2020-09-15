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

#include "impl/MeshLoaderCollada.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/System.h"
#include "system/Platform.h"

#include "scene/Scene.h"
#include "scene/World.h"
#include "scene/MeshEntity.h"
#include "scene/Node3D.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"

#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "haptic/Haptic.h"
#include "haptic/HapticShape.h"
#include "haptic/HapticSurface.h"
#include "haptic/LowLevelHaptic.h"

#include "impl/tinyXML/tinyxml.h"
#include "impl/MeshLoaderMSH.h"

#include "math/Math.h"

namespace hpl {

#define GetAdress(sStr) if(sStr[0]=='#') sStr = cString::Sub(sStr,1);

	bool cMeshLoaderCollada::mbConvertUnit = true; 

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshLoaderCollada::cMeshLoaderCollada(iLowLevelGraphics *apLowLevelGraphics, cMeshLoaderMSH *apMeshLoaderMSH, bool abLoadAndSaveMSHFormat) 
		: iMeshLoader(apLowLevelGraphics)
	{
		mpMeshLoaderMSH = apMeshLoaderMSH;
		mbLoadAndSaveMSHFormat = abLoadAndSaveMSHFormat;

		AddSupportedExtension("dae");
		AddSupportedExtension("dae_anim");

		mFlags =0;
		mfUnitScale = 1;

		m_mtxZToY = cMatrixf(-1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);
	}

	//-----------------------------------------------------------------------

	cMeshLoaderCollada::~cMeshLoaderCollada()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static cColladaNode* GetNodeFromController(const tString& asGeomId,
												tColladaControllerVec &avColladaControllers,
												cColladaScene &aColladaScene)
	{
		tString sControlId="";
		bool bGuess=false;
		for(int ctrl=0; ctrl < (int)avColladaControllers.size(); ctrl++)
		{
			cColladaController &Control = avColladaControllers[ctrl];
			if(Control.msTarget == asGeomId){
				sControlId = Control.msId;
				bGuess=false;
			}
			//Guessing, if no controller found try the one with source "".
			else if(sControlId=="" && Control.msTarget=="") {
				sControlId = Control.msId;
				bGuess = true;
			}
		}

		if(bGuess) Warning("No controller for for geometry %s, guessing on %s target = ''\n",
							asGeomId.c_str(), sControlId.c_str());

		if(sControlId==""){
			Warning("No controller refered to the geometry!\n");
			return NULL;
		}

		cColladaNode* pNode = aColladaScene.GetNodeFromSource(sControlId);
		if(pNode==NULL){
			Warning("No node for controller '%s'\n",sControlId.c_str());
		}

		return pNode;
	}

	//-------------------------------------------------

	static void FixLocalTransform(cMatrixf *apMatrix, cColladaNode *apNode,
								tColladaAnimationVec &avColladaAnimations, cSkeleton *apSkeleton, bool abHasSeveralBodies)
	{
		cColladaNode *pParentNode = apNode->pParent;
		if(pParentNode && apSkeleton==NULL)
		{
			if(avColladaAnimations.empty() == false || abHasSeveralBodies)
			{
				*apMatrix = cMath::MatrixMul(cMath::MatrixScale(pParentNode->mvScale),*apMatrix);
			}
			else
			{
				*apMatrix = cMath::MatrixMul(pParentNode->m_mtxTransform,*apMatrix);
			}
		}
	}

	//-------------------------------------------------

	static void FixLocalPosition(cVector3f *apPos, cColladaNode *apNode,
		tColladaAnimationVec &avColladaAnimations, cSkeleton *apSkeleton, bool abHasSeveralBodies)
	{
		cColladaNode *pParentNode = apNode->pParent;
		if(pParentNode && apSkeleton==NULL)
		{
			if(avColladaAnimations.empty() == false || abHasSeveralBodies)
			{
				*apPos = cMath::MatrixMul(cMath::MatrixScale(pParentNode->mvScale),*apPos);
			}
			else
			{
				*apPos = cMath::MatrixMul(pParentNode->m_mtxTransform,*apPos);
			}
		}
	}

	//-------------------------------------------------

	cMesh* cMeshLoaderCollada::LoadMesh(const tWString& asFile,tMeshLoadFlag aFlags)
	{
		/////////////////////////////////////////////////
		// TRY USING MSH LOADER
		if(mbLoadAndSaveMSHFormat)
		{
			tWString sMSHFile = cString::SetFileExtW(asFile, _W("msh"));
			cDate currentDate = cPlatform::FileModifiedDate(asFile);
			cDate mshDate = cPlatform::FileModifiedDate(sMSHFile);
			
			if(cResources::GetForceCacheLoadingAndSkipSaving() || mshDate > currentDate || cPlatform::FileExists(asFile)==false)
			{
				cMesh *pMesh = mpMeshLoaderMSH->LoadMesh(sMSHFile, aFlags);
				if(pMesh)
				{
					pMesh->SetFullPath(asFile);//Use dae as full path! (otherwise file will be loaded several times!)
					return pMesh;
				}
			}
		}
		
		/////////////////////////////////////////////////
		// SETUP TEMP DATA STRUCTURES
		//Images
        tColladaImageVec vColladaImages;
		//Textures
		tColladaTextureVec vColladaTextures;
		//Materials
		tColladaMaterialVec vColladaMaterials;
		//Lights
		tColladaLightVec vColladaLights;
		//Geometries
		tColladaGeometryVec vColladaGeometries;
		//Controllers
		tColladaControllerVec vColladaControllers;
		//Animations
		tColladaAnimationVec vColladaAnimations;
		//Scene
		cColladaScene ColladaScene;

		mFlags = aFlags;

		//Fill the structures with collada file data
		tColladaGeometryVec *pGeomVec = (aFlags & eMeshLoadFlag_NoGeometry) ? NULL : &vColladaGeometries;

		bool bRet = FillStructures(asFile, &vColladaImages, &vColladaTextures,
									&vColladaMaterials,&vColladaLights,
									pGeomVec, &vColladaControllers,
									&vColladaAnimations,
									&ColladaScene,true);

		if(bRet==false) return NULL;

		////////////////////////
		//Check so the mesh is okay
		bool bMeshIsOKToCache = true;
		for(size_t geom=0; geom<vColladaGeometries.size(); ++geom)
		{
			cColladaGeometry *pGeometry = &vColladaGeometries[geom];
			for(size_t i=0; i<pGeometry->mvVertexVec.size(); ++i)
			{
				cVector3f vNrm = pGeometry->mvVertexVec[i].norm;
				
				//Normalize the normal!
				pGeometry->mvVertexVec[i].norm.Normalize();
				
				//Skip this test as some editors (Modo...) do not export proper normals!
				/*float fSqrLength = vNrm.SqrLength();
				if(fSqrLength < 0.97f || fSqrLength > 1.03)
				{
					Error("Bad normal %d in mesh %s! Length is not 1. Will not cache file!\n", i, cString::To8Char(asFile).c_str());
					bMeshIsOKToCache = false;
				}*/
			}
		}

		////////////////////////
		//Create Skeleton
		cSkeleton * pSkeleton = NULL;
		if(vColladaControllers.empty() == false)
		{
			pSkeleton = hplNew( cSkeleton, () );

			///////////////////////////////////////////
			//Create the bones according to nodes in the scene.
			// This will give each bone a local matrix.
			tColladaNodeListIt it = ColladaScene.mRoot.mlstChildren.begin();
			for(; it != ColladaScene.mRoot.mlstChildren.end(); it++)
			{
				cColladaNode *pNode = *it;

				CreateSkeletonBone(pNode, pSkeleton->GetRootBone());
			}

			////////////////////////////////////
			//Set the bind position of the bones
			//(This will set the local matrix as the global bind)
			for(size_t i=0; i< vColladaControllers.size();i++)
			{
				cColladaController &Ctrl = vColladaControllers[i];

				for(size_t j=0; j<Ctrl.mvJoints.size(); j++)
				{
					cBone *pBone = pSkeleton->GetBoneBySid(Ctrl.mvJoints[j]);

					if(pBone)
					{
						pBone->SetTransform(cMath::MatrixInverse(Ctrl.mvMatrices[j]));
						pBone->SetValue(1);
					}
					else
					{
						Log("Bone '%s' does not exist\n",Ctrl.mvJoints[j].c_str());
					}
				}
			}

			////////////////////////////////////////////////
			//Do another pass and calculate the local matrix
			//Need to do this because the matrices stored in Controllers are in world space!
			cBoneIterator BoneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cMatrixf mtxRoot = cMatrixf::Identity;
				CalcLocalMatrixAfterControllerBindMatrixRec(BoneIt.Next(), mtxRoot,0);
			}
		}

		////////////////////////////////////
		//Create Mesh
		cMesh *pMesh = hplNew( cMesh, (cString::To8Char(asFile), asFile, mpMaterialManager,mpAnimationManager) );

		//Set the skeleton to the mesh
		if(pSkeleton) pMesh->SetSkeleton(pSkeleton);

		///////////////////////////
		//Create Sub meshes
		std::vector<cMeshCollider> vMeshColliders;
		for(size_t i=0;i<vColladaGeometries.size(); i++)
		{
			cColladaGeometry &Geom = vColladaGeometries[i];
			
			cColladaNode *pGeomNode = ColladaScene.GetNodeFromSource(Geom.msId);
			if(pGeomNode==NULL)
			{
				pGeomNode = GetNodeFromController(Geom.msId,vColladaControllers,ColladaScene);
				if(pGeomNode==NULL){
					Error("No node with geometry id '%s'\n",Geom.msId.c_str());
					continue;
				}
			}
			tString sNodeName = pGeomNode->msName;
			bool bCollideMeshShape = false;

			/////////////////////////////////////////////////////
			//If the name starts with '_' it is a special object.
			if(sNodeName.length()>0 && sNodeName[0] == '_')
			{
				tStringVec vStrings;
				tString sSepp = "_";
				cString::GetStringVec(sNodeName,vStrings,&sSepp);

				tString sSpecialName = cString::ToLowerCase(vStrings[0]);
				tString sTypeName = vStrings.size()<=1 ? "" : vStrings[1];
				
				bool bCreateMesh = true;
				
				bCollideMeshShape = true; //If mesh is created (is type name is "mesh"), then set it as a CollideSHape
				
				/////////////////////////////////////
				//COLLIDER
				if(	(sSpecialName == "collider" || sSpecialName == "charcollider") && vStrings.size()>1 && sTypeName != "mesh")
				{
					bCreateMesh = false;

					tFloatVec vVertexVec;
					tVertexVec &vArray = Geom.mvVertexVec;
					vVertexVec.resize(vArray.size() *3);

					for(size_t vtx=0; vtx < vArray.size(); ++vtx)
					{
						vVertexVec[vtx*3 + 0] = vArray[vtx].pos.x;
						vVertexVec[vtx*3 + 1] = vArray[vtx].pos.y;
						vVertexVec[vtx*3 + 2] = vArray[vtx].pos.z;
					}

					cBoundingVolume TempBV;
					TempBV.AddArrayPoints(&vVertexVec[0],(int)vArray.size());
					TempBV.CreateFromPoints(3);
                
					tString sShapeType = cString::ToLowerCase(vStrings[1]);
					eCollideShapeType ShapeType = eCollideShapeType_Box;
					cVector3f vShapeSize = TempBV.GetSize();

					if(sShapeType == "box"){
						ShapeType = eCollideShapeType_Box;
					}
					else if(sShapeType == "sphere"){
						ShapeType = eCollideShapeType_Sphere;
						vShapeSize *= cVector3f(0.5f);
					}
					else if(sShapeType == "capsule"){
						ShapeType = eCollideShapeType_Capsule;
						vShapeSize.x *= 0.5;
					}
					else if(sShapeType == "cylinder"){
						ShapeType = eCollideShapeType_Cylinder;
						vShapeSize.x *= 0.5;
					}

					cMeshCollider meshCollider;
					meshCollider.mType = ShapeType;
					meshCollider.mvSize = vShapeSize;
					meshCollider.mbCharCollider = sSpecialName == "charcollider";

					cColladaNode *pNode = ColladaScene.GetNodeFromSource(Geom.msId);
					if(pNode==NULL){Warning("No node for geometry '%s' when creating collider!\n",Geom.msId.c_str()); continue;}

					//Set the name of the group it is in.
					if(pNode->pParent)
					{
						meshCollider.msGroup = pNode->pParent->msName;
					}
					else
					{
						meshCollider.msGroup = "";
					}

					//Set offset, some primitives are created with the centre at the bottom,
					//fix this.
					//Check if the centre is not in the middle
					TempBV.SetPosition(pNode->m_mtxWorldTransform.GetTranslation());
					if(TempBV.GetWorldCenter() != pNode->m_mtxWorldTransform.GetTranslation())
					{
						cVector3f vOffset = TempBV.GetWorldCenter() -
											pNode->m_mtxWorldTransform.GetTranslation();
						vOffset = vOffset * pNode->mvScale;
					
						//World postion add
						meshCollider.m_mtxOffset = pNode->m_mtxWorldTransform;
						cVector3f vRotOffset = cMath::MatrixMul(meshCollider.m_mtxOffset.GetRotation(),
																vOffset);
						meshCollider.m_mtxOffset.SetTranslation(meshCollider.m_mtxOffset.GetTranslation() +
																vRotOffset);
					}
					else
					{
						meshCollider.m_mtxOffset = pNode->m_mtxWorldTransform;
					}
		
					
					//Add scale
					meshCollider.mvSize = meshCollider.mvSize * pNode->mvScale;

					//This is to orient the cylinder along y axis instead of x.
					if(ShapeType == eCollideShapeType_Cylinder || ShapeType == eCollideShapeType_Capsule)
					{
						meshCollider.m_mtxOffset = cMath::MatrixMul(meshCollider.m_mtxOffset,
																cMath::MatrixRotateZ(cMath::ToRad(90))
																);
					}

					vMeshColliders.push_back(meshCollider);
				}

				if(bCreateMesh==false)
					continue;
			}
			////////////////////////////////////////////
			// Luis: this is redundant, the node was retrieved above
			//Getting group the sub mesh is in
			/*
			cColladaNode *pNode = ColladaScene.GetNodeFromSource(Geom.msId);
			if(pNode==NULL)
			{
				pNode = GetNodeFromController(Geom.msId,vColladaControllers,ColladaScene);
				if(pNode==NULL){
					Warning("No node for geometry '%s' found when trying to create sub mesh, check in controllers.\n",Geom.msId.c_str());
					continue;
				}
			}*/

			// Sub mesh name and node name are the same.
			//tString sSubMeshName = pNode->msName;
			cSubMesh* pSubMesh = pMesh->CreateSubMesh(sNodeName);
			
			pSubMesh->SetLocalTransform(pGeomNode->m_mtxTransform);

			pSubMesh->SetIsCollideShape(bCollideMeshShape);
			
			//Set the scale
			pSubMesh->SetModelScale(pGeomNode->mvScale);

			//Log("Creating submesh: '%s'\n",Geom.msName.c_str());

			//To be filled by extra vertex positions (not used, use the one in Geometry)
			tColladaExtraVtxListVec &vExtraVtxVec = Geom.mvExtraVtxVec;

			//////////////////////////////
			// Create Vertex buffer
			eVertexBufferUsageType UsageType = eVertexBufferUsageType_Static;
			iVertexBuffer *pVtxBuffer = CreateVertexBuffer(Geom, UsageType);//, vExtraVtxVec);
			pSubMesh->SetVertexBuffer(pVtxBuffer);

			//Create an extra set of vertices for shadow rendering
			//Not needed... no stencil shadows.
			//pVtxBuffer->CreateShadowDouble(true);

			/////////////////////////////
			//Add material
			tString sNodeMaterial = pGeomNode->msInstanceMaterial != "" ? pGeomNode->msInstanceMaterial : Geom.msMaterial;
            tString sMatName = GetMaterialTextureFile(sNodeMaterial,vColladaMaterials,vColladaTextures,	vColladaImages);
			
			//Log("Material name: '%s'\n",sMatName.c_str());
			if(sMatName != "")
			{
				//Set the path to material according to where the model is!
				tWString sRelativePath = cString::GetRelativePathW(cString::GetFilePathW(asFile), cPlatform::GetWorkingDir());
				sMatName = cString::SetFilePath(sMatName, cString::To8Char(sRelativePath));

				//Save the full path to material name!
				pSubMesh->SetMaterialName(cString::SetFileExt(sMatName,"mat"));

				//Use Fastload material if set
				if(	mpMeshManager->GetUseFastloadMaterial())
				{
					sMatName = mpMeshManager->GetFastloadMaterial();
				}

				////////////////////////////
				// Create a fake material (needed so mat name is saved!)
				if(aFlags & eMeshLoadFlag_NoMaterial || sMatName == "")
				{
                   //Do nothing!
				}
				////////////////////////////
				// Load the material
				else
				{
					cMaterial *pMaterial = mpMaterialManager->CreateMaterial(sMatName);
					if(pMaterial==NULL)
					{
						Error("Couldn't create material '%s' for object '%s'\n",sMatName.c_str(), Geom.msName.c_str());
						//NOTE: Even if the material is null we still want to load the model!
					}
					pSubMesh->SetMaterial(pMaterial);
				}
			}
			else
			{
				pSubMesh->SetMaterialName("");
			}

			/////////////////////////////
			//If there is a controller for the mesh, get vertex-bone pairs.
			//First find the controller.
			cColladaController *pCtrl =NULL;
			for(size_t j=0; j<vColladaControllers.size(); j++){
				if(vColladaControllers[j].msTarget == Geom.msId){
					pCtrl = &vColladaControllers[j];
					break;
				}
			}
			//If no controller can be found, guess the one with target=""
			if(pCtrl==NULL){
				for(size_t j=0; j<vColladaControllers.size(); j++){
					if(vColladaControllers[j].msTarget == ""){
						pCtrl = &vColladaControllers[j];
						Warning("Guessing controller as target=''!\n");
						break;
					}
				}
			}

			//Add the pairs
			if(pCtrl && pSkeleton)
			{
				//Log("Adding vertex-bone pairs!\n");
				//Iterate the pairs
				for(size_t j=0; j<pCtrl->mvPairs.size(); j++)
				{
					//Get all vertices for this vertex pos
					tColladaExtraVtxListIt ExtraIt = vExtraVtxVec[j].begin();
					for(; ExtraIt != vExtraVtxVec[j].end(); ++ExtraIt)
					{
						cColladaExtraVtx &Extra = *ExtraIt;

						//Iterate all the influences for this vertex.
						tColladaJointPairListIt PairIt = pCtrl->mvPairs[j].begin();
						for(; PairIt != pCtrl->mvPairs[j].end(); ++PairIt)
						{
							cColladaJointPair &SrcPair = *PairIt;
							cVertexBonePair DestPair;

							int lBoneIdx = pSkeleton->GetBoneIndexBySid(pCtrl->mvJoints[SrcPair.mlJoint]);
                            DestPair.boneIdx = lBoneIdx;
							DestPair.weight = pCtrl->mvWeights[SrcPair.mlWeight];
							DestPair.vtxIdx = Extra.mlNewVtx;

							//Add pair in sub mesh
							pSubMesh->AddVertexBonePair(DestPair);

							//Log("Added pair: bone %d vtx %d weight: %f\n", DestPair.boneIdx,DestPair.vtxIdx,
							//												DestPair.weight);
						}
					}
				}

				//Transform the mesh according to the bind transform.
				pVtxBuffer->Transform(cMath::MatrixMul(cMath::MatrixScale(mfUnitScale), pCtrl->m_mtxBindShapeMatrix));
			}
			//////////////////////////////////////////////////
			//No controller, we have a non skinned mesh!
			else
			{
				//If the buffer has animations or joints, we only add the scale part of the
				//transform
				cMatrixf mtxScale = cMath::MatrixScale(pGeomNode->mvScale * mfUnitScale);
				pVtxBuffer->Transform(mtxScale);
			}

			pSubMesh->Compile();

			//Compile the vertex buffer
			pVtxBuffer->Compile(0);//eVertexCompileFlag_CreateTangents);
		}

		/////////////////////////////////////
		//Compile the bones and submeshes
		pMesh->CompileBonesAndSubMeshes();

		///////////////////////////////////////////////
		// Set colliders to sub meshes
		if(vMeshColliders.empty()==false && pMesh->GetSubMeshNum()>0)
		{
			for(size_t i=0; i<vMeshColliders.size(); ++i)
			{
				cMeshCollider& meshCollider = vMeshColliders[i];
				cSubMesh *pSubMesh = NULL;
				if(meshCollider.msGroup!="")
				{
					pSubMesh = pMesh->GetSubMeshName(meshCollider.msGroup);
					if(pSubMesh==NULL){
						Log("Sub mesh '%s' for collider was not found!\n",meshCollider.msGroup.c_str());
						continue;
					}
				}
				else
				{
					pSubMesh = pMesh->GetSubMesh(0);
				}
			
                cMeshCollider *pCollider = pSubMesh->CreateCollider(meshCollider.mType);				
				*pCollider = meshCollider;
			}
		}

		///////////////////////////////////////////////
		// Add Node Hierarchy
		// only add if there is no skeleton.
		if(pSkeleton == NULL)
		{
			cNode3D* pRootNode = pMesh->GetRootNode();

			tColladaNodeListIt it = ColladaScene.mRoot.mlstChildren.begin();
			for(; it != ColladaScene.mRoot.mlstChildren.end(); ++it)
			{
				CreateHierarchyNodes(pMesh, pRootNode,*it,vColladaGeometries);
			}

			//Clear scale on all joint nodes.
			for(int i=0; i<pMesh->GetNodeNum(); i++)
			{
				cNode3D* pMeshNode = pMesh->GetNode(i);
				cMatrixf mtxNode = pMeshNode->GetLocalMatrix();

				cSubMesh *pSubMesh = pMesh->GetSubMeshName(pMeshNode->GetName());

				if(pSubMesh)
				{
					//Clear the scale
					cMatrixf mtxScale = cMath::MatrixScale(pSubMesh->GetModelScale());
					mtxNode = cMath::MatrixMul(mtxNode, cMath::MatrixInverse(mtxScale));
				}
				else if(pMeshNode->GetName() != "" && pMeshNode->GetName()[0] != '_' &&
						pMeshNode->GetCustomFlags() != 0)
				{
					Error("Cannot find submesh '%s' in node!\n",pMeshNode->GetName().c_str());
				}

				pMeshNode->SetMatrix(mtxNode);
			}
		}
		
		///////////////////////////////////////////////
		// Create Animations
		cAnimation *pAnimation = NULL;
		if(vColladaAnimations.empty() == false)
		{
			pAnimation = hplNew( cAnimation, (pMesh->GetName(), asFile, cString::GetFileName(cString::To8Char(asFile))) );

			pAnimation->SetAnimationName("Default");
			pAnimation->ReserveTrackNum((int)vColladaAnimations.size());

			pMesh->AddAnimation(pAnimation);

			///////////////////////////////////////////////////
			// Go through all tracks and add them to the animation
			float fStart = 9999999.0f;
			float fEnd = 0;
			for(size_t i=0; i < vColladaAnimations.size(); i++)
			{
				cAnimationTrack *pTrack = CreateAnimTrack(pAnimation, pSkeleton,
														vColladaAnimations[i],&ColladaScene);
				if(pTrack==NULL) continue;
				
				//Update the state and end times if needed
				if(pTrack->GetKeyFrameNum() >0)
				{
					cKeyFrame *pFirst = pTrack->GetKeyFrame(0);
					cKeyFrame *pLast = pTrack->GetKeyFrame(pTrack->GetKeyFrameNum()-1);

					if(fStart > pFirst->time)	fStart = pFirst->time;
					if(fEnd < pLast->time)		fEnd = pLast->time;
				}
			}
			
			if(ColladaScene.mfDeltaTime >= 0)
			{
				pAnimation->SetLength(ColladaScene.mfDeltaTime);
			}
			else
			{
				pAnimation->SetLength(fEnd - fStart);
			}
			
			CleanUpNonAnimatedNodes(pAnimation,pSkeleton, &ColladaScene);
		}

		///////////////
		// Rotate/Scale skeleton if needed
		if(pSkeleton && (mbZToY || mfUnitScale != 1.0))
		{
			cMatrixf mtxScale = cMath::MatrixScale(mfUnitScale);
			//pSkeleton->GetRootBone()->SetTransform(cMath::MatrixMul(mtxScale, pSkeleton->GetRootBone()->GetLocalTransform()));

			if(mbZToY) mtxScale = cMath::MatrixMul(mtxScale, m_mtxZToY);
			
			cBoneIterator BoneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cBone *pRoot = BoneIt.Next();
				pRoot->SetTransform(cMath::MatrixMul(mtxScale, pRoot->GetLocalTransform()));

				if(pMesh)
				{
					int lBoneIdx = pSkeleton->GetBoneIndexBySid(pRoot->GetSid());
					////////
					// Apply scale to the translation of root bones animation
					for(int i = 0; i < pMesh->GetAnimationNum(); ++i)
					{
						cAnimation *pAnimation = pMesh->GetAnimation(i);

						cAnimationTrack *pTrack = pAnimation->GetTrack(lBoneIdx);

						for(int j = 0; j < pTrack->GetKeyFrameNum(); ++j)
						{
							pTrack->GetKeyFrame(j)->trans = pTrack->GetKeyFrame(j)->trans * mfUnitScale;
						}
					}
				}
			}
		}

		/////////////////////////////////////////////////
		// SAVE MSH FORMAT
		if(	cResources::GetForceCacheLoadingAndSkipSaving()==false && 
			mbLoadAndSaveMSHFormat && bMeshIsOKToCache)
		{
			tWString sMSHFile = cString::SetFileExtW(asFile, _W("msh"));
			
			mpMeshLoaderMSH->SaveMesh(pMesh, sMSHFile);
		}


		///////////////////////////////////
		///////// DEBUG ///////////////////
		///////////////////////////////////
		// Test anim data
		/*Log("ANIMATIONS:\n");
		for(size_t i=0; i < vColladaAnimations.size(); i++)
		{
			cColladaAnimation &Anim = vColladaAnimations[i];

			Log("Anim: id: '%s' name: '%s'\n",Anim.msId.c_str(), Anim.msName.c_str());

			Log("Channels:\n");
			for(size_t j=0; j< Anim.mvChannels.size(); j++)
				Log("Id: '%s' Source: '%s' Target: '%s'\n",
																Anim.mvChannels[j].msId.c_str(),
																Anim.mvChannels[j].msSource.c_str(),
																Anim.mvChannels[j].msTarget.c_str());

			Log("Samplers:\n");
			for(size_t j=0; j< Anim.mvSamplers.size(); j++)
				Log("Id: '%s' Target: '%s' Time: '%s' Values: '%s'\n",		Anim.mvSamplers[j].msId.c_str(),
																Anim.mvSamplers[j].msTarget.c_str(),
																Anim.mvSamplers[j].msTimeArray.c_str(),
																Anim.mvSamplers[j].msValueArray.c_str());
			Log("Sources:\n");
			for(size_t j=0; j< Anim.mvSources.size(); j++)
			{
				Log("Id: %s\n",Anim.mvSources[j].msId.c_str());
				for(size_t k=0;k < Anim.mvSources[j].mvValues.size(); k++)
					Log("%f, ",Anim.mvSources[j].mvValues[k]);
				Log("\n");
			}

			//CreateAnimTrack(NULL, Anim,&ColladaScene);
		}*/

		return pMesh;
	}

	//-----------------------------------------------------------------------

	cAnimation* cMeshLoaderCollada::LoadAnimation(const tWString& asFile)
	{

		/////////////////////////////////////////////////
		// TRY USING MSH LOADER
		if(mbLoadAndSaveMSHFormat)
		{
			tWString sMSHFile = cString::SetFileExtW(asFile, _W("anm"));
			cDate currentDate = cPlatform::FileModifiedDate(asFile);
			cDate mshDate = cPlatform::FileModifiedDate(sMSHFile);

			if(	cResources::GetForceCacheLoadingAndSkipSaving() ||
				mshDate > currentDate || cPlatform::FileExists(asFile)==false)
			{
				cAnimation *pAnim = mpMeshLoaderMSH->LoadAnimation(sMSHFile);
				if(pAnim)
				{
					pAnim->SetFullPath(asFile);//Use dae as full path! (otherwise file will be loaded several times!)
					return pAnim;
				}
			}
		}

		//Images
		tColladaImageVec vColladaImages;
		//Textures
		tColladaTextureVec vColladaTextures;
		//Materials
		tColladaMaterialVec vColladaMaterials;
		//Lights
		tColladaLightVec vColladaLights;
		//Geometries
		tColladaGeometryVec vColladaGeometries;
		//Controllers
		tColladaControllerVec vColladaControllers;
		//Animations
		tColladaAnimationVec vColladaAnimations;
		//Scene
		cColladaScene ColladaScene;

		bool bRet = FillStructures(asFile, NULL,NULL,
			NULL,NULL,
			&vColladaGeometries, &vColladaControllers,
			&vColladaAnimations,
			&ColladaScene,true);

		if(bRet==false) return NULL;

		////////////////////////
		//Create Skeleton
		cSkeleton * pSkeleton = NULL;
		if(vColladaControllers.empty() == false)
		{
			pSkeleton = hplNew( cSkeleton, () );

			tColladaNodeListIt it = ColladaScene.mRoot.mlstChildren.begin();
			for(; it != ColladaScene.mRoot.mlstChildren.end(); it++)
			{
				cColladaNode *pNode = *it;

				CreateSkeletonBone(pNode, pSkeleton->GetRootBone());
			}

			////////////////////////////////////
			//Set the bind transform of the bones
			//(This will set the local matrix as the global bind)
			for(size_t i=0; i< vColladaControllers.size();i++)
			{
				cColladaController &Ctrl = vColladaControllers[i];

				for(size_t j=0; j<Ctrl.mvJoints.size(); j++)
				{
					cBone *pBone = pSkeleton->GetBoneBySid(Ctrl.mvJoints[j]);

					if(pBone)
					{
						pBone->SetTransform(cMath::MatrixInverse(Ctrl.mvMatrices[j]));
						pBone->SetValue(1);
					}
					else
					{
						Log("Bone '%s' does not exist.\n",Ctrl.mvJoints[j].c_str());
					}
				}
			}

			////////////////////////////////////////////////
			//Do another pass and calculate the local matrix
			cBoneIterator BoneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cMatrixf mtxRoot = cMatrixf::Identity;
				CalcLocalMatrixAfterControllerBindMatrixRec(BoneIt.Next(), mtxRoot,0);
			}
		}

		
		///////////////////////////////////////////////
		// Create Animations
		cAnimation *pAnimation = NULL;
		if(vColladaAnimations.empty() == false)
		{
			tString sFileName = cString::GetFileName(cString::To8Char(asFile));
			pAnimation = hplNew( cAnimation, (cString::To8Char(asFile),asFile, sFileName) );

			pAnimation->ReserveTrackNum((int)vColladaAnimations.size());

			
			float fStart = 9999999.0f;
			float fEnd = 0;
			for(size_t i=0; i < vColladaAnimations.size(); i++)
			{
				cAnimationTrack *pTrack = CreateAnimTrack(pAnimation, pSkeleton, vColladaAnimations[i],&ColladaScene);
				if(pTrack==NULL) continue;
								
				//Update the state and end times if needed
				if(pTrack->GetKeyFrameNum() >0)
				{
					cKeyFrame *pFirst = pTrack->GetKeyFrame(0);
					cKeyFrame *pLast = pTrack->GetKeyFrame(pTrack->GetKeyFrameNum()-1);

					if(fStart > pFirst->time)	fStart = pFirst->time;
					if(fEnd < pLast->time)		fEnd = pLast->time;
				}
			}

			if(ColladaScene.mfDeltaTime >= 0)
			{
				pAnimation->SetLength(ColladaScene.mfDeltaTime);
			}
			else
			{
				pAnimation->SetLength(fEnd - fStart);
			}

			CleanUpNonAnimatedNodes(pAnimation,pSkeleton, &ColladaScene);
		}

		///////////////
		// Rotate/Scale skeleton if needed
		if(pSkeleton && (mbZToY || mfUnitScale != 1.0))
		{
			cMatrixf mtxScale = cMath::MatrixScale(mfUnitScale);
			//pSkeleton->GetRootBone()->SetTransform(cMath::MatrixMul(mtxScale, pSkeleton->GetRootBone()->GetLocalTransform()));

			if(mbZToY) mtxScale = cMath::MatrixMul(mtxScale, m_mtxZToY);
			
			cBoneIterator BoneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(BoneIt.HasNext())
			{
				cBone *pRoot = BoneIt.Next();
				pRoot->SetTransform(cMath::MatrixMul(mtxScale, pRoot->GetLocalTransform()));

				if(pAnimation)
				{
					int lBoneIdx = pSkeleton->GetBoneIndexBySid(pRoot->GetSid());
					////////
					// Apply scale to the translation of root bones animation
					cAnimationTrack *pTrack = pAnimation->GetTrack(lBoneIdx);

					for(int j = 0; j < pTrack->GetKeyFrameNum(); ++j)
					{
						pTrack->GetKeyFrame(j)->trans = pTrack->GetKeyFrame(j)->trans * mfUnitScale;
					}
				}
			}
		}


		if(pSkeleton) hplDelete(pSkeleton);

		/////////////////////////////////////////////////
		// SAVE MSH FORMAT
		if(cResources::GetForceCacheLoadingAndSkipSaving()==false && mbLoadAndSaveMSHFormat)
		{
			tWString sMSHFile = cString::SetFileExtW(asFile, _W("anm"));

			mpMeshLoaderMSH->SaveAnimation(pAnimation, sMSHFile);
		}

		return pAnimation;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * Go through all bones and check if they have an animation
	 * If not create single frame with the node pose.
	 */
	void cMeshLoaderCollada::CleanUpNonAnimatedNodes(cAnimation *apAnimation, cSkeleton *apSkeleton, cColladaScene* apColladaScene )
	{
		///////////////////////////
		//Skeleton
		if(apSkeleton)
		{
			for(int i=0; i< apSkeleton->GetBoneNum(); ++i)
			{
				cBone *pBone = apSkeleton->GetBoneByIndex(i);

				//Check if there is no animation for this node.
				if(apAnimation->GetTrackByName(pBone->GetName())==NULL)
				{
					cColladaNode *pNode = apColladaScene->GetNode(pBone->GetName());
					if(pNode)
					{
						cMatrixf mtxLocal = pNode->m_mtxTransform;
						cMatrixf mtxBone = pBone->GetLocalTransform();
						cMatrixf mtxInvBone = cMath::MatrixInverse(mtxBone);

						cMatrixf mtxRotChange = cMath::MatrixMul(mtxLocal.GetRotation(), mtxInvBone.GetRotation());
						cVector3f vTransChange = mtxLocal.GetTranslation() - mtxBone.GetTranslation();
						
						cAnimationTrack * pTrack = apAnimation->CreateTrack(pBone->GetName(),eAnimTransformFlag_Rotate | eAnimTransformFlag_Translate);
						cKeyFrame *pFrame = pTrack->CreateKeyFrame(0);

						//Translation
						pFrame->trans = vTransChange;
						
						//Quaternion
						pFrame->rotation = cQuaternion::Identity;
						pFrame->rotation.FromRotationMatrix(mtxRotChange);
					}
					else
					{
						Warning("Couldn't find node for bone '%s'\n",pBone->GetName().c_str());
					}
				}
			}
		}
		///////////////////////////
		//Nodes
		else
		{
			tColladaNodeListIt it = apColladaScene->mlstNodes.begin();
			for(; it != apColladaScene->mlstNodes.end();++it)
			{	
				cColladaNode *pNode = *it;
				
				//Check if there is no animation for this node.
				if(apAnimation->GetTrackByName(pNode->msName)==NULL)
				{
					cAnimationTrack * pTrack = apAnimation->CreateTrack(pNode->msId, eAnimTransformFlag_Rotate | eAnimTransformFlag_Translate);
					pTrack->SetNodeIndex(-1);
					cKeyFrame *pFrame = pTrack->CreateKeyFrame(0);

					cMatrixf mtxFrameTransform = pNode->m_mtxTransform;
					
					//translation
					pFrame->trans = mtxFrameTransform.GetTranslation();
					
					//Quaternion
					mtxFrameTransform.SetTranslation(0);
					pFrame->rotation = cQuaternion::Identity;
					pFrame->rotation.FromRotationMatrix(mtxFrameTransform);
				}
				else if(mbZToY || mfUnitScale != 1.0f)
				{
					cAnimationTrack *pTrack = apAnimation->GetTrackByName(pNode->msName);

					for(int i = 0; i < pTrack->GetKeyFrameNum(); ++i)
					{
						if(mbZToY)
						{
							cMatrixf mtxRot;
							pTrack->GetKeyFrame(i)->rotation.ToRotationMatrix(mtxRot);
							mtxRot = cMath::MatrixMul(mtxRot, m_mtxZToY);
							pTrack->GetKeyFrame(i)->rotation.FromRotationMatrix(mtxRot);
						}

						pTrack->GetKeyFrame(i)->trans = pTrack->GetKeyFrame(i)->trans * mfUnitScale;
					}
				}
			}
		}
	}


	//-----------------------------------------------------------------------


	tString cMeshLoaderCollada::GetParentName(cColladaNode *apNode, tColladaGeometryVec *apColladaGeometries)
	{
		tString sParent = "";
		if(apNode->pParent)
		{
			sParent = apNode->pParent->msName;
			//If the parent is a geometry the geometry name must be used.
			if(apNode->pParent->msSource != "")
			{
				cColladaGeometry *pGeom = GetGeometry(apNode->pParent->msSource,*apColladaGeometries);
				if(pGeom)
				{
					sParent = pGeom->msName;
				}
			}
		}

		return sParent;
	}

	//-----------------------------------------------------------------------

	static bool HasParam(const tStringVec & avVec, const tString &asParam)
	{
		for(int i=0; i< (int)avVec.size(); i++)
		{
			if(cString::ToLowerCase(avVec[i]) == asParam){
				return true;
			}
		}

		return false;
	}
	
	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::CreateHierarchyNodes(cMesh *apMesh, cNode3D* mpParentNode,
													cColladaNode* apColladaNode,
													tColladaGeometryVec &avColladaGeom)
	{
		cNode3D* pNode = mpParentNode->CreateChild(apColladaNode->msName);
		apMesh->AddNode(pNode);
		
		pNode->SetMatrix(apColladaNode->m_mtxTransform);

		//Set if this node has any geometry source
		if(apColladaNode->msSource!="")		pNode->SetCustomFlags(1);
		else								pNode->SetCustomFlags(0);

		//Log("Node: %s\n",apColladaNode->msName.c_str());
		//Log(" local node transform: %s\n",pNode->GetLocalPosition().ToString().c_str());
		//Log(" local collada transform: %s\n",apColladaNode->m_mtxTransform.GetTranslation().ToString().c_str());
		//Log(" world transform: %s\n",apColladaNode->m_mtxWorldTransform.GetTranslation().ToString().c_str());

		pNode->SetPosition(pNode->GetLocalPosition() * mfUnitScale);
		
		//Iterate children
		tColladaNodeListIt it = apColladaNode->mlstChildren.begin();
		for(; it != apColladaNode->mlstChildren.end(); ++it)
		{
			if(mbZToY)
			{
				//(*it)->m_mtxTransform = cMath::MatrixMul(m_mtxZToY, (*it)->m_mtxTransform);
			}

			CreateHierarchyNodes(apMesh, pNode,*it,avColladaGeom);
		}
	}



	//-----------------------------------------------------------------------

	cColladaGeometry* cMeshLoaderCollada::GetGeometry(const tString& asId, tColladaGeometryVec &avGeomVec)
	{
		for(size_t i=0; i<avGeomVec.size(); i++)
		{
			if(avGeomVec[i].msId == asId)
			{
				return &avGeomVec[i];
			}
		}

		return NULL;
	}

	//-----------------------------------------------------------------------

	cColladaLight* cMeshLoaderCollada::GetLight(const tString& asId, tColladaLightVec &avLightVec)
	{
		for(size_t i=0; i<avLightVec.size(); i++)
		{
			if(avLightVec[i].msId == asId)
			{
				return &avLightVec[i];
			}
		}

		return NULL;
	}

	//-----------------------------------------------------------------------
	
}
