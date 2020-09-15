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

#include "LuxProp_Lever.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxHintHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Lever::cLuxPropLoader_Lever(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Lever_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Lever::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Lever, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

static int ToAutoMoveGoal(const tString& asType)
{
	if(asType == "Middle")	return 0;
	if(asType == "Max")		return 1;
	if(asType == "Min")		return -1;

	return 0;
}

void cLuxPropLoader_Lever::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Lever  *pLever = static_cast<cLuxProp_Lever*>(apProp);

	///////////////////////////
	// General
	pLever->mLeverData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
	pLever->mLeverData.mfMoveSlowDownFactor = GetVarFloat("MoveSlowDownFactor", 3.0f);
	pLever->mLeverData.mfMoveSpeedFactor = GetVarFloat("MoveSpeedFactor", 1.0f);
	pLever->mLeverData.mfMoveThrowImpulse = GetVarFloat("MoveThrowImpulse", 6.0f);

	pLever->mbCanInteractWithStaticBody = GetVarBool("CanInteractWithStaticBody", true);
	pLever->mfMinLimitRange = cMath::ToRad(GetVarFloat("MinLimitRange", 5.0f));
	pLever->mfMaxLimitRange = cMath::ToRad(GetVarFloat("MaxLimitRange", 5.0f));
	pLever->mbMinLimitStuck = GetVarBool("MinLimitStuck", false);
	pLever->mbMaxLimitStuck = GetVarBool("MaxLimitStuck", false);

	pLever->mfMiddleAngleAmount = GetVarFloat("MiddleAngleAmount", 0);
	
	pLever->mbAutoMoveToAngle = GetVarBool("AutoMoveToAngle", true);
	pLever->mlAutoMoveGoal = ToAutoMoveGoal(GetVarString("AutoMoveGoal", "Middle"));
	pLever->mfAutoMoveSpeedFactor = GetVarFloat("AutoMoveSpeedFactor", 2.0f);
	pLever->mfAutoMoveMaxSpeed = GetVarFloat("AutoMoveMaxSpeed", 8.0f);
	
	pLever->msMinLimitSound = GetVarString("MinLimitSound", "");
	pLever->msMaxLimitSound = GetVarString("MaxLimitSound", "");

	pLever->msStuckSound = GetVarString("StuckSound", "");
}
//-----------------------------------------------------------------------

static int ToStuckState(const tString& asType)
{
	if(asType == "None")	return 0;
	if(asType == "Min")		return -1;
	if(asType == "Max")		return 1;

	return 0;
}

void cLuxPropLoader_Lever::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Lever  *pLever = static_cast<cLuxProp_Lever*>(apProp);

	int lStuckState = ToStuckState(apInstanceVars-> GetVarString("StuckState", ""));
	pLever->SetStuckState(lStuckState, false);

	pLever->mbInteractionDisablesStuck = apInstanceVars-> GetVarBool("InteractionDisablesStuck", false);

	bool bOverrideDefault = apInstanceVars->GetVarBool("OverrideDefaults", false);
	if(bOverrideDefault)
	{
		pLever->mfMiddleAngleAmount = apInstanceVars->GetVarFloat("MiddleAngleAmount", 0);
		pLever->mbAutoMoveToAngle = apInstanceVars->GetVarBool("AutoMoveToAngle", true);
		pLever->mlAutoMoveGoal = ToAutoMoveGoal(apInstanceVars->GetVarString("AutoMoveGoal", "Middle"));

		//Need to recalculate middle angle!
		pLever->CalculateMiddleAngle();
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Lever::cLuxProp_Lever(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Lever)
{
	mlCurrentState  = 0; //-1 = min, 1=max, 0= middle
	mlStuckState = 0;//-1 = min, 1=max, 0= not stuck
	mbInteractionDisablesStuck = false;

	mfStuckSoundTimer =0;

	mRotatePid.SetErrorNum(10);
	mRotatePid.p = 10.0f;
	mRotatePid.i = 0.0f;
	mRotatePid.d = 1.0f;
}

//-----------------------------------------------------------------------

cLuxProp_Lever::~cLuxProp_Lever()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Lever::CanInteract(iPhysicsBody *apBody)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return false;

	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_Lever::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody)
	{
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			if(mvBodies[i]->GetMass() > 0)
			{
				apBody = mvBodies[i];
				break;
			}
		}
	}

	if(mlStuckState !=0 && mbInteractionDisablesStuck)
		SetStuckState(0, true);

	if(mlStuckState ==0 && mbShowHints)
		gpBase->mpHintHandler->Add("EntityLever", kTranslate("Hints", "EntityLever"), 0);

	cLuxPlayerStateVars::SetupInteraction(apBody, avPos);
	gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractLever);

	mRotatePid.Reset();
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::OnSetupAfterLoad(cWorld *apWorld)
{
	int lNum=0;
	iPhysicsJoint *pJoint = mvJoints[0];
	if(pJoint->GetType() != ePhysicsJointType_Hinge)
	{
		Error("Lever %s does not have a hinge joint!\n", msName.c_str());
		return;
	}

	mpHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);

	mpLeverBody = mpHingeJoint->GetChildBody();

	if(mbAutoMoveToAngle)
	{
		mpLeverBody->SetGravity(false);
	}

	///////////////////////
	//Set update properties
	mpHingeJoint->SetStickyMinLimit(true);
	mpHingeJoint->SetStickyMaxLimit(true);

	///////////////////////
	//Set up data
	mfDefaultMinAngle = mpHingeJoint->GetMinAngle();
	mfDefaultMaxAngle = mpHingeJoint->GetMaxAngle();

	///////////////////////
	//Calculate the middle angle
	CalculateMiddleAngle();
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::OnResetProperties()
{

}
//-----------------------------------------------------------------------

void cLuxProp_Lever::UpdatePropSpecific(float afTimeStep)
{
	float fAngle = mpHingeJoint->GetAngle();

	UpdateCheckStuckSound(afTimeStep);
	UpdateCheckLimit(fAngle, afTimeStep);
	UpdateAutoMove(fAngle, afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::BeforePropDestruction()
{
	
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Lever::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return eLuxFocusCrosshair_Default;

	return eLuxFocusCrosshair_Grab;
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::SetStuckState(int alState, bool abEffects)
{
	if(mlStuckState == alState) return;

	mlStuckState = alState;

	mfStuckSoundTimer =0; //Reset the stuck timer!

	//Max
    if(mlStuckState == 1)
	{
		ChangeState(1, abEffects);

		mpHingeJoint->SetMinAngle(mfDefaultMaxAngle - mfMaxLimitRange);
		mpHingeJoint->SetMaxAngle(mfDefaultMaxAngle);
	}
	//Min
	else if(mlStuckState == -1)
	{
		ChangeState(-1, abEffects);

		mpHingeJoint->SetMinAngle(mfDefaultMinAngle);
		mpHingeJoint->SetMaxAngle(mfDefaultMinAngle + mfMinLimitRange);
	}
	//None
	else
	{
		mpHingeJoint->SetMinAngle(mfDefaultMinAngle);
		mpHingeJoint->SetMaxAngle(mfDefaultMaxAngle);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	if(alState >0)	SetStuckState(-1, true);
	else			SetStuckState(0, true);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_Lever::CalculateMiddleAngle()
{
	float fAngleT = (mfMiddleAngleAmount+1)/2; //be between 0 and 1
	mfMiddleAngle = mfDefaultMinAngle + (mfDefaultMaxAngle - mfDefaultMinAngle)*fAngleT;
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::UpdateCheckStuckSound(float afTimeStep)
{
	if(mlStuckState == 0 || mbIsInteractedWith==false) return;

	if(mfStuckSoundTimer >0)
	{
		mfStuckSoundTimer-=afTimeStep; 
		return;
	}
	
	float fSpeedSqr = mpHingeJoint->GetChildBody()->GetAngularVelocity().SqrLength();
	if(fSpeedSqr > 0.01f)
	{
		PlaySound("LeverStuck", msStuckSound, true, true);
		mfStuckSoundTimer = 1.5f;
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::UpdateCheckLimit(float afAngle, float afTimeStep)
{
	if(mlStuckState !=0) return;

	///////////////////////
	//Max
	if(afAngle > (mfDefaultMaxAngle - mfMaxLimitRange))
	{
		ChangeState(1, true);
		if(mbMaxLimitStuck) SetStuckState(1, false);
	}
	///////////////////////
	//Min
	else if(afAngle < (mfDefaultMinAngle + mfMinLimitRange))
	{
		ChangeState(-1, true);
		if(mbMinLimitStuck) SetStuckState(-1, false);
	}
	///////////////////////
	//Middle
	else if(afAngle < (mfDefaultMaxAngle - mfMaxLimitRange)-cMath::ToRad(5) ||
			afAngle > (mfDefaultMinAngle + mfMinLimitRange)+cMath::ToRad(5))
	{
		ChangeState(0, true);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::UpdateAutoMove(float afAngle, float afTimeStep)
{
	if(IsInteractedWith() || mbAutoMoveToAngle==false || mlStuckState!=0) return;

	///////////////////////////
	// Get the wanted speed
    float fGoalAngle = 0;
	if(mlAutoMoveGoal == 0) fGoalAngle = mfMiddleAngle;
	else if(mlAutoMoveGoal == -1)	fGoalAngle = mfDefaultMinAngle + mfMinLimitRange/2.0f;
	else if(mlAutoMoveGoal == 1)	fGoalAngle = mfDefaultMaxAngle - mfMaxLimitRange/2.0f;
	
	float fWantedSpeed = mfAutoMoveSpeedFactor * (afAngle - fGoalAngle);
	
	if(fWantedSpeed > mfAutoMoveMaxSpeed)	fWantedSpeed = mfAutoMoveMaxSpeed;
	if(fWantedSpeed < -mfAutoMoveMaxSpeed)	fWantedSpeed = -mfAutoMoveMaxSpeed;

	///////////////////////////
	// Calculate the torque
	cVector3f vRotateDir = mpHingeJoint->GetPinDir();
	cVector3f vBodyVel = mpLeverBody->GetAngularVelocity();
	cVector3f vHingeVel = vRotateDir * cMath::Vector3Dot(vRotateDir, vBodyVel);
	cVector3f vWantedVel = vRotateDir * fWantedSpeed;

	//This is a bit wierd? (think need to have as removing might screw things up!)
	mpLeverBody->SetAngularVelocity(vWantedVel);

	
	cVector3f vTorque = mRotatePid.Output(vWantedVel - vHingeVel, afTimeStep);
	vTorque = cMath::MatrixMul(mpLeverBody->GetInertiaMatrix(), vTorque);

	mpLeverBody->AddTorque(vTorque);
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::ChangeState(int alState, bool abEffects)
{
	if(mlCurrentState == alState) return;

	mlCurrentState = alState;

	tString sSound = "";
	
	/////////////////
	//Max
	if(mlCurrentState == 1)
	{
		sSound = msMaxLimitSound;
		ConnectionStateChange(1);
	}
	/////////////////
	//Min
	else if(mlCurrentState == -1)
	{
		sSound = msMinLimitSound;
		ConnectionStateChange(-1);
	}
	/////////////////
	//Middle
	else if(mlCurrentState == 0)
	{
		ConnectionStateChange(0);
	}

	///////////////////////
	//Play sound
	if(abEffects  && sSound != "")
	{
		cSoundEntity *pSound = mpMap->GetWorld()->CreateSoundEntity("LeverSound", sSound, true);
		if(pSound) pSound->SetPosition(mpHingeJoint->GetPivotPoint());
	}

	/////////////////
	//Connection
	InteractConnectionLimit(mlCurrentState);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Lever_SaveData, iLuxProp_SaveData)
kSerializeVar(mlCurrentState, eSerializeType_Int32)
kSerializeVar(mlStuckState, eSerializeType_Int32)
kSerializeVar(mbInteractionDisablesStuck, eSerializeType_Bool)
kSerializeVar(mfMiddleAngleAmount, eSerializeType_Float32)
kSerializeVar(mbAutoMoveToAngle, eSerializeType_Bool)
kSerializeVar(mlAutoMoveGoal, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Lever::CreateSaveData()
{
	return hplNew(cLuxProp_Lever_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Lever_SaveData *pData = static_cast<cLuxProp_Lever_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mlCurrentState);
	kCopyToVar(pData,mlStuckState);
	kCopyToVar(pData,mbInteractionDisablesStuck);
	kCopyToVar(pData,mfMiddleAngleAmount);
	kCopyToVar(pData,mbAutoMoveToAngle);
	kCopyToVar(pData,mlAutoMoveGoal);
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Lever_SaveData *pData = static_cast<cLuxProp_Lever_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mlCurrentState);
	SetStuckState(pData->mlStuckState, false);
	kCopyFromVar(pData,mbInteractionDisablesStuck);
	kCopyFromVar(pData,mfMiddleAngleAmount);
	kCopyFromVar(pData,mbAutoMoveToAngle);
	kCopyFromVar(pData,mlAutoMoveGoal);
}

//-----------------------------------------------------------------------

void cLuxProp_Lever::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{	
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
