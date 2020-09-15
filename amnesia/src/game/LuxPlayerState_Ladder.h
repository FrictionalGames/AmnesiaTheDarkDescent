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

#ifndef LUX_PLAYER_STATE_LADDER_H
#define LUX_PLAYER_STATE_LADDER_H

//----------------------------------------------

#include "LuxPlayerState_DefaultBase.h"

//----------------------------------------------

class cLuxPlayer;
class cLuxArea_Ladder;

//------------------------------------

class cLuxPlayerState_Ladder_SaveData : public iLuxPlayerState_SaveData
{
	kSerializableClassInit(cLuxPlayerState_Ladder_SaveData)
public:
	int mlState;
	cVector3f mvStartPosition;

	int mlLadderID;

	float mfTimeCount;

	cVector3f mvCharPosition;

	cVector3f mvGoalPos;
	cVector3f mvGoalRot;

	cVector3f mvPosAdd;
	cVector3f mvRotAdd;

	float mfMoveMul;

	float mfUpSpeed;
	float mfDownSpeed;

	float mfStepLength;
	float mfStepCount;
	bool mbPlayedSound;

	float mfPitchMaxLimit;
	float mfPitchMinLimit;

	float mfLeaveAtTopCount;

	bool mbLanternDrawn;
};

//----------------------------------------------

class cLuxPlayerState_Ladder : public iLuxPlayerState
{
typedef iLuxPlayerState super_class;
public:	
	cLuxPlayerState_Ladder(cLuxPlayer *apPlayer);
	virtual ~cLuxPlayerState_Ladder();

	void OnEnterState(eLuxPlayerState aPrevState);
	void OnLeaveState(eLuxPlayerState aNewState);

	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);
	virtual void OnDraw(cGuiSet *apGuiSet ,float afFrameTime);

	virtual cGuiGfxElement* GetCrosshair();

	bool OnMove(eCharDir aDir, float afMul);

	bool OnAddYaw(float afAmount);
	bool OnAddPitch(float afAmount);

	bool OnRun(bool abPressed);
	bool OnJump(bool abPressed);
	bool OnCrouch(bool abPressed);

	bool OnDoAction(eLuxPlayerAction aAction,bool abPressed);
	
	void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter){}

	float DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY);

	bool AllowPlayerMenus(){ return false;}

	bool AllowLantern(){ return false;}
	
	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return true; }
	iLuxPlayerState_SaveData* CreateSaveData();

	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

private:
	void SetupHeadTurnLimits();
	void PlaySound(const tString &asType);

	int mlState;
	cLuxArea_Ladder* mpLadder;
	cVector3f mvStartPosition;

	float mfTimeCount;

	cVector3f mvCharPosition;

	cVector3f mvGoalPos;
	cVector3f mvGoalRot;

	cVector3f mvPosAdd;
	cVector3f mvRotAdd;

	float mfMoveMul;

	float mfUpSpeed;
	float mfDownSpeed;

	float mfStepLength;
	float mfStepCount;
	bool mbPlayedSound;

	float mfPitchMaxLimit;
	float mfPitchMinLimit;
	
	float mfLeaveAtTopCount;

	bool mbLanternDrawn;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_Ladder_H
