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

#include "LuxPlayerState_Normal.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState.h"
#include "LuxEffectRenderer.h"
#include "LuxPlayerHands.h"



//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_Normal::cLuxPlayerState_Normal(cLuxPlayer *apPlayer) : iLuxPlayerState_DefaultBase(apPlayer, eLuxPlayerState_Normal)
{
}

//-----------------------------------------------------------------------

cLuxPlayerState_Normal::~cLuxPlayerState_Normal()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxPlayerState_Normal::ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	////////////////////
	// Holster (draw hand object)
	if(aAction == eLuxPlayerAction_Holster && abPressed)
	{
		mpPlayer->ChangeState(eLuxPlayerState_HandObject);
		return false;
	}

	return true;
}

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

kBeginSerialize(cLuxPlayerState_Normal_SaveData, iLuxPlayerState_DefaultBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_Normal::CreateSaveData()
{
	return hplNew(cLuxPlayerState_Normal_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_Normal::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_Normal_SaveData *pData = static_cast<cLuxPlayerState_Normal_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Normal::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_Normal_SaveData *pData = static_cast<cLuxPlayerState_Normal_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_Normal::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_Normal_SaveData *pData = static_cast<cLuxPlayerState_Normal_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------


