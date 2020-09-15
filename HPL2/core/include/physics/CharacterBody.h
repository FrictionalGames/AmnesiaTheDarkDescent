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

#ifndef HPL_CHARACTER_BODY_H
#define HPL_CHARACTER_BODY_H

#include "math/MathTypes.h"
#include "physics/PhysicsWorld.h"
#include "math/PidController.h"

namespace hpl {

	class iPhysicsWorld;
	class iCollideShape;
	class iPhysicsBody;
	class cCamera;
	class iCharacterBody;
	class iEntity3D;

	//------------------------------------------------
	
	class iCharacterBodyCallback
	{
	public:
		virtual void OnGravityCollide(iCharacterBody *apCharBody, iPhysicsBody *apBody, cCollideData *apCollideData)=0;
		virtual void OnHitGround(iCharacterBody *apCharBody,const cVector3f &avVel)=0;
	};

	//------------------------------------------------

	enum eCharDir
	{
		eCharDir_Forward=0,
		eCharDir_Right=1,
		eCharDir_LastEnum=2
	};
	
	//------------------------------------------------

	class cCharacterBodyCollideGravity : public iPhysicsWorldCollisionCallback
	{
	public:
		cCharacterBodyCollideGravity(iCharacterBody *apCharBody);

		void OnCollision(iPhysicsBody *apBody, cCollideData *apCollideData);
		
		iCharacterBody *mpCharBody;
	};

	//------------------------------------------------

	class cCharacterBodyCollidePush : public iPhysicsWorldCollisionCallback
	{
	public:
		cCharacterBodyCollidePush(iCharacterBody *apCharBody);

		void OnCollision(iPhysicsBody *apBody, cCollideData *apCollideData);

		iCharacterBody *mpCharBody;
	};
	
	//------------------------------------------------

	class cCharacterBodyRay : public iPhysicsRayCallback
	{
	public:
		cCharacterBodyRay();

		void Clear();
		bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

		float mfMinDist;
		cVector3f mvNormal;
		bool mbCollide;
	};

	//------------------------------------------------
	

	class iCharacterBody
	{
	friend class cCharacterBodyCollideGravity;
	public:
		iCharacterBody(const tString &asName, iPhysicsWorld *apWorld, const cVector3f avSize);
		virtual ~iCharacterBody();

		const tString& GetName(){ return msName;}

		///////////////////////////////////////
		//Actions
		void SetForce(const cVector3f &avForce);
		void AddForce(const cVector3f &avForce);
		cVector3f GetForce();

		void Move(eCharDir aDir, float afMul);
		void StopMovement();

		void ConnectToBody(	iPhysicsBody *apBody, const cVector3f& avConnectionPos, tPhysicsConnectionFlag alFlags);
		void DisconnectBody();

		int AddExtraSize(const cVector3f& avSize);
		void SetActiveSize(int alNum);
		int GetActiveSize(){ return mlCurrentShapeIdx; }

		void Update(float afTimeStep);

		///////////////////////////////////////
		//Helpers

		bool CheckCharacterFits(const cVector3f &avPosition, bool abFeetPosition=false, int alSizeIdx = -1, cVector3f *apPushBackVec=NULL, float afEpsilon = 0.0f);
		bool CheckRayIntersection(const cVector3f &avStart, const cVector3f &avEnd, float *apDistance, cVector3f *apNormalVec);

		///////////////////////////////////////
		// General Properties

		void SetActive(bool abX);
		bool IsActive(){ return mbActive;}


		///////////////////////////////////////
		// Physical Properties

		float GetMass();
		void SetMass(float afMass);
		
		cVector3f GetSize();
		
		iCollideShape *GetCurrentShape();
		iCollideShape *GetShape(int alIdx);
		iPhysicsBody* GetCurrentBody();

		void SetCollideCharacter(bool abX);
		bool GetCollideCharacter(){ return mbCollideCharacter;}

		void SetTestCollision(bool abX);
		bool GetTestCollision(){return mbTestCollision;}

		void SetMaxPositiveMoveSpeed(eCharDir aDir, float afX);
		float GetMaxPositiveMoveSpeed(eCharDir aDir);
		void SetMaxNegativeMoveSpeed(eCharDir aDir, float afX);
		float GetMaxNegativeMoveSpeed(eCharDir aDir);

		void SetMoveSpeed(eCharDir aDir, float afX);
		float GetMoveSpeed(eCharDir aDir);
		void SetMoveAcc(eCharDir aDir, float afX);
		float GetMoveAcc(eCharDir aDir);
		void SetMoveDeacc(eCharDir aDir, float afX);
		float GetMoveDeacc(eCharDir aDir);

		void SetMoveOppositeDirAccMul(eCharDir aDir, float afX){mfMoveOppositeDirAccMul[aDir] = afX;}
		float GetMoveOppositeDirAccMul(eCharDir aDir){ return mfMoveOppositeDirAccMul[aDir];}
		
		
		void SetDeaccelerateMoveSpeedInAir(bool abX){ mbDeaccelerateMoveSpeedInAir = abX;}
		bool GetDeaccelerateMoveSpeedInAir(){ return mbDeaccelerateMoveSpeedInAir;}

		cVector3f GetVelocity(float afFrameTime);

		void SetPosition(const cVector3f& avPos, bool abSmooth=false);
		const cVector3f& GetPosition();
		const cVector3f& GetLastPosition();
		void SetFeetPosition(const cVector3f& avPos, bool abSmooth=false);
		cVector3f GetFeetPosition();

		void SetYaw(float afX);
		void AddYaw(float afX);
		float GetYaw();
		void SetPitch(float afX);
		void AddPitch(float afX);
		float GetPitch();

		const cVector3f& GetForward();
		const cVector3f& GetRight();
		const cVector3f& GetUp();

		const cMatrixf& GetMoveMatrix(){ return m_mtxMove;}

		void SetGravityActive(bool abX);
		bool GravityIsActive();
		void SetMaxGravitySpeed(float afX);
		float GetMaxGravitySpeed();

		bool GetCustomGravityActive();
		void SetCustomGravityActive(bool abX);
		void SetCustomGravity(const cVector3f& avCustomGravity);
		cVector3f GetCustomGravity();

		void AddForceVelocity(const cVector3f& avVel){ mvVelocity += avVel;}
		void SetForceVelocity(const cVector3f& avVel){ mvVelocity = avVel;}
		cVector3f GetForceVelocity(){ return mvVelocity;}

		bool IsOnGround();
		void SetMaxOnGroundCount(int alX) { mlMaxOnGroundCount = alX;}
		int GetMaxOnGroundCount() { return mlMaxOnGroundCount;}

		void SetCollideFlags(tFlag alX);
		inline tFlag GetCollideFlags() const { return mlCollideFlags;}

		void SetGroundFriction(float afX){ mfGroundFriction = afX;}
		float GetGroundFriction(){ return mfGroundFriction;}

		void SetAirFriction(float afX){ mfAirFriction = afX;}
		float GetAirFriction(){ return mfAirFriction;}

		void SetDisableDiagSpeedBoost(bool abX){ mbDisableDiagSpeedBoost = abX;}
		bool GetDisableDiagSpeedBoost(){ return mbDisableDiagSpeedBoost;}

		void SetMoveDelay(float afX){mfMoveDelayCount=afX;}
		float GetMoveDelay(){ return mfMoveDelayCount;}

		///////////////////////////////////////
		// Interaction Properties

		bool GetMovedLastUpdate(){ return mbActive && mbMovedLastUpdate;}

		void SetMaxPushMass(float afX){ mfMaxPushMass = afX;}
		void SetPushForce(float afX){ mfPushForce = afX;}
		float GetMaxPushMass(){ return mfMaxPushMass;}
		float GetPushForce(){ return mfPushForce;}
		bool GetPushIn2D(){ return mbPushIn2D;}
		void SetPushIn2D(bool abX){ mbPushIn2D = abX;}

		void SetCharacterMaxPushMass(float afX){ mfCharacterMaxPushMass = afX;}
		void SetCharacterPushForce(float afX){ mfCharacterPushForce = afX;}
		float GetCharacterMaxPushMass(){ return mfCharacterMaxPushMass;}
		float GetCharacterPushForce(){ return mfCharacterPushForce;}
		bool GetCharacterPushIn2D(){ return mbCharacterPushIn2D;}
		void SetCharacterPushIn2D(bool abX){ mbCharacterPushIn2D = abX;}

		void SetConstantContactForceMul(float afX){ mfConstantContactForceMul = afX;}
		float GetConstantContactForceMul(){ return mfConstantContactForceMul;}
		void SetVelocityContactForceMul(float afX){ mfVelocityContactForceMul = afX;}
		float GetVelocityContactForceMul(){ return mfVelocityContactForceMul;}
		void SetMaxContactForcePerMassUnit(float afX){ mfMaxContactForcePerMassUnit = afX;}
		float GetMaxContactForcePerMassUnit(){ return mfMaxContactForcePerMassUnit;}

		void SetMaxNoSlideSlopeAngle(float afAngle);
		float GetMaxNoSlideSlopeAngle();

		void SetStickToSlope(bool abX){ mbStickToSlope = abX;}
		bool GetStickToSlope(){ return mbStickToSlope;}

		float GetMaxStepSize(){ return mfMaxStepHeight;}
		void SetMaxStepSize(float afSize){ mfMaxStepHeight = afSize;}

		float GetMaxStepSizeInAir(){ return mfMaxStepHeightInAir;}
		void SetMaxStepSizeInAir(float afSize){ mfMaxStepHeightInAir = afSize;}

		void SetStepClimbSpeed(float afX){ mfStepClimbSpeed = afX;}
		float GetStepClimbSpeed(){ return mfStepClimbSpeed;}

		void SetAccurateClimbing(bool abX){ mbAccurateClimbing = abX;}
		bool GetAccurateClimbing(){ return mbAccurateClimbing;}

		void SetClimbForwardMul(float afX){ mfClimbForwardMul= afX;}
		float GetClimbForwardMul(){ return mfClimbForwardMul;}

		void SetClimbHeightAdd(float afX){ mfClimbHeightAdd = afX;}
		float GetClimbHeightAdd(){ return mfClimbHeightAdd;}

		bool IsClimbing(){ return mbClimbing;}
		void ResetClimbing();

		const cVector3f& GetLastGroundNormal(){ return mvLastGroundNormal;}

		void SetRotateYawWhenGravityAttached(bool abX){ mbRotateYawWhenGravityAttached = abX;}
		bool GetRotateYawWhenGravityAttached(){ return mbRotateYawWhenGravityAttached;}

		void SetGravityAttachedBody(iPhysicsBody* apBody){ mpGravityAttachedBody = apBody;}
		iPhysicsBody* GetGravityAttachedBody(){ return mpGravityAttachedBody;}

		iPhysicsMaterial * GetGravityCollideMaterial(){ return mpGravityCollideMaterial;}
		
		///////////////////////////////////////
		// Connection Properties

		iPhysicsBody * GetConnectedBody();
		cVector3f GetBodyConnectionPos();
		cVector3f GetCharConnectionPos();

		const cVector3f& GetConnectionPosLocalToBody(){ return mvConnectionPosLocalToBody; }
		const cVector3f& GetConnectionPosLocalToChar(){ return mvConnectionPosLocalToChar; }
		void SetConnectionPosLocalToBody(const cVector3f& avPos){ mvConnectionPosLocalToBody = avPos; }
		void SetConnectionPosLocalToChar(const cVector3f& avPos){ mvConnectionPosLocalToChar = avPos; }

		const cVector3f& GetConnectionLocalUpBody(){ return mvConnectionLocalUpBody;}
		void SetConnectionLocalUpBody(const cVector3f& avVec){ mvConnectionLocalUpBody = avVec;}

		void SetMaxConnectionForce(float afX){ mfMaxConnectionForce = afX; }
		void SetMaxConnectionTorque(float afX){ mfMaxConnectionTorque = afX; }
		float GetMaxConnectionForce(){ return mfMaxConnectionForce; }
		float GetMaxConnectionTorque(){ return mfMaxConnectionTorque; }
		
		///////////////////////////////////////
		// User Properties

		void SetCamera(cCamera *apCam);
        cCamera* GetCamera();
		void SetCameraPosAdd(const cVector3f &avAdd);
		cVector3f GetCameraPosAdd();
		void SetCameraSmoothPosNum(int alNum){ mlCameraSmoothPosNum = alNum;}
		int GetCameraSmoothPosNum(){ return mlCameraSmoothPosNum;}

		void SetEntity(iEntity3D *apEntity);
		iEntity3D* GetEntity();
		
		void SetEntityOffset(const cMatrixf &a_mtxOffset);
		const cMatrixf & GetEntityOffset();
		
		void SetEntityPostOffset(const cMatrixf &a_mtxOffset);
		const cMatrixf & GetEntityPostOffset();
		
		void SetEntitySmoothPosNum(int alNum){ mlEntitySmoothPosNum = alNum;}
		int GetEntitySmoothPosNum(){ return mlEntitySmoothPosNum;}

		void SetUseEntitySmoothYPos(bool abX){ mbEntitySmoothYPos = abX;}
		void SetEntitySmoothYPosNum(int alX){ mlEntitySmoothYPosNum = alX;}
		bool GetUseEntitySmoothYPos(){ return mbEntitySmoothYPos;}
		int GetEntitySmoothYPosNum(){ return mlEntitySmoothYPosNum;}


		void SetUserData(void* apUserData){ mpUserData = apUserData;}
		void* GetUserData(){ return mpUserData;}
		
		void SetCallback(iCharacterBodyCallback *apCallback){ mpCallback = apCallback;}
		
		///////////////////////////////////////
		//Debug:
		const cVector3f& GetLastMovePosAdd(){ return mvLastMovePosAdd;}
		
	protected:
		void UpdateMoveMatrix();

		void PreUpdateConnection(float afTimeStep);
		void PostUpdateConnection(float afTimeStep);
		void UpdateBodyConnection(float afTimeStep);
		void UpdateCharacterConnection(float afTimeStep);
		
		cVector3f UpdatePostionFromCharSpeed(float afTimeStep);

		void AlignPosAddAccordingToGroundNormal(cVector3f &avPosAdd);

		void CheckMoveCollision(const cVector3f &avPosAdd, float afTimeStep);
		
		void CheckStepClimbing(const cVector3f &avPosAdd, float afTimeStep);

		void UpdateStepClimbing(float afTimeStep);

		void UpdateForces(float afTimeStep);
		void UpdateFriction(float afTimeStep);

		void CheckForceCollision(float afTimeStep);

		void UpdateForcePushing(float afTimeStep);

		void UpdateCamera();
		void UpdateEntity();

		void UpdateBody();

		void ClearGravityAttachment();
		void UpdateGravityAttachment();

		void EnableBodiesAroundCharacter();

		bool CheckCollision(cVector3f *apPushBackVector, const cVector3f& avPos, iPhysicsWorldCollisionCallback *apCallback,int alShapeIdx=-1);

		tString msName;

		float mfMass;

		bool mbActive;

		bool mbCollideCharacter;

		bool mbTestCollision;

		bool mbStickToSlope;

		bool mbDisableDiagSpeedBoost;

		tFlag mlCollideFlags;

		bool mbGravityActive;
		float mfMaxGravitySpeed;
		bool mbCustomGravity;
		cVector3f mvCustomGravity;

		cVector3f mvPosition;
		cVector3f mvLastPosition;
		
		float mfMaxPosMoveSpeed[2];
		float mfMaxNegMoveSpeed[2];

		float mfCurrentMoveAcc[2];
		float mfMoveAcc[2];
		float mfMoveDeacc[2];
		float mfMoveSpeed[2];
		float mfMoveOppositeDirAccMul[2];
		bool mbMoving[2];
		bool mbDeaccelerateMoveSpeedInAir;
		cVector3f mvLastMovePosAdd;

		bool mbMovedLastUpdate;

		float mfPitch;
		float mfYaw;

        int mlOnGroundCount;
		int mlMaxOnGroundCount;

		float mfMaxPushMass;
		float mfPushForce;
		bool mbPushIn2D;

		float mfCharacterMaxPushMass;
		float mfCharacterPushForce;
		bool mbCharacterPushIn2D;

		int mlMinBodyPushStrength;

		float mfCheckStepClimbCount;
		float mfCheckStepClimbInterval;

		float mfMaxNoSlideSlopeAngleCos;

		cVector3f mvForce;
		cVector3f mvVelocity;
		cVector3f mvLastGroundNormal;
		
		cVector3f mvSize;

		cMatrixf m_mtxMove;
		cVector3f mvForward;
		cVector3f mvRight;
		cVector3f mvUp;

		float mfConstantContactForceMul;
		float mfVelocityContactForceMul;
		float mfMaxContactForcePerMassUnit;

		cCamera *mpCamera;
		cVector3f mvCameraPosAdd;
		int mlCameraSmoothPosNum;
		tVector3fList mlstCameraPos;

		iEntity3D *mpEntity;
		cMatrixf m_mtxEntityOffset;
		cMatrixf m_mtxEntityPostOffset;
		int mlEntitySmoothPosNum;
		tVector3fList mlstEntityPos;

		bool mbEntitySmoothYPos;
		int mlEntitySmoothYPosNum;
		tFloatList mlstEntityYPositions;

		float mfGroundFriction;
		float mfAirFriction;

		float mfMoveDelayCount;

		void *mpUserData;
	
		float mfMaxConnectionTorque;
		float mfMaxConnectionForce;
		iPhysicsBody *mpConnectedBody;
		bool mbConnectedBodyHadGravity;
		bool mbConnectionDependOnBodyRotation;
		cVector3f mvConnectionPosLocalToBody;
		cVector3f mvConnectionPosLocalToChar;
		cVector3f mvConnectionNormalLocalToBody;
		cVector3f mvConnectionLocalUpBody;
		cVector3f mvConnectionBodyStartRotation;
		float mfConnectionDistanceToBody;
		bool mbConnectionAlignCenter;
		bool mbConnectionAlignRotation;
		bool mbConnectionCollision;
		bool mbConnectedBodyIsActive;
		
		bool mbConnectionAffectChar;
		bool mbConnectionDependOnCharRotation;
		bool mbConnectionAlignCharacterRotation;
		cVector3f mvConnectionCharLocalForward;
				
		cPidControllerVec3 mBodyConnectionPointPid;
		cPidControllerVec3 mBodyCenterPid;
		cPidControllerf mBodyOneAxisAngularPid;
		cPidControllerVec3 mBodyAngularPid;

		cPidControllerVec3 mCharConnectionPointPid;
		cPidControllerVec3 mCharRotatePid;

		bool mbRotateYawWhenGravityAttached;
		iPhysicsBody *mpGravityAttachedBody;
		iPhysicsBody *mpGravityLastAttachedBody;
		cMatrixf m_mtxGravityAttachedPrevMatrix;
		int mlGravityAttachmentContacts;
		cVector3f mvGravityAttachmentBodyVelocity;
		cVector3f mvGravityAttachmentVelocity;
		
		float mfMaxStepHeight;
		float mfMaxStepHeightInAir;
		float mfStepClimbSpeed;
		float mfClimbForwardMul;
		float mfClimbHeightAdd;
		bool mbClimbing;
		bool mbAccurateClimbing;

		iCharacterBodyCallback *mpCallback;

		iPhysicsWorld *mpWorld;

		cCharacterBodyCollideGravity *mpCollideGravityCallback;
		cCharacterBodyCollidePush *mpCollidePushCallback;
		cCharacterBodyRay *mpRayCallback;

		iPhysicsMaterial *mpGravityCollideMaterial;

		int mlCurrentShapeIdx;
		iCollideShape *mpCurrentShape;
		iPhysicsBody *mpCurrentBody;
		std::vector<iCollideShape*> mvShapes;
		std::vector<iPhysicsBody*> mvBodies;

		static std::vector<iPhysicsBody*> mvTempBodies;
	};
};
#endif // HPL_CHARACTER_BODY_H
