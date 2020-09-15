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

#include "../Common/EdHelper.h"


iEditor* cEdHelper::mpEditor = NULL;

//----------------------------------------------------------------------------------

cVector2f cEdHelper::GetInvYMousePos(const cVector2f& avMousePos, const cVector2f& avViewportSize)
{
	///////////////////////////////////////////////
	//Invert mouse y to make it usable in the screenspace picking algo
	cVector2f vMousePos = avMousePos;
	vMousePos.y = avViewportSize.y - vMousePos.y;

	return vMousePos;
}

//----------------------------------------------------------------------

void cEdHelper::CalculateRayBoundingBox(const cVector3f& avStart, const cVector3f& avEnd, cVector3f &avMin, cVector3f &avMax)
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

//----------------------------------------------------------------------

bool cEdHelper::CheckScreenSpaceMouseMeshIntersect(cCamera* apCamera, const cVector2f& avMousePos,  
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

bool cEdHelper::CheckScreenSpaceMouseSubMeshIntersect(const cVector2f& avInvYMousePos,
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

bool cEdHelper::CheckScreenSpaceMouseVertexBufferIntersect(const cVector2f& avInvYMousePos,
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

bool cEdHelper::CheckRayMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd,
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

bool cEdHelper::CheckRaySubMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd, cSubMeshEntity* apObject, cVector3f* apIntersection, float *apT,unsigned int* apTriangleIdx, tVector3fVec* apTriangle)
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

//----------------------------------------------------------------------------------

/*
void cEdHelper::GetIDsFromEntityList(const tEntityWrapperList &alstEntities, tIntList& alstEntityIDs, bool abClearList)
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
*/

//----------------------------------------------------------------------------------

void cEdHelper::DrawPyramid(cRendererCallbackFunctions* apFunctions, const cVector3f& avBaseCenter, const cVector3f& avTop, float afHalfWidth, const cColor& aColor)
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

iVertexBuffer* cEdHelper::CreatePyramidVtxBuffer(cGraphics* apGfx, const cVector3f& avBaseCenter, const cVector3f& avTip, float afHalfWidth, const cColor& aColor)
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
