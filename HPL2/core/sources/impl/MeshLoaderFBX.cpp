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

#include "impl/MeshLoaderFBX.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "graphics/Material.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "impl/MeshLoaderMSH.h"

#include "math/Math.h"

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(mpSdkManager->GetIOSettings()))
#endif

namespace hpl {

	static KFbxXMatrix GetGeometry(KFbxNode* pNode) {
		KFbxVector4 lT, lR, lS;
		KFbxXMatrix lGeometry;

		lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
		lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
		lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

	    lGeometry.SetT(lT);
	    lGeometry.SetR(lR);
	    lGeometry.SetS(lS);

	    return lGeometry;
	}

	static KFbxMatrix GetGeometryTwo(KFbxNode* pNode) {
		KFbxVector4 lT, lR, lS;

		lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
		lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
		lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

	    return KFbxMatrix( lT, lR, lS );
	}

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cMeshLoaderFBX::cMeshLoaderFBX(iLowLevelGraphics *apLowLevelGraphics, cMeshLoaderMSH *apMeshLoaderMSH, bool abLoadAndSaveMSHFormat) : iMeshLoader(apLowLevelGraphics)
	{
		mpMeshLoaderMSH = apMeshLoaderMSH;
		mbLoadAndSaveMSHFormat = abLoadAndSaveMSHFormat;

		mpSdkManager = KFbxSdkManager::Create();

		AddSupportedExtension("fbx");

		KFbxIOSettings * ios = KFbxIOSettings::Create(mpSdkManager, IOSROOT );
		mpSdkManager->SetIOSettings(ios);

		mbLog = true;
		mbLowLog = false;
	}
	
	//-----------------------------------------------------------------------

	cMeshLoaderFBX::~cMeshLoaderFBX()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cMesh* cMeshLoaderFBX::LoadMesh(const tWString& asFile, tMeshLoadFlag aFlags)
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

		KFbxScene * pScene = KFbxScene::Create(mpSdkManager,"");
		KFbxImporter* pImporter = KFbxImporter::Create(mpSdkManager,"");

		LoadScene(mpSdkManager, pScene, cString::To8Char(asFile).c_str());

		tSubMeshDataList lstSubData;

		cSkeleton* pSkeleton = new cSkeleton();
		
		if(mbLog)Log("Loading skeleton '%s':\n", asFile.c_str());
		LoadSkeletonRec(pSkeleton->GetRootBone(),pScene->GetRootNode(),0);
		
		//Check if any bones where found.
		if(pSkeleton->GetRootBone()->GetChildIterator().HasNext() == false)
		{
			delete pSkeleton;
			pSkeleton = NULL;
		}

		//Go through all nodes
		if(mbLog)Log("Loading scene '%s':\n", asFile.c_str());
		LoadSceneRec(&lstSubData,pSkeleton,NULL,pScene->GetRootNode(),0, false);

		//Finalize the bones. That is remove non-linked bones and
		//make the local matrices.
		if(pSkeleton)
		{
			cBoneIterator boneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(boneIt.HasNext())
			{
				cMatrixf mtxIdentity = cMatrixf::Identity;
				MakeFinalBonesRec(boneIt.Next(),mtxIdentity, mtxIdentity);
			}
		}
				
		//Load animations
		cAnimation *pAnimation = NULL;//LoadAnimations(pScene, pImporter, asFile,pSkeleton);

		//Clean up
		pScene->Destroy(true,true);
		pImporter->Destroy(true,true);
		
		//Create the mesh
		cMesh *pMesh = hplNew( cMesh, (cString::To8Char(asFile), asFile, mpMaterialManager,mpAnimationManager) );
		
		//Set skeleton.
		if(pSkeleton) pMesh->SetSkeleton(pSkeleton);

		//Add Animation
		if(pAnimation) pMesh->AddAnimation(pAnimation);

		tSubMeshDataListIt it = lstSubData.begin();
		for(;it != lstSubData.end();it++)
		{
			cSubMeshData& subData = *it;
			
			cSubMesh * pSubMesh = pMesh->CreateSubMesh(subData.msName);
            
			pSubMesh->SetVertexBuffer(subData.mpVtxBuffer);

			for(int i=0; i < (int)subData.mvVtxBonePairs.size(); i++)
			{
				cVertexBonePair VBPair;
				VBPair.boneIdx = subData.mvVtxBonePairs[i].boneIdx;
				VBPair.vtxIdx = subData.mvVtxBonePairs[i].vtxIdx;
				VBPair.weight = subData.mvVtxBonePairs[i].weight;

				pSubMesh->AddVertexBonePair(VBPair);
			}

			cMaterial *pMaterial = mpMaterialManager->CreateMaterial(subData.msMaterial);
			if(pMaterial)
			{
				pSubMesh->SetMaterial(pMaterial);
				pSubMesh->SetMaterialName(pMaterial->GetName());
			}
			else
			{
				pSubMesh->SetMaterialName("");
			}


			pSubMesh->Compile();
		}
		
		//Setup the joints (create new vertex coords, etc)
		if(pSkeleton) pMesh->CompileBonesAndSubMeshes();

		/////////////////////////////////////////////////
		// SAVE MSH FORMAT
		if(	cResources::GetForceCacheLoadingAndSkipSaving()==false && mbLoadAndSaveMSHFormat )
		{
			tWString sMSHFile = cString::SetFileExtW(asFile, _W("msh"));
			
			mpMeshLoaderMSH->SaveMesh(pMesh, sMSHFile);
		}
				
		return pMesh;
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	cAnimation* cMeshLoaderFBX::LoadAnimation(const tWString& asFile)
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

		KFbxScene * pScene = KFbxScene::Create(mpSdkManager,"");
		KFbxImporter* pImporter = KFbxImporter::Create(mpSdkManager,"");

		LoadScene(mpSdkManager, pScene, cString::To8Char(asFile).c_str());

		cSkeleton* pSkeleton = new cSkeleton();
		
		if(mbLog)Log("Loading skeleton '%s':\n", asFile.c_str());
		LoadSkeletonRec(pSkeleton->GetRootBone(),pScene->GetRootNode(),0);
		
		//Check if any bones where found.
		if(pSkeleton->GetRootBone()->GetChildIterator().HasNext() == false)
		{
			delete pSkeleton;
			pSkeleton = NULL;
		}

		//Go through all nodes
		//if(mbLog)Log("Loading scene '%s':\n", asFile.c_str());
		LoadSceneRec(NULL, pSkeleton,NULL,pScene->GetRootNode(),0, true);

		//Finalize the bones. That is remove non-linked bones and
		//make the local matrices.
		if(pSkeleton)
		{
			cBoneIterator boneIt = pSkeleton->GetRootBone()->GetChildIterator();
			while(boneIt.HasNext())
			{
				cMatrixf mtxIdentity = cMatrixf::Identity;
				MakeFinalBonesRec(boneIt.Next(),mtxIdentity, mtxIdentity);
			}
		}
				
		//Load animations
		cAnimation *pAnimation = LoadAnimations(pScene, pImporter, asFile,pSkeleton);

		//Clean up
		pScene->Destroy(true,true);
		pImporter->Destroy(true,true);
		
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

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	

	//-----------------------------------------------------------------------

	cAnimation*  cMeshLoaderFBX::LoadAnimations(KFbxScene *apScene, KFbxImporter* apImporter, const tWString& asFile, cSkeleton * apSkeleton)
	{
		int lTake = -1;

		KArrayTemplate<KString*> vStrings;
		apScene->FillAnimStackNameArray(vStrings);

		Log("Animations: ");
		for(int i=0;i<vStrings.GetCount();i++)
		{
			Log("'%s' ",vStrings[i]->Buffer());
		}
		Log("\n");

		int lStart;
		int lEnd;
		
        for(int i=0;i<vStrings.GetCount();i++)
		{
			KFbxTakeInfo* pTakeInfo = apScene->GetTakeInfo(*vStrings[i]);
			if(pTakeInfo==NULL)continue;

			Log("Info for '%s'\n",vStrings[i]->Buffer());

			lStart = pTakeInfo->mLocalTimeSpan.GetStart().GetMilliSeconds();
			lEnd = pTakeInfo->mLocalTimeSpan.GetStop().GetMilliSeconds();
			int lOffset = pTakeInfo->mImportOffset.GetMilliSeconds();
			int lAnimLength = lEnd - lStart;
			Log("TimeSpan: %d to %d\n",lStart,lEnd);
			Log("Offset: %d\n",lOffset);

			if(lAnimLength > 0){
				lTake = i;
				break;
			}
		}
		Log("\n");
		
		// If there where any animations in the file, create an animation and add the tracks to it.
		// Only 
		if(lTake != -1)
		{
			cAnimation*pAnimation = hplNew( cAnimation, (vStrings[lTake]->Buffer(), asFile, cString::GetFileName(cString::To8Char(asFile))) );
			pAnimation->SetLength( ((float)lEnd)/1000.0f );
			
			LoadAnimationRec(apScene,apScene->GetRootNode(),pAnimation,vStrings[lTake]->Buffer(),0, 
							cVector3f(0,0,0),cVector3f(1,1,1),cVector3f(0,0,0), apSkeleton);

			return pAnimation;
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	
	static KFCurve *GetCurve(KFbxNode *apNode, KFbxAnimLayer * apAnimLayer, tAnimTransformFlag aType, int alAxis)
	{
		KFCurve *pCurve=NULL;
		switch(aType)
		{
		case eAnimTransformFlag_Translate:
			if(alAxis==0) pCurve = apNode->LclTranslation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_T_X, true)->GetKFCurve();
			if(alAxis==1) pCurve = apNode->LclTranslation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_T_Y, true)->GetKFCurve();
			if(alAxis==2) pCurve = apNode->LclTranslation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_T_Z, true)->GetKFCurve();
			break;
		case eAnimTransformFlag_Scale:
			if(alAxis==0) pCurve = apNode->LclScaling.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_S_X, true)->GetKFCurve();
			if(alAxis==1) pCurve = apNode->LclScaling.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_S_Y, true)->GetKFCurve();
			if(alAxis==2) pCurve = apNode->LclScaling.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_S_Z, true)->GetKFCurve();
			break;
		case eAnimTransformFlag_Rotate:
			if(alAxis==0) pCurve = apNode->LclRotation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_R_X, true)->GetKFCurve();
			if(alAxis==1) pCurve = apNode->LclRotation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_R_Y, true)->GetKFCurve();
			if(alAxis==2) pCurve = apNode->LclRotation.GetCurve<KFbxAnimCurve>(apAnimLayer, KFCURVENODE_R_Z, true)->GetKFCurve();
			break;
		}

		return pCurve;
	}

	////////////////////////

	static void GetAnimTimes(tAnimTimeSet *pTimesSet,KFbxNode * apNode, KFbxAnimLayer * apAnimLayer, tAnimTransformFlag aType, int alAxis)
	{
		KFCurve *pCurve = GetCurve(apNode,apAnimLayer, aType, alAxis);

		for(int i=0;i<pCurve->KeyGetCount();i++)
		{
			KFCurveKey key = pCurve->KeyGet(i);
			float fTime = ((float)key.GetTime().GetMilliSeconds()) / 1000.0f;

			pTimesSet->insert(fTime);
		}
	}

	static void FillKeyVec(tTakeKeyDataVec *pVec,tAnimTimeSet *pTimesSet,
							KFbxNode *apNode,KFbxAnimLayer * apAnimLayer, tAnimTransformFlag aType, int alAxis)
	{
		KFCurve *pCurve = GetCurve(apNode, apAnimLayer,aType, alAxis);
		
		pVec->resize(pTimesSet->size());
		int i=0;
		tAnimTimeSetIt it = pTimesSet->begin();
		for(;it != pTimesSet->end(); it++)
		{
			KTime time;
			time.SetMilliSeconds((kLongLong)(*it * 1000));
			
			(*pVec)[i].mfTime = *it;
			(*pVec)[i].mfValue = pCurve->Evaluate(time);
			//Log("%f, ", (float) key.GetValue());
			i++;
		}
		
		/*//See if any time is missing if so insert it at the end.
		tAnimTimeSetIt it = pTimesSet->begin();
		for(;it != pTimesSet->end(); it++)
		{
			for(int i=0; i<pVec)
		}*/
	}


	////////////////////////

	static int FindTimeIndex(tTempKeyFrameDataVec *pVec, float afTime)
	{
		for(int i=0; i< (int)pVec->size(); i++)
		{
			if(std::abs(afTime - (*pVec)[i].mfTime) < kEpsilonf) return i;	
		}

		return -1;
	}

	////////////////////////

	static cVector3f GetAxisVector(int alAxis, float afValue)
	{
		switch(alAxis)
		{
		case 0: return cVector3f(afValue,0,0);
		case 1: return cVector3f(0,afValue,0);
		case 2: return cVector3f(0,0,afValue);
		}

		return cVector3f();
	}
	////////////////////////
	
	static void AddVectorElementsToTempArray(tTempKeyFrameDataVec* apDestVec,tTakeKeyDataVec* apSrcVec,
												tAnimTransformFlag aType)
	{
		for(int axis=0;axis<3;axis++)
		{
			for(int index = 0; index < (int)apSrcVec[axis].size(); index++)
			{
				cTakeKeyData *data = &apSrcVec[axis][index];
				int lPos = FindTimeIndex(apDestVec, data->mfTime);

				cVector3f vVec = GetAxisVector(axis,data->mfValue);
				

				//Log("type: %d time: %f axis: %d: vec: (%s) ",aType,data->mfTime, axis, vVec.ToString().c_str());

				if(lPos>=0)
				{
					//Log("adding to pos %d\n",lPos);
					switch(aType){
					case eAnimTransformFlag_Translate: (*apDestVec)[lPos].vTrans += vVec; break;
					case eAnimTransformFlag_Scale: (*apDestVec)[lPos].vScale += vVec; break;
					case eAnimTransformFlag_Rotate: (*apDestVec)[lPos].vRot += vVec; break;
					}
				}
				else
				{
					//Log("creating new frame\n");
					cTempKeyFrameData frame;
					
					//Clear all the values to 0
					frame.vRot =0; frame.vScale =0; frame.vTrans=0;
					frame.mfTime = data->mfTime;
					
					//Set the vector
					switch(aType){
					case eAnimTransformFlag_Translate: frame.vTrans += vVec; break;
					case eAnimTransformFlag_Scale: frame.vScale += vVec; break;
					case eAnimTransformFlag_Rotate: frame.vRot += vVec; break;
					}
					//Add frame with new time to array.
					apDestVec->push_back(frame);
				}
			}
		}
	}
	////////////////////////
	
	static cMatrixf MatrixFromEulerAngles(cVector3f avRot)
	{
		cMatrixf mtxRot = cMath::MatrixRotateX(cMath::ToRad(avRot.x));
		mtxRot = cMath::MatrixMul(cMath::MatrixRotateY(cMath::ToRad(avRot.y)), mtxRot);
		mtxRot = cMath::MatrixMul(cMath::MatrixRotateZ(cMath::ToRad(avRot.z)), mtxRot);
		
		return mtxRot;
	}
	
	////////////////////////
	
	void cMeshLoaderFBX::LoadAnimationRec(KFbxScene *apScene,KFbxNode * apNode, cAnimation* apAnimation,const tString &asAnimStackName, 
											int alDepth, 
											cVector3f vParentT, cVector3f vParentS, cVector3f vParentR, cSkeleton * apSkeleton)
	{
		const char * node_name = apNode->GetName();

		KFbxAnimStack * pAnimationStack = apScene->FindMember(FBX_TYPE(KFbxAnimStack), asAnimStackName.c_str());
		KFbxAnimLayer * pAnimLayer = pAnimationStack->GetMember(FBX_TYPE(KFbxAnimLayer), 0);

		if( pAnimationStack != NULL && apNode->GetSkeleton() )
		{
			tTakeKeyDataVec vTranslateKeys[3];
			tTakeKeyDataVec vRotateKeys[3];
			tTakeKeyDataVec vScaleKeys[3];
			
			tAnimTransformFlag transFlags = 0;

			tAnimTimeSet setTimes;
			
			for(int i=0;i<3;i++)
			{
				GetAnimTimes(&setTimes,apNode,pAnimLayer,eAnimTransformFlag_Translate, i);
				GetAnimTimes(&setTimes,apNode,pAnimLayer,eAnimTransformFlag_Rotate, i);
				GetAnimTimes(&setTimes,apNode,pAnimLayer,eAnimTransformFlag_Scale, i);
			}

			transFlags |= eAnimTransformFlag_Translate;
			transFlags |= eAnimTransformFlag_Rotate;

			KFbxVector4 pTranslation;
			KFbxQuaternion pRotation;
			KFbxVector4 pShearing;
			KFbxVector4 pScaling;
			double pSign;

			tTempKeyFrameDataVec vTempKeyFrame;
			vTempKeyFrame.resize(setTimes.size() );
			int i=0;
			tAnimTimeSetIt it = setTimes.begin();
			for(;it != setTimes.end(); it++)
			{
				KTime time;
				time.SetMilliSeconds((kLongLong)(*it * 1000));
				
				KFbxMatrix localTransform = apNode->EvaluateLocalTransform(time, KFbxNode::eSOURCE_SET);
				KFbxMatrix geometryMatrix = GetGeometryTwo(apNode);
				localTransform = localTransform * geometryMatrix;

				localTransform.GetElements( pTranslation, pRotation, pShearing, pScaling, pSign);

				vTempKeyFrame[i].mfTime = *it;
				vTempKeyFrame[i].vTrans = cVector3f( pTranslation.GetAt(0), pTranslation.GetAt(1), pTranslation.GetAt(2) );
				vTempKeyFrame[i].vScale = cVector3f( pScaling.GetAt(0), pScaling.GetAt(1), pScaling.GetAt(2) );
				vTempKeyFrame[i].qFinalRot = cQuaternion( pRotation.GetAt(3), pRotation.GetAt(0), pRotation.GetAt(1), pRotation.GetAt(2) );
				vTempKeyFrame[i].vRot = 0;
				i++;
			}
			
			cBone* pBone = apSkeleton->GetBoneByName(apNode->GetName());
			cMatrixf boneLocal = pBone->GetLocalTransform();
			cMatrixf boneLocalUnscaled = pBone->GetLocalTransformUnscaled();
			
			cQuaternion qInvBoneRot;
			qInvBoneRot.FromRotationMatrix( boneLocalUnscaled.GetRotation() );
			qInvBoneRot.v = qInvBoneRot.v * -1;
			qInvBoneRot.Normalize();
			
			//Add all of the temporary keyframes to the track. Subtract bone rest pose's translation and orientation ( the latter by premultiplying with inv bone rot )
			cAnimationTrack *pTrack = apAnimation->CreateTrack(apNode->GetName(),transFlags);
			
			for(int i=0; i< vTempKeyFrame.size(); i++)
			{
				cTempKeyFrameData *data = &vTempKeyFrame[i];
				
				cKeyFrame *pKeyFrame = pTrack->CreateKeyFrame(data->mfTime);
				pKeyFrame->trans = data->vTrans - boneLocal.GetTranslation();
				
				data->qFinalRot.Normalize();

				pKeyFrame->rotation = cMath::QuaternionMul(qInvBoneRot, data->qFinalRot );
				pKeyFrame->rotation.Normalize();
			}
		}

		for(int i=0;i<apNode->GetChildCount();i++)
		{
			LoadAnimationRec(apScene, apNode->GetChild(i),apAnimation,asAnimStackName,alDepth+1,vParentT, vParentS, vParentR, apSkeleton);
		}
	}
	
	//-----------------------------------------------------------------------

	void cMeshLoaderFBX::LoadSkeletonRec(cBone* apBone,	KFbxNode *apNode, int alDepth)
	{
		if(apNode->GetSkeleton())
		{
			apBone = LoadSkeletonData(apBone,apNode,alDepth);
		}

		for(int i=0;i<apNode->GetChildCount();i++)
		{
			LoadSkeletonRec(apBone,apNode->GetChild(i),alDepth+1);
		}
	}

	//-----------------------------------------------------------------------

	void cMeshLoaderFBX::MakeFinalBonesRec(cBone* apBone, cMatrixf a_mtxParentGlobal, cMatrixf a_mtxParentGlobalUnscaled)
	{
		//If bone is linked to geometry get the local matrix.
		if(apBone->GetValue() != 0)
		{
			cMatrixf mtxGlobal = apBone->GetLocalTransform();
			cMatrixf mtxInvParent = cMath::MatrixInverse(a_mtxParentGlobal);
			cMatrixf mtxLocal = cMath::MatrixMul(mtxInvParent,mtxGlobal);
			apBone->SetTransform(mtxLocal);
			a_mtxParentGlobal = mtxGlobal;

			// unscaled

			cMatrixf mtxGlobalUnscaled = apBone->GetLocalTransformUnscaled();
			cMatrixf mtxInvParentUnscaled = cMath::MatrixInverse(a_mtxParentGlobalUnscaled);
			cMatrixf mtxLocalUnscaled = cMath::MatrixMul(mtxInvParentUnscaled,mtxGlobalUnscaled);
			apBone->SetTransformUnscaled(mtxLocalUnscaled);
			a_mtxParentGlobalUnscaled = mtxGlobalUnscaled;

			// end special code
		}
		else
		{
			a_mtxParentGlobal = apBone->GetWorldTransform();
			a_mtxParentGlobalUnscaled = apBone->GetWorldTransformUnscaled();
		}
		
		cBoneIterator it = apBone->GetChildIterator();

		while(it.HasNext())
		{
			MakeFinalBonesRec(it.Next(),a_mtxParentGlobal, a_mtxParentGlobalUnscaled);
		}
		
	}

	//-----------------------------------------------------------------------

	//Search the scene for geometry
	void cMeshLoaderFBX::LoadSceneRec(tSubMeshDataList* apSubMeshList,cSkeleton *apSkeleton, 
										cNode3D* apHplNode,
										KFbxNode *apNode, int alDepth, bool animationOnly)
	{
		//Load the Mesh
		if(apNode->GetMesh())
		{
			LoadMeshData(apSubMeshList,apSkeleton,apHplNode,apNode,alDepth, animationOnly);
		}
		
		//Loop through children
        for(int i=0;i<apNode->GetChildCount();i++)
		{
			LoadSceneRec(apSubMeshList,apSkeleton,apHplNode,apNode->GetChild(i),alDepth+1, animationOnly);
		}
	}

	//-----------------------------------------------------------------------
	
	cBone* cMeshLoaderFBX::LoadSkeletonData(cBone* apBone,KFbxNode *apNode, int alDepth)
	{
		//////////////////////////////////////////////////////
		// Get bone properties.
		KFbxSkeleton* pSkeleton = apNode->GetSkeleton();
		
        cBone* pNewBone = apBone->CreateChildBone(apNode->GetName(), apNode->GetName());

		//Just set identity for the time being, 
		//The global matrix will be loaded when the matrix links are loaded.
		cMatrixf mtxLocal = cMatrixf::Identity;
        		
		pNewBone->SetTransform(mtxLocal);
		pNewBone->SetTransformUnscaled(mtxLocal);

		//Set value to 0, use this value later to see what bones are linked to
		//geometry.
		pNewBone->SetValue(0);
		
		if(mbLog)
		{
			//Log("%s Loading bone: '%s'\n",GetTabs(alDepth),apNode->GetName());
			//Log("%s Type: %s\n",GetTabs(alDepth),GetSkelTypeName(pSkeleton->GetSkeletonType()));
			//Log("%s Limb length: %f\n",GetTabs(alDepth),pSkeleton->GetLimbLength());
			//Log("%s NodeSize: %f\n",GetTabs(alDepth),pSkeleton->GetLimbNodeSize());
			//Log("%s Local Transform: %s\n",GetTabs(alDepth),cMath::MatrixToChar(mtxLocal));
		}
		
		return pNewBone;
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	
	void cMeshLoaderFBX::LoadMeshData(tSubMeshDataList* apSubMeshList,cSkeleton* apSkeleton, cNode3D* apHplNode, KFbxNode *apNode, int alDepth, bool animationOnly)
	{
		//Build this list with uv, pos, normal and color.
		tVertexVec mvVertexes;

		//Build this is with indicies
		tUIntVec mvIndexes;

		//Get the texture name in this
		cSubMeshData subMeshData;

		KFbxMesh *pMesh = apNode->GetMesh();
		tExtraVertricesVec vExtraVetrices;
        
        tString nodeName = apNode->GetName();
        bool isControllerNode = nodeName.substr(0,4) == "CON_";

		if ( !animationOnly && !isControllerNode )    // exclude controller objects, used only for animation, don't contain mesh data
		{
			//This lists stores values that needs an extra vertex:
			tExtraVtxValueList lstExtraValues;

			subMeshData.msName = nodeName;

			//If the mesh has been transformed to triangles or not.
			bool mbTriangulated=false;
		
			if(mbLog)Log("%s Loading node: '%s'\n",GetTabs(alDepth),apNode->GetName());

			//triangulate the mesh
		
			KFbxGeometryConverter pConverter( mpSdkManager );
			//pConverter->TriangulateInPlace(apNode);

		

			KFbxMesh *pTriMesh = pMesh;
			if(mbTriangulated==false)pTriMesh = pConverter.TriangulateMesh(pMesh);
	
			//mpSdkManager->DestroyKFbxGeometryConverter(pConverter);

			KFbxVector4 *pPositions = pMesh->GetControlPoints();
		
			//////////////////////////////////////////////////////
			//Polygons (indicies)
			if(mbLowLog)Log("%s Polygons:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
			for(int i=0;i<pTriMesh->GetPolygonCount();i++)
			{
				if(mbLowLog)Log("[");
				for(int j=0;j<pTriMesh->GetPolygonSize(i);j++)
				{
					if(mbLowLog)Log("%d", pTriMesh->GetPolygonVertex(i,j));
					if(mbLowLog)if(j != pTriMesh->GetPolygonSize(i)-1)Log(", ");

					mvIndexes.push_back(pTriMesh->GetPolygonVertex(i,j));
				}
				if(mbLowLog)Log("]");
			}
			if(mbLowLog)Log("\n");

			//////////////////////////////////////////////////////	
			//Positions (vertrices)
			if(mbLowLog)Log("%s Positions:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
			for(int i=0;i<pMesh->GetControlPointsCount();i++)
			{
				if(mbLowLog)Log("(%.1f, %.1f, %.1f) ", pPositions[i].GetAt(0),pPositions[i].GetAt(1),pPositions[i].GetAt(2));
				if(mbLowLog)if(i != pMesh->GetControlPointsCount()-1)Log(", ");

				cVector3f vPos((float)pPositions[i].GetAt(0),(float)pPositions[i].GetAt(1),(float)pPositions[i].GetAt(2));
			
				//The negative z is because it shows when the vertex has gotten a uv coord.
				//And then extra uvs can be found.
				mvVertexes.push_back(cVertex(vPos,cVector3f(0,0,-1),cColor(1,1)));
			}
			if(mbLowLog)Log("\n");

			KFbxLayer *pLayer = pTriMesh->GetLayer(0);

			//////////////////////////////////////////////////////
			//Normals
			KFbxLayerElementNormal *pNormLayer = pLayer->GetNormals();
			
			if(pNormLayer)
			{
				if(mbLog)Log("%s Normal mapping mode: %d \n",GetTabs(alDepth), pNormLayer->GetMappingMode());
				if(mbLog)Log("%s Normal reference mode: %d \n",GetTabs(alDepth), pNormLayer->GetReferenceMode());

				if(pNormLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_CONTROL_POINT)
				{
					KFbxLayerElementNormal::DirectArrayType pNormals = pNormLayer->GetDirectArray();

					if(mbLowLog)Log("%s Normals:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
					for(int i=0;i<pMesh->GetControlPointsCount();i++)
					{
						if(mbLowLog)Log("(%.1f, %.1f, %.1f) ", pNormals[i].GetAt(0),pNormals[i].GetAt(1),pNormals[i].GetAt(2));
						if(mbLowLog)if(i != pMesh->GetControlPointsCount()-1)Log(", ");

						cVector3f vPos((float)pNormals[i].GetAt(0),(float)pNormals[i].GetAt(1),(float)pNormals[i].GetAt(2));

						mvVertexes[i].norm = vPos;
					}

					if(mbLowLog)Log("\n");
				}
				else if(pNormLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_POLYGON_VERTEX)
				{
					if ( pNormLayer->GetReferenceMode() == KFbxLayerElement::eDIRECT )
					{
						KFbxLayerElementNormal::DirectArrayType pNormals = pNormLayer->GetDirectArray();

						if(mbLowLog)Log("%s Normals:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
						for ( int i = 0; i < (int)mvIndexes.size(); i++)
						{
							cVector3f vPos((float)pNormals[i].GetAt(0),(float)pNormals[i].GetAt(1),(float)pNormals[i].GetAt(2));
							mvVertexes[mvIndexes[i]].norm = vPos;
						}
					}
					else
					{
						Error("Normal mapping mode is not correct!\n");
						return;
					}
				}
			}

			//////////////////////////////////////////////////////
			//Tangents
			KFbxLayerElementTangent *pTangentLayer = pLayer->GetTangents();
			
			if(pTangentLayer)
			{
				if(mbLog)Log("%s Tangent mapping mode: %d \n",GetTabs(alDepth), pTangentLayer->GetMappingMode());
				if(mbLog)Log("%s Tangent reference mode: %d \n",GetTabs(alDepth), pTangentLayer->GetReferenceMode());

				KFbxLayerElementTangent::DirectArrayType pTangents = pTangentLayer->GetDirectArray();
				KFbxLayerElementArrayTemplate<int> pTangentIndices = pTangentLayer->GetIndexArray();

				if(pTangentLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_CONTROL_POINT)
				{
					if(mbLowLog)Log("%s Tangents:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
					for(int i=0;i<pMesh->GetControlPointsCount();i++)
					{
						if(mbLowLog)Log("(%.1f, %.1f, %.1f) ", pTangents[i].GetAt(0),pTangents[i].GetAt(1),pTangents[i].GetAt(2));
						if(mbLowLog)if(i != pMesh->GetControlPointsCount()-1)Log(", ");

						cVector3f vPos((float)pTangents[i].GetAt(0),(float)pTangents[i].GetAt(1),(float)pTangents[i].GetAt(2));

						mvVertexes[i].tan = vPos;
					}

					if(mbLowLog)Log("\n");
				}
				else if(pTangentLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_POLYGON_VERTEX)
				{
					/*for(int i=0;i<pMesh->GetControlPointsCount();i++)
					{
						if(mbLowLog)Log("(%.1f, %.1f, %.1f) ", pTangents[i].GetAt(0),pTangents[i].GetAt(1),pTangents[i].GetAt(2));
						if(mbLowLog)if(i != pMesh->GetControlPointsCount()-1)Log(", ");

						cVector3f vPos((float)pTangents[i].GetAt(0),(float)pTangents[i].GetAt(1),(float)pTangents[i].GetAt(2));

						mvVertexes[i].tan = vPos;
					}
					*/
					/*
					int polyCount = pMesh->GetPolygonCount();

					for ( int polyIndex = 0; polyIndex < polyCount; polyIndex++ )
					{
						int vertexCount = pMesh->GetPolygonSize(polyIndex);
						int polygonVertexIndex = pMesh->GetPolygonVertexIndex( polyIndex );

						for ( int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++ )
						{
							int positionIndex = polygonVertexIndex + vertexIndex;

							
							cVector3f vPos((float)pTangents[positionIndex].GetAt(0),(float)pTangents[positionIndex].GetAt(1),(float)pTangents[positionIndex].GetAt(2));
							mvVertexes[positionIndex].tan = vPos;



							switch (pTangentLayer->GetReferenceMode()) {
								case KFbxLayerElement::eDIRECT:
									for (int p = 0; p < 3; ++p)
										vert.tangent[p] = (float)tangents->GetDirectArray().GetAt( positionIndex )[p];
									break;
								case KFbxLayerElement::eINDEX_TO_DIRECT:
									{
										int index = tangents->GetIndexArray().GetAt( positionIndex );
										for (int p = 0; p < 3; ++p)
											vert.tangent[p] = (float)tangents->GetDirectArray().GetAt( index )[p];
									}
									break;
						}
					}*/

					if ( pTangentLayer->GetReferenceMode() == KFbxLayerElement::eDIRECT )
					{					
						if(mbLowLog)Log("%s Tangents:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
						for ( int i = 0; i < (int)mvIndexes.size(); i++)
						{
							cVector3f vPos((float)pTangents[i].GetAt(0),(float)pTangents[i].GetAt(1),(float)pTangents[i].GetAt(2));
							mvVertexes[mvIndexes[i]].tan = vPos;
						}
					}
					else
					{
						Error("Tangent mapping mode is not correct!\n");
						return;
					}
					
					/*for(int i=0;i<(int)mvIndexes.size();i++)
					{
						int VtxPos = mvIndexes[i];
						int lPos = pTangentIndices[VtxPos];
						cVector3f vPos((float)pTangents[lPos].GetAt(0),(float)pTangents[lPos].GetAt(1),(float)pTangents[lPos].GetAt(2));
						
						
						mvVertexes[VtxPos].tan = vPos;

						if(mbLowLog)if(i != mvIndexes.size()-1)Log(", ");
					}*/
					if(mbLowLog)Log("\n");
				}
			}

			//////////////////////////////////////////////////////
			//UV coords.
			KFbxLayerElementUV *pUvLayer = pLayer->GetUVs();

			if(pUvLayer)
			{

				KFbxLayerElementUV::DirectArrayType pUvs = pUvLayer->GetDirectArray();
				KFbxLayerElementArrayTemplate<int> pUvIndices = pUvLayer->GetIndexArray();

				if(mbLog)Log("%s UV mapping mode: %d \n",GetTabs(alDepth), pUvLayer->GetMappingMode());
				if(mbLog)Log("%s UV reference mode: %d \n",GetTabs(alDepth), pUvLayer->GetReferenceMode());

				//One UV for each position
				if(pUvLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_CONTROL_POINT)
				{
					if(mbLowLog)Log("%s Uvs:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
					for(int i=0;i<pMesh->GetControlPointsCount();i++)
					{
						if(mbLowLog)Log("(%.1f, %.1f) ", pUvs[i].GetAt(0),pUvs[i].GetAt(1));
						if(mbLowLog)if(i != pMesh->GetControlPointsCount()-1)Log(", ");

						//Invert y sicne it uses a another coord system
						cVector3f vPos(	(float)pUvs[i].GetAt(0),
							1.0f -(float)pUvs[i].GetAt(1),0);

						mvVertexes[i].tex = vPos;
					}
					if(mbLog)Log("\n");
				}
				//One UV for each vertex, find what vertex the index belongs to.
				else if(pUvLayer->GetMappingMode() == KFbxLayerElementNormal::eBY_POLYGON_VERTEX)
				{
					if(mbLowLog)Log("%s Uvs:\n %s ",GetTabs(alDepth),GetTabs(alDepth));
					for(int i=0;i<(int)mvIndexes.size();i++)
					{
						int lPos = pUvIndices[i];
						if(mbLowLog)Log("(%.1f, %.1f) ", pUvs[lPos].GetAt(0),pUvs[lPos].GetAt(1));

						//Invert y sicne it uses a another coord system
						cVector3f vPos(	(float)pUvs[lPos].GetAt(0),
							1.0f -(float)pUvs[lPos].GetAt(1),0);

						int VtxPos = mvIndexes[i];

						//check if there is allready a uv coords at this vertex,
						//if so a new has to be made.
						if(mvVertexes[VtxPos].tex.z != -1 && mvVertexes[VtxPos].tex != vPos)
						{
							lstExtraValues.push_back(cExtraVtxValue(i,vPos));
						}
						else
						{
							mvVertexes[VtxPos].tex = vPos;
						}

						if(mbLowLog)if(i != mvIndexes.size()-1)Log(", ");
					}
					if(mbLowLog)Log("\n");
				}
				else{
					Error("UV mapping mode is not correct! No uv coords will be created.\n");
					pUvLayer = NULL;
				}
			}

			//////////////////////////////////////////////////////
			//Mesh processing

			//Create a vector for extra vertrcies added.
			vExtraVetrices.resize(mvVertexes.size());
		
			//OInly useful to add extra if there are uv's
			if(pUvLayer)
			{
				//Add extra vertexes if needed
				if(mbLowLog)Log("%s Adding new vertexes to indices: ",GetTabs(alDepth));
				tExtraVtxValueListIt it = lstExtraValues.begin();
				int lStartPos = (int)mvVertexes.size();
				for(;it != lstExtraValues.end();it++)
				{
					cExtraVtxValue &val = *it;
					cVertex newVtx;

					if(mbLowLog)Log("%d ",val.mlIndexNum);	

					//Check if there is allready a point added with the same values.
					bool bOldFound = false;
					for(int i=lStartPos; i<(int)mvVertexes.size();i++)
					{
						if(mvVertexes[i].tex == val.mvVal && 
							mvVertexes[i].pos == mvVertexes[mvIndexes[val.mlIndexNum]].pos &&
							mvVertexes[i].norm == mvVertexes[mvIndexes[val.mlIndexNum]].norm &&
							mvVertexes[i].tan == mvVertexes[mvIndexes[val.mlIndexNum]].tan)
						{
							mvIndexes[val.mlIndexNum] = i;
							bOldFound = true;
							break;
						}
					}

					if(bOldFound)continue;

					unsigned int lVtxNum = mvIndexes[val.mlIndexNum];
				
					//Add the added to a list, so it is later to check up what points
					//Have been splitted.
					vExtraVetrices[lVtxNum].mvNewPoints.push_back((int)mvVertexes.size());

					newVtx.tex = val.mvVal;
					newVtx.pos = mvVertexes[lVtxNum].pos;
					newVtx.norm = mvVertexes[lVtxNum].norm;
					newVtx.tan = mvVertexes[lVtxNum].tan;

					mvVertexes.push_back(newVtx);

					mvIndexes[val.mlIndexNum] = (unsigned int)mvVertexes.size()-1;
				}
				if(mbLowLog)Log("\n");
			}

			//////////////////////////////////////////////////////
			//Material
			KFbxLayerElementMaterial *pMaterialLayer = pLayer->GetMaterials();

			if(pMaterialLayer)
			{
				if(pMaterialLayer->GetMappingMode() != KFbxLayerElementMaterial::eALL_SAME)
				{
					Error("Per polygon material not supported!\n");
					return;
				}

				KFbxLayerElementMaterial::DirectArrayType pMatArray = pMaterialLayer->ParentClass::GetDirectArray();
				KFbxSurfaceMaterial *pMat = pMatArray.GetAt(0);

				if(pMat)
				{
					// first set the material based on the material name
					subMeshData.msMaterial = pMat->GetName();
					//lProperty = pMat->FindProperty(KFbxSurfaceMaterial::sDiffuse);

					if (pMat->GetClassId() == KFbxSurfacePhong::ClassId )
					{
						fbxDouble3 col = ((KFbxSurfacePhong *)pMat)->Diffuse.Get();
						fbxDouble1 factor = ((KFbxSurfacePhong *)pMat)->DiffuseFactor.Get();
						for(int i=0;i<(int)mvVertexes.size();i++)
						{
							mvVertexes[i].col = cColor((float)col[0],(float)col[1], 
								(float)col[2], (float)factor);
						}
					}
				}
				else
				{
					Error("No material found!\n");
				}
			}
	
			//////////////////////////////////////////////////////
			//Texture

			KFbxLayerElementTexture *pTextureLayer = pLayer->GetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES);
		
			if(pUvLayer)
			{
				if(pTextureLayer)
				{
					KFbxTexture *pTexture = pTextureLayer->GetDirectArray().GetAt(0);
					if(pTexture)
					{
						subMeshData.msMaterial = cString::SetFileExt(cString::GetFileName(pTexture->GetName()),"");  // replaced GetFileName by GetName, maybe GetUrl or something else?

						if(mbLog){
							Log("%s Swap UV: %d\n",GetTabs(alDepth),pTexture->GetSwapUV()?1:0);
							Log("%s Material name: %s\n",GetTabs(alDepth),subMeshData.msMaterial.c_str());
						}
					}
					else
					{
						Error("No texture found!\n");
						pTextureLayer = NULL;

						subMeshData.msMaterial ="";
					}
				}
			}
			else
			{
				pTextureLayer = NULL;
			}


			//////////////////////////////////////////////////////
			//Matrix

			//What is the difference between source and destination?
			//Destination gives the right matrix here..hmmm...
			KFbxMatrix GMtx = apNode->EvaluateGlobalTransform( KTIME_INFINITE, KFbxNode::eSOURCE_SET, false);
			//Something else here using rotation and scaling?
			KFbxMatrix LMtx = apNode->EvaluateLocalTransform(KTIME_INFINITE, KFbxNode::eSOURCE_SET, false);    // again, why no local transform here? todo!

			subMeshData.m_mtxGlobal = cMatrixf(&GMtx.Transpose().mData[0][0]);
			subMeshData.m_mtxLocal = cMatrixf(&LMtx.Transpose().mData[0][0]);

			if(mbLog)Log("%s Matrix: %s\n",GetTabs(alDepth),cMath::MatrixToChar(subMeshData.m_mtxGlobal));


			//Transform the vertices according to the matrix
			cMatrixf mtxNormTrans = subMeshData.m_mtxLocal;
			mtxNormTrans.m[0][3] =0;
			mtxNormTrans.m[1][3] =0;
			mtxNormTrans.m[2][3] =0;

			for(int i=0; i < (int)mvVertexes.size(); i++)
			{
				mvVertexes[i].pos = cMath::MatrixMul(subMeshData.m_mtxGlobal, mvVertexes[i].pos);
				mvVertexes[i].norm = cMath::MatrixMul(mtxNormTrans, mvVertexes[i].norm);
				mvVertexes[i].norm.Normalize();
				mvVertexes[i].tan = cMath::MatrixMul(mtxNormTrans, mvVertexes[i].tan);
				mvVertexes[i].tan.Normalize();
			}

			//Invert the positions of the indices
			for(int i=0; i< (int)mvIndexes.size();i+=3)
			{
				unsigned int lTemp = mvIndexes[i+0];
				mvIndexes[i+0] = mvIndexes[i+2];
				mvIndexes[i+2] = lTemp;
			}
		}

		///////////////////////////////////////////////////////////
		//Get Links and Skin the bones.
		if(apSkeleton)
		{
			int deformerCount = pMesh->GetDeformerCount( KFbxDeformer::eSKIN );
			if(deformerCount  != 1 ) Error("No or multiple skin deformers assigned to mesh!\n");

			for(int i=0;i<deformerCount;i++)
			{
				KFbxSkin * skin = (KFbxSkin *)pMesh->GetDeformer(i, KFbxDeformer::eSKIN);

				int clusterCount = skin->GetClusterCount();

				for ( int j = 0; j < clusterCount; j++ )
				{
					KFbxLink* pLink = skin->GetCluster(j);

					KFbxNode* pBoneNode = pLink->GetLink();

					KFbxNode* pAssNode = pLink->GetAssociateModel();
					tString sAssName ="None";
					if(pAssNode)sAssName = pAssNode->GetName();

					if(mbLog)Log("%s Link: %d,LinkNode: %s, Mode: %s AssModel: %s\n",GetTabs(alDepth), i,
													pBoneNode->GetName(),
													GetLinkModeName(pLink->GetLinkMode()),
													sAssName.c_str());
				
					//////////////////////
					//Get the bone matrix:
					cBone* pBone = apSkeleton->GetBoneByName(pBoneNode->GetName());

					KFbxXMatrix mtxTemp;
					//////////////////////
					//Get transform of the linking node (bone)
				
					KFbxVector4 vTrans, vRot, vScale; 

					KFbxXMatrix transformLinkMatrix, lClusterGeometry;
					pLink->GetTransformLinkMatrix( transformLinkMatrix );

					// Multiply transformLinkMatrix by Geometric Transformation
					lClusterGeometry = GetGeometry(pLink->GetLink());
					transformLinkMatrix *= lClusterGeometry;

					vTrans = transformLinkMatrix.GetT();
					vScale = transformLinkMatrix.GetS();
					vRot = transformLinkMatrix.GetR();
					
					cVector3f vT(vTrans.GetAt(0),vTrans.GetAt(1),vTrans.GetAt(2));
					cVector3f vS(vScale.GetAt(0),vScale.GetAt(1),vScale.GetAt(2));
					cVector3f vR(vRot.GetAt(0),vRot.GetAt(1),vRot.GetAt(2));
				
					cMatrixf mtxS = cMath::MatrixScale(vS);
					cMatrixf mtxT = cMath::MatrixTranslate(vT);
					cMatrixf mtxR = MatrixFromEulerAngles( vR );


					// WARNING OR SOMETHING 
					// The rotaions are alittle crazy but so far it is working
					// If something seems screwed up here is a good place to debug.
					cMatrixf mtxBone =  cMath::MatrixMul(mtxR,mtxS);
					mtxBone = cMath::MatrixMul(mtxT,mtxBone);

					if(mbLog)Log("%s LTransform: %s\n",GetTabs(alDepth),cMath::MatrixToChar(mtxBone));
				
					//Sets the global position at rest for the bone!
					pBone->SetTransform(mtxBone);
					pBone->SetTransformUnscaled(mtxR);

					//Set value to 1 to show that the bone has a link connection.
					pBone->SetValue(1);
				
					//////////////////////
					//Get transform of the node containing link.
					//pLink->GetTransformMatrix(mtxTemp);
					//cMatrixf mtxTrans(& mtxTemp.Transpose().mData[0][0]);
					//if(mbLog)Log("%s NTransform: %s\n",GetTabs(alDepth),cMath::MatrixToChar(mtxTrans));

					if ( !animationOnly )
					{
						/////////////////////
						/// Get the data for the link
						double *pWeights = pLink->GetControlPointWeights();
						int *pVtxIndices = pLink->GetControlPointIndices();
				
						/////////////////////
						// Set up a default bone-vtx pair
						cVertexBonePair vtxBonePair;
						vtxBonePair.boneIdx = apSkeleton->GetBoneIndexByName(pBoneNode->GetName());

						if(mbLowLog)Log("%s Adding links to bone: '%s' idx: %d\n",GetTabs(alDepth),
										pBoneNode->GetName(), vtxBonePair.boneIdx);

						for(int j=0;j<pLink->GetControlPointIndicesCount();j++)
						{
							vtxBonePair.weight = (float)pWeights[j];
							vtxBonePair.vtxIdx = pVtxIndices[j];
							subMeshData.mvVtxBonePairs.push_back(vtxBonePair);

							//Add the new vertices that was made if the vertex was split.
							for(int k=0;k< (int)vExtraVetrices[pVtxIndices[j]].mvNewPoints.size(); k++)
							{
								vtxBonePair.vtxIdx = vExtraVetrices[pVtxIndices[j]].mvNewPoints[k];
							}
						}
					}
				}
			}
		}

		if ( !animationOnly && !isControllerNode )
		{
			///////////////////////////////////////////////////////////
			//Create VertexBuffer
			tVertexCompileFlag compileFlags = 0;
		
			eVertexBufferUsageType usageType = eVertexBufferUsageType_Static;
			//Do some test to see if the mesh should be dynamic

			subMeshData.mpVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware,eVertexBufferDrawType_Tri,
				usageType,
				(int)mvVertexes.size(), (int)mvIndexes.size());

			subMeshData.mpVtxBuffer->CreateElementArray( eVertexBufferElement_Position,eVertexBufferElementFormat_Float, 4);
			subMeshData.mpVtxBuffer->CreateElementArray( eVertexBufferElement_Normal,eVertexBufferElementFormat_Float, 3);
			//if(pTextureLayer)
			//{
				subMeshData.mpVtxBuffer->CreateElementArray( eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float, 3);
			//}
			subMeshData.mpVtxBuffer->CreateElementArray( eVertexBufferElement_Color0,eVertexBufferElementFormat_Float, 4);
			subMeshData.mpVtxBuffer->CreateElementArray( eVertexBufferElement_Texture1Tangent,eVertexBufferElementFormat_Float, 4);


			//Add the vertices
			for(int i=0;i<(int)mvVertexes.size();i++)
			{
				subMeshData.mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, mvVertexes[i].pos);
				subMeshData.mpVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, mvVertexes[i].col);
				subMeshData.mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, mvVertexes[i].norm);

				//if(pTextureLayer)
				//{
					subMeshData.mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, mvVertexes[i].tex);
					//This is useless right??
					subMeshData.mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture1Tangent, mvVertexes[i].tan);
				//}
			}

			//Add the indices
			for(int i=0;i<(int)mvIndexes.size();i++)
			{
				subMeshData.mpVtxBuffer->AddIndex(mvIndexes[i]);
			}

			//Compile the vertex buffer
			if(mbLog)Log("\n%s Compiling mesh\n",GetTabs(alDepth));
			subMeshData.mpVtxBuffer->Compile(compileFlags);

			//Add the sub mesh data to the list
			if(apSubMeshList) apSubMeshList->push_back(subMeshData);
		}

		//end of mesh
		if(mbLog)Log("\n");
	}
	
	//-----------------------------------------------------------------------
	
	const char* cMeshLoaderFBX::GetTabs(int alDepth)
	{
		msTemp = "";

		for(int i=0;i<alDepth;i++)msTemp+="\t";

		return msTemp.c_str();
	}

	const char* cMeshLoaderFBX::GetAttrName(KFbxNodeAttribute::EAttributeType alNum)
	{
		switch(alNum)
		{
		case KFbxNodeAttribute::eUNIDENTIFIED: return "Unidentified";
		case KFbxNodeAttribute::eNULL: return "Null";
		case KFbxNodeAttribute::eMARKER: return "Marker";
		case KFbxNodeAttribute::eSKELETON: return "Skeleton";
		case KFbxNodeAttribute::eMESH: return "Mesh"; 
		case KFbxNodeAttribute::eNURB: return "Nurb"; 
		case KFbxNodeAttribute::ePATCH: return "Patch"; 
		case KFbxNodeAttribute::eCAMERA: return "Camera"; 
		case KFbxNodeAttribute::eCAMERA_SWITCHER: return "CameraSwicther";
		case KFbxNodeAttribute::eLIGHT: return "Light";
		case KFbxNodeAttribute::eOPTICAL_REFERENCE: return "Reference";
		case KFbxNodeAttribute::eOPTICAL_MARKER: return "Marker";
		}

		return "Uknown";
	}

	const char*  cMeshLoaderFBX::GetSkelTypeName(KFbxSkeleton::ESkeletonType alNum)
	{
		switch(alNum)
		{
		case KFbxSkeleton::eROOT: return "Root";
		case KFbxSkeleton::eLIMB: return "Limb";
		case KFbxSkeleton::eLIMB_NODE:  return "Limb Node";
		case KFbxSkeleton::eEFFECTOR:  return "Effector (root)";
		}

		return "Unknown";
	}

	const char* cMeshLoaderFBX::GetRotOrderName(ERotationOrder alNum)
	{
		switch(alNum)
		{
		case eEULER_XYZ: return "X Y Z";
		case eEULER_XZY: return "X Z Y";
		case eEULER_YZX: return "Y Z X";
		case eEULER_YXZ: return "Y X Z";
		case eEULER_ZXY: return "Z X Y";
		case eEULER_ZYX:return "Z Y X";
		case eSPHERIC_XYZ: return "SphereXYZ";
		}
		return "Unknown";
	}

	const char* cMeshLoaderFBX::GetLinkModeName(KFbxLink::ELinkMode alNum)
	{
		switch(alNum)
		{
		case KFbxLink::eNORMALIZE: return "Normalize";
		case KFbxLink::eADDITIVE: return "Additive";
		case KFbxLink::eTOTAL1:  return "TotalOne";
		}
		return "Unknown";
	}

	//-----------------------------------------------------------------------

	bool cMeshLoaderFBX::LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		//int lFileFormat = -1;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( !lImportStatus )
		{
			printf("Call to KFbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

			if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
			{
				printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

		if (lImporter->IsFBX())
		{
			printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.

			printf("Animation Stack Information\n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
			printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			printf("\n");

			for(i = 0; i < lAnimStackCount; i++)
			{
				KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				printf("    Animation Stack %d\n", i);
				printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
				printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

				// Set the value of the import state to false if the animation stack should be not
				// be imported. 
				printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
				printf("\n");
			}

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
			IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
			IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
			IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
			IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
			IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
			IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			printf("Please enter password: ");

			lPassword[0] = '\0';

			scanf("%s", lPassword);
			KString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(pScene);

			if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
			{
				printf("\nPassword is wrong, import aborted.\n");
			}
		}

		// Destroy the importer.
		lImporter->Destroy();

		return lStatus;
	}
}
