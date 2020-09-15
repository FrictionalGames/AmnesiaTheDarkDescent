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

#ifndef HPL_MATH_H
#define HPL_MATH_H

#include "math/MathTypes.h"
#include "math/BoundingVolume.h"
#include "math/MeshTypes.h"

#include "graphics/Color.h"

namespace hpl {

	//---------------------------------------------

	class cFrustum;
	class iVertexBuffer;

	//---------------------------------------------
	
	
	class cMath
	{
	public:
		 //////////////////////////////////////////////////////
		////////// RANDOM GENERATION ////////////////////////
		//////////////////////////////////////////////////////
		/**
         * Generates a random integer from min to max
         * \param alMin 
         * \param alMax 
         * \return 
         */
        static int RandRectl(int alMin, int alMax);
		
		/**
		 * Generates a random float from min to max
		 * \param alMin 
		 * \param alMax 
		 * \return 
		 */
		static float RandRectf(float alMin, float alMax);

		/**
		* Generates a random float from min to max
		*/
		static cVector2f RandRectVector2f(const cVector2f &avMin,const cVector2f &avMax);

		/**
		* Generates a random float from min to max
		*/
		static cVector3f RandRectVector3f(const cVector3f &avMin,const cVector3f &avMax);

		/**
		* Generates a random float from min to max
		*/
		static cColor RandRectColor(const cColor &aMin,const cColor &aMax);

		
		/**
		 * Randomize the rand funcs.
		 * \param alSeed the seed, -1 = random seed.
		 */
		static void Randomize(int alSeed = -1);

		//////////////////////////////////////////////////////
		////////// BIT OPERATIONS ////////////////////////
		//////////////////////////////////////////////////////
		
		static int GetFlagBit(int alBitNum);


		/////////////////////////////////////////////////////
		////////// BV AND AABB OPERTIONS //////////////////
		//////////////////////////////////////////////////////

		static void ExpandAABB(cVector3f& avBaseMin,cVector3f& avBaseMax, const cVector3f& avAddMin, const cVector3f& avAddMax);
		
		//////////////////////////////////////////////////////
		////////// BV AND RECT INTERSECTION //////////////////
		//////////////////////////////////////////////////////

		static bool CheckRectIntersection(cRect2l aRect1, cRect2l aRect2);
		static bool CheckRectIntersection(cRect2f aRect1, cRect2f aRect2);

		static bool CheckPointInRectIntersection(const cVector2f& avPoint, const cRect2f& aRect);
		
		static bool CheckRectFit(const cRect2l& aRectSrc, const cRect2l& aRectDest);
		static bool CheckRectFit(const cRect2f& aRectSrc, const cRect2f& aRectDest);
		
		static cRect2f GetClipRect(const cRect2f& aRectSrc,const cRect2f& aRectDest);

        static inline bool CheckPointInSphereIntersection(const cVector3f& avPoint, const cVector3f& avSpherePos, float afSphereRadius)
		{
			cVector3f vSepAxis = avPoint - avSpherePos;
			return vSepAxis.SqrLength() <= afSphereRadius * afSphereRadius;
		}

		static inline bool CheckSphereIntersection(const cVector3f& avPosA, float afRadiusA,const cVector3f& avPosB, float afRadiusB)
		{
			float fRadiusSum = afRadiusA + afRadiusB;
			cVector3f vSepAxis = avPosA - avPosB;
			return vSepAxis.SqrLength() <= fRadiusSum * fRadiusSum;
		}

		
		/**
		* Checks intersection between sphere and line
		* \return true if intersection, else false
		*/
		static bool CheckSphereLineIntersection(const cVector3f& avSpherePos, float afSphereRadius, const cVector3f& avLineStart, const cVector3f& avLineEnd, float *apT1, cVector3f* apIntersection1, float *apT2, cVector3f* apIntersection2);
		
		/**
		* Checks intersection between two bounding volumes.
		* \return true if intersection, else false.
		*/
        static bool CheckBVIntersection(cBoundingVolume& aBV1,cBoundingVolume& aBV2);
		
		/**
		* Checks intersection between two AABB:s (defined by min and max vectors)
		* \return true if intersection, else false.
		*/
		static bool CheckAABBIntersection(	const cVector3f& avMin1,const cVector3f& avMax1,
											const cVector3f& avMin2,const cVector3f& avMax2);


		/**
		 * Check if an AABB is completely inside another.
		 */
		static bool CheckAABBInside(	const cVector3f& avInsideMin,const cVector3f& avInsideMax,
										const cVector3f& avOutsideMin,const cVector3f& avOutsideMax);

		static bool CheckPointInBVIntersection(const cVector3f& avPoint, cBoundingVolume& aBV);
		
		static bool CheckPointInAABBIntersection(const cVector3f& avPoint, const cVector3f& avMin,const cVector3f& avMax);


		/**
		* Creates a clip rect for a bounding volume that comes in -1 - 1 for points in screen. It can return points above and below 1/-1 though. Returns false if all points are behind frustum,
		*/
		static bool GetNormalizedClipRectFromBV(	cVector3f &avDestMin, cVector3f &avDestMax,
													cBoundingVolume &aBV, cFrustum *apFrustum,
													float afTanHalfFov);

		/**
		 * Creates a clip rect in pixel coords from normalized (-1 - 1) coords.
		 */
		static cRect2l GetClipRectFromNormalizedMinMax(const cVector3f &avMin, const cVector3f& avMax, const cVector2l &avScreenSize);
		
		/**
		* Creates a clip rect for a bounding volume (that contains world coords) in screen space.
		*/
		static void GetClipRectFromBV(	cRect2l &aDestRect, cBoundingVolume &aBV,
										cFrustum *apFrustum, const cVector2l &avScreenSize,
										float afTanHalfFov);
		
		/**
		 * Creates a clip rect for a sphere. Can handle even if part of sphere is behind near clip.
		 */
		static cRect2l GetClipRectFromSphere(	const cVector3f& avPosition, float afRadius,
												cFrustum *apFrustum, const cVector2l &avScreenSize,
												bool abPositionIsScreenSpace, float afTanHalfFov);

		static bool CheckSphereInPlanes(const cVector3f &avCenter, float afRadius,
										const cPlanef* apPlanes, int alPlaneCount);

		
		/**
		* Checks if AABB intersects with line. Returns false if there was no intersection, else true.
		*/
		static bool CheckAABBLineIntersection(	const cVector3f& avMin,const cVector3f& avMax, 
												const cVector3f &avLineStart, const cVector3f &avLineEnd,
												cVector3f *apIntersectionPos, float *apT);

		
		/**
		 * Checks if a point intersects with a on-screen projected mesh. Returns false if there was no intersection, else true
		 * \param avPoint : Screen coordinates of the point
		 * \param avViewportSize : Screen size
		 * \param alNumIndices : Num of triangles
		 * \param avIndices : Pointer to index array
		 * \param avVertexPos : Pointer to vertex position array
		 * \param alStride 
		 * \param amtxObjWorldMatrix 
		 * \param amtxCamViewMatrix 
		 * \param amtxCamProjMatrix 
		 * \param *apIntersectedTriIndex : Destination int pointer
		 * \return 
		 */
		static bool CheckPointInFlatPolygon( const cVector3f& avRayStart, const cVector3f& avRayEnd,
											 const cVector2f& avPoint, const cVector2f& avViewportSize, 
											 int alNumIndices, unsigned int* avIndices, float* avVertexPos, int alStride, 
											 const cMatrixf& amtxObjWorldMatrix, 
											 const cMatrixf& amtxCamViewMatrix, const cMatrixf& amtxCamProjMatrix, 
											 unsigned int* apIntersectedTriIndex, cVector3f* apIntersectionWorldSpace, tVector3fVec* apTriangleWorldSpace );

		/**
		 * Check if a point is inside a 2D triangle. Must be in normalized coordinates.
		 * \param avPoint 
		 * \param avTriangle 
		 * \return 
		 */
		static bool CheckPointInsideTriangle2D(const cVector2f& avPoint, const tVector3fVec& avTriangle);


		/**
		 * 
		 * \param avPoint1 
		 * \param avPoint2 
		 * \param avPoint3 
		 * \param avPoint4 
		 * \return 
		 */
		static bool CheckPointSameSideOfEdge2D(const cVector3f& avPoint1, const cVector3f& avPoint2, const cVector3f& avPoint3, const cVector3f& avPoint4);

		//////////////////////////////////////////////////////
		////////// FLOAT OPERATIONS ////////////////////////
		//////////////////////////////////////////////////////
		
		//-------------------------------------

		/**
		* A fast conversion of positive float to int
		*/
		static inline int FastPositiveFloatToInt(float afVal)
		{
			const tIntOrFloat positive_bias( (23 + 127) << 23 );

			tIntOrFloat num(afVal);

			num.f += positive_bias.f;
			num.i -= positive_bias.i;

			return num.i;
		}
		
		/**
		* A fast conversion of negative float to int
		*/
		static inline int FastNegativeFloatToInt(float afVal)
		{
			const tIntOrFloat negative_bias( ((23 + 127) << 23) + (1 << 22) );

			tIntOrFloat num(afVal);

			num.f += negative_bias.f;
			num.i -= negative_bias.i;

			return num.i;
		}

		/**
		 * Checks if float is negative or positive and uses algorithm accordingly
		 */
		static inline int FastPosAndNegFloatToInt(float afVal)
		{
			if(afVal>=0)	return FastPositiveFloatToInt(afVal);
			else			return FastNegativeFloatToInt(afVal);
		}

		/**
		 * Get rounded int from a float
		 * \param afVal 
		 * \return 
		 */
		static int RoundToInt(float afVal)
		{
			return (int)floor(afVal + 0.5);
		}

		static cVector2l RoundToInt(const cVector2f& avX)
		{
			return cVector2l(RoundToInt(avX.x), RoundToInt(avX.y));
		}

		static cVector3l RoundToInt(const cVector3f& avX)
		{
			return cVector3l(RoundToInt(avX.x), RoundToInt(avX.y), RoundToInt(avX.z));
		}

		/**
		 * Round float to alPrecision decimals
		 * \param afVal
		 * \param alPrecision
		 * \return 
		 */
		static float RoundFloatToDecimals(float afVal, int alPrecision);

		/**
		 * Get fraction part of a float
		 * \param afVal 
		 * \return 
		 */
		static float GetFraction(float afVal);
		/**
		 * Modulus (%) of a float
		 * \param afDividend 
		 * \param afDivisor 
		 * \return 
		 */
		static float Modulus(float afDividend, float afDivisor);

		static float ToRad(float afAngle);
		static float ToDeg(float afAngle);

		/**
		 * Get the Log 2 of an int.
		 */
		static int Log2ToInt(int alX);

		/**
		 * Get the Power of 2 of an int.
		 */
		static int Pow2(int alX);

		/**
		 * Checks if the number is a power of two.
		 */
		static bool IsPow2(int alX);

        /**
         * Wraps a value (afX) between min and max. Example: Wrap(-1, 0,10) returns 9.
		 * \param afMin The minimum value. Must be lower than max.
		 * \param afMax The maximum value. Must be higher than min and NOT 0.
         */
        static float Wrap(float afX, float afMin, float afMax);

		/**
		 * Clamps a value between min and max. Example Clamp(-1, 0,1) return 0.
		 */
		static float Clamp(float afX, float afMin, float afMax);

		inline static float Max(float afX,float afY){
			if(afX>afY) return afX;
			return afY;
		}
		inline static float Min(float afX,float afY){
			if(afX<afY) return afX;
			return afY;
		}

		inline static int Max(int alX,int alY){
			if(alX>alY) return alX;
			return alY;
		}
		inline static int Min(int alX,int alY){
			if(alX<alY) return alX;
			return alY;
		}

		inline static float Abs(float afX){ return fabs(afX); }
		inline static int Abs(int alX){ return abs(alX); }

		inline static float Sign(float afX) { return (float)((afX>0.0f) - (afX<0.0f)); }
		inline static int Sign(int alX) { return (alX>0) - (alX<0); }
		
		static float GetAngleDistance(float afAngle1, float afAngle2, float afMaxAngle);
		static float GetAngleDistanceRad(float afAngle1, float afAngle2);
		static float GetAngleDistanceDeg(float afAngle1, float afAngle2);

		static float TurnAngle(float afAngle,float afFinalAngle,float afSpeed,float afMaxAngle);
		static float TurnAngleRad(float afAngle,float afFinalAngle,float afSpeed);
		static float TurnAngleDeg(float afAngle,float afFinalAngle,float afSpeed);

		static float Interpolate(float afA,float afB,float afT);

		inline static float GetCorrectSignOfSpeed(float afCurrent, float afDest, float afSpeed)
		{
			if(afDest < afCurrent && afSpeed > 0) return -afSpeed;
			if(afDest > afCurrent && afSpeed < 0) return -afSpeed;
			return afSpeed;
		}

		static float IncreaseTo(float afX, float afAdd, float afDest);

		/**
		 * return x^2*(3-x*2), which is a curve that starts slow, speeds up and ends slow (if x is 0 -> 1)
		 */
		static float SmoothCurve(float afX);


		
		//////////////////////////////////////////////////////
		////////// VECTOR 2D ///////////////////////////////
		//////////////////////////////////////////////////////
		
		static float Vector2Dist(const cVector2f &avPosA,const cVector2f &avPosB);
		static float Vector2DistXY(const cVector3f &avPosA,const cVector3f &avPosB);
		static float Vector2DistXZ(const cVector3f &avPosA,const cVector3f &avPosB);

		static float Vector2DistSqr(const cVector2f &avPosA,const cVector2f &avPosB);
		static float Vector2DistSqrXY(const cVector3f &avPosA,const cVector3f &avPosB);
		static float Vector2DistSqrXZ(const cVector3f &avPosA,const cVector3f &avPosB);

		static cVector2f Vector2IncreaseTo(const cVector2f& avX, const cVector2f& avAdd, const cVector2f& avDest);

		/**
		 * Get the angle a vector at aStartPos has to have to look at aGoalPos
		 * \param &aStartPos 
		 * \param &avGoalPos 
		 * \return 
		 */
		static float GetAngleFromPoints2D(const cVector2f &aStartPos, const cVector2f &avGoalPos);
		/**
		 * Get a vector from an angle and a length
		 * \param afAngle 
		 * \param afLength 
		 * \return 
		 */
		static cVector2f GetVectorFromAngle2D(float afAngle, float afLength);
		/**
		 * Get angle and length of a vector
		 * \param &avVec 
		 * \param *apAngle 
		 * \param *apLength 
		 */
		static void GetAngleFromVector(const cVector2f &avVec, float *apAngle, float *apLength);
		
		/**
		 * Project Src on Dest
		 * \param &avSrcVec 
		 * \param &avDestVec 
		 * \return 
		 */
		static cVector2f ProjectVector2D(const cVector2f &avSrcVec, const cVector2f &avDestVec);

		static inline cVector2f Vector2Floor(const cVector2f &avVec)
		{
			return cVector2f(floor(avVec.x),floor(avVec.y));
		}

		static inline cVector2f Vector2Ceil(const cVector2f &avVec)
		{
			return cVector2f(ceil(avVec.x),ceil(avVec.y));
		}

		static inline cVector2f Vector2Abs(const cVector2f &avVec)
		{
			return cVector2f(Abs(avVec.x),Abs(avVec.y));
		}

		static inline cVector2f Vector2Min(const cVector2f &avVecA, const cVector2f &avVecB)
		{
			return cVector2f(Min(avVecA.x, avVecB.x),Min(avVecA.y, avVecB.y));
		}

		static inline cVector2f Vector2Max(const cVector2f &avVecA, const cVector2f &avVecB)
		{
			return cVector2f(Max(avVecA.x, avVecB.x),Max(avVecA.y, avVecB.y));
		}

		static inline bool Vector2Less(const cVector2f &avVec, const cVector2f &avCompareVec)
		{
			if(avVec.x >= avCompareVec.x) return false;
			if(avVec.y >= avCompareVec.y) return false;
			return true;
		}

		static inline bool Vector2Greater(const cVector2f &avVec, const cVector2f &avCompareVec)
		{
			if(avVec.x <= avCompareVec.x) return false;
			if(avVec.y <= avCompareVec.y) return false;
			return true;
		}

		static inline bool Vector2LessEqual(const cVector2f &avVec, const cVector2f &avCompareVec)
		{
			if(avVec.x > avCompareVec.x) return false;
			if(avVec.y > avCompareVec.y) return false;
			return true;
		}

		static inline bool Vector2GreaterEqual(const cVector2f &avVec, const cVector2f &avCompareVec)
		{
			if(avVec.x < avCompareVec.x) return false;
			if(avVec.y < avCompareVec.y) return false;
			return true;
		}

		static inline cVector2f Vector2Normalize(const cVector2f &avVec)
		{
			cVector2f vNorm = avVec;
			vNorm.Normalize();
			return vNorm;
		}

		static inline cVector2f Vector2ToRad(const cVector2f &avVec)
		{
			return cVector2f(ToRad(avVec.x),ToRad(avVec.y));
		}

		static inline cVector2f Vector2ToDeg(const cVector2f &avVec)
		{
			return cVector2f(ToDeg(avVec.x),ToDeg(avVec.y));
		}
		
		//////////////////////////////////////////////////////
		////////// VECTOR 3D ///////////////////////////////
		//////////////////////////////////////////////////////

		static cVector3f Vector3MaxLength(const cVector3f &avVec, float afMaxLength);
		static cVector3f Vector3MinLength(const cVector3f &avVec, float afMinLength);
		static cVector3f Vector3ClampLength(const cVector3f &avVec,float afMinLength, float afMaxLength);

		static inline cVector3f Vector3ToRad(const cVector3f &avVec)
		{
			return cVector3f(ToRad(avVec.x),ToRad(avVec.y),ToRad(avVec.z));
		}

		static inline cVector3f Vector3ToDeg(const cVector3f &avVec)
		{
			return cVector3f(ToDeg(avVec.x),ToDeg(avVec.y),ToDeg(avVec.z));
		}

		static cVector3f Vector3AngleDistance(const cVector3f &avAngles1, const cVector3f &avAngles2, float afMaxAngle);
		static cVector3f Vector3AngleDistanceRad(const cVector3f &avAngles1, const cVector3f &avAngles2);
		static cVector3f Vector3AngleDistanceDeg(const cVector3f &avAngles1, const cVector3f &avAngles2);


		static inline cVector3f Vector3Floor(const cVector3f &avVec)
		{
			return cVector3f(floor(avVec.x),floor(avVec.y),floor(avVec.z));
		}

		static inline cVector3f Vector3Ceil(const cVector3f &avVec)
		{
			return cVector3f(ceil(avVec.x),ceil(avVec.y),ceil(avVec.z));
		}

		static inline cVector3f Vector3Abs(const cVector3f &avVec)
		{
			return cVector3f(Abs(avVec.x),Abs(avVec.y),Abs(avVec.z));
		}

		static inline cVector3f Vector3Min(const cVector3f &avVecA, const cVector3f &avVecB)
		{
			return cVector3f(Min(avVecA.x, avVecB.x),Min(avVecA.y, avVecB.y),Min(avVecA.z, avVecB.z));
		}

		static inline cVector3f Vector3Max(const cVector3f &avVecA, const cVector3f &avVecB)
		{
			return cVector3f(Max(avVecA.x, avVecB.x),Max(avVecA.y, avVecB.y),Max(avVecA.z, avVecB.z));
		}

		static inline bool Vector3Less(const cVector3f &avVec, const cVector3f &avCompareVec)
		{
			if(avVec.x >= avCompareVec.x) return false;
			if(avVec.y >= avCompareVec.y) return false;
			if(avVec.z >= avCompareVec.z) return false;
			return true;
		}

		static inline bool Vector3Greater(const cVector3f &avVec, const cVector3f &avCompareVec)
		{
			if(avVec.x <= avCompareVec.x) return false;
			if(avVec.y <= avCompareVec.y) return false;
			if(avVec.z <= avCompareVec.z) return false;
			return true;
		}

		static inline bool Vector3LessEqual(const cVector3f &avVec, const cVector3f &avCompareVec)
		{
			if(avVec.x > avCompareVec.x) return false;
			if(avVec.y > avCompareVec.y) return false;
			if(avVec.z > avCompareVec.z) return false;
			return true;
		}

		static inline bool Vector3GreaterEqual(const cVector3f &avVec, const cVector3f &avCompareVec)
		{
			if(avVec.x < avCompareVec.x) return false;
			if(avVec.y < avCompareVec.y) return false;
			if(avVec.z < avCompareVec.z) return false;
			return true;
		}
		
		static inline cVector3f Vector3Normalize(const cVector3f &avVec)
		{
			cVector3f vNorm = avVec;
			vNorm.Normalize();
			return vNorm;
		}

		static inline float Vector3DistSqr(const cVector3f &avStartPos, const cVector3f &avEndPos)
		{
			float fDX = avEndPos.x - avStartPos.x;
			float fDY = avEndPos.y - avStartPos.y;
			float fDZ = avEndPos.z - avStartPos.z;

			return fDX*fDX + fDY*fDY + fDZ*fDZ;
		}

		static inline float Vector3Dist(const cVector3f &avStartPos, const cVector3f &avEndPos)
		{
			return sqrt(Vector3DistSqr(avStartPos, avEndPos));
		}


		static cVector3f GetAngleFromPoints3D(const cVector3f &avStartPos, const cVector3f &avGoalPos);

		/**
		 * Vector cross product, A x B = R
		 * \param avVecA 
		 * \param avVecB 
		 * \return 
		 */
		static cVector3f Vector3Cross(const cVector3f& avVecA,const cVector3f& avVecB); 

		/**
		 * Vector dot product, A * B = R
		 * \param avVecA 
		 * \param avVecB 
		 * \return 
		 */
		static float Vector3Dot(const cVector3f& avVecA,const cVector3f& avVecB);

		/**
		* Project Src on Dest
		* \param &avSrcVec
		* \param &avDestVec
		* \return 
		*/
		static cVector3f Vector3Project(const cVector3f &avSrcVec, const cVector3f &avDestVec);


		/**
		 * Reflects vector along normal 
		 * \param &avVec 
		 * \param &avNormal 
		 * \return 
		 */
		static cVector3f Vector3Reflect(const cVector3f &avVec, const cVector3f &avNormal);

		
		/**
		* Calculates the angle between two vectors.
		* \param avVecA 
		* \param avVecB 
		* \return 
		*/
		static float Vector3Angle(const cVector3f& avVecA,const cVector3f& avVecB);
		
		/**
		 * Unprojects a vector from screen size and coords.
		*/
		static cVector3f Vector3UnProject(const cVector3f& avVec, const cRect2f &aScreenRect, 
											cMatrixf a_mtxViewProj);

		static void Vector3ClampToLength(cVector3f& avVec, float afMaxLength);

		cVector3f Vector3IncreaseTo(const cVector3f& avX, const cVector3f& avAdd, const cVector3f& avDest);


		static void Vector3OrthonormalizeBasis(const cVector3f& avSrcRight, const cVector3f& avSrcUp, const cVector3f& avSrcForward,
											cVector3f& avDstRight, cVector3f& avDstUp, cVector3f& avDstForward);

		//////////////////////////////////////////////////////
		////////// SPHERICAL COORDS ////////////////////////
		//////////////////////////////////////////////////////

		/**
		* Gets the spherical coordinates (x:theta, y:phi) of a point in a sphere given by its center
		*/
		static cVector2f GetSphericalCoordsFromPoint3D(const cVector3f& avSphCenter, const cVector3f& avPoint);

		/**
		* Gets the world coordinates of a point in a sphere determined by its spherical coords and radius
		*/
		static cVector3f GetPoint3DFromSphericalCoords(const cVector3f& avSphCenter, float afSphRadius, const cVector2f& avSphCoords);

		//////////////////////////////////////////////////////
		////////// PLANES ////////////////////////
		//////////////////////////////////////////////////////

		/**
		 * Gets the dot production from two planes (just like vec4 dot)
		 * \param aPlaneA 
		 * \param aPlaneB 
		 * \return 
		 */
		static float PlaneDot(const cPlanef& aPlaneA, const cPlanef& aPlaneB);

		/**
		 * Checks if two planes are parellel
		 * \param aPlaneA 
		 * \param aPlaneB 
		 * \return 
		 */
		static bool PlaneParallel(const cPlanef& aPlaneA, const cPlanef& aPlaneB);

		/**
		 * Transforms a plane by some transformation matrix. Plane must be normalized!
		 * \param &a_mtxTransform 
		 * \param aPlane 
		 * \return 
		 */
		static cPlanef TransformPlane(const cMatrixf &a_mtxTransform, const cPlanef& aPlane);

		/**
		 * Calculates distance from a point to a plane
		 * \param aPlane The plane must be Normalized!
		 * \param avVec 
		 * \return >0 if in front of plane, 0 if on plane and <0 if behind plane
		 */
		static float PlaneToPointDist(const cPlanef& aPlane, const cVector3f& avVec);

		/**
		 * Get the line defining the intersection of 2 planes.
		 * \param aPA 
		 * \param aPB 
		 * \param avDir The direction of the line will be placed here 
		 * \param avPoint A point on the line will be placed here. 
		 */
		static void PlaneIntersectionLine(const cPlanef& aPA, const cPlanef& aPB, cVector3f& avDir, cVector3f& avPoint);

		/**
		 * Get a point where all planes intersect, if no such point exist false is returned else true.
		 * \param aP1 
		 * \param aP2 
		 * \param aP3 
		 * \param avPoint 
		 * \return 
		 */
		static bool PlaneIntersectionPoint(const cPlanef& aP1, const cPlanef& aP2, const cPlanef& aP3, cVector3f& avPoint);
		
		/**
		 * Check plane and sphere collision
		 */
		static eCollision CheckPlaneSphereCollision(const cPlanef& aPlane, const cVector3f &avCenter, float afRadius);

		/**
		* Longer argument version of CheckPlaneBVCollision where AABB and sphere (encapsulating AABB) is used for check.
		* \param aPlane 
		* \return 
		*/
		static eCollision CheckPlaneAABBCollision(	const cPlanef& aPlane, const cVector3f &avMin, const cVector3f &avMax, const cVector3f &avSphereCenter,
													float afSphereRadius);
		static eCollision CheckPlaneAABBCollision(	const cPlanef& aPlane, const cVector3f &avMin, const cVector3f &avMax);

		/**
		 * Does a collision with a BV and a plane
		 * \param aPlane 
		 * \param &aBV 
		 * \return 
		 */
		static eCollision CheckPlaneBVCollision(const cPlanef& aPlane, cBoundingVolume &aBV);

		/**
		 * Checks if line intersects with plane. Returns false if there was no intersection, else true.
		 */
		static bool CheckPlaneLineIntersection(	const cPlanef& aPlane, const cVector3f &avLineStart, const cVector3f &avLineEnd,
												cVector3f *apIntersectionPos, float *apT);

		/**
		 *  Checks if the points are all inside (inside), some inside (intersect) or all outside (outside) of the planes.
		 */
		static eCollision CheckPointsPlanesCollision(cVector3f *apVertices, int alNumOfVertices, cPlanef *apPlanes, int alNumOfPlanes);

		/**
		*  Checks if the points are all inside (inside), some inside (intersect) or all outside (outside) of the planes of an AABB.
		*/
		static eCollision CheckPointsAABBPlanesCollision(cVector3f *apVertices, int alNumOfVertices, const cVector3f& avMin, const cVector3f& avMax);

		/**
		*  Does a sepperating axis collision check! Planes do only need to contain normal!
		*/
		static bool CheckSeparatingAxisIntersection(cVector3f *apVerticesA, int alNumOfVerticesA, cPlanef *apPlanesA, int alNumOfPlanesA,
													cVector3f *apVerticesB, int alNumOfVerticesB, cPlanef *apPlanesB, int alNumOfPlanesB);


		//////////////////////////////////////////////////////
		////////// TRIANGLES ///////////////////////////////
		//////////////////////////////////////////////////////
		
        /**
         * Checks intersection between line and a triangle
         */
        static bool CheckLineTriangleIntersection(	const cVector3f& avLineStart, const cVector3f& avLineEnd, 
													const cVector3f& avP0, const cVector3f& avP1, const cVector3f& avP2,
													float *apT, bool abSkipBackfacing=true);
		
		/**
		* Checks intersection between line and a mesh. For speed reasons the matrix is INVERSE!
		*/
		static bool CheckLineTriMeshIntersection(	const cVector3f& avLineStart, const cVector3f& avLineEnd,
													const cMatrixf& a_mtxInvMeshMtx,
													const unsigned int* apIndexArray,int alIndexNum,
													const float* apVertexArray, int alVtxStride,
													cVector3f *apIntersectionPos, float *apT, int *apTriIndex, bool abSkipBackfacing=true);

		/**
		* Checks intersection between line and a mesh. For speed reasons the matrix is INVERSE!
		*/
		static bool CheckLineTriVertexBufferIntersection(	const cVector3f& avLineStart, const cVector3f& avLineEnd,
															const cMatrixf& a_mtxInvMeshMtx, iVertexBuffer *apVtxBuffer,
															cVector3f *apIntersectionPos, float *apT, int *apTriIndex, bool abSkipBackfacing=true);

		//////////////////////////////////////////////////////
		////////// QUATERNIONS ///////////////////////////////
		//////////////////////////////////////////////////////

		/**
		 * Spherical Linear Interpolation between quaternions A and B
		 * \param afT The amount inbetween the quaternions. 0.0 is A and 1 is B.
		 * \param abShortestPath Move the the shortest path.
		 * \return 
		 */
		static cQuaternion QuaternionSlerp(float afT,const cQuaternion& aqA, const cQuaternion& aqB, 
											bool abShortestPath);

		static float QuaternionDot(const cQuaternion& aqA,const cQuaternion& aqB);

		/**
		* Quaternion multiplication,  A * B = R. This means that B is applied BEFORE A.
		*/
		static cQuaternion QuaternionMul(const cQuaternion& aqA,const cQuaternion& aqB);

		
		//////////////////////////////////////////////////////
		////////// MATRICES ////////////////////////////////////
		//////////////////////////////////////////////////////
		
		/**
		* Spherical Linear Interpolation between matrix A and B
		* \param afT The amount inbetween the quaternions. 0.0 is A and 1 is B.
		* \param abShortestPath Move the the shortest path.
		* \return 
		*/
		static cMatrixf MatrixSlerp(float afT,const cMatrixf& a_mtxA, const cMatrixf& a_mtxB, 
											bool abShortestPath);

		/**
		 * Matrix multiplication,  A * B = R. This means that B is applied BEFORE A.
		 */
		static cMatrixf MatrixMul(const cMatrixf &a_mtxA,const cMatrixf &a_mtxB);
		/**
		 * Multiply and matrix and a 3d vector
		 */
		static cVector3f MatrixMul(const cMatrixf &a_mtxA,const cVector3f &avB);
		
		/**
		* Matrix multiplication only using 3x3 matrix (rotation)
		*/
		static cVector3f MatrixMul3x3(const cMatrixf &a_mtxA,const cVector3f &avB);
		/**
		* Multiply and matrix and a 3d vector and devide the result with W.
		*/
		static cVector3f MatrixMulDivideW(const cMatrixf &a_mtxA,const cVector3f &avB);

		/**
		 * Multiply matrix and a float. 
		 */
		static cMatrixf MatrixMulScalar(const cMatrixf &a_mtxA, float afB);
		
		
		/**
		 * Creates a rotation matrix along all axes according to order.
		 */
		static cMatrixf MatrixRotate(cVector3f avRot, eEulerRotationOrder aOrder);
		/**
		 * Create a rotation matrix around the X-axis according to the right hand rule
		 */
		static cMatrixf MatrixRotateX(float afAngle);
		/**
		* Create a rotation matrix around the Y-axis according to the right hand rule
		*/
		static cMatrixf MatrixRotateY(float afAngle);
		/**
		* Create a rotation matrix around the Z-axis according to the right hand rule
		*/
		static cMatrixf MatrixRotateZ(float afAngle);
		static cMatrixf MatrixScale(cVector3f avScale);
		static cMatrixf MatrixTranslate(cVector3f avTrans);
		
		/**
		 * Creates a matrix from a quaternion.
		 * \return 
		 */
		static cMatrixf MatrixQuaternion(const cQuaternion &aqRot);

		/**
		* Creates a quaternion from a matrix.
		* \return 
		*/
		static cQuaternion MatrixToQuaternion(const cMatrixf &a_mtxA);

		/**
		 * Get the minor of a matrix.
		 */
		static inline float MatrixMinor(const cMatrixf &a_mtxA,
			const size_t r0, const size_t r1, const size_t r2, 
			const size_t c0, const size_t c1, const size_t c2);
		/**
		* Get the adjoint of a matrix.
		*/
		static inline cMatrixf MatrixAdjoint(const cMatrixf &a_mtxA);
		
		/**
		* Get the determinant of a matrix.
		*/
		static inline float MatrixDeterminant(const cMatrixf &a_mtxA);
		
		/**
		 * Gets the inverse of a matrix.
		 */
		static cMatrixf MatrixInverse(const cMatrixf &a_mtxA);

		/**
		 * A mirror matrix for a plane
		 */
		static cMatrixf MatrixPlaneMirror(const cPlanef &aPlane);

		/**
		 * Converts the matrix into Euler angles, XYZ only supported at the moment.
		 * \param &a_mtxA 
		 * \param aOrder 
		 */
		static cVector3f MatrixToEulerAngles(const cMatrixf &a_mtxA, eEulerRotationOrder aOrder);

		/**
		 * Returns the euler angle (in xyz order) distance from A to B. Matrices may not have scale, only rotation.
		 */
		static cVector3f MatrixEulerAngleDistance(const cMatrixf &a_mtxA, const cMatrixf &a_mtxB);

		/**
		 * Creates a matrix from the given coordinate system
		 * \param &avRight
		 * \param &avUp
		 * \param &avForward
		 * \param &avTranslate
		 * \return 
		 */
		static cMatrixf MatrixUnitVectors(const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avForward, const cVector3f& avTranslate);

		/**
		 * Changes the near clip plane of the projection matrix (non-ortho) to a oblique one. Plane must be normalized and in camera space
		 * \param &a_mtxProjMatrix 
		 * \param aCameraSpaceClipPlane 
		 * \return 
		 */
		static cMatrixf ProjectionMatrixObliqueNearClipPlane(const cMatrixf &a_mtxProjMatrix, const cPlanef& aClipPlane);

		/**
		 * Creates a perspective projection matrix.
		 */
		static cMatrixf MatrixPerspectiveProjection(float afNearClipPlane, float afFarClipPlane, float afFOV, float afAspect, bool abInfFarPlane);
		/**
		 * Creates an orthographics projection matrix.
		 */
		static cMatrixf MatrixOrthographicProjection(float afNearClipPlane, float afFarClipPlane, const cVector2f& avViewSize);

		/**
		 * Create a char string from the matrix
		 */
		static const char* MatrixToChar(const cMatrixf &a_mtxA);
		
		//////////////////////////////////////////////////////
		////////// POLYGON MANAGEMENT ///////////////////////
		//////////////////////////////////////////////////////

		/**
		 * Creates an array with 4d tangent vectors for triangles. alIndexNum % 3 must be 0.
		 * \param apDestArray The destination array, must be 4 * number of vertices large
		 * \param apIndexArray The indices
		 * \param alIndexNum Number of indices
		 * \param apVertexArray Vertices indexed by the indices
		 * \param apTexArray The texture coords
		 * \param apNormalArray The normals
		 * \param  alVertexNum Number of vertex, normals and texcoords.
		 * \return true if success, else false
		 */
		static bool CreateTriTangentVectors(float* apDestArray, 
										const unsigned int* apIndexArray,int alIndexNum,
										const float* apVertexArray, int alVtxStride,
										const float *apTexArray,
										const float *apNormalArray,int alVertexNum);

		/**
		* Creates triangle data for a triangle mesh. alIndexNum % 3 must be 0.
		* \param avTriangles Where the data will be stored. If empty, this function resizes it.
		* \param apIndexArray The indices
		* \param alIndexNum Number of indices
		* \param apVertexArray Vertices indexed by the indices
		* \param  alVertexNum Number of vertices
		* \return true if success, else false
		*/
		static bool CreateTriangleData(tTriangleDataVec &avTriangles,
										const unsigned int* apIndexArray,int alIndexNum,
										const float* apVertexArray, int alVtxStride, int alVertexNum);

		/**
		* Creates edges for a triangle mesh. alIndexNum % 3 must be 0.
		* \param avEdges An empty edge vector, this is where the edges will be stored.
		* \param apIndexArray The indices
		* \param alIndexNum Number of indices
		* \param apVertexArray Vertices indexed by the indices
		* \param  alVertexNum Number of vertices
		* \return true if success, else false
		*/
		static bool CreateEdges(tTriEdgeVec &avEdges,
								const unsigned int* apIndexArray,int alIndexNum,
								const float* apVertexArray, int alVtxStride, int alVertexNum,
								bool *apIsDoubleSided);
};

};
#endif // HPL_MATH_H
