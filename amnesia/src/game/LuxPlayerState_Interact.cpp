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

#include "LuxPlayerState_Interact.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxMap.h"
#include "LuxProp.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPlayerState_Interact::iLuxPlayerState_Interact(cLuxPlayer *apPlayer, eLuxPlayerState aType) : iLuxPlayerState(apPlayer, aType)
{
	mpCurrentBody = NULL;
	mpCurrentProp = NULL;
	mvCurrentFocusPos = 0;
}

//-----------------------------------------------------------------------

iLuxPlayerState_Interact::~iLuxPlayerState_Interact()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::OnDestroyEntity(iLuxEntity *apEntity)
{
	iLuxEntity *pCurrentEntity = mpCurrentProp;
	if(pCurrentEntity == apEntity)
	{
		gpBase->mpPlayer->ChangeState(mPreviousState);
	}
}

void iLuxPlayerState_Interact::OnAttachBodyToStickyArea(iPhysicsBody *apBody)
{
	bool bBodyInProp = false;
	for(int i=0; i<mpCurrentProp->GetBodyNum(); ++i)
	{
		if(mpCurrentProp->GetBody(i) == apBody) 
		{
			bBodyInProp = true;
			break;
		}
	}

    if(bBodyInProp)
	{
		mpPlayer->ChangeState(mPreviousState);
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::SetupInteractVars()
{
	mpCurrentProp = cLuxPlayerStateVars::mpInteractProp;
	mpCurrentBody = cLuxPlayerStateVars::mpInteractBody;
	mvCurrentFocusPos = cLuxPlayerStateVars::mvInteractPos;

	mpCurrentProp->SetIsInteractedWith(true);
}	

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::ResetInteractVars()
{
	mpCurrentProp->SetIsInteractedWith(false);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxPlayerState_Interact_SaveData, iLuxPlayerState_SaveData)
kSerializeVar(mlCurrentPropId, eSerializeType_Int32)
kSerializeVar(mlCurrentBodyId, eSerializeType_Int32)
kSerializeVar(mvCurrentFocusPos, eSerializeType_Vector3f)
kEndSerialize()

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	iLuxPlayerState_Interact_SaveData *pData = static_cast<iLuxPlayerState_Interact_SaveData*>(apSaveData);

	if(mpCurrentProp && mpCurrentBody)
	{
		pData->mlCurrentBodyId = mpCurrentBody->GetUniqueID();
		pData->mlCurrentPropId = mpCurrentProp->GetID();
	}
	else
	{
		pData->mlCurrentPropId = -1;
		pData->mlCurrentBodyId = -1;
	}
	kCopyToVar(pData, mvCurrentFocusPos);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	iLuxPlayerState_Interact_SaveData *pData = static_cast<iLuxPlayerState_Interact_SaveData*>(apSaveData);
	
	//////////////////////
	//Setup interaction vars
	iPhysicsBody* pFoucsBody=NULL;
	iLuxEntity *pEntity = apMap->GetEntityByID(pData->mlCurrentPropId);
	if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
		pFoucsBody = pProp->GetBodyFromID(pData->mlCurrentBodyId);
		if(pFoucsBody==NULL)
		{
			Error("Could not load body with id %d in prop '%s'\n", pData->mlCurrentBodyId, mpCurrentProp->GetName().c_str());
		}
	}
	else
	{
		Error("Could not find prop with id %d\n", pData->mlCurrentPropId);
	}

	if(pFoucsBody)
	{
		//TODO: Add pos here!
		cLuxPlayerStateVars::SetupInteraction(pFoucsBody, mvCurrentFocusPos);
	}
}

//-----------------------------------------------------------------------

void iLuxPlayerState_Interact::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	iLuxPlayerState_Interact_SaveData *pData = static_cast<iLuxPlayerState_Interact_SaveData*>(apSaveData);

}

//-----------------------------------------------------------------------




