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

#ifndef HPL_PHYSICS_BODY_NEWTON_H
#define HPL_PHYSICS_BODY_NEWTON_H

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif
#include <Newton.h>

#include "physics/PhysicsBody.h"

namespace hpl {

	class cPhysicsBodyNewtonCallback : public iEntityCallback
	{
		void OnTransformUpdate(iEntity3D * apEntity);
	};

	class cPhysicsBodyNewton : public iPhysicsBody
	{
	friend class cPhysicsBodyNewtonCallback;
	public:
		cPhysicsBodyNewton(const tString &asName,iPhysicsWorld *apWorld,iCollideShape *apShape);
		~cPhysicsBodyNewton();

		void SetMaterial(iPhysicsMaterial* apMaterial);

		void SetLinearVelocity(const cVector3f &avVel);
		cVector3f GetLinearVelocity() const;
		void SetAngularVelocity(const cVector3f &avVel);
		cVector3f GetAngularVelocity() const;
		void SetLinearDamping(float afDamping);
		float GetLinearDamping() const;
		void SetAngularDamping(float afDamping);
		float GetAngularDamping() const;
		void SetMaxLinearSpeed(float afSpeed);
		float GetMaxLinearSpeed() const;
		void SetMaxAngularSpeed(float afDamping);
		float GetMaxAngularSpeed() const;
		cVector3f GetInertiaVector();
		cMatrixf GetInertiaMatrix();

		void  SetMass(float afMass);
		float GetMass() const;
		void  SetMassCentre(const cVector3f& avCentre);
		cVector3f GetMassCentre() const;

		void AddForce(const cVector3f &avForce);
		void AddForceAtPosition(const cVector3f &avForce, const cVector3f &avPos);
		void AddTorque(const cVector3f &avTorque);
		void AddImpulse(const cVector3f &avImpulse);
		void AddImpulseAtPosition(const cVector3f &avImpulse, const cVector3f &avPos);

		void Enable();
		bool GetEnabled() const;
		void SetAutoDisable(bool abEnabled);
		bool GetAutoDisable() const;
		void SetAutoDisableLinearThreshold(float afThresold);
		float GetAutoDisableLinearThreshold() const;
		void SetAutoDisableAngularThreshold(float afThresold);
		float GetAutoDisableAngularThreshold() const;
		void SetAutoDisableNumSteps(int alNum);
		int GetAutoDisableNumSteps() const;
		void SetContinuousCollision(bool abOn);
		bool GetContinuousCollision();


		void SetGravity(bool abEnabled);
		bool GetGravity() const;

		void RenderDebugGeometry(iLowLevelGraphics *apLowLevel,const cColor &aColor);

		NewtonBody *GetNewtonBody(){ return mpNewtonBody;}

		void ClearForces();
		
		void DeleteLowLevel();

		static void SetUseCallback(bool abX){ mbUseCallback = abX;}
	private:
		
		static void OnTransformCallback(const NewtonBody* apBody, const dFloat* apMatrix, int alThreadIndex);
		static void OnUpdateCallback(const NewtonBody* apBody, dFloat afTimestep, int alThreadIndex);

		NewtonBody *mpNewtonBody;
		NewtonWorld *mpNewtonWorld;

		cPhysicsBodyNewtonCallback *mpCallback;

		static bool mbUseCallback;

		//Properties
		bool mbGravity;
		
		float mfMaxLinearSpeed;
		float mfMaxAngularSpeed;
		float mfMass;

		float mfAutoDisableLinearThreshold;
		float mfAutoDisableAngularThreshold;
		int mlAutoDisableNumSteps;

		// Forces that will be set and clear on update callback
		cVector3f mvTotalForce;
		cVector3f mvTotalTorque;
	};
};
#endif // HPL_PHYSICS_BODY_NEWTON_H
