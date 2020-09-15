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

#include "math/Frustum.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/RenderableContainer.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFrustum::cFrustum()
	{
		mbInvertsCullMode = false;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cFrustum::SetupPerspectiveProj(const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
										float afFarPlane, float afNearPlane,float afFOV,float afAspect,
										const cVector3f &avOrigin, bool abInfFarPlane,
										cMatrixf* apCustomFarProjMtx, bool abObliqueNearPlane)
	{
		mProjectionType = eProjectionType_Perspective;
		mfFOV = afFOV;
		mfAspect = afAspect;
		mbObliqueNearPlane = abObliqueNearPlane;

		Setup(a_mtxProj, a_mtxView,afFarPlane,afNearPlane,avOrigin,abInfFarPlane, apCustomFarProjMtx);
	}

	void cFrustum::SetupOrthoProj(	const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
									float afFarPlane,float afNearPlane,const cVector2f &avViewSize,
									const cVector3f &avOrigin, bool abInfFarPlane)
	{
		mProjectionType = eProjectionType_Orthographic;
		mvViewSize = avViewSize;

		Setup(a_mtxProj, a_mtxView,afFarPlane,afNearPlane,avOrigin,abInfFarPlane);
	}

	void cFrustum::Setup(	const cMatrixf& a_mtxProj, const cMatrixf& a_mtxView,
							float afFarPlane,float afNearPlane,const cVector3f &avOrigin, 
							bool abInfFarPlane, cMatrixf* apCustomFarProjMtx )
	{
		m_mtxViewProj = cMath::MatrixMul(a_mtxProj, a_mtxView);
		m_mtxView = a_mtxView;
		m_mtxProj = a_mtxProj;

		mfFarPlane = afFarPlane;
		mfNearPlane = afNearPlane;

		mvOrigin = avOrigin;

		mbInfFarPlane = abInfFarPlane;

		mOriginBV.SetSize(afNearPlane*2);
		mOriginBV.SetPosition(mvOrigin);

		UpdatePlanes(apCustomFarProjMtx);
		UpdateSphere();
		UpdateVertices();
		UpdateBV();
	}
	
	//-----------------------------------------------------------------------

	const cPlanef& cFrustum::GetPlane(eFrustumPlane aType)
	{
		return mPlane[aType];
	}
	
	//-----------------------------------------------------------------------

	bool cFrustum::CollidePoint(const cVector3f& avPoint)
	{
		////////////////////////
		//Check with Bounding volume
		if(cMath::CheckPointInBVIntersection(avPoint, mBoundingVolume)==false)
		{
			return false;
		}
		
		////////////////////////
		//Check with planes
		int lPlanes = mbInfFarPlane ? 5:6;
		for(int i=0; i<lPlanes; i++)
		{
			float fDist = cMath::PlaneToPointDist(mPlane[i],avPoint);
			if(fDist < 0)
			{
				return false;
			}
		}
		return true;
	}
	
	//-----------------------------------------------------------------------


	eCollision cFrustum::CollideBoundingVolume(cBoundingVolume* apBV)
	{
		//Check if the BV is in the Frustum sphere.
		if(CollideFustrumSphere(apBV->GetWorldCenter(),apBV->GetRadius()) == eCollision_Outside)
		{
			return eCollision_Outside;
		}
		
		//Do a simple sphere collide test
		eCollision ret = CollideSphere(apBV->GetWorldCenter(),apBV->GetRadius());
		
		//If there was an intersection, collide with the AABB
		if(ret == eCollision_Intersect)
		{
			return CollideAABB(apBV->GetMin(),apBV->GetMax());
		}

		return ret;
	}

	//-----------------------------------------------------------------------

	eCollision cFrustum::CollideNode(iRenderableContainerNode* apNode)
	{
		//Check if the BV is in the Frustum sphere.
		if(CollideFustrumSphere(apNode->GetCenter(),apNode->GetRadius()) == eCollision_Outside)
		{
			return eCollision_Outside;
		}

		//Do a simple sphere collide test
		eCollision ret = CollideSphere(apNode->GetCenter(),apNode->GetRadius());

		//If there was an intersection, collide with the AABB
		if(ret == eCollision_Intersect)
		{
			return CollideAABB(apNode->GetMin(),apNode->GetMax());
		}

		return ret;
	}

	//-----------------------------------------------------------------------

	eCollision cFrustum::CollideFrustum(cFrustum *apFrustum)
	{
		/////////////////////////////
		//BV Collision
		if(cMath::CheckBVIntersection(mBoundingVolume, apFrustum->mBoundingVolume)==false)
		{
			return eCollision_Outside;
		}
		
		/////////////////////////////
		//This Frustum vs other
		int lPlanes = mbInfFarPlane ? 5 : 6;
		eCollision collision = cMath::CheckPointsPlanesCollision(&apFrustum->mvVertices[0], 8, &mPlane[0], lPlanes);
		if(collision == eCollision_Outside) return eCollision_Outside;
		if(collision == eCollision_Inside) return eCollision_Inside;

		/////////////////////////////
		//Other Frustum vs this
		lPlanes = apFrustum->mbInfFarPlane ? 5 : 6;
		collision = cMath::CheckPointsPlanesCollision(&mvVertices[0], 8, &apFrustum->mPlane[0], lPlanes);
		if(collision == eCollision_Outside) return eCollision_Outside;

		//Not sure how good it works, and not sure if it is 100% either...
		//Just letting code be here for easy switch in the future
		/*if(cMath::CheckSeparatingAxisIntersection(	mvVertices, 8, mPlane, mbInfFarPlane ? 5 : 6,
													apFrustum->mvVertices, 8, apFrustum->mPlane, apFrustum->mbInfFarPlane ? 5 : 6)==false)
		{
			return eCollision_Outside;
		}*/
		
		return eCollision_Intersect;
	}

	//-----------------------------------------------------------------------

	bool cFrustum::CheckSphereNearPlaneIntersection(const cVector3f &avCenter, float afRadius)
	{
		//////////////////////////////
		// Sphere vs near plane (needed?)
		if(cMath::CheckPlaneSphereCollision(GetPlane(eFrustumPlane_Near), avCenter, afRadius)!= eCollision_Intersect)
		{
			return false;
		}

		//////////////////////////////
		// Near plane points vs Sphere
		for(int i=0; i<4; ++i)
		{
			if(cMath::CheckPointInSphereIntersection(mvVertices[i], avCenter, afRadius)) return true;
		}

		//////////////////////////////
		// For each point in near plane make a plane between it and sphere center and check if all points lie outside this!
        for(int i=0; i<4; ++i)
		{
			cVector3f &vVtx = mvVertices[i];

            cVector3f vToVtx = cMath::Vector3Normalize(vVtx - avCenter);
            cVector3f vPlanePoint = avCenter + vToVtx * afRadius;
			cVector3f vPlaneNormal = vToVtx*-1;
			
			cPlanef plane; plane.FromNormalPoint(vPlaneNormal, vPlanePoint);

			if(cMath::CheckPointsPlanesCollision(&mvVertices[0], 4, &plane,1) == eCollision_Outside)
			{
				return false;
			}
		}
        		

		return true;
	}

	//-----------------------------------------------------------------------

	bool cFrustum::CheckAABBNearPlaneIntersection(const cVector3f &avMin, const cVector3f &avMax)
	{
		//////////////////////////////
		// AABB vs Near Plane
		if(cMath::CheckPlaneAABBCollision(GetPlane(eFrustumPlane_Near), avMin, avMax)!= eCollision_Intersect)
		{
			return false;
		}

		//////////////////////////////
		// Near plane points vs AABB
		for(int i=0; i<4; ++i)
		{
			if(cMath::CheckPointInAABBIntersection(mvVertices[i], avMin, avMax)) return true;
		}

		//////////////////////////////
		// Check if near plane points intersect with box
		if(cMath::CheckPointsAABBPlanesCollision(mvVertices, 4, avMin, avMax)!=eCollision_Outside) return true;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cFrustum::CheckBVNearPlaneIntersection(cBoundingVolume* apBV)
	{
		//////////////////////////////
		// BV vs Near Plane
		if(cMath::CheckPlaneBVCollision(GetPlane(eFrustumPlane_Near), *apBV)!= eCollision_Intersect)
		{
			return false;
		}

		//////////////////////////////
		// Near plane points vs AABB
		for(int i=0; i<4; ++i)
		{
			if(cMath::CheckPointInBVIntersection(mvVertices[i], *apBV)) return true;
		}

		//////////////////////////////
		// Check if Sphere Inside the frustum (check only frustum sides!)
		if(CollideSphere(apBV->GetWorldCenter(), apBV->GetRadius(),4)==eCollision_Inside) return true;
		
		//////////////////////////////
		// Check if AABB Inside frustum (check only frustum sides!)
		if(cMath::CheckPointsAABBPlanesCollision(mvVertices, 4, apBV->GetMin(), apBV->GetMax())!=eCollision_Outside) return true;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cFrustum::CheckFrustumNearPlaneIntersection(cFrustum* apFrustum)
	{
		const cPlanef& nearPlane = GetPlane(eFrustumPlane_Near);

		//////////////////////////////
		// Frustum Sphere vs Near Plane
		if(cMath::CheckPlaneSphereCollision(nearPlane, apFrustum->mBoundingSphere.center, apFrustum->mBoundingSphere.r)!= eCollision_Intersect)
		{
			return false;
		}

		//////////////////////////////
		// Frustum vs Near plane
		bool bInside = true;
		for(int i=0; i<8; ++i)
		{
			if( cMath::PlaneToPointDist(nearPlane, apFrustum->mvVertices[i])>0)
			{
				bInside = false;
				break;
			}
		}
		if(bInside) return false;

		//////////////////////////////
		// Near plane points vs Frustum
		if(cMath::CheckPointsPlanesCollision(mvVertices, 4, &apFrustum->mPlane[0], apFrustum->mbInfFarPlane? 5 : 6)==eCollision_Outside)
		{
			return false;
		}

		///////////////////////////////////
		// Do a sepparating axis check
		// Code above should work just aswell, but saving code if needed.
		/*if(cMath::CheckSeparatingAxisIntersection(	mvVertices, 4, &mPlane[eFrustumPlane_Near], 1,
													apFrustum->mvVertices, 8, apFrustum->mPlane, apFrustum->mbInfFarPlane ? 5 : 6)==false)
		{
			return false;
		}*/


		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cFrustum::CheckLineIntersection(const cVector3f& avStart, const cVector3f &avEnd, cVector3f &avIntersection)
	{
		float fClosestT = -1;
		
		//////////////////////////////
		// Iterate planes
		int lPlanes = mbInfFarPlane? 5 : 6;
		for(int i=0; i<lPlanes; i++)
		{
			cPlanef &plane = mPlane[i];
			float fT;
			cVector3f vIntersection;

			/////////////////////////////////
			//See if the line intersects with plane
			if(cMath::CheckPlaneLineIntersection(plane, avStart, avEnd, &vIntersection, &fT))
			{
				if(	(fT < fClosestT || fClosestT<0) && 
					CollidePoint(vIntersection+plane.GetNormal()*0.001f))
				{
                    avIntersection = vIntersection;
					fClosestT = fT;
				}
			}
		}
		
		return fClosestT >=0;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	eCollision cFrustum::CollideFustrumSphere(const cVector3f& avCenter, float afRadius)
	{
		float fRadiusSum = mBoundingSphere.r + afRadius;
		cVector3f vSepAxis = mBoundingSphere.center - avCenter;
		if(vSepAxis.SqrLength() > (fRadiusSum * fRadiusSum))
		{
			return eCollision_Outside;
		}

		return eCollision_Intersect;
	}

	//-----------------------------------------------------------------------

	eCollision cFrustum::CollideSphere(const cVector3f& avCenter, float afRadius, int alMaxPlanes)
	{
		int lPlanes = cMath::Max(mbInfFarPlane? 5 : 6, alMaxPlanes);
		for(int i=0; i<lPlanes; i++)
		{
			float fDist = cMath::PlaneToPointDist(mPlane[i],avCenter);

			if(fDist < -afRadius){
				return eCollision_Outside;
			}

			if(std::abs(fDist) < afRadius){
				return eCollision_Intersect;
			}
		}
		
		return eCollision_Inside;
	}
	//-----------------------------------------------------------------------

	eCollision cFrustum::CollideAABB(const cVector3f& avMin,const cVector3f& avMax, int alMaxPlanes)
	{
		const cVector3f& vMax = avMax;
		const cVector3f& vMin = avMin;

		//Get the corners from the AAB
		cVector3f vCorners[8] ={
			cVector3f(vMax.x,vMax.y,vMax.z),
			cVector3f(vMax.x,vMax.y,vMin.z),
			cVector3f(vMax.x,vMin.y,vMax.z),
			cVector3f(vMax.x,vMin.y,vMin.z),
			
			cVector3f(vMin.x,vMax.y,vMax.z),
			cVector3f(vMin.x,vMax.y,vMin.z),
			cVector3f(vMin.x,vMin.y,vMax.z),
			cVector3f(vMin.x,vMin.y,vMin.z),
		};

		/////////////////////////////
		//Frustum vs AABB
		eCollision collision = cMath::CheckPointsPlanesCollision(&vCorners[0], 8, &mPlane[0], mbInfFarPlane ? 5 : 6);
		if(collision == eCollision_Outside) return eCollision_Outside;
		if(collision == eCollision_Inside) return eCollision_Inside;

		/////////////////////////////
		//AABB vs Frustum
		collision = cMath::CheckPointsAABBPlanesCollision(&mvVertices[0], 8, avMin, avMax);
		if(collision == eCollision_Outside) return eCollision_Outside;

		return eCollision_Intersect;
		//return cMath::CheckPointsPlanesCollision(&vCorners[0], 8, &mPlane[0], mbInfFarPlane ? 5 : 6);
	}

	//-----------------------------------------------------------------------
	
	void cFrustum::UpdateSphere()
	{
		//////////////////////////////////////////////
		// Perspective projection
		if(mProjectionType == eProjectionType_Perspective)
		{
           	// calculate the radius of the frustum sphere
			float fViewLen = mfFarPlane - mfNearPlane;

			float fTop = mfFarPlane * tan(mfFOV * 0.5f);
			float fRight = fTop * mfAspect;

			// halfway point between near/far planes starting at the origin and extending along the z axis
			cVector3f P(0.0f, 0.0f, mfNearPlane + fViewLen * 0.5f);

			// the calculate far corner of the frustum
			cVector3f Q(fRight, fTop, fViewLen);

			// the vector between P and Q
			cVector3f vDiff = P - Q;

			// the radius becomes the length of this vector
			float fRadius = vDiff.Length();

			// get the look vector of the camera from the view matrix
			cVector3f vLookVector = m_mtxView.GetForward() * -1;
			
			// calculate the center of the sphere
			cVector3f vCenter = (mvOrigin) + (vLookVector * (fViewLen * 0.5f + mfNearPlane));
			
			mBoundingSphere = cSpheref(vCenter,fRadius);
		}
		//////////////////////////////////////////////
		// Orthographic projection
		else
		{
			// The length of the frustum
			float fViewLen = mfFarPlane - mfNearPlane;

			//The view frustum is basically a rect, get half length of all sides in order to get distance from center to a corner.
			cVector3f vHalfSize(mvViewSize.x * 0.5f,
								mvViewSize.y * 0.5f,
								fViewLen * 0.5f);

			float fRadius = vHalfSize.Length();

			// get the look vector of the camera from the view matrix
			cVector3f vLookVector = m_mtxView.GetForward() * -1;

			// calculate the center of the sphere
			cVector3f vCenter = mvOrigin + (vLookVector * (fViewLen * 0.5f + mfNearPlane));

			mBoundingSphere = cSpheref(vCenter,fRadius);
		}

	}

	//-----------------------------------------------------------------------
	
	void cFrustum::UpdatePlanes(cMatrixf* apCustomFarProjMtx )
	{
		//Left
		mPlane[eFrustumPlane_Left] = cPlanef(	m_mtxViewProj.m[3][0] + m_mtxViewProj.m[0][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[0][1],
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[0][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[0][3]);
		
		//Right
		mPlane[eFrustumPlane_Right] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[0][0],
												m_mtxViewProj.m[3][1] - m_mtxViewProj.m[0][1],	
												m_mtxViewProj.m[3][2] - m_mtxViewProj.m[0][2],
												m_mtxViewProj.m[3][3] - m_mtxViewProj.m[0][3]);

		//Bottom
		mPlane[eFrustumPlane_Bottom] = cPlanef(	m_mtxViewProj.m[3][0] + m_mtxViewProj.m[1][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[1][1],	
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[1][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[1][3]);
		
		//Top
		mPlane[eFrustumPlane_Top] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[1][0],
												m_mtxViewProj.m[3][1] - m_mtxViewProj.m[1][1],	
												m_mtxViewProj.m[3][2] - m_mtxViewProj.m[1][2],
												m_mtxViewProj.m[3][3] - m_mtxViewProj.m[1][3]);
				
		//Near
		mPlane[eFrustumPlane_Near] = cPlanef(	m_mtxViewProj.m[3][0] + m_mtxViewProj.m[2][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[2][1],	
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[2][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[2][3]);
		
		//Far
		if(apCustomFarProjMtx)
		{
			cMatrixf mtxCustomViewProj = cMath::MatrixMul(*apCustomFarProjMtx, m_mtxView);

			mPlane[eFrustumPlane_Far] = cPlanef(	mtxCustomViewProj.m[3][0] - mtxCustomViewProj.m[2][0],
													mtxCustomViewProj.m[3][1] - mtxCustomViewProj.m[2][1],	
													mtxCustomViewProj.m[3][2] - mtxCustomViewProj.m[2][2],
													mtxCustomViewProj.m[3][3] - mtxCustomViewProj.m[2][3]);
		}
		else
		{
			mPlane[eFrustumPlane_Far] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[2][0],
													m_mtxViewProj.m[3][1] - m_mtxViewProj.m[2][1],	
													m_mtxViewProj.m[3][2] - m_mtxViewProj.m[2][2],
													m_mtxViewProj.m[3][3] - m_mtxViewProj.m[2][3]);
		}
		
		for(int i=0;i<6;i++)
		{
			mPlane[i].Normalize();
		}
	}

	//-----------------------------------------------------------------------

	static eFrustumPlane vPlaneTypeOrder[4][2] = {
		{eFrustumPlane_Right, eFrustumPlane_Top},
		{eFrustumPlane_Right, eFrustumPlane_Bottom},
		{eFrustumPlane_Left, eFrustumPlane_Bottom},
		{eFrustumPlane_Left, eFrustumPlane_Top},
	};

	//-----------------------------------------------------------------------

	void cFrustum::UpdateVertices()
	{
		//NOTE: Does not work with infinite far plane!

		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Near],mPlane[eFrustumPlane_Right], mPlane[eFrustumPlane_Top], mvVertices[0]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Near],mPlane[eFrustumPlane_Right], mPlane[eFrustumPlane_Bottom], mvVertices[1]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Near],mPlane[eFrustumPlane_Left],  mPlane[eFrustumPlane_Bottom], mvVertices[2]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Near],mPlane[eFrustumPlane_Left],  mPlane[eFrustumPlane_Top], mvVertices[3]);

		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Far],mPlane[eFrustumPlane_Right], mPlane[eFrustumPlane_Top], mvVertices[4]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Far],mPlane[eFrustumPlane_Right], mPlane[eFrustumPlane_Bottom], mvVertices[5]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Far],mPlane[eFrustumPlane_Left], mPlane[eFrustumPlane_Bottom], mvVertices[6]);
		cMath::PlaneIntersectionPoint(mPlane[eFrustumPlane_Far],mPlane[eFrustumPlane_Left], mPlane[eFrustumPlane_Top], mvVertices[7]);

		//////////////////////////////////
		//Check if any near plane point is behind the near plane, this can happen to oblique clip plane!
		//Also trim the far plane if possible!
		if(mbObliqueNearPlane)
		{
			cPlanef& nearPlane = mPlane[eFrustumPlane_Near];
			cPlanef& farPlane = mPlane[eFrustumPlane_Far];

			cVector3f vNearCrossFar = cMath::Vector3Cross(farPlane.GetNormal(), nearPlane.GetNormal());

			cPlanef fakeNearPlane;
			fakeNearPlane.FromNormalPoint(farPlane.GetNormal()*-1, mvOrigin);
			
			///////////////////////////
			//Iterate near plane vertices.
			for(int i=0; i<4; ++i)
			{
				//Check if behind fake near or behind far plane
				if(	cMath::PlaneToPointDist(fakeNearPlane, mvVertices[i])<0 || 
					cMath::PlaneToPointDist(farPlane, mvVertices[i])<0)
				{
					eFrustumPlane plane0 = vPlaneTypeOrder[i][0];
					eFrustumPlane plane1 = vPlaneTypeOrder[i][1];

					cMath::PlaneIntersectionPoint(farPlane,mPlane[plane0], mPlane[plane1], mvVertices[i]);
					
					//"project" on near plane
					cPlanef splitPlane;
					splitPlane.FromNormalPoint(vNearCrossFar,mvVertices[i]); 

					cMath::PlaneIntersectionPoint(farPlane,nearPlane, splitPlane, mvVertices[i]);              
				}
			}

			///////////////////////////
			//Iterate far plane vertices and check if below near plane
			for(int i=4; i<8; ++i)
			{
				if(cMath::PlaneToPointDist(nearPlane, mvVertices[i])<0)
				{
					cPlanef splitPlane;
					splitPlane.FromNormalPoint(vNearCrossFar, mvVertices[i]); 

					cMath::PlaneIntersectionPoint(farPlane,nearPlane, splitPlane, mvVertices[i]);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cFrustum::UpdateBV()
	{
		cVector3f vMin = mvVertices[0];
		cVector3f vMax = mvVertices[0];

		for(int i=1; i<8; i++)
		{
			if(vMax.x < mvVertices[i].x)		vMax.x = mvVertices[i].x;
			else if(vMin.x > mvVertices[i].x)	vMin.x = mvVertices[i].x;

			if(vMax.y < mvVertices[i].y)		vMax.y = mvVertices[i].y;
			else if(vMin.y > mvVertices[i].y)	vMin.y = mvVertices[i].y;

			if(vMax.z < mvVertices[i].z)		vMax.z = mvVertices[i].z;
			else if(vMin.z > mvVertices[i].z)	vMin.z = mvVertices[i].z;
		}

		mBoundingVolume.SetLocalMinMax(vMin, vMax);
	}
	
	//-----------------------------------------------------------------------

	const cVector3f& cFrustum::GetOrigin()
	{
		return mvOrigin;
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cFrustum::GetOriginBV()
	{
		return &mOriginBV;
	}

	//-----------------------------------------------------------------------

	cVector3f cFrustum::GetForward()
	{
		return m_mtxView.GetForward();
	}
	//-----------------------------------------------------------------------
	
	void cFrustum::Draw(iLowLevelGraphics *apLowLevelGraphics, const cColor &aColor)
	{
		if(mbObliqueNearPlane)
		{
			//for(int i=0; i<4; ++i)
			//	apLowLevelGraphics->DrawSphere(mvVertices[i],0.05f, cColor(1,0,0));

			//for(int i=4; i<8; ++i)
			//	apLowLevelGraphics->DrawSphere(mvVertices[i],0.05f, cColor(0,1,0));
		}

		for(int i=0; i<4; ++i)
			apLowLevelGraphics->DrawLine(mvVertices[i==0?3:i-1], mvVertices[i],aColor);
		
		for(int i=4; i<8; ++i)
			apLowLevelGraphics->DrawLine(mvVertices[i==4?7:i-1], mvVertices[i],aColor);
		
		for(int i=0; i<4; ++i)
			apLowLevelGraphics->DrawLine(mvVertices[i], mvVertices[i+4],aColor);
		
		if(mbInvertsCullMode==false) return;

		//////////////////7
		//Planes
		/*for(int i=0; i<6; ++i)
		{
			cPlanef plane = mPlane[i];

			cVector3f vP = plane.GetNormal() * -plane.d;
			apLowLevelGraphics->DrawSphere(vP,0.05f, cColor(1,0,1));
			apLowLevelGraphics->DrawLine(vP,vP+plane.GetNormal(), cColor(0,1,1));

		}*/
	}

	//-----------------------------------------------------------------------
}
