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

#include "LuxProp_Object.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxEnemy.h"
#include "LuxProp_Item.h"
#include "LuxHintHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Object::cLuxPropLoader_Object(const tString& asName) : iLuxPropLoader(asName)
{
	mfGrabDefaultMaxFocusDist = gpBase->mpGameCfg->GetFloat("Player_Interaction","Grab_DefaultMaxFocusDist",0);
	mfPushDefaultMaxFocusDist = gpBase->mpGameCfg->GetFloat("Player_Interaction","Push_DefaultMaxFocusDist",0);
	mfSlideDefaultMaxFocusDist = gpBase->mpGameCfg->GetFloat("Player_Interaction","Slide_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Object::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxProp_Object, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Object::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxProp_Object  *pObject = static_cast<cLuxProp_Object*>(apProp);

	///////////////////////////
	// General
	pObject->mObjectType = GetObjectType(msEntitySubType);

	pObject->mfLifeLength = GetVarFloat("LifeLength", 0);
	pObject->mbIsFood = GetVarBool("IsFood", false);

	//Breakable
	pObject->mBreakData.mbActive = GetVarBool("BreakActive", false);

	pObject->mBreakData.mbDestroyJoints = GetVarBool("BreakDestroyJoints", false);
	pObject->mBreakData.mfMinEnergy = GetVarFloat("BreakMinEnergy", 1000);
	pObject->mBreakData.msEntity = GetVarString("BreakEntity", "");
	pObject->mBreakData.msEntityAlignBody = GetVarString("BreakEntityAlignBody", "");
	pObject->mBreakData.msSound = GetVarString("BreakSound", "");
	pObject->mBreakData.msParticleSystem = GetVarString("BreakParticleSystem", "");
	pObject->mBreakData.mfImpulse = GetVarFloat("BreakImpulse", 3);

	pObject->mfHitDamageAmount = GetVarFloat("HitDamageAmount",5);
	pObject->mlHitDamageStrength = GetVarInt("HitDamageStrength",1);
	pObject->mfMinHitDamageSpeed = GetVarFloat("MinHitDamageSpeed",3.5f);

	///////////////////////////
	// Static specific
	if(pObject->mObjectType == eLuxObjectType_Static)
	{

	}
	///////////////////////////
	// Grab specific
	else if(pObject->mObjectType == eLuxObjectType_Grab)
	{
		pObject->mGrabData.mbGrabUseDepth = GetVarBool("GrabUseDepth", false);
		pObject->mGrabData.mfGrabDepth = GetVarFloat("GrabDepth", 1.5f);
		pObject->mGrabData.mfGrabDepthInc = GetVarFloat("GrabDepthInc", 0.1f);
		pObject->mGrabData.mfGrabMinDepth = GetVarFloat("GrabMinDepth", 1.0f);
		pObject->mGrabData.mfGrabMaxDepth = GetVarFloat("GrabMaxDepth", 2.0f);
		pObject->mGrabData.mvGrabPositionOffset = GetVarVector3f("GrabPositionOffset", 0.0f);
		pObject->mGrabData.mvGrabRotationOffset = cMath::Vector3ToRad(GetVarVector3f("GrabRotationOffset", 0.0f));
		pObject->mGrabData.mfGrabThrowImpulse = GetVarFloat("GrabThrowImpulse", 10.0f);
		pObject->mGrabData.mbGrabUseOffset = GetVarBool("GrabUseOffset", false);
		pObject->mGrabData.mfGrabMassMul = GetVarFloat("GrabMassMul", 0.1f);
		pObject->mGrabData.mfForceMul = GetVarFloat("GrabForceMul", 1);
		pObject->mGrabData.mfTorqueMul = GetVarFloat("GrabTorqueMul", 1);
		pObject->mGrabData.mbUseRotation = GetVarBool("GrabUseRotation", true);

		pObject->mbGrabSkipNonOuterBodies = GetVarBool("GrabSkipNonOuterBodies", false);


		if(pObject->mfMaxFocusDistance<=0) pObject->mfMaxFocusDistance = mfGrabDefaultMaxFocusDist;
	}
	///////////////////////////
	// Push specific
	else if(pObject->mObjectType == eLuxObjectType_Push)
	{
		pObject->mPushData.mbPushAtPoint = GetVarBool("PushAtPoint", true);
		pObject->mPushData.mfPushForceMul = GetVarFloat("PushForceMul", 1.0f);
		pObject->mPushData.mfPushImpulse = GetVarFloat("PushImpulse", 2.0f);

		if(pObject->mfMaxFocusDistance<=0) pObject->mfMaxFocusDistance = mfPushDefaultMaxFocusDist;
	}
	///////////////////////////
	// Slide specific
	else if(pObject->mObjectType == eLuxObjectType_Slide)
	{
		pObject->mSlideData.mfSlideMaxSpeed = GetVarFloat("SlideMaxSpeed", 5.0f);
		pObject->mSlideData.mfSlideSlowDownFactor = GetVarFloat("SlideSlowDownFactor", 1.0f);
		pObject->mSlideData.mfSlideSpeedFactor = GetVarFloat("SlideSpeedFactor", 1.0f);
		pObject->mSlideData.mfSlideThrowImpulse = GetVarFloat("SlideThrowImpulse", 3.0f);
		
		if(pObject->mfMaxFocusDistance<=0) pObject->mfMaxFocusDistance = mfSlideDefaultMaxFocusDist;
	}
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Object::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
	cLuxProp_Object  *pObject = static_cast<cLuxProp_Object*>(apProp);

	pObject->mbDisableBreakable = apInstanceVars->GetVarBool("DisableBreakable",false);

	pObject->mbIsInsanityVision = apInstanceVars->GetVarBool("IsInsanityVision",false);
	pObject->mfVisionMaxSanity = apInstanceVars->GetVarFloat("VisionMaxSanity",30);

	if(pObject->mbIsInsanityVision)
		pObject->SetInsanityVisionVisability(false);
	
	pObject->msContainedItem = apInstanceVars->GetVarString("ContainedItem","");
	if(pObject->msContainedItem == "None") pObject->msContainedItem= "";
}

//-----------------------------------------------------------------------

eLuxObjectType cLuxPropLoader_Object::GetObjectType(const tString& asName)
{
	tString sLowName = cString::ToLowerCase(asName);

	if(sLowName == "static")return eLuxObjectType_Static;
	if(sLowName == "grab")	return eLuxObjectType_Grab;
	if(sLowName == "push")	return eLuxObjectType_Push;
	if(sLowName == "slide")	return eLuxObjectType_Slide;

    Error("Object type '%s' in '%s' does not exist!\n", asName.c_str(), msFileName.c_str());	
	return eLuxObjectType_Grab;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// COLLISION CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Object_BodyCallback::cLuxProp_Object_BodyCallback(cLuxProp_Object *apObject)
{
	mpObject = apObject;
}

cLuxProp_Object_BodyCallback::~cLuxProp_Object_BodyCallback()
{

}

//-----------------------------------------------------------------------

bool cLuxProp_Object_BodyCallback::OnAABBCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody)
{
	return true;
}

//-----------------------------------------------------------------------

static inline cVector3f GetCorrectNormal(const cVector3f& avNormal, const cVector3f& avCollidePoint, const cVector3f& avBodyACenter)
{
	cVector3f vCenterToCollidePoint = avCollidePoint - avBodyACenter;
	
	//Make sure the normal faces the other body
	if(cMath::Vector3Dot(vCenterToCollidePoint,avNormal)>0)	
		return avNormal;
	else
		return avNormal * -1;
	
}

void cLuxProp_Object_BodyCallback::OnBodyCollide(iPhysicsBody *apBody, iPhysicsBody *apCollideBody, cPhysicsContactData* apContactData)
{
	//////////////////////////////////////
	// Check breakage
	if(mpObject->mBreakData.mbActive)
	{
	
		/////////////////////////////
		// Check energy and see if it breaks.
		float fTotalEnergy =0;
		iPhysicsBody* vBodies[2] = {apBody, apCollideBody};
		for(int i=0; i<2; ++i)
		{
			iPhysicsBody *pBody = vBodies[i];
			if(pBody->GetMass()==0) continue;

			cVector3f vBodyCenter = cMath::MatrixMul(pBody->GetLocalMatrix(),pBody->GetMassCentre());

			cVector3f vVelAtImpact = pBody->GetVelocityAtPosition(apContactData->mvContactPosition);
			cVector3f vTowardsImpactNormal = GetCorrectNormal(apContactData->mvContactNormal, apContactData->mvContactPosition, vBodyCenter);
			cVector3f vVelTowardsImpact = vTowardsImpactNormal * cMath::Vector3Dot(vTowardsImpactNormal, vVelAtImpact);

			fTotalEnergy += vVelTowardsImpact.Length() * pBody->GetMass();
		}

		if(fTotalEnergy > mpObject->mBreakData.mfMinEnergy)
		{
			mpObject->Break();
		}
	}
	
	
	/////////////////////////////
	// Prop stuff
	// This is not needed since the code above does all breaking needs!
	/*iLuxEntity *pEntity = (iLuxEntity*)apCollideBody->GetUserData();
	if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
		
		if(pProp->GetPropType() != eLuxPropType_Critter) return;

		float fSpeed = apBody->GetLinearVelocity().Length();
		if(fSpeed >= mpObject->mfMinHitDamageSpeed)
		{
			pProp->GiveDamage(mpObject->mfHitDamageAmount, mpObject->mlHitDamageStrength);			
		}	
	}*/

	/////////////////////////////
	// Check character and give damage
	if(apCollideBody->IsCharacter())
	{
		float fSpeed = apBody->GetLinearVelocity().Length();
		if(fSpeed >= mpObject->mfMinHitDamageSpeed)
		{
            /*if(apCollideBody == gpBase->mpPlayer->GetCharacterBody()->GetCurrentBody())
			{
				gpBase->mpPlayer->GiveDamage(mpObject->mfHitDamageAmount, mpObject->mlHitDamageStrength,eLuxDamageType_BloodSplat);
			}
			else*/
			if(apCollideBody->GetCharacterBody()->GetUserData())
			{
				iLuxEnemy* pEnemy = (iLuxEnemy*)apCollideBody->GetCharacterBody()->GetUserData();
				pEnemy->GiveDamage(mpObject->mfHitDamageAmount, mpObject->mlHitDamageStrength);
			}
		}
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxProp_Object::cLuxProp_Object(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_Object)
{
	mbBroken = false;
	mfLifeLengthCount =0;
	mlStuckState =0;

	mfFoodAttractCount =0;
	mfInsanityVisionCount =0;
	mbInsanityVisionActive = false;

	mbIsInsanityVision = false;
	mfVisionMaxSanity = 0.0f;

	mpBodyCallback = hplNew(cLuxProp_Object_BodyCallback, (this) );
}

//-----------------------------------------------------------------------

cLuxProp_Object::~cLuxProp_Object()
{
	hplDelete(mpBodyCallback);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

bool cLuxProp_Object::CanInteract(iPhysicsBody *apBody)
{
	if(mObjectType == eLuxObjectType_Grab && mbGrabSkipNonOuterBodies && apBody->GetJointNum()>=2)
	{
		return false;
	}

	if(	(apBody->GetMass()==0 && mpMap->BodyIsInDetachableStickyArea(apBody)==false  && msInteractCallback=="") ||
		(mObjectType == eLuxObjectType_Static && msInteractCallback=="") ) 
	{
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------

bool cLuxProp_Object::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	iPhysicsBody *pBody = mpMainBody ? mpMainBody : apBody;

	mpMap->DetachBodyFromStickyArea(apBody);

	////////////////////////
	// Grab
    if(mObjectType == eLuxObjectType_Grab)
	{
		gpBase->mpHintHandler->Add("EntityGrab", kTranslate("Hints", "EntityGrab01"), 0);
		gpBase->mpHintHandler->Add("EntityGrab", kTranslate("Hints", "EntityGrab02"), 0);

		cLuxPlayerStateVars::SetupInteraction(pBody, avPos);
		gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractGrab);
	}
	////////////////////////
	// Push
	if(mObjectType == eLuxObjectType_Push)
	{
		gpBase->mpHintHandler->Add("EntityPush", kTranslate("Hints", "EntityPush"), 0);

		cLuxPlayerStateVars::SetupInteraction(pBody, avPos);
		gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractPush);
	}
	////////////////////////
	// Slide
	if(mObjectType == eLuxObjectType_Slide)
	{
		gpBase->mpHintHandler->Add("EntitySlide", kTranslate("Hints", "EntitySlide"), 0);

		cLuxPlayerStateVars::SetupInteraction(pBody, avPos);
		gpBase->mpPlayer->ChangeState(eLuxPlayerState_InteractSlide);
	}
	
	return true;
}

//-----------------------------------------------------------------------

void cLuxProp_Object::OnSetupAfterLoad(cWorld *apWorld)
{
	/////////////////////
	// Preloading
	cResources *pResources = gpBase->mpEngine->GetResources();
	if(mBreakData.msEntity != "") 
	{
		PreloadEntityModel(mBreakData.msEntity);
	}
	if(mBreakData.msParticleSystem != "")
	{
		gpBase->PreloadParticleSystem(mBreakData.msParticleSystem);	
	}
	if(mBreakData.msSound != "")
	{
		gpBase->PreloadSound(mBreakData.msSound);
	}

	/////////////////////
	// Setup bodies
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		
        pBody->AddBodyCallback(mpBodyCallback);
	}

	/////////////////////
	// Setup joints,
	mvJointData.resize(mvJoints.size());
	for(size_t i=0; i< mvJoints.size(); ++i)
	{
		iPhysicsJoint *pJoint = mvJoints[i];
		cLuxProp_Object_JointData *pJointData = &mvJointData[i];

        if(pJoint->GetType() == ePhysicsJointType_Hinge)
		{
			iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
			
			pJointData->mfMinLimit = pHingeJoint->GetMinAngle();
			pJointData->mfMaxLimit = pHingeJoint->GetMaxAngle();
			pJointData->mfLockedRange = cMath::ToRad(5);
		}
		else if(pJoint->GetType() == ePhysicsJointType_Slider)
		{
			iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);

			pJointData->mfMinLimit = pSliderJoint->GetMinDistance();
			pJointData->mfMaxLimit = pSliderJoint->GetMaxDistance();
			pJointData->mfLockedRange = cMath::ToRad(0.1f);
		}
		else if(pJoint->GetType() == ePhysicsJointType_Screw)
		{
			iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);

			pJointData->mfMinLimit = pScrewJoint->GetMinDistance();
			pJointData->mfMaxLimit = pScrewJoint->GetMaxDistance();
			pJointData->mfLockedRange = cMath::ToRad(0.1f);
		}
		
	}
}



//-----------------------------------------------------------------------

void cLuxProp_Object::OnResetProperties()
{

}
//-----------------------------------------------------------------------

void cLuxProp_Object::UpdatePropSpecific(float afTimeStep)
{
	//////////////////////////
	// Food attraction
	UpdateFoodEnemyAttraction(afTimeStep);

	////////////////////////////
	// Insanity vision
	UpdateInsanityVision(afTimeStep);

	//////////////////////////
	// Life length
	if(mfLifeLength > 0)
	{
		mfLifeLengthCount += afTimeStep;
		
		float fDist = cMath::Max(mfLifeLength - mfLifeLengthCount,0.0f);
		if(fDist < 1.0f)
		{
			mpMeshEntity->SetCoverageAmount(fDist);
		}

		if(mfLifeLengthCount >= mfLifeLength)
		{
			Break();
		}
	}

}

//-----------------------------------------------------------------------

void cLuxProp_Object::BeforePropDestruction()
{
	//////////////////////////////
	// Check if break should happen and init stuff
	if(mbBroken == false || mBreakData.mbActive==false || mvBodies.empty() || mbDisableBreakable)
	{
		return;
	}

	cWorld *pWorld = mpMap->GetWorld();
	cMatrixf mtxCenterTransform = cMatrixf::Identity;

	////////////////////////////////
	// Get the body to use as base for postion
	int lIdx = 0;
	if(mBreakData.msEntityAlignBody != "")
	{
		lIdx = GetBodyIndexFromName(mBreakData.msEntityAlignBody);
		if(lIdx < 0){
			lIdx = 0;
			Warning("Body '%s' was not found in object '%s'", mBreakData.msEntityAlignBody.c_str(), msName.c_str());
		}
	}
	iPhysicsBody *pBaseBody = mvBodies[lIdx];
	cEntityBodyExtraData* pBodyData = &mvBodyExtraData[lIdx];
	mtxCenterTransform = pBaseBody->GetLocalMatrix();

	///////////////////////
	// Destroy joints
	if(mBreakData.mbDestroyJoints)
	{
		//////////////////////
		//Iterate and destroy joints
		for(size_t i=0; i<mvJoints.size(); ++i)
		{
			mvJoints[i]->Break();
			mvJoints[i] = NULL;
		}

		//////////////////////
		//Get center of all bodies
		cVector3f vBodyCenter = 0;
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			vBodyCenter += mvBodies[i]->GetLocalPosition();
		}
		vBodyCenter = vBodyCenter / (float)mvBodies.size();
		mtxCenterTransform = cMath::MatrixTranslate(vBodyCenter);

		//////////////////////
		//Add impulse
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
			
			cVector3f vBodyCenter = cMath::MatrixMul(pBody->GetLocalMatrix(), pBody->GetMassCentre());

			cVector3f vImpulseDir = cMath::Vector3Normalize(vBodyCenter - vBodyCenter);

			pBody->AddImpulse(vImpulseDir*mBreakData.mfImpulse);
		}
	}
	///////////////////////
	// Create Entity
	else 
	{
		

		////////////////////////////
		// Create the entity
		if(mBreakData.msEntity != "")
		{
			// Multiply by inverse local to get the position to center the entity on.
			cMatrixf mtxInvLocalBody = cMath::MatrixInverse(pBodyData->m_mtxLocalTransform);
			cMatrixf mtxEntity = cMath::MatrixMul(pBaseBody->GetLocalMatrix(), mtxInvLocalBody);

			mpMap->ResetLatestEntity();
			mpMap->CreateEntity(msName + "_broken", mBreakData.msEntity, mtxEntity, mvOnLoadScale);		
			
			////////////////////////
			//Add impulse and velocity
			iLuxEntity *pEntity = mpMap->GetLatestEntity();
			if(pEntity && mpMap->EntityExists(pEntity) && pEntity->GetEntityType() == eLuxEntityType_Prop)
			{
				//If the parent has full game save, so must the broken version!
				if(mbFullGameSave)
				{
					pEntity->SetFullGameSave(true);
				}

				iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
				
				for(int i=0; i< pProp->GetBodyNum(); ++i)
				{
					iPhysicsBody *pNewBody = pProp->GetBody(i);
					cVector3f vNewBodyCenter = cMath::MatrixMul(pNewBody->GetLocalMatrix(), pNewBody->GetMassCentre());

					cVector3f vImpulseDir = cMath::Vector3Normalize(vNewBodyCenter - mtxEntity.GetTranslation());

					pNewBody->AddImpulse(vImpulseDir*mBreakData.mfImpulse + pBaseBody->GetLinearVelocity());
				}
			}
		}
	}

	///////////////////////
	// Enable and move connected props
	for(size_t i=0; i<mvConnectedProps.size(); ++i)
	{
		tString sProp = mvConnectedProps[i];
		
		//////////////////////
		//Get Prop
        iLuxEntity *pEntity = mpMap->GetEntityByName(sProp, eLuxEntityType_Prop);
		if(pEntity==NULL)
		{
			Error("Could not find prop '%s' which is connected to '%s'\n", sProp.c_str(), msName.c_str());
			continue;
		}
		iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);

		///////////////////////
		// Enable prop
		pProp->SetActive(true);

		//////////////////////
		//Move all bodies in pro
		for(int i=0; i< pProp->GetBodyNum(); ++i)
		{
			iPhysicsBody *pBody = pProp->GetBody(i);
			cVector3f vNewBodyCenter = cMath::MatrixMul(pBody->GetLocalMatrix(), pBody->GetMassCentre());

			cVector3f vImpulseDir = cMath::Vector3Normalize(vNewBodyCenter - mtxCenterTransform.GetTranslation());

			pBody->AddImpulse(vImpulseDir*mBreakData.mfImpulse + pBaseBody->GetLinearVelocity());
		}
	}

	///////////////////////
	// Create contained item
	if(msContainedItem != "")
	{
		tString sFile = "items/"+ msContainedItem;
		mpMap->ResetLatestEntity();
		mpMap->CreateEntity(msName + "_item", msContainedItem, mtxCenterTransform,1);

		iLuxEntity *pEntity = mpMap->GetLatestEntity();
		if(pEntity && pEntity->GetEntityType() == eLuxEntityType_Prop)
		{
			iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
			if(pProp->GetPropType() == eLuxPropType_Item)
			{
				cLuxProp_Item *pItem = static_cast<cLuxProp_Item*>(pProp);
				pItem->SetSpawnContainerID(GetID());
			}
		}
	}

	///////////////////////
	// Create Sound
	if(mBreakData.msSound != "")
	{
		cSoundEntity *pSound = pWorld->CreateSoundEntity(msName + "_BreakSound", mBreakData.msSound, true);
		if(pSound) pSound->SetPosition(mtxCenterTransform.GetTranslation());
	}

	///////////////////////
	// Create Particle System
	if(mBreakData.msParticleSystem != "")
	{
		cParticleSystem *pPS = pWorld->CreateParticleSystem(msName + "_BreakPS", mBreakData.msParticleSystem,1);
		if(pPS) pPS->SetMatrix(mtxCenterTransform);
	}


	///////////////////////
	// Callback
	RunCallbackFunc("Break");

}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_Object::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(CanInteract(apBody)==false) return eLuxFocusCrosshair_Default;

    
	if(mObjectType == eLuxObjectType_Static)
	{
		return mCustomFocusCrossHair==eLuxFocusCrosshair_Default ? eLuxFocusCrosshair_Grab : mCustomFocusCrossHair;
	}
	else if(mObjectType == eLuxObjectType_Grab)
	{
		return mCustomFocusCrossHair==eLuxFocusCrosshair_Default ? eLuxFocusCrosshair_Grab : mCustomFocusCrossHair;
	}
	else if(mObjectType == eLuxObjectType_Push)
	{
		return mCustomFocusCrossHair==eLuxFocusCrosshair_Default ? eLuxFocusCrosshair_Push : mCustomFocusCrossHair;
	}
	else if(mObjectType == eLuxObjectType_Slide)
	{
		return mCustomFocusCrossHair==eLuxFocusCrosshair_Default ? eLuxFocusCrosshair_Grab : mCustomFocusCrossHair;
	}
    
	return eLuxFocusCrosshair_LastEnum;
}

//-----------------------------------------------------------------------

void  cLuxProp_Object::SetStuckState(int alState)
{
	if(mlStuckState == alState) return;
	mlStuckState = alState;

	for(size_t i=0; i< mvJoints.size(); ++i)
	{
		iPhysicsJoint *pJoint = mvJoints[i];
		cLuxProp_Object_JointData *pJointData = &mvJointData[i];

		if(mlStuckState ==0)
		{
			SetJointMinMax((int)i, pJointData->mfMinLimit, pJointData->mfMaxLimit);
		}
		else if(mlStuckState ==1)
		{
			SetJointMinMax((int)i, pJointData->mfMaxLimit - pJointData->mfLockedRange, pJointData->mfMaxLimit);
		}
		else if(mlStuckState ==-1)
		{
			SetJointMinMax((int)i, pJointData->mfMinLimit, pJointData->mfMinLimit + pJointData->mfLockedRange);
		}

		//If sleeping, make sure it moves!
		if(pJoint->GetChildBody())
			pJoint->GetChildBody()->AddForce(cVector3f(1,1,1));
		
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Object::Break()
{
	mbBroken = true;
	mpMap->DestroyEntity(this);
}

//-----------------------------------------------------------------------

void cLuxProp_Object::OnHealthChange()
{
	if(mfHealth <= 0 && mBreakData.mbActive)
	{
		Break();
	}
}

//-----------------------------------------------------------------------


void cLuxProp_Object::OnDamage(float afAmount, int alStrength)
{
	
}

//-----------------------------------------------------------------------

bool cLuxProp_Object::ShowOutlinesOnConnectedBodies()
{
	if(mObjectType == eLuxObjectType_Grab) return true;

    return false;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxProp_Object::UpdateFoodEnemyAttraction(float afTimeStep)
{
	if(mbIsFood==false) return;
	if(mfFoodAttractCount > 0)
	{
		mfFoodAttractCount -= afTimeStep;
		return;
	}

	mfFoodAttractCount = 2.0f;

	///////////////////////////////////////
	// Iterate enemies and see if any is close to be attracted
	cLuxEnemyIterator enemyIt = mpMap->GetEnemyIterator();
	while(enemyIt.HasNext())
	{
		iLuxEnemy *pEnemy = enemyIt.Next();
		if(pEnemy->IsActive()==false || pEnemy->GetHealth() <= 0) continue;

        if(pEnemy->InRangeOfFood(mvBodies[0])==false) continue;

		pEnemy->SendMessage(eLuxEnemyMessage_FoodInRange, 0, false, mvBodies[0]->GetLocalPosition());
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Object::UpdateInsanityVision(float afTimeStep)
{
	if(mbIsInsanityVision==false) return;
	if(mpMeshEntity==NULL) return;
	
	//////////////////////////////////
	//Check if the object should be disabled or enabled
	float fSanity = gpBase->mpPlayer->GetSanity();
	if( (fSanity <= mfVisionMaxSanity && mbInsanityVisionActive) ||
		(fSanity > mfVisionMaxSanity && mbInsanityVisionActive==false) )
	{
		return;
	}

	//////////////////////////////////
    //Update check count
	if(mfInsanityVisionCount > 0)
	{
		mfInsanityVisionCount -= afTimeStep;
		return;
	}
	mfInsanityVisionCount = 2.0f;

	//////////////////////////////////
	//Init variables
	cCamera *pCam = gpBase->mpPlayer->GetCamera();
	bool bInsideFOV = false;
	
	///////////////////////////////
	// Check so not interacted with
    if(IsInteractedWith())
	{
		bInsideFOV = true;
	}

	///////////////////////////////
	// Iterate submeshes and see if any is in player FOV
	if(bInsideFOV==false)
	{
		for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
		{	
			cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);

			if(pCam->GetFrustum()->CollideBoundingVolume(pSubEnt->GetBoundingVolume())!=eCollision_Outside)
			{
				bInsideFOV = true;
				break;
			}
		}
	}

	//////////////////////////////////
	//Change visibility
	if(bInsideFOV==false)
	{
		SetInsanityVisionVisability(!mbInsanityVisionActive);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Object::SetInsanityVisionVisability(bool abX)
{
	mbInsanityVisionActive = abX;

	///////////////
	//Set Bodies and mesh visble
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		mvBodies[i]->SetActive(abX);
	}

	///////////////////
	//Mesh entity
	if(mpMeshEntity && mbShowMesh)
	{
		mpMeshEntity->SetActive(abX);
		mpMeshEntity->SetVisible(abX);
	}
}

//-----------------------------------------------------------------------

void cLuxProp_Object::SetJointMinMax(int alIdx, float afMin, float afMax)
{
	iPhysicsJoint *pJoint = mvJoints[alIdx];
	cLuxProp_Object_JointData *pJointData = &mvJointData[alIdx];

	if(pJoint->GetType() == ePhysicsJointType_Hinge)
	{
		iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);

		pHingeJoint->SetMinAngle(afMin);
		pHingeJoint->SetMaxAngle(afMin);
	}
	else if(pJoint->GetType() == ePhysicsJointType_Slider)
	{
		iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);

		pSliderJoint->SetMinDistance(afMin);
		pSliderJoint->SetMaxDistance(afMax);
	}
	else if(pJoint->GetType() == ePhysicsJointType_Screw)
	{
		iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);

		pScrewJoint->SetMinDistance(afMin);
		pScrewJoint->SetMaxDistance(afMax);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_Object_SaveData, iLuxProp_SaveData)
kSerializeVar(mfLifeLengthCount, eSerializeType_Float32)
kSerializeVar(msContainedItem, eSerializeType_String)
kSerializeVar(mlStuckState, eSerializeType_Int32)
kSerializeVar(mfFoodAttractCount, eSerializeType_Float32)
kSerializeVar(mbDisableBreakable, eSerializeType_Bool)
kSerializeVar(mbIsInsanityVision, eSerializeType_Bool)
kSerializeVar(mfVisionMaxSanity, eSerializeType_Float32)
kSerializeVar(mbInsanityVisionActive, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_Object::CreateSaveData()
{
	return hplNew(cLuxProp_Object_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_Object::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxProp_Object_SaveData *pData = static_cast<cLuxProp_Object_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData, mfLifeLengthCount);
	kCopyToVar(pData, msContainedItem);
	kCopyToVar(pData, mlStuckState);
	kCopyToVar(pData, mfFoodAttractCount);
	kCopyToVar(pData, mbDisableBreakable);
	kCopyToVar(pData, mbIsInsanityVision);
	kCopyToVar(pData, mfVisionMaxSanity);
	kCopyToVar(pData, mbInsanityVisionActive);

}

//-----------------------------------------------------------------------

void cLuxProp_Object::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxProp_Object_SaveData *pData = static_cast<cLuxProp_Object_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	kCopyFromVar(pData, mfLifeLengthCount);
	kCopyFromVar(pData, msContainedItem);
	SetStuckState(pData->mlStuckState);
	kCopyFromVar(pData, mfFoodAttractCount);
	kCopyFromVar(pData, mbDisableBreakable);
	kCopyFromVar(pData, mbIsInsanityVision);
	kCopyFromVar(pData, mfVisionMaxSanity);
	kCopyFromVar(pData, mbInsanityVisionActive);
}

//-----------------------------------------------------------------------

void cLuxProp_Object::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
