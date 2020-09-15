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

#include "impl/HapticShapeHaptX.h"

#include "impl/LowLevelHapticHaptX.h"
#include "impl/HapticSurfaceHaptX.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

namespace hpl {

	using namespace HaptX;
	using namespace Vectors;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cHapticShapeHaptX::cHapticShapeHaptX(	const tString &asName, eHapticShapeType aType,
											const cVector3f& avSize, 
											cLowLevelHapticHaptX *apLowLevel)
											: iHapticShape(asName, aType)
	{
		/////////////////////////////
		//Init variables
		mpLowLevel = apLowLevel;
		mvSize = avSize;
		mpInterface = apLowLevel->GetInterface();

		mShapeID = -1;

		/////////////////////////////
		//Create if simple shape
		switch(mType)
		{
		case eHapticShapeType_Box: break;
		case eHapticShapeType_Sphere: break;
		case eHapticShapeType_Cylinder: break;
		case eHapticShapeType_Capsule: break;		
		};
	}

	cHapticShapeHaptX::~cHapticShapeHaptX()
	{
		if(mShapeID >= 0) mpInterface->ShapeDestroy(mShapeID);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cHapticShapeHaptX::SetEnabled(bool abX)
	{
		if(mShapeID <0) return;

		if(abX)	mpInterface->ShapeEnable(mShapeID, mpLowLevel->GetDeviceID());
		else	mpInterface->ShapeDisable(mShapeID, mpLowLevel->GetDeviceID());
	}
	bool cHapticShapeHaptX::GetEnabled()
	{
		if(mShapeID <0) return false;
		
		bool bRet;
		mpInterface->ShapeIsEnabled(bRet,mShapeID, mpLowLevel->GetDeviceID());
		return bRet;
	}	

	//-----------------------------------------------------------------------

	void cHapticShapeHaptX::SetTransform(const cMatrixf &a_mtxTransform)
	{
		if(mShapeID <0) return;

		mpInterface->ShapeSetTransform(mShapeID,(float*)a_mtxTransform.v);
	}

	cMatrixf cHapticShapeHaptX::GetTransform()
	{
		if(mShapeID <0) return cMatrixf::Identity;
		
		Matrix4f mtxOut;
		mpInterface->ShapeGetTransform(mtxOut,mShapeID);

		cMatrixf mtxRet(&mtxOut.m[0][0]);
		
		return mtxRet;
	}

	//-----------------------------------------------------------------------

	cVector3f cHapticShapeHaptX::GetAppliedForce()
	{
		if(mShapeID <0) return 0;

		Vec3f vOut;
		mpInterface->ShapeGetAppliedForce(vOut,mShapeID);

		return cVector3f((float)vOut.x, (float)vOut.y, (float)vOut.z);
	}

	//-----------------------------------------------------------------------

	void cHapticShapeHaptX::SetSurface(iHapticSurface *apSurface)
	{
		if(mShapeID <0) return;

		iHapticSurfaceHaptX *pHXSurface = static_cast<iHapticSurfaceHaptX*>(apSurface);
		
		mpSurface = apSurface;
		mpInterface->ShapeSetSurface(mShapeID, pHXSurface->GetHaptXSurface()->GetSurfaceID());
	}

	//-----------------------------------------------------------------------

	void cHapticShapeHaptX::RenderDebug(iLowLevelGraphics *apLowLevel, const cColor &aColor)
	{
		/*for(int i=0; i<mlPosNum; ++i)
		{
			Vec3f vPos = mpHxPositions[i];
			apLowLevel->DrawSphere(cVector3f(vPos.x,vPos.y, vPos.z),0.01,aColor);
		}*/

		/*for(int i=0; i<mlIndexNum; i+=3)
		{
			Vec3f &vPos1 = mpHxPositions[mpIndices[i]];
			Vec3f &vPos2 = mpHxPositions[mpIndices[i+1]];
			Vec3f &vPos3 = mpHxPositions[mpIndices[i+2]];

			apLowLevel->DrawLine(	cVector3f(vPos1.x,vPos1.y, vPos1.z),
									cVector3f(vPos2.x,vPos2.y, vPos2.z),
									aColor);
			apLowLevel->DrawLine(	cVector3f(vPos2.x,vPos2.y, vPos2.z),
									cVector3f(vPos3.x,vPos3.y, vPos3.z),
									aColor);
			apLowLevel->DrawLine(	cVector3f(vPos3.x,vPos3.y, vPos3.z),
									cVector3f(vPos1.x,vPos1.y, vPos1.z),
									aColor);
		}*/
	}

	//-----------------------------------------------------------------------
	
	void cHapticShapeHaptX::CreateFromVertices(const unsigned int* apIndexArray, int alIndexNum,
												const float *apPosArray, int alPosStride,
												const float *apTexArray, int alTexStride,
												int alVtxNum)
	{
		Vectors::Vec3f* vPosVec = hplNewArray( Vectors::Vec3f, alVtxNum );

		for(int i=0; i<alVtxNum; ++i)
		{
			const float* pPos = &apPosArray[i * alPosStride];
			Vectors::Vec3f &vDest = vPosVec[i];

			vDest.x = pPos[0];
			vDest.y = pPos[1];
			vDest.z = pPos[2];
		}

		iHapticShapeID hShapeID;
		mpInterface->ShapeCreate(hShapeID, 
								&vPosVec[0], alVtxNum,
								(int*)apIndexArray, alIndexNum,
								mpLowLevel->GetDefaultSurface()->GetSurfaceID(),
								NULL,true,true);
		mShapeID = hShapeID;

		hplDeleteArray(vPosVec);
	}

	//-----------------------------------------------------------------------

}
