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

#ifndef LUX_PLAYER_STATE_HANDOBJECT_H
#define LUX_PLAYER_STATE_HANDOBJECT_H

//----------------------------------------------

#include "LuxPlayerState_DefaultBase.h"

//----------------------------------------------

class cLuxPlayer;

//------------------------------------

class cLuxPlayerState_HandObject_SaveData : public iLuxPlayerState_DefaultBase_SaveData
{
	kSerializableClassInit(cLuxPlayerState_HandObject_SaveData)
public:

};

//----------------------------------------------

class cLuxPlayerState_HandObject : public iLuxPlayerState_DefaultBase
{
typedef iLuxPlayerState_DefaultBase super_class;
public:	
	cLuxPlayerState_HandObject(cLuxPlayer *apPlayer);
	virtual ~cLuxPlayerState_HandObject();

	
	bool ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed);
	void ImplementedOnEnterState(eLuxPlayerState aPrevState);
	void ImplementedOnLeaveState(eLuxPlayerState aNewState);
	
	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return true; }
	iLuxPlayerState_SaveData* CreateSaveData();

	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_HANDOBJECT_H
