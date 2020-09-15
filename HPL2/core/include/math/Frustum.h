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

#ifndef HPL_FRUSTUM_H
#define HPL_FRUSTUM_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "math/BoundingVolume.h"

namespace hpl {

	class iLowLevelGraphics;
	class iRenderableContainerNode;

	//-----------------------------------------------

	class cFrustum
	{
	public:
		cFrustum();

		void SetupPerspectiveProj( const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
									float afFarPlane,float afNearPlane,float afFOV, float afAspect,
									const cVector3f &avOrigin, bool abInfFarPlane=false,
									cMatrixf* apCustomFarProjMtx=NULL,
									bool abObliqueNearPlane=false);

		void SetupOrthoProj( const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
							 float afFarPlane,float afNearPlane,const cVector2f &avViewSize,
							 const cVector3f &avOrigin, bool abInfFarPlane=false);

		const cPlanef& GetPlane(eFrustumPlane aType);
		const cVector3f& GetVertex(int alIdx){ return mvVertices[alIdx];}

		bool CollidePoint(const cVector3f& avPoint);
		eCollision CollideBoundingVolume(cBoundingVolume* apBV);
		eCollision CollideNode(iRenderableContainerNode* apNode);
		eCollision CollideFrustum(cFrustum *apFrustum);
		
		inline const cMatrixf& GetProjectionMatrix() const { return m_mtxProj;}
		inline const cMatrixf& GetViewMatrix()const { return m_mtxView;}

		inline float GetFarPlane()const { return mfFarPlane;}
		inline float GetNearPlane()const { return mfNearPlane;}
		inline float GetAspect() const { return mfAspect;}
		inline float GetFOV() const { return mfFOV;}

		bool GetInfFarPlane(){ return mbInfFarPlane;}

		void SetInvertsCullMode(bool abX){ mbInvertsCullMode=abX;}
		bool GetInvertsCullMode(){ return mbInvertsCullMode;}

		inline eProjectionType GetProjectionType() const { return mProjectionType; }
		
		inline const cVector2f& GetOrthoViewSize() const{ return mvViewSize;}
		
		bool CheckSphereNearPlaneIntersection(const cVector3f &avCenter, float afRadius);
		bool CheckAABBNearPlaneIntersection(const cVector3f &avMin, const cVector3f &avMax);
		bool CheckBVNearPlaneIntersection(cBoundingVolume* apBV);
		bool CheckFrustumNearPlaneIntersection(cFrustum* apFrustum);
		
		bool CheckLineIntersection(const cVector3f& avStart, const cVector3f &avEnd, cVector3f &avIntersection);

		cSpheref GetBoundingSphere(){ return mBoundingSphere;}
		const cBoundingVolume& GetBoundingVolume(){ return mBoundingVolume;}

		const cVector3f& GetOrigin();
		cBoundingVolume* GetOriginBV();

		cVector3f GetForward();

		void Draw(iLowLevelGraphics *apLowLevelGraphics, const cColor &aColor);

	private:
		eCollision CollideFustrumSphere(const cVector3f& avCenter, float afRadius);

		eCollision CollideSphere(const cVector3f& avCenter, float afRadius, int alMaxPlanes=6);
		eCollision CollideAABB(const cVector3f& avMin,const cVector3f& avMax, int alMaxPlanes=6);
		

		void Setup(	const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
					float afFarPlane,float afNearPlane,const cVector3f &avOrigin, 
					bool abInfFarPlane, cMatrixf* apCustomFarProjMtx=NULL);
		
		void UpdatePlanes(cMatrixf* apCustomFarProjMtx);
		void UpdateSphere();
		void UpdateVertices();
		void UpdateBV();

		float mfFarPlane;
		float mfNearPlane;
		float mfAspect;
		float mfFOV;

		cVector2f mvViewSize;
        
		bool mbInfFarPlane;
		bool mbInvertsCullMode;
		bool mbObliqueNearPlane;

		eProjectionType mProjectionType;

		cVector3f mvOrigin;
		cBoundingVolume mOriginBV;

		cMatrixf m_mtxProj;
		cMatrixf m_mtxViewProj;
		cMatrixf m_mtxView;
		cPlanef mPlane[6];
		cSpheref mBoundingSphere;
		cBoundingVolume mBoundingVolume;

		cVector3f mvVertices[8];
	};
};
#endif // HPL_FRUSTUM_H
