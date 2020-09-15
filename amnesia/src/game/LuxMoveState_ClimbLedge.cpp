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

#include "LuxMoveState_ClimbLedge.h"

#include "LuxPlayer.h"
#include "LuxHelpFuncs.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMoveState_ClimbLedge::cLuxMoveState_ClimbLedge(cLuxPlayer *apPlayer) : iLuxMoveState(apPlayer)
{
	mfHeadMoveSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","HeadMoveSpeed",0);
	mfHeadMoveSlowdownDist = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","HeadMoveSlowdownDist",0);
	
	mfGivePlayerControlDist = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","GivePlayerControlDist",0);
	
	mfMovePitchFactor = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","MovePitchFactor",0);	
	mfMaxMovePitchSpeed = gpBase->mpGameCfg->GetFloat("Player_Movement_ClimbLedge","MaxMovePitchSpeed",0);	
}

//-----------------------------------------------------------------------

cLuxMoveState_ClimbLedge::~cLuxMoveState_ClimbLedge()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnMapEnter()
{
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnEnterState(eLuxMoveState aPrevState)
{
	iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();

	////////////////////////////
	// Player and character.

	//These values must go in config!
	pCharBody->SetForceVelocity(0);
	mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_ClimbLedge,0, mfHeadMoveSpeed, mfHeadMoveSlowdownDist);


	////////////////////////////
	// Set up variables


	////////////////////////////
	// Play sound
	gpBase->mpHelpFuncs->PlayGuiSoundData("player_climb", eSoundEntryType_World);
}

void cLuxMoveState_ClimbLedge::OnLeaveState(eLuxMoveState aNewState)
{
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnUpdate(float afTimeStep)
{
	///////////////////////////////////
	//See at the top of the climbing, if so end
	if(mpPlayer->GetHeadPosAdd(eLuxHeadPosAdd_ClimbLedge).Length() <= mfGivePlayerControlDist)
	{
		mpPlayer->ChangeMoveState(eLuxMoveState_Normal);
		return;
	}

	///////////////////////////////////
	//Move the pitch to 0
	float fPitch = mpPlayer->GetCamera()->GetPitch();
	float fAdd = -fPitch * mfMovePitchFactor;
	if(fAdd < -mfMaxMovePitchSpeed) fAdd = -mfMaxMovePitchSpeed; 
	if(fAdd > mfMaxMovePitchSpeed)  fAdd = mfMaxMovePitchSpeed;
    mpPlayer->GetCamera()->AddPitch(fAdd * afTimeStep);
}

//-----------------------------------------------------------------------

bool cLuxMoveState_ClimbLedge::GetAllowsInteraction()
{
	return false;
}

//-----------------------------------------------------------------------

bool cLuxMoveState_ClimbLedge::OnMove(eCharDir aDir, float afMul)
{
	return false;
}
bool cLuxMoveState_ClimbLedge::OnAddYaw(float afAmount)
{
	return true;
}
bool cLuxMoveState_ClimbLedge::OnAddPitch(float afAmount)
{
	return true;
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnRun(bool abActive)
{
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnCrouch(bool abActive)
{
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnJump(bool abActive)
{
	
}

//-----------------------------------------------------------------------

void cLuxMoveState_ClimbLedge::OnDraw(float afFrameTime)
{
	
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------



//-----------------------------------------------------------------------




