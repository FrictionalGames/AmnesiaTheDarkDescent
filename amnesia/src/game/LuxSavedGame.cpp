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

#include "LuxSavedGame.h"

#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxEnemy.h"
#include "LuxEnemyPathfinder.h"
#include "LuxProp_SwingDoor.h"
#include "LuxProp_LevelDoor.h"
#include "LuxProp_Item.h"
#include "LuxProp_Object.h"
#include "LuxProp_Chest.h"
#include "LuxProp_Lamp.h"
#include "LuxPlayer.h"
#include "LuxInteractConnections.h"

//////////////////////////////////////////////////////////////////////////
// ENTITY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSavedGameEntity::~cLuxSavedGameEntity()
{
	for(size_t i=0; i<mvInteractConnections.Size(); ++i)
	{
		iLuxInteractConnection_SaveData *pSaveConn = mvInteractConnections[i];
		hplDelete(pSaveConn);
	}
}

//-----------------------------------------------------------------------

void cLuxSavedGameEntity::FromEntity(iLuxEntity *apEntity)
{
	mlID = apEntity->GetID();

	////////////////////////
	//Attached Props (Special for props!)
	if(apEntity->GetEntityType() == eLuxEntityType_Prop)
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);

		//////////////////////////
		// Attached props
		tLuxProp_AttachedPropListIt attachIt = pProp->mlstAttachedProps.begin();
		for(; attachIt != pProp->mlstAttachedProps.end(); ++attachIt)
		{
			cLuxProp_AttachedProp attachProp = *(*attachIt);
			attachProp.mpProp = NULL;
			Log("Saving attached prop: '%s' '%s' (%s)\n", attachProp.msName.c_str(), attachProp.msFileName.c_str(), attachProp.m_mtxOffset.ToString().c_str());
			mvAttachedProps.Add(attachProp);
		}

		//////////////////////////
		// Interact connections
		for(size_t i=0; i< pProp->mvInteractConnections.size(); ++i)
		{
			iLuxInteractConnection *pConn = pProp->mvInteractConnections[i];

			iLuxInteractConnection_SaveData *pSaveConn = pConn->CreateSaveData();
			pSaveConn->FromConnection(pConn);
			mvInteractConnections.Add(pSaveConn);
		}
	}

	////////////////////////
	//Connections
	mvConnections.Resize(apEntity->mvConnections.size());
	for(size_t i=0; i<apEntity->mvConnections.size();++i)
	{
		mvConnections[i].FromConnection(apEntity->mvConnections[i]);
	}

	////////////////////////
	//Callbacks
	for(tLuxCollideCallbackListIt it = apEntity->mlstCollideCallbacks.begin(); it != apEntity->mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		cLuxCollideCallback_SaveData saveCallback;
		saveCallback.FromCallback(pCallback);

		mlstCollideCallbacks.Add(saveCallback);
	}
}

//-----------------------------------------------------------------------

void cLuxSavedGameEntity::ToEntity(cLuxMap *apMap,iLuxEntity *apEntity)
{
	////////////////////////
	//Attached Props (Special for props!)
	if(apEntity->GetEntityType() == eLuxEntityType_Prop)
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);

		//////////////////////////
		// Attached props
		for(size_t i=0; i<mvAttachedProps.Size(); ++i)
		{
			cLuxProp_AttachedProp &attachProp = mvAttachedProps[i];

			Log("Loading attached prop: '%s' '%s' (%s)\n", attachProp.msName.c_str(), attachProp.msFileName.c_str(), attachProp.m_mtxOffset.ToString().c_str());

			pProp->AddAndAttachProp(attachProp.msName, attachProp.msFileName, attachProp.m_mtxOffset);
		}

		//////////////////////////
		// Interact connections
		for(size_t i=0; i<mvInteractConnections.Size(); ++i)
		{
			iLuxInteractConnection_SaveData *pSaveConn = mvInteractConnections[i];
			iLuxInteractConnection *pConn = pSaveConn->CreateConnection(apMap);

			if(pConn) pProp->mvInteractConnections.push_back(pConn);
		}
	}

	////////////////////////
	//Connections
	apEntity->mvConnections.resize(mvConnections.Size());
	for(size_t i=0; i<mvConnections.Size();++i)
	{
		apEntity->mvConnections[i] = hplNew( cLuxEntityConnection, () );
		mvConnections[i].ToConnection(apEntity->mvConnections[i], apMap);
	}

	////////////////////////
	//Callbacks
	cContainerListIterator<cLuxCollideCallback_SaveData> it = mlstCollideCallbacks.GetIterator();
	while(it.HasNext())
	{
		cLuxCollideCallback_SaveData& saveCallback = it.Next();
		cLuxCollideCallback *pCallback = hplNew(cLuxCollideCallback, ());

		saveCallback.ToCallback(apMap, apEntity, pCallback);
		apEntity->mlstCollideCallbacks.push_back(pCallback);
	}
}

//////////////////////////////////////////////////////////////////////////
// ENEMY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxSavedGameEnemy::FromEnemy(iLuxEnemy *apEnemy)
{
	mlID = apEnemy->GetID();
	mbActive = false; //apEnemy->IsActive();
	if(apEnemy->GetHealth() <=0) mbActive = false;

	///////////////////////
	//Patrol nodes
	mvPatrolNodes.Resize(apEnemy->GetPatrolNodeNum());
	for(size_t i=0; i<mvPatrolNodes.Size(); ++i)
	{
		mvPatrolNodes[i].mlNodeId = 	apEnemy->GetPatrolNode(i)->mpNode->GetID();
		mvPatrolNodes[i].mfWaitTime = 	apEnemy->GetPatrolNode(i)->mfWaitTime;
		mvPatrolNodes[i].msAnimation = 	apEnemy->GetPatrolNode(i)->msAnimation;
	}
}

//-----------------------------------------------------------------------

void cLuxSavedGameEnemy::ToEnemy(cLuxMap *apMap, iLuxEnemy *apEnemy)
{
	apEnemy->SetActive(mbActive);

	///////////////////////
	//Patrol nodes
	apEnemy->ClearPatrolNodes();
	if(apEnemy->GetPathFinder())
	{
		for(size_t i=0; i<mvPatrolNodes.Size(); ++i)
		{
			cAINode* pNode = apEnemy->GetPathFinder()->GetNodeContainer()->GetNodeFromID(mvPatrolNodes[i].mlNodeId);
			if(pNode==NULL) continue;

			apEnemy->AddPatrolNode(pNode, mvPatrolNodes[i].mfWaitTime, mvPatrolNodes[i].msAnimation);
		}
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// SAVED MAP
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSavedGameMap::cLuxSavedGameMap()
{

}

cLuxSavedGameMap::~cLuxSavedGameMap()
{
    DestroyAll();
}

//-----------------------------------------------------------------------

void cLuxSavedGameMap::DestroyAll()
{
	///////////////////
	//Full Entities
	{
		cContainerListIterator<iLuxEntity_SaveData*> it = mlstFullEntities.GetIterator();
		while(it.HasNext())
		{
			hplDelete(it.Next());
		}
		mlstFullEntities.Clear();
	}


	///////////////////
	// Connect and Collide Entities
	{
		cContainerListIterator<cLuxSavedGameEntity*> it = mlstCollideAndConnectEntities.GetIterator();
		while(it.HasNext())
		{
			hplDelete(it.Next());
		}
		mlstCollideAndConnectEntities.Clear();
	}

	///////////////////
	// Enemies
	{
		cContainerListIterator<cLuxSavedGameEnemy*> it = mlstEnemies.GetIterator();
		while(it.HasNext())
		{
			hplDelete(it.Next());
		}
		mlstEnemies.Clear();
	}

	///////////////////
	//Other
	mlstVars.Clear();
	mlstTimers.Clear();
	mvUseItemCallbacks.Clear();

	mlstPlayerCollideCallbacks.Clear();

	mlstEntityFunc_Look.Clear();
	mlstEntityFunc_Interact.Clear();
	mlstEntityFunc_Callback.Clear();
	mlstEntityFunc_StateChange.Clear();

	mlstSounds.Clear();
	mlstLights.Clear();
	mlstPS.Clear();

	mlstDisabledEntities.Clear();
	mlstLockedDoors.Clear();
	mlstLockedLevelDoors.Clear();
	mlstActiveItems.Clear();
	mlstOpenChests.Clear();
	mlstUnbrokenItemContainers.Clear();
	mlstBrokenContainersWithActiveItem.Clear();
	mlstUnlitLamps.Clear();
}

//-----------------------------------------------------------------------

void cLuxSavedGameMap::FromMap(cLuxMap *apMap)
{
	/////////////////////////////////
	// Init
	cWorld *pWorld = apMap->GetWorld();

	/////////////////////
	//Settings
	msName = apMap->GetName();
	msDisplayNameEntry = apMap->GetDisplayNameEntry();

	mlNumberOfQuests = apMap->mlNumberOfQuests;
	mlTotalCompletionAmount = apMap->mlTotalCompletionAmount;
	mlCurrentCompletionAmount = apMap->mlCurrentCompletionAmount;

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

	/////////////////////
	//Vars
	{
		tLuxScriptVarMapIt it = apMap->m_mapVars.begin();
		for(; it != apMap->m_mapVars.end(); ++it)
		{
			mlstVars.Add(it->second);
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
	// Player callbacks
	{
		tLuxCollideCallbackList *pCallbackList = gpBase->mpPlayer->GetCollideCallbackList();
		for(tLuxCollideCallbackListIt it = pCallbackList->begin(); it != pCallbackList->end(); ++it)
		{
			cLuxCollideCallback *pCallback = *it;
			cLuxCollideCallback_SaveData saveCallback;
			saveCallback.FromCallback(pCallback);

			mlstPlayerCollideCallbacks.Add(saveCallback);
		}
	}

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

			if(	pSound->IsSaved() && pSound->GetEntityParent() == NULL && pSound->GetParent()==NULL)
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

			if(	pPS->IsSaved() && pPS->GetEntityParent() == NULL && pPS->GetParent()==NULL && pPS->IsDying()==false)// && pPS->GetUniqueID()<0)
			{
				cEnginePS_SaveData savePS;
				savePS.FromPS(pPS);

				mlstPS.Add(savePS);
			}
		}
	}


	/////////////////////////////////
	// Connect and Collide Entities
	{
		cLuxEntityIterator it = apMap->GetEntityIterator();
		while(it.HasNext())
		{
			iLuxEntity *pEntity = it.Next();

			eLuxEntityType entityType = pEntity->GetEntityType();
			bool bAddedToColliderAndConnectEntities=false;

			///////////////////////////////////
			//Full entity save
			{
				if(pEntity->GetFullGameSave())
				{
					iLuxEntity_SaveData *pSavedEntity = pEntity->CreateSaveData();
					pEntity->SaveToSaveData(pSavedEntity);
					mlstFullEntities.Add(pSavedEntity);
					
					continue; //No need to save further stuff!
				}
			}
			
			///////////////////////////////////
			//Save collide callbacks or connections
			if(pEntity->HasCollideCallbacks() || pEntity->GetConnectionNum()>0)
			{
				cLuxSavedGameEntity *pSavedEnt = hplNew(cLuxSavedGameEntity, () );
				pSavedEnt->FromEntity(pEntity);
				mlstCollideAndConnectEntities.Add(pSavedEnt);
				
				bAddedToColliderAndConnectEntities = true;
			}

			///////////////////////////////////
			//Full functions
			{
				// Look 
				if(pEntity->msLookAtCallback != "") 
					mlstEntityFunc_Look.Add(cLuxSavedGameEntityFunc(pEntity->GetID(), pEntity->msLookAtCallback, pEntity->mbLookAtCallbackRemove));
				
				// Interact
				if(pEntity->msInteractCallback != "") 
					mlstEntityFunc_Interact.Add(cLuxSavedGameEntityFunc(pEntity->GetID(), pEntity->msInteractCallback, pEntity->mbInteractCallbackRemove));

				// Callback
				if(pEntity->msCallbackFunc != "") 
					mlstEntityFunc_Callback.Add(cLuxSavedGameEntityFunc(pEntity->GetID(), pEntity->msCallbackFunc, false));

				// State Change
				if(pEntity->msConnectionStateChangeCallback != "") 
					mlstEntityFunc_StateChange.Add(cLuxSavedGameEntityFunc(pEntity->GetID(), pEntity->msConnectionStateChangeCallback, false));
			}

			///////////////////////////////////
			//Active
			if(pEntity->IsActive()==false)	mlstDisabledEntities.Add(pEntity->GetID());
			
			
			///////////////////////////////////
			//Enemy
			if(entityType == eLuxEntityType_Enemy)
			{
				iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);
				
				cLuxSavedGameEnemy *pSavedEnemy = hplNew(cLuxSavedGameEnemy, () );
				pSavedEnemy->FromEnemy(pEnemy);	

				mlstEnemies.Add(pSavedEnemy);
			}
			///////////////////////////////////
			//Prop
			else if(entityType == eLuxEntityType_Prop)
			{
				iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
				eLuxPropType propType = pProp->GetPropType();

				//////////////
				// If has attached prop (and is not yet added) add to collide and connect!
				if(	bAddedToColliderAndConnectEntities==false && 
					(pProp->HasAttachedProp() || pProp->GetInteractionConnectionNum()>0) )
				{
					cLuxSavedGameEntity *pSavedEnt = hplNew(cLuxSavedGameEntity, () );
					pSavedEnt->FromEntity(pEntity);
					mlstCollideAndConnectEntities.Add(pSavedEnt);
				}

				//////////////
				//Swing door
				if(propType == eLuxPropType_SwingDoor)
				{
					cLuxProp_SwingDoor *pSwingDoor = static_cast<cLuxProp_SwingDoor*>(pProp);
					
					if(pSwingDoor->GetLocked()) mlstLockedDoors.Add(pSwingDoor->GetID());					
				}
				//////////////
				//Level door
				else if(propType == eLuxPropType_LevelDoor)
				{
					cLuxProp_LevelDoor *pLevelDoor = static_cast<cLuxProp_LevelDoor*>(pProp);

					if(pLevelDoor->GetLocked()) mlstLockedLevelDoors.Add(pLevelDoor->GetID());					
				}
				//////////////
				//Item
				else if(propType == eLuxPropType_Item)
				{
					cLuxProp_Item *pItem = static_cast<cLuxProp_Item*>(pProp);
					
					//Item was spawned from object
					if(pItem->GetSpawnContainerID() >= 0)
					{
						mlstBrokenContainersWithActiveItem.Add(pItem->GetSpawnContainerID());
					}
					//Item placed in editor
					else
					{
						mlstActiveItems.Add(pEntity->GetID());
					}
				}
				//////////////
				//Object
				else if(propType == eLuxPropType_Object)
				{
					cLuxProp_Object *pObject = static_cast<cLuxProp_Object*>(pProp);

					if(pObject->GetContainedItem() != "")
					{
						mlstUnbrokenItemContainers.Add(pEntity->GetID());
					}
				}
				//////////////
				//Chest
				else if(propType == eLuxPropType_Chest)
				{
					cLuxProp_Chest *pChest = static_cast<cLuxProp_Chest*>(pProp);
					
					if(pChest->GetLocked()==false) mlstOpenChests.Add(pEntity->GetID()); 
				}
				//////////////
				//Lamp
				else if(propType == eLuxPropType_Lamp)
				{
					cLuxProp_Lamp *pLamp = static_cast<cLuxProp_Lamp*>(pProp);

					if(pLamp->GetLit()==false) mlstUnlitLamps.Add(pEntity->GetID()); 
				}
			}
		}
	}
}

//-----------------------------------------------------------------------

template <class T>
static void ListToSet(cContainerList<T> &aList, std::set<T> &aSet)
{
	cContainerListIterator<T> it = aList.GetIterator();
    while(it.HasNext())
	{
		aSet.insert(it.Next());
	}
}

template <class T>
static bool ExistsInSet(T aVar, std::set<T> &aSet)
{
	typename std::set<T>::iterator it = aSet.find(aVar);
    if(it == aSet.end()) return false;
	return true;
}

//-----------------------------------------------------------------------

void cLuxSavedGameMap::ToMap(cLuxMap *apMap)
{
	/////////////////////////////////
	// Init
	cWorld *pWorld = apMap->GetWorld();

	apMap->SetDisplayNameEntry(msDisplayNameEntry);

	/////////////////////
	//Vars
	apMap->mlNumberOfQuests = mlNumberOfQuests;
	apMap->mlTotalCompletionAmount = mlTotalCompletionAmount;
	apMap->mlCurrentCompletionAmount = mlCurrentCompletionAmount;

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
	

	/////////////////////
	//Script Vars
	{
		apMap->m_mapVars.clear();
		cContainerListIterator<cLuxScriptVar> it = mlstVars.GetIterator();
		while(it.HasNext())
		{
			cLuxScriptVar& scriptVar = it.Next();
			apMap->m_mapVars.insert(tLuxScriptVarMap::value_type(scriptVar.msName, scriptVar));
		}
	}

	/////////////////////////////////
	// Use Item callbacks
	{
		STLDeleteAll(apMap->mlstUseItemCallbacks);
		for(size_t i=0; i<mvUseItemCallbacks.Size(); ++i)
		{
			cLuxUseItemCallback *pCallback = hplNew(cLuxUseItemCallback, ());
			*pCallback = mvUseItemCallbacks[i];
			apMap->mlstUseItemCallbacks.push_back(pCallback);
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
	
	/////////////////////////////////
	// Saved connections and collision callbacks
	{
		cContainerListIterator<cLuxSavedGameEntity*> it = mlstCollideAndConnectEntities.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEntity *pSavedEnt = it.Next();

			iLuxEntity *pEntity = apMap->GetEntityByID(pSavedEnt->mlID);
			if(pEntity==NULL)
			{
				Error("Saved entity with ID %d does not exist!\n", pSavedEnt->mlID);
				continue;
			}

			pSavedEnt->ToEntity(apMap, pEntity);
		}
	}

	/////////////////////////////////
	// Enemies
	{
		cContainerListIterator<cLuxSavedGameEnemy*> it = mlstEnemies.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEnemy *pSavedEnemy = it.Next();

			iLuxEntity *pEntity = apMap->GetEntityByID(pSavedEnemy->mlID);
			if(pEntity==NULL || pEntity->GetEntityType() != eLuxEntityType_Enemy)
			{
				Error("Saved enemy with ID %d does not exist!\n", pSavedEnemy->mlID);
				continue;
			}
			iLuxEnemy *pEnemy = static_cast<iLuxEnemy*>(pEntity);

			pSavedEnemy->ToEnemy(apMap, pEnemy);
		}
	}

	/////////////////////////////////
	// Player collide callbacks
	{
		tLuxCollideCallbackList *pPlayerCallbackList = gpBase->mpPlayer->GetCollideCallbackList();
		
		cContainerListIterator<cLuxCollideCallback_SaveData> it = mlstPlayerCollideCallbacks.GetIterator();
		while(it.HasNext())
		{
			cLuxCollideCallback_SaveData& saveCallback = it.Next();
			cLuxCollideCallback *pCallback = hplNew(cLuxCollideCallback, ());

			saveCallback.ToCallback(apMap,gpBase->mpPlayer , pCallback);
			pPlayerCallbackList->push_back(pCallback);
		}
	}


	/////////////////////////////////
	// Functions
	{

		///////////////////
		//Clear settings on not full game saved stuff
		cLuxEntityIterator entIt = apMap->GetEntityIterator();
		while(entIt.HasNext())
		{
			iLuxEntity *pEntity = entIt.Next();
			if(pEntity->GetFullGameSave()) continue;

			pEntity->SetPlayerLookAtCallback("", false);
			pEntity->SetPlayerInteractCallback("", false);
			pEntity->SetCallbackFunc("");
			pEntity->SetConnectionStateChangeCallback("");
		}

		///////////////////
		//Look
		cContainerListIterator<cLuxSavedGameEntityFunc> it = mlstEntityFunc_Look.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEntityFunc& saveFunc = it.Next();
			iLuxEntity *pEntity = apMap->GetEntityByID(saveFunc.mlID);
			if(pEntity)
				pEntity->SetPlayerLookAtCallback(saveFunc.msFunc, saveFunc.mbAutoRemove);
		}

		///////////////////
		//Interact
		it = mlstEntityFunc_Interact.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEntityFunc& saveFunc = it.Next();
			iLuxEntity *pEntity = apMap->GetEntityByID(saveFunc.mlID);
			if(pEntity)
				pEntity->SetPlayerInteractCallback(saveFunc.msFunc, saveFunc.mbAutoRemove);
		}

		///////////////////
		//Callback
		it = mlstEntityFunc_Callback.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEntityFunc& saveFunc = it.Next();
			iLuxEntity *pEntity = apMap->GetEntityByID(saveFunc.mlID);
			if(pEntity)
				pEntity->SetCallbackFunc(saveFunc.msFunc);
		}

		///////////////////
		//State change
		it = mlstEntityFunc_StateChange.GetIterator();
		while(it.HasNext())
		{
			cLuxSavedGameEntityFunc& saveFunc = it.Next();
			iLuxEntity *pEntity = apMap->GetEntityByID(saveFunc.mlID);
			if(pEntity)
				pEntity->SetConnectionStateChangeCallback(saveFunc.msFunc);
		}
	}

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
			
			cSoundEntity *pSound;

			if(saveSound.mlID >=0)
				pSound = pWorld->GetSoundEntityFromUniqueID(saveSound.mlID);
			else
				pSound = pWorld->CreateSoundEntity(saveSound.msName, saveSound.msSoundDataName, saveSound.mbRemoveWhenOver);
			
			if(pSound)
			{
				saveSound.ToSound(pSound);	
			}
			else
			{
				Warning("Could not create or find sound '%s' of file '%s'\n",saveSound.msName.c_str(), saveSound.msSoundDataName.c_str());
			}
		}
	}

	/////////////////////////////////
	// Particle Systems
	{
		std::set<int> setSavedPS;

		///////////////////////////////
		//Iterate saved PS and either create or update
		cContainerListIterator<cEnginePS_SaveData> savePSIt = mlstPS.GetIterator();
		while (savePSIt.HasNext())
		{
			cEnginePS_SaveData& savePS = savePSIt.Next();
			
			///////////////////////
			// PS add in editor
			if(savePS.mlID >=0)
			{
				cParticleSystem *pPS = pWorld->GetParticleSystemFromUniqueID(savePS.mlID);
				if(pPS)
				{
					savePS.ToPS(pPS);	
				}
				setSavedPS.insert(savePS.mlID);
			}
			///////////////////////
			// PS created by script
			else
			{
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

		///////////////////////////////
		//Iterate all particle systems and destroy those that where not saved (they must have been destroyed)
		cParticleSystemIterator psIt = pWorld->GetParticleSystemIterator();
		while(psIt.HasNext())
		{
			cParticleSystem *pPS = psIt.Next();

			if(	pPS->IsSaved() && pPS->GetEntityParent() == NULL && pPS->GetParent()==NULL && pPS->GetUniqueID()>=0)
			{
				if(ExistsInSet(pPS->GetUniqueID(),setSavedPS)==false)
				{
					pPS->KillInstantly();
				}
			}
		}
	}

	
	/////////////////////////////////
	// Full Game Entities
	tLuxEntityList lstSetupEntities;
	{
		gpBase->mpCurrentMapLoading = apMap;

		/////////////////////////////////////
		//Create (if needed) and Load data for entities
		cContainerListIterator<iLuxEntity_SaveData*> it = mlstFullEntities.GetIterator();
		while(it.HasNext())
		{
			iLuxEntity_SaveData *pSavedEntity = it.Next();
			iLuxEntity *pEntity = apMap->GetEntityByID(pSavedEntity->mlID);

			if(pEntity==NULL)
			{
				pEntity = pSavedEntity->CreateEntity(apMap);
			}

			if(pEntity)
			{
				pEntity->LoadFromSaveData(pSavedEntity);
				pEntity->SetSaveData(pSavedEntity);
				lstSetupEntities.push_back(pEntity);
			}
		}

		/////////////////////////////////////
		//Any full game entities that do NOT exist in save data must be removed
		cLuxEntityIterator entIt = apMap->GetEntityIterator();
		while(entIt.HasNext())
		{
			iLuxEntity *pEntity = entIt.Next();
			if(pEntity->GetFullGameSave() && EntitySaveDataExists(pEntity->GetID())==false)
			{
				apMap->DestroyEntity(pEntity);
			}
		}
		apMap->UpdateToBeDesotroyedEntities(false);

		gpBase->mpCurrentMapLoading = NULL;
	}

	/////////////////////////////////
	// Entities setting
	{
		std::set<int> setDisabledEntities, setLockedDoors,setLockedLevelDoors, setActiveItems, setUnbrokenItemContainers, setBrokenContainersWithActiveItem, setOpenChests, setUnlitLamps;
		ListToSet(mlstDisabledEntities, setDisabledEntities);
		ListToSet(mlstLockedDoors, setLockedDoors);
		ListToSet(mlstLockedLevelDoors, setLockedLevelDoors);
		ListToSet(mlstActiveItems, setActiveItems);
		ListToSet(mlstOpenChests, setOpenChests); 
		ListToSet(mlstUnbrokenItemContainers, setUnbrokenItemContainers);
		ListToSet(mlstBrokenContainersWithActiveItem, setBrokenContainersWithActiveItem);
		ListToSet(mlstUnlitLamps, setUnlitLamps);
		
		
		cLuxEntityIterator it = apMap->GetEntityIterator();
		while(it.HasNext())
		{
			iLuxEntity *pEntity = it.Next();
			
			int lID = pEntity->GetID();
			eLuxEntityType entityType = pEntity->GetEntityType();

			if(pEntity->GetFullGameSave() || entityType == eLuxEntityType_Enemy)
			{
				continue;
			}
			
			///////////////////////////
			// Active
			if(ExistsInSet(lID, setDisabledEntities))
				pEntity->SetActive(false);
			else
				pEntity->SetActive(true);
			
			///////////////////////////
			// Props
			if(entityType == eLuxEntityType_Prop)
			{
				iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
                eLuxPropType propType = pProp->GetPropType();

				////////////////
				//Swing door
				if(propType == eLuxPropType_SwingDoor)
				{
					cLuxProp_SwingDoor *pSwingDoor = static_cast<cLuxProp_SwingDoor*>(pProp);
					if(ExistsInSet(lID, setLockedDoors))
						pSwingDoor->SetLocked(true, false);
					else
						pSwingDoor->SetLocked(false, false);
				}
				////////////////
				//Level door
				else if(propType == eLuxPropType_LevelDoor)
				{
					cLuxProp_LevelDoor *pLevelDoor = static_cast<cLuxProp_LevelDoor*>(pProp);
					if(ExistsInSet(lID, setLockedLevelDoors))
						pLevelDoor->SetLocked(true);
					else
						pLevelDoor->SetLocked(false);
				}	
				////////////////
				//Item
                else if(propType == eLuxPropType_Item)
				{
					//Need to check if saved, else we might delete an item that is attached to a prop!
					if(pEntity->IsSaved() && ExistsInSet(lID, setActiveItems)==false)
					{
						apMap->DestroyEntity(pEntity);
					}
				}
				//////////////
				//Object
				else if(propType == eLuxPropType_Object)
				{
					cLuxProp_Object *pObject = static_cast<cLuxProp_Object*>(pProp);

					if(pObject->GetContainedItem() != "")
					{
						//If the object is broken and has not any unpicked item remove
						if(ExistsInSet(lID, setUnbrokenItemContainers)==false && ExistsInSet(lID, setBrokenContainersWithActiveItem)==false)
						{
							pObject->SetContainedItem("");
						}
					}
				}
				//////////////
				//Chest
				else if(propType == eLuxPropType_Chest)
				{
					cLuxProp_Chest *pChest = static_cast<cLuxProp_Chest*>(pProp);

					if(ExistsInSet(lID, setOpenChests))
						pChest->SetLocked(false, false);
					else
						pChest->SetLocked(true, false);
				}
				//////////////
				//Lamp
				else if(propType == eLuxPropType_Lamp)
				{
					cLuxProp_Lamp *pLamp = static_cast<cLuxProp_Lamp*>(pProp);

					if(ExistsInSet(lID, setUnlitLamps))
						pLamp->SetLit(false, false);
					else
						pLamp->SetLit(true, false);
				}
			}
		}
	}

	/////////////////////
	//Entities Setup
	{
		gpBase->mpCurrentMapLoading = apMap;

		for(tLuxEntityListIt it = lstSetupEntities.begin(); it != lstSetupEntities.end(); ++it)
		{
			iLuxEntity *pEntity = *it;

			pEntity->SetupSaveData(pEntity->GetSaveData());
			if(pEntity->IsActive())
				pEntity->UpdateLogic(0.001f);
		}

		gpBase->mpCurrentMapLoading = NULL;
	}

}

//-----------------------------------------------------------------------

bool cLuxSavedGameMap::EntitySaveDataExists(int alID)
{
	cContainerListIterator<iLuxEntity_SaveData*> it = mlstFullEntities.GetIterator();
	while(it.HasNext())
	{
		iLuxEntity_SaveData *pSavedEntity = it.Next();
		
		if(pSavedEntity->mlID == alID) return true;
	}
	return false;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVED MAP COLLECTION
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxSavedGameMapCollection::cLuxSavedGameMapCollection()
{
		
}

cLuxSavedGameMapCollection::~cLuxSavedGameMapCollection()
{
	cContainerListIterator<cLuxSavedGameMap*> it = mlstMaps.GetIterator();
	while(it.HasNext())
	{
		hplDelete( it.Next() );
	}
	mlstMaps.Clear();
}

//-----------------------------------------------------------------------

void cLuxSavedGameMapCollection::Reset()
{
	cContainerListIterator<cLuxSavedGameMap*> it = mlstMaps.GetIterator();
	while(it.HasNext())
	{
		hplDelete( it.Next() );
	}
	mlstMaps.Clear();
}

//-----------------------------------------------------------------------

void cLuxSavedGameMapCollection::SaveMap(cLuxMap *apMap)
{
	cLuxSavedGameMap *pSavedMap = GetSavedMap(apMap->GetName(), true);

	pSavedMap->DestroyAll();
	pSavedMap->FromMap(apMap);
	//cSerializeClass::SaveToFile(pSavedMap, cString::To16Char(apMap->GetName())+_W(".testsave"), "SavedMap");
}

//-----------------------------------------------------------------------

void cLuxSavedGameMapCollection::LoadMap(cLuxMap *apMap)
{
	cLuxSavedGameMap *pSavedMap = GetSavedMap(apMap->GetName(), false);
	if(pSavedMap==NULL) return;

	pSavedMap->ToMap(apMap);
}

//-----------------------------------------------------------------------

bool cLuxSavedGameMapCollection::MapExists(const tString& asName)
{
	cContainerListIterator<cLuxSavedGameMap*> it = mlstMaps.GetIterator();
	while(it.HasNext())
	{
		cLuxSavedGameMap *pSaveMap = it.Next();
		if(pSaveMap->msName == asName) return true;
	}
	return false;
}

//-----------------------------------------------------------------------

cLuxSavedGameMap* cLuxSavedGameMapCollection::GetSavedMap(const tString& asName, bool abCreateNew)
{
	//See if map exists and if so return.
	cContainerListIterator<cLuxSavedGameMap*> it = mlstMaps.GetIterator();
	while(it.HasNext())
	{
		cLuxSavedGameMap *pSaveMap = it.Next();
		if(pSaveMap->msName == asName) return pSaveMap;
	}

	if(abCreateNew==false) return NULL;

	//If not exist, create and return.
	cLuxSavedGameMap *pSaveMap = hplNew(cLuxSavedGameMap, () );
	mlstMaps.Add(pSaveMap);
	return pSaveMap;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SERIALIZABLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxSavedGameEntity)

kSerializeVar(mlID, eSerializeType_Int32)

kSerializeClassContainer(mvAttachedProps, cLuxProp_AttachedProp, eSerializeType_Class)
kSerializeClassContainer(mvInteractConnections, iLuxInteractConnection_SaveData, eSerializeType_ClassPointer)
kSerializeClassContainer(mvConnections, cLuxEntityConnection_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstCollideCallbacks, cLuxCollideCallback_SaveData, eSerializeType_Class)

kEndSerialize()


//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxSavedGameEnemy)

kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(mbActive, eSerializeType_Bool)

kSerializeClassContainer(mvPatrolNodes, cLuxEnemyPatrolNode_SaveData, eSerializeType_Class)

kEndSerialize()


kBeginSerializeBase(cLuxSavedGameEntityFunc)

kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(msFunc, eSerializeType_String)
kSerializeVar(mbAutoRemove, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------



kBeginSerializeBase(cLuxSavedGameMap)

kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msDisplayNameEntry, eSerializeType_String)

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

kSerializeClassContainer(mlstFullEntities, iLuxEntity_SaveData, eSerializeType_ClassPointer)

kSerializeClassContainer(mlstCollideAndConnectEntities, cLuxSavedGameEntity, eSerializeType_ClassPointer)
kSerializeClassContainer(mlstEnemies, cLuxSavedGameEnemy, eSerializeType_ClassPointer)

kSerializeClassContainer(mlstTimers, cLuxEventTimer, eSerializeType_Class)

kSerializeClassContainer(mlstVars, cLuxScriptVar, eSerializeType_Class)
kSerializeClassContainer(mvUseItemCallbacks, cLuxUseItemCallback, eSerializeType_Class)

kSerializeClassContainer(mlstPlayerCollideCallbacks, cLuxCollideCallback_SaveData, eSerializeType_Class)

kSerializeClassContainer(mlstLights, cEngineLight_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstSounds, cEngineSound_SaveData, eSerializeType_Class)
kSerializeClassContainer(mlstPS, cEnginePS_SaveData, eSerializeType_Class)

kSerializeClassContainer(mlstEntityFunc_Look, cLuxSavedGameEntityFunc, eSerializeType_Class)
kSerializeClassContainer(mlstEntityFunc_Interact, cLuxSavedGameEntityFunc, eSerializeType_Class)
kSerializeClassContainer(mlstEntityFunc_Callback, cLuxSavedGameEntityFunc, eSerializeType_Class)
kSerializeClassContainer(mlstEntityFunc_StateChange, cLuxSavedGameEntityFunc, eSerializeType_Class)

kSerializeVarContainer(mlstDisabledEntities, eSerializeType_Int32)
kSerializeVarContainer(mlstLockedDoors, eSerializeType_Int32)
kSerializeVarContainer(mlstLockedLevelDoors, eSerializeType_Int32)
kSerializeVarContainer(mlstActiveItems, eSerializeType_Int32)
kSerializeVarContainer(mlstOpenChests, eSerializeType_Int32)
kSerializeVarContainer(mlstUnbrokenItemContainers, eSerializeType_Int32)
kSerializeVarContainer(mlstBrokenContainersWithActiveItem, eSerializeType_Int32)
kSerializeVarContainer(mlstUnlitLamps, eSerializeType_Int32)

kEndSerialize()

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxSavedGameMapCollection)

kSerializeClassContainer(mlstMaps,cLuxSavedGameMap, eSerializeType_ClassPointer)

kEndSerialize()


//-----------------------------------------------------------------------

