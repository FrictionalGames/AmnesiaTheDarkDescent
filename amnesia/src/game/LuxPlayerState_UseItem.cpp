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

#include "LuxPlayerState_UseItem.h"

#include "LuxPlayer.h"
#include "LuxInventory.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEntity.h"
#include "LuxProp.h"
#include "LuxProp_Lamp.h"
#include "LuxMessageHandler.h"



//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_UseItem::cLuxPlayerState_UseItem(cLuxPlayer *apPlayer) : iLuxPlayerState_DefaultBase(apPlayer, eLuxPlayerState_UseItem)
{
	mpCurrentItem = NULL; 
	mFlashOscill.SetUp(0,1,0,1.5f,1.5f);

	mfMinUseItemDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction", "MinUseItemDistance",0);
}

//-----------------------------------------------------------------------

cLuxPlayerState_UseItem::~cLuxPlayerState_UseItem()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayerState_UseItem::ImplementedOnEnterState(eLuxPlayerState aPrevState)
{
	mpCurrentItem = cLuxPlayerStateVars::mpInventoryItem;
	cLuxPlayerStateVars::mpInventoryItem = NULL;
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_UseItem::ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed)
{
	////////////////////
	// Holster (draw hand object)
	if(aAction == eLuxPlayerAction_Holster && abPressed)
	{
		mpPlayer->ChangeState(eLuxPlayerState_HandObject);
		return false;
	}
	////////////////////////////
	// Interact
	if(aAction == eLuxPlayerAction_Interact || aAction == eLuxPlayerAction_Attack)
	{
		// Pressed
		if(abPressed)
		{
			UseItem();
			
			mpPlayer->ChangeState(eLuxPlayerState_Normal);
            return false;
		}
	}

	
	return true;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_UseItem::ImplementedUpdate(float afTimeStep)
{
	mFlashOscill.Update(afTimeStep);
}

//-----------------------------------------------------------------------

cGuiGfxElement* cLuxPlayerState_UseItem::GetCrosshair()
{
	if(mpCurrentItem==NULL) return NULL;

	return mpCurrentItem->GetImage();
}

//-----------------------------------------------------------------------

bool cLuxPlayerState_UseItem::OnDrawCrossHair(cGuiGfxElement *apGfx, const cVector3f& avPos, const cVector2f &avSize)
{
	if(mpEntityInFocus==NULL) return true;

	float fMaxFocusDistance = cMath::Max(mpEntityInFocus->GetMaxFocusDistance(), mfMinUseItemDistance);
	if(mfFocusDistance > fMaxFocusDistance) return true;	

	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(	mpEntityInFocus->GetEntityType() == eLuxEntityType_Prop)
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(mpEntityInFocus);
		if(pProp->GetPropType() == eLuxPropType_Item) return true;
	}
	else
	{
		if(pMap->GetUseItemCallback(mpCurrentItem->GetName(), mpEntityInFocus->GetName())==NULL) return true;
	}

	cVector3f vNewPos = avPos;
	vNewPos.z += 1;

	for(int i=0; i<3; ++i)
		gpBase->mpGameHudSet->DrawGfx(apGfx, vNewPos, avSize, cColor(mFlashOscill.val), eGuiMaterial_Additive);
	return true;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxPlayerState_UseItem::ShowOutlineOnEntity(iLuxEntity *apEntity, iPhysicsBody *apBody, const cVector3f &avFocusPos)
{
	return false;

	/*cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap==NULL) return false;
	
	/////////////////////////////////
	// Tinder box
	if(mpCurrentItem->GetType() == eLuxItemType_Tinderbox)
	{
		if(apEntity->GetEntityType()!=eLuxEntityType_Prop) return false;

		iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);
		if(pProp->GetPropType() !=eLuxPropType_Lamp) return false;

		cLuxProp_Lamp *pLamp = static_cast<cLuxProp_Lamp*>(pProp);
		return pLamp->CanBeIgnitByPlayer();
	}
	/////////////////////////////////
	// Puzzle item
	else
	{
		if(apEntity->CanInteract(apBody))
		{
			if(apEntity->GetEntityType() == eLuxEntityType_Prop)
			{
				iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);
				if(pProp->GetPropType() != eLuxPropType_Item) return true;
			}
			else
			{
				return true;
			}
		}

		return pMap->GetUseItemCallback(mpCurrentItem->GetName(), apEntity->GetName())!=NULL;
	}*/
}

//-----------------------------------------------------------------------

void cLuxPlayerState_UseItem::UseItem()
{
	if(mpCurrentItem==NULL) return;

	iLuxEntity *pEntity = mpEntityInFocus;
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();

	/////////////////////////
	//An object is in focus
	if(pEntity && pMap && mfFocusDistance < cMath::Max(pEntity->GetMaxFocusDistance(), mfMinUseItemDistance))
	{
		cLuxUseItemCallback *pCallback = pMap->GetUseItemCallback(mpCurrentItem->GetName(), pEntity->GetName());
		if(pCallback)
		{
            // Running the script MAY destroy this item so "Backup" the check flag.
            bool bAutoDestroy = pCallback->mbAutoDestroy;
			tString sName = pCallback->msName;
            pMap->RunScript(pCallback->msFunction+ "(\"" + pCallback->msItem + "\", \"" + pCallback->msEntity + "\")" );

			if(bAutoDestroy)
			{
				pMap->RemoveUseItemCallback(pCallback, sName);
			}
		}
		else
		{
			gpBase->mpMessageHandler->SetMessage(kTranslate("Inventory","UseItemDoesNotWork"), 0);
		}
	}
	/////////////////////////
	//No object in focus
	else
	{
		gpBase->mpMessageHandler->SetMessage(kTranslate("Inventory","UseItemHasNoObject"), 0);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_UseItem_SaveData, iLuxPlayerState_DefaultBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_UseItem::CreateSaveData()
{
	return hplNew(cLuxPlayerState_UseItem_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_UseItem::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxPlayerState_UseItem_SaveData *pData = static_cast<cLuxPlayerState_UseItem_SaveData*>(apSaveData);


	///////////////////////
	// Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_UseItem::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
	cLuxPlayerState_UseItem_SaveData *pData = static_cast<cLuxPlayerState_UseItem_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_UseItem::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
	cLuxPlayerState_UseItem_SaveData *pData = static_cast<cLuxPlayerState_UseItem_SaveData*>(apSaveData);

	///////////////////////
	// Load vars
}

//-----------------------------------------------------------------------


