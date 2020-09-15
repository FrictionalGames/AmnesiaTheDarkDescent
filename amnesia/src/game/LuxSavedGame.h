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

#ifndef LUX_SAVED_GAME_H
#define LUX_SAVED_GAME_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxSavedEngineTypes.h"
#include "LuxSavedGameTypes.h"

//----------------------------------------------

#include "LuxEnemy.h"	//<- This is a bit bad... but what u gonna do?

//----------------------------------------------

class iLuxEntity_SaveData;
class iLuxInteractConnection_SaveData;
class cLuxMap;

//----------------------------------------------

class cLuxSavedGameEntity : public iSerializable
{
	kSerializableClassInit(cLuxSavedGameEntity)
public:
	virtual ~cLuxSavedGameEntity();

	void FromEntity(iLuxEntity *apEntity);
	void ToEntity(cLuxMap *apMap, iLuxEntity *apEntity);

	int mlID;

	//Prop specfics:
	cContainerVec<cLuxProp_AttachedProp> mvAttachedProps;
	cContainerVec<iLuxInteractConnection_SaveData*> mvInteractConnections;

	//General
	cContainerVec<cLuxEntityConnection_SaveData> mvConnections;
	cContainerList<cLuxCollideCallback_SaveData> mlstCollideCallbacks;
};

//----------------------------------------------

class cLuxSavedGameEnemy : public iSerializable
{
	kSerializableClassInit(cLuxSavedGameEnemy)
public:
	void FromEnemy(iLuxEnemy *apEnemy);
	void ToEnemy(cLuxMap *apMap, iLuxEnemy *apEnemy);

	int mlID;
	bool mbActive;

	cContainerVec<cLuxEnemyPatrolNode_SaveData> mvPatrolNodes;
};

//----------------------------------------------

class cLuxSavedGameEntityFunc : public iSerializable
{
	kSerializableClassInit(cLuxSavedGameEntityFunc)
public:
	cLuxSavedGameEntityFunc() {}
	cLuxSavedGameEntityFunc(int alID, const tString& asFunc, bool abAutoRemove) : mlID(alID), msFunc(asFunc), mbAutoRemove(abAutoRemove){}

	int mlID;
    tString msFunc;	
	bool mbAutoRemove;
};

//----------------------------------------------


class cLuxSavedGameMap : public iSerializable
{
	kSerializableClassInit(cLuxSavedGameMap)
public:	
	cLuxSavedGameMap();
	~cLuxSavedGameMap();

	void DestroyAll();

	void FromMap(cLuxMap *apMap);
	void ToMap(cLuxMap *apMap);
	
	tString msName;
	tString msDisplayNameEntry;

	int mlNumberOfQuests;
	int mlTotalCompletionAmount;
	int mlCurrentCompletionAmount;

	bool mbSkyBoxActive;
	tString msSkyboxTexture;
	cColor mSkyBoxColor;

	bool mbFogActive;
	bool mbFogCulling;
	float mfFogStart;
	float mfFogEnd;
	float mfFogFalloffExp;
	cColor mFogColor;

	cContainerList<iLuxEntity_SaveData*> mlstFullEntities;
	
	cContainerList<cLuxSavedGameEntity*> mlstCollideAndConnectEntities;
	cContainerList<cLuxSavedGameEnemy*> mlstEnemies;

	cContainerList<cEngineLight_SaveData> mlstLights;
	cContainerList<cEngineSound_SaveData> mlstSounds;
	cContainerList<cEnginePS_SaveData> mlstPS;

	cContainerList<cLuxEventTimer> mlstTimers;

	cContainerList<cLuxScriptVar> mlstVars;
	cContainerVec<cLuxUseItemCallback> mvUseItemCallbacks;

	cContainerList<cLuxCollideCallback_SaveData> mlstPlayerCollideCallbacks;
	
	cContainerList<cLuxSavedGameEntityFunc> mlstEntityFunc_Look;
	cContainerList<cLuxSavedGameEntityFunc> mlstEntityFunc_Interact;
	cContainerList<cLuxSavedGameEntityFunc> mlstEntityFunc_Callback;
	cContainerList<cLuxSavedGameEntityFunc> mlstEntityFunc_StateChange;

    cContainerList<int> mlstDisabledEntities;
	cContainerList<int> mlstLockedDoors;
	cContainerList<int> mlstLockedLevelDoors;
	cContainerList<int> mlstActiveItems;
	cContainerList<int> mlstOpenChests;
	cContainerList<int> mlstUnbrokenItemContainers;
	cContainerList<int> mlstBrokenContainersWithActiveItem;
	cContainerList<int> mlstUnlitLamps;
private:
	bool EntitySaveDataExists(int alID);
};

//----------------------------------------------

class cLuxSavedGameMapCollection : public iSerializable
{
	kSerializableClassInit(cLuxSavedGameMapCollection)
public:	
	cLuxSavedGameMapCollection();
	~cLuxSavedGameMapCollection();

	void Reset();

	void SaveMap(cLuxMap *apMap);
	void LoadMap(cLuxMap *apMap);

	bool MapExists(const tString& asName);
	
	cLuxSavedGameMap* GetSavedMap(const tString& asName, bool abCreateNew);
	
public:
	cContainerList<cLuxSavedGameMap*> mlstMaps;
};

//----------------------------------------------


#endif // LUX_SAVED_GAME_H
