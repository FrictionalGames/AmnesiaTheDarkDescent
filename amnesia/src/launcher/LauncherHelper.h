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

#ifndef LAUNCHER_HELPER_H
#define LAUNCHER_HELPER_H

#include <vector>
#include "hpl.h"

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

#define kTranslate(sCategory, sEntry) gpEngine->GetResources()->Translate(sCategory, sEntry).c_str()

using namespace hpl;

extern cEngine* gpEngine;

class cUserInterface;
class cQualityChooser;

class iPresetValue
{
public:
	iPresetValue(const tString& asCat, const tString& asName);

	//virtual void UpdateConfigFile(cConfigFile* apFile)=0;

	tString msCat;
	tString msName;
};

class cPresetValueInt : public iPresetValue
{
public:
	cPresetValueInt(const tString& asCat, const tString& asName, int alDefault);

	int mlDefaultValue;
};

class cPresetValueFloat : public iPresetValue
{
	public:
	cPresetValueFloat(const tString& asCat, const tString& asName, float afDefault);

	float mfDefaultValue;
};

class cPresetValueBool : public iPresetValue
{
	public:
	cPresetValueBool(const tString& asCat, const tString& asName, bool abDefault);

	bool mbDefaultValue;
};

struct AspectRatio {
	AspectRatio() : w(0), h(0) {}
	AspectRatio(int aW, int aH) : w(aW), h(aH) {}
	int w;
	int h;
};

class cLauncherHelper
{
public:
	static void SetWidgetLabel(Fl_Widget* apWidget, const tWString &asLabel);
	static void SetMenuItemLabel(Fl_Menu_Item* apItem, const tWString &asLabel, tStringList& alstLabels);

	static tVideoModeVec GetAvailableVideoModes(cConfigFile* apFile);
	static tStringVec GetAvailableLanguages(const tString& asGameLangFolder);
	static tSoundDeviceVec GetAvailableSoundDevices();
	static int GetEqualPresetIndex(cConfigFile* apConfig, const std::vector<cConfigFile*>& avPresets);
	static bool EqualsPreset(cConfigFile* apFile, cConfigFile* apPresetFile);

	static void UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile,
							const tString& asLevel, const tString& asEntry, const tString& asValue, bool abCheckEqualsPreset=true);

	static void UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile, 
							const tString& asLevel, const tString& asEntry, bool abValue, bool abCheckEqualsPreset=true);

	static void UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile,
							const tString& asLevel, const tString& asEntry, int alValue, bool abCheckEqualsPreset=true);

	static void UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile, 
							const tString& asLevel, const tString& asEntry, float afValue, bool abCheckEqualsPreset=true);

	static void UpdateConfigFileWithPreset(cConfigFile* apFile, cConfigFile* apPreset);


	static void PopulateLanguages(const tStringVec& avLangFiles, const tString& asDefaultLang, cConfigFile* apConfig, Fl_Choice* apLangs);
	static void PopulateResolutions(const tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes, cUserInterface* apUI);
	static void PopulateSoundDevices(const tSoundDeviceVec& avSoundDevices, iSoundDeviceIdentifier* apCurrentDev, cConfigFile* apConfig, Fl_Choice* apSndDevs);

	//Extra Resolution Add Magic
	static void ReselectResolution(const tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes);
	static void cb_AddResolution(Fl_Widget*, void*);
	static bool AddCustomResolution(tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes, cUserInterface* apUI, int apWidth, int apHeight);

	static void SetInitialSSAOSamples(cConfigFile* apConfig, Fl_Choice* apSamples);
	static void SetInitialShadowChoice(cConfigFile* apConfig, Fl_Choice* apShadows, const tString& asWhat);
	static void SetInitialAnisotropy(cConfigFile* apConfig, Fl_Choice* apAni);

	static void SetInitialValues(cConfigFile* apConfig, cUserInterface* apUI);

	static void SetUpWidgetLabels(cUserInterface* apUI, tStringList& alstLabels);


	static void DetectSettings(cUserInterface* apUI, cConfigFile* apConfig, const std::vector<cConfigFile*>& avPresets, 
									 cQualityChooser* apChooser, const tString& asCardString);

	static AspectRatio AspectRatioForResolution(int w, int h);
};

#endif
