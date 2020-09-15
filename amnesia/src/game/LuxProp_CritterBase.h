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

#ifndef LUX_PROP_CRITTER_BASE_H
#define LUX_PROP_CRITTER_BASE_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class iLuxProp_CritterBase;
class cLuxCritterRayCallback : public iPhysicsRayCallback
{
public:
	cLuxCritterRayCallback(iLuxProp_CritterBase *apCritterBase) : mpCritterBase(apCritterBase) {}

	void Reset();

	bool BeforeIntersect(iPhysicsBody *pBody);
	bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

	bool GetIntersected(){ return mbIntersected;}
	const cVector3f& GetNormal(){ return mvNormal; }
	const cVector3f& GetPos(){ return mvPos; }
	iPhysicsBody * GetBody(){ return mpBody;}
	float GetT(){ return mfClosestT; }
private:
	float mfClosestT;
	bool mbIntersected;
	cVector3f mvNormal;
	cVector3f mvPos;
	iPhysicsBody *mpBody;

	iLuxProp_CritterBase *mpCritterBase;
};

//----------------------------------------------

class iLuxProp_CritterBase_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(iLuxProp_CritterBase_SaveData)
public:
	cVector3f mvVel;
	cVector3f mvGravityVel;
	cVector3f mvSwarmPoint;

	int mlAnimState;
	cVector3f mvGroundNormal;
	bool mbColliding;
	bool mbUpdateAnimation;
};

//----------------------------------------------

enum eLuxCritterState
{
	eLuxCritterState_Idle,
	eLuxCritterState_Move,
	eLuxCritterState_Flee,
	eLuxCritterState_Hunt,
	eLuxCritterState_Hit,
	eLuxCritterState_Attack_1,
	eLuxCritterState_Attack_2,
	eLuxCritterState_LastEnum,
};

//----------------------------------------------

class iLuxProp_CritterBase : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxCritterRayCallback;
friend class iLuxPropLoader_Critter;
public:	
	iLuxProp_CritterBase(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~iLuxProp_CritterBase();

	//////////////////////
	//General
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	void OnRenderSolid(cRendererCallbackFunctions* apFunctions);

	void OnHealthChange();
	void OnDamage(float afAmount, int alStrength);

	bool CausesSanityDecrease(){ return mbCausesSanityDecrease;}

	//////////////////////
	//Properties
	
	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}
	
	//////////////////////
	//Save data stuff
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);


protected:
	virtual void OnDamageCritter(float afAmount){}
	virtual void OnKillCritter(){}

	virtual void UpdateCritterSpecific(float afTimeStep)=0;
	virtual void OnShapeCollision(const cVector3f& avPushVec, float afTimeStep){}

	void CreateOrthoVectors(cVector3f &avRight, cVector3f& avUp, cVector3f &avForward);
	float GetDistanceToPlayer();
	float GetDistanceToPlayer2D();

	cMatrixf GetAttackMatrix(const cVector3f& avDir);
	bool Attack(const cVector3f& avDir);
    
	cVector3f GetWanderAdd(float afLength, float afRadius, float afTimeStep);
	cVector3f GetTowardPlayerAdd(bool abDependOnDistance, float afTimeStep);

	void UpdateMesh(float afTimeStep);
	void UpdateMovement(float afTimeStep);

	void CheckRayCollision(const cVector3f& avVelAdd,const cVector3f& avGravityAdd, float afTimeStep);
	void CheckShapeCollision(const cVector3f& avVelAdd,const cVector3f& avGravityAdd, float afTimeStep);

	//Data
	bool mbUseRayCollision;

	tString msDeathEntity;
	tString msDeathPS;
	tString msDeathSound;

	float mfAttackSizeRadius;
	cVector3f mvAttackOffset;
	cVector2f mvAttackDamageMinMax;
	cVector2f mvAttackForce;
	int mlAttackStrength;
	tString msAttackHitSound;

	cMatrixf m_mtxMeshOffset;
	
	iCollideShape *mpDamageShape;

	bool mbCausesSanityDecrease;

	//Vars
	int mlAnimState;
	cVector3f mvVel;
	cVector3f mvGravityVel;
	cVector3f mvGroundNormal;
	bool mbColliding;
	bool mbUpdateAnimation;
	
	
	cVector3f mvSwarmPoint;
	
	std::vector<cMatrixf> mvBaseMatrices;
	tVector3fList mlstFwdDirs;
	tVector3fList mlstUpDirs;
	
	iPhysicsBody *mpBody;

	cLuxCritterRayCallback *mpRayCallback;
};

//----------------------------------------------

class iLuxPropLoader_Critter : public iLuxPropLoader
{
public:
	iLuxPropLoader_Critter(const tString asName);

	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

	virtual void LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem)=0;
	virtual void LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)=0;
};

//----------------------------------------------


#endif // LUX_PROP_CRITTER_BASE_H
