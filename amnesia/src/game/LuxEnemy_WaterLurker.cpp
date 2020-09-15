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

#include "LuxEnemy_WaterLurker.h"

#include "LuxEnemyMover.h"
#include "LuxEnemyPathfinder.h"

#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxMusicHandler.h"
#include "LuxDebugHandler.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"

#include "LuxProp_Object.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemyLoader_WaterLurker::cLuxEnemyLoader_WaterLurker(const tString& asName) : iLuxEnemyLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxEnemy *cLuxEnemyLoader_WaterLurker::CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxEnemy_WaterLurker, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_WaterLurker::LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)
{
	cLuxEnemy_WaterLurker *pWaterLurker = static_cast<cLuxEnemy_WaterLurker*>(apEnemy);

	pWaterLurker->mfPlayerDetectionHeight = GetVarFloat("PlayerDetectionHeight", 0.6f);
	pWaterLurker->mfEatDamage = GetVarFloat("EatDamage", 0);
	pWaterLurker->mfAfterAttackPauseTime = GetVarFloat("AfterAttackPauseTime", 0);

	pWaterLurker->msSplashPS_Walk = GetVarString("SplashPS_Walk", "");
	pWaterLurker->msSplashPS_Run = GetVarString("SplashPS_Run", "");
	pWaterLurker->msSplashPS_Eat = GetVarString("SplashPS_Eat", "");
	pWaterLurker->msSplashPS_Attack = GetVarString("SplashPS_Attack", "");

	pWaterLurker->msSplashSound_Walk = GetVarString("SplashSound_Walk", "");
	pWaterLurker->msSplashSound_Run = GetVarString("SplashSound_Run", "");
	pWaterLurker->msSplashSound_Eat = GetVarString("SplashSound_Eat", "");
	pWaterLurker->msSplashSound_Attack = GetVarString("SplashSound_Attack", "");

	gpBase->PreloadSound(pWaterLurker->msSplashSound_Walk);
	gpBase->PreloadSound(pWaterLurker->msSplashSound_Run);
	gpBase->PreloadSound(pWaterLurker->msSplashSound_Eat);
	gpBase->PreloadSound(pWaterLurker->msSplashSound_Attack);

	gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Walk);
	gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Run);
	gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Eat);
	gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Attack);
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_WaterLurker::LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)
{
	cLuxEnemy_WaterLurker *pWaterLurker = static_cast<cLuxEnemy_WaterLurker*>(apEnemy);

	float fHeight = apInstanceVars->GetVarFloat("PlayerDetectionHeight", 0);
	if(fHeight >0) pWaterLurker->mfPlayerDetectionHeight = fHeight;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemy_WaterLurker::cLuxEnemy_WaterLurker(const tString &asName, int alID, cLuxMap *apMap) : iLuxEnemy(asName,alID,apMap, eLuxEnemyType_WaterLurker)
{
	mbUseAnimations = false;
	mbCausesSanityDecrease = false;
	mbCausesSanityDecreaseAsDefault = false;
}

//-----------------------------------------------------------------------

cLuxEnemy_WaterLurker::~cLuxEnemy_WaterLurker()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnSetupAfterLoad(cWorld *apWorld)
{
	mpMeshEntity->SetActive(false);
	mpMeshEntity->SetVisible(false);
}


//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnAfterWorldLoad()
{

}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::UpdateEnemySpecific(float afTimeStep)
{
	}	

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)
{
	kLuxBeginStateMachine
	
	////////////////////////////////
	// Default
	kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		ChangeState(eLuxEnemyState_Hunt);

	kLuxOnMessage(eLuxEnemyMessage_TakeHit)
		//DO NOTHING

	kLuxOnMessage(eLuxEnemyMessage_PlayerDead)
		ChangeState(eLuxEnemyState_Idle);	

	kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
		cAINode *pNode = mpPathfinder->GetNodeAtPos(apMessage->mvCustomValue, 0, 1.5f, true, true, true, NULL);
		if(pNode)
		{
			gpBase->mpDebugHandler->AddMessage(_W("Found food!"),false);

			mvTempPos = pNode->GetPosition();
			ChangeState(eLuxEnemyState_Search);	
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
		}
		else
		{
			gpBase->mpDebugHandler->AddMessage(_W("Found food but no node.."),false);
		}

	////////////////////////////////
	// Idle
	kLuxState(eLuxEnemyState_Idle)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Idle);
		
		///////////////////////
		// Heard sound
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

	////////////////////////////////
	// Go Home
	kLuxState(eLuxEnemyState_GoHome)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Idle);
			
			cAINode *pNode = mpPathfinder->GetNodeAtPos(GetPlayerFeetPos(), 0, 6, false, false, true, NULL);
			if(pNode)
			{
				//gpBase->mpDebugHandler->AddMessage(_W("Going to node!"),false);
				mpPathfinder->MoveTo(pNode->GetPosition());
			}
			else
			{
				//gpBase->mpDebugHandler->AddMessage(_W("Going to start position"),false);
				mpPathfinder->MoveTo(mvStartPosition);
			}
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);


		///////////////////////
		// Update
		kLuxOnUpdate
			if(mpMover->GetStuckCounter()>0.9f)
			{
				ChangeState(eLuxEnemyState_Idle);
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
			}

		///////////////////////
		// Heard sound
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

		///////////////////////
		// Movement effect
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			SplashWater(eWaterLurkerSplash_Walk);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);


		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			ChangeState(eLuxEnemyState_Idle);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
	
	
	////////////////////////////////
	// Investigate (heard a sound)
	kLuxState(eLuxEnemyState_Investigate)
		kLuxOnEnter	
			gpBase->mpDebugHandler->AddMessage(_W("Sound Heard! Vol: ")+cString::ToStringW(mfTempVal),false);

			cAINode *pNode = mpPathfinder->GetNodeAtPos(mvTempPos, 0, 10, true, true, true,NULL,1);
			if(pNode)
			{
				mpPathfinder->MoveTo(pNode->GetPosition());
			}
			else
			{
				gpBase->mpDebugHandler->AddMessage(_W("Could not find node near sound!"),false);
				mpPathfinder->MoveTo(mvTempPos);
			}
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);


		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			ChangeState(eLuxEnemyState_Idle);	
		
		kLuxOnUpdate
			if(mpMover->GetStuckCounter()>0.9f)
			{
				ChangeState(eLuxEnemyState_Idle);
			}

		///////////////////////
		// Movement effect
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			SplashWater(eWaterLurkerSplash_Walk);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);

		///////////////////////
		// Heard new sound
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			//If a new sound is loader than the previous go for that instead!
			if(mpPathfinder->IsMoving() && apMessage->mfCustomValue > mfTempVal)
			{
				cAINode *pNode = mpPathfinder->GetNodeAtPos(mvTempPos, 0, 10, true, true, true,NULL,1);
				if(pNode) 
					mpPathfinder->MoveTo(pNode->GetPosition());

				mfTempVal = apMessage->mfCustomValue;
			}

	////////////////////////////////
	// Search (Scavenge for food!)
	kLuxState(eLuxEnemyState_Search)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Idle);

			mpPathfinder->MoveTo(mvTempPos);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);

		kLuxOnUpdate
		
		///////////////////////
		// Movement effect
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			SplashWater(eWaterLurkerSplash_Walk);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);


		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			gpBase->mpDebugHandler->AddMessage(_W("to eat state!\n"),false);
			ChangeState(eLuxEnemyState_Eat);
		
		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
				
	////////////////////////////////
	// Eat
	kLuxState(eLuxEnemyState_Eat)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Idle);

			SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);
			mvTempPos = mpCharBody->GetFeetPosition();
			mpPathfinder->Stop();

		////////////////////////////////
		//Search for food and eat it!
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			cLuxProp_Object *pFood = GetClosestFood(3.5f,mfPlayerDetectionHeight);
			if(pFood)
			{
				iPhysicsBody *pFoodBody = pFood->GetBody(0);
				cVector3f vToFeet(mpCharBody->GetFeetPosition() - pFoodBody->GetLocalPosition());
				vToFeet.Normalize();

                pFoodBody->AddImpulse(vToFeet*2.5f);				

				SplashWater(eWaterLurkerSplash_Eat);
				pFood->GiveDamage(mfEatDamage, 10);
				gpBase->mpDebugHandler->AddMessage(_W("took bite!!"),false);
				gpBase->mpDebugHandler->AddMessage(_W("Health: ")+cString::ToStringW(pFood->GetHealth()),false);
			
				SendMessage(eLuxEnemyMessage_TimeOut, 1.2f, true);
				mvTempPos = pFood->GetBody(0)->GetLocalPosition();
			}
			else
			{
				gpBase->mpDebugHandler->AddMessage(_W("No more food!!"),false);
				ChangeState(eLuxEnemyState_GoHome);
			}

		////////////////////////////////
		//Move forward
		kLuxOnUpdate
			if(Dist2D(mvTempPos) > mpCharBody->GetSize().x*0.75f)
				mpMover->MoveToPos(mvTempPos);

		////////////////////////////////
		//Player detection, only hunt if player is really close.
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
			if(DistToPlayer2D() < 2.0f)
				ChangeState(eLuxEnemyState_Hunt);
			else
				mbPlayerDetected = false;


		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
			
		
	////////////////////////////////
	// Hunt
	kLuxState(eLuxEnemyState_Hunt)	
		///////////////////////
		// Enter
		kLuxOnEnter
			if(PlayerIsDetected()==false)
			{
				ChangeState(eLuxEnemyState_GoHome);	
				mbCausesSanityDecrease = false;
			}
			else if(mPreviousState == eLuxEnemyState_AttackMeleeShort)
			{
				ChangeState(eLuxEnemyState_HuntPause);
			}
			else
			{
				ChangeSoundState(eLuxEnemySoundState_Hunt);

				SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
				SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);
				SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);
				gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack,this);
				mbCausesSanityDecrease = true;
			}
			
		///////////////////////
		// Leave
		kLuxOnLeave
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
		
		///////////////////////
		// Movement effect
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			if(mpMover->GetWantedSpeedAmount() > 0.2f)
				SplashWater(eWaterLurkerSplash_Run);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);

		///////////////////////
		//Update the player position	
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			mpPathfinder->MoveTo(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
			SendMessage(eLuxEnemyMessage_TimeOut, 0.4f, true);
			if(PlayerIsDetected()==false)
			{
				ChangeState(eLuxEnemyState_GoHome);	
				mbCausesSanityDecrease = false;
			}
		
		///////////////////////
		//On update
		kLuxOnUpdate
			if(DistToPlayer() < mfNormalAttackDistance)
			{
				ChangeState(eLuxEnemyState_AttackMeleeShort);
			}
			else if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
			
		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)

	////////////////////////////////
	// Hunt Pause
	kLuxState(eLuxEnemyState_HuntPause)	
		///////////////////////
		// Enter
		kLuxOnEnter
			mpPathfinder->Stop();
			SendMessage(eLuxEnemyMessage_TimeOut, mfAfterAttackPauseTime, true);

		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			ChangeState(eLuxEnemyState_Hunt);	

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
		
	////////////////////////////////
	// Break door
	kLuxState(eLuxEnemyState_BreakDoor)	
		kLuxOnEnter
			mpPathfinder->Stop();
			SendMessage(eLuxEnemyMessage_TimeOut, 0.01f, true);
		
		///////////////////
		//Damage door
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			//SplashWater(eWaterLurkerSplash_Attack);
			Attack(mNormalAttackSize, mBreakDoorAttackDamage);
			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(0.5,1.5f), true);

		///////////////////
		//Check if door is destroyed
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			ChangeState(eLuxEnemyState_Hunt);	
			/*if(mpMap->DoorIsBroken(mlStuckDoorID))
			{
				ChangeState(eLuxEnemyState_Hunt);	
			}
			else
			{
				SendMessage(eLuxEnemyMessage_TimeOut, 1.5f, true);
			}*/
			

		kLuxOnUpdate
			//Turn towards the door!
			mpMover->TurnToPos(mvTempPos);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
        		
	////////////////////////////////
	// Attack Short
	kLuxState(eLuxEnemyState_AttackMeleeShort)
		kLuxOnEnter
			SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);		
	
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			ChangeState(mPreviousState);

		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			SplashWater(eWaterLurkerSplash_Attack);
			Attack(mNormalAttackSize, mNormalAttackDamage);
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.5f, true);
			mpPathfinder->Stop();
		
		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
	
	
	////////////////////////////////
	// Dead (just in case...)
	kLuxState(eLuxEnemyState_Dead)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Silent);

			mpPathfinder->Stop();
			//PlayAnim("Dead",false, 0.4f);	
			//PlayAnim("Dead",false, 0.3f,false,1.0f,false,true,false);	
			gpBase->mpPlayer->RemoveTerrorEnemy(this);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
			mpCharBody->SetActive(false);


		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			//PlayAnim("Dead",false, 0.3f,false,1.0f,false,true,false);	

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)

	kLuxEndStateMachine
}

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::InRangeOfFood(iPhysicsBody *apFoodBody)
{
	//TODO: Check if in water?
	float fYDist = apFoodBody->GetLocalPosition().y - mpCharBody->GetFeetPosition().y;
	
	bool bX = cMath::Abs(fYDist) <= mfPlayerDetectionHeight;
	return bX;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)
{
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();

	mpCharBody->GetCurrentBody()->RenderDebugGeometry(apFunctions->GetLowLevelGfx(), cColor(1,1));

	if(mCurrentState == eLuxEnemyState_AttackMeleeShort || mCurrentState== eLuxEnemyState_BreakDoor)
	{
		pPhysicsWorld->RenderShapeDebugGeometry(GetAttackShape(0), GetDamageShapeMatrix(cVector3f(0,0,1)), apFunctions->GetLowLevelGfx(),
												cColor(1,0,0,1));
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::SplashWater(eWaterLurkerSplash aType)
{
	tString sPS = "";
	if(aType == eWaterLurkerSplash_Walk)		sPS = msSplashPS_Walk;
	else if(aType == eWaterLurkerSplash_Run)	sPS = msSplashPS_Run;
	else if(aType == eWaterLurkerSplash_Eat)	sPS = msSplashPS_Eat;
	else if(aType == eWaterLurkerSplash_Attack)	sPS = msSplashPS_Attack;
	
	if(sPS!="")
	{
		cParticleSystem *pPS = mpMap->GetWorld()->CreateParticleSystem("LurkerSplash", sPS,1);
		if(pPS) pPS->SetPosition(mpCharBody->GetFeetPosition() + cVector3f(0, 0.7f, 0));
	}


	tString sSound = "";
	if(aType == eWaterLurkerSplash_Walk)		sSound = msSplashSound_Walk;
	else if(aType == eWaterLurkerSplash_Run)	sSound = msSplashSound_Run;
	else if(aType == eWaterLurkerSplash_Eat)	sSound = msSplashSound_Eat;
	else if(aType == eWaterLurkerSplash_Attack)	sSound = msSplashSound_Attack;

	PlaySound(sSound);
}

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::PlayerIsDetected()
{
	return AbsHeightDistToPlayer() <= mfPlayerDetectionHeight; 
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnSetActiveEnemySpecific(bool abX)
{
	//Make sure the mesh is invisible!
	mpMeshEntity->SetActive(false);
	mpMeshEntity->SetVisible(false);
}

//-----------------------------------------------------------------------

float cLuxEnemy_WaterLurker::GetDamageMul(float afAmount, int alStrength)
{
	if(	mCurrentState == eLuxEnemyState_Idle ||
		mCurrentState == eLuxEnemyState_Wait ||
		mCurrentState == eLuxEnemyState_Patrol ||
		mCurrentState == eLuxEnemyState_Investigate) 
	{
		return 1.0f;
	}
	if( mCurrentState == eLuxEnemyState_Hurt) return 0.5f;
	
	return 0.2f;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::PatrolUpdateGoal()
{
	
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::PatrolEndOfPath()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxEnemy_WaterLurker_SaveData, iLuxEnemy_SaveData)

kSerializeVar(mfPlayerDetectionHeight, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxEnemy_WaterLurker::CreateSaveData()
{
	return hplNew(cLuxEnemy_WaterLurker_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxEnemy_WaterLurker_SaveData *pData = static_cast<cLuxEnemy_WaterLurker_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mfPlayerDetectionHeight);
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxEnemy_WaterLurker_SaveData *pData = static_cast<cLuxEnemy_WaterLurker_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyFromVar(pData,mfPlayerDetectionHeight);
	
	////////////////////////
	// Handle changed enums
	if (mCurrentState >= eLuxEnemyState_PigEnumStart) mCurrentState = eLuxEnemyState_LastEnum;
	if (mNextState >= eLuxEnemyState_PigEnumStart) mNextState = eLuxEnemyState_LastEnum;
	if (mPreviousState >= eLuxEnemyState_PigEnumStart) mPreviousState = eLuxEnemyState_LastEnum;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}


//-----------------------------------------------------------------------

