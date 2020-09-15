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

#include "LuxPlayerHelpers.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxMoveState_Normal.h"
#include "LuxMusicHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxInventory.h"
#include "LuxEffectHandler.h"
#include "LuxEnemy.h"
#include "LuxProp.h"
#include "LuxProp_CritterBase.h"
#include "LuxHintHandler.h"
#include "LuxCompletionCountHandler.h"
#include "LuxInputHandler.h"
#include "LuxSaveHandler.h"
#include "LuxPlayerHands.h"
#include "LuxPostEffects.h"
#include "LuxProgressLogHandler.h"
#include "LuxDebugHandler.h"
#include "LuxPlayerState.h"
#include "LuxLoadScreenHandler.h"
#include "LuxMainMenu.h"


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER INSANITY COLLAPSE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerInsanityCollapse::cLuxPlayerInsanityCollapse(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerInsanityCollapse")
{
	mfHeightAddGoal = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_HeightAddGoal",0);
	
	mfHeightAddCollapseSpeed = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_HeightAddCollapseSpeed",0);
	mfHeightAddAwakeSpeed = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_HeightAddAwakeSpeed",0);
	mfRollCollapseSpeed = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_RollCollapseSpeed",0);
	mfRollAwakeSpeed = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_RollAwakeSpeed",0);

	mfSleepTime = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_SleepTime",0);

	mfSleepSpeedMul = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_SleepSpeedMul",0);
	mfWakeUpSpeedMul = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_WakeUpSpeedMul",0);

	msStartSound = gpBase->mpGameCfg->GetString("Player_General","InsanityCollapse_StartSound", "");
	msAwakenSound = gpBase->mpGameCfg->GetString("Player_General","InsanityCollapse_AwakenSound", "");
	msSleepLoopSound = gpBase->mpGameCfg->GetString("Player_General","InsanityCollapse_SleepLoopSound", "");
	mfSleepLoopSoundVolume = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_SleepLoopSoundVolume", 0);

	msSleepRandomSound = gpBase->mpGameCfg->GetString("Player_General","InsanityCollapse_SleepRandomSound", "");
	mfSleepRandomMinTime = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_SleepRandomMinTime", 0);
	mfSleepRandomMaxTime = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_SleepRandomMaxTime", 0);

	mfAwakenSanity = gpBase->mpGameCfg->GetFloat("Player_General","InsanityCollapse_AwakenSanity", 0);

	//Init sound var here
	mpLoopSound = NULL;
}

cLuxPlayerInsanityCollapse::~cLuxPlayerInsanityCollapse()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerInsanityCollapse::Reset()
{
	mbActive = false;
	mlState =0;
	mfHeightAdd =0;
	mfRoll =0;
	mfT=0;
	mfRandomCount=0;

	if(mpLoopSound && gpBase->mpEngine->GetSound()->GetSoundHandler()->IsValid(mpLoopSound, mlLoopSoundID))
	{
		mpLoopSound->Stop();
		mpLoopSound = NULL;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerInsanityCollapse::Start()
{
	if(mbActive) return;

	/////////////////
	//Play sound
	gpBase->mpHelpFuncs->PlayGuiSoundData(msStartSound, eSoundEntryType_World);

	/////////////////
	// Setup player
	mpPlayer->ChangeState(eLuxPlayerState_Normal);
	mpPlayer->ChangeMoveState(eLuxMoveState_Normal);
	mpPlayer->GetHelperLantern()->SetActive(false,false, false);
	mpPlayer->SetCurrentHandObjectDrawn(false);
	mpPlayer->SetInsanityCollapseSpeedMul(mfSleepSpeedMul);
	
	/////////////////
	//Set up sound
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	pSoundHandler->FadeGlobalVolume(0.75f, 0.15f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);
	pSoundHandler->FadeGlobalSpeed(0.5f, 0.125f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);

	/////////////////
	//Loop sound
	mpLoopSound = pSoundHandler->PlayGui(msSleepLoopSound,true,1.0f);
	if(mpLoopSound)
	{
		mpLoopSound->FadeIn(mfSleepLoopSoundVolume, 0.2f);
		mlLoopSoundID = mpLoopSound->GetId();
	}

	/////////////////
	//Attract enemies
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	cLuxEnemyIterator enemyIt = pMap->GetEnemyIterator();
	while(enemyIt.HasNext())
	{
		iLuxEnemy *pEnemy = enemyIt.Next();
        if(pEnemy->IsActive()) pEnemy->AlertOfPlayerPresence();
	}

	/////////////////
	//Setup variables.	
	mbActive = true;

	mlState =0;
	mfHeightAdd =0;
	mfRoll =0;
	mfT=0;
	mfRandomCount=0;
}

//-----------------------------------------------------------------------

void cLuxPlayerInsanityCollapse::Stop()
{
	if(mbActive==false) return;

	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	pSoundHandler->FadeGlobalVolume(1, 0.3f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);
	pSoundHandler->FadeGlobalSpeed(1, 0.5f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);

	mpPlayer->SetInsanityCollapseSpeedMul(1.0f);
	
	if(mpLoopSound && pSoundHandler->IsValid(mpLoopSound, mlLoopSoundID))
	{
		mpLoopSound->FadeOut(1.0f);
	}

	mbActive = false;
	
	mlState =0;
	mfT =0;
	mfRoll =0;
	mfHeightAdd=0;
	mpPlayer->FadeRollTo(0, 10,10);
	mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_InsanityCollapse, cVector3f(0,0,0),1, 0.1f);
}

//-----------------------------------------------------------------------

void cLuxPlayerInsanityCollapse::Update(float afTimeStep)
{
	if(mbActive==false) return;
	
	////////////////////////
	// Collapse
	if(mlState == 0 || mlState == 1)
	{
		//////////////////////
		// Height add
		if(mfHeightAdd > mfHeightAddGoal)
		{
			mfHeightAdd-= mfHeightAddCollapseSpeed*afTimeStep;
			if(mfHeightAdd < mfHeightAddGoal)
			{
				mfHeightAdd = mfHeightAddGoal;
				mlState = 1;
			}
			mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_InsanityCollapse, cVector3f(0,mfHeightAdd,0));
		}

		//////////////////////
		// Roll
		mfRoll += cMath::ToRad(mfRollCollapseSpeed)*afTimeStep;
		if(mfRoll > cMath::ToRad(35.0f)) mfRoll = cMath::ToRad(35.0f);

		mpPlayer->FadeRollTo(mfRoll, 10,10);
	}
	////////////////////////
	// Sleep
	if(mlState == 1)
	{
        mfT += afTimeStep;

		////////////////////////
		// Random sounds
		mfRandomCount -= afTimeStep;
		if(mfRandomCount <0)
		{
			gpBase->mpHelpFuncs->PlayGuiSoundData(msSleepRandomSound, eSoundEntryType_Gui);
			mfRandomCount = cMath::RandRectf(mfSleepRandomMinTime, mfSleepRandomMaxTime);
		}

		///////////////////////
		//Wake up
		if(mfT > mfSleepTime)
		{
			cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
			pSoundHandler->FadeGlobalVolume(1, 0.2f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);
			pSoundHandler->FadeGlobalSpeed(1, 0.25f, eSoundEntryType_World,eLuxGlobalVolumeType_InsanityCollapse, false);

			mpPlayer->SetInsanityCollapseSpeedMul(mfWakeUpSpeedMul);

			mpPlayer->FadeRollTo(0, 5, 6);

			if(mpLoopSound && pSoundHandler->IsValid(mpLoopSound, mlLoopSoundID))
			{
				mpLoopSound->FadeOut(0.3f);
			}

			mpPlayer->SetSanity(mfAwakenSanity);

			gpBase->mpHelpFuncs->PlayGuiSoundData(msAwakenSound, eSoundEntryType_World);
			mlState =2;
		}
	}
	////////////////////////
	// Awake
	else if(mlState == 2)
	{
		//////////////////////
		// Height add
		if(mfHeightAdd < 0)
		{
			mfHeightAdd += mfHeightAddAwakeSpeed*afTimeStep;
			if(mfHeightAdd > 0)
			{
				mfHeightAdd = 0;
				mbActive = false;
				mpPlayer->SetInsanityCollapseSpeedMul(1.0f);
				mpPlayer->SetCrouchDisabled(false);
				mpPlayer->SetJumpDisabled(false);
			}
			mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_InsanityCollapse, cVector3f(0,mfHeightAdd,0));
		}
	}
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER SIGHT SWAY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerCamDirEffects::cLuxPlayerCamDirEffects(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerCamDirEffects")
{
	mfStartSwayMaxSanity =  gpBase->mpGameCfg->GetFloat("Player_Sanity","StartSwayMaxSanity",0);
	mlMaxPositions = gpBase->mpGameCfg->GetInt("Player_Sanity","SwayMaxSavedPositions",0);
}

cLuxPlayerCamDirEffects::~cLuxPlayerCamDirEffects()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerCamDirEffects::Reset()
{
	mbSwayActive = false;

	mlstPrevAdd.clear();
	mvSwayAdd = 0;
	
	mvNextAdd = 0;

	mfSwayAlpha =0;
}

//-----------------------------------------------------------------------


void cLuxPlayerCamDirEffects::Update(float afTimeStep)
{
	////////////////////
	// Check if insane
	if(mbSwayActive==false && mpPlayer->GetSanity() <= mfStartSwayMaxSanity)
	{
		SetSwayActive(true);
	}
	else if(mbSwayActive && mpPlayer->GetSanity() > mfStartSwayMaxSanity)
	{
		SetSwayActive(false);
	}

	////////////////////
	// Alpha
	if(mbSwayActive && mfSwayAlpha<1)
	{
		mfSwayAlpha += afTimeStep *0.1f;
		if(mfSwayAlpha > 1) mfSwayAlpha =1;
	}
	else if(mbSwayActive==false && mfSwayAlpha>0)
	{
		mfSwayAlpha -= afTimeStep *0.2f;
		if(mfSwayAlpha < 0)
		{
			mfSwayAlpha =0;
			mlstPrevAdd.clear();
			mvNextAdd = 0;
			mvSwayAdd = 0;
		}
	}

	////////////////////
	// Check Sane
	UpdateSway(afTimeStep);
}

//-----------------------------------------------------------------------

float cLuxPlayerCamDirEffects::AddAndGetYawAdd(float afX)
{
	float fYaw = afX;
	if(mlstPrevAdd.empty()==false) fYaw = mvSwayAdd.x * mfSwayAlpha + afX *(1-mfSwayAlpha);

	if(mfSwayAlpha>0) mvNextAdd.x  = afX;
	return fYaw;
}

float cLuxPlayerCamDirEffects::AddAndGetPitchAdd(float afX)
{
	float fPitch = afX;
	if(mlstPrevAdd.empty()==false) fPitch = mvSwayAdd.y*mfSwayAlpha + afX *(1-mfSwayAlpha);

	if(mfSwayAlpha>0) mvNextAdd.y  = afX;
	return fPitch;
}

//-----------------------------------------------------------------------

void cLuxPlayerCamDirEffects::SetSwayActive(bool abX)
{
	if(mbSwayActive == abX) return;

	mbSwayActive = abX;
}

//-----------------------------------------------------------------------


void cLuxPlayerCamDirEffects::UpdateSway(float afTimeStep)
{
	if(mbSwayActive==false && mlstPrevAdd.empty() && mfSwayAlpha<=0) return;

	//////////////////////////////////
	//Add Yaw and Pitch and pop front is needed
	mlstPrevAdd.push_back(mvNextAdd);
	mvNextAdd =0;

	if((int)mlstPrevAdd.size() > mlMaxPositions)
	{
		mlstPrevAdd.pop_front();
	}
	
	////////////////////////////
	// Calculate the sway add
	cVector2f vTotal(0);
	tVector2fListIt it = mlstPrevAdd.begin();
	for(; it != mlstPrevAdd.end(); ++it)
	{
		vTotal += *it;
	}

	mvSwayAdd = vTotal/(float)mlstPrevAdd.size();
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER SPAWN PS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerSpawnPS::cLuxPlayerSpawnPS(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerSpawnPS")
{

}

cLuxPlayerSpawnPS::~cLuxPlayerSpawnPS()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::Reset()
{
	mbActive = false;
	msFileName = "";

	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap && mvSpawnPos.empty()==false)
	{
		cWorld *pWorld = pMap->GetWorld();
		for(size_t i=0; i<mvSpawnPos.size(); ++i)
		{
			cParticleSystem *pPS = mvSpawnPos[i].mpPS;

			if(pWorld->ParticleSystemExists(pPS))
				pWorld->DestroyParticleSystem(pPS);
		}
	}

	mvSpawnPos.clear();
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::Start(const tString& asFileName)
{
	if(LoadSpawnPSFile(asFileName)==false)
	{
		return;
	}

	msFileName = asFileName;
	mbActive = true;
	
	RespawnAll();
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::Stop()
{
	mbActive = false;
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::RespawnAll()
{
	DestroyAllSpawnPoints();
	GenerateAllSpawnPos();
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::Update(float afTimeStep)
{
	if(mbActive==false) return;

	cVector3f vPlayerPos = mpPlayer->GetCharacterBody()->GetFeetPosition();
	cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();

	for(size_t i=0; i<mvSpawnPos.size(); ++i)
	{
		cLuxPlayerSpawnPS_SpawnPos &spawnPos = mvSpawnPos[i];

		cVector3f vCurrentLocal = spawnPos.mvPos - vPlayerPos;

		///////////////////
		// Check if outside of "radius" (really a square)
		if(fabs(vCurrentLocal.x) > mfRadius || fabs(vCurrentLocal.z) > mfRadius)
		{
			////////////////////
			//Kill PS
			cParticleSystem *pPS = spawnPos.mpPS;
			if(pPS && pWorld->ParticleSystemExists(pPS))
			{
				pPS->Kill();
			}

			////////////////////
			//Get new position, opposite of the old
			cVector3f vAdd(0);
			cVector3f vMul(1);
			if(vCurrentLocal.x > mfRadius){			vAdd.x = vCurrentLocal.x - mfRadius;	vMul.x=-1; }
			else if(vCurrentLocal.x < -mfRadius){	vAdd.x = vCurrentLocal.x + mfRadius;	vMul.x=-1; }
			if(vCurrentLocal.z > mfRadius){			vAdd.z = vCurrentLocal.z - mfRadius;	vMul.z=-1; }
			else if(vCurrentLocal.z < -mfRadius){	vAdd.z = vCurrentLocal.z + mfRadius;	vMul.z=-1; }

			cVector3f vTemp = vCurrentLocal;
			vCurrentLocal = vCurrentLocal*vMul + vAdd*2;//*2=first remove the offset and then put it inside.

			//Still do random height!
			vCurrentLocal.y = vPlayerPos.y;
			vCurrentLocal.y += mfHeightFromFeet + cMath::RandRectf(mfHeightAddMin, mfHeightAddMax);

			spawnPos.mvPos = vPlayerPos + vCurrentLocal;

			////////////////////
			//Create Particle System
			spawnPos.mpPS = CreatePS(&spawnPos);
		}
		///////////////////
		// Update the local pos
		else
		{
			spawnPos.mvLastLocalPos = vCurrentLocal;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::GenerateAllSpawnPos()
{
	cVector3f vPlayerPos = mpPlayer->GetCharacterBody()->GetFeetPosition();

	cVector3f vStep = cVector3f(1/mfDensity, 0, 1/mfDensity);
	cVector3f vLocalStartPos = cVector3f(-mfRadius, 0, -mfRadius) + vStep*0.5f;
	cVector3f vLocalPos = vLocalStartPos;

	for(; vLocalPos.z < mfRadius; vLocalPos.z += vStep.z)
	{
		vLocalPos.x = vLocalStartPos.x;

		for(; vLocalPos.x < mfRadius; vLocalPos.x += vStep.x)
		{
			cLuxPlayerSpawnPS_SpawnPos spawnPos;

			float fRandRange = (1.0f/mfDensity)*0.2f;
			cVector3f vFinalLocalPos = vLocalPos +	cMath::RandRectVector3f(cVector3f(-fRandRange,0,-fRandRange),
																			cVector3f(fRandRange,0,fRandRange));
			vFinalLocalPos.y += mfHeightFromFeet + cMath::RandRectf(mfHeightAddMin, mfHeightAddMax);

			spawnPos.mvLastLocalPos = vFinalLocalPos;
			spawnPos.mvPos = vPlayerPos + vFinalLocalPos;

			spawnPos.mpPS = CreatePS(&spawnPos);
			if(spawnPos.mpPS)
				mvSpawnPos.push_back(spawnPos);
		}
	}
}

//-----------------------------------------------------------------------

cParticleSystem* cLuxPlayerSpawnPS::CreatePS(cLuxPlayerSpawnPS_SpawnPos *apSpawnPos)
{
	cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();
	cParticleSystem *pPS = pWorld->CreateParticleSystem("SpawnPS", msParticleSystem, 1);
	if(pPS)
	{
		pPS->SetPosition(apSpawnPos->mvPos);
		pPS->SetFadeAtDistance(mbFadePS);
		pPS->SetMinFadeDistanceStart(mfPSMinFadeStart);
		pPS->SetMinFadeDistanceEnd(mfPSMinFadeEnd);
		pPS->SetMaxFadeDistanceStart(mfPSMaxFadeStart);
		pPS->SetMaxFadeDistanceEnd(mfPSMaxFadeEnd);
		pPS->SetColor(mPSColor);
	}
	return pPS;
}

//-----------------------------------------------------------------------

void cLuxPlayerSpawnPS::DestroyAllSpawnPoints()
{
	cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();
	for(size_t i=0; i<mvSpawnPos.size(); ++i)
	{
		//Detroy particle system
		cLuxPlayerSpawnPS_SpawnPos &spawnPos = mvSpawnPos[i];
		cParticleSystem *pPS = spawnPos.mpPS;
		if(pPS && pWorld->ParticleSystemExists(pPS))
		{
			pPS->Kill();
		}		
	}
	mvSpawnPos.clear();
}

//-----------------------------------------------------------------------

bool cLuxPlayerSpawnPS::LoadSpawnPSFile(const tString& asFileName)
{
	tString sFile = cString::SetFileExt(asFileName, "sps");
	cResources *pResources = gpBase->mpEngine->GetResources();

	iXmlDocument *pXmlDoc = pResources->LoadXmlDocument(sFile);
	if(pXmlDoc==NULL)
	{
		Error("Could not load sps file: '%s'\n", sFile.c_str());
		return false;
	}

	msParticleSystem = pXmlDoc->GetAttributeString("ParticleSystem","");
	mfHeightFromFeet = pXmlDoc->GetAttributeFloat("HeightFromFeet",0);
	mfHeightAddMin = pXmlDoc->GetAttributeFloat("HeightAddMin",0);
	mfHeightAddMax = pXmlDoc->GetAttributeFloat("HeightAddMax",0);
	mfDensity = pXmlDoc->GetAttributeFloat("Density",0);
	mfRadius = pXmlDoc->GetAttributeFloat("Radius",0);
	mPSColor = pXmlDoc->GetAttributeColor("PSColor",cColor(0));
	mbFadePS = pXmlDoc->GetAttributeBool("FadePS",true);
	mfPSMinFadeStart = pXmlDoc->GetAttributeFloat("PSMinFadeStart",0);
	mfPSMinFadeEnd = pXmlDoc->GetAttributeFloat("PSMinFadeEnd",0);
	mfPSMaxFadeStart = pXmlDoc->GetAttributeFloat("PSMaxFadeStart",0);
	mfPSMaxFadeEnd = pXmlDoc->GetAttributeFloat("PSMaxFadeEnd",0);
	
	pResources->DestroyXmlDocument(pXmlDoc);

	return true;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER HURT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerHurt::cLuxPlayerHurt(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerHurt")
{
	mfEffectStartHealth =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_EffectStartHealth",0);
	mfMinMoveMul =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_MinSpeedMul",0);

	mfMaxPantCount =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_MaxPantCount",0);
	mfPantSpeed =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_PantSpeed",0);
	mfPantSize =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_PantSize",0);

	mfHealthRegainSpeed =  gpBase->mpGameCfg->GetFloat("Player_General","HealthRegainSpeed",0);
	mfHealthRegainLimit =  gpBase->mpGameCfg->GetFloat("Player_General","HealthRegainLimit",0);

	mfNoiseAlpha =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_NoiseAlpha",0);
	mfNoiseFreq =  gpBase->mpGameCfg->GetFloat("Player_General","Hurt_NoiseFreq",0);
	mNoiseColor =  gpBase->mpGameCfg->GetColor("Player_General","Hurt_NoiseColor",cColor(0));

	cGui *pGui = gpBase->mpEngine->GetGui();
	mvNoiseGfx.resize(8);
	for(size_t i=0; i<mvNoiseGfx.size(); ++i)
	{
		mvNoiseGfx[i] = pGui->CreateGfxTexture("hud_hurt_noise0"+cString::ToString((int)i)+".dds", eGuiMaterial_Modulative,eTextureType_2D);
	}
	
	//mpWhiteGfx = gpBase->mpEngine->GetGui()->CreateGfxFilledRect(cColor(1,1),eGuiMaterial_Modulative);
}

cLuxPlayerHurt::~cLuxPlayerHurt()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerHurt::Reset()
{
	mfAlpha = 0;
	mfPantCount =0;
	mfPantPosAdd = 0;
	mfPantPosAddVel = 0;
	mfPantPosAddDir = 1.0f;
	mlCurrentNoise = 0;
	mfNoiseUpdateCount =0;
}

//-----------------------------------------------------------------------

void cLuxPlayerHurt::Update(float afTimeStep)
{
	///////////////////////////
	// Health regain
	if(mpPlayer->GetHealth() < mfHealthRegainLimit)
	{
		mpPlayer->AddHealth(mfHealthRegainSpeed * afTimeStep);
	}

	////////////////////////////
	// Check if update is needed
	if(mfAlpha <=0 && mpPlayer->GetHealth() > mfEffectStartHealth) return;
	if(mpPlayer->IsDead()) return;

	float fWantedAlpha = 1 - (mpPlayer->GetHealth() / mfEffectStartHealth);
	if(fWantedAlpha > 1) fWantedAlpha = 1;
	if(fWantedAlpha < 0) fWantedAlpha = 0;

	//////////////////////////////
	// Alpha
	if(mfAlpha < fWantedAlpha)
	{
		mfAlpha += afTimeStep;
		if(mfAlpha > fWantedAlpha) mfAlpha = fWantedAlpha;
	}
	else if(mfAlpha > fWantedAlpha)
	{
		mfAlpha -= afTimeStep;
		if(mfAlpha < fWantedAlpha) mfAlpha = fWantedAlpha;
	}

	mpPlayer->SetHurtMoveSpeedMul(mfMinMoveMul*mfAlpha + (1-mfAlpha) );
	
	//////////////////////////////
	// Noise
	mfNoiseUpdateCount -= afTimeStep;
	if(mfNoiseUpdateCount<=0)
	{
		int lNoiseMax = (int)mvNoiseGfx.size()-1;
		mlCurrentNoise += cMath::RandRectl(1, lNoiseMax);
		if(mlCurrentNoise>lNoiseMax) mlCurrentNoise -= lNoiseMax+1;

		mfNoiseUpdateCount = 1.0f / mfNoiseFreq;
	}

	//////////////////////////////
	// Update pant count
	iCharacterBody *pCharBody =mpPlayer->GetCharacterBody();
	float fSpeed = pCharBody->GetVelocity(afTimeStep).Length();
	if(fSpeed < 0.05f)
	{
		if(mfPantCount > 0)
		{
			mfPantCount -= afTimeStep;
		}
	}
	else
	{
		mfPantCount += afTimeStep;

		float fMax = mfMaxPantCount * mfAlpha;
		if(mfPantCount > fMax) mfPantCount = fMax;
	}


	//////////////////////////////
	// Update panting
	if(mfPantCount > 0 && fSpeed < 0.05f)
	{
		mfPantPosAddVel += mfPantPosAddDir * afTimeStep;
		mfPantPosAddVel = cMath::Clamp(mfPantPosAddVel, -1, 1);

		mfPantPosAdd += afTimeStep * mfPantPosAddVel * mfPantSpeed;
		if(mfPantPosAddDir > 0)
		{
			if(mfPantPosAdd > mfPantSize){
				mfPantPosAddDir = -mfPantPosAddDir;
				gpBase->mpHelpFuncs->PlayGuiSoundData("hurt_pant",eSoundEntryType_Gui, cMath::Min(mfAlpha+0.3f, 1.0f) );
			}
		}
		else
		{
			if(mfPantPosAdd < -mfPantSize*0.5f){
				mfPantPosAddDir = -mfPantPosAddDir;
			}
		}
	}
	else if(mfPantPosAdd != 0)
	{
		float fDir = mfPantPosAdd > 0 ? -1 : 1;
		mfPantPosAddVel += fDir * afTimeStep;

		mfPantPosAddDir = 1;

		float fMax = cMath::Abs(mfPantPosAdd / mfPantSize);
		mfPantPosAddVel = cMath::Clamp(mfPantPosAddVel, -fMax, fMax);

		mfPantPosAdd += afTimeStep * mfPantPosAddVel * mfPantSpeed;

		if( (fDir > 0 && mfPantPosAdd >0) || (fDir < 0 && mfPantPosAdd <0) )
		{
			mfPantPosAdd =0;
		}
	}


	mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_Hurt, cVector3f(0,mfPantPosAdd,0));

}

//-----------------------------------------------------------------------

void cLuxPlayerHurt::OnDraw(float afFrameTime)
{
	if(mfAlpha <=0) return;

	cVector2f vNoiseSize(256, 256);
	
	cVector2l vCount = cVector2l( (int)(gpBase->mvHudVirtualSize.x / vNoiseSize.x)+1, (int)(gpBase->mvHudVirtualSize.y / vNoiseSize.y)+1);

	cVector3f vPos = gpBase->mvHudVirtualStartPos + cVector3f(0,0,-1);

	cColor col = mNoiseColor;
	col.a = mfAlpha*mfNoiseAlpha;
	for(int y=0; y<vCount.y; ++y)
	{
		for(int x=0; x<vCount.x; ++x)
		{
			gpBase->mpGameHudSet->DrawGfx(mvNoiseGfx[mlCurrentNoise], vPos, vNoiseSize, cColor(1,1-mfAlpha, 1-mfAlpha));
			vPos.x += vNoiseSize.x;
		}
		vPos.x = gpBase->mvHudVirtualStartPos.x;
		vPos.y += vNoiseSize.y;
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER FLASHBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerFlashback::cLuxPlayerFlashback (cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerFlashback")
{
	mfRadialBlurSize = gpBase->mpGameCfg->GetFloat("Player_General","FlashbackRadialBlurSize", 0.12f);
	mfRadialBlurStartDist = gpBase->mpGameCfg->GetFloat("Player_General","FlashbackRadialBlurStartDist", 0.4f);
	mfWorldSoundVolume = gpBase->mpGameCfg->GetFloat("Player_General","FlashbackWorldSoundVolume", 0.4f);
	mfMoveSpeedMul = gpBase->mpGameCfg->GetFloat("Player_General","FlashbackMoveSpeedMul", 1.0f);
	mfRunSpeedMul = gpBase->mpGameCfg->GetFloat("Player_General","FlashbackRunSpeedMul", 1.0f);

	Reset();

}
cLuxPlayerFlashback::~cLuxPlayerFlashback ()
{
}

//-----------------------------------------------------------------------

void cLuxPlayerFlashback::Reset()
{
	mfFlashDelay =0;
	mbActive = false;
	mlstFlashbackQueue.clear();
}

//-----------------------------------------------------------------------

void cLuxPlayerFlashback::Start(const tString &asFlashbackFile, const tString &asCallback)
{
	if(gpBase->mpDebugHandler->GetDisableFlashBacks()) return;

	ProgLog(eLuxProgressLogLevel_Medium, "Starting flashback "+ asFlashbackFile);
	
	if(mbActive)
	{
		mlstFlashbackQueue.push_back(cLuxFlashbackData(asFlashbackFile, asCallback) );
		return;
	}

	//Disable enemies
	gpBase->mpMapHandler->GetCurrentMap()->BroadcastEnemyMessage(eLuxEnemyMessage_Reset, false,0,0);

	mfFlashDelay = 0.5f; //Show flash effect after a little delay
	gpBase->mpHelpFuncs->PlayGuiSoundData("flashback_flash", eSoundEntryType_Gui);
	
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	pMap->AddCompletionAmount(gpBase->mpCompletionCountHandler->mlFlashbackCompletionValue, 2.5f);

	mfFlashbackStartCount = 1.5f;
	msFlashbackFile = asFlashbackFile;
	msCallback = asCallback;

	//World sounds
	gpBase->mpEngine->GetSound()->GetSoundHandler()->FadeGlobalVolume(mfWorldSoundVolume, (1-mfWorldSoundVolume) / 1.5f,eSoundEntryType_World,eLuxGlobalVolumeType_Flashback,false);

	mbActive = true;
}

//-----------------------------------------------------------------------

void cLuxPlayerFlashback::Update(float afTimeStep)
{
	if(mbActive==false)
	{
		if(mlstFlashbackQueue.empty()==false)
		{
			mfFlashbackStartCount -= afTimeStep;
			if(mfFlashbackStartCount < 0)
			{
				cLuxFlashbackData data = mlstFlashbackQueue.front();
				mlstFlashbackQueue.pop_front();

				Start(data.msFile, data.msCallback);
			}
		}

		return;
	}

	////////////////////////////////
	// Flash effect (showed after small delay)
	if(mfFlashDelay > 0)
	{
		mfFlashDelay -= afTimeStep;
		if(mfFlashDelay < 0)
		{
			gpBase->mpEffectHandler->GetFlash()->Start(0.5f, 0.5f, 2.5f);

			gpBase->mpPlayer->SetEventMoveSpeedMul(mfMoveSpeedMul);
			gpBase->mpPlayer->SetEventRunSpeedMul(mfRunSpeedMul);

		}
	}

    ////////////////////////////////
	// Start voices and effects
	if(mfFlashbackStartCount>0)
	{
		mfFlashbackStartCount -= afTimeStep;
		if(mfFlashbackStartCount <= 0)
		{
			//Sepia
			gpBase->mpEffectHandler->GetSepiaColor()->FadeTo(1, 1.0f / 3.5f);

			//Radial blur
			gpBase->mpEffectHandler->GetRadialBlur()->SetBlurStartDist(mfRadialBlurStartDist);
			gpBase->mpEffectHandler->GetRadialBlur()->FadeTo(mfRadialBlurSize, mfRadialBlurSize / 3.5f);
			
			//Play voices
			LoadAndPlayFlashbackFile(msFlashbackFile);
		}
	}
	////////////////////////////////
	// Update effects and check if over
	else
	{
		//TODO: Effects?

		/////////////////////////////////////
		//Are the voices done playing? (note that effects might still be playing, but flash ends when voceis are done!)
		if(gpBase->mpEffectHandler->GetPlayVoice()->VoiceDonePlaying())
		{
			float fFadeTime = 4.0f;
			//Flash at end
			gpBase->mpEffectHandler->GetFlash()->Start(1.0f, 0.1f, 2.5f);
			
			//Post effects
			gpBase->mpEffectHandler->GetSepiaColor()->FadeTo(0, 1.0f / fFadeTime);
			gpBase->mpEffectHandler->GetRadialBlur()->FadeTo(0, mfRadialBlurSize / fFadeTime);

			//World sounds
			gpBase->mpEngine->GetSound()->GetSoundHandler()->FadeGlobalVolume(1, 1 / fFadeTime,eSoundEntryType_World,eLuxGlobalVolumeType_Flashback,true);
			
			gpBase->mpPlayer->SetEventMoveSpeedMul(1.0f);
			gpBase->mpPlayer->SetEventRunSpeedMul(1.0f);

			mbActive = false;

			mfFlashbackStartCount = 6.0f; //Incase there is something in queue, wait 6 seconds and then start that.

			cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
			if(msCallback != "")
				pMap->RunScript(msCallback + "()");
		}
	}
	
}

//-----------------------------------------------------------------------

void cLuxPlayerFlashback::OnDraw(float afFrameTime)
{
}

//-----------------------------------------------------------------------

void cLuxPlayerFlashback::LoadAndPlayFlashbackFile(const tString& asFlashbackFile)
{
	tString sFile = cString::SetFileExt(asFlashbackFile,"flash");
	cResources *pResources = gpBase->mpEngine->GetResources();

	iXmlDocument *pXmlDoc = pResources->LoadXmlDocument(sFile);
	if(pXmlDoc==NULL)
	{
		Error("Could not load flashback file: '%s'\n", sFile.c_str());
		return;
	}

	cXmlElement *pVoicesElem = pXmlDoc->GetFirstElement("Voices");
	if(pVoicesElem==NULL)
	{
		Error("Could not find voice element in flashback file '%s'\n", sFile.c_str());
		pResources->DestroyXmlDocument(pXmlDoc);
		return;
	}

	cXmlNodeListIterator it = pVoicesElem->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement *pChildElem = it.Next()->ToElement();

		tString sVoiceFile = pChildElem->GetAttributeString("VoiceSound","");
		tString sEffectFile = pChildElem->GetAttributeString("EffectSound","");
		tString sTextCat = pChildElem->GetAttributeString("TextCat","");
		tString sTextEntry = pChildElem->GetAttributeString("TextEntry","");

		gpBase->mpEffectHandler->GetPlayVoice()->AddVoice(sVoiceFile, sEffectFile, sTextCat, sTextEntry, false,0,0,0);
	}

	pResources->DestroyXmlDocument(pXmlDoc);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER HEAD MOVE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerLookAt::cLuxPlayerLookAt(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerLookAt")
{
	Reset();	
}

//-----------------------------------------------------------------------


cLuxPlayerLookAt::~cLuxPlayerLookAt()
{
}

//-----------------------------------------------------------------------

void cLuxPlayerLookAt::Update(float afTimeStep)
{
	if(mbActive==false) return;

	cCamera *pCam = mpPlayer->GetCamera();
	cVector3f vGoalAngle = cMath::GetAngleFromPoints3D(pCam->GetPosition(),mvTargetPos);

	///////////////////////////
	//Get distance to goal
	cVector3f vDist; 
	vDist.x = cMath::GetAngleDistanceRad(pCam->GetPitch(),vGoalAngle.x);
	vDist.y = cMath::GetAngleDistanceRad(pCam->GetYaw(),vGoalAngle.y);

	///////////////////////////
	//Get the Speed
	cVector3f vWantedSpeed;
	vWantedSpeed.x = cMath::Min(vDist.x * mfSpeedMul, mfMaxSpeed);
	vWantedSpeed.y = cMath::Min(vDist.y * mfSpeedMul, mfMaxSpeed);

	cVector3f vSpeedDiff = vWantedSpeed - mvCurrentSpeed;
	mvCurrentSpeed += vSpeedDiff*afTimeStep*10;

	//Add Pitch
	pCam->AddPitch(mvCurrentSpeed.x * afTimeStep);

	//Add yaw
	pCam->AddYaw(mvCurrentSpeed.y * afTimeStep);
	mpPlayer->GetCharacterBody()->SetYaw(pCam->GetYaw()); 

	float fTotalDist = vDist.x*vDist.x + vDist.y*vDist.y;
	if(fTotalDist < 0.01)
	{
		gpBase->mpMapHandler->GetCurrentMap()->RunScript(msAtTargetCallback+"()");
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerLookAt::Reset()
{
	mbActive = false;
	mfMaxSpeed = 9999.0f;
	mfSpeedMul = 1.0f;
	mvTargetPos = cVector3f(0,1,0);
	mvCurrentSpeed =0;

	msAtTargetCallback = "";

	mfDestFovMul = 1.0f;
	mfFov = 1.0f;
	mfFovSpeed = 1.0f;
	mfFovMaxSpeed = 1.0f;
}

//-----------------------------------------------------------------------

void cLuxPlayerLookAt::SetTarget(const cVector3f &avTargetPos, float afSpeedMul, float afMaxSpeed, const tString& asAtTargetCallback)
{
	mvTargetPos = avTargetPos;
	mfSpeedMul = afSpeedMul;
	mfMaxSpeed = afMaxSpeed;

	msAtTargetCallback = asAtTargetCallback;
}

//-----------------------------------------------------------------------

void cLuxPlayerLookAt::SetActive(bool abX)
{
	mbActive = abX;
	if(mbActive==false)
	{
		mvCurrentSpeed =0;
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER SANITY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerSanity::cLuxPlayerSanity(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerSanity")
{
	mfHitZoomInSpeed = gpBase->mpGameCfg->GetFloat("Player_Sanity","HitZoomInSpeed",0);
	mfHitZoomOutSpeed = gpBase->mpGameCfg->GetFloat("Player_Sanity","HitZoomOutSpeed",0);
	mfHitZoomInFOVMul = gpBase->mpGameCfg->GetFloat("Player_Sanity","HitZoomInFOVMul",0);
	mfHitZoomInAspectMul = gpBase->mpGameCfg->GetFloat("Player_Sanity","HitZoomInAspectMul",0);

	mfSanityRegainSpeed = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityRegainSpeed",0);
	mfSanityRegainLimit = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityRegainLimit",0);

	mfSanityVeryLowLimit = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityVeryLowLimit",0);
	mfSanityEffectsStart = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityEffectsStart",0);

	mfSanityWaveAlphaMul = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityWaveAlphaMul",0);
	mfSanityWaveSpeedMul = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityWaveSpeedMul",0);

	mfSanityLowLimit = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityLowLimit",0);
	mfSanityLowLimitMaxTime = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityLowLimitMaxTime",0);
	mfSanityLowNewSanityAmount = gpBase->mpGameCfg->GetFloat("Player_Sanity","SanityLowNewSanityAmount",0);

	mfCheckNearEnemyInterval = gpBase->mpGameCfg->GetFloat("Player_Sanity","CheckNearEnemyInterval",0);
	mfNearEnemyDecrease = gpBase->mpGameCfg->GetFloat("Player_Sanity","NearEnemyDecrease",0);
	mfNearCritterDecrease = gpBase->mpGameCfg->GetFloat("Player_Sanity","NearCritterDecrease",0);

	Reset();
}

//-----------------------------------------------------------------------

cLuxPlayerSanity::~cLuxPlayerSanity()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::Reset()
{
	mfHitAlpha =0;
	mbHitActive = false;
	mfSanityLostCount =0;
	mfPantCount =1;
	mfCheckEnemySeenCount =0;

	mfT=0;
	mfInsaneWaveAlpha =0;

	mfSanityDrainCount =0;
	mfSanityDrainVolume =0;
	mfSanityHeartbeatCount =0;

	mfSeenEnemyCount =0;
	mbEnemyIsSeen = false;

	mbSanityEffectUpdated = false;

	mfAtLowSanityCount =0;

	mfShowHintTimer =0;
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::StartHit()
{
	mbHitActive = true;

	gpBase->mpHelpFuncs->PlayGuiSoundData("sanity_damage", eSoundEntryType_Gui);
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::SetSanityLost()
{
	mfSanityLostCount = 1.0f;
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::Update(float afTimeStep)
{
	mfT += afTimeStep;

	mbHitIsUpdated = false;
	mbSanityLostIsUpdated = false;

	//////////////////////
	// Check if player is at low sanity level and update a timer
	if(mpPlayer->GetSanity() < mfSanityLowLimit)
	{
		mfAtLowSanityCount+=afTimeStep;
		if(mfAtLowSanityCount > mfSanityLowLimitMaxTime)
		{
			mfAtLowSanityCount =0;
			mpPlayer->SetSanity(mfSanityLowNewSanityAmount);
		}
	}
	else if(mfAtLowSanityCount >0)
	{
		mfAtLowSanityCount -= afTimeStep;
		if(mfAtLowSanityCount <0) mfAtLowSanityCount =0;
	}

	//////////////////////
	// Update complex stuff
	UpdateCheckEnemySeen(afTimeStep);
	UpdateEnemySeenEffect(afTimeStep);
	UpdateHit(afTimeStep);
	UpdateInsaneEffects(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::OnDraw(float afFrameTime)
{
	
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateInsanityVisuals(float afTimeStep)
{
	if(mpPlayer->GetSanity() > mfSanityEffectsStart && mfSanityDrainVolume <=0 && mfInsaneWaveAlpha <=0)
	{
		gpBase->mpPostEffectHandler->GetInsanity()->SetActive(false);
		return;
	}

	gpBase->mpPostEffectHandler->GetInsanity()->SetActive(true);

	float fSanity = mpPlayer->GetSanity();
	float fGoalAlpha = 1 - fSanity / mfSanityEffectsStart;
	if(fGoalAlpha < 0) fGoalAlpha =0;

	////////////////////////////////
	//Update wave alpha
	if(fGoalAlpha < mfInsaneWaveAlpha)
	{
		mfInsaneWaveAlpha -= afTimeStep;
		if(mfInsaneWaveAlpha < fGoalAlpha) mfInsaneWaveAlpha = fGoalAlpha;
	}
	else
	{
		mfInsaneWaveAlpha += afTimeStep;
		if(mfInsaneWaveAlpha > fGoalAlpha) mfInsaneWaveAlpha = fGoalAlpha;
	}

	////////////////////////////////
	//Set up effects
	//Log("Zoom: %f Wave: %f\n", mfSanityDrainVolume, mfInsaneWaveAlpha);
	
	float fZoomMul = (sin(mfT*2)+1)*0.5f*0.4f + 0.6f; 

	gpBase->mpPostEffectHandler->GetInsanity()->SetWaveAlpha(mfInsaneWaveAlpha * mfSanityWaveAlphaMul);//mfInsaneWaveAlpha);
	gpBase->mpPostEffectHandler->GetInsanity()->SetWaveSpeed(mfInsaneWaveAlpha * mfSanityWaveSpeedMul);//*mfInsaneWaveAlpha);
	gpBase->mpPostEffectHandler->GetInsanity()->SetZoomAlpha(mfSanityDrainVolume * fZoomMul);
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateInsaneEffects(float afTimeStep)
{
	if(mbHitIsUpdated) return;

	if(mbSanityEffectUpdated)
	{
		gpBase->mpEffectHandler->GetImageTrail()->FadeTo(0, 1);
		mpPlayer->FadeAspectMulTo(1, 1);
		mpPlayer->FadeFOVMulTo(1, 1);

		mbSanityEffectUpdated = false;
	}

	////////////////////////////////
	// Insanity visual effect
	UpdateInsanityVisuals(afTimeStep);

	if(mpPlayer->IsDead()) return;
	////////////////////////////////
	// Player is loosing sanity!
	UpdateLosingSanity(afTimeStep);

	////////////////////////////////
	// Show that sanity is low
	UpdateLowSanity(afTimeStep);

	////////////////////////////////
	// Regain some sanity
	float fSanity = mpPlayer->GetSanity();
	if(fSanity < mfSanityRegainLimit)
	{
		fSanity += afTimeStep * mfSanityRegainSpeed;
		mpPlayer->SetSanity(fSanity);
	}

}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateCheckEnemySeen(float afTimeStep)
{
	/////////////////////////////////////
	// Check if it is time for a check!
	if(mfCheckEnemySeenCount >0)
	{
		mfCheckEnemySeenCount-=afTimeStep;
		return;
	}
	mfCheckEnemySeenCount = mfCheckNearEnemyInterval;

	/////////////////////////////////////
	// Init vars
	bool bSeenEnemy = false;
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	float fMaxRangeSqrt = 35 * 35;

	cCamera *pCam = mpPlayer->GetCamera();
	cVector3f vPlayerHeadPos = pCam->GetPosition();


	cVector3f vRight = pCam->GetRight();
	cVector3f vUp = pCam->GetUp();
	
	/////////////////////////////////////
	// Iterate critters
	float fMinCritterDistSqrt = 3.0f * 3.0f;
	bool bNearCritter = false;
	cLuxEntityIterator entIt = pMap->GetEntityIterator();
	while(entIt.HasNext())
	{
		iLuxEntity *pEntity = entIt.Next();
		if(pEntity->IsActive()==false)continue;
		if(pEntity->GetEntityType() != eLuxEntityType_Prop) continue;

		iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
		if(pProp->GetPropType() != eLuxPropType_Critter) continue;

		iLuxProp_CritterBase *pCritter = static_cast<iLuxProp_CritterBase*>(pProp);
		
		if(pCritter->CausesSanityDecrease()==false) continue;
		
		float fDistSqrt = cMath::Vector3DistSqr(pProp->GetBody(0)->GetLocalPosition(), vPlayerHeadPos); 
		if(fDistSqrt > fMinCritterDistSqrt) continue;

		bNearCritter = true;
		break;
	}

	if(bNearCritter)
	{
		mpPlayer->LowerSanity(mfNearCritterDecrease, true);
	}
	
	/////////////////////////////////////
	// Iterate enemies
	cLuxEnemyIterator it = pMap->GetEnemyIterator();
    while(it.HasNext())
	{
		iLuxEnemy *pEnemy = it.Next();
		pEnemy->SetIsSeenByPlayer(false);
		
		if(pEnemy->IsActive()==false) continue;
		if(pEnemy->CausesSanityDecrease()==false) continue;

		iCharacterBody *pCharBody = pEnemy->GetCharacterBody();

		//////////////////////////////
		//Check so enemy is in range
		float fDistSqrt = cMath::Vector3DistSqr(pCharBody->GetPosition(), vPlayerHeadPos);
		if(fDistSqrt > fMaxRangeSqrt) continue;
		
		//////////////////////////////
		//Check so enemy is in FOV
		if( pCam->GetFrustum()->CollideBoundingVolume(pCharBody->GetCurrentBody()->GetBoundingVolume()) == eCollision_Outside)
		{
			continue;
		}
		
		//////////////////////////////
		//Cast rays
		cVector3f vHalfSize = pCharBody->GetSize()*0.5f;
		cVector3f vPosAdd[5] = {
			cVector3f(0),
			vRight*vHalfSize.x,
			vRight*vHalfSize.x*-1,
			vUp*vHalfSize.y*0.8f,
			vUp*vHalfSize.y*-0.8f,
		};

		int lCount =0;
		for(int i=0; i<5; ++i)
		{
			if(gpBase->mpMapHelper->CheckLineOfSight(vPlayerHeadPos, pCharBody->GetPosition()+vPosAdd[i], false))
			{
				lCount++;
				if(lCount >=2)
				{
					bSeenEnemy = true;
					pEnemy->SetIsSeenByPlayer(true);
					break;
				}
			}
		}
		
		//if(bSeenEnemy) break; No break, since we check visibility for all enemies.
	}

	/////////////////////////////////////
	// If seen, lower sanity and increase seen count
	if(bSeenEnemy)
	{
      	mpPlayer->LowerSanity(mfNearEnemyDecrease, true);
		
		//do this in update instead!
		//gpBase->mpEffectHandler->GetRadialBlur()->SetBlurStartDist(0.3f);
		//gpBase->mpEffectHandler->GetRadialBlur()->FadeTo(0.12f, 0.12f / 3.0f);

		mbEnemyIsSeen = true;
	}
	else
	{
		if(mbEnemyIsSeen)
		{
			mbEnemyIsSeen = false;
			gpBase->mpEffectHandler->GetRadialBlur()->FadeTo(0, 0.12f / 2.0f);
		}
	}
	
}

//-----------------------------------------------------------------------


void cLuxPlayerSanity::UpdateHit(float afTimeStep)
{
	if(mpPlayer->IsDead()) return;
	if(mfHitAlpha<=0 && mbHitActive==false) return;

	mbHitIsUpdated = true;
	
	if(mbHitActive)
	{
		mfHitAlpha += afTimeStep * mfHitZoomInSpeed;
		if(mfHitAlpha >= 1)
		{
			mfHitAlpha =1;
			mbHitActive = false;
		}
	}
	else
	{
		mfHitAlpha -= afTimeStep * mfHitZoomOutSpeed;
		if(mfHitAlpha < 0) mfHitAlpha =0;
	}

	gpBase->mpEffectHandler->GetImageTrail()->FadeTo(mfHitAlpha * 0.9f, 100);
	mpPlayer->FadeAspectMulTo(1 - mfHitAlpha * mfHitZoomInAspectMul, 100);
	mpPlayer->FadeFOVMulTo(1 - mfHitAlpha * mfHitZoomInFOVMul, 100);

	mbSanityEffectUpdated = true;
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateEnemySeenEffect(float afTimeStep)
{
	if(mbEnemyIsSeen)
	{
		if(mfSeenEnemyCount <1)
		{
			mfSeenEnemyCount += afTimeStep * 0.3f;
			if(mfSeenEnemyCount>1)
			{
				mfSeenEnemyCount =1;
				gpBase->mpHintHandler->Add("EnemySeen", kTranslate("Hints", "EnemySeen"), 0);
			}
		}
		
		float fPulse = 0.5f + (sin(mfT*2.5f)*0.5f + 0.5f)*0.5f;
		
		gpBase->mpEffectHandler->GetRadialBlur()->SetBlurStartDist(0.2f);
		gpBase->mpEffectHandler->GetRadialBlur()->FadeTo(0.12f * mfSeenEnemyCount*fPulse, 10.0f);
	}
	else
	{
		if(mfSeenEnemyCount > 0)
		{
			mfSeenEnemyCount -= afTimeStep * 0.15f;
			if(mfSeenEnemyCount<0)mfSeenEnemyCount =0;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateLosingSanity(float afTimeStep)
{
	if(mfSanityLostCount <= 0)
	{
		mfSanityDrainCount = 0;
		mfSanityHeartbeatCount =0;
		mfSanityDrainVolume -= afTimeStep*0.5f;
		if(mfSanityDrainVolume < 0) mfSanityDrainVolume =0;
		
		return;
	}
	
	float fSanity = mpPlayer->GetSanity();
	float fNormalizedSanity = fSanity / 100.0f;

	mbSanityLostIsUpdated = true;
	mfSanityLostCount -= afTimeStep;

	mfSanityDrainVolume += afTimeStep * 0.1f;
	if(mfSanityDrainVolume > 1) mfSanityDrainVolume =1;

	float mfSpeedMul = 1 + (1 - fNormalizedSanity) * 2.0f;
	
	mfSanityHeartbeatCount += afTimeStep * mfSpeedMul * 0.1f;
	if(mfSanityHeartbeatCount >= 1)
	{
		mfSanityHeartbeatCount =0;
		
		float fVol = (1.0f - fNormalizedSanity*0.5f) * mfSanityDrainVolume;

		if(mpPlayer->IsDead()==false)
			gpBase->mpHelpFuncs->PlayGuiSoundData("sanity_heartbeat", eSoundEntryType_Gui, fVol);
	}
	
	mfSanityDrainCount += afTimeStep * mfSpeedMul * 0.33f;
	if(mfSanityDrainCount >= 1)
	{
		mfSanityDrainCount =0;
		tString sSoundFile="";
		if(fSanity > 75)
			sSoundFile = "sanity_drain_low";
		else if(fSanity > 50)
			sSoundFile = "sanity_drain_med"; 
		else
			sSoundFile = "sanity_drain_high";
		
		if(mpPlayer->IsDead()==false)
			gpBase->mpHelpFuncs->PlayGuiSoundData(sSoundFile, eSoundEntryType_Gui, mfSanityDrainVolume);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerSanity::UpdateLowSanity(float afTimeStep)
{
	if(mpPlayer->GetSanity() > mfSanityVeryLowLimit) return;
	
	if(mfShowHintTimer<=0)
	{
		mfShowHintTimer = 3.0f;
        gpBase->mpHintHandler->Add("SanityLow", kTranslate("Hints", "SanityLow"), 0);
	}
	else
	{
		mfShowHintTimer -= afTimeStep;
	}


	if(mbSanityLostIsUpdated==false)
	{
		gpBase->mpEffectHandler->GetImageTrail()->FadeTo(1.6f, 3);
		mbSanityEffectUpdated = true;
	}

	if(mfPantCount < 0)
	{
		mfPantCount = cMath::RandRectf(0.5f, 5.0f);

		//Play pant sound
		if(mpPlayer->IsDead()==false)
			gpBase->mpHelpFuncs->PlayGuiSoundData("sanity_pant", eSoundEntryType_Gui);
	}
	else
	{
		mfPantCount -= afTimeStep;
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER LANTERN
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerLantern::cLuxPlayerLantern(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerLantern")
{
	mDefaultColor = gpBase->mpGameCfg->GetColor("Player_Lantern","Color",cColor(0));
	mfRadius = gpBase->mpGameCfg->GetFloat("Player_Lantern","Radius",0);
	msGobo = gpBase->mpGameCfg->GetString("Player_Lantern","Gobo","");
	mvLocalOffset = gpBase->mpGameCfg->GetVector3f("Player_Lantern","LocalOffset",0);
	mbCastShadows = gpBase->mpGameCfg->GetBool("Player_Lantern","CastShadows",false);
	mfLowerOilSpeed = gpBase->mpGameCfg->GetFloat("Player_Lantern","LowerOilSpeed",0);
	mfFadeLightOilAmount = gpBase->mpGameCfg->GetFloat("Player_Lantern","FadeLightOilAmount",0);

	msOutOfOilSound = gpBase->mpGameCfg->GetString("Player_Lantern","OutOfOilSound","");
	msDisabledSound = gpBase->mpGameCfg->GetString("Player_Lantern","DisabledSound","");
	msTurnOnSound = gpBase->mpGameCfg->GetString("Player_Lantern","TurnOnSound","");
	msTurnOffSound = gpBase->mpGameCfg->GetString("Player_Lantern","TurnOffSound","");

	Reset();
}

cLuxPlayerLantern::~cLuxPlayerLantern()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerLantern::OnStart()
{

}
void cLuxPlayerLantern::Reset()
{
	mbDisabled = false;
	mbActive = false;
	mpLight = NULL;
	mfAlpha =0;
}

//-----------------------------------------------------------------------


void cLuxPlayerLantern::Update(float afTimeStep)
{
	if(mbActive ==false && mfAlpha <=0)
	{
		return;
	}
	
	////////////////////////////
	// Fade in light
	if(mbActive)
	{
		mfAlpha += afTimeStep;
		if(mfAlpha > 1.0f) mfAlpha =1;
	}
	else if(mfAlpha > 0)
	{
		mfAlpha -= afTimeStep*2.0f;
		if(mfAlpha < 0) mfAlpha =0;
	}

	cColor lightColor = mDefaultColor;
	float fOil = gpBase->mpPlayer->GetLampOil();
	if(fOil < mfFadeLightOilAmount)
	{
		lightColor =  mDefaultColor * (fOil / mfFadeLightOilAmount);
	}
	mpLight->SetDiffuseColor(lightColor * mfAlpha);


	////////////////////////////
	// Lower oil
	if(mbActive && gpBase->mpEffectHandler->GetEmotionFlash()->IsActive()==false)
	{
		float fOil = mpPlayer->GetLampOil();
		fOil -= mfLowerOilSpeed *afTimeStep;
		if(fOil <=0)
		{
			fOil = 0;
			gpBase->mpHelpFuncs->PlayGuiSoundData(msOutOfOilSound, eSoundEntryType_Gui);
			SetActive(false, true);
		}
		mpPlayer->SetLampOil(fOil);
	}

	////////////////////////////
	// Update light matrix
	if(mpLight)
	{
		cCamera *pCam = mpPlayer->GetCamera();
		cVector3f vCamRotation( pCam->GetPitch(), pCam->GetYaw(), pCam->GetRoll());
		cMatrixf mtxCamTransform = cMath::MatrixRotate(vCamRotation, eEulerRotationOrder_XYZ);
		mtxCamTransform.SetTranslation(pCam->GetPosition());

		cMatrixf mtxLocalLight = cMath::MatrixTranslate(mvLocalOffset);

		mpLight->SetMatrix(cMath::MatrixMul(mtxCamTransform, mtxLocalLight));
	}
}

//-----------------------------------------------------------------------
void cLuxPlayerLantern::OnMapEnter(cLuxMap *apMap)
{
	
}

void cLuxPlayerLantern::OnMapLeave(cLuxMap *apMap)
{

}

//-----------------------------------------------------------------------

void cLuxPlayerLantern::CreateWorldEntities(cLuxMap *apMap)
{
	cWorld *pWorld = apMap->GetWorld();

	cCamera *pCam = mpPlayer->GetCamera();

	mpLight = pWorld->CreateLightPoint("PlayerLantern",msGobo,false);
	mpLight->SetDiffuseColor(cColor(0,0));
	mpLight->SetRadius(mfRadius);
	
	mpLight->SetIsSaved(false);

	if(mbActive)
	{
		mbActive=false;
		SetActive(true, false);
	}
}

void cLuxPlayerLantern::DestroyWorldEntities(cLuxMap *apMap)
{
	if(mpLight) apMap->GetWorld()->DestroyLight(mpLight);
	mpLight = NULL;
}

//-----------------------------------------------------------------------

void cLuxPlayerLantern::SetActive(bool abX, bool abUseEffects, bool abCheckForOilAndItems, bool abCheckIfAllowed)
{
	if(mbActive == abX) return;

	/////////////////
	// Check so allowed
	if(abCheckIfAllowed && mpPlayer->GetCurrentStateData()->AllowLantern()==false)
	{
		return;
	}

	/////////////////
	// Check so player has the lantern item
    if(abCheckForOilAndItems && gpBase->mpInventory->HasItemOfType(eLuxItemType_Lantern)==false)
	{
		gpBase->mpHintHandler->Add("LanternNoItem", kTranslate("Hints", "LanternNoItem"), 0);
		return;
	}

	/////////////////
	// Check if disabled
	if(abX && mbDisabled)
	{
		return;
	}
	
	/////////////////
	// Check if there is enough oil
    if(abCheckForOilAndItems && abX && mpPlayer->GetLampOil() <=0)
	{
		if(abUseEffects)
		{
			gpBase->mpHintHandler->Add("LanternNoOil", kTranslate("Hints", "LanternNoOil"), 0);
			gpBase->mpHelpFuncs->PlayGuiSoundData(msOutOfOilSound, eSoundEntryType_Gui);
		}
		return;
	}


	/////////////////
	// Turn on / off
	mbActive = abX;
	if(mbActive)
	{
		if(abUseEffects) gpBase->mpHelpFuncs->PlayGuiSoundData(msTurnOnSound, eSoundEntryType_Gui);
	}
	else
	{
		if(abUseEffects) gpBase->mpHelpFuncs->PlayGuiSoundData(msTurnOffSound, eSoundEntryType_Gui);
	}

	/////////////////
	// Hand
	if(mbActive)
		mpPlayer->GetHands()->SetActiveHandObject("lantern");
	else
		mpPlayer->GetHands()->SetActiveHandObject("");


	/////////////////
	// Callback
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap->GetLanternLitCallback()!="")
	{
		pMap->RunScript(pMap->GetLanternLitCallback()+"(" + (mbActive ? "true" : "false") + ")" );
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerLantern::SetDisabled(bool abX)
{
	mbDisabled = abX;

	if(mbDisabled)
		SetActive(false, true);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER DEATH
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerDeath::cLuxPlayerDeath(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "PlayerDeath")
{
	cGui *pGui = gpBase->mpEngine->GetGui();

	mpWhiteModGfx = pGui->CreateGfxFilledRect(cColor(1,1),eGuiMaterial_Modulative);
	
	mfHeightAddGoal = gpBase->mpGameCfg->GetFloat("Player_General","Death_HeightAdd",0);
	mfHeightAddGoalCrouch = gpBase->mpGameCfg->GetFloat("Player_General","Death_HeightAddCrouch",0);
	mfFadeOutTime = gpBase->mpGameCfg->GetFloat("Player_General","Death_FadeTime",1000);
	
	if(mpPlayer->UsePermaDeath()) mfFadeOutTime /= 2.0f;

	mfMaxSanityGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MaxSanityGain",0);
	mfMaxHealthGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MaxHealthGain",0);
	mfMaxOilGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MaxOilGain",0);

	mfMinSanityGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MinSanityGain",0);
	mfMinHealthGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MinHealthGain",0);
	mfMinOilGain = gpBase->mpGameCfg->GetFloat("Player_General","Death_MinOilGain",0);

	mfHeightAddSpeed = gpBase->mpGameCfg->GetFloat("Player_General","Death_HeightAddSpeed",0);
	mfRollSpeed = gpBase->mpGameCfg->GetFloat("Player_General","Death_RollSpeed",0);

	if(mpPlayer->UsePermaDeath()) mfHeightAddSpeed *= 1.2f;
	if(mpPlayer->UsePermaDeath()) mfRollSpeed *= 1.2f;

	msStartSound = gpBase->mpGameCfg->GetString("Player_General","Death_StartSound", "");
	msAwakenSound = gpBase->mpGameCfg->GetString("Player_General","Death_AwakenSound", "");

	mpFont = NULL;

	mFlashOscill.SetUp(0,1,0,0.5,0.5);

	mbToMainMenu = false;
}

cLuxPlayerDeath::~cLuxPlayerDeath()
{
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::LoadFonts()
{
	mpFont = LoadFont("game_default.fnt");
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::LoadUserConfig()
{
	mbShowHint = gpBase->mpUserConfig->GetBool("Game", "ShowDeathHints", true);
}

void cLuxPlayerDeath::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Game", "ShowDeathHints", mbShowHint);
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::Reset()
{
	mfHeightAdd =0;
	mfRoll =0;
	mfTextAlpha1 =0;
	mfTextAlpha2 =0;
	mfWhiteCount =0;

	mpVoiceEntry = NULL;

	mbSkipStartSound = false;

	msCurrentHintText = _W("");

	msHintCat = "";
	msHintEntry = "";

	mfFadeAlpha =0;
	mfTextOnScreenCount =0;

	mlState =0;

	mfT =0;

	mbActive = false;
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::Start()
{
	if(mbActive) return;

	mbActive = true;

	//////////////////////////////////
	//Progress log
	gpBase->mpProgressLogHandler->AddLog(eLuxProgressLogLevel_High, "Player died!");

	//////////////////////////////////
	//Set some player stuff
	gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

	mpPlayer->ChangeState(eLuxPlayerState_Normal);
	mpPlayer->ChangeMoveState(eLuxMoveState_Normal);
	mpPlayer->GetHelperLantern()->SetActive(false,false, false);
	mpPlayer->SetCurrentHandObjectDrawn(false);
	mpPlayer->GetInsanityCollapse()->Stop();

	//////////////////////////////////
	//Text
	msCurrentHintText = msHintCat != "" ? kTranslate(msHintCat, msHintEntry) : kTranslate("Hints", "DefaultDeath");

	//////////////////////////////////
	// HARDMODE
	if (gpBase->mbHardMode && (msHintEntry != "DeathGrunt_22_Chancel"))
	{
		msCurrentHintText = kTranslate("Hints", "HardModeDeath");
	}

	//////////////////////////////////
	//Sound
	for(int i=0; i<=gpBase->mpMusicHandler->GetMaxPrio(); ++i)
	{
		gpBase->mpMusicHandler->Stop(0.2f,i);
	}

	if(mbSkipStartSound == false)
		gpBase->mpHelpFuncs->PlayGuiSoundData(msStartSound, eSoundEntryType_World);
	mbSkipStartSound = false;
	
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	pSoundHandler->FadeGlobalVolume(0, 0.15f,eSoundEntryType_World,eLuxGlobalVolumeType_Death,false);
	pSoundHandler->FadeGlobalSpeed(0.5f, 0.125f,eSoundEntryType_World,eLuxGlobalVolumeType_Death,false);
	
	///////////////////////////
	// Broadcast to all enemies
	gpBase->mpMapHandler->GetCurrentMap()->BroadcastEnemyMessage(eLuxEnemyMessage_PlayerDead, false,0,0);

	///////////////////////////
	// Reset variables
	mfHeightAdd =0;
	mfRoll =0;

	mfMinHeightAdd = mfHeightAddGoal;

	if(mpPlayer->GetCurrentMoveState() == eLuxMoveState_Normal)
	{
		cLuxMoveState_Normal *pNormalMove = static_cast<cLuxMoveState_Normal*>(mpPlayer->GetCurrentMoveStateData());
		if(pNormalMove->IsCrouching()) mfMinHeightAdd = mfHeightAddGoalCrouch;
	}

	mlState =0;

	mfFadeAlpha =0;
	mfWhiteCount =0;
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::Update(float afTimeStep)
{
	if(mbActive==false) return;

	mfT += afTimeStep;
	mFlashOscill.Update(afTimeStep);

	//////////////////////
	// Height add
	if(mlState ==0 || mlState==1)
	{
		if(mfHeightAdd > mfMinHeightAdd)
		{
			mfHeightAdd-= mfHeightAddSpeed*afTimeStep;
			if(mfHeightAdd < mfMinHeightAdd)
			{
				mfHeightAdd = mfMinHeightAdd;
			}
			mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_Death, cVector3f(0,mfHeightAdd,0));
			mlState = 1;
		}

		//////////////////////
		// Roll
		mfRoll += cMath::ToRad(mfRollSpeed)*afTimeStep;
		if(mfRoll > cMath::ToRad(65.0f)) mfRoll = cMath::ToRad(65.0f);

		mpPlayer->FadeRollTo(mfRoll, 10,10);
	}
	//////////////////////
	// Fade Out
	if(mlState==1)
	{
		mfFadeAlpha += afTimeStep * (1.0f/ mfFadeOutTime);
		if(mfFadeAlpha > 1)
		{
			mfFadeAlpha = 1;

			if(mpPlayer->UsePermaDeath())
			{
				mlState = 4;

				cLuxSoundExtraData extraData;
				if(gpBase->mpHelpFuncs->PlayGuiSoundData(mpPlayer->GetCurrentPermaDeathSound(), eSoundEntryType_Gui, 1, eSoundEntityType_Main, true, &extraData))
				{
					mpVoiceEntry = extraData.mpSoundEntry;
					mlVoiceEntryId = mpVoiceEntry->GetId();
				}
			}
		}

		if(mfFadeAlpha > 0.75f)
		{
			if(!mpPlayer->UsePermaDeath())
			{
				mfTextAlpha1 += (1 - mfTextAlpha1) * 0.5f * afTimeStep;
				if(mfTextAlpha1 > 1) mfTextAlpha1 = 1;
			}
		}
        
		if(mfTextAlpha1>1) mfTextAlpha1 = 1;

		if(mfTextAlpha1 > 0.9f && mfFadeAlpha==1)
		{
			mfTextOnScreenCount += afTimeStep;
			if(mfTextOnScreenCount > 5.5f || mbShowHint==false)
			{
				mlState = 2;

				ResetGame();
				gpBase->mpEffectHandler->GetFlash()->Start(0.7f, 1.2f, 2.5f);
				gpBase->mpHelpFuncs->PlayGuiSoundData(msAwakenSound, eSoundEntryType_Gui);

				/////////////////////////////
				//// HARDMODE
				//if (gpBase->mbHardMode && gpBase->mpPlayer->IsActive())
				//{
				//	mlState = 5;
				//}
			}
		}
	}
	//////////////////////
	// Fade to white
	if(mlState == 2)
	{
		mfWhiteCount += afTimeStep;
		if(mfWhiteCount >= 0.5f)
		{
			mlState = 3;			
		}
	}
	//////////////////////
	// Fade In
	if(mlState == 3)
	{
		mfTextAlpha1 -= afTimeStep*0.85f;
		mfFadeAlpha -= afTimeStep*0.75f;
		if(mfFadeAlpha < 0)
		{
			mfFadeAlpha = 0;
						
			mbActive = false;

			//Save the current cat and entry.
			tString sTempCat = msHintCat;
			tString sTempEntry = msHintEntry;

			Reset();

			msHintCat = sTempCat;
			msHintEntry = sTempEntry;

			//////////////////////
			// HARDMODE
			if (gpBase->mbHardMode && gpBase->mpPlayer->IsActive()) // Check if active since player is set inactive in ambush
			{
				//////////////////////
				// Load latest save

				if (gpBase->mpSaveHandler->AutoLoad(true) == false)
				{
					mbToMainMenu = true;
				}
			}
		}
	}
	//////////////////////
	// Play voice
	if(mlState == 4)
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
		if(mpVoiceEntry==NULL || pSoundHandler->IsValid(mpVoiceEntry, mlVoiceEntryId)==false)
		{
			gpBase->mpEngine->Exit();
		}
	}
	//////////////////////
	// HARDMODE
	if (mlState == 5)
	{
		//////////////////////
		// Load latest save
		if (gpBase->mpSaveHandler->AutoLoad(true) == false)
		{
			mbToMainMenu = true;
		}
	}
}


void cLuxPlayerDeath::PostUpdate(float afTimeStep)
{
	///////////////////////////
	// HARDMODE
	if (gpBase->mbHardMode == false) return;

	///////////////////////////
	// Wants to go to main menu, 
	// cant be in update since the playerclass will go bananas
	if (mbToMainMenu == false) return;
	
	//////////////////////////
	// Set MainMenu container
	gpBase->mpEngine->GetUpdater()->SetContainer("MainMenu");
	gpBase->mpLoadScreenHandler->DrawMenuScreen();

	//Reset game
	gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
	gpBase->SetCustomStory(NULL);

	//Start up menu again
	gpBase->mpMainMenu->OnLeaveContainer("");
	gpBase->mpMainMenu->OnEnterContainer("");

	mbToMainMenu = false;
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::OnDraw(float afFrameTime)
{
	if(mbActive==false) return;

	gpBase->mpGameHudSet->DrawGfx(mpWhiteModGfx,gpBase->mvHudVirtualStartPos+ cVector3f(0,0,3), gpBase->mvHudVirtualSize,cColor(1-mfFadeAlpha,1));

	if(mbShowHint==false) return; //Skip death hint

	cVector2f vFontSize = 32;
	float fSizeMul = 1.5f;
	float fSizeMulExtra =  (1- mfTextAlpha1)*4;
	float fMul = fSizeMul + fSizeMulExtra;

	tWStringVec vRows;
	mpFont->GetWordWrapRows(550,vFontSize.y, vFontSize,msCurrentHintText,&vRows);

	float fY = 300;
	for(size_t row=0; row<vRows.size(); ++row)
	{
		tWString &sStr = vRows[row];
		float fSize = mpFont->GetLength(vFontSize* fMul, sStr.c_str());
		float fX = 400 - (fSize/2.0f);

		for(size_t i=0;i<sStr.length(); ++i)
		{
			float fTAdd = 0.3f * (float)i;
			float fYAdd = sin(mfT*0.5f + fTAdd) * 5.0f + cos(mfT*0.97f - fTAdd*2.73f) * 3.5f;
					
			tWString sChar = cString::SubW(sStr, i,1);
			cVector3f vPos = cVector3f(fX, fY + fYAdd - vFontSize.y*0.5f*fMul, 6);
			cVector2f vSize = cVector2f(vFontSize.x, vFontSize.y + fabs(fYAdd)*1.5f) * fMul;

			gpBase->mpGameHudSet->DrawFont(sChar, mpFont,vPos,vSize, cColor(1, mfTextAlpha1),eFontAlign_Left);

			float fBlurAlpha = 0;
			if(mfTextAlpha1 < 0.5f)	fBlurAlpha = mfTextAlpha1 / 0.5f;
			else					fBlurAlpha = 1 - (mfTextAlpha1-0.5f) / 0.5f;

			gpBase->mpGameHudSet->DrawFont(	sChar, mpFont,vPos + cVector3f(vSize.x*0.05,vSize.y*0.05, -1), vSize*1.1, cColor(1, fBlurAlpha * 0.3f ),eFontAlign_Right);
			gpBase->mpGameHudSet->DrawFont(	sChar, mpFont,vPos + cVector3f(vSize.x*0.15,vSize.y*0.15, -2), vSize*1.3, cColor(1, fBlurAlpha * 0.2f ),eFontAlign_Right);

			fX += mpFont->GetLength(vFontSize* fMul, sChar.c_str());
		}

		fY += vFontSize.y * 1.35f;
	}

	//gpBase->mpGameHudSet->DrawFont(kTranslate("Game", "DeathPress"),mpFont, cVector3f(400, 550, 6), 17, cColor(1,mFlashOscill.val,mFlashOscill.val, mfTextAlpha2),eFontAlign_Center);
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::OnPressButton()
{
	if(mbActive==false || mfFadeAlpha < 1) return;

	mfTextOnScreenCount = 100;

    /*if(gpBase->mpSaveHandler->AutoLoad())
	{
		//////////////////////
		// Increase player attributes if low
		float fHealth = mpPlayer->GetHealth();
		float fSanity = mpPlayer->GetSanity();
		float fOil = mpPlayer->GetLampOil();

		if(fHealth < mfMaxHealthGain)
		{
			fHealth = cMath::RandRectf(cMath::Max(fHealth, mfMinHealthGain), mfMaxHealthGain);
		}
		if(fSanity < mfMaxSanityGain)
		{
			fSanity = cMath::RandRectf(cMath::Max(fSanity, mfMinSanityGain), mfMaxSanityGain);
		}
		if(fOil < mfMaxHealthGain)
		{
			fOil = cMath::RandRectf(cMath::Max(fOil, mfMinOilGain), mfMaxOilGain);
		}

		mpPlayer->SetHealth(fHealth);
		mpPlayer->SetSanity(fSanity);
		mpPlayer->SetLampOil(fOil);
	}*/
}

//-----------------------------------------------------------------------

void cLuxPlayerDeath::SetHint(const tString& asCat, const tString& asEntry)
{
	msHintCat = asCat;
	msHintEntry = asEntry;
}

void cLuxPlayerDeath::ResetGame()
{
	/////////////////////////////////////
	// Player
	mpPlayer->SetRoll(0);
	mpPlayer->GetCamera()->SetRoll(0);
	mpPlayer->GetCamera()->SetPitch(0);
	mpPlayer->SetHeadPosAdd(eLuxHeadPosAdd_Death, cVector3f(0,0,0));
	mpPlayer->SetCurrentHandObjectDrawn(true);
	mpPlayer->GetCharacterBody()->SetForceVelocity(0);
	mpPlayer->GetCharacterBody()->SetMoveSpeed(eCharDir_Forward,0);
	mpPlayer->GetCharacterBody()->SetMoveSpeed(eCharDir_Right,0);
	
	mpPlayer->SetHealth(50.0f);
	if(mpPlayer->GetSanity()<40.0f) mpPlayer->SetSanity(40.0f);

	//////////////////////////////////
	//Sound
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	pSoundHandler->FadeGlobalVolume(1, 1,eSoundEntryType_World,eLuxGlobalVolumeType_Death,false);
	pSoundHandler->FadeGlobalSpeed(1, 0.5,eSoundEntryType_World,eLuxGlobalVolumeType_Death,false);

	//////////////////////////////////
	//Check point
	gpBase->mpMapHandler->GetCurrentMap()->LoadCheckPoint();	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER LEAN
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerLean::cLuxPlayerLean(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "PlayerLean")
{
	mfMaxMovement = 0.5f;
	mfMaxRotation = cMath::ToRad(15);

	mpHeadShape = NULL;
}

cLuxPlayerLean::~cLuxPlayerLean()
{

}

//-----------------------------------------------------------------------

void cLuxPlayerLean::Reset()
{
	mfDir = 0;
	mfDirAdd = 0;
	mfMaxTime = 0.8f;
	mfMovement = 0;
	mfRotation =0;

	mfMoveSpeed = 0;

	mbPressed = false;

	mpHeadShape = NULL;
}

//-----------------------------------------------------------------------

void cLuxPlayerLean::CreateWorldEntities(cLuxMap *apMap)
{
	iPhysicsWorld *pPhysicsWorld = apMap->GetPhysicsWorld();

	float fRadius = mpPlayer->GetCharacterBody()->GetSize().x/2 * 0.68f;
	float fHeight = 0.05f * 2;
	if(fHeight < 0)fHeight = fHeight * -1;
	cMatrixf mtxOffset = cMath::MatrixRotateZ(kPi2f);
	mpHeadShape = pPhysicsWorld->CreateCylinderShape(fRadius,fHeight,&mtxOffset);
}

void cLuxPlayerLean::DestroyWorldEntities(cLuxMap *apMap)
{
	iPhysicsWorld *pPhysicsWorld = apMap->GetPhysicsWorld();

	if(mpHeadShape) pPhysicsWorld->DestroyShape(mpHeadShape);
	mpHeadShape = NULL;
}


//-----------------------------------------------------------------------

void cLuxPlayerLean::Update(float afTimeStep)
{
	if(mpPlayer->IsDead()) return;

	////////////////////////////////
	//If pressed move in direction
	if(mbPressed)
	{
		float fDir = mfDir + mfDirAdd;

		mbPressed = false;

		float fGoalPos = mfMaxMovement * fDir;
		float fGoalRot = mfMaxRotation * -fDir;

		//////////////
		//Position
		float fPrevMovement = mfMovement;
		float fMoveSpeed = (fGoalPos - mfMovement);
		if(fabsf(fMoveSpeed) <0.1f) fMoveSpeed = 0.1f*fDir;
		mfMovement += fMoveSpeed * afTimeStep * 3;

		if(fGoalPos < 0 && mfMovement < fGoalPos) mfMovement =fGoalPos;
		if(fGoalPos > 0 && mfMovement > fGoalPos) mfMovement =fGoalPos;

		//////////////
		//Rotation
		float fPrevRotation = mfRotation;
		float fRotSpeed = fGoalRot - mfRotation;
		if(fabsf(fRotSpeed) <0.13f) fRotSpeed = 0.13f*-fDir;

		mfRotation += fRotSpeed * afTimeStep * 2;

		if(fGoalRot < 0 && mfRotation < fGoalRot) mfRotation = fGoalRot;
		if(fGoalRot > 0 && mfRotation > fGoalRot) mfRotation = fGoalRot;

		////////////////////
		//Check collision
		cCamera *pCam = mpPlayer->GetCamera();
		cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
		iPhysicsWorld *pPhysicsWorld = pMap->GetPhysicsWorld();

		float fReverseMov = fPrevMovement - mfMovement;
		float fReverseRot = fPrevRotation - mfRotation;

		iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
		float fHeightAdd = pCharBody->GetSize().y + mpPlayer->GetCameraPosAdd().y;
		cVector3f vStartPos = pCharBody->GetFeetPosition() + cVector3f(0,fHeightAdd,0);

		cVector3f vPos = vStartPos + pCam->GetRight() * mfMovement;

		int lCount = 0;
		while(pPhysicsWorld->CheckShapeWorldCollision(NULL,mpHeadShape, cMath::MatrixTranslate(vPos),NULL,false,true,NULL,false))
		{
			mfMovement += fReverseMov;
			mfRotation += fReverseRot;

			if(fReverseMov < 0 && mfMovement <0) {
				mfMovement =0;
				mfRotation =0;
				break;
			}
			if(fReverseMov > 0 && mfMovement >0){
				mfMovement =0;
				mfRotation =0;
				break;
			}

			vPos = vStartPos + pCam->GetRight() * mfMovement;
			lCount++;
			if(lCount >10){
				mfMovement =0;
				mfRotation =0;
				break;
			}
		}

		mpPlayer->FadeLeanRollTo(mfRotation, 5,3);
		mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_Lean, cVector3f(mfMovement,0,0),2,0.05f);
	}
	////////////////////////////
	// Not pressed move back
	else if (mfMovement !=0 || mfRotation != 0)
	{
		mfRotation =0;
		mfMovement =0;
		mfDir = 0;
		mfDirAdd = 0;

		mpPlayer->FadeLeanRollTo(0, 4,2);
		mpPlayer->MoveHeadPosAdd(eLuxHeadPosAdd_Lean, cVector3f(0,0,0), 1.3f, 0.1);
	}
}

//-----------------------------------------------------------------------


void cLuxPlayerLean::SetLean(float afMul)
{
	mfDir = cMath::Clamp(afMul, -1.0f, 1.0f);
	if (fabsf(afMul) > 0)
	{
		mfDirAdd = 0;
		mbPressed = true;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerLean::AddLean(float afAdd)
{
	mfDirAdd = cMath::Clamp(mfDirAdd+afAdd, -1.0f, 1.0f);
	mbPressed = fabsf(mfDirAdd) > 0;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER HUD EFFECT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerHudEffect::cLuxPlayerHudEffect(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerHudEffect")
{
	cGui *pGui = gpBase->mpEngine->GetGui();

	//////////////////////////////////
	// Create damage types
	mvDamageTypes.resize(eLuxDamageType_LastEnum);

	LoadDamageData(&mvDamageTypes[eLuxDamageType_BloodSplat], "bloodsplat");
	LoadDamageData(&mvDamageTypes[eLuxDamageType_Claws], "claws");
	LoadDamageData(&mvDamageTypes[eLuxDamageType_Slash], "slash");

	//////////////////////////////////
	// Create flash gfx
	mpFlashGfx = pGui->CreateGfxFilledRect(cColor(1,1),eGuiMaterial_Alpha);
}

cLuxPlayerHudEffect::~cLuxPlayerHudEffect()
{

}
//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::AddDamageSplash(eLuxDamageType aType)
{
	
	//////////////////////////
	//Pick image to use
	cLuxPlayerDamageData *pDamageData = &mvDamageTypes[aType];
	if(pDamageData->mvImages.empty()) return;
	int lImageNum = cMath::RandRectl(0, (int)pDamageData->mvImages.size()-1);
	
	cGuiGfxElement *pGfxElem = pDamageData->mvImages[lImageNum];
	cVector2f vImageSize = pGfxElem->GetActiveSize();
	cVector2f vPos = cMath::RandRectVector2f(0, gpBase->mvHudVirtualCenterSize-vImageSize);

	//////////////////////////
	//Set properties
	for(int i=0; i<2; ++i)
	{
		cLuxPlayerHudEffect_Splash splash;

		splash.mpImage = pGfxElem;

		splash.mvPos = cVector3f(vPos.x, vPos.y, 1);
		
		splash.mvSize = vImageSize;
		splash.mfAlpha = 1.0f;

		if(i==0)
		{
			splash.mfAlphaMul = 0.3f;
			splash.mvPosVel = cVector3f(0,1.0f, 0);
			splash.mvSizeVel = cVector2f(0, 8.0f);
		}
		else
		{
			splash.mfAlphaMul = 0.7f;
			splash.mvPosVel = cVector3f(0,6.0f, 0);
			splash.mvSizeVel = cVector2f(0, 16.0f);
		}
		
		splash.mfAlphaVel = 0.4f;
		splash.mfAlphaMoveStart = 0.8f;

		mlstSplashes.push_back(splash);
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::Flash(const cColor& aColor, eGuiMaterial aFlashMaterial, float afInTime, float afOutTime)
{
	mFlashColor = aColor;
	mFlashMaterial = aFlashMaterial;
	mfFlashAlpha = 0.0f;
	mfFlashAlphaSpeed = afInTime==0 ? 100000.0f : 1.0f / afInTime;
	mfFlashAlphaOutSpeed = afOutTime==0 ? -100000.0f : -1.0f / afOutTime;
	mbFlashActive = true;
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::OnDraw(float afFrameTime)
{
	DrawSplashes(afFrameTime);
	DrawFlash(afFrameTime);
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::Update(float afTimeStep)
{
	UpdateSplashes(afTimeStep);
	UpdateFlash(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::Reset()
{
	mlstSplashes.clear();
	mfFlashAlpha =0;
	mbFlashActive = false;
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::DrawSplashes(float afFrameTime)
{
	cLuxPlayerHudEffect_SplashListIt it = mlstSplashes.begin();
	for(; it != mlstSplashes.end(); ++it)
	{
		cLuxPlayerHudEffect_Splash *pSplash = &(*it);

        gpBase->mpGameHudSet->DrawGfx(pSplash->mpImage, pSplash->mvPos,pSplash->mvSize, cColor(1, pSplash->mfAlpha * pSplash->mfAlphaMul));
	}
}

void cLuxPlayerHudEffect::UpdateSplashes(float afTimeStep)
{
	cLuxPlayerHudEffect_SplashListIt it = mlstSplashes.begin();
	for(; it != mlstSplashes.end();)
	{
		cLuxPlayerHudEffect_Splash *pSplash = &(*it);

		if(pSplash->mfAlpha < pSplash->mfAlphaMoveStart)
		{
			pSplash->mvPos += pSplash->mvPosVel * afTimeStep;
			pSplash->mvSize += pSplash->mvSizeVel * afTimeStep;
		}
		
		pSplash->mfAlpha -= pSplash->mfAlphaVel * afTimeStep;
		if(pSplash->mfAlpha < 0)
		{
			it = mlstSplashes.erase(it);
			continue;
		}

		++it;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::DrawFlash(float afFrameTime)
{
	if(mfFlashAlpha <=0 || mbFlashActive==false) return;

	cColor col = mFlashColor;
	
	col.a *= mfFlashAlpha;
	
	gpBase->mpGameHudSet->DrawGfx(mpFlashGfx,gpBase->mvHudVirtualStartPos +cVector3f(0,0,0), gpBase->mvHudVirtualSize, col, mFlashMaterial);
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::UpdateFlash(float afTimeStep)
{
	if(mbFlashActive==false) return;
	
	mfFlashAlpha += mfFlashAlphaSpeed * afTimeStep;

	if(mfFlashAlpha > 1.0f && mfFlashAlphaSpeed > 0)
	{
		mfFlashAlpha = 1.0f;
		mfFlashAlphaSpeed = mfFlashAlphaOutSpeed;
	}
	if(mfFlashAlpha < 0 && mfFlashAlphaSpeed < 0)
	{
		mfFlashAlpha = 0;
		mbFlashActive = false;
	}
}

//-----------------------------------------------------------------------

void cLuxPlayerHudEffect::LoadDamageData(cLuxPlayerDamageData *apData, const tString& asName)
{
	cGui *pGui = gpBase->mpEngine->GetGui();
	cFileSearcher *pFileSearcher = gpBase->mpEngine->GetResources()->GetFileSearcher();

	tString sFileNameBase = "graphics/hud/damage_"+asName;

	int lCount = 0;
	tString sFile = sFileNameBase + cString::ToString(lCount)+".tga";

	while(pFileSearcher->GetFilePath(sFile) != _W(""))
	{
		cGuiGfxElement *pGfx = pGui->CreateGfxImage(sFile, eGuiMaterial_Alpha);
		if(pGfx) apData->mvImages.push_back(pGfx);

		lCount++;
		sFile = sFileNameBase + cString::ToString(lCount)+".tga";
	}
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PLAYER LIGHT LEVEL
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerLightLevel::cLuxPlayerLightLevel(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerLightLevel")
{
	mfRadiusAdd = gpBase->mpGameCfg->GetFloat("Player_Darkness","RadiusAdd",0);
}

cLuxPlayerLightLevel::~cLuxPlayerLightLevel()
{
}

//-----------------------------------------------------------------------

void cLuxPlayerLightLevel::OnStart()
{
	
}

void cLuxPlayerLightLevel::Reset()
{
	mfExtendedLightLevel = 1.0f;
	mfNormalLightLevel = 1.0f;
	mfUpdateCount =0;
}

//-----------------------------------------------------------------------

void cLuxPlayerLightLevel::Update(float afTimeStep)
{
	///////////////////////////////////////
	//If count reaches 0, update light level
	if(mfUpdateCount <=0.0f)
	{
		mfUpdateCount = 1.0f / 2.0f;

		////////////////////////////////
		//Get character body properties
		iCharacterBody *pCharBody = mpPlayer->GetCharacterBody();
		cVector3f vPos = pCharBody->GetPosition();
		cVector3f vSize = pCharBody->GetSize();
		cVector3f vForward = pCharBody->GetForward();

		////////////////////////////////
		//Set up positions to test light level at.
		const int lTestPos =5;
		cVector3f vTestPos[lTestPos] =
		{
			vPos,	 //Center
			vPos + cVector3f(0,vSize.y-0.1f, 0),	//Above feet
			vPos - cVector3f(0,vSize.y-0.1f, 0),	//Head
			vPos + vForward * vSize.z*0.8f,			//In front of center
			vPos - cVector3f(0,vSize.y-0.1f, 0) + vForward * vSize.z*0.8f //In front of feet.
		};

		////////////////////////////////
		//Get lights to skip
		std::vector<iLight*> vSkipLights;
		vSkipLights.push_back(mpPlayer->GetHelperInDarkness()->GetAmbientLight());
		
		////////////////////////////////
		//Get light level at all positions and then calculate median.
		//float fTotalLight =0;
		
		mfExtendedLightLevel = 0.0f;
		mfNormalLightLevel = 0.0f;
		if(mpPlayer->GetHelperLantern()->IsActive())
		{
			mfExtendedLightLevel += 1.0f;
			mfNormalLightLevel += 1.0f;
		}
		
		for(int i=0; i<lTestPos; ++i)
		{
       		//fTotalLight += gpBase->mpMapHelper->GetLightLevelAtPos(vTestPos[i], &vSkipLights);
			float fExtLight = gpBase->mpMapHelper->GetLightLevelAtPos(vTestPos[i], &vSkipLights, mfRadiusAdd);
			float fNormalLight = gpBase->mpMapHelper->GetLightLevelAtPos(vTestPos[i], &vSkipLights, 0);
			
			mfExtendedLightLevel = cMath::Max(fExtLight, mfExtendedLightLevel);
			mfNormalLightLevel = cMath::Max(fNormalLight, mfNormalLightLevel);
		}

		//mfLightLevel = fTotalLight / (float)lTestPos;
	}
	else
	{
		mfUpdateCount -= afTimeStep;
	}
}


//-----------------------------------------------------------------------



void cLuxPlayerLightLevel::OnMapEnter(cLuxMap *apMap)
{
	mfUpdateCount =0;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PLAYER IN DARKNESS
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------

cLuxPlayerInDarkness::cLuxPlayerInDarkness(cLuxPlayer *apPlayer) : iLuxPlayerHelper(apPlayer, "LuxPlayerInDarkness")
{
	mpSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	mfMinDarknessLightLevel = gpBase->mpGameCfg->GetFloat("Player_Darkness","MinLightLevel",0);
	
	mfAmbientLightMinLightLevel = gpBase->mpGameCfg->GetFloat("Player_Darkness","AmbientLightMinLightLevel",0);
	mfAmbientLightRadius = gpBase->mpGameCfg->GetFloat("Player_Darkness","AmbientLightRadius",0);
	mfAmbientLightIntensity = gpBase->mpGameCfg->GetFloat("Player_Darkness","AmbientLightIntensity",0);
	mfAmbientLightFadeInTime = gpBase->mpGameCfg->GetFloat("Player_Darkness","AmbientLightFadeInTime",0);
	mfAmbientLightFadeOutTime = gpBase->mpGameCfg->GetFloat("Player_Darkness","AmbientLightFadeOutTime",0);
	mAmbientLightColor = gpBase->mpGameCfg->GetColor("Player_Darkness","AmbientLightColor",cColor(0));

	msLoopSoundFile  = gpBase->mpGameCfg->GetString("Player_Darkness", "LoopSoundFile","");
	mfLoopSoundVolume = gpBase->mpGameCfg->GetFloat("Player_Darkness", "LoopSoundVolume",0);
	mfLoopSoundStartupTime = gpBase->mpGameCfg->GetFloat("Player_Darkness", "LoopSoundStartupTime",0);
	mfLoopSoundFadeInSpeed = gpBase->mpGameCfg->GetFloat("Player_Darkness", "LoopSoundFadeInSpeed",0);
	mfLoopSoundFadeOutSpeed = gpBase->mpGameCfg->GetFloat("Player_Darkness", "LoopSoundFadeOutSpeed",0);

	mfSanityLossPerSecond = gpBase->mpGameCfg->GetFloat("Player_Darkness", "SanityLossPerSecond",0);
}

cLuxPlayerInDarkness::~cLuxPlayerInDarkness()
{
	
}

//-----------------------------------------------------------------------

void cLuxPlayerInDarkness::OnStart()
{

}
void cLuxPlayerInDarkness::Reset()
{
	mbActive = true;

	mpAmbientLight =NULL;

	mbAmbientLightIsOn = false;
	mbInDarkness = false;

	mpLoopSound = NULL;
	mfLoopSoundCount =0;

	mfSanityLossMul =0;

	mfShowHintTimer = 0;
}

//-----------------------------------------------------------------------


void cLuxPlayerInDarkness::Update(float afTimeStep)
{
	if (!mbActive) return;

	///////////////////////
	// Get light level
    float fExtLightLevel = mpPlayer->GetHelperLightLevel()->GetExtendedLightLevel();
	float fNormalLightLevel = mpPlayer->GetHelperLightLevel()->GetNormalLightLevel();

	///////////////////////
	// Update ambient light position.
	cVector3f vCamPos = mpPlayer->GetCamera()->GetPosition();
	mpAmbientLight->SetPosition(vCamPos - cVector3f(0,0.3f,0));


	////////////////////////////
	//Turn off ambient light
	if(fNormalLightLevel > mfAmbientLightMinLightLevel)
	{
		if(mbAmbientLightIsOn)
		{
			mbAmbientLightIsOn = false;
			mpAmbientLight->FadeTo(cColor(0.0f, 0.0f),mpAmbientLight->GetRadius(),mfAmbientLightFadeOutTime);
		}
	}
	////////////////////////////
	//Turn on ambient light
	else
	{
		if(mbAmbientLightIsOn==false)
		{
			mbAmbientLightIsOn = true;

			////////////////////////
			// HARDMODE
			if (gpBase->mbHardMode)
				mpAmbientLight->FadeTo(mAmbientLightColor*mfAmbientLightIntensity * 0.75f , mpAmbientLight->GetRadius(), mfAmbientLightFadeInTime * 2.5f);
			else
				mpAmbientLight->FadeTo(mAmbientLightColor*mfAmbientLightIntensity, mpAmbientLight->GetRadius(), mfAmbientLightFadeInTime);

		}

	}

	///////////////////////
	// Light
    if(fExtLightLevel > mfMinDarknessLightLevel)
	{
		////////////////////////////
		//Turn off loop sound
		/*if(mpLoopSound)
		{
			if(mfLoopSoundCount <= 0)
			{
				mpLoopSound->FadeOut(mfLoopSoundFadeOutSpeed);
				mpLoopSound = NULL;
			}
		}*/
        
		mfLoopSoundCount-= afTimeStep;
		if(mfLoopSoundCount <= 0) mfLoopSoundCount = 0;


		mbInDarkness = false;
		
		mfSanityLossMul -= afTimeStep*0.3f;
		if(mfSanityLossMul < 0) mfSanityLossMul = 0;
	}
	///////////////////////
	// Darkness
	else
	{
		mfSanityLossMul += afTimeStep*0.1f;
		if(mfSanityLossMul > 1) mfSanityLossMul = 1;

		////////////////////////////
		//Lower sanity
		if(	mpPlayer->GetHelperFlashback()->IsActive()==false && mpPlayer->GetSanityDrainDisabled()==false && 
			gpBase->mpEffectHandler->GetEmotionFlash()->IsActive()==false)
		{
			mpPlayer->LowerSanity(mfSanityLossPerSecond*afTimeStep*mfSanityLossMul, true);

			if(mfShowHintTimer<=0 && mfSanityLossMul > 0.05f)
			{
				mfShowHintTimer = 3.0f;
				gpBase->mpHintHandler->Add("DarknessDecrease", kTranslate("Hints", "DarknessDecrease"), 0);
			}
			else
			{
				mfShowHintTimer -= afTimeStep;
			}
		}
				
		////////////////////////////
		//Check if sound should be played
		/*if(mpLoopSound == NULL)
		{
			if(mfLoopSoundCount >= mfLoopSoundStartupTime)
			{
				mpLoopSound = mpSoundHandler->PlayGuiStream(msLoopSoundFile,true,mfLoopSoundVolume);
				if(mpLoopSound) mpLoopSound->FadeIn(1.0f, mfLoopSoundFadeInSpeed);
			}
		}*/

		mfLoopSoundCount+= afTimeStep;
		if(mfLoopSoundCount >= mfLoopSoundStartupTime) mfLoopSoundCount = mfLoopSoundStartupTime;
		

		mbInDarkness = true;
	}

}

//-----------------------------------------------------------------------
void cLuxPlayerInDarkness::OnMapEnter(cLuxMap *apMap)
{
	mbAmbientLightIsOn = false;
	mbInDarkness = false;
}

void cLuxPlayerInDarkness::OnMapLeave(cLuxMap *apMap)
{

}

//-----------------------------------------------------------------------

void cLuxPlayerInDarkness::CreateWorldEntities(cLuxMap *apMap)
{
	cWorld *pWorld = apMap->GetWorld();

	mpAmbientLight = pWorld->CreateLightPoint("PlayerDarknessAmbient","",false);
	mpAmbientLight->SetDiffuseColor(cColor(0.0f, 0.0f));

	mpAmbientLight->SetRadius(mfAmbientLightRadius);

	/////////////////////
	// HARDMODE
	if(gpBase->mbHardMode)
		mpAmbientLight->SetRadius(mfAmbientLightRadius*0.5f);


	mpAmbientLight->SetCastShadows(false);
	mpAmbientLight->SetIsSaved(false);
}

void cLuxPlayerInDarkness::DestroyWorldEntities(cLuxMap *apMap)
{
	if(mpAmbientLight) apMap->GetWorld()->DestroyLight(mpAmbientLight);
	mpAmbientLight = NULL;
}

//-----------------------------------------------------------------------

bool cLuxPlayerInDarkness::InDarkness()
{
	float fLightLevel = mpPlayer->GetHelperLightLevel()->GetExtendedLightLevel();
	return fLightLevel <= mfMinDarknessLightLevel;
}

//-----------------------------------------------------------------------

void cLuxPlayerInDarkness::SetActive(bool abX)
{
	mbActive = abX;

	if (abX == false)
	{
		if(mbAmbientLightIsOn)
		{
			mbAmbientLightIsOn = false;
			mpAmbientLight->FadeTo(cColor(0.0f, 0.0f),mpAmbientLight->GetRadius(),mfAmbientLightFadeOutTime);
		}
        
		mfLoopSoundCount = 0;
		mbInDarkness = false;
		mfSanityLossMul = 0;
	}
}

//-----------------------------------------------------------------------
