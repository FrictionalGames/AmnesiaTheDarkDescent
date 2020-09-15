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

#ifndef LUX_ENEMY_GRUNT_H
#define LUX_ENEMY_GRUNT_H

//----------------------------------------------

#include "LuxEnemy.h"

//----------------------------------------------

class cLuxEnemy_Grunt_SaveData : public iLuxEnemy_SaveData
{
	kSerializableClassInit(cLuxEnemy_Grunt_SaveData)
public:
	////////////////
	//Properties
		
};

//----------------------------------------------

class cLuxEnemy_Grunt : public iLuxEnemy
{
friend class cLuxEnemyLoader_Grunt;
typedef iLuxEnemy super_class;
public:	
	cLuxEnemy_Grunt(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxEnemy_Grunt();

	//////////////////////
	//General
	void OnSetupAfterLoad(cWorld *apWorld);
	void OnAfterWorldLoad();
	void UpdateEnemySpecific(float afTimeStep);

	//////////////////////
	//Actions
	bool StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage);


	//////////////////////
	//Debug
	void OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions);
		
	
	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);
	
private:
	//////////////////////
	// Callbacks
	bool PlayerIsDetected();

	void OnDisableTriggers();

	float GetDamageMul(float afAmount, int alStrength);

	//////////////////////
	// State stuff
	void PatrolUpdateGoal();
	void PatrolEndOfPath();
	
	//////////////
	//Data
	tString msNoticeSound;
	tString msGiveUpNoticeSound;
	tString msEnabledSound;

	float mfGroggyDamageCount;
	float mfAlertToHuntDistance;
	float mfAlertToInstantHuntDistance;
	float mfHuntPauseMinTime;
	float mfHuntPauseMaxTime;
	float mfIncreaseAlertSpeedDistance;
	float mfIncreasedAlertSpeedMul;
	float mfAlertRunTowardsToHuntLimit;
	float mfAlertRunTowardsCheckDistance;

	float mfIdleExtraTimeMin;
	float mfIdleExtraTimeMax;
	int mlIdleExtraNum;
	
	
	//////////////
	//Variables
	float mfWaitTime;
	float mfAlertRunTowardsCount;
	
};

//----------------------------------------------

class cLuxEnemyLoader_Grunt : public iLuxEnemyLoader
{
public:
	cLuxEnemyLoader_Grunt(const tString& asName);
	virtual ~cLuxEnemyLoader_Grunt(){}

	iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------


#endif // LUX_ENEMY_GRUNT_H
