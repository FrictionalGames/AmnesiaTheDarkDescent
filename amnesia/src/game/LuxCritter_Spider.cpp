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

#include "LuxCritter_Spider.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxPlayerState.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxDebugHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_Critter_Spider::cLuxPropLoader_Critter_Spider(const tString& asName) : iLuxPropLoader_Critter(asName)
{
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_Critter_Spider::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxCritter_Spider, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Critter_Spider::LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
	cLuxCritter_Spider  *pCritter_Spider = static_cast<cLuxCritter_Spider*>(apProp);

	///////////////////////////
	// General
	pCritter_Spider->mfMaxIdleSpeed = GetVarFloat("MaxIdleSpeed", 0);
	pCritter_Spider->mfMaxHuntSpeed = GetVarFloat("MaxHuntSpeed", 0);

	pCritter_Spider->msHitSound = GetVarString("HitSound", "");
	pCritter_Spider->msIdleSound = GetVarString("IdleSound", "");
	pCritter_Spider->mvIdleSoundRandMinMax  = GetVarVector2f("IdleSoundRandMinMax", 0);
	pCritter_Spider->msAttackSound  = GetVarString("AttackSound", "");

	pCritter_Spider->mvAttackPlayerDistMinMax = GetVarVector2f("AttackPlayerDistMinMax", 0);
	pCritter_Spider->mfHuntPlayerDist = GetVarFloat("HuntPlayerDist", 0);
	pCritter_Spider->mvHuntTimeMinMax = GetVarVector2f("HuntTimeMinMax", 0);
	pCritter_Spider->mvMoveTimeMinMax = GetVarVector2f("MoveTimeMinMax", 0);
	pCritter_Spider->mvIdleTimeMinMax = GetVarVector2f("IdleTimeMinMax", 0);
}

//-----------------------------------------------------------------------

void cLuxPropLoader_Critter_Spider::LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxCritter_Spider::cLuxCritter_Spider(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp_CritterBase(asName,alID,apMap)
{
	mbUseRayCollision = false;
	mfFallingCount =0;
	mfNewGroundNormalCount =0;
	mfMaxSpeed =0.5;
	mState = eLuxCritterState_Idle;
	mfCount =0;
	mfPlaySoundCount = cMath::RandRectf(1, 5);
	mbCausesSanityDecrease = true;
}

//-----------------------------------------------------------------------

cLuxCritter_Spider::~cLuxCritter_Spider()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCritter_Spider::UpdateCritterSpecific(float afTimeStep)
{
	UpdateVelocity(afTimeStep);
	UpdateGroundCheck(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxCritter_Spider::OnDamageCritter(float afAmount)
{
	if(afAmount <= 2) return;

	mfCount = 1.0f;
	mState = eLuxCritterState_Hit;
	//mvVel += mvGroundNormal * 1;
	mfMaxSpeed = 0.1f;
	mvGroundNormal = cVector3f(0,1,0);

	PlaySound("Critter_SpiderHitSound",msHitSound,true, true);

	//gpBase->mpDebugHandler->AddMessage(_W("Damage!"), false);
}

void cLuxCritter_Spider::OnKillCritter()
{

}


//-----------------------------------------------------------------------

float cLuxCritter_Spider::DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY)
{
	return afStartY;
	apSet->DrawFont(apFont, cVector3f(5,afStartY,1),12,cColor(1,1),_W("OnGround: %d Colliding: %d Health: %f"), mbOnGround, mbColliding, mfHealth);
	afStartY += 13;
	apSet->DrawFont(apFont, cVector3f(5,afStartY,1),12,cColor(1,1),_W("State: %d"), mState);
	afStartY += 13;

	return afStartY;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCritter_Spider::UpdateVelocity(float afTimeStep)
{
	cVector3f vMoveVelAdd =0;

	if(mfCount>0) mfCount-= afTimeStep;

	//If lantern is on, flee!
	if(mState != eLuxCritterState_Flee && gpBase->mpPlayer->GetHelperLantern()->IsActive())
	{
		float fDist = gpBase->mpPlayer->GetHelperLantern()->GetLight()->GetRadius();
		
		if(GetDistanceToPlayer() < fDist)
		{
			mState = eLuxCritterState_Flee;
			mfCount = cMath::RandRectf(mvMoveTimeMinMax.x, mvMoveTimeMinMax.y);
		}
	}

	////////////////////////
	// Idle
	if(mState == eLuxCritterState_Idle)
	{
		mfMaxSpeed=0.0f;
		if(mfCount<=0)
		{
			mState = eLuxCritterState_Move;
			mfCount = cMath::RandRectf(mvMoveTimeMinMax.x, mvMoveTimeMinMax.y);
		}
	}
	////////////////////////
	// Flee
	else if(mState == eLuxCritterState_Flee)
	{
		mfMaxSpeed = mfMaxIdleSpeed;

		vMoveVelAdd -= GetTowardPlayerAdd(false, afTimeStep);

		vMoveVelAdd += GetWanderAdd(1,2, afTimeStep);

		if(mfCount<=0)
		{
			mState = eLuxCritterState_Idle;
			mfCount = cMath::RandRectf(mvIdleTimeMinMax.x, mvIdleTimeMinMax.y);
		}	
	}
	////////////////////////
	// Move
	else if(mState == eLuxCritterState_Move)
	{
		mfMaxSpeed = mfMaxIdleSpeed;

		/////////////////// 
		// Wandering
		vMoveVelAdd += GetWanderAdd(1,2, afTimeStep);

		/////////////////// 
		// Swarm around point
		{
			float fSwarmPointDist = cMath::Vector3Dist(mvSwarmPoint, mpBody->GetWorldPosition());

			cVector3f vSwarmPos = cVector3f(mvSwarmPoint.x, mpBody->GetWorldPosition().x, mvSwarmPoint.z);
			cVector3f vWantedVel = cMath::Vector3Normalize(vSwarmPos - mpBody->GetWorldPosition()) * mfMaxSpeed;
			vWantedVel.y=0;

			cVector3f vForce = (vWantedVel - mvVel) * 0.01f * fSwarmPointDist;
			vMoveVelAdd += vForce * afTimeStep;
		}

		/////////////////// 
		// Pause or Hunt
		if(mfCount<=0)
		{
			//Hunt
			/*if(GetDistanceToPlayer() < mfHuntPlayerDist)
			{
				mState = eLuxCritterState_Hunt;
				mfCount = cMath::RandRectf(mvHuntTimeMinMax.x, mvHuntTimeMinMax.y);
			}
			//Pause
			else*/

			/////////////////// 
			// Check attack
			float fDistToPlayer = GetDistanceToPlayer2D();
			if(	gpBase->mpPlayer->IsDead()==false && 
				fDistToPlayer < mvAttackPlayerDistMinMax.y && 
				fDistToPlayer > mvAttackPlayerDistMinMax.x)
			{
				mState = eLuxCritterState_Attack_1;
				PlaySound("CritterAttack", msAttackSound, true, true);
				cVector3f vDir = cMath::Vector3Normalize(gpBase->mpPlayer->GetCharacterBody()->GetPosition()+cVector3f(0,0.5f,0) - mpBody->GetLocalPosition() );
				mvGravityVel += vDir * 25.0f;
				mfMaxSpeed = 3.0f;
				mbUpdateAnimation = false;
				mpMeshEntity->PlayFadeToName("Attack", false, 0.3f);
				mlAnimState = 0;
				mfCount = 0.8f;
				mvGroundNormal = cVector3f(0,1,0);
			}
			/////////////////// 
			// Paus
			else
			{
				mState = eLuxCritterState_Idle;
				mfCount = cMath::RandRectf(mvIdleTimeMinMax.x, mvIdleTimeMinMax.y);
			}
		}
	}
	////////////////////////
	// Hunt
	else if(mState == eLuxCritterState_Hunt)
	{
		mfMaxSpeed = mfMaxHuntSpeed;

		/////////////////// 
		// Move toward player
		vMoveVelAdd += GetTowardPlayerAdd(false, afTimeStep)*3;

		vMoveVelAdd += GetWanderAdd(1,2, afTimeStep);
		
		/////////////////// 
		// Check attack
		float fDistToPlayer = GetDistanceToPlayer2D();
		if(fDistToPlayer < mvAttackPlayerDistMinMax.y && fDistToPlayer > mvAttackPlayerDistMinMax.x)
		{
			mState = eLuxCritterState_Attack_1;
			PlaySound("CritterAttack", msAttackSound, true, true);
			cVector3f vDir = cMath::Vector3Normalize(gpBase->mpPlayer->GetCharacterBody()->GetPosition()+cVector3f(0,0.5f,0) - mpBody->GetLocalPosition() );
			mvGravityVel += vDir * 25.0f;
			mfMaxSpeed = 3.0f;
			mbUpdateAnimation = false;
			mpMeshEntity->PlayFadeToName("Attack", false, 0.3f);
			mlAnimState = 0;
			mfCount = 0.8f;
			mvGroundNormal = cVector3f(0,1,0);
		}

		/////////////////// 
		// Give up
		if(mfCount<=0)
		{
			mState = eLuxCritterState_Idle;
			mfCount = cMath::RandRectf(1, 3);
		}
	}
	////////////////////////
	// Hit
	else if(mState == eLuxCritterState_Hit)
	{
		if(mfCount<=0) mState = eLuxCritterState_Idle;
	}
	////////////////////////
	// Attack 1
	else if(mState == eLuxCritterState_Attack_1)
	{
		mfMaxSpeed = mfMaxHuntSpeed;
		vMoveVelAdd += GetTowardPlayerAdd(false, afTimeStep);

		if(mfCount<=0 || GetDistanceToPlayer2D() < 1.5f)
		{
			mState = eLuxCritterState_Attack_2;
			mbUpdateAnimation = true;
			Attack(cMath::Vector3Normalize(gpBase->mpPlayer->GetCharacterBody()->GetPosition() - mpBody->GetLocalPosition()));
			mfCount = 0.5f;
			//gpBase->mpDebugHandler->AddMessage(_W("Attack!"), false);
		}
	}
	////////////////////////
	// Attack 2
	else if(mState == eLuxCritterState_Attack_2)
	{
		mfMaxSpeed = mfMaxHuntSpeed;
		vMoveVelAdd += GetTowardPlayerAdd(false, afTimeStep);
		if(mfCount<=0) mState= eLuxCritterState_Idle;
	}
	
	/////////////////////
	// Update velocity
	mvVel += vMoveVelAdd;

	/////////////////////
	// Sound
	if(mState == eLuxCritterState_Move || mState == eLuxCritterState_Hunt || mState == eLuxCritterState_Flee)
	{
		mfPlaySoundCount-=afTimeStep;
		if(mfPlaySoundCount < 0)
		{
			mfPlaySoundCount = cMath::RandRectf(mvIdleSoundRandMinMax.x, mvIdleSoundRandMinMax.y);
			PlaySound("Critter_SpiderSound",msIdleSound,true, true);
		}
	}
	
	/////////////////// 
	// Cap speed

	mvVel = cMath::Vector3MaxLength(mvVel, mfMaxSpeed);
}

//-----------------------------------------------------------------------

void cLuxCritter_Spider::UpdateGroundCheck(float afTimeStep)
{
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();

	////////////////////////
	// Fix ground normal
	mbOnGround=false;
	{
		cVector3f vStartPos = mpBody->GetLocalPosition() - mvGroundNormal * (mpBody->GetShape()->GetRadius()-0.05f);
		cVector3f vEndPos = vStartPos - mvGroundNormal*0.25f;

		mpRayCallback->Reset();
		pPhysicsWorld->CastRay(mpRayCallback, vStartPos, vEndPos, false,true,true, true);
		if(mpRayCallback->GetIntersected())
		{
			mvGroundNormal = mpRayCallback->GetNormal();
			mbOnGround = true;
		}

		if(mbOnGround ==false)
		{
			//Look for ground elsewhere! Ledge climbing done here?
		}
	}

	////////////////////////
	// Gravity
	if(mbOnGround==false || mbColliding==false)
	{
		mvGravityVel -= mvGroundNormal * 9.8f * afTimeStep;
		mfFallingCount+= afTimeStep;
		if(mfFallingCount > 0.3f)
		{
			mvGroundNormal = cVector3f(0,1,0);
			mvGravityVel = mvGroundNormal * mvGravityVel.Length()*-1;
		}

		mvGravityVel = cMath::Vector3MaxLength(mvGravityVel, 10.0f);
	}
	else
	{
		mfFallingCount =0;
	}
}


//-----------------------------------------------------------------------

void cLuxCritter_Spider::OnShapeCollision(const cVector3f& avPushVec, float afTimeStep)
{
	if(mState == eLuxCritterState_Hit) return;

	/////////////////////////////
	// Update count, so
	if(mfNewGroundNormalCount > 0)
	{
		mfNewGroundNormalCount -= afTimeStep;
		//gpBase->mpDebugHandler->AddMessage(_W("Newground count too high: ")+ cString::ToStringW(mfNewGroundNormalCount), false);
		return;
	}

	/////////////////////////////
	// Get the push dir
	iPhysicsWorld *pPhysicsWorld = mpWorld->GetPhysicsWorld();

	cVector3f vPushDir = cMath::Vector3Normalize(avPushVec) * -1;
	vPushDir.Normalize();

	//Make push vec e on the same plane as the forward vec
	vPushDir = vPushDir - mvGroundNormal * cMath::Vector3Dot(mvGroundNormal, vPushDir);

	//////////////////////////////////
	// Check wall intersection
	cVector3f vStartPos = mpBody->GetLocalPosition() + vPushDir * (mpBody->GetShape()->GetRadius()-0.05f);
	cVector3f vEndPos = vStartPos + vPushDir*0.1f;

	cVector3f mvIntersectNormal(0);
	
	mpRayCallback->Reset();
	pPhysicsWorld->CastRay(mpRayCallback, vStartPos, vEndPos, false,true,true, true);
	if(mpRayCallback->GetIntersected())
	{
		//No climbing on dynamic bodies allowed
		if(mpRayCallback->GetBody()->GetMass()!=0) return;

        mvIntersectNormal = mpRayCallback->GetNormal();

		if(mvIntersectNormal ==0) return;
		
		//Must be enough difference!
		if(cMath::Vector3Dot(mvIntersectNormal, mvGroundNormal) > 8)//0.71f)
		{
			//gpBase->mpDebugHandler->AddMessage(_W("Dot only: ")+ cString::ToStringW(cMath::Vector3Dot(mvIntersectNormal, mvGroundNormal)), false);
			return;
		}
	}
	else
	{
		return;
	}

	//////////////////////////////////
	// Check so that the space above is free.
	float fTestHeight = 1.5f;

	vStartPos = mpBody->GetLocalPosition() - vPushDir * mpBody->GetShape()->GetRadius();
	vEndPos = vStartPos + mvGroundNormal * fTestHeight;
	
	mpRayCallback->Reset();
	
	pPhysicsWorld->CastRay(mpRayCallback, vStartPos, vEndPos, false,false,false, true);
    if(mpRayCallback->GetIntersected())
	{
		//gpBase->mpDebugHandler->AddMessage(_W("Space above not free!"), false);
		return;
	}

	//////////////////////////////////
	// Check so that there is something to climb on
	int lTestPoints = 5; 
	int lClimbPointCount =0;
	for(int i=1; i<=5; ++i)
	{
		float fHeightAmount = (fTestHeight/ (float)lTestPoints)*(float)i;
		vStartPos = mpBody->GetLocalPosition() + mvGroundNormal * fHeightAmount;
		vEndPos = vStartPos + vPushDir * (mpBody->GetShape()->GetRadius()+0.25f);

		pPhysicsWorld->CastRay(mpRayCallback, vStartPos, vEndPos, false,false,false, true);
		if(mpRayCallback->GetIntersected()) lClimbPointCount++;
	}
	if(lClimbPointCount < 3)
	{
		//gpBase->mpDebugHandler->AddMessage(_W("Climb points only: ")+cString::ToStringW(lClimbPointCount), false);
		return;
	}


	//////////////////////////////////
	// Set new ground normal
    cVector3f vOldGroundNormal = mvGroundNormal;
	mvGroundNormal = mvIntersectNormal;
	
    //Calculate new forward
	cVector3f vForward = cMath::Vector3Normalize(mvVel);
	cVector3f vRight = cMath::Vector3Cross(vOldGroundNormal, vForward);
	vRight.Normalize();
	vForward = cMath::Vector3Cross(vRight, mvGroundNormal);
	vForward.Normalize();

	mvVel = vForward * mvVel.Length();
	
	//mfNewGroundNormalCount = 2.0f;
	//gpBase->mpDebugHandler->AddMessage(_W("New ground normal! ")+cString::ToStringW(mvGroundNormal.x)+_W(":")+cString::ToStringW(mvGroundNormal.y)+_W(":")
	//															+cString::ToStringW(mvGroundNormal.z), false);

	//TODO: Calc new speed.
}



//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxCritter_Spider_SaveData, iLuxProp_CritterBase_SaveData)
kSerializeVar(mlState, eSerializeType_Int32)
kSerializeVar(mfFallingCount, eSerializeType_Float32)
kSerializeVar(mfNewGroundNormalCount, eSerializeType_Float32)
kSerializeVar(mbOnGround, eSerializeType_Bool)
kSerializeVar(mfMaxSpeed, eSerializeType_Float32)
kSerializeVar(mfCount, eSerializeType_Float32)
kSerializeVar(mfPlaySoundCount, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxCritter_Spider::CreateSaveData()
{
	return hplNew(cLuxCritter_Spider_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxCritter_Spider::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxCritter_Spider_SaveData *pData = static_cast<cLuxCritter_Spider_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	pData->mlState = mState;
	kCopyToVar(pData,mfFallingCount);
	kCopyToVar(pData,mfNewGroundNormalCount);
	kCopyToVar(pData,mbOnGround);
	kCopyToVar(pData,mfMaxSpeed);
	kCopyToVar(pData,mfCount);
	kCopyToVar(pData,mfPlaySoundCount);
}

//-----------------------------------------------------------------------

void cLuxCritter_Spider::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxCritter_Spider_SaveData *pData = static_cast<cLuxCritter_Spider_SaveData*>(apSaveData);
	
	//////////////////
	//Set variables
	mState = (eLuxCritterState)pData->mlState;
	kCopyFromVar(pData,mfFallingCount);
	kCopyFromVar(pData,mfNewGroundNormalCount);
	kCopyFromVar(pData,mbOnGround);
	kCopyFromVar(pData,mfMaxSpeed);
	kCopyFromVar(pData,mfCount);
	kCopyFromVar(pData,mfPlaySoundCount);
}

//-----------------------------------------------------------------------

void cLuxCritter_Spider::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
