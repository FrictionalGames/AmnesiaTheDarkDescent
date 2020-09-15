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

#ifndef LUX_CRITTER_SPIDER_H
#define LUX_CRITTER_SPIDER_H

//----------------------------------------------

#include "LuxProp_CritterBase.h"

//----------------------------------------------

class cLuxCritter_Spider_SaveData : public iLuxProp_CritterBase_SaveData
{
	kSerializableClassInit(cLuxCritter_Spider_SaveData)
public:
	int mlState;
	float mfFallingCount;
	float mfNewGroundNormalCount;
	bool mbOnGround;
	float mfMaxSpeed;
	float mfCount;
	float mfPlaySoundCount;
};

//----------------------------------------------

class cLuxCritter_Spider : public iLuxProp_CritterBase
{
typedef iLuxProp_CritterBase super_class;
friend class cLuxPropLoader_Critter_Spider;
friend class cLuxCritter_SpiderRayCallback;
public:	
	cLuxCritter_Spider(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxCritter_Spider();

	//////////////////////
	//General
	void UpdateCritterSpecific(float afTimeStep);
	void OnShapeCollision(const cVector3f& avPushVec, float afTimeStep);
	
	void OnDamageCritter(float afAmount);
	void OnKillCritter();
	
	float DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY);


	//////////////////////
	//Properties
	
	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}
	
	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);


private:
	
	void UpdateVelocity(float afTimeStep);
	void UpdateGroundCheck(float afTimeStep);
	
	//Data
	float mfMaxIdleSpeed;
	float mfMaxHuntSpeed;
	
	tString msHitSound;
	tString msIdleSound;
	cVector2f mvIdleSoundRandMinMax;
	tString msAttackSound;

	cVector2f mvAttackPlayerDistMinMax;
	float mfHuntPlayerDist;
	cVector2f mvHuntTimeMinMax;
	cVector2f mvMoveTimeMinMax;
	cVector2f mvIdleTimeMinMax;

	//Vars
	eLuxCritterState mState;
	float mfFallingCount;
	float mfNewGroundNormalCount;
	bool mbOnGround;
	float mfMaxSpeed;
	float mfCount;
	float mfPlaySoundCount;
};

//----------------------------------------------

class cLuxPropLoader_Critter_Spider : public iLuxPropLoader_Critter
{
public:
	cLuxPropLoader_Critter_Spider(const tString& asName);
	virtual ~cLuxPropLoader_Critter_Spider(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_CRITTER_SPIDER_H
