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

#include "LuxMoveState.h"

#include "LuxPlayer.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxMoveState::iLuxMoveState(cLuxPlayer *apPlayer)
{
	mpPlayer = apPlayer;

	mfMaxForwardSpeed = 0.0f;
	mfMaxBackwardSpeed = 0.0f;
	mfMaxSidwaySpeed = 0.0f;
	
	mfForwardAcc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardAcc",0);
	mfSidewayAcc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayAcc",0);
	mfForwardDeacc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardDeacc",0);
	mfSidewayDeacc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayDeacc",0);

	mfForwardOppositeDirAccMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardOppositeDirAccMul",0);
	mfSidewayOppositeDirAccMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayOppositeDirAccMul",0);

	mfMaxForwardSpeedMul = 1.0f;
	mfMaxBackwardSpeedMul = 1.0f;
	mfMaxSidwaySpeedMul = 1.0f;
}

//-----------------------------------------------------------------------

iLuxMoveState::~iLuxMoveState()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxMoveState::Update(float afTimeStep)
{
	//////////////////////////////
	// Move state specific update
	OnUpdate(afTimeStep);

	//////////////////////////////
	// Set move state variables
	iCharacterBody *pCharBody = gpBase->mpPlayer->GetCharacterBody();	

	pCharBody->SetMaxPositiveMoveSpeed(eCharDir_Forward,mfMaxForwardSpeed * mfMaxForwardSpeedMul);
	pCharBody->SetMaxNegativeMoveSpeed(eCharDir_Forward,-mfMaxBackwardSpeed * mfMaxBackwardSpeedMul);
	pCharBody->SetMaxPositiveMoveSpeed(eCharDir_Right,mfMaxSidwaySpeed * mfMaxSidwaySpeedMul);
	pCharBody->SetMaxNegativeMoveSpeed(eCharDir_Right,-mfMaxSidwaySpeed * mfMaxSidwaySpeedMul);
	pCharBody->SetMoveAcc(eCharDir_Forward,mfForwardAcc);
	pCharBody->SetMoveAcc(eCharDir_Right,mfSidewayAcc);
	pCharBody->SetMoveDeacc(eCharDir_Forward,mfForwardDeacc);
	pCharBody->SetMoveDeacc(eCharDir_Right,mfSidewayDeacc);
	pCharBody->SetMoveOppositeDirAccMul(eCharDir_Forward,mfForwardOppositeDirAccMul);
	pCharBody->SetMoveOppositeDirAccMul(eCharDir_Right,	mfSidewayOppositeDirAccMul);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------



