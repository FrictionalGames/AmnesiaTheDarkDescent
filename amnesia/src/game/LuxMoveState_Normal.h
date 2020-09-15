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

#ifndef LUX_MOVE_STATE_NORMAL_H
#define LUX_MOVE_STATE_NORMAL_H

//----------------------------------------------

#include "LuxMoveState.h"

//----------------------------------------------

class cLuxMoveState_Normal;

class cLuxMoveState_Normal_Callback : public iCharacterBodyCallback
{
public:
	cLuxMoveState_Normal_Callback(cLuxMoveState_Normal *apMoveState);

	void OnGravityCollide(iCharacterBody *apCharBody, iPhysicsBody *apBody, cCollideData *apCollideData);
	void OnHitGround(iCharacterBody *apCharBody,const cVector3f &avVel){}


private:
	cLuxMoveState_Normal *mpMoveState;
};

//----------------------------------------------

enum eLuxFootSound
{
	eLuxFootSound_Step,	
	eLuxFootSound_Landing,
};

//----------------------------------------------

class cLuxMoveState_Normal : public iLuxMoveState
{
friend class cLuxMoveState_Normal_Callback;
friend class cLuxPlayer_SaveData;
public:	
	cLuxMoveState_Normal(cLuxPlayer *apPlayer);
	~cLuxMoveState_Normal();
	
	void OnMapEnter();

	void OnEnterState(eLuxMoveState aPrevState);
	void OnLeaveState(eLuxMoveState aNewState);

	void OnUpdate(float afTimeStep);

	bool GetAllowsInteraction();

	bool OnMove(eCharDir aDir, float afMul);
	bool OnAddYaw(float afAmount);
	bool OnAddPitch(float afAmount);

	void OnRun(bool abActive);
	void OnCrouch(bool abActive);
	void OnJump(bool abActive);
	
	void Jump();

	void OnDraw(float afFrameTime);

	float GetHeadBobCount(){ return mfHeadBobCount;}

	bool IsRunning(){ return mbRunning;}
	bool IsCrouching(){ return mbCrouching;}

	void SetCrouch(bool abActive);
	void ResetJumping();

	float GetMoveSpeedMul();
	float GetRunSpeedMul();
	
private:
	bool UpdateLedgeCheck(float afTimeStep);
	void UpdateMovement(float afTimeStep);
	void UpdateSpeedMultipliers(float afTimeStep);
	void UpdateJumpAndGroundCheck(float afTimeStep);
	void UpdateHeadBob(float afTimeStep);

	void FootSound(eLuxFootSound aType);

	void FallDamage(float afYSpeed);

	cLuxMoveState_Normal_Callback *mpCallback;

	tString msJumpSound;
	tString msCrouchSound;
	tString msStandSound;
    
	bool mbWasMoving;

	float mfRunForwardMul;
	float mfRunBackwardMul;
	float mfRunSidewayMul;

	float mfCrouchForwardMul;
	float mfCrouchBackwardMul;
	float mfCrouchSidewayMul;

	float mfInAirForwardMul;
	float mfInAirBackwardMul;
	float mfInAirSidewayMul;

	float mfDefaultForwardMul; 

	float mfMinHitGroundBounceSpeed;

	bool mbRunning;
	bool mbCrouching;

	bool mbJumping;
	float mfJumpStartForce;
	float mfJumpCrouchStartForce;
	float mfMaxJumpCount;
	float mfJumpCount;

	cVector3f mvHeadBob;
	float mfPrevHeadBobCount;
	float mfHeadBobCount;
	bool mbBobbing;

	bool mbWasOnGround;

	bool mbHeadGroundBounceActive;
	float mfHeadGroundBounce;
	
	cVector2f mvWalkBobMax;
	cVector2f mvRunBobMax;
	cVector2f mvCrouchBobMax;
	
	float mfWalkMinBobSpeed;
	float mfWalkMaxBobSpeed;
	
	float mfRunMinBobSpeed;
	float mfRunMaxBobSpeed;
	
	float mfCrouchMinBobSpeed;
	float mfCrouchMaxBobSpeed;

	cVector2f mvCurrentBobMax;
	cVector2f mvBobMaxGoal;

	float mfGroundBounceSize;
	float mfGroundBounceSpeed;
	
	float mfBounceSizeMul;
	float mfBounceSpeedMul;

	float mfClimbLedgeCount;
	float mfMaxClimbLedgeCount;

	float mfClimbLedgeCheckDistForward;
	float mfClimbLedgeCheckDistFromTop;
	float mfClimbLedgeCheckDistTopToEnd;
	float mfClimbLedgeMaxPushDist;

	float mfFallDamageBounceSizeMul;
	float mfFallDamageBounceSpeedMul;

	float mfFallDamageSpeed_Min;
	float mfFallDamage_Min;
	tString msFallDamageSound_Min;

	float mfFallDamageSpeed_Med;
	float mfFallDamage_Med;
	tString msFallDamageSound_Med;

	float mfFallDamageSpeed_Max;
	float mfFallDamage_Max;
	tString msFallDamageSound_Max;
};

//----------------------------------------------


#endif // LUX_MOVE_STATE_H
