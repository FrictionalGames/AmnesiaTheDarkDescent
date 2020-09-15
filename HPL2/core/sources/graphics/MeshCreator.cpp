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

#include "graphics/MeshCreator.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "resources/AnimationManager.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshCreator::cMeshCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpResources = apResources;
	}

	//-----------------------------------------------------------------------

	cMeshCreator::~cMeshCreator()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshCreator::CreateWireframeVertexBuffer(iVertexBuffer *apSrc)
	{
		iVertexBuffer *pDest = apSrc->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Static, apSrc->GetVertexElementFlags());

        pDest->ResizeIndices(apSrc->GetIndexNum()*4);
		unsigned int* pDestArray = pDest->GetIndices();
		int lDestCount=0;
		
		unsigned int* pIdxArray = apSrc->GetIndices();
		for(int tri=0; tri < apSrc->GetIndexNum(); tri+=3)
		{
			unsigned int* pTri = &pIdxArray[tri];

			for(int i=0; i<3; ++i)
			{
				int lNext = i+1 >=3 ? 0 : i+1;
				pDestArray[lDestCount] = pTri[i]; 
				pDestArray[lDestCount+1] = pTri[lNext];
                lDestCount+=2;
			}
		}

		pDest->UpdateData(0, true);

		return pDest;
	}

	//-----------------------------------------------------------------------
	
	cMesh* cMeshCreator::CreateBox(const tString &asName,cVector3f avSize, const tString &asMaterial)
	{
		cMesh *pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()) );

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		iVertexBuffer *pVtxBuff = CreateBoxVertexBuffer(avSize);
		pSubMesh->SetVertexBuffer(pVtxBuff);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreatePlane(const tString& asName, const cVector3f& avCorner1, const cVector3f& avCorner2, 
									 const cVector2f& avCorner1UV, const cVector2f& avCorner2UV, const cVector2f& avCorner3UV, const cVector2f& avCorner4UV, 
									 const tString& asMaterial)
	{
		if(avCorner1==avCorner2)
		{
			Error("CreatePlane failed: plane corners are coincident. Plane name: %s\n", asName.c_str());
			return NULL;
		}

		cVector3f vDiff = avCorner2 - avCorner1;
		tIntVec vPlaneAxes;
		int lPlaneNormalAxis = -1;
		int lNumSameCoords = 0;
		for(int i=0;i<3;++i)
		{
			if(vDiff.v[i]==0)
			{
				lPlaneNormalAxis = i;
				++lNumSameCoords;
			}
			else vPlaneAxes.push_back(i);
		}
		if(lPlaneNormalAxis<0 || lNumSameCoords>1)
		{
			Error("CreatePlane failed: plane corners are not coplanar. Plane name: %s\n", asName.c_str());
			return NULL;
		}

		cVector3f vCenter = (avCorner1 + avCorner2)*0.5f;

		tVector3fVec vTempCoords;
		vTempCoords.resize(4);
		vTempCoords[0] = avCorner1;
		vTempCoords[1] = avCorner2;

		cVector3f vTest1;
		cVector3f vTest2;

		for(int i=2; i<4;++i)
		{
			int lIndex1 = vPlaneAxes[0];
			int lIndex2 = vPlaneAxes[1];

			vTempCoords[i].v[lPlaneNormalAxis] = vTempCoords[0].v[lPlaneNormalAxis];
			vTempCoords[i].v[lIndex1] = vTempCoords[i-2].v[lIndex1];
			vTempCoords[i].v[lIndex2] = vTempCoords[3-i].v[lIndex2];

			vTest1 = vTempCoords[2];
			vTest2 = vTempCoords[3];
		}

		tVector3fVec vCoords;
		vCoords.resize(4);
		for(int i=0;i<(int)vCoords.size(); ++i)
		{
			int lCornerIndex;
			cVector3f vCorner = vTempCoords[i] - vCenter;
            if(vCorner.v[vPlaneAxes[0]]<=0)
			{
				if(vCorner.v[vPlaneAxes[1]]>0)
					lCornerIndex=0;
				else
					lCornerIndex=1;
			}
			else
			{
				if(vCorner.v[vPlaneAxes[1]]>0)
					lCornerIndex=3;
				else
					lCornerIndex=2;
			}
			vCoords[lCornerIndex] = vTempCoords[i];
		}
		cVector3f vFirstCorner = vCoords[1];
		for(int i=0;i<(int)vCoords.size();++i)
		{
			vCoords[i]-=vFirstCorner;
		}
		
		iVertexBuffer* pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(
								eVertexBufferType_Hardware,
								eVertexBufferDrawType_Tri, eVertexBufferUsageType_Dynamic,4,6);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		cVector3f vTexCoords[4] = 
		{
			cVector3f(avCorner1UV.x, avCorner1UV.y,0),
			cVector3f(avCorner2UV.x, avCorner2UV.y,0),			// Start Corner
			cVector3f(avCorner3UV.x, avCorner3UV.y,0),
			cVector3f(avCorner4UV.x, avCorner4UV.y,0)			// End Corner
		};

		for(int i=0;i<4;i++)
		{
			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vCoords[i]);
			pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1,1,1));
			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, vTexCoords[i] );
			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal,cVector3f(0,1,0));

		}
		
		for(int i=0;i<3;i++) pVtxBuffer->AddIndex(i);
		for(int i=2;i<5;i++) pVtxBuffer->AddIndex(i==4?0:i);

		pVtxBuffer->Compile(eVertexCompileFlag_CreateTangents);

		cMesh *pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()) ); 

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);
		pSubMesh->SetMaterialName(asMaterial);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreateSphere(const tString& asName, float afRadius, int alSections, int alSlices, const tString &asMaterial)
	{
		//////////////////////////////////////////////////
		// Create vertex buffer
		iVertexBuffer* pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, 
																	eVertexBufferUsageType_Static);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		/////////////////////////////////
		// Set up variables
		float fInvSlices = 1.0f/alSlices;
		float fDiameter = 2*afRadius;
		float fHeightStep = fDiameter*fInvSlices;
		float fSectionStep = k2Pif/(float)alSections;

		cColor col = cColor(1,1);

		/////////////////////////////////
		// Create north pole vertex
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0, afRadius, 0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0, 1, 0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		/////////////////////////////////
		// Create slice vertices
		float fPiOverSlices = kPif*fInvSlices;
		float fHalfPi		= kPif*0.5f;
		for(int i=0;i<alSlices-1; ++i)
		{
			float fHeight = afRadius - afRadius*(1+sin(fPiOverSlices*(float)(i+1)-fHalfPi));
			float fAngle = asin(fHeight/afRadius);
			tVector3fVec vVertices;

			CreateCircumference(afRadius*cos(fAngle), fSectionStep, fHeight, vVertices);
			for(int j=0;j<(int)vVertices.size();++j)
			{
				const cVector3f& vVertex = vVertices[j];
				cVector3f vNormal = vVertex;
				vNormal.Normalize();
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
				pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
			}
		}
		/////////////////////////////////
		// Create south pole vertex
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0, -afRadius, 0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0, -1, 0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		//////////////////////////////////
		// Create triangles (Wrap helpers do this for us)

		// Create north pole slice triangles
		WrapUpperCap(pVtxBuffer, 0, 1, alSections);
		
		// Create faces for inner slices
		for(int i=0;i<alSlices-2;++i)
			WrapSides(pVtxBuffer, 1+i*alSections, alSections);

		// Create triangles for south pole slice
		{
			int lLastVertex = pVtxBuffer->GetVertexNum()-1;
			int lSliceStart = lLastVertex-alSections;

			WrapLowerCap(pVtxBuffer, lLastVertex, lSliceStart, alSections); 
		}
		
		////////////////////////////////////////////////////////////
		// Compile vertex buffer and create mesh
		if(!pVtxBuffer->Compile(eVertexCompileFlag_CreateTangents))
		{
			hplDelete(pVtxBuffer);
			pVtxBuffer = NULL;
		}

		cMesh* pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()));
		cSubMesh* pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;		
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreateCylinder(const tString& asName, const cVector2f &avSize, int alSections, const tString &asMaterial)
	{
		iVertexBuffer* pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, 
																	eVertexBufferUsageType_Static);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		float fAngleStep = k2Pif/(float)alSections;
		float fHalfHeight = avSize.y*0.5f;
		cColor col = cColor(1,1);

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0,fHalfHeight,0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0,1,0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
		tVector3fVec vVertices;
		CreateCircumference(avSize.x, fAngleStep, 0, vVertices);
		float vHeights[] = { fHalfHeight, -fHalfHeight };

		for(int i=0;i<2;++i)
		{
			for(int j=0;j<(int)vVertices.size();++j)
			{
				cVector3f vVertex = vVertices[j];
				vVertex.y = vHeights[i];
				cVector3f vNormal = vVertex;
				vNormal.Normalize();

				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
				pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
			}
		}
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0,-fHalfHeight,0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0,-1,0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		// Top cap
		{
			WrapUpperCap(pVtxBuffer, 0, 1, alSections);
		}

		// Sides
		{
			WrapSides(pVtxBuffer, 1, alSections);
		}

		// Bottom cap
		{
			int lLastVertex = pVtxBuffer->GetVertexNum()-1;
			int lCapStart = lLastVertex-alSections;
			WrapLowerCap(pVtxBuffer, lLastVertex, lCapStart, alSections);
		}

		if(!pVtxBuffer->Compile(eVertexCompileFlag_CreateTangents))
		{
			hplDelete(pVtxBuffer);
			pVtxBuffer = NULL;
		}

		cMesh* pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()));
		cSubMesh* pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreateCapsule(const tString &asName, const cVector2f &avSize, int alSections, int alSlices, const tString &asMaterial)
	{
		iVertexBuffer* pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, 
																	eVertexBufferUsageType_Static);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		float fAngleStep = k2Pif/(float)alSections;
		cColor col = cColor(1,1);

		float fRadius = avSize.x;
		float fHalfHeight = avSize.y*0.5f;
		float fCylinderHalfHeight = fHalfHeight-fRadius;
		fCylinderHalfHeight = cMath::Clamp(fCylinderHalfHeight, 0, fCylinderHalfHeight);

		float fInvSlices = 1.0f/alSlices;
		float fSectionStep = k2Pif/(float)alSections;

		// North pole
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0, cMath::Max(fHalfHeight, fRadius), 0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0, 1, 0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		// Slices
		float fHalfPi		= kPif*0.5f;
		float fHalfPiOverSlices = fHalfPi*fInvSlices;
		for(int i=0;i<alSlices-1; ++i)
		{
			float fHeight = fCylinderHalfHeight + fRadius - fRadius*(1+sin(fHalfPiOverSlices*(float)(i+1)-fHalfPi));
			float fAngle = asin((fHeight-fCylinderHalfHeight)/fRadius);
			tVector3fVec vVertices;

			CreateCircumference(avSize.x*cos(fAngle), fSectionStep, fHeight, vVertices);
			for(int j=0;j<(int)vVertices.size();++j)
			{
				const cVector3f& vVertex = vVertices[j];
				cVector3f vNormal = vVertex - fCylinderHalfHeight;
				vNormal.Normalize();

				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
				pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
			}
		}

		{
			tVector3fVec vCylinderVertices;
			CreateCircumference(avSize.x, fSectionStep, 0, vCylinderVertices);
			float vCylinderHeights[] = { fCylinderHalfHeight, -fCylinderHalfHeight };
			for(int i=0;i<2;++i)
			{
				float fHeight = vCylinderHeights[i];
				for(int j=0;j<(int)vCylinderVertices.size();++j)
				{
					cVector3f vVertex = vCylinderVertices[j];
					vVertex.y = fHeight;
					cVector3f vNormal = vVertex;
					vNormal.Normalize();

					pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
					pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
					pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
					pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
				}
			}
		}

		for(int i=alSlices-2;i>=0; --i)
		{
			// Same absolute height and radius than upper dome, only reverse order

			float fHeight = fCylinderHalfHeight + fRadius - fRadius*(1+sin(fHalfPiOverSlices*(float)(i+1)-fHalfPi));
			float fAngle = asin((fHeight-fCylinderHalfHeight)/fRadius);
			tVector3fVec vVertices;

			// and height is negative here
			CreateCircumference(avSize.x*cos(fAngle), fSectionStep, -fHeight, vVertices);
			for(int j=0;j<(int)vVertices.size();++j)
			{
				const cVector3f& vVertex = vVertices[j];
				cVector3f vNormal = vVertex - fCylinderHalfHeight;
				vNormal.Normalize();

				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
				pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
				pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
			}
		}

		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0,cMath::Min(-fHalfHeight, -fRadius),0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0,-1,0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		WrapUpperCap(pVtxBuffer, 0, 1, alSections);
		for(int i=0;i<2*alSlices-1;++i)
		{
			WrapSides(pVtxBuffer, 1 + i*alSections, alSections);
		}

		int lLastVertex = pVtxBuffer->GetVertexNum()-1;
		WrapLowerCap(pVtxBuffer, lLastVertex, lLastVertex-alSections, alSections);

		if(!pVtxBuffer->Compile(eVertexCompileFlag_CreateTangents))
		{
			hplDelete(pVtxBuffer);
			pVtxBuffer = NULL;
		}

		cMesh* pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()));
		cSubMesh* pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreateCone(const tString &asName, const cVector2f &avSize, int alSections, const tString &asMaterial)
	{
		//////////////////////////////////////////
		// Create Vertex Buffer
		iVertexBuffer* pVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, 
																	eVertexBufferUsageType_Static);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		////////////////////////
		// Set up variables
		float fAngleStep = k2Pif/(float)alSections;
		float fHalfHeight = avSize.y*0.5f;
		cColor col = cColor(1,1);

		/////////////////////////
		// Create apex vertex
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0,fHalfHeight,0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0,1,0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		///////////////////////////////////////////
		// Create vertices for base
		tVector3fVec vVertices;
		CreateCircumference(avSize.x, fAngleStep, -fHalfHeight, vVertices);

		for(int i=0;i<(int)vVertices.size();++i)
		{
			const cVector3f& vVertex = vVertices[i];
			cVector3f vNormal = vVertex;
			vNormal.Normalize();

			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vVertex);
			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, vNormal);
			pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
			pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));
		}

		// Base center vertex
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, cVector3f(0,-fHalfHeight,0));
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal, cVector3f(0,-1,0));
		pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, col);
		pVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0));

		////////////////////////////
		// Triangles for sides
		{
			WrapUpperCap(pVtxBuffer, 0, 1, alSections);
		}

		////////////////////////////
		// Triangles for base
		{
			int lLastVertex = pVtxBuffer->GetVertexNum()-1;
			int lCapStart = lLastVertex-alSections;
			WrapLowerCap(pVtxBuffer, lLastVertex, lCapStart, alSections);
		}

		if(!pVtxBuffer->Compile(eVertexCompileFlag_CreateTangents))
		{
			hplDelete(pVtxBuffer);
			pVtxBuffer = NULL;
		}

		cMesh* pMesh = hplNew( cMesh, (asName, _W(""), mpResources->GetMaterialManager(), mpResources->GetAnimationManager()));
		cSubMesh* pSubMesh = pMesh->CreateSubMesh("Main");

		cMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshCreator::CreateSkyBoxVertexBuffer(float afSize)
	{
		iVertexBuffer* pSkyBox = mpLowLevelGraphics->CreateVertexBuffer(
										eVertexBufferType_Hardware,
										eVertexBufferDrawType_Quad,eVertexBufferUsageType_Static);
		pSkyBox->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pSkyBox->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);
		pSkyBox->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);

					
		float fSize = afSize;

		for(int x=-1; x<=1;x++)
			for(int y=-1; y<=1;y++)
				for(int z=-1; z<=1;z++)
				{
					if(x==0 && y==0 && z==0)continue;
					if(std::abs(x) + std::abs(y) + std::abs(z) > 1)continue;

					//Direction (could say inverse normal) of the quad.
					cVector3f vDir(0);
					cVector3f vSide(0);

					cVector3f vAdd[4];
					if(std::abs(x)){
						vDir.x = (float)x;

						vAdd[0].y = 1;	vAdd[0].z = 1;	vAdd[0].x =0;
						vAdd[1].y = -1;	vAdd[1].z = 1;	vAdd[1].x =0;
						vAdd[2].y = -1;	vAdd[2].z = -1;	vAdd[2].x =0;
						vAdd[3].y = 1;	vAdd[3].z = -1;	vAdd[3].x =0;
					}
					else if(std::abs(y)){
						vDir.y = (float)y;

						vAdd[0].z = 1;	vAdd[0].x = 1;	vAdd[0].y =0;
						vAdd[1].z = -1;	vAdd[1].x = 1;	vAdd[1].y =0;
						vAdd[2].z = -1;	vAdd[2].x = -1;	vAdd[2].y =0;
						vAdd[3].z = 1;	vAdd[3].x = -1;	vAdd[3].y =0;
					}
					else if(std::abs(z)){
						vAdd[0].y = 1;	vAdd[0].x = 1;	vAdd[0].z =0;
						vAdd[1].y = 1;	vAdd[1].x = -1;	vAdd[1].z =0;
						vAdd[2].y = -1;	vAdd[2].x = -1;	vAdd[2].z =0;
						vAdd[3].y = -1;	vAdd[3].x = 1;	vAdd[3].z =0;

						vDir.z = (float)z;
					}


					//Log("Side: (%.0f : %.0f : %.0f) [ ", vDir.x,  vDir.y,vDir.z);
					for(int i=0;i<4;i++)
					{
						int idx = i;
						if(x + y + z < 0) idx = 3-i;

						pSkyBox->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1,1,1));
						pSkyBox->AddVertexVec3f(eVertexBufferElement_Position, (vDir+vAdd[idx])*fSize);
						pSkyBox->AddVertexVec3f(eVertexBufferElement_Texture0, vDir+vAdd[idx]);
                        
						vSide = vDir+vAdd[idx];
						//Log("%d: (%.1f : %.1f : %.1f) ", i,vSide.x,  vSide.y,vSide.z);
					}
					//Log("\n");
				}

		for(int i=0;i<24;i++) pSkyBox->AddIndex(i);
		
		if(!pSkyBox->Compile(0))
		{
			hplDelete(pSkyBox);
			return NULL;
		}
		return pSkyBox;
	}
	
	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshCreator::CreateBoxVertexBuffer(cVector3f avSize)
	{
		iVertexBuffer* pBox = mpLowLevelGraphics->CreateVertexBuffer(
			eVertexBufferType_Hardware,
			eVertexBufferDrawType_Tri,eVertexBufferUsageType_Static);
		pBox->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		pBox->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		pBox->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		pBox->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);


		avSize = avSize*0.5;

		int lVtxIdx =0;

		for(int x=-1; x<=1;x++)
			for(int y=-1; y<=1;y++)
				for(int z=-1; z<=1;z++)
				{
					if(x==0 && y==0 && z==0)continue;
					if(std::abs(x) + std::abs(y) + std::abs(z) > 1)continue;

					//Direction (could say inverse normal) of the quad.
					cVector3f vDir(0);
					cVector3f vSide(0);
					
					cVector3f vAdd[4];
					if(std::abs(x)){
						vDir.x = (float)x;

						vAdd[0].y = 1;	vAdd[0].z = 1;	vAdd[0].x = 0;
						vAdd[1].y = -1;	vAdd[1].z = 1;	vAdd[1].x = 0;
						vAdd[2].y = -1;	vAdd[2].z = -1;	vAdd[2].x = 0;
						vAdd[3].y = 1;	vAdd[3].z = -1;	vAdd[3].x = 0;
					}
					else if(std::abs(y)){
						vDir.y = (float)y;

						vAdd[0].z = 1;	vAdd[0].x = 1;	vAdd[0].y = 0;
						vAdd[1].z = -1;	vAdd[1].x = 1;	vAdd[1].y = 0;
						vAdd[2].z = -1;	vAdd[2].x = -1;	vAdd[2].y = 0;
						vAdd[3].z = 1;	vAdd[3].x = -1;	vAdd[3].y = 0;
					}
					else if(std::abs(z)){
						vAdd[0].y = 1;	vAdd[0].x = 1;	vAdd[0].z = 0;
						vAdd[1].y = 1;	vAdd[1].x = -1;	vAdd[1].z = 0;
						vAdd[2].y = -1;	vAdd[2].x = -1;	vAdd[2].z = 0;
						vAdd[3].y = -1;	vAdd[3].x = 1;	vAdd[3].z = 0;

						vDir.z = (float)z;
					}


					//Log("Side: (%.0f : %.0f : %.0f) [ ", vDir.x,  vDir.y,vDir.z);
					for(int i=0;i<4;i++)
					{
						int idx = GetBoxIdx(i,x,y,z);
						cVector3f vTex = GetBoxTex(i,x,y,z,vAdd);
						
						pBox->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1,1,1));
						pBox->AddVertexVec3f(eVertexBufferElement_Position, (vDir+vAdd[idx])*avSize);
						pBox->AddVertexVec3f(eVertexBufferElement_Normal, vDir);

						//texture coord
						cVector3f vCoord = cVector3f((vTex.x+1)*0.5f,(vTex.y+1)*0.5f,0);
						pBox->AddVertexVec3f(eVertexBufferElement_Texture0,vCoord);
						
						vSide = vDir+vAdd[idx];
						//Log("%d: Tex: (%.1f : %.1f : %.1f) ", i,vTex.x,  vTex.y,vTex.z);
						//Log("%d: (%.1f : %.1f : %.1f) ", i,vSide.x,  vSide.y,vSide.z);
					}

					for(int i=0;i<3;i++)pBox->AddIndex(lVtxIdx + i);
					pBox->AddIndex(lVtxIdx + 2);
					pBox->AddIndex(lVtxIdx + 3);
					pBox->AddIndex(lVtxIdx + 0);

					lVtxIdx +=4;

					//Log("\n");
				}
		
		if(!pBox->Compile(eVertexCompileFlag_CreateTangents))
		{
			hplDelete(pBox);
			return NULL;
		}
		return pBox;
	}

	cVector3f cMeshCreator::GetBoxTex(int i,int x, int y, int z, cVector3f *vAdd)
	{
		cVector3f vTex;

		if(std::abs(x)){
			vTex.x = vAdd[i].z;
			vTex.y = vAdd[i].y;
		}
		else if(std::abs(y)){
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].z;
		}
		else if(std::abs(z)){
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].y;
		}
		
		//Inverse for negative directions
		if(x+y+z <0)
		{
			vTex.x = -vTex.x;
			vTex.y = -vTex.y;
		}

		return vTex;
	}
	int cMeshCreator::GetBoxIdx(int i,int x, int y, int z)
	{
		int idx = i;
		if(x + y + z > 0) idx = 3-i;

		return idx;
	}

	//-----------------------------------------------------------------------

	void cMeshCreator::CreateCircumference(float afRadius, float afAngleStep, float afHeight, tVector3fVec& avVertices)
	{
		float fEnd = k2Pif-afAngleStep;
		for(float fAngle=0; fAngle<=fEnd; fAngle+=afAngleStep)
		{
			cVector3f vPoint = cVector3f(cMath::RoundFloatToDecimals(afRadius*cos(fAngle), 6), 
										 cMath::RoundFloatToDecimals(afHeight, 6), 
										 cMath::RoundFloatToDecimals(afRadius*sin(fAngle),6));
			avVertices.push_back(vPoint);
		}
	}

	//-----------------------------------------------------------------------

	void cMeshCreator::WrapSides(iVertexBuffer* apVtxBuffer, int alStartVertexIdx,int alSections)
	{
		/////////////////////////////////////////////////
		// Create indices  like this		0 --- 1 --- 2 --- ... --- 0
		//									|   / |   / |
		//									|  /  |  /  |
		//									| /	  | /   |
		//									Se---Se+1---Se+2---2S-1--- S


		for(int i=0;i<alSections-1;++i)
		{
			int lPoint0 = alStartVertexIdx+i;
			int lPoint1 = lPoint0+1;
			int lPoint2 = lPoint0+alSections;
			int lPoint3 = lPoint2+1;

			apVtxBuffer->AddIndex(lPoint0);
			apVtxBuffer->AddIndex(lPoint2);
			apVtxBuffer->AddIndex(lPoint1);

			apVtxBuffer->AddIndex(lPoint1);
			apVtxBuffer->AddIndex(lPoint2);
			apVtxBuffer->AddIndex(lPoint3);
		}

		{
			int lPoint0 = alStartVertexIdx+alSections-1;
			int lPoint1 = alStartVertexIdx;
			int lPoint2 = lPoint0+alSections;
			int lPoint3 = lPoint0+1;

			apVtxBuffer->AddIndex(lPoint0);
			apVtxBuffer->AddIndex(lPoint2);
			apVtxBuffer->AddIndex(lPoint1);

			apVtxBuffer->AddIndex(lPoint1);
			apVtxBuffer->AddIndex(lPoint2);
			apVtxBuffer->AddIndex(lPoint3);

			//Log("%d %d %d\t %d %d %d\n", lPoint0, lPoint2, lPoint1,
			//						lPoint1, lPoint2, lPoint3);
		}
	}

	void cMeshCreator::WrapUpperCap(iVertexBuffer* apVtxBuffer, int alCenterVertexIdx, int alStartVertexIdx, int alSections)
	{
		for(int i=0;i<alSections-1;++i)
		{
			int lBase = alStartVertexIdx+i;
			apVtxBuffer->AddIndex(alCenterVertexIdx);
			apVtxBuffer->AddIndex(lBase);
			apVtxBuffer->AddIndex(lBase+1);
		}
		apVtxBuffer->AddIndex(alCenterVertexIdx);
		apVtxBuffer->AddIndex(alStartVertexIdx+alSections-1);
		apVtxBuffer->AddIndex(alStartVertexIdx);
	}

	void cMeshCreator::WrapLowerCap(iVertexBuffer* apVtxBuffer, int alCenterVertexIdx, int alStartVertexIdx, int alSections)
	{
		for(int i=0;i<alSections-1;++i)
		{
			int lBase = alStartVertexIdx+i;
			apVtxBuffer->AddIndex(alCenterVertexIdx);
			apVtxBuffer->AddIndex(lBase+1);
			apVtxBuffer->AddIndex(lBase);
		}

		apVtxBuffer->AddIndex(alCenterVertexIdx);
		apVtxBuffer->AddIndex(alStartVertexIdx);
		apVtxBuffer->AddIndex(alStartVertexIdx+alSections-1);
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

}
