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

#include "engine/Engine.h"

#include "system/System.h"
#include "sound/Sound.h"
#include "physics/Physics.h"
#include "ai/AI.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "gui/Gui.h"
#include "haptic/Haptic.h"
#include "scene/Scene.h"
#include "generate/Generate.h"

#include "system/LogicTimer.h"
#include "system/String.h"
#include "system/Platform.h"
#include "system/Timer.h"
#include "system/Mutex.h"

#include "input/Input.h"
#include "input/Mouse.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer.h"

#include "engine/Updater.h"
#include "engine/ScriptFuncs.h"
#include "engine/EngineInitVars.h"

#include "system/LowLevelSystem.h"
#include "engine/LowLevelEngineSetup.h"

#include "impl/SDLEngineSetup.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FPS COUNTER
	//////////////////////////////////////////////////////////////////////////

	cFPSCounter::cFPSCounter(iLowLevelSystem* apLowLevelSystem)
	{
		mfFPS = 60;

		mlFramecounter=0;
		mfFrametimestart=0;
		mfFrametime=0;
	
		mfUpdateRate = 1;

		mpLowLevelSystem = apLowLevelSystem;

		mfFrametimestart = ((float)cPlatform::GetApplicationTime()) / 1000.0f; 
	}
	
	void cFPSCounter::AddFrame()
	{
		mlFramecounter++;

		mfFrametime = (((float)cPlatform::GetApplicationTime()) / 1000.0f) - mfFrametimestart;
		
		// update the timer
		if (mfFrametime >= mfUpdateRate)
		{
			mfFPS = ((float)mlFramecounter)/mfFrametime;
			mlFramecounter = 0;
			mfFrametimestart = ((float)cPlatform::GetApplicationTime()) / 1000.0f; 
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// SETUP VAR CONTAINER
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	cSetupVarContainer::cSetupVarContainer()
	{
		msBlank = "";
	}

	//-----------------------------------------------------------------------


	void cSetupVarContainer::AddString(const tString& asName, const tString& asValue)
	{
		std::map<tString, tString>::value_type val(asName,asValue);
		m_mapVars.insert(val);
	}

	void cSetupVarContainer::AddInt(const tString& asName, int alValue)
	{
		AddString(asName, cString::ToString(alValue));
	}
	void cSetupVarContainer::AddFloat(const tString& asName, float afValue)
	{
		AddString(asName, cString::ToString(afValue));
	}
	void cSetupVarContainer::AddBool(const tString& asName, bool abValue)
	{
		AddString(asName, abValue ? "true" : "false");
	}

	//-----------------------------------------------------------------------


	const tString& cSetupVarContainer::GetString(const tString& asName)
	{
		std::map<tString, tString>::iterator it = m_mapVars.find(asName);
		if(it == m_mapVars.end()) return msBlank;
		else return it->second;
	}

	float cSetupVarContainer::GetFloat(const tString& asName, float afDefault)
	{
		const tString& sVal = GetString(asName);
		if(sVal == "") 
			return afDefault;
		else 
			return cString::ToFloat(sVal.c_str(),afDefault);
	}
	int cSetupVarContainer::GetInt(const tString& asName, int alDefault)
	{
		const tString& sVal = GetString(asName);
		if(sVal == "") 
			return alDefault;
		else 
			return cString::ToInt(sVal.c_str(),alDefault);
	}
	bool cSetupVarContainer::GetBool(const tString& asName, bool abDefault)
	{
		const tString& sVal = GetString(asName);
		if(sVal == "") 
			return abDefault;
		else 
			return cString::ToBool(sVal.c_str(),abDefault);
	}

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUCNTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cEngine* CreateHPLEngine(eHplAPI aApi, tFlag alHplModuleFlags, cEngineInitVars *apVars)
	{
		iLowLevelEngineSetup *pGameSetup = NULL;

		switch(aApi)
		{
			case eHplAPI_OpenGL: pGameSetup = hplNew(cSDLEngineSetup, (alHplModuleFlags) ); break;
		}

		return hplNew( cEngine,  (pGameSetup,alHplModuleFlags, apVars) ); 
	}

	//-----------------------------------------------------------------------

	void DestroyHPLEngine(cEngine* apGame)
	{
		hplDelete( apGame );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cEngine::mbDevicePlugged = false;
	bool cEngine::mbDeviceRemoved = false;

	//-----------------------------------------------------------------------

	cEngine::cEngine(iLowLevelEngineSetup *apGameSetup,tFlag alHplSetupFlags, cEngineInitVars *apVars)
	{
		GameInit(apGameSetup,alHplSetupFlags, apVars);

		//Set up variables
		mbWaitIfAppOutOfFocus = false;

		mbApplicationHasInputFocus = false;
		mbApplicationHasMouseFocus = false;
		mbApplicationIsVisible = false;

		mvEngineTypeStrings.resize(eVariableType_LastEnum);
		mvEngineTypeStrings[eVariableType_Int] =	"Int";
		mvEngineTypeStrings[eVariableType_Float] =	"Float";
		mvEngineTypeStrings[eVariableType_Vec2] =	"Vector2";
		mvEngineTypeStrings[eVariableType_Vec3] =	"Vector3";
		mvEngineTypeStrings[eVariableType_Color] =	"Color";
		mvEngineTypeStrings[eVariableType_String] =	"String";
		mvEngineTypeStrings[eVariableType_Enum] =	"Enum";
		mvEngineTypeStrings[eVariableType_Bool] =	"Bool";
	}


	//-----------------------------------------------------------------------

	void cEngine::GameInit(iLowLevelEngineSetup *apGameSetup,tFlag alHplSetupFlags, cEngineInitVars *apVars)
	{
		mpGameSetup = apGameSetup;

		Log("Creating Engine Modules\n");
		Log("--------------------------------------------------------\n");

		//Create the modules that game connects to and init them!
		Log(" Creating graphics module\n");
		mpGraphics = mpGameSetup->CreateGraphics();
		
		Log(" Creating system module\n");
		mpSystem = mpGameSetup->CreateSystem();

		Log(" Creating resource module\n");
		mpResources = mpGameSetup->CreateResources(mpGraphics);

		Log(" Creating input module\n");
		mpInput = mpGameSetup->CreateInput(mpGraphics);

		Log(" Creating sound module\n");
		mpSound = mpGameSetup->CreateSound();

		Log(" Creating physics module\n");
		mpPhysics = mpGameSetup->CreatePhysics();

		Log(" Creating ai module\n");
		mpAI = mpGameSetup->CreateAI();

		Log(" Creating gui module\n");
		mpGui = hplNew(cGui,());

		Log(" Creating generate module\n");
		mpGenerate = hplNew(cGenerate,());

		Log(" Creating haptic module\n");
#ifdef INCLUDE_HAPTIC
		mpHaptic = mpGameSetup->CreateHaptic();
#else
		mpHaptic = NULL;
#endif


		Log(" Creating scene module\n");
		mpScene = mpGameSetup->CreateScene(mpGraphics, mpResources, mpSound,mpPhysics,mpSystem,mpAI,mpGui,mpHaptic);

		Log("--------------------------------------------------------\n\n");


		//Init the resources
		mpResources->Init(mpGraphics,mpSystem, mpSound,mpScene,mpGui, mpPhysics);

		//Init the graphics
		mpGraphics->Init(	apVars->mGraphics.mvScreenSize.x,
							apVars->mGraphics.mvScreenSize.y,
							apVars->mGraphics.mlDisplay,
							apVars->mGraphics.mlScreenBpp,
							apVars->mGraphics.mbFullscreen,
							apVars->mGraphics.mlMultisampling, 
							apVars->mGraphics.mGpuProgramFormat,
							apVars->mGraphics.msWindowCaption,
							apVars->mGraphics.mvWindowPosition,
							mpResources,alHplSetupFlags);
		
		//Init Sound
		mpSound->Init(mpResources, apVars->mSound.mlSoundDeviceID,
						apVars->mSound.mbUseEnvironmentalAudio,
						apVars->mSound.mlMaxChannels,
						apVars->mSound.mlStreamUpdateFreq,
						apVars->mSound.mbUseThreading,
						apVars->mSound.mbUseVoiceManagement,
						apVars->mSound.mlMaxMonoChannelsHint,
						apVars->mSound.mlMaxStereoChannelsHint,
						apVars->mSound.mlStreamBufferSize,
						apVars->mSound.mlStreamBufferCount,
						apVars->mSound.mbLowLevelLogging);

		//Init physics
		mpPhysics->Init(mpResources);

		//Init AI
		mpAI->Init();

		//Init Gui
		mpGui->Init(mpResources,mpGraphics,mpSound,mpScene, mpInput);
		
		//Init Generate
		mpGenerate->Init(mpResources,mpGraphics);


		//Init haptic
		if(mpHaptic) mpHaptic->Init(mpResources);

		Log("Initializing Game Module\n");
		Log("--------------------------------------------------------\n");
		//Create the updatehandler
		Log(" Adding engine updates\n");
		mpUpdater = hplNew( cUpdater,(mpSystem->GetLowLevel()));

		//Add some loaded modules to the updater
		mpUpdater->AddGlobalUpdate(mpInput);
		mpUpdater->AddGlobalUpdate(mpPhysics);
		mpUpdater->AddGlobalUpdate(mpScene);
		mpUpdater->AddGlobalUpdate(mpGraphics);
		mpUpdater->AddGlobalUpdate(mpSound);
		mpUpdater->AddGlobalUpdate(mpAI);
		mpUpdater->AddGlobalUpdate(mpGui);
		mpUpdater->AddGlobalUpdate(mpResources);
		if(mpHaptic) mpUpdater->AddGlobalUpdate(mpHaptic);

		//Setup the "default" updater container
		mpUpdater->AddContainer("Default");
		mpUpdater->SetContainer("Default");

		//Create the logic timer.
		mpLogicTimer = mpSystem->CreateLogicTimer(apVars->mGame.mlUpdateRate);

		//Init some standard script funcs
		Log(" Initializing script functions\n");
		cScriptFuncs::Init(mpGraphics,mpResources,mpSystem,mpInput,mpScene,mpSound,this);
		
		mpMutex = cPlatform::CreateMutEx();

		//Since game is not done:
		mbGameIsDone=false;

		mbPaused = false;

		mbRenderOnce = false;

		mfGameTime =0;

		mbLimitFPS = true;

		mpFPSCounter = hplNew( cFPSCounter,(mpSystem->GetLowLevel()) );
		mpFrameTimer = cPlatform::CreateTimer();
		Log("--------------------------------------------------------\n\n");

		Log("User Initialization\n");
		Log("--------------------------------------------------------\n");
	}

	//-----------------------------------------------------------------------

	cEngine::~cEngine()
	{
		Log("--------------------------------------------------------\n\n");

		hplDelete(mpLogicTimer);
		hplDelete(mpFPSCounter);
		hplDelete(mpFrameTimer);
		hplDelete(mpMutex);
		
		hplDelete(mpUpdater);
		
		hplDelete(mpGui);
		hplDelete(mpGenerate);
		hplDelete(mpScene);
		if(mpHaptic) hplDelete(mpHaptic);
		hplDelete(mpInput);
		hplDelete(mpSound);
		hplDelete(mpGraphics);
		hplDelete(mpResources);
		hplDelete(mpPhysics);
		hplDelete(mpAI);
		hplDelete(mpSystem);
		
		Log(" Deleting game setup provided by user\n");
		hplDelete(mpGameSetup);
		
		Log("HPL Exit was successful!\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	int glClearUpdateCheck=0;
	void cEngine::Run()
	{
		//Log line that ends user init.
		Log("--------------------------------------------------------\n\n");
		
		double fNumOfTimes=0;
		double fMediumTime=0;

		mpUpdater->BroadcastMessageToAll(eUpdateableMessage_OnStart);
		
		mpLogicTimer->Reset();

		//Loop the game... fix the var...
		unsigned long lTempTime = cPlatform::GetApplicationTime();
		
		Log("Game Running\n");
		Log("--------------------------------------------------------\n");

		mfFrameTime = 0;
		mpFrameTimer->Start();
		
		bool bIsUpdated = true;
		bool bBufferSwap = false;
		bool bSwappedOnce = false;
		
		//cMemoryManager::SetLogCreation(true);

		while(!GetGameIsDone())
		{
			//////////////////////////
			//Check if application is in focus.
			if(mbWaitIfAppOutOfFocus) CheckIfAppInFocusElseWait();

			//////////////////////////
			//Check if paused
			if(GetPaused())
			{
				cPlatform::Sleep(10);
				mpInput->Update(1.0f/100.0f);
				bIsUpdated = true;

				mpUpdater->RunMessage(eUpdateableMessage_OnPauseUpdate, 1.0f/100.0f);
			}
			else
			{
				//////////////////////////
				//Update logic.
				while(mpLogicTimer->WantUpdate() && !GetGameIsDone())
				{
					/////////////////////////////////////////////
					// Run Update callback in updater
					mpUpdater->RunMessage(eUpdateableMessage_PreUpdate, GetStepSize());
					mpUpdater->RunMessage(eUpdateableMessage_Update, GetStepSize());
					mpUpdater->RunMessage(eUpdateableMessage_PostUpdate, GetStepSize());
					bIsUpdated = true;

                    if (mpInput->isQuitMessagePosted()) {

                        mpUpdater->RunMessage(eUpdateableMessage_OnQuit);

                        mpInput->resetQuitMessagePosted();
                    }

					/////////////////////////////////////////////
					// Run Check if any application focus changed
					CheckAndBroadcastFocusChange();

					/////////////////////////////////////////////
					// Run Check if devices were plugged/unplugged
					CheckAndBroadcastDeviceChange();

					/////////////////////////////////////////////
					// If log update is active, clear it regularly.
					if(GetUpdateLogActive())
					{
						glClearUpdateCheck++;
						if(glClearUpdateCheck % 20 == 0)
						{
							if(mpUpdater->GetCurrentContainerName() == "Default") ClearUpdateLogFile();
						}
					}
				
					//Increase game time.
					mfGameTime += GetStepSize();
				}
				mpLogicTimer->EndUpdateLoop();
			}

			//if(GetGameIsDone()) Log("1\n");

            ////////////////////////////////////
			// If rendering once, make a check and if already drawn screen, just continue
			if(mbRenderOnce && bSwappedOnce)
			{
				continue;
			}

			//if(GetGameIsDone()) Log("2\n");

			////////////////////////////////////////////////
			//Swap buffers and call callback, do this after update, so hardware can work during update
			if(bBufferSwap)
			{
				bBufferSwap = false;
				START_TIMING(WaitAndFinishRendering)
				//mpGraphics->GetLowLevel()->WaitAndFinishRendering();
				STOP_TIMING(WaitAndFinishRendering)

				START_TIMING(SwapBuffers)
				mpGraphics->GetLowLevel()->SwapBuffers();
				STOP_TIMING(SwapBuffers)
				
				//Log("Swap done: %d\n", cPlatform::GetApplicationTime());
				mpUpdater->RunMessage(eUpdateableMessage_OnPostBufferSwap);
				bSwappedOnce =true;
				if(mbRenderOnce) continue;
			}

			//if(GetGameIsDone()) Log("3\n");

			////////////////////////////////////
			// Render frame
			if(mbLimitFPS==false || bIsUpdated)
			{
				///////////////////////////////////////
           		//Get the the from the last frame.
				UpdateFrameTimer();

				//On draw callback sending that to gui, etc
				START_TIMING(OnDraw)
				mpUpdater->RunMessage(eUpdateableMessage_OnDraw, mfFrameTime);
				STOP_TIMING(OnDraw)
				
				//Render this frame
				START_TIMING(RenderAll)
				mpScene->Render(mfFrameTime, tSceneRenderFlag_All);
				STOP_TIMING(RenderAll)

				START_TIMING(PostRender)
				mpUpdater->RunMessage(eUpdateableMessage_OnPostRender, mfFrameTime);
				STOP_TIMING(PostRender)
				
				START_TIMING(FlushRender)
				mpGraphics->GetLowLevel()->FlushRendering();
				STOP_TIMING(FlushRender)
				
				//Update fps counter.
				mpFPSCounter->AddFrame();
	           	
				fNumOfTimes++;
				bIsUpdated = false;
				bBufferSwap = true;
			}

			//if(GetGameIsDone()) Log("4\n");
		}
		Log("--------------------------------------------------------\n\n");
	
		Log("Statistics\n");
		Log("--------------------------------------------------------\n");

		unsigned long lTime = cPlatform::GetApplicationTime() - lTempTime;
		fMediumTime = fNumOfTimes/(((double)lTime)/1000);
		
		Log(" Medium framerate: %f\n", fMediumTime);
		Log("--------------------------------------------------------\n\n");

		Log("User Exit\n");
		Log("--------------------------------------------------------\n");
		
		mpUpdater->BroadcastMessageToAll(eUpdateableMessage_OnExit);
	}
	//-----------------------------------------------------------------------

	void cEngine::Exit()
	{
		mpMutex->Lock();

		mbGameIsDone = true;
		//Log("Exit!\n");

		mpMutex->Unlock();
	}

	bool cEngine::GetGameIsDone()
	{
		mpMutex->Lock();

		bool bDone = mbGameIsDone;

		mpMutex->Unlock();

		return bDone;
	}
	//-----------------------------------------------------------------------

	void cEngine::ResetLogicTimer()
	{
		mpLogicTimer->Reset();
	}

	void cEngine::SetUpdatesPerSec(int alUpdatesPerSec)
	{
		mpLogicTimer->SetUpdatesPerSec(alUpdatesPerSec);
	}

	int cEngine::GetUpdatesPerSec()
	{
		return mpLogicTimer->GetUpdatesPerSec();
	}

	float cEngine::GetStepSize()
	{
		return mpLogicTimer->GetStepSize();
	}

	//-----------------------------------------------------------------------
	
	float cEngine::GetFPS()
	{
		return mpFPSCounter->mfFPS;
	}

	//-----------------------------------------------------------------------

	void cEngine::SetSpeedMul(float afX)
	{
		mpLogicTimer->SetSpeedMul(afX);
	}

	//-----------------------------------------------------------------------

	float cEngine::GetAvgFrameTimeInMS()
	{
		return (1.0f/mpFPSCounter->mfFPS)*1000.0f;
	}
	
	//-----------------------------------------------------------------------

	void cEngine::SetFPSUpdateRate(float afSec)
	{
		mpFPSCounter->mfUpdateRate = afSec;
	}
	float cEngine::GetFPSUpdateRate()
	{
		return mpFPSCounter->mfUpdateRate;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cEngine::CreateLocalVar(const tString& asName)
	{
		cScriptVar* pVar;
		pVar= GetLocalVar(asName);
		if(pVar==NULL)
		{
			cScriptVar Var;
			Var.msName = asName;
			m_mapLocalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName),Var));
			pVar= GetLocalVar(asName);
			if(pVar==NULL)FatalError("Very strange error when creating script var!\n");
		}
		return pVar;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cEngine::GetLocalVar(const tString& asName)
	{
		tScriptVarMapIt it = m_mapLocalVars.find(cString::ToLowerCase(asName));
		if(it==m_mapLocalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------------------

	tScriptVarMap* cEngine::GetLocalVarMap()
	{
		return &m_mapLocalVars;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cEngine::CreateGlobalVar(const tString& asName)
	{
		cScriptVar* pVar;
		pVar= GetGlobalVar(asName);
		if(pVar==NULL)
		{
			cScriptVar Var;
			Var.msName = asName;
			m_mapGlobalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName),Var));
			pVar= GetGlobalVar(asName);
			if(pVar==NULL)FatalError("Very strange error when creating script var!\n");
		}
		return pVar;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cEngine::GetGlobalVar(const tString& asName)
	{
		tScriptVarMapIt it = m_mapGlobalVars.find(cString::ToLowerCase(asName));
		if(it==m_mapGlobalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------------------

	tScriptVarMap* cEngine::GetGlobalVarMap()
	{
		return &m_mapGlobalVars;
	}

	//-----------------------------------------------------------------------

	void cEngine::ClearAllVariables()
	{
		m_mapGlobalVars.clear();
		m_mapLocalVars.clear();
	}

	//-----------------------------------------------------------------------

	eVariableType cEngine::GetEngineTypeFromString(const tString& asType)
	{
		for(int i=0;i<(int)mvEngineTypeStrings.size();++i)
		{
			if(asType==mvEngineTypeStrings[i])
				return (eVariableType)i;
		}

		return eVariableType_LastEnum;
	}

	eVariableType cEngine::GetEngineTypeFromStringW(const tWString& asType)
	{
		return GetEngineTypeFromString(cString::To8Char(asType));
	}

	//-----------------------------------------------------------------------

	void cEngine::SetPaused(bool abPaused)
	{
		mpMutex->Lock();

		if(mbPaused != abPaused)
		{
			mbPaused = abPaused;
			if(mbPaused==false) mpLogicTimer->Reset();
		}

		mpMutex->Unlock();
	}

	//-----------------------------------------------------------------------
	
	bool cEngine::GetPaused()
	{
		mpMutex->Lock();

		bool bRet = mbPaused;

		mpMutex->Unlock();

		return bRet;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cEngine::UpdateFrameTimer()
	{
		mpFrameTimer->Stop();
		mfFrameTime = (float) mpFrameTimer->GetTimeInSec();

		/*mlstFrameTimes.push_back(mpFrameTimer->GetTimeInMilliSec());
		if((int)mlstFrameTimes.size() >= mlMaxFrameTimes)
		{
			double fFrameSum=0.0;
			for(tDoubleListIt it = mlstFrameTimes.begin(); it != mlstFrameTimes.end(); it++)
			{
				fFrameSum += *it;
			}
			mfAvgFrameTimeInMS = (float)(fFrameSum / (double)mlstFrameTimes.size());
			mlstFrameTimes.clear();
		}*/
				
		mpFrameTimer->Start();
	}

	//-----------------------------------------------------------------------

	void cEngine::CheckAndBroadcastFocusChange()
	{
		bool bHadInputFocus = mbApplicationHasInputFocus;
		bool bHadMouseFocus = mbApplicationHasMouseFocus;
		bool bHadVisibility = mbApplicationIsVisible;

        iLowLevelGraphics *pllGfx = mpGraphics->GetLowLevel();
		mbApplicationHasInputFocus = pllGfx->GetWindowInputFocus();
		mbApplicationHasMouseFocus = pllGfx->GetWindowMouseFocus();
		mbApplicationIsVisible = pllGfx->GetWindowIsVisible();

		if(bHadInputFocus && !mbApplicationHasInputFocus) mpUpdater->RunMessage(eUpdateableMessage_AppLostInputFocus);
		if(!bHadInputFocus && mbApplicationHasInputFocus) mpUpdater->RunMessage(eUpdateableMessage_AppGotInputFocus);

		if(bHadMouseFocus && !mbApplicationHasMouseFocus) mpUpdater->RunMessage(eUpdateableMessage_AppLostMouseFocus);
		if(!bHadMouseFocus && mbApplicationHasMouseFocus) mpUpdater->RunMessage(eUpdateableMessage_AppGotMouseFocus);

		if(bHadVisibility && !mbApplicationIsVisible) mpUpdater->RunMessage(eUpdateableMessage_AppLostVisibility);
		if(!bHadVisibility && mbApplicationIsVisible) mpUpdater->RunMessage(eUpdateableMessage_AppGotVisibility);
	}

	//-----------------------------------------------------------------------

	void cEngine::CheckIfAppInFocusElseWait()
	{
		iLowLevelGraphics *pllGfx = mpGraphics->GetLowLevel();
		while(	pllGfx->GetWindowInputFocus()==false)
		{
			cPlatform::Sleep(100);
			mpInput->Update(1.0f/10.0f);
		}
	}

	//-----------------------------------------------------------------------

	void cEngine::CheckAndBroadcastDeviceChange()
	{
		if(mbDevicePlugged)
		{
			mbDevicePlugged = false;
			mpUpdater->RunMessage(eUpdateableMessage_AppDeviceWasPlugged);
		}

		if(mbDeviceRemoved)
		{
			mbDeviceRemoved = false;
			mpUpdater->RunMessage(eUpdateableMessage_AppDeviceWasRemoved);
		}
	}

	//-----------------------------------------------------------------------
}
