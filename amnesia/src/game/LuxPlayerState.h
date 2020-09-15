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

#ifndef LUX_PLAYER_STATE_H
#define LUX_PLAYER_STATE_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxTypes.h"

//----------------------------------------------

class cLuxPlayer;
class iLuxProp;
class cLuxMap;
class cLuxInventory_Item;
class cLuxArea_Ladder;

//----------------------------------------------

class cLuxPlayerStateVars
{
public:
	static void SetupHandObject(const tString& asHandObject){ msHandObject = asHandObject; }
	static void SetupInteraction(iPhysicsBody *apBody, const cVector3f &avInteractPos);
	static void SetupUseItem(cLuxInventory_Item *apItem){mpInventoryItem = apItem; }
	static void SetupLadder(cLuxArea_Ladder *apLadder, const cVector3f &avStartPos){mpLadder = apLadder; mvLadderStartPos = avStartPos;}

	///////////////////////
	//Ladder
	static cLuxArea_Ladder *mpLadder;
	static cVector3f mvLadderStartPos;
	
	///////////////////////
	//Interaction
	static tString msHandObject;

	///////////////////////
	//Use item
	static cLuxInventory_Item *mpInventoryItem;

	///////////////////////
	//General
	static cVector3f mvInteractPos;
	
	static iPhysicsBody *mpInteractBody;
	static iLuxProp *mpInteractProp;
};


//----------------------------------------------

class iLuxPlayerState_SaveData : public iSerializable
{
	kSerializableClassInit(iLuxPlayerState_SaveData)
public:
	int mlPreviousState;
};

//----------------------------------------------

class iLuxPlayerState
{
public:	
	iLuxPlayerState(cLuxPlayer *apPlayer, eLuxPlayerState aType);
	virtual ~iLuxPlayerState();

	virtual void OnEnterState(eLuxPlayerState aPrevState)=0;
	virtual void OnLeaveState(eLuxPlayerState aNewState)=0;

	virtual void OnMapEnter(cLuxMap *apMap){}
	virtual void OnMapLeave(cLuxMap *apMap){}

	virtual void Update(float afTimeStep)=0;
	virtual void PostUpdate(float afTimeStep)=0;
	virtual void OnDraw(cGuiSet *apGuiSet ,float afFrameTime){}

	virtual bool OnDoAction(eLuxPlayerAction aAction,bool abPressed)=0;
	
	virtual void OnScroll(float afAmount){}

	virtual bool OnMove(eCharDir aDir, float afMul){return true;}

	virtual bool OnAddYaw(float afAmount){ return true;}
	virtual bool OnAddPitch(float afAmount){ return true;}

	virtual bool OnRun(bool abPressed){ return true;}
	virtual bool OnCrouch(bool abPressed){ return true;}
	virtual bool OnJump(bool abPressed){ return true;}

	virtual cGuiGfxElement* GetCrosshair(){ return mpDefaultCrosshairGfx;}
	virtual bool OnDrawCrossHair(cGuiGfxElement *apGfx, const cVector3f& avPos, const cVector2f &avSize){ return true;}

	virtual void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter)=0;
	virtual bool AllowBuoyancy(iPhysicsBody *apBody){ return true;}
	virtual void OnDestroyEntity(iLuxEntity *apEntity){}
	virtual void OnAttachBodyToStickyArea(iPhysicsBody *apBody){}

	virtual float DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY)=0;

	virtual bool AllowPlayerMenus(){ return true;}
	virtual bool AllowLantern(){ return true;}
	
	virtual void RenderSolid(cRendererCallbackFunctions* apFunctions){}
	virtual void RenderTrans(cRendererCallbackFunctions* apFunctions){}

	void SetPreviousState(eLuxPlayerState aState) { mPreviousState = aState; }

	/////////////////////////////////
	//Save data stuff
	virtual bool IsSaved()=0;
	virtual iLuxPlayerState_SaveData* CreateSaveData()=0;
	
	virtual void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
	virtual void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
	eLuxPlayerState mType; 
	cLuxPlayer *mpPlayer;

	cGuiGfxElement* mpDefaultCrosshairGfx;
	cGuiGfxElement* mpSimpleInteractCrosshairGfx;
	std::vector<cGuiGfxElement*> mvFocusCrosshairGfx;

	eLuxPlayerState mPreviousState;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_H
