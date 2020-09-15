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

//
//  LauncherAppDelegate.m
//  Launcher
//
//  Created by Edward Rudd on 3/20/09.
//  Copyright 2009 OutOfOrder.cc. All rights reserved.
//

#import "LauncherAppDelegate.h"

#import <AL/al.h>
#import <AL/alc.h>
#import <AL/alext.h>

#import "system/Platform.h"
#import "system/String.h"

#import "HPLMinimal.h"

#include <SDL2/SDL.h>

using namespace hpl;

static inline void EmptyNSArrayController(NSArrayController *a) {
	[a removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [[a arrangedObjects] count])]];
}

static inline void EmptyNSArray(NSMutableArray *a) {
	[a removeObjectsInRange:NSMakeRange(0, [a count])];
}

hpl::tWString gsPersonalDir;
#ifdef USERDIR_RESOURCES
hpl::tWString gsUserResourceDir;
#endif

hpl::tWString gsCfgFile;
NSString* gsMapName;

hpl::tWString gsMainSaveFolder;

hpl::tString gsDefaultBaseLanguage;
hpl::tString gsDefaultGameLanguage;

hpl::tString gsBaseLanguageFolder;
hpl::tString gsGameLanguageFolder;
hpl::tString gsCustomStoriesPath;

hpl::tWString gsDefaultMainConfigPath;
hpl::tWString gsDefaultMainConfigPathLow;
hpl::tWString gsDefaultMainConfigPathMedium;
hpl::tWString gsDefaultMainConfigPathHigh;
hpl::tString gsGameName;
hpl::tWString gsBaseSavePath;
hpl::tWString gsCrashFlagPath;

hpl::tString gsGLRendererString;

NSString *gsEditorSuiteFolder = nil;

#include "../game/LuxBasePersonal.h"

@interface LauncherAppDelegate (Private)

- (void)fetchOpenALDevices;
- (void)fetchDisplayModes;
- (void)fetchLanguages;
- (bool)LoadLanguage:(const tString&)asName;
- (void)updateLanguage;
- (void)loadSettings;
- (void)CheckEqualsPreset;
- (void)LoadPreset:(cConfigFile *)apPreset;
- (NSString *)Translate:(NSString *) asName withCategory: (NSString *) asCat;

@end

#define kTranslate(aCategory, aName) [self Translate:aName withCategory:aCategory]

#if (defined(__PPC__) || defined(__ppc__))
#define WCHAR_ENCODING NSUTF32BigEndianStringEncoding
#else
#define WCHAR_ENCODING NSUTF32LittleEndianStringEncoding
#endif

static inline NSString *NSStringFromWchar(const tWString& asStr)
{
	return [[[NSString alloc] initWithBytes:asStr.c_str() length:asStr.size()*4 encoding:WCHAR_ENCODING] autorelease];
}

static inline NSString *NSStringFromchar(const tString& asStr)
{
	return [NSString stringWithUTF8String:asStr.c_str()];
}

#pragma mark - AspectRatio

struct AspectRatio {
    AspectRatio() : w(0), h(0) {}
    AspectRatio(int aW, int aH) : w(aW), h(aH) {}
	int w;
	int h;
};

AspectRatio AspectRatioForResolution(int w, int h) {
    if (w == 0 || h == 0) return AspectRatio();
	int a = w, b = h;
	for (int r = a % b;
		 r != 0;
		 a = b, b = r, r = a % b);
	
	AspectRatio ret(w / b, h / b);
	if (ret.w == 8 && ret.h == 5) {
		ret.w = 16;
		ret.h = 10;
	}
	return ret;
}

#pragma mark - HPLVideoMode

@interface HPLVideoMode : NSObject {
@private
    cVideoMode _mode;
}
- (id)initWithVideoMode:(const cVideoMode&)mode;

- (BOOL)custom;
- (NSInteger)display;
- (NSInteger)width;
- (NSInteger)height;
- (NSString*)description;

@end

@implementation HPLVideoMode

- (id)initWithVideoMode:(const hpl::cVideoMode &)mode
{
    self = [super init];
    if (self) {
        _mode = mode;
    }
    return self;
}

- (BOOL)custom
{
    return _mode.mbCustom ? YES : NO;
}

- (NSInteger)display
{
    return _mode.mlDisplay;
}

- (NSInteger)width
{
    return _mode.mvScreenSize.x;
}

- (NSInteger)height
{
    return _mode.mvScreenSize.y;
}

- (NSString *)Translate:(NSString *) asName withCategory: (NSString *) asCat
{
    return [(LauncherAppDelegate*)[NSApp delegate] Translate: asName withCategory: asCat];
}

- (NSString*)description
{
    NSString *label;
    AspectRatio ratio = AspectRatioForResolution(_mode.mvScreenSize.x, _mode.mvScreenSize.y);
    if (_mode.mbCustom)
    {
        label = [NSString stringWithFormat:@"%dx%d [%d:%d] (%@)",
                 _mode.mvScreenSize.x, _mode.mvScreenSize.y, ratio.w, ratio.h,
                 kTranslate(@"Launcher", @"Custom")];
    } else if (_mode.isFullScreenDesktop()) {
        label = [NSString stringWithFormat:@"%@ %s",
                 kTranslate(@"OptionsMenu", @"FullScreenDesktop"),
                 SDL_GetDisplayName(_mode.mlDisplay)];
    } else {
        label = [NSString stringWithFormat:@"%dx%d [%d:%d]",
                 _mode.mvScreenSize.x, _mode.mvScreenSize.y, ratio.w, ratio.h];
    }
    return label;
}

@end

#pragma mark - Config Loading

cConfigFile* LoadConfigFile(const tWString& asDefaultPath, const tWString& asWantedPath, bool abForceLoadDefault)
{
	cConfigFile *pConfig;
	bool bLoadedWantedPath = false;
	
	//////////////////////
	//Check if wanted exist and created config using existing file
	if(abForceLoadDefault==false && cPlatform::FileExists(asWantedPath))
	{
		pConfig = new cConfigFile(asWantedPath);
		bLoadedWantedPath = true;
	}
	else
	{
		pConfig = new cConfigFile(asDefaultPath
#ifdef USERDIR_RESOURCES
								  ,gsUserResourceDir
#endif
								  );
	}
	
	////////////////////////////////////
	// Load the settings config file
	if(pConfig->Load()==false)
	{
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

bool InitPaths(const hpl::tWString &asConfigfile) {
	hpl::cConfigFile *pInitCfg = new hpl::cConfigFile(asConfigfile);
	if (pInitCfg->Load()==false) {
		delete pInitCfg;
		return false;
	}
	//Set the name of the folder (in Lux) that all save stuff will be put.
	gsMainSaveFolder = pInitCfg->GetStringW("Directories","MainSaveFolder",_W(""));
	
	
	//Get the config file paths
#if USE_SDL2
	gsDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsSDL2",_W(""));
#else
	gsDefaultMainConfigPath = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettings",_W(""));
#endif
	
	// Get presets paths
	gsDefaultMainConfigPathLow = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsLow",_W(""));
	gsDefaultMainConfigPathMedium = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsMedium",_W(""));
	gsDefaultMainConfigPathHigh = pInitCfg->GetStringW("ConfigFiles", "DefaultMainSettingsHigh",_W(""));
	
	gsDefaultBaseLanguage = pInitCfg->GetString("ConfigFiles", "DefaultBaseLanguage", "");
	gsDefaultGameLanguage = pInitCfg->GetString("ConfigFiles", "DefaultGameLanguage", "");
	
	//Directories
	gsBaseLanguageFolder = pInitCfg->GetString("Directories","BaseLanguageFolder","");
	gsGameLanguageFolder = pInitCfg->GetString("Directories","GameLanguageFolder","");
    gsCustomStoriesPath = pInitCfg->GetString("Directories","CustomStoryPath","");
	
	//Various variables
	gsGameName = pInitCfg->GetString("Variables","GameName","");
	
	delete pInitCfg;
	
	//////////////////////////////
	//Set up the directories to be created
    tWStringVec vDirs;
    hpl::SetupBaseDirs(vDirs, PERSONAL_RELATIVEGAME_PARENT, gsMainSaveFolder
#ifdef USERDIR_RESOURCES
                                           , true, hpl::cString::To16Char(gsCustomStoriesPath)
#endif
                                           );
    //Create directories
    hpl::CreateBaseDirs(vDirs, gsPersonalDir);
	
	//Set the base directory from which all saving will take place.
	gsBaseSavePath = gsPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT + gsMainSaveFolder + _W("/");
	
	//Set Crash flag file path
	gsCrashFlagPath = gsBaseSavePath + _W("crash_flag");
	
	return true;
}

SDL_Window* sdlwin = 0;
SDL_GLContext sdlcontext = 0;

#pragma mark - Main App Delegate

@implementation LauncherAppDelegate

- (BOOL)windowShouldClose:(id)window
{
	if ([window isEqualTo:mainWindow]) {
		[[NSApplication sharedApplication] terminate:window];
	}
	return YES;
}

#pragma mark - Main Init and DeInit

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [mainWindow makeKeyAndOrderFront:self];

    SDL_GL_DeleteContext(sdlcontext);
    SDL_DestroyWindow(sdlwin);
}

// Use awakeFromNib to get "Early" initialization before the window is displayed
- (void)awakeFromNib {
    // Set defailt conig file
	gsCfgFile = L"config/main_init.cfg";

    bool setCWD = true;

	// Load this up as a global to search alternate resource path
	gsPersonalDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);
#ifdef USERDIR_RESOURCES
	gsUserResourceDir = gsPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES;
#endif

    // parse command line args
	NSArray *args = [[NSProcessInfo processInfo] arguments];
	for (NSInteger i=1,l=[args count]; i<l; ++i) {
		NSString *arg = [args objectAtIndex:i];
        if ([arg isEqualToString: @"-cwd"]) {
            setCWD = false;
        } else {
            tWString sTemp = hpl::cString::To16Char([arg UTF8String]);
            if (sTemp == L"ptest") {
                sTemp = L"config/ptest_main_init.cfg";
            }
            if (hpl::cPlatform::FileExists(sTemp)) {
                gsCfgFile = sTemp;
            }
#   ifdef USERDIR_RESOURCES
            if (hpl::cPlatform::FileExists(gsUserResourceDir + sTemp)) {
                gsCfgFile = sTemp;
            }
#   endif
        }
    }

    if (setCWD) {
        tString dataDir = cPlatform::GetDataDir();
        [[NSFileManager defaultManager] changeCurrentDirectoryPath:[NSString stringWithUTF8String:dataDir.c_str()]];
    }

	_mainConfig = NULL;
	mpQualityChooser = NULL;
	memset(mvPresets,0,sizeof(mvPresets));

	[_languages setContent:[NSMutableArray array]];
	[_resolutions setContent:[NSMutableArray array]];
	[_audiodevices setContent:[NSMutableArray array]];
	[_labels setContent:[NSMutableDictionary dictionary]];
	[_selected setContent:[NSMutableDictionary dictionary]];

	_settingsMap = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"Startup" ofType:@"plist"]];

	NSString *editorFile = @"EditorSuite.plist";
    NSString *levelEditor = nil;
    OSStatus rc;
    CFURLRef url = NULL;
    NSString *bID = @"com.frictionalgames.HPL2.LevelEditor";
    rc = LSFindApplicationForInfo(kLSUnknownCreator, (CFStringRef)bID, NULL, NULL, &url);
    if (rc == noErr) {
        levelEditor = [(NSURL *)url path];
    }
    if (url) CFRelease(url);

    if (levelEditor) {
        NSLog(@"Found Editor Suite: %@", levelEditor);
        gsEditorSuiteFolder = [[levelEditor stringByDeletingLastPathComponent] retain];
        editorFile = [gsEditorSuiteFolder stringByAppendingPathComponent: editorFile];
    }
	if (editorFile) {
		_editorLaunchItems = [[NSArray alloc] initWithContentsOfFile:editorFile];
		if (_editorLaunchItems) {
			// We have editor entries build the editor tab
			_editorTab = [[NSTabViewItem alloc] initWithIdentifier:@"Editor"];
			[_editorTab setLabel:@"Editor Suite"];
			[_tabView addTabViewItem:_editorTab];

			NSRect frame = [_tabView contentRect];
			NSInteger btnMin = frame.size.width / 12;
			NSInteger btnWidth = btnMin * 4;
			NSRect rect = NSMakeRect(btnMin, frame.size.height - frame.origin.y, btnWidth, 26);
			NSLog(@"Frame %@ Rect %@",NSStringFromRect(frame), NSStringFromRect(rect));

			for (NSInteger cur=0,l=[_editorLaunchItems count]; cur<l; ++cur) {
				NSDictionary *dict = [_editorLaunchItems objectAtIndex:cur];

				NSLog(@"Button (%d) - %@",cur,[dict objectForKey:@"Label"]);
				NSButton *btn = [[NSButton alloc] initWithFrame:rect];
				[btn setButtonType:NSMomentaryPushButton];
				[btn setImagePosition:NSNoImage];
				[btn setBezelStyle:NSRoundedBezelStyle];
				[btn setTitle:[dict objectForKey:@"Label"]];
				[btn setTag:cur];
				[btn setTarget:self];
				[btn setAction:@selector(launchEditorButton:)];

				[[_editorTab view] addSubview:btn];
				[btn release];

				rect.origin.y -= 32;
				if (rect.origin.y <= 0) {
					rect.origin.y = frame.size.height - frame.origin.y;
					rect.origin.x += btnMin * 6;
				}
			}
		}
	}


    tWString sCfgFile = gsCfgFile;
#ifdef USERDIR_RESOURCES
    if (hpl::cPlatform::FileExists(gsUserResourceDir + sCfgFile)) {
        sCfgFile = gsUserResourceDir + sCfgFile;
    }
#endif
	if (!hpl::cPlatform::FileExists(sCfgFile) || !InitPaths(sCfgFile)) {
		hpl::cPlatform::CreateMessageBox(hpl::eMsgBoxType_Error, L"Error loading configuration file!",
										 (L"Launcher could not find main init file: "+gsCfgFile).c_str());
		[[NSApplication sharedApplication] terminate:self];
		return;
	}
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    sdlwin = SDL_CreateWindow("GL Info",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          100, 100,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    sdlcontext = SDL_GL_CreateContext(sdlwin);

    const GLubyte* pCardString = glGetString(GL_RENDERER);
    if (pCardString) {
        gsGLRendererString = hpl::tString((const char *)pCardString);
        [[_labels content] setValue:[NSString stringWithCString:(const char*)pCardString encoding:NSASCIIStringEncoding] forKey:@"txtVideoDevice"];
    }
    // Delete the window AFTER the main window is shown

	mpQualityChooser = new cQualityChooser("launcher/launcher_card_database.cfg");

	tWString sConfigFilePath = gsBaseSavePath + _W("main_settings.cfg");

	bool bConfigFileExists = cPlatform::FileExists(sConfigFilePath);
	bool bLastInitCrashed = cPlatform::FileExists(gsCrashFlagPath);
	
	_mainConfig = LoadConfigFile(gsDefaultMainConfigPath, sConfigFilePath, bLastInitCrashed);
	if(_mainConfig==NULL) {
		hpl::cPlatform::CreateMessageBox(hpl::eMsgBoxType_Error, L"Error loading configuration file!",
										 (L"Could not load main config file: "+gsDefaultMainConfigPath).c_str());
		[[NSApplication sharedApplication] terminate:self];
        return;
	}
	
	/////////////////////////////////////////////////
	// Load presets
	mvPresets[0] = hplNew(cConfigFile, (gsDefaultMainConfigPathLow));
	mvPresets[1] = hplNew(cConfigFile, (gsDefaultMainConfigPathMedium));
	mvPresets[2] = hplNew(cConfigFile, (gsDefaultMainConfigPathHigh));
	
	for(int i=0;i<3;++i)
		mvPresets[i]->Load();

	int exitflag=-1;
	bool bShowLauncher = _mainConfig->GetBool("Main", "ShowLauncher", true);

	if (!bShowLauncher && !bLastInitCrashed)
	{
		// Run Amnesia Binary
		[self launchButton:self];
		[[NSApplication sharedApplication] terminate:self];
        return;
	}

	[self LoadLanguage: _mainConfig->GetString("Main", "StartLanguage", gsDefaultGameLanguage)];
	
	[self fetchLanguages];
	// Update the dropdowns
	[self fetchOpenALDevices];
	// Setup Sorting for resolutions
	NSArray *sort = [NSArray arrayWithObjects:
                     [[[NSSortDescriptor alloc] initWithKey:@"custom" ascending:YES] autorelease],
                     [[[NSSortDescriptor alloc] initWithKey:@"display" ascending:YES] autorelease],
					 [[[NSSortDescriptor alloc] initWithKey:@"width" ascending:YES] autorelease],
					 [[[NSSortDescriptor alloc] initWithKey:@"height" ascending:YES] autorelease],
					 nil];
	[_resolutions setSortDescriptors:sort];
	[self fetchDisplayModes];
	
	[self loadSettings];

	[self updateLanguage];
	mbSettingPreset = NO;
	[self CheckEqualsPreset];

	// Setup KVO
	NSInteger opts = NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld;
	NSMutableDictionary *d = [_selected content];
	for (NSString *k in _settingsMap) {
		[d addObserver:self forKeyPath:k options:0 context:[_settingsMap valueForKey:k]];
	}

	// If no config file load Medium preset
	if (!bConfigFileExists) {
		[[_selected content] setValue:[NSNumber numberWithInt:1] forKey:@"Quality"];
		[self detectButton: self];
	}
}

- (void)dealloc {
	[_settingsMap release];
	[_editorLaunchItems release];

	[_customHeight release];
	[_customWidth release];
	[customResolutionWindow release];

	[_labels release];
	[_selected release];
	[_resolutions release];
	[_audiodevices release];

	[_editorTab release];
	[_tabView release];
	[mainWindow release];

    [gsEditorSuiteFolder release];

	if (_mainConfig)
		delete _mainConfig;
	if (mpQualityChooser)
		delete mpQualityChooser;
	if (_langFile)
		delete _langFile;
	if (mvPresets[0])
		delete mvPresets[0];
	if (mvPresets[1])
		delete mvPresets[1];
	if (mvPresets[2])
		delete mvPresets[2];
	[super dealloc];
}

#pragma mark - Fetch Lists

- (void)fetchLanguages {
	tWStringList lstLangs;
	tWString sPath = hpl::cString::To16Char(gsGameLanguageFolder);
#ifdef USERDIR_RESOURCES
	if (cPlatform::FileExists(gsUserResourceDir + sPath)) {
		sPath = gsUserResourceDir + sPath;
	}
#endif
	cPlatform::FindFilesInDir(lstLangs, sPath, _W("*.lang"));

	lstLangs.sort();

	tWStringListIt it = lstLangs.begin();

	EmptyNSArrayController(_languages);
	
	NSMutableArray *a = [NSMutableArray array];

	for(;it!=lstLangs.end();++it)
	{
		tWString sLang = *it;

		NSString *sFile = NSStringFromWchar(sLang);
		NSRange r = [sFile rangeOfString:@"."];
		NSString *key = [sFile substringToIndex:r.location];
		NSMutableDictionary *d = [NSMutableDictionary dictionaryWithObjectsAndKeys:
						   kTranslate(@"Languages",key), @"Label",
						   key, @"Key", 
						   sFile, @"File",
						   nil];
		[a addObject:d];
	}
	// Setup sorting for languages
	NSArray *sort = [NSArray arrayWithObjects:
						 [[[NSSortDescriptor alloc] initWithKey:@"Label" ascending:YES] autorelease],
						 nil];
	[a sortUsingDescriptors:sort];
	[_languages addObjects: a];
}

- (void)fetchOpenALDevices {
	EmptyNSArrayController(_audiodevices);
	
	bool bEnumerate = (alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE);
	bool bEnumerate_all = (alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATE_ALL_EXT") == AL_TRUE);
	if (bEnumerate) {
		const char *s;
		if (bEnumerate_all) {
			// walk devices
			s = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		} else {
			s = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		}
		while (*s != '\0') {
			[_audiodevices addObject:[NSDictionary dictionaryWithObject:[NSString stringWithCString:s encoding:NSASCIIStringEncoding] forKey:@"Label"]];
			// find next string
			while (*s++ != '\0')
				;
		}
	}
}

- (void)fetchDisplayModes {
	EmptyNSArrayController(_resolutions);

    tVideoModeVec vVidModes;
    cPlatform::GetAvailableVideoModes(vVidModes, 32);

    int lDisplay = _mainConfig->GetInt("Screen", "Display", 0);
	int lScreenWidth = _mainConfig->GetInt("Screen", "Width", -1);
	int lScreenHeight = _mainConfig->GetInt("Screen", "Height", -1);
    cVideoMode currMode(lDisplay, cVector2l(lScreenWidth, lScreenHeight), -1, -1);

    int lResFoundIndex = -1;

    if(lScreenWidth==-1 || lScreenHeight==-1)
	{
        _mainConfig->SetInt("Screen", "Display", 0);
		_mainConfig->SetInt("Screen", "Width", -1);
		_mainConfig->SetInt("Screen", "Height", -1);
	}
	else
    {
		for(int i=0;i<(int)vVidModes.size();++i)
		{
			cVideoMode& vMode = vVidModes[i];
			if(vMode==currMode)
			{
				lResFoundIndex = i;
				break;
			}
		}

		if(lResFoundIndex==-1)
		{
			cVideoMode vMode(cVector2l(lScreenWidth, lScreenHeight));

			vVidModes.push_back(vMode);

            lResFoundIndex = int(vVidModes.size() - 1);
		}
    }

    HPLVideoMode *dSelectedRes;

    for (int i=0; i<(int)vVidModes.size(); ++i)
    {
        HPLVideoMode *dinfo = [[[HPLVideoMode alloc] initWithVideoMode:vVidModes[i] ] autorelease];
        NSLog(@"Add Mode: %d - %d,%d %d %d",
              vVidModes[i].mlDisplay,
              vVidModes[i].mvScreenSize.x, vVidModes[i].mvScreenSize.y,
              vVidModes[i].mlBitsPerPixel,
              vVidModes[i].mlRefreshRate);
        if (i == lResFoundIndex) {
            dSelectedRes = dinfo;
        }
        [_resolutions addObject:dinfo];
    }

	NSInteger lSelectedRes = [[_resolutions arrangedObjects] indexOfObject:dSelectedRes];
	[[_selected content] setValue:[NSNumber numberWithInt:lSelectedRes] forKey:@"Resolution"];
}

#pragma mark - Languages

- (bool)LoadLanguage: (const tString&) asName
{
	////////////////////////////////////////////
	//Check if the language is already loaded.
	//tString sLowName = cString::ToLowerCase(asName);
	//if(gsCurrentLanguage == sLowName && abForceReload==false) return false;

	//if(gsCurrentLanguage != "")
	//{
	if (_langFile) {
		delete _langFile;
		_langFile = NULL;
	}

	//msCurrentLanguage = sLowName;

	tString sGameFileName = hpl::cString::SetFileExt(asName,"lang");
	tString sBaseFileName = "base_"+sGameFileName;

	////////////////////////////////////////////
	//Load the language files
	_langFile = new cLanguageFile(NULL);
	_langFile->AddFromFile(hpl::cString::To16Char(gsBaseLanguageFolder + sBaseFileName), false);

	////////////////////////////////////////////
	//If not default language, add default to so only missing entries are filled in
	if(sGameFileName != gsDefaultGameLanguage)
		_langFile->AddFromFile(hpl::cString::To16Char(gsGameLanguageFolder + gsDefaultGameLanguage), false
#ifdef USERDIR_RESOURCES
							   ,gsUserResourceDir
#endif
							   );

	if(sBaseFileName != gsDefaultBaseLanguage)
		_langFile->AddFromFile(hpl::cString::To16Char(gsBaseLanguageFolder + gsDefaultBaseLanguage), false
#ifdef USERDIR_RESOURCES
							   ,gsUserResourceDir
#endif
							   );

	return true;
}

- (NSString *)Translate:(NSString *) asName withCategory: (NSString *) asCat
{
	return NSStringFromWchar(_langFile->Translate([asCat UTF8String], [asName UTF8String]));
}

- (void)updateLanguage {
	NSMutableDictionary *d = [_labels content];
	[d setValue:kTranslate(@"Launcher", @"TabGeneral")		forKey:@"tabGeneral"];
	[d setValue:kTranslate(@"Launcher", @"TabGraphics")		forKey:@"tabGraphics"];
	[d setValue:kTranslate(@"Launcher", @"TabSound")		forKey:@"tabSound"];
	
	[d setValue:kTranslate(@"Launcher", @"DetectSettings")	forKey:@"lblDetect"];
	[d setValue:kTranslate(@"Launcher", @"ShowAdvanced")	forKey:@"lblAdvanced"];
	[d setValue:kTranslate(@"Launcher", @"ShowBasic")		forKey:@"lblBasic"];

	[d setValue:kTranslate(@"Launcher", @"Quality")			forKey:@"lblQuality"];
	[d setValue:kTranslate(@"Launcher", @"VideoDevice")		forKey:@"lblVideoDevice"];
	[d setValue:kTranslate(@"Launcher", @"SoundDevice")		forKey:@"lblDevice"];
	[d setValue:kTranslate(@"Launcher", @"ShadowQuality")	forKey:@"lblShadowQuality"];
	[d setValue:kTranslate(@"Launcher", @"ShadowResolution")forKey:@"lblShadowResolution"];

	[d setValue:kTranslate(@"OptionsMenu", @"TexQuality")	forKey:@"lblTextureQuality"];

	[d setValue:kTranslate(@"OptionsMenu", @"Low")			forKey:@"lblLow"];
	[d setValue:kTranslate(@"OptionsMenu", @"Medium")		forKey:@"lblMedium"];
	[d setValue:kTranslate(@"OptionsMenu", @"High")			forKey:@"lblHigh"];
	[d setValue:kTranslate(@"Launcher", @"Custom")			forKey:@"lblCustom"];

	[d setValue:kTranslate(@"OptionsMenu", @"Language")		forKey:@"lblLanguage"];
	[d setValue:kTranslate(@"OptionsMenu", @"Resolution")	forKey:@"lblResolution"];
	[d setValue:kTranslate(@"OptionsMenu", @"FullScreen")	forKey:@"lblFullScreen"];
	
	[d setValue:kTranslate(@"OptionsMenu", @"Anisotropy")	forKey:@"lblAnisotropy"];
	[d setValue:kTranslate(@"Launcher", @"SSAORes")			forKey:@"lblSSAOResolution"];
	[d setValue:[@"SSAO " stringByAppendingString: kTranslate(@"OptionsMenu", @"SSAOSamples")]
															forKey:@"lblSSAOSamples"];
	[d setValue:kTranslate(@"Launcher", @"EdgeSmooth")		forKey:@"lblEdgeSmooth"];
	[d setValue:kTranslate(@"OptionsMenu", @"TexFilter")	forKey:@"lblTextureFilter"];
	[d setValue:kTranslate(@"OptionsMenu", @"Nearest")		forKey:@"lblNearest"];
	[d setValue:kTranslate(@"OptionsMenu", @"Bilinear")		forKey:@"lblBilinear"];
	[d setValue:kTranslate(@"OptionsMenu", @"Trilinear")	forKey:@"lblTrilinear"];
	[d setValue:kTranslate(@"OptionsMenu", @"WorldReflection")forKey:@"lblWorldReflection"];
	[d setValue:kTranslate(@"Launcher", @"Parallax")		forKey:@"lblParallax"];
	[d setValue:kTranslate(@"Launcher", @"Off")				forKey:@"lblOff"];
	[d setValue:kTranslate(@"Launcher", @"On")				forKey:@"lblOn"];
	[d setValue:kTranslate(@"Launcher", @"LaunchGame")		forKey:@"lblLaunch"];
	[d setValue:kTranslate(@"Global", @"Cancel")			forKey:@"lblCancel"];

	for (d in [_languages content]) {
		[d setValue:kTranslate(@"Languages",[d valueForKey:@"Key"]) forKey:@"Label"];
	}
    [_resolutions rearrangeObjects];
}


#pragma mark - Setting Management

- (void)loadSettings {
	NSMutableDictionary *sel = [_selected content];
	
	{
		NSInteger lLangIndex = -1;
		NSInteger lDefaultLangIndex = -1;
		
		NSString *sCurLang = NSStringFromchar(_mainConfig->GetString("Main", "StartLanguage", gsDefaultGameLanguage));
		NSString *sDefLang = NSStringFromchar(gsDefaultGameLanguage);

		NSArray *a = [_languages content];
		for (NSDictionary *d in a) {
			NSString *sLang = [d valueForKey:@"File"];
			if (lDefaultLangIndex==-1 && [sLang isEqualToString:sDefLang]) {
				lDefaultLangIndex = [a indexOfObjectIdenticalTo:d];
			}
			
			if ([sLang isEqualToString:sCurLang]) {
				lLangIndex = [a indexOfObjectIdenticalTo:d];
				break;
			}
		}
		if (lLangIndex==-1) {
			lLangIndex = lDefaultLangIndex;
		}
		[sel setValue: [NSNumber numberWithInt:lLangIndex] forKey:@"Language"];
	}
	{
		NSInteger lSoundDev = _mainConfig->GetInt("Sound", "Device", -1);
		if (lSoundDev == -1 || lSoundDev >= (NSInteger)[[_audiodevices content] count]) {
			tString sDev = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
			lSoundDev = [[_audiodevices content] indexOfObject:NSStringFromchar(sDev)];
			if (lSoundDev == NSNotFound) {
				lSoundDev = 0;
			}
		}
		[sel setValue: [NSNumber numberWithInt:lSoundDev] forKey:@"OpenALDevice"];
	}

	for (NSString *k in _settingsMap) {
		NSDictionary *o = [_settingsMap valueForKey:k];
		NSString *t = [o valueForKey:@"Type"];
		if ([t isEqualToString: @"Int"]) {
			[sel setValue:[NSNumber numberWithInt:
						_mainConfig->GetInt(
								[[o valueForKey:@"Level"] UTF8String],
								[[o valueForKey:@"Name"] UTF8String],
								[[o valueForKey:@"Default"] intValue]
						    )] forKey:k];
		} else if ([t isEqualToString: @"Bool"]) {
			[sel setValue:[NSNumber numberWithInt:
						   _mainConfig->GetBool(
							    [[o valueForKey:@"Level"] UTF8String],
							    [[o valueForKey:@"Name"] UTF8String],
								[[o valueForKey:@"Default"] boolValue]?true:false
							)?1:0] forKey:k];
		} else if ([t isEqualToString: @"Float"]) {
			NSInteger val = (int)_mainConfig->GetFloat(
													 [[o valueForKey:@"Level"] UTF8String],
													 [[o valueForKey:@"Name"] UTF8String],
													 [[o valueForKey:@"Default"] floatValue]
													 );
			if (val < 1 && [k isEqualToString:@"TextureAnisotropy"]) {
				val = 1;
			}
			[sel setValue:[NSNumber numberWithInt:val] forKey:k];
		} else {
			//NSLog(@"Did we handle Type %@ for Key %@",t,k);
		}
	}
}

- (void)saveSettings {
	_mainConfig->Save();
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	//NSLog(@"Observer %@: %@",keyPath, change);
	NSDictionary *o = (NSDictionary *)context;
	NSDictionary *sel = [_selected content];

	NSString *t = [o valueForKey:@"Type"];
	if ([t isEqualToString: @"Int"] || [t isEqualToString: @"OpenALDevice"]) {
		_mainConfig->SetInt(
							 [[o valueForKey:@"Level"] UTF8String],
							 [[o valueForKey:@"Name"] UTF8String],
							 [[sel valueForKey:keyPath] intValue]
							 );
	} else if ([t isEqualToString: @"Bool"]) {
		_mainConfig->SetBool(
							 [[o valueForKey:@"Level"] UTF8String],
							 [[o valueForKey:@"Name"] UTF8String],
						     [[sel valueForKey:keyPath] boolValue] ? true: false
							 );
		if ([keyPath isEqualToString:@"Parallax"]) {
			_mainConfig->SetInt("Graphics", "ParallaxQuality", 0);
		}
	} else if ([t isEqualToString: @"Float"]) {
		_mainConfig->SetFloat(
							 [[o valueForKey:@"Level"] UTF8String],
							 [[o valueForKey:@"Name"] UTF8String],
							 [[sel valueForKey:keyPath] floatValue]
							 );
	} else if ([t isEqualToString: @"Language"]) {
		NSDictionary *d = [[_languages content] objectAtIndex:[[sel valueForKey:keyPath] intValue]];

		//NSLog(@"Language ID:%@",[d valueForKey:@"File"]);
		_mainConfig->SetString(
							  [[o valueForKey:@"Level"] UTF8String],
							  [[o valueForKey:@"Name"] UTF8String],
							  [[d valueForKey:@"File"] UTF8String]
							  );
		[self LoadLanguage:[[d valueForKey:@"File"] UTF8String]];
		[self updateLanguage];
	} else if ([t isEqualToString: @"Resolution"]) {
		HPLVideoMode *d = [[_resolutions arrangedObjects] objectAtIndex:[[sel valueForKey:keyPath] intValue]];
        _mainConfig->SetInt("Screen", "Display", [d display]);
		_mainConfig->SetInt("Screen", "Width", [d width]);
		_mainConfig->SetInt("Screen", "Height", [d height]);
	} else if ([t isEqualToString: @"Quality"]) {
		NSInteger qNum = [[sel valueForKey:keyPath] intValue];
		if (qNum < 3) {
			[self LoadPreset:mvPresets[qNum]];
		}
	} else {
		NSLog(@"Did we handle Saving Type %@ for Key %@",t,keyPath);
	}

	NSNumber *preset = [o valueForKey:@"Preset"];
	if (preset && [preset boolValue]==YES) {
		[self CheckEqualsPreset];
	}
}

- (void)CheckEqualsPreset {
	if(mbSettingPreset)
		return;

	cConfigFile* pPreset;

	bool bEqualsPreset;

	for(int i=0;i<3;++i)
	{
		pPreset = mvPresets[i];
		bEqualsPreset = true;
//		NSLog(@"Checking Preset %d",i);

		for (NSString *k in _settingsMap) {
			NSDictionary *d = [_settingsMap valueForKey:k];
			NSNumber *preset = [d valueForKey:@"Preset"];
			if (preset && [preset boolValue]==YES) {
				NSString *level = [d valueForKey:@"Level"];
				NSString *name = [d valueForKey:@"Name"];
				if (!level || !name) continue;
//				NSLog(@"Checking: %@/%@", level, name);
				tString sLevel = [level UTF8String];
				tString sName = [name UTF8String];
				NSString *t = [d valueForKey:@"Type"];
				if ([t isEqualToString:@"Int"]) {
					NSInteger def = [[d valueForKey:@"Default"] intValue];
					bEqualsPreset = bEqualsPreset 
							&& (_mainConfig->GetInt(sLevel,sName, def) == pPreset->GetInt(sLevel,sName, def));
//					NSLog(@"  %d :: %d == %d",bEqualsPreset , _mainConfig->GetInt(sLevel,sName, def), pPreset->GetInt(sLevel,sName, def));
				} else if ([t isEqualToString:@"Bool"]) {
					bool def= [[d valueForKey:@"Default"] boolValue];
					bEqualsPreset = bEqualsPreset 
							&& (_mainConfig->GetBool(sLevel,sName, def) == pPreset->GetBool(sLevel,sName, def));
//					NSLog(@"  %d :: %d == %d",bEqualsPreset,_mainConfig->GetBool(sLevel,sName, def), pPreset->GetBool(sLevel,sName, def));
				} else if ([t isEqualToString:@"Float"]) {
					float def = [[d valueForKey:@"Default"] floatValue];
					bEqualsPreset = bEqualsPreset 
							&& (_mainConfig->GetFloat(sLevel,sName, def) == pPreset->GetFloat(sLevel,sName, def));
//					NSLog(@"  %d :: %f == %f",bEqualsPreset, _mainConfig->GetFloat(sLevel,sName, def), pPreset->GetFloat(sLevel,sName, def));
				}
				if (!bEqualsPreset) {
					break;
				}
			}
		}

		if(bEqualsPreset)
		{
			[[_selected content] setValue:[NSNumber numberWithInt:i] forKey:@"Quality"];
			return;
		}
	}

	if(bEqualsPreset==false)
		[[_selected content] setValue:[NSNumber numberWithInt:3] forKey:@"Quality"];
}

- (void)LoadPreset:(cConfigFile *)apPreset {
	if(apPreset==NULL)
		return;

	mbSettingPreset = true;

	NSDictionary *sel = [_selected content];
	for (NSString *k in _settingsMap) {
		NSDictionary *d = [_settingsMap valueForKey:k];
		NSNumber *preset = [d valueForKey:@"Preset"];
		if (preset && [preset boolValue]==YES) {
			NSString *level = [d valueForKey:@"Level"];
			NSString *name = [d valueForKey:@"Name"];
			if (!level || !name) continue;
			tString sLevel = [level UTF8String];
			tString sName = [name UTF8String];
			NSString *t = [d valueForKey:@"Type"];
			if ([t isEqualToString:@"Int"]) {
				NSInteger def = [[d valueForKey:@"Default"] intValue];
				[sel setValue:[NSNumber numberWithInt:apPreset->GetInt(sLevel,sName, def)] forKey: k];
			} else if ([t isEqualToString:@"Bool"]) {
				bool def= [[d valueForKey:@"Default"] boolValue];
				[sel setValue:[NSNumber numberWithInt:apPreset->GetBool(sLevel,sName, def)?1:0] forKey: k];
			} else if ([t isEqualToString:@"Float"]) {
				float def = [[d valueForKey:@"Default"] floatValue];
				[sel setValue:[NSNumber numberWithInt:(int)apPreset->GetFloat(sLevel,sName, def)] forKey: k];
			}
		}
	}

	mbSettingPreset = false;
}

#pragma mark - Button Actions

- (void)launchEditorButton:(NSButton *)sender {
	NSLog(@"Button %@ with Tag %d",sender, [sender tag]);
	NSDictionary *dict = [_editorLaunchItems objectAtIndex:[sender tag]];
	NSString *exeName = [dict objectForKey:@"Executable"];
	NSString *folderName = [dict objectForKey:@"Folder"];
    NSString *methodName = [dict objectForKey:@"Method"];
	NSFileManager *fm = [NSFileManager defaultManager];
	NSString *cwd = [fm currentDirectoryPath];
	BOOL ret = NO;
	if (exeName) {
		NSString *exePath = [[gsEditorSuiteFolder stringByAppendingPathComponent:exeName] stringByStandardizingPath];
		if ([fm fileExistsAtPath:exePath]) {
			NSLog(@"Launched Editor Addon %@ via %@",[dict objectForKey:@"Label"], exePath);
			ret =  [[NSWorkspace sharedWorkspace] launchApplication:exePath];
		}
	} else if (folderName) {
		NSString *folderPath = [[cwd stringByAppendingPathComponent:folderName] stringByStandardizingPath];
		if ([fm fileExistsAtPath:folderPath]) {
			NSLog(@"Launched Editor Addon %@ via %@",[dict objectForKey:@"Label"], folderPath);
			ret =  [[NSWorkspace sharedWorkspace] openFile:folderPath];
		}
	} else if (methodName) {
        if ([methodName isEqualToString:@"openCustomStories"]) {
            [self openCustomStories:sender];
            ret = YES;
        }
    }
	if (!ret) {
		NSAlert *alert = [NSAlert alertWithMessageText:kTranslate(@"Global",@"Warning")
										 defaultButton:kTranslate(@"Global",@"OK") // will default to localized OK
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Could not start HPL Tool %@.",[dict objectForKey:@"Label"]];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
}

- (IBAction)launchButton:(NSButton *)sender {
	[self saveSettings];
	
	NSWorkspace *ws = [NSWorkspace sharedWorkspace];
	NSFileManager *fm = [NSFileManager defaultManager];
	NSBundle *bundle = [NSBundle mainBundle];
	NSString *gamePathLoc = [bundle objectForInfoDictionaryKey:@"GamePath"];
	NSString *cwd = [fm currentDirectoryPath];
	OSStatus ret = noErr;
	if (gamePathLoc) {
		NSString *exe = [[cwd stringByAppendingPathComponent: gamePathLoc] stringByStandardizingPath];
		if ([fm fileExistsAtPath:exe]) {
			NSError *err;
			NSLog(@"Launched Amnesia Game Via %@",exe);

            NSMutableArray *args = [NSMutableArray arrayWithObject:@"-cwd"];
            if ([sender tag] == 2) {
                [args addObject: @"ptest"];
            } else {
                [args addObject: [NSString stringWithUTF8String:hpl::cString::To8Char(gsCfgFile).c_str()]];
            }
            NSLog(@"Args %@",args);
            NSTask *task = [NSTask launchedTaskWithLaunchPath:exe arguments:args];
            if (![task isRunning]) {
                ret = 1;
            }
		}
	}
	if (ret == noErr) {
		[[NSApplication sharedApplication] terminate:self];
	} else {
		NSAlert *alert = [NSAlert alertWithMessageText:kTranslate(@"Global",@"Warning")
										 defaultButton:kTranslate(@"Global",@"OK") // will default to localized OK
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"%@", @"Could not start Amnesia."];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
}

- (IBAction)openPDFButton:(NSButton *)sender {
	NSString *pdfFile = @"Remember - Short Story Collection.pdf";
	NSFileManager *fm = [NSFileManager defaultManager];
	NSString *cwd = [fm currentDirectoryPath];

	BOOL ret = NO;
	if (pdfFile) {
		NSString *filePath = [[cwd stringByAppendingPathComponent:pdfFile] stringByStandardizingPath];
		if ([fm fileExistsAtPath: filePath]) {
			NSLog(@"Launched PDF File %@", filePath);
			ret = [[NSWorkspace sharedWorkspace] openFile:filePath];
		}
	}
	if (!ret) {
		NSAlert *alert = [NSAlert alertWithMessageText:kTranslate(@"Global",@"Warning")
										 defaultButton:kTranslate(@"Global",@"OK") // will default to localized OK
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Could not open PDF file %@.", pdfFile];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
}

- (IBAction)openCustomStories:(NSButton *)sender {
	NSFileManager *fm = [NSFileManager defaultManager];
	NSString *cwd = [fm currentDirectoryPath];

	BOOL ret = NO;

	if (gsCustomStoriesPath.length()) {
		NSString *folderPath;
#ifdef USERDIR_RESOURCES
		folderPath = [[NSStringFromWchar(gsUserResourceDir)
						stringByAppendingPathComponent:NSStringFromchar(gsCustomStoriesPath)]
						stringByStandardizingPath];
		if ([fm fileExistsAtPath: folderPath]) {
			NSLog(@"Launch Folder %@", folderPath);
			ret = [[NSWorkspace sharedWorkspace] openFile:folderPath];
		}
#endif
		folderPath = [[cwd stringByAppendingPathComponent:NSStringFromchar(gsCustomStoriesPath)]
					   stringByStandardizingPath];
		if ([fm fileExistsAtPath: folderPath]) {
#ifdef USERDIR_RESOURCES
			NSArray *dirC = [fm contentsOfDirectoryAtPath: folderPath error: NULL];
			if ([dirC count]>0) {
#endif
			NSLog(@"Launch Folder %@", folderPath);
			ret = [[NSWorkspace sharedWorkspace] openFile:folderPath];
#ifdef USERDIR_RESOURCES
			}
#endif
		}
	}
	if (!ret) {
		NSAlert *alert = [NSAlert alertWithMessageText:kTranslate(@"Global",@"Warning")
										 defaultButton:kTranslate(@"Global",@"OK") // will default to localized OK
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Could not open Custom Stories Folder"];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
    }
}

- (IBAction)advancedButton:(id)sender {
	NSNumber *n = [[_selected content] valueForKey:@"GraphicsTab"];
	if ([n intValue] == 0) {
		[[_selected content] setValue:[NSNumber numberWithInt:1] forKey:@"GraphicsTab"];
	} else {
		[[_selected content] setValue:[NSNumber numberWithInt:0] forKey:@"GraphicsTab"];
	}
}

- (IBAction)detectButton:(id)sender {
	tString sClosestMatch;
	eQRating rating = mpQualityChooser->GetQualityRatingByCardString([[[_labels content] valueForKey:@"txtVideoDevice"] UTF8String],sClosestMatch);
	
	NSString *msg = nil;
	if(sClosestMatch.empty()==false)
	{
		msg = [kTranslate(@"Launcher", @"UnlistedVCardClosestMatch") stringByAppendingFormat:@" %s", sClosestMatch.c_str()]; 
	}
	
	if(rating==eQRating_Unknown)
	{
		msg = kTranslate(@"Launcher", @"UnlistedVCard");
		rating = eQRating_Medium;
	}
	
	if(rating==eQRating_Unsupported)
	{
		msg = kTranslate(@"Launcher", @"UnsupportedVCard");
	}
	else
	{
		[[_selected content ] setValue:[NSNumber numberWithInt:rating] forKey:@"Quality"];
	};
	if (msg) {
		NSAlert *alert = [NSAlert alertWithMessageText:kTranslate(@"Global",@"Warning")
										 defaultButton:kTranslate(@"Global",@"OK") // will default to localized OK
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"%@", msg];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
}

- (IBAction)enterCustomResolution:(id)sender {
	NSLog(@"We Be Custom");
	[customResolutionWindow makeFirstResponder:_customWidth];
	[[NSApplication sharedApplication] beginSheet:customResolutionWindow modalForWindow:mainWindow 
									modalDelegate:self didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:self];
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo {
	[sheet orderOut:self];
	NSLog(@"Sheet did End, %d",returnCode);
	if (returnCode == NSOKButton) {
		NSNumber *width = [NSNumber numberWithInteger:[_customWidth integerValue]];
		NSNumber *height = [NSNumber numberWithInteger:[_customHeight integerValue]];

		NSLog(@"Adding resolution %@ x %@",width,height);
		//Loop though existing resolutions
		NSInteger selectedRes = -1;
		for (NSDictionary *d in [_resolutions arrangedObjects]) {
			if ([[d valueForKey:@"Width"] isEqualToNumber:width]
					&& [[d valueForKey:@"Height"] isEqualToNumber:height]) {
				selectedRes = [[_resolutions arrangedObjects] indexOfObject:d];
				break;
			}
		}
		if (selectedRes == -1) {
            cVideoMode vidMode(cVector2l([width intValue], [height intValue]));
            HPLVideoMode *d = [[[HPLVideoMode alloc] initWithVideoMode:vidMode] autorelease];
			[_resolutions addObject:d];
			selectedRes = [[_resolutions arrangedObjects] indexOfObject:d];
		}
		[[_selected content] setValue:[NSNumber numberWithInt:selectedRes] forKey:@"Resolution"];
	}
	[_customWidth setStringValue:@""];
	[_customHeight setStringValue:@""];	
}

- (IBAction)addCustomResolution:(id)sender {
	if ([_customWidth integerValue] < 640) {
		[customResolutionWindow makeFirstResponder:_customWidth];
		return;
	} else if ([_customHeight integerValue] < 480) {
		[customResolutionWindow makeFirstResponder:_customHeight];
		return;
	}
	[[NSApplication sharedApplication] endSheet:customResolutionWindow returnCode:NSOKButton];
}

- (IBAction)dismissCustomResolution:(id)sender {
	[[NSApplication sharedApplication] endSheet:customResolutionWindow returnCode:NSCancelButton];
}
@end
