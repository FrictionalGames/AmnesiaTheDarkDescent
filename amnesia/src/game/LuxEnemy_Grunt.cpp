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

#include "LuxEnemy_Grunt.h"

#include "LuxEnemyMover.h"
#include "LuxEnemyPathfinder.h"

#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxMusicHandler.h"
#include "LuxDebugHandler.h"
#include "LuxGlobalDataHandler.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemyLoader_Grunt::cLuxEnemyLoader_Grunt(const tString& asName) : iLuxEnemyLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxEnemy *cLuxEnemyLoader_Grunt::CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxEnemy_Grunt, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_Grunt::LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)
{
	cLuxEnemy_Grunt *pGrunt = static_cast<cLuxEnemy_Grunt*>(apEnemy);

	pGrunt->msNoticeSound = GetVarString("NoticeSound");
	pGrunt->msGiveUpNoticeSound = GetVarString("GiveUpNoticeSound");
	pGrunt->msEnabledSound = GetVarString("EnabledSound");
	gpBase->PreloadSound(pGrunt->msNoticeSound);
	gpBase->PreloadSound(pGrunt->msEnabledSound);

	pGrunt->mfGroggyDamageCount = GetVarFloat("GroggyDamageCount", 0);
	pGrunt->mfAlertToHuntDistance = GetVarFloat("AlertToHuntDistance", 0);
	pGrunt->mfAlertToInstantHuntDistance = GetVarFloat("AlertToInstantHuntDistance", 0);
	pGrunt->mfHuntPauseMinTime = GetVarFloat("HuntPauseMinTime", 0);
	pGrunt->mfHuntPauseMaxTime = GetVarFloat("HuntPauseMaxTime", 0);
	pGrunt->mfIncreaseAlertSpeedDistance = GetVarFloat("IncreaseAlertSpeedDistance", 0);
	pGrunt->mfIncreasedAlertSpeedMul = GetVarFloat("IncreasedAlertSpeedMul", 0);
	
	pGrunt->mfAlertRunTowardsToHuntLimit = GetVarFloat("AlertRunTowardsToHuntLimit", 0);
	pGrunt->mfAlertRunTowardsCheckDistance = GetVarFloat("AlertRunTowardsCheckDistance", 0);

	pGrunt->mfIdleExtraTimeMin = GetVarFloat("IdleExtraTimeMin", 2);
	pGrunt->mfIdleExtraTimeMax = GetVarFloat("IdleExtraTimeMax", 6);
	pGrunt->mlIdleExtraNum = GetVarInt("IdleExtraNum", 3);
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_Grunt::LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemy_Grunt::cLuxEnemy_Grunt(const tString &asName, int alID, cLuxMap *apMap) : iLuxEnemy(asName,alID,apMap, eLuxEnemyType_Grunt)
{
	mfWaitTime =0;
	mfAlertRunTowardsCount = 0;

	mbAlignEntityWithGroundRay = true;
}

//-----------------------------------------------------------------------

cLuxEnemy_Grunt::~cLuxEnemy_Grunt()
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnSetupAfterLoad(cWorld *apWorld)
{
}


//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnAfterWorldLoad()
{

}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::UpdateEnemySpecific(float afTimeStep)
{

}

//-----------------------------------------------------------------------

bool cLuxEnemy_Grunt::StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)
{
	kLuxBeginStateMachine
	
	////////////////////////////////
	// Default
	kLuxOnMessage(eLuxEnemyMessage_Reset)
		
		gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
		gpBase->mpPlayer->RemoveTerrorEnemy(this);


		ChangeState(eLuxEnemyState_Wait);

	kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		ChangeState(eLuxEnemyState_Alert);

	kLuxOnMessage(eLuxEnemyMessage_TakeHit)
		mfDamageCount += apMessage->mfCustomValue;
		if(mfDamageCount > mfGroggyDamageCount)
		{
			ChangeState(eLuxEnemyState_Hurt);
			mfDamageCount =0;
		}

	//If enemy is out of range (having been in, then turn him off)
	kLuxOnMessage(eLuxEnemyMessage_PlayerOutOfRange)
		SetActive(false);

	kLuxOnMessage(eLuxEnemyMessage_PlayerDead)
		ChangeState(eLuxEnemyState_Idle);	

	kLuxOnMessage(eLuxEnemyMessage_HelpMe)
		ShowPlayerPosition();
		ChangeState(eLuxEnemyState_Hunt);

	////////////////////////////////
	// Idle
	kLuxState(eLuxEnemyState_Idle)
		kLuxOnEnter
			mbPlayerInRange = false; //Reset so new checks are made in case player is near.
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

		kLuxOnMessage(eLuxEnemyMessage_PlayerInRange)
			gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName)+_W(" enabled!"), false );
			
			if(gpBase->mpGlobalDataHandler->GetEnemyActivateSoundAllowed())
			{
				////////////////////////
				// HARDMODE

				//if (gpBase->mbHardMode == false)
				{
					gpBase->mpGlobalDataHandler->SetEnemyActivateSoundMade();
					PlaySound(msEnabledSound);
				}
			}

			if(mvPatrolNodes.empty())
				ChangeState(eLuxEnemyState_Wait);
			else
				ChangeState(eLuxEnemyState_Patrol);

		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
			//Nothing

	////////////////////////////////
	// Go Home
	kLuxState(eLuxEnemyState_GoHome)
		kLuxOnEnter
			mpPathfinder->MoveTo(mvStartPosition);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
		
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			ChangeState(eLuxEnemyState_Idle);

		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

	////////////////////////////////
	// Wait
	kLuxState(eLuxEnemyState_Wait)
		kLuxOnEnter
			if(mfWaitTime <= 0)
				SendMessage(eLuxEnemyMessage_TimeOut, cMath::RandRectf(1, 3), true);
			else
				SendMessage(eLuxEnemyMessage_TimeOut, mfWaitTime, true);
			mfWaitTime =0;

			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(mfIdleExtraTimeMin, mfIdleExtraTimeMax), true);

			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
		
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(mPreviousState == eLuxEnemyState_Search)
			{
				FadeOutCurrentAnim(0.2f);
				ChangeState(eLuxEnemyState_Search);	
			}
			else
			{
				if(GetPatrolNodeNum()>0)
				{
					FadeOutCurrentAnim(0.2f);
					ChangeState(eLuxEnemyState_Patrol);	
				}
				else
				{
					SendMessage(eLuxEnemyMessage_TimeOut, cMath::RandRectf(3, 5), true);
				}
			}

		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			PlayAnim("IdleExtra"+cString::ToString(cMath::RandRectl(1,mlIdleExtraNum)),false, 0.3f);

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(2, 6), true);
	
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

	
	////////////////////////////////
	// Patrol
	kLuxState(eLuxEnemyState_Patrol)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Idle);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			PatrolUpdateGoal();


		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
		
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			PatrolEndOfPath();

		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}
	
	////////////////////////////////
	// Investigate
	kLuxState(eLuxEnemyState_Investigate)
		kLuxOnEnter	
			ChangeSoundState(eLuxEnemySoundState_Alert);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			
			mpPathfinder->Stop();
			PlayAnim("Notice"+cString::ToString(cMath::RandRectl(1,2)), false, 0.3f);		

			//mfForwardSpeed *= 1.5f;
			PlaySound(msNoticeSound);

			mfFOVMul = 4.0f; //When hearing a sound, enemy gets extra alert.

			gpBase->mpDebugHandler->AddMessage(_W("Sound Heard! Vol: ")+cString::ToStringW(mfTempVal),false);

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

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


		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			
			PlaySound(msGiveUpNoticeSound);

			if(mPreviousState == eLuxEnemyState_Search)
				ChangeState(eLuxEnemyState_Search);	
			else
				ChangeState(eLuxEnemyState_Patrol);	
			
		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}

		kLuxOnLeave
			mfFOVMul = 1.0f;
			FadeOutCurrentAnim(0.2f);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

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
	// Alert
	#ifndef LUX_DEMO_VERSION
	kLuxState(eLuxEnemyState_Alert)
		kLuxOnEnter	
			ChangeSoundState(eLuxEnemySoundState_Alert);

			SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);
			mpPathfinder->MoveTo(mvLastKnownPlayerPos);
			
			gpBase->mpPlayer->AddTerrorEnemy(this);
			//gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Search,this);

			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 1.2f;
			mfFOVMul = 4.0f;

			mfAlertRunTowardsCount =0;

		kLuxOnLeave	
			mfFOVMul = 1.0f;
			if(mNextState != eLuxEnemyState_BreakDoor)
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
			}
		
		kLuxOnUpdate
			float fDistToPlayer = DistToPlayer();

			//Check if player is moving towards enemy
			if(fDistToPlayer < mfAlertRunTowardsCheckDistance)
			{
				float fPlayerDirAmount = GetPlayerMovementTowardEnemyAmount();
				mfAlertRunTowardsCount += fPlayerDirAmount;
			}
			else
			{
				mfAlertRunTowardsCount-=1;
			}
			if(mfAlertRunTowardsCount<0) mfAlertRunTowardsCount=0;

			//Update the speed of movement.
			if(fDistToPlayer > mfIncreaseAlertSpeedDistance)
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				mfForwardSpeed *= 1.2f * mfIncreasedAlertSpeedMul;
			}
			else
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				mfForwardSpeed *= 1.2f;
			}
						
			
			//Stuck at door, break it
			if(gpBase->mpPlayer->GetTerror() >= 1 && mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
			//Player is no longer seen, see if time to search
			else if(PlayerIsDetected()==false)
			{
				if(gpBase->mpPlayer->GetTerror() < 1)
					ChangeState(eLuxEnemyState_Search);
			}
			//Player is seen, see if close enough for hunt
			else
			{
				if(	(gpBase->mpPlayer->GetTerror() >= 1 && (fDistToPlayer > mfAlertToHuntDistance || mfAlertRunTowardsCount>mfAlertRunTowardsToHuntLimit) ) || 
					fDistToPlayer < mfAlertToInstantHuntDistance)
				{
					gpBase->mpPlayer->SetTerror(1.0f);
					ChangeState(eLuxEnemyState_Hunt);
				}
			}

		//Reach end of path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
				float fDistToPlayer = DistToPlayer();

				//Path ended and player is not seen or enemy is stuck (this should only happen when at a distance!
				if(PlayerIsDetected()==false || (apMessage->mlCustomValue == 1 && fDistToPlayer>5))
				{
					ChangeState(eLuxEnemyState_Search);
				}
				else if(apMessage->mlCustomValue==1 && PlayerIsDetected())
				{
					//This is when the enemy should just stnad still but I think nothing is really needed
				}
				

		//Update move to
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			if(PlayerIsDetected())
				mpPathfinder->MoveTo(mvLastKnownPlayerPos);
			
			SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);

		kLuxOnMessage(eLuxEnemyMessage_TakeHit)
			//ChangeState(eLuxEnemyState_Hunt);
			ChangeState(eLuxEnemyState_Hurt);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);

	////////////////////////////////
	// Search
	kLuxState(eLuxEnemyState_Search)
		kLuxOnEnter
			ChangeSoundState(eLuxEnemySoundState_Alert);

			SendMessage(eLuxEnemyMessage_TimeOut, mfPlayerSearchTime, true);
			
			SendMessage(eLuxEnemyMessage_TimeOut_2,cMath::RandRectf(0,1), true);
		
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
			gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Search,this);
			
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 1.0f;

		kLuxOnLeave
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
		
		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}

		//At node
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			mpPathfinder->Stop();
			SendMessage(eLuxEnemyMessage_TimeOut_2,cMath::RandRectf(1,3), true);
		
		//Wait a few secs
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			//cAINode * pNode = GetSearchForPlayerNode();
			cAINode * pNode = mpPathfinder->GetNodeAtPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition(), 4, 12,false, false, true, NULL);
			if(pNode)
				mpPathfinder->MoveTo(pNode->GetPosition());
			else
				ChangeState(eLuxEnemyState_Patrol);
		
		//End of searching
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			ChangeState(eLuxEnemyState_Patrol);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);

		//Hear sound
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}
			

	////////////////////////////////
	// Hunt
	kLuxState(eLuxEnemyState_Hunt)	
		///////////////////////
		// Enter
		kLuxOnEnter
			if(mlAttackHitCounter >= 1)
			{
				mlAttackHitCounter =0;
				
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
				
				ChangeState(eLuxEnemyState_HuntPause);
			}
			else
			{
				ChangeSoundState(eLuxEnemySoundState_Hunt);
				SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
				SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);
				mfFOVMul = 4.0f;
				
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
				gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack,this);
				gpBase->mpPlayer->AddTerrorEnemy(this);

				mpPathfinder->MoveTo(mvLastKnownPlayerPos);

				SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);
			}
			

		///////////////////////
		// Leave
		kLuxOnLeave
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfFOVMul = 1.0f;
		
		///////////////////////
		// Update
		kLuxOnUpdate
			//////////////////////////
			//Short attack
			float fDistToPlayer = DistToPlayer();
			if(CanSeePlayer() && fDistToPlayer < mfNormalAttackDistance)
			{
				ChangeState(eLuxEnemyState_AttackMeleeShort);
			}
			else if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
		
		/////////////////////////////
		// Check if close enough for launch attack
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			//////////////////////////
			//Launch attack
			float fDist = DistToPlayer();
			if(CanSeePlayer() && fDist > mfNormalAttackDistance && fDist < mfNormalAttackDistance*2 && mpMover->GetStuckCounter()<0.5f)
			{
				ChangeState(eLuxEnemyState_AttackMeleeLong);
			}
			SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);

		////////////////////////
		// End of current path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			float fDistToPlayer = DistToPlayer();
			
			//Check if was end of path because of stuck.,
			if(apMessage->mlCustomValue==1 && fDistToPlayer >= mfNormalAttackDistance)
			{
				if(PlayerIsDetected() == false)
				{
					//Give some extra time to find player!
					SendMessage(eLuxEnemyMessage_TimeOut_3, 1.5f, true);
				}
				else 
				{
					ChangeState(eLuxEnemyState_HuntWander);
				}
			}
			//Get new path
			else
			{
				mpPathfinder->MoveTo(mvLastKnownPlayerPos);

				if(PlayerIsDetected() == false)
				{
					//When lost player give some extra time to catch up
					SendMessage(eLuxEnemyMessage_TimeOut_3, 1.5f, true);
				}
			}
			

		//////////////////
		// Update path and call for help!
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			mpMap->BroadcastEnemyMessage(eLuxEnemyMessage_HelpMe, true, mpCharBody->GetPosition(), mfActivationDistance*0.5f,
											0,false, mpCharBody->GetFeetPosition());
		
			mpPathfinder->MoveTo(mvLastKnownPlayerPos);
			
			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);	
		
		//////////////////
		//When lost player give some extra time to catch up
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)
			if(PlayerIsDetected() == false)
			{
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
				ChangeState(eLuxEnemyState_Search);
			}
			else
			{
				SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
			}

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	
	////////////////////////////////
	// Hunt Wander (Remain in hunt mode but go to nearby reachable node)
	kLuxState(eLuxEnemyState_HuntWander)
		///////////////////////
		// Enter
		kLuxOnEnter
			mpMover->ResetStuckCounter();

			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 1.2f;

			mfFOVMul = 4.0f;

			cAINode * pNode = mpPathfinder->GetNodeAtPos(mpCharBody->GetFeetPosition(), 2, 5, false, true, true, NULL);
			if(pNode)
				mpPathfinder->MoveTo(pNode->GetPosition());
			else
				ChangeState(eLuxEnemyState_Hunt);

		///////////////////////
		// Enter
		kLuxOnLeave
			mfFOVMul = 1.0f;

		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			ChangeState(eLuxEnemyState_Hunt);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)

		
	////////////////////////////////
	// Hunt Pause (Walk slowly towards the player)
	kLuxState(eLuxEnemyState_HuntPause)	
		///////////////////////
		// Enter
		kLuxOnEnter	
		SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(mfHuntPauseMinTime, mfHuntPauseMaxTime), true);
			mpPathfinder->Stop();
			
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 0.6f;

			mfFOVMul = 4.0f;

			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);

		///////////////////////
		// Enter
		kLuxOnLeave
			mfFOVMul = 1.0f;
		
		///////////////////////
		// Update
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(DistToPlayer2D() > 2.0f)
				mpPathfinder->MoveTo(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());

			if(CanSeePlayer())
			{
				SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);	
			}
		
		///////////////////////
		// Time out
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)	
			ChangeState(eLuxEnemyState_Alert);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	#endif
		
	////////////////////////////////
	// Break door
	kLuxState(eLuxEnemyState_BreakDoor)	
		kLuxOnEnter
			mpPathfinder->Stop();
			PlayAnim("BreakDoor",false, 0.3f);
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mlAttackHitCounter =0; //When returning from door breakage there should be no pause!
			mfFOVMul = 1.0f;

		kLuxOnUpdate
			//Turn towards the door!
			mpMover->TurnToPos(mvTempPos);

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			if(PlayerIsDetected())
			{
				ChangeState(eLuxEnemyState_Alert);
			}
			else if(mpMap->DoorIsBroken(mlStuckDoorID))
			{
				if(mPreviousState == eLuxEnemyState_Hurt)
					ChangeState(eLuxEnemyState_Hunt);
				else 
					ChangeState(mPreviousState);
			}
			else
			{
				PlayAnim("BreakDoor",false, 0.3f);
			}

			
		
		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			Attack(mNormalAttackSize, mBreakDoorAttackDamage);
			

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
        		
	////////////////////////////////
	// Attack Short
	kLuxState(eLuxEnemyState_AttackMeleeShort)
		kLuxOnEnter
			mpPathfinder->Stop();
			int lNum = cMath::RandRectl(1, 2);
			PlayAnim("SwingClaws0"+cString::ToString(lNum),false, 0.3f);
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mfFOVMul = 1.0f;

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			if(mPreviousState == eLuxEnemyState_Hurt)
				ChangeState(eLuxEnemyState_Hunt);
			else 
				ChangeState(mPreviousState);

		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			Attack(mNormalAttackSize, mNormalAttackDamage);
		
		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	
	////////////////////////////////
	// Attack Launch
	kLuxState(eLuxEnemyState_AttackMeleeLong)
		kLuxOnEnter
			mpPathfinder->Stop();
			SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
			mfForwardSpeed *= 1.5f;
			PlayAnim("SwingLaunch",false, 0.3f);
			mlTempVal = 0;
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mfFOVMul = 1.0f;
			SetMoveSpeed(eLuxEnemyMoveSpeed_Run);

		kLuxOnUpdate
			if(mlTempVal==0)
			{
				mpMover->MoveToPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
			}
			


		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			mlTempVal = 1;
			Attack(mNormalAttackSize, mNormalAttackDamage);
		
		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			ChangeState(mPreviousState);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	
	////////////////////////////////
	// Hurt
	kLuxState(eLuxEnemyState_Hurt)
		kLuxOnEnter
			mpPathfinder->Stop();	
			PlayAnim("Flinch",false, 0.5f);
			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mfFOVMul = 1.0f;

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			if(PlayerIsDetected())
			{
				ChangeState(eLuxEnemyState_Hunt);	
			}
			else
			{
				ChangeState(eLuxEnemyState_Alert);	
			}
		
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			if(cMath::RandRectl(0,1)==0 && DistToPlayer() < mfNormalAttackDistance*1.3f)
			{
				ChangeState(eLuxEnemyState_AttackMeleeShort);
			}

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)

	////////////////////////////////
	// Dead (TEMP! USE RAGDOLL)
	kLuxState(eLuxEnemyState_Dead)
		kLuxOnEnter
			mpPathfinder->Stop();
			//PlayAnim("Dead",false, 0.4f);	
			PlayAnim("Dead",false, 0.3f,false,1.0f,false,true,false);	
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

void cLuxEnemy_Grunt::OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)
{
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();

	if(mCurrentState == eLuxEnemyState_AttackMeleeShort)
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

bool cLuxEnemy_Grunt::PlayerIsDetected()
{
	if(CanSeePlayer())
	{
		return true;
	}
	else
	{
		return (DistToPlayer() < mpCharBody->GetSize().x && PlayerInFOV());
	}
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnDisableTriggers()
{
	if(mCurrentState == eLuxEnemyState_Idle) return;

	ChangeState(eLuxEnemyState_Patrol);
}

//-----------------------------------------------------------------------

float cLuxEnemy_Grunt::GetDamageMul(float afAmount, int alStrength)
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

void cLuxEnemy_Grunt::PatrolUpdateGoal()
{
	if(mvPatrolNodes.empty())
	{
		ChangeState(eLuxEnemyState_Wait);
		return;
	}
	
	cLuxEnemyPatrolNode* pPatrolNode = GetCurrentPatrolNode();
	
	mpPathfinder->MoveTo(pPatrolNode->mpNode->GetPosition());
	
	/*if(OutsideStartRadius() && DistToPlayer() > mfActivationDistance * 0.5f)
	{
		ChangeState(eLuxEnemyState_GoHome);
	}
	else
	{
		cAINode * pNode = GetPatrolAroundPlayerNode();
		if(pNode)
			mpPathfinder->MoveTo(pNode->GetPosition());
		else
			ChangeState(eLuxEnemyState_Wait);
	}*/
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::PatrolEndOfPath()
{
	if(IsAtLastPatrolNode())
	{
		if(mbIsSeenByPlayer==false && DistToPlayer() > 10 && mbAutoRemoveAtPathEnd)
		{
            SetActive(false);	

			RunCallbackFunc("OnAutoDisabled");

			return;
		}
	}

	//Log("Current node: %d/%d\n", mlCurrentPatrolNode, mvPatrolNodes.size());

	cLuxEnemyPatrolNode *pNode = GetCurrentPatrolNode();
	if(pNode)
		mfWaitTime = pNode->mfWaitTime;
	else
		mfWaitTime =0;

	ChangeState(eLuxEnemyState_Wait);

	IncCurrentPatrolNode(true);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxEnemy_Grunt_SaveData, iLuxEnemy_SaveData)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxEnemy_Grunt::CreateSaveData()
{
	return hplNew(cLuxEnemy_Grunt_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxEnemy_Grunt_SaveData *pData = static_cast<cLuxEnemy_Grunt_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	//kCopyToVar(pData,mbLit);
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxEnemy_Grunt_SaveData *pData = static_cast<cLuxEnemy_Grunt_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	
	////////////////////////
	// Handle changed enums
	if (mCurrentState >= eLuxEnemyState_PigEnumStart) mCurrentState = eLuxEnemyState_LastEnum;
	if (mNextState >= eLuxEnemyState_PigEnumStart) mNextState = eLuxEnemyState_LastEnum;
	if (mPreviousState >= eLuxEnemyState_PigEnumStart) mPreviousState = eLuxEnemyState_LastEnum;
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}


//-----------------------------------------------------------------------

