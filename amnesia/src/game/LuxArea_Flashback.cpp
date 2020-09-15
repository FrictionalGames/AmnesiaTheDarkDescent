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

#include "LuxArea_Flashback.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxHelpFuncs.h"
#include "LuxMessageHandler.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Flashback::cLuxAreaLoader_Flashback(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Flashback::~cLuxAreaLoader_Flashback()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Flashback::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
	cLuxArea_Flashback *pArea = hplNew(cLuxArea_Flashback, (asName, alID, apMap));
	return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Flashback::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
	cLuxArea_Flashback *pFlashArea = static_cast<cLuxArea_Flashback*>(apArea);

    pFlashArea->msFlashbackFile = GetVarString("FlashbackFile","");
	pFlashArea->msCallback = GetVarString("Callback","");
}

void cLuxAreaLoader_Flashback::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Flashback::cLuxArea_Flashback(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Flashback)
{
	mfCheckCollisionCount =0;
}

//-----------------------------------------------------------------------

cLuxArea_Flashback::~cLuxArea_Flashback()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Flashback::OnUpdate(float afTimeStep)
{
	//Do not update this unless it is a proper game update (when eveything is 100% intialized)
	if(afTimeStep < gpBase->mpEngine->GetStepSize()*0.8f) return;

	//////////////////////////
	// Check update count
	mfCheckCollisionCount-=afTimeStep;
	if(mfCheckCollisionCount>0) return;
	mfCheckCollisionCount = 0.1f;

	//////////////////////////
	// Check collision
	if(CollidesWithPlayer())
	{
		SetActive(false);

		gpBase->mpPlayer->GetHelperFlashback()->Start(msFlashbackFile, msCallback);
	}
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

kBeginSerialize(cLuxArea_Flashback_SaveData, iLuxArea_SaveData)
kSerializeVar(msFlashbackFile, eSerializeType_String)
kSerializeVar(msCallback, eSerializeType_String)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Flashback_SaveData::CreateArea(cLuxMap *apMap)
{
	return hplNew(cLuxArea_Flashback, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Flashback::CreateSaveData()
{
	return hplNew(cLuxArea_Flashback_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Flashback::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxArea_Flashback_SaveData *pData = static_cast<cLuxArea_Flashback_SaveData*>(apSaveData);

    kCopyToVar(pData, msFlashbackFile);
	kCopyToVar(pData, msCallback);
}

//-----------------------------------------------------------------------

void cLuxArea_Flashback::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxArea_Flashback_SaveData *pData = static_cast<cLuxArea_Flashback_SaveData*>(apSaveData);

	kCopyFromVar(pData, msFlashbackFile);
	kCopyFromVar(pData, msCallback);
}

//-----------------------------------------------------------------------

void cLuxArea_Flashback::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

