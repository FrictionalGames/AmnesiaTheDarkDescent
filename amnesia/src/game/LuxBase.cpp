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

#include "LuxBase.h"

#include "LuxInputHandler.h"

#include "LuxMapHandler.h"
#include "LuxMapHelper.h"
#include "LuxMap.h"

#include "LuxDebugHandler.h"
#include "LuxSaveHandler.h"
#include "LuxScriptHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxEffectRenderer.h"
#include "LuxMusicHandler.h"
#include "LuxMessageHandler.h"
#include "LuxEffectHandler.h"
#include "LuxJournal.h"
#include "LuxCompletionCountHandler.h"
#include "LuxGlobalDataHandler.h"
#include "LuxHintHandler.h"
#include "LuxConfigHandler.h"
#include "LuxPostEffects.h"
#include "LuxInsanityHandler.h"
#include "LuxProgressLogHandler.h"
#include "LuxLoadScreenHandler.h"

#include "LuxInventory.h"

#include "LuxCredits.h"

#include "LuxDemoEnd.h"

#include "LuxPreMenu.h"

#include "LuxMainMenu.h"

#include "LuxPlayer.h"

#include "LuxStaticProp.h"

#include "LuxProp_Object.h"
#include "LuxProp_SwingDoor.h"
#include "LuxProp_Lever.h"
#include "LuxProp_Wheel.h"
#include "LuxProp_Lamp.h"
#include "LuxProp_Photocell.h"
#include "LuxProp_MoveObject.h"
#include "LuxProp_Item.h"
#include "LuxProp_Chest.h"
#include "LuxProp_LevelDoor.h"
#include "LuxProp_Button.h"
#include "LuxProp_OilBarrel.h"
#include "LuxProp_EmotionStone.h"
#include "LuxProp_NPC.h"
#include "LuxProp_MultiSlider.h"

#include "LuxCritter_Bug.h"
#include "LuxCritter_Spider.h"

#include "LuxAreaNodes.h"
#include "LuxArea_Script.h"
#include "LuxArea_Flashback.h"
#include "LuxArea_Ladder.h"
#include "LuxArea_Liquid.h"
#include "LuxArea_Sticky.h"
#include "LuxArea_Insanity.h"
#include "LuxArea_Examine.h"
#include "LuxArea_Sign.h"
#include "LuxArea_Rope.h"
#include "LuxArea_SlimeDamage.h"

#include "LuxEnemy_Grunt.h"
#include "LuxEnemy_WaterLurker.h"
#include "LuxEnemy_ManPig.h"

#include "LuxCommentaryIcon.h"
#include "LuxAchievementHandler.h"



//////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define kCurrentVersion_Main 1
#define kCurrentVersion_Minor 4

//-----------------------------------------------------------------------

extern tString gsSerialKey;

//-----------------------------------------------------------------------

eLuxAxis StringToAxis(const tString &asAxis)
{
	tString sLowAxis = cString::ToLowerCase(asAxis);

	if(sLowAxis == "x") return eLuxAxis_X;
	if(sLowAxis == "y") return eLuxAxis_Y;
	if(sLowAxis == "z") return eLuxAxis_Z;

	Error("Axis '%s' does not exist!\n", asAxis.c_str());

	return eLuxAxis_X;
}

//-----------------------------------------------------------------------

void LuxCalcGuiSetOffset(const cVector2f &avVirtualSizeIn, const cVector2f& avScreenSize, cVector2f& avOutSize, cVector2f & avOutOffset)
{
	
	float fWantedRatio = avVirtualSizeIn.x / avVirtualSizeIn.y;
	float fScreenRatio = avScreenSize.x / avScreenSize.y;
	//float fAddY = avVirtualSizeIn.y * (fScreenRatio-fWantedRatio);

	if(fScreenRatio >= (4.0f / 3.0f)-0.001f)
	{
		float fAddX = avVirtualSizeIn.x * (fScreenRatio-fWantedRatio); //The "left overs" on both sides
	//avOutSize.y = avVirtualSizeIn.y + fAddY;

		avOutSize.x = avVirtualSizeIn.x + fAddX;
		avOutSize.y = avVirtualSizeIn.y;

		avOutOffset.x = fAddX*0.5f;
		avOutOffset.y =0;
	}
	else
	{
		avOutOffset =0;
		avOutSize.x = avVirtualSizeIn.x;
		avOutSize.y = avVirtualSizeIn.y;
	}
}

void LuxCalcGuiSetScreenOffset(const cVector2f &avVirtualSizeIn, cVector2f& avOutSize, cVector2f & avOutOffset)
{
	LuxCalcGuiSetOffset(avVirtualSizeIn, gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetScreenSizeFloat(), avOutSize, avOutOffset);
}

//-----------------------------------------------------------------------

void ProgLog(eLuxProgressLogLevel aLevel, const tString& asMessage)
{
	gpBase->mpProgressLogHandler->AddLog(aLevel, asMessage);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CUSTOM STORY SETTINGS CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxCustomStorySettings::cLuxCustomStorySettings()
{
}

cLuxCustomStorySettings::cLuxCustomStorySettings(cLuxCustomStorySettings* apStory)
{
	msStoryRootFolder = apStory->msStoryRootFolder;

	msName = apStory->msName;
	msAuthor = apStory->msAuthor;

	msDefaultExtraLanguage = apStory->msDefaultExtraLanguage;
	msExtraLangFilePrefix = apStory->msExtraLangFilePrefix;
	
	msMapsFolder = apStory->msMapsFolder;
	msStartMap = apStory->msStartMap;
	msStartPos = apStory->msStartPos;
}

cLuxCustomStorySettings::~cLuxCustomStorySettings()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CUSTOM STORY SETTINGS PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxCustomStorySettings::CreateFromPath(const tWString& asPath)
{
	/////////////////////////////////////////////////////////////////
	// We must make sure this is a valid custom story: 
	//	- first checking if its a valid path,
	//	- then checking the custom_story_settings.cfg file
	//	- then checking if the data in the cfg file at least exists
	if(cPlatform::FolderExists(asPath)==false)
		return false;

	tString sErrorMsg;

	tWString sFile = cString::AddSlashAtEndW(asPath) + _W("custom_story_settings.cfg");
	bool bValid = false;

	cConfigFile* pCustomStoryCfg = hplNew(cConfigFile,(sFile));
	bValid = pCustomStoryCfg->Load();
	if(bValid)
	{
		msStoryRootFolder = cString::AddSlashAtEndW(asPath);

		msMapsFolder = cString::To8Char(msStoryRootFolder) + pCustomStoryCfg->GetString("Main", "MapsFolder", "maps");
		msMapsFolder = cString::AddSlashAtEnd(msMapsFolder);

		msStartMap = pCustomStoryCfg->GetString("Main", "StartMap", "");
		msStartPos = pCustomStoryCfg->GetString("Main", "StartPos", "");

		tWString sStartMapPath = cString::To16Char(msMapsFolder) + cString::To16Char(msStartMap);

		if(msStartMap=="" || 
			cPlatform::FileExists(sStartMapPath)==false)
		{
			sErrorMsg = "could not find start map";
			bValid = false;
		}

		msName = cString::To16Char(pCustomStoryCfg->GetString("Main", "Name", ""));
		if(msName==_W(""))
			msName = kTranslate("CustomStory", "NoName");
		msAuthor = cString::To16Char(pCustomStoryCfg->GetString("Main", "Author", ""));
		if(msAuthor==_W(""))
			msAuthor = kTranslate("CustomStory", "NoAuthor");
		this->msImgFile = pCustomStoryCfg->GetString("Main", "ImgFile", "");

		msExtraLangFilePrefix = pCustomStoryCfg->GetString("Main", "ExtraLangFilePrefix", "extra_");
		msDefaultExtraLanguage = pCustomStoryCfg->GetString("Main", "DefaultExtraLangFile", gpBase->msDefaultGameLanguage);
	}
	else
	{
		sErrorMsg = "could not find custom_story_settings.cfg file or it was invalid";
	}

	if(bValid==false)
		Log("Error creating custom story from path \"%ls\" : %s.\n", asPath.c_str(), sErrorMsg.c_str());

	hplDelete(pCustomStoryCfg);

	return bValid;
}

//-----------------------------------------------------------------------

void cLuxCustomStorySettings::SetActive()
{
	cLuxCustomStorySettings* pStory = hplNew(cLuxCustomStorySettings,(this));
	
	gpBase->SetCustomStory(pStory);
}

//-----------------------------------------------------------------------

bool cLuxCustomStorySettings::StartGame()
{
	gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);

	gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

	gpBase->mpEngine->GetUpdater()->SetContainer("Default");
	gpBase->mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);

	gpBase->mpProgressLogHandler->CreateAndResetLogFile();

	return gpBase->StartGame(msStartMap, msMapsFolder, msStartPos);
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PTEST FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static inline tString DecryptString(const tString &asEncStr)
{
	size_t lBuffPos=0;
	tString sOutStr;

	cBinaryBuffer keyBuff;
	keyBuff.AddInt32(0xc3af2528);keyBuff.AddInt32(0xd4152761);

	keyBuff.SetPos(0);
	for(size_t i=0; i<asEncStr.size(); ++i)
	{
		sOutStr +=  asEncStr[i] ^ keyBuff.GetChar();
		if(++lBuffPos >= keyBuff.GetSize()) keyBuff.SetPos(0);
	}

	return sOutStr;
}

//-----------------------------------------------------------------------

static inline unsigned int GetFileCRC(const tString& asFilePath, unsigned int alKey)
{
	cBinaryBuffer buff;
	if(buff.Load(cString::To16Char(asFilePath))==false) return 0;

	return buff.GetCRC(alKey, 0);
}

static inline unsigned int GetFileCRC(const tWString& asFilePath, unsigned int alKey)
{
	cBinaryBuffer buff;
	if(buff.Load(asFilePath)==false) return 0;

	return buff.GetCRC(alKey, 0);
}

//-----------------------------------------------------------------------

static unsigned char gv_main_init_str[27] = {0x4B, 0x4A, 0xC1, 0xA5, 0x8, 0x40, 0x3A, 0xA4, 0x5C, 0x4D, 0x5B, 0x5C, 0x77, 0x45, 0x49, 0x41, 0x46, 0x77, 0x41, 0x46, 0x41, 0x5C, 0x6, 0x4B, 0x4E, 0x4F, 0};
static unsigned char gv_error_mess_str[29] = {0x6C, 0x4A, 0x8F, 0xAD, 0xE, 0x53, 0x35, 0xB9, 0x4D, 0x5B, 0x5B, 0x8, 0x5F, 0x41, 0x5C, 0x40, 0x8, 0x5C, 0x40, 0x4D, 0x8, 0x4E, 0x41, 0x44, 0x4D, 0x5B, 0x9, 0x9, 0};
static unsigned char gv_start_map_str[13] = {0x18, 0x14, 0xF0, 0xA0, 0x4, 0x4B, 0x79, 0xA7, 0x6, 0x45, 0x49, 0x58, 0};
static unsigned char gv_start_folder_str[12] = {0x45, 0x44, 0xDF, 0xB0, 0x4E, 0x57, 0x61, 0xB1, 0x5B, 0x5C, 0x7, 0};
static unsigned char gv_start_pos_str[18] = {0x78, 0x49, 0xCE, 0xBA, 0x4, 0x55, 0x46, 0xA0, 0x49, 0x5A, 0x5C, 0x69, 0x5A, 0x4D, 0x49, 0x77, 0x19, 0};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxBase::cLuxBase()
{
	///////////////////////////////
	// Init pointers
	mpEngine = NULL;

	mpMainConfig = NULL;
	mpUserConfig = NULL;
	mpUserKeyConfig = NULL;

	mpCustomStory = NULL;

	mpMenuCfg = NULL;
	mpGameCfg = NULL;
	mpDemoCfg = NULL;

	mpCurrentMapLoading = NULL;


	///////////////////////////////
	// Init string
	msGameName = "";

	msCurrentLanguage = "";

	///////////////////////////////
	// Init variables
	mbPTestActivated = false;

	///////////////////////////////
	// HARDMODE
	mbHardMode = false;
	mbAllowHardmode = false;
}

//-----------------------------------------------------------------------

cLuxBase::~cLuxBase()
{
	ExitEngine();

	if(mpCustomStory)
		hplDelete(mpCustomStory);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxBase::Init(const tString &asCommandline)
{
	/////////////////////////////
	// Parse the command line
	if(ParseCommandLine(asCommandline)==false) return false;

	/////////////////////////////
	// Init basic app and engine stuff
	if(InitApp()==false) return false;

	/////////////////////////////
	// Load the config files
	if(InitMainConfig()==false) return false;

	Log("Version %d.%d \n",kCurrentVersion_Main, kCurrentVersion_Minor);

	#ifdef COPY_PROTECTION_ENABLED
	/////////////////////////////
	// Copy Protection check
	eSerialInputReturnState serialState = AskForSerial();
	while(serialState!=eSerialInputReturnState_Validated)
	{
		if(serialState==eSerialInputReturnState_WrongKey)
		{
			cPlatform::CreateMessageBox(_W("Error!"),_W("Invalid Serial!\n"));
		}
		else
		{
			msErrorMessage = _W("Serial key needed to run game!\n");
			return false;
		}

		serialState = AskForSerial();
	}
	
	//calculate the CRC for the serial key.
	int lCRCKey = 1337;
	cCRC serialCRC(lCRCKey);
	for(size_t i=0; i<gsSerialKey.size(); ++i)
		serialCRC.PutByte(gsSerialKey[i]);	
	Log(" - s%X-%X", (int)serialCRC.Done(), lCRCKey);

	#endif
	Log("\n");
	
	/////////////////////////////
	// Init the engine
	if(InitEngine()==false) return false;

	/////////////////////////////
	// Check so all needed features are supported
	if(CheckFeatureSupport()==false) return false;

	/////////////////////////////
	// Init the game data and structures
	if(InitGame()==false) return false;


	//////////////////////////
	// Start premenu
	if(mbShowPreMenu && mbShowMenu)
	{
		mpEngine->GetUpdater()->SetContainer("PreMenu");
	}
	//////////////////////////
	// Start menu
	else if( mbShowMenu)
	{
		///////////////////////////////////////
		// Draw loading screen
		mpLoadScreenHandler->DrawMenuScreen();

		mpEngine->GetUpdater()->SetContainer("MainMenu");
	}
	//////////////////////////
	// Start game directly and create default profile
	else
	{
		//Create (if not existing) and set profile.
		CreateProfile(msDefaultProfileName);
		SetProfile(msDefaultProfileName);

		//Load user config
		if(InitUserConfig()==false) return false;
		
		//Unlock input if not in window
		if (mpDebugHandler->GetDebugWindowActive() == false)
		{
			if (mpConfigHandler->mbFullscreen == false)
			{
				mpEngine->GetInput()->GetLowLevel()->LockInput(true);
			}
			mpEngine->GetInput()->GetLowLevel()->RelativeMouse(true);
		}

		//Load map and start game.
		//By using "" user config values are used.
		StartGame("","", "");
	}

	
	//////////////////////////
	//The init is over
	InitOver();
		
	return true;
}

//-----------------------------------------------------------------------

void cLuxBase::Exit()
{
	if(mbSaveConfigAtExit) SaveConfig();
	ExitGame();
	ExitConfig();
}

//-----------------------------------------------------------------------

void cLuxBase::Run()
{
	mpEngine->Run();
}

void cLuxBase::Reset()
{
	mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
}

//-----------------------------------------------------------------------

void cLuxBase::RunModuleMessage(eLuxUpdateableMessage aMessage, void * apData)
{
	for(size_t i=0; i<mvModules.size(); ++i)
	{
		iLuxUpdateable *pModule = mvModules[i];

		pModule->LuxRunMessage(aMessage, apData);
	}
}

bool cLuxBase::StartGame(const tString& asFile, const tString& asFolder, const tString& asStartPos)
{
	///////////////////////////////////////
	// Draw loading screen
	mpLoadScreenHandler->DrawMenuScreen();

	///////////////////
	//Reset everything
	mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);
	mpProgressLogHandler->CreateAndResetLogFile();

	///////////////////
	//Get map file
	tString sMapFile = asFile;
	if(sMapFile == "")
	{
		if(mpCustomStory)
			sMapFile = mpCustomStory->msStartMap;
		else
			sMapFile = mpUserConfig->GetString("Map","File","");

		if(sMapFile == "") sMapFile = msStartMapFile;
	}

	///////////////////
	//Get map folder
	tString sMapFolder = asFolder;
	if(sMapFolder == "")
	{
		if(mpCustomStory)
			sMapFolder = mpCustomStory->msMapsFolder;
		else
			sMapFolder = mpUserConfig->GetString("Map","Folder","");

		if(sMapFolder == "") sMapFolder = msStartMapFolder;
	}
    mpMapHandler->SetMapFolder(sMapFolder);

	///////////////////
	//Get start pos
    tString sStartPos = asStartPos;
	if(sStartPos == "")
	{
		if(mpCustomStory)
			sStartPos = mpCustomStory->msStartPos;
		else
			sStartPos = mpUserConfig->GetString("Map","StartPos","");

		if(sStartPos == "") sStartPos = msStartMapPos;

	}

	///////////////////
	//Special ptest setup
	if(gpBase->mbPTestActivated)
	{
		#ifndef SKIP_PTEST_TESTS
			sMapFile = DecryptString((char*)gv_start_map_str);
			sMapFolder = DecryptString((char*)gv_start_folder_str);
			sStartPos = DecryptString((char*)gv_start_pos_str);
		#endif
	}

	//////////////////
	//Global script
	mpGlobalDataHandler->LoadAndInitGlobalScript();
	
	//////////////////
	//Load map
	cLuxMap *pMap = mpMapHandler->LoadMap(sMapFile, true);
	mpMapHandler->SetCurrentMap(pMap, true, true, sStartPos);
	
	///////////////////
	//Send message that game has been started.
	RunModuleMessage(eLuxUpdateableMessage_OnGameStart, NULL);

	return true;
}

//-----------------------------------------------------------------------

bool cLuxBase::StartCustomStory()
{
	if(mpCustomStory)
		return mpCustomStory->StartGame();

	return false;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//#define LOG_CRC 0

//-----------------------------------------------------------------------

bool cLuxBase::ParseCommandLine(const tString &asCommandline)
{
	msDefaultInitConfigFile = _W("config/main_init.cfg");

	if(asCommandline == "ptest")
	{
		mbPTestActivated = true;
		msInitConfigFile = cString::To16Char(DecryptString((char*)gv_main_init_str)); //_W("config/ptest_main_init.cfg");
		
		/*#ifndef SKIP_PTEST_TESTS
			msErrorMessage = cString::To16Char(DecryptString((char*)gv_error_mess_str));
			unsigned int lCRC = GetFileCRC(msInitConfigFile, 0x11af54e2);
			#ifdef LOG_CRC
				Log("main cfg crc: %x\n", lCRC);
			#endif
			if(lCRC != 0x4f5c2612) return false;
		#endif*/

		return true;
	}

	//////////////////////////////////
	// HARDMODE
	if(asCommandline == "hardmode")
	{
		msInitConfigFile = msDefaultInitConfigFile;
		mbHardMode = true;
		return true;
	}

	//////////////////////////////////
	//Main Init config file
	// TODO: Parse the command line better?
	msInitConfigFile = cString::To16Char(asCommandline);
	if(msInitConfigFile==_W("")) 
		msInitConfigFile = msDefaultInitConfigFile;

	return true;
}

//-----------------------------------------------------------------------

#include "LuxBasePersonal.h"

//-----------------------------------------------------------------------

bool cLuxBase::InitApp()
{
	// We load this HERE so the "userdir" magic can work
	tWString sPersonalDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);
#ifdef USERDIR_RESOURCES
	//Set the user resource directory (used for mac and linux builds to not require saving files in game dir)
	msUserResourceDir = sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES;
#endif

	/////////////////////////
	// Load the Init file
#ifdef USERDIR_RESOURCES
	cConfigFile *pInitCfg = hplNew(cConfigFile, (msInitConfigFile, msUserResourceDir));
#else
	cConfigFile *pInitCfg = hplNew(cConfigFile, (msInitConfigFile ));
#endif
	if(pInitCfg->Load()==false){
		msErrorMessage =_W("Could not load main init file: ")+msInitConfigFile;
		return false;
	}
	
	
	//Set the name of the folder (in Lux) that all save stuff will be put.
	msMainSaveFolder = pInitCfg->GetStringW("Directories","MainSaveFolder",_W(""));
	
	
	//Get the config file paths
	msDefaultUserConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultUserSettings",_W(""));
#if USE_SDL2
	
    msDefaultUserKeyConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultUserKeysSDL2", _W(""));
	msDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsSDL2",_W(""));

	if(msDefaultUserKeyConfigPath.empty() || msDefaultMainConfigPath.empty())
	{
		if(msInitConfigFile!=msDefaultInitConfigFile)
		{
			cConfigFile *pInitCfgDefaults = hplNew(cConfigFile, (msDefaultInitConfigFile));
			if(pInitCfgDefaults->Load()==false){
				msErrorMessage =_W("Could not load default main init file: ")+msDefaultInitConfigFile;
				return false;
			}

			tWString sSDL2CfgWarning = _W("The following 'ConfigFiles' path entries were not defined in ") + msInitConfigFile;

			if(msDefaultMainConfigPath.empty())
			{
				sSDL2CfgWarning += _W("\n - DefaultMainSettingsSDL2");

				msDefaultMainConfigPath = pInitCfgDefaults->GetStringW("ConfigFiles", "DefaultMainSettingsSDL2",_W(""));
			}

			if(msDefaultUserKeyConfigPath.empty())
			{
				sSDL2CfgWarning += _W("\n - DefaultUserKeysSDL2");
			
				msDefaultUserKeyConfigPath = pInitCfgDefaults->GetStringW("ConfigFiles", "DefaultUserKeysSDL2",_W(""));
			}

			sSDL2CfgWarning += _W("\nGame defaults will be used");
			
			hplDelete(pInitCfgDefaults);

			if(msDefaultMainConfigPath.empty()==false && msDefaultUserKeyConfigPath.empty()==false)
				cPlatform::CreateMessageBox(eMsgBoxType_Warning, _W("Warning"), sSDL2CfgWarning.c_str());
		}
		
		if(msDefaultMainConfigPath.empty() || msDefaultUserKeyConfigPath.empty())
		{
			msErrorMessage = _W("Could not load default settings files:");
			if(msDefaultMainConfigPath.empty())
				msErrorMessage += _W("\n - DefaultMainSettingsSDL2");

			if(msDefaultUserKeyConfigPath.empty())
				msErrorMessage += _W("\n - DefaultUserKeysSDL2");

			return false;
		}
	}
#else
	msDefaultUserKeyConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultUserKeys", _W(""));
	msDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettings",_W(""));
#endif

	msGameConfigPath = pInitCfg->GetStringW("ConfigFiles", "Game",_W(""));
	msMenuConfigPath = pInitCfg->GetStringW("ConfigFiles", "Menu",_W(""));
	msPreMenuConfigPath = pInitCfg->GetStringW("ConfigFiles", "PreMenu", _W(""));
	msDemoConfigPath = pInitCfg->GetStringW("ConfigFiles", "Demo", _W(""));

	msResourceConfigPath = pInitCfg->GetString("ConfigFiles", "Resources","");
	msMaterialConfigPath = pInitCfg->GetString("ConfigFiles", "Materials","");

	msDefaultBaseLanguage = pInitCfg->GetString("ConfigFiles", "DefaultBaseLanguage", "");
	msDefaultGameLanguage = pInitCfg->GetString("ConfigFiles", "DefaultGameLanguage", "");

	//Directories
	msBaseLanguageFolder = pInitCfg->GetString("Directories","BaseLanguageFolder","");
	msGameLanguageFolder = pInitCfg->GetString("Directories","GameLanguageFolder","");

	msCustomStoryPath = pInitCfg->GetString("Directories", "CustomStoryPath", "");

	//Various variables
	msGameName = pInitCfg->GetString("Variables","GameName","");
	mbAllowHardmode = pInitCfg->GetBool("Variables", "AllowHardMode", false);
	
	//Start map
	msStartMapFile = pInitCfg->GetString("StartMap","File","");
	msStartMapFolder = pInitCfg->GetString("StartMap","Folder","");
	msStartMapPos = pInitCfg->GetString("StartMap","Pos","");

	//Delete the config file
	hplDelete(pInitCfg);

	/////////////////////////////
	// Check some file CRC
	if(mbPTestActivated)
	{
		/*#ifndef SKIP_PTEST_TESTS
			msErrorMessage = cString::To16Char(DecryptString((char*)gv_error_mess_str));	

			unsigned int lCRC = GetFileCRC(msGameConfigPath, 0x56af34e2);
			#ifdef LOG_CRC
				Log("Game cfg crc: %x\n", lCRC);
			#endif
			if(lCRC != 0x3f2a8fae) return false;
			

			lCRC = GetFileCRC("entities/ptest/enemy_suitor/enemy_suitor_alois.ent", 0x561f3416);
			#ifdef LOG_CRC
				Log("enemy alois crc: %x\n", lCRC);
			#endif
			if(lCRC != 0xa7388492) return false;
			
			lCRC = GetFileCRC("entities/ptest/enemy_suitor/enemy_suitor_basile.ent", 0x59af34a4);
			#ifdef LOG_CRC
				Log("enemy basile crc: %x %d %x\n", lCRC, lCRC == 0xa6c66ee00, 0xa6c66ee00);
			#endif
			if(lCRC != 1818684928) return false;
			

			lCRC = GetFileCRC("entities/ptest/enemy_suitor/enemy_suitor_malo.ent", 0x59af34a4);
			#ifdef LOG_CRC
				Log("enemy malo crc: %x\n", lCRC);
			#endif
			if(lCRC != 0xac5e94c0) return false;
			
		#endif*/
	}
	
	//////////////////////////////
    //Set up the directories to be created
    tWStringVec vDirs;
#ifdef USERDIR_RESOURCES
    hpl::SetupBaseDirs(vDirs, PERSONAL_RELATIVEGAME_PARENT, msMainSaveFolder,
                    true, hpl::cString::To16Char(msCustomStoryPath));
#else
    hpl::SetupBaseDirs(vDirs, PERSONAL_RELATIVEGAME_PARENT, msMainSaveFolder);
#endif

    //Create directories	
    hpl::CreateBaseDirs(vDirs, sPersonalDir);

	//Set the base directory from which all saving will take place.
	msBaseSavePath = sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT + msMainSaveFolder + _W("/");

	//Set Crash flag file path
	msCrashFlagPath = msBaseSavePath + _W("crash_flag");
	msFirstStartFlagPath = msBaseSavePath + _W("first_start_flag");

	/////////////////////////
	//Set up log file locations
	SetLogFile(msBaseSavePath + _W("hpl.log"));
	SetUpdateLogFile(msBaseSavePath + _W("hpl_update.log"));

	return true;
}

//-----------------------------------------------------------------------

bool cLuxBase::CheckFeatureSupport()
{
	iLowLevelGraphics *pLowLevelGfx = mpEngine->GetGraphics()->GetLowLevel();

	///////////////////////////////
	// Features
	{
		if(pLowLevelGfx->GetCaps(eGraphicCaps_ShaderModel_2)==0)
		{
			msErrorMessage = _W("Shader model 2 not supported! Make sure your graphic card drivers are up to date!\n");
			return false;
		}

		if(pLowLevelGfx->GetCaps(eGraphicCaps_MaxDrawBuffers)<4)
		{
			msErrorMessage = _W("Not enough drawbuffers supported! Make sure your graphic card drivers are up to date!\n");
			return false;
		}

		if(pLowLevelGfx->GetCaps(eGraphicCaps_PackedDepthStencil)==0)
		{
			msErrorMessage = _W("Packed Depth and Stencil not supported! Make sure your graphic card drivers are up to date!\n");
			return false;
		}
	}
	
	return true;
}

//-----------------------------------------------------------------------

cConfigFile* cLuxBase::LoadConfigFile(const tWString& asDefaultPath, const tWString& asWantedPath, bool abForceLoadDefault, bool *abDidLoadDefault)
{
	cConfigFile *pConfig;
	bool bLoadedWantedPath = false;
	
	//////////////////////
	//Check if wanted exist and created config using existing file
	if(abForceLoadDefault==false && cPlatform::FileExists(asWantedPath))
	{
		pConfig = hplNew( cConfigFile, (asWantedPath) );
		bLoadedWantedPath = true;
	}
	else
	{
#ifdef USERDIR_RESOURCES
		pConfig = hplNew( cConfigFile, (asDefaultPath, msUserResourceDir) );
#else
		pConfig = hplNew( cConfigFile, (asDefaultPath) );
#endif
	}

	////////////////////////////////////
	// Load the settings config file
	if(pConfig->Load()==false)
	{
		msErrorMessage = _W("Failed to load config file!");
		return NULL;
	}

	//Set correct path for saving.
	if(bLoadedWantedPath==false)
	{
		pConfig->SetFileLocation(asWantedPath);
	}
	if (abDidLoadDefault) {
		*abDidLoadDefault = !bLoadedWantedPath;
	}

	return pConfig;
}

//-----------------------------------------------------------------------

bool cLuxBase::InitMainConfig()
{
	///////////////////////////////////////////////////////////////
	//Check if last init crashed, load default main config if so
	// Skip this for now since there is a launcher
	//bool bLastInitCrashed = false; //CheckCrashFlag(); 
	
	/////////////////////////////////////////////////
	// Load the main settings
	mpMainConfig = LoadConfigFile(msDefaultMainConfigPath, msBaseSavePath + _W("main_settings.cfg"),false);
	if(mpMainConfig==NULL) return false;

	//Load some basic variables
	mbSaveConfigAtExit = mpMainConfig->GetBool("Main","SaveConfig",true);
	mbExitMenuDirectly  = mpMainConfig->GetBool("Main","ExitMenuDirectly",false);
	
	msDefaultProfileName = mpMainConfig->GetStringW("Main","DefaultProfileName",_W(""));

	mbShowPreMenu = mpMainConfig->GetBool("Main", "ShowPreMenu", true);
	mbShowMenu = mpMainConfig->GetBool("Main", "ShowMenu",true);

	SetUpdateLogActive(mpMainConfig->GetBool("Main","UpdateLogActive", true));
	
	////////////////////////////////////
	// Load the game config file
#ifdef USERDIR_RESOURCES
	mpGameCfg = hplNew( cConfigFile, (msGameConfigPath, msUserResourceDir) );
#else
	mpGameCfg = hplNew( cConfigFile, (msGameConfigPath) );
#endif
	if(mpGameCfg->Load()==false)
	{
		msErrorMessage = _W("Failed to load game config file!");
		return false;
	}

	////////////////////////////////////
	// Load the menu config file
#ifdef USERDIR_RESOURCES
	mpMenuCfg = hplNew( cConfigFile, (msMenuConfigPath, msUserResourceDir) );
#else
	mpMenuCfg = hplNew( cConfigFile, (msMenuConfigPath) );
#endif

	if(mpMenuCfg->Load()==false)
	{
		msErrorMessage = _W("Failed to load menu config file!");
		return false;
	}

	////////////////////////////////////
	// Load the demo config file
#ifdef LUX_DEMO_VERSION
	mpDemoCfg = hplNew( cConfigFile, (msDemoConfigPath) );
	if(mpDemoCfg->Load()==false)
	{
		msErrorMessage = _W("Failed to load demo config file!");
		return false;
	}
#endif

	///////////////////////////////////
	// Create and init config handler
	mpConfigHandler = hplNew(cLuxConfigHandler,());
	mpConfigHandler->LoadMainConfig();

	///////////////////////////////////
	// Ptest extra fixes!!
	if(mbPTestActivated)
	{
		#ifndef SKIP_PTEST_TESTS
			mbShowMenu = true;
			mbShowPreMenu = true;
			mpConfigHandler->mbLoadDebugMenu = false;
			mpConfigHandler->msLangFile = "english.lang";
		#endif
	}

	return true;
}

bool cLuxBase::InitUserConfig()
{
	////////////////////////////////////////////////
	// Clear previous config
	if(mpUserConfig) hplDelete(mpUserConfig);
	if(mpUserKeyConfig) hplDelete(mpUserKeyConfig);
	mpUserConfig = NULL;
	mpUserKeyConfig = NULL;

	////////////////////////////////////////////////
	// Check if a valid profile is set
	if(msProfileName.empty())
		return false;
	
	/////////////////////////////////////////////////
	// Load the user settings
	mpUserConfig = LoadConfigFile(msDefaultUserConfigPath, msMainProfileSavePath +_W("user_settings.cfg") );
	if(mpUserConfig==NULL) return false;

	/////////////////////////
	//Load user key config
	bool bDidLoadDefault;
	mpUserKeyConfig = LoadConfigFile(msDefaultUserKeyConfigPath, msMainProfileSavePath +_W("user_keys.cfg") , false, &bDidLoadDefault);
	if(mpUserKeyConfig==NULL) return false;

#ifdef __APPLE__
	// Heinous kludge to get a default Mac keyboard shortcut without relying on different config files.
	if (bDidLoadDefault) {
		mpUserKeyConfig->SetString("Attack", "Secondary", "Keyboard::LeftMeta");
	}
#endif

	/////////////////////////
	//Load the profile language --- REMOVED - Language is now loaded only at startup
	//LoadLanguage(mpUserConfig->GetString("Game", "Language", msDefaultGameLanguage));

	/////////////////////////
	//Let all modules load user config.
	RunModuleMessage(eLuxUpdateableMessage_LoadUserConfig);

	return true;
}	

void cLuxBase::ExitConfig()
{
	hplDelete(mpConfigHandler);
	Log(" Deleting config files.\n");
	hplDelete(mpMainConfig);
	if(mpUserConfig) hplDelete(mpUserConfig);
	if(mpUserKeyConfig) hplDelete(mpUserKeyConfig);
	hplDelete(mpGameCfg);
	hplDelete(mpMenuCfg);
	if(mpDemoCfg) hplDelete(mpDemoCfg);
}

//-----------------------------------------------------------------------

void cLuxBase::SaveConfig()
{
	/////////////////////////////////////
	//Save all stuff
	mpConfigHandler->SaveMainConfig();

	//////////////////////////////////
	//Make other modules save too!
	RunModuleMessage(eLuxUpdateableMessage_SaveMainConfig);
	if(mpUserConfig) RunModuleMessage(eLuxUpdateableMessage_SaveUserConfig);

	/////////////////////
	// Main variables
	mpMainConfig->SetBool("Main", "ShowMenu", mbShowMenu);
	mpMainConfig->SetBool("Main", "ShowPreMenu", mbShowPreMenu);

	mpMainConfig->SetBool("Main","UpdateLogActive",GetUpdateLogActive());

	/////////////////////
	// User variables
	//if(mpUserConfig) 
	//{
		//mpUserConfig->SetString("Game", "Language",msCurrentLanguage);
	//}
	
	
	///////////////////////
	//Save the config files
	Log(" Saving main config.\n");
	mpMainConfig->Save();
	if(mpUserConfig)
	{
		Log(" Saving user config.\n");
		mpUserConfig->Save();
		if(mpUserKeyConfig)
			mpUserKeyConfig->Save();
	}
}	

//-----------------------------------------------------------------------

bool cLuxBase::InitEngine()
{
	//////////////////////////////////////////////////////////////////
	//Prepare to init, create crash_flag just in case it actually crashes :)
	// Skip this now that we have a launcher.
	//RaiseCrashFlag();

	cEngineInitVars vars;
	vars.mGraphics.mvScreenSize =  mpConfigHandler->mvScreenSize;
	vars.mGraphics.mlDisplay = mpConfigHandler->mlDisplay;
	vars.mGraphics.mbFullscreen =  mpConfigHandler->mbFullscreen;
	vars.mGraphics.msWindowCaption = msGameName + " Loading...";

	vars.mSound.mlSoundDeviceID = mpConfigHandler->mlSoundDevID;
	vars.mSound.mlMaxChannels = mpConfigHandler->mlMaxSoundChannels;
	vars.mSound.mlStreamBufferCount = mpConfigHandler->mlSoundStreamBuffers;
	vars.mSound.mlStreamBufferSize = mpConfigHandler->mlSoundStreamBufferSize;

	// Sound device filter set here (if needed)
#if defined(WIN32)
	iLowLevelSound::SetSoundDeviceNameFilter("software");
#endif
	
	//renderer variables
	iRenderer::SetShadowMapQuality((eShadowMapQuality)mpConfigHandler->mlShadowQuality);
	iRenderer::SetShadowMapResolution((eShadowMapResolution)mpConfigHandler->mlShadowRes);
	
	iRenderer::SetParallaxQuality((eParallaxQuality)mpConfigHandler->mlParallaxQuality);
	iRenderer::SetParallaxEnabled(mpConfigHandler->mbParallaxEnabled);

	iRenderer::SetRefractionEnabled(mpConfigHandler->mbRefraction);

	cRendererDeferred::SetSSAOBufferSizeDiv(mpConfigHandler->mlSSAOResolution==0? 2 : 1);
	cRendererDeferred::SetSSAONumOfSamples(mpConfigHandler->mlSSAOSamples);
	cRendererDeferred::SetSSAOLoaded(mpConfigHandler->mbSSAOActive);
	cRendererDeferred::SetGBufferType((eDeferredGBuffer)mpMainConfig->GetInt("Graphics","GBufferType", eDeferredGBuffer_32Bit));
	cRendererDeferred::SetNumOfGBufferTextures(mpMainConfig->GetInt("Graphics","NumOfGBufferTextures", 3));
	cRendererDeferred::SetEdgeSmoothLoaded(mpConfigHandler->mbEdgeSmooth);

	cRendererDeferred::SetOcclusionTestLargeLights(mpConfigHandler->mbOcclusionTestLights);

	iLowLevelGraphics::SetForceShaderModel3And4Off(mpConfigHandler->mbForceShaderModel3And4Off);

	//Other vars
	cResources::SetForceCacheLoadingAndSkipSaving(mpConfigHandler->mbForceCacheLoadingAndSkipSaving);
	cResources::SetCreateAndLoadCompressedMaps(false);
	//cResources::SetCreateAndLoadCompressedMaps(mbPTestActivated || mpConfigHandler->mbCreateAndLoadCompressedMaps);
    
	/////////////////////////
	// Create the engine
	mpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_All, &vars);
	
	/////////////////////////
	// Set up more properties
	mpEngine->GetGraphics()->GetLowLevel()->SetVsyncActive(mpConfigHandler->mbVSync, mpConfigHandler->mbAdaptiveVSync);
	
	float fGamma = mpMainConfig->GetFloat("Graphics","Gamma", 1.0f);
	mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(fGamma);
	
	mpEngine->SetLimitFPS(mpMainConfig->GetBool("Engine","LimitFPS", false));
	mpEngine->SetWaitIfAppOutOfFocus(mpMainConfig->GetBool("Engine","SleepWhenOutOfFocus", true));

	cMaterialManager* pMatMgr = mpEngine->GetResources()->GetMaterialManager();
	pMatMgr->SetTextureSizeDownScaleLevel(mpConfigHandler->mlTextureQuality);
	pMatMgr->SetTextureFilter((eTextureFilter)mpConfigHandler->mlTextureFilter);
	pMatMgr->SetTextureAnisotropy(mpConfigHandler->mfTextureAnisotropy);
	
	cSound *pSound = mpEngine->GetSound();
	pSound->GetLowLevel()->SetVolume(mpMainConfig->GetFloat("Sound","Volume",1.0f));

	/////////////////////////
	//Load configurations
#ifdef USERDIR_RESOURCES
	mpEngine->GetResources()->LoadResourceDirsFile(msResourceConfigPath, msUserResourceDir);
#else
	mpEngine->GetResources()->LoadResourceDirsFile(msResourceConfigPath);
#endif

	mpEngine->GetPhysics()->LoadSurfaceData(msMaterialConfigPath);

	/////////////////////////
	//Load the start language
	//if(mbPTestActivated)
	//	LoadLanguage("english.lang");
	//else
		LoadLanguage(mpMainConfig->GetString("Main", "StartLanguage",msDefaultGameLanguage));

	/////////////////////////
	//Add extra paths
	//TO add...?

	/////////////////////////
	// Misc set up
	mpEngine->GetResources()->GetMeshManager()->SetFastloadMaterial("fastload_material.mat");

	////////////////////////////////////////////////
	// Seems there was no crash, set flag to false
	// Skip crash flag for now, as there is a launcher
	//LowerCrashFlag();

	////////////////////////////////////////////////
	// Check stuff matches after engine creation
	iSoundDeviceIdentifier* pSndDevice = mpEngine->GetSound()->GetLowLevel()->GetCurrentSoundDevice();
	mpConfigHandler->mlSoundDevID = pSndDevice->GetID();
	
	return true;
}

void cLuxBase::ExitEngine()
{
	if(mpEngine) DestroyHPLEngine(mpEngine);
}

//-----------------------------------------------------------------------

//Helper functions to more easily create modules.

#define CreateModule( aClassType, asContainer ) (aClassType*)AddModule(hplNew(aClassType, () ), asContainer);
#define CreateGlobalModule( aClassType) (aClassType*)AddGlobalModule(hplNew(aClassType, () ));

//-----------------------------------------------------------------------

bool cLuxBase::InitGame()
{
	///////////////////////////////////////
	// Demo output
	#ifdef LUX_DEMO_VERSION
		Log("- DEMO VERSION -\n");
	#endif

	///////////////////////////////////////
	// Create game data
	mvHudVirtualCenterSize = cVector2f(800,600);
	LuxCalcGuiSetScreenOffset(mvHudVirtualCenterSize, mvHudVirtualSize, mvHudVirtualOffset);
	mvHudVirtualStartPos = cVector3f(-mvHudVirtualOffset.x,-mvHudVirtualOffset.y,0);

	mpGameHudSet = mpEngine->GetGui()->CreateSet("GameHud",NULL);
	mpGameHudSet->SetVirtualSize(mvHudVirtualSize,-1000, 1000, mvHudVirtualOffset);
	mpGameDebugSet = mpEngine->GetGui()->CreateSet("GameDebug",NULL);
	mpGameDebugSet->SetDrawPriority(1);
	mpGameHudSet->SetDrawPriority(0);
	
	/////////////////////////////
	// Load another font if game is in chinese, 
	// might want to fix this later since some debug text is tiny now

	if (msCurrentLanguage == "chinese.lang")
		mpDefaultFont = mpEngine->GetResources()->GetFontManager()->CreateFontData("game_default.fnt");
	else
		mpDefaultFont = mpEngine->GetResources()->GetFontManager()->CreateFontData("font_default.fnt");

	///////////////////////////////////////
	// Create updater containers
	mpEngine->GetUpdater()->AddContainer("PreMenu");
	mpEngine->GetUpdater()->AddContainer("MainMenu");	
	mpEngine->GetUpdater()->AddContainer("Inventory");
	mpEngine->GetUpdater()->AddContainer("Journal");
	mpEngine->GetUpdater()->AddContainer("Credits");
	mpEngine->GetUpdater()->AddContainer("LoadScreen");
#ifdef LUX_DEMO_VERSION
	mpEngine->GetUpdater()->AddContainer("DemoEnd");
#endif

	///////////////////////////////////////
	// Create Modules

	//Global
	mpInputHandler = CreateGlobalModule( cLuxInputHandler);
	mpHelpFuncs = CreateGlobalModule( cLuxHelpFuncs);
	mpSaveHandler = CreateGlobalModule( cLuxSaveHandler);
	mpScriptHandler = CreateGlobalModule( cLuxScriptHandler);
	mpProgressLogHandler = CreateGlobalModule( cLuxProgressLogHandler);
	
	//Default
	mpMapHandler = CreateModule( cLuxMapHandler, "Default");
	mpMapHelper = CreateModule( cLuxMapHelper, "Default");
	mpPlayer = CreateModule( cLuxPlayer, "Default");
	mpInsanityHandler = CreateModule( cLuxInsanityHandler, "Default"); 
	mpDebugHandler = CreateModule( cLuxDebugHandler, "Default");
	mpEffectRenderer = CreateModule( cLuxEffectRenderer, "Default");
	mpMusicHandler = CreateModule( cLuxMusicHandler, "Default");
	mpMessageHandler = CreateModule( cLuxMessageHandler, "Default");
	mpEffectHandler = CreateModule( cLuxEffectHandler, "Default");
	mpCompletionCountHandler = CreateModule( cLuxCompletionCountHandler, "Default"); 
	mpGlobalDataHandler = CreateModule( cLuxGlobalDataHandler, "Default"); 
	mpHintHandler = CreateModule( cLuxHintHandler, "Default"); 
	mpPostEffectHandler = CreateModule( cLuxPostEffectHandler, "Default"); 
	mpAchievementHandler = CreateModule( iLuxAchievementHandler, "Default");

	InitAchievements();
	
	//PreMenu
	mpPreMenu = CreateModule( cLuxPreMenu, "PreMenu");

	//Main Menu
	mpMainMenu = CreateModule( cLuxMainMenu, "MainMenu");

	//Inventory
	mpInventory = CreateModule( cLuxInventory, "Inventory");

	//Journal
	mpJournal = CreateModule( cLuxJournal, "Journal");

	//Credits
#ifndef LUX_DEMO_VERSION
	mpCredits = CreateModule( cLuxCredits, "Credits");
#endif

	//Demo End
	mpDemoEnd = NULL;
#ifdef LUX_DEMO_VERSION
	mpDemoEnd = CreateModule( cLuxDemoEnd, "DemoEnd");
#endif

	//Load screen
	mpLoadScreenHandler = CreateModule( cLuxLoadScreenHandler, "LoadScreen");


	//Make sure all of the modules are reset!
	mpEngine->GetUpdater()->BroadcastMessageToAll(eUpdateableMessage_Reset);

	///////////////////////////////////////
	// Create and Add Loaders
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxStaticPropLoader, ("StaticProp")));

    mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Object, ("Object")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_SwingDoor, ("SwingDoor")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Lever, ("Lever")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Wheel, ("Wheel")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Lamp, ("Lamp")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Photocell, ("Photocell")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_MoveObject, ("MoveObject")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Item, ("Item")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Chest, ("Chest")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_LevelDoor, ("LevelDoor")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Button, ("Button")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_OilBarrel, ("OilBarrel")));
	
#ifndef LUX_DEMO_VERSION
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_EmotionStone, ("EmotionStone")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_NPC, ("NPC")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_MultiSlider, ("MultiSlider")));
#endif
	
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Critter_Bug, ("CritterBug")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxPropLoader_Critter_Spider, ("CritterSpider")));

	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxEnemyLoader_Grunt, ("Enemy_Grunt")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxEnemyLoader_WaterLurker, ("Enemy_WaterLurker")));
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxEnemyLoader_ManPig, ("Enemy_ManPig")));

#ifndef LUX_DEMO_VERSION
	mpEngine->GetResources()->AddEntityLoader(hplNew(cLuxCommentaryIconLoader, ("CommentaryIcon")));
#endif

	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaNodeLoader_PlayerStart, ("PlayerStart")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaNodeLoader_PathNode, ("PathNode")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaNodeLoader_PosNode, ("PosNode")));

	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Script, ("Script")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Flashback, ("Flashback")));
#ifndef LUX_DEMO_VERSION
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Ladder, ("Ladder")));
#endif
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Liquid, ("Liquid")));
#ifndef LUX_DEMO_VERSION
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Sticky, ("Sticky")));
#endif
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Insanity, ("Insanity")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Examine, ("Examine")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_Sign, ("Sign")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaLoader_SlimeDamage, ("SlimeDamage")));
	mpEngine->GetResources()->AddAreaLoader(hplNew(cLuxAreaRopeLoader, ("Rope")));
	

	RunModuleMessage(eLuxUpdateableMessage_LoadFonts);
	///////////////////////////////////////////////////////
	// Run the LoadMainConfig message for game modules, couldn't be run before
	RunModuleMessage(eLuxUpdateableMessage_LoadMainConfig);

	return true;
}


//-----------------------------------------------------------------------

void cLuxBase::ExitGame()
{
	Log(" Deleting game modules.\n");
	for(size_t i=0; i<mvModules.size(); ++i)
	{
		Log("   '%s'\n", mvModules[i]->GetName().c_str());
		hplDelete( mvModules[i]);
	}
}

//-----------------------------------------------------------------------

void cLuxBase::InitOver()
{
	//Set proper caption when loading is done.
    gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetWindowCaption(msGameName);

}

//-----------------------------------------------------------------------

bool cLuxBase::CreateProfile(const tWString& asName)
{
	tWString sPath = msBaseSavePath + asName + _W("/");

	if(cPlatform::FolderExists(sPath)) return false;

	cPlatform::CreateFolder(sPath);
	
	return true;
}

void cLuxBase::SetProfile(const tWString& asName)
{
	//If a profile is already set, save it.
	if(msProfileName != _W("") && mpUserConfig)
	{
		// User variables

		// Commented out cos there's no language setting in user config now
		//mpUserConfig->SetString("Game", "Language",msCurrentLanguage);
		
		//Save the config files
		Log(" Saving user config.\n");
		mpUserConfig->Save();
		if(mpUserKeyConfig)
			mpUserKeyConfig->Save();
	}

	msProfileName = asName;
	if(msProfileName!=_W(""))
	{
		msMainProfileSavePath = cString::AddSlashAtEndW(msBaseSavePath + asName,_W('/'));
		msProfileSavePath = msMainProfileSavePath;
		Log(" Setting profile: '%s' Path: '%s'\n",cString::To8Char(asName).c_str(), cString::To8Char(msMainProfileSavePath).c_str());
	}
	else
	{
		msMainProfileSavePath = _W("");
		msProfileSavePath = _W("");
	}
}

//-----------------------------------------------------------------------

void cLuxBase::PreloadSound(const tString &asFile)
{
	if(asFile=="") return;
	gpBase->mpEngine->GetResources()->GetSoundEntityManager()->Preload(asFile);
}

void cLuxBase::PreloadParticleSystem(const tString &asFile)
{
	if(asFile=="") return;
	gpBase->mpEngine->GetResources()->GetParticleManager()->Preload(asFile);	
}

//-----------------------------------------------------------------------

void cLuxBase::SetCustomStory(cLuxCustomStorySettings* apCustomStory)
{
	//Debug
	//Log("Trying to set custom story 0x%x\n", apCustomStory);

	if(mpCustomStory==apCustomStory)
		return;

	//Debug
	//Log(" Not current one, setting up\n", apCustomStory);

	if(mpCustomStory)
	{
		hplDelete(mpCustomStory);
		mpCustomStory = NULL;
	}

	mpCustomStory = apCustomStory;

	if(mpCustomStory)
	{
		tWStringVec vStoryFolders;
		tWString sSep = _W("/");
		cString::GetStringVecW(mpCustomStory->msStoryRootFolder, vStoryFolders, &sSep);
		tWString sStoryFolder = vStoryFolders[vStoryFolders.size()-1];

		msProfileSavePath = cString::AddSlashAtEndW(msMainProfileSavePath + _W("custom")) +
								cString::AddSlashAtEndW(sStoryFolder);

		mpEngine->GetResources()->AddResourceDir(mpCustomStory->msStoryRootFolder, true);
	}
	else
		msProfileSavePath = msMainProfileSavePath;

	LoadLanguage(msCurrentLanguage, true);
}

//-----------------------------------------------------------------------

bool cLuxBase::LoadLanguage(const tString& asName, bool abForceReload)
{
	cResources *pResources = mpEngine->GetResources();

	////////////////////////////////////////////
	//Check if the language is already loaded.
	tString sLowName = cString::ToLowerCase(asName);
	if(msCurrentLanguage == sLowName && abForceReload==false) return false;

	if(msCurrentLanguage != "")
	{
		pResources->ClearTranslations();	
	}

	msCurrentLanguage = sLowName;

	//Debug
	//Log("Loading language %s\n", asName.c_str());

	tString sGameFileName = cString::SetFileExt(asName,"lang");
	tString sBaseFileName = "base_"+sGameFileName;

	////////////////////////////////////////////////////
	//Clear the resources so we can load other fonts
	// NOTE: This made sense when the lang was changed on the fly
	// Stays here commented out just in case
	/*RunModuleMessage(eLuxUpdateableMessage_ClearFonts);

	pResources->ClearResourceDirs();
	pResources->AddResourceDir(_W("core/shaders"),false);
	pResources->AddResourceDir(_W("core/textures"),false);
	pResources->AddResourceDir(_W("core/models"),false);
	pResources->LoadResourceDirsFile(msResourceConfigPath);

	if(mpCustomStory)
		mpEngine->GetResources()->AddResourceDir(mpCustomStory->msStoryRootFolder, true);
	*/

	////////////////////////////////////////////
	//Load the language files

	// Custom story
	if(mpCustomStory && mpCustomStory->msExtraLangFilePrefix!="")
	{
		tString sExtraLangFileName = cString::To8Char(mpCustomStory->msStoryRootFolder) + mpCustomStory->msExtraLangFilePrefix + sGameFileName;
		if(gpBase->mpEngine->GetResources()->GetFileSearcher()->GetFilePath(sExtraLangFileName)!=_W(""))
			pResources->AddLanguageFile(sExtraLangFileName, true);
	}
	
	// Main game lang
#ifdef USERDIR_RESOURCES
	pResources->AddLanguageFile(msGameLanguageFolder + sGameFileName, true, msUserResourceDir);
#else
	pResources->AddLanguageFile(msGameLanguageFolder + sGameFileName, true);
#endif

#ifdef USERDIR_RESOURCES
	pResources->AddLanguageFile(msBaseLanguageFolder + sBaseFileName, true, msUserResourceDir);
#else
	pResources->AddLanguageFile(msBaseLanguageFolder + sBaseFileName, true);
#endif

    
	////////////////////////////////////////////
	//If not default language, add default to so only missing entries are filled in
	if(mpCustomStory && sGameFileName != mpCustomStory->msDefaultExtraLanguage)
		pResources->AddLanguageFile(cString::To8Char(mpCustomStory->msStoryRootFolder) + 
										mpCustomStory->msExtraLangFilePrefix + 
										mpCustomStory->msDefaultExtraLanguage, false);

	if(sGameFileName != msDefaultGameLanguage)
#ifdef USERDIR_RESOURCES
		pResources->AddLanguageFile(msGameLanguageFolder + msDefaultGameLanguage, false, msUserResourceDir);
#else
		pResources->AddLanguageFile(msGameLanguageFolder + msDefaultGameLanguage, false);
#endif

	if(sBaseFileName != msDefaultBaseLanguage)	
#ifdef USERDIR_RESOURCES
		pResources->AddLanguageFile(msBaseLanguageFolder + msDefaultBaseLanguage, false, msUserResourceDir);
#else
		pResources->AddLanguageFile(msBaseLanguageFolder + msDefaultBaseLanguage, false);
#endif

	// Refresh all modules with new translation
	//RunModuleMessage(eLuxUpdateableMessage_LoadFonts, NULL);
    
	return true;
}


//-----------------------------------------------------------------------


iLuxUpdateable* cLuxBase::AddModule(iLuxUpdateable *apModule, const tString& asContainer)
{
	mpEngine->GetUpdater()->AddUpdate(asContainer, apModule);
	mvModules.push_back(apModule);
	return apModule;
}

iLuxUpdateable* cLuxBase::AddGlobalModule(iLuxUpdateable *apModule)
{
	mpEngine->GetUpdater()->AddGlobalUpdate(apModule);
	mvModules.push_back(apModule);
	return apModule;
}
//-----------------------------------------------------------------------

void cLuxBase::RaiseCrashFlag()
{
	FILE* pCrashFlagFile = cPlatform::OpenFile(msCrashFlagPath.c_str(), _W("w"));
	fclose(pCrashFlagFile);
}

void cLuxBase::LowerCrashFlag()
{
	cPlatform::RemoveFile(msCrashFlagPath);
}

bool cLuxBase::CheckCrashFlag()
{
	return cPlatform::FileExists(msCrashFlagPath);
}

//-----------------------------------------------------------------------

void cLuxBase::RaiseFirstStartFlag()
{
	if(CheckFirstStartFlag())
		return;

	FILE* pFirstStartFlagFile = cPlatform::OpenFile(msFirstStartFlagPath.c_str(), _W("w"));
	fclose(pFirstStartFlagFile);
}

void cLuxBase::LowerFirstStartFlag()
{
	cPlatform::RemoveFile(msFirstStartFlagPath);
}

bool cLuxBase::CheckFirstStartFlag()
{
	return cPlatform::FileExists(msFirstStartFlagPath);
}

void cLuxBase::InitAchievements()
{
	if(mpAchievementHandler == NULL) return;

	mpAchievementHandler->CreateAchievement(eLuxAchievement_Insanity, "Insanity");

	// tdd
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Alchemist, "Alchemist");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_EscapeArtist, "EscapeArtist");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Descendant, "Descendant");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Pipeworker, "Pipeworker");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Restorer, "Restorer");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Survivor, "Survivor");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Sacrifice, "Sacrifice");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Benefactor, "Benefactor");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Illuminatus, "Illuminatus");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_NOPE, "NOPE");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Quitter, "Quitter");
	
	// justine
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Egotist, "Egotist");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Altruist, "Altruist");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Vacillator, "Vacillator");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_StillAlive, "StillAlive");

	/////////////////////
	// HARDMODE
	mpAchievementHandler->CreateAchievement(eLuxAchievement_Masochist, "Masochist");
	
	
	// general
	mpAchievementHandler->CreateAchievement(eLuxAchievement_MasterArchivist, "MasterArchivist");
	//mpAchievementHandler->CreateAchievement(eLuxAchievement_Insomniac, "Insomniac");
	/*
	// pig
	mpAchievementHandler->CreateAchievement(eLuxAchievement_TheTeeth, "TheTeeth");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_TheThroat, "TheThroat");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_TheGut, "TheGut");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_TheEntrails, "TheEntrails");
	mpAchievementHandler->CreateAchievement(eLuxAchievement_TheHeart, "TheHeart");
	*/

	mpAchievementHandler->RegisterAchievements();
	/*
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Alchemist);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_EscapeArtist);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Descendant);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Pipeworker);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Restorer);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Survivor);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Sacrifice);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Benefactor);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Illuminatus);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Quitter);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_NOPE);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Egotist);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Altruist);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_Vacillator);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_StillAlive);
	mpAchievementHandler->UnlockAchievement(eLuxAchievement_MasterArchivist);
	*/
}
