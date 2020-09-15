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

#include "math/Math.h"

#include "math/Frustum.h"
#include "graphics/VertexBuffer.h"

#include "system/LowLevelSystem.h"

#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <map>

namespace hpl {

	static char mpTempChar[1024];

	//////////////////////////////////////////////////////////////////////////
	// RANDOM GENERATION
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	int cMath::RandRectl(int alMin, int alMax)
	{
		return (rand()%(alMax-alMin+1))+alMin;
	}
	
	//-----------------------------------------------------------------------

	float cMath::RandRectf(float afMin, float afMax)
	{
		float fRand= (float)rand()/(float)RAND_MAX;
        				
		return afMin + fRand*(afMax-afMin);
	}

	//-----------------------------------------------------------------------
	
	cVector2f cMath::RandRectVector2f(const cVector2f &avMin,const cVector2f &avMax)
	{
		return cVector2f(	RandRectf(avMin.x, avMax.x),
							RandRectf(avMin.y, avMax.y));
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::RandRectVector3f(const cVector3f &avMin,const cVector3f &avMax)
	{
		return cVector3f(	RandRectf(avMin.x, avMax.x),
							RandRectf(avMin.y, avMax.y),
							RandRectf(avMin.z, avMax.z));
	}

	//-----------------------------------------------------------------------
	
	cColor cMath::RandRectColor(const cColor &aMin,const cColor &aMax)
	{
		return cColor(	RandRectf(aMin.r, aMax.r),
							RandRectf(aMin.g, aMax.g),
							RandRectf(aMin.b, aMax.b),
							RandRectf(aMin.a, aMax.a));
	}

	//-----------------------------------------------------------------------

	void cMath::Randomize(int alSeed)
	{
		if(alSeed==-1)
		{
			srand((unsigned)time(NULL) );
		}
		else
		{
			srand(alSeed);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BIT WISE OPERATIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int cMath::GetFlagBit(int alBitNum)
	{
		switch(alBitNum)
		{
		case 0:		return eFlagBit_0;
		case 1:		return eFlagBit_1;
		case 2:		return eFlagBit_2;
		case 3:		return eFlagBit_3;
		case 4:		return eFlagBit_4;
		case 5:		return eFlagBit_5;
		case 6:		return eFlagBit_6;
		case 7:		return eFlagBit_7;
		case 8:		return eFlagBit_8;
		case 9:		return eFlagBit_9;
		case 10:	return eFlagBit_10;
		case 11:	return eFlagBit_11;
		case 12:	return eFlagBit_12;
		case 13:	return eFlagBit_13;
		case 14:	return eFlagBit_14;
		case 15:	return eFlagBit_15;
		}
		return 0x1 << alBitNum;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BV AND AABB OPERATIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMath::ExpandAABB(cVector3f& avBaseMin,cVector3f& avBaseMax, const cVector3f& avAddMin, const cVector3f& avAddMax)
	{
		if(avBaseMin.x > avAddMin.x)	avBaseMin.x = avAddMin.x;
		if(avBaseMin.y > avAddMin.y)	avBaseMin.y = avAddMin.y;
		if(avBaseMin.z > avAddMin.z)	avBaseMin.z = avAddMin.z;

		if(avBaseMax.x < avAddMax.x)	avBaseMax.x = avAddMax.x;
		if(avBaseMax.y < avAddMax.y)	avBaseMax.y = avAddMax.y;
		if(avBaseMax.z < avAddMax.z)	avBaseMax.z = avAddMax.z;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// INTERSECTIONS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	bool cMath::CheckRectIntersection(cRect2l aRect1, cRect2l aRect2)
	{
		if(aRect1.w<0) 
		{
			aRect1.w*=-1;
			aRect1.x-=aRect1.w;
		}

		if(aRect1.h<0) 
		{
			aRect1.h*=-1;
			aRect1.y-=aRect1.h;
		}

		if(aRect2.w<0) 
		{
			aRect2.w*=-1;
			aRect2.x-=aRect2.w;
		}

		if(aRect2.h<0) 
		{
			aRect2.h*=-1;
			aRect2.y-=aRect2.h;
		}
		
		return (aRect1.x>aRect2.x+(aRect2.w-1) || aRect2.x>aRect1.x+(aRect1.w-1) ||
			aRect1.y>aRect2.y+(aRect2.h-1) || aRect2.y>aRect1.y+(aRect1.h-1))==false;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckRectIntersection(cRect2f aRect1, cRect2f aRect2)
	{
		if(aRect1.w<0) 
		{
			aRect1.w*=-1;
			aRect1.x-=aRect1.w;
		}

		if(aRect1.h<0) 
		{
			aRect1.h*=-1;
			aRect1.y-=aRect1.h;
		}

		if(aRect2.w<0) 
		{
			aRect2.w*=-1;
			aRect2.x-=aRect2.w;
		}

		if(aRect2.h<0) 
		{
			aRect2.h*=-1;
			aRect2.y-=aRect2.h;
		}
		return (aRect1.x>aRect2.x+(aRect2.w) || aRect2.x>aRect1.x+(aRect1.w) ||
			aRect1.y>aRect2.y+(aRect2.h) || aRect2.y>aRect1.y+(aRect1.h))==false;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointInRectIntersection(const cVector2f& avPoint, const cRect2f& aRect)
	{
		if(avPoint.x<aRect.x || avPoint.x>aRect.x+aRect.w || avPoint.y<aRect.y || avPoint.y>aRect.y+aRect.h)
			return false;
		else
			return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckRectFit(const cRect2l& aRectSrc, const cRect2l& aRectDest)
	{
		//check is size is smaller and doesn't overlap
		if(aRectSrc.w>aRectDest.w || aRectSrc.h>aRectDest.h || 
			aRectSrc.x+aRectSrc.w > aRectDest.x+aRectDest.w || 
			aRectSrc.y+aRectSrc.h > aRectDest.y+aRectDest.h) return false;

		//check if x,y is in borders
		if(aRectSrc.x<aRectDest.x || aRectSrc.y<aRectDest.y ||
			aRectSrc.x>aRectDest.x+aRectDest.w || aRectSrc.y>aRectDest.y+aRectDest.h) return false;

		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckRectFit(const cRect2f& aRectSrc, const cRect2f& aRectDest)
	{
		//check is size is smaller and doesn't overlap
		if(aRectSrc.w>aRectDest.w || aRectSrc.h>aRectDest.h || 
			aRectSrc.x+aRectSrc.w > aRectDest.x+aRectDest.w || 
			aRectSrc.y+aRectSrc.h > aRectDest.y+aRectDest.h) return false;

		//check if x,y is in borders
		if(aRectSrc.x<aRectDest.x || aRectSrc.y<aRectDest.y ||
			aRectSrc.x>aRectDest.x+aRectDest.w || aRectSrc.y>aRectDest.y+aRectDest.h) return false;

		return true;
	}

	//-----------------------------------------------------------------------
	
	cRect2f cMath::GetClipRect(const cRect2f& aRectSrc,const cRect2f& aRectDest)
	{
		cRect2f outputRect = aRectSrc;

		if(outputRect.x < aRectDest.x){
			outputRect.w -= aRectDest.x-outputRect.x;
			outputRect.x = aRectDest.x;
		}
		if(outputRect.y < aRectDest.y){
			outputRect.h -= aRectDest.y-outputRect.y;
			outputRect.y = aRectDest.y;
		}

		if(outputRect.x+outputRect.w > aRectDest.x+aRectDest.w){
			outputRect.w -= (outputRect.x+outputRect.w)-(aRectDest.x+aRectDest.w);
		}
		if(outputRect.y+outputRect.h > aRectDest.y+aRectDest.h){
			outputRect.h -= (outputRect.y+outputRect.h)-(aRectDest.y+aRectDest.h);
		}

		return outputRect;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckSphereLineIntersection(const cVector3f& avSpherePos, float afSphereRadius, const cVector3f &avLineStart, const cVector3f &avLineEnd, float *apT1, cVector3f* apIntersection1, float *apT2, cVector3f* apIntersection2)
	{

		/////////////////////////////////////////////////
		// Sphere-Line intersection
		// Used formula explained in http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
		// plus a check for intersection within the line segment range
		//
		// returns true if 1 or 2 intersection points are found

		bool bIntersect = false;

		cVector3f vUnitDirectionVector = (avLineEnd - avLineStart);
		float fSegmentLength = vUnitDirectionVector.Length();
		vUnitDirectionVector.Normalize();

		cVector3f vStartToSpherePos = (avLineStart - avSpherePos);

		float fB = 2*cMath::Vector3Dot(vUnitDirectionVector, vStartToSpherePos);
		float fC = cMath::Vector3Dot(vStartToSpherePos, vStartToSpherePos) - (afSphereRadius*afSphereRadius);

		float fDiscriminant = fB*fB - 4*fC;

		if( fDiscriminant >= 0)
		{
			float fDiscSqRoot = sqrt(fDiscriminant);
			float fT1;
			float fT2;

            fT1 = 0.5f*(-fB + fDiscSqRoot);
			fT2 = 0.5f*(-fB - fDiscSqRoot);

			if(fT1 <= fSegmentLength)
			{
				bIntersect = true;

				if(apT1) *apT1 = fT1;
				
				if(apIntersection1)
					*apIntersection1 = avLineStart + vUnitDirectionVector*fT1;
			}
			else
			{
				if(apT1) *apT1 = -1;
			}

			if(fT2 <= fSegmentLength)
			{
				bIntersect = true;

				if(apT2) *apT2 = fT2;

				if(apIntersection2)
					*apIntersection2 = avLineStart + vUnitDirectionVector*fT2;
			}
			else
			{
				if(apT2) *apT2 = -1;
			}

		}

		return bIntersect;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointInFlatPolygon( const cVector3f& avRayStart, const cVector3f& avRayEnd,
										 const cVector2f& avPoint, const cVector2f& avViewportSize, 
										 int alNumIndices, unsigned int* avIndices, float* avVertexPos, int alStride, 
										 const cMatrixf& amtxObjWorldMatrix, 
										 const cMatrixf& amtxCamViewMatrix, const cMatrixf& amtxCamProjMatrix, 
										 unsigned int* apIntersectedTriIndex, cVector3f* apIntersectionWorldSpace, tVector3fVec* apTriangle)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////
		// HUGE inconvenience with this algo: if one of the vertices of a triangle lies 
		// behind the camera, the normal is inverted and the triangle automatically skipped.
		// No improvement if forced to check anyway, as the CheckPointInsideTriangle2D will also fail
		// because of this.
		// Need to find a way around for this. 
		bool bInside = false;

		cVector2f vPoint = (avPoint*2)/avViewportSize - 1;
		
		cMatrixf mtxScreenSpaceTransform = cMath::MatrixMul(amtxCamProjMatrix,
															amtxCamViewMatrix);

		cVector3f vForward = cVector3f(0,0,1);

		float fShortestT = 1000.0f;

		unsigned int lTriIndex;

		cVector3f vClosestIntersection;
		tVector3fVec vTriangleWorldSpace;
		tVector3fVec vTriangleMinDistWorldSpace;
		tVector3fVec vTriangleScreenSpace;
		vTriangleWorldSpace.resize(3);
		vTriangleMinDistWorldSpace.resize(3);
		vTriangleScreenSpace.resize(3);

		cPlanef trianglePlane;
		
		for(int i=0; i<alNumIndices;i+=3)
		{
			for(int j=0;j<3;++j)
			{
				int lBaseIndex = avIndices[i+j]*alStride;
				vTriangleWorldSpace[j] = cMath::MatrixMul(amtxObjWorldMatrix, 
														  cVector3f(
																	avVertexPos[lBaseIndex],
																	avVertexPos[lBaseIndex+1],
																	avVertexPos[lBaseIndex+2])
														 );
				vTriangleScreenSpace[j] = cMath::MatrixMulDivideW(mtxScreenSpaceTransform, vTriangleWorldSpace[j]);
			}

			//////////////////////////////////////////////////////////
			// Check if triangle is facing away and skip if so

			cVector3f vEdge1 = vTriangleScreenSpace[1] - vTriangleScreenSpace[0];
			cVector3f vEdge2 = vTriangleScreenSpace[2] - vTriangleScreenSpace[0];

			cVector3f vTriNormal = cMath::Vector3Cross(vEdge1, vEdge2);
			vTriNormal.Normalize();

			float fDotProduct = cMath::Vector3Dot(vTriNormal, vForward);
			if(fDotProduct >= -kEpsilonf)
				continue;

			float fT;
			cVector3f vIntersection;
			if(cMath::CheckPointInsideTriangle2D(vPoint, vTriangleScreenSpace))
			{
				trianglePlane.FromPoints(vTriangleWorldSpace[0],vTriangleWorldSpace[1],vTriangleWorldSpace[2]); 
				if(cMath::CheckPlaneLineIntersection(trianglePlane, avRayStart, avRayEnd, &vIntersection, &fT))
				{
					if(fT<fShortestT)
					{
						fShortestT = fT;
						lTriIndex = i;

						bInside = true;

						vTriangleMinDistWorldSpace[0] = vTriangleWorldSpace[0];
						vTriangleMinDistWorldSpace[1] = vTriangleWorldSpace[1];
						vTriangleMinDistWorldSpace[2] = vTriangleWorldSpace[2];

						vClosestIntersection = vIntersection;
					}
				}
			}
		}

		if(bInside)
		{
			if(apIntersectedTriIndex)
				*apIntersectedTriIndex = lTriIndex;
			if(apIntersectionWorldSpace)
				*apIntersectionWorldSpace = vClosestIntersection;
			if(apTriangle)
			{
				apTriangle->clear();
				*apTriangle = vTriangleMinDistWorldSpace;
			}
		}

		return bInside;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointInsideTriangle2D(const cVector2f& avPoint, const tVector3fVec& avTriangle)
	{
		/*bool bInside = true;

		cVector3f vPoint = cVector3f(avPoint);

		cVector3f vEdge[3];
		cVector3f vNormal[3];
		cVector3f vCenter[3];

		vEdge[0] = avTriangle[1] - avTriangle[0];
		vCenter[0] = (avTriangle[1] + avTriangle[0])*0.5f;

		vEdge[1] = avTriangle[2] - avTriangle[1];
		vCenter[1] = (avTriangle[2] + avTriangle[1])*0.5f;

		vEdge[2] = avTriangle[0] - avTriangle[2];
		vCenter[2] = (avTriangle[0] + avTriangle[2])*0.5f;

		for(int i=0;i<3;++i)
		{
			vEdge[i].z = 0;
			vCenter[i].z = 0;

			vNormal[i] = cVector3f(vEdge[i].y, -vEdge[i].x, 0);

			cVector3f vDir = vPoint - vCenter[i];

            bInside = bInside && (cMath::Vector3Dot(cVector3f(vDir),cVector3f(vNormal[i]))>0);

			if(bInside==false)
				break;
		}
		
		return bInside;*/

		
		cVector3f vPoint = avPoint;
		vPoint.z = 0;

		cVector3f vTri[3];

		for(int i=0;i<3;++i)
		{
			vTri[i] = avTriangle[i];
			vTri[i].z = 0;
		}

		//Log("Point:\n\t(%f %f)\nTriangle:\n\t(%f %f)\n\t(%f %f)\n\t(%f %f)\n", vPoint.x, vPoint.y,
		//																	   vTri[0].x, vTri[0].y,
		//																	   vTri[1].x, vTri[1].y,
		//																	   vTri[2].x, vTri[2].y); 

		return (cMath::CheckPointSameSideOfEdge2D(vPoint, vTri[0], vTri[1], vTri[2]) &&
				cMath::CheckPointSameSideOfEdge2D(vPoint, vTri[1], vTri[0], vTri[2]) &&
				cMath::CheckPointSameSideOfEdge2D(vPoint, vTri[2], vTri[0], vTri[1]));		
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointSameSideOfEdge2D(const cVector3f& avPoint1, const cVector3f& avPoint2, const cVector3f& avPointA, const cVector3f& avPointB)
	{
		cVector3f vEdge = avPointB-avPointA;
		cVector3f vCP1 = cMath::Vector3Cross(vEdge, avPoint1-avPointA);
		cVector3f vCP2 = cMath::Vector3Cross(vEdge, avPoint2-avPointA);

		return (cMath::Vector3Dot(vCP1,vCP2)>=0);
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckBVIntersection(cBoundingVolume& aBV1,cBoundingVolume& aBV2)
	{
		//////////////////////////////////////////
		//Check Sphere collision.
		//Not needed I think.
		/*float fRadiusSum = aBV1->GetRadius() + aBV2->GetRadius();
		cVector3f vSepAxis = aBV1->GetWorldCenter() - aBV2->GetWorldCenter();
		if(vSepAxis.SqrLength() > (fRadiusSum * fRadiusSum))
		{
		return eBVCollision_Outside;
		}*/

		///////////////////////////////////////////
		//Check box collision
		aBV1.UpdateSize();
		aBV2.UpdateSize();

		const cVector3f &vMin1 = aBV1.mvWorldMin;
		const cVector3f &vMin2 = aBV2.mvWorldMin;

		const cVector3f &vMax1 = aBV1.mvWorldMax;
		const cVector3f &vMax2 = aBV2.mvWorldMax;

		//Log("Min1: %s Max1: %s  vs  Min2: %s Max2: %s\n", vMin1.ToString().c_str(),vMax1.ToString().c_str(),
		//												vMin2.ToString().c_str(), vMax2.ToString().c_str());

		if(vMax1.x < vMin2.x || vMax1.y < vMin2.y || vMax1.z < vMin2.z ||
			vMax2.x < vMin1.x || vMax2.y < vMin1.y || vMax2.z < vMin1.z)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckAABBIntersection(	const cVector3f& avMin1,const cVector3f& avMax1,
										const cVector3f& avMin2,const cVector3f& avMax2)
	{
		if(	avMax1.x < avMin2.x || avMax1.y < avMin2.y || avMax1.z < avMin2.z ||
			avMax2.x < avMin1.x || avMax2.y < avMin1.y || avMax2.z < avMin1.z)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckAABBInside(const cVector3f& avInsideMin,const cVector3f& avInsideMax,
								const cVector3f& avOutsideMin,const cVector3f& avOutsideMax)
	{
		if(	avInsideMin.x >= avOutsideMin.x && avInsideMin.y >= avOutsideMin.y && avInsideMin.z >= avOutsideMin.z &&
			avInsideMax.x <= avOutsideMax.x && avInsideMax.y <= avOutsideMax.y && avInsideMax.z <= avOutsideMax.z)
		{
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointInBVIntersection(const cVector3f& avPoint, cBoundingVolume& aBV)
	{
		cVector3f vMax = aBV.GetMax();
		cVector3f vMin = aBV.GetMin();

		if(avPoint.x > vMax.x || avPoint.y > vMax.y || avPoint.z > vMax.z ||
			avPoint.x < vMin.x || avPoint.y < vMin.y || avPoint.z < vMin.z)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPointInAABBIntersection(const cVector3f& avPoint, const cVector3f& avMin,const cVector3f& avMax)
	{
		if(avPoint.x > avMax.x || avPoint.y > avMax.y || avPoint.z > avMax.z ||
			avPoint.x < avMin.x || avPoint.y < avMin.y || avPoint.z < avMin.z)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	static int gvBoundVolumeVtxConnections[8][3] =
	{
		{1,2,4},//0
		{0,3,5},//1
		{0,3,6},//2
		{1,2,7},//3

		{0,5,6},//4
		{1,4,7},//5
		{2,4,7},//6
		{3,5,6},//7
	};

	bool cMath::GetNormalizedClipRectFromBV(cVector3f &avDestMin, cVector3f &avDestMax,
											cBoundingVolume &aBV, cFrustum *apFrustum,
											float afTanHalfFov)
	{
		/////////////////////////////////
		// Setup variables.
		cVector3f vMax = aBV.GetMax();
		cVector3f vMin = aBV.GetMin();
		cVector3f vCorners[8];
		vCorners[0] = cVector3f(vMax.x,vMax.y,vMax.z);
		vCorners[1] = cVector3f(vMax.x,vMax.y,vMin.z);
		vCorners[2] = cVector3f(vMax.x,vMin.y,vMax.z);
		vCorners[3] = cVector3f(vMax.x,vMin.y,vMin.z);

		vCorners[4] = cVector3f(vMin.x,vMax.y,vMax.z);
		vCorners[5] = cVector3f(vMin.x,vMax.y,vMin.z);
		vCorners[6] = cVector3f(vMin.x,vMin.y,vMax.z);
		vCorners[7] = cVector3f(vMin.x,vMin.y,vMin.z);

		float fTan = afTanHalfFov > 0 ? afTanHalfFov : tan(apFrustum->GetFOV()*0.5f);
		float fNearPlane =apFrustum->GetNearPlane();
		float fNearTop =  fTan * fNearPlane;
		float fNearRight = apFrustum->GetAspect() * fNearTop;

		tVector3fList lstScreenSpacePos;

		/////////////////////////////////
		// Transform to camera space
		for(int i=0; i< 8; i++)
		{
			cVector3f& vPos = vCorners[i];
			vPos = MatrixMul(apFrustum->GetViewMatrix(), vPos);
		}

		/////////////////////////////////
		// Transform to screen space
		bool bVisible = false;
		for(int i=0; i< 8; i++)
		{
			cVector3f vPos = vCorners[i];

			if(vPos.z<0)bVisible = true;

			////////////////////////
			//Perspective Projection
			if(apFrustum->GetProjectionType()==eProjectionType_Perspective)
			{
				float fZ = vPos.z;

				///////////////////////////////////////
				//If behind camera, clip so that it is in front
				if(fZ > 0)
				{
					for(int j=0; j<3; ++j)
					{
						// Get the connection and see if it is in front of camera
						int lConnection = gvBoundVolumeVtxConnections[i][j];
						cVector3f& vConnectPos = vCorners[ lConnection ];
						if(vConnectPos.z >=0) continue;

						//The relationships between connection-pos and connection-nearplane can be seen as 
						//two equal triangles. Use that to get T and use that to calculate the postion.
						//This is a special case for intersection with the nearplane!
						cVector3f vDelta = vPos - vConnectPos;
						float fConnectToNear = (-fNearPlane) - vConnectPos.z;
						float fT = fConnectToNear / vDelta.z;

						//Get the position of intersection
						cVector3f vScreenPos = vConnectPos + vDelta * fT;

						//Normalize
						vScreenPos.x /= fNearRight;
						vScreenPos.y /= fNearTop;
						lstScreenSpacePos.push_back(vScreenPos);
					}
				}
				///////////////////////////////////////
				//Project to near plane and normalize
				else
				{
					if(fZ ==0) fZ = 0.0001f;

					vPos.x = ((vPos.x / fZ) * -fNearPlane) / fNearRight;
					vPos.y = ((vPos.y / fZ) * -fNearPlane) / fNearTop;
					lstScreenSpacePos.push_back(vPos);
				}
			}
			////////////////////////
			//Orthogonal Projection
			else
			{
				// TODO: there must be a better way to do this
				vPos = MatrixMul(apFrustum->GetProjectionMatrix(),vCorners[i]);
				lstScreenSpacePos.push_back(vPos);
			}
		}

		////////////////////////
		//Check of any pos is visible
		if(bVisible==false) return false;


		////////////////////////
		//Go through all screen position and get min/max
		avDestMax = -10000.0f;
		avDestMin = 10000.0f;
		for(tVector3fListIt it= lstScreenSpacePos.begin(); it != lstScreenSpacePos.end(); ++it)		
		{
			cVector3f& vPos = *it;

			//X
			if(vPos.x < avDestMin.x) avDestMin.x = vPos.x;
			if(vPos.x > avDestMax.x) avDestMax.x = vPos.x;

			//Y
			if(vPos.y < avDestMin.y) avDestMin.y = vPos.y;
			if(vPos.y > avDestMax.y) avDestMax.y = vPos.y;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	cRect2l cMath::GetClipRectFromNormalizedMinMax(const cVector3f &avMin, const cVector3f& avMax, const cVector2l &avScreenSize)
	{
		cVector3f vMin=avMin;
		cVector3f vMax=avMax;
		////////////////////////
		//Clip min and max
		if(vMin.x < -1) vMin.x = -1;
		if(vMin.y < -1) vMin.y = -1;
		if(vMax.x > 1) vMax.x = 1;
		if(vMax.y > 1) vMax.y = 1;

		////////////////////////
		//Get the screen coordinates
		cVector2f vHalfScreenSize = cVector2f((float)avScreenSize.x, (float)avScreenSize.y) *0.5f;
		cVector2l vTopLeft;
		cVector2l vBottomRight;
		vTopLeft.x = (int)(vHalfScreenSize.x + vMin.x * vHalfScreenSize.x);
		vTopLeft.y = (int)(vHalfScreenSize.y + vMax.y * -vHalfScreenSize.y);
		vBottomRight.x = (int)(vHalfScreenSize.x + vMax.x * vHalfScreenSize.x);
		vBottomRight.y = (int)(vHalfScreenSize.y + vMin.y * -vHalfScreenSize.y);

		////////////////////////
		//Set the clip rect.
		cRect2l destRect;
		destRect.x = vTopLeft.x;
		destRect.y = vTopLeft.y;
		destRect.w = (vBottomRight.x - vTopLeft.x);
		destRect.h = (vBottomRight.y - vTopLeft.y);
		
		return destRect;
	}
	
	//-----------------------------------------------------------------------


	void cMath::GetClipRectFromBV(	cRect2l &aDestRect, cBoundingVolume &aBV,
									cFrustum *apFrustum, const cVector2l &avScreenSize,
									float afTanHalfFov)
	{
		////////////////////////
		//Get the normalized clip coordinates
		cVector3f vMin, vMax;
		bool bVisible = GetNormalizedClipRectFromBV(vMin, vMax, aBV, apFrustum, afTanHalfFov);
		
		////////////////////////
		// If no point in front, it is not visible and just return full screen rect.
		if(bVisible == false)
		{
			aDestRect.x = 0;
			aDestRect.y = 0;
			aDestRect.w = avScreenSize.x;
			aDestRect.h = avScreenSize.y;
			return;
		}

		aDestRect = GetClipRectFromNormalizedMinMax(vMin, vMax, avScreenSize);
	}

	//-----------------------------------------------------------------------

	cRect2l cMath::GetClipRectFromSphere(	const cVector3f& avPosition, float afRadius,
											cFrustum *apFrustum,
											const cVector2l &avScreenSize,
											bool abPositionIsScreenSpace, float afTanHalfFov)
	{
		cVector3f vLightPos = avPosition;

		///////////////////////
		//Transform position to screen space if needed.
		const cMatrixf &mtxView = apFrustum->GetViewMatrix();
		if(abPositionIsScreenSpace==false)
		{
			vLightPos = MatrixMul(mtxView, vLightPos);
		}
		
		//////////////////////////////////////
		//Set up the size of the near plane
		float fTan = afTanHalfFov > 0 ? afTanHalfFov : tan(apFrustum->GetFOV()*0.5f);
		float fNearPlane =apFrustum->GetNearPlane();
		float fNearTop =  fTan * fNearPlane;
		float fNearRight = apFrustum->GetAspect() * fNearTop;
		
		/////////////////////////////////////////////
		//Set up variables determining screen size
        cVector2f vHalfScreenSize = cVector2f((float)avScreenSize.x, (float)avScreenSize.y) *0.5f;

		int lRight = avScreenSize.x-1;
		int lTop = 0;
		int lLeft = 0; 
		int lBottom = avScreenSize.y-1;
		
		/////////////////////////////////////////////
		//Set up variables to speed up calculations
		float fD;
		float fSqrRadius= afRadius*afRadius;
		cVector3f vSqrLightPos = vLightPos*vLightPos;
		
		//////////////////////
		//Check if the XZ-planes have valid solutions, if not skip
		fD = fSqrRadius*vSqrLightPos.x - (vSqrLightPos.x + vSqrLightPos.z) * (fSqrRadius - vSqrLightPos.z);
		if (fD>=0)
		{
			fD= sqrtf(fD);
		
			//Get the two solutions for the Plane's Normal X coordinate
			float fNx1 = (afRadius*vLightPos.x + fD) / (vSqrLightPos.x+vSqrLightPos.z);
			float fNx2 = (afRadius*vLightPos.x - fD) / (vSqrLightPos.x+vSqrLightPos.z);
			
			//Get the two solutions for the Plane's Normal Z coordinate
			float fNz1 = (afRadius-fNx1*vLightPos.x) / vLightPos.z;
			float fNz2 = (afRadius-fNx2*vLightPos.x) / vLightPos.z;
			
			//Get the two solutions for the Plane-Sphere intersection point Z coordinate
			float Pz1 = (vSqrLightPos.x+vSqrLightPos.z-fSqrRadius) / (vLightPos.z-(fNz1/fNx1)*vLightPos.x);
			float Pz2 = (vSqrLightPos.x+vSqrLightPos.z-fSqrRadius) / (vLightPos.z-(fNz2/fNx2)*vLightPos.x);
			
			//Check if position1 is in front of frustum
			if (Pz1<0)
			{
				//Project X to near plane and normalize
				float fNearX = fNz1 * fNearPlane/fNx1;		
				fNearX = fNearX/fNearRight;
				
				//Get screen coordinate
				int lScreenX = (int)(vHalfScreenSize.x + vHalfScreenSize.x * fNearX);
				
				//Check if position is left or right of the light position 
				float fPx= -Pz1 * fNz1/fNx1;
				if (fPx < vLightPos.x)	lLeft = Max(lLeft,lScreenX);
				else					lRight = Min(lRight,lScreenX);
			}
			
			//Check if position1 is in front of frustum
			if (Pz2<0)
			{
				//Project X to near plane and normalize
				float fNearX = fNz2 * fNearPlane/fNx2;
				fNearX = fNearX/fNearRight;
				
				//Get screen coordinate
				int lScreenX = (int)(vHalfScreenSize.x + vHalfScreenSize.x * fNearX);

				//Check if position is left or right of the light position
				float fPx= -Pz2 * fNz2/fNx2;
				if (fPx < vLightPos.x)	lLeft = Max(lLeft,lScreenX);
				else					lRight = Min(lRight,lScreenX);
			}
		}
		
		//////////////////////
		//Check if the YZ-planes have valid solutions, if not skip
		fD=fSqrRadius*vSqrLightPos.y - (vSqrLightPos.y+vSqrLightPos.z)*(fSqrRadius-vSqrLightPos.z);
		if (fD>=0)
		{
			fD = sqrtf(fD);
			
			//Get the two solutions for the Plane's Normal Y coordinate
			float fNy1 = (afRadius*vLightPos.y + fD) / (vSqrLightPos.y+vSqrLightPos.z);
			float fNy2 = (afRadius*vLightPos.y - fD) / (vSqrLightPos.y+vSqrLightPos.z);
			
			//Get the two solutions for the Plane's Normal Z coordinate
			float fNz1 = (afRadius-fNy1*vLightPos.y) / vLightPos.z;
			float fNz2 = (afRadius-fNy2*vLightPos.y) / vLightPos.z;
			
			//Get the two solutions for the Plane-Sphere intersection point Z coordinate
			float fPz1 = (vSqrLightPos.y+vSqrLightPos.z-fSqrRadius) / (vLightPos.z-(fNz1/fNy1)*vLightPos.y);
			float fPz2 = (vSqrLightPos.y+vSqrLightPos.z-fSqrRadius) / (vLightPos.z-(fNz2/fNy2)*vLightPos.y);
			
			//Check if position1 is in front of frustum
			if (fPz1<0)
			{
				//Project Y to near plane and normalize
				float fNearY = fNz1 * fNearPlane/fNy1;
				fNearY = fNearY/fNearTop;
				
				//Get screen coordinate
				int lScreenY = (int)(vHalfScreenSize.y - vHalfScreenSize.y*fNearY);
				
				//Check if position is below or above of the light position
				float fPy = -fPz1*fNz1/fNy1;
				if (fPy>vLightPos.y)	lTop = Max(lTop,lScreenY);
				else					lBottom = Min(lBottom,lScreenY);
			}
			
			//Check if position1 is in front of frustum
			if (fPz2<0)
			{
				//Project Y to near plane and normalize
				float fNearY= fNz2 *fNearPlane/fNy2;
				fNearY = fNearY/fNearTop;
				
				//Get screen coordinate
				int lScreenY = (int)(vHalfScreenSize.y - vHalfScreenSize.y*fNearY);
				
				//Check if position is below or above of the light position
				float fPy=-fPz2*fNz2/fNy2;
				if (fPy>vLightPos.y)	lTop = Max(lTop,lScreenY);
				else					lBottom = Min(lBottom,lScreenY);
			}
		}
		
		cRect2l outputRect;
		outputRect.x = lLeft;
		outputRect.y = lTop;
		outputRect.w = lRight-lLeft+1;
		outputRect.h = lBottom-lTop+1;

		return outputRect;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////
	// SPHERICAL COORDS
	//////////////////////////////////////////////////////


	cVector2f cMath::GetSphericalCoordsFromPoint3D(const cVector3f& avSphCenter, const cVector3f& avPoint)
	{
		cVector2f vPolarCoords = 0;

		cVector3f vRadius = avPoint-avSphCenter;

		float fRadius = vRadius.Length();
		float fInvRadius = 1/fRadius;

		vPolarCoords.x = acos(vRadius.y*fInvRadius);
		vPolarCoords.x = Wrap(vPolarCoords.x, 0, k2Pif);

		float fRadiusInSection = cVector2f(avPoint.x, avPoint.z).Length();
		if(fRadiusInSection>kEpsilonf)
		{
			vPolarCoords.y = atan2(vRadius.z,vRadius.x);
			vPolarCoords.y = Wrap(vPolarCoords.y, 0, k2Pif);
		}
		return vPolarCoords;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::GetPoint3DFromSphericalCoords(const cVector3f& avSphCenter, float afSphRadius, const cVector2f& avSphCoords)
	{
		float fSinTheta = sin(avSphCoords.x);
		float fCosTheta = cos(avSphCoords.x);
		float fSinPhi = sin(avSphCoords.y);
		float fCosPhi = cos(avSphCoords.y);

		cVector3f vDir = cVector3f(fSinTheta*fCosPhi,
			fCosTheta,
			fSinTheta*fSinPhi);

		return (avSphCenter + vDir*afSphRadius);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////
	// PLANES
	//////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	float cMath::PlaneDot(const cPlanef& aPlaneA, const cPlanef& aPlaneB)
	{
		return	aPlaneA.a * aPlaneB.a +
				aPlaneA.b * aPlaneB.b +
				aPlaneA.c * aPlaneB.c +
				aPlaneA.d * aPlaneB.d;
	}

	//-----------------------------------------------------------------------

	bool cMath::PlaneParallel(const cPlanef& aPlaneA, const cPlanef& aPlaneB)
	{
		float fDot = aPlaneA.a*aPlaneB.a+ aPlaneA.b*aPlaneB.b+aPlaneA.c*aPlaneB.c;
		return fDot > 0.99999f;
	}

	//-----------------------------------------------------------------------

	cPlanef cMath::TransformPlane(const cMatrixf &a_mtxTransform, const cPlanef& aPlane)
	{
		cVector3f vNormal = aPlane.GetNormal();
		cVector3f vPoint = vNormal * -aPlane.d;

		cMatrixf mtxNormalTrans = cMath::MatrixInverse(a_mtxTransform.GetRotation()).GetTranspose();

        return cPlanef( Vector3Normalize(MatrixMul(mtxNormalTrans, vNormal)), 
						MatrixMul(a_mtxTransform, vPoint) );
	}

	//-----------------------------------------------------------------------


	bool cMath::CheckSphereInPlanes(const cVector3f &avCenter, float afRadius, const cPlanef* apPlanes, int alPlaneCount)
	{
		for(int i=0; i<alPlaneCount; i++)
		{
			float fDist = cMath::PlaneToPointDist(apPlanes[i],avCenter);

			if(fDist < -afRadius){
				return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckAABBLineIntersection(	const cVector3f& avMin,const cVector3f& avMax,
											const cVector3f &avLineStart, const cVector3f &avLineEnd,
											cVector3f *apIntersectionPos, float *apT)
	{
		cPlanef vPlanes[6] ={
				cPlanef(cVector3f(-1,0,0),avMin), //Left
				cPlanef(cVector3f(1,0,0), avMax), //Right

				cPlanef(cVector3f(0,-1,0),avMin), //Bottom	
				cPlanef(cVector3f(0,1,0) ,avMax), //Top
				
				cPlanef(cVector3f(0,0,-1),avMin), //Back
				cPlanef(cVector3f(0,0,1), avMax), //Front
		};
		
		cVector3f vLargerMin = avMin - cVector3f(0.001f);
		cVector3f vLargerMax = avMax + cVector3f(0.001f);

		cVector3f vIntersection;
		float fShortestT = 1000.0f;
		bool bIntersected=false;
		for(int i=0; i<6; ++i)
		{
			float fT;
			if(CheckPlaneLineIntersection(vPlanes[i],avLineStart, avLineEnd,&vIntersection,&fT))
			{
				//Check this is the shortest intersection and if it is inside the AABB
				if(fT < fShortestT && CheckPointInAABBIntersection(vIntersection,vLargerMin,vLargerMax))
				{
					//Log("    Intersected!\n");
					if(apIntersectionPos) *apIntersectionPos = vIntersection;
					bIntersected = true;
                    fShortestT = fT;					
				}
			}
		}

		if(apT) *apT = fShortestT;

		return bIntersected;
	}

	//////////////////////////////////////////////////////////////////////////
	// FLOAT OPERATIONS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	float cMath::RoundFloatToDecimals(float afVal, int alPrecision)
	{
			float fPow = pow((float)10,alPrecision);
			float fInvPow = 1.0f/fPow;
			float fAdd = 0.5f*fInvPow;

			afVal += fAdd;

			float fFloor = floor(fPow*afVal);
			fFloor *= fInvPow;

			return fFloor;
		}

	//-----------------------------------------------------------------------

	float cMath::GetFraction(float afVal)
	{
		return afVal - floor(afVal);
	}

	//-----------------------------------------------------------------------

	float cMath::Modulus(float afDividend, float afDivisor)
	{
		float fNum = std::floor(std::abs(afDividend/afDivisor));

		float fRemain = std::abs(afDividend) - std::abs(afDivisor)*fNum;

		return fRemain;
	}

	//-----------------------------------------------------------------------

	float cMath::Wrap(float afX, float afMin, float afMax)
	{
		//Quick check if value is okay. If so just return.
		if(afX >= afMin && afX <= afMax) return afX;

		//Change setup so that min is 0
		afMax = afMax - afMin;
		float fOffSet = afMin;
		afMin =0;
		afX = afX - fOffSet;

		float fNumOfMax = std::floor(std::abs(afX/afMax));

		if(afX>=afMax)afX = afX - fNumOfMax*afMax;
		else if(afX<afMin) afX = ((fNumOfMax+1.0f)*afMax)+afX;

		return afX + fOffSet;
	}

	//-----------------------------------------------------------------------

	float cMath::Clamp(float afX, float afMin, float afMax)
	{
		if(afX > afMax) afX = afMax;
		else if(afX < afMin) afX = afMin;

		return afX;
	}

	//-----------------------------------------------------------------------

	float cMath::GetAngleDistanceRad(float afAngle1, float afAngle2)
	{
		return GetAngleDistance(afAngle1, afAngle2,k2Pif);
	}

	float cMath::GetAngleDistanceDeg(float afAngle1, float afAngle2)
	{
		return GetAngleDistance(afAngle1, afAngle2,360.0f);
	}

	float cMath::GetAngleDistance(float afAngle1, float afAngle2, float afMaxAngle)
	{
		afAngle1 = Wrap(afAngle1,0, afMaxAngle);
		afAngle2 = Wrap(afAngle2,0, afMaxAngle);

		if(afAngle1==afAngle2)
		{
			return 0;
		}
		else
		{
			float fDist1 = afAngle2 - afAngle1;
			float fDist2 = afMaxAngle - std::abs(fDist1);

			if(fDist1>0) fDist2 = -fDist2;

			if(std::abs(fDist1) < std::abs(fDist2)) 
				return fDist1;
			else
				return fDist2;
		}
	}

	//-----------------------------------------------------------------------

	float cMath::TurnAngle(float afAngle,float afFinalAngle,float afSpeed,float afMaxAngle)
	{
		if(afAngle!=afFinalAngle)
		{
			float fAngleDist = afFinalAngle-afAngle;
			if( (afFinalAngle>afAngle && fAngleDist<afMaxAngle) || 
				(afFinalAngle<afAngle && fAngleDist<(-afMaxAngle)))
				afAngle = afAngle + afSpeed;
			else
				afAngle = afAngle + (-afSpeed);
		}
		if(Abs(GetAngleDistance(afAngle, afFinalAngle,afMaxAngle*2))<=(afSpeed*1.5))afAngle= afFinalAngle;

		return afAngle;
	}

	float cMath::TurnAngleRad(float afAngle,float afFinalAngle,float afSpeed)
	{
		return TurnAngle(afAngle, afFinalAngle, afSpeed,kPif);
	}

	float cMath::TurnAngleDeg(float afAngle,float afFinalAngle,float afSpeed)
	{
		return TurnAngle(afAngle, afFinalAngle, afSpeed,180.0f);
	}

	//-----------------------------------------------------------------------

	float cMath::GetAngleFromPoints2D(const cVector2f &avStartPos, const cVector2f &avGoalPos)
	{
		float fDx;
		float fDy;
		float fAns;

		fDx = avGoalPos.x - avStartPos.x;
		fDy = avGoalPos.y - avStartPos.y;
		if(fDx==0)fDx= 0.00001f;
		if(fDy==0)fDy= 0.00001f;


		if(fDx>=0 && fDy<0)
		{
			fAns = atan(fDx/(-fDy));
			//Log("1\n");
		}
		else if(fDx>=0 && fDy>=0)
		{
			fAns = atan(fDy/fDx) + kPi2f;
			//Log("2\n");
		}
		else if(fDx<0 && fDy>=0)
		{
			fAns = atan((-fDx)/fDy)+kPif;
			//Log("3\n");
		}
		else if(fDx<0 && fDy<0)
		{
			fAns = atan((-fDy)/(-fDx))+kPi2f+kPif;
			//Log("4\n");
		}
		return fAns;
	}

	//-----------------------------------------------------------------------

	float cMath::ToRad(float afAngle)
	{
		return (afAngle/360.0f)*k2Pif;
	}

	//-----------------------------------------------------------------------

	float cMath::ToDeg(float afAngle)
	{
		return (afAngle/k2Pif)*360.0f;
	}

	//-----------------------------------------------------------------------

	int cMath::Log2ToInt(int alX)
	{
		switch(alX)
		{
		case 1:		return 0;
		case 2:		return 1;
		case 4:		return 2;
		case 8:		return 3;
		case 16:	return 4;
		case 32:	return 5;
		case 64:	return 6;
		case 128:	return 7;
		case 256:	return 8;
		case 512:	return 9;
		default: return (int)floor((log((float)alX) / log(2.0f)) + 0.5f);
		}
		//return (int)floor((log((float)aX) / log(2.0f)) + 0.5f);
	}

	//-----------------------------------------------------------------------

	int cMath::Pow2(int alX)
	{
		return 1 << alX;
	}

	//-----------------------------------------------------------------------

	bool cMath::IsPow2(int alX)
	{
		switch(alX)
		{
		case 0:		return true;
		case 1:		return true;
		case 2:		return true;
		case 4:		return true;
		case 8:		return true;
		case 16:	return true;
		case 32:	return true;
		case 64:	return true;
		case 128:	return true;
		case 256:	return true;
		case 512:	return true;
		case 1024:	return true;
		case 2048:	return true;
		case 4096:	return true;
		case 8192:	return true;
		default: return false;
		}
	}

	//-----------------------------------------------------------------------

	float cMath::Interpolate(float afA,float afB,float afT)
	{
		return afA * (1-afT) + afB * afT; 
	}

	//-----------------------------------------------------------------------
	
	float cMath::IncreaseTo(float afX, float afAdd, float afDest)
	{
		if(afX < afDest){
			afX += afAdd;
			if(afX > afDest) afX = afDest;
		}
		else if(afX > afDest){
			afX -= afAdd;
			if(afX < afDest) afX = afDest;
		}
		
		return afX;
	}

	//-----------------------------------------------------------------------

	float cMath::SmoothCurve(float afX)
	{
		return afX*afX*(3-afX*2);
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// VECTOR 2D
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	float cMath::Vector2Dist(const cVector2f &avPosA,const cVector2f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDy = avPosA.y - avPosB.y;

		return sqrt(fDx*fDx + fDy*fDy);
	}
	
	//-----------------------------------------------------------------------

	float cMath::Vector2DistXY(const cVector3f &avPosA,const cVector3f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDy = avPosA.y - avPosB.y;

		return sqrt(fDx*fDx + fDy*fDy);
	}

	float cMath::Vector2DistXZ(const cVector3f &avPosA,const cVector3f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDz = avPosA.z - avPosB.z;

		return sqrt(fDx*fDx + fDz*fDz);
	}
	
	//-----------------------------------------------------------------------

	float cMath::Vector2DistSqr(const cVector2f &avPosA,const cVector2f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDy = avPosA.y - avPosB.y;

		return fDx*fDx + fDy*fDy;
	}

	//-----------------------------------------------------------------------

	float cMath::Vector2DistSqrXY(const cVector3f &avPosA,const cVector3f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDy = avPosA.y - avPosB.y;

		return fDx*fDx + fDy*fDy;
	}

	//-----------------------------------------------------------------------

	float cMath::Vector2DistSqrXZ(const cVector3f &avPosA,const cVector3f &avPosB)
	{
		float fDx = avPosA.x - avPosB.x;
		float fDz = avPosA.z - avPosB.z;

		return fDx*fDx + fDz*fDz;
	}

	//-----------------------------------------------------------------------

	cVector2f cMath::Vector2IncreaseTo(const cVector2f& avX, const cVector2f& avAdd, const cVector2f& avDest)
	{
		cVector2f vRet;
		vRet.x = IncreaseTo(avX.x, avAdd.x, avDest.x);
		vRet.y = IncreaseTo(avX.y, avAdd.y, avDest.y);

		return vRet;
	}

	//-----------------------------------------------------------------------

	
	cVector2f cMath::GetVectorFromAngle2D(float afAngle, float afLength)
	{
		cVector2f vVec;

		vVec.x = sin(afAngle) * afLength;
		vVec.y = -cos(afAngle) * afLength;

		return vVec;
	}
	
	//-----------------------------------------------------------------------

	void cMath::GetAngleFromVector(const cVector2f &avVec, float *apAngle, float *apLength)
	{
		*apLength = sqrt(avVec.x*avVec.x + avVec.y*avVec.y);

		*apAngle = GetAngleFromPoints2D(0,avVec);
	}

	//-----------------------------------------------------------------------

	cVector2f cMath::ProjectVector2D(const cVector2f &avSrcVec, const cVector2f &avDestVec)
	{
		float fTemp = (avSrcVec.x*avDestVec.x + avSrcVec.y*avDestVec.y) /
						(avDestVec.x*avDestVec.x + avDestVec.y*avDestVec.y);

		return  avDestVec * fTemp;
	}
	
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// VECTOR 3D
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3MaxLength(const cVector3f &avVec, float afMaxLength)
	{
		float fLength = avVec.Length();
		if(fLength > afMaxLength)
		{
			return (avVec / fLength) * afMaxLength;
		}
		else
		{
			return avVec;
		}
	}
	
	cVector3f cMath::Vector3MinLength(const cVector3f &avVec, float afMinLength)
	{
		float fLength = avVec.Length();
		if(fLength < afMinLength)
		{
			return (avVec / fLength) * afMinLength;
		}
		else
		{
			return avVec;
		}
	}
	
	cVector3f cMath::Vector3ClampLength(const cVector3f &avVec,float afMinLength, float afMaxLength)
	{
		float fLength = avVec.Length();
		if(fLength < afMinLength)
		{
			return (avVec / fLength) * afMinLength;
		}
		else if(fLength > afMaxLength)
		{
			return (avVec / fLength) * afMaxLength;
		}
		else
		{
			return avVec;
		}
	}
	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3AngleDistance(const cVector3f &avAngles1, const cVector3f &avAngles2, float afMaxAngle)
	{
		return cVector3f(	GetAngleDistance(avAngles1.x, avAngles2.x, afMaxAngle),
							GetAngleDistance(avAngles1.y, avAngles2.y, afMaxAngle),
							GetAngleDistance(avAngles1.z, avAngles2.z, afMaxAngle)
							);
	}

	cVector3f cMath::Vector3AngleDistanceRad(const cVector3f &avAngles1, const cVector3f &avAngles2)
	{
		return Vector3AngleDistance(avAngles1, avAngles2, k2Pif);
	}

	cVector3f cMath::Vector3AngleDistanceDeg(const cVector3f &avAngles1, const cVector3f &avAngles2)
	{
		return Vector3AngleDistance(avAngles1, avAngles2, 360.0f);
	}
	
	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3Cross(const cVector3f& avVecA,const cVector3f& avVecB)
	{
		cVector3f vResult;
		
		vResult.x = avVecA.y * avVecB.z - avVecA.z * avVecB.y;
		vResult.y = avVecA.z * avVecB.x - avVecA.x * avVecB.z;
		vResult.z = avVecA.x * avVecB.y - avVecA.y * avVecB.x;
		
		return vResult;
	}

	//-----------------------------------------------------------------------

	float cMath::Vector3Dot(const cVector3f& avVecA,const cVector3f& avVecB)
	{
		return avVecA.x*avVecB.x + avVecA.y*avVecB.y + avVecA.z*avVecB.z;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3Project(const cVector3f &avSrcVec, const cVector3f &avDestVec)
	{
		float fTemp = (avSrcVec.x*avDestVec.x + avSrcVec.y*avDestVec.y + avSrcVec.z*avDestVec.z) /
					(avDestVec.x*avDestVec.x + avDestVec.y*avDestVec.y + avDestVec.z*avDestVec.z);

		return  avDestVec * fTemp;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3Reflect(const cVector3f &avVec, const cVector3f &avNormal)
	{
		return avVec - avNormal*Vector3Dot(avVec, avNormal)*2;
	}

	//-----------------------------------------------------------------------

	float cMath::Vector3Angle(const cVector3f& avVecA,const cVector3f& avVecB)
	{
		float fCos = Vector3Dot(avVecA,avVecB);

		if(std::abs(fCos - 1) <= kEpsilonf) return 0;
		
		return acos(fCos);
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3UnProject(const cVector3f& avVec,const cRect2f &aScreenRect, 
										cMatrixf a_mtxViewProj)
	{
		cMatrixf mtxInvViewProj = MatrixInverse(a_mtxViewProj);

		cVector3f vNormalized;
		vNormalized.x =  ((avVec.x - aScreenRect.x)*2.0f / aScreenRect.w) - 1.0f;
		vNormalized.y = -(((avVec.y - aScreenRect.y)*2.0f / aScreenRect.h) - 1.0f);
		vNormalized.z = 2.0f*avVec.z - 1.0f;

		//Log("Normalized: %s\n",vNormalized.ToString().c_str());
		
		// Object coordinates.
		vNormalized = MatrixMulDivideW(mtxInvViewProj,vNormalized);
		//vNormalized = MatrixMul(mtxInvViewProj,vNormalized);

		//Log("Normalized After: %s\n",vNormalized.ToString().c_str());
		
		return vNormalized *-1;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::Vector3IncreaseTo(const cVector3f& avX, const cVector3f& avAdd, const cVector3f& avDest)
	{
		cVector3f vRet;
		vRet.x = IncreaseTo(avX.x, avAdd.x, avDest.x);
		vRet.y = IncreaseTo(avX.y, avAdd.y, avDest.y);
		vRet.z = IncreaseTo(avX.z, avAdd.z, avDest.z);

		return vRet;
	}

	//-----------------------------------------------------------------------

	void cMath::Vector3ClampToLength(cVector3f& avVec, float afMaxLength)
	{
		if(afMaxLength <=0) return;

        float fLength = avVec.Length();
		if(fLength > afMaxLength) avVec = (avVec / fLength) * afMaxLength;
	}

	//-----------------------------------------------------------------------
	
	//Helper function for GetAnglesFromPoints3D
	static float GetAngleFromPoints2DSimple(const cVector3f &avStartPos, const cVector3f &avGoalPos)
	{
		cVector3f vDelta = avGoalPos - avStartPos;
		
		if(vDelta.x == 0)vDelta.x = -kEpsilonf;
		if(vDelta.y == 0)vDelta.y = kEpsilonf;

		if(vDelta.y>=0 && vDelta.x<=0)
		{
			return -atan(vDelta.y/vDelta.x);
		}
		else if(vDelta.y<0 && vDelta.x<=0)
		{
			return -atan(vDelta.y/vDelta.x);
		}
		else
		{
			Error("Error in GetAngle 3D code! ARGHHH run in terror\n");
			return -1000;
		}
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::GetAngleFromPoints3D(const cVector3f &avStartPos, const cVector3f &avGoalPos)
	{
		cVector3f vAngle = cVector3f(0,0,0);
		
		vAngle.y = -GetAngleFromPoints2D(cVector2f(avStartPos.x,avStartPos.z),
										cVector2f(avGoalPos.x, avGoalPos.z));

		//Log("Y Angle: %f\n",vAngle.y);

		//vAngle.y = Wrap(vAngle.y,0.0f, k2Pif);

		cMatrixf mtxRot = MatrixRotateY(-vAngle.y);
		cVector3f vDelta = avGoalPos - avStartPos;
		
		//Log("vDelta: %s\n",vDelta.ToString().c_str());

		cVector3f vGoal = MatrixMul(mtxRot, vDelta);

		//Log("vGoal: %s\n",vGoal.ToString().c_str());

		vAngle.x = GetAngleFromPoints2DSimple(cVector3f(0,0,0),cVector2f(vGoal.z, vGoal.y));
		
		//Log("X Angle: %f\n",vAngle.x);

		vAngle.x = Wrap(vAngle.x,0.0f, k2Pif); 
		
		return vAngle;
	}

	//-----------------------------------------------------------------------

	void cMath::Vector3OrthonormalizeBasis(const cVector3f& av1, const cVector3f& av2, const cVector3f& av3,
											cVector3f& avDst1, cVector3f& avDst2, cVector3f& avDst3)
	{
		//////////////////////////////////////////////////////////////////////////
		// Orthonormalization of a non orthonormal basis via Gram-Schmidt method
		cVector3f u1 = av1;
		avDst1 = u1;
		avDst1.Normalize();

		cVector3f u2 = av2 - Vector3Project(av2, u1);
		avDst2 = u2;
		avDst2.Normalize();

		cVector3f u3 = av3 - Vector3Project(av3, u1) - Vector3Project(av3, u2);
		avDst3 = u3;
		avDst3.Normalize();

		/*float fInvLen1 = 1/u1.Length();
		float fInvLen1Sqr = fInvLen1*fInvLen1;
		avDst1 = u1*fInvLen1;

		cVector3f u2 = av2 - Vector3Dot(av2, u1)*fInvLen1Sqr;
		float fInvLen2 = 1/u2.Length();
		float fInvLen2Sqr = fInvLen2*fInvLen2;
		avDst2 = u2*fInvLen2;

		cVector3f u3 = av3 - Vector3Dot(av3, u1)*fInvLen1Sqr - Vector3Dot(av3,u2)*fInvLen2Sqr;
		float fInvLen3 = 1/u3.Length();
		avDst3 = u3*fInvLen3;
		*/
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PLANES
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	float cMath::PlaneToPointDist(const cPlanef& aPlane, const cVector3f& avVec)
	{
		return (aPlane.a * avVec.x) + (aPlane.b * avVec.y) + (aPlane.c * avVec.z) + aPlane.d;
	}
	
	//-----------------------------------------------------------------------
	
	//TODO: This only works when there is no 0 in the planes.
	void cMath::PlaneIntersectionLine(	const cPlanef& aPA, const cPlanef& aPB, 
										cVector3f &avDir, cVector3f &avPoint)
	{
        avDir = Vector3Cross(cVector3f(aPA.a,aPA.b,aPA.c), cVector3f(aPB.a,aPB.b,aPB.c));

		//Set x to 0 so the calculation can be solved
		avPoint.x = 0;

		//find a value that sets b + b to 0.
		float fVal = (-aPB.b) / aPA.b;

		//Get z 
		avPoint.z = ((aPA.d * fVal) + aPB.d) / ((aPA.c * fVal) + aPB.c);

		//Get y
		avPoint.y = (aPA.d - (aPA.c *  avPoint.z)) / aPA.b;
	}

	//-----------------------------------------------------------------------

	bool cMath::PlaneIntersectionPoint(const cPlanef& aP1, const cPlanef& aP2, const cPlanef& aP3, cVector3f& avPoint)
	{
		///////////////////////////
		// Check if plane is parallel to another, if so return false
		if(PlaneParallel(aP1,aP2) || PlaneParallel(aP1,aP3) || PlaneParallel(aP2,aP3)) return false;

		cVector3f vN1 = aP1.GetNormal();
		cVector3f vN2 = aP2.GetNormal();
		cVector3f vN3 = aP3.GetNormal();

        cVector3f vCross12 = Vector3Cross(vN1, vN2);
		cVector3f vCross31 = Vector3Cross(vN3, vN1);
		cVector3f vCross23 = Vector3Cross(vN2, vN3);
		
		avPoint = (vCross23* -aP1.d + vCross31*-aP2.d + vCross12*-aP3.d) / Vector3Dot(vN1,vCross23);
		return true;
	}

	//-----------------------------------------------------------------------

	eCollision cMath::CheckPlaneSphereCollision(const cPlanef& aPlane, const cVector3f &avCenter, float afRadius)
	{
		float fDist = cMath::PlaneToPointDist(aPlane, avCenter);

		if(Abs(fDist) <= afRadius)	return eCollision_Intersect;
		if(fDist > afRadius)		return eCollision_Inside;
		else						return eCollision_Outside;
	}

	//-----------------------------------------------------------------------
	
	eCollision cMath::CheckPlaneBVCollision(const cPlanef& aPlane, cBoundingVolume &aBV)
	{
		return CheckPlaneAABBCollision(aPlane, aBV.GetMin(), aBV.GetMax());//, aBV.GetWorldCenter(), aBV.GetRadius());
	}
	
	//-----------------------------------------------------------------------

	eCollision cMath::CheckPlaneAABBCollision(	const cPlanef& aPlane, const cVector3f &avMin, const cVector3f &avMax)
	{
		cVector3f vCorners[8] ={
				cVector3f(avMax.x,avMax.y,avMax.z),
				cVector3f(avMax.x,avMax.y,avMin.z),
				cVector3f(avMax.x,avMin.y,avMax.z),
				cVector3f(avMax.x,avMin.y,avMin.z),

				cVector3f(avMin.x,avMax.y,avMax.z),
				cVector3f(avMin.x,avMax.y,avMin.z),
				cVector3f(avMin.x,avMin.y,avMax.z),
				cVector3f(avMin.x,avMin.y,avMin.z),
		};

		int lInCount = 8;
		for(int j=0;j<8;j++)
		{
			float fDist = cMath::PlaneToPointDist(aPlane,vCorners[j]);
			if(fDist < 0) lInCount--;
		}

		if(lInCount==0)			return eCollision_Outside;
		else if(lInCount == 8)	return eCollision_Inside;
		else					return eCollision_Intersect;	
	}

	//-----------------------------------------------------------------------


	eCollision cMath::CheckPlaneAABBCollision(const cPlanef& aPlane, const cVector3f &avMin, const cVector3f &avMax, const cVector3f &avSphereCenter,
											float afSphereRadius)
	{
		///////////////////////////////
		// Sphere collision
		float fDist = cMath::PlaneToPointDist(aPlane, avSphereCenter);
		
		if(fDist < -afSphereRadius) return eCollision_Outside;
		if(fDist > afSphereRadius)	return eCollision_Inside;

		///////////////////////////////
		// AABB collision
		return CheckPlaneAABBCollision(aPlane, avMin, avMax);
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckPlaneLineIntersection(	const cPlanef& aPlane, const cVector3f &avLineStart, const cVector3f &avLineEnd,
											cVector3f* apIntersectionPos, float *apT)
	{
		cVector3f vLineDelta = avLineEnd - avLineStart;

		float fNegStartToPlaneDist = -PlaneToPointDist(aPlane, avLineStart);
		float fDeltaPlaneNormalMul = aPlane.a*vLineDelta.x + aPlane.b*vLineDelta.y + aPlane.c*vLineDelta.z;

		//Line is on the plane or parallel
		if(fDeltaPlaneNormalMul == 0)
		{
			return false;
		}

		float fT = fNegStartToPlaneDist / fDeltaPlaneNormalMul;

		if(apT) *apT = fT;
		if(apIntersectionPos) *apIntersectionPos = avLineStart + vLineDelta * fT;

		//Is there an intersection
		if(fT < 0 || fT > 1) return false;

		return true;
	}

	//-----------------------------------------------------------------------

	eCollision cMath::CheckPointsPlanesCollision(cVector3f *apVertices, int alNumOfVertices, cPlanef *apPlanes, int alNumOfPlanes)
	{
		int lTotalIn =0;
		////////////////////////////
		// Iterate planes
		for(int i=0; i<alNumOfPlanes; i++)
		{
			cPlanef& plane = apPlanes[i];
			int lInCount=alNumOfVertices;
			
			////////////////////////////
			// Iterate points and check if inside
			for(int j=0;j<alNumOfVertices;j++)
			{
				float fDist = cMath::PlaneToPointDist(plane, apVertices[j]);
				if(fDist < 0) lInCount--;
			}

			if(lInCount==0) return eCollision_Outside;
			if(lInCount==alNumOfVertices) lTotalIn++;
		}

		if(lTotalIn == alNumOfPlanes) return eCollision_Inside;

		return eCollision_Intersect;
	}

	//-----------------------------------------------------------------------

	eCollision cMath::CheckPointsAABBPlanesCollision(cVector3f *apVertices, int alNumOfVertices, const cVector3f& avMin, const cVector3f& avMax)
	{
		cPlanef vAABBPlanes[6] = {
				cPlanef(-1,0,0, avMax.x),//X-max
				cPlanef(1,0,0,  -avMin.x),//X-min
				cPlanef(0,-1,0, avMax.y),//Y-max
				cPlanef(0, 1,0, -avMin.y),//Y-min
				cPlanef(0,0,-1, avMax.z),//Z-max
				cPlanef(0,0,1,  -avMin.z),//Z-min
		};

		return cMath::CheckPointsPlanesCollision(apVertices, alNumOfVertices, vAABBPlanes, 6);
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckSeparatingAxisIntersection(cVector3f *apVerticesA, int alNumOfVerticesA, cPlanef *apPlanesA, int alNumOfPlanesA,
												cVector3f *apVerticesB, int alNumOfVerticesB, cPlanef *apPlanesB, int alNumOfPlanesB)
	{
		int vPlaneNumVec[2] = {alNumOfPlanesA, alNumOfPlanesB};
		cPlanef* vPlaneVec[2] = {apPlanesA, apPlanesB};
		int vVtxNumVec[2] = {alNumOfVerticesA, alNumOfVerticesB};
		cVector3f* vVtxVec[2] = {apVerticesA, apVerticesB};
		cVector3f vMin[2], vMax[2];

		/////////////////////////////////
		// Go through A and B
		for(int planevec=0; planevec<2; ++planevec)
		{
			cPlanef *pPlanes = vPlaneVec[planevec];
			int lNumOfPlanes = vPlaneNumVec[planevec];

			if(pPlanes==NULL || lNumOfPlanes==0) continue;

			/////////////////////////////////
			// Iterate planes
			for(int plane=0; plane<lNumOfPlanes; ++plane)
			{
				cVector3f vNormal = pPlanes[plane].GetNormal();
				
				/////////////////////////////////
				// Iterate points and project on normal
                for(int vtxvec=0; vtxvec < 2; ++vtxvec)
				{
					cVector3f *pVertices = vVtxVec[vtxvec];
					int lNumOfVertices = vVtxNumVec[vtxvec];
					
					if(pVertices==NULL || lNumOfVertices==0) continue;
					
					/////////////////////////////////
					// Project the points and get min / max
					vMax[vtxvec] = vMin[vtxvec] = 	Vector3Dot(pVertices[0], vNormal);
					for(int vtx=1; vtx< lNumOfVertices; ++vtx)
					{
						float fDot = Vector3Dot(vVtxVec[vtxvec][vtx], vNormal);
						if(vMin[vtxvec] > fDot)			vMin[vtxvec] = fDot;
						else if(vMax[vtxvec] < fDot)	vMax[vtxvec] = fDot;
					}
				}

				if( vMin[0] > vMax[1] || vMin[1] > vMax[0]) return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// TRIANGLES
	////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cMath::CheckLineTriangleIntersection(	const cVector3f& avLineStart, const cVector3f& avLineEnd, 
												const cVector3f& avP0, const cVector3f& avP1, const cVector3f& avP2,
												float *apT, bool abSkipBackfacing)
	{
		cVector3f vLineDelta = avLineEnd - avLineStart;
		
		cVector3f vEdge1 = avP1 - avP0;
		cVector3f vEdge2 = avP2 - avP0;

		if(abSkipBackfacing)
		{
			cVector3f vNormal = Vector3Cross(vEdge1, vEdge2);

			float fDDotN = Vector3Dot(vNormal, vLineDelta);
			if(fDDotN<0)
				return false;
		}


		cVector3f vDCrossE = Vector3Cross(vLineDelta, vEdge2);

        float fEDotN = Vector3Dot(vEdge1, vDCrossE);
		if(fabs(fEDotN) < 0.00001f) return false; //Line is parallel
		
		float fF = 1.0f / fEDotN;
		
		cVector3f vTriToStart = avLineStart - avP0;
		
		//Get U
		float fU = fF * Vector3Dot(vTriToStart, vDCrossE);
		if(fU < 0.0f || fU > 1.0f) return false;

		//Get V
		cVector3f vQ = Vector3Cross(vTriToStart,vEdge1);
		float fV = fF * Vector3Dot(vLineDelta, vQ);
		if (fV < 0.0 || fU + fV > 1.0) return false;

        //Get T
		float fT = fF * Vector3Dot(vEdge2, vQ);
        if(fT <0 || fT >1.0f) return false;

		if(apT) *apT = fT;
		return true;
	}
	
	//-----------------------------------------------------------------------

	bool cMath::CheckLineTriMeshIntersection(	const cVector3f& avLineStart, const cVector3f& avLineEnd,
												const cMatrixf& a_mtxInvMeshMtx,
												const unsigned int* apIndexArray,int alIndexNum,
												const float* apVertexArray, int alVtxStride,
												cVector3f *apIntersectionPos, float *apT, int *apTriIndex, bool abSkipBackfacing)
	{
		////////////////////////////
		// Transform the line to local mesh space
		cVector3f vLocalLineStart = MatrixMul(a_mtxInvMeshMtx, avLineStart);
		cVector3f vLocalLineEnd = MatrixMul(a_mtxInvMeshMtx, avLineEnd);
		

		////////////////////////////
		// Set up variables
		float fMinT = 99999.0f;
		int lTriIdx = -1;
        
		////////////////////////////
		// Iterate triangles
		for(int idx=0; idx < alIndexNum; idx+=3)
		{
			// Get the vertex positions
			const float *pVtx0 = &apVertexArray[apIndexArray[idx]*alVtxStride];
			const float *pVtx1 = &apVertexArray[apIndexArray[idx+1]*alVtxStride];
			const float *pVtx2 = &apVertexArray[apIndexArray[idx+2]*alVtxStride];
            
			//Get triangles positions
            cVector3f vP0(pVtx0[0], pVtx0[1], pVtx0[2]);
			cVector3f vP1(pVtx1[0], pVtx1[1], pVtx1[2]);
			cVector3f vP2(pVtx2[0], pVtx2[1], pVtx2[2]);

			float fT;
			if(CheckLineTriangleIntersection( vLocalLineStart, vLocalLineEnd, vP0, vP1, vP2, &fT, abSkipBackfacing))
			{
				if(fT < fMinT)
				{
					fMinT = fT;
					lTriIdx = idx;
				}
			}
		}

        ////////////////////////////
		// Check if the there was a proper intersection
		if(fMinT < 0 || fMinT >1.0f) return false;

		if(apT) *apT = fMinT;

		////////////////////////////
		// Get intersection position
		if(apIntersectionPos)
		{
			*apIntersectionPos = avLineStart + (avLineEnd - avLineStart)*fMinT;
		}
		if(apTriIndex)
		{
			*apTriIndex = lTriIdx;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CheckLineTriVertexBufferIntersection( const cVector3f& avLineStart, const cVector3f& avLineEnd,
														const cMatrixf& a_mtxInvMeshMtx, iVertexBuffer *apVtxBuffer,
														cVector3f *apIntersectionPos, float *apT, int *apTriIndex, bool abSkipBackfacing)
	{
		return CheckLineTriMeshIntersection(avLineStart, avLineEnd, 
											a_mtxInvMeshMtx,
											apVtxBuffer->GetIndices(), apVtxBuffer->GetIndexNum(),
											apVtxBuffer->GetFloatArray(eVertexBufferElement_Position), 
											apVtxBuffer->GetElementNum(eVertexBufferElement_Position),
											apIntersectionPos, apT, apTriIndex, abSkipBackfacing);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// QUATERNIONS
	////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	float cMath::QuaternionDot(const cQuaternion& aqA,const cQuaternion& aqB)
	{
		return aqA.w*aqB.w + aqA.v.x*aqB.v.x + aqA.v.y*aqB.v.y + aqA.v.z*aqB.v.z;
	}

	//-----------------------------------------------------------------------

	cQuaternion cMath::QuaternionMul(const cQuaternion& aqA,const cQuaternion& aqB)
	{
		cQuaternion qOut;

		qOut.w =	aqA.w * aqB.w -		aqA.v.x * aqB.v.x -		aqA.v.y * aqB.v.y -		aqA.v.z * aqB.v.z;
		qOut.v.x =	aqA.w * aqB.v.x +	aqA.v.x * aqB.w +		aqA.v.y * aqB.v.z -		aqA.v.z * aqB.v.y;
		qOut.v.y =	aqA.w * aqB.v.y +	aqA.v.y * aqB.w +		aqA.v.z * aqB.v.x -		aqA.v.x * aqB.v.z;
		qOut.v.z =	aqA.w * aqB.v.z +	aqA.v.z * aqB.w +		aqA.v.x * aqB.v.y -		aqA.v.y * aqB.v.x;

		return qOut;		
	}
	
	//-----------------------------------------------------------------------

	cQuaternion cMath::QuaternionSlerp(float afT,const cQuaternion& aqA, const cQuaternion& aqB, 
									bool abShortestPath)
	{
		float fCos = QuaternionDot(aqA,aqB);
		
		//If the rotations are the same, return linear interpolation instead of spherical
		if ( std::abs(fCos - 1) <= kEpsilonf)
		{
			cQuaternion qLerp(aqA * (1.0f - afT) + aqB * afT);
			qLerp.Normalize();
			return qLerp;
		}

		float fAngle = acos(fCos);

		float fSin = sin(fAngle);
		float fInvSin = 1.0f/fSin;
		float fCoeff0 = sin((1.0f-afT)*fAngle)*fInvSin;
		float fCoeff1 = sin(afT*fAngle)*fInvSin;
		// Do we need to invert rotation?
		if (fCos < 0.0f && abShortestPath)
		{
			fCoeff0 = -fCoeff0;
			// taking the complement requires renormalisation
			cQuaternion qT(aqA * fCoeff0 + aqB*fCoeff1);
			qT.Normalize();
			return qT;
		}
		else
		{
			return aqA*fCoeff0 + aqB*fCoeff1;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// MATRICES
	////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixSlerp(float afT,const cMatrixf& a_mtxA, const cMatrixf& a_mtxB, 
										bool abShortestPath)
	{
		if(afT <= 0) return a_mtxA;
		if(afT >= 1) return a_mtxB;

		cVector3f vPos =  a_mtxA.GetTranslation() * (1- afT) + a_mtxB.GetTranslation() * afT;

		cQuaternion qA; qA.FromRotationMatrix(a_mtxA);
		cQuaternion qB; qB.FromRotationMatrix(a_mtxB);

		cQuaternion qFinal = cMath::QuaternionSlerp(afT,qA,qB,true);

		cMatrixf mtxFinal = cMath::MatrixQuaternion(qFinal);
		mtxFinal.SetTranslation(vPos);

		return mtxFinal;
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixMul(const cMatrixf &a_mtxA,const cMatrixf &a_mtxB)
	{
		cMatrixf mtxC;
		
		mtxC.m[0][0] = a_mtxA.m[0][0] * a_mtxB.m[0][0] + a_mtxA.m[0][1] * a_mtxB.m[1][0] + a_mtxA.m[0][2] * a_mtxB.m[2][0] + a_mtxA.m[0][3] * a_mtxB.m[3][0];
		mtxC.m[0][1] = a_mtxA.m[0][0] * a_mtxB.m[0][1] + a_mtxA.m[0][1] * a_mtxB.m[1][1] + a_mtxA.m[0][2] * a_mtxB.m[2][1] + a_mtxA.m[0][3] * a_mtxB.m[3][1];
		mtxC.m[0][2] = a_mtxA.m[0][0] * a_mtxB.m[0][2] + a_mtxA.m[0][1] * a_mtxB.m[1][2] + a_mtxA.m[0][2] * a_mtxB.m[2][2] + a_mtxA.m[0][3] * a_mtxB.m[3][2];
		mtxC.m[0][3] = a_mtxA.m[0][0] * a_mtxB.m[0][3] + a_mtxA.m[0][1] * a_mtxB.m[1][3] + a_mtxA.m[0][2] * a_mtxB.m[2][3] + a_mtxA.m[0][3] * a_mtxB.m[3][3];

		mtxC.m[1][0] = a_mtxA.m[1][0] * a_mtxB.m[0][0] + a_mtxA.m[1][1] * a_mtxB.m[1][0] + a_mtxA.m[1][2] * a_mtxB.m[2][0] + a_mtxA.m[1][3] * a_mtxB.m[3][0];
		mtxC.m[1][1] = a_mtxA.m[1][0] * a_mtxB.m[0][1] + a_mtxA.m[1][1] * a_mtxB.m[1][1] + a_mtxA.m[1][2] * a_mtxB.m[2][1] + a_mtxA.m[1][3] * a_mtxB.m[3][1];
		mtxC.m[1][2] = a_mtxA.m[1][0] * a_mtxB.m[0][2] + a_mtxA.m[1][1] * a_mtxB.m[1][2] + a_mtxA.m[1][2] * a_mtxB.m[2][2] + a_mtxA.m[1][3] * a_mtxB.m[3][2];
		mtxC.m[1][3] = a_mtxA.m[1][0] * a_mtxB.m[0][3] + a_mtxA.m[1][1] * a_mtxB.m[1][3] + a_mtxA.m[1][2] * a_mtxB.m[2][3] + a_mtxA.m[1][3] * a_mtxB.m[3][3];

		mtxC.m[2][0] = a_mtxA.m[2][0] * a_mtxB.m[0][0] + a_mtxA.m[2][1] * a_mtxB.m[1][0] + a_mtxA.m[2][2] * a_mtxB.m[2][0] + a_mtxA.m[2][3] * a_mtxB.m[3][0];
		mtxC.m[2][1] = a_mtxA.m[2][0] * a_mtxB.m[0][1] + a_mtxA.m[2][1] * a_mtxB.m[1][1] + a_mtxA.m[2][2] * a_mtxB.m[2][1] + a_mtxA.m[2][3] * a_mtxB.m[3][1];
		mtxC.m[2][2] = a_mtxA.m[2][0] * a_mtxB.m[0][2] + a_mtxA.m[2][1] * a_mtxB.m[1][2] + a_mtxA.m[2][2] * a_mtxB.m[2][2] + a_mtxA.m[2][3] * a_mtxB.m[3][2];
		mtxC.m[2][3] = a_mtxA.m[2][0] * a_mtxB.m[0][3] + a_mtxA.m[2][1] * a_mtxB.m[1][3] + a_mtxA.m[2][2] * a_mtxB.m[2][3] + a_mtxA.m[2][3] * a_mtxB.m[3][3];

		mtxC.m[3][0] = a_mtxA.m[3][0] * a_mtxB.m[0][0] + a_mtxA.m[3][1] * a_mtxB.m[1][0] + a_mtxA.m[3][2] * a_mtxB.m[2][0] + a_mtxA.m[3][3] * a_mtxB.m[3][0];
		mtxC.m[3][1] = a_mtxA.m[3][0] * a_mtxB.m[0][1] + a_mtxA.m[3][1] * a_mtxB.m[1][1] + a_mtxA.m[3][2] * a_mtxB.m[2][1] + a_mtxA.m[3][3] * a_mtxB.m[3][1];
		mtxC.m[3][2] = a_mtxA.m[3][0] * a_mtxB.m[0][2] + a_mtxA.m[3][1] * a_mtxB.m[1][2] + a_mtxA.m[3][2] * a_mtxB.m[2][2] + a_mtxA.m[3][3] * a_mtxB.m[3][2];
		mtxC.m[3][3] = a_mtxA.m[3][0] * a_mtxB.m[0][3] + a_mtxA.m[3][1] * a_mtxB.m[1][3] + a_mtxA.m[3][2] * a_mtxB.m[2][3] + a_mtxA.m[3][3] * a_mtxB.m[3][3];

		return mtxC;	
	}
	
	//-----------------------------------------------------------------------
	
	cVector3f cMath::MatrixMul(const cMatrixf &a_mtxA,const cVector3f &avB)
	{
		cVector3f vC;
		
		vC.x = ( a_mtxA.m[0][0] * avB.x + a_mtxA.m[0][1] * avB.y + a_mtxA.m[0][2] * avB.z + a_mtxA.m[0][3] );
		vC.y = ( a_mtxA.m[1][0] * avB.x + a_mtxA.m[1][1] * avB.y + a_mtxA.m[1][2] * avB.z + a_mtxA.m[1][3] );
		vC.z = ( a_mtxA.m[2][0] * avB.x + a_mtxA.m[2][1] * avB.y + a_mtxA.m[2][2] * avB.z + a_mtxA.m[2][3] );

		return vC;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::MatrixMul3x3(const cMatrixf &a_mtxA,const cVector3f &avB)
	{
		cVector3f vC;

		vC.x = ( a_mtxA.m[0][0] * avB.x + a_mtxA.m[0][1] * avB.y + a_mtxA.m[0][2] * avB.z );
		vC.y = ( a_mtxA.m[1][0] * avB.x + a_mtxA.m[1][1] * avB.y + a_mtxA.m[1][2] * avB.z );
		vC.z = ( a_mtxA.m[2][0] * avB.x + a_mtxA.m[2][1] * avB.y + a_mtxA.m[2][2] * avB.z );

		return vC;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::MatrixMulDivideW(const cMatrixf &a_mtxA,const cVector3f &avB)
	{
		cVector3f vC;

		float fInvW = 1.0f / ( a_mtxA.m[3][0] * avB.x + a_mtxA.m[3][1] * avB.y + a_mtxA.m[3][2] * avB.z + a_mtxA.m[3][3] );

		vC.x = ( a_mtxA.m[0][0] * avB.x + a_mtxA.m[0][1] * avB.y + a_mtxA.m[0][2] * avB.z + a_mtxA.m[0][3] ) * fInvW;
		vC.y = ( a_mtxA.m[1][0] * avB.x + a_mtxA.m[1][1] * avB.y + a_mtxA.m[1][2] * avB.z + a_mtxA.m[1][3] ) * fInvW;
		vC.z = ( a_mtxA.m[2][0] * avB.x + a_mtxA.m[2][1] * avB.y + a_mtxA.m[2][2] * avB.z + a_mtxA.m[2][3] ) * fInvW;

		return vC;
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixMulScalar(const cMatrixf &a_mtxA, float afB)
	{
		cMatrixf mtxC;

		mtxC.m[0][0] = a_mtxA.m[0][0]*afB; mtxC.m[0][1] = a_mtxA.m[0][1]*afB; mtxC.m[0][2] = a_mtxA.m[0][2]*afB; mtxC.m[0][3] = a_mtxA.m[0][3]*afB;
		mtxC.m[1][0] = a_mtxA.m[1][0]*afB; mtxC.m[1][1] = a_mtxA.m[1][1]*afB; mtxC.m[1][2] = a_mtxA.m[1][2]*afB; mtxC.m[1][3] = a_mtxA.m[1][3]*afB;
		mtxC.m[2][0] = a_mtxA.m[2][0]*afB; mtxC.m[2][1] = a_mtxA.m[2][1]*afB; mtxC.m[2][2] = a_mtxA.m[2][2]*afB; mtxC.m[2][3] = a_mtxA.m[2][3]*afB;
		mtxC.m[3][0] = a_mtxA.m[3][0]*afB; mtxC.m[3][1] = a_mtxA.m[3][1]*afB; mtxC.m[3][2] = a_mtxA.m[3][2]*afB; mtxC.m[3][3] = a_mtxA.m[3][3]*afB;

		return mtxC;
	}

	
	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixRotate(cVector3f avRot, eEulerRotationOrder aOrder)
	{
		cMatrixf mtxRot = cMatrixf::Identity;
		switch(aOrder)
		{
		case eEulerRotationOrder_XYZ:	mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										break;
		case eEulerRotationOrder_XZY:	mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										break;
		case eEulerRotationOrder_YXZ:	mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										break;
		case eEulerRotationOrder_YZX:	mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										break;
		case eEulerRotationOrder_ZXY:	mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										break;
		case eEulerRotationOrder_ZYX:	mtxRot = MatrixMul(MatrixRotateZ(avRot.z), mtxRot);
										mtxRot = MatrixMul(MatrixRotateY(avRot.y), mtxRot);
										mtxRot = MatrixMul(MatrixRotateX(avRot.x), mtxRot);
										break;
		}

		return mtxRot;
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixRotateX(float afAngle)
	{
		return cMatrixf(1,0,0,0,
						0, cos(afAngle),-sin(afAngle),0,
						0,sin(afAngle),cos(afAngle),0,
						0,0,0,1);
	}
	
	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixRotateY(float afAngle)
	{
		return cMatrixf(cos(afAngle),0,sin(afAngle),0,
						0,1,0,0,
						-sin(afAngle),0, cos(afAngle),0,
						0,0,0,1);
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixRotateZ(float afAngle)
	{
		return cMatrixf(cos(afAngle),-sin(afAngle),0,0,
						sin(afAngle), cos(afAngle),0,0,
						0,0,1,0,
						0,0,0,1);
	}
	
	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixQuaternion(const cQuaternion &aqRot)
	{
		cMatrixf mtxOut;
		//Set the non rotation part.
		mtxOut.m[0][3] = 0;
		mtxOut.m[1][3] = 0;
		mtxOut.m[2][3] = 0;
		mtxOut.m[3][0] = 0;
		mtxOut.m[3][1] = 0;
		mtxOut.m[3][2] = 0;
		mtxOut.m[3][3] = 1;

		aqRot.ToRotationMatrix(mtxOut);

		return mtxOut;
	}

	cQuaternion cMath::MatrixToQuaternion(const cMatrixf &a_mtxA)
	{
		cQuaternion qOut(a_mtxA);
		return qOut;
		
	}
	
	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixScale(cVector3f avScale)
	{
		return cMatrixf(avScale.x, 0,0,0,
						0, avScale.y,0,0,
						0, 0,avScale.z,0,
						0, 0,0,1);
	}

	//-----------------------------------------------------------------------
	
	cMatrixf cMath::MatrixTranslate(cVector3f avTrans)
	{
		return cMatrixf(1, 0,0,avTrans.x,
						0, 1,0,avTrans.y,
						0, 0,1,avTrans.z,
						0, 0,0,1);

	}
	//-----------------------------------------------------------------------
	
	float cMath::MatrixMinor(const cMatrixf &a_mtxA,
		const size_t r0, const size_t r1, const size_t r2, 
		const size_t c0, const size_t c1, const size_t c2)
	{
		return a_mtxA.m[r0][c0] * (a_mtxA.m[r1][c1] * a_mtxA.m[r2][c2] - a_mtxA.m[r2][c1] * a_mtxA.m[r1][c2]) -
				a_mtxA.m[r0][c1] * (a_mtxA.m[r1][c0] * a_mtxA.m[r2][c2] - a_mtxA.m[r2][c0] * a_mtxA.m[r1][c2]) +
				a_mtxA.m[r0][c2] * (a_mtxA.m[r1][c0] * a_mtxA.m[r2][c1] - a_mtxA.m[r2][c0] * a_mtxA.m[r1][c1]);
	}
	
	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixAdjoint(const cMatrixf &a_mtxA)
	{
		return cMatrixf( MatrixMinor(a_mtxA, 1, 2, 3, 1, 2, 3),
						-MatrixMinor(a_mtxA, 0, 2, 3, 1, 2, 3),
						MatrixMinor(a_mtxA, 0, 1, 3, 1, 2, 3),
						-MatrixMinor(a_mtxA, 0, 1, 2, 1, 2, 3),

						-MatrixMinor(a_mtxA, 1, 2, 3, 0, 2, 3),
						MatrixMinor(a_mtxA, 0, 2, 3, 0, 2, 3),
						-MatrixMinor(a_mtxA, 0, 1, 3, 0, 2, 3),
						MatrixMinor(a_mtxA, 0, 1, 2, 0, 2, 3),

						MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 3),
						-MatrixMinor(a_mtxA, 0, 2, 3, 0, 1, 3),
						MatrixMinor(a_mtxA, 0, 1, 3, 0, 1, 3),
						-MatrixMinor(a_mtxA, 0, 1, 2, 0, 1, 3),

						-MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 2),
						MatrixMinor(a_mtxA, 0, 2, 3, 0, 1, 2),
						-MatrixMinor(a_mtxA, 0, 1, 3, 0, 1, 2),
						MatrixMinor(a_mtxA, 0, 1, 2, 0, 1, 2));
	}
	
	//-----------------------------------------------------------------------

	float cMath::MatrixDeterminant(const cMatrixf &a_mtxA)
	{
		return	a_mtxA.m[0][0] * MatrixMinor(a_mtxA, 1, 2, 3, 1, 2, 3) -
				a_mtxA.m[0][1] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 2, 3) +
				a_mtxA.m[0][2] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 3) -
				a_mtxA.m[0][3] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 2);
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixInverse(const cMatrixf &a_mtxA)
	{
		return MatrixMulScalar(MatrixAdjoint(a_mtxA), (1.0f / MatrixDeterminant(a_mtxA)) );
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixPlaneMirror(const cPlanef &aPlane)
	{
		return	cMatrixf(	1-2*aPlane.a*aPlane.a,	-2*aPlane.b*aPlane.a,	-2*aPlane.c*aPlane.a,	-2*aPlane.a*aPlane.d,
							-2*aPlane.a*aPlane.b,	1-2*aPlane.b*aPlane.b,	-2*aPlane.c*aPlane.b,	-2*aPlane.b*aPlane.d,
							-2*aPlane.a*aPlane.c,	-2*aPlane.b*aPlane.c,	1-2*aPlane.c*aPlane.c,	-2*aPlane.c*aPlane.d,
							0,						0,						0,						1);
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::MatrixToEulerAngles(const cMatrixf &a_mtxA, eEulerRotationOrder aOrder)
	{
		cVector3f vAngles;

		/*if (a_mtxA.m[2][0] > 0.998f) // singularity at north pole
		{ 
			vAngles.y = atan2(a_mtxA.m[0][2],a_mtxA.m[2][2]);
			vAngles.z = kPif/2;
			vAngles.x = 0;
			Log("Special case 1\n");
		}
		if (a_mtxA.m[2][0] < -0.998f) // singularity at south pole
		{ 
			vAngles.y = atan2(a_mtxA.m[0][2],a_mtxA.m[2][2]);
			vAngles.z = -kPif/2;
			vAngles.x = 0;
			Log("Special case 2\n");
		}
		else*/
		{
			vAngles.x = atan2(a_mtxA.m[2][1],a_mtxA.m[2][2]);
			vAngles.y = -asin(Clamp(a_mtxA.m[2][0], -1, 1) );
			vAngles.z = atan2(a_mtxA.m[1][0],a_mtxA.m[0][0]);
		}
		return vAngles;
	}

	//-----------------------------------------------------------------------

	cVector3f cMath::MatrixEulerAngleDistance(const cMatrixf &a_mtxA, const cMatrixf &a_mtxB)
	{
		//Transform so check is done local to matrix B.
		cMatrixf mtxBTrans = a_mtxB.GetTranspose();
		
		cMatrixf mtxLocalA = MatrixMul(mtxBTrans, a_mtxA);
		cMatrixf mtxLocalATrans = mtxLocalA.GetTranspose();

		cVector3f vFwd = mtxLocalATrans.GetForward();
		cVector3f vUp = mtxLocalATrans.GetUp();

		cVector3f vOutput(0);

		//Log("Global A: Fwd: (%s) Up: (%s)\n", a_mtxA.GetTranspose().GetForward().ToString().c_str(), a_mtxA.GetTranspose().GetUp().ToString().c_str());
		//Log("Global B: Fwd: (%s) Up: (%s)\n", a_mtxB.GetTranspose().GetForward().ToString().c_str(), a_mtxB.GetTranspose().GetUp().ToString().c_str());


		//Log("Local: Fwd: (%s) vs (0,0,1)     Up: (%s) vs (0,1,0)\n", vFwd.ToString().c_str(), vUp.ToString().c_str());

		////////////////
		//X Rotation (align fwd vector with z-axis)
        if(fabs(vFwd.y)<0.0001f && fabs(vFwd.z)<0.0001f) 
		{
			vOutput.x = 0;
		}
		else
		{
			//Only consider in yz plane
			cVector2f vYZ(vFwd.y, vFwd.z);
			vYZ.Normalize();

			vOutput.x = acos(Clamp(vYZ.y,-1,1));
			if(vYZ.x<0) vOutput.x = -vOutput.x;
		}

		////////////////
		// Rotate forward
		cMatrixf mtxRot = MatrixRotateX(vOutput.x);
		vFwd = MatrixMul3x3(mtxRot, vFwd);
		
		//Log("Rotated Fwd: (%s)\n", vFwd.ToString().c_str());

		////////////////
		//Y Rotation (align fwd vector with z-axis)
		if(fabs(vFwd.x)<0.0001f && fabs(vFwd.z)<0.0001f)
		{
			vOutput.y = 0;
		}
		else
		{
			//Only consider in xz plane
			cVector2f vXZ(vFwd.x, vFwd.z);
			vXZ.Normalize();

			vOutput.y = acos(Clamp(vXZ.y,-1,1));
			if(vXZ.x>0) vOutput.y = -vOutput.y;
		}
		
		////////////////
		// Rotate up
		mtxRot = MatrixMul(MatrixRotateY(vOutput.y), mtxRot);
		vUp = MatrixMul3x3(mtxRot, vUp);

		//Log("Rotated Up: (%s)\n", vUp.ToString().c_str());


		////////////////
		//Z Rotation
		if(fabs(vUp.y)<0.0001f && fabs(vUp.x)<0.0001f)
		{
			vOutput.z = 0;
		}
		else
		{
			//Only consider in xy plane
			cVector2f vXY(vUp.x, vUp.y);
			vXY.Normalize();

			vOutput.z = acos(Clamp(vXY.y,-1,1));
			if(vXY.x<0) vOutput.z = -vOutput.z;
		}

		
		vOutput = MatrixMul3x3(a_mtxB, vOutput);
		return vOutput;
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixUnitVectors(const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avForward, const cVector3f& avTranslate)
	{
		return cMatrixf( avRight.x, avUp.x, avForward.x, avTranslate.x,
						 avRight.y, avUp.y, avForward.y, avTranslate.y,
						 avRight.z, avUp.z, avForward.z, avTranslate.z,
						 0		, 0		  , 0		   , 1);
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::ProjectionMatrixObliqueNearClipPlane(const cMatrixf &a_mtxProjMatrix, const cPlanef& aClipPlane)
	{
		cMatrixf mtxOutput = a_mtxProjMatrix;
		cPlanef pQ;

		pQ.a = (Sign(aClipPlane.a) + a_mtxProjMatrix.m[0][2]) / a_mtxProjMatrix.m[0][0];
		pQ.b = (Sign(aClipPlane.b) + a_mtxProjMatrix.m[1][2]) / a_mtxProjMatrix.m[1][1];
		pQ.c = -1.0f;
		pQ.d = (1.0f + a_mtxProjMatrix.m[2][2]) / a_mtxProjMatrix.m[2][3];

		cPlanef pC = aClipPlane * (2.0f / PlaneDot(aClipPlane, pQ));

		mtxOutput.m[2][0] = pC.a;
		mtxOutput.m[2][1] = pC.b;
		mtxOutput.m[2][2] = pC.c + 1.0f;
		mtxOutput.m[2][3] = pC.d;

		return mtxOutput;
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixPerspectiveProjection(float afNearClipPlane, float afFarClipPlane, float afFOV, float afAspect, bool abInfFarPlane)
	{
		float fFar = afFarClipPlane;
		float fNear = afNearClipPlane;
		float fTop = tan(afFOV*0.5f) * fNear;
		float fBottom = -fTop;
		float fRight = afAspect * fTop;
		float fLeft = afAspect * fBottom;

		float fA = (2.0f*fNear) / (fRight - fLeft);
		float fB = (2.0f*fNear) / (fTop - fBottom);
		float fD = -1.0f;
		
		float fC,fZ;
		if(abInfFarPlane)
		{
			fC= -2.0f * fNear;
			fZ= -1.0f;
		}
		else {
			fC = -(2.0f*fFar*fNear) / (fFar - fNear);
			fZ = -(fFar + fNear)/(fFar - fNear);
		}

		return cMatrixf(
			fA,0, 0, 0,
			0,fB, 0, 0,
			0, 0,fZ,fC,
			0, 0,fD,0);
	}

	//-----------------------------------------------------------------------

	cMatrixf cMath::MatrixOrthographicProjection(float afNearClipPlane, float afFarClipPlane, const cVector2f& avViewSize)
	{
		float fFar = afFarClipPlane;
		float fNear = afNearClipPlane;

		float fA = 2.0f / avViewSize.x;
		float fB = 2.0f / avViewSize.y;
		float fC = -2.0f / (fFar - fNear);

		float fZ = -(fFar + fNear)/(fFar - fNear);

		return cMatrixf(
			fA,0, 0, 0,
			0,fB, 0, 0,
			0, 0,fC, fZ,
			0, 0, 0, 1);
	}

	//-----------------------------------------------------------------------

	const char* cMath::MatrixToChar(const cMatrixf &a_mtxA)
	{
		sprintf(mpTempChar,"[%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f]",
							a_mtxA.m[0][0],a_mtxA.m[0][1],a_mtxA.m[0][2],a_mtxA.m[0][3],
							a_mtxA.m[1][0],a_mtxA.m[1][1],a_mtxA.m[1][2],a_mtxA.m[1][3],
							a_mtxA.m[2][0],a_mtxA.m[2][1],a_mtxA.m[2][2],a_mtxA.m[2][3],
							a_mtxA.m[3][0],a_mtxA.m[3][1],a_mtxA.m[3][2],a_mtxA.m[3][3]);

		return mpTempChar;
	}	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// POLYGONS
	////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	static inline cVector3f GetVector3(const float* apVertexArray, int alIdx,int alStride)
	{
		const float* apVec = &apVertexArray[alIdx *alStride];

		return cVector3f(apVec[0],apVec[1],apVec[2]);
	}
	
	static inline void AddVector3(float* apArray, int alIdx,const cVector3f &avVec,int alStride)
	{
		float* apVec = &apArray[alIdx * alStride];
		apVec[0] += avVec.x;
		apVec[1] += avVec.y;
		apVec[2] += avVec.z;
	}

	static inline void SetVector4(const cVector3f &avVec, float afW,float* apArray, int alIdx)
	{
		float* apVec = &apArray[alIdx * 4];
		apVec[0] = avVec.x;
		apVec[1] = avVec.y;
		apVec[2] = avVec.z;
		apVec[3] = afW;
	}

	static inline bool Vector3Equal(const float* apArrayA, int alIdxA,const float* apArrayB, int alIdxB,
					int alStride)
	{	
		if(apArrayA[alIdxA*alStride + 0] == apArrayB[alIdxB*alStride + 0] &&
			apArrayA[alIdxA*alStride + 1] == apArrayB[alIdxB*alStride + 1] &&
			apArrayA[alIdxA*alStride + 2] == apArrayB[alIdxB*alStride + 2])
		{
			return true;
		}

		return false;
	}

	bool cMath::CreateTriTangentVectors(float* apDestArray, 
										const unsigned int* apIndexArray,int alIndexNum,
										const float* apVertexArray, int alVtxStride, 
										const float *apTexArray, 
										const float *apNormalArray,
										int alVertexNum)
	{
		//Create two temp arrays and clear them.
		tVector3fVec vTempTangents1;
		tVector3fVec vTempTangents2;

		//Log("Creating tangents:\n");
		//Log("Num of indices: %d\n",alIndexNum);
		//Log("Num of vertrices: %d\n",alVertexNum);
		
		vTempTangents1.resize(alVertexNum,cVector3f(0,0,0));
		vTempTangents2.resize(alVertexNum,cVector3f(0,0,0));
		
		//Iterate through the triangles
		for(int triIdx=0; triIdx<alIndexNum; triIdx+=3)
		{
			//Log("Triangle %d: ",triIdx/3);

			//Get the indices of the triangle
			int idx1 = apIndexArray[triIdx + 0];
			int idx2 = apIndexArray[triIdx + 1];
			int idx3 = apIndexArray[triIdx + 2];

			//Log("1: '%d' 2: '%d'  3: '%d' ", idx1, idx2, idx3);

            //Get the 3 points making up the triangle
			cVector3f vPos1 = GetVector3(apVertexArray,idx1,alVtxStride);
			cVector3f vPos2 = GetVector3(apVertexArray,idx2,alVtxStride);
			cVector3f vPos3 = GetVector3(apVertexArray,idx3,alVtxStride);
			
			//Get the 3 texture coords in the triangle.
			cVector3f vTex1 = GetVector3(apTexArray,idx1,3);
			cVector3f vTex2 = GetVector3(apTexArray,idx2,3);
			cVector3f vTex3 = GetVector3(apTexArray,idx3,3);
			
			//Get the vectors between the positions.
            cVector3f vPos1To2 = vPos2 - vPos1;
			cVector3f vPos1To3 = vPos3 - vPos1;

			//Get the vectors between the tex coords
			cVector3f vTex1To2 = vTex2 - vTex1;
			cVector3f vTex1To3 = vTex3 - vTex1;
			
			//Get the direction of the S and T tangents
			float fR = 1.0f / (vTex1To2.x * vTex1To3.y - vTex1To2.y * vTex1To3.x);

			cVector3f vSDir((vTex1To3.y * vPos1To2.x - vTex1To2.y * vPos1To3.x) * fR, 
							(vTex1To3.y * vPos1To2.y - vTex1To2.y * vPos1To3.y) * fR,
							(vTex1To3.y * vPos1To2.z - vTex1To2.y * vPos1To3.z) * fR
							); 

			cVector3f vTDir((vTex1To2.x * vPos1To3.x - vTex1To3.x * vPos1To2.x) * fR,
							(vTex1To2.x * vPos1To3.y - vTex1To3.x * vPos1To2.y) * fR,
							(vTex1To2.x * vPos1To3.z - vTex1To3.x * vPos1To2.z) * fR
							);

			//Add the temp arrays with the values:
			vTempTangents1[idx1] += vSDir;
			vTempTangents1[idx2] += vSDir;
			vTempTangents1[idx3] += vSDir;

			vTempTangents2[idx1] += vTDir;
			vTempTangents2[idx2] += vTDir;
			vTempTangents2[idx3] += vTDir;

			//Log("\n");
		}
		
		//Log("Looking for duplicates: \n");
		//Go through the vertrices and find normal and vertex copies. Smooth the tangents on these
		float fMaxCosAngle = -1.0f;
		for(int i=0; i < alVertexNum; i++)
		{
			for(int j=i+1; j< alVertexNum; j++)
			{
				//Log("(%.1f, %.1f, %.1f)", apVertexArray[i+0],apVertexArray[i+1],apVertexArray[i+2]);
				//Log(" vs ");
				//Log("(%.1f, %.1f, %.1f)\n", apVertexArray[j+0],apVertexArray[j+1],apVertexArray[j+2]);

				if(Vector3Equal(apVertexArray, i, apVertexArray, j,alVtxStride) && 
					Vector3Equal(apNormalArray, i, apNormalArray, j,3))
				{
					//Log("Found at %d and %d!\n", i, j);

					cVector3f vAT1 = vTempTangents1[i];
					cVector3f vAT2 = vTempTangents2[i];

					cVector3f vBT1 = vTempTangents1[j];
					cVector3f vBT2 = vTempTangents2[j];

					
					if(Vector3Dot(vAT1, vBT1)>= fMaxCosAngle)
					{
						vTempTangents1[j] += vAT1;
						vTempTangents1[i] += vBT1;
					}

					if(Vector3Dot(vAT2, vBT2)>= fMaxCosAngle)
					{
						vTempTangents2[j] += vAT2;
						vTempTangents2[i] += vBT2;
					}
				}
			}
		}

		//Iterate through the dest array and set tangent values
		for(int vtxIdx=0; vtxIdx < alVertexNum; vtxIdx++)
		{
			
			cVector3f vNormal = GetVector3(apNormalArray,vtxIdx,3);
			cVector3f &vTempTan1 = vTempTangents1[vtxIdx];
			cVector3f &vTempTan2 = vTempTangents2[vtxIdx];

            // Gram-Schmidt orthogonalize
			cVector3f vTan = vTempTan1 - (vNormal * cMath::Vector3Dot(vNormal, vTempTan1));
			vTan.Normalize();
			
			//Log("Add tangent %d: ",vtxIdx);
			//Log(" %.1f, %.1f, %.1f ",vTan.x, vTan.y, vTan.z);
			//Log("\n");
			
			//Calculate if left or right handed.
			float fW = (cMath::Vector3Dot(cMath::Vector3Cross(vNormal, vTempTan1), vTempTan2) < 0.0f) ? -1.0f : 1.0f;

			SetVector4(vTan, fW, apDestArray,vtxIdx);
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cMath::CreateTriangleData(tTriangleDataVec &avTriangles,
									const unsigned int* apIndexArray,int alIndexNum,
									const float* apVertexArray, int alVtxStride, int alVertexNum)
	{
		int lNumOfTri = alIndexNum / 3;
		if((int)avTriangles.size() < lNumOfTri) avTriangles.resize(lNumOfTri);
		
		//Log("Creating triangle data:\n");
		for(int tri=0, idx=0; tri < lNumOfTri; tri++,idx+=3)
		{
			//Log("checking: tri %d idx %d\n",tri, idx);
			//Calculate normal
			const float *pVtx0 = &apVertexArray[apIndexArray[idx]*alVtxStride];
			const float *pVtx1 = &apVertexArray[apIndexArray[idx+1]*alVtxStride];
			const float *pVtx2 = &apVertexArray[apIndexArray[idx+2]*alVtxStride];
			
			cVector3f vEdge1( pVtx1[0] - pVtx0[0], pVtx1[1] - pVtx0[1], pVtx1[2] - pVtx0[2]);
			cVector3f vEdge2( pVtx2[0] - pVtx0[0], pVtx2[1] - pVtx0[1], pVtx2[2] - pVtx0[2]);

			avTriangles[tri].normal = Vector3Cross(vEdge2, vEdge1);
		}
		
		return true;
	}

	
	//-----------------------------------------------------------------------

	static bool EdgePointEqual(const float* apVertexArray, 
								const cTriEdge &edge1,  const cTriEdge &edge2, int alStride)
	{
		if(Vector3Equal(apVertexArray, edge1.point1,apVertexArray, edge2.point1,alStride) &&
			Vector3Equal(apVertexArray, edge1.point2,apVertexArray, edge2.point2,alStride))
		{
			return true;
		}

		if(Vector3Equal(apVertexArray, edge1.point1,apVertexArray, edge2.point2,alStride) &&
			Vector3Equal(apVertexArray, edge1.point2,apVertexArray, edge2.point1,alStride))
		{
			return true;
		}

		return false;
	}
    
	/////////////////////////

	static bool EdgeTriEqual(const cTriEdge &edge1,  const cTriEdge &edge2)
	{
		if(edge1.tri1 == edge2.tri1 && edge1.tri2 == edge2.tri2) return true;
		if(edge1.tri1 == edge1.tri1 && edge1.tri2 == edge2.tri1) return true;
		return false;
	}

	/////////////////////////

	static bool EdgeEqual(const float* apVertexArray, const cTriEdge &edge1,  const cTriEdge &edge2, int alStride)
	{
		if(EdgePointEqual(apVertexArray,edge1, edge2,alStride) && EdgeTriEqual(edge1, edge2)) return true;

		return false;
	}

	/////////////////////////
	
	static const float *gpVertexArray;
	static const unsigned int *gpIndexArray;
	static int glVertexStride;


	//////////////////////////////////////////////////////

	class cVertexIndices
	{
	public:
		cVertexIndices(unsigned int alIdx){
			mlstIndices.push_back(alIdx);
		}
		tUIntList mlstIndices;	
	};

	typedef std::map<cVector3f,cVertexIndices> tVtxIdxMap;
	typedef tVtxIdxMap::iterator tVtxIdxMapIt;
	
	//////////////////////////////////////////////////////

	class cEdgeCompare
	{
	public:
		//The point1 must be > than point2 for this to work!
		bool operator()(const cTriEdge &Edge1, const cTriEdge &Edge2)const
		{
			cVector3f vPoint1_1 = GetVector3(gpVertexArray,Edge1.point1,glVertexStride);
			cVector3f vPoint1_2 = GetVector3(gpVertexArray,Edge1.point2,glVertexStride);
			cVector3f vPoint2_1 = GetVector3(gpVertexArray,Edge2.point1,glVertexStride);
			cVector3f vPoint2_2 = GetVector3(gpVertexArray,Edge2.point2,glVertexStride);
			
			//1 - 1
			if(vPoint1_1.x != vPoint2_1.x) return vPoint1_1.x > vPoint2_1.x;
			if(vPoint1_1.y != vPoint2_1.y) return vPoint1_1.y > vPoint2_1.y;
			if(vPoint1_1.z != vPoint2_1.z) return vPoint1_1.z > vPoint2_1.z;
			//2 - 2
			if(vPoint1_2.x != vPoint2_2.x) return vPoint1_2.x > vPoint2_2.x;
			if(vPoint1_2.y != vPoint2_2.y) return vPoint1_2.y > vPoint2_2.y;
			if(vPoint1_2.z != vPoint2_2.z) return vPoint1_2.z > vPoint2_2.z;

			return false;
		}
	};

	typedef std::set<cTriEdge, cEdgeCompare> tTriEdgeListMap;
	typedef tTriEdgeListMap::iterator tTriEdgeListMapIt;

	//////////////////////////////////////////////////////
	
	static void CheckEdgeSwitch(cTriEdge *apEdge)
	{
		cVector3f vPoint1 = GetVector3(gpVertexArray,apEdge->point1,glVertexStride);
		cVector3f vPoint2 = GetVector3(gpVertexArray,apEdge->point2,glVertexStride);
		if(vPoint1 < vPoint2){
			unsigned int lTemp = apEdge->point1;
			apEdge->point1 = apEdge->point2;
			apEdge->point2 = lTemp;
		}
	}

	//////////////////////////////////////////////////////

	void AddEdgeToMap(cTriEdge &aEdge, tTriEdgeListMap &aMap)
	{
		tTriEdgeListMapIt it = aMap.find(aEdge);
		//if edge is not added, create a new element.
		if(it == aMap.end())
		{
			aEdge.tri2 = -1;
			aMap.insert(aEdge);
		}
		//if added check if there already exist an edge with the triangles
		else
		{	
			if(it->tri2 != -1) return;
			if(it->tri1 == aEdge.tri1) return;

			//Set the other triangle! voila our edge is done!
			it->tri2 = aEdge.tri1;
		}
	}

	//////////////////////////////////////////////////////

	bool cMath::CreateEdges(tTriEdgeVec &avEdges,
							const unsigned int* apIndexArray,int alIndexNum,
							const float* apVertexArray, int alVtxStride,int alVertexNum,
							bool *apIsDoubleSided)
	{
		const bool bLog= false;

		//Initial setup
		*apIsDoubleSided = false;

		tVtxIdxMap mapVtxIndices;
		tTriEdgeListMap mapTriEdgeLists;

		gpIndexArray = apIndexArray;
		gpVertexArray = apVertexArray;
		glVertexStride = alVtxStride;

		////////////////////////////////////////////////
		//Iterate indices and add all that reference the same vertex to
		// the same element in a map
		for(int idx=0; idx < alIndexNum; idx++)
		{
			cVector3f vVtx = GetVector3(apVertexArray,apIndexArray[idx],alVtxStride);
			if(bLog)Log("Checking idx: %d with vec: %s, ",idx,vVtx.ToString().c_str());

			tVtxIdxMapIt it = mapVtxIndices.find(vVtx);
			//If vertex already exist just add the index
			if(it != mapVtxIndices.end())
			{
				if(bLog)Log("Allready added, appending.!\n");
				it->second.mlstIndices.push_back(idx);
			}
			//if vertex is not added create new vertex and add.
			else
			{
				if(bLog)Log("Adding as new!\n");
				mapVtxIndices.insert(tVtxIdxMap::value_type(vVtx, cVertexIndices(idx)));
			}
		}
		if(bLog){
			Log("FINAL VTX MAP:\n");
			tVtxIdxMapIt VtxIt = mapVtxIndices.begin();
			for(; VtxIt != mapVtxIndices.end(); ++VtxIt)
			{
				const cVector3f &vVtx = VtxIt->first;
				const cVertexIndices &Data = VtxIt->second;
				Log("Vtx: %s Idx: ",vVtx.ToString().c_str());
				for(tUIntList::const_iterator it = Data.mlstIndices.begin(); it != Data.mlstIndices.end(); ++it)
				{
					Log("%d, ",*it);
				}
				Log("\n");
			}
		}


		////////////////////////////////////////////////
		//Iterate vertices and check what indices belong to it.
		//create a edges from these.
		//TODO: iterate the amp here instead.
		tVtxIdxMapIt VtxIt = mapVtxIndices.begin();
		for(; VtxIt != mapVtxIndices.end(); ++VtxIt)
		{
			const cVector3f vVtx = VtxIt->first;
			cVertexIndices &Data = VtxIt->second;

            //Iterate the indices and create edges.
			tUIntListIt it = Data.mlstIndices.begin();
			for(; it != Data.mlstIndices.end(); ++it)
			{
				int lTriIdx = ((*it)/3)*3;
				unsigned int lVtx = apIndexArray[*it]; //The num of the vertex this index reference to.

				//Create edges
				cTriEdge edge1,edge2;
				edge1.point1 = lVtx; edge2.point1 = lVtx;
				edge1.tri1 = lTriIdx/3;	edge2.tri1 = lTriIdx/3;

				//Get the index the vertex has in the tri (0 -2)
				int lIdxInTri = (*it) % 3;
				//Log("Idx in tri: %d\n",lIdxInTri);

				//Get the end points of the edge.
				int lPoint1 = lIdxInTri+1;
				if(lPoint1>2) lPoint1 =0;
				int lPoint2 = lIdxInTri-1;
				if(lPoint2<0) lPoint2 =2;

				//Set the end points.
				edge1.point2 = apIndexArray[lTriIdx + lPoint1];
				edge2.point2 = apIndexArray[lTriIdx + lPoint2];

				//Switch points if they 1 < 2
				CheckEdgeSwitch(&edge1);
				CheckEdgeSwitch(&edge2);
				
				AddEdgeToMap(edge1,mapTriEdgeLists);
				AddEdgeToMap(edge2,mapTriEdgeLists);	
			}
		}
		if(bLog){
			Log("FINAL EDGE MAP:\n");
			tTriEdgeListMapIt EdgeIt = mapTriEdgeLists.begin();
			for(;EdgeIt != mapTriEdgeLists.end(); ++EdgeIt)
			{
				cTriEdge &Edge = const_cast<cTriEdge&>(*EdgeIt);

				Log("P1: %d P2: %d Tri1: %d Tri2: %d\n",Edge.point1,Edge.point2,Edge.tri1,Edge.tri2);
			}
		}

		////////////////////////////////////////////////
		//Iterate edge map and check triangles it has, create edges from this.
		avEdges.reserve(mapTriEdgeLists.size());
		tTriEdgeListMapIt EdgeIt = mapTriEdgeLists.begin();
		for(;EdgeIt != mapTriEdgeLists.end(); ++EdgeIt)
		{
			cTriEdge &Edge = const_cast<cTriEdge&>(*EdgeIt);
			const unsigned int *pTri1 = &apIndexArray[Edge.tri1 * 3];
			const unsigned int *pTri2 = NULL;
			if(Edge.tri2 >= 0) pTri2 = &apIndexArray[Edge.tri2 * 3];

			if(Edge.tri2 == -1){	
				Edge.invert_tri2 = true;
				*apIsDoubleSided = true;
			}
			else{
				Edge.invert_tri2 = false;
			}
			
			//Get position of point1 in triangle
			int lPoint1InTri=0;
			for(int i=0; i < 3; i++){
				if(Vector3Equal(apVertexArray,pTri1[i],apVertexArray, Edge.point1,alVtxStride))
				{
					lPoint1InTri = i;
					break;
				}
			}
			//The next position in the triangle.
			int lNextInTri = lPoint1InTri +1;
			if(lNextInTri >=3 ) lNextInTri =0;

			//Log("Point in: %d Next: %d\n",lPoint1InTri,lNextInTri);

			//If next point is NOT point 2, then the edge
			//must be switched.
			if(Vector3Equal(apVertexArray,pTri1[ lNextInTri],apVertexArray,Edge.point2,alVtxStride))
			{
				unsigned int lTemp = Edge.point1;
				Edge.point1 = Edge.point2;
				Edge.point2 = lTemp;
				//if(bLog)Log("Switching points\n");
			}
			
			//Add the final edge.
			avEdges.push_back(Edge);
		}
		
		return true;
	}

	/*bool cMath::CreateEdges(tTriEdgeVec &avEdges,
							const unsigned int* apIndexArray,int alIndexNum,
							const float* apVertexArray, int alVtxStride,int alVertexNum,
							bool *apIsDoubleSided)
	{
		const bool bLog= false;

		//Setup
		*apIsDoubleSided = false;

		////////////////////////////////////////////////////////////////////////
		//For each vertex check what triangles reference it and make edges to the
		//vertices in that triangle.
		for(int vtx=0; vtx < alVertexNum; vtx++)
		{
			if(bLog)Log("Checking vtx: %d\n",vtx);
			int lCount=0; //The number of times the vertex is referred to.
			tUIntList mlstIndices;
			for(int idx=0; idx < alIndexNum; idx++)
			{
				if(Vector3Equal(apVertexArray,vtx, apVertexArray, apIndexArray[idx],alVtxStride))
				{
					lCount++;
					mlstIndices.push_back(idx);
				}
			}
			if(lCount==0){
				Warning("Found unreferenced vertex when building edges!\n");
			}
			else 
			{
				///////////////////////////////////////////
				//Create the edges
				//Log("Vertex has %d references!\n",lCount);
				tUIntListIt it = mlstIndices.begin();
				for(; it != mlstIndices.end(); ++it)
				{
					//Get the triangle start index.
					int lTriIdx = ((*it)/3)*3;
					
					//if(bLog)Log("Tri index: %d!\n",lTriIdx);
					//Create edges
					cTriEdge edge1,edge2;
					edge1.point1 = vtx;
					edge2.point1 = vtx;
					edge1.tri1 = lTriIdx/3;
					edge2.tri1 = lTriIdx/3;
                    
					//Get the index the vertex has in the tri (0 -2)
					int lIdxInTri = (*it) % 3;
					//Log("Idx in tri: %d\n",lIdxInTri);
					
					//Get the end points of the edge.
					int lPoint1 = lIdxInTri+1;
					if(lPoint1>2) lPoint1 =0;
					int lPoint2 = lIdxInTri-1;
					if(lPoint2<0) lPoint2 =2;

					//if(bLog)Log("P1: %d P2: %d!\n",apIndexArray[lTriIdx +lPoint1], 
					//							apIndexArray[lTriIdx +lPoint2]);

					//Set the end points.
					edge1.point2 = apIndexArray[lTriIdx + lPoint1];
					edge2.point2 = apIndexArray[lTriIdx + lPoint2];

					avEdges.push_back(edge1);
					avEdges.push_back(edge2);
				}
			}
		}

		////////////////////////////////////////////////////////////////////////
		//Check for triangles that share the same edge. Pair these.
		if(bLog)Log("Looking for shared edges on triangles.\n");
		tTriEdgeVec vTempEdges;
		
		//Check for the last element as well incase it has no pair..
		for(int e1 =0; e1 < (int)avEdges.size(); e1++)
		{
			bool bFound = false;
			cTriEdge edge1 = avEdges[e1];

			if(bLog)Log("Checking p(%d)-p(%d)|(%d)\n",
				edge1.point1,edge1.point2,
				edge1.tri1);

			for(int e2 =e1+1; e2 < (int)avEdges.size(); e2++)
			{
				cTriEdge edge2 = avEdges[e2];

				//if(bLog)Log("Checking %d to %d\n",e1,e2);
				
				//Check if the edges has different triangles but the same points.
				if(edge1.tri1 != edge2.tri1 && EdgePointEqual(apVertexArray,edge1,edge2,alVtxStride))
				{
					bFound = true;

					if(bLog)Log("Found a pair: p(%d)-p(%d)|(%d)\n",edge2.point1,edge2.point2,
														edge2.tri1);
					
					edge1.tri2 = edge2.tri1;
					edge1.invert_tri2 = false;
					
					break;
				}
			}
				
			//If no edge was found it is at a hole.
			if(bFound == false)
			{
				edge1.tri2 = edge1.tri1;
				edge1.invert_tri2 = true;
				
				if(bLog) Log("No pair found, at a hole\n");
			}

			bFound = false;
			
			//SLOW AS HELL THIS IS...
			for(size_t i=0; i< vTempEdges.size(); i++)
			{
				if(EdgeEqual(apVertexArray,vTempEdges[i],edge1,alVtxStride)){
					bFound = true;
					break;
				}
				
				//if it is the last edge, don't add it if there is another edge with
				//the same points and different triangle sides.
				if(EdgePointEqual(apVertexArray,vTempEdges[i],edge1,alVtxStride) && 
					vTempEdges[i].tri1 != vTempEdges[i].tri2)
				{
					bFound = true;
					break;
				}
			}

			//If not already added, add
			if(bFound== false){
				
				//If a face with inverted tri was added, the mesh is double sided.
				if(edge1.invert_tri2){
					*apIsDoubleSided = true;
				}

				vTempEdges.push_back(edge1);
				if(bLog)Log("Added!\n");
			}
			if(bLog)Log("------------\n");
		}

		////////////////////////////////////////////////////////////////////////
		//Clear the old list and add the new nice pairs.
        avEdges.clear();
		avEdges.reserve(vTempEdges.size());
		
		//Log("FINAL EDGES:\n");
		//Create the final edges. This means making sure so that 
		//edge point 1 is before point 2 in triangle 1 and 2 point 2 before 
		//1 in triangle 2.
		for(size_t edge=0; edge< vTempEdges.size(); edge++)
		{
			cTriEdge Edge = vTempEdges[edge];
			const unsigned int *pTri1 = &apIndexArray[Edge.tri1 * 3];
			const unsigned int *pTri2 = &apIndexArray[Edge.tri2 * 3];

			if(bLog)Log("Edge %d:\n",edge);

			if(bLog)Log("Tri1: 0:(%.2f %.2f %.2f) 1:(%.2f %.2f %.2f) 2:(%.2f %.2f %.2f)\n",
						apVertexArray[pTri1[0]*alVtxStride+0],apVertexArray[pTri1[0]*alVtxStride+1],apVertexArray[pTri1[0]*alVtxStride+2],
						apVertexArray[pTri1[1]*alVtxStride+0],apVertexArray[pTri1[1]*alVtxStride+1],apVertexArray[pTri1[1]*alVtxStride+2],
						apVertexArray[pTri1[2]*alVtxStride+0],apVertexArray[pTri1[2]*alVtxStride+1],apVertexArray[pTri1[2]*alVtxStride+2]);
			
			if(bLog)Log("Tri2: 0:(%.2f %.2f %.2f) 1:(%.2f %.2f %.2f) 2:(%.2f %.2f %.2f)\n",
						apVertexArray[pTri2[0]*alVtxStride+0],apVertexArray[pTri2[0]*alVtxStride+1],apVertexArray[pTri2[0]*alVtxStride+2],
						apVertexArray[pTri2[1]*alVtxStride+0],apVertexArray[pTri2[1]*alVtxStride+1],apVertexArray[pTri2[1]*alVtxStride+2],
						apVertexArray[pTri2[2]*alVtxStride+0],apVertexArray[pTri2[2]*alVtxStride+1],apVertexArray[pTri2[2]*alVtxStride+2]);

			if(bLog)Log("Point1: (%.2f %.2f %.2f) Point2: (%.2f %.2f %.2f)\n",
						apVertexArray[Edge.point1*alVtxStride+0],apVertexArray[Edge.point1*alVtxStride+1],apVertexArray[Edge.point1*alVtxStride+2],
						apVertexArray[Edge.point2*alVtxStride+0],apVertexArray[Edge.point2*alVtxStride+1],apVertexArray[Edge.point2*alVtxStride+2]);
			
			//Get position of point1 in triangle
			int lPoint1InTri=0;
			for(int i=0; i < 3; i++){
				if(Vector3Equal(apVertexArray,pTri1[i],apVertexArray, Edge.point1,alVtxStride))
				{
					lPoint1InTri = i;
					break;
				}
			}
			//The next position in the triangle.
			int lNextInTri = lPoint1InTri +1;
			if(lNextInTri >=3 ) lNextInTri =0;

			if(bLog)Log("Point in: %d Next: %d\n",lPoint1InTri,lNextInTri);
			
			//If next point is NOT point 2, then the edge
			//must be switched.
            if(Vector3Equal(apVertexArray,pTri1[ lNextInTri],apVertexArray,Edge.point2,alVtxStride))
			{
				unsigned int lTemp = Edge.point1;
				Edge.point1 = Edge.point2;
				Edge.point2 = lTemp;
				if(bLog)Log("Switching points\n");
			}
			
			if(bLog)Log("%d: p(%d)-p(%d)|(%d)-(%d)\n",edge,
				Edge.point1,Edge.point2,
				Edge.tri1, Edge.tri2);

			//Log("-----------\n");
			
			avEdges.push_back(Edge);
		}

		return true;
	}*/


	

}
