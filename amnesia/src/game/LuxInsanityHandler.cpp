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

#include "LuxInsanityHandler.h"

#include "LuxPlayer.h"
#include "LuxDebugHandler.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxProgressLogHandler.h"
#include "LuxEffectHandler.h"
#include "LuxMessageHandler.h"

//////////////////////////////////////////////////////////////////////////
// INSANITY EVENT INTERFACE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxInstanityEvent::iLuxInstanityEvent()
{
	mbUsed = false;
	mbOver = true;
	mfMaxSanity = 100;
}

//-----------------------------------------------------------------------

iLuxInstanityEvent::~iLuxInstanityEvent()
{

}

//-----------------------------------------------------------------------

void iLuxInstanityEvent::LoadData(cXmlElement * apVarElem)
{
	msName = apVarElem->GetAttributeString("Name","Unknown");
	msSet = apVarElem->GetAttributeString("Set","");
	mfMaxSanity = apVarElem->GetAttributeFloat("MaxSanity",100);

	OnLoadData(apVarElem);
}

//-----------------------------------------------------------------------

void iLuxInstanityEvent::Start()
{
	mbUsed = true;
	mbOver = false;

	gpBase->mpProgressLogHandler->AddLog(eLuxProgressLogLevel_Low, "Starting Insanity event: '"+msName+"'"+ " set: '"+msSet+"'");
	gpBase->mpDebugHandler->AddMessage(	_W("Starting Insanity event: '")+cString::To16Char(msName)+_W("'")+
										_W("set: '")+cString::To16Char(msSet)+_W("'") , false);

	OnStart();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY BUGS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInstanityEvent_Bugs::cLuxInstanityEvent_Bugs()
{
	
}

cLuxInstanityEvent_Bugs::~cLuxInstanityEvent_Bugs()
{

}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Bugs::OnLoadData(cXmlElement * apVarElem)
{
	tString sImage = apVarElem->GetAttributeString("BugImage","");
	if(sImage != "")
		mpBugImage = gpBase->mpEngine->GetGui()->CreateGfxImage(sImage, eGuiMaterial_Alpha);

	mlNumOfBugs = apVarElem->GetAttributeInt("NumOfBugs",10);

	mfMinSizeMul = apVarElem->GetAttributeFloat("MinSizeMul",1);
	mfMaxSizeMul = apVarElem->GetAttributeFloat("MaxSizeMul",1);

	mfWanderCircleDist = apVarElem->GetAttributeFloat("WanderCircleDist",1);
	mfWanderCircleRadius = apVarElem->GetAttributeFloat("WanderCircleRadius",1);
	mfSwarmPointMul = apVarElem->GetAttributeFloat("SwarmPointMul",1);
	mfMaxSpeed = apVarElem->GetAttributeFloat("MaxSpeed",1);

	msLoopSound = apVarElem->GetAttributeString("LoopSound","");
	mfSoundVolume = apVarElem->GetAttributeFloat("SoundVolume",1);
	mfDuration = apVarElem->GetAttributeFloat("Duration",5);
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Bugs::OnStart()
{
	//////////////////////////////////////////
	// Set up variables
	cVector2f vGuiSetCenterSize = cVector2f(800, 600);
	cVector2f vGuiSetSize, vGuiSetOffset;
	LuxCalcGuiSetScreenOffset(vGuiSetCenterSize, vGuiSetSize, vGuiSetOffset);
	cVector2f vGuiSetMin = cVector2f(-vGuiSetOffset.x,-vGuiSetOffset.y);
	cVector2f vGuiSetMax = cVector2f(800.0f+vGuiSetOffset.x, 600.0f+vGuiSetOffset.y);

	//////////////////////////////////////////
	// Create the bugs 
	mvBugs.resize(mlNumOfBugs);
    for(int i=0; i<mlNumOfBugs; ++i)
	{
		cLuxInstanityEvent_Bugs_Bug *pBug = &mvBugs[i];

		/////////////////////
		// Position (keep at edges of screen)
		cVector2f vPos;
		bool bVertical = cMath::RandRectf(0,1)>0.7;
        if(bVertical)
		{
			vPos.x = cMath::RandRectl(0,1)==0 ? vGuiSetMin.x : vGuiSetMax.x;
			vPos.y = cMath::RandRectf(vGuiSetMin.y, vGuiSetMax.y);
		}
		else
		{
			vPos.x = cMath::RandRectf(vGuiSetMin.x, vGuiSetMax.x);
			vPos.y = cMath::RandRectl(0,1)==0 ? vGuiSetMin.y : vGuiSetMax.y;
		}
		
		/////////////////////
		// Set up variables
		pBug->mbActive = false;
		pBug->mfAlpha =0;
		pBug->mfAngle = cMath::RandRectf(0, k2Pif);
		pBug->mvPos = vPos;
		pBug->mvVel = cVector2f(0,0);
		pBug->mfSizeMul = cMath::RandRectf(mfMinSizeMul, mfMaxSizeMul);
	}

	//////////////////////////////////////////
	// Start sound
	if(msLoopSound != "")
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

		mpSoundEntry = pSoundHandler->PlayGuiStream(msLoopSound, true, mfSoundVolume);
		if(mpSoundEntry)
		{
			mlSoundEntryID = mpSoundEntry->GetId();
			mpSoundEntry->FadeIn(1, 1.0f / 2.0f);
		}
	}

	//////////////////////////////////
	// Set up time
	mfTimeCount = mfDuration;
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Bugs::OnExit()
{
	//////////////////////////////////
	// Destroy Bugs
	mvBugs.clear();
	
	//////////////////////////////////
	// Destroy Sound
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	if(mpSoundEntry && pSoundHandler->IsValid(mpSoundEntry, mlSoundEntryID))
	{
		mpSoundEntry->FadeOut(1.0f/2.0f);
		mpSoundEntry = NULL;
	}
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Bugs::Update(float afTimeStep)
{
	cVector2f vSwarmPoint(400, 300);
	bool bActive = true;
	int lDisabledCount =0;

	/////////////////////////////////
	// Time
	mfTimeCount -= afTimeStep;
	if(mfTimeCount <0)
	{
		mfTimeCount = 0;
		bActive = false;
	}

	/////////////////////////////////
	// Bugs
	for(int i=0; i<mlNumOfBugs; ++i)
	{
		cLuxInstanityEvent_Bugs_Bug *pBug = &mvBugs[i];

		/////////////////// 
		// Alpha
		if(bActive && pBug->mfAlpha < 1)
		{
			pBug->mfAlpha += cMath::RandRectf(0.2f,1.0f)*afTimeStep;
			if(pBug->mfAlpha>1) pBug->mfAlpha=1;
		}
		else if(bActive==false)
		{
			pBug->mfAlpha -= cMath::RandRectf(0.2f,1.0f)*afTimeStep;
			if(pBug->mfAlpha<0)
			{
				pBug->mfAlpha=0;
				lDisabledCount++;
			}
		}
        	
		/////////////////// 
		// Wandering
		if(pBug->mvVel.Length() > kEpsilonf)
		{
			cVector2f vDir = pBug->mvVel; vDir.Normalize();
			float fAngle = cMath::RandRectf(0, k2Pif);

			cVector3f vForce = cMath::MatrixMul(cMath::MatrixRotateY(fAngle),cVector3f(mfWanderCircleRadius,0,0));

			pBug->mvVel += (vDir*mfWanderCircleDist + cVector2f(vForce.x, vForce.z)) * afTimeStep;
		}
		
		/////////////////// 
		// Swarm around point
		{
			float fSwarmPointDist = cMath::Vector2Dist(vSwarmPoint, pBug->mvPos);

			cVector2f vWantedVel = vSwarmPoint - pBug->mvPos;
			vWantedVel.Normalize(); vWantedVel *= mfSwarmPointMul;
			
			cVector2f vAcc = vWantedVel * cMath::Min(fSwarmPointDist*0.025f ,1.0f);
			pBug->mvVel += vAcc * afTimeStep;
		}
		
		/////////////////////////7
		// Cap speed
        float fSpeed = pBug->mvVel.Length();
        if(fSpeed > mfMaxSpeed)
		{
			pBug->mvVel = (pBug->mvVel/fSpeed)*mfMaxSpeed;
		}
		
		/////////////////// 
		// Update pos
		pBug->mvPos += pBug->mvVel * afTimeStep;
		
		/////////////////// 
		// Calculate angle
        if(pBug->mvVel.Length()>kEpsilonf)
		{
			pBug->mfAngle = cMath::GetAngleFromPoints2D(0, pBug->mvVel);
		}
	}

	////////////////////////////
	//Check if over
	if(bActive==false && lDisabledCount==(int)mvBugs.size())
	{
		EventIsDone();
	}
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Bugs::OnDraw(float afFrameTime)
{
	cGuiSet *pSet = gpBase->mpGameHudSet;

	for(int i=0; i<mlNumOfBugs; ++i)
	{
		cLuxInstanityEvent_Bugs_Bug *pBug = &mvBugs[i];
		
		pSet->DrawGfx(mpBugImage, pBug->mvPos, mpBugImage->GetActiveSize()*pBug->mfSizeMul, cColor(1,pBug->mfAlpha),
						eGuiMaterial_LastEnum, pBug->mfAngle);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY PARTICLES
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInstanityEvent_Particles::cLuxInstanityEvent_Particles()
{
	mfTimeCount =0;
	mpSoundEntry = NULL;
	mlSoundEntryID = 0;
	mpPS = NULL;
}

cLuxInstanityEvent_Particles::~cLuxInstanityEvent_Particles()
{

}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Particles::OnLoadData(cXmlElement * apVarElem)
{
	msSoundFile = apVarElem->GetAttributeString("SoundFile", "");
	mbLoopSound = apVarElem->GetAttributeBool("LoopSound", false);
	mfSoundVolume = apVarElem->GetAttributeFloat("SoundVolume", 0);
	mfSoundFadeInTime = apVarElem->GetAttributeFloat("SoundFadeInTime", 1);
	mfSoundFadeOutTime = apVarElem->GetAttributeFloat("SoundFadeOutTime", 1);

	msParticleSystem = apVarElem->GetAttributeString("ParticleSystem", "");

	mfDuration = apVarElem->GetAttributeFloat("Duration", 1);
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Particles::OnStart()
{
	//////////////////////////////////
	// Create Sound
	if(msSoundFile != "")
	{
		cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

		mpSoundEntry = pSoundHandler->PlayGuiStream(msSoundFile, mbLoopSound, mfSoundVolume);
        if(mpSoundEntry)
		{
			mlSoundEntryID = mpSoundEntry->GetId();
			if(mfSoundFadeInTime>0)
				mpSoundEntry->FadeIn(1, 1.0f / mfSoundFadeInTime);
		}
	}

	//////////////////////////////////
	// Create Particles system
    if(msParticleSystem != "")
	{
		cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();
		mpPS = pWorld->CreateParticleSystem(GetName()+"_PS",msParticleSystem,1);
		if(mpPS)
		{
			mpPS->SetPosition(gpBase->mpPlayer->GetCamera()->GetPosition());
			mpPS->SetIsSaved(false);
		}
	}

	//////////////////////////////////
	// Set up time
	mfTimeCount = mfDuration;
}

void cLuxInstanityEvent_Particles::OnExit()
{
	/////////////////////////////////
	// Destroy Particles
	cLuxMap* pMap = gpBase->mpMapHandler->GetCurrentMap();
	if (pMap)
	{
		cWorld *pWorld = pMap->GetWorld();
		if(mpPS && pWorld->ParticleSystemExists(mpPS))
		{
			mpPS->Kill();
			mpPS = NULL;
		}
	}

	//////////////////////////////////
	// Destroy Sound
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
	if(mpSoundEntry && pSoundHandler->IsValid(mpSoundEntry, mlSoundEntryID))
	{
		if(mfSoundFadeOutTime>0)
			mpSoundEntry->FadeOut(1.0f/mfSoundFadeOutTime);
		else
			mpSoundEntry->Stop();
		mpSoundEntry = NULL;
	}
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Particles::Update(float afTimeStep)
{
	/////////////////////////////////
	// Particles
	cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();
	if(mpPS && pWorld->ParticleSystemExists(mpPS))
	{
		mpPS->SetPosition(gpBase->mpPlayer->GetCamera()->GetPosition());
	}

	/////////////////////////////////
	// Time
    mfTimeCount -= afTimeStep;
	if(mfTimeCount <0)
	{
		EventIsDone();
	}
}

void cLuxInstanityEvent_Particles::OnDraw(float afFrameTime)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY SOUND STREAM
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInstanityEvent_SoundStream::cLuxInstanityEvent_SoundStream() : iLuxInstanityEvent()
{
    mpWhiteGfx = gpBase->mpEngine->GetGui()->CreateGfxFilledRect(cColor(1), eGuiMaterial_Modulative);
}

cLuxInstanityEvent_SoundStream::~cLuxInstanityEvent_SoundStream()
{

}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_SoundStream::OnLoadData(cXmlElement * apVarElem)
{
	msFile = apVarElem->GetAttributeString("File", "");
	mfVolume = apVarElem->GetAttributeFloat("Volume", 1);
	mfSoundDelayTime = apVarElem->GetAttributeFloat("SoundDelayTime", 0);
	if(mfSoundDelayTime <=0) mfSoundDelayTime = 0.001f;
	
	mbFadeScreen = apVarElem->GetAttributeBool("FadeScreen", false);
	mFadeColor = apVarElem->GetAttributeColor("FadeColor", cColor(1));
	mfFadeInSpeed = apVarElem->GetAttributeFloat("FadeInTime", 0);
	mfFadeOutSpeed = apVarElem->GetAttributeFloat("FadeOutTime", 0);
	
	if(mfFadeInSpeed<=0) mfFadeInSpeed = 0.001f;
	if(mfFadeOutSpeed<=0) mfFadeOutSpeed = 0.001f;
	mfFadeInSpeed = 1.0f / mfFadeInSpeed;
	mfFadeOutSpeed = 1.0f / mfFadeOutSpeed;

	msSubtitleCat = apVarElem->GetAttributeString("SubtitleCat", "");
	msSubtitleEntry = apVarElem->GetAttributeString("SubtitleEntry", "");
	
	msFadeImageFile = apVarElem->GetAttributeString("FadeImageFile", "");
	mbDisablePlayer = apVarElem->GetAttributeBool("DisablePlayer", false);
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_SoundStream::OnStart()
{
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	mpFadeImage = NULL;
	mpSoundEntry = NULL;
	mfSoundCount = mfSoundDelayTime;
	mfFadeAlpha =0;
	mfFadeAlphaSpeed = mfFadeInSpeed;
	if(mbDisablePlayer) gpBase->mpPlayer->SetActive(false);

	if(msSubtitleCat != "" && msSubtitleEntry !="")
	{
		gpBase->mpDefaultFont->GetWordWrapRows(600,20,18, kTranslate(msSubtitleCat,msSubtitleEntry), &mvCurrentTextRows);
	}
	
	if(msFadeImageFile != "")
	{
		mpFadeImage = gpBase->mpEngine->GetGui()->CreateGfxTexture(msFadeImageFile, eGuiMaterial_Alpha, eTextureType_Rect);
	}
}

void cLuxInstanityEvent_SoundStream::OnExit()
{
	if(mpFadeImage) 
		gpBase->mpEngine->GetGui()->DestroyGfx(mpFadeImage);
	
	if(mbDisablePlayer)
		gpBase->mpPlayer->SetActive(true);

	mvCurrentTextRows.clear();
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_SoundStream::Update(float afTimeStep)
{
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	///////////////////////////////
	// Play sound
	if(mpSoundEntry==NULL && mfSoundCount>0)
	{
		mfSoundCount-= afTimeStep;
		if(mfSoundCount <=0)
		{
			mpSoundEntry = pSoundHandler->PlayGuiStream(msFile, false, mfVolume);
			if(mpSoundEntry)
				mlSoundEntryID = mpSoundEntry->GetId();
		}
	}

	////////////////////////////////
	// Fade screen
	if(mbFadeScreen)
	{
		mfFadeAlpha += mfFadeAlphaSpeed*afTimeStep;
		if(mfFadeAlphaSpeed > 0 && mfFadeAlpha > 1.0f)
		{
			mfFadeAlpha = 1.0f;
		}
		if(mfFadeAlphaSpeed < 0 && mfFadeAlpha < 0.0f)
		{
			mfFadeAlpha = 0.0f;
			EventIsDone();
		}
	}

	////////////////////////////////
	// Check if sound is over
	if(mfSoundCount <=0 && (mpSoundEntry==NULL || pSoundHandler->IsValid(mpSoundEntry, mlSoundEntryID)==false) )
	{
		if(mbFadeScreen==false)
		{
			EventIsDone();
		}
		if(mfFadeAlphaSpeed>0)
			mfFadeAlphaSpeed = -mfFadeOutSpeed;
	}
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_SoundStream::OnDraw(float afFrameTime)
{
	if(	gpBase->mpMessageHandler->ShowEffectSubtitles() &&
		mvCurrentTextRows.empty()==false && 
		gpBase->mpEffectHandler->GetPlayVoice()->IsActive()==false)
	{
		cVector3f vStartPos(400-300/2, 580 - (mvCurrentTextRows.size()*(18.0f+2.0f)), 4);

		for(size_t i=0; i<mvCurrentTextRows.size(); ++i)
		{
			gpBase->mpGameHudSet->DrawFont(mvCurrentTextRows[i],gpBase->mpDefaultFont, vStartPos, 18,cColor(1,1));
			vStartPos.y+= 18+2;
		}
	}

	if(mbFadeScreen)
	{
		if(mpFadeImage)
			gpBase->mpGameHudSet->DrawGfx(mpFadeImage, gpBase->mvHudVirtualStartPos, gpBase->mvHudVirtualSize, mFadeColor * mfFadeAlpha);
		else
			gpBase->mpGameHudSet->DrawGfx(mpWhiteGfx, gpBase->mvHudVirtualStartPos, gpBase->mvHudVirtualSize, mFadeColor*mfFadeAlpha + cColor(1,1)*(1-mfFadeAlpha));
	}
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY STEPS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInstanityEvent_Steps::cLuxInstanityEvent_Steps() : iLuxInstanityEvent()
{
}

cLuxInstanityEvent_Steps::~cLuxInstanityEvent_Steps()
{

}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Steps::OnLoadData(cXmlElement * apVarElem)
{
	msSound = apVarElem->GetAttributeString("Sound", "");
	mfStepTime = apVarElem->GetAttributeFloat("StepTime", 1);
	mlStepNum = apVarElem->GetAttributeInt("StepNum", 0);
	mfDistance = apVarElem->GetAttributeFloat("Distance", 0); 
		
	mfTimeMulPerStep = apVarElem->GetAttributeFloat("TimeMulPerStep", 0); 
	mfDistanceMulPerStep = apVarElem->GetAttributeFloat("DistanceMulPerStep", 0); 
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Steps::OnStart()
{
	cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

	mvPosition = cMath::MatrixMul( cMath::MatrixRotateY(cMath::RandRectf(0, k2Pif)), cVector3f(mfDistance, 0, 0) );

	mfSoundCountMax = mfStepTime;
	mfSoundCount =0;
	mlCount = mlStepNum;
}

void cLuxInstanityEvent_Steps::OnExit()
{
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Steps::Update(float afTimeStep)
{
	cWorld *pWorld = gpBase->mpMapHandler->GetCurrentMap()->GetWorld();

	///////////////////////////////
	// Play sound
	if(mfSoundCount <=0)
	{
		cSoundEntity *pSound = pWorld->CreateSoundEntity("insanity_step", msSound, true);
		if(pSound)
		{
			pSound->SetPosition(gpBase->mpPlayer->GetCharacterBody()->GetPosition() + mvPosition);
		}

		mfSoundCount = mfSoundCountMax;
		mfSoundCountMax *= mfTimeMulPerStep;
		mvPosition = cMath::Vector3Normalize(mvPosition) * mvPosition.Length() * mfDistanceMulPerStep;

		mlCount--;
		if(mlCount <=0) EventIsDone();
	}
	else
	{
		mfSoundCount -= afTimeStep;
	}
}

//-----------------------------------------------------------------------

void cLuxInstanityEvent_Steps::OnDraw(float afFrameTime)
{
	
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInsanityHandler::cLuxInsanityHandler() : iLuxUpdateable("LuxInsanityHandler")
{
	mfMaxSanity_LongWait = gpBase->mpGameCfg->GetFloat("Insanity", "MaxSanity_LongWait", 0);
	mfMaxSanity_MedWait = gpBase->mpGameCfg->GetFloat("Insanity", "MaxSanity_MedWait", 0);
	mfMaxSanity_ShortWait = gpBase->mpGameCfg->GetFloat("Insanity", "MaxSanity_ShortWait", 0);

	mfTimeBetween_LongWait = gpBase->mpGameCfg->GetFloat("Insanity", "TimeBetween_LongWait", 0);
	mfTimeBetween_MedWait = gpBase->mpGameCfg->GetFloat("Insanity", "TimeBetween_MedWait", 0);
	mfTimeBetween_ShortWait = gpBase->mpGameCfg->GetFloat("Insanity", "TimeBetween_ShortWait", 0);

	tString sEventFile = gpBase->mpGameCfg->GetString("Insanity", "EventsFile", "");
	LoadEvents(sEventFile);

	mlCurrentEvent = -1; //need to set this here because of stuff in reset.
}

//-----------------------------------------------------------------------

cLuxInsanityHandler::~cLuxInsanityHandler()
{
	STLDeleteAll(mvEvents);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInsanityHandler::OnStart()
{
	
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::Reset()
{
	if(mlCurrentEvent >=0)
		mvEvents[mlCurrentEvent]->OnExit();
	mlCurrentEvent = -1;
	
	mfNewEventCount =0;
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::Update(float afTimeStep)
{
	//////////////////////////
	// Update time between events if not event is playing
	if(mfNewEventCount > 0 && mlCurrentEvent < 0)
	{
		mfNewEventCount -= afTimeStep;
	}
	
	///////////////////////////
	// Update current event
	if(mlCurrentEvent>=0)
	{
		mvEvents[mlCurrentEvent]->Update(afTimeStep);
		if(mvEvents[mlCurrentEvent]->IsOver())
		{
			mvEvents[mlCurrentEvent]->OnExit();
			mlCurrentEvent = -1;

			float fSanity = gpBase->mpPlayer->GetSanity();

			//Between between events depends on sanity level.
			if(fSanity <= mfMaxSanity_ShortWait)
				mfNewEventCount = mfTimeBetween_ShortWait;
			else if(fSanity <= mfMaxSanity_MedWait)
				mfNewEventCount = mfTimeBetween_MedWait;
			else
				mfNewEventCount = mfTimeBetween_LongWait;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::OnDraw(float afFrameTime)
{
	if(mlCurrentEvent>=0) mvEvents[mlCurrentEvent]->OnDraw(afFrameTime);
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::StartEvent()
{
	if(mvEvents.empty()) return;

	float fPlayerSanity = gpBase->mpPlayer->GetSanity();

	if(mlCurrentEvent >=0)
		mvEvents[mlCurrentEvent]->OnExit();
	mlCurrentEvent = -1;
	
	/////////////////////////////////
	// Get number of events available
	int lTotalEnabledEvents = 0;	//All whose set is not disabled
	int lUnusedEvents =0;			//All events not disabled and unused.
	for(size_t i=0; i<mvEvents.size(); ++i)
	{
		iLuxInstanityEvent *pEvent = mvEvents[i];
		bool bUsed = pEvent->IsUsed();
		bool bDisabled = SetIsDisabled(pEvent->GetSet()) || fPlayerSanity>pEvent->GetMaxSanity();

		if(bUsed == false && bDisabled==false)	lUnusedEvents++;
		if(bDisabled==false)	lTotalEnabledEvents++;
	}

	if(lTotalEnabledEvents==0) return;

	/////////////////////////////////////
	//If all events are used, reset!
	if(lUnusedEvents ==0)
	{
		for(size_t i=0; i<mvEvents.size(); ++i)
		{
			iLuxInstanityEvent *pEvent = mvEvents[i];
			pEvent->SetUsed(false);
		}

		lUnusedEvents = lTotalEnabledEvents; 
	}

	/////////////////////////////////
	// Get random event (iterate and increment current, skipping unavailable events.
	int lIdx = cMath::RandRectl(0, lUnusedEvents);
	int lCurrentEvent = 0;
	for(size_t i=0; i<mvEvents.size(); ++i)
	{
		iLuxInstanityEvent *pEvent = mvEvents[i];
		if(SetIsDisabled(pEvent->GetSet()) || pEvent->IsUsed() || fPlayerSanity>pEvent->GetMaxSanity()) continue;

		if(lCurrentEvent == lIdx)
		{
			mlCurrentEvent = i;
			break;
		}
		lCurrentEvent++;
	}
	
	///////////////////////////////
	// If no events are available, return
	if(mlCurrentEvent < 0) return;

	///////////////////////////////
	// Start the event
	mvEvents[mlCurrentEvent]->Start();

	ProgLog(eLuxProgressLogLevel_Low, "Starting insanity event "+ mvEvents[mlCurrentEvent]->GetName());
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::StartEvent(int alIdx)
{
	if(mlCurrentEvent >=0)
		mvEvents[mlCurrentEvent]->OnExit();
	
	mlCurrentEvent = alIdx;
	mvEvents[mlCurrentEvent]->Start();
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::StartEvent(const tString &asName)
{
	int lEventIndex = -1;
	
	for(size_t i=0; i<mvEvents.size(); ++i)
	{
		if (mvEvents[i]->GetName() == asName)
		{
			lEventIndex = i;
			break;
		}
	}

	if (lEventIndex < 0) return;

	if(mlCurrentEvent >=0)
		mvEvents[mlCurrentEvent]->OnExit();
	
	mlCurrentEvent = lEventIndex;
	mvEvents[mlCurrentEvent]->Start();
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::StopCurrentEvent()
{
	if(mlCurrentEvent >=0)
		mvEvents[mlCurrentEvent]->OnExit();

	mlCurrentEvent = -1;
}

//-----------------------------------------------------------------------

bool cLuxInsanityHandler::NewEventIsPossible()
{
	return mfNewEventCount <= 0 && gpBase->mpPlayer->GetSanity() <= mfMaxSanity_LongWait && mlCurrentEvent <0;
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::ReloadEvents()
{
	Reset();

	STLDeleteAll(mvEvents);
	mvEvents.clear();

	tString sEventFile = gpBase->mpGameCfg->GetString("Insanity", "EventsFile", "");
	LoadEvents(sEventFile);
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::EnableSet(const tString &asSet)
{
	m_setDisabledSets.erase(asSet);
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::DisableSet(const tString &asSet)
{
	m_setDisabledSets.insert(asSet);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxInstanityEvent* cLuxInsanityHandler::EventTypeToData(const tString& asType)
{
	tString sLowType = cString::ToLowerCase(asType);

    if(sLowType == "soundstream") return hplNew(cLuxInstanityEvent_SoundStream, () );
	if(sLowType == "steps") return hplNew(cLuxInstanityEvent_Steps, () );
	if(sLowType == "particles") return hplNew(cLuxInstanityEvent_Particles, () );
	if(sLowType == "bugs") return hplNew(cLuxInstanityEvent_Bugs, () );

	Error("Insanity event type '%s' does not exist!\n", asType.c_str());
	return NULL;
}

//-----------------------------------------------------------------------

void cLuxInsanityHandler::LoadEvents(const tString& asFile)
{
	if(asFile == "")
	{
		Warning("No sanity events file specified!");
		return;
	}

	///////////////////////
	// Load document 
	cResources *pResources = gpBase->mpEngine->GetResources();
	iXmlDocument *pXmlDoc = pResources->LoadXmlDocument(asFile);
	if(pXmlDoc==NULL)
	{
		Error("Could not load sanity events file: '%s'\n", asFile.c_str());
		return;
	}

	///////////////////////
	// Iterate children and create events
    cXmlNodeListIterator it = pXmlDoc->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement *pChildElem = it.Next()->ToElement();

		tString sType = pChildElem->GetValue();
		iLuxInstanityEvent* pEvent = EventTypeToData(sType);
		if(pEvent)
		{
			pEvent->LoadData(pChildElem);
			mvEvents.push_back(pEvent);
		}
	}

	///////////////////////
	// Destroy document
	pResources->DestroyXmlDocument(pXmlDoc);
}

//-----------------------------------------------------------------------

bool cLuxInsanityHandler::SetIsDisabled(const tString& asSet)
{
	tStringSetIt it = m_setDisabledSets.find(asSet);
	return it != m_setDisabledSets.end();
}

//-----------------------------------------------------------------------

