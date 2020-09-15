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

#ifndef LUX_PLAYER_STATE_DEFAULT_BASE_H
#define LUX_PLAYER_STATE_DEFAULT_BASE_H

//----------------------------------------------

#include "LuxPlayerState.h"

//----------------------------------------------

class cLuxPlayer;

//------------------------------------

class iLuxPlayerState_DefaultBase_SaveData : public iLuxPlayerState_SaveData
{
	kSerializableClassInit(iLuxPlayerState_DefaultBase_SaveData)
public:

};


//----------------------------------------------

class iLuxPlayerState_DefaultBase : public iLuxPlayerState
{
typedef iLuxPlayerState super_class;
public:	
	iLuxPlayerState_DefaultBase(cLuxPlayer *apPlayer, eLuxPlayerState aType);
	virtual ~iLuxPlayerState_DefaultBase();

	void OnEnterState(eLuxPlayerState aPrevState);
	void OnLeaveState(eLuxPlayerState aNewState);

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);
	virtual void OnDraw(cGuiSet *apGuiSet ,float afFrameTime);

	virtual cGuiGfxElement* GetCrosshair();

	bool OnDoAction(eLuxPlayerAction aAction,bool abPressed);

	void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter){}

	float DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY);

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved(){ return true; }
	
	void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	virtual bool ShowOutlineOnEntity(iLuxEntity *apEntity, iPhysicsBody *apBody, const cVector3f &avFocusPos);

	virtual void ImplementedUpdate(float afTimeStep){}
	virtual bool ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed){return true;}
	virtual void ImplementedOnEnterState(eLuxPlayerState aPrevState){}
	virtual void ImplementedOnLeaveState(eLuxPlayerState aNewState){}

	bool CanInteractWithEntity();	

	void AddOutlineObjects(iPhysicsBody *apBody, iLuxEntity *apEntity, const cVector3f &avFocusPos);
	void GetAttachedBodies(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies);

	iPhysicsBody *mpBodyInFocus;
	iLuxEntity *mpEntityInFocus;
	float mfFocusDistance;
	cVector3f mvFocusPos;
	bool mbCurrentEntityHasOutline;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_DEFAULT_BASE_H
