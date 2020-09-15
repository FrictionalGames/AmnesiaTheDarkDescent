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

#include "LuxArea_SlimeDamage.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxHelpFuncs.h"
#include "LuxMessageHandler.h"
#include "LuxEffectHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_SlimeDamage::cLuxAreaLoader_SlimeDamage(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_SlimeDamage::~cLuxAreaLoader_SlimeDamage()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_SlimeDamage::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
	cLuxArea_SlimeDamage *pArea = hplNew(cLuxArea_SlimeDamage, (asName, alID, apMap));
	return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_SlimeDamage::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
	cLuxArea_SlimeDamage *pFlashArea = static_cast<cLuxArea_SlimeDamage*>(apArea);

	pFlashArea->mlSlimeType = GetVarInt("SlimeType",0);

	pFlashArea->mfMinCheckAttackTime = GetVarFloat("MinCheckAttackTime",0);
	pFlashArea->mfMaxCheckAttackTime = GetVarFloat("MaxCheckAttackTime",0);
	pFlashArea->mbDisableAfterAttack = GetVarBool("DisableAfterAttack",false);
	pFlashArea->msCallback = GetVarString("Callback","");

	pFlashArea->mfCheckCollisionCount = cMath::RandRectf(pFlashArea->mfMinCheckAttackTime, pFlashArea->mfMaxCheckAttackTime);
	
	//Load from config file.
	tString sPrefix = "SlimeType" + cString::ToString(pFlashArea->mlSlimeType);

	pFlashArea->msAttackSound = gpBase->mpGameCfg->GetString("Slime",sPrefix+"_AttackSound", "");
	pFlashArea->msAttackPS = gpBase->mpGameCfg->GetString("Slime",sPrefix+"_AttackPS", "");
	pFlashArea->mfMinAttackDamage = gpBase->mpGameCfg->GetFloat("Slime",sPrefix+"_MinAttackDamage", 0);
	pFlashArea->mfMaxAttackDamage = gpBase->mpGameCfg->GetFloat("Slime",sPrefix+"_MaxAttackDamage", 0);
	pFlashArea->mfScreenShakeAmount = gpBase->mpGameCfg->GetFloat("Slime",sPrefix+"_ScreenShakeAmount", 0);
}

void cLuxAreaLoader_SlimeDamage::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_SlimeDamage::cLuxArea_SlimeDamage(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_SlimeDamage)
{
	mfCheckCollisionCount = 0;
}

//-----------------------------------------------------------------------

cLuxArea_SlimeDamage::~cLuxArea_SlimeDamage()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_SlimeDamage::OnUpdate(float afTimeStep)
{
	//////////////////////////
	// Check update count
	mfCheckCollisionCount-=afTimeStep;
	if(mfCheckCollisionCount>0) return;

	mfCheckCollisionCount = cMath::RandRectf(mfMinCheckAttackTime, mfMaxCheckAttackTime);

	//////////////////////////
	// Check collision
	if(CollidesWithPlayer())
	{
		/////////////////////////
		//Damage player
		float fDamage = cMath::RandRectf(mfMinAttackDamage, mfMaxAttackDamage);
		gpBase->mpPlayer->GiveDamage(fDamage,1,eLuxDamageType_BloodSplat, true, false);

		/////////////////////////
		//Sound
		PlaySound("SlimeAttack", msAttackSound, true, false);

		/////////////////////////
		//Particle
		if(msAttackPS != "")
		{
			cParticleSystem *pPS = mpMap->GetWorld()->CreateParticleSystem("SlimeAttack", msAttackPS,1);
			if(pPS)
			{
				pPS->SetPosition(mpBody->GetLocalPosition());
			}
		}
		
		/////////////////////////
		//ScreenShake
		gpBase->mpEffectHandler->GetScreenShake()->Start(mfScreenShakeAmount, 0.5, 0.1,0.3f);

		/////////////////////////
		//Run Callback
		if(msCallback != "")
		{
			mpMap->RunScript(msCallback+"(\""+msName+"\")");
		}

		/////////////////////////
		//Check if area should be disabled
		if(mbDisableAfterAttack)
		{
			SetActive(false);
		}
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

kBeginSerialize(cLuxArea_SlimeDamage_SaveData, iLuxArea_SaveData)

kSerializeVar(mlSlimeType, eSerializeType_Int32)

kSerializeVar(mfMinCheckAttackTime, eSerializeType_Float32)
kSerializeVar(mfMaxCheckAttackTime, eSerializeType_Float32)
kSerializeVar(mbDisableAfterAttack, eSerializeType_Bool)
kSerializeVar(msCallback, eSerializeType_String)

kSerializeVar(mfCheckCollisionCount, eSerializeType_Float32)

kSerializeVar(msAttackSound, eSerializeType_String)
kSerializeVar(msAttackPS, eSerializeType_String)
kSerializeVar(mfMinAttackDamage, eSerializeType_Float32)
kSerializeVar(mfMaxAttackDamage, eSerializeType_Float32)
kSerializeVar(mfScreenShakeAmount, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_SlimeDamage_SaveData::CreateArea(cLuxMap *apMap)
{
	return hplNew(cLuxArea_SlimeDamage, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_SlimeDamage::CreateSaveData()
{
	return hplNew(cLuxArea_SlimeDamage_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_SlimeDamage::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxArea_SlimeDamage_SaveData *pData = static_cast<cLuxArea_SlimeDamage_SaveData*>(apSaveData);
	
	kCopyToVar(pData, mlSlimeType);

    kCopyToVar(pData, mfMinCheckAttackTime);
	kCopyToVar(pData, mfMaxCheckAttackTime);
	kCopyToVar(pData, mbDisableAfterAttack);
	kCopyFromVar(pData, msCallback);

	kCopyToVar(pData, mfCheckCollisionCount);

	kCopyToVar(pData, msAttackSound);
	kCopyToVar(pData, msAttackPS);
	kCopyToVar(pData, mfMinAttackDamage);
	kCopyToVar(pData, mfMaxAttackDamage);
	kCopyToVar(pData, mfScreenShakeAmount);
}

//-----------------------------------------------------------------------

void cLuxArea_SlimeDamage::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxArea_SlimeDamage_SaveData *pData = static_cast<cLuxArea_SlimeDamage_SaveData*>(apSaveData);

	kCopyFromVar(pData, mlSlimeType);

	kCopyFromVar(pData, mfMinCheckAttackTime);
	kCopyFromVar(pData, mfMaxCheckAttackTime);
	kCopyFromVar(pData, mbDisableAfterAttack);
	kCopyFromVar(pData, msCallback);

	kCopyFromVar(pData, mfCheckCollisionCount);

	kCopyFromVar(pData, msAttackSound);
	kCopyFromVar(pData, msAttackPS);
	kCopyFromVar(pData, mfMinAttackDamage);
	kCopyFromVar(pData, mfMaxAttackDamage);
	kCopyFromVar(pData, mfScreenShakeAmount);
}

//-----------------------------------------------------------------------

void cLuxArea_SlimeDamage::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

