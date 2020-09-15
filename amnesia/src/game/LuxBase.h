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

#ifndef LUX_BASE_H
#define LUX_BASE_H

//----------------------------------------------

#include "StdAfx.h"

#include "LuxTypes.h"

//----------------------------------------------

class cLuxMapHandler;
class cLuxMapHelper;
class cLuxInputHandler;

class cLuxEffectHandler;

class cLuxDebugHandler;
class cLuxSaveHandler;
class cLuxScriptHandler;
class cLuxHelpFuncs;
class cLuxEffectRenderer;
class cLuxMusicHandler;
class cLuxMessageHandler;
class cLuxJournal;
class cLuxCompletionCountHandler;
class cLuxGlobalDataHandler;
class cLuxHintHandler;
class cLuxConfigHandler;
class cLuxPostEffectHandler;
class cLuxInsanityHandler;
class cLuxProgressLogHandler;
class cLuxLoadScreenHandler;
class iLuxAchievementHandler;

class cLuxInventory;

class cLuxCredits;

class cLuxDemoEnd;

class cLuxPreMenu;

class cLuxMainMenu;

class cLuxPlayer;


//----------------------------------------------

extern eLuxAxis StringToAxis(const tString &asAxis);

/**
* Calculates an offset and size based on virtual size that keeps the ratio of the size. Note that the outsize is the size of the ENTIRE screen!
*/
extern void LuxCalcGuiSetOffset(const cVector2f &avVirtualSizeIn, const cVector2f& avScreenSize, cVector2f& avOutSize, cVector2f & avOutOffset);

/**
* Same as CalcGuiSetOffset, but assumes avScreenSize is fromm screen.
*/
extern void LuxCalcGuiSetScreenOffset(const cVector2f &avVirtualSizeIn, cVector2f& avOutSize, cVector2f & avOutOffset);


extern void ProgLog(eLuxProgressLogLevel aLevel, const tString& asMessage);

//----------------------------------------------

/**
 * LuxCustomStorySettings - class used to store custom made story data
 *
 */
class cLuxCustomStorySettings
{
public:
	cLuxCustomStorySettings();
	cLuxCustomStorySettings(cLuxCustomStorySettings* apStory);
	~cLuxCustomStorySettings();

	bool CreateFromPath(const tWString& asPath);

	/**
	 * Creates a copy of the current object and "feeds" it to cLuxBase
	 * (this because these objects are created and destroyed in the Custom Story List window)
	 */
	void SetActive();

	/**
	 * Starts a new game using the custom settings. 
	 * NOTE: Should be called from a copied object, NEVER from one in the Custom Story List window!!
	 */
	bool StartGame();

	tWString msName;
	tWString msAuthor;
	tString msImgFile;

	tWString msStoryRootFolder;
	tString msStartMap;
	tString msStartPos;
	tString msMapsFolder;

	tString msExtraLangFilePrefix;
	tString msDefaultExtraLanguage;
};

typedef std::vector<cLuxCustomStorySettings*> tCustomStorySettingsVec;

//----------------------------------------------

class cLuxBase
{
public:	
	cLuxBase();
	~cLuxBase();

    bool Init(const tString &asCommandline);
	void Exit();

	void Run();

	void Reset();

	void RunModuleMessage(eLuxUpdateableMessage aMessage, void * apData=NULL);

	bool StartGame(const tString& asFile, const tString& asFolder, const tString& asStartPos);
	bool StartCustomStory();

	void SaveConfig();

	bool InitUserConfig();

	bool CreateProfile(const tWString& asName);
	void SetProfile(const tWString& asName);

	void PreloadSound(const tString &asFile);
	void PreloadParticleSystem(const tString &asFile);

	void SetCustomStory(cLuxCustomStorySettings* apCustomStory);

//private:
	bool ParseCommandLine(const tString &asCommandline);

	bool InitApp();
	bool CheckFeatureSupport();

	bool InitMainConfig();
	void ExitConfig();

	cConfigFile* LoadConfigFile(const tWString& asDefaultPath, const tWString& asWantedPath, bool abForceLoadDefault=false, bool *abDidLoadDefault = NULL);
		
	bool InitEngine();
	void ExitEngine();

	bool InitGame();
	void ExitGame();

	void InitOver();

	
	bool LoadLanguage(const tString& asName, bool abForceReload=false);

	iLuxUpdateable *AddModule(iLuxUpdateable *apModule, const tString& asContainer);
	iLuxUpdateable *AddGlobalModule(iLuxUpdateable *apModule);


	void RaiseCrashFlag();
	void LowerCrashFlag();
	bool CheckCrashFlag();

	void RaiseFirstStartFlag();
	void LowerFirstStartFlag();
	bool CheckFirstStartFlag();

	void InitAchievements();

	/////////////////////////
	// Public variables
public:
	cEngine *mpEngine;

	cConfigFile* mpMainConfig;
	cConfigFile* mpUserConfig;
	cConfigFile* mpUserKeyConfig;

	cConfigFile* mpGameCfg;
	cConfigFile* mpMenuCfg;
	cConfigFile* mpDemoCfg;
	cGuiSet *mpGameDebugSet;
	cGuiSet *mpGameHudSet;
	iFontData *mpDefaultFont;
	cVector2f mvHudVirtualCenterSize;//This is size of what is inside a 4:3 ratio!
	cVector2f mvHudVirtualSize;
	cVector2f mvHudVirtualOffset;
	cVector3f mvHudVirtualStartPos;

	cLuxCustomStorySettings* mpCustomStory;

	cLuxConfigHandler *mpConfigHandler;
	cLuxInputHandler *mpInputHandler;
	cLuxEffectHandler *mpEffectHandler;
	cLuxMapHandler *mpMapHandler;
	cLuxMapHelper *mpMapHelper;
	cLuxDebugHandler *mpDebugHandler;
	cLuxSaveHandler *mpSaveHandler;
	cLuxScriptHandler *mpScriptHandler;
	cLuxHelpFuncs *mpHelpFuncs;
	cLuxEffectRenderer *mpEffectRenderer;
	cLuxInventory *mpInventory;
	cLuxPreMenu *mpPreMenu;
	cLuxMainMenu *mpMainMenu;
	cLuxPlayer *mpPlayer;
	cLuxMusicHandler *mpMusicHandler;
	cLuxMessageHandler *mpMessageHandler;
	cLuxJournal *mpJournal;
	cLuxCompletionCountHandler *mpCompletionCountHandler;
	cLuxGlobalDataHandler *mpGlobalDataHandler;
	cLuxHintHandler *mpHintHandler;
	cLuxPostEffectHandler *mpPostEffectHandler;
	cLuxInsanityHandler *mpInsanityHandler;
	cLuxProgressLogHandler *mpProgressLogHandler;
	cLuxLoadScreenHandler *mpLoadScreenHandler;
	cLuxCredits *mpCredits;
	cLuxDemoEnd* mpDemoEnd;
	iLuxAchievementHandler* mpAchievementHandler;

	tString msGameName;
	tWString msErrorMessage;

	bool mbShowPreMenu;
	bool mbShowMenu;

	tString msStartMapFile;
	tString msStartMapFolder;
	tString msStartMapPos;

	tWString msDefaultProfileName;

	tWString msBaseSavePath;
	tWString msProfileSavePath;
	tWString msMainProfileSavePath;
	tWString msProfileName;
#ifdef USERDIR_RESOURCES
	tWString msUserResourceDir;
#endif

	bool mbExitMenuDirectly;

	bool mbPTestActivated;

	cLuxMap *mpCurrentMapLoading;

	/////////////////////////
	// HARDMODE
	bool mbHardMode;
	bool mbAllowHardmode;

	/////////////////////////
	// Kinda private variables
	tWString msDefaultInitConfigFile;
	tWString msInitConfigFile;

	tWString msDefaultMainConfigPath;
	tWString msDefaultUserConfigPath;
	tWString msDefaultUserKeyConfigPath;
	tString msResourceConfigPath;
	tString msMaterialConfigPath;
	tString msDefaultBaseLanguage;
	tString msDefaultGameLanguage;
	
	tString msBaseLanguageFolder;
	tString msGameLanguageFolder;

	tString msCustomStoryPath;

	tString msCurrentLanguage;

	tWString msMainSaveFolder;
	tWString msGameConfigPath;
	tWString msMenuConfigPath;
	tWString msPreMenuConfigPath;
	tWString msDemoConfigPath;

	tWString msCrashFlagPath;
	tWString msFirstStartFlagPath;

    bool mbSaveConfigAtExit;

	std::vector<iLuxUpdateable*> mvModules;

};

//----------------------------------------------

extern cLuxBase* gpBase;

//----------------------------------------------


#endif // LUX_BASE_H
