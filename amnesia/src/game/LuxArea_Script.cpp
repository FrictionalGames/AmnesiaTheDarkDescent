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

#include "LuxArea_Script.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Script::cLuxAreaLoader_Script(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Script::~cLuxAreaLoader_Script()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Script::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
	cLuxArea_Script *pArea = hplNew(cLuxArea_Script, (asName, alID, apMap));
	return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Script::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
	cLuxArea_Script *pScriptArea = static_cast<cLuxArea_Script*>(apArea);

	apArea->SetPlayerLookAtCallback(	GetVarString("PlayerLookAtCallback",""),
										GetVarBool("PlayerLookAtCallbackAutoRemove",false) );

	apArea->SetPlayerInteractCallback(	GetVarString("PlayerInteractCallback",""),
										GetVarBool("PlayerInteractCallbackAutoRemove",false) );


	pScriptArea->mbItemInteraction = GetVarBool("ItemInteraction", false);

	float fMaxFocusDistance = GetVarFloat("MaxFocusDistance", -1);
	if(fMaxFocusDistance >=0)
		pScriptArea->mfMaxFocusDistance = fMaxFocusDistance;
}

void cLuxAreaLoader_Script::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Script::cLuxArea_Script(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Script)
{
	mfMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","ScriptArea_MaxFocusDist",0);
}

//-----------------------------------------------------------------------

cLuxArea_Script::~cLuxArea_Script()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxArea_Script::CanInteract(iPhysicsBody *apBody)
{
	if(mbItemInteraction) return true;

    return msInteractCallback!="";
}

//-----------------------------------------------------------------------

bool cLuxArea_Script::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return msInteractCallback!="";
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxArea_Script::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(msInteractCallback!="")
	{
		return mCustomFocusCrossHair==eLuxFocusCrosshair_Default ? eLuxFocusCrosshair_Grab : mCustomFocusCrossHair;
	}
	else
	{
		return eLuxFocusCrosshair_Default;
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

kBeginSerialize(cLuxArea_Script_SaveData, iLuxArea_SaveData)
kSerializeVar(mbItemInteraction,eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Script_SaveData::CreateArea(cLuxMap *apMap)
{
	return hplNew(cLuxArea_Script, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Script::CreateSaveData()
{
	return hplNew(cLuxArea_Script_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Script::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxArea_Script_SaveData *pData = static_cast<cLuxArea_Script_SaveData*>(apSaveData);

	kCopyToVar(pData, mbItemInteraction);
}

//-----------------------------------------------------------------------

void cLuxArea_Script::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxArea_Script_SaveData *pData = static_cast<cLuxArea_Script_SaveData*>(apSaveData);

	kCopyFromVar(pData, mbItemInteraction);
}

//-----------------------------------------------------------------------

void cLuxArea_Script::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

