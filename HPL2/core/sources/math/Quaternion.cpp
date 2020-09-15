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

#include "math/Quaternion.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cQuaternion::cQuaternion()
	{
	}

	//-----------------------------------------------------------------------

	cQuaternion::cQuaternion(float afAngle, const cVector3f & avAxis)
	{
		FromAngleAxis(afAngle,avAxis);
	}

	//-----------------------------------------------------------------------

	cQuaternion::cQuaternion(float afW, float afX, float afY, float afZ)
	{
		w = afW;
		v.x = afX;
		v.y = afY;
		v.z = afZ;
	}

	//-----------------------------------------------------------------------
	cQuaternion::cQuaternion(const cMatrix<float> &a_mtxRot)
	{
		FromRotationMatrix(a_mtxRot);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	

	const cQuaternion cQuaternion::Identity = cQuaternion(1.0f,0.0f,0.0f,0.0f);

	//-----------------------------------------------------------------------
	
	void cQuaternion::Normalize()
	{
		float fLen = w*w + v.x*v.x + v.y*v.y + v.z*v.z;
		float fFactor = 1.0f / sqrt(fLen);
		v = v * fFactor;
		w = w * fFactor;
	}

	//-----------------------------------------------------------------------
	
	void cQuaternion::ToRotationMatrix(cMatrixf &a_mtxDest) const
	{
		cMatrixf mtxA;

		float fTx  = 2.0f*v.x;
		float fTy  = 2.0f*v.y;
		float fTz  = 2.0f*v.z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*v.x;
		float fTxy = fTy*v.x;
		float fTxz = fTz*v.x;
		float fTyy = fTy*v.y;
		float fTyz = fTz*v.y;
		float fTzz = fTz*v.z;

		a_mtxDest.m[0][0] = 1.0f-(fTyy+fTzz);
		a_mtxDest.m[0][1] = fTxy-fTwz;
		a_mtxDest.m[0][2] = fTxz+fTwy;
		a_mtxDest.m[1][0] = fTxy+fTwz;
		a_mtxDest.m[1][1] = 1.0f-(fTxx+fTzz);
		a_mtxDest.m[1][2] = fTyz-fTwx;
		a_mtxDest.m[2][0] = fTxz-fTwy;
		a_mtxDest.m[2][1] = fTyz+fTwx;
		a_mtxDest.m[2][2] = 1.0f-(fTxx+fTyy);
	}


	//-----------------------------------------------------------------------

	void cQuaternion::FromRotationMatrix(const cMatrix<float> &a_mtxRot)
	{
		float fTrace = a_mtxRot.m[0][0]+a_mtxRot.m[1][1]+a_mtxRot.m[2][2];
		float fRoot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrt(fTrace + 1.0f);  // 2w
			w = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;  // 1/(4w)
			v.x = (a_mtxRot.m[2][1]-a_mtxRot.m[1][2])*fRoot;
			v.y = (a_mtxRot.m[0][2]-a_mtxRot.m[2][0])*fRoot;
			v.z = (a_mtxRot.m[1][0]-a_mtxRot.m[0][1])*fRoot;
		}
		else
		{
			// |w| <= 1/2
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if ( a_mtxRot.m[1][1] > a_mtxRot.m[0][0] )
				i = 1;
			if ( a_mtxRot.m[2][2] > a_mtxRot.m[i][i] )
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = sqrt(a_mtxRot.m[i][i]-a_mtxRot.m[j][j]-a_mtxRot.m[k][k] + 1.0f);
			float* apkQuat[3] = { &v.x, &v.y, &v.z };
			*apkQuat[i] = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;
			w = (a_mtxRot.m[k][j]-a_mtxRot.m[j][k])*fRoot;
			*apkQuat[j] = (a_mtxRot.m[j][i]+a_mtxRot.m[i][j])*fRoot;
			*apkQuat[k] = (a_mtxRot.m[k][i]+a_mtxRot.m[i][k])*fRoot;
		}
	}
	
	//-----------------------------------------------------------------------

	void cQuaternion::FromAngleAxis(float afAngle, const cVector3f &avAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		float fHalfAngle = 0.5f*afAngle;
		float fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		v.x = fSin*avAxis.x;
		v.y = fSin*avAxis.y;
		v.z = fSin*avAxis.z;
	}

	//-----------------------------------------------------------------------

	cQuaternion cQuaternion::operator+(const cQuaternion &aqB) const
	{
		cQuaternion qOut;
		qOut.v = v + aqB.v;
		qOut.w = w + aqB.w;

		return qOut;
	}
	//-----------------------------------------------------------------------
	
	cQuaternion cQuaternion::operator-(const cQuaternion &aqB) const
	{
		cQuaternion qOut;
		qOut.v = v - aqB.v;
		qOut.w = w - aqB.w;

		return qOut;
	}
	//-----------------------------------------------------------------------
	cQuaternion cQuaternion::operator*(const cQuaternion &aqB) const
	{
		cQuaternion qOut;
		
		qOut.w = w * aqB.w - v.x * aqB.v.x - v.y * aqB.v.y - v.z * aqB.v.z;
		qOut.v.x = w * aqB.v.x + v.x * aqB.w + v.y * aqB.v.z - v.z * aqB.v.y;
		qOut.v.y = w * aqB.v.y + v.y * aqB.w + v.z * aqB.v.x - v.x * aqB.v.z;
		qOut.v.z = w * aqB.v.z + v.z * aqB.w + v.x * aqB.v.y - v.y * aqB.v.x;
		
		return qOut;
	}

	cQuaternion cQuaternion::operator/(const cQuaternion &aqB) const
	{
		cQuaternion qOut;
		qOut.v = v / aqB.v;
		qOut.w = w / aqB.w;

		return qOut;
	}
	//-----------------------------------------------------------------------
	cQuaternion cQuaternion::operator*(float afScalar) const
	{
		cQuaternion qOut;
		qOut.v = v * afScalar;
		qOut.w = w * afScalar;

		return qOut;
	}

	//-----------------------------------------------------------------------
}
