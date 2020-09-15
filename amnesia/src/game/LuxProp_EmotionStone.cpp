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

#include "LuxProp_EmotionStone.h"

#include "LuxPlayer.h"
#include "LuxEffectHandler.h"
#include "LuxHelpFuncs.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_EmotionStone::cLuxPropLoader_EmotionStone(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_EmotionStone::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_EmotionStone, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_EmotionStone::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_EmotionStone  *pEmotionStone = static_cast<cLuxProp_EmotionStone*>(apProp);

	///////////////////////////
	// General
	//pEmotionStone->mSwingDoorData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
}

//-----------------------------------------------------------------------

void cLuxPropLoader_EmotionStone::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_EmotionStone  *pEmotionStone = static_cast<cLuxProp_EmotionStone*>(apProp);

	pEmotionStone->msDescCat = apInstanceVars->GetVarString("DescCat","");
	pEmotionStone->msDescEntry = apInstanceVars->GetVarString("DescEntry","");

	pEmotionStone->msTextCat = apInstanceVars->GetVarString("TextCat","");
	pEmotionStone->msTextEntry = apInstanceVars->GetVarString("TextEntry","");

	pEmotionStone->msSound = apInstanceVars->GetVarString("Sound","");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_EmotionStone::cLuxProp_EmotionStone(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_EmotionStone)
{
}

//-----------------------------------------------------------------------

cLuxProp_EmotionStone::~cLuxProp_EmotionStone()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_EmotionStone::CanInteract(iPhysicsBody *apBody)
{
	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_EmotionStone::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	gpBase->mpEffectHandler->GetEmotionFlash()->Start(msTextCat, msTextEntry, msSound);

	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::UpdatePropSpecific(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::BeforePropDestruction()
{

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_EmotionStone::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Grab;
}

//-----------------------------------------------------------------------

tWString cLuxProp_EmotionStone::GetFocusText()
{
	if(msDescCat == "" || msDescEntry == "") return _W("");

	return gpBase->mpHelpFuncs->ParseString(kTranslate(msDescCat ,msDescEntry));
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
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

kBeginSerialize(cLuxProp_EmotionStone_SaveData, iLuxProp_SaveData)

kSerializeVar(msDescCat, eSerializeType_String)
kSerializeVar(msDescEntry, eSerializeType_String)

kSerializeVar(msTextCat, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)

kSerializeVar(msSound, eSerializeType_String)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_EmotionStone::CreateSaveData()
{
	return hplNew(cLuxProp_EmotionStone_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_EmotionStone_SaveData *pData = static_cast<cLuxProp_EmotionStone_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, msDescCat);
	kCopyToVar(pData, msDescEntry);

	kCopyToVar(pData, msTextCat);
	kCopyToVar(pData, msTextEntry);

	kCopyToVar(pData, msSound);
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_EmotionStone_SaveData *pData = static_cast<cLuxProp_EmotionStone_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, msDescCat);
	kCopyFromVar(pData, msDescEntry);

	kCopyFromVar(pData, msTextCat);
	kCopyFromVar(pData, msTextEntry);

	kCopyFromVar(pData, msSound);
}

//-----------------------------------------------------------------------

void cLuxProp_EmotionStone::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
