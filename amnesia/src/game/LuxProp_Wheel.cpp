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

#include "LuxProp_Wheel.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxHintHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Wheel::cLuxPropLoader_Wheel(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Wheel_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Wheel::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Wheel, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

static int ToSpinDir(const tString& asType)
{
	if(asType == "BothWays")	return 0;
	if(asType == "ToMax")		return 1;
	if(asType == "ToMin")		return -1;

	return 0;
}

void cLuxPropLoader_Wheel::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Wheel  *pWheel = static_cast<cLuxProp_Wheel*>(apProp);

	///////////////////////////
	// General
	pWheel->mWheelData.mfMoveMaxSpeed = GetVarFloat("MoveMaxSpeed", 13.5f);
	pWheel->mWheelData.mfMoveSlowDownFactor = GetVarFloat("MoveSlowDownFactor", 3.0f);
	pWheel->mWheelData.mfMoveSpeedFactor = GetVarFloat("MoveSpeedFactor", 1.0f);
	pWheel->mWheelData.mfMoveThrowImpulse = GetVarFloat("MoveThrowImpulse", 6.0f);

	pWheel->mbCanInteractWithStaticBody = GetVarBool("CanInteractWithStaticBody", true);

	pWheel->mfMinLimit = cMath::ToRad(GetVarFloat("MinLimit", -360));
	pWheel->mfMaxLimit = cMath::ToRad(GetVarFloat("MaxLimit", 360));
	pWheel->mfMinLimitRange = cMath::ToRad(GetVarFloat("MinLimitRange", 5.0f));
	pWheel->mfMaxLimitRange = cMath::ToRad(GetVarFloat("MaxLimitRange", 5.0f));
	pWheel->mbMinLimitStuck = GetVarBool("MinLimitStuck", false);
	pWheel->mbMaxLimitStuck = GetVarBool("MaxLimitStuck", false);

	pWheel->mlSpinDir = ToSpinDir(GetVarString("SpinDir", "BothWays"));

	pWheel->mbSlowDownRotation = GetVarBool("SlowDownRotation", true);
	pWheel->mbSkipMiddleState = GetVarBool("SkipMiddleState", true);

	pWheel->msMinLimitSound = GetVarString("MinLimitSound", "");
	pWheel->msMaxLimitSound = GetVarString("MaxLimitSound", "");

	pWheel->msStuckSound = GetVarString("StuckSound", "");
}

//-----------------------------------------------------------------------

static int ToStuckState(const tString& asType)
{
	if(asType == "None")	return 0;
	if(asType == "Min")		return -1;
	if(asType == "Max")		return 1;

	return 0;
}

void cLuxPropLoader_Wheel::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Wheel  *pWheel = static_cast<cLuxProp_Wheel*>(apProp);

	bool bOverrideDefault = apInstanceVars->GetVarBool("OverrideDefaults", false);
	if(bOverrideDefault)
	{
		pWheel->mfMinLimit = cMath::ToRad(apInstanceVars->GetVarFloat("MinLimit", -360));
		pWheel->mfMaxLimit = cMath::ToRad(apInstanceVars->GetVarFloat("MaxLimit", 360));
		pWheel->mbMinLimitStuck = apInstanceVars->GetVarBool("MinLimitStuck", false);
		pWheel->mbMaxLimitStuck = apInstanceVars->GetVarBool("MaxLimitStuck", false);

		pWheel->mlSpinDir = ToSpinDir(apInstanceVars->GetVarString("SpinDir", "BothWays"));
	}
	
	//Important that this is here so limits are set up properly!
	int lStuckState = ToStuckState(apInstanceVars->GetVarString("StuckState", ""));
	pWheel->SetStuckState(lStuckState, false);

	pWheel->mbInteractionDisablesStuck = apInstanceVars-> GetVarBool("InteractionDisablesStuck", false);

}
//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Wheel::cLuxProp_Wheel(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Wheel)
{
	mlCurrentState  = 0; //-1 = min, 1=max, 0= middle
	mlStuckState = 0;//-1 = min, 1=max, 0= not stuck
	mfAngle =0;
	mfPrevAngle=0;
	
	mfJointAngle = 0;
	mfPrevJointAngle = 0;

	mfLastToMin = 0;
	mfLastToMax = 0;

	mfStuckSoundTimer =0;

	mbAutoMoving = false;
	mfAutoMoveGoal = 0;
	
	mRotatePid.SetErrorNum(10);
	mRotatePid.p = 10.0f;
	mRotatePid.i = 0.0f;
	mRotatePid.d = 0.5f;

	mbInteractionDisablesStuck = false;
}

//-----------------------------------------------------------------------

cLuxProp_Wheel::~cLuxProp_Wheel()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_Wheel::CanInteract(iPhysicsBody *apBody)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return false;

	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_Wheel::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody)
	{
		apBody = mpWheelBody;
	}
	ResetAutoMove();

	if(mlStuckState !=0 && mbInteractionDisablesStuck)
		SetStuckState(0, true);
	
	if(mlStuckState ==0 && mbShowHints)
		gpBase->mpHintHandler->Add("EntityWheel", kTranslate("Hints", "EntityWheel"), 0);

	cLuxPlayerStateVars::SetupInteraction(apBody, avPos);
	gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractWheel);
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::OnResetProperties()
{
	
}


//-----------------------------------------------------------------------

void cLuxProp_Wheel::OnSetupAfterLoad(cWorld *apWorld)
{
	int lNum=0;
	iPhysicsJoint *pJoint = mvJoints[0];
	if(pJoint->GetType() != ePhysicsJointType_Hinge)
	{
		Error("Wheel %s does not have a hinge joint!\n", msName.c_str());
		return;
	}

	mpHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);

	mpWheelBody = mpHingeJoint->GetChildBody();

	mpWheelBody->SetGravity(false);

	///////////////////////
	//Set update properties
	if(mlSpinDir==0)
	{
		mpHingeJoint->SetStickyMinLimit(true);
		mpHingeJoint->SetStickyMaxLimit(true);
	}

	mpHingeJoint->SetMinAngle(0);
	mpHingeJoint->SetMaxAngle(0);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::UpdatePropSpecific(float afTimeStep)
{
	if(mfStuckSoundTimer >0) mfStuckSoundTimer-=afTimeStep; 

	UpdateAngle(afTimeStep);
	UpdateCheckLimit(afTimeStep);
	UpdateAutoRotation(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::BeforePropDestruction()
{
	
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Wheel::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return eLuxFocusCrosshair_Default;

	return eLuxFocusCrosshair_Grab;
}

//-----------------------------------------------------------------------

float cLuxProp_Wheel::OnInteractDebugDraw(cGuiSet *apSet,iFontData *apFont, float afStartY)
{
	apSet->DrawFont(apFont,cVector3f(5,afStartY,5),12,cColor(1,1),_W("WheelAngle: %f, Max: %f, Min: %f"),cMath::ToDeg(mfAngle),
						cMath::ToDeg(mfMaxLimit),cMath::ToDeg(mfMinLimit)); 
	afStartY += 13.0f;
	
	return afStartY;
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SetStuckState(int alState, bool abEffects)
{
	if(mlStuckState == alState) return;

	mlStuckState = alState;

	mfStuckSoundTimer =0; //Reset the stuck timer!

	//Max
    if(mlStuckState == 1)
	{
		mfAngle = mfMaxLimit - mfMaxLimitRange/2.0f;
		mfPrevAngle = mfAngle;

		float fAngle = mpHingeJoint->GetAngle();
		float fMax = cMath::Min(fAngle + cMath::ToRad(1),cMath::ToRad(180));
		float fMin = cMath::Max(fAngle - cMath::ToRad(1),cMath::ToRad(-180));

		mpHingeJoint->SetMaxAngle(fMax);
		mpHingeJoint->SetMinAngle(fMin);

		ChangeState(1, abEffects);
		ResetAutoMove();
	}
	//Min
	else if(mlStuckState == -1)
	{
		mfAngle = mfMinLimit + mfMinLimitRange/2.0f;
		mfPrevAngle = mfAngle;

		float fAngle = mpHingeJoint->GetAngle();
		float fMax = cMath::Min(fAngle + cMath::ToRad(1),cMath::ToRad(180));
		float fMin = cMath::Max(fAngle - cMath::ToRad(1),cMath::ToRad(-180));

		mpHingeJoint->SetMaxAngle(fMax);
		mpHingeJoint->SetMinAngle(fMin);

		ChangeState(-1, abEffects);
		ResetAutoMove();
	}
	//None
	else
	{
		mpHingeJoint->SetMinAngle(0);
		mpHingeJoint->SetMaxAngle(0);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SetAngle(float afX, bool abAutoMoveToAngle)
{
	if(afX == mfAngle) return;
	
	float fWantedAngle = afX;
	if(fWantedAngle < mfMinLimit) fWantedAngle = mfMinLimit;
	if(fWantedAngle > mfMaxLimit) fWantedAngle = mfMaxLimit;

	//If it is a request to move directly, then we can do proper equal
	if(abAutoMoveToAngle)
	{
		if(fWantedAngle == mfAngle) return;
	}
	//If not then just check if close enough
	else
	{
		if(cMath::Abs(fWantedAngle - mfAngle) < 0.0001f) return;
	}

	if(abAutoMoveToAngle)
	{
		mbAutoMoving = true;
		mfAutoMoveGoal = fWantedAngle;
		mRotatePid.Reset();
	}
	else
	{
		mfAngle = fWantedAngle;
		mfPrevAngle = mfAngle;
		ResetAutoMove();
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
    if(alState>0)	SetStuckState(-1,true);
	else			SetStuckState(0, true);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_Wheel::UpdateAngle(float afTimeStep)
{
	/////////////////////////////////////////
	//Update the Angle value
	float fLast = mfAngle;
	mfJointAngle = mpHingeJoint->GetAngle();
	float fAngleAdd = cMath::GetAngleDistanceRad(mfPrevJointAngle, mfJointAngle);

	mfPrevAngle = mfAngle;
	mfAngle += fAngleAdd;

	mfPrevJointAngle = mfJointAngle;

    /////////////////////////////////////
	//Check for movement and play stuck sound
    if(mlStuckState!=0 && mbIsInteractedWith)
	{
		if(mfStuckSoundTimer <= 0 && fabs(fAngleAdd) >0.01f * afTimeStep)
		{
			PlaySound("WheelStuck", msStuckSound, true, true);
			mfStuckSoundTimer = 1.5f;
		}
	}

	/////////////////////////////////////
	//Get value to used to set limit for spinning in one direction.
	float fSpinVal =0;
	if(mlSpinDir==1 && mfAngle > mfLastToMax)
	{
		mfLastToMax = mfAngle;
		fSpinVal = mpHingeJoint->GetAngle();
	}
	if(mlSpinDir==-1 && mfAngle < mfLastToMin){
		mfLastToMin = mfAngle;
		fSpinVal = mpHingeJoint->GetAngle();
	}


	//If no limits, skip the rest.
	if(mfMaxLimit ==0 && mfMinLimit==0) return;

	/////////////////////////////////////
	//Set Max and min limit
	if(mlStuckState==0)
	{
		if(mlSpinDir == 1)
		{
			SetMaxJointAngle(mfMaxLimit);	
			mpHingeJoint->SetMinAngle(fSpinVal - cMath::ToRad(0.5f));
		}
		else if(mlSpinDir == -1)
		{
			SetMinJointAngle(mfMinLimit);	
			mpHingeJoint->SetMaxAngle(fSpinVal + cMath::ToRad(0.5f));
		}
		else
		{
			SetMaxJointAngle(mfMaxLimit);	
			SetMinJointAngle(mfMinLimit);
		}
	}

	/////////////////////////////////////
	//Connections
	InteractConnectionTurn(mfAngle, mfPrevAngle,mfMinLimit, mfMaxLimit);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::UpdateCheckLimit(float afTimeStep)
{
	if(mlStuckState !=0) return;

	if(mfMaxLimit ==0 && mfMinLimit==0) return;
	
	///////////////////////
	//Max
	if(mfAngle > (mfMaxLimit - mfMaxLimitRange))
	{
		ChangeState(1, true);
		if(mbMaxLimitStuck) SetStuckState(1, false);
	}
	///////////////////////
	//Min
	else if(mfAngle < (mfMinLimit + mfMinLimitRange))
	{
		ChangeState(-1, true);
		if(mbMinLimitStuck) SetStuckState(-1, false);
	}
	///////////////////////
	//Middle
	else
	{
		ChangeState(0, true);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::UpdateAutoRotation(float afTimeStep)
{
	/////////////////////////////
	//Auto move to angle
	if(mbAutoMoving)
	{
		if(cMath::Abs(mfAngle-mfAutoMoveGoal) < cMath::ToRad(0.1f))
		{
			ResetAutoMove();
		}
		else
		{
			float fWantedSpeed = (mfAngle - mfAutoMoveGoal) * 10.0f  + (mfPrevAngle - mfAngle)*afTimeStep*1.0f;

			//Log("Angle: %f Wanted: %f Diff: %f Speed: %f\n", cMath::ToDeg(mfAngle), cMath::ToDeg(mfAutoMoveGoal),cMath::Abs(mfAngle-mfAutoMoveGoal), fWantedSpeed);
			
			cVector3f vRotateDir = mpHingeJoint->GetPinDir();
			cVector3f vBodyVel = mpWheelBody->GetAngularVelocity();
			cVector3f vHingeVel = vRotateDir * cMath::Vector3Dot(vRotateDir, vBodyVel);
			cVector3f vWantedVel = vRotateDir * fWantedSpeed;

			cVector3f vTorque = mRotatePid.Output(vWantedVel - vHingeVel, afTimeStep);
			vTorque = cMath::MatrixMul(mpWheelBody->GetInertiaMatrix(), vTorque);
			
			mpWheelBody->AddTorque(vTorque);
		}
	}
	
	//If there is an interaction, do not do any slow down
	if(IsInteractedWith())
	{
		return;
	}

	/////////////////////////////
	//Slowdown rotation to 0
	if(mbAutoMoving==false && mbSlowDownRotation)
	{
		cVector3f vBodyVel = mpWheelBody->GetAngularVelocity();
		cVector3f vWantedVel = 0;

		cVector3f vTorque = mRotatePid.Output(vWantedVel - vBodyVel, afTimeStep);
		vTorque = cMath::MatrixMul(mpWheelBody->GetInertiaMatrix(), vTorque);

		mpWheelBody->AddTorque(vTorque);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SetMaxJointAngle(float afMaxAngle)
{
	float fDistToMax = afMaxAngle - mfAngle;

	if(fDistToMax > kPif){//TOo far away, no need to set limit.
		mpHingeJoint->SetMaxAngle(k2Pif);
		return; 
	}

	float fJointMax = mfJointAngle + fDistToMax; //Get the value of the joint maximum.

	//The max value is in the 0 -> Pi half
	if(mfJointAngle >=0 && fJointMax <= kPif)
	{
		mpHingeJoint->SetMaxAngle(fJointMax);
	}
	//The max value is in the 0 -> -Pi half
	else if(mfJointAngle <0 && fJointMax<0)
	{
		mpHingeJoint->SetMaxAngle(fJointMax);
	}
	else
	{
		mpHingeJoint->SetMaxAngle(k2Pif);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SetMinJointAngle(float afMinAngle)
{
	float fDistToMax = mfAngle - afMinAngle;

	if(fDistToMax > kPif){//TOo far away, no need to set limit.
		mpHingeJoint->SetMinAngle(-k2Pif);
		return; 
	}

	float fJointMax = mfJointAngle - fDistToMax; //Get the value of the joint minimum

	//The max value is in the 0 -> Pi half
	if(mfJointAngle >=0 && fJointMax >= 0)
	{
		mpHingeJoint->SetMinAngle(fJointMax);
	}
	//The max value is in the 0 -> -Pi half
	else if(mfJointAngle <0 && fJointMax > -kPif)
	{
		mpHingeJoint->SetMinAngle(fJointMax);
	}
	else
	{
		mpHingeJoint->SetMinAngle(-k2Pif);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::ChangeState(int alState, bool abEffects)
{
	if(mlCurrentState == alState) return;
	if(mbSkipMiddleState && alState==0) return;

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

	///////////////////////
	//Play sound
	if(abEffects  && sSound != "")
	{
		cSoundEntity *pSound = mpMap->GetWorld()->CreateSoundEntity("WheelSound", sSound, true);
		if(pSound) pSound->SetPosition(mpHingeJoint->GetPivotPoint());
	}

	/////////////////
	//Connection
	InteractConnectionLimit(mlCurrentState);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::ResetAutoMove()
{
	mbAutoMoving = false;
	mRotatePid.Reset();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Wheel_SaveData, iLuxProp_SaveData)
kSerializeVar(mfAngle, eSerializeType_Float32)
kSerializeVar(mfPrevAngle, eSerializeType_Float32)

kSerializeVar(mfLastToMax, eSerializeType_Float32)
kSerializeVar(mfLastToMin, eSerializeType_Float32)

kSerializeVar(mlCurrentState, eSerializeType_Int32)
kSerializeVar(mlStuckState, eSerializeType_Int32)

kSerializeVar(mfJointAngle, eSerializeType_Float32)
kSerializeVar(mfPrevJointAngle, eSerializeType_Float32)

kSerializeVar(mbAutoMoving, eSerializeType_Bool)
kSerializeVar(mfAutoMoveGoal, eSerializeType_Float32)

kSerializeVar(mfMinLimit, eSerializeType_Float32)
kSerializeVar(mfMaxLimit, eSerializeType_Float32)
kSerializeVar(mbMinLimitStuck, eSerializeType_Bool)
kSerializeVar(mbMaxLimitStuck, eSerializeType_Bool)

kSerializeVar(mlSpinDir, eSerializeType_Int32)

kSerializeVar(mbInteractionDisablesStuck, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Wheel::CreateSaveData()
{
	return hplNew(cLuxProp_Wheel_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Wheel_SaveData *pData = static_cast<cLuxProp_Wheel_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mfAngle);
	kCopyToVar(pData,mfPrevAngle);

	kCopyToVar(pData,mfLastToMin);
	kCopyToVar(pData,mfLastToMax);
	
	kCopyToVar(pData,mlCurrentState);
	kCopyToVar(pData,mlStuckState);

	kCopyToVar(pData,mfJointAngle);
	kCopyToVar(pData,mfPrevJointAngle);

	kCopyToVar(pData,mbAutoMoving);
	kCopyToVar(pData,mfAutoMoveGoal);

	kCopyToVar(pData,mfMinLimit);
	kCopyToVar(pData,mfMaxLimit);
	kCopyToVar(pData,mbMinLimitStuck);
	kCopyToVar(pData,mbMaxLimitStuck);

	kCopyToVar(pData,mlSpinDir);

	kCopyToVar(pData,mbInteractionDisablesStuck);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Wheel_SaveData *pData = static_cast<cLuxProp_Wheel_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mfAngle);
	kCopyFromVar(pData,mfPrevAngle);

	kCopyFromVar(pData,mfLastToMin);
	kCopyFromVar(pData,mfLastToMax);

	kCopyFromVar(pData,mlCurrentState);

	kCopyFromVar(pData,mfJointAngle);
	kCopyFromVar(pData,mfPrevJointAngle);

	kCopyFromVar(pData,mbAutoMoving);
	kCopyFromVar(pData,mfAutoMoveGoal);

	kCopyFromVar(pData,mfMinLimit);
	kCopyFromVar(pData,mfMaxLimit);
	kCopyFromVar(pData,mbMinLimitStuck);
	kCopyFromVar(pData,mbMaxLimitStuck);

	kCopyFromVar(pData,mlSpinDir);
	kCopyFromVar(pData,mbInteractionDisablesStuck);

	SetStuckState(pData->mlStuckState, false);
}

//-----------------------------------------------------------------------

void cLuxProp_Wheel::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
