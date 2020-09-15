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
//  LauncherAppDelegate.h
//  Launcher
//
//  Created by Edward Rudd on 3/20/09.
//  Copyright 2009 OutOfOrder.cc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "resources/ConfigFile.h"
#import "../launcher/QualityChooser.h"
#import "resources/LanguageFile.h"

@interface LauncherAppDelegate : NSObject<NSApplicationDelegate> {
	IBOutlet NSWindow *mainWindow;
	
	IBOutlet NSWindow *customResolutionWindow;

	IBOutlet NSTextField *_customWidth, *_customHeight;

	IBOutlet NSTabView *_tabView;
	NSTabViewItem *_editorTab;

	IBOutlet NSObjectController *_labels;
	IBOutlet NSObjectController *_selected;
	
	IBOutlet NSArrayController *_languages;
	IBOutlet NSArrayController *_resolutions;
	IBOutlet NSArrayController *_audiodevices;

	hpl::cConfigFile *_mainConfig;
	cQualityChooser *mpQualityChooser;
	cLanguageFile *_langFile;
	NSDictionary *_settingsMap;

	NSArray *_editorLaunchItems;

	hpl::cConfigFile* mvPresets[3];

	BOOL mbSettingPreset;
}

- (void)launchEditorButton:(NSButton *)sender;

- (IBAction)openCustomStories:(NSButton *)sender;
- (IBAction)openPDFButton:(NSButton *)sender;
- (IBAction)launchButton:(id)sender;
- (IBAction)advancedButton:(id)sender;
- (IBAction)detectButton:(id)sender;
- (IBAction)enterCustomResolution:(id)sender;
- (IBAction)dismissCustomResolution:(id)sender;
- (IBAction)addCustomResolution:(id)sender;

@end
