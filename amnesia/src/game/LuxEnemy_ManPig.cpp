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

#include "LuxEnemy_ManPig.h"

#include "LuxEnemyMover.h"
#include "LuxEnemyPathfinder.h"

#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxMusicHandler.h"
#include "LuxDebugHandler.h"
#include "LuxGlobalDataHandler.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"
#include "LuxEffectHandler.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemyLoader_ManPig::cLuxEnemyLoader_ManPig(const tString& asName) : iLuxEnemyLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxEnemy *cLuxEnemyLoader_ManPig::CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)
{
	return hplNew(cLuxEnemy_ManPig, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_ManPig::LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)
{
	cLuxEnemy_ManPig *pManPig = static_cast<cLuxEnemy_ManPig*>(apEnemy);

	pManPig->mfDefaultForwardSpeed[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Walk] =	GetVarFloat("Quadruped_Walk_ForwardSpeed", 0);
	pManPig->mfDefaultBackwardSpeed[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Walk] =	GetVarFloat("Quadruped_Walk_BackwardSpeed", 0);
	pManPig->mfDefaultForwardAcc[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Walk] =		GetVarFloat("Quadruped_Walk_ForwardAcc", 0);
	pManPig->mfDefaultForwardDeacc[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Walk] =	GetVarFloat("Quadruped_Walk_ForwardDeacc", 0);

	pManPig->mfDefaultForwardSpeed[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Run] =		GetVarFloat("Quadruped_Run_ForwardSpeed", 0);
	pManPig->mfDefaultBackwardSpeed[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Run] =	GetVarFloat("Quadruped_Run_BackwardSpeed", 0);
	pManPig->mfDefaultForwardAcc[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Run] =		GetVarFloat("Quadruped_Run_ForwardAcc", 0);
	pManPig->mfDefaultForwardDeacc[eLuxEnemyPoseType_Quadruped][eLuxEnemyMoveSpeed_Run] =		GetVarFloat("Quadruped_Run_ForwardDeacc", 0);

	pManPig->mfStoppedToWalkSpeed[eLuxEnemyPoseType_Quadruped] =	GetVarFloat("Quadruped_StoppedToWalkSpeed", 0);
	pManPig->mfWalkToStoppedSpeed[eLuxEnemyPoseType_Quadruped] =	GetVarFloat("Quadruped_WalkToStoppedSpeed", 0);
	pManPig->mfWalkToRunSpeed[eLuxEnemyPoseType_Quadruped] =		GetVarFloat("Quadruped_WalkToRunSpeed", 0);
	pManPig->mfRunToWalkSpeed[eLuxEnemyPoseType_Quadruped] =		GetVarFloat("Quadruped_RunToWalkSpeed", 0);

	pManPig->msNoticeSound = GetVarString("NoticeSound");
	pManPig->msGiveUpNoticeSound = GetVarString("GiveUpNoticeSound");
	pManPig->msEnabledSound = GetVarString("EnabledSound");
	pManPig->msChaseSound = GetVarString("ChaseSound");
	gpBase->PreloadSound(pManPig->msNoticeSound);
	gpBase->PreloadSound(pManPig->msEnabledSound);
	gpBase->PreloadSound(pManPig->msChaseSound);

	pManPig->mfGroggyDamageCount = GetVarFloat("GroggyDamageCount", 0);
	pManPig->mfAlertToHuntDistance = GetVarFloat("AlertToHuntDistance", 0);
	pManPig->mfAlertToInstantHuntDistance = GetVarFloat("AlertToInstantHuntDistance", 0);
	pManPig->mfHuntPauseMinTime = GetVarFloat("HuntPauseMinTime", 0);
	pManPig->mfHuntPauseMaxTime = GetVarFloat("HuntPauseMaxTime", 0);
	pManPig->mfIncreaseAlertSpeedDistance = GetVarFloat("IncreaseAlertSpeedDistance", 0);
	pManPig->mfIncreasedAlertSpeedMul = GetVarFloat("IncreasedAlertSpeedMul", 0);
	
	pManPig->mfAlertRunTowardsToHuntLimit = GetVarFloat("AlertRunTowardsToHuntLimit", 0);
	pManPig->mfAlertRunTowardsCheckDistance = GetVarFloat("AlertRunTowardsCheckDistance", 0);

	pManPig->msTeslaMindFuckLoop = GetVarString("TeslaSoundLoop");
	pManPig->mbIsTelsa = GetVarBool("IsTelsa", false);
}

//-----------------------------------------------------------------------

static eLuxIdleBehavior ToIdleBehavior(const tString& asStr)
{
	if(asStr == "None") return eLuxIdleBehavior_None;
	if(asStr == "Stalk") return eLuxIdleBehavior_Stalk;
	if(asStr == "Track") return eLuxIdleBehavior_Track;

	Error("eLuxIdleBehavior behavior '%s' does not exist!\n", asStr.c_str());
	return eLuxIdleBehavior_None;
}

static eLuxEnemyMoveSpeed ToMoveSpeed(const tString& asStr)
{
	if(asStr == "Run") return eLuxEnemyMoveSpeed_Run;
	if(asStr == "Walk") return eLuxEnemyMoveSpeed_Walk;

	Error("eLuxEnemyMoveSpeed '%s' does not exist, falling back to walk!\n", asStr.c_str());
	return eLuxEnemyMoveSpeed_Walk;
}

void cLuxEnemyLoader_ManPig::LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)
{
	cLuxEnemy_ManPig *pManPig = static_cast<cLuxEnemy_ManPig*>(apEnemy);

	pManPig->mbThreatenOnAlert = apInstanceVars->GetVarBool("ThreatenOnAlert", false);
	pManPig->mbFleeFromPlayer = apInstanceVars->GetVarBool("FleeFromPlayer", false);
	pManPig->mbAutoDisableAfterFlee = apInstanceVars->GetVarBool("AutoDisableAfterFlee", false);
	pManPig->mIdleBehavior = ToIdleBehavior(apInstanceVars->GetVarString("IdleBehavior", "None"));
	pManPig->mPatrolMoveSpeed = ToMoveSpeed(apInstanceVars->GetVarString("PatrolMoveSpeed", "Walk"));
	pManPig->mbAllowZeroWaitTime = apInstanceVars->GetVarBool("AllowZeroNodeWaitTimes", false);
	pManPig->mfDamageMul = apInstanceVars->GetVarFloat("DamageMul", 1.0f);
	pManPig->mfRunSpeedMul = apInstanceVars->GetVarFloat("RunSpeedMul", 1.0f);
	pManPig->mbPlayActivateSound = apInstanceVars->GetVarBool("PlayActivateSound", true);
	pManPig->mfLanternSensitivity = apInstanceVars->GetVarFloat("LanternSensitivity", 1.0f);
	pManPig->mfHuntPauseTimeMul = apInstanceVars->GetVarFloat("HuntPauseTimeMul", 1.0f);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemy_ManPig::cLuxEnemy_ManPig(const tString &asName, int alID, cLuxMap *apMap) : iLuxEnemy(asName,alID,apMap, eLuxEnemyType_ManPig)
{
	mfWaitTime =0;
	mfAlertRunTowardsCount = 0;

	mfCheckFlashLightShining =0;

	mfFleeCheckIfInvisbleCount=0;
	
	mbAlignEntityWithGroundRay = true;

	mbIsTelsa=false;

	mbLastShortAttackWasMiss = false;
	mbForceChargeAttack = false;

	mfTeslaFlickerTimer=0;
	mlTeslaFlickerState=0;
	mfTeslaEffectAmount = 1.0f;
	mfTeslaSpecialNoticeCount =0;
	mbTeslaMindFuckActive = false;
	mfTeslaMindFuckPulse =0;
	mfTeslaMindFuckPulseAdd=1;
	mbTeslaTerror = false;
	mfBlackOutCount=0;
	mfBlackOutDurationCount=0;
	mbTeslaForceSighting = false;
	mbTeslaFadeDisabled = false;
	mbTeslaSoundDisabled = false;
	mbTeslaEasyEscapeDisabled = false;

	mbThreatenOnAlert = false;
	mbFleeFromPlayer = false;
	mIdleBehavior = eLuxIdleBehavior_None;

	mCurrentMoveType = eLuxEnemyMoveType_Normal;

	mfDamageMul = 1.0f;
	mfRunSpeedMul = 1.0f;

	mpHuntSound = NULL;
	mlHuntSoundId = -1;

	mpMindFuckSound = NULL;
	mlMindFuckSoundId = -1;

	for(int i=0; i<eLuxEnemyMoveType_LastEnum; ++i)
	{
		eLuxEnemyPoseType pose = eLuxEnemyPoseType_Biped;
		msIdleAnimationName[i][pose] = "IdleBiped";
		msWalkAnimationName[i][pose] = "WalkBiped";
		msRunAnimationName[i][pose] =  i==eLuxEnemyMoveType_Normal ? "RunBiped" : "FleeBiped";;

		pose = eLuxEnemyPoseType_Quadruped;
		msIdleAnimationName[i][pose] = "IdleQuadruped";
		msWalkAnimationName[i][pose] = "WalkQuadruped";
		msRunAnimationName[i][pose] = i==eLuxEnemyMoveType_Normal ? "RunQuadruped" : "FleeQuadruped";
	}
}

//-----------------------------------------------------------------------

cLuxEnemy_ManPig::~cLuxEnemy_ManPig()
{
	if(mbIsTelsa) ResetMindFuckEffects();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::OnSetupAfterLoad(cWorld *apWorld)
{
	if(mbIsTelsa) mpMeshEntity->SetVisible(false);
}


//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::OnAfterWorldLoad()
{
	mvDefaultLightColors.clear();
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		mvDefaultLightColors.push_back(mvLights[i]->GetDiffuseColor());
	}

	
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::UpdateEnemySpecific(float afTimeStep)
{
	if(mbIsTelsa) UpdateTesla(afTimeStep);
	
	UpdateCheckInLantern(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::ChangePose(eLuxEnemyPoseType aPose, bool abSendMessage)
{
	if(mCurrentPose == aPose) return;

	//Make sure the enemy is still and stays so for 0.5 seconds, and animation is updated for that.
	mpCharBody->StopMovement();
	mpCharBody->SetMoveDelay(0.5);
	mpMover->UpdateMoveAnimation(0.001f);

	//Set new pose state
	eLuxEnemyPoseType prevPose = mCurrentPose;
	mCurrentPose = aPose;
		
	//Make sure that the correct animation is set.
	mpMover->mMoveState = eLuxEnemyMoveState_LastEnum;
	mpMover->UpdateMoveAnimation(0.001f);

	if(abSendMessage)
		SendMessage(eLuxEnemyMessage_ChangePose, 0, false, 0,0, aPose);
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::ChangeMoveType(eLuxEnemyMoveType aMoveType)
{
	if(mCurrentMoveType == aMoveType) return;

	mCurrentMoveType = aMoveType;

	mpMover->mMoveState = eLuxEnemyMoveState_LastEnum;
	mpMover->UpdateMoveAnimation(0.001f);
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)
{
	kLuxBeginStateMachine
	
	////////////////////////////////
	// Default
	////////////////////////////////

	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_Reset)
		
		gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
		
		gpBase->mpPlayer->RemoveTerrorEnemy(this);

		mbLastShortAttackWasMiss = false;
		mbForceChargeAttack = false;
		

		ChangeState(eLuxEnemyState_Wait);

	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		ChangeState(eLuxEnemyState_Alert);
	
	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_TakeHit)
		/*mfDamageCount += apMessage->mfCustomValue;
		if(mfDamageCount > mfGroggyDamageCount)
		{
			ChangeState(eLuxEnemyState_Hurt);
			mfDamageCount =0;
		}*/
	
	//------------------------------

	//If enemy is out of range (having been in, then turn him off)
	kLuxOnMessage(eLuxEnemyMessage_PlayerOutOfRange)
		SetActive(false);

	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_PlayerDead)
		ChangeState(eLuxEnemyState_Idle);	

	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_HelpMe)
		ShowPlayerPosition();
		ChangeState(eLuxEnemyState_Hunt);

	//------------------------------

	kLuxOnMessage(eLuxEnemyMessage_ChangePose)
		
		

	//------------------------------

	////////////////////////////////
	// Idle
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Idle)
		kLuxOnEnter
			mbPlayerInRange = false; //Reset so new checks are made in case player is near.
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

		kLuxOnMessage(eLuxEnemyMessage_PlayerInRange)
			gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName)+_W(" enabled!"), false );
			
			if(gpBase->mpGlobalDataHandler->GetEnemyActivateSoundAllowed())
			{
				if(mbPlayActivateSound)
				{
					gpBase->mpGlobalDataHandler->SetEnemyActivateSoundMade();
					PlaySound(msEnabledSound);
				}
			}
			

			ChangeState(eLuxEnemyState_Patrol);

		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
			//Nothing

	//------------------------------
	
	////////////////////////////////
	// Go Home
	////////////////////////////////

	//------------------------------

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
	
	//------------------------------

	////////////////////////////////
	// Wait
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Wait)
		kLuxOnEnter
			if(mfWaitTime <= 0)
			{
				//gpBase->mpDebugHandler->AddMessage(_W("wait time=0"), false);
				if(mbAllowZeroWaitTime)
				{
					//gpBase->mpDebugHandler->AddMessage(_W("Zero wait time!!"), false);
					SendMessage(eLuxEnemyMessage_TimeOut, 0.01f, true);
				}
				else
					SendMessage(eLuxEnemyMessage_TimeOut, cMath::RandRectf(1, 3), true);
			}
			else
			{
				SendMessage(eLuxEnemyMessage_TimeOut, mfWaitTime, true);
			}
			mfWaitTime =0;

			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(2, 5), true);

			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mpPathfinder->Stop();

		kLuxOnLeave
			mpMover->SetOverideMoveState(false);

		//------------------------------
		
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(mPreviousState == eLuxEnemyState_Search ||
				mPreviousState == eLuxEnemyState_Stalk ||
				mPreviousState == eLuxEnemyState_Track)
			{
				FadeOutCurrentAnim(0.2f);
				ChangeState(mPreviousState);	
			}
			else
			{
				if(GetPatrolNodeNum()>0 || mIdleBehavior!=eLuxIdleBehavior_None)
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
			int lRand = cMath::RandRectl(1,2);
			PlayAnim("Idle"+GetCurrentPoseSuffix()+"Extra"+cString::ToString(lRand),false, 0.3f);

		//------------------------------

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(4, 13), true);
	
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

	//------------------------------

	////////////////////////////////
	// Patrol
	////////////////////////////////

	//------------------------------
	kLuxState(eLuxEnemyState_Patrol)
		kLuxOnEnter
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
			
			gpBase->mpPlayer->RemoveTerrorEnemy(this);
			
			ChangeSoundState(eLuxEnemySoundState_Idle);
			SetMoveSpeed(mPatrolMoveSpeed);
			if(mPatrolMoveSpeed==eLuxEnemyMoveSpeed_Run) mfForwardSpeed *= mfRunSpeedMul;
			PatrolUpdateGoal();


		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
		
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)

			/////////////////////////////////////
			//If at end, check if we want to remove
			if(IsAtLastPatrolNode() && CheckEnemyAutoRemoval(10))
			{
				//Do nothing...
			}
			////////////////////////////////////
			// Do stuff for node and go to next.
			else
			{
				cLuxEnemyPatrolNode *pNode = GetCurrentPatrolNode();
				if(pNode)
				{
					//////////////////////////////////
					// No animation is played
					if(pNode->msAnimation=="")
					{
						mfWaitTime = pNode->mfWaitTime;
						ChangeState(eLuxEnemyState_Wait);
					}
					//////////////////////////////////
					//Play animation
					else
					{
						PlayAnim(pNode->msAnimation, pNode->mbLoopAnimation, 0.2f);
					
						//////////////////////////////////
						// Animation is looping, check the end with timer
						if(pNode->mbLoopAnimation)
						{
							SendMessage(eLuxEnemyMessage_TimeOut, pNode->mfWaitTime,true);
							mfWaitTime =0;
						}
						//////////////////////////////////
						// Animation is NOT looping, check end with AnimationOver message and wait after anim is done.
						else
						{
							mfWaitTime = pNode->mfWaitTime;
						}
					}
				}
				else
				{
					mfWaitTime =0;
					ChangeState(eLuxEnemyState_Wait);
				}

				IncCurrentPatrolNode(true);
			}

		//////////////////////////
		//Non looping animation ends
		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			ChangeState(eLuxEnemyState_Wait);

		//////////////////////////
		//Looping animation shall end
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			mpMover->UseMoveStateAnimations();
			ChangeState(eLuxEnemyState_Wait);

		//////////////////////////
		//Sound heard
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}
	
	//------------------------------
	
	////////////////////////////////
	// Investigate
	////////////////////////////////

	//------------------------------	
	kLuxState(eLuxEnemyState_Investigate)
		kLuxOnEnter	
			ChangeSoundState(eLuxEnemySoundState_Alert);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			
			mpPathfinder->Stop();
			PlayAnim("Notice"+GetCurrentPoseSuffix(), false, 0.3f);

			//mfForwardSpeed *= 1.5f;
			PlaySound(msNoticeSound);

			mfFOVMul = 4.0f; //When hearing a sound, enemy gets extra alert.

			gpBase->mpDebugHandler->AddMessage(_W("Sound Heard! Vol: ")+cString::ToStringW(mfTempVal),false);

			ForceTeslaSighting();

		kLuxOnLeave
				mfFOVMul = 1.0f;
			FadeOutCurrentAnim(0.2f);
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);


		//------------------------------

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

		//------------------------------

		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			
			PlaySound(msGiveUpNoticeSound);

			if(mPreviousState == eLuxEnemyState_Search)
				ChangeState(eLuxEnemyState_Search);	
			else
				ChangeState(eLuxEnemyState_Patrol);	

		//------------------------------
			
		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}

		//------------------------------

		
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			//If a new sound is loader than the previous go for that instead!
			if(mpPathfinder->IsMoving() && apMessage->mfCustomValue > mfTempVal)
			{
				cAINode *pNode = mpPathfinder->GetNodeAtPos(mvTempPos, 0, 10, true, true, true,NULL,1);
				if(pNode) 
					mpPathfinder->MoveTo(pNode->GetPosition());
				
				mfTempVal = apMessage->mfCustomValue;
			}

	//------------------------------

	////////////////////////////////
	// Alert
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Alert)
		//////////////
		//Enter
		kLuxOnEnter
			
			ForceTeslaSighting();

			/////////////////////////
			// Flee
			if(mbFleeFromPlayer && mbThreatenOnAlert==false)
			{
				ChangeState(eLuxEnemyState_Flee);
			}
			/////////////////////////
			// Go into alert!
			else
			{
				/////////////////////////
				// Set up sound
				ChangeSoundState(eLuxEnemySoundState_Alert);

				gpBase->mpPlayer->AddTerrorEnemy(this);
				//gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Search,this);

				/////////////////////////
				// Stay and threaten
				if(mbThreatenOnAlert)
				{
					float fThreatLength = mbFleeFromPlayer ? 3.0f : 6.0f;
					float fThreatFristStopCheck = mbFleeFromPlayer ? 1.0f : 3.5f;

					mpPathfinder->Stop();
					
					PlayAnim("ThreatLoop"+GetCurrentPoseSuffix(), true, 0.3f);		
					
					SendMessage(eLuxEnemyMessage_TimeOut_2, fThreatLength, true); //Attack!!
					SendMessage(eLuxEnemyMessage_TimeOut_3, fThreatFristStopCheck, true);//Check if stop the threat
					mbSkipVisibilityRangeHandicaps = true;
				}
				/////////////////////////
				// Move towards player
				else
				{
					mpPathfinder->MoveTo(mvLastKnownPlayerPos);
					SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
					//mfForwardSpeed *= 1.2f;
					SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true); //To update path
				}

				/////////////////////////
				// Set up properties
				mfFOVMul = 4.0f;

				mfAlertRunTowardsCount =0;
			}

		//////////////
		//Leave
		kLuxOnLeave	

			mbSkipVisibilityRangeHandicaps = false;

			if(mbThreatenOnAlert)
			{
				mpCharBody->SetMoveDelay(0.6f);
				mpMover->SetOverideMoveState(false);
			}

			mfFOVMul = 1.0f;
			if(mNextState != eLuxEnemyState_BreakDoor)
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
			}

		//------------------------------
		
		//////////////
		//Enter
		kLuxOnUpdate
			float fDistToPlayer = DistToPlayer();

			//////////////////////////////
			//Turn towards player
			if(mbThreatenOnAlert && PlayerIsDetected())
			{
				float fAngleDist = cMath::GetAngleDistanceRad(	mpCharBody->GetYaw()+kPif, 
																gpBase->mpPlayer->GetCharacterBody()->GetYaw());
				if(fabs(fAngleDist)>cMath::ToRad(30))
					mpMover->TurnToPos(GetPlayerFeetPos());
			}

			//////////////////////////////
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

			//////////////////////////////
			//Update the speed of movement.
			if(fDistToPlayer > mfIncreaseAlertSpeedDistance)
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				//mfForwardSpeed *= 1.2f * mfIncreasedAlertSpeedMul;
				mfForwardSpeed *= mfIncreasedAlertSpeedMul;
			}
			else
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
				//mfForwardSpeed *= 1.2f;
			}
						
			//////////////////////////////
			//Stuck at door, break it
			if(gpBase->mpPlayer->GetTerror() >= 1 && mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
			//////////////////////////////
			//Player is no longer seen, see if time to search or wait
			else if(PlayerIsDetected()==false)
			{
				float fTerror = gpBase->mpPlayer->GetTerror();
				if (mbIsTelsa == true)
				{
					if (fTerror < 0.1) ChangeState(eLuxEnemyState_Wait);
				}
				else if(fTerror < 0.45)
				{
					ChangeState(eLuxEnemyState_Wait);
				}
				else if(fTerror < 1)
				{
					ChangeState(eLuxEnemyState_Search);
				}
			}
			//////////////////////////////
			//Player is seen, see if close enough for hunt
			else
			{
				if(mbThreatenOnAlert)
				{
					//////////////
					//Flee
					if(mbFleeFromPlayer)
					{
						//If close enough or running quicly towards enemy
						if(mfAlertRunTowardsCount>mfAlertRunTowardsToHuntLimit || fDistToPlayer < mfAlertToInstantHuntDistance*2)
						{
							if(cMath::RandRectl(0,3)>0)
								ChangeState(eLuxEnemyState_Flee);
							else
								ChangeState(eLuxEnemyState_Hunt);
						}
					}
					//////////////
					//Attack
					else
					{
						//If terror is topped and distance to player is under a value or player is running towards piggie
						//Or if distance to player is less than a value
						if( (gpBase->mpPlayer->GetTerror() >= 1 && ( mfAlertRunTowardsCount>mfAlertRunTowardsToHuntLimit || fDistToPlayer < mfAlertToInstantHuntDistance*2)) ||
							fDistToPlayer < mfAlertToInstantHuntDistance)
						{
							gpBase->mpPlayer->SetTerror(1.0f);
							
							ChangeState(eLuxEnemyState_Hunt);
						}
					}
				}
				else
				{
					//If terror is topped and distance to player is over a value or player is running towards piggie
					//Or if distance to player is less than a value
					float fTerror = gpBase->mpPlayer->GetTerror();
					if (mbIsTelsa==true) fTerror *= 3;
					if(	(fTerror >= 1 && (fDistToPlayer > mfAlertToHuntDistance || mfAlertRunTowardsCount>mfAlertRunTowardsToHuntLimit) ) || 
						fDistToPlayer < mfAlertToInstantHuntDistance)
					{
						gpBase->mpPlayer->SetTerror(1.0f);

						ChangeState(eLuxEnemyState_Hunt);
					}
				}
			}

		//------------------------------

		//////////////
		//Reach end of path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			if(mbThreatenOnAlert==false)
			{
				float fDistToPlayer = DistToPlayer();

				//Path ended and player is not seen or enemy is stuck (this should only happen when at a distance!
				if(PlayerIsDetected()==false || (apMessage->mlCustomValue == 1 && fDistToPlayer>5))
				{
					if(mbIsTelsa==false)
						ChangeState(eLuxEnemyState_Search);
					else
						ChangeState(eLuxEnemyState_Wait);
				}
				else if(apMessage->mlCustomValue==1 && PlayerIsDetected())
				{
					//This is when the enemy should just stnad still but I think nothing is really needed
				}
			}
		
		//------------------------------		

		//////////////
		//Update move to
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			if(PlayerIsDetected())
				mpPathfinder->MoveTo(mvLastKnownPlayerPos);
			
			SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);

		//------------------------------

		//////////////
		//Start Hunt
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			if(mbFleeFromPlayer)
			{
				ChangeState(eLuxEnemyState_Flee);
			}
			else
			{
				gpBase->mpPlayer->SetTerror(1.0f);
				ChangeState(eLuxEnemyState_Hunt);
			}

		/////////////
		//Check if threat should end
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)
			if(PlayerIsDetected()==false)
				ChangeState(eLuxEnemyState_Patrol);
			else
				SendMessage(eLuxEnemyMessage_TimeOut_3, 0.75f, true);


		//------------------------------

		//////////////
		//Takes a hit
		kLuxOnMessage(eLuxEnemyMessage_TakeHit)
			ChangeState(eLuxEnemyState_Hurt);
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);

		//------------------------------

		///////////
		//Overload
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)

	//------------------------------

	////////////////////////////////
	// Search
	////////////////////////////////

	//------------------------------	
	
	kLuxState(eLuxEnemyState_Search)
		kLuxOnEnter
			ForceTeslaSighting();

			ChangeSoundState(eLuxEnemySoundState_Alert);

			SendMessage(eLuxEnemyMessage_TimeOut, mfPlayerSearchTime, true);
			
			SendMessage(eLuxEnemyMessage_TimeOut_2,cMath::RandRectf(0,1), true);
		
			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
			gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Search,this);
			
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 1.0f;

		kLuxOnLeave
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

		//------------------------------
		
		kLuxOnUpdate
			if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
		
		//------------------------------

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
	
	//------------------------------

	////////////////////////////////
	// Flee
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Flee)
		///////////////////////
		// Enter
		kLuxOnEnter
			IncCurrentPatrolNode(true);

			if(FleeTryToFindSafeNode())
			{
				mfFleeCheckIfInvisbleCount =0;
				mfFOVMul=5;

				SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
				mfForwardSpeed *= mfRunSpeedMul;
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
				
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
				
				SendMessage(eLuxEnemyMessage_TimeOut, 2.0f, true); //Check if player is too close!
				SendMessage(eLuxEnemyMessage_TimeOut_2, 0.2f, true); //Check if not seen by player

				//ChangeMoveType(eLuxEnemyMoveType_Flee);
			}
			else
			{
				ChangeState(eLuxEnemyState_Hunt);
			}
		
		///////////////////////
		// Leave
		kLuxOnLeave
			//ChangeMoveType(eLuxEnemyMoveType_Normal);
			mfFOVMul=1;

		//------------------------------

		//////////////////
		// Check if player is too close!
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(DistToPlayer2D()<2.5f)
			{
				ChangeState(eLuxEnemyState_Hunt);
			}
			
			SendMessage(eLuxEnemyMessage_TimeOut, 1.0f, true);

		//------------------------------
		
		//////////////////
		// Check if no longer visible.
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)

			if(PlayerIsDetected()==false)	mfFleeCheckIfInvisbleCount += 0.2f;
			else							mfFleeCheckIfInvisbleCount = 0;

			if(mfFleeCheckIfInvisbleCount>=1.6)
			{
				if(CheckEnemyAutoRemoval(15)==false) ChangeState(eLuxEnemyState_Wait);
			}
			else
			{
				SendMessage(eLuxEnemyMessage_TimeOut_2, 0.2f, true);
			}
		
			
		//------------------------------
		
		////////////////////////
		// End of current path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)

			//Check if pig can be auto removed, else continue fleeing or, go on patrol
			if(mbAutoDisableAfterFlee==false || CheckEnemyAutoRemoval(15)==false)
			{
				if(PlayerIsDetected())
				{
					if(FleeTryToFindSafeNode()==false)
						ChangeState(eLuxEnemyState_Alert);
				}
				else
				{
					ChangeState(eLuxEnemyState_Wait);
				}
			}

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
		
	//------------------------------

	////////////////////////////////
	// Stalk
	// - The enemy walks close to the player but tries to avoid being seen or forced to go into alert mode.
	////////////////////////////////

	//------------------------------
	
	kLuxState(eLuxEnemyState_Stalk)	
		///////////////////////
		// Enter
		kLuxOnEnter
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			SendMessage(eLuxEnemyMessage_TimeOut, 2.5f, true);
			mfFOVMul =0.5f; //We want small FOV so that we do not go into alert unless needed.

			if(StalkFindNode()==false)
			{
				mfWaitTime = cMath::RandRectf(2, 4);
				ChangeState(eLuxEnemyState_Wait);
			}

		///////////////////////
		// Leave
		kLuxOnLeave
			mfFOVMul = 1.0f;
		//------------------------------

		////////////////////////////////
		//See if player see the enemy, might
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			bool bSeenByPlayer = IsSeenByPlayer();
			float fPlayerDistance = DistToPlayer();

			///////////////////////////
			// Select the move speed
			if(fPlayerDistance < 7.5f || bSeenByPlayer)
			{
				SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
				mfForwardSpeed *= mfRunSpeedMul;
			}
			else
				SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

			///////////////////////////
			// See if a new node needs to be found.
			if(	fPlayerDistance < 4.5f || 
				bSeenByPlayer ||
				IsVisibleToPlayerAtFeetPos(mpPathfinder->GetNextGoalPos()) ||
				IsVisibleToPlayerAtFeetPos(mpPathfinder->GetFinalGoalPos())
				)
			{
				if(StalkFindNode()==false)
				{
					mfWaitTime = cMath::RandRectf(1, 3);
					ChangeState(eLuxEnemyState_Wait);
				}
			}
			///////////////////////////
			// If player is too close, we need to go into alert
			if(fPlayerDistance < 6.0f && bSeenByPlayer)
				ChangeState(eLuxEnemyState_Alert);

			///////////////////////////
			// Repeat check after 2.5 seconds.
			SendMessage(eLuxEnemyMessage_TimeOut, 2.5f, true);

		
		//------------------------------
		
		////////////////////////
		// End of current path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			mfWaitTime = cMath::RandRectf(2, 4);
			ChangeState(eLuxEnemyState_Wait);
		//------------------------------
		
		////////////////////////
		// Player detected
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
			
			//Quickly check if we need new position
			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true); 
	

		//------------------------------
	
	//------------------------------

	////////////////////////////////
	// Track
	// - The enemy is out to get the player, but do so undiscovered and coming up from behind.
	////////////////////////////////

	//------------------------------
	kLuxState(eLuxEnemyState_Track)	
		///////////////////////
		// Enter
		kLuxOnEnter
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			SendMessage(eLuxEnemyMessage_TimeOut, 2.5f, true);
			
			if(TrackFindNode()==false)
			{
				mfWaitTime = cMath::RandRectf(1, 2);
				ChangeState(eLuxEnemyState_Wait);
			}

		///////////////////////
		// Leave
		kLuxOnLeave
			mfFOVMul = 1.0f;

		//------------------------------

		//////////////////////////
		//Check if enemy might be seen
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(	mbIsTelsa == false &&
				IsInPlayerFovAtFeetPos(mpCharBody->GetFeetPosition())==false &&
				//IsVisibleToPlayerAtFeetPos(mpCharBody->GetFeetPosition())==false &&
				DistToPlayer2D() > 6.0f)
			{
				if(TrackTeleportBehindPlayer())
				{
					mpPathfinder->Stop();

					if(TrackFindNode()==false)
					{
						mfWaitTime = cMath::RandRectf(1, 2);
						ChangeState(eLuxEnemyState_Wait);
					}
				}

			}
			SendMessage(eLuxEnemyMessage_TimeOut, 2.5f, true);

		
		//------------------------------
		
		////////////////////////
		// End of current path
		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			if(TrackFindNode()==false)
			{
				if(DistToPlayer2D() < 4.0f)
				{
					mfWaitTime = cMath::RandRectf(2, 6);
					ChangeState(eLuxEnemyState_Wait);
				}
				else if(DistToPlayer2D() < 6.0f)
				{
					mfWaitTime = cMath::RandRectf(1, 3);
					ChangeState(eLuxEnemyState_Wait);
				}
				else
				{
					if(TrackFindNode()==false)
					{
						mfWaitTime = cMath::RandRectf(1, 2);
						ChangeState(eLuxEnemyState_Wait);
					}
				}
				
			}
		
		//------------------------------

		////////////////////////
		// Sound Heard
		kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
			if(apMessage->mfCustomValue > mfHearVolume)
			{
				ChangeState(eLuxEnemyState_Investigate);
				mvTempPos = apMessage->mvCustomValue;
				mfTempVal = apMessage->mfCustomValue;
			}

		//------------------------------

	//------------------------------


	////////////////////////////////
	// Hunt
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Hunt)	
		///////////////////////
		// Enter
		kLuxOnEnter
			ForceTeslaSighting();

			gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);

			int lMaxHits = mbIsTelsa ? 1 : cMath::RandRectl(1, 3);
			if(mlAttackHitCounter >= lMaxHits || mbLastShortAttackWasMiss)
			{
				mlAttackHitCounter =0;
								
				ChangeState(eLuxEnemyState_HuntPause);
				mbLastShortAttackWasMiss = false;
			}
			else
			{
				ChangeSoundState(eLuxEnemySoundState_Hunt);
				SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
				mfForwardSpeed *= mfRunSpeedMul;
				SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);
				mfFOVMul = 4.0f;
				
				gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack,this);
				
				gpBase->mpPlayer->AddTerrorEnemy(this);
				
				mpPathfinder->MoveTo(mvLastKnownPlayerPos);

				SendMessage(eLuxEnemyMessage_TimeOut_2, 0.5f, true);
				SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.25f, 1.0f), true);
			}
			

		///////////////////////
		// Leave
		kLuxOnLeave
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfFOVMul = 1.0f;

		//------------------------------
		
		///////////////////////
		// Update
		kLuxOnUpdate
			//////////////////////////
			//Short attack
			float fDistToPlayer = DistToPlayer();
			if(CanSeePlayer() && fDistToPlayer < mfNormalAttackDistance)
			{
				if(mbForceChargeAttack)
				{
					ChangeState(eLuxEnemyState_AttackMeleeLong);
					mbForceChargeAttack =false;
				}
				else
				{
					ChangeState(eLuxEnemyState_AttackMeleeShort);
				}
			}
			else if(mbStuckAtDoor)// && mpMap->DoorIsClosed(mlStuckDoorID))
			{
				iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
				mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();
				ChangeState(eLuxEnemyState_BreakDoor);
			}
		
		//------------------------------
		
		/////////////////////////////
		// Check if close enough for launch attack
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
			//////////////////////////
			//Launch attack
			float fDist = DistToPlayer();
			if(CanSeePlayer() && fDist > mfNormalAttackDistance && fDist < mfNormalAttackDistance*4.0f && mpMover->GetStuckCounter()<0.5f)
			{
				ChangeState(eLuxEnemyState_AttackMeleeLong);
			}
			SendMessage(eLuxEnemyMessage_TimeOut_2, 1.5f, true);

		/////////////////////////////
		// Sound
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)
			
			PlayHuntSound();
			SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.5f, 2.0f), true);

		//------------------------------

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
					SendMessage(eLuxEnemyMessage_TimeOut_4, 1.5f, true);
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
					SendMessage(eLuxEnemyMessage_TimeOut_4, 1.5f, true);
				}
			}
			
		//------------------------------

		//////////////////
		// Update path and call for help!
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			mpMap->BroadcastEnemyMessage(eLuxEnemyMessage_HelpMe, true, mpCharBody->GetPosition(), mfActivationDistance*0.5f,
											0,false, mpCharBody->GetFeetPosition());
		
			mpPathfinder->MoveTo(mvLastKnownPlayerPos);
			
			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);

		//------------------------------
		
		//////////////////
		//When lost player give some extra time to catch up
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_4)
			if(PlayerIsDetected() == false)
			{
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
				
				gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);

				if(mbThreatenOnAlert || mbIsTelsa)
					ChangeState(eLuxEnemyState_Patrol);
				else
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
	
	//------------------------------
	
	///////////////////////////////
	// Hunt Wander (Remain in hunt mode but go to nearby reachable node)
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_HuntWander)
		///////////////////////
		// Enter
		kLuxOnEnter

			ForceTeslaSighting();

			mpMover->ResetStuckCounter();

			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 1.2f;

			mfFOVMul = 4.0f;

			cAINode * pNode = mpPathfinder->GetNodeAtPos(mpCharBody->GetFeetPosition(), 2, 5, false, true, true, NULL);
			if(pNode)
				mpPathfinder->MoveTo(pNode->GetPosition());
			else
				ChangeState(eLuxEnemyState_Hunt);

			SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.5f, 2.0f), true);

		///////////////////////
		// Leave
		kLuxOnLeave
			mfFOVMul = 1.0f;
		
		//------------------------------

		/////////////////////////////
		// Sound
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)

			PlayHuntSound();
			SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.5f, 3.0f), true);

		//------------------------------

		kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
			ChangeState(eLuxEnemyState_Hunt);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)

	//------------------------------

	////////////////////////////////
	// Hunt Pause (Walk slowly towards the player)
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_HuntPause)	
		///////////////////////
		// Enter
		kLuxOnEnter	

			ForceTeslaSighting();

			SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(mfHuntPauseMinTime, mfHuntPauseMaxTime)*mfHuntPauseTimeMul, true);
			mpPathfinder->Stop();
			
			SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
			mfForwardSpeed *= 0.6f;

			mfFOVMul = 4.0f;

			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
			SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.5f, 3.0f), true);

		///////////////////////
		// Leave
		kLuxOnLeave
			mfFOVMul = 1.0f;

		//------------------------------
		
		///////////////////////
		// Update
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			
			if(DistToPlayer2D() > 2.0f)
				mpPathfinder->MoveTo(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
			else
				mpMover->TurnToPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());

			if(CanSeePlayer())
			{
				SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);	
			}
		
		///////////////////////
		// Time out
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)	
			ChangeState(eLuxEnemyState_Alert);

		/////////////////////////////
		// Sound
		kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)

			PlayHuntSound();
			SendMessage(eLuxEnemyMessage_TimeOut_3, cMath::RandRectf(0.5f, 3.0f), true);

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
		
	//------------------------------

	////////////////////////////////
	// Break door
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_BreakDoor)	
		kLuxOnEnter
			mpPathfinder->Stop();
			PlayAnim("Attack"+GetCurrentPoseSuffix()+cString::ToString(cMath::RandRectl(1,3)),false, 0.3f);
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mlAttackHitCounter =0; //When returning from door breakage there should be no pause!
			mfFOVMul = 1.0f;

		//------------------------------

		kLuxOnUpdate
			//Turn towards the door!
			mpMover->TurnToPos(mvTempPos);

		//------------------------------

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
				PlayAnim("Attack"+GetCurrentPoseSuffix()+cString::ToString(cMath::RandRectl(1,3)),false, 0.3f);
			}

			
		
		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			Attack(mNormalAttackSize, mBreakDoorAttackDamage,20.0f);
			

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
        		
	//------------------------------

	////////////////////////////////
	// Attack Short
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_AttackMeleeShort)
		kLuxOnEnter

			ForceTeslaSighting();

			mpPathfinder->Stop();

			if(cMath::RandRectl(0,2)==0)
				PlayAnim("Bite"+GetCurrentPoseSuffix(),false, 0.3f);
			else
				PlayAnim("Attack"+GetCurrentPoseSuffix()+cString::ToString(cMath::RandRectl(1,3)),false, 0.3f);
			mfFOVMul = 4.0f;

			mfTeslaSpecialNoticeCount = 0.35f; //Give player some time to escape.

		kLuxOnLeave
			mfFOVMul = 1.0f;

		//------------------------------

		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			
			if(mPreviousState == eLuxEnemyState_Hurt)
				ChangeState(eLuxEnemyState_Hunt);
			else 
				ChangeState(mPreviousState);
		
		//------------------------------

		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			
			if(Attack(mNormalAttackSize, mNormalAttackDamage, mfDamageMul)==false)
			{
				mbLastShortAttackWasMiss = true;
				mbForceChargeAttack = cMath::RandRectl(0,1)==0;
			}

		//------------------------------
		
		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	
	//------------------------------

	////////////////////////////////
	// Attack Launch
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_AttackMeleeLong)
		kLuxOnEnter

			ForceTeslaSighting();

			mpPathfinder->Stop();
			SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
			mfForwardSpeed *= 2.0f;
			mlTempVal = 0;
			mfFOVMul = 4.0f;
			SendMessage(eLuxEnemyMessage_TimeOut, 1.0f, true); //Run a bit at full speed before charge.

			mfTeslaSpecialNoticeCount = 0.35f; //Give player some time to escape.

		kLuxOnLeave
			mfFOVMul = 1.0f;
			SetMoveSpeed(eLuxEnemyMoveSpeed_Run);

		//------------------------------

		kLuxOnUpdate
			//If close enough, charge early.
			if(mlTempVal==0 && DistToPlayer2D() < 1.3f)
			{
				mlTempVal = 1;
				PlayAnim("Charge"+GetCurrentPoseSuffix(),false, 0.3f, false);
			}

			//Move towards player
			mpMover->MoveToPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());

		//------------------------------
			
		kLuxOnMessage(eLuxEnemyMessage_TimeOut)
			if(mlTempVal==0)
			{
				mlTempVal = 1;
				PlayAnim("Charge"+GetCurrentPoseSuffix(),false, 0.3f, false);
			}

		kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
			if(mlTempVal==1)
			{
				Attack(mNormalAttackSize, mNormalAttackDamage, mfDamageMul);
			}
		
		kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
			ChangeState(mPreviousState);
			

		//------------------------------

		////////////////////////
		// Overload global
		kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
		kLuxOnMessage(eLuxEnemyMessage_HelpMe)
	
	//------------------------------

	////////////////////////////////
	// Hurt
	////////////////////////////////

	//------------------------------

	kLuxState(eLuxEnemyState_Hurt)
		kLuxOnEnter
			mpPathfinder->Stop();	
			PlayAnim("Flinch"+GetCurrentPoseSuffix(),false, 0.5f);
			SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
			mfFOVMul = 4.0f;

		kLuxOnLeave
			mfFOVMul = 1.0f;

		//------------------------------

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

	//------------------------------

	////////////////////////////////
	// Dead (TEMP! USE RAGDOLL)
	////////////////////////////////

	//------------------------------

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

void cLuxEnemy_ManPig::OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)
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

bool cLuxEnemy_ManPig::PlayerIsDetected()
{
	if(CanSeePlayer())
	{
		return true;
	}
	else
	{
		return (DistToPlayer() < mpCharBody->GetSize().x && PlayerInFOV())/* || (mfInLanternLightCount>=1 && mbBlind==false)*/;
	}
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::OnDisableTriggers()
{
	if(mCurrentState == eLuxEnemyState_Idle) return;

	ChangeState(eLuxEnemyState_Patrol);
}

//-----------------------------------------------------------------------

float cLuxEnemy_ManPig::GetDamageMul(float afAmount, int alStrength)
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

void cLuxEnemy_ManPig::OnSetActiveEnemySpecific(bool abX)
{
	if(mbIsTelsa)
	{
		mpMeshEntity->SetVisible(false);

		for(size_t i=0; i<mvLights.size(); ++i)
		{
			mvLights[i]->SetVisible(abX);
		}
	}

	if(abX==false && mbIsTelsa)
	{
		ResetMindFuckEffects();
		mbTeslaTerror = false;
		mfTeslaSpecialNoticeCount =0;

		gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);		
		gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
	}
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::CheckEnemyAutoRemoval(float afDistance)
{
	if(mbIsSeenByPlayer==false && DistToPlayer() > afDistance && mbAutoRemoveAtPathEnd)
	{
		SetActive(false);	

		RunCallbackFunc("OnAutoDisabled");

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::PatrolUpdateGoal()
{
	if(mvPatrolNodes.empty())
	{
		switch(mIdleBehavior)
		{
		case eLuxIdleBehavior_None:
			ChangeState(eLuxEnemyState_Wait);
			break;

		case eLuxIdleBehavior_Stalk:
			ChangeState(eLuxEnemyState_Stalk);
			break;

		case eLuxIdleBehavior_Track:
			ChangeState(eLuxEnemyState_Track);
			break;

		}
		
		
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

bool cLuxEnemy_ManPig::FleeTryToFindSafeNode()
{
	float fLength=20;
	cVector3f vDirToPlayer = GetDirection2DToPlayer();

	//Try a couple of times to find a good node!
	for(int i=0; i<10; ++i)
	{
		/////////////////////////////////////
		//Find a node to run to
		cAINode * pNode = mpPathfinder->GetNodeAtPos(	mpCharBody->GetPosition() + vDirToPlayer*fLength*0.2f,
														0, fLength, false, false, true, NULL);
		if(pNode == NULL && DistToPlayer2D(pNode->GetPosition())<4.0f)
		{
			continue;
		}

		/////////////////////////////////////
		//See if first node takes you away from player
		mpPathfinder->MoveTo(pNode->GetPosition());
		cVector3f vDirToNode = GetDirection2D(mpPathfinder->GetNextGoalPos());
		if(cMath::Vector3Angle(vDirToPlayer, vDirToNode)<cMath::ToRad(45))
		{
			mpPathfinder->Stop();
			continue;
		}

		//Path was found!
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::StalkFindNode()
{

	//Try a couple of times to find a good node!
	int lNumOfTries=10;
	for(int i=0; i<lNumOfTries; ++i)
	{
		/////////////////////////////////////
		//Find a node to run to
		cAINode * pNode = mpPathfinder->GetNodeAtPos(GetPlayerFeetPos(), 7, 30, false, false, true, NULL);
		if(pNode==NULL)
		{
			return false;
		}

		////////////////////////////////////
		//Check if player cannot see enemy at this position or if it is far enough away.
		float fDistance = DistToPlayer2D(pNode->GetPosition());
		
		if(fDistance < 15.5f && IsVisibleToPlayerAtFeetPos(pNode->GetPosition()))
		{
			continue;
		}
		
		//////////////////////////////
		//Move to pos
		mpPathfinder->MoveTo(pNode->GetPosition());

		/////////////////////////////////////
		//Check so player does not see the first path node
		if(IsVisibleToPlayerAtFeetPos(mpPathfinder->GetNextGoalPos()))
		{
			mpPathfinder->Stop();
			continue;
		}
		
		return true; //Take this node!
	}

	return false;
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::TrackFindNode()
{
	float fDistToPlayer = DistToPlayer2D();

	float fMaxDistance = cMath::Max(fDistToPlayer*0.65f, 3.5f);
	float fMinDistance = cMath::Max(fDistToPlayer *0.25f, 1.5f);
	
	int lNumOfTries=10;
	for(int i=0; i<lNumOfTries; ++i)
	{
		/////////////////////////////////////
		//Find a node to run to
		cAINode * pNode = mpPathfinder->GetNodeAtPos(GetPlayerFeetPos(), fMinDistance, fMaxDistance, false, false, true, NULL);
		if(pNode==NULL)
		{
			return false;
		}

		if(i==lNumOfTries-1)
		{
			mpPathfinder->MoveTo(pNode->GetPosition());
			return true;
		}

		//////////////////////////////
		//Move to pos
		mpPathfinder->MoveTo(pNode->GetPosition());

		return true; //Take this node!
	}

	return false;
	
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::TrackTeleportBehindPlayer()
{
	float fDistance = DistToPlayer2D();

	cVector3f vPlayerBackward = gpBase->mpPlayer->GetCharacterBody()->GetForward()*-1;

	int lNumOfTries=10;
	for(int i=0; i<lNumOfTries; ++i)
	{
		/////////////////////////////////////
		//Find a node to run to
		cAINode * pNode = mpPathfinder->GetNodeAtPos(GetPlayerFeetPos(),
														fDistance*0.5f, fDistance, false, true, false, NULL);
		if(pNode==NULL)
		{
			return false;
		}

		if(i==lNumOfTries-1)
		{
			mpPathfinder->MoveTo(pNode->GetPosition());
			return true;
		}

		if(IsVisibleToPlayerAtFeetPos(pNode->GetPosition()))
		{
			continue;
		}
		
		mpCharBody->SetFeetPosition(pNode->GetPosition());

		return true; //Take this node!
	}

	return false;
}

//-----------------------------------------------------------------------


void cLuxEnemy_ManPig::PlayHuntSound()
{
	if(mpHuntSound && mpWorld->SoundEntityExists(mpHuntSound, mlHuntSoundId)) return;

	mpHuntSound = PlaySound(msChaseSound);
	if(mpHuntSound) mlHuntSoundId = mpHuntSound->GetCreationID();
}

//-----------------------------------------------------------------------

bool cLuxEnemy_ManPig::InsidePlayerView()
{
	cFrustum *pFrust = gpBase->mpPlayer->GetCamera()->GetFrustum();

	//Log("Check inside ray:\n");

	/////////////////////////
	//If near enough, just check if in frustum
	if(cMath::Vector3Dist(mpCharBody->GetPosition(), gpBase->mpPlayer->GetCharacterBody()->GetPosition())<2.25f)
	{
		//Log(" true: close!\n");
		return pFrust->CollideBoundingVolume(mpCharBody->GetCurrentBody()->GetBoundingVolume())!=eCollision_Outside;
	}
	
	/////////////////////////
	//Check inside frustum
	if(pFrust->CollideBoundingVolume(mpCharBody->GetCurrentBody()->GetBoundingVolume())!=eCollision_Inside)
	{
		//Log(" false: outside frustum!\n");
		return false;
	}

	/////////////////////////
	//Check line of sight
	cVector3f vStart = pFrust->GetOrigin();
	cVector3f vPositions[3];
	vPositions[0] = mpCharBody->GetPosition();

	cVector3f vSideAdd = mpCharBody->GetRight()*mpCharBody->GetSize().x*0.4f;
	vPositions[1] = mpCharBody->GetPosition() + vSideAdd;
	vPositions[2] = mpCharBody->GetPosition() - vSideAdd;
	
	for(int i=0; i<3; ++i)
	{
		if(gpBase->mpMapHelper->CheckLineOfSight(vStart, vPositions[i], true))
		{
			//Log(" true: ray %d!\n", i);
			return true;
		}
	}

	//Log(" false: no rays!\n");
	return false;
}

void cLuxEnemy_ManPig::UpdateCheckInLantern(float afTimeStep)
{
	if(gpBase->mpPlayer->GetHelperLantern()->IsActive()==false || mbDisableTriggers)
	{
		mfInLanternLightCount =0;
		return;
	}

	mfCheckFlashLightShining-=afTimeStep;
	if(mfCheckFlashLightShining>0) 
	{
		return;
	}
	mfCheckFlashLightShining = 0.2f;

	/////////////////////////////
	//Check if inside light ray
	if(InsidePlayerView())
	{
		if(mfInLanternLightCount<1)
		{
			mfInLanternLightCount+= 0.1f*mfLanternSensitivity;
			if(mfInLanternLightCount>1) mfInLanternLightCount=1;
			mvLastKnownPlayerPos = gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition();
		}
	}
	else
	{
		mfInLanternLightCount-= 0.2f;
		if(mfInLanternLightCount<0) mfInLanternLightCount=0;
	}
	
	//gpBase->mpDebugHandler->AddMessage(_W("LightCount:")+cString::ToStringW(mfInLanternLightCount,2),false);
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::ForceTeslaSighting()
{
	if(mbIsTelsa==false) return;

	if(mlMindFuckBlinkState==1)
	{
		if(mfMindFuckBlinkAmount>=1)
			mpMeshEntity->SetVisible(true);
		else
			mbTeslaForceSighting = true;
	}
	else
	{
		mfMindFuckBlinkCount =0;
		mbTeslaForceSighting = true;
	}
	
	//mlMindFuckBlinkState=1;
}

void cLuxEnemy_ManPig::SetTeslaEffectsAmount(float afX)
{
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		mvLights[i]->SetDiffuseColor(mvDefaultLightColors[i]*afX);
	}
	mpMeshEntity->SetIlluminationAmount(afX);
}

void cLuxEnemy_ManPig::ResetMindFuckEffects()
{
	gpBase->mpEffectHandler->GetImageTrail()->FadeTo(0, 1);
	gpBase->mpPlayer->FadeAspectMulTo(1.0f, 10.0f);
	gpBase->mpPlayer->FadeFOVMulTo(1.0f,1.0f);
	mbTeslaMindFuckActive = false;
	mfTeslaMindFuckPulse =0;
	mfTeslaMindFuckPulseAdd=1;
	if (mbTeslaFadeDisabled == false)
		gpBase->mpEffectHandler->GetFade()->SetDirectAlpha(0);

	if(mpMindFuckSound)
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		
		if(pSoundHandler->IsValid(mpMindFuckSound, mlMindFuckSoundId))
		{
			mpMindFuckSound->FadeOut(2);
		}
		mpMindFuckSound=NULL;
	}

	mfBlackOutCount=0;
	mfBlackOutDurationCount=0;

	mfMindFuckBlinkCount=0;
	mfMindFuckBlinkAmount=0;
	mlMindFuckBlinkState=0;

	mpMeshEntity->SetVisible(false);
}

void cLuxEnemy_ManPig::SetTeslaFadeDisabled(bool abX)
{
	mbTeslaFadeDisabled = abX;
}

void cLuxEnemy_ManPig::SetTeslaSoundDisabled(bool abX)
{
	mbTeslaSoundDisabled = abX;

	if(abX == true && mpMindFuckSound)
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		
		if(pSoundHandler->IsValid(mpMindFuckSound, mlMindFuckSoundId))
		{
			mpMindFuckSound->FadeOut(2);
		}
		mpMindFuckSound=NULL;
	}
}

void cLuxEnemy_ManPig::SetTeslaEasyEscapeDisabled(bool abX)
{
	mbTeslaEasyEscapeDisabled = abX;
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::UpdateTesla(float afTimeStep)
{
	if(gpBase->mpPlayer->IsDead() || IsActive()==false)
	{
		if(mbTeslaMindFuckActive)
		{
			ResetMindFuckEffects();
		}
		
		return;
	}

	const float fMaxNoticeCount = 1.0f;
	///////////////////////////////////////
	// Tesla Flickering
	{
		mfBlackOutCount -= afTimeStep;
		if(mfBlackOutCount<0)
		{
			mfBlackOutDurationCount = cMath::RandRectf(2.0f, 4.0f);
			mfBlackOutCount = mfBlackOutDurationCount + cMath::RandRectf(2.0f, 8.0f);
		}

		if(mfBlackOutDurationCount>0) mfBlackOutDurationCount -= afTimeStep;
		
		mfTeslaFlickerTimer-= afTimeStep;
		if(mfTeslaFlickerTimer<=0)
		{
			if(mlTeslaFlickerState==0)
			{
				mlTeslaFlickerState = 1;
				mfTeslaFlickerTimer = cMath::RandRectf(0.05f, 0.2f);
				//if (cMath::RandRectl(0,8)==0)
				//{
				//	mfTeslaFlickerTimer = cMath::RandRectf(0.7f, 1.3f);
				//}
				//else
				//{
				//	mfTeslaFlickerTimer = cMath::RandRectf(0.05f, 0.2f);
				//}
			}
			else
			{
				mlTeslaFlickerState = 0;
				mfTeslaFlickerTimer = cMath::RandRectl(0,8)==0 ? cMath::RandRectf(0.1f, 0.2f) : cMath::RandRectf(0.05f, 0.15f);
			}
		}

		if(mfBlackOutDurationCount>0 && mfTeslaSpecialNoticeCount<0.7f)
		{
			mfTeslaEffectAmount -= afTimeStep*10;
			if(mfTeslaEffectAmount<0) mfTeslaEffectAmount =0;
		}
		else if(mlTeslaFlickerState==0)
		{
			mfTeslaEffectAmount -= afTimeStep*25;
			if(mfTeslaEffectAmount<0) mfTeslaEffectAmount =0;
		}
		else
		{
			mfTeslaEffectAmount += afTimeStep*16;
			if(mfTeslaEffectAmount>1) mfTeslaEffectAmount =1;
		}

		SetTeslaEffectsAmount(mlTeslaFlickerState==1 || mpMeshEntity->IsVisible() == false ? 0.0f : 1.0f);
	}

	///////////////////////////////////////
	// Special Notice Checks
	{
		bool bWasNoticed = false;

		//////////////////////////////////
		// Movement
		/*const float fMinSpeed = 1.4;
		float fSpeed = gpBase->mpPlayer->GetAvgSpeed();
		if(fSpeed>fMinSpeed)
		{
			float fMul = 0.15f;
			float fDistance = DistToPlayer2D();
			
			if(fDistance>30) fMul *= 0.5f;
			else if(fDistance>15) fMul *= 0.75f;

			mfTeslaSpecialNoticeCount += (fSpeed/fMinSpeed)*afTimeStep*fMul;
			bWasNoticed = true;
		}*/
		
		//////////////////////////////////
		// Vision
		/*if(mbIsSeenByPlayer)// InsidePlayerView())
		{
			mfTeslaSpecialNoticeCount += afTimeStep*0.15f;
			bWasNoticed = true;
		}*/

		///////////////////////////////
		// Terror
		/*if(mbTeslaTerror)
		{
			mfTeslaSpecialNoticeCount += afTimeStep;
		}*/

		//if(mfTeslaSpecialNoticeCount>fMaxNoticeCount) mfTeslaSpecialNoticeCount =fMaxNoticeCount;

		///////////////////////////////
		// Distance
		float fDistance = DistToPlayer2D();
		if(fDistance < 20)
		{
			mfTeslaSpecialNoticeCount = cMath::Clamp(1.0f-fDistance/19.0f, 0.0f, 1.0f);
		}

		///////////////////////////////
		// No notice
		if(bWasNoticed==false)
		{
			mfTeslaSpecialNoticeCount -= afTimeStep*0.5f;
			if(mfTeslaSpecialNoticeCount<0.0f) mfTeslaSpecialNoticeCount=0;
		}

		///////////////////////////////
		// Check if spotted
		/*if(mfTeslaSpecialNoticeCount>=1.0f)
		{
			ShowPlayerPosition();
		}*/

		///////////////////////////////
		// Make it easier to escape.
		if(mbTeslaEasyEscapeDisabled==false && 
		   mfTeslaSpecialNoticeCount<0.35f && 
		   mbCanSeePlayer == false &&
		   gpBase->mpPlayer->GetHealth()<75)
		{
			if(mCurrentState == eLuxEnemyState_Hunt ||
				mCurrentState == eLuxEnemyState_HuntPause ||
				mCurrentState == eLuxEnemyState_HuntWander)
			{
				mfWaitTime=0.5f;
				gpBase->mpPlayer->RemoveTerrorEnemy(this);
				ChangeState(eLuxEnemyState_Wait);
				mbPlayerDetected=false;
				mbCanSeePlayer=false;
			}
		}

		//gpBase->mpDebugHandler->AddMessage(_W("Notice: ")+cString::ToStringW(mfTeslaSpecialNoticeCount), false);

	}

	///////////////////////////////////////
	// Effects
	{
		/////////////////////////
		// Active
		if(mfTeslaSpecialNoticeCount>0.0f)
		{
			mbTeslaMindFuckActive = true;

			////////////////////////
			// Image trail
			//gpBase->mpEffectHandler->GetImageTrail()->FadeTo(mfTeslaSpecialNoticeCount, 3.0f);

			////////////////////////
			// Pulse update
			mfTeslaMindFuckPulse += afTimeStep*mfTeslaMindFuckPulseAdd*3*mfTeslaSpecialNoticeCount;
			if(mfTeslaMindFuckPulse>1 && mfTeslaMindFuckPulseAdd>0)
			{
				mfTeslaMindFuckPulse = 1;
				mfTeslaMindFuckPulseAdd = -1;
			}
			if(mfTeslaMindFuckPulse<0 && mfTeslaMindFuckPulseAdd<0)
			{
				mfTeslaMindFuckPulse = 0;
				mfTeslaMindFuckPulseAdd = 1;
			}

			////////////////////////
			// Aspect Mul
			//gpBase->mpPlayer->FadeAspectMulTo(1.0f + mfTeslaMindFuckPulse*mfTeslaSpecialNoticeCount*0.3f, 10.0f);
			//gpBase->mpPlayer->FadeFOVMulTo(1.0f - mfTeslaMindFuckPulse*mfTeslaSpecialNoticeCount*0.3f, 10.0f);

			////////////////////////
			// Blink
			{
				float fCountMul = 1.0f;
				if(mlMindFuckBlinkState==0)
				{
					if(mfMindFuckBlinkCount<0.3f) fCountMul= mfTeslaSpecialNoticeCount;
				}
				else
				{
					if(mfTeslaSpecialNoticeCount<0.7f && mfMindFuckBlinkCount>0.2f) fCountMul += (1-mfTeslaSpecialNoticeCount)*5;
				}
				
				mfMindFuckBlinkCount-=afTimeStep*fCountMul;
				if(mfMindFuckBlinkCount<0)
				{
					if(mlMindFuckBlinkState==0)
					{
						mlMindFuckBlinkState=1;
						if (mfTeslaSpecialNoticeCount < 0.5f && cMath::RandRectl(0,8)==0)
							mfMindFuckBlinkCount = cMath::RandRectf(0.75f, 1.2f);
						else
							mfMindFuckBlinkCount = cMath::RandRectf(0.1f, 0.3f);
					}
					else
					{
						mlMindFuckBlinkState=0;
						mfMindFuckBlinkCount = cMath::RandRectl(0,4)==0 ? cMath::RandRectf(0.75f, 2.2f) : cMath::RandRectf(0.1f, 0.5f);
					}
				}

				if(mlMindFuckBlinkState==0 && mfMindFuckBlinkAmount>0)
				{
					mfMindFuckBlinkAmount -= afTimeStep*6;
					if(mfMindFuckBlinkAmount<0){
						mfMindFuckBlinkAmount=0;
					}
				}
				if(mlMindFuckBlinkState==1 && mfMindFuckBlinkAmount<1)
				{
					mfMindFuckBlinkAmount += afTimeStep*15;
					if(mfMindFuckBlinkAmount>1)
					{
						mfMindFuckBlinkAmount=1;
						
						mpMeshEntity->SetVisible(false);
						int lMaxRand =  3;
						if(mfTeslaSpecialNoticeCount<0.7f || mCurrentState==eLuxEnemyState_Patrol)
						{
							lMaxRand =2;
						}
						if(mbTeslaForceSighting || cMath::RandRectl(0, lMaxRand)==0) mpMeshEntity->SetVisible(true);

						mbTeslaForceSighting = false;
					}
				}
						
				//float fDarkness = 0.4f + (mfTeslaSpecialNoticeCount/fMaxNoticeCount)*0.5f;
				if (mbTeslaFadeDisabled == false)
					gpBase->mpEffectHandler->GetFade()->SetDirectAlpha(mfMindFuckBlinkAmount);
			}
			

			////////////////////////
			// Sound
			if (mbTeslaSoundDisabled == false)
			{
				cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
				if(mpMindFuckSound==NULL || pSoundHandler->IsValid(mpMindFuckSound, mlMindFuckSoundId)==false)
				{
					mpMindFuckSound = pSoundHandler->PlayGui(msTeslaMindFuckLoop, true, 1.0f);
					if(mpMindFuckSound)
					{
						mlMindFuckSoundId = mpMindFuckSound->GetId();
					}
				}
				else
				{
					mpMindFuckSound->FadeVolumeMulTo(0.1f + (mfTeslaSpecialNoticeCount/fMaxNoticeCount)*0.9, 10.0f);
				}
			}
		}
		/////////////////////////
		// Not active
		else
		{
			if(mbTeslaMindFuckActive)
			{
				ResetMindFuckEffects();
			}
		}


	}

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxEnemy_ManPig_SaveData, iLuxEnemy_SaveData)

kSerializeVar(mbThreatenOnAlert, eSerializeType_Bool)
kSerializeVar(mbFleeFromPlayer, eSerializeType_Bool)
kSerializeVar(mbAutoDisableAfterFlee, eSerializeType_Bool)
kSerializeVar(mfLanternSensitivity, eSerializeType_Float32)
kSerializeVar(mIdleBehavior, eSerializeType_Int32)
kSerializeVar(mPatrolMoveSpeed, eSerializeType_Int32)
kSerializeVar(mfDamageMul, eSerializeType_Float32)
kSerializeVar(mfRunSpeedMul, eSerializeType_Float32)
kSerializeVar(mfInLanternLightCount, eSerializeType_Float32)
kSerializeVar(mbAllowZeroWaitTime, eSerializeType_Bool)
kSerializeVar(mfHuntPauseTimeMul, eSerializeType_Float32)
kSerializeVar(mbTeslaTerror, eSerializeType_Bool)
kSerializeVar(mbTeslaFadeDisabled, eSerializeType_Bool)
kSerializeVar(mbTeslaSoundDisabled, eSerializeType_Bool)
kSerializeVar(mbTeslaEasyEscapeDisabled, eSerializeType_Bool)
kSerializeVar(mlCurrentPose, eSerializeType_Int32)
kSerializeVar(mlCurrentMoveType, eSerializeType_Int32)
kSerializeVar(mbSkipVisibilityRangeHandicaps, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxEnemy_ManPig::CreateSaveData()
{
	return hplNew(cLuxEnemy_ManPig_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::SaveToSaveData(apSaveData);
	cLuxEnemy_ManPig_SaveData *pData = static_cast<cLuxEnemy_ManPig_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyToVar(pData,mbThreatenOnAlert);
	kCopyToVar(pData,mbFleeFromPlayer);
	kCopyToVar(pData,mbAutoDisableAfterFlee);
	kCopyToVar(pData,mfLanternSensitivity);
	kCopyToVar(pData,mIdleBehavior);
	kCopyToVar(pData,mPatrolMoveSpeed);
	kCopyToVar(pData,mfRunSpeedMul);
	kCopyToVar(pData,mfDamageMul);
	kCopyToVar(pData,mfInLanternLightCount);
	kCopyToVar(pData,mbAllowZeroWaitTime);
	kCopyToVar(pData,mfHuntPauseTimeMul);
	kCopyToVar(pData,mbTeslaTerror);
	kCopyToVar(pData,mbTeslaFadeDisabled);
	kCopyToVar(pData,mbTeslaSoundDisabled);
	kCopyToVar(pData,mbTeslaEasyEscapeDisabled);

	pData->mlCurrentPose = mCurrentPose;
	pData->mlCurrentMoveType = mCurrentMoveType;

	kCopyToVar(pData,mbSkipVisibilityRangeHandicaps);
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	//////////////////
	//Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxEnemy_ManPig_SaveData *pData = static_cast<cLuxEnemy_ManPig_SaveData*>(apSaveData);

	//////////////////
	//Set variables
	kCopyFromVar(pData,mbThreatenOnAlert);
	kCopyFromVar(pData,mbFleeFromPlayer);
	kCopyFromVar(pData,mbAutoDisableAfterFlee);
	kCopyFromVar(pData,mfLanternSensitivity);
	mIdleBehavior  = (eLuxIdleBehavior)pData->mIdleBehavior;
	mPatrolMoveSpeed  = (eLuxEnemyMoveSpeed)pData->mPatrolMoveSpeed;
	kCopyFromVar(pData,mfRunSpeedMul);
	kCopyFromVar(pData,mfDamageMul);
	kCopyFromVar(pData,mfInLanternLightCount);
	kCopyFromVar(pData,mbAllowZeroWaitTime);
	kCopyFromVar(pData,mfHuntPauseTimeMul);
	kCopyFromVar(pData,mbTeslaTerror);
	kCopyFromVar(pData,mbTeslaFadeDisabled);
	kCopyFromVar(pData,mbTeslaSoundDisabled);
	kCopyFromVar(pData,mbTeslaEasyEscapeDisabled);

	mCurrentPose = (eLuxEnemyPoseType)pData->mlCurrentPose;
	mCurrentMoveType = (eLuxEnemyMoveType)pData->mlCurrentMoveType;

	kCopyFromVar(pData,mbSkipVisibilityRangeHandicaps);
}

//-----------------------------------------------------------------------

void cLuxEnemy_ManPig::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------


