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

#include "LuxProp_Chest.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMessageHandler.h"
#include "LuxCompletionCountHandler.h"
#include "LuxHelpFuncs.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Chest::cLuxPropLoader_Chest(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Chest::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Chest, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Chest::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Chest  *pChest = static_cast<cLuxProp_Chest*>(apProp);

	///////////////////////////
	// General
	pChest->mSwingDoorData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
	pChest->mSwingDoorData.mfMoveSlowDownFactor = GetVarFloat("MoveSlowDownFactor", 3.0f);
	pChest->mSwingDoorData.mfMoveSpeedFactor = GetVarFloat("MoveSpeedFactor", 1.0f);
	pChest->mSwingDoorData.mfMoveThrowImpulse = GetVarFloat("MoveThrowImpulse", 6.0f);
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Chest::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Chest  *pChest = static_cast<cLuxProp_Chest*>(apProp);

	pChest->mlCoinsNeeded = apInstanceVars->GetVarInt("CoinsNeeded",0);
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// MESSAGE CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxChestMessageCallback::cLuxChestMessageCallback(cLuxProp_Chest *apChest)
{
	mpChest = apChest;
}

//-----------------------------------------------------------------------

void cLuxChestMessageCallback::OnPress(bool abYes)
{
	if(abYes)
	{
		gpBase->mpPlayer->AddCoins(-mpChest->mlCoinsNeeded);
		mpChest->SetLocked(false, true);

		ProgLog(eLuxProgressLogLevel_Medium, "Opened chest "+ mpChest->msName);

		mpChest->mpMap->AddCompletionAmount(gpBase->mpCompletionCountHandler->mlChestCompletionValue);

		gpBase->mpHelpFuncs->PlayGuiSoundData("gameplay_open_chest", eSoundEntryType_Gui);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Chest::cLuxProp_Chest(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Chest)
{
	mbLocked = false;	

	mpMessageCallback = hplNew(cLuxChestMessageCallback, (this));
}

//-----------------------------------------------------------------------

cLuxProp_Chest::~cLuxProp_Chest()
{
	hplDelete(mpMessageCallback);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Chest::CanInteract(iPhysicsBody *apBody)
{
	return mbLocked;
}

//-----------------------------------------------------------------------

bool cLuxProp_Chest::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(mbLocked==false) return false;

	tWString sText = kTranslate("Game", "InteractChest01")+_W(" ")+cString::ToStringW(mlCoinsNeeded)+_W(" ")+kTranslate("Game", "InteractChest02")+_W("\n");
	sText += kTranslate("Game", "InteractChest03")+_W(" ")+cString::ToStringW(gpBase->mpPlayer->GetCoins())+_W("\n");

	if(gpBase->mpPlayer->GetCoins() < mlCoinsNeeded)
	{
		sText += kTranslate("Game", "InteractChest03_NoCoins")+_W("\n");
		gpBase->mpMessageHandler->StartPauseMessage(sText,false, NULL);
	}
	else
	{
		sText += kTranslate("Game", "InteractChest03_Question")+_W("\n");
		gpBase->mpMessageHandler->StartPauseMessage(sText,true, mpMessageCallback);
	}
	
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::OnSetupAfterLoad(cWorld *apWorld)
{
	////////////////////////////////////
	// Set up joints
	int lNum=0;
	for(size_t i=0; i< mvJoints.size(); ++i)
	{
		iPhysicsJoint *pJoint = mvJoints[i];
		if(pJoint->GetType() != ePhysicsJointType_Hinge) continue;

		iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
		mvJointData.push_back(cLuxChestJointData());

		iPhysicsBody *pChildBody = pJoint->GetChildBody();

		///////////////////////
		//Set update properties
		pHingeJoint->SetStickyMinLimit(true);
		pHingeJoint->SetStickyMaxLimit(true);

		///////////////////////
		//Set up data
		mvJointData[lNum].mpHingeJoint = pHingeJoint;
		mvJointData[lNum].mpChildBody = pChildBody;

		mvJointData[lNum].mfMaxAngle = pHingeJoint->GetMaxAngle();

		++lNum;
	}


	SetLocked(true,false);
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_Chest::UpdatePropSpecific(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Chest::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(mbLocked)
		return eLuxFocusCrosshair_Grab;
	else
		return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

tWString cLuxProp_Chest::GetFocusText()
{
	if(mbLocked)
		return	_W("Locked. Costs ")+cString::ToStringW(mlCoinsNeeded)+_W(" Thalers to open.");
	else
		return _W("");
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::ImplementedOnSetActive(bool abX)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::OnDamage(float afAmount, int alStrength)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::InFocusDraw(cGuiSet *apGuiSet, float afFrameTime)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::SetLocked(bool abLocked, bool abEffects)
{
	if(mbLocked == abLocked) return;

	mbLocked = abLocked;

	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		iPhysicsJointHinge *pHingeJoint = mvJointData[i].mpHingeJoint;

		if(mbLocked)
		{
			pHingeJoint->SetMinAngle(cMath::ToRad(0.0f));
			pHingeJoint->SetMaxAngle(cMath::ToRad(2.0f));
			pHingeJoint->SetStickyMinLimit(false);
			pHingeJoint->SetStickyMaxLimit(false);
		}
		else
		{
			pHingeJoint->SetMaxAngle(mvJointData[i].mfMaxAngle);
			pHingeJoint->SetMinAngle(mvJointData[i].mfMaxAngle-cMath::ToRad(2.0f));
			pHingeJoint->SetStickyMinLimit(false);
			pHingeJoint->SetStickyMaxLimit(false);
		}
		mvJointData[i].mpChildBody->AddForce(cVector3f(0,1,0));
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	if(alState > 0)		SetLocked(true, true);
	else				SetLocked(false, true);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxChestJointData* cLuxProp_Chest::GetJointDataFromBody(iPhysicsBody *apBody)
{
	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		if(mvJointData[i].mpChildBody == apBody) return &mvJointData[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------

cLuxChestJointData* cLuxProp_Chest::GetJointDataFromJoint(iPhysicsJoint *apJoint)
{
	for(size_t i=0; i<mvJointData.size(); ++i)
	{
		if(mvJointData[i].mpHingeJoint == apJoint) return &mvJointData[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Chest_SaveData, iLuxProp_SaveData)

kSerializeVar(mbLocked, eSerializeType_Bool)

kSerializeVar(mlCoinsNeeded, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Chest::CreateSaveData()
{
	return hplNew(cLuxProp_Chest_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Chest_SaveData *pData = static_cast<cLuxProp_Chest_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, mbLocked);
	kCopyToVar(pData, mlCoinsNeeded);
	
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Chest_SaveData *pData = static_cast<cLuxProp_Chest_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	SetLocked(pData->mbLocked, false);
	kCopyFromVar(pData, mlCoinsNeeded);
}

//-----------------------------------------------------------------------

void cLuxProp_Chest::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
