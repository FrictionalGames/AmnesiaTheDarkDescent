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

#include "LuxPlayerState.h"

#include "LuxPlayer.h"
#include "LuxProp.h"

//////////////////////////////////////////////////////////////////////////
// SETUP VARIABLES
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

///////////////////////
//General
tString cLuxPlayerStateVars::msHandObject = "";

///////////////////////
//Interaction
cVector3f cLuxPlayerStateVars::mvInteractPos;
iPhysicsBody *cLuxPlayerStateVars::mpInteractBody = NULL;
iLuxProp *cLuxPlayerStateVars::mpInteractProp = NULL;

///////////////////////
//Use Item
cLuxInventory_Item *cLuxPlayerStateVars::mpInventoryItem  = NULL;

///////////////////////
//Ladder
cLuxArea_Ladder *cLuxPlayerStateVars::mpLadder = NULL;
cVector3f cLuxPlayerStateVars::mvLadderStartPos = 0;
//-----------------------------------------------------------------------

void cLuxPlayerStateVars::SetupInteraction(iPhysicsBody *apBody, const cVector3f &avInteractPos)
{
	mpInteractBody = apBody;
	
	iLuxEntity *pEntity = (iLuxEntity*)apBody->GetUserData();
	if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
	{
		mpInteractProp = static_cast<iLuxProp*>(pEntity);
	}

	mvInteractPos = avInteractPos;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPlayerState::iLuxPlayerState(cLuxPlayer *apPlayer, eLuxPlayerState aType)
{
	mpPlayer = apPlayer;

	mType = aType;

	cGui *pGui = gpBase->mpEngine->GetGui();
	mpDefaultCrosshairGfx = pGui->CreateGfxImage("hud_crosshair_default.tga",eGuiMaterial_Alpha);
	mpSimpleInteractCrosshairGfx = pGui->CreateGfxImage("hud_crosshair_active.tga",eGuiMaterial_Alpha);
	
	mPreviousState = eLuxPlayerState_LastEnum;
}

//-----------------------------------------------------------------------

iLuxPlayerState::~iLuxPlayerState()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBaseVirtual(iLuxPlayerState_SaveData)
kSerializeVar(mlPreviousState, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

void iLuxPlayerState::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	apSaveData->mlPreviousState = mPreviousState;
}

//-----------------------------------------------------------------------

void iLuxPlayerState::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{

}

//-----------------------------------------------------------------------

void iLuxPlayerState::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	mPreviousState = (eLuxPlayerState)apSaveData->mlPreviousState;
}

//-----------------------------------------------------------------------


