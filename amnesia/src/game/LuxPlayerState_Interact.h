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

#ifndef LUX_PLAYER_STATE_INTERACT_H
#define LUX_PLAYER_STATE_INTERACT_H

//----------------------------------------------

#include "LuxPlayerState.h"

//----------------------------------------------

class iLuxProp;

//----------------------------------------------

class iLuxPlayerState_Interact_SaveData : public iLuxPlayerState_SaveData
{
	kSerializableClassInit(iLuxPlayerState_Interact_SaveData)
public:
	int mlCurrentPropId;
	int mlCurrentBodyId;
	cVector3f mvCurrentFocusPos;
};

//----------------------------------------------

class iLuxPlayerState_Interact : public iLuxPlayerState
{
typedef iLuxPlayerState super_class;
public:	
	iLuxPlayerState_Interact(cLuxPlayer *apPlayer, eLuxPlayerState aType);
	virtual ~iLuxPlayerState_Interact();
	
	void OnDestroyEntity(iLuxEntity *apEntity);
	void OnAttachBodyToStickyArea(iPhysicsBody *apBody);

    /////////////////////////////////
	//Save data stuff
	virtual void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	void SetupInteractVars();
	void ResetInteractVars();

	iPhysicsBody *mpCurrentBody;
	iLuxProp *mpCurrentProp;
	cVector3f mvCurrentFocusPos;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_H
