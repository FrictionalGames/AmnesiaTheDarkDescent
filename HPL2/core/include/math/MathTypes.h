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

#ifndef HPL_MATHTYPES_H
#define HPL_MATHTYPES_H

#pragma warning( disable : 4996 ) //disable unsafe warning!

#include <list>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Matrix.h"
#include "system/SystemTypes.h"

namespace hpl {

	#define kPif (3.141592654f)
	#define kPi2f (1.570796327f)
	#define kPi4f (0.7853981634f)
	#define k2Pif (6.283185307f)

	#define kEpsilonf (0.0001f)

	#define kSqrt2f (1.414213562f)

	//------------------------------------------

	enum eEulerRotationOrder
	{
		eEulerRotationOrder_XYZ,
		eEulerRotationOrder_XZY,
		eEulerRotationOrder_YXZ,
		eEulerRotationOrder_YZX,
		eEulerRotationOrder_ZXY,
		eEulerRotationOrder_ZYX,
		eEulerRotationOrder_LastEnum,
	};

	//-------------------------------------------

	enum eProjectionType
	{
		eProjectionType_Perspective,
		eProjectionType_Orthographic,
	};

	//-----------------------------------------------
	enum eFrustumPlane
	{
		eFrustumPlane_Left,
		eFrustumPlane_Right,
		eFrustumPlane_Bottom,
		eFrustumPlane_Top,
		eFrustumPlane_Near,
		eFrustumPlane_Far,
		eFrustumPlane_LastEnum,
	};

	//-----------------------------------------------

	enum eCollision
	{
		eCollision_Inside,
		eCollision_Outside,
		eCollision_Intersect,
		eCollision_LastEnum
	};

	//-------------------------------------------

	class cLinearOscillation
	{
	public:
		float max,min,val, up_speed, down_speed;

		cLinearOscillation(){}
		cLinearOscillation(float afMin, float afMax, float afVal, float afUpSpeed,float afDownSpeed)
		{
			SetUp(afMin,afMax,afVal, afUpSpeed, afDownSpeed);
		}

		void SetUp(float afMin, float afMax, float afVal, float afUpSpeed,float afDownSpeed)
		{
			min = afMin;
			max = afMax;
			val = afVal;

			up_speed = std::abs(afUpSpeed);
			down_speed = -std::abs(afDownSpeed);

			add = up_speed;
		}

		void Update(float afTimeStep)
		{
			val += add * afTimeStep;
			if(add > 0)
			{
				if(val >= max){
					val = max;
					add = down_speed;
				}
			}
			else
			{
				if(val <= min){
					val = min;
					add = up_speed;
				}
			}
		}

	private:
		float add;
	};

	//-------------------------------------------

	template <class T> class cSmoothInterpolation {
	public:
		cSmoothInterpolation()
		{
			mlMaxValues = 1;
		}

		void SetMaxValues(size_t alMax)
		{
			mlMaxValues = alMax;
		}

		void Add(const T& aNewVal)
		{
			mlstValues.push_back(aNewVal);
			if(mlstValues.size() > mlMaxValues) mlstValues.pop_front();
		}

		void Clear()
		{
			mlstValues.clear();
		}

		T GetOutput()
		{
			T totalValue(0);
			for(typename std::list<T>::iterator it = mlstValues.begin(); it != mlstValues.end(); ++it)
			{
				totalValue += *it;
			}
			return totalValue / (float)mlstValues.size();
		}

	private:
		size_t mlMaxValues;
		std::list<T> mlstValues;

	};

	//-------------------------------------------

	template <class T> class cPlane {
	public:
		T a,b,c,d;
		
		/////////////////

		cPlane(T aA, T aB, T aC, T aD){
			a = aA; b = aB; c = aC; d = aD;
		}

		cPlane(){
			a = 0; b = 0; c = 0; d = 0;
		}

		/////////////////

		inline bool operator==(const cPlane<T> &aPlane) const
		{
			return (a == aPlane.a && b == aPlane.b &&  c == aPlane.c &&  d == aPlane.d);
		}

		inline bool operator!=(const cPlane<T> &aPlane) const
		{
			return !(a == aPlane.a && b == aPlane.b &&  c == aPlane.c &&  d == aPlane.d);
		}


		/////////////////

		inline cPlane<T> operator*(const T &aVal) const
		{
			return cPlane<T>(a*aVal, b*aVal,c*aVal, d*aVal);
		}
			
		inline cPlane<T>& operator*=(const T &aVal) const
		{
			a *= aVal;
			b *= aVal;
			c *= aVal;
			d *= aVal;
			return *this;
		}

		/////////////////

		cPlane(const cVector3<T> &avNormal, const cVector3<T> &avPoint)
		{
			FromNormalPoint(avNormal, avPoint);
		}

		/////////////////

		cPlane(const cVector3<T> &avPoint0,const cVector3<T> &avPoint1,
				const cVector3<T> &avPoint2)
		{
			FromPoints(avPoint0,avPoint1,avPoint2);
		}

		/////////////////

		inline void FromNormalPoint(const cVector3<T> &avNormal, const cVector3<T> &avPoint)
		{
			a = avNormal.x;
			b = avNormal.y;
			c = avNormal.z;

			//Dot product
			d = -(a*avPoint.x + b*avPoint.y + c*avPoint.z);
		}

		/////////////////

		inline void FromPoints(const cVector3<T> &avPoint0,const cVector3<T> &avPoint1,
								const cVector3<T> &avPoint2)
		{
			cVector3<T> vEdge1 = avPoint1 - avPoint0;
			cVector3<T> vEdge2 = avPoint2 - avPoint0;

			//Cross product
			a = vEdge1.y * vEdge2.z - vEdge1.z * vEdge2.y;
			b = vEdge1.z * vEdge2.x - vEdge1.x * vEdge2.z;
			c = vEdge1.x * vEdge2.y - vEdge1.y * vEdge2.x;

			//Dot product
			d = -(a*avPoint0.x + b*avPoint0.y + c*avPoint0.z);

			Normalize();
		}

		/////////////////

		inline void Normalize()
		{
			T fMag = sqrt(a*a + b*b + c*c);
			a = a / fMag;
			b = b / fMag;
			c = c / fMag;
			d = d / fMag;
		}

		/////////////////

		inline cVector3<T> GetNormal() const
		{
			return cVector3<T>(a,b,c);
		}

		/////////////////

		void FromVec(T *apV)
		{
			a = apV[0];
			b = apV[1];
			c = apV[2];
			d = apV[3];
		}
	};

	typedef cPlane<float> cPlanef;
	typedef std::vector<cPlanef> tPlanefVec;
	typedef tPlanefVec tPlanefVecIt;

	//-------------------------------------------

	template <class T> class cSphere {
	public:
		T r;
		cVector3<T> center;

		cSphere() {
		}

		cSphere(cVector3<T> aV, T aR) {
			center = aV; r = aR;
		}
	};

	typedef cSphere<float> cSpheref;

	//-------------------------------------------

	template <class T> class cRect2 {
	public:
		T x,y,w,h;
		cRect2(T aX, T aY, T aW, T aH){
			x = aX; y = aY; w = aW; h = aH;
		}
		cRect2(T aX, T aY){
			x = aX; y = aY; w = 0; h = 0;
		}
		cRect2(){
			x = 0; y = 0; w = 0; h = 0;
		}
		cRect2(cVector2<T> aPos, cVector2<T> aSize){
			x = aPos.x; y = aPos.y; w = aSize.x; h = aSize.y;
		}

		void FromVec(T *apV)
		{
			x = apV[0];
			y = apV[1];
			w = apV[2];
			h = apV[3];
		}
	};

	typedef cRect2<float> cRect2f;
	typedef cRect2<int> cRect2l;
	typedef std::list<cRect2f> tRect2lList;
	typedef tRect2lList tRect2lListIt;

	//-------------------------------------------
	
	typedef union tIntOrFloatUnion
	{
		int	i;
		float f;

		tIntOrFloatUnion(){}
		tIntOrFloatUnion(int lVal) : i(lVal){}
		tIntOrFloatUnion(float fVal) : f(fVal){}
	} tIntOrFloat;

	//-------------------------------------------

	typedef cVector2<float> cVector2f;
	typedef cVector2<int> cVector2l;

	typedef cMatrix<float> cMatrixf;

	typedef std::vector<cMatrixf> tMatrixfVec;
	typedef tMatrixfVec::iterator tMatrixfVecIt;

	typedef std::list<cMatrixf> tMatrixfList;
	typedef tMatrixfList::iterator tMatrixfListIt;

	typedef std::list<cVector2f> tVector2fList;
	typedef tVector2fList::iterator tVector2fListIt;

	typedef cVector3<float> cVector3f;
	typedef cVector3<int> cVector3l;

	typedef std::list<cVector3f> tVector3fList;
	typedef tVector3fList::iterator tVector3fListIt;

	typedef std::vector<cVector2f> tVector2fVec;
	typedef tVector2fVec::iterator tVector2fVecIt;

	typedef std::vector<cVector3f> tVector3fVec;
	typedef tVector3fList::iterator tVector3fVecIt;

	//-------------------------------------------
}

#include "math/Quaternion.h"


#endif // HPL_MATHTYPES_H
