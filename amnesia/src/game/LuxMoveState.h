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

#ifndef LUX_MOVE_STATE_H
#define LUX_MOVE_STATE_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxTypes.h"

//----------------------------------------------

class cLuxPlayer;

//----------------------------------------------

class iLuxMoveState
{
public:	
	iLuxMoveState(cLuxPlayer *apPlayer);
	virtual ~iLuxMoveState();

	virtual void OnMapEnter()=0;

	virtual void OnEnterState(eLuxMoveState aPrevState)=0;
	virtual void OnLeaveState(eLuxMoveState aNewState)=0;

	void Update(float afTimeStep);

	virtual bool GetAllowsInteraction()=0;

	virtual bool OnMove(eCharDir aDir, float afMul)=0;
	virtual bool OnAddYaw(float afAmount)=0;
	virtual bool OnAddPitch(float afAmount)=0;

	virtual void OnRun(bool abActive)=0;
	virtual void OnCrouch(bool abActive)=0;
	virtual void OnJump(bool abActive)=0;

	virtual void OnDraw(float afFrameTime)=0;
	
protected:
	virtual void OnUpdate(float afTimeStep)=0;

	cLuxPlayer *mpPlayer;

	float mfMaxForwardSpeed;
	float mfMaxBackwardSpeed;
	float mfMaxSidwaySpeed;
	float mfForwardAcc;
	float mfSidewayAcc;
	float mfForwardDeacc;
	float mfSidewayDeacc;

	float mfForwardOppositeDirAccMul;
	float mfSidewayOppositeDirAccMul;

	float mfMaxForwardSpeedMul;
	float mfMaxBackwardSpeedMul;
	float mfMaxSidwaySpeedMul;
};

//----------------------------------------------


#endif // LUX_MOVE_STATE_H
