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

#ifndef HPL_PHYSICS_JOINT_BALL_NEWTON_H
#define HPL_PHYSICS_JOINT_BALL_NEWTON_H

#include "physics/PhysicsJointBall.h"
#include "impl/PhysicsJointNewton.h"

namespace hpl {

	class cPhysicsJointBallNewton : public iPhysicsJointNewton<iPhysicsJointBall>
	{
	public:
		cPhysicsJointBallNewton(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
			iPhysicsWorld *apWorld, const cVector3f &avPivotPoint, const cVector3f &avPinDir);
		~cPhysicsJointBallNewton();

        void SetConeLimits(float afMaxConeAngle, float afMaxTwistAngle);		
		cVector3f GetAngles();

		cVector3f GetVelocity();
		cVector3f GetAngularVelocity();
		float GetForceSize();

		float GetDistance();
		float GetAngle();
		
	private:
	};
};
#endif // HPL_PHYSICS_JOINT_BALL_NEWTON_H
