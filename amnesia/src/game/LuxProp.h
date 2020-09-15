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

#ifndef LUX_PROP_H
#define LUX_PROP_H

//----------------------------------------------

#include "LuxEntity.h"
#include "LuxSavedEngineTypes.h"


//----------------------------------------------

class iLuxInteractConnection;
class iLuxInteractConnection_SaveData;

//----------------------------------------------

class cLuxInteractData_Grab
{
public:
	bool mbGrabUseDepth;
	float mfGrabDepth;
	float mfGrabDepthInc;
	float mfGrabMinDepth;
	float mfGrabMaxDepth;
	bool mbGrabUseOffset;
	cVector3f mvGrabPositionOffset;
	cVector3f mvGrabRotationOffset;
	float mfGrabThrowImpulse;
	float mfGrabMassMul;
	float mfForceMul;
	float mfTorqueMul;
	bool mbUseRotation;
};

class cLuxInteractData_Push
{
public:
	bool mbPushAtPoint;
	float mfPushForceMul;
	float mfPushImpulse;
};

class cLuxInteractData_Slide
{
public:
	float mfSlideMaxSpeed;
	float mfSlideSlowDownFactor;
	float mfSlideSpeedFactor;
	float mfSlideThrowImpulse;
};

class iLuxInteractData_RotateBase
{
public:
	float mfMoveMaxSpeed;
	float mfMoveSlowDownFactor;
	float mfMoveSpeedFactor;
	float mfMoveThrowImpulse;
};

class cLuxInteractData_SwingDoor : public iLuxInteractData_RotateBase
{
public:
	
};

class cLuxInteractData_Lever : public iLuxInteractData_RotateBase
{
public:
};

class cLuxInteractData_Wheel : public iLuxInteractData_RotateBase
{
public:
	
};

//----------------------------------------------
class iLuxProp;

//----------------------------------------------

class cLuxPropConnectedProp  : public iSerializable
{
	kSerializableClassInit(cLuxPropConnectedProp)
public:
	tString msName;
};

//----------------------------------------------

class iLuxProp_SaveData : public iLuxEntity_SaveData
{
	kSerializableClassInit(iLuxProp_SaveData)
public:
	virtual ~iLuxProp_SaveData();

	////////////////
	//Properties
	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;
	cVector3f mvOnLoadScale;

	float mfHealth;

	bool mbCheckOutsidePlayer;

	bool mbEffectsActive;
	float mfEffectsAlpha;

	bool mbStaticPhysics;

	bool mbMoving;

	bool mbMovingLinear;
	float mfMoveLinearMaxSpeed;
	float mfMoveLinearAcc;
	float mfMoveLinearSpeed;
	float mfMoveLinearSlowdownDist;
	cVector3f mvMoveLinearGoal;

	bool mbMovingAngular;
	float mfMoveAngularMaxSpeed;
	float mfMoveAngularAcc;
	float mfMoveAngularSpeed;
	float mfMoveAngularSlowdownDist;
	cMatrixf m_mtxMoveAngularGoal;
	
	bool mbMoveAngularNoGoal;
	cVector3f mvMoveAngularNoGoalDir;
	
	bool mbMoveAngularUseOffset;
	cVector3f mvMoveAngularWorldOffset;
	cVector3f mvMoveAngularLocalOffset;

	int mlCurrentNonLoopAnimIndex;
	tString msAnimCallback;

	cContainerVec<cLuxProp_AttachedProp> mvAttachedProps;

	cContainerVec<cLuxPropConnectedProp> mvConnectedProps;

	////////////////
	//Data
	cEngineMeshEntity_SaveData mMeshEntity;

	cContainerVec<cEngineBody_SaveData> mvBodies;
	cContainerVec<cEngineJoint_SaveData> mvJoints;
	cContainerVec<cEnginePS_SaveData> mvPS;
	cContainerVec<cEngineBeam_SaveData> mvBeams;
	cContainerVec<cEngineLight_SaveData> mvLights;
	cContainerVec<cEngineSound_SaveData> mvSounds;
	cContainerVec<cEngineBillboard_SaveData> mvBillboards;

	cContainerVec<iLuxInteractConnection_SaveData*> mvInteractConnections;

	////////////////
	//Methods
	cEnginePS_SaveData* GetParticleSystem(cParticleSystem* apPS);
	cEngineSound_SaveData* GetSoundEntity(cSoundEntity* apSound);
	cEngineJoint_SaveData* GetJoint(iPhysicsJoint* apJoint);

	iLuxEntity* CreateEntity(cLuxMap *apMap);
};

//----------------------------------------------

class cLuxProp_LightData
{
public:
	cColor mOnColor;
	float mfOnRadius;
	bool mbFlickering;
};

class cLuxProp_BillboardData
{
public:
	cColor mOnColor;
	bool mbConnectedToLight;
};

class cLuxProp_PSData
{
public:
	iEntity3D *mpParent;
	tString msName;
	tString msDataName;
	cMatrixf m_mtxLocalTransform;
};

class cLuxProp_BodyData
{
public:
    float mfMass;
};

//----------------------------------------------

class LuxPropDefaultBodySettings
{
public:
	bool mbCollideCharacter;
};

//----------------------------------------------

class iLuxProp : public iLuxEntity
{
typedef iLuxEntity super_class;
friend class cLuxSavedGameEntity;
friend class iLuxPropLoader;
friend class cLuxProp_WorldCollisionCallback;
public:	
	iLuxProp(const tString &asName, int alID, cLuxMap *apMap, eLuxPropType aPropType);
	virtual ~iLuxProp();

	//////////////////////
	//General
	void SetupAfterLoad(cWorld *apWorld);

	void OnUpdate(float afTimeStep);

	virtual float OnInteractDebugDraw(cGuiSet *apSet,iFontData *apFont, float afStartY){return afStartY;}

	void BeforeEntityDestruction();

	virtual void OnRenderSolid(cRendererCallbackFunctions* apFunctions);
    
	//////////////////////
	//Actions
	void GiveDamage(float afAmount, int alStrength);

	void SetDisableCollisionUntilOutSidePlayer(bool abX);

	void MoveLinearTo(const cVector3f& avGoal, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed);
	void MoveAngularTo(	const cMatrixf& a_mtxGoal, float afAcc, float afMaxSpeed, float afSlowdownDist, bool abResetSpeed, bool abUseOffset,
						const cVector3f &avWorldOffset, const cVector3f &avLocalOffset);
	void RotateAtSpeed(	float afAcc, float afGoalSpeed, const cVector3f& avAxis, bool abResetSpeed,
						bool abUseOffset, const cVector3f &avWorldOffset, const cVector3f &avLocalOffset);
	void StopMove();
	
	void FadeInMeshEntity(float afTime);

	void ResetProperties();

	/**
	 * asCallback only used if loop is false! Callback syntax: MyFunc(string asProp)
	 */
	void PlayAnimation(const tString& asName, float afFadeTime, bool abLoop, const tString& asCallback);

	//////////////////////
	//Properties
	eLuxPropType GetPropType(){ return mPropType;}
	
	void SetHealth(float afX);
	float GetHealth(){ return mfHealth;}

	void SetEffectsActive(bool abActive, bool abFadeAndPlaySounds);

	float GetEffectsAlpha(){ return mfEffectsAlpha;}

	void SetIsInteractedWith(bool abX){  mbIsInteractedWith = abX;}
	bool IsInteractedWith(){  return mbIsInteractedWith;}

	bool IsMoving(){ return mbMoving; }
	
	virtual cLuxInteractData_Grab* GetGrabData(){ return NULL;}
	virtual cLuxInteractData_Push* GetPushData(){ return NULL;}
	virtual cLuxInteractData_Slide* GetSlideData(){ return NULL;}
	virtual iLuxInteractData_RotateBase* GetMoveBaseData(){ return NULL;}

	virtual iEntity3D* GetAttachEntity();

	virtual bool ShowOutlinesOnConnectedBodies(){ return true;}
	
	const cMatrixf& GetOnLoadTransform(){ return m_mtxOnLoadTransform; }

	bool GetStaticPhysics(bool abX){ return mbStaticPhysics;}
	void SetStaticPhysics(bool abX);

	//////////////////////
	//Attached Prop
	iLuxProp* GetAttachmentParent(){ return mpAttachmentParent;}
	void SetAttachmentParent(iLuxProp *apProp){mpAttachmentParent = apProp; }

	void AddAndAttachProp(const tString& asName, const tString& asFileName, const cMatrixf& a_mtxOffset);
	bool DestroyAttachedProp(const tString& asName);
	void DestroyAttachedProp(cLuxProp_AttachedProp *pProp);
	bool RemoveAttachedProp(iLuxProp *apProp);
	bool HasAttachedProp(){ return mlstAttachedProps.empty()==false; }
	

    //////////////////////
	//Connections
	int AddInteractConnection(iLuxInteractConnection *apConnection);
	iLuxInteractConnection* GetInteractionConnectionFromIndex(int alIndex);
	iLuxInteractConnection* GetInteractionConnectionFromName(const tString& asName);
	void DestroyInteractConnection(iLuxInteractConnection *apConnection);
	int GetInteractionConnectionNum(){ return (int)mvInteractConnections.size(); }

	void InteractConnectionLimit(int alState);
	void InteractConnectionTurn(float afAngle, float afPrevAngle, float afMinAngle, float afMaxAngle);

	//////////////////////
	//Data
	cMeshEntity * GetMeshEntity(){ return mpMeshEntity;}
	virtual cMeshEntity* GetEffectMeshEntity(){ return mpMeshEntity;}

	int GetBodyNum(){ return (int)mvBodies.size();}
	iPhysicsBody* GetBody(int alIdx){ return mvBodies[alIdx];}
	iPhysicsBody* GetBodyFromID(int alID);
	int GetBodyIndexFromName(const tString& asName);

	iPhysicsBody* GetMainBody(){ return mpMainBody ? mpMainBody : mvBodies[0];}
		
	//////////////////////
	//Save data stuff
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

protected:
	void OnSetActive(bool abX);

	virtual void OnHealthChange(){}
	virtual void OnDamage(float afAmount, int alStrength){}

	virtual void OnResetProperties()=0;

	virtual void OnSetupAfterLoad(cWorld *apWorld)=0;
	virtual void UpdatePropSpecific(float afTimeStep)=0;
	virtual void ImplementedOnSetActive(bool abX){}
	virtual void BeforePropDestruction(){}

	virtual void OnStartMove(){}

	void UpdateAttachedProps(float afTimeStep, bool abForceUpdate);

	void UpdateAnimation(float afTimeStep);

	void SetupEffectData();
	bool BillboardConnectedToLight(cBillboard *apBB);
	void UpdateEffectFading(float afTimeStep);
	
	void UpdateMoveSoundVolume();

	void UpdateMoving(float afTimeStep);
	void UpdateLinearMoving(float afTimeStep);
	void UpdateAngularMoving(float afTimeStep);
	
	void UpdateMeshFading(float afTimeStep);

	void CheckMoveCollision(cVector3f& avMoveVel, float &afSpeed, float afTimeStep);

	void UpdateCheckIfOutsidePlayer(float afTimeStep);

	cVector3f GetWorldCenterPos();

	//////////////
	//Variables
	float mfHealth;

	bool mbCheckOutsidePlayer;

	bool mbIsInteractedWith;

	bool mbEffectsActive;
	float mfEffectsAlpha;
	bool mbEffectAlphaFading;

    bool mbStaticPhysics;
	
	bool mbHasFlickering;

	bool mbMoving;
	
	float mfMovingVolume;
	float mfMoveStartCount;

	bool mbMovingLinear;
	float mfMoveLinearMaxSpeed;
	float mfMoveLinearAcc;
	float mfMoveLinearSpeed;
	float mfMoveLinearSlowdownDist;
	cVector3f mvMoveLinearGoal;

	bool mbMovingAngular;
	float mfMoveAngularMaxSpeed;
	float mfMoveAngularAcc;
	float mfMoveAngularSpeed;
	float mfMoveAngularSlowdownDist;
	cMatrixf m_mtxMoveAngularGoal;
	bool mbMoveAngularNoGoal;
	cVector3f mvMoveAngularNoGoalDir;

	bool mbMoveAngularUseOffset;
	cVector3f mvMoveAngularWorldOffset;
	cVector3f mvMoveAngularLocalOffset;

	cMatrixf m_mtxLastBodyMoveMatrix;

	int mlCurrentNonLoopAnimIndex;
	tString msAnimCallback;

	bool mbDissolveOnDestruction;
	float mfDissolveTime;

	float mfFadeInAlpha;
	float mfFadeInSpeed;

	iLuxProp *mpAttachmentParent;
	tLuxProp_AttachedPropList mlstAttachedProps;

	tStringVec mvConnectedProps;

	//////////////
	//Data
	int mlToughness;

	bool mbShowMesh;
	
	cWorld *mpWorld;
	std::vector<iPhysicsBody*> mvBodies;
	std::vector<iPhysicsJoint*> mvJoints;

	std::vector<cEntityBodyExtraData> mvBodyExtraData;

	std::vector<LuxPropDefaultBodySettings> mvDefaultBodySettings;

	std::vector<iLight*> mvLights;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cBeam*> mvBeams;
	std::vector<cSoundEntity*> mvSoundEntities;

	std::vector<iLuxInteractConnection*> mvInteractConnections;

	std::vector<cLuxProp_BodyData> mvBodyData;
	std::vector<cLuxProp_PSData> mvEffectPSData;
	std::vector<cLuxProp_BillboardData> mvEffectBillboardData;
	std::vector<cLuxProp_LightData> mvEffectLightData;

	cMeshEntity *mpMeshEntity;

	tString msMainBodyName;
	iPhysicsBody *mpMainBody;

	bool mbMoveCheckCollision;
	tString msMoveStartSound;
	tString msMoveStopSound;
	tString msMoveLoopSound;

	cSoundEntity* mpMoveLoopSound;
	int mlMoveLoopSoundID;

	tString msEffectsOnSound;
	tString msEffectsOffSound;
	float mfEffectsOnTime;
	float mfEffectsOffTime;
	cColor mEffectsOffLightColor;
	float mfEffectsOffLightRadius;

	bool mbShowHints;

	//////////////
	//Save specific
	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;
	cVector3f mvOnLoadScale;
	
private:
	eLuxPropType mPropType;
};

//----------------------------------------------

class iLuxPropLoader : public cEntityLoader_Object
{
public:
	iLuxPropLoader(const tString& asName);
	virtual ~iLuxPropLoader(){}

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	
	virtual iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap)=0;
	virtual void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)=0;
	virtual void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)=0;

protected:
	float mfDefaultMaxFocusDistance;
	bool mbForceFullGameSave;
};

//----------------------------------------------


#endif // LUX_PROP_H
