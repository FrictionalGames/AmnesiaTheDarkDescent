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

#ifndef LUX_SAVED_ENGINE_TYPES_H
#define LUX_SAVED_ENGINE_TYPES_H

//---------------------------------------------------------------

#include "LuxBase.h"


//---------------------------------------------------------------

class cEngineCharacterBody_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineCharacterBody_SaveData)
public:
	void FromBody(iCharacterBody *apBody);
	void ToBody(iCharacterBody *apBody);

	bool mbActive;
	float mfMass;
	cVector3f mvPosition;
	float mfYaw;
	float mfPitch;
	float mfSpeedForward;
	float mfSpeedRight;
	cVector3f mvForceVelocity;
	int mlActiveSize;
};

//---------------------------------------------------------------

class cEngineVerletParticle_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineVerletParticle_SaveData)
public:
	void FromParticle(cVerletParticle *apPart);
	void ToParticle(cVerletParticle *apPart);
	
	cVector3f mvPosition;
	cVector3f mvPrevPosition;
	cVector3f mvSmoothPosition;
	float mfInvMass;
};

//---------------------------------------------------------------


class cEngineRope_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineRope_SaveData)
public:
	cRopeEntity* CreateRope(cLuxMap *apMap);

	void FromRope(cRopeEntity *apRope);
	void ToRope(cRopeEntity *apRope, cLuxMap *apMap);

	cContainerVec<cEngineVerletParticle_SaveData> mvParticles;

	tString msName;
	int mlUniqueGfxID;

	tString msPhysicsName;
	int mlUniquePhysicsID;

	cVector3f mvStartPos;
	cVector3f mvEndPos;

	cLuxIdPair mStartBody;
	cLuxIdPair mEndBody;
	cVector3f mvStartBodyLocalPos;
	cVector3f mvEndBodyLocalPos;

	int mlMaxIterations;
	float mfTotalLength;
	float mfMinTotalLength; 
	float mfMaxTotalLength;
	float mfSegmentLength;

	float mfDamping;
	float mfStrength;
	float mfStiffness;
	
	tString msMaterial;
	
	float mfParticleRadius;

	int mlMaxSegments;
	float mfRadius;
	float mfLengthTileAmount;
	float mfLengthTileSize;
	
	tString msSound;
	float mfSoundStartSpeed;
	float mfSoundStopSpeed;

	bool mbMotorActive;
	float mfMotorWantedLength;
	float mfMotorSpeedMul;
	float mfMotorMinSpeed;
	float mfMotorMaxSpeed;
    	
	bool mbAutoMove;
	float mfAutoMoveSpeed;
	float mfAutoMoveAcc;
	float mfAutoMoveMaxSpeed;
};

//---------------------------------------------------------------

class cEngineBillboard_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineBillboard_SaveData)
public:
	void FromBillboard(cBillboard *apBillboard);
	void ToBillboard(cBillboard *apBillboard);

	cColor mColor;
};

//---------------------------------------------------------------

class cEngineAnimationState_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineAnimationState_SaveData)
public:
	void FromAnim(cAnimationState *apAnim);
	void ToAnim(cAnimationState *apAnim);
	
	bool mbActive;
	bool mbLoop;

	float mfWeight;
	float mfFadeStep;
	float mfTimePos;
	float mfSpeed;
};

//---------------------------------------------------------------

class cEngineSubMeshEntity_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineSubMeshEntity_SaveData)
public:
	void FromSubMeshEntity(cSubMeshEntity *apSubMeshEntity);
	void ToSubMeshEntity(cSubMeshEntity *apSubMeshEntity);

	bool mbActive;
	bool mbVisible;
	float mfIlluminationAmount;

	cMatrixf m_mtxTransform;
};

class cEngineMeshEntity_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineMeshEntity_SaveData)
public:
	void FromMeshEntity(cMeshEntity *apMeshEntity);
	void ToMeshEntity(cMeshEntity *apMeshEntity);

	bool mbActive;
	bool mbVisible;
	float mfIlluminationAmount;
	cMatrixf m_mtxTransform;
	cContainerVec<cEngineAnimationState_SaveData> mvAnimations;
	cContainerVec<cEngineSubMeshEntity_SaveData> mvSubMeshEntities;
};

//---------------------------------------------------------------

class cEngineBody_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineBody_SaveData)
public:
	void FromBody(iPhysicsBody *apBody);
	void ToBody(iPhysicsBody *apBody);

	float mfMass;
	bool mbActive;
	bool mbCollideCharacter;
	cVector3f mvLinearVelocity;
	cVector3f mvAngularVelocity;
	cMatrixf m_mtxTransform;
};

//---------------------------------------------------------------

class cEngineJointController_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineJointController_SaveData)
public:
	tString msName;

	float mfDestValue;
	bool mbActive;
};

//---------------------------------------------------------------

class cEngineJoint_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineJoint_SaveData)
public:
	void FromJoint(iPhysicsJoint *apJoint, iPhysicsWorld *apPhysicsWorld);
	void ToJoint(iPhysicsJoint *apJoint);

	tString msName;
	int mlID;
	float mfMinLimit;
	float mfMaxLimit;

	bool mbBroken;

	tString msOnMinCallback;
	tString msOnMaxCallback;

	cContainerVec<cEngineJointController_SaveData> mvControllers;
};

//---------------------------------------------------------------

class cEnginePSEmitter_SaveData : public iSerializable
{
	kSerializableClassInit(cEnginePSEmitter_SaveData)
public:
	bool mbActive;
};

class cEnginePS_SaveData : public iSerializable
{
	kSerializableClassInit(cEnginePS_SaveData)
public:
	void FromPS(cParticleSystem *apPS);
	void ToPS(cParticleSystem *apPS);

	tString msName;
	int mlID;
	tString msType;
	cVector3f mvSize;
	cMatrixf m_mtxTransform;

	cColor mColor;
	bool mbFadeAtDistance;
	float mfMinFadeDistanceStart;
	float mfMinFadeDistanceEnd;
	float mfMaxFadeDistanceStart;
	float mfMaxFadeDistanceEnd;

	bool mbActive;
	bool mbVisible;

	cContainerVec<cEnginePSEmitter_SaveData> mvEmitterActive;
};

//---------------------------------------------------------------

class cEngineBeam_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineBeam_SaveData)
public:
	void FromBeam(cBeam *apBeam);
	void ToBeam(cBeam *apBeam);

	tString msName;
	int mlID;
	tString msFile;

	cVector3f mvStartPos;
	cVector3f mvEndPos;
};

//---------------------------------------------------------------

class cEngineSound_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineSound_SaveData)
public:
	void FromSound(cSoundEntity* apSound);
	void ToSound(cSoundEntity* apSound);

	tString msName;
	int mlID;
	bool mbActive;
	bool mbStopped;
	tString msSoundDataName;
	float mfMinDistance;
	float mfMaxDistance;
	float mfVolume;
	cVector3f mvLocalPosition;
	bool mbRemoveWhenOver;
};

//---------------------------------------------------------------

class cEngineLight_SaveData : public iSerializable
{
	kSerializableClassInit(cEngineLight_SaveData)
public:
	void FromLight(iLight *apLight);
	void ToLight(iLight *apLight);

	tString msName;
	int mlID;
	bool mbActive;
	bool mbVisible;
	bool mbOnlyAffectInSector;

	cColor mDiffuseColor;
	float mfFarAttenuation;

	bool mbFlickering;
	tString msFlickerOffSound;
	tString msFlickerOnSound;
	tString msFlickerOffPS;
	tString msFlickerOnPS;
	float mfFlickerOnMinLength;
	float mfFlickerOffMinLength;
	float mfFlickerOnMaxLength;
	float mfFlickerOffMaxLength;
	cColor mFlickerOffColor;
	float mfFlickerOffRadius;
	bool mbFlickerFade;
	float mfFlickerOnFadeMinLength;
	float mfFlickerOnFadeMaxLength;
	float mfFlickerOffFadeMinLength;
	float mfFlickerOffFadeMaxLength;
};

//---------------------------------------------------------------


#endif // LUX_SAVED_ENGINE_TYPES_H
