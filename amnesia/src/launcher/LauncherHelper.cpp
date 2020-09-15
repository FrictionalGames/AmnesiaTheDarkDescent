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

#include "LauncherHelper.h"
#include "hpl.h"
#include "launcher.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>

#ifdef USERDIR_RESOURCES
extern tWString gsUserResourceDir;
#endif

using namespace hpl;

int glDefaultSSAOSamples = 8;

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////
// Default int values
static cPresetValueInt gvIntPresets[] = 
{
	cPresetValueInt("Graphics", "ShadowQuality", 1),
	cPresetValueInt("Graphics", "ShadowResolution", 1),
	cPresetValueInt("Graphics", "TextureQuality", 1),
	cPresetValueInt("Graphics", "TextureFilter", 1),
	cPresetValueInt("Graphics", "SSAOResolution", 1),
	cPresetValueInt("Graphics", "SSAOSamples", glDefaultSSAOSamples),

	cPresetValueInt("", "", 0),
};

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////
// Default float values
static cPresetValueFloat gvFloatPresets[] = 
{
	cPresetValueFloat("Graphics", "TextureAnisotropy", 1.0f),

	cPresetValueFloat("", "", 0.0f),
};

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////
// Default bool values
static cPresetValueBool gvBoolPresets[] =
{
	cPresetValueBool("Graphics", "ParallaxEnabled", true),
	cPresetValueBool("Graphics", "WorldReflection", true),
	cPresetValueBool("Graphics", "SSAOActive", true),
	cPresetValueBool("Graphics", "EdgeSmooth", true),

	cPresetValueBool("", "", false),
};

//------------------------------------------------------------------------------

/*
static tString gvPresetEntries[] =
{
	// Int Values
	"ShadowQuality",
	"ShadowResolution",

	"TextureQuality",
	"TextureFilter",

	"SSAOResolution",
	"SSAOSamples",

	// Float Values


	// numeric values
	"ShadowQuality",
	"ShadowResolution",

	"TextureQuality",
	"TextureFilter",
	"TextureAnisotropy",

	"SSAOResolution",
	"SSAOSamples",
	"",
	
	// boolean values
	"ParallaxEnabled",
	"WorldReflection",
	"SSAOActive",
	"EdgeSmooth",
	""
};
*/

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

iPresetValue::iPresetValue(const tString& asCat, const tString& asName) : msCat(asCat), msName(asName)
{
}

cPresetValueInt::cPresetValueInt(const tString& asCat, const tString& asName, int alDefault) : iPresetValue(asCat, asName), mlDefaultValue(alDefault)
{
}

cPresetValueFloat::cPresetValueFloat(const tString& asCat, const tString& asName, float afDefault) : iPresetValue(asCat, asName), mfDefaultValue(afDefault)
{
}

cPresetValueBool::cPresetValueBool(const tString& asCat, const tString& asName, bool abDefault) : iPresetValue(asCat, asName), mbDefaultValue(abDefault)
{
}

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// LAUNCHER HELPER - Static helper functions
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

void cLauncherHelper::SetWidgetLabel(Fl_Widget* apWidget, const tWString& asLabel)
{
	tString sUTF8Label = cString::S16BitToUTF8(asLabel);
	
	apWidget->copy_label(sUTF8Label.c_str());
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetMenuItemLabel(Fl_Menu_Item* apItem, const tWString &asLabel, tStringList& alstLabels)
{
	tString sUTF8Label = cString::S16BitToUTF8(asLabel);
	
	alstLabels.push_back(sUTF8Label);
	apItem->label(alstLabels.back().c_str());
}

//------------------------------------------------------------------------------

tVideoModeVec cLauncherHelper::GetAvailableVideoModes(cConfigFile* apFile)
{
	tVideoModeVec vVidModes;
	cPlatform::GetAvailableVideoModes(vVidModes, 32);

	int lScreenWidth = apFile->GetInt("Screen", "Width", -1);
    int lScreenHeight = apFile->GetInt("Screen", "Height", -1);
    int lDisplay = apFile->GetInt("Screen", "Display", 0);
    cVideoMode curMode(lDisplay, cVector2l(lScreenWidth, lScreenHeight), 32, 1);

	if(lScreenWidth==-1 || lScreenHeight==-1)
	{
		apFile->SetInt("Screen", "Width", -1);
		apFile->SetInt("Screen", "Height", -1);
		apFile->SetInt("Screen", "Display", 0);
	}
	else
	{
		int lResFoundIndex = -1;
		for(int i=0;i<(int)vVidModes.size();++i)
		{
			cVideoMode& vMode = vVidModes[i];
			if(vMode == curMode)
			{
				lResFoundIndex = i;
				break;
			}
		}
		
		if(lResFoundIndex==-1)
		{
			cVideoMode vMode(cVector2l(lScreenWidth, lScreenHeight));
			
			vVidModes.push_back(vMode);
		}
	}

	return vVidModes;
}

//------------------------------------------------------------------------------

tStringVec cLauncherHelper::GetAvailableLanguages(const tString& asGameLangFolder)
{
	tStringVec vLangFiles;
	////////////////////////////////////
	// Retrieve available languages
	tWStringList lstLangs;
	tWString sPath = cString::To16Char(asGameLangFolder);
#ifdef USERDIR_RESOURCES
	if (cPlatform::FileExists(gsUserResourceDir + sPath)) {
		sPath = gsUserResourceDir + sPath;
	}
#endif
	cPlatform::FindFilesInDir(lstLangs, sPath, _W("*.lang"));

	tWStringListIt it = lstLangs.begin();
	for(;it!=lstLangs.end();++it)
	{
		tWString sLang = *it;

		vLangFiles.push_back(cString::To8Char(sLang));
	}

	return vLangFiles;
}

//------------------------------------------------------------------------------

tSoundDeviceVec cLauncherHelper::GetAvailableSoundDevices()
{
	////////////////////////////////////
	// Retrieve sound devices
	#if defined(WIN32)
		iLowLevelSound::SetSoundDeviceNameFilter("software");
	#endif

	return iLowLevelSound::GetFilteredSoundDevices();
}

//------------------------------------------------------------------------------

int cLauncherHelper::GetEqualPresetIndex(cConfigFile* apConfig, const std::vector<cConfigFile*>& avPresets)
{
	for(size_t i = 0; i < avPresets.size(); ++i)
	{
		if(EqualsPreset(apConfig, avPresets[i]))
			return (int)i;
	}

	return -1;
}

//------------------------------------------------------------------------------

bool cLauncherHelper::EqualsPreset(cConfigFile* apFile, cConfigFile* apPresetFile)
{
	if(apFile==NULL || apPresetFile==NULL)
		return false;

	bool bEqualsPreset = true;

	// Check ints
	for(int i=0; bEqualsPreset && gvIntPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvIntPresets[i].msCat;
		const tString& sName = gvIntPresets[i].msName;

		bEqualsPreset = bEqualsPreset && 
			apFile->GetInt(sCat, sName , -1) ==
			apPresetFile->GetInt(sCat, sName, 0);
	}

	// Check floats
	for(int i=0; bEqualsPreset && gvFloatPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvFloatPresets[i].msCat;
		const tString& sName = gvFloatPresets[i].msName;

		bEqualsPreset = bEqualsPreset && 
			apFile->GetFloat(sCat, sName , -1.0f) ==
			apPresetFile->GetFloat(sCat, sName, 0.0f);
	}

	// Check bools
	for(int i=0; bEqualsPreset && gvBoolPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvBoolPresets[i].msCat;
		const tString& sName = gvBoolPresets[i].msName;

		bEqualsPreset = bEqualsPreset && 
			apFile->GetBool(sCat, sName , false) ==
			apPresetFile->GetBool(sCat, sName, true);
	}
	
	return bEqualsPreset;
}

//------------------------------------------------------------------------------

void cLauncherHelper::UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile,
									   const tString& asLevel, const tString& asEntry, const tString& asValue, bool abCheckEqualsPreset)
{
	apFile->SetString(asLevel, asEntry, asValue);
	if(abCheckEqualsPreset) apUI->CheckEqualsPreset();
}

//------------------------------------------------------------------------------

void cLauncherHelper::UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile,
									   const tString& asLevel, const tString& asEntry, bool abValue, bool abCheckEqualsPreset)
{
	apFile->SetBool(asLevel, asEntry, abValue);
	if(abCheckEqualsPreset) apUI->CheckEqualsPreset();
}

//------------------------------------------------------------------------------

void cLauncherHelper::UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile, 
									   const tString& asLevel, const tString& asEntry, int alValue, bool abCheckEqualsPreset)
{
	apFile->SetInt(asLevel, asEntry, alValue);
	if(abCheckEqualsPreset) apUI->CheckEqualsPreset();
}

//------------------------------------------------------------------------------

void cLauncherHelper::UpdateConfigFile(cUserInterface* apUI, cConfigFile* apFile, 
									   const tString& asLevel, const tString& asEntry, float afValue, bool abCheckEqualsPreset)
{
	apFile->SetFloat(asLevel, asEntry, afValue);
	if(abCheckEqualsPreset) apUI->CheckEqualsPreset();
}

//------------------------------------------------------------------------------

void cLauncherHelper::UpdateConfigFileWithPreset(cConfigFile* apFile, cConfigFile* apPresetFile)
{
	if(apFile==NULL || apPresetFile==NULL) return;

	
	for(int i=0; gvIntPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvIntPresets[i].msCat;
		const tString& sName = gvIntPresets[i].msName;
		int lDefault = gvIntPresets[i].mlDefaultValue;

		apFile->SetInt(sCat, sName, 
			apPresetFile->GetInt(sCat, sName, lDefault));
	}

	for(int i=0; gvFloatPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvFloatPresets[i].msCat;
		const tString& sName = gvFloatPresets[i].msName;
		float fDefault = gvFloatPresets[i].mfDefaultValue;

		apFile->SetFloat(sCat, sName, 
			apPresetFile->GetFloat(sCat, sName, fDefault));
	}

	for(int i=0; gvBoolPresets[i].msName!=""; ++i)
	{
		const tString& sCat = gvBoolPresets[i].msCat;
		const tString& sName = gvBoolPresets[i].msName;
		bool bDefault = gvBoolPresets[i].mbDefaultValue;

		apFile->SetBool(sCat, sName, 
			apPresetFile->GetBool(sCat, sName, bDefault));
	}
	
	/*
	UpdateConfigFile("Graphics", "ShadowQuality", apPreset->GetInt("Graphics", "ShadowQuality", 1));
	UpdateConfigFile("Graphics", "ShadowResolution", apPreset->GetInt("Graphics", "ShadowResolution", 1));
	UpdateConfigFile("Graphics", "SSAOResolution", apPreset->GetInt("Graphics", "SSAOResolution", 0));
	UpdateConfigFile("Graphics", "SSAOSamples", apPreset->GetInt("Graphics", "SSAOSamples", glDefaultSSAOSamples));
	UpdateConfigFile("Graphics", "TextureQuality", apPreset->GetInt("Graphics", "TextureQuality", 1));
	UpdateConfigFile("Graphics", "TextureFilter", apPreset->GetInt("Graphics", "TextureFilter", 1));

	UpdateConfigFile("Graphics", "WorldReflection", apPreset->GetBool("Graphics", "WorldReflection", "true"));
  
  UpdateConfigFile("Graphics", "SSAOActive", apPreset->GetBool("Graphics", "SSAOActive", true));
  
  
  UpdateConfigFile("Graphics", "TextureQuality", apPreset->GetInt("Graphics", "TextureQuality", 1));
  UpdateConfigFile("Graphics", "TextureFilter", apPreset->GetInt("Graphics", "TextureFilter", 1));
  UpdateConfigFile("Graphics", "TextureAnisotropy", apPreset->GetFloat("Graphics", "TextureAnisotropy", 1));
  
  UpdateConfigFile("Graphics", "EdgeSmooth", apPreset->GetBool("Graphics", "EdgeSmooth", false));
  
  UpdateConfigFile("Graphics", "ParallaxEnabled", apPreset->GetBool("Graphics", "ParallaxEnabled", true));
  UpdateConfigFile("Graphics", "ParallaxQuality", apPreset->GetInt("Graphics", "ParallaxQuality", 0));

  mbSettingPreset = false;
  */
}

//------------------------------------------------------------------------------

void cLauncherHelper::PopulateLanguages(const tStringVec& avLangFiles, const tString& asDefaultLang, cConfigFile* apConfig, Fl_Choice* apLangs)
{
	///////////////////////////////////////////////////////////////////////////////////
	// Lang
	int lLangIndex = -1;
	int lDefaultLangIndex = -1;
	tString sCurrentLang = apConfig->GetString("Main", "StartLanguage", asDefaultLang);

	for(size_t i=0;i<avLangFiles.size();++i)
	{
		const tString& sLangFile = avLangFiles[i];

		tString sLangName = cString::ToLowerCase(cString::Sub(sLangFile, 0, 
									cString::GetLastStringPos(sLangFile, ".")));
									   
		tString sLangEntry = cString::S16BitToUTF8(kTranslate("Languages", sLangName));
		//tString sLangEntry = cString::To8Char(kTranslate("Languages", sLangName));
									   	
		apLangs->add(sLangEntry.c_str());

		if(lDefaultLangIndex==-1 && sLangFile==asDefaultLang)
			lDefaultLangIndex = i;
		if(sLangFile == sCurrentLang)
			lLangIndex = i;
	}

	if(lLangIndex==-1)
		lLangIndex = lDefaultLangIndex;

	apLangs->value(lLangIndex);
}

//------------------------------------------------------------------------------

void cLauncherHelper::cb_AddResolution(Fl_Widget* o, void* v) 
{
	cUserInterface* ui = (cUserInterface*)v;
	ui->AddResolutionWindow->show();
}

//------------------------------------------------------------------------------

bool cLauncherHelper::AddCustomResolution(tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes, cUserInterface* apUI, int apWidth, int apHeight)
{
	if (apWidth < 640 || apHeight < 480) {
		return false;
	}
	//update config
	UpdateConfigFile(apUI, apConfig, "Screen", "Width", apWidth);
	UpdateConfigFile(apUI, apConfig, "Screen", "Height", apHeight);
	UpdateConfigFile(apUI, apConfig, "Screen", "Display", 0);
	// Find existing mode first.
	int lSelectedRes = -1;
	for(int i=0; i<(int)avVidModes.size(); ++i)
	{
		const cVideoMode& vMode = avVidModes[i];
		if(vMode.mvScreenSize.x==apWidth && vMode.mvScreenSize.y==apHeight)
			lSelectedRes = i;
	}
	if (lSelectedRes != -1) {
		const Fl_Menu_Item *item = apRes->menu();
		for (int i=1; i<(int)apRes->size(); ++i,++item) {
			if ((long int)item->user_data()==lSelectedRes) {
				apRes->value(item);
				return true;
			}
		}
	}
	cVideoMode vMode(cVector2l(apWidth, apHeight));
	avVidModes.push_back(vMode);

	PopulateResolutions(avVidModes, apConfig, apRes, apUI);

	return true;
}

//------------------------------------------------------------------------------

void cLauncherHelper::ReselectResolution(const tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes)
{
	// Reset resolution selection
	int lScreenWidth, lScreenHeight, lDisplay;
	lScreenWidth = apConfig->GetInt("Screen", "Width", -1);
	lScreenHeight = apConfig->GetInt("Screen", "Height", -1);
	lDisplay = apConfig->GetInt("Screen", "Display", 0);
	cVideoMode curMode(lDisplay, cVector2l(lScreenWidth, lScreenHeight), 32, 1);

	int lSelectedRes = -1;
	for(int i=0; i<(int)avVidModes.size(); ++i)
	{
		const cVideoMode& vMode = avVidModes[i];
		if(vMode == curMode)
			lSelectedRes = i;
	}
	const Fl_Menu_Item *item = apRes->menu();
	for (int i=1; i<(int)apRes->size(); ++i,++item) {
		if ((long int)item->user_data()==lSelectedRes) {
			apRes->value(item);
			break;
		}
	}
}

//------------------------------------------------------------------------------

void cLauncherHelper::PopulateResolutions(const tVideoModeVec& avVidModes, cConfigFile* apConfig, Fl_Choice* apRes, cUserInterface* apUI)
{
	int lScreenWidth, lScreenHeight, lDisplay;
	lScreenWidth = apConfig->GetInt("Screen", "Width", -1);
	lScreenHeight = apConfig->GetInt("Screen", "Height", -1);
	lDisplay = apConfig->GetInt("Screen", "Display", 0);
	cVideoMode curMode(lDisplay, cVector2l(lScreenWidth, lScreenHeight), 32, 1);
	// Clear out menu
	apRes->menu(NULL);

	int lSelectedRes = -1;
	int lItemPos = -1;
	for(int i=0; i<(int)avVidModes.size(); ++i)
	{
		const cVideoMode& vMode = avVidModes[i];
		tString sModeStr;
		if (vMode.isFullScreenDesktop())
		{
			sModeStr = cString::S16BitToUTF8(kTranslate("OptionsMenu", "FullScreenDesktop")) + " " + cString::S16BitToUTF8(cPlatform::GetDisplayName(vMode.mlDisplay));
		}
		else
		{
			sModeStr = cString::ToString(vMode.mvScreenSize.x) + "x" + cString::ToString(vMode.mvScreenSize.y);
			// Calculate aspect ratio
			AspectRatio ratio = AspectRatioForResolution(vMode.mvScreenSize.x, vMode.mvScreenSize.y);
			sModeStr += "  [" + cString::ToString(ratio.w) + ':' + cString::ToString(ratio.h)+ "]";
			// Check if custom added mode
			if(vMode.mbCustom)
			{
				tWString sAppendix = tWString(kTranslate("Launcher", "Custom")); 
				sModeStr += " (" + cString::S16BitToUTF8(sAppendix) + ")";
			}
		}
		lItemPos = apRes->add(sModeStr.c_str(), 0, NULL, (void*)i);
			
		if(vMode == curMode)
			lSelectedRes = lItemPos;
	}
	if(lItemPos>-1) {
		apRes->mode(lItemPos,FL_MENU_DIVIDER);
	}
	// Add Custom Menu option
	apRes->add("(Add Custom...)", 0, (Fl_Callback*)cb_AddResolution, (void*)apUI);

	if(lSelectedRes==-1)
	{
		if(lScreenWidth==-1 || lScreenHeight==-1)
			lSelectedRes = 0;
	}

	apRes->value(lSelectedRes);
}

//------------------------------------------------------------------------------

void cLauncherHelper::PopulateSoundDevices(const tSoundDeviceVec& avSoundDevices, iSoundDeviceIdentifier* apCurrentDev, cConfigFile* apConfig, Fl_Choice* apSndDevs)
{
	////////////////////////////////////////////////////////////////////////////////////
	// Sound
	int lHintedSoundDev = apConfig->GetInt("Sound", "Device", -1);
	bool bCurDevSupported = false;

	int lIndex = -1;
	for(int i=0;i<(int)avSoundDevices.size();++i)
	{
		iSoundDeviceIdentifier* pDev = avSoundDevices[i];
		tString sDev = cString::ReplaceStringTo(pDev->GetName(), "/", "\\/");
		
		apSndDevs->add(sDev.c_str());

		/////////////////////////////////////////
		// If we are not actively indicating a dev
		// that could be unsupported, pick first default
		if(lHintedSoundDev==-1 && lIndex==-1 && pDev->IsDefault())
			lIndex = i;

		if(apCurrentDev==pDev)
		{
			bCurDevSupported = true;
			lIndex = i;
		}
	}

	if(bCurDevSupported==false && lHintedSoundDev!=-1)
	{
		tString sDev = "(" + cString::S16BitToUTF8(tWString(kTranslate("Launcher", "Unsupported"))) +") " + apCurrentDev->GetName();
		lIndex = apSndDevs->add(sDev.c_str());
	}
	
	apSndDevs->value(lIndex);
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetInitialSSAOSamples(cConfigFile* apConfig, Fl_Choice* apSamples)
{
	int lSSAOSamples = apConfig->GetInt("Graphics", "SSAOSamples", glDefaultSSAOSamples);

	int lSelectedItem = 0;
	for(int i=0;i<apSamples->size();++i)
	{
		const Fl_Menu_Item& item = apSamples->menu()[i];
		
		int lValue = (long int)item.user_data();
		if(lSSAOSamples==lValue)
		{
			lSelectedItem = i;
			break;			
		}		
	}
	
	apSamples->value(lSelectedItem);
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetInitialShadowChoice(cConfigFile* apConfig, Fl_Choice* apShadows, const tString& asWhat)
{
	apShadows->value(apConfig->GetInt("Graphics", "Shadow"+asWhat, 1));
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetInitialAnisotropy(cConfigFile* apConfig, Fl_Choice* apAni)
{
	int lAnisotropy = (int)apConfig->GetFloat("Graphics", "TextureAnisotropy", 1.0f);

	int lSelectedItem = 0;
	for(int i=0;i<apAni->size();++i)
	{
		const Fl_Menu_Item& item = apAni->menu()[i];
		
		int lValue = (long int)item.user_data();
		if(lAnisotropy==lValue)
		{
			lSelectedItem = i;
			break;			
		}		
	}

	apAni->value(lSelectedItem);
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetInitialValues(cConfigFile* apConfig, cUserInterface* apUI)
{
	apUI->ChBFullScreen->value(apConfig->GetBool("Screen", "FullScreen", true)?1:0);

	// Shadows
	SetInitialShadowChoice(apConfig, apUI->CBShadowQ, "Quality");
	SetInitialShadowChoice(apConfig, apUI->CBShadowRes, "Resolution");

	// Reflection
	apUI->ChBWaterReflect->value(apConfig->GetBool("Graphics", "WorldReflection", true)?1:0);

	// SSAO
	apUI->ChBSSAO->value(apConfig->GetBool("Graphics", "SSAOActive", true)?1:0);
	apUI->CBSSAORes->value(apConfig->GetInt("Graphics", "SSAOResolution", 0));
	SetInitialSSAOSamples(apConfig, apUI->CBSSAOSamples);

	// Textures
	apUI->CBTexQ->value((apUI->CBTexQ->size()-1-1) - apConfig->GetInt("Graphics", "TextureQuality", 1));
	apUI->CBTexFilter->value(apConfig->GetInt("Graphics", "TextureFilter", 1));
	SetInitialAnisotropy(apConfig, apUI->CBAnisotropy);

	// Edge Smooth
	apUI->ChBEdgeSmooth->value((int)apConfig->GetBool("Graphics", "EdgeSmooth", false));

	// Parallax
	bool bParallaxEnabled = apConfig->GetBool("Graphics", "ParallaxEnabled", true);
	apUI->CBParallax->value(bParallaxEnabled?1:0);
}

//------------------------------------------------------------------------------

void cLauncherHelper::SetUpWidgetLabels(cUserInterface* apUI, tStringList& alstLabels)
{
	alstLabels.clear();

	cLauncherHelper::SetWidgetLabel(apUI->TabGeneral, kTranslate("Launcher", "TabGeneral"));

	cLauncherHelper::SetWidgetLabel(apUI->CBQPreset, kTranslate("Launcher", "Quality"));
	cLauncherHelper::SetMenuItemLabel(apUI->MIQPLow, kTranslate("OptionsMenu", "Low"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MIQPMed, kTranslate("OptionsMenu", "Medium"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MIQPHigh, kTranslate("OptionsMenu", "High"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MIQPCustom, kTranslate("Launcher", "Custom"), alstLabels);

	cLauncherHelper::SetWidgetLabel(apUI->BDetectSettings, kTranslate("Launcher", "DetectSettings"));

	cLauncherHelper::SetWidgetLabel(apUI->CBLanguage, kTranslate("OptionsMenu", "Language"));

	cLauncherHelper::SetWidgetLabel(apUI->BShowAdvanced, kTranslate("Launcher", "ShowAdvanced"));
	cLauncherHelper::SetWidgetLabel(apUI->BShowBasic, kTranslate("Launcher", "ShowBasic"));

	cLauncherHelper::SetWidgetLabel(apUI->CBResolution, kTranslate("OptionsMenu", "Resolution"));
	cLauncherHelper::SetWidgetLabel(apUI->ChBFullScreen, kTranslate("OptionsMenu", "FullScreen"));

	cLauncherHelper::SetWidgetLabel(apUI->CBShadowQ, kTranslate("Launcher", "ShadowQuality"));
	cLauncherHelper::SetMenuItemLabel(apUI->MISQLow, kTranslate("OptionsMenu", "Low"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MISQMed, kTranslate("OptionsMenu", "Medium"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MISQHigh, kTranslate("OptionsMenu", "High"), alstLabels);

	cLauncherHelper::SetWidgetLabel(apUI->CBShadowRes, kTranslate("Launcher", "ShadowResolution"));
	cLauncherHelper::SetMenuItemLabel(apUI->MISRLow, kTranslate("OptionsMenu", "Low"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MISRMed, kTranslate("OptionsMenu", "Medium"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MISRHigh, kTranslate("OptionsMenu", "High"), alstLabels);



	cLauncherHelper::SetWidgetLabel(apUI->CBTexQ, kTranslate("OptionsMenu", "TexQuality"));
	cLauncherHelper::SetMenuItemLabel(apUI->MITQLow, kTranslate("OptionsMenu", "Low"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MITQMed, kTranslate("OptionsMenu", "Medium"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MITQHigh, kTranslate("OptionsMenu", "High"), alstLabels);

	cLauncherHelper::SetWidgetLabel(apUI->CBAnisotropy, kTranslate("OptionsMenu", "Anisotropy"));
	cLauncherHelper::SetMenuItemLabel(apUI->MIAnisotropyOff, kTranslate("Launcher", "Off"), alstLabels);

	cLauncherHelper::SetWidgetLabel(apUI->CBSSAORes, kTranslate("Launcher", "SSAORes"));
	cLauncherHelper::SetMenuItemLabel(apUI->MISSAOResMed, kTranslate("OptionsMenu", "Medium"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MISSAOResHigh, kTranslate("OptionsMenu", "High"), alstLabels);

	tWString sSSAOSamplesLabel = _W("SSAO ") + tWString(kTranslate("OptionsMenu", "SSAOSamples"));
	cLauncherHelper::SetWidgetLabel(apUI->CBSSAOSamples, sSSAOSamplesLabel);

	cLauncherHelper::SetWidgetLabel(apUI->ChBEdgeSmooth, kTranslate("Launcher", "EdgeSmooth"));

	cLauncherHelper::SetWidgetLabel(apUI->CBTexFilter, kTranslate("OptionsMenu", "TexFilter"));
	cLauncherHelper::SetMenuItemLabel(apUI->MITFilterNearest, kTranslate("OptionsMenu", "Nearest"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MITFilterBilinear, kTranslate("OptionsMenu", "Bilinear"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MITFilterTrilinear, kTranslate("OptionsMenu", "Trilinear"), alstLabels);

	cLauncherHelper::SetWidgetLabel(apUI->ChBWaterReflect, kTranslate("OptionsMenu", "WorldReflection"));
	cLauncherHelper::SetWidgetLabel(apUI->CBParallax, kTranslate("Launcher", "Parallax"));
	cLauncherHelper::SetMenuItemLabel(apUI->MIParallaxOff, kTranslate("Launcher", "Off"), alstLabels);
	cLauncherHelper::SetMenuItemLabel(apUI->MIParallaxOn, kTranslate("Launcher", "On"), alstLabels);


	cLauncherHelper::SetWidgetLabel(apUI->CBSoundDevices, kTranslate("Launcher", "SoundDevice"));

	cLauncherHelper::SetWidgetLabel(apUI->BLaunchGame, kTranslate("Launcher", "LaunchGame"));
	cLauncherHelper::SetWidgetLabel(apUI->BCancel, kTranslate("Global", "Cancel"));

	cLauncherHelper::SetWidgetLabel(apUI->TabGraphics,kTranslate("Launcher", "TabGraphics"));
	cLauncherHelper::SetWidgetLabel(apUI->LabelVideoDevice, kTranslate("Launcher", "VideoDevice"));
	cLauncherHelper::SetWidgetLabel(apUI->TabSound,kTranslate("Launcher", "TabSound"));
}

//------------------------------------------------------------------------------

void cLauncherHelper::DetectSettings(cUserInterface* apUI, cConfigFile* apConfig, const std::vector<cConfigFile*>& avPresets, 
									 cQualityChooser* apChooser, const tString& asCardString)
{
	tString sClosestMatch;
	eQRating rating = apChooser->GetQualityRatingByCardString(asCardString, sClosestMatch); 

	if(sClosestMatch.empty()==false)
	{
		sClosestMatch = cString::S16BitToUTF8(kTranslate("Launcher", "UnlistedVCardClosestMatch")) + " " + sClosestMatch; 
		fl_message(sClosestMatch.c_str());
	}

	if(rating==eQRating_Unknown)
	{
		rating = eQRating_Medium;
		tString sMsg = cString::S16BitToUTF8(kTranslate("Launcher", "UnlistedVCard"));
		fl_message(sMsg.c_str());
	}
		
	if(rating==eQRating_Unsupported)
	{
		tString sMsg = cString::S16BitToUTF8(kTranslate("Launcher", "UnsupportedVCard"));
		fl_message(sMsg.c_str());
	}
	else
	{
		/////////////
		// Update preset
		cLauncherHelper::UpdateConfigFileWithPreset(apConfig, avPresets[rating]);

		//////////////
		// Get native resolution
		tVideoModeVec vResolutions = cLauncherHelper::GetAvailableVideoModes(apConfig);

		int lWidth, lHeight;
		cPlatform::GetDisplayResolution(0, lWidth, lHeight);

		UpdateConfigFile(apUI, apConfig, "Screen", "Width", lWidth);
		UpdateConfigFile(apUI, apConfig, "Screen", "Height", lHeight);
		UpdateConfigFile(apUI, apConfig, "Screen", "Display", vResolutions.back().mlDisplay);

		ReselectResolution(vResolutions, apConfig, apUI->CBResolution);

		apUI->CheckEqualsPreset();
		SetInitialValues(apConfig, apUI);
	}
}

//------------------------------------------------------------------------------

AspectRatio cLauncherHelper::AspectRatioForResolution(int w, int h) {
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

//------------------------------------------------------------------------------
