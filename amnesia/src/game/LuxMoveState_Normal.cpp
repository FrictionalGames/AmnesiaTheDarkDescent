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

#include "LuxMoveState_Normal.h"

#include "LuxPlayer.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxHelpFuncs.h"
#include "LuxPlayerHelpers.h"

//////////////////////////////////////////////////////////////////////////
// CHARACTER CALLBACK
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------

cLuxMoveState_Normal_Callback::cLuxMoveState_Normal_Callback(cLuxMoveState_Normal *apMoveState)
{
	mpMoveState = apMoveState;
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal_Callback::OnGravityCollide(iCharacterBody *apCharBody, iPhysicsBody *apBody, cCollideData *apCollideData)
{
	//Check if there is enough speed for a bounce!
	if(	apCollideData->mvContactPoints[0].mvNormal.y > 0.001f &&
		apCharBody->GetForceVelocity().y < -mpMoveState->mfMinHitGroundBounceSpeed)
	{
		if(mpMoveState->mbHeadGroundBounceActive == false)
		{
			mpMoveState->mbHeadGroundBounceActive = true;
			mpMoveState->mfBounceSizeMul = 1.0f;
			mpMoveState->mfBounceSpeedMul = 1.0f;
			mpMoveState->FootSound(eLuxFootSound_Landing);

			mpMoveState->FallDamage(apCharBody->GetForceVelocity().y);
		}
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMoveState_Normal::cLuxMoveState_Normal(cLuxPlayer *apPlayer) : iLuxMoveState(apPlayer)
{
	////////////////////////////
	// Create data
	mpCallback = hplNew( cLuxMoveState_Normal_Callback, (this));
	
	mbRunning = false;
	mbCrouching = false;
	mbJumping = false;

	////////////////////////////
	// Sounds
	msJumpSound = gpBase->mpGameCfg->GetString("Player_Movement_Normal","JumpSound","player_jump");
	msCrouchSound = gpBase->mpGameCfg->GetString("Player_Movement_Normal","CrouchSound","player_crouch");
	msStandSound = gpBase->mpGameCfg->GetString("Player_Movement_Normal","StandSound","player_stand");
	
	////////////////////////////
	// Set up speeds
	mfMaxForwardSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","MaxForwardSpeed",0);
	mfMaxBackwardSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","MaxBackwardSpeed",0);
	mfMaxSidwaySpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","MaxSidwaySpeed",0);
	
	////////////////////////////
	// Set movement multipliers
	mfRunForwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","RunForwardMul",0);
	mfRunBackwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","RunBackwardMul",0);
	mfRunSidewayMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","RunSidewayMul",0);

	mfCrouchForwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","CrouchForwardMul",0);
	mfCrouchBackwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","CrouchBackwardMul",0);
	mfCrouchSidewayMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","CrouchSidewayMul",0);

	mfInAirForwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","InAirForwardMul",0);
	mfInAirBackwardMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","InAirBackwardMul",0);
	mfInAirSidewayMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","InAirSidewayMul",0);

	////////////////////////////
	// Set up jumping
	mfMaxJumpCount =gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","MaxJumpCount",0);
	mfJumpCount = mfMaxJumpCount;
	mfJumpStartForce = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","JumpStartForce",0);
	mfJumpCrouchStartForce = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","JumpCrouchStartForce",0);

	////////////////////////////
	// Set up bobbing
	mvCrouchBobMax = gpBase->mpGameCfg->GetVector2f("Player_Movement_Normal","CrouchBobMax",0);
	mvWalkBobMax = gpBase->mpGameCfg->GetVector2f("Player_Movement_Normal","WalkBobMax",0);
	mvRunBobMax = gpBase->mpGameCfg->GetVector2f("Player_Movement_Normal","RunBobMax",0);

	mfCrouchMinBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","CrouchMinBobSpeed",0);
	mfCrouchMaxBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","CrouchMaxBobSpeed",0);
	
	mfWalkMinBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","WalkMinBobSpeed",0);
	mfWalkMaxBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","WalkMaxBobSpeed",0);
	
	mfRunMinBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","RunMinBobSpeed",0);
	mfRunMaxBobSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","RunMaxBobSpeed",0);

	////////////////////////////
	// Set up ground bounce
	mfGroundBounceSize = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","GroundBounceSize",0);
	mfGroundBounceSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","GroundBounceSpeed",0);
	mfMinHitGroundBounceSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_Normal","MinHitGroundBounceSpeed",0);

	////////////////////////////
	// Set up ledge climbing
	mfMaxClimbLedgeCount =  1.0f / gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","CheckUpdatesPerSecond",0);

	mfClimbLedgeCheckDistForward = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","ClimbLedgeCheckDistForward",0);
	mfClimbLedgeCheckDistFromTop = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","ClimbLedgeCheckDistFromTop",0);
	mfClimbLedgeCheckDistTopToEnd = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","ClimbLedgeCheckDistTopToEnd",0);
	mfClimbLedgeMaxPushDist = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","CheckMaxPushDist",0);


	////////////////////////////
	// Set up fall damage
	mfFallDamageBounceSizeMul = gpBase->mpGameCfg->GetFloat("Player_General","FallDamageBounceSizeMul",0);
	mfFallDamageBounceSpeedMul = gpBase->mpGameCfg->GetFloat("Player_General","FallDamageBounceSpeedMul",0);

	mfFallDamageSpeed_Min = gpBase->mpGameCfg->GetFloat("Player_General","FallDamageSpeed_Min",0);
	mfFallDamage_Min = gpBase->mpGameCfg->GetFloat("Player_General","FallDamage_Min",0);
	msFallDamageSound_Min = gpBase->mpGameCfg->GetString("Player_General","FallDamageSound_Min","");

	mfFallDamageSpeed_Med = gpBase->mpGameCfg->GetFloat("Player_General","FallDamageSpeed_Med",0);
	mfFallDamage_Med = gpBase->mpGameCfg->GetFloat("Player_General","FallDamage_Med",0);
	msFallDamageSound_Med = gpBase->mpGameCfg->GetString("Player_General","FallDamageSound_Med","");

	mfFallDamageSpeed_Max = gpBase->mpGameCfg->GetFloat("Player_General","FallSpeed_Max",0);
	mfFallDamage_Max = gpBase->mpGameCfg->GetFloat("Player_General","FallDamage_Max",0);
	msFallDamageSound_Max = gpBase->mpGameCfg->GetString("Player_General","FallDamageSound_Max","");
}

//-----------------------------------------------------------------------

cLuxMoveState_Normal::~cLuxMoveState_Normal()
{
	hplDelete(mpCallback);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnMapEnter()
{
	////////////////////////////
	// Set up variables
	
	//Perhaps have this in player and call the movestes from the callback?
    mpPlayer->GetCharacterBody()->SetCallback(mpCallback);

	if(mpPlayer->GetCurrentMoveState()==eLuxMoveState_Normal)
	{
		if(mbCrouching)        
			mpPlayer->GetCharacterBody()->SetActiveSize(1);
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnEnterState(eLuxMoveState aPrevState)
{
	////////////////////////////
	// Set up variables
	mbRunning = false;
	
	mbWasMoving = false;
	mbWasOnGround = false;

	mfClimbLedgeCount = 0;

	mbHeadGroundBounceActive = false;
	mfHeadGroundBounce = 0;
	
	mfBounceSizeMul = 1.0f;
	mfBounceSpeedMul = 1.0f;
	
	mvHeadBob =0;
	mfPrevHeadBobCount =0;
	mfHeadBobCount = 0;
	mbBobbing = false;
	mvBobMaxGoal = mvWalkBobMax;
	mvCurrentBobMax = mvBobMaxGoal;

	if(aPrevState!= eLuxMoveState_Normal)
	{
		mbCrouching = false;
	}

	//The mul without any extras, used to determine the bob speed compared to move speed.
	mfDefaultForwardMul = 1.0f;
}

void cLuxMoveState_Normal::OnLeaveState(eLuxMoveState aNewState)
{
	//Reset head bob
	mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_Bob, 0.0f, 0.5f, 0.01f);
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnUpdate(float afTimeStep)
{
	////////////////////////////
	// Check if there is any ledge up ahead.
	if(UpdateLedgeCheck(afTimeStep)) return;

	////////////////////////////
	// Update movement related stuff
	UpdateMovement(afTimeStep);

	
	////////////////////////////
	// Update multipliers
	UpdateSpeedMultipliers(afTimeStep);

	////////////////////////////
	// Update jump and ground checks
	UpdateJumpAndGroundCheck(afTimeStep);

	////////////////////////////
	// Update head bob
	UpdateHeadBob(afTimeStep);
	
}

//-----------------------------------------------------------------------

bool cLuxMoveState_Normal::GetAllowsInteraction()
{
	return true;
}

//-----------------------------------------------------------------------

bool cLuxMoveState_Normal::OnMove(eCharDir aDir, float afMul)
{
	return true;
}
bool cLuxMoveState_Normal::OnAddYaw(float afAmount)
{
	return true;
}
bool cLuxMoveState_Normal::OnAddPitch(float afAmount)
{
	return true;
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnRun(bool abActive)
{
	if(abActive)
	{
		if(mpPlayer->GetCrouchDisabled()==false && mpPlayer->GetInsanityCollapse()->IsActive()==false)
		{
			//Stand up if moving and running!
			iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
			if(pCharBody->IsOnGround() && mpPlayer->GetPressedMove())
			{
				SetCrouch(false);
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnCrouch(bool abActive)
{
	//Do nothing on button release
	if(abActive==false) return;

	SetCrouch(!mbCrouching);	
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnJump(bool abActive)
{
	//////////////////////////
	//Jump pressed
	if(abActive && mbJumping == false)
	{
		iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
		if(pCharBody->IsOnGround()==false) return;

		Jump();
	}
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::Jump()
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	SetCrouch(false);

	gpBase->mpHelpFuncs->PlayGuiSoundData(msJumpSound, eSoundEntryType_World);

	float fStartForce = mbCrouching ? mfJumpCrouchStartForce : mfJumpStartForce;
	fStartForce *= mpPlayer->GetScriptJumpForceMul();

	pCharBody->AddForce(cVector3f(0, fStartForce * mpPlayer->GetDefaultMass(),0));		
	mbJumping = true;
	mfJumpCount = 0;
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::OnDraw(float afFrameTime)
{
	return;//Skip for now! Have some special thingy for this!
	float fY = 70;
	gpBase->mpGameDebugSet->DrawFont(gpBase->mpDefaultFont, cVector3f(5,70,0),12,cColor(1,1),
								_W("Crouch: %d Running: %d JumpCount: %f\n"),mbCrouching,mbRunning,mfJumpCount);
	fY+=13;
	gpBase->mpGameDebugSet->DrawFont(gpBase->mpDefaultFont, cVector3f(5,fY,0),12,cColor(1,1),
								_W("Bob: %ls (%f). Bounce: %f\n"),cString::To16Char(mvHeadBob.ToString()).c_str(), cMath::ToDeg(cMath::Wrap(mfHeadBobCount,0, k2Pif)),
											mfHeadGroundBounce);
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::SetCrouch(bool abActive)
{
	if(mbCrouching == abActive) return;

	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	/////////////////////
	//Crouch
	if(abActive)
	{
		gpBase->mpHelpFuncs->PlayGuiSoundData(msCrouchSound, eSoundEntryType_World);

		mbCrouching = true;
		pCharBody->SetActiveSize(1);

		//Move the main size add to the size crouch size.
		float fSizeDiff = mpPlayer->GetBodySize().y - mpPlayer->GetBodyCrouchSize().y;
		mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_Main, cVector3f(0,-fSizeDiff, 0), 1.3f, 0.05f);
	}
	//////////////////////
	//Stand up
	else
	{
		cVector3f vPosTestAdd[5] = {cVector3f(0,0.001f,0), 
									cVector3f(0.01f,0.001f,0), cVector3f(-0.01f,0.001f,0),
									cVector3f(0,0.001f,0.01f), cVector3f(0,0.001f,-0.01f)
								};

		////////////////////////////////////////
		// Iterate through some positions to see if player will fit there
		bool bFits = false;
		int lCurrentFitTest =0;
		for(; lCurrentFitTest<5; ++lCurrentFitTest)
		{
			if(pCharBody->CheckCharacterFits(pCharBody->GetFeetPosition()+vPosTestAdd[lCurrentFitTest],true, 0))
			{
				bFits = true;
				break;
			}
		}


		////////////////////////////////////////
		// Player fits, stand up!
		if(bFits)
		{
			gpBase->mpHelpFuncs->PlayGuiSoundData(msStandSound, eSoundEntryType_World);

			mbCrouching = false;
			pCharBody->SetPosition(pCharBody->GetPosition()+vPosTestAdd[lCurrentFitTest]); //Make sure to move the player to the fitting positon!
			pCharBody->SetActiveSize(0);
			mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_Main, 0, 1.6f, 0.05f);
		}
		else
		{
			//Some grunt or something?
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::ResetJumping()
{
	mbJumping = false;
}

//-----------------------------------------------------------------------

float cLuxMoveState_Normal::GetMoveSpeedMul()
{
	float fMoveMul =	mpPlayer->GetInteractionMoveSpeedMul() * 
						mpPlayer->GetScriptMoveSpeedMul() * 
						mpPlayer->GetEventMoveSpeedMul() * 
						mpPlayer->GetHurtMoveSpeedMul() *
						mpPlayer->GetInsanityCollapseSpeedMul();
	if(mpPlayer->IsInWater()) fMoveMul *= mpPlayer->GetWaterSpeedMul();
	
	return fMoveMul;
}

//-----------------------------------------------------------------------

float cLuxMoveState_Normal::GetRunSpeedMul()
{
	return mpPlayer->GetScriptRunSpeedMul() * mpPlayer->GetEventRunSpeedMul();
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::UpdateMovement(float afTimeStep)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	//////////////////////////////////////
	//Update the running state
	mbRunning = mpPlayer->IsPressingRun();


	//////////////////////////////////////
	//Check if character is running, if so stand up (if possible)
	if(pCharBody->IsOnGround() && mpPlayer->GetPressedMove() && mbWasMoving==false)
	{
		mbWasMoving = true;

		if(mbRunning)
		{
			if(mpPlayer->GetCrouchDisabled()==false && mpPlayer->GetInsanityCollapse()->IsActive()==false)
			{
				SetCrouch(false);
			}
		}
	}
	else
	{
		mbWasMoving = false;
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::UpdateSpeedMultipliers(float afTimeStep)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	float fMoveMul =	GetMoveSpeedMul();

	mfMaxForwardSpeedMul = fMoveMul;
	mfMaxBackwardSpeedMul = fMoveMul;
	mfMaxSidwaySpeedMul = fMoveMul;

	mfDefaultForwardMul = 1.0f;

	////////////////////////
	//On ground
	if(pCharBody->IsOnGround())
	{
		//Crouching
		if(mbCrouching)
		{
			mfMaxForwardSpeedMul *= mfCrouchForwardMul;
			mfMaxBackwardSpeedMul *= mfCrouchBackwardMul;
			mfMaxSidwaySpeedMul *= mfCrouchSidewayMul;

			mfDefaultForwardMul *= mfCrouchForwardMul;
		}
		//Running
		else if(mbRunning)
		{
			//The cumbersome-looking algorithm is because it should be easy to set the amount of running
			//speed the player has.
			float fRunSpeedMul = GetRunSpeedMul();

			mfMaxForwardSpeedMul *= 1 + (mfRunForwardMul-1) * fRunSpeedMul;
			mfMaxBackwardSpeedMul *= 1 + (mfRunBackwardMul-1) * fRunSpeedMul;
			mfMaxSidwaySpeedMul *= 1 + (mfRunSidewayMul-1) * fRunSpeedMul;

			mfDefaultForwardMul *= mfRunForwardMul;
		}	
	}
	////////////////////////
	//Not on ground
	else
	{
		mfMaxForwardSpeedMul = mfInAirForwardMul;
		mfMaxBackwardSpeedMul = mfInAirBackwardMul;
		mfMaxSidwaySpeedMul = mfInAirSidewayMul;

		mfDefaultForwardMul = mfInAirForwardMul;
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::UpdateJumpAndGroundCheck(float afTimeStep)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	////////////////////////////
	// Check player landing
	if(pCharBody->IsOnGround() && pCharBody->GetForceVelocity().y <=0)
	{
		mbWasOnGround = true;
	}
	else if(pCharBody->IsOnGround()==false)
	{
		mbWasOnGround = false;
	}


	////////////////////////////
	// Update Jump button is held down.
	if(mbJumping)
	{
		mfJumpCount += afTimeStep;
		if(mfJumpCount >= mfMaxJumpCount)
		{
			mfJumpCount = mfMaxJumpCount;
			mbJumping = false;
		}

		iPhysicsWorld *pPhysicsWorld = gpBase->mpMapHandler->GetCurrentMap()->GetPhysicsWorld();
		cVector3f vGravity = pCharBody->GetCustomGravityActive() ?	pCharBody->GetCustomGravity() : 
		pPhysicsWorld->GetGravity();

		//Remove some gravity and make jump last longer.
		float fMul = 0.4f + 0.5f * (1 - mfJumpCount / mfMaxJumpCount);
		pCharBody->AddForce(cVector3f(0,-vGravity.y * pCharBody->GetMass() * fMul,0));		
	}
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::UpdateHeadBob(float afTimeStep)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	/////////////////////////
	//Get the current bob max
	mvBobMaxGoal = mvWalkBobMax;
	float fMinBobSpeed = mfWalkMinBobSpeed;
	float fMaxBobSpeed = mfWalkMaxBobSpeed;
	if(mbCrouching)
	{
		mvBobMaxGoal = mvCrouchBobMax;
		fMinBobSpeed = mfCrouchMinBobSpeed;
		fMaxBobSpeed = mfCrouchMaxBobSpeed;
	}
    else if(mbRunning)
	{
		mvBobMaxGoal = mvRunBobMax;
		fMinBobSpeed = mfRunMinBobSpeed;
		fMaxBobSpeed = mfRunMaxBobSpeed;
	}

	float fPlayerSpeed = pCharBody->GetVelocity(afTimeStep).Length();
	float fMaxPlayerSpeed = mfMaxForwardSpeed * mfDefaultForwardMul;
	float fBobSpeed = fMinBobSpeed + (fPlayerSpeed/fMaxPlayerSpeed) * (fMaxBobSpeed - fMinBobSpeed);
	
	/////////////////////
	// Check if moving
	bool bMoving = pCharBody->IsOnGround() && pCharBody->GetMovedLastUpdate();

	//If not moving fade size to 0 too.
	if(bMoving==false)
	{
		mvBobMaxGoal =0;
	}
	
	//Fade into the new max
	float fAdd = 0.1f;
	mvCurrentBobMax = cMath::Vector2IncreaseTo(mvCurrentBobMax, fAdd * afTimeStep, mvBobMaxGoal);
	

	/////////////////////////
	//Player is not moving
	if(bMoving==false)
	{
        //If the head is in a bob, move it back to the rest state (sin = 1)
		if(mbBobbing)
		{
			//Wrap x between -90 and 270 and see where what direction to go
			float fX = cMath::Wrap(mfHeadBobCount,cMath::ToRad(-90),cMath::ToRad(270) );
			float fAdd;
			if(fX <= cMath::ToRad(90)) fAdd = 1;
			else						fAdd = -1;

			float fPrevCos = cos(mfHeadBobCount);

			mfPrevHeadBobCount = mfHeadBobCount;
			mfHeadBobCount += afTimeStep * k2Pif * fAdd * 3.1f;
			
			float fCos = cos(mfHeadBobCount);
			float fSin = cos(mfHeadBobCount);

			//See if count passed a cos =0 and that sin is above 1.
			if(fSin > 0 && ((fCos <=0 && fPrevCos >= 0) || (fCos >=0 && fPrevCos <= 0)))
			{
				mbBobbing = false;
				mvHeadBob.y = 0;
				mvHeadBob.x = 0;
			}
		}
	}
	/////////////////////////
	//Player moves
	else
	{
		//Set and update variables
		mbBobbing = true;
		mfPrevHeadBobCount = mfHeadBobCount;
		mfHeadBobCount += fBobSpeed * afTimeStep * k2Pif;

		//Footstep
		float fPrevCos = cos(mfPrevHeadBobCount);
		float fCos = cos(mfHeadBobCount);
		float fSin = sin(mfHeadBobCount);
		
		//Check if in lowest position
		if(fSin < 0 && ((fCos <=0 && fPrevCos >= 0) || (fCos >=0 && fPrevCos <= 0)))
		{
			FootSound(eLuxFootSound_Step);
		}
	}
	
	if(mbBobbing)
	{
		mvHeadBob.y = sin(mfHeadBobCount) * mvCurrentBobMax.y - mvCurrentBobMax.y;
		mvHeadBob.x = sin(mfHeadBobCount/2 - kPi4f) * mvCurrentBobMax.x;
	}
	else
	{
		mvHeadBob =0;
	}


	/////////////////////////
	//Ground bounce
	if(mbHeadGroundBounceActive)
	{
		mfHeadGroundBounce += afTimeStep * mfGroundBounceSpeed * mfBounceSpeedMul;
		
		//Slow down on way down, and back up go slow, speed up and then slow.
		if(mfHeadGroundBounce < 0.5f)
			mvHeadBob.y += sin(kPif + mfHeadGroundBounce * kPif) * mfGroundBounceSize * mfBounceSizeMul;
		else
			mvHeadBob.y += (cMath::SmoothCurve( (mfHeadGroundBounce-0.5f)*2.0f )-1) * mfGroundBounceSize * mfBounceSizeMul;

		if(mfHeadGroundBounce >= 1)
		{
			mbHeadGroundBounceActive = false;
			mfHeadGroundBounce = 0;
		}
	}
	
	
	mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_Bob,mvHeadBob);
}


//-----------------------------------------------------------------------

bool cLuxMoveState_Normal::UpdateLedgeCheck(float afTimeStep)
{
	//////////////////////////////
	// Skipping this for now! It messes up puzzles too much!
	return false;

	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	////////////////////////////
	//Check player is in the air and pressing jump
	if(pCharBody->IsOnGround() || mpPlayer->IsPressingJump()==false)
	{
		mfClimbLedgeCount =0;
		return false;
	}

	////////////////////////////
	//Check so counter is right
	mfClimbLedgeCount -= afTimeStep;
	if(mfClimbLedgeCount > 0) return false;
	
	mfClimbLedgeCount = mfMaxClimbLedgeCount;
	
	///////////////////////////////
	//Get the position for the ray
	//TODO: Should start a little bit above center, (var is specified in config!)
	cVector3f vStartPos = 	pCharBody->GetFeetPosition() + 
							cVector3f(0,pCharBody->GetSize().y - mfClimbLedgeCheckDistFromTop,0) + 
							pCharBody->GetForward()* (pCharBody->GetSize().x*0.5f + mfClimbLedgeCheckDistForward);
		
	cVector3f vStartPosAdd = cVector3f(0, -mfClimbLedgeCheckDistTopToEnd, 0);

	//////////////////////////////
	// Cast the ray and check if there is some ground ahead.
	float fMinDistance =1000000.0f;
	cVector3f vRayStart[3];
	vRayStart[0] = vStartPos;
	vRayStart[1] = vStartPos + pCharBody->GetRight() * pCharBody->GetSize().x/4;
	vRayStart[2] = vStartPos - pCharBody->GetRight() * pCharBody->GetSize().x/4;
	
	bool bIntersection = false;
	for(int i=0; i<3; ++i)
	{
		float fDistance;
		if(pCharBody->CheckRayIntersection(vRayStart[i], vRayStart[i]+vStartPosAdd, &fDistance, NULL))
		{
			bIntersection = true;
			if(fDistance < fMinDistance) fMinDistance = fDistance;
		}
	}
	
	if(bIntersection== false) return false;

	//////////////////////////////
	// Check if the new position works
	
	//Set up variables
	cVector3f vFeetPos = vStartPos - cVector3f(0,fMinDistance-0.01f,0);
	cVector3f vPushBack=0;
	
	//Check if fit
	bool bFitting = pCharBody->CheckCharacterFits(vFeetPos, true,-1, &vPushBack);

	//Check if the character will fit after applying push back vector.
	if(	bFitting==false && vPushBack.y > 0 && vPushBack.Length() <= mfClimbLedgeMaxPushDist)
	{
		bFitting = pCharBody->CheckCharacterFits(vFeetPos + vPushBack, true);
		if(bFitting) vFeetPos += vPushBack;
    }

	//Check if crouching works
	if( bFitting==false && mbCrouching==false)
	{
		bFitting = pCharBody->CheckCharacterFits(vFeetPos, true,1);
		if(bFitting) SetCrouch(true);
	}
	
	if(bFitting == false) return false;	
	
	//////////////////////////////////////
	// Player fits! Set up stuff

	//Find out how much the new old position differs from the new one and then translate this into
	// the local space of the character and set that as a head position add.
	//This so that it will be possible to fade to the new positon.
	cVector3f vPosDifference = pCharBody->GetFeetPosition() - vFeetPos;
	cVector3f vDiff =0;
	vDiff.y = vPosDifference.y;
	vDiff.z = cMath::Vector3Dot(pCharBody->GetForward(),vPosDifference);
	vDiff.x = cMath::Vector3Dot(pCharBody->GetRight(),vPosDifference);
	mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_ClimbLedge, vDiff);
	
	//Set the new position
	pCharBody->SetFeetPosition(vFeetPos);
	
	//Change the move state
	mpPlayer->ChangeMoveState(eLuxMoveState_ClimbLedge);

	return false;
}

//-----------------------------------------------------------------------

void cLuxMoveState_Normal::FootSound(eLuxFootSound aType)
{
	///////////////////////
	//Get Material
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
    iPhysicsMaterial *pGravityMat = pCharBody->GetGravityCollideMaterial();
	
	tString sMaterialStep;
	
	if(mpPlayer->IsInWater())
	{
		sMaterialStep = mpPlayer->GetWaterStepSound();
	}
	else
	{
		sMaterialStep = pGravityMat ? pGravityMat->GetSurfaceData()->GetStepType() : "rock";
	}
	
	///////////////////////
	//Get Sound data name
	tString sSoundDataName="";
	if(aType == eLuxFootSound_Step)
	{
		tString sMoveType = "walk";
		if(mbRunning) sMoveType = "run";
		if(mbCrouching) sMoveType = "sneak";

		sSoundDataName = "step_"+sMoveType+"_"+sMaterialStep;
	}
	else if(aType == eLuxFootSound_Landing)
	{
		sSoundDataName = "step_run_"+sMaterialStep;
	}

	///////////////////////
	//Play sound
	cLuxSoundExtraData extraData;
	gpBase->mpHelpFuncs->PlayGuiSoundData(sSoundDataName, eSoundEntryType_World,1,eSoundEntityType_Main, true,&extraData);

	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();

	float fVolumeMul = 1.0f;
	float fDistanceMul = 1.0f;

	if (gpBase->mbHardMode)
	{
		if (mbRunning)
		{
			fVolumeMul = 2.0f;
			fDistanceMul = 1.25f;
		}
	}
	
	pMap->BroadcastEnemySoundMessage(	mpPlayer->GetCharacterBody()->GetFeetPosition() + cVector3f(0,0.1f,0),extraData.mfVolume * fVolumeMul,
										extraData.mfMinDistance, extraData.mfMaxDistance * fDistanceMul);
}

//-----------------------------------------------------------------------


void cLuxMoveState_Normal::FallDamage(float afYSpeed)
{
	if(mpPlayer->GetNoFallDamage()) return;

	tString sSound = "";
	float fDamage = 0.0f;

	//Hard
	if(afYSpeed < -mfFallDamageSpeed_Max)
	{
		sSound = msFallDamageSound_Max;
		fDamage = mfFallDamage_Max;
	}
	//Medium
	else if(afYSpeed < -mfFallDamageSpeed_Med)
	{
		sSound = msFallDamageSound_Med;
		fDamage = mfFallDamage_Med;
	}
	//Light
	else if(afYSpeed < -mfFallDamageSpeed_Min)
	{
		sSound = msFallDamageSound_Min;
		fDamage = mfFallDamage_Min;
	}

	if(fDamage >0)
	{
		mfBounceSizeMul = mfFallDamageBounceSizeMul;
		mfBounceSpeedMul = mfFallDamageBounceSpeedMul;
		mpPlayer->GiveDamage(fDamage,100,eLuxDamageType_BloodSplat, false, false);
		gpBase->mpHelpFuncs->PlayGuiSoundData(sSound, eSoundEntryType_World);
	}
}

//-----------------------------------------------------------------------




