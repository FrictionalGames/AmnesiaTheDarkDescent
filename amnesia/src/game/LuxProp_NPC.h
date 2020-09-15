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

#ifndef LUX_PROP_NPC_H
#define LUX_PROP_NPC_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_NPC_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_NPC_SaveData)
public:
	tString msFollowPlayerArea;
	float mfHeadAngle;
	bool mbPlayerIsInArea;
	bool mbAwake;
	bool mbPlayingWakeAnim;
	bool mbFollowPlayer;
};

//----------------------------------------------

class cLuxProp_NPC_HeadMoveBone
{
public:
	int mlBoneIdx;
    float mfMul;
	cVector3f mvRotVec;
};

//----------------------------------------------

class cLuxProp_NPC : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_NPC;
public:	
	cLuxProp_NPC(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_NPC();

	//////////////////////
	//General
	void AfterWorldLoad();

	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Settings
	void SetAwake(bool abX, bool abEffects);
	void SetFollowPlayer(bool abX){ mbFollowPlayer = abX; }
	
	//////////////////////
	//Debug
	float DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY);

	
	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState);


	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
	void UpdateWakeState(float afTimeStep);
	void UpdateCheckPlayerIsInArea(float afTimeStep);
	void UpdateHeadMovement(float afTimeStep);
	
	//////////////////////
	// Data
	std::vector<cLuxProp_NPC_HeadMoveBone> mvHeadMoveBones;

	float mfHeadMoveSpeedMul;
	float mfHeadMoveMaxSpeed;
	float mfMaxHeadAngle;
    	
	//////////////////////
	// Variables
	tString msFollowPlayerArea;
	iLuxEntity *mpFollowPlayerArea;

	bool mbPlayerIsInArea;
	float mfInAreaCount;

	float mfHeadAngle;

	bool mbAwake;
	bool mbPlayingWakeAnim;
	bool mbFollowPlayer;
};

//----------------------------------------------

class cLuxPropLoader_NPC : public iLuxPropLoader
{
public:
	cLuxPropLoader_NPC(const tString& asName);
	virtual ~cLuxPropLoader_NPC(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_OIL_BARELL_H
