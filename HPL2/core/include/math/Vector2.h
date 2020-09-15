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

#ifndef HPL_VECTOR2_H
#define HPL_VECTOR2_H

#include <math.h>
#include <stdio.h>
#include "system/SystemTypes.h"

namespace hpl {

	template <class T> 	class cVector2 {
		public:
			union{
				struct {
					T x,y;
				};
				T v[2];
			};

			//////////////////////////////////////////
			// Constructors
			/////////////////////////////////////////
			cVector2() : x(0),y(0)
			{}
			cVector2(T afVal) : x(afVal),y(afVal)
			{}
			cVector2(T afX, T afY) : x(afX),y(afY)
			{}
			cVector2(cVector2<T> const &avVec) : x(avVec.x), y(avVec.y)
			{}

			//////////////////////////////////////////
			// Copy
			/////////////////////////////////////////

			inline cVector2<T>& operator=(const cVector2<T> &aVec)
			{
				x = aVec.x;
				y = aVec.y;
				return *this;
			}

			inline cVector2<T>& operator=(const T aVal)
			{
				x = aVal;
				y = aVal;
				return *this;
			}
			
			//////////////////////////////////////////
			// Boolean
			/////////////////////////////////////////

			inline bool operator==(const cVector2<T> &aVec) const
			{
				if(x == aVec.x && y==aVec.y)return true;
				else return false;
			}

			inline bool operator!=(const cVector2<T> &aVec) const
			{
				if(x == aVec.x && y==aVec.y)return false;
				else return true;
			}
			
			//////////////////////////////////////////
			// Vector Arithmetic
			/////////////////////////////////////////

			inline cVector2<T> operator+(const cVector2<T> &aVec) const
			{
				cVector2<T> vec;
				vec.x = x + aVec.x;
				vec.y = y + aVec.y;
				return vec;
			}

			inline cVector2<T> operator-(const cVector2<T> &aVec) const
			{
				cVector2<T> vec;
				vec.x = x - aVec.x;
				vec.y = y - aVec.y;
				return vec;
			}

			inline cVector2<T> operator*(const cVector2<T> &aVec) const
			{
				cVector2<T> vec;
				vec.x = x * aVec.x;
				vec.y = y * aVec.y;
				return vec;
			}

			inline cVector2<T> operator/(const cVector2<T> &aVec) const
			{
				cVector2<T> vec;
				vec.x = x / aVec.x;
				vec.y = y / aVec.y;
				return vec;
			}

			inline cVector2<T>& operator-=(const cVector2<T>  &aVec)
			{
				x-=aVec.x;
				y-=aVec.y;
				return *this;
			}

			inline cVector2<T>& operator+=(const cVector2<T> &aVec)
			{
				x+=aVec.x;
				y+=aVec.y;
				return *this;
			}

			inline cVector2<T>& operator*=(const cVector2<T>  &aVec)
			{
				x*=aVec.x;
				y*=aVec.y;
				return *this;
			}

			inline cVector2<T>& operator/=(const cVector2<T> &aVec)
			{
				x/=aVec.x;
				y/=aVec.y;
				return *this;
			}

			//////////////////////////////////////////
			// Real Arithmetic
			/////////////////////////////////////////

			inline cVector2<T> operator/(const T &aVal) const
			{
				cVector2<T> vec;
				vec.x = x / aVal;
				vec.y = y / aVal;
				return vec;
			}

			inline cVector2<T> operator*(const T &aVal) const
			{
				cVector2<T> vec;
				vec.x = x * aVal;
				vec.y = y * aVal;
				return vec;
			}

			inline cVector2<T> operator+(const T &aVal) const
			{
				cVector2<T> vec;
				vec.x = x + aVal;
				vec.y = y + aVal;
				return vec;
			}

			cVector2<T> operator-(const T &aVal) const
			{
				cVector2<T> vec;
				vec.x = x - aVal;
				vec.y = y - aVal;
				return vec;
			}

			//////////////////////////////////////////
			// Methods 
			/////////////////////////////////////////
			
			inline void FromVec(const T *apVec)
			{
				x = apVec[0];
				y = apVec[1];
			}

			inline T Length() const
			{
				return sqrt( x * x + y * y);
			}

			inline T SqrLength() const
			{
				return x * x + y * y;
			}

			/**
			 * Only use this on double or float vectors
			 * \return Length of Vector
			 */
			T Normalize()
			{
				T length  = sqrt( x * x + y * y);

				if ( length > 1e-08 )
				{
					T InvLength = 1.0f / length;
					x *= InvLength;
					y *= InvLength;
				}

				return length;
			}

			//////////////////////////////////////////
			// Printing
			/////////////////////////////////////////

			tString ToString() const{
				char buf[512];
				sprintf(buf,"%f : %f",x,y);
				tString str = buf;
				return str;
			}

			tString ToFileString() const{
				char buf[512];
				sprintf(buf,"%g %g",x,y);
				tString str = buf;
				return str;
			}
	};
	
};
#endif // HPL_VECTOR2_H
