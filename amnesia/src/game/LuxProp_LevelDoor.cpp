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

#include "LuxProp_LevelDoor.h"

#include "LuxMapHandler.h"
#include "LuxSavedGame.h"
#include "LuxMessageHandler.h"
#include "LuxHelpFuncs.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_LevelDoor::cLuxPropLoader_LevelDoor(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","LevelDoor_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_LevelDoor::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_LevelDoor, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_LevelDoor::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_LevelDoor  *pLevelDoor = static_cast<cLuxProp_LevelDoor*>(apProp);

	///////////////////////////
	// General
	pLevelDoor->msEnterSound = GetVarString("EnterSound", "");
	pLevelDoor->msExitSound = GetVarString("ExitSound", "");
}

//-----------------------------------------------------------------------

void cLuxPropLoader_LevelDoor::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_LevelDoor  *pLevelDoor = static_cast<cLuxProp_LevelDoor*>(apProp);
	
	pLevelDoor->msTextEntry = apInstanceVars->GetVarString("TextEntry","");
	pLevelDoor->msMapFile = apInstanceVars->GetVarString("MapFile","");
	pLevelDoor->msStartPos = apInstanceVars->GetVarString("StartPos","");

	pLevelDoor->mbLocked = apInstanceVars->GetVarBool("Locked",false);
	pLevelDoor->msLockedSound = apInstanceVars->GetVarString("LockedSound","");
	pLevelDoor->msLockedTextCat = apInstanceVars->GetVarString("LockedTextCat","");
	pLevelDoor->msLockedTextEntry = apInstanceVars->GetVarString("LockedTextEntry","");

	pLevelDoor->mbShowStats = apInstanceVars->GetVarBool("ShowStats",true);
}
//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_LevelDoor::cLuxProp_LevelDoor(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_LevelDoor)
{
	mfLockedCount =0;
}

//-----------------------------------------------------------------------

cLuxProp_LevelDoor::~cLuxProp_LevelDoor()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_LevelDoor::CanInteract(iPhysicsBody *apBody)
{
	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_LevelDoor::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(mbLocked)
	{
		//Wait until last locked message until showing a new.
		if(mfLockedCount <=0)
		{
			gpBase->mpHelpFuncs->PlayGuiSoundData(msLockedSound,eSoundEntryType_World);
			if(msLockedTextCat != "" && msLockedTextEntry != "")
			{
				gpBase->mpMessageHandler->SetMessage(kTranslate(msLockedTextCat, msLockedTextEntry), 0);
			}
			mfLockedCount = 2.0f;
		}
	}
	else
	{
		gpBase->mpMapHandler->ChangeMap(msMapFile,msStartPos, msEnterSound, msExitSound);
	}

	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::OnResetProperties()
{

}
//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::UpdatePropSpecific(float afTimeStep)
{
	if(mfLockedCount >0)
	{
		mfLockedCount -= afTimeStep;
	}
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_LevelDoor::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_LevelDoor;
}

//-----------------------------------------------------------------------

tWString cLuxProp_LevelDoor::GetFocusText()
{
	tWString sStr =	kTranslate("Levels", msTextEntry) + _W("\n");

	/*if(mbShowStats)
	{
		int lPercent=0;
		tString sMapName = gpBase->mpMapHandler->FileToMapName(msMapFile);
		cLuxSavedGameMap *pSavedMap = gpBase->mpMapHandler->GetSavedMapCollection()->GetSavedMap(sMapName,false);
		if(pSavedMap)
		{
			float fP = ((float)pSavedMap->mlCurrentCompletionAmount / (float)pSavedMap->mlTotalCompletionAmount) *100.0f;
			lPercent = (int)fP;
		}
		
		sStr += cString::ToStringW(lPercent)+_W("% ")+kTranslate("CompletionCount", "Completed");
	}*/
	
	return sStr;
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::SetLocked(bool abLocked)
{
	mbLocked = abLocked;
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	
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

kBeginSerialize(cLuxProp_LevelDoor_SaveData, iLuxProp_SaveData)

kSerializeVar(msMapFile, eSerializeType_String)
kSerializeVar(msStartPos, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)

kSerializeVar(mbLocked, eSerializeType_Bool)
kSerializeVar(msLockedSound, eSerializeType_String)
kSerializeVar(msLockedTextCat, eSerializeType_String)
kSerializeVar(msLockedTextEntry, eSerializeType_String)

kSerializeVar(mbShowStats, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_LevelDoor::CreateSaveData()
{
	return hplNew(cLuxProp_LevelDoor_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_LevelDoor_SaveData *pData = static_cast<cLuxProp_LevelDoor_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, msTextEntry);
	kCopyToVar(pData, msStartPos);
	kCopyToVar(pData, msMapFile);

	kCopyToVar(pData, mbLocked);
	kCopyToVar(pData, msLockedSound);
	kCopyToVar(pData, msLockedTextCat);
	kCopyToVar(pData, msLockedTextEntry);

	kCopyToVar(pData, mbShowStats);
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_LevelDoor_SaveData *pData = static_cast<cLuxProp_LevelDoor_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, msTextEntry);
	kCopyFromVar(pData, msStartPos);
	kCopyFromVar(pData, msMapFile);

	kCopyFromVar(pData, mbLocked);
	kCopyFromVar(pData, msLockedSound);
	kCopyFromVar(pData, msLockedTextCat);
	kCopyFromVar(pData, msLockedTextEntry);

	kCopyFromVar(pData, mbShowStats);
}

//-----------------------------------------------------------------------

void cLuxProp_LevelDoor::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
