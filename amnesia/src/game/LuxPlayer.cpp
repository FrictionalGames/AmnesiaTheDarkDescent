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

#include "LuxPlayer.h"

#include "LuxMap.h"
#include "LuxInventory.h"
#include "LuxMessageHandler.h"
#include "LuxMapHandler.h"
#include "LuxHelpFuncs.h"

#include "LuxPlayerState_Normal.h"
#include "LuxPlayerState_HandObject.h"
#include "LuxPlayerState_UseItem.h"
#include "LuxPlayerState_InteractGrab.h"
#include "LuxPlayerState_InteractPush.h"
#include "LuxPlayerState_InteractSlide.h"
#include "LuxPlayerState_InteractSwingDoor.h"
#include "LuxPlayerState_InteractLever.h"
#include "LuxPlayerState_InteractWheel.h"
#include "LuxPlayerState_Ladder.h"
#include "LuxHintHandler.h"
#include "LuxEffectHandler.h"

#include "LuxEntity.h"

#include "LuxMoveState_Normal.h"
#include "LuxMoveState_ClimbLedge.h"

#include "LuxPlayerHelpers.h"
#include "LuxPlayerHands.h"

#include "LuxAreaNodes.h"


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayer::cLuxPlayer() : iLuxUpdateable("LuxPlayer"), iLuxCollideCallbackContainer()
{
	//////////////////////////////////
	// Init data pointers
	mpCharBody = NULL;
	mpTerrorSound = NULL;

	//////////////////////////////////
	// Create and setup camera
	mpCamera = gpBase->mpEngine->GetScene()->CreateCamera(eCameraMoveMode_Walk);

	mbFreeCameraActive = false;
	mfFreeCameraSpeed = 0.1f;

	//TODO: More setup?
	cVector2f vScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
	mfAspect = vScreenSize.x / vScreenSize.y;
	mfFOV = cMath::ToRad(gpBase->mpGameCfg->GetFloat("Player_General","FOV", 0));
	
	mpCamera->SetFOV(mfFOV);
	mpCamera->SetAspect(mfAspect);
	mpCamera->SetFarClipPlane(gpBase->mpGameCfg->GetFloat("Player_General","FarClipPlane",0));
	mpCamera->SetNearClipPlane(gpBase->mpGameCfg->GetFloat("Player_General","NearClipPlane",0));
	mpCamera->SetPitchLimits(-cMath::ToRad(70), cMath::ToRad(70) );

	//////////////////////////////////
	// Init General properties
	mfHeadSpinDamageSpeed = gpBase->mpGameCfg->GetFloat("Player_General","HeadSpinDamageSpeed",0);
	mfHeadSpinDeacc = gpBase->mpGameCfg->GetFloat("Player_General","HeadSpinDeacc",0);

	msTerrorSound = gpBase->mpGameCfg->GetString("Player_General","TerrorSound","");

	mfTerrorIncSpeed = gpBase->mpGameCfg->GetFloat("Player_General","TerrorIncSpeed",0);
	mfTerrorDecSpeed = gpBase->mpGameCfg->GetFloat("Player_General","TerrorDecSpeed",0);

	mlMaxPrevSpeeds = gpBase->mpGameCfg->GetInt("Player_General","MaxPrevSpeedsSaved",0);

	mfAutoKillYPos = gpBase->mpGameCfg->GetFloat("Player_General","AutoKillYPos",0);

	//////////////////////////////////
	// Init body properties
	mvBodySize = gpBase->mpGameCfg->GetVector3f("Player_Body","Size",0);
	mvBodyCrouchSize = gpBase->mpGameCfg->GetVector3f("Player_Body","CrouchSize",0);
	
	mvCameraPosAdd = gpBase->mpGameCfg->GetVector3f("Player_Body","CameraPosAdd",0);

	mfDefaultMass = gpBase->mpGameCfg->GetFloat("Player_Body","Mass",0);


	//////////////////////////////////
	// Init some other variables
	mbUsePermaDeath = gpBase->mbPTestActivated;
	
	//////////////////////////////////
	// Create states
	mvStates.resize(eLuxPlayerState_LastEnum);

	mvStates[eLuxPlayerState_Normal] = hplNew( cLuxPlayerState_Normal, (this) );
	mvStates[eLuxPlayerState_HandObject] = hplNew( cLuxPlayerState_HandObject, (this) );
	mvStates[eLuxPlayerState_UseItem] = hplNew( cLuxPlayerState_UseItem, (this) );
	mvStates[eLuxPlayerState_InteractGrab] = hplNew( cLuxPlayerState_InteractGrab, (this) );
	mvStates[eLuxPlayerState_InteractPush] = hplNew( cLuxPlayerState_InteractPush, (this) );
	mvStates[eLuxPlayerState_InteractSlide] = hplNew( cLuxPlayerState_InteractSlide, (this) );
	mvStates[eLuxPlayerState_InteractSwingDoor] = hplNew( cLuxPlayerState_InteractSwingDoor, (this) );
	mvStates[eLuxPlayerState_InteractLever] = hplNew( cLuxPlayerState_InteractLever, (this) );
	mvStates[eLuxPlayerState_InteractWheel] = hplNew( cLuxPlayerState_InteractWheel, (this) );
	mvStates[eLuxPlayerState_Ladder] = hplNew( cLuxPlayerState_Ladder, (this) );

	//////////////////////////////////
	// Create move states
	mvMoveStates.resize(eLuxMoveState_LastEnum);

	mvMoveStates[eLuxMoveState_Normal] = hplNew( cLuxMoveState_Normal, (this) );
	mvMoveStates[eLuxMoveState_ClimbLedge] = hplNew( cLuxMoveState_ClimbLedge, (this) );

	//////////////////////////////////
	// Create head pos adds
	mvHeadPosAdds.resize(eLuxHeadPosAdd_LastEnum);


	//////////////////////////////////
	// Create graphics
	mpFocusFont = NULL;
	
	//////////////////////////////////
	// Create helpers
	mpHudEffect = hplNew( cLuxPlayerHudEffect, (this) );
	mvHelpers.push_back(mpHudEffect);

	mpHelperLightLevel = hplNew( cLuxPlayerLightLevel, (this) );
	mvHelpers.push_back(mpHelperLightLevel);

	mpHelperInDarkness = hplNew( cLuxPlayerInDarkness, (this) );
	mvHelpers.push_back(mpHelperInDarkness);

	mpLean = hplNew( cLuxPlayerLean, (this) );
	mvHelpers.push_back(mpLean);

	mpDeath = hplNew( cLuxPlayerDeath, (this) );
	mvHelpers.push_back(mpDeath);

	mpHands = hplNew( cLuxPlayerHands, (this) );
	mvHelpers.push_back(mpHands);

	mpLantern = hplNew( cLuxPlayerLantern, (this) );
	mvHelpers.push_back(mpLantern);

	mpSanity = hplNew( cLuxPlayerSanity, (this) );
	mvHelpers.push_back(mpSanity);

	mpLookAt = hplNew( cLuxPlayerLookAt, (this) );
	mvHelpers.push_back(mpLookAt);

	mpFlashback = hplNew( cLuxPlayerFlashback, (this) );
	mvHelpers.push_back(mpFlashback);

	mpHurt = hplNew( cLuxPlayerHurt, (this) );
	mvHelpers.push_back(mpHurt);

	mpSpawnPS = hplNew( cLuxPlayerSpawnPS, (this) );
	mvHelpers.push_back(mpSpawnPS);

	mpCamDirEffects = hplNew( cLuxPlayerCamDirEffects, (this));
	mvHelpers.push_back(mpCamDirEffects);

	mpInsanityCollapse = hplNew( cLuxPlayerInsanityCollapse, (this));
	mvHelpers.push_back(mpInsanityCollapse);
}

//-----------------------------------------------------------------------

cLuxPlayer::~cLuxPlayer()
{
	DestroyCollideCallbacks();

	STLDeleteAll(mvHelpers);
	STLDeleteAll(mvStates);
	STLDeleteAll(mvMoveStates);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayer::LoadFonts()
{
	tString sFontFile = gpBase->mpGameCfg->GetString("Player_General","FocusTextFont","");
	mpFocusFont = LoadFont(sFontFile);

	RunHelperLuxMessage(eLuxUpdateableMessage_LoadFonts,NULL);
}

//-----------------------------------------------------------------------

void cLuxPlayer::Reset()
{
	////////////////////////
	// Reset variables
	mbActive = true;

	mMoveState = eLuxMoveState_Normal;
	mvMoveStates[mMoveState]->OnEnterState(eLuxMoveState_LastEnum);
	
	mState = eLuxPlayerState_Normal;
	mvStates[mState]->OnEnterState(eLuxPlayerState_LastEnum);
	
	mbPressedMove = false;
	mbPressingRun = false;
	mbPressingJump = false;

	mbJumpDisabled = false;
	mbCrouchDisabled = false;
	mbSanityDrainDisabled = false;

	mbIsInWater = false;
	msWaterStepSound = "";
	mfWaterSpeedMul = 1.0f;

	mvHeadSpeed =0;
	mvHeadSpinSpeed =0;

	mfInteractionMoveSpeedMul = 1.0f;

	mfLookSpeedMul = 1.0f;

	mfEventMoveSpeedMul = 1.0f;
	mfEventRunSpeedMul = 1.0f;

	mfScriptMoveSpeedMul = 1.0f;
	mfScriptRunSpeedMul = 1.0f;

	mfHurtMoveSpeedMul = 1.0f;

	mfInsanityCollapseSpeedMul = 1.0f;

	mfScriptJumpForceMul = 1.0f;

	mfCurrentFocusDistance = 0;
	mpEntityInFocus = NULL;
	mpBodyInFocus = NULL;

	mpCharBody = NULL;

	msCurrentPermaDeathSound = "";

	mfHealth = 100;
	mfSanity = 100;
	mfLampOil = 100;
	mlTinderboxes =0;

	mvHeadPosAddSum =0;

	mfTerror =0;

	mlCoins =0;

	mfAvgSpeed =0;
	mlstPrevSpeeds.clear();

	msFocusText = _W("");
	msLastFocusText = _W("");
	mfFocusTextAlpha =0;

	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	if(mpTerrorSound && pSoundHandler->IsValid(mpTerrorSound, mlTerrorSoundID)) mpTerrorSound->Stop();
	mpTerrorSound = NULL;
	mlTerrorSoundID = -1;

	mfAspectMul = 1.0f;
	mfFOVMul = 1.0f;
	mfAspectMulGoal = 1.0f;
	mfFOVMulGoal = 1.0f;
	mfAspectMulSpeed = 0.0f;
	mfFOVMulSpeed = 0.0f;
	mfRoll=0;
	mfRollGoal=0;
	mfRollSpeedMul=0;
	mfRollMaxSpeed=0;

	mfLeanRoll=0;
	mfLeanRollGoal=0;
	mfLeanRollSpeedMul=0;
	mfLeanRollMaxSpeed=0;

	mvCamAnimPos =0;
	mvCamAnimPosGoal=0;
	mfRollSpeedMul=0;
	mfRollMaxSpeed=0;

	mbNoFallDamage = false;

	mbScriptShowFocusIconAndCrossHair = true;

	for(size_t i=0; i<mvHeadPosAdds.size(); ++i)
	{
		mvHeadPosAdds[i].mbMoving = false;
		mvHeadPosAdds[i].mvAdd = 0;
	}

	DestroyCollideCallbacks();

	m_setTerrorEnemies.clear();

	////////////////////////
	//Reset camera
	mpCamera->SetRoll(0.0f);
	mpCamera->SetPitch(0.0f);
	mpCamera->SetYaw(0.0f);
	mpCamera->SetFOV(mfFOV*mfFOVMul);
	mpCamera->SetAspect(mfAspect*mfAspectMul);

	///////////////
	// Free camera
	mbFreeCameraActive = false;
	mfFreeCameraSpeed = 0.1f;

	////////////////////////
	// Reset Helpers
	RunHelperMessage(eUpdateableMessage_Reset,0);
}

//-----------------------------------------------------------------------

void cLuxPlayer::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxPlayer::Update(float afTimeStep)
{
	////////////////////////
	// Update current move state
	mvMoveStates[mMoveState]->Update(afTimeStep);

	////////////////////////
	// Update current state
	mvStates[mState]->Update(afTimeStep);

	////////////////////////
	// Update head
	UpdateHeadPosAdd(afTimeStep);
	UpdateHeadSpin(afTimeStep);

	//mpCamera->AddYaw(afTimeStep * kPif);
	//mpCharBody->AddYaw(afTimeStep * kPif * 0.5f);

	////////////////////////
	// Run Helper message
	RunHelperMessage(eUpdateableMessage_Update,afTimeStep);

	////////////////////////
	//Clear some variables
	mbPressedMove = false;

	////////////////////////
	//Check if too far down
	if(mpCharBody->GetPosition().y < mfAutoKillYPos) SetHealth(0);
			
	////////////////////////
	// Update misc
	CheckCollisionCallback("Player", gpBase->mpMapHandler->GetCurrentMap());
	UpdateCamera(afTimeStep);
	UpdateTerror(afTimeStep);
	UpdateFocusText(afTimeStep);
	UpdateAvgSpeed(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxPlayer::PostUpdate(float afTimeStep)
{
	////////////////////////
	// Run Helper message
	RunHelperMessage(eUpdateableMessage_PostUpdate,afTimeStep);

	////////////////////////
	// Post Update current state
	mvStates[mState]->PostUpdate(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxPlayer::OnDraw(float afFrameTime)
{
	////////////////////////
	// Draw with current move state
	mvMoveStates[mMoveState]->OnDraw(afFrameTime);

	////////////////////////
	// Draw with current state
	mvStates[mState]->OnDraw(gpBase->mpGameHudSet, afFrameTime);

	////////////////////////
	// Run Helper message
	RunHelperMessage(eUpdateableMessage_OnDraw,afFrameTime);

	////////////////////////
	// Draw player HUD
	DrawHud(afFrameTime);
}

//-----------------------------------------------------------------------

void cLuxPlayer::LoadUserConfig()
{
	mbShowCrossHair = gpBase->mpUserConfig->GetBool("Game", "ShowCrossHair", true);
	mFocusIconStyle = StringToFocusIconStyle(gpBase->mpUserConfig->GetString("Game", "FocusIconStyle", "Default"));

	RunHelperLuxMessage(eLuxUpdateableMessage_LoadUserConfig,0);
}

void cLuxPlayer::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Game", "ShowCrossHair", mbShowCrossHair);
	gpBase->mpUserConfig->SetString("Game", "FocusIconStyle", FocusIconStyleToString(mFocusIconStyle));

	RunHelperLuxMessage(eLuxUpdateableMessage_SaveUserConfig,0);
}

//-----------------------------------------------------------------------

void cLuxPlayer::OnMapEnter(cLuxMap *apMap)
{
	////////////////////////////////
	//Init all move states 
	for(int i=0; i<eLuxMoveState_LastEnum; ++i) mvMoveStates[i]->OnMapEnter();
	

	////////////////////////////////
	//Init current move state
	mvMoveStates[mMoveState]->OnEnterState(mMoveState);

	////////////////////////////////
	//Call current state
	mvStates[mState]->OnMapEnter(apMap);

	////////////////////////
	// Run Helper message
	RunHelperLuxMessage(eLuxUpdateableMessage_OnMapEnter, apMap);
	
	////////////////////////////////
	//Reset focus text
	msFocusText = _W("");
	msLastFocusText = _W("");
	mfFocusTextAlpha =0;

	////////////////////////////////
	//Reset variables
	mbIsInWater = false;
	mpEntityInFocus = NULL;
	mpBodyInFocus = NULL;
}

//-----------------------------------------------------------------------

void cLuxPlayer::OnMapLeave(cLuxMap *apMap)
{
	////////////////////////
	// Set default state
	ChangeState(eLuxPlayerState_Normal);
	
	////////////////////////////////
	//Call current state
	mvStates[mState]->OnMapLeave(apMap);

	////////////////////////
	// Clear collide callbacks
	DestroyCollideCallbacks();

	////////////////////////
	// Run Helper message
	RunHelperLuxMessage(eLuxUpdateableMessage_OnMapLeave, apMap);

	/////////////////////
	// Reset stuff
	m_setTerrorEnemies.clear();
	
	mfAspectMulGoal = 1.0f;
	mfAspectMul = 1.0f;
	mfFOVMulGoal = 1.0f;
	mfFOVMul = 1.0f;
	mfRollGoal=0;
	mfRoll=0;

	mfLeanRoll=0;
	mfLeanRollGoal=0;
	mfLeanRollSpeedMul=0;
	mfLeanRollMaxSpeed=0;

	mpCamera->SetRoll(0.0f);
	mpCamera->SetFOV(mfFOV*mfFOVMul);
	mpCamera->SetAspect(mfAspect*mfAspectMul);
}

//-----------------------------------------------------------------------

void cLuxPlayer::CreateWorldEntities(cLuxMap *apMap)
{
	////////////////////////////////
	//Create and Init character body
	CreateCharacterBody(apMap->GetPhysicsWorld());
	
	////////////////////////
	// Run Helper message
	for(size_t i=0; i<mvHelpers.size(); ++i) mvHelpers[i]->CreateWorldEntities(apMap);
}

void cLuxPlayer::DestroyWorldEntities(cLuxMap *apMap)
{
	////////////////////////////////
	//Destroy character body
	if(mpCharBody)
	{
		apMap->GetPhysicsWorld()->DestroyCharacterBody(mpCharBody);
		mpCharBody = NULL;
	}

	////////////////////////
	// Run Helper message
	for(size_t i=0; i<mvHelpers.size(); ++i) mvHelpers[i]->DestroyWorldEntities(apMap);
}
//-----------------------------------------------------------------------

void cLuxPlayer::OnEnterContainer(const tString& asOldContainer)
{
	
}

void cLuxPlayer::OnLeaveContainer(const tString& asNewContainer)
{
	//Reset some variables
	mbPressingRun = false;
	mbPressingJump = false;

	//Do not be in an interaction mode 
	if(	mState == eLuxPlayerState_InteractGrab || 
		mState == eLuxPlayerState_InteractSlide ||
		mState == eLuxPlayerState_InteractPush ||
		mState == eLuxPlayerState_InteractWheel ||
		mState == eLuxPlayerState_InteractLever)
	{
		ChangeState(eLuxPlayerState_Normal);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	mvStates[mState]->RenderSolid(apFunctions);

	for(size_t i=0; i<mvHelpers.size(); ++i) mvHelpers[i]->RenderSolid(apFunctions);
}

void cLuxPlayer::RenderTrans(cRendererCallbackFunctions* apFunctions)
{
	mvStates[mState]->RenderTrans(apFunctions);
}


//-----------------------------------------------------------------------

void cLuxPlayer::GiveDamage(float afAmount, int alStrength, eLuxDamageType aType, bool abSpinHead, bool abLethal)
{
	if(mfHealth <=0) return;

	mfHealth -= afAmount;

	mpHudEffect->AddDamageSplash(aType);
	mpHudEffect->Flash(cColor(0.6f,0,0, 0.5f),eGuiMaterial_Alpha,0,0.25f);
	if(abSpinHead) SpinHead(mfHeadSpinDamageSpeed);

	if(abLethal==false && mfHealth < 10)
	{
		mfHealth = 10;
	}

	if(mfHealth <=0)
	{
		mpDeath->Start();
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::GiveSanityDamage(float afAmount)
{
	gpBase->mpHintHandler->Add("SanityHit", kTranslate("Hints", "SanityHit"), 0);

	LowerSanity(afAmount, true);

	mpSanity->StartHit();
}


void cLuxPlayer::LowerSanity(float afAmount, bool abUseEffect)
{
	if(mfHealth <=0) return;

	mfSanity -= afAmount;
	if(mfSanity < 0)
	{
		mfSanity =0;


		mpInsanityCollapse->Start();

		//////////////////
		// HARDMODE
		if (gpBase->mbHardMode)
		{
			SetHealth(0.f);
		}
	}

	if(abUseEffect)
		mpSanity->SetSanityLost();
}

//-----------------------------------------------------------------------

void cLuxPlayer::Move(eCharDir aDir, float afMul)
{	
	if (mbFreeCameraActive)
	{
		if (aDir == eCharDir_Forward)
		{
			mpCamera->MoveForward(mfFreeCameraSpeed * afMul);
		}
		else if (aDir == eCharDir_Right)
		{
			mpCamera->MoveRight(mfFreeCameraSpeed * afMul);
		}

		return;
	}

	mbPressedMove = true;
	if(mvStates[mState]->OnMove(aDir, afMul))
	{
		if(mvMoveStates[mMoveState]->OnMove(aDir, afMul))
		{
			mpCharBody->Move(aDir, afMul);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::AddYaw(float afAmount)
{
	if (mbFreeCameraActive)
	{
		mpCamera->AddYaw(mpCamDirEffects->AddAndGetPitchAdd(afAmount * 2.0f));
		return;
	}

	afAmount = afAmount * mfLookSpeedMul;

	if(mvStates[mState]->OnAddYaw(afAmount))
	{
		if(mvMoveStates[mMoveState]->OnAddYaw(afAmount))
		{
			//must do like this or else limits will fail!
			mpCamera->AddYaw( mpCamDirEffects->AddAndGetYawAdd(afAmount * 2.0f) );
			mpCharBody->SetYaw(mpCamera->GetYaw());
		}
	}
}

void cLuxPlayer::AddPitch(float afAmount)
{
	if (mbFreeCameraActive)
	{
		mpCamera->AddPitch(mpCamDirEffects->AddAndGetPitchAdd(afAmount * 2.0f));
		return;
	}

	afAmount = afAmount * mfLookSpeedMul;

	if(mvStates[mState]->OnAddPitch(afAmount))
	{
		if(mvMoveStates[mMoveState]->OnAddPitch(afAmount))
		{
			mpCamera->AddPitch( mpCamDirEffects->AddAndGetPitchAdd(afAmount * 2.0f) );
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetLean(float afMul)
{
	mpLean->SetLean(afMul);
}

void cLuxPlayer::AddLean(float afAdd)
{
	mpLean->AddLean(afAdd);
}

//-----------------------------------------------------------------------

void cLuxPlayer::Scroll(float afAmount)
{
	mvStates[mState]->OnScroll(afAmount);
}

//-----------------------------------------------------------------------


void cLuxPlayer::DoAction(eLuxPlayerAction aAction, bool abPressed)
{
	if(mvStates[mState]->OnDoAction(aAction, abPressed))
	{
		if(aAction== eLuxPlayerAction_Lantern && abPressed)
		{
			mpLantern->SetActive(!mpLantern->IsActive(), true);
		}

	}
}

//-----------------------------------------------------------------------


void cLuxPlayer::Run(bool abPressed)
{
	mbPressingRun = abPressed;
	if(mvStates[mState]->OnRun(abPressed))
	{
		mvMoveStates[mMoveState]->OnRun(abPressed);
	}
}

void cLuxPlayer::Jump(bool abPressed)
{
	if(abPressed && (mbJumpDisabled || mpInsanityCollapse->IsActive()) ) return;

	mbPressingJump = abPressed;
	if(mvStates[mState]->OnJump(abPressed))
	{
		mvMoveStates[mMoveState]->OnJump(abPressed);
	}
}

void cLuxPlayer::Crouch(bool abPressed)
{
	if(abPressed && (mbCrouchDisabled || mpInsanityCollapse->IsActive()) ) return;

	if(mvStates[mState]->OnCrouch(abPressed))
	{
		mvMoveStates[mMoveState]->OnCrouch(abPressed);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::ChangeState(eLuxPlayerState aState)
{
	if(mState == aState) return;

	mvStates[mState]->OnLeaveState(aState);

	eLuxPlayerState prevState = mState;
	mState = aState;

	mvStates[mState]->SetPreviousState(prevState);
	mvStates[mState]->OnEnterState(prevState);
}

//-----------------------------------------------------------------------

void cLuxPlayer::ChangeMoveState(eLuxMoveState aState)
{
	if(mMoveState == aState) return;

	mvMoveStates[mMoveState]->OnLeaveState(aState);

	eLuxMoveState prevState = mMoveState;
	mMoveState = aState;

	mvMoveStates[mMoveState]->OnEnterState(prevState);
}

//-----------------------------------------------------------------------

void cLuxPlayer::MoveHeadPosAdd(eLuxHeadPosAdd aType, const cVector3f& avGoal, float afSpeed, float afSlowdownDist)
{
	mvHeadPosAdds[aType].mbMoving = true;
	mvHeadPosAdds[aType].mvMoveGoal = avGoal;
	mvHeadPosAdds[aType].mfMoveSpeed = afSpeed;
	mvHeadPosAdds[aType].mfMoveSlowDownDist = afSlowdownDist;
}

//-----------------------------------------------------------------------

void cLuxPlayer::PlaceAtStartNode(cLuxNode_PlayerStart *apNode)
{
	ChangeState(eLuxPlayerState_Normal);

	SetIsInWater(false);//Needs to rest this or else player will spalsh when walking on the ground.
	if(mpSpawnPS->IsActive())
		mpSpawnPS->RespawnAll(); //Needs to respawn all!

	mpCharBody->SetFeetPosition(apNode->GetPosition());
	mpCharBody->SetYaw(apNode->GetAngle());
	mpCamera->SetYaw(apNode->GetAngle());
	mpCamera->SetPitch(0);
	mpCharBody->Update(0.001f);
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetHeadPosAdd(eLuxHeadPosAdd aType, const cVector3f& avVector)
{
	mvHeadPosAdds[aType].mvAdd = avVector;
}

const cVector3f& cLuxPlayer::GetHeadPosAdd(eLuxHeadPosAdd aType)
{
	return mvHeadPosAdds[aType].mvAdd;
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetActive(bool abX)
{
	mbActive = abX;
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetHealth(float afX)
{
	if(mfHealth <=0 && afX <= 0) return;

	mfHealth = afX;

	if(mfHealth <=0)
	{
		mpDeath->Start();
	}
}

void cLuxPlayer::SetSanity(float afX)
{
	mfSanity = afX;

	//////////////////
	// HARDMODE
	if (gpBase->mbHardMode)
	{
		if (mfSanity <= 0)
		{
			SetHealth(0.f);
		}
	}

}

void cLuxPlayer::SetLampOil(float afX)
{
	mfLampOil = afX;
}

//-----------------------------------------------------------------------

void cLuxPlayer::AddHealth(float afX)
{
	if( (mfHealth >= 100 && afX>0) || mfHealth<0) return;
	
	mfHealth += afX;
	if(mfHealth > 100) mfHealth = 100;

	if(mfHealth <=0)
	{
		mpDeath->Start();
	}
}

void cLuxPlayer::AddSanity(float afX, bool abShowEffect)
{
	if(mfSanity >= 100 && afX>0) return;

	mfSanity += afX;
	if(mfSanity > 100) mfSanity = 100;
	if(mfSanity < 0)
	{
		mfSanity = 0;
		mpInsanityCollapse->Start();

		//////////////////
		// HARDMODE
		if (gpBase->mbHardMode)
		{
			SetHealth(0.f);
		}
	}

	if(afX >0 && abShowEffect)
	{
		gpBase->mpEffectHandler->GetSanityGainFlash()->Start();
	}
}

void cLuxPlayer::AddLampOil(float afX)
{
	if(mfLampOil >= 100 && afX>0) return;

	mfLampOil += afX;
	if(mfLampOil > 100) mfLampOil = 100;
	if(mfLampOil < 0) mfLampOil = 0;
}

//-----------------------------------------------------------------------

void cLuxPlayer::AddTerrorEnemy(iLuxEnemy *apEnemy)
{
	m_setTerrorEnemies.insert(apEnemy);
}

void cLuxPlayer::RemoveTerrorEnemy(iLuxEnemy *apEnemy)
{
	m_setTerrorEnemies.erase(apEnemy);
}

void cLuxPlayer::ClearTerrorEnemies()
{
	m_setTerrorEnemies.clear();
}

void cLuxPlayer::StopTerrorSound()
{
	if(mpTerrorSound && gpBase->mpEngine->GetSound()->GetSoundHandler()->IsValid(mpTerrorSound, mlTerrorSoundID))
	{
		mpTerrorSound->Stop();
		mpTerrorSound = NULL;
		mlTerrorSoundID = -1;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetFocusText(const tWString &asText)
{
	if(msFocusText == asText) return;

	msLastFocusText = msFocusText;
	msFocusText = asText;
}

//-----------------------------------------------------------------------

void cLuxPlayer::AddCoins(int alX)
{
	mlCoins += alX;
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetCurrentHandObjectDrawn(bool abX)
{
	cLuxInventory_Item *pItem = gpBase->mpInventory->GetEquippedHandItem();
	if(pItem == NULL) return;

	////////////////
	// Draw
    if(abX)
	{
		mpHands->SetActiveHandObject(pItem->GetSubType());
	}
	////////////////
	// Holster
	else
	{
		mpHands->SetActiveHandObject("");
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::FadeFOVMulTo(float afX, float afSpeed)
{
	mfFOVMulGoal = afX;
	mfFOVMulSpeed = afSpeed;
}

void cLuxPlayer::FadeAspectMulTo(float afX, float afSpeed)
{
	mfAspectMulGoal = afX;
	mfAspectMulSpeed = afSpeed;
}

void cLuxPlayer::FadeRollTo(float afX, float afSpeedMul, float afMaxSpeed)
{
	mfRollGoal = afX;
	mfRollSpeedMul = afSpeedMul;
	mfRollMaxSpeed = afMaxSpeed;
}

void cLuxPlayer::FadeLeanRollTo(float afX, float afSpeedMul, float afMaxSpeed)
{
	mfLeanRollGoal = afX;
	mfLeanRollSpeedMul = afSpeedMul;
	mfLeanRollMaxSpeed = afMaxSpeed;
}

void cLuxPlayer::SetRoll(float afX)
{
	mfRoll = afX;
	mfRollGoal = afX;

}

//-----------------------------------------------------------------------

eLuxFocusIconStyle cLuxPlayer::StringToFocusIconStyle(const tString& asX)
{
	tString sLowCase = cString::ToLowerCase(asX);
	
	if(sLowCase=="default")
		return eLuxFocusIconStyle_Default;
	else if(sLowCase=="simple")
		return eLuxFocusIconStyle_Simple;
	else
		Log("Error converting string to FocusIconStyle : '%s'\n", asX.c_str());

	return eLuxFocusIconStyle_Default;
}

tString cLuxPlayer::FocusIconStyleToString(eLuxFocusIconStyle aX)
{
	if(aX==eLuxFocusIconStyle_Default)
		return "Default";
	else if(aX==eLuxFocusIconStyle_Simple)
		return "Simple";

	return "Default";
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Free cam
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxPlayer::SetFreeCamActive(bool abX)
{
	mbFreeCameraActive = abX;

	if (abX)
	{
		mpCharBody->SetCamera(NULL);
	}
	else
	{
		mpCharBody->SetCamera(mpCamera);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::SetFreeCamSpeed(float afSpeed)
{
	mfFreeCameraSpeed = afSpeed;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxPlayer::CanDrawCrossHair()
{
	if(gpBase->mpMessageHandler->IsPauseMessageActive()) return false;
	if(mbScriptShowFocusIconAndCrossHair==false) return false;
	if(mbShowCrossHair==false && mState != eLuxPlayerState_UseItem)
	{
		if(mpEntityInFocus)
		{
			if(mpEntityInFocus->CanInteract(mpBodyInFocus) && mfCurrentFocusDistance <= mpEntityInFocus->GetMaxFocusDistance()) 
				return true;
			else
				return false;
		}
		else
		{
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------

void cLuxPlayer::DrawHud(float afFrameTime)
{
	cVector2f vSetSize = gpBase->mvHudVirtualCenterSize;

	/////////////////////////////////////////
	// Cross hair
	if(CanDrawCrossHair())
	{
        cGuiGfxElement *pCrossGfx = mvStates[mState]->GetCrosshair();
		if(pCrossGfx)
		{
			cVector2f vGfxSize = pCrossGfx->GetImageSize();
			cVector2f vPos = (vSetSize - vGfxSize)/2.0f;
			cVector3f vFinalPos = cVector3f(vPos.x, vPos.y,1);

			if(mvStates[mState]->OnDrawCrossHair(pCrossGfx,vFinalPos, vGfxSize))
			{
				//Log("Drawcrosshair: %p\n", pCrossGfx);
				gpBase->mpGameHudSet->DrawGfx(pCrossGfx,vFinalPos,vGfxSize,cColor(1,1));
			}
		}
	}
	
	/////////////////////////////////////////
	// Focus text
	float fFocusTextY = 450;
	if(msFocusText != _W(""))
	{
		tWStringVec vRows;
		mpFocusFont->GetWordWrapRows(500, 22, 22, msFocusText,&vRows);
        
		for(size_t i=0; i<vRows.size(); ++i)
			gpBase->mpGameHudSet->DrawFont(vRows[i],mpFocusFont,cVector3f(400, fFocusTextY+i*24,1),22,cColor(1,mfFocusTextAlpha),	eFontAlign_Center);
	}
	else if(mfFocusTextAlpha >0)
	{
		tWStringVec vRows;
		mpFocusFont->GetWordWrapRows(500, 22, 22, msLastFocusText,&vRows);

		for(size_t i=0; i<vRows.size(); ++i)
			gpBase->mpGameHudSet->DrawFont(vRows[i],mpFocusFont,cVector3f(400, fFocusTextY+i*24,1),22,cColor(1,mfFocusTextAlpha),	eFontAlign_Center);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::RunHelperMessage(eUpdateableMessage aMessage, float afX)
{
	for(size_t i=0; i<mvHelpers.size(); ++i)
	{
		iLuxPlayerHelper *pHelper = mvHelpers[i];
		pHelper->RunMessage(aMessage, afX);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::RunHelperLuxMessage(eLuxUpdateableMessage aMessage, void *apData)
{
	for(size_t i=0; i<mvHelpers.size(); ++i)
	{
		iLuxPlayerHelper *pHelper = mvHelpers[i];
		pHelper->LuxRunMessage(aMessage, apData);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateHeadPosAdd(float afTimeStep)
{
	cVector3f vAdd = mvCameraPosAdd;

	/////////////////////////
	// Iterate different head offsets and add
    for(size_t i=0; i<mvHeadPosAdds.size(); ++i)
	{
		cLuxHeadPosAdd *pPosAdd = &mvHeadPosAdds[i];

		/////////////////////
		// Movement
		if(pPosAdd->mbMoving)
		{
			cVector3f vDir = pPosAdd->mvMoveGoal - pPosAdd->mvAdd;
			float fDist = vDir.Length();
			vDir = vDir / fDist;
			
			//If close enough to goal, slow down.
            float fSpeed = pPosAdd->mfMoveSpeed;
			if(fDist < pPosAdd->mfMoveSlowDownDist)
			{
				fSpeed *= fDist / pPosAdd->mfMoveSlowDownDist;
			}

			pPosAdd->mvAdd += vDir * fSpeed * afTimeStep;

			if(fDist < 0.001f)
			{
				pPosAdd->mbMoving = false;
				pPosAdd->mvAdd = pPosAdd->mvMoveGoal;
			}
		}

		vAdd += pPosAdd->mvAdd;
	}
	mvHeadPosAddSum = vAdd - mvCameraPosAdd;

	
	mpCharBody->SetCameraPosAdd(vAdd);
}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateCamera(float afTimeStep)
{
	if (mbFreeCameraActive) return;


	////////////////
	// FOV
	if(mfFOVMul != mfFOVMulGoal)
	{
		float fSpeed = (mfFOVMulGoal - mfFOVMul) * mfFOVMulSpeed;
		mfFOVMul += afTimeStep * fSpeed;
		
		if(cMath::Abs(mfFOVMulGoal - mfFOVMul) < 0.004f) 
			mfFOVMul = mfFOVMulGoal;

		mpCamera->SetFOV(mfFOV*mfFOVMul);
	}

	////////////////
	// Aspect
	if(mfAspectMul != mfAspectMulGoal)
	{
		float fSpeed = (mfAspectMulGoal - mfAspectMul) * mfAspectMulSpeed;
		mfAspectMul += afTimeStep * fSpeed;
		
		if(cMath::Abs(mfAspectMulGoal - mfAspectMul) < 0.004f) mfAspectMul = mfAspectMulGoal;

		mpCamera->SetAspect(mfAspect*mfAspectMul);
	}

	bool bUpdatedRoll = false;
	////////////////
	// Roll
	if(mfRoll != mfRollGoal)
	{
		float fSpeed = (mfRollGoal - mfRoll) * mfRollSpeedMul;
		if(fSpeed > mfRollMaxSpeed) fSpeed = mfRollMaxSpeed;
		if(fSpeed < -mfRollMaxSpeed) fSpeed = -mfRollMaxSpeed;
		
		mfRoll += afTimeStep * fSpeed;

		if(cMath::Abs(mfRollGoal - mfRoll) < 0.004f) mfRoll = mfRollGoal;

		bUpdatedRoll = true;
	}

	////////////////
	// Lean Roll
	if(mfLeanRoll != mfLeanRollGoal)
	{
		float fSpeed = (mfLeanRollGoal - mfLeanRoll) * mfLeanRollSpeedMul;
		if(fSpeed > mfLeanRollMaxSpeed) fSpeed = mfLeanRollMaxSpeed;
		if(fSpeed < -mfLeanRollMaxSpeed) fSpeed = -mfLeanRollMaxSpeed;

		mfLeanRoll += afTimeStep * fSpeed;

		if(cMath::Abs(mfLeanRollGoal - mfLeanRoll) < 0.004f) mfLeanRoll = mfLeanRollGoal;

		bUpdatedRoll = true;
	}

	if(bUpdatedRoll)
		mpCamera->SetRoll(mfRoll + mfLeanRoll);

	////////////////
	// Cam pos
	if(mvCamAnimPos != mvCamAnimPosGoal)
	{
		cVector3f vDir = mvCamAnimPosGoal - mvCamAnimPos;
		float fSpeed = vDir.Length() * mfCamAnimPosSpeedMul;
		if(fSpeed > mfCamAnimPosMaxSpeed) fSpeed = mfCamAnimPosMaxSpeed;
		vDir.Normalize();

        mvCamAnimPos += vDir * fSpeed;
	}

}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateTerror(float afTimeStep)
{
	if(m_setTerrorEnemies.empty() || IsDead())
	{
		mfTerror -= mfTerrorDecSpeed * afTimeStep;
		if(mfTerror < 0) mfTerror =0;
	}
	else
	{
		float fSpeed = mpCharBody->GetVelocity(afTimeStep).Length();

		mfTerror += mfTerrorIncSpeed * afTimeStep; //+ fSpeed * afTimeStep * 0.1f;
		if(mfTerror > 1) mfTerror = 1;
	}


	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	if(mfTerror > 0)
	{
		if(mpTerrorSound == NULL)
		{
			mpTerrorSound = pSoundHandler->PlayGui(msTerrorSound,true,1.0f);
			if(mpTerrorSound)
			{
				mpTerrorSound->SetVolumeMul(0.0f);
				mlTerrorSoundID = mpTerrorSound->GetId();
			}
		}
	}
	if(mpTerrorSound && pSoundHandler->IsValid(mpTerrorSound, mlTerrorSoundID))
	{
		mpTerrorSound->SetVolumeMul(mfTerror);
		if(mfTerror <= 0)
		{
			mpTerrorSound->GetChannel()->Stop();
			mpTerrorSound = NULL;
			mlTerrorSoundID = -1;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::SpinHead(float afSpeed)
{
	mvHeadSpinSpeed = cVector2f(cMath::RandRectf(-1,1), cMath::RandRectf(0,0.5f));
	if(mvHeadSpinSpeed.x == 0 && mvHeadSpinSpeed.y == 0) mvHeadSpinSpeed.x = 1;
	else mvHeadSpinSpeed.Normalize();

	mvHeadSpinSpeed *= afSpeed;

}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateHeadSpin(float afTimeStep)
{
	if(mvHeadSpinSpeed == 0)
	{
		return;
	}
	
	mpCamera->AddPitch(mvHeadSpinSpeed.y * afTimeStep);
	mpCamera->AddYaw(mvHeadSpinSpeed.x * afTimeStep);
	mpCharBody->SetYaw(mpCamera->GetYaw());

	float fLength = mvHeadSpinSpeed.Length();
	cVector2f vDir = mvHeadSpinSpeed;
	vDir.Normalize();
	
	fLength -= afTimeStep * mfHeadSpinDeacc;
	if(fLength < 0) fLength = 0;
    mvHeadSpinSpeed = vDir * fLength;	
}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateFocusText(float afTimeStep)
{
	if(	gpBase->mpMessageHandler->IsPauseMessageActive() || 
		//gpBase->mpMessageHandler->IsMessageActive() || <- this interfeere with signs and stuff so not a good thing to have!
		msFocusText == _W(""))
	{
		mfFocusTextAlpha -= afTimeStep*2.0f;
		if(mfFocusTextAlpha < 0.0f) mfFocusTextAlpha =0;
	}
	else
	{
		mfFocusTextAlpha += afTimeStep*2.0f;
		if(mfFocusTextAlpha > 1.0f) mfFocusTextAlpha =1;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayer::UpdateAvgSpeed(float afTimeStep)
{
	float fSpeed = mpCharBody->GetVelocity(afTimeStep).Length();
	mlstPrevSpeeds.push_back(fSpeed);
	if((int)mlstPrevSpeeds.size() > mlMaxPrevSpeeds) mlstPrevSpeeds.pop_front();

	mfAvgSpeed = 0;
	std::list<float>::iterator it = mlstPrevSpeeds.begin();
	for(; it != mlstPrevSpeeds.end(); ++it)
	{
		mfAvgSpeed += *it;
	}
	mfAvgSpeed /= (float)mlstPrevSpeeds.size();
}

//-----------------------------------------------------------------------

void cLuxPlayer::CreateCharacterBody(iPhysicsWorld *apPhysicsWorld)
{
	mpCharBody = apPhysicsWorld->CreateCharacterBody("Player",mvBodySize);
	
	//TODO: Use config here.
	mpCharBody->SetCustomGravity(	gpBase->mpGameCfg->GetVector3f("Player_Body","GravityForce",0) );
	mpCharBody->SetCustomGravityActive(true);
	mpCharBody->SetMass(mfDefaultMass);

	mpCharBody->SetAccurateClimbing(	gpBase->mpGameCfg->GetBool("Player_Body","AccurateClimbing",false) );
	mpCharBody->SetMaxNoSlideSlopeAngle(cMath::ToRad(gpBase->mpGameCfg->GetFloat("Player_Body","MaxNoSlideSlopeAngle",0) ) );
	mpCharBody->SetMaxPushMass(			gpBase->mpGameCfg->GetFloat("Player_Body","MaxPushMass",0) );
	mpCharBody->SetPushForce(			gpBase->mpGameCfg->GetFloat("Player_Body","PushForce",0) );
	mpCharBody->SetCharacterMaxPushMass(gpBase->mpGameCfg->GetFloat("Player_Body","CharacterMaxPushMass",0) );
	mpCharBody->SetCharacterPushForce(	gpBase->mpGameCfg->GetFloat("Player_Body","CharacterPushForce",0) );
	mpCharBody->SetMaxStepSize(			gpBase->mpGameCfg->GetFloat("Player_Body","MaxStepSize",0) );
	mpCharBody->SetMaxStepSizeInAir(	gpBase->mpGameCfg->GetFloat("Player_Body","MaxStepSizeInAir",0) );
	mpCharBody->SetStepClimbSpeed(		gpBase->mpGameCfg->GetFloat("Player_Body","StepClimbSpeed",0) );
	
	mpCharBody->SetCamera(mpCamera);
	mpCharBody->SetCameraPosAdd(mvCameraPosAdd);
	mpCharBody->SetCameraSmoothPosNum(	gpBase->mpGameCfg->GetInt("Player_Body","CameraSmoothPosNum",0) );

	//Create crouch extra
	mpCharBody->AddExtraSize(mvBodyCrouchSize);

	//temp placement!
	mpCharBody->SetFeetPosition(cVector3f(8.5f, 5.2f, 9.25f));
	mpCharBody->Update(0.001f);

	//mpCharBody->SetActive(false);
}


//-----------------------------------------------------------------------



