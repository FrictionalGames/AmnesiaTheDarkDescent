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

#include "LuxProp_Lamp.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxInventory.h"
#include "LuxMessageHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxGlobalDataHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Lamp::cLuxPropLoader_Lamp(const tString& asName) : iLuxPropLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Lamp::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Lamp, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Lamp::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Lamp  *pLamp = static_cast<cLuxProp_Lamp*>(apProp);

	///////////////////////////
	// General
	pLamp->mbCanBeLitByPlayer = GetVarBool("CanBeLitByPlayer", true);

	pLamp->mbCanBeGrabbed = GetVarBool("CanBeGrabbed", false);

	///////////////////////////
	// Grab specific
	pLamp->mGrabData.mbGrabUseDepth = GetVarBool("GrabUseDepth", false);
	pLamp->mGrabData.mfGrabDepth = GetVarFloat("GrabDepth", 1.5f);
	pLamp->mGrabData.mfGrabDepthInc = GetVarFloat("GrabDepthInc", 0.1f);
	pLamp->mGrabData.mfGrabMinDepth = GetVarFloat("GrabMinDepth", 1.0f);
	pLamp->mGrabData.mfGrabMaxDepth = GetVarFloat("GrabMaxDepth", 2.0f);
	pLamp->mGrabData.mvGrabPositionOffset = GetVarVector3f("GrabPositionOffset", 0.0f);
	pLamp->mGrabData.mvGrabRotationOffset = cMath::Vector3ToRad(GetVarVector3f("GrabRotationOffset", 0.0f));
	pLamp->mGrabData.mfGrabThrowImpulse = GetVarFloat("GrabThrowImpulse", 10.0f);
	pLamp->mGrabData.mbGrabUseOffset = GetVarBool("GrabUseOffset", true);
	pLamp->mGrabData.mfGrabMassMul = GetVarFloat("GrabMassMul", 0.1f);
	pLamp->mGrabData.mfForceMul = GetVarFloat("GrabForceMul", 1);
	pLamp->mGrabData.mfTorqueMul = GetVarFloat("GrabTorqueMul", 1);
	pLamp->mGrabData.mbUseRotation = GetVarBool("GrabUseRotation", true);
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Lamp::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
    cLuxProp_Lamp  *pLamp = static_cast<cLuxProp_Lamp*>(apProp);

	pLamp->SetLit(apInstanceVars->GetVarBool("Lit",true), false);

	/////////////////////////////////
	//Connect a light to the lamp
	pLamp->msConnectionLight = apInstanceVars->GetVarString("ConnectedLight","");
	pLamp->mfConnectionLightAmount = apInstanceVars->GetVarFloat("ConnectionLightAmount",0);
	pLamp->mbConnectionLightUseOnColor = apInstanceVars->GetVarBool("ConnectionLightUseOnColor",false);
	pLamp->mbConnectionLightUseSpec = apInstanceVars->GetVarBool("ConnectionLightUseSpec",false);
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LAMP LIGHT CONNECTION
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxLampLightConnection::cLuxLampLightConnection(iLight *apLight)
{
	mpLight = apLight;
}

cLuxLampLightConnection::~cLuxLampLightConnection()
{
	tLuxLampLightConnection_LampListIt it = mlstLamps.begin();
	for(; it != mlstLamps.end(); ++it)
	{
		cLuxLampLightConnection_Lamp *pLampConnection = *it;
		pLampConnection->mpLamp->mpLightConnection = NULL;
	}

	STLDeleteAll(mlstLamps);
}

//-----------------------------------------------------------------------

void cLuxLampLightConnection::Update(float afTimeStep)
{
	cColor finalColor = mpLight->GetDefaultDiffuseColor();
	
	tLuxLampLightConnection_LampListIt it = mlstLamps.begin();
	for(; it != mlstLamps.end(); ++it)
	{
		cLuxLampLightConnection_Lamp *pLampConnection = *it;
		cLuxProp_Lamp *pLamp = pLampConnection->mpLamp;
		if(pLamp->mvLights.empty()) continue;

		cColor lightColor = pLampConnection->mbUseLightOnColor ?	pLamp->mvEffectLightData[0].mOnColor :
																	pLamp->mvLights[0]->GetDiffuseColor();
		if(pLampConnection->mbUseLightSpec==false) lightColor.a = 0;

	    finalColor = finalColor + lightColor * pLampConnection->mfAmount * pLampConnection->mpLamp->GetEffectsAlpha();
	}

	mpLight->SetDiffuseColor(finalColor);
}

//-----------------------------------------------------------------------

void cLuxLampLightConnection::AddLamp(cLuxProp_Lamp *apLamp, float afAmount, bool abUseOnColor, bool abUseSpec)
{
	cLuxLampLightConnection_Lamp *pLampConnection = hplNew( cLuxLampLightConnection_Lamp, () );
	pLampConnection->mpLamp =apLamp;
	pLampConnection->mfAmount = afAmount;
	pLampConnection->mbUseLightOnColor = abUseOnColor;
	pLampConnection->mbUseLightSpec = abUseSpec;

	mlstLamps.push_back(pLampConnection);
}

//-----------------------------------------------------------------------

void cLuxLampLightConnection::RemoveLamp(cLuxProp_Lamp *apLamp)
{
	tLuxLampLightConnection_LampListIt it = mlstLamps.begin();
	for(; it != mlstLamps.end(); )
	{
		cLuxLampLightConnection_Lamp *pLampConnection = *it;
		if(pLampConnection->mpLamp == apLamp)
		{
			it = mlstLamps.erase(it);
        }
		else
		{
			++it;
		}
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Lamp::cLuxProp_Lamp(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Lamp)
{
	mbLit = true;
	mpLightConnection = NULL;
	mbLightConnectionSetup = false;
}

//-----------------------------------------------------------------------

cLuxProp_Lamp::~cLuxProp_Lamp()
{
	if(mpLightConnection)
		mpLightConnection->RemoveLamp(this);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Lamp::CanInteract(iPhysicsBody *apBody)
{
	if(CanBeIgnitByPlayer() && mbLit==false) return true;
	if(msInteractCallback != "") return true;
	
	return false;
}

//-----------------------------------------------------------------------

bool cLuxProp_Lamp::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	//////////////////////
	//Turn off
	if(mbLit)
	{
		//SetLit(false, true);
		//Nothing...
	}
	//////////////////////
	//Ignite
	else
	{
		/////////////////////
		// Check so has enough tinderboxes
		if(gpBase->mpPlayer->GetTinderboxes()<=0)
		{
			gpBase->mpMessageHandler->SetMessage(kTranslate("Game","NoMoreTinderboxes"), 0);
			return false;
		}

		/////////////////////
		// Add sanity
		float fIncreaseAmount = 1.0f - gpBase->mpPlayer->GetSanity() / 100.0f;
		fIncreaseAmount = fIncreaseAmount*fIncreaseAmount; //Want exp curve
		float fSanityAdd =	gpBase->mpGlobalDataHandler->GetLightLampMinSanityIncrease() * (1-fIncreaseAmount) + 
							gpBase->mpGlobalDataHandler->GetLightLampMaxSanityIncrease()*fIncreaseAmount;
		gpBase->mpPlayer->AddSanity(fSanityAdd, false);

		////////////////////
		// Negate tinderboxes
		gpBase->mpPlayer->AddTinderboxes(-1);
		
		gpBase->mpHelpFuncs->PlayGuiSoundData("ui_use_tinderbox", eSoundEntryType_Gui);

		RunCallbackFunc("OnIgnite");
		
		SetLit(true, true);
	}

	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::OnSetupAfterLoad(cWorld *apWorld)
{
	
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::UpdatePropSpecific(float afTimeStep)
{
	if(mbLightConnectionSetup == false)
	{
		SetupLampLightConnection();
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::BeforePropDestruction()
{
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Lamp::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	//if(CanBeIgnitByPlayer())	return eLuxFocusCrosshair_Ignite;
	if(mCustomFocusCrossHair != eLuxFocusCrosshair_Default && mbLit==true)
	{
		return mCustomFocusCrossHair;
	}

	if(CanInteract(apBody)) return eLuxFocusCrosshair_Ignite;

	return eLuxFocusCrosshair_Default;
}

tWString cLuxProp_Lamp::GetFocusText()
{
	if(CanInteract(GetMainBody()) && mbLit==false) 
	{
		return _W("x ") + cString::ToStringW(gpBase->mpPlayer->GetTinderboxes());
	}
	return _W("");
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::SetLit(bool abX, bool abUseEffects)
{
	if(mbLit == abX) return;

    mbLit =  abX;
	
	SetEffectsActive(mbLit, abUseEffects);
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	if(alState > 0)	SetLit(true, true);
	if(alState < 0)	SetLit(false, true);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_Lamp::SetupLampLightConnection()
{
	if(msConnectionLight != "")
	{
		iLight *pConnectionLight = NULL;
		//Iterate all lights and get light with name and with NO parent (otherwise a light connected to an entity is gotten).
		cLightListIterator lightIt = mpWorld->GetLightIterator();
		while(lightIt.HasNext())
		{
			iLight *pLight = lightIt.Next();
            if(pLight->GetEntityParent() == NULL && pLight->GetName() == msConnectionLight)
			{
				pConnectionLight = pLight;
				break;
			}
		}
		
			
		if(pConnectionLight)
		{
			mpMap->AddLampLightConnection(this, pConnectionLight, mfConnectionLightAmount, mbConnectionLightUseOnColor, mbConnectionLightUseSpec);
		}
		else
		{
			Error("Light '%s' that is connected to lamp '%s' does not exist!\n", msConnectionLight.c_str(), msName.c_str());
		}
	}
	mbLightConnectionSetup = true;
}

//-----------------------------------------------------------------------

bool cLuxProp_Lamp::CanBeIgnitByPlayer()
{
	if(mbLit || mbCanBeLitByPlayer==false) return false;
	return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Lamp_SaveData, iLuxProp_SaveData)
kSerializeVar(mbLit,	eSerializeType_Bool)
kSerializeVar(mbLightConnectionSetup,	eSerializeType_Bool)
kSerializeVar(msConnectionLight,			eSerializeType_String)
kSerializeVar(mfConnectionLightAmount,		eSerializeType_Float32)
kSerializeVar(mbConnectionLightUseOnColor,	eSerializeType_Bool)
kSerializeVar(mbConnectionLightUseSpec,		eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Lamp::CreateSaveData()
{
	return hplNew(cLuxProp_Lamp_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Lamp_SaveData *pData = static_cast<cLuxProp_Lamp_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mbLit);
	kCopyToVar(pData,mbLightConnectionSetup);
	kCopyToVar(pData,msConnectionLight);
	kCopyToVar(pData,mfConnectionLightAmount);
	kCopyToVar(pData,mbConnectionLightUseOnColor);
	kCopyToVar(pData,mbConnectionLightUseSpec);
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Lamp_SaveData *pData = static_cast<cLuxProp_Lamp_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,	mbLit);
	kCopyFromVar(pData,mbLightConnectionSetup);
	kCopyFromVar(pData,msConnectionLight);
	kCopyFromVar(pData,mfConnectionLightAmount);
	kCopyFromVar(pData,mbConnectionLightUseOnColor);
	kCopyFromVar(pData,mbConnectionLightUseSpec);
}

//-----------------------------------------------------------------------

void cLuxProp_Lamp::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);

	SetupLampLightConnection();
}

//-----------------------------------------------------------------------
