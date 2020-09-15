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

#ifndef LUX_ENEMY_WATER_LURKER_H
#define LUX_ENEMY_WATER_LURKER_H

//----------------------------------------------

#include "LuxEnemy.h"

//----------------------------------------------

class cLuxEnemy_WaterLurker_SaveData : public iLuxEnemy_SaveData
{
	kSerializableClassInit(cLuxEnemy_WaterLurker_SaveData)
public:
	////////////////
	//Properties
	float mfPlayerDetectionHeight;	
};

//----------------------------------------------

enum eWaterLurkerSplash
{
	eWaterLurkerSplash_Walk,
	eWaterLurkerSplash_Run,
	eWaterLurkerSplash_Eat,
	eWaterLurkerSplash_Attack,
};

//----------------------------------------------

class cLuxEnemy_WaterLurker : public iLuxEnemy
{
friend class cLuxEnemyLoader_WaterLurker;
typedef iLuxEnemy super_class;
public:	
	cLuxEnemy_WaterLurker(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxEnemy_WaterLurker();

	//////////////////////
	//General
	void OnSetupAfterLoad(cWorld *apWorld);
	void OnAfterWorldLoad();
	void UpdateEnemySpecific(float afTimeStep);

	//////////////////////
	//Actions
	bool StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage);

	//////////////////////
	//Callbacks
	bool InRangeOfFood(iPhysicsBody *apFoodBody);

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
	// Actions
	void SplashWater(eWaterLurkerSplash aType);

	//////////////////////
	// Callbacks
	bool PlayerIsDetected();
	void OnSetActiveEnemySpecific(bool abX);

	float GetDamageMul(float afAmount, int alStrength);

	//////////////////////
	// State stuff
	void PatrolUpdateGoal();
	void PatrolEndOfPath();
	
	//////////////
	//Data
	float mfPlayerDetectionHeight;
	float mfEatDamage;
	float mfAfterAttackPauseTime;

	tString msSplashPS_Walk;
	tString msSplashPS_Run;
	tString msSplashPS_Eat;
	tString msSplashPS_Attack;

	tString msSplashSound_Walk;
	tString msSplashSound_Run;
	tString msSplashSound_Eat;
	tString msSplashSound_Attack;
	
	//////////////
	//Variables

	
};

//----------------------------------------------

class cLuxEnemyLoader_WaterLurker : public iLuxEnemyLoader
{
public:
	cLuxEnemyLoader_WaterLurker(const tString& asName);
	virtual ~cLuxEnemyLoader_WaterLurker(){}

	iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------


#endif // LUX_ENEMY_WATER_LURKER_H
