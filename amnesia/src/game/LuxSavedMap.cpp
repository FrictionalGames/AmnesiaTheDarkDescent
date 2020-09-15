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

#include "LuxSavedMap.h"

#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEntity.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSavedMap::cLuxSavedMap()
{
	
}

//-----------------------------------------------------------------------

cLuxSavedMap::~cLuxSavedMap()
{
	cContainerListIterator<iLuxEntity_SaveData*> it = mlstEntities.GetIterator();
	while(it.HasNext())
	{
		iLuxEntity_SaveData *pSaveEntity = it.Next();
		hplDelete( pSaveEntity );
	}
	mlstEntities.Clear();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxSavedMap::FromMap(cLuxMap *apMap)
{
	/////////////////////////////////
	// Init
	cWorld *pWorld = apMap->GetWorld();


	/////////////////////////////////
	// General properties
	msFileName = apMap->GetFileName();
	msDisplayNameEntry = apMap->GetDisplayNameEntry();

	msLanternLitCallback = apMap->msLanternLitCallback;

	mlNumberOfQuests = apMap->mlNumberOfQuests;
	mlTotalCompletionAmount = apMap->mlTotalCompletionAmount;
	mlCurrentCompletionAmount = apMap->mlCurrentCompletionAmount;

	msCheckPointName = apMap->msCheckPointName;
	msCheckPointStartPos = apMap->msCheckPointStartPos;
	msCheckPointCallback = apMap->msCheckPointCallback;
	mlCheckPointCount = apMap->mlCheckPointCount;
	msCheckPointMusic = apMap->msCheckPointMusic;
	mlCheckPointMusicPrio = apMap->mlCheckPointMusicPrio;
	mbCheckPointMusicResume = apMap->mbCheckPointMusicResume;
	mfCheckPointMusicVolume = apMap->mfCheckPointMusicVolume;


	/////////////////////////////////
	// Sky box
	mbSkyBoxActive = pWorld->GetSkyBoxActive();
	msSkyboxTexture = pWorld->GetSkyBoxTexture() ? pWorld->GetSkyBoxTexture()->GetName() : "";
	mSkyBoxColor = pWorld->GetSkyBoxColor();

	/////////////////////////////////
	// Fog
	mbFogActive = pWorld->GetFogActive();
	mbFogCulling = pWorld->GetFogCulling();
	mfFogStart = pWorld->GetFogStart();
	mfFogEnd = pWorld->GetFogEnd();
	mfFogFalloffExp = pWorld->GetFogFalloffExp();
	mFogColor = pWorld->GetFogColor();
	
	/////////////////////////////////
	// Lights
	{
		cLightListIterator lightIt = pWorld->GetLightIterator();
		while(lightIt.HasNext())
		{
			iLight *pLight = lightIt.Next();

			if(pLight->IsSaved() && pLight->GetEntityParent() == NULL && pLight->GetParent()==NULL)
			{
				cEngineLight_SaveData saveLight;
				saveLight.FromLight(pLight);

				mlstLights.Add(saveLight);
			}
		}
	}

	/////////////////////////////////
	// Sounds
	{
		cSoundEntityIterator soundIt = pWorld->GetSoundEntityIterator();
		while(soundIt.HasNext())
		{
			cSoundEntity *pSound = soundIt.Next();

			if(	pSound->IsSaved() && pSound->GetEntityParent() == NULL && pSound->GetParent()==NULL && pSound->GetData()->GetLoop() )
			{
				cEngineSound_SaveData saveSound;
				saveSound.FromSound(pSound);

				mlstSounds.Add(saveSound);
			}
		}
	}

	/////////////////////////////////
	// Particle systems
	{
		cParticleSystemIterator psIt = pWorld->GetParticleSystemIterator();
		while(psIt.HasNext())
		{
			cParticleSystem *pPS = psIt.Next();

			if(	pPS->IsSaved() && pPS->GetEntityParent() == NULL && pPS->GetParent()==NULL && pPS->IsDying()==false)
			{
				cEnginePS_SaveData savePS;
				savePS.FromPS(pPS);

				mlstPS.Add(savePS);
			}
		}
	}

	/////////////////////////////////
	// Entities
    cLuxEntityIterator entityIt = apMap->GetEntityIterator();
	while(entityIt.HasNext())
	{
		iLuxEntity *pEntity = entityIt.Next();
        if(pEntity->IsSaved() && pEntity->GetDestroyMe()==false)
		{
			iLuxEntity_SaveData *pSaveData = pEntity->CreateSaveData();
			pEntity->SaveToSaveData(pSaveData);
			
			mlstEntities.Add(pSaveData);
		}
	}

	/////////////////////
	//Vars
	{
		tLuxScriptVarMapIt it = apMap->m_mapVars.begin();
		for(; it != apMap->m_mapVars.end(); ++it)
		{
			mlstVars.Add(it->second);
		}
	}

	/////////////////////
	//Timers
	{
		tLuxEventTimerListIt it = apMap->mlstTimers.begin();
		for(; it != apMap->mlstTimers.end(); ++it)
		{
			cLuxEventTimer *pTimer = *it;
			mlstTimers.Add(*pTimer);
		}
	}

	/////////////////////////////////
	// Ropes
	{
		cRopeEntityIterator ropeIt = apMap->GetWorld()->GetRopeEntityIterator();
		while(ropeIt.HasNext())
		{
			cRopeEntity *pRope = ropeIt.Next();
			
			cEngineRope_SaveData saveRope;
            saveRope.FromRope(pRope);

			mlstRopes.Add(saveRope);
		}
	}


	/////////////////////////////////
	// Use Item callbacks
	{
		tLuxUseItemCallbackListIt it = apMap->mlstUseItemCallbacks.begin();
		for(; it != apMap->mlstUseItemCallbacks.end(); ++it)
		{
			cLuxUseItemCallback *pCallback = *it;
			mvUseItemCallbacks.Add(*pCallback);
		}
	}

}

//-----------------------------------------------------------------------

void cLuxSavedMap::ToMap(cLuxMap *apMap)
{	
	/////////////////////////////////
	// Init
	cWorld *pWorld = apMap->GetWorld();

	apMap->SetDisplayNameEntry(msDisplayNameEntry);

	apMap->msLanternLitCallback = msLanternLitCallback;

	apMap->mlNumberOfQuests = mlNumberOfQuests;
	apMap->mlTotalCompletionAmount = mlTotalCompletionAmount;
	apMap->mlCurrentCompletionAmount = mlCurrentCompletionAmount;

	apMap->msCheckPointName = msCheckPointName;
	apMap->msCheckPointStartPos = msCheckPointStartPos;
	apMap->msCheckPointCallback = msCheckPointCallback;
	apMap->mlCheckPointCount = mlCheckPointCount;
	apMap->msCheckPointMusic = msCheckPointMusic;
	apMap->mlCheckPointMusicPrio = mlCheckPointMusicPrio;
	apMap->mbCheckPointMusicResume = mbCheckPointMusicResume;
	apMap->mfCheckPointMusicVolume = mfCheckPointMusicVolume;

	/////////////////////////////////
	// Sky box
	pWorld->SetSkyBoxActive(mbSkyBoxActive);
	if(msSkyboxTexture=="")	
	{
		pWorld->SetSkyBox(NULL, true);
	}
	else
	{
		iTexture *pTexture = gpBase->mpEngine->GetResources()->GetTextureManager()->CreateCubeMap(msSkyboxTexture, true);
		pWorld->SetSkyBox(pTexture, true);
	}

	pWorld->SetSkyBoxColor(mSkyBoxColor);

	/////////////////////////////////
	// Fog
	pWorld->SetFogActive(mbFogActive);
	pWorld->SetFogCulling(mbFogCulling);
	pWorld->SetFogStart(mfFogStart);
	pWorld->SetFogEnd(mfFogEnd);
	pWorld->SetFogFalloffExp(mfFogFalloffExp);
	pWorld->SetFogColor(mFogColor);

	/////////////////////////////////
	// Lights
	{
		cContainerListIterator<cEngineLight_SaveData> lightSaveLight = mlstLights.GetIterator();
		while (lightSaveLight.HasNext())
		{
			cEngineLight_SaveData& saveLight = lightSaveLight.Next();

			iLight *pLight = pWorld->GetLightFromUniqueID(saveLight.mlID);
			if(pLight)
			{
				saveLight.ToLight(pLight);	
			}
			else
			{
				Warning("Could not find saved light '%s'\n", saveLight.msName.c_str());
			}
		}
	}

	/////////////////////////////////
	// Sounds
	{
		cContainerListIterator<cEngineSound_SaveData> saveSoundIt = mlstSounds.GetIterator();
		while (saveSoundIt.HasNext())
		{
			cEngineSound_SaveData& saveSound = saveSoundIt.Next();

			cSoundEntity *pSound = pWorld->CreateSoundEntity(saveSound.msName, saveSound.msSoundDataName, saveSound.mbRemoveWhenOver);
			if(pSound)
			{
				saveSound.ToSound(pSound);	
			}
			else
			{
				Warning("Could not create sound '%s'\n", saveSound.msSoundDataName.c_str());
			}
		}
	}

	/////////////////////////////////
	// Particle Systems
	{
		cContainerListIterator<cEnginePS_SaveData> savePSIt = mlstPS.GetIterator();
		while (savePSIt.HasNext())
		{
			cEnginePS_SaveData& savePS = savePSIt.Next();

			cParticleSystem *pPS = pWorld->CreateParticleSystem(savePS.msName, savePS.msType, 1);
			if(pPS)
			{
				savePS.ToPS(pPS);
			}
			else
			{
				Warning("Could not create particle system '%s'\n", savePS.msType.c_str());
			}
		}
	}

	/////////////////////////////////
	// Entities
	tLuxEntityList lstEntities;
	{
		gpBase->mpCurrentMapLoading = apMap;	

		/////////////////////
		//Create entities
		cContainerListIterator<iLuxEntity_SaveData*> it = mlstEntities.GetIterator();
		while(it.HasNext())
		{
			iLuxEntity_SaveData *pSaveEntity = it.Next();
			iLuxEntity *pGameEntity = pSaveEntity->CreateEntity(apMap);
			if (pGameEntity) {
				pGameEntity->LoadFromSaveData(pSaveEntity);
				pGameEntity->SetSaveData(pSaveEntity);

				lstEntities.push_back(pGameEntity);
			}
		}

		gpBase->mpCurrentMapLoading = NULL;
	}
	
	/////////////////////////////////
	// Ropes (needs to be after entities)
	{
		cContainerListIterator<cEngineRope_SaveData> saveRopeIt = mlstRopes.GetIterator();
		while (saveRopeIt.HasNext())
		{
			cEngineRope_SaveData& saveRope = saveRopeIt.Next();

			cRopeEntity *pRope = saveRope.CreateRope(apMap);
			saveRope.ToRope(pRope, apMap);
		}
	}

	//Setup entities after the load.
	apMap->AfterWorldLoadEntitySetup();

	/////////////////////
	//Vars
	{
		apMap->m_mapVars.clear();
		cContainerListIterator<cLuxScriptVar> it = mlstVars.GetIterator();
		while(it.HasNext())
		{
			cLuxScriptVar& scriptVar = it.Next();
			apMap->m_mapVars.insert(tLuxScriptVarMap::value_type(scriptVar.msName, scriptVar));
		}
	}

	/////////////////////
	//Timers
	{
		STLDeleteAll(apMap->mlstTimers);
		cContainerListIterator<cLuxEventTimer> it = mlstTimers.GetIterator();
		while(it.HasNext())
		{
			cLuxEventTimer& savedTimer = it.Next();
			
			cLuxEventTimer *pTimer = hplNew(cLuxEventTimer,());
			*pTimer = savedTimer;

			apMap->mlstTimers.push_back(pTimer);
		}
		
	}

	/////////////////////
	//Entities Setup
	{
		gpBase->mpCurrentMapLoading = apMap;

		for(tLuxEntityListIt it = lstEntities.begin(); it != lstEntities.end(); ++it)
		{
			iLuxEntity *pEntity = *it;

			pEntity->SetupSaveData(pEntity->GetSaveData());
			if(pEntity->IsActive())
				pEntity->UpdateLogic(0.001f); 
		}

		gpBase->mpCurrentMapLoading = NULL;
	}

	/////////////////////////////////
	// Use Item callbacks
	//Log("---Checking callbacks!\n");
	{
		STLDeleteAll(apMap->mlstUseItemCallbacks);
		for(size_t i=0; i<mvUseItemCallbacks.Size(); ++i)
		{
			cLuxUseItemCallback *pCallback = hplNew(cLuxUseItemCallback, ());
			*pCallback = mvUseItemCallbacks[i];
			
			//Log("Adding callback: '%s' '%s' '%s' '%s'\n", pCallback->msName.c_str(), pCallback->msEntity.c_str(), pCallback->msItem.c_str(), pCallback->msFunction.c_str());

			apMap->mlstUseItemCallbacks.push_back(pCallback);
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
// SERIALIZABLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxSavedMap)
kSerializeVar(msFileName, eSerializeType_String)
kSerializeVar(msDisplayNameEntry, eSerializeType_String)

kSerializeVar(msLanternLitCallback, eSerializeType_String)

kSerializeVar(mlNumberOfQuests, eSerializeType_Int32)
kSerializeVar(mlTotalCompletionAmount, eSerializeType_Int32)
kSerializeVar(mlCurrentCompletionAmount, eSerializeType_Int32)

kSerializeVar(mbSkyBoxActive, eSerializeType_Bool)
kSerializeVar(msSkyboxTexture, eSerializeType_String)
kSerializeVar(mSkyBoxColor, eSerializeType_Color)

kSerializeVar(mbFogActive, eSerializeType_Bool)
kSerializeVar(mbFogCulling, eSerializeType_Bool)
kSerializeVar(mfFogStart, eSerializeType_Float32)
kSerializeVar(mfFogEnd, eSerializeType_Float32)
kSerializeVar(mfFogFalloffExp, eSerializeType_Float32)
kSerializeVar(mFogColor, eSerializeType_Color)

kSerializeVar(msCheckPointName, eSerializeType_String)
kSerializeVar(msCheckPointStartPos, eSerializeType_String)
kSerializeVar(msCheckPointCallback, eSerializeType_String)
kSerializeVar(mlCheckPointCount, eSerializeType_Int32)
kSerializeVar(msCheckPointMusic, eSerializeType_String)
kSerializeVar(mlCheckPointMusicPrio, eSerializeType_Int32)
kSerializeVar(mbCheckPointMusicResume, eSerializeType_Bool)
kSerializeVar(mfCheckPointMusicVolume, eSerializeType_Float32)

kSerializeClassContainer(mlstLights, cEngineLight_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstRopes, cEngineRope_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstSounds, cEngineSound_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstPS, cEnginePS_SaveData, eSerializeType_Class)

kSerializeClassContainer(mlstEntities,iLuxEntity_SaveData, eSerializeType_ClassPointer)

kSerializeClassContainer(mlstTimers, cLuxEventTimer,  eSerializeType_Class)
kSerializeClassContainer(mlstVars, cLuxScriptVar,  eSerializeType_Class)

kSerializeClassContainer(mvUseItemCallbacks,cLuxUseItemCallback, eSerializeType_Class)
kEndSerialize()


//-----------------------------------------------------------------------

