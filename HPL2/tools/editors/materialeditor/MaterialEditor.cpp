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

#include "hpl.h"
using namespace hpl;

#include "MaterialEditor.h"

#include "../common/DirectoryHandler.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorEditModeFactory.h"
#include "../common/EditorEditModeDummy.h"
#include "../common/EditorEditModeStaticObjects.h"
#include "../common/EditorEditModeEntities.h"
#include "../common/EditorEditModeSelect.h"
#include "../common/EditorEditModeLights.h"
#include "../common/EditorEditModeAreas.h"
#include "../common/EditorEditModeBillboards.h"
#include "../common/EditorEditModeSounds.h"
#include "../common/EditorEditModeParticleSystems.h"
#include "../common/EditorEditModePrimitive.h"

#include "../common/EditorWindowFactory.h"
#include "../common/EditorWindowViewport.h"
#include "../common/EditorWindowStaticObjects.h"
#include "../common/EditorWindowSelect.h"
#include "../common/EditorWindowEntitySearch.h"
#include "../common/EditorWindowTextureBrowser.h"
#include "../common/EditorWindowMaterialEditor.h"
#include "../common/EditorWindowOptions.h"

#include "../common/EditorGrid.h"
#include "../common/EditorSelection.h"

#include "../common/EntityWrapper.h"

#include <algorithm>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// MATERIAL EDITOR
///////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cMaterialEditor::cMaterialEditor(const tString& asCommandLine) : iEditorBase(_W("Materials"), _W("*.mat"))
{
	msCommandLineFile = asCommandLine;
}

cMaterialEditor::~cMaterialEditor()
{
	OnSaveConfig();
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

//--------------------------------------------------------------------


//--------------------------------------------------------------------

void cMaterialEditor::OnInit()
{
	mpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");
}

//--------------------------------------------------------------------

void cMaterialEditor::OnInitLayout()
{
	mpMaterialEditor = hplNew( cEditorWindowMaterialEditor,(this, NULL, cString::To16Char(msCommandLineFile), NULL, true));
	mpMaterialEditor->Init();
	mpMaterialEditor->SetActive(true);

	AddWindow(mpMaterialEditor);
}

//--------------------------------------------------------------------

void cMaterialEditor::OnSetUpDirectories()
{
	const tWString& sWorkingDir = GetWorkingDir();

	mpDirHandler->AddLookUpDir(eDir_Maps, sWorkingDir + mpMainConfig->GetStringW("Directories", "MapsDir", _W("maps")), true); 
}

//--------------------------------------------------------------------

void cMaterialEditor::OnUpdate(float afTimeStep)
{
}

//--------------------------------------------------------------------

void cMaterialEditor::OnPostUpdateLayout()
{
	/*
	///////////////////////////////////////
	// Update Title Bar
	tString sTitlebarFilename;

	if(msSaveFilename ==_W(""))
		sTitlebarFilename = "Unnamed Map ";
	else
		sTitlebarFilename = cString::To8Char(cString::GetFileNameW(msSaveFilename));

    cPlatform::SetWindowCaption(sTitlebarFilename + sModified + " - " + msCaption);*/
}

//--------------------------------------------------------------------

void cMaterialEditor::OnInitInput()
{
	/*
	/////////////////////////////////////////////////////////
	// Set up Special input keys
	mpEngine->GetInput()->CreateAction("ViewMode",-1)->AddKey(eKey_LeftAlt);
	mpEngine->GetInput()->CreateAction("SelectToolActive",-1)->AddKey(eKey_LeftShift);
	mpEngine->GetInput()->CreateAction("AddToCurrentSelection",-1)->AddKey(eKey_LeftCtrl);
	*/
}

//--------------------------------------------------------------------

void cMaterialEditor::OnLoadConfig()
{
	//////////////////////////////////////////////////////////////
	// Set up loading stuff that is specific to this editor, 
	// and stuff like log filename (this is done pre engine creation)
	tWString sConfigFile = GetHomeDir() + _W("MaterialEditor.cfg");

	mpLocalConfig = hplNew(cConfigFile, ( sConfigFile));
	mpLocalConfig->Load();

	SetSettingValue("ScreenWidth", mpLocalConfig->GetString("Screen","Width","900"));
	SetSettingValue("ScreenHeight", mpLocalConfig->GetString("Screen","Height","700"));
	SetSettingValue("FullScreen", "false");

	 // Set Input config
	SetSettingValue("TumbleFactor", mpLocalConfig->GetString("Input", "TumbleSpeed", "0.005"));
	SetSettingValue("TrackFactor", mpLocalConfig->GetString("Input", "TrackFactor", "0.005"));
	SetSettingValue("ZoomFactor", mpLocalConfig->GetString("Input", "ZoomFactor", "0.001"));
	SetSettingValue("MouseWheelZoom", mpLocalConfig->GetString("Input", "MouseWheelZoom", "0.1"));

	SetSettingValue("RotateSnap", mpLocalConfig->GetString("Options","RotateSnap", cString::ToString(kPi2f/3)));
	SetSettingValue("ScaleSnap",  mpLocalConfig->GetString("Options","ScaleSnap", "0.5"));

	mpActionHandler->SetMaxUndoSize(mpLocalConfig->GetInt("Options","UndoStackSize", 10));

	/////////////////////////////////
	// Recent files setup
	for(int i=0;i<10;++i)
	{
		tWString sRecent = mpLocalConfig->GetStringW("RecentUsedFiles", "RecentFile" + cString::ToString(i+1), _W(""));

		if(sRecent==_W(""))
			break;

		if(cPlatform::FileExists(sRecent))
		{
			tWStringListIt itFoundString = find(mlstRecentFiles.begin(),mlstRecentFiles.end(), sRecent);
			if(itFoundString==mlstRecentFiles.end())
				mlstRecentFiles.push_back(sRecent);
		}
	}

	// Window caption
	msCaption = "HPL Material Editor";
	
	SetLogFile(GetHomeDir() + _W("MaterialEditor.log"));

	msLastLoadPath = mpLocalConfig->GetStringW("Directories", "LastUsedPath", GetMainLookUpDir(eDir_Maps));
	if(msLastLoadPath==_W(""))
		msLastLoadPath = GetMainLookUpDir(eDir_Maps); 
}

//--------------------------------------------------------------------

void cMaterialEditor::OnSaveConfig()
{
	mpLocalConfig->SetString("Screen","Width", GetSetting("ScreenWidth"));
	mpLocalConfig->SetString("Screen","Height", GetSetting("ScreenHeight"));
	mpLocalConfig->SetString("Screen","Fullscreen", GetSetting("Fullscreen"));

	mpLocalConfig->SetString("Input", "TumbleFactor", GetSetting("TumbleFactor"));
	mpLocalConfig->SetString("Input", "TrackFactor", GetSetting("TrackFactor"));
	mpLocalConfig->SetString("Input", "ZoomFactor", GetSetting("ZoomFactor"));
	mpLocalConfig->SetString("Input", "MouseWheelZoom", GetSetting("MouseWheelZoom"));

	mpLocalConfig->SetString("Options", "RotateSnap", GetSetting("RotateSnap"));
	mpLocalConfig->SetString("Options", "ScaleSnap", GetSetting("ScaleSnap"));

	mpLocalConfig->SetInt("Options","UndoStackSize",(int)mpActionHandler->GetMaxUndoSize());

	// Save recent file names
	int i=0;
	for(tWStringListIt it=mlstRecentFiles.begin(); it!=mlstRecentFiles.end();++it)
	{
		tWString sRecent = *it;

		if(sRecent==_W(""))
			break;
		
		if(cPlatform::FileExists(sRecent))
		{
			mpLocalConfig->SetString("RecentUsedFiles", "RecentFile" + cString::ToString(++i), cString::To8Char(sRecent));
		}
	}
	// Save last used path
	mpLocalConfig->SetString("Directories", "LastUsedPath", (const tString&)cString::To8Char(msLastLoadPath));

	mpLocalConfig->Save();
	hplDelete(mpLocalConfig);
}

//--------------------------------------------------------------------


//--------------------------------------------------------------------
