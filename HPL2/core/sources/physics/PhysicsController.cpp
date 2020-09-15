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

#include "physics/PhysicsController.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJoint.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iPhysicsController::iPhysicsController(const tString &asName, iPhysicsWorld *apWorld)
	{
		msName = asName;
		
		mpWorld = apWorld;

		mpBody = NULL;
		mpJoint = NULL;

		mbActive = false;

		mPidController.SetErrorNum(10);

		mbMulMassWithOutput = false;

		mfMaxOutput = 0;

		mbLogInfo = false;

		mbPaused = false;
	}
	
	//-----------------------------------------------------------------------

	iPhysicsController::~iPhysicsController()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iPhysicsController::SetPidIntegralSize(int alSize)
	{
		mPidController.SetErrorNum(alSize);
	}

	//-----------------------------------------------------------------------

	
	void iPhysicsController::Update(float afTimeStep)
	{
		if(mbActive==false || mbPaused) return;
		if(mpBody==NULL) return;

		cVector3f vInput = GetInputValue(mInputType);
		//Get the local input.
		vInput = cMath::MatrixMul(cMath::MatrixInverse(mpBody->GetLocalMatrix().GetRotation()),vInput);

		float fValue = GetAxisValue(mInputAxis, vInput);
		float fError = mfDestValue - fValue;

		float fOutput = GetOutputValue(fError,fValue,afTimeStep);

		if(mfMaxOutput>0){
			if(fOutput>0)
				fOutput = cMath::Min(fOutput, mfMaxOutput);
			else
				fOutput = cMath::Max(fOutput, -mfMaxOutput);
		}
		
		if(mbLogInfo)
			Log("%s | Input: %f Dest: %f Error: %f OutPut: %f\n",msName.c_str(),fValue,mfDestValue,fError,fOutput);

		AddOutputValue(mOutputType,mOutputAxis,fOutput);
		
		////////////////////////////////////////
		//Check if dest vale is reached
		if(mEndType == ePhysicsControllerEnd_OnDest && mpJoint)
		{
			if(std::abs(fValue - mfDestValue) < kEpsilonf)
			{
				mbActive = false;
				iPhysicsController *pNext = mpJoint->GetController(msNextController);
				if(pNext) pNext->SetActive(true);
			}
		}
	}

	//-----------------------------------------------------------------------

	void iPhysicsController::SetActive(bool abX)
	{	
		if(abX == mbActive) return;

        mPidController.Reset();

		mbActive = abX;
	}

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVector3f iPhysicsController::GetInputValue(ePhysicsControllerInput aInput)
	{
		switch(aInput)
		{
		case ePhysicsControllerInput_AngularSpeed: return mpBody ? mpBody->GetAngularVelocity() : 0;
		case ePhysicsControllerInput_LinearSpeed: return mpBody ? mpBody->GetLinearVelocity() : 0;
		case ePhysicsControllerInput_JointAngle: return mpJoint ? mpJoint->GetAngle() : 0;
		case ePhysicsControllerInput_JointDist: return mpJoint ? mpJoint->GetDistance() : 0;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	float iPhysicsController::GetOutputValue(float afError,float afInput, float afTimeStep)
	{
		if(mType == ePhysicsControllerType_Pid)
		{
			mPidController.p = mfA;
			mPidController.i = mfB;
			mPidController.d = mfC;

			return mPidController.Output(afError, afTimeStep);
		}
		else
		{
			return afError *mfA - afInput *mfB;
		}

		return 0;
	}
	
	//-----------------------------------------------------------------------

	void iPhysicsController::AddOutputValue(ePhysicsControllerOutput aOutput,
									ePhysicsControllerAxis aAxis,
									float afVal)
	{
		cVector3f vVec(0,0,0);
		
		switch(aAxis)
		{
		case ePhysicsControllerAxis_X: vVec.x = afVal; break;
		case ePhysicsControllerAxis_Y: vVec.y = afVal; break;
		case ePhysicsControllerAxis_Z: vVec.z = afVal; break;
		}

		if(mbMulMassWithOutput) vVec = vVec * mpBody->GetMass();

		//Set the output to body space
		vVec = cMath::MatrixMul(mpBody->GetLocalMatrix().GetRotation(), vVec);


		switch(aOutput)
		{
		case ePhysicsControllerOutput_Torque: mpBody->AddTorque(vVec); break;
		case ePhysicsControllerOutput_Force: mpBody->AddForce(vVec); break;
		}
	}
	
	//-----------------------------------------------------------------------

	float iPhysicsController::GetAxisValue(ePhysicsControllerAxis aAxis, const cVector3f &avVec)
	{
		switch(aAxis)
		{
		case ePhysicsControllerAxis_X: return avVec.x;
		case ePhysicsControllerAxis_Y: return avVec.y;
		case ePhysicsControllerAxis_Z: return avVec.z;
		}
		return 0;
	}

	//-----------------------------------------------------------------------
}
