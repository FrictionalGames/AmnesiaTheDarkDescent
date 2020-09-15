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

#include "LuxProp_MultiSlider.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxHintHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_MultiSlider::cLuxPropLoader_MultiSlider(const tString& asName) : iLuxPropLoader(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","MultiSlider_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_MultiSlider::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_MultiSlider, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_MultiSlider::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_MultiSlider  *pMultiSlider = static_cast<cLuxProp_MultiSlider*>(apProp);

	///////////////////////////
	// General
	pMultiSlider->mSlideData.mfSlideMaxSpeed = GetVarFloat("SlideMaxSpeed", 5.0f);
	pMultiSlider->mSlideData.mfSlideSlowDownFactor = GetVarFloat("SlideSlowDownFactor", 1.0f);
	pMultiSlider->mSlideData.mfSlideSpeedFactor = GetVarFloat("SlideSpeedFactor", 1.0f);
	pMultiSlider->mSlideData.mfSlideThrowImpulse = GetVarFloat("SlideThrowImpulse", 3.0f);

	pMultiSlider->mlNumOfStates = GetVarInt("NumOfStates", 2);
	if(pMultiSlider->mlNumOfStates < 2)
	{
		Warning("MultiSlider '%s' has fewer than 2 states! It will not function properly!\n", pMultiSlider->GetName().c_str());
	}
	pMultiSlider->mfStickToStateMaxDist = GetVarFloat("StickToStateMaxDist", 0.1f);

	pMultiSlider->mbCanInteractWithStaticBody = GetVarBool("CanInteractWithStaticBody", true);
	
	pMultiSlider->mbAutoMoveToCurrentState = GetVarBool("AutoMoveToCurrentState", true);
	pMultiSlider->mfAutoMoveSpeedFactor = GetVarFloat("AutoMoveSpeedFactor", 2);
	pMultiSlider->mfAutoMoveMaxSpeed = GetVarFloat("AutoMoveMaxSpeed", 8);
	
	pMultiSlider->msChangeStateSound = GetVarString("ChangeStateSound", "");
	
	pMultiSlider->msStuckSound = GetVarString("StuckSound", "");
}
//-----------------------------------------------------------------------

void cLuxPropLoader_MultiSlider::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_MultiSlider  *pMultiSlider = static_cast<cLuxProp_MultiSlider*>(apProp);

	int lStuckState = apInstanceVars->GetVarInt("StuckState", -1);
	pMultiSlider->SetStuckState(lStuckState, false);

	pMultiSlider->mbInteractionDisablesStuck = apInstanceVars->GetVarBool("InteractionDisablesStuck", false);

	pMultiSlider->msChangeStateCallback = apInstanceVars->GetVarString("ChangeStateCallback", "");
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_MultiSlider::cLuxProp_MultiSlider(const tString &asName, int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_MultiSlider)
{
	mlCurrentState  = -1;
	mlStuckState = -1;
	mbInteractionDisablesStuck = false;

	mAutoMovePid.SetErrorNum(10);
	mAutoMovePid.p = 40.0f;
	mAutoMovePid.i = 0.0f;
	mAutoMovePid.d = 0.0f;//Derative fucks up big time.. so skip!

	mfStuckSoundTimer =0;
}

//-----------------------------------------------------------------------

cLuxProp_MultiSlider::~cLuxProp_MultiSlider()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_MultiSlider::CanInteract(iPhysicsBody *apBody)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return false;

	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_MultiSlider::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
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
		SetStuckState(-1, true);

	gpBase->mpHintHandler->Add("EntitySlide", kTranslate("Hints", "EntitySlide"), 0);

	cLuxPlayerStateVars::SetupInteraction(apBody, avPos);
	gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractSlide);

	mAutoMovePid.Reset();
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::OnSetupAfterLoad(cWorld *apWorld)
{
	iPhysicsJoint *pJoint = mvJoints[0];
	if(pJoint->GetType() != ePhysicsJointType_Slider)
	{
		Error("MultiSlider %s does not have a hinge joint!\n", msName.c_str());
		return;
	}

	mpSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);

	mpSliderBody = mpSliderJoint->GetChildBody();

	///////////////////////
	//Set up data
	mfDefaultMinDist = mpSliderJoint->GetMinDistance();
	mfDefaultMaxDist = mpSliderJoint->GetMaxDistance();

	///////////////////////
	//Set up limits
	mvStates.resize(mlNumOfStates);
	float fTotalDist = mfDefaultMaxDist - mfDefaultMinDist;
	float fStatePosAdd = fTotalDist / (float)(mlNumOfStates-1);
	float fStatePos = mfDefaultMinDist;
	for(size_t i=0; i<mvStates.size(); ++i)
	{
		cLuxProp_MultiSlider_State *pState = &mvStates[i];
		pState->mfPos = fStatePos;
		fStatePos += fStatePosAdd;
	}
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::OnResetProperties()
{

}
//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::UpdatePropSpecific(float afTimeStep)
{
	float fDist = mpSliderJoint->GetDistance();

	UpdateCheckStuckSound(afTimeStep);
	UpdateCheckNewState(fDist, afTimeStep);
	UpdateAutoMove(fDist, afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::BeforePropDestruction()
{
	
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_MultiSlider::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(apBody->GetMass()==0 && mbCanInteractWithStaticBody==false) return eLuxFocusCrosshair_Default;

	return eLuxFocusCrosshair_Grab;
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::SetStuckState(int alState, bool abEffects)
{
	if(mlStuckState == alState) return;

	mlStuckState = alState;

	mfStuckSoundTimer =0; //Reset the stuck timer!

	//////////////////////////
	//Stuck on
    if(mlStuckState >= 0)
	{
		ChangeState(alState, abEffects);

		float fPos = mvStates[mlStuckState].mfPos;
		float fMin = mlStuckState==0 ? fPos : fPos - 0.05f;
		float fMax = mlStuckState==(int)mvStates.size()-1 ? fPos : fPos + 0.05f;
		
		mpSliderJoint->SetMinDistance(fMin);
		mpSliderJoint->SetMaxDistance(fMax);
	}
	//////////////////////////
	//Stuck off
	else
	{
		mpSliderJoint->SetMinDistance(mfDefaultMinDist);
		mpSliderJoint->SetMaxDistance(mfDefaultMaxDist);
	}

	mpSliderBody->Enable();
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
	//Since there are multiple states, it does not really work with connections.
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::UpdateCheckStuckSound(float afTimeStep)
{
	if(mlStuckState <0 || mbIsInteractedWith==false) return;

	if(mfStuckSoundTimer >0)
	{
		mfStuckSoundTimer-=afTimeStep; 
		return;
	}
	
	float fSpeedSqr = mpSliderJoint->GetChildBody()->GetLinearVelocity().SqrLength();
	if(fSpeedSqr > 0.01f)
	{
		PlaySound("MultiSliderStuck", msStuckSound, true, true);
		mfStuckSoundTimer = 1.5f;
	}
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::UpdateCheckNewState(float afPos, float afTimeStep)
{
	if(mlStuckState >=0) return;

	for(size_t i=0; i<mvStates.size(); ++i)
	{
		cLuxProp_MultiSlider_State *pState = &mvStates[i];
		if(i==mlCurrentState) continue;

        float fDistToState = cMath::Abs(afPos-pState->mfPos);
		if(fDistToState <= mfStickToStateMaxDist)
		{
			//TODO: Move lever to state ??
			ChangeState(i, true);
			break;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::UpdateAutoMove(float afPos, float afTimeStep)
{
	if(IsInteractedWith() || mbAutoMoveToCurrentState==false || mlStuckState!=-1 || mlCurrentState==-1) return;
	
	///////////////////////////
	// Get the wanted speed
    float fGoalPos = mvStates[mlCurrentState].mfPos;
	
	float fWantedSpeed = mfAutoMoveSpeedFactor * (fGoalPos - afPos);
	if(cMath::Abs(fWantedSpeed)<0.003f) return;
	
	if(fWantedSpeed > mfAutoMoveMaxSpeed)	fWantedSpeed = mfAutoMoveMaxSpeed;
	if(fWantedSpeed < -mfAutoMoveMaxSpeed)	fWantedSpeed = -mfAutoMoveMaxSpeed;

	///////////////////////////
	// Calculate the torque
	cVector3f vMoveDir = mpSliderJoint->GetPinDir();
	cVector3f vBodyVel = mpSliderBody->GetLinearVelocity();
	float fSliderSpeed = cMath::Vector3Dot(vMoveDir, vBodyVel);
	
	//mpMultiSliderBody->SetAngularVelocity(vWantedVel);
	
	float fForceSize = mAutoMovePid.Output(fWantedSpeed - fSliderSpeed, afTimeStep);
	fForceSize *= mpSliderBody->GetMass();

	//Log("Pos: %f Goal: %f Wanted: %f current: %f Force: %f\n", afPos,fGoalPos, fWantedSpeed, fSliderSpeed, fForceSize);
	
	mpSliderBody->AddForce(vMoveDir*fForceSize);
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::ChangeState(int alState, bool abEffects)
{
	if(mlCurrentState == alState) return;

	mlCurrentState = alState;

	///////////////////////
	//Play sound
	if(abEffects  && msChangeStateSound != "")
	{
		cSoundEntity *pSound = mpMap->GetWorld()->CreateSoundEntity("MultiSliderSound", msChangeStateSound, true);
		if(pSound) pSound->SetPosition(mpSliderBody->GetLocalPosition());
	}

	///////////////////////
	//Callback
	if(msChangeStateCallback!="")
	{
		mpMap->RunScript(msChangeStateCallback + "(\""+msName+"\", "+cString::ToString(mlCurrentState)+")");
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_MultiSlider_SaveData, iLuxProp_SaveData)
kSerializeVar(mlCurrentState, eSerializeType_Int32)
kSerializeVar(mlStuckState, eSerializeType_Int32)
kSerializeVar(mbInteractionDisablesStuck, eSerializeType_Bool)
kSerializeVar(msChangeStateCallback, eSerializeType_String)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_MultiSlider::CreateSaveData()
{
	return hplNew(cLuxProp_MultiSlider_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_MultiSlider_SaveData *pData = static_cast<cLuxProp_MultiSlider_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mlCurrentState);
	kCopyToVar(pData,mlStuckState);
	kCopyToVar(pData,mbInteractionDisablesStuck);
	kCopyToVar(pData,msChangeStateCallback);
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_MultiSlider_SaveData *pData = static_cast<cLuxProp_MultiSlider_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData,mlCurrentState);
	SetStuckState(pData->mlStuckState, false);
	kCopyFromVar(pData,mbInteractionDisablesStuck);
	kCopyFromVar(pData,msChangeStateCallback);
}

//-----------------------------------------------------------------------

void cLuxProp_MultiSlider::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{	
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
