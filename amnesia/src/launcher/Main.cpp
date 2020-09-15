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

#include "launcher.h"
#include "hpl.h"
#include "QualityChooser.h"

#include "FL/fl_ask.H"

#ifdef WIN32
	#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#if USE_SDL2
#include <SDL2/SDL.h>
#elif defined WIN32
#include "glut.h"
#elif defined __linux__
#include <GL/glx.h>
#endif


using namespace hpl;

//--------------------------------------------------------------------------------

#include "../game/LuxBasePersonal.h"

//-----------------------------------------------------------------------

cEngine* gpEngine = NULL;

#ifdef USERDIR_RESOURCES
tWString gsUserResourceDir;
#endif

//tWString gsInitConfigFile;
tWString gsMainSaveFolder;

tString gsDefaultBaseLanguage;
tString gsDefaultGameLanguage;

tString gsBaseLanguageFolder;
tString gsGameLanguageFolder;
tString gsCustomStoriesPath;

tWString gsDefaultMainConfigPath;
tWString gsDefaultMainConfigPathLow;
tWString gsDefaultMainConfigPathMedium;
tWString gsDefaultMainConfigPathHigh;
tString gsGameName;
tWString gsBaseSavePath;
tWString gsCrashFlagPath;

std::vector<cConfigFile*> gvPresets;

bool InitPaths(const tWString& asInitConfigFile, const tWString &asDefaultInitConfigFile)
{
    // Load this up as a global to search alternate resource path
	tWString sPersonalDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);

    tWString sInitConfigFile = asInitConfigFile;
#ifdef USERDIR_RESOURCES
    gsUserResourceDir = sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES;
    if (cPlatform::FileExists(gsUserResourceDir + asInitConfigFile)) {
        sInitConfigFile = gsUserResourceDir + asInitConfigFile;
    }
#endif
	/////////////////////////
	// Load the Init file
	cConfigFile *pInitCfg = hplNew(cConfigFile, (sInitConfigFile));
	if(pInitCfg->Load()==false){
		fl_message("%s",cString::To8Char((_W("Could not load main init file: ")+asInitConfigFile)).c_str());
		return false;
	}
	
	//Set the name of the folder (in Lux) that all save stuff will be put.
	gsMainSaveFolder = pInitCfg->GetStringW("Directories","MainSaveFolder",_W(""));
	
	//Get the config file paths
#if USE_SDL2
	gsDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsSDL2",_W(""));
	if(gsDefaultMainConfigPath.empty() && asInitConfigFile!=asDefaultInitConfigFile)
	{
		fl_message("%s",cString::To8Char((_W("No DefaultMainSettingsSDL2 path defined in: ")+asInitConfigFile)+_W(" - Using default")).c_str());

		cConfigFile *pInitCfgDefaults = hplNew(cConfigFile, (asDefaultInitConfigFile));
		if(pInitCfgDefaults->Load()==false){
			fl_message("%s",cString::To8Char((_W("Could not load default main init file: ")+asDefaultInitConfigFile)).c_str());
			return false;
		}

		gsDefaultMainConfigPath = pInitCfgDefaults->GetStringW("ConfigFiles", "DefaultMainSettingsSDL2",_W(""));
		hplDelete(pInitCfgDefaults);

		if(gsDefaultMainConfigPath.empty())
		{
			fl_message("Settings file for entry DefaultMainSettingsSDL2 in default config is empty");
			
			return false;
		}
	}
#else
	gsDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettings",_W(""));
#endif

	// Get presets paths
	gsDefaultMainConfigPathLow = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsLow",_W(""));
	gsDefaultMainConfigPathMedium = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsMedium",_W(""));
	gsDefaultMainConfigPathHigh = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsHigh",_W(""));

	gsDefaultBaseLanguage = pInitCfg->GetString("ConfigFiles", "DefaultBaseLanguage", "");
	//gsDefaultGameLanguage = pInitCfg->GetString("ConfigFiles", "DefaultGameLanguage", "");

	//Directories
	gsBaseLanguageFolder = pInitCfg->GetString("Directories","BaseLanguageFolder","");
	gsGameLanguageFolder = pInitCfg->GetString("Directories","GameLanguageFolder","");
    gsCustomStoriesPath = pInitCfg->GetString("Directories","CustomStoryPath","");

	//Various variables
	gsGameName = pInitCfg->GetString("Variables","GameName","");
	

	//Delete the config file
	hplDelete(pInitCfg);

    //////////////////////////////
    //Set up the directories to be created
    tWStringVec vDirs;
    hpl::SetupBaseDirs(vDirs, PERSONAL_RELATIVEGAME_PARENT, gsMainSaveFolder
#ifdef USERDIR_RESOURCES
                       , true, hpl::cString::To16Char(gsCustomStoriesPath)
#endif
                       );
    //Create directories	
    hpl::CreateBaseDirs(vDirs, sPersonalDir);

	//Set the base directory from which all saving will take place.
	gsBaseSavePath = sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT + gsMainSaveFolder + _W("/");

	//Set Crash flag file path
	gsCrashFlagPath = gsBaseSavePath + _W("crash_flag");

	// Store the log file in the users personal directory not in the install directory
	SetLogFile(gsBaseSavePath + _W("launcher.log"));
	return true;
}


//--------------------------------------------------------------------------------

cConfigFile* LoadConfigFile(const tWString& asDefaultPath, const tWString& asWantedPath, bool abForceLoadDefault)
{
	cConfigFile *pConfig;
	bool bLoadedWantedPath = false;

	tWString sErrorMessageExtra;
	
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
		pConfig = hplNew( cConfigFile, (asDefaultPath, gsUserResourceDir) );
#else
		pConfig = hplNew( cConfigFile, (asDefaultPath) );
#endif
	}

	////////////////////////////////////
	// Load the settings config file
	if(pConfig->Load()==false)
	{
		if(bLoadedWantedPath==false &&
			asDefaultPath.empty()) {
			fl_message("Empty default config file in startup config file");
		}
		//msErrorMessage = _W("Failed to load config file!");
		return NULL;
	}

	//Set correct path for saving.
	if(bLoadedWantedPath==false)
	{
		pConfig->SetFileLocation(asWantedPath);
	}

	return pConfig;
}

//--------------------------------------------------------------------------------

bool LoadLanguage(cEngine* apEngine, const tString& asName, bool abForceReload)
{
	cResources *pResources = apEngine->GetResources();

	////////////////////////////////////////////
	//Check if the language is already loaded.
	//tString sLowName = cString::ToLowerCase(asName);
	//if(gsCurrentLanguage == sLowName && abForceReload==false) return false;

	//if(gsCurrentLanguage != "")
	//{
		pResources->ClearTranslations();	
	//}

	//msCurrentLanguage = sLowName;

	tString sGameFileName = cString::SetFileExt(asName,"lang");
	tString sBaseFileName = "base_"+sGameFileName;

	////////////////////////////////////////////
	//Load the language files
	pResources->AddLanguageFile(gsBaseLanguageFolder + sBaseFileName, true);

    
	////////////////////////////////////////////
	//If not default language, add default to so only missing entries are filled in
    if(sGameFileName != gsDefaultGameLanguage)	
#ifdef USERDIR_RESOURCES
		pResources->AddLanguageFile(gsGameLanguageFolder + gsDefaultGameLanguage, false, gsUserResourceDir);
#else
		pResources->AddLanguageFile(gsGameLanguageFolder + gsDefaultGameLanguage, false);
#endif

	if(sBaseFileName != gsDefaultBaseLanguage)
#ifdef USERDIR_RESOURCES
	pResources->AddLanguageFile(gsBaseLanguageFolder + gsDefaultBaseLanguage, false,gsUserResourceDir);
#else
	pResources->AddLanguageFile(gsBaseLanguageFolder + gsDefaultBaseLanguage, false);
#endif
    
	return true;
}

#if USE_SDL2
#include <SDL2/SDL.h>

tString SDL2GetRenderer() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* sdlwin = SDL_CreateWindow("GL Info",0, 0, 50, 50, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    SDL_GLContext sdlctx = SDL_GL_CreateContext(sdlwin);
    tString ret;
    const char* pCardString = (const char*)glGetString(GL_RENDERER);
    if (pCardString)
    {
        ret = pCardString;
    }
    SDL_GL_DeleteContext(sdlctx);
    SDL_DestroyWindow(sdlwin);
    return ret;
}
#elif defined __linux__
tString LinuxGetRenderer() {
	Display *dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "Error: unable to open the display %s\n", XDisplayName(NULL));
		return "";
	}

	int attributeSingle[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		None };
	int attributeDouble[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		None };

	XVisualInfo *visinfo;
	GLXContext ctx = NULL;

	visinfo = glXChooseVisual(dpy,0, attributeSingle);
	if (!visinfo)
		visinfo = glXChooseVisual(dpy, 0, attributeDouble);

	if (visinfo)
		ctx = glXCreateContext(dpy, visinfo, NULL, True);

	if (!visinfo) {
		fprintf(stderr, "Error: couldn't find RGB GLX visual\n");
		return "";
	}

	if (!ctx) {
		fprintf(stderr, "Error: glXCreateContext failed\n");
		XFree(visinfo);
		return "";
	}

	XSetWindowAttributes attr;
	unsigned long mask;
	Window root, win;
	int width = 100, height = 100;

	root = RootWindow(dpy, 0);

	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	win = XCreateWindow(dpy, root, 0, 0, width, height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr);

	tString ret = "";
	if (glXMakeCurrent(dpy, win, ctx)) {
		const char *pCardString = (const char *)glGetString(GL_RENDERER);
		if (pCardString) {
			ret = tString(pCardString);
		}
	}

	glXDestroyContext(dpy, ctx);
	XFree(visinfo);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	return ret;
}
#endif
//--------------------------------------------------------------------------------

int hplMain(const tString &asCommandLine)
{
	tWString sParams = _W(""); //REMOVE COMMAND LINE AS FIX FOR EPIC GAMES cString::To16Char(asCommandLine);
	tWString sCfgFileDefault = _W("config/main_init.cfg");
	tWString sCfgFile;
	if(sParams==_W("ptest"))
		sCfgFile = _W("config/ptest_main_init.cfg");
	else if(sParams.empty())
		sCfgFile = sCfgFileDefault;
	else
		sCfgFile = sParams;

	if(InitPaths(sCfgFile, sCfgFileDefault)==false)
		return -1;

	// Create video card database handler
	cQualityChooser* pChooser = hplNew(cQualityChooser,("launcher/launcher_card_database.cfg"));

#if USE_SDL2
    tString sCardString = SDL2GetRenderer();
#elif defined WIN32
	// Temp GLUT retrieval of the card string.
	int argc = 1;
	char *argv[] = { "" };
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE);
	int lWin = glutCreateWindow("");
	const GLubyte* pCardString = glGetString(GL_RENDERER);
	tString sCardString;
	if(pCardString)
		sCardString = tString((const char*)pCardString);
	glutDestroyWindow(lWin);
#elif defined(__linux__)
	tString sCardString = LinuxGetRenderer();
#endif

	tWString sConfigFilePath = gsBaseSavePath + _W("main_settings.cfg");

	bool bConfigFileExists = cPlatform::FileExists(sConfigFilePath);
	bool bLastInitCrashed = cPlatform::FileExists(gsCrashFlagPath);
	/////////////////////////////////////////////////
	// Load the main settings
	cConfigFile* pMainConfig = LoadConfigFile(gsDefaultMainConfigPath, sConfigFilePath, bLastInitCrashed);
	if(pMainConfig==NULL) return -1;

	cEngineInitVars vars;
	vars.mSound.mlSoundDeviceID = pMainConfig->GetInt("Sound", "Device", -1);
	//////////////////////////////////////////////////////////////////////////
	// If sound devices should be filtered, set this before creating engine
	#if defined(WIN32)
	iLowLevelSound::SetSoundDeviceNameFilter("software");
	#endif

	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_Video, &vars);
	
	/////////////////////////////////////////////////
	// Load presets
	gvPresets.push_back(hplNew(cConfigFile, (gsDefaultMainConfigPathLow)));
	gvPresets.push_back(hplNew(cConfigFile, (gsDefaultMainConfigPathMedium)));
	gvPresets.push_back(hplNew(cConfigFile, (gsDefaultMainConfigPathHigh)));
	gvPresets.push_back(NULL);
	
	for(int i=0;i<(int)gvPresets.size();++i)
	{
		if(gvPresets[i]) gvPresets[i]->Load();
	}

	//////////////////////////////////////////////////
	// Run FLTK window in a loop
	int exitflag = -1;
	bool bShowLauncher = pMainConfig->GetBool("Main", "ShowLauncher", true);

	if(bShowLauncher)
	{
		while(exitflag==-1)
		{
			LoadLanguage(gpEngine, pMainConfig->GetString("Main", "StartLanguage", "english.lang"), true);
			cUserInterface* win = hplNew(cUserInterface,(exitflag, pMainConfig, bConfigFileExists, bLastInitCrashed, sCardString, pChooser, gvPresets, gpEngine ));

			int ret = win->Run();
			if(ret) 
			{

			} 
			else 
			{
				ret = exitflag;
			}

			hplDelete(win);
		}
	}

	////////////////////////////////////////////
	// Set up game exe
	tWString sGameExe;
#ifdef WIN32
	sGameExe = _W("Amnesia.exe");
#elif __linux__
	#if (defined(i386) && !defined(__LP64__))
		sGameExe = _W("./Amnesia.bin.x86");
	#else
		sGameExe = _W("./Amnesia.bin.x86_64");
	#endif
#endif


	//////////////////////////////
	// PTEST arg
	if(exitflag==2)
		sParams = _W("ptest");

	///////////////////////////////
	// Call program
    if(exitflag!=0)
		cPlatform::RunProgram(sGameExe, sParams);
		
	///////////////////////////////////////
	// Clean up
	DestroyHPLEngine(gpEngine);

	hplDelete(pMainConfig);
	hplDelete(pChooser);

	return 0;
}
