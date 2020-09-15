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

#include "graphics/DecalCreator.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Renderer.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/AnimationManager.h"
#include "scene/MeshEntity.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cDecalCreator::cDecalCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources) : mvDecalSize(0)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpResources = apResources;

		mvDecalRight = 0;
		mvDecalUp = 0;
		mvDecalForward = 0;
		mfDecalOffset = 0;

		SetDecalRight(	cVector3f(1,0,0),false);
		SetDecalUp(		cVector3f(0,1,0),false);
		SetDecalForward(cVector3f(0,0,1),false);
		SetDecalPosition(0);
		SetDecalOffset(0);
		SetDecalSize(1);
		SetUVSubDivisions(1);
		SetCurrentSubDiv(0);
		SetColor(1);

		mpDecalMaterial = NULL;
		mpDecalVB = NULL;

		mlMaxDecalTriangleCount = 250;

		mbCompiled = false;
	}

	//-----------------------------------------------------------------------

	cDecalCreator::~cDecalCreator()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	bool cDecalCreator::AddSubMesh(cSubMeshEntity* apSubMesh)
	{
		if(apSubMesh==NULL)
			return false;

		cBoundingVolume* pMeshBV = apSubMesh->GetBoundingVolume();
		cBoundingVolume* pDecalBV = GetDecalBoundingVolume();

		if(cMath::CheckBVIntersection(*pMeshBV, *pDecalBV))
		{
			AddAffectedSubMesh(apSubMesh);
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::AddAffectedSubMesh(cSubMeshEntity* apSubMesh)
	{
		if(apSubMesh==NULL)
			return;

		mvMeshes.push_back(apSubMesh);

		mbUpdated = true;
	}

	void cDecalCreator::ClearMeshes()
	{
		mvMeshes.clear();

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalPosition(const cVector3f& avPosition)
	{
		if(mvDecalPosition==avPosition)
			return;

		mvDecalPosition = avPosition;

		mbUpdated = true;
		mbBVUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalUp(const cVector3f& avUp, bool abComputeBasis)
	{
		if(mvDecalUp==avUp)
			return;

		mvDecalUp = avUp;
		mvDecalUp.Normalize();

		if(abComputeBasis)	ComputeBasis();

		mbUpdated = true;
		mbBVUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalRight(const cVector3f& avRight, bool abComputeBasis)
	{
		if(mvDecalRight==avRight)
			return;

		mvDecalRight = avRight;
		mvDecalRight.Normalize();

		if(abComputeBasis)	ComputeBasis();

		mbUpdated = true;
		mbBVUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalForward(const cVector3f& avForward, bool abComputeBasis)
	{
		if(mvDecalForward==avForward)
			return;

		mvDecalForward = avForward;
		mvDecalForward.Normalize();

		if(abComputeBasis)	ComputeBasis();

		mbUpdated = true;
		mbBVUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalOffset(float afOffset)
	{
		if(mfDecalOffset==afOffset)
			return;

		mfDecalOffset = afOffset;

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetDecalSize(const cVector3f& avSize)
	{
		if(mvDecalSize==avSize)
			return;

		mvDecalSize = avSize;

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetMaterial(const tString& asMat)
	{
		if(msMaterial==asMat)
			return;

		cMaterialManager* pManager = mpResources->GetMaterialManager();
		cMaterial* pMat = pManager->CreateMaterial(asMat);
		if(pMat)
		{
			if(mpDecalMaterial==pMat)
				pManager->Destroy(pMat);
			else
			{
				if(mpDecalMaterial)
					pManager->Destroy(mpDecalMaterial);

				mpDecalMaterial = pMat;
				msMaterial = asMat;
			}
		}
		else
		{
			if(mpDecalMaterial)
					pManager->Destroy(mpDecalMaterial);
			mpDecalMaterial = NULL;
			msMaterial = "";
		}

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetColor(const cColor& aCol)
	{
		if(mColor==aCol)
			return;

		mColor = aCol;

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetUVSubDivisions(const cVector2l& avSubDiv)
	{
		if(mvSubDiv==avSubDiv)
			return;

		mvSubDiv = avSubDiv;

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::SetCurrentSubDiv(int alSubDiv)
	{
		if(mlCurrentSubDiv==alSubDiv)
			return;

		mlCurrentSubDiv = alSubDiv;

		mbUpdated = true;
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	bool cDecalCreator::Compile()
	{
		if(mbUpdated)
		{
			mbCompiled = false;

			if(mvMeshes.empty())
				return false;
			if(cMath::Abs(mvDecalSize.x)<kEpsilonf || cMath::Abs(mvDecalSize.y)<kEpsilonf)
				return false;

			mbUpdated = false;

			mlDecalVertexCount = 0;
			mlDecalTriangleCount = 0;

			cVector3f vDecalHalfSize = mvDecalSize*0.5f;

			cVector3f vAxes[] = { mvDecalRight, mvDecalUp, mvDecalForward };
			float vSign[] = { 1.0f, -1.0f };

			
			mvMatrices.clear();
			mvTransformedBases.clear();
			mvClipPlanes.clear();
			for(int i=0;i<3;++i)
			{
				cVector3f vAdd = vAxes[i]*vDecalHalfSize.v[i];
				for(int j=0;j<2;++j)
				{
					mvClipPlanes.push_back(cPlanef(vAxes[i]*vSign[j], mvDecalPosition - vAdd*vSign[j]));
					//Log("ClipPlane %i %s %f\n",i+j, mvClipPlanes.back().GetNormal().ToFileString().c_str(), mvClipPlanes.back().d);
				}
			}
			
			if(mpDecalVB)
			{
				hplDelete(mpDecalVB);
				mpDecalVB = NULL;
			}
			
			mpDecalVB = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware,
																eVertexBufferDrawType_Tri,
																eVertexBufferUsageType_Static);
			mpDecalVB->CreateElementArray(eVertexBufferElement_Position, eVertexBufferElementFormat_Float, 4);
			mpDecalVB->CreateElementArray(eVertexBufferElement_Color0, eVertexBufferElementFormat_Float, 4);
			mpDecalVB->CreateElementArray(eVertexBufferElement_Texture0, eVertexBufferElementFormat_Float, 3);
			mpDecalVB->CreateElementArray(eVertexBufferElement_Normal, eVertexBufferElementFormat_Float, 3);
						
			///////////////////////////////////////////
			// Clip near triangles
			for(size_t i=0;i<mvMeshes.size();++i)
			{
				cSubMeshEntity* pAffectedMesh = mvMeshes[i];
				ClipMesh(pAffectedMesh, mpDecalVB);
			}

			//////////////////////////////////////////////////////////
			// Set up texture coordinates
			float fInvW = 1.0f/mvDecalSize.x;
			float fInvH = 1.0f/mvDecalSize.z;
			float *pVertexPositions = mpDecalVB->GetFloatArray(eVertexBufferElement_Position);

			//////////////////////////////////////////////////////////
			// Set up subdivisions
			cVector2f vInvSubDivSize = cVector2f(1.0f/mvSubDiv.x,1.0f/mvSubDiv.y);
			float fStartU = (float)(mlCurrentSubDiv%mvSubDiv.x);
			float fStartV = (float)(mlCurrentSubDiv/mvSubDiv.x);
			
			for(int i=0;i<mpDecalVB->GetVertexNum();++i)
			{
				int lBaseIdx = i*mpDecalVB->GetElementNum(eVertexBufferElement_Position);
				cVector3f vPointRelToCenter = cVector3f(pVertexPositions[lBaseIdx],
														pVertexPositions[lBaseIdx+1],
														pVertexPositions[lBaseIdx+2])-mvDecalPosition;

				float u = vInvSubDivSize.x*(fStartU+cMath::Vector3Dot(vPointRelToCenter, mvDecalRight)*fInvW+0.5f);
				float v = vInvSubDivSize.y*(fStartV+cMath::Vector3Dot(vPointRelToCenter, mvDecalForward)*fInvH+0.5f);
				u = cMath::Clamp(u, 0.0f, 1.0f);
				v = 1.0f-cMath::Clamp(v, 0.0f, 1.0f);
				mpDecalVB->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(u,v,0));
			}
			mbCompiled = mpDecalVB->Compile(eVertexCompileFlag_CreateTangents);
		}

		return mbCompiled;
	}
	
	bool cDecalCreator::CanCreateDecal()
	{
		mbCanCreate = Compile();

		return mbCanCreate;
	}

	//-----------------------------------------------------------------------

	cMesh* cDecalCreator::CreateDecalMesh()
	{
		if(CanCreateDecal()==false)
			return NULL;

		cMaterial* pMat = mpResources->GetMaterialManager()->CreateMaterial(msMaterial);
		if(pMat==NULL)
			return NULL;

		cMesh *pMesh = hplNew( cMesh, ("", _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()) ); 
		cSubMesh* pSubMesh = pMesh->CreateSubMesh("Decal");
		pSubMesh->SetVertexBuffer(mpDecalVB->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Static, mpDecalVB->GetVertexElementFlags()));
		pSubMesh->SetMaterial(pMat);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	void cDecalCreator::DrawDebug(cRendererCallbackFunctions* apFunctions, bool abDrawAxes, bool abDrawWireframe)
	{
		apFunctions->SetMatrix(NULL);

		if(mpDecalVB)
		{
			if(mpDecalMaterial)
			{
				apFunctions->SetDepthTest(true);
				apFunctions->SetDepthWrite(false);
				apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
				apFunctions->SetAlphaMode(eMaterialAlphaMode_Solid);

				apFunctions->SetProgram(NULL);

				for(int i=0; i<kMaxTextureUnits; ++i)
				{
					iTexture *pTexture = mpDecalMaterial->GetTextureInUnit(eMaterialRenderMode_Diffuse,i);
					apFunctions->SetTexture(i, pTexture);

				}
				
				apFunctions->SetVertexBuffer(mpDecalVB);

				apFunctions->DrawCurrent();

				apFunctions->SetTextureRange(NULL,0);
				apFunctions->SetBlendMode(eMaterialBlendMode_None);
			}
			if(abDrawWireframe)	apFunctions->DrawWireFrame(mpDecalVB, cColor(1));
		}

		if(abDrawAxes)
		{
			cVector3f vHalfDecalSize = mvDecalSize*0.5f;
			cMatrixf mtxTransform = cMath::MatrixUnitVectors(mvDecalRight, mvDecalUp, mvDecalForward, mvDecalPosition);
			//cMatrixf mtxTransform = cMatrixf::Identity;
			mtxTransform.SetTranslation(mvDecalPosition);

			apFunctions->SetMatrix(&mtxTransform);
			//apFunctions->GetLowLevelGfx()->DrawSphere(0, 0.01f, 1);
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(vHalfDecalSize.x,0,0), cColor(1,0,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,vHalfDecalSize.y,0), cColor(0,1,0,1));
			apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,0,vHalfDecalSize.z), cColor(0,0,1,1));
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfDecalSize*-1, vHalfDecalSize, 1);

			for(size_t i=0;i<mvTransformedBases.size();i+=4)
			{
				const cVector3f& vRight = mvTransformedBases[i+1];
				const cVector3f& vUp = mvTransformedBases[i+2];
				const cVector3f& vFwd = mvTransformedBases[i+3];

				cMatrixf mtxBasis = cMath::MatrixUnitVectors(vRight, vUp, vFwd, mvTransformedBases[i]);
				apFunctions->SetMatrix(&mtxBasis);
				apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(vHalfDecalSize.x,0,0), cColor(1,0,0,1));
				apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,vHalfDecalSize.y,0), cColor(0,1,0,1));
				apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,0,vHalfDecalSize.z), cColor(0,0,1,1));
				apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfDecalSize*-1, vHalfDecalSize, 1);
			}

			//for(int i=0;i<(int)mvMatrices.size();++i)
			//{
			//	cMatrixf& mtxTransform = mvMatrices[i];

			//	apFunctions->SetMatrix(&mtxTransform);
			//	apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(1,0,0), cColor(1,0,0,1));
			//	apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,1,0), cColor(0,1,0,1));
			//	apFunctions->GetLowLevelGfx()->DrawLine(0,cVector3f(0,0,1), cColor(0,0,1,1));
			//}

			apFunctions->SetMatrix(NULL);
		}

		apFunctions->SetBlendMode(eMaterialBlendMode_None);
		apFunctions->SetDepthTest(false);
	}

	cBoundingVolume* cDecalCreator::GetDecalBoundingVolume()
	{
		if(mbBVUpdated)
		{
			mbBVUpdated=false;

			cMatrixf mtxTransform = cMath::MatrixUnitVectors(mvDecalRight, mvDecalUp, mvDecalForward, 0).GetTranspose();
			mtxTransform = cMath::MatrixInverse(mtxTransform);

			mDecalBV.SetTransform(mtxTransform);
			mDecalBV.SetPosition(mvDecalPosition);
			mDecalBV.SetSize(mvDecalSize);
		}
			
		return &mDecalBV; 
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cDecalCreator::ComputeBasis()
	{
		cVector3f vUp = mvDecalUp;
		cVector3f vRgt = mvDecalRight;
		cVector3f vFwd = mvDecalForward;
		cMath::Vector3OrthonormalizeBasis(vUp, vRgt, vFwd, mvDecalUp, mvDecalRight, mvDecalForward);
	}

	//-----------------------------------------------------------------------

	bool cDecalCreator::AddPolygon(	int alVertexCount, const cVector3f* apVertices, const cVector3f* apNormals, iVertexBuffer* apDecalVB, 
									const cMatrixf& amtxWorldMatrix, const cMatrixf& amtxWorldNormalRot)
	{
		int lCount = mlDecalVertexCount;
		int lPolyTriCount = alVertexCount-2;

		if(mlDecalTriangleCount + lPolyTriCount > mlMaxDecalTriangleCount)
			return false;

		mlDecalTriangleCount += lPolyTriCount;

		for(int i=0;i<lPolyTriCount;++i)
		{
			apDecalVB->AddIndex(lCount);
			apDecalVB->AddIndex(lCount+i+1);
			apDecalVB->AddIndex(lCount+i+2);
		}

		//float f = 1.0f / (1.0f - kEpsilonf);

		//////////////////////////////////////////////////////
		// Add the polys multiplied by the mesh world matrix
		for(int i=0;i<alVertexCount;++i)
		{
			//float fAlpha = (cMath::Vector3Dot(mvDecalUp, vNormal) / vNormal.Length()-kEpsilonf)*f;
			//fAlpha = cMath::Clamp(fAlpha, 0.0f, 1.0f);

			cVector3f vPos = cMath::MatrixMul(amtxWorldMatrix, apVertices[i]+apNormals[i]*mfDecalOffset);
			cVector3f vNormal = cMath::MatrixMul3x3(amtxWorldNormalRot, apNormals[i]);
			vNormal.Normalize();

			//Log("Local Vertex Pos: (%s)\n", (apVertices[i]+apNormals[i]*mfDecalOffset).ToFileString().c_str());
			//Log("World Vertex Pos: (%s)\n", vPos.ToFileString().c_str());

			apDecalVB->AddVertexVec3f(eVertexBufferElement_Position, vPos);
			apDecalVB->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
			apDecalVB->AddVertexColor(eVertexBufferElement_Color0, cColor(mColor.r,mColor.g,mColor.b,mColor.a));

			++lCount;
		}

		mlDecalVertexCount = lCount;

		return true;

	}

	//-----------------------------------------------------------------------

	void cDecalCreator::ClipMesh(cSubMeshEntity* apSubMesh,iVertexBuffer* apDecalVB)
	{
		//Log("Clipping mesh %s\n", apSubMesh->GetName().c_str());

		cVector3f vNewVertices[9];
		cVector3f vNewNormals[9];
		float fInvThree = 1.0f/3.0f;

		cMatrixf mtxSubMeshWorldMatrix = apSubMesh->GetWorldMatrix();
		cMatrixf mtxInvSubMeshWorldMatrix = cMath::MatrixInverse(mtxSubMeshWorldMatrix);
		cMatrixf mtxSubMeshWorldNormalRot = mtxInvSubMeshWorldMatrix.GetRotation().GetTranspose();
		

		//////////////////////////////////////////////////
		// Transform clip planes to local submesh coordinates
		tPlanefVec vTransformedPlanes;
		for(int i=0;i<(int)mvClipPlanes.size();++i)
		{
			vTransformedPlanes.push_back(cMath::TransformPlane(mtxInvSubMeshWorldMatrix, mvClipPlanes[i]));
			//Log("Plane %d transformed normal:(%s) d:%f\n", i, vTransformedPlanes.back().GetNormal().ToFileString().c_str(), vTransformedPlanes.back().d);
		}
		cVector3f vTransformedUp = vTransformedPlanes[2].GetNormal();

		iVertexBuffer* pSubMeshVB = apSubMesh->GetVertexBuffer();

		float* pVertices = pSubMeshVB->GetFloatArray(eVertexBufferElement_Position);
		float* pNormals = pSubMeshVB->GetFloatArray(eVertexBufferElement_Normal);
		unsigned int* pIndices = pSubMeshVB->GetIndices();
		int lPosStride = pSubMeshVB->GetElementNum(eVertexBufferElement_Position);
		int lNrmStride = pSubMeshVB->GetElementNum(eVertexBufferElement_Normal);

		// Clip every triangle in submesh
		for(int j=0;j<pSubMeshVB->GetIndexNum();j+=3)
		{
			cVector3f vTriangle[3];
			cVector3f vNormal[3];
			
			for(int k=0;k<3;++k)
			{
				int lPosBaseIdx = pIndices[j+k]*lPosStride;
				int lNrmBaseIdx = pIndices[j+k]*lNrmStride;

				vTriangle[k] = cVector3f(pVertices[lPosBaseIdx],
										pVertices[lPosBaseIdx+1],
										pVertices[lPosBaseIdx+2]);
				vNormal[k] = cVector3f(pNormals[lNrmBaseIdx],
										pNormals[lNrmBaseIdx+1],
										pNormals[lNrmBaseIdx+2]);

			}

			// Skip if backfacing
			cVector3f vTriNormal = cMath::Vector3Cross(vTriangle[2]-vTriangle[0], vTriangle[1]-vTriangle[0]);
			vTriNormal.Normalize();

			if(cMath::Vector3Dot(vTransformedUp, vTriNormal)<=kEpsilonf)
				continue;

			vNewVertices[0] = vTriangle[0];
			vNewVertices[1] = vTriangle[1];
			vNewVertices[2] = vTriangle[2];

			vNewNormals[0] = vNormal[0];
			vNewNormals[1] = vNormal[1];
			vNewNormals[2] = vNormal[2];

			// Clip triangle against planes
			int lCount = ClipPolygon(3, vNewVertices, vNewNormals, vNewVertices, vNewNormals, vTransformedPlanes);
			if((lCount!=0) && (AddPolygon(lCount, vNewVertices, vNewNormals, apDecalVB, mtxSubMeshWorldMatrix,mtxSubMeshWorldNormalRot)==false)) break;
		}		
	}

	//-----------------------------------------------------------------------

	int cDecalCreator::ClipPolygon(int alVertexCount, const cVector3f* apVertices, const cVector3f* apNormals,
									cVector3f* apNewVertices, cVector3f* apNewNormals, const std::vector<cPlanef>& avPlanes)
	{
		/*Log("Clipping triangle with vertices (%s) (%s) (%s)\n", apVertices[0].ToString().c_str(),
														  apVertices[1].ToString().c_str(),
														  apVertices[2].ToString().c_str());*/
		cVector3f vTempVertices[9];
		cVector3f vTempNormals[9];

		int lCount = ClipPolygonAgainstPlane(avPlanes[0], alVertexCount, apVertices, apNormals, vTempVertices, vTempNormals);
		if(lCount!=0)
		{
			lCount = ClipPolygonAgainstPlane(avPlanes[1], lCount, vTempVertices, vTempNormals, apNewVertices, apNewNormals);
			if(lCount!=0)
			{
				lCount = ClipPolygonAgainstPlane(avPlanes[2], lCount, apNewVertices, apNewNormals, vTempVertices, vTempNormals);
				if(lCount!=0)
				{
					lCount = ClipPolygonAgainstPlane(avPlanes[3], lCount, vTempVertices, vTempNormals, apNewVertices, apNewNormals);
					if(lCount!=0)
					{
						lCount = ClipPolygonAgainstPlane(avPlanes[4], lCount, apNewVertices, apNewNormals, vTempVertices, vTempNormals);
						if(lCount!=0)
						{
							lCount = ClipPolygonAgainstPlane(avPlanes[5], lCount, vTempVertices, vTempNormals, apNewVertices, apNewNormals);
						}
					}
				}
			}
		}
		
		return lCount;
	}

	//-----------------------------------------------------------------------

	int cDecalCreator::ClipPolygonAgainstPlane(const cPlanef& aPlane, int alVertexCount, 
											   const cVector3f* apVertices, const cVector3f* apNormals, 
											   cVector3f* apNewVertices, cVector3f* apNewNormals)
	{
		//Log("\nClipping poly against plane %s %f\n", aPlane.GetNormal().ToFileString().c_str(), aPlane.d);
		bool bNegative[10];
		cVector3f vPlaneNormal = aPlane.GetNormal();

		//Log("Classifying vertices\n");
		int lNegativeCount = 0;
		for(int i=0;i<alVertexCount;++i)
		{
			bool bNeg = (cMath::PlaneToPointDist(aPlane, apVertices[i]) < kEpsilonf);
			bNegative[i] = bNeg;
			lNegativeCount += bNeg ? 1 :0;

			//Log("Vertex %s is %s the plane\n", apVertices[i].ToFileString().c_str(), bNeg?"behind":"in front of");
		}
		//Log("\n");

		if(lNegativeCount==alVertexCount)
			return 0;

		int lCount = 0;
		for(int i=0;i<alVertexCount;++i)
		{
			int j = (i!=0)? i-1 : alVertexCount-1;

			//Log("Testing edge (%s)->(%s)\n", apVertices[i].ToFileString().c_str(), apVertices[j].ToFileString().c_str());

			if(bNegative[i])
			{
				if(bNegative[j]==false)
				{
					//Log("\tEdge intersects plane\n");
					const cVector3f& v1 = apVertices[j];
					const cVector3f& v2 = apVertices[i];
					cVector3f vEdge = v1-v2;
					float t = cMath::PlaneToPointDist(aPlane, v1)/cMath::Vector3Dot(vPlaneNormal, vEdge);
					apNewVertices[lCount] = v1*(1.0f-t) + v2*t;

					//Log("\tIntersection pos: (%s) (t: %f)\n", apNewVertices[lCount].ToFileString().c_str(), t); 
					
					const cVector3f& n1 = apNormals[j];
					const cVector3f& n2 = apNormals[i];
					apNewNormals[lCount] = n1*(1.0f-t) + n2*t;
					apNewNormals[lCount].Normalize();

					++lCount;
				}
				//else
					//Log("\tEdge is behind plane\n");
			}
			else
			{
				if(bNegative[j])
				{
					//Log("\tEdge intersects plane\n");

					const cVector3f& v1 = apVertices[i];
					const cVector3f& v2 = apVertices[j];
					cVector3f vEdge = v1-v2;
					float t = cMath::PlaneToPointDist(aPlane, v1)/cMath::Vector3Dot(vPlaneNormal, vEdge);
					apNewVertices[lCount] = v1*(1.0f-t) + v2*t;

					//Log("\tIntersection pos: (%s) (t: %f)\n", apNewVertices[lCount].ToFileString().c_str(), t); 
					
					const cVector3f& n1 = apNormals[i];
					const cVector3f& n2 = apNormals[j];
					apNewNormals[lCount] = n1*(1.0f-t) + n2*t;
					apNewNormals[lCount].Normalize();
					
					++lCount;
				}
				//else
					//Log("\tEdge is in front of plane\n");

				apNewVertices[lCount] = apVertices[i];
				apNewNormals[lCount] = apNormals[i];

				//Log("\tAdding Vertex (%s)\n", apNewVertices[lCount].ToFileString().c_str()); 

				++lCount;
			}
		}

		return lCount;
	}


	//-----------------------------------------------------------------------

}
