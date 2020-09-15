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

#include "LuxProp_OilBarrel.h"

#include "LuxPlayer.h"
#include "LuxMessageHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxInventory.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_OilBarrel::cLuxPropLoader_OilBarrel(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_OilBarrel::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_OilBarrel, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_OilBarrel::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_OilBarrel  *pOilBarrel = static_cast<cLuxProp_OilBarrel*>(apProp);

	///////////////////////////
	// General
	//pOilBarrel->mSwingDoorData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
	pOilBarrel->msFillSound = GetVarString("FillSound", "");
	pOilBarrel->msEmptySound = GetVarString("EmptySound", "");
}

//-----------------------------------------------------------------------

void cLuxPropLoader_OilBarrel::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_OilBarrel  *pOilBarrel = static_cast<cLuxProp_OilBarrel*>(apProp);

	//pOilBarrel->mlCoinsNeeded = apInstanceVars->GetVarInt("CoinsNeeded",0);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_OilBarrel::cLuxProp_OilBarrel(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_OilBarrel)
{
	mbAmountCalculated = false;
}

//-----------------------------------------------------------------------

cLuxProp_OilBarrel::~cLuxProp_OilBarrel()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_OilBarrel::CanInteract(iPhysicsBody *apBody)
{
	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_OilBarrel::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	CalculateOilAmount();

	float fOil = gpBase->mpPlayer->GetLampOil();

	///////////////////////////////////
	// Check if barrel is empty or lantern is full
	if(mfOilAmount <= 0)
	{
		PlaySound("EmptyBarell",msEmptySound, true, false);
		gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Empty"), 0);
		return false;
	}
	else if(fOil > 99)
	{
		gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_LanternFull"), 0);
		return false;
	}
	
	///////////////////////////////////
	// Increase oil amount
	fOil += mfOilAmount;
	float fFillAmount = mfOilAmount;
	if(fOil > 100)
	{	
		fFillAmount = mfOilAmount - (fOil - 100.0f);
        mfOilAmount = fOil - 100.0f;
		if(mfOilAmount < 1.0f) mfOilAmount =0; //If very little oil left, just let it be empty.
		fOil = 100;
	}
	else
	{
		fFillAmount = mfOilAmount;
		mfOilAmount = 0;
	}

	gpBase->mpPlayer->SetLampOil(fOil);
	PlaySound("FillLantern",msFillSound, true, false);

	///////////////////////////////////
	// Display fill message
	if(fOil >= 100)
	{
		gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Refill_Full"), 0);
	}
	else
	{
		if(fFillAmount < 25)
			gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Refill_Amount_025"),0);
		else if(fFillAmount < 50)
			gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Refill_Amount_050"),0);
		else if(fFillAmount < 75)
			gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Refill_Amount_075"),0);
		else 
			gpBase->mpMessageHandler->SetMessage(kTranslate("Game", "OilBarrel_Refill_Amount_100"),0);
	}

	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::UpdatePropSpecific(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_OilBarrel::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Grab;
}

//-----------------------------------------------------------------------

tWString cLuxProp_OilBarrel::GetFocusText()
{
	CalculateOilAmount();

	tWString sText;
	if(mfOilAmount <=0)
	{
		sText = kTranslate("Game", "OilBarrel_FocusText_Empty");
	}
	else
	{
		if(mfOilAmount < 25)
			sText = kTranslate("Game", "OilBarrel_FocusText_025");
		else if(mfOilAmount < 50)
			sText = kTranslate("Game", "OilBarrel_FocusText_050");
		else if(mfOilAmount < 75)
			sText = kTranslate("Game", "OilBarrel_FocusText_075");
		else 
			sText = kTranslate("Game", "OilBarrel_FocusText_100");
	}
		
	return sText;
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::CalculateOilAmount()
{
	if(mbAmountCalculated) return;
	
	//////////////////////
	// HARDMODE
	if (gpBase->mbHardMode) 
	{
		mfOilAmount = 10.0f;
		mbAmountCalculated = true;
		return;
	}

	mfOilAmount = 10.0f; //Base amount

	float fTotalOil = gpBase->mpPlayer->GetLampOil();
	
	//Iterate items and add amount and count to total
	for(int i=0; i<gpBase->mpInventory->GetItemNum(); ++i)
	{
		cLuxInventory_Item *pItem = gpBase->mpInventory->GetItem(i);
		if(pItem->GetType() != eLuxItemType_LampOil) continue;

		fTotalOil += pItem->GetAmount() * (float)pItem->GetCount();
	}
	
    float fAdd = 100 - fTotalOil;
	if(fAdd < 0) fAdd = 0;
	if(fAdd > 90) fAdd = 90;

	mfOilAmount += fAdd;

    mbAmountCalculated = true;    
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_OilBarrel_SaveData, iLuxProp_SaveData)

kSerializeVar(mbAmountCalculated, eSerializeType_Bool)
kSerializeVar(mfOilAmount, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_OilBarrel::CreateSaveData()
{
	return hplNew(cLuxProp_OilBarrel_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_OilBarrel_SaveData *pData = static_cast<cLuxProp_OilBarrel_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, mbAmountCalculated);
	kCopyToVar(pData, mfOilAmount);
	
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_OilBarrel_SaveData *pData = static_cast<cLuxProp_OilBarrel_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, mbAmountCalculated);
	kCopyFromVar(pData, mfOilAmount);
}

//-----------------------------------------------------------------------

void cLuxProp_OilBarrel::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
