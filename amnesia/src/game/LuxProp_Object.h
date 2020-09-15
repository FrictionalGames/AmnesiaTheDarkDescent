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

#ifndef LUX_PROP_OBJECT_H
#define LUX_PROP_OBJECT_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Object_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_Object_SaveData)
public:
	float mfLifeLengthCount;
	tString msContainedItem;
	int mlStuckState;
	float mfFoodAttractCount;
	bool mbDisableBreakable;
	bool mbIsInsanityVision;
	float mfVisionMaxSanity;
	bool mbInsanityVisionActive;
};

//----------------------------------------------
class cLuxProp_Object;

class cLuxProp_Object_BodyCallback : public iPhysicsBodyCallback
{
public:
	cLuxProp_Object_BodyCallback(cLuxProp_Object *apObject);
	~cLuxProp_Object_BodyCallback();

	bool OnAABBCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody);
	void OnBodyCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody, cPhysicsContactData* apContactData);
	
private:
	cLuxProp_Object *mpObject;
};

//----------------------------------------------

class cLuxProp_Object_BreakData
{
public:
	bool mbActive;
	
	bool mbDestroyJoints;
	float mfMinEnergy;
	tString msEntity;
	tString msEntityAlignBody;
	tString msSound;
	tString msParticleSystem;
	float mfImpulse;
};

//----------------------------------------------

class cLuxProp_Object_JointData
{
public:
	float mfMaxLimit;
	float mfMinLimit;
	float mfLockedRange;
};

//----------------------------------------------

class cLuxProp_Object : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Object;
friend class cLuxProp_Object_BodyCallback;
public:	
	cLuxProp_Object(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_Object();

	//////////////////////
	//General
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Actions
	void SetStuckState(int alState);

	void Break();

	void OnHealthChange();
	void OnDamage(float afAmount, int alStrength);

	//////////////////////
	//Properties
	cLuxInteractData_Grab* GetGrabData(){ return &mGrabData;}
	cLuxInteractData_Push* GetPushData(){ return &mPushData;}
	cLuxInteractData_Slide* GetSlideData(){ return &mSlideData;}

	bool IsFood(){ return mbIsFood;}

	bool ShowOutlinesOnConnectedBodies();

	const tString& GetContainedItem(){ return msContainedItem; }
	void SetContainedItem(const tString& asItem){ msContainedItem = asItem; }

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
	void UpdateFoodEnemyAttraction(float afTimeStep);
	void UpdateInsanityVision(float afTimeStep);
	void SetInsanityVisionVisability(bool abX);

	void SetJointMinMax(int alIdx, float afMin, float afMax);
	

	//General
	eLuxObjectType mObjectType;

	//Vars
	bool mbBroken;
	float mfLifeLengthCount;
	tString msContainedItem;
	int mlStuckState;
	bool mbDisableBreakable;

	bool mbIsInsanityVision;
	float mfVisionMaxSanity;
	bool mbInsanityVisionActive;

	float mfFoodAttractCount;
	float mfInsanityVisionCount;
	
	//Data
	cLuxProp_Object_BodyCallback *mpBodyCallback;

	cLuxProp_Object_BreakData mBreakData;

	std::vector<cLuxProp_Object_JointData> mvJointData;

	float mfLifeLength;

	bool mbIsFood;

	float mfHitDamageAmount;
	int mlHitDamageStrength;
	float mfMinHitDamageSpeed;

	bool mbGrabSkipNonOuterBodies;

	cLuxInteractData_Grab mGrabData;
	cLuxInteractData_Push mPushData;	
	cLuxInteractData_Slide mSlideData;
};

//----------------------------------------------

class cLuxPropLoader_Object : public iLuxPropLoader
{
public:
	cLuxPropLoader_Object(const tString& asName);
	virtual ~cLuxPropLoader_Object(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
	eLuxObjectType GetObjectType(const tString&asName);

	float mfGrabDefaultMaxFocusDist;
	float mfPushDefaultMaxFocusDist;
	float mfSlideDefaultMaxFocusDist;
};

//----------------------------------------------


#endif // LUX_PROP_H
