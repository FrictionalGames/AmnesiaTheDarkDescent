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

#include "impl/PhysicsJointBallNewton.h"

#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsWorldNewton.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPhysicsJointBallNewton::cPhysicsJointBallNewton(const tString &asName, 
						iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
						iPhysicsWorld *apWorld, const cVector3f &avPivotPoint, const cVector3f &avPinDir)
	: iPhysicsJointNewton<iPhysicsJointBall>(asName,apParentBody,apChildBody,apWorld,avPivotPoint,avPinDir)
	{
		mpNewtonJoint = NewtonConstraintCreateBall(mpNewtonWorld,avPivotPoint.v,
												mpNewtonChildBody, mpNewtonParentBody);

		mfMaxConeAngle =0;
		mfMaxTwistAngle = 0;
	}

	//-----------------------------------------------------------------------

	cPhysicsJointBallNewton::~cPhysicsJointBallNewton()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
		
	void cPhysicsJointBallNewton::SetConeLimits(float afMaxConeAngle, float afMaxTwistAngle)
	{
		NewtonBallSetConeLimits(mpNewtonJoint, mvStartPinDir.v,afMaxConeAngle,afMaxTwistAngle);
		mfMaxConeAngle = afMaxConeAngle;
		mfMaxTwistAngle = afMaxTwistAngle;
	}
	
	cVector3f cPhysicsJointBallNewton::GetAngles()
	{
		cVector3f vAngles;
		NewtonBallGetJointAngle(mpNewtonJoint,&vAngles.v[0]);
		return vAngles;
	}

	//-----------------------------------------------------------------------
	
	cVector3f cPhysicsJointBallNewton::GetVelocity()
	{
		return cVector3f(0,0,0);
	}
	cVector3f cPhysicsJointBallNewton::GetAngularVelocity()
	{
		cVector3f vVel;
		NewtonBallGetJointOmega(mpNewtonJoint,&vVel.v[0]);
		return vVel;
	}
	float cPhysicsJointBallNewton::GetForceSize()
	{
		cVector3f vForce;
		NewtonBallGetJointForce(mpNewtonJoint,&vForce.v[0]);
		return vForce.Length();
	}

	//-----------------------------------------------------------------------
	
	float cPhysicsJointBallNewton::GetDistance()
	{
		return 0;
	}
	float cPhysicsJointBallNewton::GetAngle()
	{
		return 0;
	}

	//-----------------------------------------------------------------------

}
