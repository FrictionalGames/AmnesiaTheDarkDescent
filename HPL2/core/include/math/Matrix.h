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

#ifndef HPL_MATRIX_H
#define HPL_MATRIX_H

#include <math.h>
#include "Vector3.h"

namespace hpl {

template <class T> 	class cMatrix {
	public:
	//The ways to reference the matrix
	//format is [row][col]
	union
	{
		T m[4][4];
		T v[16];
	};

	//////////////////////////////////////////
	// Constructors
	/////////////////////////////////////////

	inline cMatrix()
	{
	}

	//-----------------------------------------------------------------------

	inline cMatrix(T* pA)
	{
		FromVec(pA);
	}

	//-----------------------------------------------------------------------


	inline cMatrix(double* pA)
	{
		m[0][0] = (T)pA[0];
		m[0][1] = (T)pA[1];
		m[0][2] = (T)pA[2];
		m[0][3] = (T)pA[3];
		m[1][0] = (T)pA[4];
		m[1][1] = (T)pA[5];
		m[1][2] = (T)pA[6];
		m[1][3] = (T)pA[7];
		m[2][0] = (T)pA[8];
		m[2][1] = (T)pA[9];
		m[2][2] = (T)pA[10];
		m[2][3] = (T)pA[11];
		m[3][0] = (T)pA[12];
		m[3][1] = (T)pA[13];
		m[3][2] = (T)pA[14];
		m[3][3] = (T)pA[15];
	}

	//-----------------------------------------------------------------------


	inline cMatrix(
		T a00, T a01, T a02, T a03,
		T a10, T a11, T a12, T a13,
		T a20, T a21, T a22, T a23,
		T a30, T a31, T a32, T a33 )
	{
		m[0][0] = a00;
		m[0][1] = a01;
		m[0][2] = a02;
		m[0][3] = a03;
		m[1][0] = a10;
		m[1][1] = a11;
		m[1][2] = a12;
		m[1][3] = a13;
		m[2][0] = a20;
		m[2][1] = a21;
		m[2][2] = a22;
		m[2][3] = a23;
		m[3][0] = a30;
		m[3][1] = a31;
		m[3][2] = a32;
		m[3][3] = a33;
	}

	static const cMatrix<T> Identity;
	static const cMatrix<T> Zero;

	//-----------------------------------------------------------------------

	inline bool operator==(const cMatrix<T>& aMtx) const
	{
		if(	m[0][0] == aMtx.m[0][0] &&
			m[0][1] == aMtx.m[0][1] &&
			m[0][2] == aMtx.m[0][2] &&
			m[0][3] == aMtx.m[0][3] &&
			m[1][0] == aMtx.m[1][0] &&
			m[1][1] == aMtx.m[1][1] &&
			m[1][2] == aMtx.m[1][2] &&
			m[1][3] == aMtx.m[1][3] &&
			m[2][0] == aMtx.m[2][0] &&
			m[2][1] == aMtx.m[2][1] &&
			m[2][2] == aMtx.m[2][2] &&
			m[2][3] == aMtx.m[2][3] &&
			m[3][0] == aMtx.m[3][0] &&
			m[3][1] == aMtx.m[3][1] &&
			m[3][2] == aMtx.m[3][2] &&
			m[3][3] == aMtx.m[3][3])
		{
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	inline bool operator!=(const cMatrix<T>& aMtx)
	{
		return !(*this == aMtx);
	}

	//-----------------------------------------------------------------------

	inline void FromVec(const T* pA)
	{
		m[0][0] = pA[0];
		m[0][1] = pA[1];
		m[0][2] = pA[2];
		m[0][3] = pA[3];
		m[1][0] = pA[4];
		m[1][1] = pA[5];
		m[1][2] = pA[6];
		m[1][3] = pA[7];
		m[2][0] = pA[8];
		m[2][1] = pA[9];
		m[2][2] = pA[10];
		m[2][3] = pA[11];
		m[3][0] = pA[12];
		m[3][1] = pA[13];
		m[3][2] = pA[14];
		m[3][3] = pA[15];
	}

	//-----------------------------------------------------------------------

	inline void FromTranspose(const T* pA)
	{
		m[0][0] = pA[0];
		m[1][0] = pA[1];
		m[2][0] = pA[2];
		m[3][0] = pA[3];

		m[0][1] = pA[4];
		m[1][1] = pA[5];
		m[2][1] = pA[6];
		m[3][1] = pA[7];

		m[0][2] = pA[8];
		m[1][2] = pA[9];
		m[2][2]= pA[10];
		m[3][2] = pA[11];

		m[0][3] = pA[12];
		m[1][3] = pA[13];
		m[2][3] = pA[14];
		m[3][3] = pA[15];
	}


	//-----------------------------------------------------------------------

	inline cVector3<T> GetRight() const
	{
		/* @todo ??? */
		return cVector3<T>(m[0][0], m[0][1], m[0][2]);
	}

	inline void SetRight(const cVector3<T>&  avVec)
	{
		m[0][0] = avVec.x;
		m[0][1] = avVec.y;
		m[0][2] = avVec.z;
	}

	//-----------------------------------------------------------------------

	inline cVector3<T> GetUp() const
	{
		/* @todo ??? */
		return cVector3<T>(m[1][0], m[1][1], m[1][2]);
	}

	inline void SetUp(const cVector3<T>&  avVec)
	{
		m[1][0] = avVec.x;
		m[1][1] = avVec.y;
		m[1][2] = avVec.z;
	}

	//-----------------------------------------------------------------------

	inline cVector3<T> GetForward() const
	{
		/* @todo ??? */
		return cVector3<T>(m[2][0], m[2][1], m[2][2]);
	}

	inline void SetForward(const cVector3<T>&  avVec)
	{
		m[2][0] = avVec.x;
		m[2][1] = avVec.y;
		m[2][2] = avVec.z;
	}

	//-----------------------------------------------------------------------

	inline cVector3<T> GetTranslation() const
	{
		/* @todo ??? */
		return cVector3<T>(m[0][3], m[1][3], m[2][3]);
	}

	inline void SetTranslation(const cVector3<T>& avTrans)
	{
		m[0][3] = avTrans.x;
		m[1][3] = avTrans.y;
		m[2][3] = avTrans.z;
	}

	//-----------------------------------------------------------------------

	inline cMatrix<T> GetRotation() const
	{
		return cMatrix<T>(  m[0][0], m[0][1], m[0][2] ,0,
							m[1][0], m[1][1], m[1][2] ,0,
							m[2][0], m[2][1], m[2][2] ,0,
							0,		 0,		  0		  ,1);
	}


	//-----------------------------------------------------------------------

	inline cMatrix<T> GetTranspose() const
	{
		return cMatrix<T>(m[0][0], m[1][0], m[2][0] ,m[3][0],
						m[0][1], m[1][1], m[2][1] ,m[3][1],
						m[0][2], m[1][2], m[2][2] ,m[3][2],
						m[0][3], m[1][3], m[2][3] ,m[3][3]);
	}

	//-----------------------------------------------------------------------

	tString ToString() const
	{
		char buf[512];
		sprintf(buf,"[%f : %f : %f : %f] [%f : %f : %f : %f] [%f : %f : %f : %f] [%f : %f : %f : %f]",
					m[0][0],m[0][1],m[0][2],m[0][3],
					m[1][0],m[1][1],m[1][2],m[1][3],
					m[2][0],m[2][1],m[2][2],m[2][3],
					m[3][0],m[3][1],m[3][2],m[3][3]);
		return buf;
	}

	tString ToFileString() const
	{
		char buf[512];
		sprintf(buf,"%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
			m[0][0],m[0][1],m[0][2],m[0][3],
			m[1][0],m[1][1],m[1][2],m[1][3],
			m[2][0],m[2][1],m[2][2],m[2][3],
			m[3][0],m[3][1],m[3][2],m[3][3]);
		return buf;
	}
};

};

#endif // HPL_MATRIX_H
