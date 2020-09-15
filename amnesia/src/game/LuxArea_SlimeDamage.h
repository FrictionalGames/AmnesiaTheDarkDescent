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

#ifndef LUX_AREA_SLIME_DAMAGE_H
#define LUX_AREA_SLIME_DAMAGE_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_SlimeDamage_SaveData : public iLuxArea_SaveData
{
	kSerializableClassInit(cLuxArea_SlimeDamage_SaveData)
public:
	iLuxArea* CreateArea(cLuxMap *apMap);

	int mlSlimeType;

	float mfMinCheckAttackTime;
	float mfMaxCheckAttackTime;
	bool mbDisableAfterAttack;
	tString msCallback;

	float mfCheckCollisionCount;

	tString msAttackSound;
	tString msAttackPS;
	float mfMinAttackDamage;
	float mfMaxAttackDamage;
	float mfScreenShakeAmount;
};

//----------------------------------------------

class cLuxArea_SlimeDamage : public iLuxArea
{
typedef iLuxArea super_class;
friend class cLuxAreaLoader_SlimeDamage;
public:	
	cLuxArea_SlimeDamage(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxArea_SlimeDamage();

	//////////////////////
	//General
	void OnUpdate(float afTimeStep);

	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}

	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);
protected:

private:
	
	/////////////////////////
	// Data
	int mlSlimeType;

	float mfMinCheckAttackTime;
	float mfMaxCheckAttackTime;
	bool mbDisableAfterAttack;
	tString msCallback;

	tString msAttackSound;
	tString msAttackPS;
	float mfMinAttackDamage;
	float mfMaxAttackDamage;
	float mfScreenShakeAmount;

	/////////////////////////
	// Variables
	float mfCheckCollisionCount;
};

//----------------------------------------------

class cLuxAreaLoader_SlimeDamage : public iLuxAreaLoader
{
public:
	cLuxAreaLoader_SlimeDamage(const tString& asName);
	~cLuxAreaLoader_SlimeDamage();

	iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);
	
	void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
	void SetupArea(iLuxArea *apArea, cWorld *apWorld);
	
};

//----------------------------------------------


#endif // LUX_AREA_SLIME_DAMAGE_H
