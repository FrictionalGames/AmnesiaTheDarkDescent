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

#ifndef LUX_ENEMY_H
#define LUX_ENEMY_H

//----------------------------------------------

#include "LuxEntity.h"
#include "LuxSavedEngineTypes.h"

//----------------------------------------------

class cLuxEnemyPathfinder;
class cLuxEnemyMover;
class cLuxProp_Object;
	  
//----------------------------------------------

class cLuxEnemyPatrolNode
{
public:
    cAINode *mpNode;
	float mfWaitTime;
	tString msAnimation;
	bool mbLoopAnimation;
};


//----------------------------------------------

class cLuxStateMessage_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxStateMessage_SaveData)
public:
	int mlType;
	float mfTime;
	float mfCount;

	cVector3f mvCustomValue;
	float mfCustomValue;
	int mlCustomValue;

	int mlState;
};

//----------------------------------------------

class cLuxEnemyMover_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxEnemyMover_SaveData)
public:
	void FromMover(cLuxEnemyMover *apMover);
	void ToMover(cLuxEnemyMover *apMover);

	bool mbTurning;
	float mfTurnGoalAngle;
	float mfTurnSpeed;
	float mfTurnBreakAcc;

	float mfStuckCounter;

	int mlMoveState;
	bool mbOverideMoveState;
};

//----------------------------------------------

class cLuxEnemyPathfinder_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxEnemyPathfinder_SaveData)
public:
	void FromPathfinder(cLuxEnemyPathfinder *apPathfinder);
	void ToPathfinder(cLuxEnemyPathfinder *apPathfinder);
	void SetupPathfinder(cLuxEnemyPathfinder *apPathfinder);
	
	bool mbMoving;
	cVector3f mvMoveGoalPos;
	
	cContainerVec<int> mvPathNodeIds;
};

//----------------------------------------------


class cLuxEnemyPatrolNode_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxEnemyPatrolNode_SaveData)
public:
	int mlNodeId;
	float mfWaitTime;
	tString msAnimation;
	bool mbLoopAnimation;
};

//----------------------------------------------


class iLuxEnemy_SaveData : public iLuxEntity_SaveData
{
	kSerializableClassInit(iLuxEnemy_SaveData)
public:
	virtual ~iLuxEnemy_SaveData();

	////////////////
	//Properties
	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;
	cVector3f mvOnLoadScale;

	bool mbDisabled;
	bool mbDisableTriggers;

	float mfHealth;
	bool mbCausesSanityDecrease;

	bool mbHallucination;
	float mfHallucinationEndDist;
	
	int mlCurrentState;
	int mlNextState;
	int mlPreviousState;

	int mlSoundState;

	float mfLookForPlayerCount;
	int mlPlayerInLOSCount;
	bool mbCanSeePlayer;
	bool mbPlayerDetected;
	bool mbPlayerInRange;

	float mfCheckAtDoorCount;
	bool mbStuckAtDoor;
	int mlStuckDoorID;

	float mfForwardSpeed;
	float mfBackwardSpeed;
	float mfForwardAcc;
	float mfForwardDeacc;

	float mfDarknessGlowAlpha;
	float mfDarknessGlowAlphaGoal;

	int mlAttackHitCounter;
	
	float mfFOVMul;
	
	tString msCurrentAnimName;
	bool mbAnimationIsSpeedDependant;
	float mfAnimationSpeedMul;
	bool mbUseMoveAnimWhenCurrentIsOver;

	cVector3f mvStartPosition;

	cVector3f mvLastKnownPlayerPos;
	float mfLastPlayerPosCount;

	cVector3f mvTempPos;
	float mfTempVal;
	float mlTempVal;
	float mfDamageCount;

	cContainerVec<cLuxEnemyPatrolNode_SaveData> mvPatrolNodes;
	int mlCurrentPatrolNode;

	////////////////
	//Data
	cEngineMeshEntity_SaveData mMeshEntity;

	cContainerVec<cEnginePS_SaveData> mvPS;
	cContainerVec<cEngineBeam_SaveData> mvBeams;
	cContainerVec<cEngineLight_SaveData> mvLights;
	cContainerVec<cEngineBillboard_SaveData> mvBillboards;

	cContainerVec<cLuxStateMessage_SaveData> mvMessages;

	cLuxEnemyMover_SaveData mMover;
	cLuxEnemyPathfinder_SaveData mPathfinder;

	cEngineCharacterBody_SaveData mCharBody;
	
	cEnginePS_SaveData* GetParticleSystem(cParticleSystem* apPS);

	iLuxEntity* CreateEntity(cLuxMap *apMap);

};

//----------------------------------------------

enum eLuxEnemyState
{
	eLuxEnemyState_Idle,
	eLuxEnemyState_GoHome,

	eLuxEnemyState_Wait,
	eLuxEnemyState_Patrol,
	eLuxEnemyState_Investigate,
	eLuxEnemyState_Alert,
	eLuxEnemyState_Search,
	eLuxEnemyState_Eat,

	eLuxEnemyState_Hurt,
	
	eLuxEnemyState_Hunt,
	eLuxEnemyState_HuntPause,
	eLuxEnemyState_HuntWander,

	eLuxEnemyState_AttackMeleeShort,
	eLuxEnemyState_AttackMeleeLong,
	eLuxEnemyState_AttackRange,
	eLuxEnemyState_BreakDoor,

	eLuxEnemyState_Dead,

	eLuxEnemyState_PigEnumStart,

	eLuxEnemyState_Flee,
	eLuxEnemyState_Stalk,
	eLuxEnemyState_Track,

	eLuxEnemyState_LastEnum
};

//----------------------------------------------

enum eLuxEnemyStateEvent
{
	eLuxEnemyStateEvent_Enter,
	eLuxEnemyStateEvent_Leave,
	eLuxEnemyStateEvent_Update,
	eLuxEnemyStateEvent_Message,

	eLuxEnemyStateEvent_LastEnum
};

//----------------------------------------------

enum eLuxEnemyMoveSpeed
{
	eLuxEnemyMoveSpeed_Walk,
	eLuxEnemyMoveSpeed_Run,

	eLuxEnemyMoveSpeed_LastEnum
};

//----------------------------------------------

enum eLuxEnemyPoseType
{
	eLuxEnemyPoseType_Biped = 0,
	eLuxEnemyPoseType_Quadruped,
	eLuxEnemyPoseType_LastEnum
};

//----------------------------------------------

enum eLuxEnemyMoveType
{
	eLuxEnemyMoveType_Normal,
	eLuxEnemyMoveType_Flee,
	eLuxEnemyMoveType_LastEnum
};


//----------------------------------------------

enum eLuxEnemySoundState
{
	eLuxEnemySoundState_Silent,
	eLuxEnemySoundState_Idle,
	eLuxEnemySoundState_Alert,
	eLuxEnemySoundState_Hunt,
	
	eLuxEnemySoundState_LastEnum
};


//----------------------------------------------

class cLuxStateMessage
{
public:
	  eLuxEnemyMessage mType;
	  float mfTime;
	  float mfCount;

	  cVector3f mvCustomValue;
	  float mfCustomValue;
	  int mlCustomValue;

	  eLuxEnemyState mState;
};

typedef std::list<cLuxStateMessage>		tLuxStateMessageList;
typedef tLuxStateMessageList::iterator	tLuxStateMessageListIt;

//----------------------------------------------

#define kLuxBeginStateMachine		if(alState < 0){ if(0){
#define kLuxEndStateMachine			return true;}}else{FatalError("Tried calling undefined state %d!\n", alState ); \
									return false;} return false;
#define kLuxState(x)				return true;}} else if(x == alState){if(0){
#define kLuxOnMessage(x)			return true;} else if(aEvent == eLuxEnemyStateEvent_Message && apMessage && apMessage->mType == x){
#define kLuxOnEvent(x)				return true;} else if(x == aEvent){
#define kLuxOnEnter					kLuxOnEvent(eLuxEnemyStateEvent_Enter)
#define kLuxOnUpdate				kLuxOnEvent(eLuxEnemyStateEvent_Update)
#define kLuxOnLeave					kLuxOnEvent(eLuxEnemyStateEvent_Leave)


//----------------------------------------------

class cEnemyAttackDamageData
{
public:
	cEnemyAttackDamageData() :	mfMinDamage(0),mfMaxDamage(0), mfForce(0),mfMaxImpulse(0),mlStrength(0), mfHitSpeed(3), 
								mDamageType(eLuxDamageType_BloodSplat),mWeaponHitType(eLuxWeaponHitType_Sword),
								mbCheckPlayer(true), mbCheckProps(true), msHitSound("") {}
	
	float mfMinDamage;
	float mfMaxDamage;
	float mfForce;
	float mfMaxImpulse;
	int mlStrength;
	float mfHitSpeed;
	eLuxDamageType mDamageType;
	eLuxWeaponHitType mWeaponHitType;
	bool mbCheckPlayer;
	bool mbCheckProps;
	tString msHitSound;
};

//----------------------------------------------

class cEnemyAttackSizeData
{
public:
	cEnemyAttackSizeData() : mlShapeIdx(-1), mvOffset(0) {}
	
	int mlShapeIdx;
	cVector3f mvOffset;
};

//----------------------------------------------

class iLuxEnemy : public iLuxEntity
{
typedef iLuxEntity super_class;
friend class iLuxEnemyLoader;
friend class cLuxEnemy_WorldCollisionCallback;
friend class cLuxEnemyMover;
friend class cLuxEnemyPathfinder;
public:	
	iLuxEnemy(const tString &asName, int alID, cLuxMap *apMap, eLuxEnemyType aEnemyType);
	virtual ~iLuxEnemy();

	//////////////////////
	//General
	void SetupAfterLoad(cWorld *apWorld);
	void AfterWorldLoad();
	void OnMapEnter();
	
	void OnUpdate(float afTimeStep);

	void OnRenderSolid(cRendererCallbackFunctions* apFunctions);

	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState);

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	iEntity3D* GetAttachEntity();

	//////////////////////
	//Actions
	void GiveDamage(float afAmount, int alStrength);
	void ShowPlayerPosition();
	void AlertOfPlayerPresence();

	void ChangeState(eLuxEnemyState aState);

	void SendMessage(eLuxEnemyMessage aType, float afTime=0, bool abLocalScope=false, const cVector3f& avX=0,float afX=0, int alX=0);

	void PlayAnim(	const tString &asName, bool abLoop, float afFadeTime, 
					bool abDependsOnSpeed=false, float afSpeedMul=1.0f,
					bool abSyncWithPrevFrame=false,
					bool abOverideMoveState=true,
					bool abUseMoveAnimWhenCurrentIsOver=true);
	void FadeOutCurrentAnim(float afFadeTime);
	float ConvertAnimToAbsoluteTime(float afRelativeTimePostion);
    
	cSoundEntity* PlaySound(const tString &asName);

	void SetPositionAtStartPos();

	void ResetProperties();

	void FadeToSmoke(bool abPlaySound);

	//////////////////////
	// Movement animation names

	virtual const tString & GetBackwardAnimationName() { return msBackwardAnimationName[eLuxEnemyMoveType_Normal][mCurrentPose]; }
	virtual const tString & GetIdleAnimationName() { return msIdleAnimationName[eLuxEnemyMoveType_Normal][mCurrentPose]; }
	virtual const tString & GetWalkAnimationName() { return msWalkAnimationName[eLuxEnemyMoveType_Normal][mCurrentPose]; }
	virtual const tString & GetRunAnimationName() { return msRunAnimationName[eLuxEnemyMoveType_Normal][mCurrentPose]; }

	//////////////////////
	//Patrol nodes
	void AddPatrolNode(cAINode *apNode, float afWaitTime, const tString & asAnimation, bool abLoopAnimation=false);
	void ClearPatrolNodes();

    cLuxEnemyPatrolNode* GetCurrentPatrolNode();
	bool IsAtLastPatrolNode();
	void IncCurrentPatrolNode(bool abLoopIfAtEnd);

	cLuxEnemyPatrolNode* GetPatrolNode(int alIdx){ return &mvPatrolNodes[alIdx];}
	int GetPatrolNodeNum(){ return (int)mvPatrolNodes.size();}

	//////////////////////
	//Water
	bool IsInWater(){ return mbInWater;}
	float GetWaterSurfaceY(){ return mfWaterSurfaceY;}
	cSurfaceData* GetWaterSurfaceData(){ return mpWaterSurfaceData;}
	
	void SetInWater(bool abX){ mbInWater = abX;}
	void SetWaterSurfaceY(float afX){ mfWaterSurfaceY = afX;}
	void SetWaterSurfaceData(cSurfaceData* apData){ mpWaterSurfaceData = apData;}

	//////////////////////
	//Properties
	eLuxEnemyType GetEnemyType(){ return mEnemyType;}

	void SetDisabled(bool abX);
	bool IsDisabled(){ return mbDisabled;}

	void SetDisableTriggers(bool abX);
	bool GetDisableTriggers(){ return mbDisableTriggers;}

	eLuxEnemyState GetCurrentEnemyState(){ return mCurrentState;}
	string& GetCurrentEnemyStateName();

	bool CanSeePlayer(){ return mbCanSeePlayer;}
	bool GetPlayerDetected(){ return mbPlayerDetected;}
	bool GetPlayerInRange(){ return mbPlayerInRange;}

	float GetHealth(){ return mfHealth; }

	cAnimationState* GetCurrentAnimation(){ return mpCurrentAnimation;}

	const tString& GetDangerMusic() { return msDangerMusic; }
	int GetDangerMusicPrio() { return mlDangerMusicPrio; }

	const tString& GetMusic(eLuxEnemyMusic aType) { return msMusic[aType]; }
	int GetMusicPrio(eLuxEnemyMusic aType) { return mlMusicPrio[aType]; }

	float GetActivationDistance() { return mfActivationDistance;}

	const tString& GetHitSound(eLuxWeaponHitType aType){ return msHitSound[aType];}	
	const tString& GetHitPS(eLuxWeaponHitType aType){ return msHitPS[aType];}

	int GetBodyNum(){ return 1; }
	iPhysicsBody* GetBody(int alIdx){ return mpCharBody->GetCurrentBody();}

	void SetIsSeenByPlayer(bool abX){ mbIsSeenByPlayer = abX;}
	bool GetIsSeenByPlayer(){ return mbIsSeenByPlayer;}

	bool CausesSanityDecrease(){ return mbCausesSanityDecrease;}
	void SetSanityDecreaseActive(bool abX){ mbCausesSanityDecrease = abX;}

	void SetHallucination(bool abX){ mbHallucination = abX;}
	void SetHallucinationEndDist(float afX){ mfHallucinationEndDist = afX;}

	float GetInLanternLightCount(){ return mfInLanternLightCount;}

	//////////////////////
	//Callbacks
	virtual bool InRangeOfFood(iPhysicsBody *apFoodBody);
	
	//////////////////////
	//Data
	cMeshEntity * GetMeshEntity(){ return mpMeshEntity;}
	iCharacterBody * GetCharacterBody() { return mpCharBody; }

	iCollideShape* GetAttackShape(int alIdx){ return mvAttackShapes[alIdx];}

	cLuxEnemyPathfinder* GetPathFinder(){ return mpPathfinder;}
	cLuxEnemyMover* GetMover(){ return mpMover;}
	
	//////////////////////
	// Debug
	float DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY);

	//////////////////////
	//Save data stuff
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

protected:
    //////////////////////////////
	// Update and related

	tString GetCurrentPoseSuffix();

	bool StateEvent(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage);
	virtual bool StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)=0;

	void UpdateStateMachine(float afTimeStep);
	void CheckStateChange();

	void ChangeSoundState(eLuxEnemySoundState aState);

	void UpdateSoundState(float afTimeStep);
	void UpdateAnimation(float afTimeStep);
	void UpdateCharBody(float afTimeStep);
	void UpdateCanSeePlayer(float afTimeStep);
	void UpdatePlayerDetected(float afTimeStep);
	void UpdatePlayerInRange(float afTimeStep);
	void UpdateCheckStuckAtDoor(float afTimeStep);
	void UpdateCheckLastPlayerPos(float afTimeStep);
	void UpdateDarknessGlow(float afTimeStep);
	void UpdateRegenHealth(float afTimeStep);
	void UpdateHallucination(float afTimeStep);
	void UpdateAlignEntityWithGroundRay(float afTimeStep);

	//////////////////////////////
	// Callbacks
	virtual bool PlayerIsDetected()=0;
	virtual void OnSetActiveEnemySpecific(bool abX){}
	virtual void OnResetProperties(){}
	virtual void OnDisableTriggers(){}

	/////////////////////////////////////
	// Helpers
	bool TriggersDisabled();

	bool Attack(const cEnemyAttackSizeData &aSizeData, const cEnemyAttackDamageData &aDamageData, float afDamageMul=1.0f);

	void SetMoveSpeed(eLuxEnemyMoveSpeed aType);

	cAINode *GetSearchForPlayerNode(int alMaxIterations=5, float afMaxAngleMul=1.0f, float afMinDistMul=1.0f, float afMaxDistMul=1.0f);
	cAINode *GetPatrolAroundPlayerNode(int alMaxIterations=5, float afMaxAngleMul=1.0f, float afMinDistMul=1.0f, float afMaxDistMul=1.0f);

	bool LineOfSight(const cVector3f &avPos, const cVector3f &avSize, bool abCheckFOV);
	bool LineOfSight(const cVector3f &avPos, const cVector3f &avSize, bool abCheckFOV, const cVector3f& avSourcePos);

	int CreateAttackShape(cWorld *apWorld, cVector3f &avSize, eCollideShapeType aType=eCollideShapeType_Box);

	cMatrixf GetDamageShapeMatrix(const cVector3f& avOffset);

	cLuxProp_Object* GetClosestFood(float afMaxDist, float afMaxHeightDist);

	bool IsSeenByPlayer();
	bool IsInPlayerFovAtFeetPos(const cVector3f& avFeetPos);
	bool IsVisibleToPlayerAtFeetPos(const cVector3f& avFeetPos);
	
	cVector3f GetPlayerFeetPos();

	float Dist2D(const cVector3f &avPos);
	float DistToChar(iCharacterBody *apBody);
	float DistToChar2D(iCharacterBody *apBody);
	float AbsHeightDistToChar(iCharacterBody *apBody);
	cVector3f GetDirection2D(const cVector3f &avPos);
    
	float DistToPlayer();
	float DistToPlayer2D();
	float DistToPlayer2D(const cVector3f& avPos);
	float AbsHeightDistToPlayer();
	cVector3f GetDirection2DToPlayer();
	
	//gets the cos of angle between player->enemy and player move dir. 1=directly towards, -1=directly away
	float GetPlayerMovementTowardEnemyAmount();
	
	bool OutsideStartRadius();
	bool InFOV(const cVector3f &avPos);
	bool PlayerInFOV();

	void OnSetActive(bool abX);
	
	
	virtual float GetDamageMul(float afAmount, int alStrength)=0;
	virtual void OnDamage(float afAmount, int alStrength){}
	virtual void OnSetupAfterLoad(cWorld *apWorld)=0;
	virtual void OnAfterWorldLoad()=0;
	virtual void UpdateEnemySpecific(float afTimeStep)=0;
	virtual void OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)=0;

	//////////////
	//Variables
	bool mbDisabled;

	bool mbDisableTriggers;

	bool mbHallucination;
	float mfHallucinationEndDist;

	float mfHealth;
	bool mbCausesSanityDecrease;
	bool mbCausesSanityDecreaseAsDefault;

	eLuxEnemyState mCurrentState;
	eLuxEnemyState mNextState;
	eLuxEnemyState mPreviousState;

	eLuxEnemySoundState mSoundState;

	cSoundEntity *mpCurrentSound;
	int mlCurrentSoundID;
	float mfAmbientSoundCount;

	float mfLookForPlayerCount;
	int mlPlayerInLOSCount;
	bool mbCanSeePlayer;
	bool mbPlayerDetected;
	bool mbPlayerInRange;

	float mfCheckAtDoorCount;
	bool mbStuckAtDoor;
	int mlStuckDoorID;

	float mfDarknessGlowAlpha;
	float mfDarknessGlowAlphaGoal;
	float mfDarknessGlowUpdateCount;

	float mfForwardSpeed;
	float mfBackwardSpeed;
	float mfForwardAcc;
	float mfForwardDeacc;

	float mfFOVMul;

	float mfInLanternLightCount;

	cAnimationState *mpCurrentAnimation;
	bool mbAnimationIsSpeedDependant;
	float mfAnimationSpeedMul;
	bool mbUseMoveAnimWhenCurrentIsOver;

	tLuxStateMessageList mlstMessages;

	cVector3f mvStartPosition;

	cAINode *mpLastSearchNode;

	cVector3f mvLastKnownPlayerPos;
	float mfLastPlayerPosCount;

	int mlAttackHitCounter;

	cVector3f mvTempPos;
	float mfTempVal;
	float mlTempVal;
	float mfDamageCount;

	std::vector<cLuxEnemyPatrolNode> mvPatrolNodes;
	int mlCurrentPatrolNode;

	bool mbIsSeenByPlayer;

	float mfCheckGroundRayCount;
	tFloatList mlstGroundCheckDists;
	int mlMaxGroundCheckDists;

	bool mbInWater;
	float mfWaterSurfaceY;
	cSurfaceData* mpWaterSurfaceData;

	eLuxEnemyPoseType mCurrentPose;
	
	//////////////
	//Data
	cWorld *mpWorld;

	bool mbUseAnimations;

	int mlToughness;

	bool mbAlignEntityWithGroundRay;

	bool mbAutoRemoveAtPathEnd;

	float mfSightRange;
	float mfDarknessSightRange;
	float mfHearVolume;
    float mfFOV;
	float mfFOVXMul;
	float mfActivationDistance;
	bool mbSkipVisibilityRangeHandicaps;

	float mfMaxRegenHealth;
	float mfRegenHealthSpeed;

	float mfEnemyDarknessGlowMaxDistance;

	float mfPlayerInDarknessLightLevel;
	float mfCrouchVisibleRangeMul;

	tString msDangerMusic;
	int mlDangerMusicPrio;

	tString msMusic[eLuxEnemyMusic_LastEnum];
	int mlMusicPrio[eLuxEnemyMusic_LastEnum];

	float mfPlayerSearchMaxAngle;
	float mfPlayerSearchMinDistMul;
	float mfPlayerSearchMaxDistMul;
	float mfPlayerSearchTime;

	float mfPlayerPatrolMaxAngle;
	float mfPlayerPatrolMinDist;
	float mfPlayerPatrolMaxDist;

	float mfTurnSpeedMul;
	float mfTurnMaxSpeed;
	float mfTurnMinBreakAngle;
	float mfTurnBreakMul;

	float mfMoveSpeedAnimMul;

	float mfStoppedToWalkSpeed[eLuxEnemyPoseType_LastEnum];
	float mfWalkToStoppedSpeed[eLuxEnemyPoseType_LastEnum];
	float mfWalkToRunSpeed[eLuxEnemyPoseType_LastEnum];
	float mfRunToWalkSpeed[eLuxEnemyPoseType_LastEnum];

	float mfDefaultForwardSpeed[eLuxEnemyPoseType_LastEnum][eLuxEnemyMoveSpeed_LastEnum];
	float mfDefaultBackwardSpeed[eLuxEnemyPoseType_LastEnum][eLuxEnemyMoveSpeed_LastEnum];
	float mfDefaultForwardAcc[eLuxEnemyPoseType_LastEnum][eLuxEnemyMoveSpeed_LastEnum];
	float mfDefaultForwardDeacc[eLuxEnemyPoseType_LastEnum][eLuxEnemyMoveSpeed_LastEnum];

	float mfWaterStepSpeedWalk;
	float mfWaterStepSpeedRun;
	float mfWaterStepSpeedMisc;

	tString msHitPS[eLuxWeaponHitType_LastEnum];
	tString msHitSound[eLuxWeaponHitType_LastEnum];

	tString msAmbientSound[eLuxEnemySoundState_LastEnum];
	float mfAmbientSoundMinTime[eLuxEnemySoundState_LastEnum];
	float mfAmbientSoundMaxTime[eLuxEnemySoundState_LastEnum];

	float mfNormalAttackDistance;
	cEnemyAttackDamageData mNormalAttackDamage;
	cEnemyAttackDamageData mBreakDoorAttackDamage;
	cEnemyAttackSizeData mNormalAttackSize;
	
	iCharacterBody *mpCharBody;

	cMeshEntity *mpMeshEntity;

	std::vector<iLight*> mvLights;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cBeam*> mvBeams;
	
	cLuxEnemyPathfinder *mpPathfinder;
	cLuxEnemyMover *mpMover;

	std::vector<iCollideShape*> mvAttackShapes;

	cMatrixf m_mtxCharMeshOffset;

	string msPolledEnemyStateName;

	tString msIdleAnimationName[eLuxEnemyMoveType_LastEnum][eLuxEnemyPoseType_LastEnum];
	tString msWalkAnimationName[eLuxEnemyMoveType_LastEnum][eLuxEnemyPoseType_LastEnum];
	tString msRunAnimationName[eLuxEnemyMoveType_LastEnum][eLuxEnemyPoseType_LastEnum];
	tString msBackwardAnimationName[eLuxEnemyMoveType_LastEnum][eLuxEnemyPoseType_LastEnum];

	//////////////
	//Save specific
	tString msFileName;
	cMatrixf m_mtxOnLoadTransform;
	cVector3f mvOnLoadScale;
	
private:
	eLuxEnemyType mEnemyType;
};

//----------------------------------------------

class iLuxEnemyLoader : public cEntityLoader_Object
{
public:
	iLuxEnemyLoader(const tString& asName);
	virtual ~iLuxEnemyLoader(){}

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	
	virtual iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)=0;
	virtual void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)=0;
	virtual void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)=0;

protected:
	eLuxDamageType ToDamageType(const tString& asType);
	void LoadAttackDamageData(const tString &asPrefix, cEnemyAttackDamageData *apData);
	void LoadAttackSizeData(const tString &asPrefix, cEnemyAttackSizeData *apData, iLuxEnemy *apEnemy, cWorld *apWorld);

	float mfDefaultMaxFocusDistance;
};

//----------------------------------------------


#endif // LUX_Enemy_H
