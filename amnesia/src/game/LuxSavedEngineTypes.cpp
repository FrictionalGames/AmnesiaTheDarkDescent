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

#include "LuxSavedEngineTypes.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// CHARACTER BODY
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineCharacterBody_SaveData::FromBody(iCharacterBody *apBody)
{
	mbActive = apBody->IsActive();
	mfMass = apBody->GetMass();
	mvPosition = apBody->GetPosition();
	mfYaw = apBody->GetYaw();
	mfPitch = apBody->GetPitch();
	mfSpeedForward = apBody->GetMoveSpeed(eCharDir_Forward);
	mfSpeedRight = apBody->GetMoveSpeed(eCharDir_Right);
	mvForceVelocity = apBody->GetForceVelocity();
	mlActiveSize = apBody->GetActiveSize();
}

//------------------------------------------------------------------------

void cEngineCharacterBody_SaveData::ToBody(iCharacterBody *apBody)
{
	apBody->SetActive(mbActive);
	apBody->SetMass(mfMass);
	apBody->SetPosition(mvPosition);
	apBody->SetYaw(mfYaw);
	apBody->SetPitch(mfPitch);
	apBody->SetMoveSpeed(eCharDir_Forward, mfSpeedForward);
	apBody->SetMoveSpeed(eCharDir_Right, mfSpeedRight);
	apBody->SetForceVelocity(mvForceVelocity);
	apBody->SetActiveSize(mlActiveSize);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineCharacterBody_SaveData)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(mfMass, eSerializeType_Float32)
kSerializeVar(mvPosition, eSerializeType_Vector3f)
kSerializeVar(mfYaw, eSerializeType_Float32)
kSerializeVar(mfPitch, eSerializeType_Float32)
kSerializeVar(mfSpeedForward, eSerializeType_Float32)
kSerializeVar(mfSpeedRight, eSerializeType_Float32)
kSerializeVar(mvForceVelocity, eSerializeType_Vector3f)
kSerializeVar(mlActiveSize, eSerializeType_Int32)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// VERLET PARTICLE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineVerletParticle_SaveData::FromParticle(cVerletParticle *apPart)
{
	mvPosition = apPart->GetPosition();
	mvPrevPosition = apPart->GetPrevPosition();
	mvSmoothPosition = apPart->GetSmoothPosition();
	mfInvMass = apPart->GetInvMass();
}

//------------------------------------------------------------------------

void cEngineVerletParticle_SaveData::ToParticle(cVerletParticle *apPart)
{
	apPart->SetPosition(mvPosition, false);
	apPart->SetPrevPosition(mvPrevPosition);
	apPart->SetSmoothPosition(mvSmoothPosition);
	apPart->SetInvMass(mfInvMass);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineVerletParticle_SaveData)
kSerializeVar(mvPosition, eSerializeType_Vector3f)
kSerializeVar(mvPrevPosition, eSerializeType_Vector3f)
kSerializeVar(mvSmoothPosition, eSerializeType_Vector3f)
kSerializeVar(mfInvMass, eSerializeType_Float32)
kEndSerialize()

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// ROPE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cRopeEntity* cEngineRope_SaveData::CreateRope(cLuxMap *apMap)
{
	////////////////////
	// Create physics rope
	iPhysicsRope *pPhysicsRope = apMap->GetPhysicsWorld()->CreateRope(msPhysicsName,mvStartPos, mvEndPos);
    pPhysicsRope->SetUniqueID(mlUniquePhysicsID);

	pPhysicsRope->SetParticleRadius(mfParticleRadius);

	pPhysicsRope->SetMotorSound(msSound);
	pPhysicsRope->SetMotorSoundStartSpeed(mfSoundStartSpeed);
	pPhysicsRope->SetMotorSoundStopSpeed(mfSoundStopSpeed);

	////////////////////
	// Create rope entity
    cRopeEntity *pRope = apMap->GetWorld()->CreateRopeEntity(msName,pPhysicsRope, mlMaxSegments);
	pRope->SetUniqueID(mlUniqueGfxID);
	
	pRope->SetRadius(mfRadius);
	pRope->SetLengthTileAmount(mfLengthTileAmount);
	pRope->SetLengthTileSize(mfLengthTileSize);
	
	cMaterial *pMaterial = gpBase->mpEngine->GetResources()->GetMaterialManager()->CreateMaterial(msMaterial);
	if(pMaterial) pRope->SetMaterial(pMaterial);

	return pRope;
}

//------------------------------------------------------------------------

void cEngineRope_SaveData::FromRope(cRopeEntity *apRope)
{
	iPhysicsRope *pPhysicsRope = apRope->GetPhysicsRope();

	msName = apRope->GetName();
	mlUniqueGfxID = apRope->GetUniqueID();

	msPhysicsName = pPhysicsRope->GetName();
	mlUniquePhysicsID = pPhysicsRope->GetUniqueID();

	/////////////////////
	//Creation stuff

	//Not sure if these are needed when the level editor creates the ropes.
	mvStartPos = pPhysicsRope->GetStartParticle()->GetPosition();
	mvEndPos = pPhysicsRope->GetEndParticle()->GetPosition();

	msMaterial = cString::To8Char(apRope->GetMaterial()->GetFullPath());
	
	mlMaxSegments = apRope->GetMaxSegments();

	msSound = pPhysicsRope->GetMotorSound();
	mfSoundStartSpeed = pPhysicsRope->GetMotorSoundStartSpeed();
	mfSoundStopSpeed = pPhysicsRope->GetMotorSoundStopSpeed();

	mfParticleRadius = pPhysicsRope->GetParticleRadius();

	mfRadius = apRope->GetRadius();
	mfLengthTileAmount = apRope->GetLengthTileAmount();
	mfLengthTileSize = apRope->GetLengthTileSize();

	/////////////////////
	//Variables stuff
	mStartBody = LuxGetIdPairFromBody(pPhysicsRope->GetAttachedStartBody());
	mEndBody = LuxGetIdPairFromBody(pPhysicsRope->GetAttachedEndBody());
	
	mvStartBodyLocalPos = pPhysicsRope->GetAttachment(0)->mvBodyLocalPos;
	mvEndBodyLocalPos = pPhysicsRope->GetAttachment(1)->mvBodyLocalPos;
	
	
	mlMaxIterations = pPhysicsRope->GetMaxIterations();
	mfTotalLength = pPhysicsRope->GetTotalLength();
	mfMinTotalLength = pPhysicsRope->GetMinTotalLength();
	mfMaxTotalLength = pPhysicsRope->GetMaxTotalLength();
	mfSegmentLength = pPhysicsRope->GetSegmentLength();

	mfDamping = pPhysicsRope->GetDamping();
	mfStrength = pPhysicsRope->GetStrength();
	mfStiffness = pPhysicsRope->GetStiffness();

	mbMotorActive = pPhysicsRope->GetMotorActive();
	mfMotorWantedLength = pPhysicsRope->GetMotorWantedLength();
	mfMotorSpeedMul = pPhysicsRope->GetMotorSpeedMul();
	mfMotorMinSpeed = pPhysicsRope->GetMotorMinSpeed();
	mfMotorMaxSpeed = pPhysicsRope->GetMotorMaxSpeed();
	
	mbAutoMove = pPhysicsRope->GetAutoMoveActive();
	mfAutoMoveSpeed  = pPhysicsRope->GetAutoMoveSpeed();
	mfAutoMoveAcc = pPhysicsRope->GetAutoMoveAcc();
	mfAutoMoveMaxSpeed = pPhysicsRope->GetAutoMoveAcc();

	/////////////////////
	//Particles
	cVerletParticleIterator partIt = pPhysicsRope->GetParticleIterator();
	while(partIt.HasNext())
	{
		cVerletParticle *pPart = partIt.Next();
		cEngineVerletParticle_SaveData savePart;

        savePart.FromParticle(pPart);

		mvParticles.Add(savePart);
	}
}

//------------------------------------------------------------------------

void cEngineRope_SaveData::ToRope(cRopeEntity *apRope, cLuxMap *apMap)
{
	iPhysicsRope *pPhysicsRope = apRope->GetPhysicsRope();

	pPhysicsRope->SetMaxIterations(mlMaxIterations);
	pPhysicsRope->SetTotalLength(mfTotalLength);
	pPhysicsRope->SetMinTotalLength(mfMinTotalLength);
	pPhysicsRope->SetMaxTotalLength(mfMaxTotalLength);
	pPhysicsRope->SetSegmentLength(mfSegmentLength);

	pPhysicsRope->SetDamping(mfDamping);
	pPhysicsRope->SetStrength(mfStrength);
	pPhysicsRope->SetStiffness(mfStiffness);

	pPhysicsRope->SetMotorActive(mbMotorActive);
	pPhysicsRope->SetMotorWantedLength(mfMotorWantedLength);
	pPhysicsRope->SetMotorSpeedMul(mfMotorSpeedMul);
	pPhysicsRope->SetMotorMinSpeed(mfMotorMinSpeed);
	pPhysicsRope->SetMotorMaxSpeed(mfMotorMaxSpeed);

	pPhysicsRope->SetAutoMoveActive(mbAutoMove);
	pPhysicsRope->SetAutoMoveSpeed(mfAutoMoveSpeed);
	pPhysicsRope->SetAutoMoveAcc(mfAutoMoveAcc);
	pPhysicsRope->SetAutoMoveAcc(mfAutoMoveMaxSpeed);

	////////////////////////
	//Set particles
	int lPartCount=0;
	cVerletParticleIterator partIt = pPhysicsRope->GetParticleIterator();
	while(partIt.HasNext())
	{
		cVerletParticle *pPart = partIt.Next();
		cEngineVerletParticle_SaveData& savePart = mvParticles[lPartCount];

		savePart.ToParticle(pPart);

		++lPartCount;
	}

	////////////////////////
	//Set bodies
	//TODO: Save exact body attachment params!
	iPhysicsBody *pStartBody = apMap->GetBodyFromEntityBodyIdPair(mStartBody);
	if(pStartBody)
	{
		pPhysicsRope->SetAttachedStartBody(pStartBody);
		pPhysicsRope->GetAttachment(0)->mvBodyLocalPos = mvStartBodyLocalPos;
	}

	iPhysicsBody *pEndBody = apMap->GetBodyFromEntityBodyIdPair(mEndBody);
	if(pEndBody)
	{
		pPhysicsRope->SetAttachedEndBody(pEndBody);
		pPhysicsRope->GetAttachment(1)->mvBodyLocalPos = mvEndBodyLocalPos;
	}
	
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineRope_SaveData)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msPhysicsName, eSerializeType_String)
kSerializeVar(mlUniquePhysicsID, eSerializeType_Int32)
kSerializeVar(mlUniqueGfxID, eSerializeType_Int32)

kSerializeClassContainer(mvParticles, cEngineVerletParticle_SaveData, eSerializeType_Class)

kSerializeVar(mvStartPos, eSerializeType_Vector3f)
kSerializeVar(mvEndPos, eSerializeType_Vector3f)

kSerializeVar(mStartBody, eSerializeType_Class)
kSerializeVar(mEndBody, eSerializeType_Class)

kSerializeVar(mvStartBodyLocalPos, eSerializeType_Vector3f)
kSerializeVar(mvEndBodyLocalPos, eSerializeType_Vector3f)

kSerializeVar(mlMaxIterations, eSerializeType_Int32)
kSerializeVar(mfTotalLength, eSerializeType_Float32)
kSerializeVar(mfMinTotalLength, eSerializeType_Float32)
kSerializeVar(mfMaxTotalLength, eSerializeType_Float32)
kSerializeVar(mfSegmentLength, eSerializeType_Float32)

kSerializeVar(mfDamping, eSerializeType_Float32)
kSerializeVar(mfStrength, eSerializeType_Float32)
kSerializeVar(mfStiffness, eSerializeType_Float32)

kSerializeVar(msMaterial, eSerializeType_String)

kSerializeVar(mfParticleRadius, eSerializeType_Float32)

kSerializeVar(mlMaxSegments, eSerializeType_Int32)
kSerializeVar(mfRadius, eSerializeType_Float32)
kSerializeVar(mfLengthTileAmount, eSerializeType_Float32)
kSerializeVar(mfLengthTileSize, eSerializeType_Float32)

kSerializeVar(msSound, eSerializeType_String)
kSerializeVar(mfSoundStartSpeed, eSerializeType_Float32)
kSerializeVar(mfSoundStopSpeed, eSerializeType_Float32)

kSerializeVar(mbMotorActive, eSerializeType_Bool)
kSerializeVar(mfMotorWantedLength, eSerializeType_Float32)
kSerializeVar(mfMotorSpeedMul, eSerializeType_Float32)
kSerializeVar(mfMotorMinSpeed, eSerializeType_Float32)
kSerializeVar(mfMotorMaxSpeed, eSerializeType_Float32)

kSerializeVar(mbAutoMove, eSerializeType_Bool)
kSerializeVar(mfAutoMoveSpeed, eSerializeType_Float32)
kSerializeVar(mfAutoMoveAcc, eSerializeType_Float32)
kSerializeVar(mfAutoMoveMaxSpeed, eSerializeType_Float32)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// BILLBOARD
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineBillboard_SaveData::FromBillboard(cBillboard *apBillboard)
{
	mColor = apBillboard->GetColor();
}

//------------------------------------------------------------------------

void cEngineBillboard_SaveData::ToBillboard(cBillboard *apBillboard)
{
	apBillboard->SetColor(mColor);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineBillboard_SaveData)
kSerializeVar(mColor, eSerializeType_Color)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ANIMATION STATE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineAnimationState_SaveData::FromAnim(cAnimationState *apAnim)
{
	mbActive = apAnim->IsActive();
	mbLoop = apAnim->IsLooping();

	mfWeight = apAnim->GetWeight();
	mfFadeStep = apAnim->GetFadeStep();
	mfTimePos = apAnim->GetTimePosition();
	mfSpeed = apAnim->GetSpeed();

	//Log("  %d %d %f\n", mbActive, mbLoop, mfTimePos);
}
//------------------------------------------------------------------------

void cEngineAnimationState_SaveData::ToAnim(cAnimationState *apAnim)
{
	apAnim->SetActive(mbActive);
	apAnim->SetLoop(mbLoop);

	apAnim->SetWeight(mfWeight);
	apAnim->SetFadeStep(mfFadeStep);
	apAnim->SetTimePosition(mfTimePos);
	apAnim->SetSpeed(mfSpeed);

	//Log("  %d %d %f\n", mbActive, mbLoop, mfTimePos);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineAnimationState_SaveData)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(mbLoop, eSerializeType_Bool)

kSerializeVar(mfWeight, eSerializeType_Float32)
kSerializeVar(mfFadeStep, eSerializeType_Float32)
kSerializeVar(mfTimePos, eSerializeType_Float32)
kSerializeVar(mfSpeed, eSerializeType_Float32)
kEndSerialize()
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SUB MESH ENTITY
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineSubMeshEntity_SaveData::FromSubMeshEntity(cSubMeshEntity *apSubMeshEntity)
{
	mbActive = apSubMeshEntity->IsActive();
	mbVisible = apSubMeshEntity->IsVisible();
	mfIlluminationAmount = apSubMeshEntity->GetIlluminationAmount();
	m_mtxTransform = apSubMeshEntity->GetLocalMatrix();
}

//------------------------------------------------------------------------

void cEngineSubMeshEntity_SaveData::ToSubMeshEntity(cSubMeshEntity *apSubMeshEntity)
{
	apSubMeshEntity->SetActive(mbActive);
	apSubMeshEntity->SetVisible(mbVisible);
	apSubMeshEntity->SetIlluminationAmount(mfIlluminationAmount);
	apSubMeshEntity->SetMatrix(m_mtxTransform);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineSubMeshEntity_SaveData)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(mbVisible, eSerializeType_Bool)
kSerializeVar(mfIlluminationAmount, eSerializeType_Float32)
kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
kEndSerialize()
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// MESH ENTITY
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineMeshEntity_SaveData::FromMeshEntity(cMeshEntity *apMeshEntity)
{
	mbActive = apMeshEntity->IsActive();
	mbVisible = apMeshEntity->IsVisible();
	mfIlluminationAmount = apMeshEntity->GetIlluminationAmount();
	m_mtxTransform = apMeshEntity->GetLocalMatrix();

	mvAnimations.Resize(apMeshEntity->GetAnimationStateNum());
	//if(mvAnimations.Size()>0) Log("Saving anims  for '%s'\n", apMeshEntity->GetName().c_str());
	for(size_t i=0; i< mvAnimations.Size(); ++i)
	{
		cAnimationState *pAnimState =  apMeshEntity->GetAnimationState((int)i);
        mvAnimations[i].FromAnim(pAnimState);
	}

	mvSubMeshEntities.Resize(apMeshEntity->GetSubMeshEntityNum());
	for(size_t i=0; i< mvSubMeshEntities.Size(); ++i)
	{
		cSubMeshEntity *pSubEnt =  apMeshEntity->GetSubMeshEntity((int)i);
		mvSubMeshEntities[i].FromSubMeshEntity(pSubEnt);
	}
}

//------------------------------------------------------------------------

void cEngineMeshEntity_SaveData::ToMeshEntity(cMeshEntity *apMeshEntity)
{
	apMeshEntity->SetActive(mbActive);
	apMeshEntity->SetVisible(mbVisible);
	apMeshEntity->SetIlluminationAmount(mfIlluminationAmount);
	apMeshEntity->SetMatrix(m_mtxTransform);
	
	//If not equal, something is wrong so skip!
	if(mvAnimations.Size() == apMeshEntity->GetAnimationStateNum())
	{
		//if(mvAnimations.Size()>0) Log("Loading anims  for '%s'\n", apMeshEntity->GetName().c_str());
		for(size_t i=0; i< mvAnimations.Size(); ++i)
		{
			cAnimationState *pAnimState =  apMeshEntity->GetAnimationState((int)i);
			mvAnimations[i].ToAnim(pAnimState);
		}
	}

	//If not equal, something is wrong so skip!
	if(mvSubMeshEntities.Size() == apMeshEntity->GetSubMeshEntityNum())
	{
		for(size_t i=0; i< mvSubMeshEntities.Size(); ++i)
		{
			cSubMeshEntity *pSubEnt =  apMeshEntity->GetSubMeshEntity((int)i);
			mvSubMeshEntities[i].ToSubMeshEntity(pSubEnt);
		}
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cEngineMeshEntity_SaveData)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(mbVisible, eSerializeType_Bool)
kSerializeVar(mfIlluminationAmount, eSerializeType_Float32)
kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
kSerializeClassContainer(mvAnimations, cEngineAnimationState_SaveData, eSerializeType_Class)
kSerializeClassContainer(mvSubMeshEntities, cEngineSubMeshEntity_SaveData, eSerializeType_Class)
kEndSerialize()

//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// BODY
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineBody_SaveData::FromBody(iPhysicsBody *apBody)
{
	mbActive = apBody->IsActive();
	mfMass = apBody->GetMass();
	mbCollideCharacter = apBody->GetCollideCharacter();
	mvLinearVelocity = apBody->GetLinearVelocity();
	mvAngularVelocity = apBody->GetAngularVelocity();
	m_mtxTransform = apBody->GetWorldMatrix();

	//This updates the variables mass and collide characters (that are saved) according to
	// the current state (that might have changed these in some manner)
	gpBase->mpPlayer->GetCurrentStateData()->OnSaveBody(apBody, mfMass, mbCollideCharacter);
}

//------------------------------------------------------------------------

void cEngineBody_SaveData::ToBody(iPhysicsBody *apBody)
{
	apBody->SetActive(mbActive);
	apBody->SetMass(mfMass);
	apBody->SetCollideCharacter(mbCollideCharacter);
	apBody->SetAngularVelocity(mvAngularVelocity);
	apBody->SetLinearVelocity(mvLinearVelocity);
	apBody->SetMatrix(m_mtxTransform);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineBody_SaveData)
kSerializeVar(mfMass,eSerializeType_Float32)
kSerializeVar(mbActive,eSerializeType_Bool)
kSerializeVar(mbCollideCharacter,eSerializeType_Bool)
kSerializeVar(mvLinearVelocity,eSerializeType_Vector3f)
kSerializeVar(mvAngularVelocity,eSerializeType_Vector3f)
kSerializeVar(m_mtxTransform,eSerializeType_Matrixf)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// JOINT
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
void cEngineJoint_SaveData::FromJoint(iPhysicsJoint *apJoint, iPhysicsWorld *apPhysicsWorld)
{
	if(apJoint==NULL || apPhysicsWorld->JointExists(apJoint)==false)
	{
		mbBroken = true;
		return;
	}

	msName = apJoint->GetName();
	mlID = apJoint->GetUniqueID();
	mbBroken = false;

	///////////////////////////////
	//Controllers
	cPhysicsControllerIterator ctrlIt = apJoint->GetControllerIterator();
	while(ctrlIt.HasNext())
	{
		iPhysicsController *pCtrl = ctrlIt.Next();
		cEngineJointController_SaveData saveCtrl;

		saveCtrl.msName = pCtrl->GetName();
		saveCtrl.mfDestValue = pCtrl->GetDestValue();
		saveCtrl.mbActive = pCtrl->IsActive();

		mvControllers.Add(saveCtrl);
	}

	///////////////////////////////
	//Type specific
	switch(apJoint->GetType())
	{
	case ePhysicsJointType_Ball:
		{
			iPhysicsJointBall *pBallJoint = static_cast<iPhysicsJointBall*>(apJoint);
			mfMaxLimit = pBallJoint->GetMaxConeAngle();
			mfMinLimit = pBallJoint->GetMaxTwistAngle();
			break;
		}
	case ePhysicsJointType_Hinge:
		{
			iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(apJoint);
			mfMaxLimit = pHingeJoint->GetMaxAngle();
			mfMinLimit = pHingeJoint->GetMinAngle();
			break;
		}
	case ePhysicsJointType_Screw:
		{
			iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(apJoint);
			mfMinLimit = pScrewJoint->GetMinDistance();
			mfMaxLimit = pScrewJoint->GetMaxDistance();
			break;
		}
	case ePhysicsJointType_Slider:
		{
			iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(apJoint);
			mfMinLimit = pSliderJoint->GetMinDistance();
			mfMaxLimit = pSliderJoint->GetMaxDistance();
			break;
		}
	}
}

//------------------------------------------------------------------------

void cEngineJoint_SaveData::ToJoint(iPhysicsJoint *apJoint)
{
	//////////////////////////////
	//Controllers
	for(int i =0; i< (int)mvControllers.Size(); ++i)
	{
		iPhysicsController *pCtrl = apJoint->GetController(mvControllers[i].msName);

		if(pCtrl)
		{
			pCtrl->SetDestValue(mvControllers[i].mfDestValue);
			pCtrl->SetActive(mvControllers[i].mbActive);
		}
	}

	///////////////////////////////
	//Type specific
	switch(apJoint->GetType())
	{
	case ePhysicsJointType_Ball:
		{
			iPhysicsJointBall *pBallJoint = static_cast<iPhysicsJointBall*>(apJoint);
			pBallJoint->SetConeLimits(mfMaxLimit,mfMinLimit);
			break;
		}
	case ePhysicsJointType_Hinge:
		{
			iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(apJoint);
			pHingeJoint->SetMaxAngle(mfMaxLimit);
			pHingeJoint->SetMinAngle(mfMinLimit);
			break;
		}
	case ePhysicsJointType_Screw:
		{
			iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(apJoint);
			pScrewJoint->SetMinDistance(mfMinLimit);
			pScrewJoint->SetMaxDistance(mfMaxLimit);
			break;
		}
	case ePhysicsJointType_Slider:
		{
			iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(apJoint);
			pSliderJoint->SetMinDistance(mfMinLimit);
			pSliderJoint->SetMaxDistance(mfMaxLimit);
			break;
		}
	}
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineJointController_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mfDestValue,eSerializeType_Float32)
kSerializeVar(mbActive,eSerializeType_Bool)
kEndSerialize()

kBeginSerializeBase(cEngineJoint_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(mfMinLimit,eSerializeType_Float32)
kSerializeVar(mfMaxLimit,eSerializeType_Float32)
kSerializeVar(mbBroken,eSerializeType_Bool)
kSerializeVar(msOnMinCallback,eSerializeType_String)
kSerializeVar(msOnMaxCallback,eSerializeType_String)
kSerializeClassContainer(mvControllers,cEngineJointController_SaveData,eSerializeType_Class)
kEndSerialize()
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PS
//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------
void cEnginePS_SaveData::FromPS(cParticleSystem *apPS)
{
	if(apPS)
	{
		msName = apPS->GetName();
		mlID = apPS->GetUniqueID();

		msType = apPS->GetDataName();
		mvSize = apPS->GetDataSize();
		m_mtxTransform = apPS->GetLocalMatrix();

		mColor = apPS->GetColor();
		mbFadeAtDistance = apPS->GetFadeAtDistance();
		mfMinFadeDistanceStart = apPS->GetMinFadeDistanceStart();
		mfMinFadeDistanceEnd =apPS->GetMinFadeDistanceEnd();
		mfMaxFadeDistanceStart =apPS->GetMaxFadeDistanceStart();
		mfMaxFadeDistanceEnd =apPS->GetMaxFadeDistanceEnd();

		mbVisible = apPS->IsVisible();
		mbActive = apPS->IsActive();

		mvEmitterActive.Resize(apPS->GetEmitterNum());
		for(int i=0; i< apPS->GetEmitterNum(); ++i)
		{
			iParticleEmitter *pEmitter = apPS->GetEmitter(i);

			if(pEmitter->IsDying() || pEmitter->IsDead())
			{
				mvEmitterActive[i].mbActive = false;
			}
			else
			{
				mvEmitterActive[i].mbActive = true;
			}
		}
	}
	else
	{
		msName = "NULL";
		msType = "";
		mvEmitterActive.Clear();
	}
}

//------------------------------------------------------------------------

void cEnginePS_SaveData::ToPS(cParticleSystem *apPS)
{
	if(apPS==NULL) return;

	apPS->SetActive(mbActive);
	apPS->SetVisible(mbVisible);
	apPS->SetUniqueID(mlID);

	apPS->SetMatrix(m_mtxTransform);

	apPS->SetColor(mColor);
	apPS->SetFadeAtDistance(mbFadeAtDistance);
	apPS->SetMinFadeDistanceStart(mfMinFadeDistanceStart);
	apPS->SetMinFadeDistanceEnd(mfMinFadeDistanceEnd);
	apPS->SetMaxFadeDistanceStart(mfMaxFadeDistanceStart);
	apPS->SetMaxFadeDistanceEnd(mfMaxFadeDistanceEnd);

	for(size_t i=0; i< mvEmitterActive.Size(); ++i)
	{
		iParticleEmitter *pEmitter = apPS->GetEmitter((int)i);

		if(mvEmitterActive[i].mbActive==false)
		{
			pEmitter->KillInstantly();
		}
	}
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEnginePSEmitter_SaveData)
kSerializeVar(mbActive,eSerializeType_Bool)
kEndSerialize()

kBeginSerializeBase(cEnginePS_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlID, eSerializeType_Int32)

kSerializeVar(mbActive,eSerializeType_Bool)
kSerializeVar(mbVisible,eSerializeType_Bool)

kSerializeVar(msType,eSerializeType_String)
kSerializeVar(mvSize,eSerializeType_Vector3f)
kSerializeVar(m_mtxTransform,eSerializeType_Matrixf)

kSerializeVar(mColor,eSerializeType_Color)
kSerializeVar(mbFadeAtDistance,eSerializeType_Bool)
kSerializeVar(mfMinFadeDistanceStart,eSerializeType_Float32)
kSerializeVar(mfMinFadeDistanceEnd,eSerializeType_Float32)
kSerializeVar(mfMaxFadeDistanceStart,eSerializeType_Float32)
kSerializeVar(mfMaxFadeDistanceEnd,eSerializeType_Float32)

kSerializeClassContainer(mvEmitterActive,cEnginePSEmitter_SaveData,eSerializeType_Class)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// BEAM
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineBeam_SaveData::FromBeam(cBeam *apBeam)
{
	msName = apBeam->GetName();
	mlID = apBeam->GetUniqueID();
	msFile = apBeam->GetFileName();

	mvStartPos = apBeam->GetLocalPosition();
	mvEndPos = apBeam->GetEnd()->GetLocalPosition();
}

//------------------------------------------------------------------------

void cEngineBeam_SaveData::ToBeam(cBeam *apBeam)
{
	apBeam->LoadXMLProperties(msFile);	

	apBeam->SetPosition(mvStartPos);
	apBeam->GetEnd()->SetPosition(mvEndPos);
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineBeam_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(msFile,eSerializeType_String)

kSerializeVar(mvStartPos,eSerializeType_Vector3f)
kSerializeVar(mvEndPos,eSerializeType_Vector3f)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SOUND
//////////////////////////////////////////////////////////////////////////

void cEngineSound_SaveData::FromSound(cSoundEntity* apSound)
{
	msName = apSound->GetName();
	mlID = apSound->GetUniqueID();
	mbActive = apSound->IsActive();
	mbRemoveWhenOver = apSound->GetRemoveWhenOver();

	if(apSound->IsStopped() || apSound->IsFadingOut())
		mbStopped = true;
	else
		mbStopped = false;

	msSoundDataName = apSound->GetData()->GetName();

	mfMinDistance = apSound->GetMinDistance();
	mfMaxDistance = apSound->GetMaxDistance();
	mfVolume = apSound->GetVolume();
	mvLocalPosition = apSound->GetLocalPosition();
}

//------------------------------------------------------------------------

void cEngineSound_SaveData::ToSound(cSoundEntity* apSound)
{
	apSound->SetUniqueID(mlID);
	apSound->SetActive(mbActive);
	if(mbStopped) apSound->Stop(false);

	apSound->SetMinDistance(mfMinDistance);
	apSound->SetMaxDistance(mfMaxDistance);
	apSound->SetVolume(mfVolume);
	apSound->SetPosition(mvLocalPosition);
}
//------------------------------------------------------------------------

kBeginSerializeBase(cEngineSound_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(mbActive,eSerializeType_Bool)
kSerializeVar(mbStopped,eSerializeType_Bool)
kSerializeVar(msSoundDataName,eSerializeType_String)
kSerializeVar(mfMinDistance,eSerializeType_Float32)
kSerializeVar(mfMaxDistance,eSerializeType_Float32)
kSerializeVar(mfVolume,eSerializeType_Float32)
kSerializeVar(mvLocalPosition,eSerializeType_Vector3f)
kSerializeVar(mbRemoveWhenOver,eSerializeType_Bool)
kEndSerialize()

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LIGHT
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

void cEngineLight_SaveData::FromLight(iLight *apLight)
{
	bool bHasParent = true;
	if(apLight->GetParent() == NULL && apLight->GetEntityParent()==NULL)
	{
		bHasParent = false;
	}

	msName = apLight->GetName();
	mlID = apLight->GetUniqueID();

	mbActive = apLight->IsActive();
	mbVisible = apLight->GetVisibleVar();
	
	
	if(bHasParent==false)
	{
		if(apLight->IsFading() && apLight->GetFlickerActive()==false)
		{
			apLight->SetDiffuseColor(apLight->GetDestColor());
			apLight->SetRadius(apLight->GetDestRadius());
		}

		if(apLight->GetFlickerActive())
		{
			mDiffuseColor = apLight->GetFlickerOnColor();
			mfFarAttenuation = apLight->GetFlickerOnRadius();
		}
		else
		{
			mDiffuseColor = apLight->GetDiffuseColor();
			mfFarAttenuation = apLight->GetRadius();
		}

		//TODO: Add billboard attaching!
		
		mbFlickering = apLight->GetFlickerActive();
		msFlickerOffSound = apLight->GetFlickerOffSound();
		msFlickerOnSound = apLight->GetFlickerOnSound();
		msFlickerOffPS = apLight->GetFlickerOffPS();
		msFlickerOnPS = apLight->GetFlickerOnPS();
		mfFlickerOnMinLength = apLight->GetFlickerOnMinLength();
		mfFlickerOffMinLength = apLight->GetFlickerOffMinLength();
		mfFlickerOnMaxLength = apLight->GetFlickerOnMaxLength();
		mfFlickerOffMaxLength = apLight->GetFlickerOffMaxLength();
		mFlickerOffColor = apLight->GetFlickerOffColor();
		mfFlickerOffRadius = apLight->GetFlickerOffRadius();
		mbFlickerFade = apLight->GetFlickerFade();
		mfFlickerOnFadeMinLength = apLight->GetFlickerOnFadeMinLength();
		mfFlickerOnFadeMaxLength = apLight->GetFlickerOnFadeMaxLength();
		mfFlickerOffFadeMinLength = apLight->GetFlickerOffFadeMinLength();
		mfFlickerOffFadeMaxLength = apLight->GetFlickerOffFadeMaxLength();
	}
}

//------------------------------------------------------------------------

void cEngineLight_SaveData::ToLight(iLight *apLight)
{
	bool bHasParent = true;
	if(apLight->GetParent() == NULL && apLight->GetEntityParent()==NULL)
	{
		bHasParent = false;
	}

	if(bHasParent==false)
	{
		apLight->SetActive(mbActive);
		apLight->SetVisible(mbVisible);
	}
	if(bHasParent==false)
	{
		apLight->SetDiffuseColor(mDiffuseColor);
		apLight->SetRadius(mfFarAttenuation);

		//TODO: Attach billboards.

		apLight->SetFlickerActive(mbFlickering);
		apLight->SetFlicker(mFlickerOffColor,mfFlickerOffRadius,
			mfFlickerOnMinLength,mfFlickerOnMaxLength,
			msFlickerOnSound,msFlickerOnPS,
			mfFlickerOffMinLength,mfFlickerOffMaxLength,
			msFlickerOffSound,msFlickerOffPS,
			mbFlickerFade,mfFlickerOnFadeMinLength,mfFlickerOnFadeMaxLength,
			mfFlickerOffFadeMinLength, mfFlickerOffFadeMaxLength);
	}
}

//------------------------------------------------------------------------

kBeginSerializeBase(cEngineLight_SaveData)
kSerializeVar(msName,eSerializeType_String)
kSerializeVar(mlID, eSerializeType_Int32)
kSerializeVar(mbActive,eSerializeType_Bool)
kSerializeVar(mbVisible,eSerializeType_Bool)
kSerializeVar(mbOnlyAffectInSector,eSerializeType_Bool)

kSerializeVar(mDiffuseColor, eSerializeType_Color)
kSerializeVar(mfFarAttenuation, eSerializeType_Float32)

kSerializeVar(mbFlickering,eSerializeType_Bool)
kSerializeVar(msFlickerOffSound,eSerializeType_String)
kSerializeVar(msFlickerOnSound,eSerializeType_String)
kSerializeVar(msFlickerOffPS,eSerializeType_String)
kSerializeVar(msFlickerOnPS,eSerializeType_String)
kSerializeVar(mfFlickerOnMinLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOffMinLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOnMaxLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOffMaxLength, eSerializeType_Float32)
kSerializeVar(mFlickerOffColor, eSerializeType_Color)
kSerializeVar(mfFlickerOffRadius, eSerializeType_Float32)
kSerializeVar(mbFlickerFade,eSerializeType_Bool)
kSerializeVar(mfFlickerOnFadeMinLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOnFadeMaxLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOffFadeMinLength, eSerializeType_Float32)
kSerializeVar(mfFlickerOffFadeMaxLength, eSerializeType_Float32)
kEndSerialize()

//------------------------------------------------------------------------



