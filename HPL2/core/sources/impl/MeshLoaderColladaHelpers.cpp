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
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "graphics/Material.h"

#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "impl/tinyXML/tinyxml.h"

#include "math/Math.h"

namespace hpl {

#define GetAdress(sStr) if(sStr.length()>0 && sStr[0]=='#') sStr = cString::Sub(sStr,1);
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//This fucntions checks if y is up and if not switches palces with z and y.
	cVector3f cMeshLoaderCollada::GetVectorPos(const cVector3f &avVec)
	{
		if(mbZToY)
		{
			return cVector3f(-avVec.x, avVec.z, avVec.y);
		}
		else
		{
			return avVec;
		}
	}

	cVector3f cMeshLoaderCollada::GetVectorNormal(const cVector3f &avVec)
	{
		if(mbZToY)
		{
			return cVector3f(-avVec.x, avVec.z, avVec.y);
		}
		else
		{
			return avVec;
		}
	}

	cMatrixf cMeshLoaderCollada::GetMatrix(const cMatrixf &amtxA)
	{
		return amtxA;
	}

	cVector3f cMeshLoaderCollada::GetVectorPosFromPtr(float *apVec)
	{
		if(mbZToY)
		{
			return cVector3f(apVec[0],apVec[2],apVec[1]);
		}
		else
		{
			return cVector3f(apVec[0],apVec[1],apVec[2]);
		}
	}

	cVector3f cMeshLoaderCollada::GetVectorRotationFromPtr(float *apVec)
	{
		return cVector3f(apVec[0],apVec[1],apVec[2]);
	}

	cVector3f cMeshLoaderCollada::GetVectorScaleFromPtr(float *apVec)
	{
		if(mbZToY)
		{
			return cVector3f(apVec[0],apVec[2],apVec[1]);
		}
		else
		{
			return cVector3f(apVec[0],apVec[1],apVec[2]);
		}
	}

	//-----------------------------------------------------------------------

	class cTempAnimData
	{
	public:
		cTempAnimData(): mvTrans(0,0,0), mvRot(0,0,0), mvScale(1,1,1) {}

		cVector3f mvTrans;
		cVector3f mvRot;
		cVector3f mvScale;
		float mfTime;

		int mlIndex;
	};

	typedef std::vector<cTempAnimData> tTempAnimDataVec;

	typedef std::set<float> tTempTimesSet;
	typedef std::set<float>::iterator tTempTimesSetIt;

	static cTempAnimData* GetTempAnimData(float afTime, tTempAnimDataVec &avTempData)
	{
		for(size_t i=0; i<avTempData.size(); ++i)
		{
			if(avTempData[i].mfTime == afTime) return &avTempData[i];
		}

		return NULL;
	}

	/////////////////
	
	//Get times in TimeVec closes to afTime.
	static void GetAnimTimes(float afTime,float *apTimeBefore,float *apTimeAfter,tFloatVec* apTimeVec)
	{
		*apTimeBefore = -1; *apTimeAfter=-1;
		for(size_t i=0; i<apTimeVec->size(); ++i)
		{
			float fTime = (*apTimeVec)[i];
			
			if(fTime <= afTime)
			{
				*apTimeBefore = fTime;
			}
			else if(fTime >= afTime)
			{
				*apTimeAfter = fTime;
			}
		}
	}

	/////////////////

	cAnimationTrack* cMeshLoaderCollada::CreateAnimTrack(cAnimation *apAnimation , cSkeleton *apSkeleton,
														 cColladaAnimation &aAnim, cColladaScene *apScene)	
	{
		tTempAnimDataVec vTempData;
		tTempTimesSet setTempTimes;

		//Log("Animation: %s Target: %s\n",aAnim.msId.c_str(), aAnim.msTargetNode.c_str());

		//Get the node that will be animated by this track
		cColladaNode* pNode = apScene->GetNode(aAnim.msTargetNode);
		if(pNode==NULL)
		{
			Error("Couldn't find node '%s' for animation id '%s'\n",aAnim.msTargetNode.c_str(), aAnim.msId.c_str());
			return NULL;
		}

		cBone* pBone = NULL;
		
		if(apSkeleton)
		{
			pBone = apSkeleton->GetBoneByName(aAnim.msTargetNode);
			if(pBone == NULL)
			{
				Error("Couldn't find bone '%s'\n",aAnim.msTargetNode.c_str());
				return NULL;
			}
		}
		else
		{

		}
		
		/////////////////////////////////////////////////
		//Go through all sample and add the different times.
		for(size_t i=0; i< aAnim.mvSamplers.size(); i++)
		{
			cColladaSampler &Sampler = aAnim.mvSamplers[i];

			tFloatVec *pValueVec = aAnim.GetSourceVec(Sampler.msTimeArray);
			if(pValueVec == NULL){
				Error("Time array not found in sampler '%s'!\n",Sampler.msId.c_str());
				return NULL;
			}
			
			//Go through all and add to set.
			for(tFloatVecIt it = pValueVec->begin(); it != pValueVec->end(); ++it)
			{
				setTempTimes.insert(*it);			
			}
		}

		///////////////////////////////
		//Resize temp data and fill with times used.
		vTempData.resize(setTempTimes.size());
		int lCount=0;
		for(tTempTimesSetIt it= setTempTimes.begin(); it != setTempTimes.end(); ++it,++lCount)
		{
			vTempData[lCount].mfTime = *it;	
			vTempData[lCount].mlIndex = lCount;	
		}

		bool bLoadedTranslation=false;
		bool bLoadedRotation=false;
				
		/////////////////////////////////////////////////
		//Go through all samples and get data
		for(size_t i=0; i< aAnim.mvSamplers.size(); i++)
		{
			cColladaSampler &Sampler = aAnim.mvSamplers[i];

			//Log("Sampler %s\n",Sampler.msId.c_str());
			
			//////////////////////////
			//Get the times
			tFloatVec *pTimeVec = aAnim.GetSourceVec(Sampler.msTimeArray);
			if(pTimeVec == NULL){
				Error("Time array not found!\n");
				return NULL;
			}
			
			//Get the Sid of the transformation that this sampler changes.
			tString sTarget = cString::SetFileExt(cString::GetFileName(Sampler.msTarget),"");
			tString sExt = cString::ToLowerCase(cString::GetFileExt(Sampler.msTarget));

			////////////////////
			//Get the transformation and add the values accordingly
			cColladaTransform *pTrans = pNode->GetTransform(sTarget);
			if(pTrans == NULL){
				//Error("Transform '%s' not found!\n",sTarget.c_str());
				continue;
			}

			///////////////////////////
			//Get the values for the transformation
			tFloatVec *pValueVec = aAnim.GetSourceVec(Sampler.msValueArray);
			if(pValueVec == NULL){
				Error("Value array not found!\n");
				return NULL;
			}
			
			////////////////////////////////
            //Set the translation in this sampler
			if(pTrans->msType == "translate")
			{
				//Log("Loading Translation\n");
				bLoadedTranslation = true;

				for(size_t j=0; j <pTimeVec->size(); j++)
				{	
					//Check the translation type
					//If only a single axis is changes the others are set to
					//the node translation. THIS MIGHT BE WRONG!
					//TODO: Maybe the bind matrix should be here:
					cVector3f vTrans = pNode->m_mtxTransform.GetTranslation();

					//Get the temp data:
					cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
					if(pTempData==NULL){ Error("Code error at %d\n",__LINE__); return NULL;}

					// XYZ
					if(sExt=="")
					{
						pTempData->mvTrans = cVector3f((*pValueVec)[j*3 + 0],
							(*pValueVec)[j*3 +1],
							(*pValueVec)[j*3 +2]);
					}
					// X
					else if(sExt=="x")
					{
						pTempData->mvTrans = cVector3f((*pValueVec)[j],vTrans.y,vTrans.z);
					}
					// Y
					else if(sExt=="y")
					{
						pTempData->mvTrans = cVector3f(vTrans.x,(*pValueVec)[j],vTrans.z);
					}
					// Z
					else if(sExt=="z")
					{
						pTempData->mvTrans = cVector3f(vTrans.x,vTrans.y,(*pValueVec)[j]);
					}

					//Scale and rotate data
					pTempData->mvTrans = (pTempData->mvTrans);

					//What we wanna use is the relative movement of the bone.
					//The exported translation sets new local translation!.
					if(pBone)
						pTempData->mvTrans -= pBone->GetLocalTransform().GetTranslation();
				}
				
				////////////////////////////////
				//Go through all of Temp data and find times not added
				float fTimeBefore=-1, fTimeAfter=-1;
				for(size_t j=0; j<vTempData.size(); ++j)
				{
					GetAnimTimes(vTempData[j].mfTime,&fTimeBefore, &fTimeAfter, pTimeVec);
					
					//Time exists
					if(fTimeBefore == vTempData[j].mfTime){
						//Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
						continue;
					}
					//Time between two keys
					else if(fTimeBefore>=0 && fTimeAfter>=0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);
						
						float fT = (vTempData[j].mfTime - pBefore->mfTime)/(pAfter->mfTime -  pBefore->mfTime);

						vTempData[j].mvTrans = pBefore->mvTrans*(1.0f-fT) + pAfter->mvTrans*fT;

						//Log("Interpolated sample %s time %f between %f and %f. T=%f\n",
						//						Sampler.msId.c_str(), vTempData[j].mfTime,
						//						pBefore->mfTime,pAfter->mfTime,fT);
					}
					//Time before start
					else if(fTimeBefore<0 && fTimeAfter>=0)
					{
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);

						vTempData[j].mvTrans = pAfter->mvTrans;

						//Log("Added before at sample %s time %f using %f\n",
						//					Sampler.msId.c_str(), vTempData[j].mfTime,
						//					pAfter->mfTime);
					}
					//Time after end
					else if(fTimeBefore>=0 && fTimeAfter<0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);
						
						vTempData[j].mvTrans = pBefore->mvTrans;

						//Log("Added after at sample %s time %f using %f\n",
						//						Sampler.msId.c_str(), vTempData[j].mfTime,
						//						pBefore->mfTime);
					}
				}
			}
			//////////////////////////////////
			//Add the rotation in this sampler
			else if(pTrans->msType == "rotate")
			{
				bLoadedRotation = true;

				for(size_t j=0; j <pTimeVec->size(); j++)
				{	
					//Get the temp data:
					cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
					if(pTempData==NULL){ Error("Code error at %d\n",__LINE__); return NULL;}

					pTempData->mvRot += (cVector3f(pTrans->mvValues[0],
																  pTrans->mvValues[1],
																  pTrans->mvValues[2]) * (*pValueVec)[j]);
				}
				
				size_t lVecNum=0;
				if(pTrans->mvValues[0]>0.001f)  lVecNum =0;
				else if(pTrans->mvValues[1]>0.001f)  lVecNum =1;
				else if(pTrans->mvValues[2]>0.001f)  lVecNum =2;

				////////////////////////////////
				//Go through all of Temp data and find times not added
				float fTimeBefore=-1, fTimeAfter=-1;
				for(size_t j=0; j<vTempData.size(); ++j)
				{
					GetAnimTimes(vTempData[j].mfTime,&fTimeBefore, &fTimeAfter, pTimeVec);

					/////////////////////
					//Time exists
					if(fTimeBefore == vTempData[j].mfTime){
						//Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
						continue;
					}
					/////////////////////
					//Time between two keys
					else if(fTimeBefore>=0 && fTimeAfter>=0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);

						float fT = (vTempData[j].mfTime - pBefore->mfTime)/(pAfter->mfTime -  pBefore->mfTime);

						vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum]*(1.0f-fT) + 
														pAfter->mvRot.v[lVecNum]*fT;
						
						//Log("Interpolated sample %s time %f between %f and %f. T=%f\n",
						//	Sampler.msId.c_str(), vTempData[j].mfTime,
						//	pBefore->mfTime,pAfter->mfTime,fT);
					}
					/////////////////////
					//Time before start
					else if(fTimeBefore<0 && fTimeAfter>=0)
					{
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);

						vTempData[j].mvRot.v[lVecNum] = pAfter->mvRot.v[lVecNum];

						//Log("Added before at sample %s time %f using %f\n",
						//	Sampler.msId.c_str(), vTempData[j].mfTime,
						//	pAfter->mfTime);
					}
					/////////////////////
					//Time after end
					else if(fTimeBefore>=0 && fTimeAfter<0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);

						vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum];
					}
				}
			}
			//////////////////////////////////
			//Add the rotation and translation from a matrix in this sampler
			else if(pTrans->msType == "matrix")
			{
				bLoadedRotation = true;
				bLoadedTranslation = true;

				for(size_t j=0; j <pTimeVec->size(); j++)
				{	
					//Get the temp data:
					cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
					if(pTempData==NULL){ Error("Code error at %d\n",__LINE__); return NULL;}

					cMatrixf mtxFrame = GetMatrix(cMatrixf(&(*pValueVec)[j*16]));

					if(pBone)
					{
						cMatrixf mtxBone = pBone->GetLocalTransform();
						cMatrixf mtxInvBone = cMath::MatrixInverse(mtxBone);
						cVector3f vChange = mtxFrame.GetTranslation() - mtxBone.GetTranslation();
						mtxFrame = cMath::MatrixMul(mtxInvBone.GetRotation(), mtxFrame.GetRotation());
						mtxFrame.SetTranslation(vChange);
					}

					pTempData->mvTrans += (mtxFrame.GetTranslation());
					
					pTempData->mvRot += (cMath::Vector3ToDeg(cMath::MatrixToEulerAngles(mtxFrame, eEulerRotationOrder_XYZ)));

				}
				
				

				////////////////////////////////
				//Go through all of Temp data and find times not added
				float fTimeBefore=-1, fTimeAfter=-1;
				for(size_t j=0; j<vTempData.size(); ++j)
				{
					GetAnimTimes(vTempData[j].mfTime,&fTimeBefore, &fTimeAfter, pTimeVec);

					/////////////////////
					//Time exists
					if(fTimeBefore == vTempData[j].mfTime){
						//Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
						continue;
					}
					/////////////////////
					//Time between two keys
					else if(fTimeBefore>=0 && fTimeAfter>=0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);

						float fT = (vTempData[j].mfTime - pBefore->mfTime)/(pAfter->mfTime -  pBefore->mfTime);

						for(size_t lVecNum=0; lVecNum < 3; ++lVecNum)
							vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum]*(1.0f-fT) + 
															pAfter->mvRot.v[lVecNum]*fT;

						vTempData[j].mvTrans = pBefore->mvTrans*(1.0f-fT) + pAfter->mvTrans*fT;
					}
					/////////////////////
					//Time before start
					else if(fTimeBefore<0 && fTimeAfter>=0)
					{
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter,vTempData);

						for(size_t lVecNum=0; lVecNum < 3; ++lVecNum)
							vTempData[j].mvRot.v[lVecNum] = pAfter->mvRot.v[lVecNum];

						vTempData[j].mvTrans = pAfter->mvTrans;
					}
					/////////////////////
					//Time after end
					else if(fTimeBefore>=0 && fTimeAfter<0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore,vTempData);

						for(size_t lVecNum=0; lVecNum < 3; ++lVecNum)
							vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum];

						vTempData[j].mvTrans = pBefore->mvTrans;
					}
				}
			}
		}
		
		//Log(" Loaded trans: %d, rot: %d\n", bLoadedTranslation, bLoadedRotation);
		

		/*Log("Animation %s\n",aAnim.msName.c_str());
		for(size_t i=0; i <vTempData.size(); i++)
		{
			Log("Time: %f T: (%s) R: (%s) S: (%s)\n", vTempData[i].mfTime, 
				vTempData[i].mvTrans.ToString().c_str(),
				vTempData[i].mvRot.ToString().c_str(), 
				vTempData[i].mvScale.ToString().c_str());
		}*/

		////////////////////////////////////
		//Create new animation track
		cAnimationTrack * pTrack = apAnimation->CreateTrack(aAnim.msTargetNode,
															eAnimTransformFlag_Rotate | 
															eAnimTransformFlag_Translate);
		////////////////////////////////////
		//Iterate the temporary data and add to the track.
		for(size_t i=0; i < vTempData.size(); i++)
		{
			cKeyFrame *pFrame = pTrack->CreateKeyFrame(vTempData[i].mfTime - apScene->mfStartTime);

			//////////////////
			//Get the the node matrix offset
			cMatrixf mtxLocal = pNode->m_mtxTransform;
			cMatrixf mtxRotChange = mtxLocal.GetRotation();
			cVector3f vTransChange = mtxLocal.GetTranslation();
			if(pBone)
			{
				cMatrixf mtxBone = pBone->GetLocalTransform();
				cMatrixf mtxInvBone = cMath::MatrixInverse(mtxBone);
				
				mtxRotChange = cMath::MatrixMul(mtxLocal.GetRotation(), mtxInvBone.GetRotation());
				vTransChange = mtxLocal.GetTranslation() - mtxBone.GetTranslation();
			}			

			///////////////////////////
            //Set Translation
			if(bLoadedTranslation)
			{
				pFrame->trans = vTempData[i].mvTrans;
			}
			else
			{
				pFrame->trans = vTransChange;
			}
			
			///////////////////////////
			//Set Rotation
			if(bLoadedRotation)
			{
				//Create the quaternion from rotations.
				cVector3f vRadRot = vTempData[i].mvRot;
				vRadRot = cVector3f(cMath::ToRad(vRadRot.x),cMath::ToRad(vRadRot.y),cMath::ToRad(vRadRot.z) );
				cMatrixf mtxRot = cMath::MatrixRotate(vRadRot, eEulerRotationOrder_XYZ);
				cQuaternion qRot;
				qRot.FromRotationMatrix(mtxRot);

				pFrame->rotation = qRot;
				//pFrame->base_angles = vRadRot;
			}
			else
			{
				pFrame->rotation.FromRotationMatrix(mtxRotChange);
				//pFrame->base_angles = cMath::MatrixToEulerAngles(mtxRotChange, eEulerRotationOrder_XYZ);
			}
		}

		return pTrack;
	}

	static tString gsTemp;
	static const char* GetTabs(int alDepth)
	{
		gsTemp = "";

		for(int i=0;i<alDepth;i++)gsTemp+="\t";

		return gsTemp.c_str();
	}

	//-----------------------------------------------------------------------


	void cMeshLoaderCollada::CalcLocalMatrixAfterControllerBindMatrixRec(cBone* apBone, cMatrixf a_mtxParentGlobal, int alDepth)
	{
		//Log("%s %s \n",GetTabs(alDepth),apBone->GetName().c_str());

		cMatrixf mtxGlobal;
		//////////////////////
		//The bone is bound to the skin and has a world matrix
		if(apBone->GetValue()!=0)
		{
			mtxGlobal = apBone->GetLocalTransform();
			cMatrixf mtxInvParent = cMath::MatrixInverse(a_mtxParentGlobal);

			cMatrixf mtxLocal = cMath::MatrixMul(mtxInvParent,mtxGlobal);

			apBone->SetTransform(mtxLocal);
		}
		////////////////////////
		// Bone is not bound and has local matrix.
		else
		{
			mtxGlobal = apBone->GetWorldTransform();
		}


		cBoneIterator it = apBone->GetChildIterator();
		while(it.HasNext())
		{
			CalcLocalMatrixAfterControllerBindMatrixRec(it.Next(),mtxGlobal,alDepth+1);
		}
	}


	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::CreateSkeletonBone(cColladaNode* apColladaNode, cBone* apParentBone)
	{
		cBone *pBone = apParentBone->CreateChildBone(apColladaNode->msId, apColladaNode->msSid);

		pBone->SetTransform(apColladaNode->m_mtxTransform);

		tColladaNodeListIt it = apColladaNode->mlstChildren.begin();
		for(; it != apColladaNode->mlstChildren.end();it++)
		{
			CreateSkeletonBone(*it, pBone);
		}
	}

	//-----------------------------------------------------------------------

	iVertexBuffer * cMeshLoaderCollada::CreateVertexBuffer(cColladaGeometry & aGeometry, 
		eVertexBufferUsageType aUsageType)
		//,tColladaExtraVtxListVec &vExtraVtxVec)
	{
		//tVertexVec vVertexVec;
		//tUIntVec vIndexVec;
		//SplitVertices(aGeometry,vExtraVtxVec,vVertexVec,vIndexVec);

		//Log("Creating vertex buffer for '%s'\n",aGeometry.msName.c_str());

		//TEMP: use the one in geometry directly.
		//vExtraVtxVec = aGeometry.mvExtraVtxVec;

		//Create vertex buffer and fill it
		iVertexBuffer *pVtxBuff = mpLowLevelGraphics->CreateVertexBuffer(
			eVertexBufferType_Hardware,
			eVertexBufferDrawType_Tri, aUsageType,
			(int)aGeometry.mvVertexVec.size(), (int)aGeometry.mvIndexVec.size());
		pVtxBuff->CreateElementArray( eVertexBufferElement_Position,eVertexBufferElementFormat_Float, 4);
		pVtxBuff->CreateElementArray( eVertexBufferElement_Normal,eVertexBufferElementFormat_Float, 3);
		pVtxBuff->CreateElementArray( eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float, 3);
		pVtxBuff->CreateElementArray( eVertexBufferElement_Color0,eVertexBufferElementFormat_Float, 4);
		pVtxBuff->CreateElementArray( eVertexBufferElement_Texture1Tangent,eVertexBufferElementFormat_Float, 4);
		
		pVtxBuff->ResizeArray(eVertexBufferElement_Texture1Tangent,(int)aGeometry.mvTangents.size());

		//Add vertices
		for(size_t j=0; j<aGeometry.mvVertexVec.size();j++)
		{
			pVtxBuff->AddVertexVec3f(eVertexBufferElement_Position,aGeometry.mvVertexVec[j].pos);
			pVtxBuff->AddVertexVec3f(eVertexBufferElement_Normal,aGeometry.mvVertexVec[j].norm);
			pVtxBuff->AddVertexVec3f(eVertexBufferElement_Texture0,aGeometry.mvVertexVec[j].tex);
		
			pVtxBuff->AddVertexColor(eVertexBufferElement_Color0,cColor(1,1));
		}
		
		//Add tangents
		memcpy(pVtxBuff->GetFloatArray(eVertexBufferElement_Texture1Tangent),&aGeometry.mvTangents[0], aGeometry.mvTangents.size()*sizeof(float));
		
		//Add indices
		for(size_t j=0; j<aGeometry.mvIndexVec.size();j++)
		{	
			//Flip order of indices
			size_t idx = (j/3)*3 + (2-(j%3));

			pVtxBuff->AddIndex(aGeometry.mvIndexVec[idx]);	
		}

		//Compile the vertex buffer
		pVtxBuff->Compile(0);//eVertexCompileFlag_CreateTangents);

		return pVtxBuff;
	}


	
	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::LoadLights(TiXmlElement* apRootElem, tColladaLightVec &avColladaLightVec)
	{
		TiXmlElement* pLightElem = apRootElem->FirstChildElement("light");
		for(;pLightElem!=NULL;	pLightElem = pLightElem->NextSiblingElement("light"))
		{
			cColladaLight Light;
			Light.msId = cString::ToString(pLightElem->Attribute("id"),"");
			Light.msName = cString::ToString(pLightElem->Attribute("name"),"");
			
			TiXmlElement *pTechniqueCommonElem = pLightElem->FirstChildElement("technique_common");

			//////////////////////////////////////////////
			//COLLADA 1.4
			if(pTechniqueCommonElem)
			{
				TiXmlElement *pTypeElem = pTechniqueCommonElem->FirstChildElement();
				if(pTypeElem==NULL){
					Log("No Type element found!\n");
					continue;
				}
				
				Light.msType = cString::ToString(pTypeElem->Value(),"");
				
				/////////////
				//Color
				TiXmlElement *pParamElem = pTypeElem->FirstChildElement("color");
				if(pParamElem)
				{
					TiXmlText *pText = pParamElem->FirstChild()->ToText();
					tFloatVec vColor;
					cString::GetFloatVec(pText->Value(), vColor);
					Light.mDiffuseColor.r = vColor[0];
					Light.mDiffuseColor.g = vColor[1];
					Light.mDiffuseColor.b = vColor[2];
				}
				else
				{
					Light.mDiffuseColor = cColor(1,1);
				}

				/////////////
				//Angle
				pParamElem = pTypeElem->FirstChildElement("falloff_angle");
				if(pParamElem)
				{
					TiXmlText *pText = pParamElem->FirstChild()->ToText();
					Light.mfAngle = cString::ToFloat(pText->Value(),0);
				}
				else
				{
					Light.mfAngle =0;
				}
			}
			///////////////////////////////////////////////
			//COLLADA 1.3 (NOT SUPPORTED ANY LONGER)
			else
			{
				Light.msType = cString::ToLowerCase(cString::ToString(pLightElem->Attribute("type"),""));

				TiXmlElement *pParamElem = pLightElem->FirstChildElement("param");
				for(; pParamElem; pParamElem = pParamElem->NextSiblingElement("param"))
				{
					tString sName = cString::ToString(pParamElem->Attribute("name"),"");
					TiXmlText *pText = pParamElem->FirstChild()->ToText();

					if(sName == "COLOR")
					{
						tFloatVec vColor;
						cString::GetFloatVec(pText->Value(), vColor);
						Light.mDiffuseColor.r = vColor[0];
						Light.mDiffuseColor.g = vColor[1];
						Light.mDiffuseColor.b = vColor[2];
					}
					else if(sName == "ANGLE")
					{
						Light.mfAngle = cString::ToFloat(pText->Value(),0);
					}
				}
			}

			//Log("Loaded light '%s', type '%s', color: %f %f %f\n", Light.msId.c_str(), 
			//				Light.msType.c_str(),
			//				Light.mDiffuseColor.r,Light.mDiffuseColor.g,Light.mDiffuseColor.b);

			avColladaLightVec.push_back(Light);
		}
	}

	//-----------------------------------------------------------------------

	static cColladaAnimation& GetAnimationFromTarget(const tString& asTargetNode, 
														tColladaAnimationVec &avAnimations)
	{
		for(size_t i=0; i< avAnimations.size(); i++)
		{
			if(avAnimations[i].msTargetNode == asTargetNode)
			{
				return avAnimations[i];
			}
		}
		
        //No animation with that target found, create new.
		avAnimations.push_back(cColladaAnimation());
		cColladaAnimation& Anim = avAnimations[avAnimations.size() -1];
		Anim.msTargetNode = asTargetNode;

		return Anim;
	}

	void cMeshLoaderCollada::LoadAnimations(TiXmlElement* apRootElem, tColladaAnimationVec &avAnimations,
		cColladaScene *apColladaScene)
	{
		TiXmlElement* pAnimElem = apRootElem->FirstChildElement("animation");
		for(;pAnimElem!=NULL; pAnimElem = pAnimElem->NextSiblingElement("animation"))
		{
			/////////////////////////////////////////////
			// Check if there are multiple meshes
			if(pAnimElem->FirstChildElement("animation") != NULL)
			{
				/////////////
				// Go down one level
				LoadAnimations(pAnimElem, avAnimations, apColladaScene);
				continue;
			}

			/////////////////////////////////////////////
			//Check what animation to use.
			TiXmlElement *pTestChannelElem = pAnimElem->FirstChildElement("channel");
			if(pTestChannelElem==NULL){Warning("Animation missing channel!\n");	continue;}
			
			//Get target node name
			tString sTestTarget = cString::ToString(pTestChannelElem->Attribute("target"),"");
			tStringVec vTargetStrings;
			tString sTargetSepp = "/";
			cString::GetStringVec(sTestTarget,vTargetStrings,&sTargetSepp);
			sTestTarget = vTargetStrings[0];
			
			cColladaAnimation& Anim = GetAnimationFromTarget(sTestTarget, avAnimations);
            
			//Anim.msName = cString::ToString(pAnimElem->Attribute("name"),""); No need..
			Anim.msId = cString::ToString(pAnimElem->Attribute("id"),"");
						
			//////////////////////////////////
			// Load all Channels
			TiXmlElement *pChannelElem = pAnimElem->FirstChildElement("channel");
			for(; pChannelElem!=NULL ; pChannelElem = pChannelElem->NextSiblingElement("channel"))
			{
				cColladaChannel	Channel;
				Channel.msId = cString::ToString(pChannelElem->Attribute("id"),"");
				Channel.msSource = cString::ToString(pChannelElem->Attribute("source"),"");
				Channel.msTarget  = cString::ToString(pChannelElem->Attribute("target"),"");
				GetAdress(Channel.msSource);

				Anim.mvChannels.push_back(Channel);    			
			}

			//////////////////////////////////
			// Load all Samplers
			TiXmlElement *pSamplerElem = pAnimElem->FirstChildElement("sampler");
			for(; pSamplerElem!=NULL ; pSamplerElem = pSamplerElem->NextSiblingElement("sampler"))
			{
				cColladaSampler Sampler;
				Sampler.msId = cString::ToString(pSamplerElem->Attribute("id"),"");

				//Iterate the inputs and find the needed types.
				TiXmlElement *pInput = pSamplerElem->FirstChildElement("input");
				for(; pInput!=NULL ; pInput = pInput->NextSiblingElement("input"))
				{
					tString sSemantic = cString::ToString(pInput->Attribute("semantic"),"");
					tString sSource = cString::ToString(pInput->Attribute("source"),"");
					GetAdress(sSource);

					if(sSemantic == "INPUT")
					{
						Sampler.msTimeArray = sSource;
					}
					else if(sSemantic == "OUTPUT")
					{
						Sampler.msValueArray = sSource;
					}
				}
				Anim.mvSamplers.push_back(Sampler);
			}

			//////////////////////////////////
			// Set target for the Samplers.
			// This means that the channels can be skipped later on makes things simpler!
			// These values will only work if the samplers are not shared among channels
			for(size_t i=0; i< Anim.mvChannels.size(); i++)
			{
				for(size_t j=0; j<Anim.mvSamplers.size(); j++)
				{
					if(Anim.mvChannels[i].msSource == Anim.mvSamplers[j].msId)
					{
						Anim.mvSamplers[j].msTarget = Anim.mvChannels[i].msTarget;
					}
				}
			}

			//////////////////////////////////
			// Iterate through all the sources
			TiXmlElement *pSourceElem = pAnimElem->FirstChildElement("source");
			for(; pSourceElem!=NULL ; pSourceElem = pSourceElem->NextSiblingElement("source"))
			{
				Anim.mvSources.push_back(cColladaAnimSource());
				cColladaAnimSource &Source = Anim.mvSources.back();

				Source.msId = cString::ToString(pSourceElem->Attribute("id"),"");

				TiXmlElement *pArrayElem = pSourceElem->FirstChildElement("float_array");
				if(pArrayElem == NULL){
					//Warning("No array element found for animation data (shouldn't be anything bad)!\n"); 
					continue;
				}

				int lCount = cString::ToInt(pArrayElem->Attribute("count"),0);
				Source.mvValues.reserve(lCount);

				// Count can be 0, check to avoid crashes
				if(lCount>0)	
				{
					TiXmlText *pText = pArrayElem->FirstChild()->ToText();
					cString::GetFloatVec(pText->Value(),Source.mvValues);
				}
			}
		}
	}

	//-----------------------------------------------------------------------



	void cMeshLoaderCollada::LoadColladaScene(TiXmlElement* apRootElem,cColladaNode *apParentNode, 
											cColladaScene *apScene,tColladaLightVec *apColladaLightVec)
	{
		cColladaNode *pNode = apParentNode->CreateChild();
		apScene->mlstNodes.push_back(pNode);
		
		//The local matrix
		cMatrixf mtxTransform = cMatrixf::Identity;

		//Vector to save transform values in later on.
		tFloatVec vValVec;
		vValVec.reserve(5);

		///////////////////////////////////////////
		//Get properties
		pNode->msId = cString::ToString(apRootElem->Attribute("id"),"");
		pNode->msName = cString::ToString(apRootElem->Attribute("name"),"");
		pNode->msSid = cString::ToString(apRootElem->Attribute("sid"),pNode->msId);
		pNode->msType = cString::ToString(apRootElem->Attribute("type"),"");

		// XXX
		// Removed as it introduces backwards compatibility issues!!
		//if(pNode->msName.empty())
		//{
		//	pNode->msName = pNode->msId;
		//	Warning("Scene node with id '%s' has empty name! Setting id as name\n", pNode->msId.c_str());
		//}

		/////////////////////////////////////////////
		//Get source, if there is any.
		TiXmlElement *pInstanceElem = apRootElem->FirstChildElement("instance_geometry");
		if(pInstanceElem==NULL) pInstanceElem = apRootElem->FirstChildElement("instance_light");
		if(pInstanceElem==NULL) pInstanceElem = apRootElem->FirstChildElement("instance_controller");
		if(pInstanceElem==NULL) pInstanceElem = apRootElem->FirstChildElement("instance");
		if(pInstanceElem)
		{
			/////////////////////////////////////////////
			//Get geometry instance
			tString sSource = cString::ToString(pInstanceElem->Attribute("url"),"");
			if(sSource[0] == '#') pNode->mbSourceIsFile = false;
			else					pNode->mbSourceIsFile = true;

			GetAdress(sSource);

			pNode->msSource = sSource;

			/////////////////////////////////////////////
			//Get Material instance, if any
			TiXmlElement *pBindMaterialElem = pInstanceElem->FirstChildElement("bind_material");
			if(pBindMaterialElem)
			{
				TiXmlElement *pTechniqueCommonElem = pBindMaterialElem->FirstChildElement("technique_common");
                if(pTechniqueCommonElem)
				{
					TiXmlElement *pInstanceMaterialElem = pTechniqueCommonElem->FirstChildElement("instance_material");
                    if(pInstanceMaterialElem)
					{
						pNode->msInstanceMaterial = cString::ToString(pInstanceMaterialElem->Attribute("target"),"");
						GetAdress(pNode->msInstanceMaterial);
					}
				}
			}
		}

		

		//Log("Node. %s, type: %s\n",pNode->msId.c_str(),pNode->msType.c_str());

		cVector3f vTranslation = cVector3f(0,0,0);
		
		///////////////////////////////////////////////////////////
		//Iterate through all of the transforms.
		TiXmlElement *pTransformElem = apRootElem->FirstChildElement();
		while(pTransformElem)
		{
			tString sVal = pTransformElem->Value();
			tString sSid = cString::ToString(pTransformElem->Attribute("sid"),"");

			TiXmlNode *pChildElem = pTransformElem->FirstChild();
			if(pChildElem==NULL){
				pTransformElem = pTransformElem->NextSiblingElement();
				continue;
			}

			//Log("val: %s\n",sVal.c_str());

			TiXmlText *pText= pChildElem->ToText();
			if(pText==NULL)
			{
				pTransformElem = pTransformElem->NextSiblingElement();
				continue;
			}

			cString::GetFloatVec(pText->Value(), vValVec);
			
			// Translation
			if(sVal == "translate")
			{
				cVector3f vTrans = GetVectorPosFromPtr(&vValVec[0]);

				mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixTranslate(vTrans));
			}
			// Rotation
			else if(sVal == "rotate")
			{
				cQuaternion qRot;
				cVector3f vRotAxis = GetVectorRotationFromPtr(&vValVec[0]);

				qRot.FromAngleAxis(cMath::ToRad(vValVec[3]),vRotAxis);

				mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixQuaternion(qRot));
			}
			// Scaling
			else if(sVal == "scale")
			{	
				cVector3f vScale = GetVectorScaleFromPtr(&vValVec[0]);
				pNode->mvScale = vScale;
				
				//If this node is a light, do not apply scale.
				if(apColladaLightVec && GetLight(pNode->msSource, *apColladaLightVec))
				{
				}
				//Colliders do not use scale.
				else if(cString::ToLowerCase(cString::Sub(pNode->msName,1,8))=="collider"
					|| cString::ToLowerCase(cString::Sub(pNode->msName,1,12))=="charcollider"
					||  cString::ToLowerCase(cString::Sub(pNode->msName,1,4))=="area")
				{
				
				}
				//This a geometry node (or something else..). Apply scale as normal
				else
				{
					mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixScale(vScale));
				}
			}
			else if(sVal == "matrix")
			{
				cMatrixf mtxBone = GetMatrix(cMatrixf(&vValVec[0]));
				mtxTransform = cMath::MatrixMul(mtxTransform, mtxBone);
			}


			pNode->mlstTransforms.push_back(cColladaTransform());
			cColladaTransform &Transform = pNode->mlstTransforms.back();

			Transform.msSid = sSid;
			Transform.mvValues = vValVec;
			Transform.msType = sVal;

			/*Log("Transform:\n");
			Log("Sid: %s type: %s ",Transform.msSid.c_str(), Transform.msType.c_str());
			Log("Values: ");
			for(size_t i=0; i < Transform.mvValues.size();i++)
			{
			Log("%f ",Transform.mvValues[i]);
			}
			Log("\n");*/

			vValVec.clear();
			pTransformElem = pTransformElem->NextSiblingElement();
		}

		pNode->m_mtxTransform = mtxTransform;
		pNode->m_mtxWorldTransform = cMath::MatrixMul(apParentNode->m_mtxWorldTransform, mtxTransform);

		//Load all children
		TiXmlElement *pNodeElem = apRootElem->FirstChildElement("node");
		while(pNodeElem)
		{
			LoadColladaScene(pNodeElem, pNode,apScene,apColladaLightVec);

			pNodeElem = pNodeElem->NextSiblingElement("node");
		}
	}

	//-----------------------------------------------------------------------


	void cMeshLoaderCollada::LoadControllers(TiXmlElement* apRootElem,
		tColladaControllerVec &avColladaControllerVec,
		tColladaGeometryVec *apColladaGeometryVec)
	{
		TiXmlElement* pCtrlElem = apRootElem->FirstChildElement("controller");
		for(;pCtrlElem!=NULL; pCtrlElem = pCtrlElem->NextSiblingElement("controller"))
		{
			avColladaControllerVec.push_back(cColladaController());

			cColladaController &Controller = avColladaControllerVec[avColladaControllerVec.size()-1];

			Controller.msId = cString::ToString(pCtrlElem->Attribute("id"),"");
			
			///////////////////////////////////////
			// Get Skin element.
			TiXmlElement *pSkinElem = pCtrlElem->FirstChildElement("skin");
			if(pSkinElem==NULL){ Error("No Skin found in controller!\n"); continue;}
			
			Controller.msTarget = cString::ToString(pSkinElem->Attribute("source"),"");
			GetAdress(Controller.msTarget);

			//Get the bind matrix
			TiXmlElement *pBindMatrixElem = pSkinElem->FirstChildElement("bind_shape_matrix");
			if(pBindMatrixElem)
			{
				TiXmlText *pText = pBindMatrixElem->FirstChild()->ToText();
				tFloatVec vValues;
				cString::GetFloatVec(pText->Value(), vValues);

				Controller.m_mtxBindShapeMatrix = GetMatrix(&vValues[0]);
			}
			else
			{
				Warning("No bind matrix in controller '%s' using identity\n",Controller.msId.c_str());
				Controller.m_mtxBindShapeMatrix = cMatrixf::Identity;
			}
			

			//These are used so you can find what the different sources contain.
			tString sJointNameSource="";
			tString sJointWeightSource="";
			tString sJointMatrixSource="";
			int lJointOffset=-1;
			int lWeightOffset=-1;
			
			////////////////////////////////////////
			// Load Joint information
			{
				TiXmlElement *pJointsElem = pSkinElem->FirstChildElement("joints");
				if(pJointsElem==NULL){ Warning("Couldn't find joint element for controller!\n"); continue;}
				
				TiXmlElement *pInputElem = pJointsElem->FirstChildElement("input");
				for(; pInputElem != NULL; pInputElem = pInputElem->NextSiblingElement("input"))
				{
					tString sSemantic = cString::ToString(pInputElem->Attribute("semantic"),"");
					tString sSource = cString::ToString(pInputElem->Attribute("source"),"");
					GetAdress(sSource);
					
					//The names of the joints
					if(sSemantic=="JOINT")
					{
						sJointNameSource = sSource;
					}
					else if(sSemantic=="INV_BIND_MATRIX")
					{
						sJointMatrixSource = sSource;
					}
				}
			}

			////////////////////////////////////////
			// Load Joint weight info
			{
				TiXmlElement *pJointWeightElem = pSkinElem->FirstChildElement("vertex_weights");
				if(pJointWeightElem==NULL){ Warning("Couldn't find vertex_weights element for controller!\n"); continue;}

				TiXmlElement *pInputElem = pJointWeightElem->FirstChildElement("input");
				for(; pInputElem != NULL; pInputElem = pInputElem->NextSiblingElement("input"))
				{
					tString sSemantic = cString::ToString(pInputElem->Attribute("semantic"),"");
					tString sSource = cString::ToString(pInputElem->Attribute("source"),"");
					int lOffset  = cString::ToInt(pInputElem->Attribute("offset"),-1);
					GetAdress(sSource);

					//The names of the joints
					if(sSemantic=="JOINT")
					{
						lJointOffset = lOffset;
					}
					//The weight of the joints
					else if(sSemantic=="WEIGHT")
					{
						lWeightOffset = lOffset;
						sJointWeightSource = sSource;
					}
				}	
			}

			////////////////////////////////////////
			// Get the sources and load / apply them
			TiXmlElement *pSourceElem = pSkinElem->FirstChildElement("source");
			while(pSourceElem)
			{
				tString sId = cString::ToString(pSourceElem->Attribute("id"),"");
				
				//////////////////
				//Name of joints
				if(sId == sJointNameSource)
				{
					TiXmlElement *pNameArrayElem = pSourceElem->FirstChildElement("Name_array");
					if(pNameArrayElem==NULL){ Warning("Couldn't find name array!\n"); continue;}
                    
					int lCount = cString::ToInt(pNameArrayElem->Attribute("count"),0);

					//Reserve for faster push_back
					Controller.mvJoints.reserve(lCount);
					
					TiXmlText *pNameText = pNameArrayElem->FirstChild()->ToText();
					if(pNameText==NULL){ Error("No joint name data found!\n"); continue;}
					
					tStringVec vJoints;

					cString::GetStringVec(pNameText->Value(),vJoints);

					///////
					// Support for bones names that have blankspace
					if(vJoints.size() != lCount)
					{
						// Name has blankspace
						tString sRootName = vJoints[0];
						tString sJoint = sRootName;

						for(size_t i = 1; i < vJoints.size(); ++i)
						{
							if(vJoints[i] == sRootName)
							{
								////////////////
								// Reached a new hierarchy
								// Push back the joint and start on a new joint
								Controller.mvJoints.push_back(sJoint);
								sJoint = sRootName;
							}
							else
							{
								// Append the part
								sJoint += " " + vJoints[i];
							}
						}

						if(sJoint != sRootName)
						{
							// Add the last joint
							Controller.mvJoints.push_back(sJoint);
						}
					}
					else
					{
						// No blankspace in name, copy the data
						for(int i = 0; i < lCount; ++i) Controller.mvJoints.push_back(vJoints[i]);
					}
				}
				//////////////////
				//Joint weights or matrices
				else if(sId == sJointWeightSource || sId == sJointMatrixSource)
				{
					TiXmlElement *pFloatArrayElem = pSourceElem->FirstChildElement("float_array");
					if(pFloatArrayElem==NULL){ Warning("Couldn't find name array!\n"); continue;}
					
					int lCount = cString::ToInt(pFloatArrayElem->Attribute("count"),0);

					//Get the text data
					TiXmlText *pText = pFloatArrayElem->FirstChild()->ToText();
					if(pText==NULL){ Error("No value data found!\n"); return;}
					
					//Convert text to floats
					tFloatVec vValVec;
					vValVec.reserve(lCount);
					cString::GetFloatVec(pText->Value(), vValVec);
					
					//Weights
					if(sId == sJointWeightSource)
					{
						Controller.mvWeights = vValVec;
					}
					//Matrices
					else
					{
						Controller.mvMatrices.reserve(lCount / 16);
						
						for(int i=0; i< (lCount / 16); i++)
						{
							cMatrixf mtxTemp(&vValVec[i*16]);
							Controller.mvMatrices.push_back(GetMatrix(mtxTemp));
						}
					}
				}

				pSourceElem = pSourceElem->NextSiblingElement("source");
			}

			////////////////////////////////////////
			// Get joint - vertex pairs.
			{
				TiXmlElement *pJointWeightElem = pSkinElem->FirstChildElement("vertex_weights");
				if(pJointWeightElem==NULL){ Warning("Couldn't find vertex_weights element for controller!\n"); continue;}
				
				////////////////////////////
				// Vcount - get the number of joints for each vertex.
				TiXmlElement *pVCountElem = pJointWeightElem->FirstChildElement("vcount");
				if(pVCountElem==NULL){ Warning("Couldn't find vertex_weights vcount element for controller!\n"); continue;}

				//Get the text data
				TiXmlText *pVCountText = pVCountElem->FirstChild()->ToText();
				if(pVCountText==NULL){ Error("No value data found!\n"); continue;}

                tIntVec vVCount;
				cString::GetIntVec(pVCountText->Value(),vVCount);

				/////////////////////////////
				//V - Get the pairs
				TiXmlElement *pVElem = pJointWeightElem->FirstChildElement("v");
				if(pVElem==NULL){ Warning("Couldn't find vertex_weights v element for controller!\n"); continue;}

				//Get the text data
				TiXmlText *pVText = pVElem->FirstChild()->ToText();
				if(pVText==NULL){ Error("No value data found!\n"); continue;}

				tIntVec vV;
				cString::GetIntVec(pVText->Value(),vV);
				
				int lVtx=0;
				int lNumOfPairs = ((int)vV.size())/2;
				int lPairCount=0;

				
				Controller.mvPairs.resize(vVCount.size());
				for(int lPair=0; lPair<lNumOfPairs; ++lPair)
				{
					cColladaJointPair Pair;

					Pair.mlJoint = vV[lPair*2 + lJointOffset];
					Pair.mlWeight = vV[lPair*2 + lWeightOffset];

					//Log("Pair: %d, %d vtx: %d\n",Pair.mlJoint, Pair.mlWeight,lVtx);

					Controller.mvPairs[lVtx].push_back(Pair);
					
					//Check if it is time for a new vertex.
					lPairCount++;
					if(lPairCount >= vVCount[lVtx]){
						lVtx++; lPairCount =0;
					}
				}
			}
		}
	}
	
	//-----------------------------------------------------------------------
	
	static tVector3fVec *gpVertexVec = NULL;

	///////////////////////////////////////////

	class cColladaTestTri
	{
	public:
		cColladaTestTri(cColladaVtxIndex *apTriIndex)
		{
			//Log("TestTri:\n");
			//Get the vectors
			for(int i=0; i<3; i++)
			{
				mvPos[i] = (*gpVertexVec)[apTriIndex[i].mlVtx];

				//Log("Pos%d: %s\n",i,mvPos[i].ToString().c_str());
			}
			
			//Sort the vectors 
			for(int i=0; i<3; i++)
			{
				for(int j=0; j<3;j++)
				{
					if(j==i)continue;

					if(mvPos[i] < mvPos[j])
					{
						cVector3f vTemp = mvPos[i];
						mvPos[i] = mvPos[j];
						mvPos[j] = vTemp;
					}
				}
			}

			//for(int i=0; i<3; i++) Log("Pos%d: %s\n",i,mvPos[i].ToString().c_str());
		}

		cVector3f mvPos[3];
	};

	///////////////////////////////////////////
	
	class cColladaTestTriCompare
	{
	public:
		bool operator()(const cColladaTestTri& aTri1, const cColladaTestTri& aTri2)const
		{
			//0
			if(aTri1.mvPos[0].x != aTri2.mvPos[0].x) return aTri1.mvPos[0].x < aTri2.mvPos[0].x;
			if(aTri1.mvPos[0].y != aTri2.mvPos[0].y) return aTri1.mvPos[0].y < aTri2.mvPos[0].y;
			if(aTri1.mvPos[0].z != aTri2.mvPos[0].z) return aTri1.mvPos[0].z < aTri2.mvPos[0].z;
			//1
			if(aTri1.mvPos[1].x != aTri2.mvPos[1].x) return aTri1.mvPos[1].x < aTri2.mvPos[1].x;
			if(aTri1.mvPos[1].y != aTri2.mvPos[1].y) return aTri1.mvPos[1].y < aTri2.mvPos[1].y;
			if(aTri1.mvPos[1].z != aTri2.mvPos[1].z) return aTri1.mvPos[1].z < aTri2.mvPos[1].z;
			//2
			if(aTri1.mvPos[2].x != aTri2.mvPos[2].x) return aTri1.mvPos[2].x < aTri2.mvPos[2].x;
			if(aTri1.mvPos[2].y != aTri2.mvPos[2].y) return aTri1.mvPos[2].y < aTri2.mvPos[2].y;
			if(aTri1.mvPos[2].z != aTri2.mvPos[2].z) return aTri1.mvPos[2].z < aTri2.mvPos[2].z;

			return false;
		}
	};
	
	///////////////////////////////////////////

	typedef std::set<cColladaTestTri, cColladaTestTriCompare> tColladaTestTriMap;
	typedef tColladaTestTriMap::iterator tColladaTestTriMapIt;


	///////////////////////////////////////////

	void cMeshLoaderCollada::LoadGeometry(TiXmlElement* apRootElem, tColladaGeometryVec &avColladaGeometryVec)
	{
		TiXmlElement* pGeomElem = apRootElem->FirstChildElement("geometry");
		for(; pGeomElem!=NULL; pGeomElem = pGeomElem->NextSiblingElement("geometry"))
		{
			//There should only be one mesh
			TiXmlElement* pMeshElem = pGeomElem->FirstChildElement("mesh");
			if(pMeshElem==NULL){ 
				//Warning("No Mesh element found in geometry element '%s'!\n",Geometry.msName.c_str()); 
				continue;
			}

			avColladaGeometryVec.push_back(cColladaGeometry());
			cColladaGeometry &Geometry = avColladaGeometryVec.back();

			//Get main properties
			Geometry.msId = cString::ToString(pGeomElem->Attribute("id"),"");
			Geometry.msName = cString::ToString(pGeomElem->Attribute("name"),"");

			if(Geometry.msName == "")
			{
				Geometry.msName = Geometry.msId;
				//Fix to skip a max addition on mesh names
				int lPos = cString::GetLastStringPos(Geometry.msName,"-");
				if(lPos >0) Geometry.msName = cString::Sub(Geometry.msName,0,lPos);
			}

			///////////////////////////////////////////////////
			//Check if the geometry should loaded
			if(cString::Sub(Geometry.msName,0,4) == "_ref" ||
				cString::Sub(Geometry.msName,0,3) == "_bb" ||
				cString::Sub(Geometry.msName,0,4) == "_ps" ||
				cString::Sub(Geometry.msName,0,6) == "_sound")
			{
				continue;
			}

			///////////////////////////////////////////////////
			//Iterate through all sources (the vertices)
			TiXmlElement* pSourceElem = pMeshElem->FirstChildElement("source");
			while(pSourceElem)
			{
				//Add a new array
				Geometry.mvArrayVec.push_back(cColladaVtxArray());
				int lPos = (int)Geometry.mvArrayVec.size()-1;

				//Get id
				tString sSourceId = cString::ToString(pSourceElem->Attribute("id"),"");
				//Log("Vertex id: '%s' \n",sSourceId.c_str());

				Geometry.mvArrayVec[lPos].msId = sSourceId;

				LoadVertexData(pSourceElem,Geometry.mvArrayVec[lPos].mvArray);

				/*Log("Array: %s\n",sSourceId.c_str());
				for(int i=0; i< (int)Geometry.mvArrayVec[lPos].mvArray.size(); i++)
				{
					Log("(%s) ",Geometry.mvArrayVec[lPos].mvArray[i].ToString().c_str());
				}
				Log("\n");*/

				pSourceElem = pSourceElem->NextSiblingElement("source");
			}

			///////////////////////////////////////////////////
			//Get the "real" name for the vertices
			//This always includes postions and can include normals and tex coords aswell.
			TiXmlElement* pVerticesElem = pMeshElem->FirstChildElement("vertices");
			if(pVerticesElem==NULL){ Error("Vertices not found!\n"); return;}

			//Iterate the inputs
			TiXmlElement* pVtxInput = pVerticesElem->FirstChildElement("input");
			while(pVtxInput)
			{
				tString sSemantic = cString::ToString(pVtxInput->Attribute("semantic"),"");

				if(sSemantic == "POSITION")
				{
					tString sSource = cString::ToString(pVtxInput->Attribute("source"),"");
					GetAdress(sSource);

					//Log("Position vertex source: %s\n", sSource.c_str());

					for(int i=0;i<(int) Geometry.mvArrayVec.size(); i++)
					{
						//If the vertex array is found just change the name and break.
						if(Geometry.mvArrayVec[i].msId == sSource)
						{
							Geometry.mvArrayVec[i].msId = cString::ToString(pVerticesElem->Attribute("id"),"");
							break;
						}
					}
				}
				else
				{
					tString sSource = cString::ToString(pVtxInput->Attribute("source"),"");
					GetAdress(sSource);

					//Log("%s vertex source: %s\n",sSemantic.c_str(), sSource.c_str());

					//Find array and set some properties
					for(int i=0;i<(int) Geometry.mvArrayVec.size(); i++)
						if(Geometry.mvArrayVec[i].msId == sSource)
						{
							Geometry.mvArrayVec[i].msType = sSemantic;
							Geometry.mvArrayVec[i].mbIsInVertex = true;
							break;
						}
				}

				pVtxInput = pVtxInput->NextSiblingElement("input");
			}


			///////////////////////////////////////////////////
			//Get the Triangles, save them in a row
			TiXmlElement* pTriElem = pMeshElem->FirstChildElement("triangles");
			if(pTriElem==NULL)
			{
				pTriElem = pMeshElem->FirstChildElement("polylist");
				if(pTriElem && pTriElem->NextSibling("polylist"))
					Warning("Geometry '%s' seem to have multitexturing!\n",Geometry.msName.c_str());

			}
			else
			{
				if(pTriElem->NextSibling("triangles"))
					Warning("Geometry '%s' seem to have multitexturing!\n",Geometry.msName.c_str());
				
			}

			
			if(pTriElem==NULL){ 
				//Warning("No triangle or polylist element found, testing polygons.\n");
				pTriElem = pMeshElem->FirstChildElement("polygons");
				if(pTriElem==NULL){ 
					Error("No Polygons found!\n"); return;
				}
			}
			int lTriElements =0;

			int lTriCount = cString::ToInt(pTriElem->Attribute("count"),0);
			Geometry.msMaterial = cString::ToString(pTriElem->Attribute("material"),"");
			GetAdress(Geometry.msMaterial);

			//Get the inputs to figure what the indices in he triangles mean.
			TiXmlElement* pTriInputElem = pTriElem->FirstChildElement("input");
			while(pTriInputElem)
			{
				tString sSemantic = cString::ToString(pTriInputElem->Attribute("semantic"),"");
				tString sSource = cString::ToString(pTriInputElem->Attribute("source"),"");
				int lIdx = cString::ToInt(pTriInputElem->Attribute("idx"),-1);
				if(lIdx < 0) lIdx = cString::ToInt(pTriInputElem->Attribute("offset"),-1);

				GetAdress(sSource);

				int lArrayNum = -1;

				//Get what array that belongs to this input.
				for(int i=0; i<(int)Geometry.mvArrayVec.size(); i++)
				{
					if(Geometry.mvArrayVec[i].msId == sSource)
					{
						lArrayNum = i;
						break;
					}
				}

				//Set the properties for the sort
				if(sSemantic == "VERTEX")
				{
					Geometry.mlPosIdxNum = lIdx;
					Geometry.mlPosArrayIdx = lArrayNum;
				}
				else if(sSemantic == "NORMAL")
				{
					Geometry.mlNormIdxNum = lIdx;
					Geometry.mlNormArrayIdx = lArrayNum;
				}
				else if(sSemantic == "TEXCOORD")
				{
					Geometry.mlTexIdxNum = lIdx;
					Geometry.mlTexArrayIdx = lArrayNum;
				}

				//Increase element num
				if(lTriElements < lIdx+1) lTriElements =lIdx+1;

				//next input
				pTriInputElem = pTriInputElem->NextSiblingElement("input");
			}

			//Go through the arrays and check if any are in the vertex
			for(size_t i=0; i<Geometry.mvArrayVec.size(); i++)
			{
				if(Geometry.mvArrayVec[i].mbIsInVertex)
				{
					if(Geometry.mvArrayVec[i].msType == "NORMAL") {
						Geometry.mlNormArrayIdx = i;
						Geometry.mlNormIdxNum = Geometry.mlPosIdxNum;
					}
					else if(Geometry.mvArrayVec[i].msType == "TEXCOORD") {
						Geometry.mlTexArrayIdx = i;
						Geometry.mlTexIdxNum = Geometry.mlPosIdxNum;
					}
				}
			}

			//Check so that none of the arrays are missing
			if(Geometry.mlNormArrayIdx<0){
				Warning("No normals for geometry '%s'\n",Geometry.msName.c_str());
				continue;
			}
			if(Geometry.mlTexArrayIdx<0 && Geometry.msName[0] != '_'){
				Warning("No tex coords for geometry '%s'\n",Geometry.msName.c_str());
				continue;
			}

			//////////////////////////////
			// If Z is up axis or the unit scale is not 1, go through all the geometry and convert
			if(mbZToY || mfUnitScale != 1.0f)
			{
				//Positions
				tVector3fVec &vVtxVec = Geometry.mvArrayVec[Geometry.mlPosArrayIdx].mvArray;
				for(size_t i=0; i < vVtxVec.size();i++)
				{
					vVtxVec[i] = GetVectorPos(vVtxVec[i]);
				}

				//Normals
				tVector3fVec &vNormVec = Geometry.mvArrayVec[Geometry.mlNormArrayIdx].mvArray;
				for(size_t i=0; i < vNormVec.size();i++)
				{
					vNormVec[i] = GetVectorNormal(vNormVec[i]);
				}
			}

			//reserve space for the indices
			Geometry.mvIndices.reserve(lTriElements * 3);
			
			//Set the vertex array used.
			gpVertexVec = &Geometry.mvArrayVec[Geometry.mlPosArrayIdx].mvArray;

			tColladaTestTriMap map_TestTris;

			// Load all the Indices
			TiXmlElement* pPElem = pTriElem->FirstChildElement("p");
			while(pPElem)
			{
				TiXmlText *pText = pPElem->FirstChild()->ToText();
				if(pText==NULL){ Error("No tri data found!\n"); return;}

				//Get the indices for the triangle
				tIntVec vIndexArray;
				cString::GetIntVec(pText->Value(),vIndexArray);

				int lTriangleNum = (int)vIndexArray.size()/ (3*lTriElements);
                for(int triangle=0; triangle< lTriangleNum; triangle++)
				{
					cColladaVtxIndex DataVec[3];
					int lTriangleAdd = triangle*3*lTriElements;
					                                				
					//Iterate the points in triangle
					//If Z is used as y the order must be reversed.
					//Turns out, it is not so... because x is negated.
					/*if(mbZToY)
					{
						for(int i=2; i>= 0; i--)
						{
							DataVec[2-i].mlVtx = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlPosIdxNum];
							DataVec[2-i].mlNorm = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlNormIdxNum];
							DataVec[2-i].mlTex = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlTexIdxNum];
						}
					}
					else*/
					{
						for(int i=0; i< 3; i++)
						{
							DataVec[i].mlVtx = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlPosIdxNum];
							DataVec[i].mlNorm = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlNormIdxNum];
							DataVec[i].mlTex = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlTexIdxNum];
						}
					}

					//Skipping this test right now as it is only really interesting for shadow volumes and it will destroy double sided textures!
					//std::pair<tColladaTestTriMapIt,bool> TestPair = map_TestTris.insert(DataVec);
					//if(TestPair.second)
					{
						//Add the data to the indices
						for(int i=0;i<3;i++)
						{
							Geometry.mvIndices.push_back(DataVec[i]);
						}
					}
					/*else
					{
						Error("Geometry '%s' has two faces using same vertices! Skipping face.\n",Geometry.msName.c_str());
					}*/
				}
				
				//Next p
				pPElem = pPElem->NextSiblingElement("p");
			}

			///////////////////////////////////
			//Split the vertices and make em usable
			SplitVertices(Geometry,Geometry.mvExtraVtxVec,Geometry.mvVertexVec,Geometry.mvIndexVec);
			Geometry.Clear();

			////////////////////////////////////
            //Create Tangents
			tFloatVec vPosVec;  vPosVec.resize(Geometry.mvVertexVec.size() *4);
			tFloatVec vNormVec; vNormVec.resize(Geometry.mvVertexVec.size() *3);
			tFloatVec vTexVec;  vTexVec.resize(Geometry.mvVertexVec.size() *3);
			
			float *pPosData = &vPosVec[0];
			float *pNormData = &vNormVec[0];
			float *pTexData = &vTexVec[0];
			
			//Fill vectors
			for(size_t i=0; i<Geometry.mvVertexVec.size(); ++i)
			{
				cVertex &vertex = Geometry.mvVertexVec[i];
				
                pPosData[0] = vertex.pos.x;
				pPosData[1] = vertex.pos.y;
				pPosData[2] = vertex.pos.z;
				pPosData[3] = 1;

				pNormData[0] = vertex.norm.x;
				pNormData[1] = vertex.norm.y;
				pNormData[2] = vertex.norm.z;
				
				pTexData[0] = vertex.tex.x;
				pTexData[1] = vertex.tex.y;
				pTexData[2] = vertex.tex.z;

				pPosData +=4;
				pNormData +=3;
				pTexData +=3;
			}
			
			//Creates tangents
			Geometry.mvTangents.resize(Geometry.mvVertexVec.size() *4);
			cMath::CreateTriTangentVectors( &Geometry.mvTangents[0],
						&Geometry.mvIndexVec[0], (int)Geometry.mvIndexVec.size(),
						&vPosVec[0],4, &vTexVec[0],&vNormVec[0],
						(int)Geometry.mvVertexVec.size());
		}
	}

	//////////////////////////////////////

	void cMeshLoaderCollada::LoadVertexData(TiXmlElement* apSourceElem, tVector3fVec &avVtxVec)
	{
		//Get some info on the build up of the array
		TiXmlElement* pTechniqueElem = apSourceElem->FirstChildElement("technique_common");
		if(pTechniqueElem==NULL) pTechniqueElem = apSourceElem->FirstChildElement("technique");
		if(pTechniqueElem==NULL){ Warning("No technique or technique_common element found!\n"); return;}

		//Get some attributes from the accessor
		TiXmlElement* pAccessor = pTechniqueElem->FirstChildElement("accessor");
		if(pAccessor==NULL){ Warning("No accessor element for source data found!\n"); return;}

		int lElements = cString::ToInt(pAccessor->Attribute("stride"),0);
		int lVtxCount = cString::ToInt(pAccessor->Attribute("count"),0);

		//Log("Elems: %d Count: %d\n",lElements,lVtxCount);

		//Load the array
		TiXmlElement* pDataElem = apSourceElem->FirstChildElement("float_array");
		if(pDataElem==NULL)
		{
			//try with array as well.
			pDataElem = apSourceElem->FirstChildElement("array");
			if(pDataElem==NULL){
				Warning("No data found!\n"); return;
			}
		}

		TiXmlText* pTextElem = pDataElem->FirstChild()->ToText();
		if(pTextElem==NULL){ Warning("No text found!\n"); return;}

		const char* pChars = pTextElem->Value();

		FillVertexVec(pChars,avVtxVec,lElements,lVtxCount);
	}

	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::LoadImages(TiXmlElement* apRootElem, tColladaImageVec &avColladaImageVec)
	{
		TiXmlElement* pImageElem = apRootElem->FirstChildElement("image");
		while(pImageElem)
		{
			cColladaImage Image;
			Image.msId = cString::ToString(pImageElem->Attribute("id"),"");
			Image.msName = cString::ToString(pImageElem->Attribute("name"),"");
			
			TiXmlElement* pInitFromElem = pImageElem->FirstChildElement("init_from");
			//COLLADA 1.4	           
			if(pInitFromElem)
			{
				if(pInitFromElem->FirstChild())
				{
					TiXmlText *pText = pInitFromElem->FirstChild()->ToText();
					Image.msSource = cString::ToString(pText->Value(),"");
				}
				else
				{
					Image.msSource = "";
				}
			}
			//COLLADA 1.3
			else
			{
				Image.msSource = cString::ToString(pImageElem->Attribute("source"),"");
			}
			
			avColladaImageVec.push_back(Image);

			pImageElem = pImageElem->NextSiblingElement("image");
		}
	}

	//-----------------------------------------------------------------------
	
	class cEffectNewParam
	{
	public:
		tString msId;
		tString msType;
		tString msData;
	};

	tString* GetFinalSource(std::vector<cEffectNewParam> &avParams,tString& asId)
	{
		for(size_t i=0; i<avParams.size(); ++i)
		{
			if(asId == avParams[i].msId)
			{
				return GetFinalSource(avParams,avParams[i].msData);
			}
		}

		return &asId;
	}

	void cMeshLoaderCollada::LoadTextures(TiXmlElement* apRootElem, tColladaTextureVec &avColladaTextureVec)
	{
		TiXmlElement* pTextureElem = apRootElem->FirstChildElement();
		for(;pTextureElem!=NULL; pTextureElem = pTextureElem->NextSiblingElement())
		{	
			cColladaTexture Texture;

			//Get the main properties of the texture
			Texture.msId = cString::ToString(pTextureElem->Attribute("id"),"");
			
			/////////////////////////////////////////
			//COLLADA 1.4
			TiXmlElement *pProfileCommon = pTextureElem->FirstChildElement("profile_COMMON");
			if(pProfileCommon)
			{
				std::vector<cEffectNewParam> vNewParams;
				//////////////////////////
				//Iterate all newparams
				TiXmlElement *pNewParamElem = pProfileCommon->FirstChildElement("newparam");
				for(; pNewParamElem != NULL; pNewParamElem = pNewParamElem->NextSiblingElement("newparam"))
				{
					vNewParams.push_back(cEffectNewParam());
					cEffectNewParam& newParam = vNewParams.back();
					
					newParam.msId = pNewParamElem->Attribute("sid");

					TiXmlElement *pChildElem = pNewParamElem->FirstChildElement();
					if(pChildElem)
					{
						newParam.msType = pChildElem->Value();
						//Log("Newparam '%s' type '%s'\n",newParam.msId.c_str(),newParam.msType.c_str());
						
						tString sDataName ="";
						if(newParam.msType == "surface")		sDataName = "init_from";
						else if(newParam.msType == "sampler2D")	sDataName = "source";

						if(sDataName == "") continue;
						TiXmlElement *pValueElem = pChildElem->FirstChildElement(sDataName.c_str());
						if(pValueElem)
						{
							TiXmlText *pText = pValueElem->FirstChild()->ToText();
                            newParam.msData = pText->Value();
						}
						else
						{
							Warning("Data element '%s' missing from newparam '%s'\n",sDataName.c_str(),
																				newParam.msId.c_str());
						}
					}

					
				}
				
				//////////////////////////
				//Get the first technique
				TiXmlElement *pTechniqueElem = pProfileCommon->FirstChildElement("technique");
				if(pTechniqueElem==NULL){
					Warning("No effect technique element found!\n");
					continue;
				}

				TiXmlElement *pTypeElem = pTechniqueElem->FirstChildElement();
				if(pTypeElem==NULL){
					Warning("No effect type element found!\n");
					continue;
				}
				if(cString::ToString(pTypeElem->Value(), "") == "extra"){
					pTypeElem = pTypeElem->NextSiblingElement();
					if(pTypeElem==NULL){
						Warning("No effect type element found!\n");
						continue;
					}
				}
				
				///////////////////////
                //Diffuse
                TiXmlElement *pDiffuseElem = pTypeElem->FirstChildElement("diffuse");
				if(pDiffuseElem)
				{
					TiXmlElement *pLocalTexture = pDiffuseElem->FirstChildElement("texture");
                    if(pLocalTexture)
					{
						tString _tstr = cString::ToString(pLocalTexture->Attribute("texture"),"");
						Texture.msImage = *GetFinalSource(vNewParams,_tstr);
					}
					else
					{
						Texture.msImage = "";
						//Warning("No diffuse texture effect element for effect '%s'! No file texture file will be loaded.\n",
						//			Texture.msId.c_str());
					}
				}
				else
				{
					Warning("No diffuse effect element!\n");
				}
			}
			/////////////////////////////////////////
			//COLLADA 1.3
			else
			{
				////////////////////////////
				//Iterate root params
				TiXmlElement* pParamElem = pTextureElem->FirstChildElement("param");
				while(pParamElem)
				{

					pParamElem = pParamElem->NextSiblingElement("param");
				}

				///////////////////////////
				//Iterate techniques
				TiXmlElement* pTechniqueElem = pTextureElem->FirstChildElement("technique");
				while(pTechniqueElem)
				{
					tString sProfile = cString::ToString(pTechniqueElem->Attribute("profile"),"");

					////////////////////////////
					//Technique params:
					TiXmlElement* pTechParam = pTechniqueElem->FirstChildElement("param");
					while(pTechParam)
					{
						pTechParam = pTechParam->NextSiblingElement("param");
					}

					///////////////////////////////
					//Technique inputs:
					TiXmlElement* pTechInput = pTechniqueElem->FirstChildElement("input");
					while(pTechInput)
					{
						tString sSemantic = cString::ToString(pTechInput->Attribute("semantic"),"");

						//Get the image of the texture
						if(sSemantic == "IMAGE")
						{
							Texture.msImage = cString::ToString(pTechInput->Attribute("source"),"");
						}

						pTechInput = pTechInput->NextSiblingElement("input");
					}

					pTechniqueElem = pTechniqueElem->NextSiblingElement("technique");
				}
			}


			//Log("Texture: id: '%s' image: '%s'\n",Texture.msId.c_str(), Texture.msImage.c_str());

			avColladaTextureVec.push_back(Texture);
		}
	}
	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::LoadMaterials(TiXmlElement* apRootElem, tColladaMaterialVec &avColladaMaterialVec)
	{
		TiXmlElement* pMaterialElem = apRootElem->FirstChildElement("material");
		while(pMaterialElem)
		{
			cColladaMaterial Material;
			Material.msId = cString::ToString(pMaterialElem->Attribute("id"),"");
			Material.msName = cString::ToString(pMaterialElem->Attribute("name"),"");

			///////////////////////////////////////////
			// COLLADA 1.4
			TiXmlElement *pIstanceEffectElem = pMaterialElem->FirstChildElement("instance_effect");
			if(pIstanceEffectElem)
			{
				Material.msTexture = cString::ToString(pIstanceEffectElem->Attribute("url"),"");
			}
			///////////////////////////////////////////
			// COLLADA 1.3
			else
			{
				//The rest of the material loader is gonna be a little lame
				//Just gonna look for a texture.

				TiXmlElement* pShaderElem = pMaterialElem->FirstChildElement("shader");
				if(pShaderElem==NULL){ Warning("No shader found!\n"); continue;}

				TiXmlElement* pTechElem = pShaderElem->FirstChildElement("technique");
				if(pTechElem==NULL){ Warning("No technique found!\n"); continue;}

				TiXmlElement* pPassElem = pTechElem->FirstChildElement("pass");
				if(pPassElem==NULL){ Warning("No pass found!\n"); continue;}

				//Iterate through the inputs and try to find a texture
				TiXmlElement* pInputElem = pPassElem->FirstChildElement("input");
				while(pInputElem)
				{
					tString sSemantic =  cString::ToString(pInputElem->Attribute("semantic"),"");

					if(sSemantic == "TEXTURE")
					{
						Material.msTexture = cString::ToString(pInputElem->Attribute("source"),"");
					}

					pInputElem = pInputElem->NextSiblingElement("input");
				}
			}

			//Log("Material: id: '%s' name: '%s' texture: '%s'\n",Material.msId.c_str(), 
			//											Material.msName.c_str(),
			//											Material.msTexture.c_str());

			//Add to vector
			avColladaMaterialVec.push_back(Material);

			//Next material
			pMaterialElem = pMaterialElem->NextSiblingElement("material");
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// HELPER METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static cVertex IndexDataToVertex(const cColladaVtxIndex &aData, const cColladaGeometry &aGeometry)
	{
		cVertex Vtx;

		Vtx.col = cColor(1,1);
		Vtx.pos = aGeometry.mvArrayVec[aGeometry.mlPosArrayIdx].mvArray[aData.mlVtx];
		if(aGeometry.mlNormArrayIdx>=0)
			Vtx.norm = aGeometry.mvArrayVec[aGeometry.mlNormArrayIdx].mvArray[aData.mlNorm];
		if(aGeometry.mlTexArrayIdx>=0)
			Vtx.tex = aGeometry.mvArrayVec[aGeometry.mlTexArrayIdx].mvArray[aData.mlTex];

		//Flip the y coord on the tex
		Vtx.tex.y = 1 - Vtx.tex.y;

		return Vtx;
	}

	static cColladaExtraVtx IndexDataToExtra(const cColladaVtxIndex &aData, int alNewVtx)
	{
		cColladaExtraVtx Extra;
		Extra.mlNewVtx = alNewVtx;
		Extra.mlVtx = aData.mlVtx;
		Extra.mlNorm = aData.mlNorm;
		Extra.mlTex = aData.mlTex;
		return Extra;
	}


	void cMeshLoaderCollada::SplitVertices(cColladaGeometry &aGeometry,tColladaExtraVtxListVec &avExtraVtxVec, 
		tVertexVec &avVertexVec, tUIntVec &avIndexVec)
	{
		//Resize the extra array and the vertex array
		int lVtxSize = (int)aGeometry.mvArrayVec[aGeometry.mlPosArrayIdx].mvArray.size();
		avExtraVtxVec.resize(lVtxSize);
		avVertexVec.resize(lVtxSize);
		avIndexVec.resize(aGeometry.mvIndices.size());

		tColladaVtxIndexVec &vIndices = aGeometry.mvIndices;

		for(int i=0; i<(int) vIndices.size(); i++)
		{
			cColladaVtxIndex &Data = vIndices[i];
			//Log("Index %d: ", i);

			//If the vertex extra is empty this is the first of
			//this vertex added, so no need to split.
			if(avExtraVtxVec[Data.mlVtx].empty())
			{
				avExtraVtxVec[Data.mlVtx].push_back(IndexDataToExtra(Data, Data.mlVtx));

				avVertexVec[Data.mlVtx] = IndexDataToVertex(Data,aGeometry);
				avIndexVec[i] = Data.mlVtx;
				//Log("New vertex added");
			}
			//There is already a vertex added at this position, check if this exist
			//else create a new.
			else
			{
				bool bFound = false;
				tColladaExtraVtxListIt it = avExtraVtxVec[Data.mlVtx].begin();
				for(; it != avExtraVtxVec[Data.mlVtx].end(); ++it)
				{
					//If the split has already been made for this combo just set the vertex position.
					if(it->Equals(Data))
					{
						avIndexVec[i] = it->mlNewVtx;
						bFound = true;
						//Log("Old vertex split used");
						break;
					}
				}

				//If no split was found create a new
				if(bFound == false)
				{
					//The new vertex will be put at the end.
					avExtraVtxVec[Data.mlVtx].push_back(IndexDataToExtra(Data, (int)avVertexVec.size()));
					avIndexVec[i] = (int)avVertexVec.size();

					avVertexVec.push_back(IndexDataToVertex(Data,aGeometry));
					//Log("New split made");
				}
			}

			//Log("\n");
		}
	}


	//-----------------------------------------------------------------------

	tString cMeshLoaderCollada::GetTopString(const tString asPath)
	{
		int pos = cString::GetLastStringPos(asPath,"-");

		if(pos<0) return "";

		return asPath.substr(pos+1);
	}

	//-----------------------------------------------------------------------

	void cMeshLoaderCollada::FillVertexVec(const char* apChars,tVector3fVec &avVtxVec, int alElements, int alVtxCount)
	{
		if((int)avVtxVec.size() < alVtxCount) avVtxVec.resize(alVtxCount);

		//This is a bit slower but shouldn't matter since we have cache files.
		//Also, the biggie is probably string -> float, which is no different really. Just some extra copying to arrays happen.
		//Plus side is that it is a lot safer!
		tFloatVec vFloatData;
		cString::GetFloatVec(apChars, vFloatData);
		
		int lLoadedVtxNum = (int)vFloatData.size() / alElements;
		if(lLoadedVtxNum != alVtxCount)
		{
			Warning("Vertex array in does not have correct number of values stored. %d instead of %d\n", vFloatData.size(), alElements * alVtxCount);
		}
		
		for(int i=0; i<alVtxCount; ++i)
		{
			if(i>=lLoadedVtxNum) break;

			float* fData = &vFloatData[i*alElements];

			avVtxVec[i].x = fData[0];
			if(alElements > 1)
			{
				avVtxVec[i].y = fData[1];
				if(alElements > 2)
					avVtxVec[i].z = fData[2];
			}
		}

		/*
		int lArraySize = alElements * alVtxCount;

		int lArrayCount=0;
		int lStringCount =0;

		float vValArray[3];
		int lValArrayCount=0;
		char vTempChar[256];
		int lTempCharCount=0;

		int lVertexNum =0;

		while(lArrayCount < lArraySize)
		{
			char c= apChars[lStringCount];
			//if a space is found, convert the previous characters to a float.
			if(c == ' ' || c == 0 || c== '\n' || c=='\t')
			{
				if(lTempCharCount>0)
				{
					//Add the float to temp float array
					vTempChar[lTempCharCount] =0;
					vValArray[lValArrayCount] = (float)atof(vTempChar);

					lTempCharCount=0;
					lArrayCount++;

					//If enough values have been added put in vector and add to array.
					lValArrayCount++;
					if(lValArrayCount == alElements)
					{
						lValArrayCount =0;
						cVector3f vVec;
						vVec.x = vValArray[0];vVec.y = vValArray[1];
						if(alElements==3) vVec.z = vValArray[2];
						if (lVertexNum >= alVtxCount) {
							Error("Tried to access beyond end of data structure (%d)\n", alVtxCount);
							return;
						}
						avVtxVec[lVertexNum] = vVec;
						lVertexNum++;
						//Log("%s\n", vVec.ToString().c_str());
					}
				}
			}
			//If character is not a space, add to temp char.
			else
			{
				vTempChar[lTempCharCount] = apChars[lStringCount];
				lTempCharCount++;
			}

			lStringCount++;
		}*/
	}

	//-----------------------------------------------------------------------

	tString cMeshLoaderCollada::GetMaterialTextureFile(const tString &asMaterial, 
		tColladaMaterialVec &avColladaMaterialVec,
		tColladaTextureVec &avColladaTextureVec,
		tColladaImageVec &avColladaImageVec)
	{
		if(asMaterial == "") return "";

		//Get the texture Id
		tString sTexId ="";
		for(size_t mat=0; mat < avColladaMaterialVec.size(); mat++)
		{
			if(avColladaMaterialVec[mat].msId == asMaterial)
			{
				sTexId = avColladaMaterialVec[mat].msTexture;
				GetAdress(sTexId);
				break;
			}
		}

		if(sTexId == "")
		{
			Warning("Material '%s' was not found!\n",asMaterial.c_str());
			return "";
		}

		//Get file Id
		tString sFileId="";
		for(size_t tex=0; tex < avColladaTextureVec.size(); tex++)
		{
			if(avColladaTextureVec[tex].msId == sTexId)
			{
				sFileId = avColladaTextureVec[tex].msImage;
				GetAdress(sFileId);
				break;
			}
		}

		if(sFileId == "")
		{
			Warning("Texture '%s' was not found!\n",sTexId.c_str());
			return "";
		}

		//Get file source
		for(size_t img=0; img < avColladaImageVec.size(); img++)
		{
			if(avColladaImageVec[img].msId == sFileId)
			{
				return cString::GetFileName(avColladaImageVec[img].msSource);
			}
		}

		Warning("Couldn't file image file id '%s'\n",sFileId.c_str());

		return "";
	}

	//-----------------------------------------------------------------------
	
	/*
	OLD SKINNING DATA
	void cMeshLoaderCollada::LoadControllers(TiXmlElement* apRootElem,
	tColladaControllerVec &avColladaControllerVec,
	tColladaGeometryVec &avColladaGeometryVec)
	{
	TiXmlElement* pCtrlElem = apRootElem->FirstChildElement("controller");
	for(;pCtrlElem!=NULL; pCtrlElem = pCtrlElem->NextSiblingElement("controller"))
	{
	avColladaControllerVec.push_back(cColladaController());

	cColladaController &Controller = avColladaControllerVec[avColladaControllerVec.size()-1];

	tString sTarget =cString::ToString(pCtrlElem->Attribute("target"),"");
	tString sId = cString::ToString(pCtrlElem->Attribute("id"),"");

	Controller.msId = sId;

	///////////////////////////////////////
	// Get Skin element.
	TiXmlElement *pSkinElem = pCtrlElem->FirstChildElement("skin");
	if(pSkinElem==NULL){ Error("No Skin found in controller!\n"); continue;}

	//1.4 support, check skin for source if not found.
	if(sTarget == ""){
	sTarget = cString::ToString(pSkinElem->Attribute("source"),"");
	GetAdress(sTarget);
	}

	Controller.msTarget = sTarget;


	////////////////////////////////////////
	// Find the geometry for the controller.
	cColladaGeometry* pGeom =NULL;
	for(int i=0;i<(int)avColladaGeometryVec.size();i++)
	{
	if(sTarget == avColladaGeometryVec[i].msId)
	{
	if(avColladaGeometryVec[i].mvIndices.empty())
	{
	Error("Target geometry is empty!\n");
	break;
	}

	pGeom = &avColladaGeometryVec[i];
	break;
	}
	}
	if(pGeom==NULL){
	Error("Target '%s' for Controller '%s' couldn't be found\n",sTarget.c_str(),sId.c_str());
	continue;
	}



	////////////////////////////////////////
	// Get the types of sources used in the controller. 
	tString sNormalSourceId;
	tString sPositionSourceId;
	tString sJointSourceId;

	TiXmlElement *pVerticesElem = pSkinElem->FirstChildElement("vertices");
	if(pVerticesElem==NULL){ Error("No vertices element!\n"); continue;}

	TiXmlElement *pVtxInput = pVerticesElem->FirstChildElement("input");
	while(pVtxInput)
	{
	tString sSemantic = cString::ToString(pVtxInput->Attribute("semantic"),"");
	tString sSource = cString::ToString(pVtxInput->Attribute("source"),"");
	GetAdress(sSource);

	//Log("Vertex: %s Sem: %s\n",sSource.c_str(), sSemantic.c_str());

	//Check the semantic and set the strings to point to right source.
	if(sSemantic == "BIND_SHAPE_POSITION")
	{
	sPositionSourceId = sSource;
	}
	else if(sSemantic == "BIND_SHAPE_NORMAL")
	{
	sNormalSourceId = sSource;
	}
	else if(sSemantic == "JOINTS_AND_WEIGHTS")
	{
	sJointSourceId = sSource;
	}

	pVtxInput = pVtxInput->NextSiblingElement("input");
	}

	////////////////////////////////////////
	// Get the sources and load / apply them

	TiXmlElement *pSourceElem = pSkinElem->FirstChildElement("source");
	while(pSourceElem)
	{
	tString sId = cString::ToString(pSourceElem->Attribute("id"),"");

	//Log("Loading controller source %s\n",sId.c_str());

	//new vectors for positions
	if(sId == sPositionSourceId)
	{
	if(pGeom->mlPosArrayIdx >= 0)
	{
	LoadVertexData(pSourceElem, pGeom->mvArrayVec[pGeom->mlPosArrayIdx].mvArray);
	}
	}
	//new vectors for normals
	else if(sId == sNormalSourceId)
	{
	if(pGeom->mlNormArrayIdx >= 0)
	LoadVertexData(pSourceElem, pGeom->mvArrayVec[pGeom->mlNormArrayIdx].mvArray);
	}
	//data for joints
	else if(sId == sJointSourceId)
	{
	LoadJointData(pSourceElem, Controller);
	}

	pSourceElem = pSourceElem->NextSiblingElement("source");
	}
	}
	}

	//////////////////////////////////////

	void cMeshLoaderCollada::LoadJointData(TiXmlElement* apSourceElem, cColladaController &aController)
	{
	tString sJointArrayName ="";
	tString sMatrixArrayName ="";
	tString sWeightArrayName ="";

	/////////////////////////////////
	//Get technique element
	TiXmlElement *pTechniqueElem = apSourceElem->FirstChildElement("technique");
	if(pTechniqueElem==NULL){ Warning("No technique element found!\n"); return;}


	/////////////////////////////////
	//Get the names of joint, matrix and weight array.
	TiXmlElement *pAccessorElem = pTechniqueElem->FirstChildElement("accessor");
	while(pAccessorElem)
	{
	tString sSource = cString::ToString(pAccessorElem->Attribute("source"),"");
	GetAdress(sSource);

	TiXmlElement *pParamElem = pAccessorElem->FirstChildElement("param");
	if(pParamElem==NULL){ Warning("Accessor contains no param!\n"); return;}

	tString sParamName = cString::ToString(pParamElem->Attribute("name"),"");

	if(sParamName == "JOINT")
	{
	sJointArrayName = sSource;
	}
	else if(sParamName == "INV_BIND_MATRIX")
	{
	sMatrixArrayName = sSource;
	}
	else if(sParamName == "WEIGHT")
	{
	sWeightArrayName = sSource;
	}

	pAccessorElem = pAccessorElem->NextSiblingElement("accessor");
	}

	//Log("Joint: %s Matrix: %s Weight: %s\n",sJointArrayName.c_str(),
	//	sMatrixArrayName.c_str(),
	//	sWeightArrayName.c_str());

	/////////////////////////////////
	// Get the pairs
	TiXmlElement *pCombinerElem = pTechniqueElem->FirstChildElement("combiner");
	if(pCombinerElem == NULL){ Error("No combiner found!\n"); return;}

	int lPairCount = cString::ToInt(pCombinerElem->Attribute("count"),0);

	aController.mvPairs.resize(lPairCount);
	//Log("Pair Count: %d\n",lPairCount);

	//Check which element is joint and which is weight.
	TiXmlElement *pCombInput = pCombinerElem->FirstChildElement("input");
	while(pCombInput)
	{
	tString sSemantic = cString::ToString(pCombInput->Attribute("semantic"),"");
	int lIdx = cString::ToInt(pCombInput->Attribute("idx"),0);

	if(sSemantic == "JOINT")
	{
	aController.mlJointPairIdx = lIdx;
	//Log("Joint has idx %d\n",lIdx);
	}
	else if(sSemantic == "WEIGHT")
	{
	aController.mlWeightPairIdx = lIdx;
	//Log("Weight has idx %d\n",lIdx);
	}
	else
	{
	Warning("Unknown semantic '%s' found in combiner input\n", sSemantic);
	}

	pCombInput = pCombInput->NextSiblingElement("input");
	}

	// Load the data for the pairs
	int lPairIdx =0;
	TiXmlElement *pCombV = pCombinerElem->FirstChildElement("v");
	while(pCombV)
	{
	TiXmlText *pText = pCombV->FirstChild()->ToText();
	if(pText==NULL){ Error("No index data found!\n"); return;}

	//Get the indices joints and weights
	tIntVec vIndexArray;
	cString::GetIntVec(pText->Value(),vIndexArray);

	//Add each pair in a list at each vertex
	int lNumOfPairs = (int) vIndexArray.size() /2;
	for(int i=0;i<lNumOfPairs;i++)
	{
	cColladaJointPair Pair;

	Pair.mlJoint = vIndexArray[i*2 + aController.mlJointPairIdx];
	Pair.mlWeight = vIndexArray[i*2 + aController.mlWeightPairIdx];

	//Log("Pair: %d, %d\n",Pair.mlJoint, Pair.mlWeight);

	aController.mvPairs[lPairIdx].push_back(Pair);
	}


	lPairIdx++;
	pCombV = pCombV->NextSiblingElement("v");
	}


	/////////////////////////////////
	// Get the arrays

	////////////////////////////////
	//Get the names of the joints:
	TiXmlElement *pNameArrayElem = apSourceElem->FirstChildElement("Name_array");
	if(pNameArrayElem==NULL){ Error("No name array found!\n"); return;}

	tString sNameId = cString::ToString(pNameArrayElem->Attribute("id"),"");
	int lNameCount = cString::ToInt(pNameArrayElem->Attribute("count"),0);

	if(sNameId != sJointArrayName){
	Error("Name array and joint array is not the same!\n");
	return;
	}

	TiXmlText *pNameText = pNameArrayElem->FirstChild()->ToText();
	if(pNameText==NULL){ Error("No joint name data found!\n"); return;}

	aController.mvJoints.reserve(lNameCount);
	cString::GetStringVec(pNameText->Value(), aController.mvJoints);


	//////////////////////////////
	// Get weights and matrices
	TiXmlElement *pFloatArrayElem = apSourceElem->FirstChildElement("float_array");
	while(pFloatArrayElem)
	{
	tString sId = cString::ToString(pFloatArrayElem->Attribute("id"),"");
	int lCount = cString::ToInt(pFloatArrayElem->Attribute("count"),0);

	TiXmlText *pText = pFloatArrayElem->FirstChild()->ToText();
	if(pText==NULL){ Error("No value data found!\n"); return;}

	tFloatVec vValVec;
	vValVec.reserve(lCount);
	cString::GetFloatVec(pText->Value(), vValVec);

	//Get the matrix arrray
	if(sId == sMatrixArrayName)
	{
	aController.mvMatrices.reserve(lCount / 16);

	for(int i=0; i< (lCount / 16); i++)
	{
	cMatrixf mtxTemp(&vValVec[i*16]);

	aController.mvMatrices.push_back(mtxTemp);

	//Log("Mtx: %s\n",cMath::MatrixToChar(mtxTemp));
	}
	}
	//Get the weight array
	else if(sId == sWeightArrayName)
	{
	aController.mvWeights = vValVec;

	//for(int i=0; i <(int)aController.mvWeights.size();i++)
	//{
	//Log("Weight: %f\n",Controller.mvWeights[i]);
	//}
	}

	pFloatArrayElem = pFloatArrayElem->NextSiblingElement("float_array");
	}
	}*/

}
