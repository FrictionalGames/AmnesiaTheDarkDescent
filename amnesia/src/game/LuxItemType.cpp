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

#include "LuxItemType.h"

#include "LuxInventory.h"
#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxPlayerState.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEntity.h"
#include "LuxJournal.h"
#include "LuxCompletionCountHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxHintHandler.h"


//////////////////////////////////////////////////////////////////////////
// INTERFACE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxItemType::iLuxItemType(const tString& asName, eLuxItemType aType)
{
	msName = asName;
	mType = aType;
	mbHasCount = false;
	mbShowPickUpMessage = true;
}

//-----------------------------------------------------------------------

iLuxItemType::~iLuxItemType()
{
	
}

//-----------------------------------------------------------------------

void iLuxItemType::AddCompletionAmount(int alAmount)
{
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap==NULL) return;

	pMap->AddCompletionAmount(alAmount);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUZZLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Puzzle::cLuxItemType_Puzzle() : iLuxItemType("Puzzle", eLuxItemType_Puzzle)
{
	
}


//-----------------------------------------------------------------------

bool cLuxItemType_Puzzle::BeforeAddItem(cLuxInventory_Item *apItem)
{
	ProgLog(eLuxProgressLogLevel_Medium, "Picked up puzzle item "+ apItem->GetName());

	AddCompletionAmount(gpBase->mpCompletionCountHandler->mlItemCompletionValue);
	return false;
}

void cLuxItemType_Puzzle::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	if(gpBase->mpPlayer->GetCurrentState()==eLuxPlayerState_UseItem)
	{
		gpBase->mpPlayer->ChangeState(eLuxPlayerState_Normal);
	}

	cLuxPlayerStateVars::SetupUseItem(apItem);

	gpBase->mpPlayer->ChangeState(eLuxPlayerState_UseItem);

	gpBase->mpInventory->ExitPressed();
}


//////////////////////////////////////////////////////////////////////////
// COINS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Coins::cLuxItemType_Coins() : iLuxItemType("Coins", eLuxItemType_Coins)
{

}

//-----------------------------------------------------------------------

bool cLuxItemType_Coins::BeforeAddItem(cLuxInventory_Item *apItem)
{
	int lCoins = gpBase->mpPlayer->GetCoins();
	lCoins += (int)apItem->GetAmount();
	gpBase->mpPlayer->SetCoins(lCoins);

	gpBase->mpHintHandler->Add("PickCoin", kTranslate("Hints", "PickCoin"), 0);

	return true;
}

//-----------------------------------------------------------------------

void cLuxItemType_Coins::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// NOTE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Note::cLuxItemType_Note() : iLuxItemType("Note", eLuxItemType_Note)
{
	mbShowPickUpMessage = false;
}

//-----------------------------------------------------------------------

bool cLuxItemType_Note::BeforeAddItem(cLuxInventory_Item *apItem)
{
	cLuxNote *pNote = gpBase->mpJournal->AddNote(apItem->GetStringVal(), apItem->GetImageName());
	if(pNote==NULL)return true;

	ProgLog(eLuxProgressLogLevel_Medium, "Picked up note "+ apItem->GetStringVal());

	if(apItem->GetAmount() > 0)
	{
		gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
		gpBase->mpJournal->SetForceInstantExit(true);
		if(apItem->GetAmount() >= 2)
			gpBase->mpJournal->OpenNote(pNote, true);
		else
			gpBase->mpJournal->OpenNote(pNote, false);
	}

	AddCompletionAmount(gpBase->mpCompletionCountHandler->mlNoteCompletionValue);

	return true;
}

//-----------------------------------------------------------------------

void cLuxItemType_Note::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DIARY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxItemType_Diary::mbShowJournalOnPickup = true;

//-----------------------------------------------------------------------

cLuxItemType_Diary::cLuxItemType_Diary() : iLuxItemType("Diary", eLuxItemType_Diary)
{
	mbShowPickUpMessage = false;
}

//-----------------------------------------------------------------------

bool cLuxItemType_Diary::BeforeAddItem(cLuxInventory_Item *apItem)
{
	int lDiaryIdx;
	cLuxDiary *pDiary = gpBase->mpJournal->AddDiary(apItem->GetStringVal(), apItem->GetImageName(), lDiaryIdx);
	if(pDiary==NULL) return true;

	ProgLog(eLuxProgressLogLevel_Medium, "Picked up diary "+ apItem->GetStringVal());

	mbShowJournalOnPickup = true;
	const tString &sCallbackFunc = apItem->GetExtraStringVal();
	if(sCallbackFunc != "")
	{
		cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	
		pMap->RunScript(sCallbackFunc+"(\""+apItem->GetName()+ "\","+ cString::ToString(lDiaryIdx)+")" );
	}

	if(mbShowJournalOnPickup)
	{
		gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
		gpBase->mpJournal->SetForceInstantExit(true);
		gpBase->mpJournal->OpenDiary(pDiary, true);
	}
	else
	{
		gpBase->mpJournal->SetDiaryAsLastRead(pDiary);
	}

	AddCompletionAmount(gpBase->mpCompletionCountHandler->mlDiaryCompletionValue);

	return true;
}

//-----------------------------------------------------------------------

void cLuxItemType_Diary::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LANTERN
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Lantern::cLuxItemType_Lantern() : iLuxItemType("Lantern", eLuxItemType_Lantern)
{

}

//-----------------------------------------------------------------------

bool cLuxItemType_Lantern::BeforeAddItem(cLuxInventory_Item *apItem)
{
	ProgLog(eLuxProgressLogLevel_Medium, "Picked up latern");

	gpBase->mpHintHandler->Add("PickLantern", kTranslate("Hints", "PickLantern"), 0);

	return false;
}

//-----------------------------------------------------------------------

void cLuxItemType_Lantern::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	cLuxPlayerLantern *pLantern = gpBase->mpPlayer->GetHelperLantern();
	pLantern->SetActive(!pLantern->IsActive(), true);
}

//-----------------------------------------------------------------------

tWString cLuxItemType_Lantern::GetDisplayedNameAdd(cLuxInventory_Item *apItem)
{
	cLuxPlayerLantern *pLantern = gpBase->mpPlayer->GetHelperLantern();
	tWString sStr = pLantern->IsActive() ? kTranslate("Inventory", "LanternOn"): kTranslate("Inventory", "LanternOff");
	return _W(" (") +sStr + _W(")");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// HEALTH
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Health::cLuxItemType_Health() : iLuxItemType("Health", eLuxItemType_Health)
{
	mbHasCount = true;
	mlMaxCount = 99;
}

bool cLuxItemType_Health::BeforeAddItem(cLuxInventory_Item *apItem)
{
	gpBase->mpHintHandler->Add("PickHealthPotion", kTranslate("Hints", "PickHealthPotion"), 0);

	return false;
}

void cLuxItemType_Health::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	float fHealth = gpBase->mpPlayer->GetHealth();
	if(fHealth >= 100) return;

	fHealth += apItem->GetAmount();
	if(fHealth > 100) fHealth = 100;
	gpBase->mpPlayer->SetHealth(fHealth);
	gpBase->mpInventory->RemoveItem(apItem);

	gpBase->mpHelpFuncs->PlayGuiSoundData("ui_use_health", eSoundEntryType_Gui);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// SANITY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Sanity::cLuxItemType_Sanity() : iLuxItemType("Sanity", eLuxItemType_Sanity)
{
	mbHasCount = true;
	mlMaxCount = 99;
}

bool cLuxItemType_Sanity::BeforeAddItem(cLuxInventory_Item *apItem)
{
	gpBase->mpHintHandler->Add("PickSanityPotion", kTranslate("Hints", "PickSanityPotion"), 0);

	return false;
}

void cLuxItemType_Sanity::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	float fSanity = gpBase->mpPlayer->GetSanity();
	if(fSanity >= 100) return;

	gpBase->mpPlayer->AddSanity(apItem->GetAmount());
	gpBase->mpInventory->RemoveItem(apItem);

	gpBase->mpHelpFuncs->PlayGuiSoundData("ui_use_sanity", eSoundEntryType_Gui);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LAMP OIL
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_LampOil::cLuxItemType_LampOil()  : iLuxItemType("LampOil", eLuxItemType_LampOil)
{
	mbHasCount = true;
	mlMaxCount = 99;
}

bool cLuxItemType_LampOil::BeforeAddItem(cLuxInventory_Item *apItem)
{
	gpBase->mpHintHandler->Add("PickOil", kTranslate("Hints", "PickOil"), 0);

	return false;
}

void cLuxItemType_LampOil::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	cLuxInventory *pInventory = gpBase->mpInventory;
	if(pInventory->HasItemOfType(eLuxItemType_Lantern)==false)
	{
		pInventory->SetMessageText(kTranslate("Inventory","OilNeedsLantern"),0);
		return;
	}

	float fLampOil = gpBase->mpPlayer->GetLampOil();
	if(fLampOil >= 100) return;

	fLampOil += apItem->GetAmount();
	if(fLampOil > 100) fLampOil = 100;
	gpBase->mpPlayer->SetLampOil(fLampOil);
	pInventory->RemoveItem(apItem);

	gpBase->mpHelpFuncs->PlayGuiSoundData("ui_use_oil", eSoundEntryType_Gui);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// TINDERBOX
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_Tinderbox::cLuxItemType_Tinderbox()  : iLuxItemType("Tinderbox", eLuxItemType_Tinderbox)
{
	mbHasCount = true;
	mlMaxCount = 99;
}

bool cLuxItemType_Tinderbox::GetHasMaxAmount()
{
	//if(gpBase->mpPlayer->GetTinderboxes()>=10) return true;
	return false;
}

bool cLuxItemType_Tinderbox::BeforeAddItem(cLuxInventory_Item *apItem)
{
	gpBase->mpHintHandler->Add("PickTinderbox", kTranslate("Hints", "PickTinderbox"), 0);

	gpBase->mpPlayer->AddTinderboxes(1);

	return true;
}

void cLuxItemType_Tinderbox::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
    cLuxPlayerStateVars::SetupUseItem(apItem);

	gpBase->mpPlayer->ChangeState(eLuxPlayerState_UseItem);

	gpBase->mpInventory->ExitPressed();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// HAND OBJECT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxItemType_HandObject::cLuxItemType_HandObject() : iLuxItemType("HandObject", eLuxItemType_HandObject)
{

}

//-----------------------------------------------------------------------

bool cLuxItemType_HandObject::BeforeAddItem(cLuxInventory_Item *apItem)
{
	if(gpBase->mpInventory->GetEquippedHandItem() == NULL)
	{
		gpBase->mpInventory->SetEquippedHandItem(apItem);
	}

    return false;
}

//-----------------------------------------------------------------------

void cLuxItemType_HandObject::OnUse(cLuxInventory_Item *apItem, int alSlotIndex)
{
	gpBase->mpInventory->SetEquippedHandItem(apItem);
}

//-----------------------------------------------------------------------



