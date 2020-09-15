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

#include "LuxProp_Photocell.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Photocell::cLuxPropLoader_Photocell(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Photocell::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Photocell, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Photocell::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Photocell  *pPhotocell = static_cast<cLuxProp_Photocell*>(apProp);

	
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Photocell::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Photocell  *pPhotocell = static_cast<cLuxProp_Photocell*>(apProp);

	pPhotocell->mfLightLevelOnLimit = apInstanceVars->GetVarFloat("LightLevelOnLimit", 0);
	pPhotocell->mfLightLevelOffLimit = apInstanceVars->GetVarFloat("LightLevelOffLimit", 0);
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Photocell::cLuxProp_Photocell(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Photocell)
{
	mfLightLevel = 0;
	mfLightLevelCheckCount = 0;
	mbLit = false;
}

//-----------------------------------------------------------------------

cLuxProp_Photocell::~cLuxProp_Photocell()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Photocell::CanInteract(iPhysicsBody *apBody)
{
	return false;
	
}

//-----------------------------------------------------------------------

bool cLuxProp_Photocell::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	iPhysicsBody *pBody = mpMainBody ? mpMainBody : apBody;

	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::OnResetProperties()
{

}
//-----------------------------------------------------------------------


void cLuxProp_Photocell::UpdatePropSpecific(float afTimeStep)
{
	/////////////////////
	// Update light level
	UpdateLightLevel(afTimeStep);

	/////////////////////
	// Set mesh alpha
	float fGoalAmount = cMath::Clamp( (mfLightLevel - mfLightLevelOffLimit) / (mfLightLevelOnLimit - mfLightLevelOffLimit), 0.0f, 1.0f);
	
	if(mpMeshEntity && mpMeshEntity->GetIlluminationAmount() != fGoalAmount)
	{
		float fCurrent = mpMeshEntity->GetIlluminationAmount();
		if(fCurrent < fGoalAmount)
		{
			fCurrent += afTimeStep;
			if(fCurrent > fGoalAmount) fCurrent = fGoalAmount;
		}
		else if(fCurrent > fGoalAmount)
		{
			fCurrent -= afTimeStep;
			if(fCurrent < fGoalAmount) fCurrent = fGoalAmount;
		}

		mpMeshEntity->SetIlluminationAmount(fCurrent);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::BeforePropDestruction()
{
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Photocell::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_Photocell::UpdateLightLevel(float afTimeStep)
{
	mfLightLevelCheckCount += afTimeStep;
	if(mfLightLevelCheckCount < 1.0f / 30.0f) return;

    mfLightLevelCheckCount = 0;

	mfLightLevel = 0;
	cBoundingVolume *pBV = mpMeshEntity->GetBoundingVolume();
	cVector3f vAxisAdd = pBV->GetSize()*0.5f + cVector3f(0.02f);
	cVector3f vSamplePos[6] = {
		pBV->GetWorldCenter() + cVector3f(vAxisAdd.x,0,0),
		pBV->GetWorldCenter() - cVector3f(vAxisAdd.x,0,0),
		pBV->GetWorldCenter() + cVector3f(0,vAxisAdd.y,0),
		pBV->GetWorldCenter() - cVector3f(0,vAxisAdd.y,0),
		pBV->GetWorldCenter() + cVector3f(0,0,vAxisAdd.z),
		pBV->GetWorldCenter() - cVector3f(0,0,vAxisAdd.z)
	};

	for(int i=0; i<6; ++i)
	{
		float fLight = gpBase->mpMapHelper->GetLightLevelAtPos(vSamplePos[i]);
		if(fLight > mfLightLevel) mfLightLevel = fLight;
	}

	////////////////////////////
	//Check if on / off
	if(mbLit)
	{
		if(mfLightLevel < mfLightLevelOffLimit)
		{
			//Log("LightLevel: %f, OFF!\n", mfLightLevel);

			mbLit = false;
			ConnectionStateChange(-1);
		}
	}
	else
	{
		if(mfLightLevel > mfLightLevelOnLimit)
		{
			//Log("LightLevel: %f, ON!\n", mfLightLevel);

			mbLit = true;
			ConnectionStateChange(1);
		}
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Photocell_SaveData, iLuxProp_SaveData)
kSerializeVar(mbLit,	eSerializeType_Float32)
kSerializeVar(mfLightLevel,	eSerializeType_Float32)
kSerializeVar(mfLightLevelCheckCount,	eSerializeType_Float32)
kSerializeVar(mfLightLevelOnLimit,	eSerializeType_Float32)
kSerializeVar(mfLightLevelOffLimit,	eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Photocell::CreateSaveData()
{
	return hplNew(cLuxProp_Photocell_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Photocell_SaveData *pData = static_cast<cLuxProp_Photocell_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mbLit);
	kCopyToVar(pData,mfLightLevel);
	kCopyToVar(pData,mfLightLevelCheckCount);
	kCopyToVar(pData,mfLightLevelOnLimit);
	kCopyToVar(pData,mfLightLevelOffLimit);
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Photocell_SaveData *pData = static_cast<cLuxProp_Photocell_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mbLit);
	kCopyFromVar(pData,mfLightLevel);
	kCopyFromVar(pData,mfLightLevelCheckCount);
	kCopyFromVar(pData,mfLightLevelOnLimit);
	kCopyFromVar(pData,mfLightLevelOffLimit);
}

//-----------------------------------------------------------------------

void cLuxProp_Photocell::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
