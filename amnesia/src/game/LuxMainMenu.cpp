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

#include "LuxMainMenu.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxInputHandler.h"
#include "LuxPlayer.h"
#include "LuxAreaNodes.h"
#include "LuxHelpFuncs.h"
#include "LuxSaveHandler.h"
#include "LuxConfigHandler.h"
#include "LuxDebugHandler.h"
#include "LuxLoadScreenHandler.h"

#include "LuxMainMenu_Profile.h"
#include "LuxMainMenu_Options.h"
#include "LuxMainMenu_KeyConfig.h"
#include "LuxMainMenu_StartGame.h"
#include "LuxMainMenu_LoadGame.h"
#include "LuxMainMenu_CustomStory.h"

#include "LuxDemoEnd.h"
#include <sstream>
#include "LuxAchievementHandler.h"

//--------------------------------------------------------------------------------

static const bool gbDebug_SkipBGScene = false;
static const bool gbDebug_FastLoadOptions = false;
const int glHardMode_SaveCost = 4;

#if MAC_OS || LINUX
std::wstring LongToWString(const long long &l)
{
    std::string s;
    std::stringstream strstream;
    strstream << l;
    strstream >> s;
    std::wstring temp(s.length(),L' ');
    std::copy(s.begin(),s.end(), temp.begin());
    return temp;
}

const tWString gsHardMode_SaveCostString = tWString(LongToWString((static_cast<long long>(glHardMode_SaveCost))));
#else
const tWString gsHardMode_SaveCostString = tWString(to_wstring(static_cast<long long>(glHardMode_SaveCost)));
#endif
//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// WINDOW INTERFACE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxMainMenuWindow::iLuxMainMenuWindow(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin)
{
	mpGui = gpBase->mpEngine->GetGui();

	mpGuiSet = apGuiSet;
	mpGuiSkin = apGuiSkin;

	mpWindow = NULL;

	mvScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
}

void iLuxMainMenuWindow::SetActive(bool abX)
{
	if(mpWindow == NULL) return;

    mpWindow->SetVisible(abX);
	mpWindow->SetEnabled(abX);

	if(abX)
	{
		mpWindow->CenterGlobalPositionInSet();
	}

	OnSetActive(abX);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu::cLuxMainMenu() : iLuxUpdateable("LuxDebugHandler")
{
	///////////////////////////////
	//Get Engine
	mpGui = gpBase->mpEngine->GetGui();
	mpScene = gpBase->mpEngine->GetScene();
	mpGraphics = gpBase->mpEngine->GetGraphics();

	///////////////////////////////
	//Setup GUI stuff
	mpGuiSkin = mpGui->CreateSkin("gui_main_menu.skin");
	mpGuiSet = mpGui->CreateSet("MainMenu",mpGuiSkin);
	mpGuiSet->SetDrawMouse(false);//Init
	
	//////////////////////////////
	// Make skin font size a bit bigger if its chinese
	if (gpBase->msCurrentLanguage == "chinese.lang")
	{
		mpGuiSet->GetSkin()->GetFont(eGuiSkinFont_Default)->mvSize *= 1.4f;
		mpGuiSet->GetSkin()->GetFont(eGuiSkinFont_Disabled)->mvSize *= 1.4f;
	}

	///////////////////////////////
	//Create Viewport
	mpViewport = mpScene->CreateViewport();
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);
	mpGuiSet->SetActive(false);

	mpViewport->AddGuiSet(mpGuiSet);

	///////////////////////
	// Create Windows
	mvWindows.resize(eLuxMainMenuWindow_LastEnum, NULL);

	mvWindows[eLuxMainMenuWindow_Profiles] = hplNew(cLuxMainMenu_Profile, (mpGuiSet, mpGuiSkin));
	mvWindows[eLuxMainMenuWindow_Options] = hplNew(cLuxMainMenu_Options, (mpGuiSet, mpGuiSkin));
	mvWindows[eLuxMainMenuWindow_KeyConfig] = hplNew(cLuxMainMenu_KeyConfig, (mpGuiSet, mpGuiSkin));
	mvWindows[eLuxMainMenuWindow_LoadGame] = hplNew(cLuxMainMenu_LoadGame, (mpGuiSet, mpGuiSkin));
	mvWindows[eLuxMainMenuWindow_StartGame] = hplNew(cLuxMainMenu_StartGame, (mpGuiSet, mpGuiSkin));


	cLuxMainMenu_CustomStory* pCustomStoryWindow = hplNew(cLuxMainMenu_CustomStory,(mpGuiSet, mpGuiSkin));
	mvWindows[eLuxMainMenuWindow_CustomStoryList] = hplNew(cLuxMainMenu_CustomStoryList, (mpGuiSet, mpGuiSkin, pCustomStoryWindow));
	mvWindows[eLuxMainMenuWindow_CustomStory] = pCustomStoryWindow;

	///////////////////////
	// Load settings
	mvScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();

	mfMainFadeInTime = gpBase->mpMenuCfg->GetFloat("Main","MainFadeInTime", 0);
	mfMainFadeOutTimeFast = gpBase->mpMenuCfg->GetFloat("Main","MainFadeOutTimeFast", 0);
	mfMainFadeOutTimeSlow = gpBase->mpMenuCfg->GetFloat("Main","MainFadeOutTimeSlow", 0);
	 
	mfTopMenuFadeInTime = gpBase->mpMenuCfg->GetFloat("Main","TopMenuFadeInTime", 0);
	mfTopMenuFadeOutTime = gpBase->mpMenuCfg->GetFloat("Main","TopMenuFadeOutTime", 0);
	
	mvTopMenuStartPos = gpBase->mpMenuCfg->GetVector2f("Main","TopMenuStartRelativePos", 0) * mvScreenSize;
	mvTopMenuStartPos.z = 2;
	mvTopMenuFontSize = gpBase->mpMenuCfg->GetVector2f("Main","TopMenuFontRelativeSize", 0) * mvScreenSize;

	mvTopMenuStartPosInGame = gpBase->mpMenuCfg->GetVector2f("Main", "TopMenuStartRelativePosInGame", 0) * mvScreenSize;
	mvTopMenuStartPosInGame.z = 2;
	
	mvTopMenuFontSize.x  *= (mvScreenSize.y / mvScreenSize.x) / (3.0f/4.0f);//Make font more narrow to compensate for wide screen.

	mvLogoPos = gpBase->mpMenuCfg->GetVector2f("Main", "MainMenuLogoStartRelativePos", 0) * mvScreenSize;
	mvLogoPos.z = 2;
	mvLogoSize = gpBase->mpMenuCfg->GetVector2f("Main", "MainMenuLogoRelativeSize", 0) * mvScreenSize;
	
	msMusic = gpBase->mpMenuCfg->GetString("Main", "Music", "");
	msZoomSound = gpBase->mpMenuCfg->GetString("Main", "ZoomSound", "");

	mfBgCamera_FOV= cMath::ToRad(gpBase->mpMenuCfg->GetFloat("Main", "BGCamera_FOV", 0.1));
	mfBgCamera_ZoomedFOV = cMath::ToRad(gpBase->mpMenuCfg->GetFloat("Main", "BGCamera_ZoomedFOV", 1)); 
	
	///////////////////////////////
	//Load data
	mpFont = NULL;

	mpTopBackground = mpGui->CreateGfxFilledRect(cColor(0,1),eGuiMaterial_Alpha);
	mpBlackFade = mpGui->CreateGfxFilledRect(cColor(0,1),eGuiMaterial_Alpha);

	//Crete programs for blur
	for(int i=0; i<2;++i)
	{
		cParserVarContainer programVars;
		if(i==0) programVars.Add("BlurHorisontal");
		
		mpBlurProgram[i] = mpGraphics->CreateGpuProgramFromShaders("MainMenuBlur"+cString::ToString(i),
																	"mainmenu_screen_blur_vtx.glsl",
																	"mainmenu_screen_blur_frag.glsl",
																	&programVars);
	}

	mpScreenTexture = NULL;
	mpScreenGfx = NULL;
	mpScreenBlurTexture = NULL;
	mpScreenBlurGfx = NULL;

	mpBgCamera = NULL;
	mpBgWorld = NULL;

	///////////////////////////////
	//Setup variables
	mbGuiCreated = false;

	mbExiting = false;
	mbRecreateGui = false;

	mCurrentWindow = eLuxMainMenuWindow_LastEnum;

	mpLastFocusedItem = NULL;
	mpSaveCost = NULL;
	mpNumTinderboxes = NULL;

	Reset();
}

//-----------------------------------------------------------------------

cLuxMainMenu::~cLuxMainMenu()
{
	for(size_t i=0; i<mvWindows.size(); ++i)
	{
		if(mvWindows[i]) hplDelete(mvWindows[i]);
	}
}

void cLuxMainMenu::OnQuit()
{
    cLuxMapHandler *mpMapHandler = gpBase->mpMapHandler;
    if(mpMapHandler->GetCurrentMap())
    {
        //Save
        gpBase->mpSaveHandler->AutoSave();
        // Destroy Map
        mpMapHandler->DestroyMap(mpMapHandler->GetCurrentMap(),false);

        //Reset game
        gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
        gpBase->SetCustomStory(NULL);
    }
    
    //Quit Game
    mExitMessage = eLuxMainMenuExit_QuitGame;
    OnMenuExit();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu::LoadUserConfig()
{
}

void cLuxMainMenu::SaveUserConfig()
{
	 
}

void cLuxMainMenu::OnClearFonts()
{
}

void cLuxMainMenu::LoadFonts()
{
	tString sFontFile = gpBase->mpMenuCfg->GetString("Main","TopMenuFont","");
	mpFont = LoadFont(sFontFile);
}

//-----------------------------------------------------------------------

void cLuxMainMenu::OnStart()
{
}

//-----------------------------------------------------------------------


void cLuxMainMenu::Reset()
{
	//TODO: Destroy world and camera too!
	if(mpBgWorld)
		mpScene->DestroyWorld(mpBgWorld);
	if(mpBgCamera)
		mpScene->DestroyCamera(mpBgCamera);

	mpViewport->SetCamera(NULL);
	mpViewport->SetWorld(NULL);

	mpBgWorld = NULL;
	mpBgCamera = NULL;
}

//-----------------------------------------------------------------------

void cLuxMainMenu::OnEnterContainer(const tString& asOldContainer)
{
	//Unlock input if not in window
	if(gpBase->mpConfigHandler->mbFullscreen==false) {
		gpBase->mpEngine->GetInput()->GetLowLevel()->LockInput(false);
	}
	gpBase->mpEngine->GetInput()->GetLowLevel()->RelativeMouse(false);

	////////////////////////////
	//Set up states and viewport
	gpBase->mpInputHandler->ChangeState(eLuxInputState_MainMenu);

	mpViewport->SetActive(true);
	mpViewport->SetVisible(true);

	
//	gpBase->SetDrawOnLiveCursor(false);

	mpGuiSet->SetActive(true);
	mpGui->SetFocus(mpGuiSet);

	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHandler->Play(msMusic, 1.0f, 0.35f, true, false);
	}
	else
	{
		ProgLog(eLuxProgressLogLevel_Low, "Enter main menu.");
		gpBase->mpMapHandler->PauseSoundsAndMusic();
	}
#ifdef USE_GAMEPAD
	if(gpBase->mpInputHandler->IsGamepadPresent() == false)
	{
		mpGuiSet->SetDrawMouse(true);
		mpGuiSet->SetMouseMovementEnabled(true);
	}
	else if(gpBase->mpInputHandler->IsGamepadPresent())
	{
		mpGuiSet->SetDrawMouse(false);
		mpGuiSet->SetMouseMovementEnabled(false);
	}
#else
	mpGuiSet->SetDrawMouse(true);
#endif
	////////////////////////////
	//Set up menu variables

	mfMenuFadeAlpha = 1.0f;	//Fade in from black or from sharp -> blurry if ingame
	
	mbExiting = false;

	mCurrentWindow = eLuxMainMenuWindow_LastEnum;

	CreateGui();
	CreateBackground();

	////////////////////////////////////
	//No User Config, start with Profiles
	if(gpBase->mpUserConfig==NULL)
	{
		SetWindowActive(eLuxMainMenuWindow_Profiles);
	}
	
	//Always fade in top menu:
	mfTopMenuAlpha =0.01f;
	UpdateTopMenu(1.0f/60.0f);

	mfCamTimer = 0.0f;

	////////////////////////////////////
	//Debug
	if(gbDebug_FastLoadOptions)
	{
		gpBase->SetProfile(gpBase->msDefaultProfileName);
		gpBase->InitUserConfig();
		gpBase->mpMainMenu->RecreateGui();

		SetWindowActive(eLuxMainMenuWindow_Options);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::OnLeaveContainer(const tString& asNewContainer)
{
	//Unlock input if not in window
	if (gpBase->mpDebugHandler->GetDebugWindowActive()==false)
	{
		if(	gpBase->mpConfigHandler->mbFullscreen==false)
		{
			gpBase->mpEngine->GetInput()->GetLowLevel()->LockInput(true);
		}
		gpBase->mpEngine->GetInput()->GetLowLevel()->RelativeMouse(true);
	}

	////////////////////////////
	//Turn off music
	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHandler = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHandler->Stop(0.33f);
	}
	
	////////////////////////////
	//Set up states
	mpViewport->SetActive(false);
	mpViewport->SetVisible(false);

	mpGuiSet->SetActive(false);

	DestroyBackground();
}

//-----------------------------------------------------------------------

void cLuxMainMenu::Update(float afTimeStep)
{
	//Log("Update start!\n");
	//Log("1\n");
	if(mbRecreateGui)
	{
		eLuxMainMenuWindow currentWindow = mCurrentWindow;
		mCurrentWindow = eLuxMainMenuWindow_LastEnum;
		//Log("1.1\n");
		CreateGui();
		//Log("1.2\n");
		SetWindowActive(currentWindow);

        mbRecreateGui = false;
	}

	//Log("2\n");
	UpdateBase(afTimeStep);
	//Log("3\n");
	UpdateTopMenu(afTimeStep);
	//Log("Update end!\n");

	//mpViewport->GetRenderSettings()->mbLog = true;
}

//-----------------------------------------------------------------------


void cLuxMainMenu::OnDraw(float afFrameTime)
{
	/////////////////////////////////
	//Screen background
	if(mpScreenGfx)
	{
		if(mpScreenGfx && mfMenuFadeAlpha>0) 
			mpGuiSet->DrawGfx(mpScreenGfx,cVector3f(0,0,0),mvScreenSize);
		
		if(mpScreenBlurGfx)
			mpGuiSet->DrawGfx(mpScreenBlurGfx,cVector3f(0,0,0.2f),mvScreenSize,cColor(1, 1-mfMenuFadeAlpha));

		if(	mfMenuFadeAlpha > 0 && mbExiting && mExitMessage != eLuxMainMenuExit_ReturnToGame )
		{
			mpGuiSet->DrawGfx(	mpBlackFade,cVector3f(0,0,50), mvScreenSize, cColor(1 ,mfMenuFadeAlpha));
		}

		//Top Menu background
		mpGuiSet->DrawGfx(	mpTopBackground,cVector3f(0,mvTopMenuStartPos.y,0.5f),
							cVector2f(mvScreenSize.x, mvScreenSize.y - mvTopMenuStartPos.y),
							cColor(1 ,0.5f*mfTopMenuAlpha));
	}
	/////////////////////////////////
	//3D background
	else
	{
		if(mfMenuFadeAlpha > 0)
			mpGuiSet->DrawGfx(	mpBlackFade,cVector3f(0,0,50), mvScreenSize, cColor(1 ,mfMenuFadeAlpha));

		if(mpLogoGfx)
			mpGuiSet->DrawGfx( mpLogoGfx, mvLogoPos, mvLogoSize, cColor(1, mfTopMenuAlpha) );
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::OnPostRender(float afFrameTime)
{
	//Debug:
	//Turn of logging so it only happens one frame?
	//mpViewport->GetRenderSettings()->mbLog->mbLog = true;
}

//-----------------------------------------------------------------------

void cLuxMainMenu::SetWindowActive(eLuxMainMenuWindow aWindow)
{
	if(mCurrentWindow == aWindow) return;

#ifdef USE_GAMEPAD
	bool bHasGamepad = gpBase->mpInputHandler->IsGamepadPresent();

	switch(aWindow)
	{
	case eLuxMainMenuWindow_Options:
	case eLuxMainMenuWindow_KeyConfig:
	case eLuxMainMenuWindow_LoadGame:
	case eLuxMainMenuWindow_Profiles:
	case eLuxMainMenuWindow_StartGame:
		mpGuiSet->SetDrawFocus(bHasGamepad);
		break;
	default:
		mpGuiSet->SetDrawFocus(false);
		break;
	}

#endif

	if(mCurrentWindow != eLuxMainMenuWindow_LastEnum)
	{
		if(mvWindows[mCurrentWindow]) mvWindows[mCurrentWindow]->SetActive(false);
	}

	mCurrentWindow = aWindow;
	if(aWindow == eLuxMainMenuWindow_LastEnum)
	{
		SetTopMenuVisible(true);
	}
	else
	{
		SetTopMenuVisible(false);
		if(mvWindows[mCurrentWindow]) mvWindows[mCurrentWindow]->SetActive(true);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::ExitPressed()
{
	/////////////////////////
	//Top menu is active
	if(mCurrentWindow == eLuxMainMenuWindow_LastEnum)
	{
		if(gpBase->mpMapHandler->MapIsLoaded())
		{
			ExitMenu(eLuxMainMenuExit_ReturnToGame);
		}
	}
	/////////////////////////
	//Window is active
	else
	{
		iLuxMainMenuWindow *pWindow = mvWindows[mCurrentWindow];
        if(pWindow) pWindow->ExitPressed();		
	}

}

//-----------------------------------------------------------------------

void cLuxMainMenu::AppLostInputFocus()
{
	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHdlr->Pause();
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::AppGotInputFocus()
{
	if(gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		cMusicHandler* pMusHdlr = gpBase->mpEngine->GetSound()->GetMusicHandler();
		pMusHdlr->Resume();
	}
}

//-----------------------------------------------------------------------
#ifdef USE_GAMEPAD
void cLuxMainMenu::AppDeviceWasPlugged()
{
	bool bHasGamepad = gpBase->mpInputHandler->IsGamepadPresent();
	mpGuiSet->SetDrawFocus(bHasGamepad);
}

//-----------------------------------------------------------------------

void cLuxMainMenu::AppDeviceWasRemoved()
{
	AppDeviceWasPlugged();
}
#endif
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu::ExitMenu(eLuxMainMenuExit aMessage)
{
	SetTopMenuVisible(false);
	mbExiting = true;
	mExitMessage = aMessage;
	gpBase->mpInputHandler->ChangeState(eLuxInputState_Null);

	mpGuiSet->SetDrawMouse(false);
	mpGui->SetFocus(NULL);

	if(	aMessage == eLuxMainMenuExit_ContinueGame ||
		aMessage == eLuxMainMenuExit_StartGame)
	{
		if(msZoomSound != "")
			gpBase->mpEngine->GetSound()->GetSoundHandler()->PlayGui(msZoomSound, false,1.0f);
	}

	if (aMessage == eLuxMainMenuExit_QuitToMenu || aMessage == eLuxMainMenuExit_QuitAndSave)
	{
		tString mapName = gpBase->mpMapHandler->GetCurrentMap()->GetName();
		if (mapName == "00_rainy_hall" || mapName == "01_old_archives")
		{
			gpBase->mpAchievementHandler->UnlockAchievement(eLuxAchievement_NOPE);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::OnMenuExit()
{
	switch(mExitMessage)
	{
	////////////////
	// Quit Game
	case eLuxMainMenuExit_QuitGame:

		if(gpBase->mpDemoEnd && gpBase->mpDemoEnd->ShowOnAllExit())
			gpBase->mpEngine->GetUpdater()->SetContainer("DemoEnd");
		else
			gpBase->mpEngine->Exit();
		
		break;
	////////////////
	// Return To Game
	case eLuxMainMenuExit_ReturnToGame:
		
		ProgLog(eLuxProgressLogLevel_Low, "Return to game from menu");

		gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);
		gpBase->mpEngine->GetUpdater()->SetContainer("Default");

		break;
	////////////////
	// Start Game
	case eLuxMainMenuExit_StartGame:

		gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

		//THIS IS VERY TEMP!
		//TODO: FIX!
		{
			if(gpBase->mpUserConfig==NULL)
			{
				gpBase->CreateProfile(gpBase->msDefaultProfileName);
				gpBase->SetProfile(gpBase->msDefaultProfileName);
				gpBase->InitUserConfig();
			}
			
			gpBase->mpEngine->GetUpdater()->SetContainer("Default");

			gpBase->StartGame("", "", ""); //""= using user config values.
		}
		break;

	////////////////
	// Continue Game
	case eLuxMainMenuExit_ContinueGame:
		{

			gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

			if(gpBase->mpUserConfig==NULL)
			{
				gpBase->CreateProfile(gpBase->msDefaultProfileName);
				gpBase->SetProfile(gpBase->msDefaultProfileName);
				gpBase->InitUserConfig();
			}

			gpBase->mpEngine->GetUpdater()->SetContainer("Default");
			gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);

			gpBase->mpSaveHandler->AutoLoad(true);
		}
		break;
	////////////////
	// Quit To Menu
	case eLuxMainMenuExit_QuitToMenu:
		{
			gpBase->mpLoadScreenHandler->DrawMenuScreen();

			//Reset game
			gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
			gpBase->SetCustomStory(NULL);

			//Start up menu again
			OnLeaveContainer("");
			OnEnterContainer("");
		}
		break;
	
	////////////////
	// Quit And Save
	case eLuxMainMenuExit_QuitAndSave:
		{
			gpBase->mpLoadScreenHandler->DrawMenuScreen();

			//Save
			gpBase->mpSaveHandler->AutoSave();

			//Destroy map
			cLuxMapHandler *mpMapHandler = gpBase->mpMapHandler;
			if(mpMapHandler->GetCurrentMap())
			{
				mpMapHandler->DestroyMap(mpMapHandler->GetCurrentMap(),false);
			}
			
			//Reset game
			gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
			gpBase->SetCustomStory(NULL);

			//Start up menu again
			OnLeaveContainer("");
			OnEnterContainer("");
		}
		break;
        
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::UpdateBase(float afTimeStep)
{
	//////////////////////////
	//Exiting the main menu
	if(mbExiting)
	{
		//Do checks
		float fFadeSpeed = mfMainFadeOutTimeFast;
		bool bZoomCamera = false;

		if(	mExitMessage == eLuxMainMenuExit_StartGame ||
			mExitMessage == eLuxMainMenuExit_ContinueGame)
		{
			bZoomCamera = true;
			fFadeSpeed = mfMainFadeOutTimeSlow;
		}

		//Zoom in camera
		if(bZoomCamera &&mpBgCamera)
		{
			mpBgCamera->SetFOV(mfBgCamera_FOV*(1.0f-mfMenuFadeAlpha) + mfBgCamera_ZoomedFOV*mfMenuFadeAlpha);
		}

		//Fade or exit!
		if(mfMenuFadeAlpha < 1.0f)
		{
			mfMenuFadeAlpha += afTimeStep * (1.0f/fFadeSpeed);
			if(mfMenuFadeAlpha > 1.0f) mfMenuFadeAlpha =1.0f;

			//gpBase->mpEngine->GetSound()->GetLowLevel()->SetVolume(1-mfMenuFadeAlpha);
		}
		else
		{
			OnMenuExit();
		}
	}
	//////////////////////////
	//Normal operation
	else
	{
		if(mfMenuFadeAlpha > 0.0f)
		{
			mfMenuFadeAlpha -= afTimeStep * (1.0f/mfMainFadeInTime);
			if(mfMenuFadeAlpha < 0.0f) mfMenuFadeAlpha =0;
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::UpdateTopMenu(float afTimeStep)
{
	bool bAlphaChanged = false;

	////////////////
	// Visible
	if(mbTopMenuVisible && mbExiting==false)
	{
		if(mfTopMenuAlpha < 1.0f)
		{
			mfTopMenuAlpha += afTimeStep * (1.0f/mfTopMenuFadeInTime);
			if(mfTopMenuAlpha >1) mfTopMenuAlpha =1;
			bAlphaChanged = true;
		}
	}
	////////////////
	// Not Visible
	else
	{
		if(mfTopMenuAlpha > 0.0f)
		{
			mfTopMenuAlpha -= afTimeStep * (1.0f/mfTopMenuFadeOutTime);
			if(mfTopMenuAlpha <0) mfTopMenuAlpha =0;
			bAlphaChanged = true;
		}
	}


	////////////////////
	// Update widgets
	if(bAlphaChanged)
	{
		for(size_t i=0; i<mvTopMenuLabels.size(); ++i)
		{
			cWidgetLabel *pLabel = mvTopMenuLabels[i];

			cColor col = pLabel->GetDefaultFontColor();
			pLabel->SetDefaultFontColor(cColor(col.r, col.g, col.b, mfTopMenuAlpha));
		}
	}

	for (size_t i = 0; i < mvTopMenuLabels.size(); ++i)
	{
		cWidgetLabel *pLabel = mvTopMenuLabels[i];
		if (pLabel->GetName() == "Save")
		{
			if (pLabel->HasFocus())
			{
				mbFadeInDescription = true;
			}
			else
			{
				mbFadeInDescription = false;
			}
			break;
		}
	}


	if (mbFadeInDescription)
	{
		mfDescriptionAlpha += afTimeStep;
		if (mfDescriptionAlpha > 1.0f)
			mfDescriptionAlpha = 1.0f;
	}
	else
	{
		mfDescriptionAlpha -= afTimeStep;
		if (mfDescriptionAlpha < 0.5f)
			mfDescriptionAlpha = 0.5f;
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::SetTopMenuVisible(bool abVisible)
{
	mbTopMenuVisible = abVisible;
	if(abVisible)
	{
		mpGuiSet->SetDefaultFocusNavWidget(mvTopMenuLabels.front());
		mpGuiSet->SetFocusedWidget(mpLastFocusedItem);

	}
	else
	{
		for(size_t i=0; i<mvTopMenuLabels.size(); ++i)
		{
			iWidget* pLabel = mvTopMenuLabels[i];
			if(pLabel->HasFocus())
			{
				mpLastFocusedItem = pLabel;
				
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::CreateGui()
{
	/////////////////////////////
	//If Gui is already created, then delete all widget before creating them again
	if(mbGuiCreated)
	{
		mpGuiSet->DestroyAllWidgets();
	}

	//////////////////////////
	// Create the gui parts
	CreateTopMenuGui();
	

	//////////////////////
	// Create the windows
	for(size_t i=0; i<mvWindows.size(); ++i)
	{
		iLuxMainMenuWindow *pWindow = mvWindows[i];
		if(pWindow){
			pWindow->CreateGui();
			pWindow->SetActive(false);
		}
	}

	mbGuiCreated = true;
}

//-----------------------------------------------------------------------

void cLuxMainMenu::SetupTopMenuLabel(cWidgetLabel *apLabel)
{
	apLabel->SetTextAlign(eFontAlign_Center);
	apLabel->SetDefaultFontColor(cColor(1,1));
	apLabel->SetDefaultFontSize(mvTopMenuFontSize*mfTopMenuFontSizeMul);
	apLabel->SetDefaultFontType(mpFont);
	apLabel->AddCallback(eGuiMessage_MouseEnter,this, kGuiCallback(TopMenuTextMouseEnter));
	apLabel->AddCallback(eGuiMessage_MouseLeave,this, kGuiCallback(TopMenuTextMouseLeave));
	apLabel->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(TopMenuTextPress));
	apLabel->AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(TopMenuTextDraw));
	apLabel->SetAutogenerateSize(true);
	apLabel->SetPosition(apLabel->GetLocalPosition() - cVector3f(apLabel->GetSize().x*0.5f,0,0));

	/////////////////////////////////////////////////////
	// Set up navigation
	if (mvTopMenuLabels.empty() == false)
	{
		iWidget* pPrevLabel = mvTopMenuLabels.back();
		pPrevLabel->SetFocusNavigation(eUIArrow_Down, apLabel);

		apLabel->SetFocusNavigation(eUIArrow_Up, pPrevLabel);
	}
	mvTopMenuLabels.push_back(apLabel);
}

//-----------------------------------------------------------------------

void cLuxMainMenu::CreateTopMenuGui()
{
	///////////////////
	//Set up variables
	cWidgetLabel *pLabel =0;
	cWidgetLabel *pSaveLabel = 0;


	float fInvScreenRatio = mvScreenSize.y / mvScreenSize.x;
	float fWidthMul = fInvScreenRatio / (3.0f/4.0f);
	
	cVector2f vSize(mvScreenSize.x*0.15f, mvTopMenuFontSize.y);
	cVector3f vPos = gpBase->mpMapHandler->MapIsLoaded() ? mvTopMenuStartPosInGame : mvTopMenuStartPos;


	///////////////////
	//Init
	mvTopMenuLabels.clear();

	//TODO: Move these?
	mbTopMenuVisible = true;
	mfTopMenuAlpha = 1.0f;
	
	///////////////////
	//Create labels

	/////////////////////////////////////////////////
	// Set up font size to fit screen size
	// Determine longest label

	tWStringVec vLabels;
	vLabels.push_back(kTranslate("MainMenu", "Continue"));
	vLabels.push_back(kTranslate("MainMenu","Back To Game"));
	vLabels.push_back(kTranslate("MainMenu","Start Game"));
	vLabels.push_back(kTranslate("MainMenu","Load Game"));
	vLabels.push_back(kTranslate("MainMenu","Custom Map"));
	vLabels.push_back(kTranslate("MainMenu","Options"));
	vLabels.push_back(kTranslate("MainMenu","Change Profile"));
	vLabels.push_back(kTranslate("MainMenu","Exit"));
	vLabels.push_back(kTranslate("MainMenu","ExitToMainMenu"));
	vLabels.push_back(kTranslate("MainMenu","ExitAndSave"));

	//////////////////////
	// HARDMODE
	vLabels.push_back(kTranslate("MainMenu","Save")); // TRANSLATE THIS
	//////////////////////


	float fMaxLabelLength = 0;
	int lLongestStringSize;
	// Create a temp label to be able to access font data
	pLabel = mpGuiSet->CreateWidgetLabel();
	for(size_t i=0;i<vLabels.size();++i)
	{
		const tWString& sLabel = vLabels[i];

		float fLabelLength = pLabel->GetDefaultFontType()->GetLength(mvTopMenuFontSize, sLabel.c_str());

		if(fMaxLabelLength < fLabelLength)
		{
			fMaxLabelLength = fLabelLength;
			lLongestStringSize = (int)sLabel.size();
		}
	}
	mpGuiSet->DestroyWidget(pLabel);

	float fSafeMaxLabelLength = mvScreenSize.x * 0.35f;
	mfTopMenuFontSizeMul = 1.0f;

	///////////////////////////////////////////////////////////
	// If longest label is longer than safe length, compute new length
	if(fMaxLabelLength>fSafeMaxLabelLength)
	{
		float fCharWidth = fSafeMaxLabelLength/lLongestStringSize;
		mfTopMenuFontSizeMul = fCharWidth/mvTopMenuFontSize.x;

		mfTopMenuFontSizeMul = cMath::Clamp(mfTopMenuFontSizeMul, 0.7f, mfTopMenuFontSizeMul);
	}

	float fRowAdd = mvTopMenuFontSize.y*mfTopMenuFontSizeMul*1.3f;

	///////////////
	//Continue
	if(	gpBase->mbPTestActivated==false &&
		gpBase->mpMapHandler->MapIsLoaded()==false && gpBase->mpSaveHandler->SaveFileExists()) 
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Continue"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressContinue));
		SetupTopMenuLabel(pLabel);
		vPos.y += fRowAdd;
	}
	
	///////////////
	//Start game
	if(gpBase->mpMapHandler->MapIsLoaded()) 
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Back To Game"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressBackToGame));
	}
	else
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Start Game"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressStartGame));
	}
	SetupTopMenuLabel(pLabel);
	vPos.y += fRowAdd;

	///////////////
	//Load game
	if(	gpBase->mbPTestActivated==false &&
		gpBase->mpSaveHandler->SaveFileExists() && gpBase->mpMapHandler->MapIsLoaded()==false)
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Load Game"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this,kGuiCallback(PressLoadGame));
		SetupTopMenuLabel(pLabel);
		vPos.y += fRowAdd;
	}

	///////////////
	// HARDMODE
	// Save game
	if (gpBase->mbPTestActivated == false && 
		gpBase->mbHardMode && 
		gpBase->mpMapHandler->MapIsLoaded() == true)
	{
		////////////////////////////
		// Get translation and insert cost
		pLabel = mpGuiSet->CreateWidgetLabel(vPos, 0, kTranslate("MainMenu", "Save"), NULL, "Save");
		pLabel->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(PressSaveGame));
		pSaveLabel = pLabel;
		
		SetupTopMenuLabel(pLabel);

																					  
		////////////////////////////
		// Set enabled to false if player does not have enough resources  
		pLabel->SetEnabled(gpBase->mpPlayer->GetTinderboxes() >= 4);

		////////////////////////////
		// Save cost label
		tWString sText = kTranslate("MainMenu", "HardModeSaveCost");
		size_t lNumIndex = sText.find(L"#");
		
		if (lNumIndex != std::wstring::npos)
		{
			tWString sSubString = sText.substr(lNumIndex + 1, sText.size() - lNumIndex);
			sText = sText.substr(0, lNumIndex);
			sText.replace(lNumIndex, gsHardMode_SaveCostString.size(), gsHardMode_SaveCostString);
			sText += sSubString;
		}

		cVector3f vSaveDescriptionPosition = 0;
		vSaveDescriptionPosition.x = (mvScreenSize.x / 16.0f);
		vSaveDescriptionPosition.x = pLabel->GetGlobalPosition().x + pLabel->GetSize().x + 100;
		vSaveDescriptionPosition.y = mvTopMenuStartPosInGame.y - (2.5f * fRowAdd);
		vSaveDescriptionPosition.z = 2.0f;

		vSaveDescriptionPosition.x = mvTopMenuStartPosInGame.x;

		mpSaveCost = mpGuiSet->CreateWidgetLabel(vSaveDescriptionPosition, 0, sText, NULL, "SaveDescription");
		mpSaveCost->SetTextAlign(eFontAlign_Center);
		mpSaveCost->SetDefaultFontColor(cColor(0.5f, 0));
		mpSaveCost->SetDefaultFontSize(mvTopMenuFontSize*mfTopMenuFontSizeMul * 0.85f);
		mpSaveCost->SetDefaultFontType(mpFont);
		mpSaveCost->SetAutogenerateSize(true);
		mpSaveCost->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(HardModeTextDraw));
		mpSaveCost->SetAutogenerateSize(true);
		mpSaveCost->SetPosition(mpSaveCost->GetLocalPosition() - cVector3f(mpSaveCost->GetSize().x*0.5f, 0, 0));

		vSaveDescriptionPosition.y += fRowAdd * 0.65f;

		//////////////////////////////////
		// Tinder inventory label
		sText = kTranslate("MainMenu", "HardModeTinderboxInInventory");
		lNumIndex = sText.find(L"#");

		if (lNumIndex != std::wstring::npos)
		{
#if MAC_OS || LINUX
            tWString sNumTinderboxes = tWString(LongToWString(static_cast<long long>(gpBase->mpPlayer->GetTinderboxes())));
#else
            tWString sNumTinderboxes = tWString(std::to_wstring(static_cast<long long>(gpBase->mpPlayer->GetTinderboxes())));
#endif
            
			tWString sSubString = sText.substr(lNumIndex + 1, sText.size() - lNumIndex);
			sText = sText.substr(0, lNumIndex);
			sText.replace(lNumIndex, sNumTinderboxes.size(), sNumTinderboxes);
			sText += sSubString;
		}

		mpNumTinderboxes = mpGuiSet->CreateWidgetLabel(vSaveDescriptionPosition, 0, sText, NULL, "NumTinderboxes");
		mpNumTinderboxes->SetTextAlign(eFontAlign_Center);
		mpNumTinderboxes->SetDefaultFontColor(cColor(0.5f, 0));
		mpNumTinderboxes->SetDefaultFontSize(mvTopMenuFontSize*mfTopMenuFontSizeMul * 0.75f);
		mpNumTinderboxes->SetDefaultFontType(mpFont);
		mpNumTinderboxes->SetAutogenerateSize(true);
		mpNumTinderboxes->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(HardModeTextDraw));
		mpNumTinderboxes->SetAutogenerateSize(true);
		mpNumTinderboxes->SetPosition(mpNumTinderboxes->GetLocalPosition() - cVector3f(mpNumTinderboxes->GetSize().x*0.5f, 0, 0));

		mbFadeInDescription = true;
		mfDescriptionAlpha = 0.0f;
		vPos.y += fRowAdd;
	}

	///////////////
	//Custom map
	
	#ifndef LUX_DEMO_VERSION
		if(	gpBase->mbPTestActivated==false &&
			gpBase->mpMapHandler->MapIsLoaded()==false) 
		{
			pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Custom Map"));
			pLabel->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(PressCustomStory));
			SetupTopMenuLabel(pLabel);
			vPos.y += fRowAdd;
		}
	#endif
	

	///////////////
	//Options
	pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Options"));
	pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressOptions));
	SetupTopMenuLabel(pLabel);
	vPos.y += fRowAdd;

	///////////////
	//Change profile
	if(gpBase->mpMapHandler->MapIsLoaded()==false) 
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Change Profile"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressChangeProfile));
		SetupTopMenuLabel(pLabel);
		vPos.y += fRowAdd;
	}

	///////////////
	//Exit
	if(gpBase->mpMapHandler->MapIsLoaded()==false || gpBase->mbExitMenuDirectly) 
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Exit"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressExit));
	}
	else
	{
		pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","ExitToMainMenu"));
		pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressExitToMainMenu));

		if(gpBase->mbPTestActivated==false && gpBase->mpPlayer->IsDead()==false && gpBase->mbHardMode == false)
		{
			// Set up label right above
			SetupTopMenuLabel(pLabel);
			vPos.y += fRowAdd;

			pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","ExitAndSave"));
			pLabel->AddCallback(eGuiMessage_MouseDown,this, kGuiCallback(PressExitAndSave));

		}
	}
	SetupTopMenuLabel(pLabel);
	vPos.y += fRowAdd;

	/////////////////////////////////
	// Link first and last labels nav
	mvTopMenuLabels.front()->SetFocusNavigation(eUIArrow_Up, mvTopMenuLabels.back());
	mvTopMenuLabels.back()->SetFocusNavigation(eUIArrow_Down, mvTopMenuLabels.front());

	mpLastFocusedItem = mvTopMenuLabels.front();
	mpGuiSet->SetDefaultFocusNavWidget(mpLastFocusedItem);
	mpGuiSet->SetFocusedWidget(mpLastFocusedItem);

	////////////////////////////////
	// HARDMODE - Disable focus when save is disabled
	if (pSaveLabel && pSaveLabel->IsEnabled() == false)
	{
		iWidget* pWidgetAbove = pSaveLabel->GetFocusNavigation(eUIArrow_Up);
		iWidget* pWidgetBelow = pSaveLabel->GetFocusNavigation(eUIArrow_Down);

		pWidgetAbove->SetFocusNavigation(eUIArrow_Down, pWidgetBelow);
		pWidgetBelow->SetFocusNavigation(eUIArrow_Up, pWidgetAbove);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::CreateBackground()
{
	////////////////////////////
	// A map is loaded, use a screen shot as background.
	if(gpBase->mpMapHandler->MapIsLoaded())
	{
		CreateScreenTextures();
		RenderBlurTexture();
	}
	////////////////////////////
	// No map is loaded, create scene.
	else
	{
		mpLogoGfx = mpGui->CreateGfxImage(gpBase->mpMenuCfg->GetString("Main", "MenuLogo", "menu_logo.tga"), eGuiMaterial_Alpha);
		if(mpLogoGfx)
		{
			float fLogoAspect = mpLogoGfx->GetActiveSize().x/mpLogoGfx->GetActiveSize().y;
			mvLogoSize.x = mvLogoSize.y*fLogoAspect;
		}

		/////////////////////////////////////
		// World set up (from menu config)
		tString sBGWorldFile = gpBase->mpMenuCfg->GetString("Main", "BGScene", "");
		if(gbDebug_SkipBGScene) sBGWorldFile = "";
		
		if(sBGWorldFile != "")	mpBgWorld = mpScene->LoadWorld(sBGWorldFile, 0);
		else					mpBgWorld = NULL;

		mpBgCamera = mpScene->CreateCamera(eCameraMoveMode_Fly);	
		mpBgCamera->SetRotateMode(eCameraRotateMode_Matrix);

		//Camera Settings
		cVector2f vScreenSize = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat();
		float mfAspect = vScreenSize.x / vScreenSize.y;
		mpBgCamera->SetFOV(mfBgCamera_FOV);
		mpBgCamera->SetAspect(mfAspect);


		cMatrixf mtxCamMatrix = cMatrixf::Identity;

		if(mpBgWorld)
		{
			cStartPosEntity* pStartPos = mpBgWorld->GetStartPosEntity("CameraPos");
			if(pStartPos)
				mtxCamMatrix = pStartPos->GetWorldMatrix();
			else
				Error("Could not find start pos entity CameraPos in '%s'\n", sBGWorldFile.c_str());
		}
		
		mpBgCamera->SetRotationMatrix(cMath::MatrixInverse(mtxCamMatrix.GetRotation()));
		mpBgCamera->SetPosition(mtxCamMatrix.GetTranslation());

		if(mpBgWorld) mpViewport->SetWorld(mpBgWorld);
		mpViewport->SetCamera(mpBgCamera);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::CreateScreenTextures()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();
	cVector3l vTexSize = pLowGfx->GetScreenSizeInt();
	vTexSize.z = 0;

	mpScreenTexture = mpGraphics->CreateTexture("Screen",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);
	mpScreenTexture->SetWrapSTR(eTextureWrap_ClampToEdge);

	mpScreenBlurTexture = mpGraphics->CreateTexture("ScreenBlur",eTextureType_Rect,eTextureUsage_RenderTarget);
	mpScreenBlurTexture->CreateFromRawData(vTexSize,ePixelFormat_RGBA,NULL);
	
	mpScreenGfx = mpGui->CreateGfxTexture(mpScreenTexture,false,eGuiMaterial_Diffuse);
	mpScreenBlurGfx = mpGui->CreateGfxTexture(mpScreenBlurTexture,false,eGuiMaterial_Alpha);
}

//-----------------------------------------------------------------------

void cLuxMainMenu::RenderBlur(iTexture *apInputTexture, iTexture *apTempTexture, iFrameBuffer **apBlurBuffers)
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();

	//Draw horizontal blur to temp from screen
	mpBlurProgram[0]->Bind();
	pLowGfx->SetCurrentFrameBuffer(apBlurBuffers[0]);

	pLowGfx->SetTexture(0,apInputTexture);

	pLowGfx->DrawQuad(0,mvScreenSize,cVector2f(0, mvScreenSize.y),cVector2f(mvScreenSize.x,0),cColor(1,1));
	mpBlurProgram[0]->UnBind();

	//Draw vertical blur to final from temp
	mpBlurProgram[1]->Bind();
	pLowGfx->SetCurrentFrameBuffer(apBlurBuffers[1]);

	pLowGfx->SetTexture(0,apTempTexture);

	pLowGfx->DrawQuad(0,mvScreenSize,cVector2f(0, mvScreenSize.y),cVector2f(mvScreenSize.x,0),cColor(1,1));
	mpBlurProgram[1]->UnBind();
}

void cLuxMainMenu::RenderBlurTexture()
{
	iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();

	//////////////////////////////
	// Create frame buffers
	iTexture* pTempBlurTexture = mpGraphics->CreateTexture("TempBlur",eTextureType_Rect,eTextureUsage_RenderTarget);
	pTempBlurTexture->CreateFromRawData(cVector3l((int)mvScreenSize.x, (int)mvScreenSize.y,0),ePixelFormat_RGBA,NULL);
	pTempBlurTexture->SetWrapSTR(eTextureWrap_ClampToEdge);
	
	iFrameBuffer *pBlurBuffer[2];
	for(int i=0; i<2; ++i)
	{
		pBlurBuffer[i] = mpGraphics->CreateFrameBuffer("MainMenuBlurBuffer"+cString::ToString(i));
		if(i==0) pBlurBuffer[i]->SetTexture2D(0,pTempBlurTexture);
		else	pBlurBuffer[i]->SetTexture2D(0,mpScreenBlurTexture);

		pBlurBuffer[i]->CompileAndValidate();
	}
	
	//////////////////////////////
	// Render

	//Set up main states
	pLowGfx->SetBlendActive(false);
	pLowGfx->SetDepthTestActive(false);
	pLowGfx->SetDepthWriteActive(false);
	
	pLowGfx->SetOrthoProjection(mvScreenSize,-1000,1000);
	pLowGfx->SetIdentityMatrix(eMatrix_ModelView);

	//Copy screen to screen texture
	pLowGfx->CopyFrameBufferToTexure(mpScreenTexture,0,pLowGfx->GetScreenSizeInt(),0);
	
	RenderBlur(mpScreenTexture,pTempBlurTexture,pBlurBuffer);

	for(int i=0; i<6; ++i)
		RenderBlur(mpScreenBlurTexture,pTempBlurTexture,pBlurBuffer);

	///////////////////////
	// Exit

	//Render states
	pLowGfx->SetTexture(0,NULL);
	pLowGfx->SetCurrentFrameBuffer(NULL);
	pLowGfx->SetDepthTestActive(true);

	//Flush the rendering
	pLowGfx->FlushRendering();
	pLowGfx->WaitAndFinishRendering();
	
	//Destroy data
	mpGraphics->DestroyTexture(pTempBlurTexture);
	for(int i=0; i<2; ++i)
	{
		mpGraphics->DestroyFrameBuffer(pBlurBuffer[i]);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu::DestroyBackground()
{
	///////////////////////////
	//Back ground world is loaded
	if(mpBgWorld && mpBgCamera)
	{
		if(mpLogoGfx) mpGui->DestroyGfx(mpLogoGfx);

		mpScene->DestroyCamera(mpBgCamera);
		mpScene->DestroyWorld(mpBgWorld);

		mpViewport->SetWorld(NULL);
		mpViewport->SetCamera(NULL);

		mpLogoGfx = NULL;
		mpBgCamera = NULL;
		mpBgWorld = NULL;
	}
	///////////////////////////
	//No background is loaded.
	else
	{
		if(mpScreenGfx) mpGui->DestroyGfx(mpScreenGfx);
		if(mpScreenTexture) mpGraphics->DestroyTexture(mpScreenTexture);
		if(mpScreenBlurGfx) mpGui->DestroyGfx(mpScreenBlurGfx);
		if(mpScreenBlurTexture) mpGraphics->DestroyTexture(mpScreenBlurTexture);

		mpScreenGfx = NULL;
		mpScreenTexture = NULL;
		mpScreenBlurTexture = NULL;
		mpScreenBlurGfx = NULL;
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// GUI CALLBACKS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxMainMenu::TopMenuTextMouseEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	if (mbTopMenuVisible) mpGuiSet->SetFocusedWidget(apWidget);
	
	//apWidget->SetDefaultFontColor(cColor(232.0f/255.0f, 201.0f/255.0f, 28.0f/255.0f, mfTopMenuAlpha));

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, TopMenuTextMouseEnter);

bool cLuxMainMenu::TopMenuTextMouseLeave(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible)
	{
		if(mpGuiSet->GetFocusedWidget()==apWidget)
			mpGuiSet->SetFocusedWidget(NULL);
	}
	//apWidget->SetDefaultFontColor(cColor(1.0f, mfTopMenuAlpha));
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, TopMenuTextMouseLeave);

bool cLuxMainMenu::TopMenuTextPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal&eUIButton_Primary)
		return apWidget->ProcessMessage(eGuiMessage_MouseDown, aData);
	else if(aData.mlVal&eUIButton_Secondary)
	{
		ExitPressed();
		return true;
	}
	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, TopMenuTextPress);

bool cLuxMainMenu::TopMenuTextDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget->HasFocus())
		apWidget->SetDefaultFontColor(cColor(232.0f/255.0f, 201.0f/255.0f, 28.0f/255.0f, mfTopMenuAlpha));
	else
		apWidget->SetDefaultFontColor(cColor(1.0f, mfTopMenuAlpha));

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, TopMenuTextDraw);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressContinue(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetTopMenuVisible(false);
	
	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","Continue old game?"),
										kTranslate("MainMenu","Yes"), kTranslate("MainMenu","No"),
										this,
										kGuiCallback(ClickedContinuePopup));
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressContinue)


bool cLuxMainMenu::ClickedContinuePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bStartGame = aData.mlVal ==0 ? true : false;
	mpGuiSet->SetDrawFocus(false);

	if(bStartGame)
	{
		ExitMenu(eLuxMainMenuExit_ContinueGame);
	}
	else
	{
		SetTopMenuVisible(true);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedContinuePopup)

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressStartGame(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;
	
	/////////////
	//HARDMODE

    if (gpBase->mbAllowHardmode == true && gpBase->mpCustomStory == 0)
	{
		SetWindowActive(eLuxMainMenuWindow_StartGame);
		gpBase->mbHardMode = false;
		return true;
	}
	else
	{
		SetTopMenuVisible(false);
	
		mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
		cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""), kTranslate("MainMenu", "Start a new game?"),
			kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
			this,
			kGuiCallback(ClickedStartGamePopup));

		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
		pPopUp->SetKillOnEscapeKey(false);

		return true;
	}

}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressStartGame);

bool cLuxMainMenu::ClickedStartGamePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bStartGame = aData.mlVal ==0 ? true : false;
	mpGuiSet->SetDrawFocus(false);
	
	if(bStartGame)
	{
		gpBase->SetCustomStory(NULL);
		ExitMenu(eLuxMainMenuExit_StartGame);
	}
	else
	{
		SetTopMenuVisible(true);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedStartGamePopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressBackToGame(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	ExitMenu(eLuxMainMenuExit_ReturnToGame);
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressBackToGame);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressLoadGame(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetWindowActive(eLuxMainMenuWindow_LoadGame);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressLoadGame);

bool cLuxMainMenu::ClickedLoadGamePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedLoadGamePopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressCustomStory(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetWindowActive(eLuxMainMenuWindow_CustomStoryList);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressCustomStory);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressExit(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetTopMenuVisible(false);
	
	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu", "Sure you want to quit?"),
									kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
									this,
									kGuiCallback(ClickedExitPopup));
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressExit);

bool cLuxMainMenu::ClickedExitPopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bExit = aData.mlVal ==0 ? true : false;
	mpGuiSet->SetDrawFocus(false);

	if(bExit)
	{
		ExitMenu(eLuxMainMenuExit_QuitGame);
	}
	else
	{
		SetTopMenuVisible(true);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedExitPopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressExitToMainMenu(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetTopMenuVisible(false);

	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu", "Sure you want to exit to main menu?"),
																kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
																this,
																kGuiCallback(ClickedExitToMainMenuPopup));
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);
		
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressExitToMainMenu);

bool cLuxMainMenu::ClickedExitToMainMenuPopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bExit = aData.mlVal ==0 ? true : false;
	mpGuiSet->SetDrawFocus(false);

	if(bExit)
	{
		ExitMenu(eLuxMainMenuExit_QuitToMenu);
	}
	else
	{
		SetTopMenuVisible(true);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedExitToMainMenuPopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressExitAndSave(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetTopMenuVisible(false);
	
	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu", "Sure you want to exit and save?"),
		kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
		this,
		kGuiCallback(ClickedExitAndSavePopup));
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressExitAndSave);

bool cLuxMainMenu::ClickedExitAndSavePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bExit = aData.mlVal ==0 ? true : false;
	mpGuiSet->SetDrawFocus(false);

	if(bExit)
	{
		ExitMenu(eLuxMainMenuExit_QuitAndSave);
	}
	else
	{
		SetTopMenuVisible(true);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedExitAndSavePopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressChangeProfile(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetWindowActive(eLuxMainMenuWindow_Profiles);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressChangeProfile);

//-----------------------------------------------------------------------

bool cLuxMainMenu::PressOptions(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbTopMenuVisible==false)
		return true;

	SetWindowActive(eLuxMainMenuWindow_Options);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressOptions);

//-----------------------------------------------------------------------

//////////////////////
// HARDMODE
bool cLuxMainMenu::PressSaveGame(iWidget* apWidget, const cGuiMessageData& aData)
{
	if (mbTopMenuVisible == false)
		return true;

	SetTopMenuVisible(false);

	////////////////////////////
	// Get translation and insert cost
	tWString sText = kTranslate("MainMenu", "HardModeSavePopup");
	size_t lNumIndex = sText.find(L"#");
	sText.replace(lNumIndex, gsHardMode_SaveCostString.size(), gsHardMode_SaveCostString);

	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());
	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""), sText,
		kTranslate("MainMenu", "Save"), kTranslate("Global", "Cancel"),
		this,
		kGuiCallback(ClickedSaveGamePopup));
	
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, PressSaveGame);

//-----------------------------------------------------------------------

bool cLuxMainMenu::ClickedSaveGamePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bSave = aData.mlVal == 0 ? true : false;
	
	mpGuiSet->SetDrawFocus(false);
	SetTopMenuVisible(true);

	if (bSave == false)
		return true;

	///////////////////////////
	// Get num tinderboxes
	int lNumTinderboxes = gpBase->mpPlayer->GetTinderboxes();

	///////////////////////////
	// decrement tinderboxes
	if (lNumTinderboxes < 4)
		return true;

	gpBase->mpPlayer->SetTinderboxes(cMath::Max(0, lNumTinderboxes - 4));

	///////////////////////////
	// Save game
	gpBase->mpSaveHandler->HardModeSave();

	//////////////////////////
	// Update Labels
	tWString sText = kTranslate("MainMenu", "HardModeTinderboxInInventory");
	size_t lNumIndex = sText.find(L"#");

	if (lNumIndex != std::wstring::npos)
	{
#if MAC_OS || LINUX
        tWString sNumTinderboxes = tWString(LongToWString(static_cast<long long>(gpBase->mpPlayer->GetTinderboxes())));

#else
        tWString sNumTinderboxes = tWString(std::to_wstring(static_cast<long long>(gpBase->mpPlayer->GetTinderboxes())));

#endif
		tWString sSubString = sText.substr(lNumIndex + 1, sText.size() - lNumIndex);
		sText = sText.substr(0, lNumIndex);
		sText.replace(lNumIndex, sNumTinderboxes.size(), sNumTinderboxes);
		sText += sSubString;
	}
	cVector3f vGlobalPos = mpNumTinderboxes->GetGlobalPosition();
	vGlobalPos.x = mvTopMenuStartPosInGame.x;
	mpNumTinderboxes->SetText(sText);
	mpNumTinderboxes->SetGlobalPosition(vGlobalPos);
	mpNumTinderboxes->SetPosition(mpNumTinderboxes->GetLocalPosition() - cVector3f(mpNumTinderboxes->GetSize().x*0.5f, 0, 0));

	///////////////////////////
	// Get save label
#if MAC_OS || LINUX
    cWidgetLabel* pSaveLabel = 0;
#else
    cWidgetLabel* pSaveLabel = nullptr;
#endif
	for (size_t i = 0; i < mvTopMenuLabels.size(); ++i)
	{
		if (mvTopMenuLabels[i]->GetName() == "Save")
		{
			pSaveLabel = mvTopMenuLabels[i];
			break;
		}
	}

	if (pSaveLabel)
	{
		///////////////////////////
		// Set save label disabled
		pSaveLabel->SetEnabled(false);

		iWidget* pWidgetAbove = pSaveLabel->GetFocusNavigation(eUIArrow_Up);
		iWidget* pWidgetBelow = pSaveLabel->GetFocusNavigation(eUIArrow_Down);

		pWidgetAbove->SetFocusNavigation(eUIArrow_Down, pWidgetBelow);
		pWidgetBelow->SetFocusNavigation(eUIArrow_Up, pWidgetAbove);

	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, ClickedSaveGamePopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu::HardModeTextDraw(iWidget* apWidget, const cGuiMessageData& aData)
{
	apWidget->SetDefaultFontColor(cColor(mfDescriptionAlpha, mfTopMenuAlpha));
	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu, HardModeTextDraw);

