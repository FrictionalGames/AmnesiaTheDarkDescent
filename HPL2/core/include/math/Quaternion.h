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

#ifndef HPL_QUATERNION_H
#define HPL_QUATERNION_H

#include "math/Vector3.h"
#include "math/Matrix.h"

namespace hpl {

	class cQuaternion
	{
	public:
		cVector3<float> v;
		float w;
		
		cQuaternion();
		cQuaternion(float afAngle, const cVector3<float> &avAxis);
		cQuaternion(float afW, float afX, float afY, float afZ);
		cQuaternion(const cMatrix<float> &a_mtxRot);

		void Normalize();
		void ToRotationMatrix(cMatrix<float> &a_mtxDest) const;
		void FromRotationMatrix(const cMatrix<float> &a_mtxRot);

		/**
		 * Set the quaternion from and angle and axis.
		 * \param afAngle 
		 * \param &avAxis MUST be unit length!
		 */
		void FromAngleAxis(float afAngle, const cVector3<float> &avAxis);


		cQuaternion operator+(const cQuaternion &aqB) const;
		cQuaternion operator-(const cQuaternion &aqB) const;
		cQuaternion operator*(const cQuaternion &aqB) const;
		cQuaternion operator/(const cQuaternion &aqB) const;
		cQuaternion operator*(float afScalar) const;

		static const cQuaternion Identity;

	private:
	};
};
#endif // HPL_QUATERNION_H
