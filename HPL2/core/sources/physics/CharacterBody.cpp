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

#include "physics/CharacterBody.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "system/LowLevelSystem.h"
#include "scene/Camera.h"
#include "math/Math.h"

#include "engine/Engine.h"
#include "scene/Scene.h"
#include "scene/World.h"

namespace hpl {
	//////////////////////////////////////////////////////////////////////////
	// COLLIDE GRAVITY
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	std::vector<iPhysicsBody*> iCharacterBody::mvTempBodies;

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// COLLIDE GRAVITY
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cCharacterBodyCollideGravity::cCharacterBodyCollideGravity(iCharacterBody *apCharBody)
	{
		mpCharBody = apCharBody;
	}

	//-----------------------------------------------------------------------

	void cCharacterBodyCollideGravity::OnCollision(iPhysicsBody *apBody, cCollideData *apCollideData)
	{
		mpCharBody->mpGravityCollideMaterial = apBody->GetMaterial();

		/////////////////////////////////////
		//Check if body allow character attachment, if so attach character to it
		if(	apBody->GetGravityCanAttachCharacter() && apBody->IsCharacter()==false &&
			apCollideData->mvContactPoints[0].mvNormal.y > 0.001f)	//Quick check to make sure that character is just not sliding against the body.
		{
			//Choose the body that has the most contacts.
			if(	mpCharBody->mlGravityAttachmentContacts < apCollideData->mlNumOfPoints)
			{
				mpCharBody->mlGravityAttachmentContacts = apCollideData->mlNumOfPoints;
				mpCharBody->mpGravityAttachedBody = apBody;
			}
		}

		if(mpCharBody->mpCallback) mpCharBody->mpCallback->OnGravityCollide(mpCharBody,apBody,apCollideData);

		if(apBody->GetMass()==0 || apBody->GetPushedByCharacterGravity()==false) return;

		//////////////////////////////////////
		//Go through all of the contact points and check if any is a movement up.
		//This means the body is below the character and should be pushed down.
		bool bPushDown=false;
		cVector3f vMedianPoint(0,0,0);
		float fNumPoints=0;
		for(int i=0; i< apCollideData->mlNumOfPoints; i++)
		{
			//TODO: Get the point at which to apply the force.
			cCollidePoint &point = apCollideData->mvContactPoints[i];
			if(point.mvNormal.y > 0.001f)
			{
				bPushDown = true;
				fNumPoints+=1;
				vMedianPoint += point.mvPoint;
			}
		}
		
		if(bPushDown)
		{
			vMedianPoint = vMedianPoint / fNumPoints;

			//Transfer the velocity of the player to the body.
			// SKIPPING THIS! For now. It makes stuff very unstable.
			/*if(mpCharBody->GetForceVelocity().y < 0.1f && mpCharBody->IsOnGround()==false)
			{
				float fImpulse = mpCharBody->GetForceVelocity().y * (mpCharBody->GetMass() / apBody->GetMass());
				apBody->AddImpulseAtPosition(cVector3f(0,fImpulse,0), vMedianPoint);
			}*/

			float fForceMul = mpCharBody->mbCustomGravity ? mpCharBody->mvCustomGravity.y : mpCharBody->mpWorld->GetGravity().y;
			if(fForceMul < 0)
				apBody->AddForceAtPosition(cVector3f(0,mpCharBody->GetMass() * fForceMul,0), vMedianPoint);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// COLLIDE PUSH
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cCharacterBodyCollidePush::cCharacterBodyCollidePush(iCharacterBody *apCharBody)
	{
		mpCharBody = apCharBody;
	}

	//-----------------------------------------------------------------------

	void cCharacterBodyCollidePush::OnCollision(iPhysicsBody *apBody, cCollideData *apCollideData)
	{
		///////////////////////////////////
		//Check what bodies not to push.
		if(apBody->GetMass()==0 && apBody->IsCharacter()==false) return;
		if(apBody->GetMass() > mpCharBody->GetMaxPushMass()) return;

		///////////////////////////////
		//Character is hit
		if(apBody->IsCharacter())
		{
			iCharacterBody *pHitCharBody = apBody->GetCharacterBody();
			if(pHitCharBody->GetMass() > mpCharBody->GetCharacterMaxPushMass()) return;


			float fHitTop = pHitCharBody->GetPosition().y + pHitCharBody->GetSize().y/2;
			//Make bottom 10% of size longer up to remove uneeded pushing.
			float fBottom = mpCharBody->GetPosition().y - (mpCharBody->GetSize().y/2 - mpCharBody->GetSize().y*0.1f);

			//Log("Top: %f Bottom: %f. HitSize: %f Size: %f. HitPos: %f Pos: %f\n",fHitTop, fBottom,pHitCharBody->GetSize().y,mpCharBody->GetSize().y,
			//																		pHitCharBody->GetPosition().y,mpCharBody->GetPosition().y);

			//If body is on top of hitbody, skip doing a push.
			if(fHitTop < fBottom) return;

			cVector3f vDir = pHitCharBody->GetPosition() - mpCharBody->GetPosition();
			if(mpCharBody->GetCharacterPushIn2D()) vDir.y =0; 
			vDir.Normalize();

			pHitCharBody->AddForce(vDir * mpCharBody->GetCharacterPushForce());
		}
		//////////////////////////////
		//Normal body is hit
		else
		{
				///////////////////////////////////////// 
			//Go through all of the contact points and find median
			cVector3f vMedianPoint(0,0,0);
			float fNumPoints=0;

			for(int i=0; i< apCollideData->mlNumOfPoints; i++)
			{
				cCollidePoint &point = apCollideData->mvContactPoints[i];

				fNumPoints+=1;
				vMedianPoint += point.mvPoint;
			}
			vMedianPoint = vMedianPoint / fNumPoints;

			//If median is below (or almost below the player), skip push
			float fMinY = mpCharBody->GetFeetPosition().y;
			fMinY += 0.01f;
			if(vMedianPoint.y  <  fMinY) return;
			
			if(mpCharBody->GetPushIn2D())
			{
				cVector3f vDir = vMedianPoint - mpCharBody->GetPosition();
				vDir.y =0; vDir.Normalize();
			
				apBody->AddForceAtPosition(vDir * mpCharBody->GetPushForce(), vMedianPoint);
			}
			else
			{
				cVector3f vDir = cMath::Vector3Normalize(vMedianPoint - mpCharBody->GetPosition());

				apBody->AddForceAtPosition(vDir * mpCharBody->GetPushForce(), vMedianPoint);
			}
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// RAY CAST
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cCharacterBodyRay::cCharacterBodyRay()
	{

	}

	//-----------------------------------------------------------------------

	void cCharacterBodyRay::Clear()
	{
		mfMinDist = 10000.0f;
		mbCollide = false;
	}

	//-----------------------------------------------------------------------

	bool cCharacterBodyRay::OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams)
	{
		if(	pBody->IsCharacter()==false && pBody->GetCollideCharacter() && 
			apParams->mfDist < mfMinDist)
		{
			mfMinDist = apParams->mfDist;
			mvNormal = apParams->mvNormal;
			mbCollide = true;
		}

		return true;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iCharacterBody::iCharacterBody(const tString &asName, iPhysicsWorld *apWorld, const cVector3f avSize)
	{

		/////////////////////////////
		//Set up main properties
		msName = asName;

		mpWorld = apWorld;

		mvSize = avSize;

		mpCallback = NULL;

		/////////////////////////////
		// Create Shape
		float fRadius = cMath::Max(avSize.x, avSize.z)*0.5f;
		cMatrixf mtxOffset = cMath::MatrixRotateZ(kPi2f);
		iCollideShape *pCollider = NULL;
		
		float fForceAdd = 0.2f;
		if(fabs(fRadius*2.0f - avSize.y)<0.01)
		{
			mpCurrentShape = mpWorld->CreateSphereShape(fRadius,NULL);
		}
		else
		{
			mpCurrentShape = mpWorld->CreateCylinderShape(fRadius, avSize.y,&mtxOffset);
		}
		mpCurrentBody = mpWorld->CreateBody(asName, mpCurrentShape);
		
		mpCurrentBody->SetMass(0);
		mpCurrentBody->SetGravity(false);
		mpCurrentBody->SetIsCharacter(true);
		mpCurrentBody->SetCharacterBody(this);
		mpCurrentBody->SetCollideFlags(mlCollideFlags);
		mpCurrentBody->SetCollideCharacter(true);
		
		mvShapes.push_back(mpCurrentShape);
		mvBodies.push_back(mpCurrentBody);
	

		/////////////////////////////
		//Create callbacks
		mpCollideGravityCallback = hplNew( cCharacterBodyCollideGravity, (this) );
		mpCollidePushCallback = hplNew( cCharacterBodyCollidePush, (this) );
		mpRayCallback = hplNew( cCharacterBodyRay, () );

		/////////////////////////////
		//Set up properties
		
		mbActive = true;
		mbCollideCharacter = true;
		mbTestCollision = true;

		mbEntitySmoothYPos = false;
		mlEntitySmoothYPosNum = 20;

		mbDisableDiagSpeedBoost = true;

		mlCurrentShapeIdx =0;

		mfYaw =0;
		mfPitch =0;

		mlCollideFlags = eFlagBit_All;

		//The maximum angle of a slope (compare to gravity direction) where no sliding occurs
		mfMaxNoSlideSlopeAngleCos = cos(cMath::ToRad(45.0f));

		//Set move properties
		for(int i=0; i<eCharDir_LastEnum; i++)
		{
			mfMaxPosMoveSpeed[i]=10;
			mfMaxNegMoveSpeed[i]=-10;
			mfMoveSpeed[i]=0;
			mfMoveAcc[i]=20;
			mfMoveDeacc[i]=20;
			mbMoving[i]=false;
			mfCurrentMoveAcc[i] =0;
			mfMoveOppositeDirAccMul[i] = 1;
		}

		mbMovedLastUpdate = false;

		mbDeaccelerateMoveSpeedInAir = false;
		
		mvForce = cVector3f(0,0,0);
		mvVelocity = cVector3f(0,0,0);

		mvLastGroundNormal = cVector3f(0,0,0);
		
		mbGravityActive = true;

		mfMaxGravitySpeed = 30.0f;

		mfConstantContactForceMul = 16.0f;
		mfVelocityContactForceMul = 4.0f;
		mfMaxContactForcePerMassUnit = 250.0f;

		mpCamera = NULL;
		mvCameraPosAdd = cVector3f(0,0,0);

		mpEntity = NULL;
		m_mtxEntityOffset = cMatrixf::Identity;
		m_mtxEntityPostOffset = cMatrixf::Identity;

		mpUserData = NULL;

		mlCameraSmoothPosNum =0;
		mlEntitySmoothPosNum =0;

		mlOnGroundCount = 0;
		mlMaxOnGroundCount = 12;

		mfMaxStepHeight = mvSize.y *0.2f;
		mfMaxStepHeightInAir = mfMaxStepHeight;
		mfStepClimbSpeed = 1.0f;
		mfClimbForwardMul = 1.0f;
		mfClimbHeightAdd = 0.01f;
		mbClimbing = false;
		mbAccurateClimbing = false;

		mfCheckStepClimbCount = 0;
		mfCheckStepClimbInterval = 1/20.0f;

		mfGroundFriction = 6.0f;
		mfAirFriction = 0.3f;
		
		mfMass = 1;

		mbStickToSlope = true;

		mfMaxPushMass = 0;
		mfPushForce = 0;
		mbPushIn2D = true;

		mfCharacterMaxPushMass =0;
		mfCharacterPushForce =0;
		mbCharacterPushIn2D = true;

		mlMinBodyPushStrength = 0;

		mfMaxConnectionTorque =0;
		mfMaxConnectionForce =0;
		mpConnectedBody = NULL;
		mBodyConnectionPointPid.SetErrorNum(10);
		mBodyCenterPid.SetErrorNum(10);
		mBodyOneAxisAngularPid.SetErrorNum(10);
		mBodyAngularPid.SetErrorNum(10);
		mCharConnectionPointPid.SetErrorNum(10);
		mCharRotatePid.SetErrorNum(10);

		mbRotateYawWhenGravityAttached = true;
		mpGravityAttachedBody = NULL;
		mlGravityAttachmentContacts =0;
		mpGravityLastAttachedBody = NULL;
		mvGravityAttachmentVelocity =0;	
        
		mbCustomGravity = false;
		mvCustomGravity = cVector3f(0,9.8f,0);

		mpGravityCollideMaterial = NULL;

		mfMoveDelayCount=0;
	}

	//-----------------------------------------------------------------------

	iCharacterBody::~iCharacterBody()
	{
		DisconnectBody();

		for(size_t i=0; i< mvBodies.size(); i++) mpWorld->DestroyBody(mvBodies[i]);

		hplDelete(mpCollideGravityCallback);
		hplDelete(mpCollidePushCallback);
		hplDelete(mpRayCallback);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	float iCharacterBody::GetMass()
	{
		return mfMass;
	}
	void iCharacterBody::SetMass(float afMass)
	{
		mfMass = afMass;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetActive(bool abX)
	{
		if(mbActive == abX) return;

		mbActive = abX;
		
		if(mbActive==false) 
		{
			mpCurrentBody->SetActive(false);
		}
		else if(mbActive && mbTestCollision) 
		{
			mpCurrentBody->SetActive(true);
		}       
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetCollideCharacter(bool abX)
	{
		if(abX == mbCollideCharacter) return;

		mbCollideCharacter = abX;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetTestCollision(bool abX)
	{
		if(mbTestCollision == abX) return;

		mbTestCollision = abX;
		
		if(mbTestCollision==false) 
		{
			mpCurrentBody->SetActive(false);
		}
		else if(mbTestCollision && mbActive) 
		{
			mpCurrentBody->SetActive(true);
		}

	}

	//-----------------------------------------------------------------------

	cVector3f iCharacterBody::GetSize()
	{
		return mvSize;
	}

	//-----------------------------------------------------------------------
	
	int iCharacterBody::AddExtraSize(const cVector3f& avSize)
	{
		float fRadius = cMath::Max(avSize.x, avSize.z)*0.5f;
		cMatrixf mtxOffset = cMath::MatrixRotateZ(kPi2f);
		iCollideShape *pCollider = mpWorld->CreateCylinderShape(fRadius, avSize.y,&mtxOffset);
		iPhysicsBody *pBody = mpWorld->CreateBody(msName, pCollider);

		pBody->SetMass(0);
		pBody->SetGravity(false);
		pBody->SetIsCharacter(true);
		pBody->SetCharacterBody(this);
		pBody->SetActive(false);
		pBody->SetCollideFlags(mlCollideFlags);
		pBody->SetCollideCharacter(true);
		
		mvBodies.push_back(pBody);
		mvShapes.push_back(pCollider);

		return (int)mvShapes.size() -1;
	}
	
	//-----------------------------------------------------------------------

	void iCharacterBody::SetActiveSize(int alNum)
	{
		if(alNum == mlCurrentShapeIdx) return;

		cVector3f vFeetPosition = GetFeetPosition();
		mpCurrentBody->SetActive(false);

		mlCurrentShapeIdx = alNum;

		mpCurrentShape = mvShapes[alNum];
		mpCurrentBody = mvBodies[alNum];

		SetPosition(vFeetPosition + cVector3f(0,mpCurrentShape->GetSize().y/2.0f,0), true);
				
		if(mbActive && mbTestCollision)
			mpCurrentBody->SetActive(true);
		else
			mpCurrentBody->SetActive(false);
		
		//Set size of the new shape
		mvSize.y = mpCurrentShape->GetHeight();
		mvSize.x = mpCurrentShape->GetRadius() * 2;
		mvSize.z = mpCurrentShape->GetRadius() * 2;
	}
	
	//-----------------------------------------------------------------------
	
	void iCharacterBody::SetMaxPositiveMoveSpeed(eCharDir aDir, float afX)
	{
		mfMaxPosMoveSpeed[aDir] = afX;
	}
	float iCharacterBody::GetMaxPositiveMoveSpeed(eCharDir aDir)
	{
		return mfMaxPosMoveSpeed[aDir];
	}
	void iCharacterBody::SetMaxNegativeMoveSpeed(eCharDir aDir, float afX)
	{
		mfMaxNegMoveSpeed[aDir] = afX;
	}
	float iCharacterBody::GetMaxNegativeMoveSpeed(eCharDir aDir)
	{
		return mfMaxNegMoveSpeed[aDir];
	}
	
	//-----------------------------------------------------------------------

	void iCharacterBody::SetMoveSpeed(eCharDir aDir, float afX)
	{
		mfMoveSpeed[aDir] = afX;
	}
	float iCharacterBody::GetMoveSpeed(eCharDir aDir)
	{
		return mfMoveSpeed[aDir];
	}

	void iCharacterBody::SetMoveAcc(eCharDir aDir, float afX)
	{
		mfMoveAcc[aDir] = afX;
	}
	float iCharacterBody::GetMoveAcc(eCharDir aDir)
	{
		return mfMoveAcc[aDir];
	}

	void iCharacterBody::SetMoveDeacc(eCharDir aDir, float afX)
	{
		mfMoveDeacc[aDir] = afX;
	}
	float iCharacterBody::GetMoveDeacc(eCharDir aDir)
	{
		return mfMoveDeacc[aDir];
	}

	//-----------------------------------------------------------------------

	cVector3f iCharacterBody::GetVelocity(float afFrameTime)
	{
		if(afFrameTime <=0) return 0;
		
        return (mvPosition - mvLastPosition) / afFrameTime;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetPosition(const cVector3f& avPos, bool abSmooth)
	{
		//mvForce =0;
		//mvVelocity =0;
		mvLastPosition = avPos;
		mvPosition = avPos;

		mpCurrentBody->SetPosition(avPos);
		
		if(!abSmooth){
			mlstCameraPos.clear();
			mlstEntityPos.clear();
			mlstEntityYPositions.clear();
		}
	}
	const cVector3f& iCharacterBody::GetPosition()
	{
		return mvPosition;
	}
	const cVector3f& iCharacterBody::GetLastPosition()
	{
		return mvLastPosition;
	}

	//-----------------------------------------------------------------------
	
	void iCharacterBody::SetFeetPosition(const cVector3f& avPos, bool abSmooth)
	{
		SetPosition(avPos + cVector3f(0,mvSize.y/2,0),abSmooth);
	}

	cVector3f iCharacterBody::GetFeetPosition()
	{
		return mvPosition - cVector3f(0,mvSize.y/2,0);
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetYaw(float afX)
	{
		//Do not set rotation, if character is being aligned.
		if(mpConnectedBody && mbConnectionAlignCharacterRotation) return;
        
		mfYaw = afX;
	}

	float iCharacterBody::GetYaw()
	{
		return mfYaw;
	}

	void iCharacterBody::AddYaw(float afX)
	{
		mfYaw += afX;
	}

	void iCharacterBody::SetPitch(float afX)
	{
		//Do not set rotation, if character is being aligned.
		if(mpConnectedBody && mbConnectionAlignCharacterRotation) return;

		mfPitch = afX;
	}

	void iCharacterBody::AddPitch(float afX)
	{
		mfPitch += afX;
	}

	float iCharacterBody::GetPitch()
	{
		return mfPitch;
	}

	//-----------------------------------------------------------------------

	const cVector3f& iCharacterBody::GetForward()
	{
		return mvForward;
	}
	const cVector3f& iCharacterBody::GetRight()
	{
		return mvRight;
	}
	const cVector3f& iCharacterBody::GetUp()
	{
		return mvUp;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetGravityActive(bool abX)
	{
		mbGravityActive = abX;
	}

	bool iCharacterBody::GravityIsActive()
	{
		return mbGravityActive;
	}

	void iCharacterBody::SetMaxGravitySpeed(float afX)
	{
		mfMaxGravitySpeed = afX;
	}
	float iCharacterBody::GetMaxGravitySpeed()
	{
		return mfMaxGravitySpeed;
	}

	//-----------------------------------------------------------------------

	bool iCharacterBody::GetCustomGravityActive()
	{
		return mbCustomGravity;
	}
	
	void iCharacterBody::SetCustomGravityActive(bool abX)
	{
		mbCustomGravity = abX;
	}
	
	void iCharacterBody::SetCustomGravity(const cVector3f& avCustomGravity)
	{
		mvCustomGravity = avCustomGravity;
	}
	
	cVector3f iCharacterBody::GetCustomGravity()
	{
		return mvCustomGravity;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetCollideFlags(tFlag alX)
	{	
		if(mlCollideFlags == alX) return;

		mlCollideFlags = alX;
		
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
			pBody->SetCollideFlags(mlCollideFlags);
		}
	}
	
	//-----------------------------------------------------------------------

	void iCharacterBody::SetMaxNoSlideSlopeAngle(float afAngle)
	{
		mfMaxNoSlideSlopeAngleCos = cos(afAngle);
	}
	
	float iCharacterBody::GetMaxNoSlideSlopeAngle()
	{
		return acos(mfMaxNoSlideSlopeAngleCos);
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::ResetClimbing()
	{
		mbClimbing = false;
	}
	
	//-----------------------------------------------------------------------

	void iCharacterBody::SetForce(const cVector3f &avForce)
	{
		mvForce = avForce;
	}
	
	void iCharacterBody::AddForce(const cVector3f &avForce)
	{
		mvForce += avForce;
	}
	
	cVector3f iCharacterBody::GetForce()
	{
		return mvForce;
	}

	//-----------------------------------------------------------------------
	
	void iCharacterBody::Move(eCharDir aDir, float afMul)
	{
		if(mfMoveDelayCount>0) return; //Just skip any movement.

		mfCurrentMoveAcc[aDir] += afMul;

		mbMoving[aDir] = true;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::StopMovement()
	{
		SetForceVelocity(0);

		for(int i=0; i<2; ++i)
		{
			mfCurrentMoveAcc[i] = 0;
			mbMoving[i] = false;
			mfMoveAcc[i] =0;
			mfMoveSpeed[i]=0;
		}
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::ConnectToBody(	iPhysicsBody *apBody, const cVector3f& avConnectionPos, 
										tPhysicsConnectionFlag alFlags)
	{
		if(apBody == NULL) return;

		//////////////////////////////////////////
		// Set the connected body and add charater as connected on body
		mpConnectedBody = apBody;
		mpConnectedBody->AddConnectedCharacter(this);

		//////////////////////////////////////////
		// Set if connection depends on body rotation.
		mbConnectionDependOnBodyRotation = (alFlags & ePhysicsConnectionFlag_DependOnBodyRotation) != 0;

		////////////////////////////////
		//Get inverse body matrix
        cMatrixf mtxInvBodyWorld = cMath::MatrixInverse(mpConnectedBody->GetLocalMatrix());
		
		//////////////////////////////////////////
		// Set the local connection postion for body
        if(mbConnectionDependOnBodyRotation)
		{
			mvConnectionPosLocalToBody = cMath::MatrixMul(mtxInvBodyWorld, avConnectionPos);
		}
		else
		{
			mvConnectionPosLocalToBody = avConnectionPos - mpConnectedBody->GetWorldPosition();
		}

		//////////////////////////////////////////
		// Get the start rotation for body
		mvConnectionBodyStartRotation = cMath::MatrixToEulerAngles(mpConnectedBody->GetLocalMatrix().GetRotation(), eEulerRotationOrder_XYZ);
		
		//////////////////////////////////////////
		// Set up pids
		mBodyConnectionPointPid.Reset();
		mBodyCenterPid.Reset();
		mBodyOneAxisAngularPid.Reset();
		mBodyAngularPid.Reset();

		mCharConnectionPointPid.Reset();
		mCharRotatePid.Reset();

		//////////////////////////////////////////
		// More body properties
		mbConnectionAlignRotation = (alFlags & ePhysicsConnectionFlag_AlignBodyRotation) != 0;
		mbConnectionAlignCenter = (alFlags & ePhysicsConnectionFlag_AlignBodyCenter) != 0;

		//Connection point to body normal.
        mvConnectionNormalLocalToBody = cMath::Vector3Normalize(mpConnectedBody->GetWorldPosition() - avConnectionPos);
		mfConnectionDistanceToBody = mvConnectionPosLocalToBody.Length();

		//The local wanted up vector (when this vector is up, body is correct!)
		mvConnectionLocalUpBody = cMath::MatrixMul(mtxInvBodyWorld.GetRotation(), GetUp());
		mvConnectionLocalUpBody.Normalize();

		///////////////////////////////////
		// Disable gravity or not
		if(alFlags & ePhysicsConnectionFlag_DisableBodyGravity)
		{
			mbConnectedBodyHadGravity = mpConnectedBody->GetGravity();
			if(mbConnectedBodyHadGravity)
				mpConnectedBody->SetGravity(false);
		}

		///////////////////////////////////
		// Connection affects character
		mbConnectionAffectChar = (alFlags & ePhysicsConnectionFlag_AffectChar) != 0;

		///////////////////////////////////
		//Local character connection point
		mvConnectionPosLocalToChar = avConnectionPos - mvPosition;


		///////////////////////////////////
		// Align character rotation
		mbConnectionAlignCharacterRotation = (alFlags & ePhysicsConnectionFlag_AlignCharRotation) != 0;
		if(mbConnectionAlignCharacterRotation)
		{
			//Make sure it is local, with no rotation from body.
			mvConnectionCharLocalForward = cMath::MatrixMul(mtxInvBodyWorld.GetRotation(), mvForward);
			
			//Character to connection vector
			cMatrixf mtxInvCharRot = cMath::MatrixInverse(m_mtxMove.GetRotation());
			mvConnectionPosLocalToChar = cMath::MatrixMul(mtxInvCharRot, mvConnectionPosLocalToChar);
		}

		///////////////////////////////////
		// Depend on character rotation
		
		//Do not allow connection dependent on rotation when this is true!
		if(mbConnectionAlignCharacterRotation)
			mbConnectionDependOnCharRotation = false;
		else
			mbConnectionDependOnCharRotation = (alFlags & ePhysicsConnectionFlag_DependOnCharRotation) != 0;
		
		if(mbConnectionDependOnCharRotation)
		{
			//If body is aligned according to character rotation, then we need to make sure vectors are local when rotation is 0,
			//hence we multiply with inverse rotation.
			UpdateMoveMatrix();
			cMatrixf mtxInvCharRot = cMath::MatrixInverse(m_mtxMove.GetRotation());
			
			//Character to connection vector
			mvConnectionPosLocalToChar = cMath::MatrixMul(mtxInvCharRot, mvConnectionPosLocalToChar);

			//Connection to body normal
			mvConnectionNormalLocalToBody = cMath::MatrixMul(mtxInvCharRot, mvConnectionNormalLocalToBody);
		}


		////////////////////////////////////////
		//If collision between connected character and body is allowed.
		mbConnectionCollision = (alFlags & ePhysicsConnectionFlag_AllowCollision) != 0;

		mbConnectedBodyIsActive = mpConnectedBody->IsActive();
		
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::DisconnectBody()
	{
		if(mpConnectedBody == NULL) return;

		mpConnectedBody->RemoveConnectedCharacter(this);

		if(mbConnectedBodyHadGravity)
			mpConnectedBody->SetGravity(true);

		mpConnectedBody = NULL;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::Update(float afTimeStep)
	{
		if(mbActive == false) return;

		/////////////////////////
		// Move delay
		if(mfMoveDelayCount>0)
		{
			mfMoveDelayCount-=afTimeStep;
		}

		//////////////////////////
		//Init
		PreUpdateConnection(afTimeStep);
		ClearGravityAttachment();

		//////////////////////////
		//Push near objects
		//TODO: Not really working....
		//if(mbTestCollision) 
		//	UpdateForcePushing(afTimeStep);

		//////////////////////////
		// Update the movement
		
		//Set the last position
		mvLastPosition = mvPosition;

		//Update the move matrix.
		UpdateMoveMatrix();

		//Update some step climbing variables.
		UpdateStepClimbing(afTimeStep);

		//Update the position according to speed and heading
		cVector3f vPosAdd = UpdatePostionFromCharSpeed(afTimeStep);

		AlignPosAddAccordingToGroundNormal(vPosAdd);

		mvLastMovePosAdd = vPosAdd;

		//////////////////////////
		//Check for collision.
		if(mbTestCollision)
		{
			//XZ
			CheckMoveCollision(vPosAdd, afTimeStep);
		}
		else
		{
			mvPosition += vPosAdd;
		}

		//////////////////////////
		//Update External forces
		UpdateForces(afTimeStep);
		
		if(mbTestCollision)
			CheckForceCollision(afTimeStep);

		UpdateFriction(afTimeStep);
		
		//////////////////////////////
		//Final updates
		UpdateGravityAttachment();
		UpdateBody();

		UpdateCharacterConnection(afTimeStep);
		UpdateBodyConnection(afTimeStep);
		
		//Not needed, since bodies are affected by force anyway!
		//EnableBodiesAroundCharacter();

		//////////////////////////////
		//Update attached objects
		UpdateCamera();
		UpdateEntity();

		PostUpdateConnection(afTimeStep);
	}
	
	//-----------------------------------------------------------------------

	void iCharacterBody::SetCamera(cCamera *apCam)
	{
		mpCamera = apCam;
	}
	
	cCamera* iCharacterBody::GetCamera()
	{
		return mpCamera;
	}

	void iCharacterBody::SetCameraPosAdd(const cVector3f &avAdd)
	{
		mvCameraPosAdd = avAdd;
	}
	cVector3f iCharacterBody::GetCameraPosAdd()
	{
		return mvCameraPosAdd;
	}

	//-----------------------------------------------------------------------

	iCollideShape * iCharacterBody::GetCurrentShape()
	{ 
		return mpCurrentShape;
	}

	iPhysicsBody* iCharacterBody::GetCurrentBody()
	{
		return mpCurrentBody;
	}

	iCollideShape *iCharacterBody::GetShape(int alIdx)
	{	
		return mvShapes[alIdx];
	}

	bool iCharacterBody::IsOnGround()
	{
		return mlOnGroundCount >0;
	}

	//-----------------------------------------------------------------------
	
	bool iCharacterBody::CheckCharacterFits(const cVector3f &avPosition, bool abFeetPosition, int alSizeIdx, cVector3f *apPushBackVec, float afEpsilon)
	{
		if(alSizeIdx <0) alSizeIdx = mlCurrentShapeIdx;

		iCollideShape *pShape = mvShapes[alSizeIdx];
		cVector3f vPos = avPosition;
        if(abFeetPosition)
		{
			vPos += cVector3f(0,pShape->GetSize().y/2.0f,0);
		}

		cVector3f vPushBack(0);
		CheckCollision(&vPushBack,vPos, NULL, alSizeIdx);

		if(apPushBackVec) *apPushBackVec = vPushBack;

		if(afEpsilon == 0.0f)
		{
			return cMath::Vector3Abs(vPushBack) == cVector3f(afEpsilon);
		}
		else
		{
			return cMath::Vector3Abs(vPushBack) < cVector3f(afEpsilon);
		}
	}

	//-----------------------------------------------------------------------

	bool iCharacterBody::CheckRayIntersection(const cVector3f &avStart, const cVector3f &avEnd, float *apDistance, cVector3f *apNormalVec)
	{
		mpRayCallback->Clear();
		mpWorld->CastRay(mpRayCallback,avStart,avEnd,apDistance!=NULL,apNormalVec!=NULL,false);
		bool bCollide = mpRayCallback->mbCollide;
		if(bCollide)
		{
			if(apDistance) *apDistance = mpRayCallback->mfMinDist;
			if(apNormalVec) *apNormalVec = mpRayCallback->mvNormal;
		}
		return bCollide;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::SetEntity(iEntity3D *apEntity)
	{
		mpEntity = apEntity;
	}
	iEntity3D* iCharacterBody::GetEntity()
	{
		return mpEntity;
	}
	void iCharacterBody::SetEntityOffset(const cMatrixf &a_mtxOffset)
	{
		m_mtxEntityOffset = a_mtxOffset;
	}
	const cMatrixf& iCharacterBody::GetEntityOffset()
	{
		return m_mtxEntityOffset;
	}

	void iCharacterBody::SetEntityPostOffset(const cMatrixf &a_mtxOffset)
	{
		m_mtxEntityPostOffset = a_mtxOffset;
	}
	const cMatrixf& iCharacterBody::GetEntityPostOffset()
	{
		return m_mtxEntityPostOffset;
	}

	//-----------------------------------------------------------------------
	
	iPhysicsBody * iCharacterBody::GetConnectedBody()
	{
		return mpConnectedBody;
	}
	
	cVector3f iCharacterBody::GetBodyConnectionPos()
	{
		if(mpConnectedBody == NULL) return 0;
		
		if(mbConnectionDependOnBodyRotation)
			return cMath::MatrixMul(mpConnectedBody->GetWorldMatrix(), mvConnectionPosLocalToBody);
		else
			return mpConnectedBody->GetWorldPosition() + mvConnectionPosLocalToBody;
	}
	
	cVector3f iCharacterBody::GetCharConnectionPos()
	{
		if(mpConnectedBody == NULL) return 0;

		if(mbConnectionDependOnCharRotation || mbConnectionAlignCharacterRotation)
			return mvPosition + cMath::MatrixMul3x3(m_mtxMove, mvConnectionPosLocalToChar);
		else
			return mvPosition + mvConnectionPosLocalToChar;
	}

	//-----------------------------------------------------------------------

	
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iCharacterBody::UpdateMoveMatrix()
	{
		m_mtxMove = cMath::MatrixRotate(cVector3f(mfPitch,mfYaw,0),eEulerRotationOrder_XYZ);
		m_mtxMove.SetTranslation(mvPosition);

		mvForward = cMath::MatrixMul3x3(m_mtxMove,cVector3f(0,0,-1));
		mvRight = cMath::MatrixMul3x3(m_mtxMove,cVector3f(1,0,0));
		mvUp = cMath::MatrixMul3x3(m_mtxMove,cVector3f(0,1,0));
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::PreUpdateConnection(float afTimeStep)
	{
		if(mpConnectedBody == NULL) return;
	
		if(mbConnectionCollision==false)
		{
			mbConnectedBodyIsActive = mpConnectedBody->IsActive();
			if(mbConnectedBodyIsActive) mpConnectedBody->SetActive(false);
		}
	}
	
	void iCharacterBody::PostUpdateConnection(float afTimeStep)
	{
		if(mpConnectedBody == NULL) return;
		
		if(mbConnectionCollision==false)
		{
			if(mbConnectedBodyIsActive) mpConnectedBody->SetActive(true);
		}
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateBodyConnection(float afTimeStep)
	{
		//No connected body = nothing to do
		if(mpConnectedBody == NULL) return;

		//Nothing to do on static bodies.
		if(mpConnectedBody->GetMass() == 0) return;
		
		cVector3f vTotalTorque=0;
		cVector3f vTotalForce=0;

		//Get the maximum force can be made by the character
		float fMaxTotalCharForce = mfMaxConnectionForce;
		if(IsOnGround()==false)
		{
			float fGravitySize = mbCustomGravity ? mvCustomGravity.Length() : mpWorld->GetGravity().Length();
			fMaxTotalCharForce = fGravitySize * GetMass();
		}
		float fMaxRelativeCharForce = fMaxTotalCharForce / mpConnectedBody->GetMass();
		float fMaxRelativeCharTorque = mfMaxConnectionTorque / mpConnectedBody->GetMass(); //Not really correct


		//////////////////////////////////
		// Linear ConnectionPoint Correction

		//Set up PID (move this SetConnectedBody?)
		mBodyConnectionPointPid.p = 200.0f;
		mBodyConnectionPointPid.i = 0.0f;
		mBodyConnectionPointPid.d = 10.0f;

		//Get the body position of the connection positions
		cVector3f vBodyConnection = GetBodyConnectionPos();
		cVector3f vCharConnection = GetCharConnectionPos();

		cVector3f vError = vCharConnection - vBodyConnection;
		cVector3f vForce = mBodyConnectionPointPid.Output(vError, afTimeStep);
		
		//Make sure this force is not too much for player, do this here to get correct torque!
		cMath::Vector3ClampToLength(vForce, fMaxRelativeCharForce);
		
		vTotalForce += vForce;
		
		//Only use torque if connection depends on rotation!
		if(mbConnectionDependOnBodyRotation)
		{
			cVector3f vTorque = mpConnectedBody->GetTorqueFromForceAtPosition(vForce, vBodyConnection);
			vTotalTorque += vTorque;
		}
		
		//get the normal form connection to body, this will change if connections depends on body rotation.
		cVector3f vNormalToBody = mvConnectionNormalLocalToBody;
		if(mbConnectionDependOnCharRotation)
		{
			vNormalToBody = cMath::MatrixMul(m_mtxMove.GetRotation(), vNormalToBody);
		}
		

		//////////////////////////////////
		// Linear Center Correction
		if(mbConnectionAlignCenter)
		{
			//Setup bid
			mBodyCenterPid.p = 200.0f;
			mBodyCenterPid.i = 0.0f;
			mBodyCenterPid.d = 10.0f;
			
			cVector3f vCenter = mpConnectedBody->GetWorldPosition();
			cVector3f vWantedPosition = vCharConnection + vNormalToBody * mfConnectionDistanceToBody;
			
			vError = vWantedPosition - vCenter;
			vForce = mBodyCenterPid.Output(vError, afTimeStep);
			
			vTotalForce += vForce;
		}		
		
		
		//////////////////////////////////
		// Angular correction
		if(mbConnectionAlignRotation)
		{
			//////////////////////////////////
			// The connection depends on rotation, meaning we need to rotate around the connection axis
            if(mbConnectionDependOnBodyRotation)
			{
				//Set up pid
				mBodyOneAxisAngularPid.p = 20.0f;
				mBodyOneAxisAngularPid.i = 0.0f;
				mBodyOneAxisAngularPid.d = 2.5f;

				//The current direction of up, given that up was == character up when connected.
				cVector3f vBodyUp = cMath::MatrixMul(mpConnectedBody->GetWorldMatrix().GetRotation(), mvConnectionLocalUpBody);

				//The right vector if connection to body is forward.
				cVector3f vBodyRight = cMath::Vector3Cross(vNormalToBody, GetUp());

				float fError =  cMath::Vector3Angle(vBodyUp, GetUp());

				//Get by checking how the up is compared to right, we can get the sign of the rotation.
				// ">" since the error is in the opposite direction, ie the way want to rotate in!
				if(cMath::Vector3Dot(vBodyRight, vBodyUp)>0) fError = -fError;
	            
				//Log("Error: %f\n", cMath::ToDeg(fError));
				float fTorqueSize = mBodyOneAxisAngularPid.Output(fError, afTimeStep);

				cVector3f vTorque = vNormalToBody * fTorqueSize;
				cMath::Vector3ClampToLength(vTorque, fMaxRelativeCharTorque);
								
				vTotalTorque += vTorque;
			}
			////////////////////////////////////
			//Rotate around all axes
			else
			{
				//Set up pid
				mBodyAngularPid.p = 20.0f;
				mBodyAngularPid.i = 0.0f;
				mBodyAngularPid.d = 2.5f;
				
				cVector3f vCurrentRotation = cMath::MatrixToEulerAngles(mpConnectedBody->GetLocalMatrix().GetRotation(), eEulerRotationOrder_XYZ);
				cVector3f vError = mvConnectionBodyStartRotation - vCurrentRotation;

				cVector3f vTorque = mBodyAngularPid.Output(vError, afTimeStep);
				cMath::Vector3ClampToLength(vTorque, fMaxRelativeCharTorque);
				
				vTotalTorque += vTorque;
			}
		}

		//////////////////////////////////
		// Apply forces
		const float fMaxTorque = 10.0f;
		const float fMaxForce = 100.0f;

		//Make sure that the force is not too large before multiplying with mass
		cMath::Vector3ClampToLength(vTotalForce,fMaxForce);
		cMath::Vector3ClampToLength(vTotalForce,fMaxTotalCharForce);
				
		vTotalForce = vTotalForce * mpConnectedBody->GetMass();
		mpConnectedBody->AddForce(vTotalForce);
        
		//Make sure that the force is not too large before multiplying with intertia and mass
		cMath::Vector3ClampToLength(vTotalTorque, fMaxTorque);

		vTotalTorque =  cMath::MatrixMul(mpConnectedBody->GetInertiaMatrix(),vTotalTorque);
		mpConnectedBody->AddTorque(vTotalTorque);
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateCharacterConnection(float afTimeStep)
	{
		//If no connection, do nothing
		if(mpConnectedBody == NULL) return;

		//Check if character is affected.
		if(mbConnectionAffectChar==false) return;

		//////////////////////////////////////
		// Align rotation (Only works for yaw for the time being)
		if(mbConnectionAlignCharacterRotation)
		{
			//-1 since normal is toward char otherwise.
			cVector3f vWantedForward = cMath::MatrixMul(mpConnectedBody->GetWorldMatrix().GetRotation(), mvConnectionCharLocalForward);

			cVector3f vWantedForwardXZ = cMath::Vector3Normalize(cVector3f(vWantedForward.x, 0, vWantedForward.z));	
			
			//Get the angle from character to the body connection point.
			float fWantedYaw = cMath::Vector3Angle(cVector3f(0,0,-1), vWantedForwardXZ);
			if(vWantedForwardXZ.x >0) fWantedYaw = -fWantedYaw;

			//Offset that angle to get yaw
			mfYaw = fWantedYaw;

			UpdateMoveMatrix();
		}
		
		//////////////////////////////////////
		// Align character to connection

		//Set up PID (move this SetConnectedBody?)
		mCharConnectionPointPid.p = 100.0f;
		mCharConnectionPointPid.i = 0.0f;
		mCharConnectionPointPid.d = 0.0f;

		//Get the body position of the connection positions
		cVector3f vBodyConnection = GetBodyConnectionPos();
		cVector3f vCharConnection = GetCharConnectionPos();

		cVector3f vError = vBodyConnection - vCharConnection;
		cVector3f vForce = mCharConnectionPointPid.Output(vError, afTimeStep);

        AddForce(vForce * GetMass());
	}

	//-----------------------------------------------------------------------


	cVector3f iCharacterBody::UpdatePostionFromCharSpeed(float afTimeStep)
	{
		/////////////////////////////////////////////////////
		// A special mul if the player is moving diagonally
        float fDiagMul = 1.0f;
        if(mbDisableDiagSpeedBoost && mbMoving[eCharDir_Forward] && mbMoving[eCharDir_Right])
		{
			fDiagMul = 0.7071f; //=~ 1 / sqrt(2)
		}

		/////////////////////////////////////////////////////
		// If the character is not moving in a direction and not on ground, apply deacceleration.
		mbMovedLastUpdate = false;
		for(int i=0; i < eCharDir_LastEnum; i++)
		{
			////////////////////////////
			// If not moving, then lower the speed to zero
			if(mbMoving[i]==false)
			{
				//see if we want to lower speed
				if(IsOnGround() || mbDeaccelerateMoveSpeedInAir)
				{
					if(mfMoveSpeed[i]>0)
					{
						mfMoveSpeed[i] -= mfMoveDeacc[i] * afTimeStep;
						if(mfMoveSpeed[i]<0)mfMoveSpeed[i] =0;
					}
					else
					{
						mfMoveSpeed[i] += mfMoveDeacc[i] * afTimeStep;
						if(mfMoveSpeed[i]>0)mfMoveSpeed[i] =0;
					}
				}
			}
			////////////////////////////
			// If moving, then lower/increase the speed so it is not over/under any max/min speed
			else
			{
				mbMovedLastUpdate = true;

				float fMaxPosSpeed = mfMaxPosMoveSpeed[i] * fDiagMul;
				float fMaxNegSpeed = mfMaxNegMoveSpeed[i] * fDiagMul;

				//see if we want to lower speed
				if(IsOnGround() || mbDeaccelerateMoveSpeedInAir)
				{
					if(mfMoveSpeed[i] > fMaxPosSpeed)
					{
						mfMoveSpeed[i] -= mfMoveDeacc[i] * afTimeStep;
						if(mfMoveSpeed[i] < fMaxPosSpeed) mfMoveSpeed[i] = fMaxPosSpeed;
					}
					else if(mfMoveSpeed[i] < fMaxNegSpeed)
					{
						mfMoveSpeed[i] += mfMoveDeacc[i] * afTimeStep;
						if(mfMoveSpeed[i] > fMaxNegSpeed) mfMoveSpeed[i] = fMaxNegSpeed;
					}
				}

				mbMoving[i] = false;
			}
		}

		/////////////////////////////////////////
		//Update speed and position

		//Update speed
		for(int i=0; i < eCharDir_LastEnum; i++)
		{
			float fMaxPosSpeed = mfMaxPosMoveSpeed[i] * fDiagMul;
			float fMaxNegSpeed = mfMaxNegMoveSpeed[i] * fDiagMul;

			//If on ground and moving in opposite direction of the current speed, add a acc multiplier.
			float fAccMul=1.0f;
			if(IsOnGround() && ( (mfCurrentMoveAcc[i] > 0 && mfMoveSpeed[i]<0) || (mfCurrentMoveAcc[i] < 0 && mfMoveSpeed[i]>0) ) )
			{
				fAccMul = mfMoveOppositeDirAccMul[i];
			}

			float fSpeedAdd = mfCurrentMoveAcc[i] * mfMoveAcc[i] * fAccMul * afTimeStep;
			
			//Only update speed if max speed had not been reached.
			//This way the current speed can be over max speed (if the player first runs and then goes over to walk)
			if(fSpeedAdd > 0 && mfMoveSpeed[i] < fMaxPosSpeed)
			{
				mfMoveSpeed[i] += fSpeedAdd;
				if(mfMoveSpeed[i] > fMaxPosSpeed) mfMoveSpeed[i] = fMaxPosSpeed;
			}
			else if(fSpeedAdd < 0 && mfMoveSpeed[i] > fMaxNegSpeed)
			{
				mfMoveSpeed[i] += fSpeedAdd;
				if(mfMoveSpeed[i] < fMaxNegSpeed) mfMoveSpeed[i] = fMaxNegSpeed;
			}
			


			mfCurrentMoveAcc[i] = 0;	//Reset
		}
		
		//Init variables
		cVector3f vPosAdd =0;

		//Movement velocity
		vPosAdd += mvForward * mfMoveSpeed[eCharDir_Forward] * afTimeStep;
		vPosAdd += mvRight * mfMoveSpeed[eCharDir_Right] * afTimeStep;

		//TODO: This does not work when one lowers the speed (since it also caps max speed!). So leave in strafe bug / trick for now!
		//Make sure speed is not greater than max forward.
		/*float fMaxStep = mfMoveSpeed[eCharDir_Forward] >=0 ? mfMaxPosMoveSpeed[eCharDir_Forward] : mfMaxNegMoveSpeed[eCharDir_Forward];
		
		fMaxStep *= afTimeStep;
		fMaxStep = cMath::Abs(fMaxStep);

		float fStepLength = vPosAdd.Length();
		if(fStepLength > fMaxStep)
		{
			vPosAdd = vPosAdd / fStepLength;
			vPosAdd = vPosAdd * fMaxStep;
		}*/

		//Finally update position
		return vPosAdd;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::AlignPosAddAccordingToGroundNormal(cVector3f &avPosAdd)
	{
		//Do not do any alignment if body is not on ground or on its way up.
		if(IsOnGround()==false || mvVelocity.y > 0) return;

		if(mvLastGroundNormal.SqrLength() < 0.01f) return;
        
		cVector3f vBodyForward = cMath::Vector3Normalize(avPosAdd);

		cVector3f vRight = cMath::Vector3Cross(vBodyForward, mvLastGroundNormal);
		cVector3f vForward = cMath::Vector3Cross(mvLastGroundNormal, vRight);

		avPosAdd = vForward * avPosAdd.Length();
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::CheckMoveCollision(const cVector3f &avPosAdd, float afTimeStep)
	{
		//NOTE: We do not skip this test since we always need at least one test for collision, even if the
		//		character never moved.
		
		//Test if the callback for pushing should be used.
		iPhysicsWorldCollisionCallback *pCallback = NULL;
		if(avPosAdd != 0)	pCallback = mpCollidePushCallback;

		//Get new position
		mvPosition += avPosAdd;
		
		//Check collision
		cVector3f vPushBack(0);
		bool bCollide = CheckCollision(&vPushBack, mvPosition, pCallback);

		//If collision, push back
		if(bCollide)
		{
			mvPosition += vPushBack;
		
			////////////////////////
			//If the body is in the air, then deduct the speed when hitting an obstacle.
			if(IsOnGround()==false && vPushBack.SqrLength() != 0)
			{
				cVector3f vNormal = cMath::Vector3Normalize(vPushBack);
				cVector3f vForwardVel = mvForward * mfMoveSpeed[eCharDir_Forward];
				cVector3f vRightVel = mvRight * mfMoveSpeed[eCharDir_Right];
				
				/////////////////////////////////////
				//Forward velocity reflection
				//Make sure that new velocity points in the right direction and that it is not too large!
				if(mfMoveSpeed[eCharDir_Forward] != 0)
				{
					vForwardVel = vForwardVel - vNormal * cMath::Vector3Dot(vNormal, vForwardVel);
					float fForwardSpeed = vForwardVel.Length();
					if(mfMoveSpeed[eCharDir_Forward] > 0)
						if(mfMoveSpeed[eCharDir_Forward] > fForwardSpeed) mfMoveSpeed[eCharDir_Forward] = fForwardSpeed;
					else
						if(mfMoveSpeed[eCharDir_Forward] < fForwardSpeed) mfMoveSpeed[eCharDir_Forward] = -fForwardSpeed;
				}
				
				/////////////////////////////////////	
				//Right velocity reflection
				//Make sure that new velocity points in the right direction and that it is not too large!
				if(mfMoveSpeed[eCharDir_Right] != 0)
				{
					vRightVel = vRightVel - vNormal * cMath::Vector3Dot(vNormal, vRightVel);
					float fRightSpeed = vRightVel.Length();
					if(mfMoveSpeed[eCharDir_Right] > 0)
						if(mfMoveSpeed[eCharDir_Right] > fRightSpeed) mfMoveSpeed[eCharDir_Right] = fRightSpeed;
					else
						if(mfMoveSpeed[eCharDir_Right] < fRightSpeed) mfMoveSpeed[eCharDir_Right] = -fRightSpeed;
				}
			}
		}


		/////////////////////////////
		//Always check climbing!
		CheckStepClimbing(avPosAdd,afTimeStep);
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::CheckStepClimbing(const cVector3f &avPosAdd, float afTimeStep)
	{
		if(mfCheckStepClimbCount > 0) return;
		if(avPosAdd.SqrLength() < kEpsilonf) return;
		
		//Send a ray in front of the player.
		float fRadius = mpCurrentShape->GetRadius();
		float fPosAddLength = avPosAdd.Length();
		float fForwadAdd = cMath::Max(fPosAddLength, 0.05f);
		
		//The direction of the movement.
		cVector3f vMoveDir = cMath::Vector3Normalize(avPosAdd);

		cVector3f vStepAdd[3];
		cVector3f vStart[3];
		cVector3f vEnd[3];
		bool bCollided[3];
		float fMinDist[3];
		int lNumRays= mbAccurateClimbing ? 3 : 1;
		
		/////////////////////////////////
		//Calculate the different movements
		vStepAdd[0] = vMoveDir*(fRadius+fForwadAdd);

		if(mbAccurateClimbing)
		{
			cVector3f vRightDir = cMath::MatrixMul(cMath::MatrixRotateY(kPi4f),vMoveDir);
			vStepAdd[1] = (vRightDir*fRadius)+(vMoveDir*fForwadAdd);

			cVector3f vLeftDir = cMath::MatrixMul(cMath::MatrixRotateY(-kPi4f),vMoveDir);
			vStepAdd[2] = (vLeftDir*fRadius)+(vMoveDir*fForwadAdd);
		}

		/////////////////////////////////
		//Shot the rays
		// Shoot from middle of body, making max climb height no higher than that!
		for(int i=0; i< lNumRays; ++i)
		{
			vStart[i] = mvPosition+ vStepAdd[i];//mvPosition + cVector3f(0,mvSize.y/2,0)+ vStepAdd[i];
			vEnd[i] = vStart[i] - cVector3f(0,mvSize.y/2.0f,0);//cVector3f(0,mvSize.y,0);

			bCollided[i] = CheckRayIntersection(vStart[i],vEnd[i],&fMinDist[i], NULL);
		}
		

		bool bFirmlyOnGround = mlOnGroundCount > mlMaxOnGroundCount-4;
		float fMaxHeight = (bFirmlyOnGround || mbClimbing) ? mfMaxStepHeight : mfMaxStepHeightInAir;

		/////////////////////////////////
		// Check if the step can be climbed.
		for(int i=0; i< lNumRays; ++i)
		{
			if(bCollided[i]==false) continue;
			
			float fHeight = mvSize.y/2.0f - fMinDist[i];

			if(fHeight <= fMaxHeight && fHeight>0.025f)
			{
				//Check if there is any collision on the new pos
				cVector3f vStepPos = mvPosition + cVector3f(0,fHeight+mfClimbHeightAdd,0)+ (vMoveDir*fForwadAdd*mfClimbForwardMul);
				
				if(CheckCharacterFits(vStepPos))
				{
					//Climb the stair.
					mvPosition.y += mfStepClimbSpeed * afTimeStep;
					mbClimbing = true;
					break;
				}
			}
		}

		mfCheckStepClimbCount = mfCheckStepClimbInterval;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateStepClimbing(float afTimeStep)
	{
		if(mbClimbing)
		{
			mfCheckStepClimbCount =0;	//If climbing, we always check
			mlOnGroundCount = mlMaxOnGroundCount;
		}
		else
		{
			mfCheckStepClimbCount -= afTimeStep;
		}

		mbClimbing = false;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateForces(float afTimeStep)
	{
		
		////////////////////
		// Gravity
		if(mbGravityActive && mbClimbing==false)
		{
			if(mbCustomGravity)	mvVelocity += mvCustomGravity * afTimeStep;
			else				mvVelocity += mpWorld->GetGravity() * afTimeStep;

			float fLength = mvVelocity.Length();
			if(fLength> mfMaxGravitySpeed)
			{
				mvVelocity = (mvVelocity /fLength) * mfMaxGravitySpeed;
			}
		}
        

		//////////////////
		// Normal force
		mvVelocity += mvForce * (afTimeStep * (1.0f/mfMass));
		mvForce =0;
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateFriction(float afTimeStep)
	{
		////////////////////
		// Friction (only do for x and z? Really good idea? I think so!)
		float fFriction = IsOnGround() ? mfGroundFriction : mfAirFriction;

		cVector3f vVelXZ(mvVelocity.x, 0,mvVelocity.z);
		float fSpeed = vVelXZ.Length();
		vVelXZ.Normalize();

		fSpeed -= fFriction * afTimeStep;
		if(fSpeed<0) fSpeed=0;

		mvVelocity.x = vVelXZ.x * fSpeed;
		mvVelocity.z = vVelXZ.z * fSpeed;

		////////////////////
		// Attachment friction
		fSpeed = mvGravityAttachmentVelocity.Length();
		if(fSpeed >0)
		{
			cVector3f vDir = mvGravityAttachmentVelocity / fSpeed;

			fSpeed -= mfAirFriction * afTimeStep;
			if(fSpeed<0) fSpeed =0;

			mvGravityAttachmentVelocity = vDir * fSpeed;
		}
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::CheckForceCollision(float afTimeStep)
	{
		//TODO: Will have to do very specific stuff if custom gravity is no restricted to y axis.
		//		That is true for a lot of the code...

		////////////////////////////
		// Get position add
		cVector3f vPosAdd = mvVelocity * afTimeStep;
		
		//Extra velocity after leaving a moving gravity attachment
		vPosAdd += mvGravityAttachmentVelocity * afTimeStep;

		cVector3f vNewVelocity = 0;
		
		////////////////////////////
		// XZ collision
		if(vPosAdd.x != 0 || vPosAdd.z != 0)
		{
			mvPosition.x += vPosAdd.x;
			mvPosition.z += vPosAdd.z;
			
			cVector3f vPushBack(0);
			bool bCollide = CheckCollision(	&vPushBack, mvPosition, mpCollidePushCallback);

			if(bCollide && cMath::Vector3Abs(vPushBack) != cVector3f(0))
			{
				mvPosition += vPushBack;
				
				//Reflect the velocity (with no "bounce")
				cVector3f vNormal = cMath::Vector3Normalize(vPushBack);
				cVector3f vXZVel = cVector3f(mvVelocity.x, 0, mvVelocity.z);

				cVector3f vNewVelAdd = vXZVel - vNormal * cMath::Vector3Dot(vNormal, vXZVel);
				vNewVelocity += vNewVelAdd;
				
				//Reset attachment velocity (correct?)
				mvGravityAttachmentVelocity.x =0;
				mvGravityAttachmentVelocity.z =0;
			}
			else
			{
				vNewVelocity.x += mvVelocity.x;
				vNewVelocity.z += mvVelocity.z;
			}
			
		}

		////////////////////////////
		// Y collision
		if(vPosAdd.y != 0)
		{
			cVector3f vOldPos = mvPosition;

			mvPosition.y += vPosAdd.y;
			
			cVector3f vPushBack(0);
			bool bCollide = CheckCollision(	&vPushBack, mvPosition, mpCollideGravityCallback);
			
			//////////////////////////
			// Check if there was any collision
			if(bCollide && cMath::Vector3Abs(vPushBack) != cVector3f(0))
			{
				if(mpCallback) mpCallback->OnHitGround(this, mvVelocity);

				//Set groundnormal, and make sure it is not too steep!
				mvLastGroundNormal = cMath::Vector3Normalize(vPushBack);
				if(mvLastGroundNormal.y <= 0.2f)
				{
					mvLastGroundNormal = cVector3f(0,1,0);
				}

				//Reset attachment velocity (correct?)
				mvGravityAttachmentVelocity.y =0;
				
				//Check if the push back is mostly up and velocity points down, else decrement, ie signal that the player is not on ground.
				if(mvVelocity.y && cMath::Vector3Normalize(vPushBack).y >= 0.001f)
				{
					mlOnGroundCount = mlMaxOnGroundCount;
				}
				else
				{
					mlOnGroundCount--;
					if(mlOnGroundCount<0) mlOnGroundCount =0;
				}
				
				
				mvPosition += vPushBack;
					
				//////////////
				//Refection (and no bounce)
				cVector3f vNormal = cMath::Vector3Normalize(vPushBack);
				
				//If the slope is less than a certain angle, then we treat it like a flat (according to gravity dir) surface.
				// >= because cos gets large as angle gets closer to 0.
				cVector3f vInvGravityNormal = mbCustomGravity ? cMath::Vector3Normalize(mvCustomGravity*-1) : cVector3f(0,1,0);
				if(cMath::Vector3Dot(vNormal, vInvGravityNormal) >= mfMaxNoSlideSlopeAngleCos)
				{
					vNormal = vInvGravityNormal;
				}
				
				cVector3f vYVel = cVector3f(0,mvVelocity.y, 0);

				//float fDot = cMath::Vector3Dot(vNormal, vInvGravityNormal);
				//float fAngle = cMath::ToDeg(acos(fDot));
				//Log("normal: %s, vel: %s. Slope: dot: %f, angle: %f, maxcos: %f\n", vNormal.ToString().c_str(), vYVel.ToString().c_str(),fDot,fAngle,mfMaxNoSlideSlopeAngleCos);
				
				cVector3f vNewVelAdd = vYVel - vNormal * cMath::Vector3Dot(vNormal, vYVel);
				vNewVelocity += vNewVelAdd;

				//If the new velocity is zero, then we set back to old positon (to prevent slope sliding)
				if(fabs(vNewVelAdd.Length()) < 0.0001f )
				{
					mvPosition = vOldPos;
				}
			}
			else
			{
				//Decerement the ground count,
				mlOnGroundCount--;
				if(mlOnGroundCount<0) mlOnGroundCount =0;

				//Calculate the new velocity
				vNewVelocity.y += mvVelocity.y;

				///////////////////////////
				//If no collision and on ground and not climbing then cast ray to get ground normal
				if(mlOnGroundCount > 0 && mbClimbing==false)
				{
					mpRayCallback->Clear();
					cVector3f vStart = GetFeetPosition() + cVector3f(0,0.001f,0);
					cVector3f vEnd = vStart - cVector3f(0,mvSize.x*2.0001f,0);
					mpWorld->CastRay(mpRayCallback,vStart,vEnd,true,true,false);

					//Check if there was a collision and also check so normal is not too steep
					if(mpRayCallback->mbCollide && mpRayCallback->mvNormal.y > 0.2f)
					{
						mvLastGroundNormal = mpRayCallback->mvNormal;
					}
					else
					{
						mvLastGroundNormal = cVector3f(0,1,0);
					}
				}
			}

			

		}

		mvVelocity = vNewVelocity;
	}

	//-----------------------------------------------------------------------

	
	void iCharacterBody::UpdateForcePushing(float afTimeStep)
	{
		cCollideData collideData;
		collideData.SetMaxSize(32);

		cCollideData extraCollideData;
		extraCollideData.SetMaxSize(1);

		cBoundingVolume boundingVolume = mpCurrentShape->GetBoundingVolume();
		boundingVolume.SetPosition(mvPosition);

		cMatrixf mtxTransform = cMath::MatrixTranslate(mvPosition);
		
		//////////////////////////////
        // Iterate bodies		
		mvTempBodies.resize(0);
		mpWorld->GetBodiesInBV(&boundingVolume, &mvTempBodies);

		for(size_t i=0; i<mvTempBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvTempBodies[i];

			if(pBody->IsActive()==false) continue;
            if(pBody->GetMass() == 0) continue;
			if(pBody->IsCharacter() && mbCollideCharacter==false) continue;
			if(pBody->GetCollideCharacter()==false) continue;
						
			if(cMath::CheckBVIntersection(boundingVolume,*pBody->GetBoundingVolume())==false)
			{
				continue;
			}

			collideData.SetMaxSize(32);
			bool bRet = mpWorld->CheckShapeCollision(pBody->GetShape(),pBody->GetLocalMatrix(),
													mpCurrentShape,mtxTransform,
													collideData, 32, true);
			
			if(bRet)
			{
				float fNumOfPoints = (float)collideData.mlNumOfPoints;
				
				float fMaxForce = mfMaxContactForcePerMassUnit * pBody->GetMass() / fNumOfPoints;

				float fC = mfConstantContactForceMul / fNumOfPoints * mfMass;
				float fV = mfVelocityContactForceMul / fNumOfPoints * mfMass;

				////////////////////////
				//Iterate contact points and add a spring force.
				for(int i=0; i< collideData.mlNumOfPoints; i++)
				{
					cCollidePoint &point = collideData.mvContactPoints[i];
					
					cVector3f vBodyVelocityAtPos = pBody->GetVelocityAtPosition(point.mvPoint);
					
					cVector3f vForce = point.mvNormal * point.mfDepth * fC - 
											  point.mvNormal * cMath::Vector3Dot(vBodyVelocityAtPos,point.mvNormal)  * fV;
					
					float fLength = vForce.Length();
					if(fLength > fMaxForce)	
					{
						vForce = (vForce / fLength) * fMaxForce;
					}
					
					pBody->AddForceAtPosition(vForce, point.mvPoint);
				}
			}
    	}
	}

	//-----------------------------------------------------------------------


	void iCharacterBody::UpdateCamera()
	{
		if(mpCamera==NULL) return;

		//////////////////
		//Get the camera pos add.
		cVector3f vAdd=0;
		vAdd.y += mvCameraPosAdd.y;
		vAdd += mvRight * mvCameraPosAdd.x;
		vAdd += mvForward * mvCameraPosAdd.z;

		float fMainHeight = mvShapes[0]->GetSize().y;

		///////////////////////////////
		//Do NOT do any smoothing
		if(mlCameraSmoothPosNum <=0)
		{
			cVector3f vPos = mvPosition + cVector3f(0,fMainHeight - mvSize.y/2.0f,0);
			mpCamera->SetPosition(vPos + vAdd);
		}
		///////////////////////////////
		//Smooth the camera position
		else
		{
			//////////////////////////////
			//Add the newest position.
			cVector3f vHeadPos = mvPosition + cVector3f(0,fMainHeight - mvSize.y/2.0f,0);
			mlstCameraPos.push_back(vHeadPos);
			
			//If to too large remove the oldest.
			if((int)mlstCameraPos.size() > mlCameraSmoothPosNum)
			{
				mlstCameraPos.erase(mlstCameraPos.begin());
			}

			float fPositionNum = (float)mlstCameraPos.size();

			/////////////////////////////////////////
			//Add all positions and devide by the number of em.
			//that way we get the average
			cVector3f vTotalPos(0,0,0);
			tVector3fListIt it=mlstCameraPos.begin();
			for(; it!= mlstCameraPos.end(); ++it)
			{
				vTotalPos += *it;
			}

			cVector3f vPos = vTotalPos / fPositionNum;
			
			//////////////////////////////////
			// Set up new position
			cVector3f vFirstSize = mvShapes[0]->GetSize();

			//cVector3f vHeadPos = (vPos - cVector3f(0,mvSize.y/2.0f,0)) + cVector3f(0,vFirstSize.y,0);
			
			mpCamera->SetPosition(vPos + vAdd);
		}

		//No need to smooth this yaw.
		mpCamera->SetYaw(mfYaw);
	}

	//-----------------------------------------------------------------------


	void iCharacterBody::UpdateEntity()
	{
		if(mpEntity==NULL) return;

		///////////////////////////////
		//Y Position Smoothing
		float fSmoothY = mvPosition.y;
		if(mbEntitySmoothYPos)
		{
			fSmoothY =0.0f;
			mlstEntityYPositions.push_back(mvPosition.y);
			if((int)mlstEntityYPositions.size() > mlEntitySmoothYPosNum) 
				mlstEntityYPositions.pop_front();
			
			//Iterate values and get average
			for(tFloatListIt it = mlstEntityYPositions.begin(); it != mlstEntityYPositions.end(); ++it)
			{
				fSmoothY += *it;
			}
			fSmoothY = fSmoothY / (float)mlstEntityYPositions.size();
		}

		///////////////////////////////
		//Do NOT do any smoothing
		if(mlEntitySmoothPosNum <=0)
		{
			cMatrixf mtxEntity = cMath::MatrixRotateY(mfYaw);
			mtxEntity.SetTranslation(cVector3f(mvPosition.x, fSmoothY, mvPosition.z));
			
			mpEntity->SetMatrix(cMath::MatrixMul(mtxEntity,m_mtxEntityOffset));
		}
		//Smooth the entity position
		else
		{
			/////////////////////////////////
			//Add the newest position.
			mlstEntityPos.push_back(mvPosition);
			
			//If to too large remove the oldest.
			if((int)mlstEntityPos.size() > mlEntitySmoothPosNum)
			{
				mlstEntityPos.erase(mlstEntityPos.begin());
			}

			float fPositionNum = (float)mlstEntityPos.size();

			//////////////////////////////////
			//Add all positions and devide by the number of em.
			//that way we get the average
			cVector3f vTotalPos(0,0,0);
			tVector3fListIt it=mlstEntityPos.begin();
			for(; it!= mlstEntityPos.end(); ++it)
			{
				vTotalPos += *it;
			}

			cVector3f vPos = vTotalPos / fPositionNum;
			if(mbEntitySmoothYPos) vPos.y = fSmoothY;
		
			//////////////////////////////////
			// Set up new matrix
			cMatrixf mtxEntity = m_mtxMove;
			mtxEntity.SetTranslation(0);
			
			mtxEntity = cMath::MatrixMul(m_mtxEntityPostOffset,mtxEntity);
			mtxEntity.SetTranslation(mtxEntity.GetTranslation() + vPos);
			
			mtxEntity = cMath::MatrixMul(mtxEntity,m_mtxEntityOffset);
			
			mpEntity->SetMatrix(mtxEntity);
		}
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::UpdateBody()
	{
		mpCurrentBody->SetPosition(mvPosition);
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::ClearGravityAttachment()
	{
		mpGravityLastAttachedBody = mpGravityAttachedBody;
		mpGravityAttachedBody = NULL;
 		mlGravityAttachmentContacts = 0;
	}

	//-----------------------------------------------------------------------


	void iCharacterBody::UpdateGravityAttachment()
	{
		if(mpGravityAttachedBody==NULL)
		{
			////////////////////////
			//If the character was just attached, do somethings
			if(mpGravityLastAttachedBody != NULL)
			{
				mvGravityAttachmentVelocity = mvGravityAttachmentBodyVelocity;
			}

			return;
		}

		mvGravityAttachmentVelocity =0;

		//Log("Attched body: %s\n", mpGravityAttachedBody->GetName().c_str());

		/////////////////////////////
		// If the character was not just attached, update the position according to attachment body
		if(mpGravityLastAttachedBody == mpGravityAttachedBody)
		{
			///////////
			// Transform the current position according to entire matrix difference since previous matrix
			if(mpGravityAttachedBody->GetGravityAttachmentRotation())
			{
				//Transform the position
				cMatrixf mtxInvPrevWorld = cMath::MatrixInverse(m_mtxGravityAttachedPrevMatrix);
				cMatrixf mtxDelta = cMath::MatrixMul(mpGravityAttachedBody->GetWorldMatrix(),mtxInvPrevWorld);
				
				mvPosition = cMath::MatrixMul(mtxDelta, mvPosition);

				//Rotate the body (only along Y)
				if(mbRotateYawWhenGravityAttached)
				{
					cMatrixf mtxRotDelta = cMath::MatrixMul(mpGravityAttachedBody->GetWorldMatrix().GetRotation(), 
																mtxInvPrevWorld.GetRotation());

					cVector3f vForward = cMath::MatrixMul(mtxRotDelta, cVector3f(0,0,-1));
					vForward.y =0; //only get x,z part.
					float fLength = vForward.Length();
					if(fLength > 0.0001f)
					{

						vForward = vForward / fLength;	//Normalize
	                    
						float fAngle = acos( -vForward.z );	//simply forward dot (0,0,-1)
						if(vForward > 0) fAngle = -fAngle;	//Determine direction.
						
						mfYaw += fAngle;
						UpdateMoveMatrix();
					}
				}
				
				
			}
			///////////
			// Transform only using positon delta
			else
			{
				mvPosition +=	mpGravityAttachedBody->GetWorldMatrix().GetTranslation() - 
								m_mtxGravityAttachedPrevMatrix.GetTranslation();
			}

			
		}

		//mvGravityAttachmentBodyVelocity = mpGravityAttachedBody->GetLinearVelocity();
		if(mpGravityAttachedBody->GetGravityAttachmentVelocity())
		{
			switch(mpGravityAttachedBody->GetGravityAttachmentVelocityAxes())
			{
			case eVelocityAxes_XZ:
				mvGravityAttachmentBodyVelocity.x = mpGravityAttachedBody->GetVelocityAtPosition(GetFeetPosition()).x;
				mvGravityAttachmentBodyVelocity.y = 0;
				mvGravityAttachmentBodyVelocity.z = mpGravityAttachedBody->GetVelocityAtPosition(GetFeetPosition()).z;
				break;
			
			case eVelocityAxes_Y:
				mvGravityAttachmentBodyVelocity.x = 0;
				mvGravityAttachmentBodyVelocity.y = mpGravityAttachedBody->GetVelocityAtPosition(GetFeetPosition()).y;
				mvGravityAttachmentBodyVelocity.z = 0;
				break;
			
			case eVelocityAxes_XYZ:
				mvGravityAttachmentBodyVelocity = mpGravityAttachedBody->GetVelocityAtPosition(GetFeetPosition());
				break;
			}
			
			
		}
		else
			mvGravityAttachmentBodyVelocity = 0;

		m_mtxGravityAttachedPrevMatrix = mpGravityAttachedBody->GetWorldMatrix();
	}

	//-----------------------------------------------------------------------

	void iCharacterBody::EnableBodiesAroundCharacter()
	{
		if(mvPosition == mvLastPosition) return;

		//Make size a little larger so that bodies get enabled too.
		cBoundingVolume tempBv;
		tempBv.SetSize(mvSize * 1.1f);	
		tempBv.SetPosition(mvPosition);

        mpWorld->EnableBodiesInBV(&tempBv, true);
	}
	
	//-----------------------------------------------------------------------

	bool iCharacterBody::CheckCollision(cVector3f *apPushBackVector, const cVector3f& avPos, iPhysicsWorldCollisionCallback *apCallback,
										int alShapeIdx)
	{
		if(alShapeIdx <0) alShapeIdx = mlCurrentShapeIdx;
		iCollideShape *pShape = mvShapes[alShapeIdx];

		return mpWorld->CheckShapeWorldCollision(apPushBackVector, pShape, cMath::MatrixTranslate(avPos),
												mpCurrentBody, false, true, 
												apCallback, true,mlMinBodyPushStrength, 
												mlCollideFlags, false);
	}
	
	//-----------------------------------------------------------------------

}
