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

#include "scene/ParticleEmitter.h"

#include "system/LowLevelSystem.h"

#include "resources/Resources.h"
#include "resources/MaterialManager.h"

#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Renderer.h"

#include "scene/Camera.h"
#include "math/Math.h"

#include "engine/Engine.h"
#include "scene/Scene.h"
#include "scene/World.h"

#include "scene/ParticleSystem.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DATA LOADER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iParticleEmitterData::iParticleEmitterData(const tString &asName,cResources* apResources,
		cGraphics *apGraphics)
	{
		msName = asName;
		mpResources = apResources;
		mpGraphics = apGraphics;

		mfWarmUpTime =0;
		mfWarmUpStepsPerSec = 20;
	}

	//-----------------------------------------------------------------------

	iParticleEmitterData::~iParticleEmitterData()
	{
		for(int i=0;i<(int)mvMaterials.size();i++)
		{
			if(mvMaterials[i]) mpResources->GetMaterialManager()->Destroy(mvMaterials[i]);
		}
	}
	//-----------------------------------------------------------------------

	void iParticleEmitterData::AddMaterial(cMaterial *apMaterial)
	{
		mvMaterials.push_back(apMaterial);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iParticleEmitter::iParticleEmitter(tString asName,tMaterialVec *avMaterials,
										unsigned int alMaxParticles, cVector3f avSize,
										cGraphics *apGraphics,cResources *apResources) 
		:iRenderable(asName)
	{

		mpGraphics = apGraphics;
		mpResources = apResources;

		/////////////////////////////////////
		//Create and set up particle data
		mvParticles.resize(alMaxParticles);
		for(int i=0;i<(int)alMaxParticles;i++)
		{
			mvParticles[i] = hplNew( cParticle, () );
		}
		mlMaxParticles = alMaxParticles;
		mlNumOfParticles =0;

		mvMaterials = avMaterials;

		//////////////////////////////////
		//Create vertex buffer
		mpVtxBuffer = apGraphics->GetLowLevel()->CreateVertexBuffer(eVertexBufferType_Hardware,
																	eVertexBufferDrawType_Tri, eVertexBufferUsageType_Stream,
																	alMaxParticles*4, alMaxParticles*6);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);
		
		//////////////////////////////////
		//Fill the indices with quads
		for(int i=0;i<(int)alMaxParticles;i++)
		{
			int lStart = i*4;
			for(int j=0;j<3;j++) mpVtxBuffer->AddIndex(lStart + j);
			for(int j=2;j<5;j++) mpVtxBuffer->AddIndex(lStart + (j==4?0:j));
		}

		//////////////////////////////////
		//Fill with texture coords (will do for most particle systems)
		for(int i=0;i<(int)alMaxParticles;i++)
		{
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(1,1,0));
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0,1,0));
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(0,0,0));
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, cVector3f(1,0,0));
		}

		//////////////////////////////////
		//Set default values for pos and col
		for(int i=0;i<(int)alMaxParticles*4;i++){
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, 0);
			mpVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1));
		}
		
		////////////////////////////////////
		// Compile vertex buffer
		mpVtxBuffer->Compile(0);

		
		////////////////////////////////////
		//Setup vars
		mlSleepCount = 60*5; //Start with high sleep count to make sure a looping particle system reaches equilibrium. 5 secs should eb enough!

		mbDying = false;
		mfFrame =0;

		mbUpdateGfx = true;
		mbUpdateBV = true;

		mlDirectionUpdateCount = -1;
		mvDirection = cVector3f(0,0,0);

		mvMaxDrawSize =0;

		mlAxisDrawUpdateCount = -1;

		mbApplyTransformToBV = false; 

		mBoundingVolume.SetSize(0);
		mBoundingVolume.SetPosition(0);

		mDrawType = eParticleEmitterType_FixedPoint;
		mCoordSystem = eParticleEmitterCoordSystem_World;
		
		mbUsesDirection = false;	//If Direction should be udpdated
		
	}

	//-----------------------------------------------------------------------

	iParticleEmitter::~iParticleEmitter()
	{
		for(int i=0;i<(int)mvParticles.size();i++)
		{
			hplDelete(mvParticles[i]);
		}

		hplDelete(mpVtxBuffer);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iParticleEmitter::SetSubDivUV(const cVector2l &avSubDiv)
	{
		//Check so that there is any subdivision and that no sub divison axis is
		//equal or below zero
		if( (avSubDiv.x > 1 || avSubDiv.x > 1) && (avSubDiv.x >0 && avSubDiv.y >0))
		{
			int lSubDivNum = avSubDiv.x * avSubDiv.y;

			mvSubDivUV.resize(lSubDivNum);

			float fInvW = 1.0f / (float)avSubDiv.x;
			float fInvH = 1.0f / (float)avSubDiv.y;

			for(int x=0; x < avSubDiv.x; ++x)
				for(int y=0; y < avSubDiv.y; ++y)
				{
					int lIdx = y*avSubDiv.x + x;

					float fX = (float)x;
					float fY = (float)y;

					cPESubDivision *pSubDiv = &mvSubDivUV[lIdx];

					pSubDiv->mvUV[0] = cVector3f( (fX +1)*fInvW,	(fY +1)*fInvH,0);	//1,1
					pSubDiv->mvUV[1] = cVector3f( fX*fInvW,			(fY +1)*fInvH,0);	//0,1
					pSubDiv->mvUV[2] = cVector3f( fX*fInvW,			fY*fInvH,0);		//0,0
					pSubDiv->mvUV[3] = cVector3f( (fX +1)*fInvW,	fY*fInvH,0);		//1,0
				}
		}

	}

	//-----------------------------------------------------------------------

	//Seems like this fucntion is never called any more...
	void iParticleEmitter::UpdateLogic(float afTimeStep)
	{
		if(IsActive()==false) return;

		//////////////////////////////
		// Update sleep
		if(IsDying()==false && iRenderer::GetRenderFrameCount() != mlRenderFrameCount)
		{
			if(mlSleepCount <= 0)return;
			mlSleepCount--;
		}
		else
		{
			if(mlSleepCount<10) mlSleepCount =10;
		}

		//////////////////////////////
		// Update vars
		mbUpdateGfx = true;
		mbUpdateBV = true;

		//////////////////////////////
		// Update direction
		if(mbUsesDirection)
		{
			if(mlDirectionUpdateCount != GetMatrixUpdateCount())
			{
				mlDirectionUpdateCount = GetMatrixUpdateCount();
				cMatrixf mtxInv = cMath::MatrixInverse(GetWorldMatrix());
				mvDirection = mtxInv.GetUp();
			}
		}

		UpdateMotion(afTimeStep);

		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void iParticleEmitter::KillInstantly()
	{
		mlMaxParticles = 0;
		mlNumOfParticles = 0;
		mbDying = true;
	}

	//-----------------------------------------------------------------------

	cMaterial* iParticleEmitter::GetMaterial()
	{
		return (*mvMaterials)[(int)mfFrame];
	}

	//-----------------------------------------------------------------------

	static inline void SetCol(float *apCol, const cColor &aCol)
	{
		apCol[0] = aCol.r;
		apCol[1] = aCol.g;
		apCol[2] = aCol.b;
		apCol[3] = aCol.a;
	}

	static inline void SetPos(float *apPos, const cVector3f &aPos)
	{
		apPos[0] = aPos.x;
		apPos[1] = aPos.y;
		apPos[2] = aPos.z;
	}

	static inline void SetTex(float *apTex, const cVector3f &aPos)
	{
		apTex[0] = aPos.x;
		apTex[1] = aPos.y;
		apTex[2] = aPos.z;
	}

	bool iParticleEmitter::UpdateGraphicsForViewport(cFrustum *apFrustum,float afFrameTime)
	{
		//if(mbUpdateGfx == false) return;

		//////////////////////////
		// Get Data
		float *pPosArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
		float *pColArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Color0);


		//////////////////////////
		// Set up color mul
        cColor colorMul = mpParentSystem->mColor;
		
		//Set alpha based on fade distance.
		if(mpParentSystem->mbFadeAtDistance)
		{
			float fDistSqr = cMath::Vector3DistSqr(mpParentSystem->GetBoundingVolume()->GetWorldCenter(), apFrustum->GetOrigin());
			float fMinStart = mpParentSystem->mfMinFadeDistanceStart;
			float fMaxStart = mpParentSystem->mfMaxFadeDistanceStart;
			float fMinEnd = mpParentSystem->mfMinFadeDistanceEnd;
			float fMaxEnd = mpParentSystem->mfMaxFadeDistanceEnd;

			///////////
			// Below min
			if(fMinStart >0 && fDistSqr < fMinStart* fMinStart)
			{
				if(fDistSqr <= fMinEnd * fMinEnd)
				{
					colorMul.a = 0;
				}
				else
				{
					float fDist = sqrt(fDistSqr);
                    colorMul.a *= (fDist - fMinEnd)/(fMinStart - fMinEnd);
				}
			}
			///////////
			// Above max
			if(fMaxStart >0 && fDistSqr > fMaxStart* fMaxStart)
			{
				if(fDistSqr >= fMaxEnd * fMaxEnd)
				{
					colorMul.a = 0;
				}
				else
				{
					float fDist = sqrt(fDistSqr);
					colorMul.a *= 1 - (fDist - fMaxStart)/(fMaxEnd - fMaxStart);
				}
			}
		}
		//Change color based on alpha
		if(mbMultiplyRGBWithAlpha)
		{
			colorMul.r *= colorMul.a;
			colorMul.g *= colorMul.a;
			colorMul.b *= colorMul.a;
		}

		//////////////////////////////
		// If alpha is 0, skip rendering anything
		if(colorMul.a <= 0)
		{
			mpVtxBuffer->SetElementNum(0);
			return false;
		}
		
		//////////////////////////////
		// RENDERING

		if ( mPEType == ePEType_Beam)
		{
			//				for (int i=0; i<(int)mlNumOfParticles; i++)
			//				{
			//					cParticle *pParticle = mvParticles[i];

			//					for (int j = 0; j < (int) pParticle->mvBeamPoints->size(); j++)
			//					{

			//					}
			//				}

		}
		else
		{
			//////////////////////////////////////////////////
			// SUB DIVISION SET UP
			if(mvSubDivUV.size() > 1)
			{
				float *pTexArray = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture0);	

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					cPESubDivision &subDiv = mvSubDivUV[pParticle->mlSubDivNum];

					SetTex(&pTexArray[i*12 + 0*3],subDiv.mvUV[0]);
					SetTex(&pTexArray[i*12 + 1*3],subDiv.mvUV[1]);
					SetTex(&pTexArray[i*12 + 2*3],subDiv.mvUV[2]);
					SetTex(&pTexArray[i*12 + 3*3],subDiv.mvUV[3]);

					/*SetTex(&pTexArray[i*12 + 0*3], cVector3f(1,1,0));
					SetTex(&pTexArray[i*12 + 1*3], cVector3f(0,1,0));
					SetTex(&pTexArray[i*12 + 2*3], cVector3f(0,0,0));
					SetTex(&pTexArray[i*12 + 3*3], cVector3f(1,0,0));*/
				}
			}

			//////////////////////////////////////////////////
			// FIXED POINT
			if(mDrawType == eParticleEmitterType_FixedPoint)
			{
				cVector3f vAdd[4] = {
					cVector3f( mvDrawSize.x,-mvDrawSize.y,0),
						cVector3f(-mvDrawSize.x,-mvDrawSize.y,0),
						cVector3f(-mvDrawSize.x, mvDrawSize.y,0),
						cVector3f( mvDrawSize.x, mvDrawSize.y,0)
				};
				
				//If this is a reflection, need to invert the ordering.
				if(apFrustum->GetInvertsCullMode())
				{
					for(int i=0; i<4; ++i)	vAdd[i].y = -vAdd[i].y;
				}

				// NEW

				// ---


				int lVtxStride = mpVtxBuffer->GetElementNum(eVertexBufferElement_Position);
				int lVtxQuadSize = lVtxStride*4;

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					//This is not the fastest thing possible...
					cVector3f vParticlePos = pParticle->mvPos;	

					if(mCoordSystem == eParticleEmitterCoordSystem_Local){
						vParticlePos = cMath::MatrixMul(mpParentSystem->GetWorldMatrix(), vParticlePos);
					}

					cVector3f vPos = cMath::MatrixMul(apFrustum->GetViewMatrix(), vParticlePos);
					cColor finalColor = pParticle->mColor * colorMul;

					SetPos(&pPosArray[i*lVtxQuadSize + 0*lVtxStride], vPos + vAdd[0]);
					SetCol(&pColArray[i*16 + 0*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 1*lVtxStride], vPos + vAdd[1]);
					SetCol(&pColArray[i*16 + 1*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 2*lVtxStride], vPos + vAdd[2]);
					SetCol(&pColArray[i*16 + 2*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 3*lVtxStride], vPos + vAdd[3]);
					SetCol(&pColArray[i*16 + 3*4], finalColor);
				}
			}
			//////////////////////////////////////////////////
			// DYNAMIC POINT
			else if(mDrawType == eParticleEmitterType_DynamicPoint)
			{
				cVector3f vAdd[4] = {
						cVector3f( mvDrawSize.x,-mvDrawSize.y,0),
						cVector3f(-mvDrawSize.x,-mvDrawSize.y,0),
						cVector3f(-mvDrawSize.x, mvDrawSize.y,0),
						cVector3f( mvDrawSize.x, mvDrawSize.y,0)
				};

				//If this is a reflection, need to invert the ordering.
				if(apFrustum->GetInvertsCullMode())
				{
					for(int i=0; i<4; ++i)	vAdd[i].y = -vAdd[i].y;
				}
				
				int lVtxStride = mpVtxBuffer->GetElementNum(eVertexBufferElement_Position);
				int lVtxQuadSize = lVtxStride*4;

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					//This is not the fastest thing possible
					cVector3f vParticlePos = pParticle->mvPos;


					if(mCoordSystem == eParticleEmitterCoordSystem_Local){
						vParticlePos = cMath::MatrixMul(mpParentSystem->GetWorldMatrix(), vParticlePos);
					}

					cVector3f vPos = cMath::MatrixMul(apFrustum->GetViewMatrix(), vParticlePos);


					// NEW

					cVector3f vParticleSize = pParticle->mvSize;
					cColor finalColor = pParticle->mColor * colorMul;

					if ( mbUsePartSpin )
					{
						cMatrixf mtxRotationMatrix = cMath::MatrixRotateZ(pParticle->mfSpin);


						SetPos(&pPosArray[i*lVtxQuadSize + 0*lVtxStride], vPos + cMath::MatrixMul(mtxRotationMatrix, vAdd[0]*vParticleSize));
						SetCol(&pColArray[i*16 + 0*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 1*lVtxStride], vPos + cMath::MatrixMul(mtxRotationMatrix, vAdd[1]*vParticleSize));
						SetCol(&pColArray[i*16 + 1*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 2*lVtxStride], vPos + cMath::MatrixMul(mtxRotationMatrix, vAdd[2]*vParticleSize));
						SetCol(&pColArray[i*16 + 2*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 3*lVtxStride], vPos + cMath::MatrixMul(mtxRotationMatrix, vAdd[3]*vParticleSize));
						SetCol(&pColArray[i*16 + 3*4], finalColor);

					}
					else
					{
						//--

						SetPos(&pPosArray[i*lVtxQuadSize + 0*lVtxStride], vPos + vAdd[0]*vParticleSize);
						SetCol(&pColArray[i*16 + 0*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 1*lVtxStride], vPos + vAdd[1]*vParticleSize);
						SetCol(&pColArray[i*16 + 1*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 2*lVtxStride], vPos + vAdd[2]*vParticleSize);
						SetCol(&pColArray[i*16 + 2*4], finalColor);

						SetPos(&pPosArray[i*lVtxQuadSize + 3*lVtxStride], vPos + vAdd[3]*vParticleSize);
						SetCol(&pColArray[i*16 + 3*4], finalColor);

					}
				}
			}
			//////////////////////////////////////////////////
			// LINE
			else if(mDrawType == eParticleEmitterType_Line)
			{
				int lVtxStride = mpVtxBuffer->GetElementNum(eVertexBufferElement_Position);
				int lVtxQuadSize = lVtxStride*4;	

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					//This is not the fastest thing possible...

					cVector3f vParticlePos1 = pParticle->mvPos;
					cVector3f vParticlePos2 = pParticle->mvLastPos;

					if(mCoordSystem == eParticleEmitterCoordSystem_Local){
						vParticlePos1 = cMath::MatrixMul(mpParentSystem->GetWorldMatrix(), vParticlePos1);
						vParticlePos2 = cMath::MatrixMul(mpParentSystem->GetWorldMatrix(), vParticlePos2);
					}

					cVector3f vPos1 = cMath::MatrixMul(apFrustum->GetViewMatrix(), vParticlePos1);
					cVector3f vPos2 = cMath::MatrixMul(apFrustum->GetViewMatrix(), vParticlePos2);

					cVector3f vDirY;
					cVector2f vDirX;

					if(vPos1 == vPos2)
					{
						vDirY = cVector3f(0,1,0);
						vDirX = cVector2f(1,0);
					}
					else
					{
						vDirY = vPos1 - vPos2;
						vDirY.Normalize();
						vDirX = cVector2f(vDirY.y,-vDirY.x);
						vDirX.Normalize();
					}

					vDirX = vDirX * mvDrawSize.x * pParticle->mvSize.x;
					vDirY = vDirY * mvDrawSize.y * pParticle->mvSize.y;

					if(apFrustum->GetInvertsCullMode()) vDirY = vDirY*-1;

					cColor finalColor = pParticle->mColor * colorMul;
					
					SetPos(&pPosArray[i*lVtxQuadSize + 0*lVtxStride], vPos2 + vDirY*-1 + vDirX);
					SetCol(&pColArray[i*16 + 0*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 1*lVtxStride], vPos2 + vDirY*-1 + vDirX*-1);
					SetCol(&pColArray[i*16 + 1*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 2*lVtxStride], vPos1 + vDirY + vDirX*-1);
					SetCol(&pColArray[i*16 + 2*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 3*lVtxStride], vPos1 + vDirY + vDirX);
					SetCol(&pColArray[i*16 + 3*4], finalColor);
				}
			}
			//////////////////////////////////////////////////
			// AXIS
			else if(mDrawType == eParticleEmitterType_Axis)
			{
				if(mlAxisDrawUpdateCount != GetMatrixUpdateCount())
				{
					mlAxisDrawUpdateCount = GetMatrixUpdateCount();
					cMatrixf mtxInv = cMath::MatrixInverse(GetWorldMatrix());
					mvRight = mtxInv.GetRight();
					mvForward = mtxInv.GetForward();
				}

				cVector3f vAdd[4];
				/*= 
				{
				mvRight		 +	mvForward * 1,
				mvRight * -1 +	mvForward * 1,
				mvRight * -1 +	mvForward * -1,
				mvRight		 +	mvForward * -1
				};*/

				int lVtxStride = mpVtxBuffer->GetElementNum(eVertexBufferElement_Position);
				int lVtxQuadSize = lVtxStride*4;

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					//This is not the fastest thing possible
					cVector3f vParticlePos = pParticle->mvPos;


					if(mCoordSystem == eParticleEmitterCoordSystem_Local){
						vParticlePos = cMath::MatrixMul(mpParentSystem->GetWorldMatrix(), vParticlePos);
					}

					cVector3f vPos = vParticlePos;//cMath::MatrixMul(apCamera->GetViewMatrix(), vParticlePos);
					cVector2f &vSize = pParticle->mvSize;

					vAdd[0] = mvRight	* vSize.x	 +	mvForward * vSize.y;
					vAdd[1] = mvRight * -vSize.x	 +	mvForward * vSize.y;
					vAdd[2] = mvRight * -vSize.x	 +	mvForward * -vSize.y;
					vAdd[3] = mvRight	* vSize.x	 +	mvForward * -vSize.y;

					cColor finalColor = pParticle->mColor * colorMul;

					SetPos(&pPosArray[i*lVtxQuadSize + 0*lVtxStride], vPos + vAdd[0]);
					SetCol(&pColArray[i*16 + 0*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 1*lVtxStride], vPos + vAdd[1]);
					SetCol(&pColArray[i*16 + 1*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 2*lVtxStride], vPos + vAdd[2]);
					SetCol(&pColArray[i*16 + 2*4], finalColor);

					SetPos(&pPosArray[i*lVtxQuadSize + 3*lVtxStride], vPos + vAdd[3]);
					SetCol(&pColArray[i*16 + 3*4], finalColor);
				}
			}

			mpVtxBuffer->SetElementNum(mlNumOfParticles * 6);

			//Update the vertex buffer data

			if(mvSubDivUV.size() > 1)
				mpVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Color0 | eVertexElementFlag_Texture0, false);
			else
				mpVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Color0, false);

		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* iParticleEmitter::GetVertexBuffer()
	{
		return mpVtxBuffer;
	}

	//-----------------------------------------------------------------------

	bool iParticleEmitter::IsVisible()
	{ 
		if(IsActive()==false) return false;
		return mbIsVisible; 
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* iParticleEmitter::GetBoundingVolume()
	{
		if(mbUpdateBV)
		{
            cVector3f vMin;
			cVector3f vMax;

			if(mlNumOfParticles >0)
			{
				//Make a bounding volume that encompasses start pos too!
				vMin = GetWorldPosition();
				vMax = GetWorldPosition();

				for(int i=0;i<(int)mlNumOfParticles;i++)
				{
					cParticle *pParticle = mvParticles[i];

					//X
					if(pParticle->mvPos.x < vMin.x)		 vMin.x = pParticle->mvPos.x;
					else if(pParticle->mvPos.x > vMax.x) vMax.x = pParticle->mvPos.x;

					//Y
					if(pParticle->mvPos.y < vMin.y)		 vMin.y = pParticle->mvPos.y;
					else if(pParticle->mvPos.y > vMax.y) vMax.y = pParticle->mvPos.y;

					//Z
					if(pParticle->mvPos.z < vMin.z)		 vMin.z = pParticle->mvPos.z;
					else if(pParticle->mvPos.z > vMax.z) vMax.z = pParticle->mvPos.z;
				}
			}
			else
			{
				vMin = GetWorldPosition();
				vMax = GetWorldPosition();;
			}

			/////////////////////////7
			// Add size for axis
			if(mDrawType == eParticleEmitterType_Axis)
			{
				if(mlAxisDrawUpdateCount != GetMatrixUpdateCount())
				{
					mlAxisDrawUpdateCount = GetMatrixUpdateCount();
					cMatrixf mtxInv = cMath::MatrixInverse(GetWorldMatrix());
					mvRight = mtxInv.GetRight();
					mvForward = mtxInv.GetForward();
				}
				
				cVector3f vAdd = mvRight*mvMaxDrawSize.x + mvForward*mvMaxDrawSize.y;

				vMax += vAdd;
				vMin -= vAdd;
			}
			/////////////////////////7
			// Add size for other particle types.
			else
			{
				vMax += cVector3f(mvMaxDrawSize.x,mvMaxDrawSize.y, mvMaxDrawSize.x);
				vMin -= cVector3f(mvMaxDrawSize.x,mvMaxDrawSize.y, mvMaxDrawSize.x);
			}
			
			mBoundingVolume.SetLocalMinMax(vMin, vMax);

			//Log("Min: (%f, %f, %f) Max: (%f, %f, %f)\n", vMin.x, vMin.y, vMin.z, vMax.x,vMax.y,vMax.z);

			if(mCoordSystem == eParticleEmitterCoordSystem_Local)
			{
				mBoundingVolume.SetTransform(mpParentSystem->GetWorldMatrix());
			}

			mbUpdateBV = false;
		}

		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	cMatrixf* iParticleEmitter::GetModelMatrix(cFrustum *apFrustum)
	{
		if(apFrustum)
		{
			if(mDrawType == eParticleEmitterType_Axis)
			{
				m_mtxTemp = cMatrixf::Identity;
			}
			//This is really not good...
			else if(mCoordSystem == eParticleEmitterCoordSystem_World)
			{
				m_mtxTemp = cMath::MatrixInverse(apFrustum->GetViewMatrix());
			}
			else
			{
				m_mtxTemp = cMath::MatrixInverse(apFrustum->GetViewMatrix());
				//m_mtxTemp = cMath::MatrixMul(cMath::MatrixInverse(apCamera->GetViewMatrix()), 
				//								GetWorldMatrix());
			}

			//m_mtxTemp.SetTranslation(cVector3f(0,0,0));//GetWorldMatrix().GetTranslation());

			//m_mtxTemp = cMatrixf::Identity;

			//cMatrixf mtxCam = apCamera->GetViewMatrix();
			//Log("MATRIX: %s\n",mtxCam.ToString().c_str());

			return &m_mtxTemp;

		}
		else
		{
			return &GetWorldMatrix();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticle* iParticleEmitter::CreateParticle()
	{
		if(mlNumOfParticles == mlMaxParticles) return NULL;
		++mlNumOfParticles;
		return mvParticles[mlNumOfParticles-1];
	}

	//-----------------------------------------------------------------------

	void iParticleEmitter::SwapRemove(unsigned int alIndex)
	{
		if(alIndex < mlNumOfParticles-1)
		{
			cParticle* pTemp = mvParticles[alIndex];
			mvParticles[alIndex] = mvParticles[mlNumOfParticles-1];
			mvParticles[mlNumOfParticles-1] = pTemp;
		}
		mlNumOfParticles--;
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}
