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

#include "LuxMapHandler.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxEffectRenderer.h"
#include "LuxEffectHandler.h"
#include "LuxDebugHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxSavedGame.h"
#include "LuxSaveHandler.h"
#include "LuxConfigHandler.h"
#include "LuxLoadScreenHandler.h"
#include "LuxMainMenu.h"

#include "LuxEnemy.h"
#include "LuxAchievementHandler.h"

//////////////////////////////////////////////////////////////////////////
// SOUND ENTITY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cMapHandlerSoundCallback::cMapHandlerSoundCallback()
{
	///////////////////////
	//Load document
	tString sFile = "sounds/EnemySounds.dat";
	iXmlDocument* pXmlDoc = gpBase->mpEngine->GetResources()->LoadXmlDocument(sFile);
	if(pXmlDoc ==NULL)
	{
		Error("Couldn't load XML file '%s'!\n",sFile.c_str());
		return;
	}

	//////////////////////
	// Load data
	cXmlNodeListIterator it = pXmlDoc->GetChildIterator();
	while(it.HasNext())
	{
		cXmlElement *pChildElem = it.Next()->ToElement();

		tString sName = pChildElem->GetAttributeString("name");
		mvEnemyHearableSounds.push_back(sName);
	}

	gpBase->mpEngine->GetResources()->DestroyXmlDocument( pXmlDoc );
}

//-----------------------------------------------------------------------

void cMapHandlerSoundCallback::OnStart(cSoundEntity *apSoundEntity)
{
	cLuxMap *pMap = gpBase->mpMapHandler->GetCurrentMap();
	if(pMap==NULL) return;
	
	///////////////////////////
	//Check if the sound is something to worry bout
	tString sTypeName = apSoundEntity->GetData()->GetName();

	bool bUsed=false;
	for(size_t i=0; i< mvEnemyHearableSounds.size(); ++i)
	{
		tString &sName = mvEnemyHearableSounds[i];
		if(sTypeName.size() >= sName.size() && sName == sTypeName.substr(0,sName.size()))
		{
			bUsed = true;
			break;
		}
	}
	if(bUsed == false) return;
	
	///////////////////////////
	//Iterate enemies and send sound message to those close enough
	float fMaxDist = apSoundEntity->GetMaxDistance();
	float fMinDist = apSoundEntity->GetMaxDistance();
	float fVolume = apSoundEntity->GetVolume();
	cVector3f vPos = apSoundEntity->GetWorldPosition();
	
	pMap->BroadcastEnemySoundMessage(vPos, fVolume, fMinDist, fMaxDist);
}	

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// RENDER CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxDebugRenderCallback::cLuxDebugRenderCallback()
{
}

//-----------------------------------------------------------------------

void cLuxDebugRenderCallback::OnPostSolidDraw(cRendererCallbackFunctions* apFunctions)
{
	if(mpPhysicsWorld)
	{
		apFunctions->SetMatrix(NULL);
		apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
		apFunctions->SetTextureRange(NULL,0);
		apFunctions->SetProgram(NULL);
		
		apFunctions->SetDepthTest(true);
		apFunctions->SetDepthWrite(false);

		//mpPhysicsWorld->RenderDebugGeometry(apFunctions->GetLowLevelGfx(), cColor(1,1,1,1));
	}

	gpBase->mpDebugHandler->RenderSolid(apFunctions);
	gpBase->mpMapHandler->RenderSolid(apFunctions);
	gpBase->mpPlayer->RenderSolid(apFunctions);
	gpBase->mpEffectRenderer->RenderSolid(apFunctions);
}

//-----------------------------------------------------------------------

void cLuxDebugRenderCallback::OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions)
{
	gpBase->mpPlayer->RenderTrans(apFunctions);
	gpBase->mpEffectRenderer->RenderTrans(apFunctions);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMapHandler::cLuxMapHandler() : iLuxUpdateable("LuxMapHandler")
{
	//////////////////////////
	//Create and setup view port
	mpViewport = gpBase->mpEngine->GetScene()->CreateViewport();
	gpBase->mpEngine->GetScene()->SetCurrentListener(mpViewport);

	//////////////////////////
	//Set up post effects
	cGraphics *pGraphics = gpBase->mpEngine->GetGraphics();
	cPostEffectComposite *pPostEffectComp = pGraphics->CreatePostEffectComposite();
	mpViewport->SetPostEffectComposite(pPostEffectComp);
	
	//Bloom
	cPostEffectParams_Bloom bloomParams;
	bloomParams.mfBlurSize = 1.0f;
	bloomParams.mvRgbToIntensity = bloomParams.mvRgbToIntensity * 1.0f;
	mpPostEffect_Bloom = pGraphics->CreatePostEffect(&bloomParams);
	pPostEffectComp->AddPostEffect(mpPostEffect_Bloom, 100);
	
	//Image trail
	cPostEffectParams_ImageTrail imageTrailParams;
	mpPostEffect_ImageTrail = pGraphics->CreatePostEffect(&imageTrailParams);
	pPostEffectComp->AddPostEffect(mpPostEffect_ImageTrail, 10);
	mpPostEffect_ImageTrail->SetActive(false);

	//Radial
	cPostEffectParams_RadialBlur radialBlurParams;
	radialBlurParams.mfSize = 0.0f;
	mpPostEffect_RadialBlur = pGraphics->CreatePostEffect(&radialBlurParams);
	pPostEffectComp->AddPostEffect(mpPostEffect_RadialBlur, 9);
	mpPostEffect_RadialBlur->SetActive(false);

	//Sepia
	cPostEffectParams_ColorConvTex sepiaParams;
	sepiaParams.msTextureFile = "colorconv_sepia.tga";
	sepiaParams.mfFadeAlpha = 0.0f;
	mpPostEffect_Sepia = pGraphics->CreatePostEffect(&sepiaParams);
	pPostEffectComp->AddPostEffect(mpPostEffect_Sepia, 4);
	mpPostEffect_Sepia->SetActive(false);
	
	//////////////////////////
	//Saving
	mpSavedGame = hplNew( cLuxSavedGameMapCollection, () );

	
	//////////////////////////
	//Callbacks
	mpSoundCallback = hplNew( cMapHandlerSoundCallback, () );
	cSoundEntity::AddGlobalCallback(mpSoundCallback);

	//////////////////////////
	//Threading
	mpSavedGameMutex = cPlatform::CreateMutEx();

	//////////////////////////
	//Variables
	mbPausedSoundsAndMusic = false;
	mpDataCache =NULL;

	Reset();
}

//-----------------------------------------------------------------------

cLuxMapHandler::~cLuxMapHandler()
{
	hplDelete(mpSavedGame);
	hplDelete(mpSavedGameMutex);
	hplDelete(mpSoundCallback);
	STLDeleteAll(mlstMaps);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMapHandler::OnStart()
{
	//////////////////////
	//Set up viewport 	
	mpViewport->SetCamera(gpBase->mpPlayer->GetCamera());

	mpViewport->AddGuiSet(gpBase->mpGameDebugSet);
	mpViewport->AddGuiSet(gpBase->mpGameHudSet);

    
	mpViewport->AddRendererCallback(&mRenderCallback);
	UpdateViewportRenderProperties();
}

//-----------------------------------------------------------------------

void cLuxMapHandler::UpdateViewportRenderProperties()
{
	cRenderSettings *pRenderSettings = mpViewport->GetRenderSettings();
	pRenderSettings->mbRenderWorldReflection = gpBase->mpConfigHandler->mbWorldReflection;
	pRenderSettings->mbRenderShadows = gpBase->mpConfigHandler->mbShadowsActive;
}

//-----------------------------------------------------------------------

void cLuxMapHandler::Update(float afTimeStep)
{
	//TODO: Bad placement! Moooove!
	gpBase->mpEffectRenderer->ClearRenderLists();

	CheckMapChange(afTimeStep);

	if(mpCurrentMap && mMapChangeData.mbActive==false)
		mpCurrentMap->Update(afTimeStep);
}

//-----------------------------------------------------------------------

void cLuxMapHandler::Reset()
{
	// Stop all sounds (deleting maps will stop world entries, but will let GUI ones live)
	cSound *pSound = gpBase->mpEngine->GetSound();
	pSound->GetSoundHandler()->StopAll(eSoundEntryType_All);

	STLDeleteAll(mlstMaps);
	mpCurrentMap = NULL;

	ResumeSoundsAndMusic(); //Make sure that all sounds and music are resumed!

	msMapFolder = "";

	mbUpdateActive = true;

	mMapChangeData.mbActive = false;

	mpSavedGame->Reset();

	gpBase->mpHelpFuncs->CleanupData();

	DestroyDataCache();
}

//-----------------------------------------------------------------------

void cLuxMapHandler::OnQuit()
{
    gpBase->mpEngine->GetUpdater()->SetContainer("MainMenu");

    gpBase->mpLoadScreenHandler->DrawMenuScreen();
    
    //Destroy map
    cLuxMapHandler *mpMapHandler = gpBase->mpMapHandler;
    if(mpMapHandler->GetCurrentMap())
    {
        //Save
        gpBase->mpSaveHandler->AutoSave();
        
        mpMapHandler->DestroyMap(mpMapHandler->GetCurrentMap(),false);

        //Reset game
        gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
        gpBase->SetCustomStory(NULL);
    }

    //Start up menu again
    gpBase->mpMainMenu->OnLeaveContainer("");
    gpBase->mpMainMenu->OnEnterContainer("");
}


//-----------------------------------------------------------------------

void cLuxMapHandler::LoadUserConfig()
{
	mbShowCommentary = gpBase->mpUserConfig->GetBool("Game","ShowCommentary", false);
}

void cLuxMapHandler::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Game","ShowCommentary", mbShowCommentary);
}

//-----------------------------------------------------------------------

void cLuxMapHandler::CreateDataCache()
{
	if(mpDataCache) DestroyDataCache();
	
	mpDataCache = hplNew(cLuxModelCache, () );
	mpDataCache->Create();
}

void cLuxMapHandler::DestroyDataCache()
{
	if(mpDataCache==NULL) return;
	hplDelete(mpDataCache);
	mpDataCache = NULL;
}

//-----------------------------------------------------------------------

void cLuxMapHandler::SetUpdateActive(bool abX)
{
	mbUpdateActive = abX;
	
	if(mpCurrentMap) mpCurrentMap->GetWorld()->SetActive(mbUpdateActive);
}

//-----------------------------------------------------------------------

void cLuxMapHandler::RenderSolid(cRendererCallbackFunctions* apFunctions)
{
	//mpViewport->GetRenderSettings()->mbLog = false;
	if(mpCurrentMap) mpCurrentMap->OnRenderSolid(apFunctions);
}

//-----------------------------------------------------------------------

void cLuxMapHandler::OnEnterContainer(const tString& asOldContainer)
{
	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);

	if(mpCurrentMap) mpCurrentMap->GetWorld()->SetActive(true);

	ResumeSoundsAndMusic();
}

void cLuxMapHandler::OnLeaveContainer(const tString& asNewContainer)
{
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	if(mpCurrentMap) mpCurrentMap->GetWorld()->SetActive(false);
}


//-----------------------------------------------------------------------

void cLuxMapHandler::ChangeMap(const tString& asMapName, const tString& asStartPos, const tString& asStartSound, const tString& asEndSound)
{
	mMapChangeData.mbActive = true;
	mMapChangeData.msMapFile = cString::SetFileExt(asMapName, "map");
	mMapChangeData.msStartPos = asStartPos;
    mMapChangeData.msSound = asEndSound;

    gpBase->mpHelpFuncs->PlayGuiSoundData(asStartSound, eSoundEntryType_Gui);

	gpBase->mpEffectHandler->GetFade()->FadeOut(1.5f);

	gpBase->mpPlayer->SetActive(false);
}

//-----------------------------------------------------------------------

cLuxMap* cLuxMapHandler::LoadMap(const tString& asFileName, bool abLoadEntities)
{
	cLuxMap *pMap = hplNew( cLuxMap, ( FileToMapName(asFileName)) );
	
	pMap->LoadFromFile(msMapFolder+asFileName, abLoadEntities);

	mlstMaps.push_back(pMap);

	return pMap;
}
//-----------------------------------------------------------------------

void cLuxMapHandler::DestroyMap(cLuxMap* apMap, bool abLoadingSaveGame)
{
	////////////////////////////////
	//If the map do me destroyed is current, make sure it is not current
	if(mpCurrentMap == apMap) SetCurrentMap(NULL, abLoadingSaveGame, false,"");

    STLFindAndDelete(mlstMaps,apMap);
}

//-----------------------------------------------------------------------

void cLuxMapHandler::SetCurrentMap(cLuxMap* apMap, bool abRunScript, bool abFirstTime, const tString& asPlayerPos)
{
	if(mpCurrentMap == apMap) return;

	//////////////////////////////////
	//Unload stuff from previous map
    if(mpCurrentMap)
	{
		mpCurrentMap->OnLeave(abRunScript);
		
		//Leave callback for modules
		gpBase->RunModuleMessage(eLuxUpdateableMessage_DestroyWorldEntities, mpCurrentMap);
		gpBase->RunModuleMessage(eLuxUpdateableMessage_OnMapLeave, mpCurrentMap);
	}

	mpCurrentMap = apMap;

	//////////////////////////////////
	//Setup stuff for previous map
	if(mpCurrentMap)
	{
		//Enter callback for modules
		gpBase->RunModuleMessage(eLuxUpdateableMessage_CreateWorldEntities, mpCurrentMap);
		gpBase->RunModuleMessage(eLuxUpdateableMessage_OnMapEnter, mpCurrentMap);

		//Set the player position
		mpCurrentMap->PlacePlayerAtStartPos(asPlayerPos);

		//Create an automatic checkpoint
		mpCurrentMap->SetCheckPoint("_auto", asPlayerPos, "");
		
		//Map enter callback
		mpCurrentMap->OnEnter(abRunScript, abFirstTime);

		//Set this as world in viewport
		mpViewport->SetWorld(mpCurrentMap->GetWorld());

		mRenderCallback.mpPhysicsWorld = mpCurrentMap->GetPhysicsWorld();
		mRenderCallback.mpLowLevelGfx = gpBase->mpEngine->GetGraphics()->GetLowLevel();
	}
	else
	{
		//If no map, set NULL as world
		mpViewport->SetWorld(NULL);
	}
}

//-----------------------------------------------------------------------

void cLuxMapHandler::PauseSoundsAndMusic()
{
	if(mbPausedSoundsAndMusic) return;

	cSound *pSound = gpBase->mpEngine->GetSound();
	pSound->GetSoundHandler()->PauseAll(eSoundEntryType_All);
	pSound->GetMusicHandler()->Pause();
	mbPausedSoundsAndMusic = true;
}

void cLuxMapHandler::ResumeSoundsAndMusic()
{
	if(mbPausedSoundsAndMusic)
	{
		cSound *pSound = gpBase->mpEngine->GetSound();
		pSound->GetSoundHandler()->ResumeAll(eSoundEntryType_All);
		pSound->GetMusicHandler()->Resume();
		mbPausedSoundsAndMusic = false;
	}
}

//-----------------------------------------------------------------------

void cLuxMapHandler::ClearSaveMapCollection()
{
	mpSavedGameMutex->Lock();
		mpSavedGame->Reset();
	mpSavedGameMutex->Unlock();
}

//-----------------------------------------------------------------------

void cLuxMapHandler::SetSavedMapCollection(cLuxSavedGameMapCollection *apMaps)
{
	mpSavedGameMutex->Lock();
	{
		hplDelete(mpSavedGame);

		mpSavedGame = apMaps;
	}
	mpSavedGameMutex->Unlock();
}

//-----------------------------------------------------------------------

void cLuxMapHandler::AppLostInputFocus()
{
	PauseSoundsAndMusic();
}

//-----------------------------------------------------------------------

void cLuxMapHandler::AppGotInputFocus()
{
	ResumeSoundsAndMusic();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMapHandler::LoadMainConfig()
{
	mpPostEffect_Bloom->SetDisabled(gpBase->mpMainConfig->GetBool("Graphics", "PostEffectBloom", true)==false);
	mpPostEffect_ImageTrail->SetDisabled(gpBase->mpMainConfig->GetBool("Graphics", "PostEffectImageTrail", true)==false);
	mpPostEffect_Sepia->SetDisabled(gpBase->mpMainConfig->GetBool("Graphics", "PostEffectSepia", true)==false);
	mpPostEffect_RadialBlur->SetDisabled(gpBase->mpMainConfig->GetBool("Graphics", "PostEffectRadialBlur", true)==false);

	cRenderSettings *pRenderSettings = mpViewport->GetRenderSettings();
	pRenderSettings->mbUseEdgeSmooth = gpBase->mpConfigHandler->mbEdgeSmooth; //This is saved in config handler!
}

void cLuxMapHandler::SaveMainConfig()
{
	gpBase->mpMainConfig->SetBool("Graphics", "PostEffectBloom", mpPostEffect_Bloom->IsDisabled()==false);
	gpBase->mpMainConfig->SetBool("Graphics", "PostEffectImageTrail", mpPostEffect_ImageTrail->IsDisabled()==false);
	gpBase->mpMainConfig->SetBool("Graphics", "PostEffectSepia", mpPostEffect_Sepia->IsDisabled()==false);
	gpBase->mpMainConfig->SetBool("Graphics", "PostEffectRadialBlur", mpPostEffect_RadialBlur->IsDisabled()==false);
}

//-----------------------------------------------------------------------

tString cLuxMapHandler::FileToMapName(const tString& asFile)
{
	return cString::ToLowerCase(cString::GetFileName(cString::SetFileExt(asFile, "")));
}

//-----------------------------------------------------------------------

void cLuxMapHandler::SetShowCommentary(bool abX)
{
	mbShowCommentary = abX;

	if(mbShowCommentary==false)
	{
		gpBase->mpEffectHandler->GetPlayCommentary()->Stop();
	}
}

//-----------------------------------------------------------------------

void cLuxMapHandler::CheckMapChange(float afTimeStep)
{
	if(mMapChangeData.mbActive==false) return;
	if(gpBase->mpEffectHandler->GetFade()->IsFading()) return;

	///////////////////////////////////////
	// Setup variables
    float fTimeTaken =0;

	///////////////////////////////////////
	// Fade out and disable player
	mMapChangeData.mbActive = false;
	gpBase->mpEffectHandler->GetFade()->FadeIn(2.0f);
	gpBase->mpPlayer->SetActive(true);

	///////////////////////////////////////
	// Write pending savegame queries
	cLuxSaveHandlerThreadClass* pThreadClass = gpBase->mpSaveHandler->GetThreadClass();
	if(pThreadClass->IsRunning())
		pThreadClass->ProcessPendingSaves();

	//////////////////////////////////
	//Clean up
	// Must do this before OnEnter!
	gpBase->mpHelpFuncs->CleanupData();

	///////////////////////
	// Load map
	tString sNewMapName = FileToMapName(mMapChangeData.msMapFile);
	if(mpCurrentMap->GetName() != sNewMapName)
	{
		mpSavedGameMutex->Lock();

		//////////////////////
		// Run onleave before saving!
		mpCurrentMap->RunScript("OnLeave()");//since script is not run in SetCurrenMap

		///////////////////////////////////////
		// Draw loading screen
		unsigned long lLoadStartTime = cPlatform::GetApplicationTime();
		gpBase->mpLoadScreenHandler->DrawGameScreen();

		//////////////////////
		// Save old map
		mpSavedGame->SaveMap(mpCurrentMap);

		//////////////////////
		// Fadeout sounds and disable stop (meaning they will fade even when sound entities are destroyed)
		gpBase->mpEngine->GetSound()->GetSoundHandler()->FadeOutAll(eSoundEntryType_World, 0.5f, true);
		
		//////////////////////
		// Load new map
		cLuxMap *pLastMap = mpCurrentMap;
		cLuxMap *pMap = LoadMap(mMapChangeData.msMapFile,true);
		if(pMap == NULL)
		{
			Error("Could not load map '%s'!\n", mMapChangeData.msMapFile.c_str());
			return;
		}
		
		if(pLastMap)
		{
			if (pLastMap->GetName() == "08_cellar_maze" && pMap->GetName() == "09_back_hall")
			{
				gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_EscapeArtist);
			}

			if (pLastMap->GetName() == "14_elevator" && pMap->GetName() == "15_prison_south")
			{
				gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_Descendant);
			}

			//////////////
			// HARDMODE
			if (gpBase->mbHardMode &&
				pLastMap->GetName() == "27_torture_chancel_redux" && pMap->GetName() == "28_inner_sanctum")
			{
				gpBase->mpPlayer->AddSanity(100.f, true);
			}
		}

		//////////////////////
		// Set new and destroy old
		bool bFirstTime = mpSavedGame->MapExists(sNewMapName)==false;	
		
		SetCurrentMap(pMap, false, bFirstTime, mMapChangeData.msStartPos);
		DestroyMap(pLastMap, false);

		//////////////////////
		// Load new map data
		mpSavedGame->LoadMap(mpCurrentMap);

		//////////////////////
		// Run enter script! (otherwise a save in oneter will not be correct!)
		if(bFirstTime) mpCurrentMap->RunScript("OnStart()");
		mpCurrentMap->RunScript("OnEnter()");


		mpSavedGameMutex->Unlock();

		//////////////////////////////////
		//Check if any more load time needed
		fTimeTaken = (float)(cPlatform::GetApplicationTime() - lLoadStartTime)/1000.0f;
		
		ProgLog(eLuxProgressLogLevel_High, "Entering map "+ mpCurrentMap->GetName());
	}
	///////////////////////
	// Map already loaded.
	else
	{
		mpCurrentMap->PlacePlayerAtStartPos(mMapChangeData.msStartPos);
	}

	//////////////////////////////////
	// Check if text should be left on a bit longer
	if(fTimeTaken>0)
	{
		gpBase->mpLoadScreenHandler->GameScreenLoadDone(mMapChangeData.msSound, fTimeTaken);
	}
	else
	{
		//Play finished sound
		gpBase->mpHelpFuncs->PlayGuiSoundData(mMapChangeData.msSound, eSoundEntryType_Gui);

	}
}

//-----------------------------------------------------------------------



