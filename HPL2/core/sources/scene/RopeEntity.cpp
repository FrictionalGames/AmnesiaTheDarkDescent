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

#include "scene/RopeEntity.h"

#include "math/Math.h"

#include "graphics/Graphics.h"
#include "graphics/Material.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"

#include "resources/Resources.h"
#include "resources/MaterialManager.h"

#include "scene/Camera.h"
#include "scene/World.h"
#include "scene/Scene.h"

#include "physics/PhysicsRope.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRopeEntity::cRopeEntity(const tString& asName, cResources *apResources,cGraphics *apGraphics,
								iPhysicsRope *apRope, int alMaxSegments) :	iRenderable(asName)
	{
		mpMaterialManager = apResources->GetMaterialManager();
		mpLowLevelGraphics = apGraphics->GetLowLevel();
		
		mColor = cColor(1,1,1,1);

		mpMaterial = NULL;
		
		mpRope = apRope;
		mlMaxSegments = alMaxSegments;

		mfRadius = mpRope->GetParticleRadius();
		mfLengthTileAmount = 1;
		mfLengthTileSize = 1;

		mpVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(	eVertexBufferType_Hardware, eVertexBufferDrawType_Tri, eVertexBufferUsageType_Dynamic,
																4 * mlMaxSegments, 6 * mlMaxSegments);
		
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		for(int i=0; i<mlMaxSegments; ++i)
		{
			cVector3f vTexCoords[4] = {cVector3f(1,1,0),	//Bottom left
										cVector3f(-1,1,0),	//Bottom right
										cVector3f(-1,-1,0),	//Top left
										cVector3f(1,-1,0)};	//Top right

			for(int j=0;j<4;j++)
			{
				mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position,	0);
				mpVtxBuffer->AddVertexColor(eVertexBufferElement_Color0,	mColor);
				mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, 0);
				mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal,	cVector3f(0,0,1));
			}

			for(int j=0;j<3;j++) mpVtxBuffer->AddIndex(j + i*4);
			for(int j=2;j<5;j++) mpVtxBuffer->AddIndex( (j==4?0:j)  + i*4);
		}

		mpVtxBuffer->Compile(eVertexCompileFlag_CreateTangents);

		mbApplyTransformToBV = false;

		mlLastUpdateCount = -1;
	}

	//-----------------------------------------------------------------------

	cRopeEntity::~cRopeEntity()
	{
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		if(mpVtxBuffer) hplDelete(mpVtxBuffer);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRopeEntity::SetMultiplyAlphaWithColor(bool abX)
	{
		if(mbMultiplyAlphaWithColor == abX) return;

		mbMultiplyAlphaWithColor = abX;
	}

	//-----------------------------------------------------------------------

	void cRopeEntity::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Color0);

		cColor finalColor = mColor;
		if(mbMultiplyAlphaWithColor)
		{
			finalColor.r = finalColor.r * mColor.a;
			finalColor.g = finalColor.g * mColor.a;
			finalColor.b = finalColor.b * mColor.a;
		}

		for(int i=0; i<mlMaxSegments * 4; ++i)
		{
			pColors[0] = finalColor.r;
			pColors[1] = finalColor.g;
			pColors[2] = finalColor.b;
			pColors[3] = finalColor.a;
			pColors+=4;
		}
		
		mpVtxBuffer->UpdateData(eVertexElementFlag_Color0,false);
	}

	//-----------------------------------------------------------------------

	void cRopeEntity::SetMaterial(cMaterial * apMaterial)
	{
		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cRopeEntity::GetBoundingVolume()
	{
		if(mlLastUpdateCount != mpRope->GetUpdateCount())
		{
			cVector3f vMin(1000000.0f);
			cVector3f vMax(-1000000.0f);

			cVerletParticleIterator it = mpRope->GetParticleIterator();
			while(it.HasNext())
			{
				cVerletParticle *pPart = it.Next();
				
				cMath::ExpandAABB(vMin,vMax, pPart->GetPosition(), pPart->GetPosition());
			}
			
			mBoundingVolume.SetLocalMinMax(vMin-cVector3f(mfRadius),vMax+cVector3f(mfRadius));

			mlLastUpdateCount = mpRope->GetUpdateCount();
		}

		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	void cRopeEntity::UpdateGraphicsForFrame(float afFrameTime)
	{
		
	}

	//-----------------------------------------------------------------------

	static inline void SetVec3(float *apPos, const cVector3f &aPos)
	{
		apPos[0] = aPos.x;
		apPos[1] = aPos.y;
		apPos[2] = aPos.z;
	}

	static inline void SetVec4(float *apPos, const cVector3f &aPos)
	{
		apPos[0] = aPos.x;
		apPos[1] = aPos.y;
		apPos[2] = aPos.z;
		apPos[3] = 1;
	}

	//-----------------------------------------------------------------------

	static cVector2f gvPosAdd[4] = {
		//cVector2f (1,1), cVector2f (1,0), cVector2f (-1,0), cVector2f (-1,1)
		cVector2f (1,0), cVector2f (-1,0), cVector2f (-1,1), cVector2f (1,1)
	};

	bool cRopeEntity::UpdateGraphicsForViewport(cFrustum *apFrustum,float afFrameTime)
	{
		float *pPosArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
		float *pUvArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture0);
		float *pNrmArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Normal);
		float *pTanArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture1Tangent);

		float fSegmentLength = mpRope->GetSegmentLength();

		cVector3f vTexCoords[4] = {	
				cVector3f(1,1,0),	//Bottom left
				cVector3f(0,1,0),	//Bottom right
				cVector3f(0,0,0),	//Top left
				cVector3f(1,0,0)	//Top right
		};

		vTexCoords[0].y *= mfLengthTileAmount;
		vTexCoords[1].y *= mfLengthTileAmount;

		cVerletParticleIterator it = mpRope->GetParticleIterator();
		int lCount=0;
		cVector3f vPrevPos;
		while(it.HasNext())
		{
			if(lCount >= mlMaxSegments) break;
			++lCount;

			cVerletParticle *pPart = it.Next();

			if(lCount == 1){
				vPrevPos = pPart->GetPosition();
				continue; 
			}

			/////////////////////////
			//Calculate properties
			cVector3f vPos = pPart->GetSmoothPosition();
			cVector3f vDelta = vPos - vPrevPos;
			float fLength = vDelta.Length();
			cVector3f vUp = vDelta / fLength;
			cVector3f vRight = cMath::Vector3Normalize(cMath::Vector3Cross(vUp, apFrustum->GetForward()));
			cVector3f vFwd = cMath::Vector3Cross(vRight, vUp);

			/////////////////////////
			//Update position
			for(int i=0; i<4; ++i)
				SetVec4(&pPosArray[i*4], vPrevPos + vRight * gvPosAdd[i].x*mfRadius + vUp * gvPosAdd[i].y*fLength);
			
			/////////////////////////
			//Update uv
			if(lCount==2 && (fLength < fSegmentLength || fSegmentLength==0))
			{
				//////////////////
				//No segments
				if(fSegmentLength==0)
				{
					float fYAdd = 1 - fLength/ mfLengthTileSize;

					SetVec3(&pUvArray[0*3], vTexCoords[0] - cVector3f(0,fYAdd,0));
					SetVec3(&pUvArray[1*3], vTexCoords[1] - cVector3f(0,fYAdd,0));

					SetVec3(&pUvArray[2*3], vTexCoords[2]);
					SetVec3(&pUvArray[3*3], vTexCoords[3]);
				}
				//////////////////
				//First segment of many
				else
				{
					float fYAdd = (1 - (fLength / fSegmentLength))*mfLengthTileAmount;

					SetVec3(&pUvArray[0*3], vTexCoords[0] - cVector3f(0,fYAdd,0) );
					SetVec3(&pUvArray[1*3], vTexCoords[1] - cVector3f(0,fYAdd,0) );

					SetVec3(&pUvArray[2*3], vTexCoords[2]);
					SetVec3(&pUvArray[3*3], vTexCoords[3]);
				}
			}
			else
			{
				for(int i=0; i<4; ++i)
					SetVec3(&pUvArray[i*3], vTexCoords[i]);
			}

			/////////////////////////
			//Update Normal and Tangent
			for(int i=0; i<4; ++i)
			{
				SetVec3(&pNrmArray[i*3], vFwd);
				SetVec4(&pTanArray[i*4], vRight);
			}
						
			/////////////////////////
			//Update pointers
			pPosArray += 4 * 4;
			pUvArray +=	 3 * 4;
			pNrmArray += 3 * 4;
			pTanArray += 4 * 4;

			/////////////////////////
			//Update misc
			vPrevPos = vPos;
		}

		mpVtxBuffer->SetElementNum((lCount-1) * 6);

		mpVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Texture0 | eVertexElementFlag_Texture1 | eVertexElementFlag_Normal, false);

		return true;
	}

	//-----------------------------------------------------------------------

	cMatrixf* cRopeEntity::GetModelMatrix(cFrustum *apFrustum)
	{
		if(apFrustum==NULL)return &GetWorldMatrix();
		
		return NULL;
	}

	//-----------------------------------------------------------------------

	int cRopeEntity::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//-----------------------------------------------------------------------

	bool cRopeEntity::IsVisible()
	{
		if(mColor.r <= 0 && mColor.g <= 0 && mColor.b <= 0) return false;

		return mbIsVisible;
	}

	
	//-----------------------------------------------------------------------


	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}
