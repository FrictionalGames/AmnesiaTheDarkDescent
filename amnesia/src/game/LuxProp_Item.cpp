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

#include "LuxProp_Item.h"

#include "LuxPlayer.h"
#include "LuxMap.h"
#include "LuxHelpFuncs.h"
#include "LuxInventory.h"
#include "LuxItemType.h"
#include "LuxMessageHandler.h"
#include "LuxEffectRenderer.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Item::cLuxPropLoader_Item(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Item_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Item::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Item, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Item::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Item  *pItem = static_cast<cLuxProp_Item*>(apProp);

	///////////////////////////
	// General
	pItem->mItemType = gpBase->mpInventory->GetItemTypeFromString(msEntitySubType);

	pItem->msImageFile = GetVarString("ImageFile", "");
	pItem->msPickSound = GetVarString("PickSound", "Generic");
	pItem->msSubItemTypeName = GetVarString("SubItemTypeName", "");

	///////////////////////////
	// Coins
	if(pItem->mItemType == eLuxItemType_Coins)
	{
		pItem->mfAmount = GetVarFloat("CoinAmount", 0);
	}
	///////////////////////////
	// Health
	else if(pItem->mItemType == eLuxItemType_Health)
	{
		pItem->mfAmount = GetVarFloat("HealAmount", 0);
	}
	///////////////////////////
	// Sanity
	else if(pItem->mItemType == eLuxItemType_Sanity)
	{
		pItem->mfAmount = GetVarFloat("HealAmount", 0);
	}
	///////////////////////////
	// Lamp Oil
	else if(pItem->mItemType == eLuxItemType_LampOil)
	{
		pItem->mfAmount = GetVarFloat("OilAmount", 0);
	}
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Item::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Item  *pItem = static_cast<cLuxProp_Item*>(apProp);

	///////////////////////////
	// All
	tString sCustomSubItemTypeName = apInstanceVars->GetVarString("CustomSubItemTypeName","");	

    if(sCustomSubItemTypeName != "") pItem->msSubItemTypeName = sCustomSubItemTypeName;

	///////////////////////////
	// Note
	if(pItem->mItemType == eLuxItemType_Note)
	{
		pItem->msVal = apInstanceVars->GetVarString("NoteText", "");
		pItem->mfAmount = apInstanceVars->GetVarBool("OpenNoteInJournal", true) ? 1 : -1;
		pItem->mfAmount *= apInstanceVars->GetVarBool("UseNarration", false) ? 2 : 1;
	}
	///////////////////////////
	// Diary
	else if(pItem->mItemType == eLuxItemType_Diary)
	{
		pItem->msVal = apInstanceVars->GetVarString("DiaryText", "");
		pItem->msExtraVal = apInstanceVars->GetVarString("DiaryCallback", "");
	}
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Item::cLuxProp_Item(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Item)
{
	mfAmount = 1.0f;
	mlSpawnContainerID =-1;
	mfFlashAlpha =0;
}

//-----------------------------------------------------------------------

cLuxProp_Item::~cLuxProp_Item()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Item::CanInteract(iPhysicsBody *apBody)
{
	if(mbDestroyMe) return false; //To be 100% a player cannot double click on a note (if he is REALLY fast after closing journal)
	return true;
}

bool cLuxProp_Item::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	////////////////////
	//Picked up item
	bool bRemoveProp=true;	

	gpBase->mpInventory->AddItem(msName, mItemType, msSubItemTypeName, msImageFile, mfAmount, msVal,msExtraVal ,&bRemoveProp);

	if(bRemoveProp)
	{
		mpMap->DestroyEntity(this);

		gpBase->mpHelpFuncs->PlayGuiSoundData("ui/pick_"+msPickSound, eSoundEntryType_World);

		iLuxItemType *pType = gpBase->mpInventory->GetItemTypeData(mItemType);
		if(pType->ShowPickUpMessage())
		{
			tString sEntry = "ItemName_"+msSubItemTypeName;
			gpBase->mpMessageHandler->SetMessage(kTranslate("Inventory", "PickedUp")+_W(" ")+kTranslate("Inventory",sEntry), 0);
		}

		RunCallbackFunc("OnPickup");
	}
	/////////////////////
	//Item is NOT picked up
	else
	{
	}

	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Item::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_Item::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Item::UpdatePropSpecific(float afTimeStep)
{
	if(mbInteractionDisabled) return;

	/////////////////////////////////
	// If near player, flash
	cCamera *pCam =  gpBase->mpPlayer->GetCamera();
	cVector3f vCameraPos = pCam->GetPosition();
	cVector3f vBodyPos = mvBodies[0]->GetLocalPosition();
	vCameraPos.y=0; 
	vBodyPos.y =0;

	float fDistSqrt = cMath::Vector3DistSqr(vCameraPos, vBodyPos);
	if(fDistSqrt < 4.0f * 4.0f)
	{
		mfFlashAlpha += afTimeStep;
		if(mfFlashAlpha >1)mfFlashAlpha =1;
	}
	else
	{
		mfFlashAlpha -=afTimeStep;
		if(mfFlashAlpha <0)mfFlashAlpha =0;
	}

	if(mfFlashAlpha> 0)
	{
		for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
		{
			cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);

			if(pCam->GetFrustum()->CollideBoundingVolume(pSubEnt->GetBoundingVolume()) != eCollision_Outside)
				gpBase->mpEffectRenderer->AddFlashObject(pSubEnt, mfFlashAlpha);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Item::BeforePropDestruction()
{
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Item::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Pick;
}

//-----------------------------------------------------------------------

void cLuxProp_Item::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	//TODO!
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

kBeginSerialize(cLuxProp_Item_SaveData, iLuxProp_SaveData)
kSerializeVar(msVal, eSerializeType_String)
kSerializeVar(msExtraVal, eSerializeType_String)
kSerializeVar(mlSpawnContainerID, eSerializeType_Int32)
kSerializeVar(mfAmount, eSerializeType_Float32)
kSerializeVar(msSubItemTypeName, eSerializeType_String)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Item::CreateSaveData()
{
	return hplNew(cLuxProp_Item_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Item::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Item_SaveData *pData = static_cast<cLuxProp_Item_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,msVal);
	kCopyToVar(pData,msExtraVal);
	kCopyToVar(pData,mlSpawnContainerID);
	kCopyToVar(pData,mfAmount);
	kCopyToVar(pData,msSubItemTypeName);
}

//-----------------------------------------------------------------------

void cLuxProp_Item::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Item_SaveData *pData = static_cast<cLuxProp_Item_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,msVal);
	kCopyFromVar(pData,msExtraVal);
	kCopyFromVar(pData,mlSpawnContainerID);
	kCopyFromVar(pData,mfAmount);
	kCopyFromVar(pData,msSubItemTypeName);
}

//-----------------------------------------------------------------------

void cLuxProp_Item::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
