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

#ifndef HPL_PHYSICS_JOINT_NEWTON_H
#define HPL_PHYSICS_JOINT_NEWTON_H

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif
#include <Newton.h>
#include "impl/PhysicsWorldNewton.h"
#include "impl/PhysicsBodyNewton.h"
#include "math/Math.h"
#include "system/LowLevelSystem.h"

namespace hpl {

	template <typename T>
	class iPhysicsJointNewton : public T
	{
	public:
		iPhysicsJointNewton(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
						iPhysicsWorld *apWorld,const cVector3f &avPivotPoint,const cVector3f &avPinDir)
		: T(asName, apParentBody, apChildBody, apWorld,avPivotPoint,avPinDir)
		{
			cPhysicsWorldNewton *pNWorld = static_cast<cPhysicsWorldNewton*>(apWorld);

			mpNewtonWorld = pNWorld->GetNewtonWorld();

			cPhysicsBodyNewton *pNParent = static_cast<cPhysicsBodyNewton*>(apParentBody);
			cPhysicsBodyNewton *pNChild = static_cast<cPhysicsBodyNewton*>(apChildBody);

			if(apParentBody==NULL)
				mpNewtonParentBody = NULL;
			else
				mpNewtonParentBody = pNParent->GetNewtonBody();

			mpNewtonChildBody = pNChild->GetNewtonBody();
		}

		virtual ~iPhysicsJointNewton()
		{
			//Skip this for now and let newton handle it..
			//Log("Destroying newton joint!\n");
			if(this->mpChildBody || this->mpParentBody)
				NewtonDestroyJoint(mpNewtonWorld,mpNewtonJoint);
		}

		//-------------------------------------------

		void SetCollideBodies(bool abX)
		{
			NewtonJointSetCollisionState(mpNewtonJoint,abX ? 1: 0);
		}

		bool GetCollideBodies()
		{
			return NewtonJointGetCollisionState(mpNewtonJoint)==0 ? false : true;
		}

		//-------------------------------------------

		void SetStiffness(float afX)
		{
			NewtonJointSetStiffness(mpNewtonJoint, afX);
		}
		float GetStiffness()
		{
			return NewtonJointGetStiffness(mpNewtonJoint);
		}

		//-------------------------------------------

	protected:
		//-------------------------------------------	

		cMatrixf GetMatrixFromPinAndPivot(const cVector3f& avPinDir,const cVector3f& avPivot)
		{
			cMatrixf mtxPinAndPivot = cMatrixf::Identity;
			cVector3f vUp = cMath::Vector3Normalize(avPinDir);
			cVector3f vTemp = cMath::Vector3Normalize(cVector3f(1) - vUp);
			if(vTemp == vUp) vTemp = cMath::Vector3Normalize(vTemp * cVector3f(1.5f, 0.5f, 2.0f)); //Make sure all does not become wierd in the special case.

			cVector3f vRight = cMath::Vector3Normalize( cMath::Vector3Cross(vUp, vTemp ));
			cVector3f vForward = cMath::Vector3Normalize(cMath::Vector3Cross(vUp, vRight));

			mtxPinAndPivot.SetUp(vUp);
			mtxPinAndPivot.SetRight(vRight);
			mtxPinAndPivot.SetForward(vForward);
			mtxPinAndPivot = mtxPinAndPivot.GetTranspose();

			mtxPinAndPivot.SetTranslation(avPivot);

			return mtxPinAndPivot;
		}


		//-------------------------------------------
		
		void CreateCustomJoint(int alMaxDOF)
		{
			mlMaxDOF = alMaxDOF;
			mpNewtonJoint = NewtonConstraintCreateUserJoint (mpNewtonWorld, mlMaxDOF, StaticSubmitConstraints, StaticGetInfo, mpNewtonChildBody, mpNewtonParentBody); 

			NewtonJointSetUserData (mpNewtonJoint, this);
		}

		//-------------------------------------------

		void CalculateGlobalMatrix (const cMatrixf& a_mtxLocalMatrix0, const cMatrixf& a_mtxLocalMatrix1, cMatrixf& a_mtxMatrix0, cMatrixf& a_mtxMatrix1)
		{
			cMatrixf mtxMatrix0 = this->mpChildBody->GetLocalMatrix();
			cMatrixf mtxMatrix1 = this->mpParentBody ? this->mpParentBody->GetLocalMatrix() : cMatrixf::Identity;

			a_mtxMatrix0 = cMath::MatrixMul(mtxMatrix0, a_mtxLocalMatrix0);
			a_mtxMatrix1 = cMath::MatrixMul(mtxMatrix1, a_mtxLocalMatrix1);
		}
		
		//-------------------------------------------

		void CalculateLocalMatrix (const cMatrixf& a_mtxPinsAndPivotFrame, cMatrixf& a_mtxLocalMatrix0, cMatrixf& a_mtxLocalMatrix1)
		{
			cMatrixf mtxMatrix0 = this->mpChildBody->GetLocalMatrix();
			cMatrixf mtxMatrix1 = this->mpParentBody ? this->mpParentBody->GetLocalMatrix() : cMatrixf::Identity;

			// calculate the relative matrix of the pin and pivot on each body
			a_mtxLocalMatrix0 = cMath::MatrixMul( cMath::MatrixInverse(mtxMatrix0), a_mtxPinsAndPivotFrame);
			a_mtxLocalMatrix1 = cMath::MatrixMul( cMath::MatrixInverse(mtxMatrix1), a_mtxPinsAndPivotFrame);
		}
		
		//-------------------------------------------

		virtual void SubmitConstraints (dFloat afTimestep, int alThreadIndex){}
		virtual void GetInfo (NewtonJointRecord* apInfo){}
		
		//-------------------------------------------

		static void StaticSubmitConstraints (const NewtonJoint* apJoint, dFloat afTimestep, int alThreadIndex)
		{
			iPhysicsJointNewton<T> *pJointData = (iPhysicsJointNewton<T>*)NewtonJointGetUserData(apJoint);

			pJointData->SubmitConstraints(afTimestep, alThreadIndex);
		}
		
		static void StaticGetInfo (const NewtonJoint* apJoint, NewtonJointRecord* apInfo)
		{
			iPhysicsJointNewton<T> *pJointData = (iPhysicsJointNewton<T>*)NewtonJointGetUserData(apJoint);

            pJointData->GetInfo(apInfo);
		}

		//-------------------------------------------

		NewtonJoint* mpNewtonJoint;
		NewtonWorld* mpNewtonWorld;
		NewtonBody* mpNewtonParentBody;
		NewtonBody* mpNewtonChildBody;
		
		int mlMaxDOF;
	};
};
#endif // HPL_PHYSICS_JOINT_NEWTON_H
