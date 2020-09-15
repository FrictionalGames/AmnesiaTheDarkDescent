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

#ifndef LUX_MOVE_STATE_CLIMB_LEDGE_H
#define LUX_MOVE_STATE_CLIMB_LEDGE_H

//----------------------------------------------

#include "LuxMoveState.h"

//----------------------------------------------

class cLuxMoveState_ClimbLedge : public iLuxMoveState
{
public:	
	cLuxMoveState_ClimbLedge(cLuxPlayer *apPlayer);
	~cLuxMoveState_ClimbLedge();
	
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

	void OnDraw(float afFrameTime);
	
private:
	float mfHeadMoveSpeed;
	float mfHeadMoveSlowdownDist;
	float mfGivePlayerControlDist;
	float mfMovePitchFactor;
	float mfMaxMovePitchSpeed;
};

//----------------------------------------------


#endif // LUX_MOVE_STATE_CLIMB_LEDGE_H
