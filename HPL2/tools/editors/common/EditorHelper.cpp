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

#include "EditorHelper.h"

#include "EditorBaseClasses.h"
#include "EntityWrapper.h"
#include "EditorWorld.h"
#include "EditorWindowViewport.h"


iEditorBase* cEditorHelper::mpEditor = NULL;

//----------------------------------------------------------------------------------

cVector2f cEditorHelper::GetInvYMousePos(const cVector2f& avMousePos, const cVector2f& avViewportSize)
{
	///////////////////////////////////////////////
	//Invert mouse y to make it usable in the screenspace picking algo
	cVector2f vMousePos = avMousePos;
	vMousePos.y = avViewportSize.y - vMousePos.y;

	return vMousePos;
}

bool cEditorHelper::CheckEntityAgainstRay(iEntityWrapper* apEntity, iEntityWrapper* apCurrentPickedEntity,
											const cVector3f& avRayBVMin, const cVector3f& avRayBVMax, 
											const cVector3f& avRayStart, const cVector3f& avRayEnd, 
											float* apfMinDistance, cVector3f* apIntersectionPos, tVector3fVec* apTriangle)
{
	return false;
	/*
	if(apEntity == NULL ||
	   apfMinDistance == NULL)
		return false;

	/////////////////////////////////////////
	// Check Object BV against ray BV
	if(apEntity->CheckInsideBoundingVolume(avRayBVMin, avRayBVMax)==false)
		return false;

	cVector3f vIntersectionPos;
	tVector3fVec vTriangleHit;

	///////////////////////////////////////////////////////////////
	// Check Object BV against ray, and check accurate collision too
	if(apEntity->CheckRayBoundingVolumeIntersection(avRayStart, avRayEnd, &vIntersectionPos)==false ||
		apEntity->CheckRayAccurateIntersection(avRayStart,avRayEnd, &vIntersectionPos, &vTriangleHit)==false)
		return false;
	
	// Get distance to intersection. If distance is smaller than previous, set as picked.
	// Also do this if entity has higher priority
	bool bEntityHasHigherPrio = false;
	if(apCurrentPickedEntity)
		bEntityHasHigherPrio = (apEntity->GetPickPrio()>apCurrentPickedEntity->GetPickPrio());

	float fIntersectionDist = cMath::Vector3DistSqr(avRayStart, vIntersectionPos);
	if(bEntityHasHigherPrio && fIntersectionDist<*apfMinDistance+0.5f || fIntersectionDist<*apfMinDistance)
	{
		*apfMinDistance = fIntersectionDist;
		if(apIntersectionPos)
			*apIntersectionPos = vIntersectionPos;
		if(apTriangle)
			*apTriangle = vTriangleHit;
		return true;
	}

	return false;*/
}

//----------------------------------------------------------------------

bool cEditorHelper::CheckEntityAgainstBox(iEntityWrapper* apEntity, 
											const cRect2l& aMouseRect, const cVector2l& avViewportSize, 
											cFrustum* apFrustum)
{
	/*
	if(apEntity==NULL)
		return false;

	cBoundingVolume* pBV = apEntity->GetPickBV(apEntity->GetEditorWorld()->GetEditor()->GetFocusedViewport()->GetCamera());		

	if(apFrustum->CollideBoundingVolume(pBV)!=eCollision_Outside)
	{
		cRect2l entClipRect;
		cMath::GetClipRectFromBV(entClipRect,*pBV,apFrustum,avViewportSize, apFrustum->GetFOV()*0.5f);
		
		return cMath::CheckRectIntersection(entClipRect,aMouseRect);
	}
*/
	return false;
}

//----------------------------------------------------------------------

void cEditorHelper::CalculateRayBoundingBox(const cVector3f& avStart, const cVector3f& avEnd, cVector3f &avMin, cVector3f &avMax)
{
	avMin = avEnd;
	avMax = avStart;

	if(avStart.x<avEnd.x)
	{
		avMin.x = avStart.x;
		avMax.x = avEnd.x;
	}
	if(avStart.y<avEnd.y)
	{
		avMin.y = avStart.y;
		avMax.y = avEnd.y;
	}
	if(avStart.z<avEnd.z)
	{
		avMin.z = avStart.z;
		avMax.z = avEnd.z;
	}
}


bool cEditorHelper::CheckScreenSpaceMouseMeshIntersect(cCamera* apCamera, const cVector2f& avMousePos,  
													   const cVector3f& avRayStart, const cVector3f& avRayEnd,
													   const cVector2f& avViewportSize, cMeshEntity* apMesh,
													   cVector3f* apIntersectionPos, tVector3fVec* apTriangle)
{
	if(apMesh==NULL)
		return false;

	bool bIntersect = false;

	//////////////////////////
	// Get Matrices
	cMatrixf mtxCameraViewMatrix = apCamera->GetViewMatrix();
	cMatrixf mtxCameraProjMatrix = apCamera->GetProjectionMatrix();
	
	cVector3f vMeshIntersectPos;
	cVector3f vOldMeshIntersectPos;
	float fOldDistance = 999999999.0f;

	//For each submesh
	for(int i=0;i<apMesh->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMesh = apMesh->GetSubMeshEntity(i);
		cMatrixf mtxSubMeshWorldMatrix = pSubMesh->GetWorldMatrix();

		bIntersect = bIntersect || CheckScreenSpaceMouseSubMeshIntersect(GetInvYMousePos(avMousePos, avViewportSize), 
																		 avRayStart, 
																		 avRayEnd, 
																		 avViewportSize,
																		 pSubMesh,
																		 mtxCameraViewMatrix,
																		 mtxCameraProjMatrix,
																		 mtxSubMeshWorldMatrix,
																		 &vMeshIntersectPos,
																		 apTriangle);

		if(bIntersect)
		{
			if(cMath::Vector3DistSqr(avRayStart,vMeshIntersectPos) < fOldDistance)
			{
				vOldMeshIntersectPos = vMeshIntersectPos;
				fOldDistance = cMath::Vector3DistSqr(avRayStart,vOldMeshIntersectPos);
			}
		}
	}
	
	if(bIntersect && apIntersectionPos)
		*apIntersectionPos = vMeshIntersectPos;

	return bIntersect;
}

//----------------------------------------------------------------------------------

bool cEditorHelper::CheckScreenSpaceMouseSubMeshIntersect(const cVector2f& avInvYMousePos,
														  const cVector3f& avRayStart, const cVector3f& avRayEnd,
                                                          const cVector2f& avViewportSize, cSubMeshEntity* apSubMesh,
														  const cMatrixf& amtxCameraViewMatrix, 
														  const cMatrixf& amtxCameraProjMatrix,
														  const cMatrixf& amtxSubMeshWorldMatrix,
														  cVector3f* apIntersectionPos,
														  tVector3fVec* apTriangle)
{
	if(apSubMesh==NULL)
		return false;

	bool bIntersect;
	cVector3f vSubMeshIntersectPos;

	//Get Vertex Buffer
	iVertexBuffer* pVtxBuffer = apSubMesh->GetVertexBuffer();

	bIntersect = CheckScreenSpaceMouseVertexBufferIntersect(avInvYMousePos, 
															avRayStart, avRayEnd, 
															avViewportSize, pVtxBuffer,
															amtxCameraViewMatrix,
															amtxCameraProjMatrix,
															amtxSubMeshWorldMatrix,
															&vSubMeshIntersectPos,
															apTriangle);

	if(bIntersect && apIntersectionPos)
		*apIntersectionPos = vSubMeshIntersectPos;

	return bIntersect;
}

//----------------------------------------------------------------------------------

bool cEditorHelper::CheckScreenSpaceMouseVertexBufferIntersect(const cVector2f& avInvYMousePos,
														   const cVector3f& avRayStart, const cVector3f& avRayEnd,
														   const cVector2f& avViewportSize, iVertexBuffer* apVtxBuffer,
														   const cMatrixf& amtxCameraViewMatrix,
														   const cMatrixf& amtxCameraProjMatrix,
														   const cMatrixf& amtxVtxBufferWorldMatrix,
														   cVector3f* apIntersectionPos,
														   tVector3fVec* apTriangle)
{
	if(apVtxBuffer==NULL)
		return false;

	bool bIntersect = false;

	cVector3f vVtxBufferIntersectionPos;

	int lIndexNum = apVtxBuffer->GetIndexNum();
	unsigned int* pIndices = apVtxBuffer->GetIndices();
	float* pVertices = apVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
	int lStride = apVtxBuffer->GetElementNum(eVertexBufferElement_Position);

	tVector3fVec vTriangle;

	//Check if mouse position intersects a triangle in screen space
	if(cMath::CheckPointInFlatPolygon(avRayStart, avRayEnd,
									  avInvYMousePos,avViewportSize,
									  lIndexNum,pIndices,pVertices,lStride,
									  amtxVtxBufferWorldMatrix,
									  amtxCameraViewMatrix,amtxCameraProjMatrix,
									  NULL, &vVtxBufferIntersectionPos, &vTriangle))
	{
		bIntersect = true;

		if(apIntersectionPos)
			*apIntersectionPos = vVtxBufferIntersectionPos;
		if(apTriangle)
			*apTriangle = vTriangle;
	}

	return bIntersect;
}

//----------------------------------------------------------------------------------

bool cEditorHelper::CheckRayMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd,
												  cMeshEntity* apObject, cVector3f* apIntersectionPos, tVector3fVec* apTriangle, float* apT)
{
	float fMinT = 1000.0f;
	cVector3f vClosestIntersection;
	int lHitSubMeshIndex = -1;
	int lHitTriIndex = -1;
	tVector3fVec vHitTriangle;
	bool bIntersected = false;

	for(int i=0;i<apObject->GetSubMeshEntityNum();++i)
	{
		float fT;
		cVector3f vIntersection;
		unsigned int lTriIndex;
		cSubMeshEntity* pSubMesh = apObject->GetSubMeshEntity(i);

		if(CheckRaySubMeshEntityIntersect(avRayStart, avRayEnd, pSubMesh, &vIntersection, &fT, &lTriIndex))
		{
			bIntersected = true;
			if(fT<fMinT)
			{
				fMinT = fT;
				lHitSubMeshIndex = i;
				lHitTriIndex = lTriIndex;
				vClosestIntersection = vIntersection;
			}
		}
	}

	if(bIntersected)
	{
		if(apIntersectionPos)
			*apIntersectionPos = vClosestIntersection;
		if(apTriangle)
		{
			apTriangle->clear();
			cSubMeshEntity* pSubMesh = apObject->GetSubMeshEntity(lHitSubMeshIndex);
			float* pVertices = pSubMesh->GetSubMesh()->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Position);
			int lStride = pSubMesh->GetSubMesh()->GetVertexBuffer()->GetElementNum(eVertexBufferElement_Position);
			unsigned int* pIndices = pSubMesh->GetSubMesh()->GetVertexBuffer()->GetIndices();		
			for(int i=0;i<3;++i)
			{
				int lBaseIndex = pIndices[lHitTriIndex+i]*lStride;
				apTriangle->push_back(cMath::MatrixMul(*pSubMesh->GetModelMatrix(NULL),cVector3f(pVertices[lBaseIndex],pVertices[lBaseIndex+1], pVertices[lBaseIndex+2])));
			}
		}
		if(apT)
			*apT = fMinT;
	}

	return bIntersected;
}

//----------------------------------------------------------------------------------

bool cEditorHelper::CheckRaySubMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd, cSubMeshEntity* apObject, cVector3f* apIntersection, float *apT,unsigned int* apTriangleIdx, tVector3fVec* apTriangle)
{
	iVertexBuffer* pVB = apObject->GetSubMesh()->GetVertexBuffer();
	cMatrixf mtxInvWorld = cMath::MatrixInverse(apObject->GetWorldMatrix());
	int lTriIndex = -1;

	if(cMath::CheckLineTriVertexBufferIntersection(avRayStart, avRayEnd, mtxInvWorld, pVB, apIntersection, apT, &lTriIndex))
	{
		if(apTriangleIdx)
			*apTriangleIdx = lTriIndex;
		if(apTriangle)
		{
			apTriangle->clear();
			float* pVertices = apObject->GetSubMesh()->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Position);
			int lStride = apObject->GetSubMesh()->GetVertexBuffer()->GetElementNum(eVertexBufferElement_Position);
			unsigned int* pIndices = apObject->GetSubMesh()->GetVertexBuffer()->GetIndices();		
			for(int i=0;i<3;++i)
			{
				int lBaseIndex = pIndices[lTriIndex+i]*lStride;
				apTriangle->push_back(cMath::MatrixMul(apObject->GetWorldMatrix(),
														cVector3f(pVertices[lBaseIndex],pVertices[lBaseIndex+1], pVertices[lBaseIndex+2])));
			}
		}

		return true;
	}

	return false;
}

void cEditorHelper::GetTrianglesInsideClipPlanes(std::vector<cPlanef>& avPlanes, tEntityWrapperList& alstEntities, const cVector3f avBaseNormal, tVector3fVec& avTriangles, tVector3fVec& avNormals)
{
	/*
	avTriangles.clear();
	avNormals.clear();
	tEntityWrapperListIt it = alstEntities.begin();
	for(;it!=alstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cMeshEntity* pMeshEnt = (cMeshEntity*)pEnt->GetMeshEntity();
		for(int i=0;i<pMeshEnt->GetSubMeshEntityNum();++i)
		{
			cSubMeshEntity* pSubMeshEnt = pMeshEnt->GetSubMeshEntity(i);
			const cMatrixf& mtxWorldMatrix = pSubMeshEnt->GetWorldMatrix();
			iVertexBuffer* pVBCopy = pSubMeshEnt->GetSubMesh()->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Static, eVertexElementFlag_Position|eVertexElementFlag_Normal);
			pVBCopy->Transform(mtxWorldMatrix);
			float* pPositions = pVBCopy->GetFloatArray(eVertexBufferElement_Position);
			float* pNormals = pVBCopy->GetFloatArray(eVertexBufferElement_Normal);
			unsigned int* pIndices = pVBCopy->GetIndices();
			for(int j=0;j<pVBCopy->GetIndexNum();j+=3)
			{
				tVector3fVec vTriangle;
				tVector3fVec vNormal;

				bool bAddTriangle = true;
				int lNegNormalCount = 0;
				for(int k=0;k<3;++k)
				{
					unsigned int pIndexBase = pIndices[j+k];
					unsigned int pPosIndex = pIndexBase*pVBCopy->GetElementNum(eVertexBufferElement_Position);
					unsigned int pNrmIndex = pIndexBase*pVBCopy->GetElementNum(eVertexBufferElement_Normal);
					vTriangle.push_back(cVector3f(pPositions[pPosIndex],
													pPositions[pPosIndex+1],
													pPositions[pPosIndex+2]));
					cVector3f vVertexNormal = cVector3f(pNormals[pNrmIndex],
													pNormals[pNrmIndex+1],
													pNormals[pNrmIndex+2]);
					vNormal.push_back(vVertexNormal);
				}

				cVector3f vAvgNormal = (vNormal[0] + vNormal[1] + vNormal[2])/3.0f;
				if(cMath::Vector3Dot(vAvgNormal, avBaseNormal)<0)
					bAddTriangle = false;

				int lEdgeFails=0;
				for(int k=0;k<3;++k)
				{
					if(bAddTriangle==false)
						break;

					cVector3f& vVertex1 = vTriangle[k];
					cVector3f& vVertex2 = vTriangle[(k+1)%3];

					cVector3f vIntersection;
					
					for(int l=0;l<(int)avPlanes.size();++l)
					{
						cPlanef& plane = avPlanes[l];
						if(cMath::CheckPlaneLineIntersection(plane, vVertex1, vVertex2, &vIntersection, NULL)==false)
						{
							if(cMath::PlaneToPointDist(plane, vVertex1)<0 &&
								cMath::PlaneToPointDist(plane, vVertex2)<0)
							{
								++lEdgeFails;
							}
						}
					}
				}
				
				if(bAddTriangle && lEdgeFails<3)
				{
					avTriangles.push_back(vTriangle[0]);
					avTriangles.push_back(vTriangle[1]);
					avTriangles.push_back(vTriangle[2]);
					avNormals.push_back(vNormal[0]);
					avNormals.push_back(vNormal[1]);
					avNormals.push_back(vNormal[2]);
				}

			}
			hplDelete(pVBCopy);
		}
	}
	*/
}

void cEditorHelper::GetTrianglesIntersectingSphere(const cVector3f& avSphereCenter, float afRadius, tSubMeshEntityVec& avSubMeshes, const cVector3f& avBaseNormal, 
												   tVector3fVec& avTriangles, tVector3fVec& avNormals)
{
	cBoundingVolume bv;
	bv.SetPosition(avSphereCenter);
	bv.SetSize(afRadius*2);

	avTriangles.clear();
	avNormals.clear();
	float fInv3 = 1.0f/3.0f;
	tSubMeshEntityVecIt it = avSubMeshes.begin();
	for(;it!=avSubMeshes.end();++it)
	{
		cSubMeshEntity* pSubMeshEnt = *it;
		cBoundingVolume *pObjectBV = pSubMeshEnt->GetBoundingVolume();

		if(cMath::CheckBVIntersection(*pObjectBV, bv)==false) continue;		

		cMatrixf mtxInvWorldMatrix = cMath::MatrixInverse(pSubMeshEnt->GetWorldMatrix());
		cVector3f vTransSphCenter = cMath::MatrixMul(mtxInvWorldMatrix, avSphereCenter);
		cVector3f vTransBaseNormal = cMath::MatrixMul(mtxInvWorldMatrix, avBaseNormal);

		iVertexBuffer* pVB = pSubMeshEnt->GetVertexBuffer();


		float* pPositions = pVB->GetFloatArray(eVertexBufferElement_Position);
		float* pNormals = pVB->GetFloatArray(eVertexBufferElement_Normal);
		unsigned int* pIndices = pVB->GetIndices();
		for(int j=0;j<pVB->GetIndexNum();j+=3)
		{
			tVector3fVec vTriVertices;
			tVector3fVec vVertexNormals;

			for(int k=0;k<3;++k)
			{
				unsigned int pIndexBase = pIndices[j+k];
				unsigned int pPosIndex = pIndexBase*pVB->GetElementNum(eVertexBufferElement_Position);
				unsigned int pNrmIndex = pIndexBase*pVB->GetElementNum(eVertexBufferElement_Normal);
				vTriVertices.push_back(cVector3f(pPositions[pPosIndex],
												pPositions[pPosIndex+1],
												pPositions[pPosIndex+2]));
				vVertexNormals.push_back(cVector3f(pNormals[pNrmIndex],
											pNormals[pNrmIndex+1],
											pNormals[pNrmIndex+2]));
			}

			cVector3f vTriNormal = (vVertexNormals[0] + vVertexNormals[1] + vVertexNormals[2])*fInv3;
			if(cMath::Vector3Dot(vTriNormal, vTransBaseNormal)<0)
				continue;

			// Check for vertices
			bool bAddTriangle = cMath::CheckPointInSphereIntersection(vTriVertices[0], vTransSphCenter, afRadius) ||
								cMath::CheckPointInSphereIntersection(vTriVertices[1], vTransSphCenter, afRadius) ||
								cMath::CheckPointInSphereIntersection(vTriVertices[2], vTransSphCenter, afRadius);

			// Check edges
			if(bAddTriangle==false)
			{
				for(int k=0;k<2;++k)
				{
					if(cMath::CheckSphereLineIntersection(vTransSphCenter, afRadius, vTriVertices[k], vTriVertices[k+1], NULL, NULL, NULL, NULL))
					{
						bAddTriangle=true;
						break;
					}
				}
				if(bAddTriangle==false &&
					cMath::CheckSphereLineIntersection(vTransSphCenter, afRadius, vTriVertices[2], vTriVertices[0], NULL, NULL, NULL, NULL))
					bAddTriangle=true;
			}
			// Check if inside
			if(bAddTriangle==false)
			{
				cPlanef triPlane;
				triPlane.FromNormalPoint(vTriNormal, vTriVertices[0]);
				cVector3f vCenterProjectedOnPlane;
				if(cMath::CheckPlaneLineIntersection(triPlane, vTransSphCenter, vTransSphCenter-vTriNormal*afRadius,&vCenterProjectedOnPlane, NULL))
				{
					bAddTriangle = true;
				}

			}
			
			if(bAddTriangle)
			{
				avTriangles.push_back(vTriVertices[0]);
				avTriangles.push_back(vTriVertices[1]);
				avTriangles.push_back(vTriVertices[2]);
				avNormals.push_back(vVertexNormals[0]);
				avNormals.push_back(vVertexNormals[1]);
				avNormals.push_back(vVertexNormals[2]);
			}
		}
	}
}

//----------------------------------------------------------------------------------

void cEditorHelper::GetIDsFromEntityList(const tEntityWrapperList &alstEntities, tIntList& alstEntityIDs, bool abClearList)
{
	if(abClearList)
		alstEntityIDs.clear();
	
	tEntityWrapperList::const_iterator it = alstEntities.begin();
	for(;it!=alstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		alstEntityIDs.push_back(pEnt->GetID());
	}
}

//----------------------------------------------------------------------------------

bool cEditorHelper::LoadEntityFile(int alID, 
								   const tString& asName, 
								   const tString& asFilename, 
								   cMeshEntity** apEntity, 
								   bool abLoadAsTemp)
{
	if(asFilename=="")
		return false;

	cWorld* pWorld;
	if(abLoadAsTemp)
		pWorld = mpEditor->GetTempWorld();
	else
		pWorld = mpEditor->GetEditorWorld()->GetWorld();

	cResources* pRes = mpEditor->GetEngine()->GetResources();
	tWString sFullPath = pRes->GetFileSearcher()->GetFilePath(asFilename);

	cMeshEntity* pEntity = NULL;
	cXmlElement* pXmlEntity = NULL;

	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();
	if(pDoc->CreateFromFile(sFullPath)==false)
	{
		pRes->DestroyXmlDocument(pDoc);
		if(apEntity)
			*apEntity = NULL;

		return false;
	}

	pEntity = (cMeshEntity*) mpEditor->GetEngineEntityLoader()->Load(asName, 
																	 alID,
																	 true, 
																	 pDoc, 
																	 cMatrixf::Identity,
																	 cVector3f(1),
																	 pWorld,
																	 asFilename, 
																	 sFullPath, 
																	 NULL);

	pRes->DestroyXmlDocument(pDoc);

	if(apEntity)
		*apEntity = pEntity;

	return pEntity!=NULL;
}

bool cEditorHelper::LoadTextureResource(eEditorTextureResourceType aTexType, const tString& asFile, iTexture** apTexture, const tString& asAnimMode, float afFrameTime)
{
	if(asFile=="")
		return false;

	cTextureManager* pManager = mpEditor->GetEngine()->GetResources()->GetTextureManager();
	iTexture* pTexture = NULL;

	tString sAnimMode = cString::ToLowerCase(asAnimMode);

	if(sAnimMode=="" || sAnimMode=="none")
	{
		switch(aTexType)
		{
		case eEditorTextureResourceType_1D:
			pTexture = pManager->Create1D(asFile, true);
			break;
		case eEditorTextureResourceType_2D:
			pTexture = pManager->Create2D(asFile,true);
			break;
		case eEditorTextureResourceType_3D:
			pTexture = pManager->Create3D(asFile,true);
			break;
		case eEditorTextureResourceType_CubeMap:
			pTexture = pManager->CreateCubeMap(asFile,true);
			break;
		}
	}
	else
	{
		eTextureType texType;
		eTextureAnimMode animMode = eTextureAnimMode_None;
		switch(aTexType)
		{
		case eEditorTextureResourceType_1D:
			texType = eTextureType_1D;
			break;
		case eEditorTextureResourceType_2D:
			texType = eTextureType_2D;
			break;
		case eEditorTextureResourceType_3D:
			texType = eTextureType_3D;
			break;
		case eEditorTextureResourceType_CubeMap:
			texType = eTextureType_CubeMap;
			break;
		}
		if(sAnimMode=="loop")
			animMode = eTextureAnimMode_Loop;
		else if(sAnimMode=="oscillate")
			animMode = eTextureAnimMode_Oscillate;

		pTexture = pManager->CreateAnim(asFile, true, texType);
		if(pTexture)
		{
			pTexture->SetAnimMode(animMode);
			pTexture->SetFrameTime(afFrameTime);
		}
	}
	
	if(apTexture==NULL)
	{
		if(pTexture) pManager->Destroy(pTexture);
	}
	else
	{
		*apTexture=pTexture;
	}

	return (pTexture!=NULL);
}

//----------------------------------------------------------------------------------

bool cEditorHelper::LoadResourceFile(eEditorResourceType aResType, const tString& asFile, void** apEditorResource, int alID)
{
	if(asFile=="")
		return false;

	bool bIsValid=false;
    cResources* pResources = mpEditor->GetEngine()->GetResources();
	iResourceManager* pManager = NULL;
	void* pResource = NULL;
	
	switch(aResType)
	{
	case eEditorResourceType_Material:
		pManager = pResources->GetMaterialManager();
		pResource = ((cMaterialManager*)pManager)->CreateMaterial(asFile);
		break;
	case eEditorResourceType_Texture:
		pManager = pResources->GetTextureManager();
		pResource = ((cTextureManager*)pManager)->Create1D(asFile, true);
		if(pResource==NULL)
			pResource = ((cTextureManager*)pManager)->Create2D(asFile,true);
		if(pResource==NULL)
			pResource = ((cTextureManager*)pManager)->Create3D(asFile,true);
		break;
	case eEditorResourceType_Sound:
		pManager = pResources->GetSoundEntityManager();
		pResource = ((cSoundEntityManager*)pManager)->CreateSoundEntity(asFile);
		break;
	case eEditorResourceType_ParticleSystem:
		pManager = pResources->GetParticleManager();
		pResource = mpEditor->GetEditorWorld()->GetWorld()->CreateParticleSystem(cString::ToString(alID), asFile,0);
		break;
	default:
		break;
	}

	bIsValid = (pResource!=NULL);

	if(apEditorResource==NULL)
	{
		if(aResType==eEditorResourceType_ParticleSystem)
		{
			mpEditor->GetEditorWorld()->GetWorld()->DestroyParticleSystem((cParticleSystem*)pResource);
		}
		else
		{
			if(pResource)
				pManager->Destroy((iResourceBase*)pResource);
		}
	}
	else
	{
		*apEditorResource = pResource;
	}

	return bIsValid;
}

//----------------------------------------------------------------------------------

void cEditorHelper::DrawBillboard(iTexture *apGfx, const cVector3f& avWorldPosition,
									const cVector2f& avSize, const cColor& aColor, 
									cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	apFunctions->SetModelViewMatrix(cMatrixf::Identity);

	cVector3f vViewSpacePos;
	cVector3f vViewSpaceSize; 
	
	GetViewSpacePosAndSize(apViewport->GetCamera(), avWorldPosition, avSize, vViewSpacePos, vViewSpaceSize);

	// TODO: I have to turn cull mode off because the quad is drawn facing away to the camera :S
	// Any fixes for this?
	// Yeah, need to inverse the vertex order somehow. It is possible to have a static variable with a vertex array for this.
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(true);
	apFunctions->SetTexture(0, apGfx);
	apFunctions->SetAlphaMode(eMaterialAlphaMode_Trans);
	apFunctions->GetLowLevelGfx()->SetCullActive(false);
	apFunctions->DrawQuad(vViewSpacePos - cVector3f(vViewSpaceSize.x,vViewSpaceSize.y,0)*0.5f, cVector2f(vViewSpaceSize.x, vViewSpaceSize.y), cVector2f(0,1), cVector2f(1,0),false, aColor);
	apFunctions->GetLowLevelGfx()->SetCullActive(true);
	apFunctions->SetTexture(0,NULL);
	apFunctions->SetAlphaMode(eMaterialAlphaMode_Solid);
	apFunctions->SetDepthWrite(false);
	apFunctions->SetMatrix(NULL);
}

//----------------------------------------------------------------------------------

void cEditorHelper::DrawPyramid(cRendererCallbackFunctions* apFunctions, const cVector3f& avBaseCenter, const cVector3f& avTop, float afHalfWidth, const cColor& aColor)
{
	cVector3f vNormal = avTop-avBaseCenter;
	cVector3f vPoint = avBaseCenter+1;
	cVector3f vRight = cMath::Vector3Cross(vNormal, vPoint);
	vRight.Normalize();
	cVector3f vForward = cMath::Vector3Cross(vNormal, vRight);
	vForward.Normalize();

	cVector3f vVertices[4] = 
	{
		avBaseCenter + (vRight+vForward)*afHalfWidth,
		avBaseCenter + (vRight-vForward)*afHalfWidth,
		avBaseCenter + (vRight*(-1)-vForward)*afHalfWidth,
		avBaseCenter + (vRight*(-1)+vForward)*afHalfWidth,
	};

	tVertexVec vTriPoints[4];
	vTriPoints[0].push_back(cVertex(avTop,aColor));
	vTriPoints[0].push_back(cVertex(vVertices[0],aColor));
	vTriPoints[0].push_back(cVertex(vVertices[1],aColor));

	vTriPoints[1].push_back(cVertex(avTop,aColor));
	vTriPoints[1].push_back(cVertex(vVertices[1],aColor));
	vTriPoints[1].push_back(cVertex(vVertices[2],aColor));

	vTriPoints[2].push_back(cVertex(avTop,aColor));
	vTriPoints[2].push_back(cVertex(vVertices[2],aColor));
	vTriPoints[2].push_back(cVertex(vVertices[3],aColor));

	vTriPoints[3].push_back(cVertex(avTop,aColor));
	vTriPoints[3].push_back(cVertex(vVertices[3],aColor));
	vTriPoints[3].push_back(cVertex(vVertices[0],aColor));

	//apFunctions->SetMatrix(NULL);
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(true);
	for(int i=0;i<4;++i)
	{
		apFunctions->GetLowLevelGfx()->DrawTriangle(vTriPoints[i]);
	}
	apFunctions->SetDepthWrite(false);
}

//----------------------------------------------------------------------------------

iVertexBuffer* cEditorHelper::CreatePyramidVtxBuffer(cGraphics* apGfx, const cVector3f& avBaseCenter, const cVector3f& avTip, float afHalfWidth, const cColor& aColor)
{
	iVertexBuffer* pVB = apGfx->GetLowLevel()->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, eVertexBufferUsageType_Dynamic);

	cVector3f vNormal = avTip-avBaseCenter;
	cVector3f vPoint = avBaseCenter+1;
	cVector3f vRight = cMath::Vector3Cross(vNormal, vPoint);
	vRight.Normalize();
	cVector3f vForward = cMath::Vector3Cross(vNormal, vRight);
	vForward.Normalize();

	cVector3f vVertices[5] = 
	{
		avTip,
		avBaseCenter + (vRight+vForward)*afHalfWidth,
		avBaseCenter + (vRight-vForward)*afHalfWidth,
		avBaseCenter + (vRight*(-1)-vForward)*afHalfWidth,
		avBaseCenter + (vRight*(-1)+vForward)*afHalfWidth,
	};

	unsigned int vIndices[12] = 
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
	};

	pVB->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
	pVB->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
	pVB->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);

	for(int i=0;i<5;++i)
	{
		pVB->AddVertexVec3f(eVertexBufferElement_Position, vVertices[i]);
		pVB->AddVertexColor(eVertexBufferElement_Color0, aColor);
	}
	for(int i=0;i<4;++i)
	{
		cVector3f vEdge1 = vVertices[vIndices[i+1]]-vVertices[vIndices[i]];
		cVector3f vEdge2 = vVertices[vIndices[i+2]]-vVertices[vIndices[i]];
		cVector3f vFaceNormal = cMath::Vector3Cross(vEdge1, vEdge2);
		pVB->AddVertexVec3f(eVertexBufferElement_Normal, vFaceNormal);
		pVB->AddVertexVec3f(eVertexBufferElement_Normal, vFaceNormal);
		pVB->AddVertexVec3f(eVertexBufferElement_Normal, vFaceNormal);
		
		pVB->AddIndex(vIndices[i*3]);
		pVB->AddIndex(vIndices[i*3+1]);
		pVB->AddIndex(vIndices[i*3+2]);
	}

	pVB->Compile(0);
	

	return pVB;
}

//----------------------------------------------------------------------------------

void cEditorHelper::GetViewSpacePosAndSize(cCamera* apCamera, const cVector3f& avWorldPosition, const cVector3f& avWorldSize, cVector3f& avViewSpacePosition, cVector3f& avViewSpaceSize)
{
	if(apCamera==NULL)
	{
		avViewSpacePosition = avWorldPosition;
		avViewSpaceSize = avWorldSize;
	}
	else
	{
		avViewSpacePosition = cMath::MatrixMul(apCamera->GetViewMatrix(), avWorldPosition);
		switch(apCamera->GetProjectionType())
		{
		case eProjectionType_Orthographic:
			avViewSpaceSize = avWorldSize * apCamera->GetOrthoViewSize().x * 0.25f;
			break;
		case eProjectionType_Perspective:
			avViewSpaceSize = avWorldSize * cMath::Abs(avViewSpacePosition.z);
			break;
		default:
			break;
		}
	}
}

//----------------------------------------------------------------------------------
