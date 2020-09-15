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

#ifndef LUX_AREA_ROPE_H
#define LUX_AREA_ROPE_H

//----------------------------------------------

#include "LuxEntity.h"

//----------------------------------------------

class cLuxRope_SaveData : public iLuxEntity_SaveData
{
	kSerializableClassInit(cLuxRope_SaveData)
public:
	////////////////
	//Variables
	bool mbRopeCreated;

	////////////////
	//Properties
	cVector3f mvStartPos;
	cVector3f mvEndPos;

	tString msEndPosNode; 
	tString msStartBody;
	tString msEndBody;

	float mfMinTotalLength;
	float mfMaxTotalLength;
	float mfSegmentLength;
	float mfDamping;
	float mfStrength;
	float mfStiffness;
	tString msMaterial;
	float mfRadius;
	float mfLengthTileAmount;
	float mfLengthTileSize;
	tString msSound;
	float mfSoundStartSpeed;
	float mfSoundStopSpeed;
	bool mbAutoMove;
	float mfAutoMoveAcc;
	float mfAutoMoveMaxSpeed;
	
	////////////////
	//Methods
	iLuxEntity* CreateEntity(cLuxMap *apMap);
};

//----------------------------------------------

class cLuxRope : public iLuxEntity
{
typedef iLuxEntity super_class;
friend class cLuxAreaRopeLoader;
public:
	cLuxRope(const tString& asName, int alID, cLuxMap *apMap);
	~cLuxRope();

	void AfterWorldLoad();

	void OnSetActive(bool abX);
	void OnUpdate(float afTimeStep);

	//////////////////////
	//Connection
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState);

	//////////////////////
	//Quick and dirt entity implementations
	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos){ return eLuxFocusCrosshair_Default; }
	iEntity3D* GetAttachEntity(){ return NULL;}
	void GiveDamage(float afAmount, int alStrength){}
	bool CanInteract(iPhysicsBody *apBody){ return false; }
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos){ return false;}
	int GetBodyNum(){ return 0;}
	iPhysicsBody* GetBody(int alIdx){ return NULL;}

	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);
private:
	//First tries to find a body by name, if failed, searches for entity.
	iPhysicsBody* GetBody(const tString& asName);

	//////////////////////////////
	// Variables
	bool mbRopeCreated;

	iPhysicsRope *mpRope;
	cRopeEntity *mpRopeGfx;

	//////////////////////////////
	// Properties
	cVector3f mvStartPos;
	cVector3f mvEndPos;

	tString msEndPosNode; 
	tString msStartBody;
	tString msEndBody;
	
	float mfMinTotalLength;
	float mfMaxTotalLength;
	float mfSegmentLength;
	float mfDamping;
	float mfStrength;
	float mfStiffness;
	tString msMaterial;
	float mfRadius;
	float mfLengthTileAmount;
	float mfLengthTileSize;
	tString msSound;
	float mfSoundStartSpeed;
	float mfSoundStopSpeed;
	bool mbAutoMove;
	float mfAutoMoveAcc;
	float mfAutoMoveMaxSpeed;
};

//----------------------------------------------

class cLuxAreaRopeLoader : public iAreaLoader
{
public:
	cLuxAreaRopeLoader(const tString& asName) : iAreaLoader(asName){}
	virtual ~cLuxAreaRopeLoader(){}

	void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld);
};

//----------------------------------------------


#endif// LUX_AREA_ROPE_H
