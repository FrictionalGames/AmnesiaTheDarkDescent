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

#ifndef LUX_PLAYER_STATE_INTERACT_WHEEL_H
#define LUX_PLAYER_STATE_INTERACT_WHEEL_H

//----------------------------------------------

#include "LuxPlayerState_InteractRotateBase.h"

//----------------------------------------------

class cLuxInteractData_Wheel;

//------------------------------------

class cLuxPlayerState_InteractWheel_SaveData : public iLuxPlayerState_InteractRotateBase_SaveData
{
	kSerializableClassInit(cLuxPlayerState_InteractWheel_SaveData)
public:

};
//----------------------------------------------

class cLuxPlayerState_InteractWheel : public iLuxPlayerState_InteractRotateBase
{
typedef iLuxPlayerState_InteractRotateBase super_class;
public:	
	cLuxPlayerState_InteractWheel(cLuxPlayer *apPlayer);
	virtual ~cLuxPlayerState_InteractWheel();

	void RenderSolid(cRendererCallbackFunctions* apFunctions);

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return false; }
	iLuxPlayerState_SaveData* CreateSaveData();

	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	void EnterRotateBase(eLuxPlayerState aPrevState);
	void LeaveRotateBase(eLuxPlayerState aNewState);

	float GetSpeedAdd(cCamera *apCam);
	void OnThrow();

	cLuxInteractData_Wheel *mpWheelData;

	std::list<cVector2f> mlstMoveAdds;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_WHEEL_H
