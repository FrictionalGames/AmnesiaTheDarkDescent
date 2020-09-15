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

#include "ParticleEditor.h"

#include "../common/DirectoryHandler.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorWindowFactory.h"
#include "../common/EditorWindowViewport.h"
#include "../common/EditorWindowOptions.h"
#include "ParticleEditorWindowEmitters.h"
#include "ParticleEditorWindowEmitterParams.h"

#include "../common/EditorGrid.h"
#include "../common/EditorWorld.h"
#include "../common/EditorSelection.h"

#include "../common/EntityWrapper.h"
#include "EntityWrapperParticleEmitter.h"

#include "ParticleEditorWorld.h"

#include <algorithm>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// PARTICLE EDITOR
///////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cParticleEditor::cParticleEditor() : iEditorBase(_W("Particle Systems"), _W("*.ps"))
{
}

cParticleEditor::~cParticleEditor()
{
	OnSaveConfig();
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void cParticleEditor::AppSpecificReset()
{
	///////////////////////////////////////
	// Viewport Config
	mvViewports[0]->SetPreset(eEditorWindowViewportPreset_Perspective);
	mvViewports[0]->SetRenderMode(eRenderer_Main);
	SetFocusedViewport(mvViewports[0]);

	mpWEmitters->Reset();
}

//--------------------------------------------------------------------


//--------------------------------------------------------------------

//--------------------------------------------------------------------

//--------------------------------------------------------------------

//--------------------------------------------------------------------

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

bool cParticleEditor::MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	///////////////////////////////////////////////
	// Menu Item "File.New"
	if(apWidget==mpMainMenuNew)
	{
		Command_New();
	}
	///////////////////////////////////////////////
	// Menu Item "Save"
	else if(apWidget==mpMainMenuSave)
	{
		Command_Save();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Save As"
	else if(apWidget==mpMainMenuSaveAs)
	{
		Command_SaveAs();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Open"
	else if(apWidget==mpMainMenuLoad)
	{
		Command_Load();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Exit"
	else if(apWidget==mpMainMenuExit)
	{
		Command_Exit();
	}
	///////////////////////////////////////////////
	// Menu Item "Edit.Options"
	else if(apWidget==mpMainMenuOptions)
	{
		mpWindowOptions->SetActive(true);
	}
	///////////////////////////////////////////////
	// Menu Item "View.Room.ShowFloor"
	else if(apWidget==mpMIShowFloor)
	{
		bool bChecked = mpMIShowFloor->IsChecked();
		((cParticleEditorWorld*)mpEditorWorld)->SetShowFloor(!bChecked);
	}
	///////////////////////////////////////////////
	// Menu Item "View.Room.ShowWalls"
	else if(apWidget==mpMIShowWalls)
	{
		bool bChecked = mpMIShowWalls->IsChecked();
		((cParticleEditorWorld*)mpEditorWorld)->SetShowWalls(!bChecked);
	}
	///////////////////////////////////////////////
	// Menu Item "View.BG Color"
	else if(apWidget==mpMISetColor[0] ||
			apWidget==mpMISetColor[1] ||
			apWidget==mpMISetColor[2])
	{
		cColor col[] = { cColor(0,1), cColor(0.5f, 1), cColor(1) };
		int i=0;
		for(;i<3;++i)
			if(apWidget==mpMISetColor[i])
				break;

		((cParticleEditorWorld*)mpEditorWorld)->SetBGColor(col[i]);
	}

	UpdateMenu();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cParticleEditor, MainMenu_ItemClick);

//--------------------------------------------------------------------

//--------------------------------------------------------------------

void cParticleEditor::OnNew()
{
	((cParticleEditorWorld*)mpEditorWorld)->AddEmitter();
	mpWEmitters->Update(0);
	mpWEmitters->SelectEntry(0);
}

//--------------------------------------------------------------------

void cParticleEditor::OnPostWorldLoad()
{
	mpWEmitters->Update(0);
	mpWEmitters->SelectEntry(0);
}

//--------------------------------------------------------------------

void cParticleEditor::AppSpecificLoad(iXmlDocument* apDoc)
{
	cXmlElement* pXmlSession = apDoc->GetFirstElement("EditorSession");
	if(pXmlSession==NULL) return;

	/*
	///////////////////////////////////////
	// Viewport Config
	cXmlElement* pXmlViewportConfig = pXmlSession->GetFirstElement("ViewportConfig");
	cXmlNodeListIterator it = pXmlViewportConfig->GetChildIterator();
	cXmlElement* pXmlViewport = it.Next()->ToElement();
	mvViewport->Load(pXmlViewport);
*/
	///////////////////////////////////////
	// Global Lighting
}

void cParticleEditor::AppSpecificSave(iXmlDocument* apDoc)
{
	apDoc->SetValue("ParticleSystem");
	//iEditorBase::AppSpecificSave(apDoc);
	/*
	cXmlElement* pXmlSession = apDoc->GetFirstElement("EditorSession");
	if(pXmlSession==NULL)
		pXmlSession = apDoc->CreateChildElement("EditorSession");

	///////////////////////////////////////
	// Viewport Config
	cXmlElement* pXmlViewportConfig = pXmlSession->CreateChildElement("ViewportConfig");
	cXmlElement* pXmlViewport = pXmlViewportConfig->CreateChildElement("Viewport");
	mvViewport->Save(pXmlViewport);
	SetFocusedViewport(mvViewport);
	*/
	///////////////////////////////////////
	// Grid Config
	/*cXmlElement* pXmlGridConfig = pXmlSession->CreateChildElement("GridConfig");
	pXmlGridConfig->SetAttributeBool("Snap", mpGrid->GetSnapToGrid());
	pXmlGridConfig->SetAttributeFloat("Height", mpGrid->GetHeight());
	pXmlGridConfig->SetAttributeFloat("SnapSeparation",mpGrid->GetSnapSeparation());
	pXmlGridConfig->SetAttributeInt("GridPlane", (int)mpGrid->GetPlaneAxis());*/

	///////////////////////////////////////
	// Global lights Config
}

//--------------------------------------------------------------------

void cParticleEditor::UpdateEditMenu()
{
	mpMainMenuUndo->SetEnabled( !mpActionHandler->IsDoneActionsListEmpty());
	mpMainMenuRedo->SetEnabled( !mpActionHandler->IsUndoneActionsListEmpty());
}

//--------------------------------------------------------------------

void cParticleEditor::UpdateMenu()
{
	cParticleEditorWorld* pWorld = (cParticleEditorWorld*)mpEditorWorld;
	mpMIShowFloor->SetChecked(pWorld->GetShowFloor());
	mpMIShowWalls->SetChecked(pWorld->GetShowWalls());

	cColor col[] = { cColor(0,1), cColor(0.5f, 1), cColor(1) };
	cColor colBG = pWorld->GetBGColor();
	for(int i=0;i<3;++i)
	{
		bool bChecked = colBG==col[i];
		mpMISetColor[i]->SetChecked(bChecked);
	}
}

//--------------------------------------------------------------------

void cParticleEditor::OnInit()
{
	iEntityWrapperType* pType = hplNew(cEntityWrapperTypeParticleEmitter,());

	mpEditorWorld->AddEntityType(pType);
}

//--------------------------------------------------------------------

cWidgetMainMenu* cParticleEditor::CreateMainMenu()
{
	/////////////////////////////////
	// Setup main menu
	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem = NULL;

	mpMainMenu = mpSet->CreateWidgetMainMenu();

	// File menu
	pItem = mpMainMenu->AddMenuItem(_W("File"));
	mpMainMenuNew = pItem->AddMenuItem(_W("New"));
	mpMainMenuNew->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	mpMainMenuLoad = pItem->AddMenuItem(_W("Open"));
	mpMainMenuLoad->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	mpMainMenuSave = pItem->AddMenuItem(_W("Save"));
	mpMainMenuSave->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuSave->AddShortcut(eKeyModifier_Ctrl, eKey_S);

	mpMainMenuSaveAs = pItem->AddMenuItem(_W("Save as"));
	mpMainMenuSaveAs->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	mpMainMenuRecent = pItem->AddMenuItem(_W("Open Recent"));

	pItem->AddSeparator();

	mpMainMenuExit = pItem->AddMenuItem(_W("Quit"));
	mpMainMenuExit->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
#if defined(WIN32)
	mpMainMenuExit->AddShortcut(eKeyModifier_Alt, eKey_F4);
#elif defined(__linux__)
	mpMainMenuExit->AddShortcut(eKeyModifier_Ctrl, eKey_Q);
#elif defined(__APPLE__)
	mpMainMenuExit->AddShortcut(eKeyModifier_Ctrl, eKey_Q);
#endif
    
	// Edit menu
	pItem = mpMainMenu->AddMenuItem(_W("Edit"));
	mpMainMenuUndo =  pItem->AddMenuItem(_W("Undo"));
	mpMainMenuUndo->AddShortcut(eKeyModifier_Ctrl, eKey_Z);
	mpMainMenuRedo = pItem->AddMenuItem(_W("Redo"));
	mpMainMenuRedo->AddShortcut(eKeyModifier_Ctrl, eKey_Y);
	pItem->AddSeparator();
	mpMainMenuOptions = pItem->AddMenuItem(_W("Options"));
	mpMainMenuOptions->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	// View Menu
	tWString sColors[] = { _W("Black"),_W("Grey"), _W("White") };
	pItem = mpMainMenu->AddMenuItem(_W("View"));
	pSubItem = pItem->AddMenuItem(_W("Room"));
	mpMIShowFloor = pSubItem->AddMenuItem(_W("Show floor"));
	mpMIShowFloor->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMIShowWalls = pSubItem->AddMenuItem(_W("Show walls"));
	mpMIShowWalls->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	pSubItem = pItem->AddMenuItem(_W("BG Color"));
	for(int i=0;i<3;++i)
	{
		mpMISetColor[i] = pSubItem->AddMenuItem(sColors[i]);
		mpMISetColor[i]->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	}

	return mpMainMenu;

}

void cParticleEditor::SetUpWindowAreas()
{
	SetLayoutVec3f(eLayoutVec3_ViewportAreaPos, cVector3f(512, mpMainMenu->GetSize().y+3,1));
	SetLayoutVec2f(eLayoutVec2_ViewportAreaSize, cVector2f(512));
}

void cParticleEditor::CreateViewports()
{
	///////////////////////////////////
	// Viewport Setup
	cEditorWindowViewport* pViewport = hplNew(cEditorWindowViewport,(this, mpFrameBuffer, false));
	pViewport->Init();

	pViewport->SetEngineViewportPositionAndSize(0, GetLayoutVec2l(eLayoutVec2_ViewportAreaSize));
	
	pViewport->SetNormalPosition(GetLayoutVec3f(eLayoutVec3_ViewportAreaPos));
	pViewport->SetNormalSize(GetLayoutVec2f(eLayoutVec2_ViewportAreaSize) + cVector2f(0,200));
	pViewport->SetEngineViewportNormalPosition(cVector2l(0));
	pViewport->SetEngineViewportNormalSize(GetLayoutVec2l(eLayoutVec2_ViewportAreaSize));

	pViewport->SetActive(true);
	pViewport->SetDrawDebug(false);

	pViewport->GetVCamera()->LockToGrid(false);
	
	AddWindow(pViewport);

	mvViewports.push_back(pViewport);
}

void cParticleEditor::OnInitLayout()
{
	mpWindowOptions = hplNew(cEditorWindowOptions,(this));
	mpWindowOptions->Init();
	mpWindowOptions->SetActive(false);
	AddWindow(mpWindowOptions);

	mpWEmitters = hplNew(cParticleEditorWindowEmitters,(this));
	mpWEmitters->Init();
	mpWEmitters->SetActive(true);
	AddWindow(mpWEmitters);

	UpdateMenu();
}

//--------------------------------------------------------------------

void cParticleEditor::OnSetUpDirectories()
{
}

//--------------------------------------------------------------------

void cParticleEditor::OnUpdate(float afTimeStep)
{
}

//--------------------------------------------------------------------

void cParticleEditor::OnPostUpdateLayout()
{
	///////////////////////////////////////
	// Update Title Bar
	tString sTitlebarFilename;

	if(msSaveFilename ==_W(""))
		sTitlebarFilename = "Unnamed Particle System ";
	else
		sTitlebarFilename = cString::To8Char(cString::GetFileNameW(msSaveFilename));

	tString sModified = mpEditorWorld->IsModified()?"(modified)":"";

    mpEngine->GetGraphics()->GetLowLevel()->SetWindowCaption(sTitlebarFilename + sModified + " - " + msCaption);

	///////////////////////////////////////
	// Update Recent Files
	mpMainMenuRecent->ClearMenuItems();
	mpMainMenuRecent->SetEnabled(mlstRecentFiles.empty()==false);
	for(tWStringListIt it=mlstRecentFiles.begin(); it!=mlstRecentFiles.end();++it)
	{
		tWString sRecent = *it;

		if(sRecent==_W(""))
			break;
		
		if(cPlatform::FileExists(sRecent))
		{
			cWidgetMenuItem* pItem = mpMainMenuRecent->AddMenuItem(sRecent);
			pItem->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(RecentFileCallback));
		}
	}
	UpdateEditMenu();
}

//--------------------------------------------------------------------

void cParticleEditor::OnInitInput()
{
	/////////////////////////////////////////////////////////
	// Set up Special input keys
	cAction *pAction = mpEngine->GetInput()->CreateAction("ViewMode",-1);
	pAction->AddKey(eKey_LeftAlt);
#if defined(__linux__)
	pAction->AddKey(eKey_LeftSuper);
	pAction->AddKey(eKey_Z);
#endif
	mpEngine->GetInput()->CreateAction("SelectToolActive",-1)->AddKey(eKey_LeftShift);
	mpEngine->GetInput()->CreateAction("AddToCurrentSelection",-1)->AddKey(eKey_LeftCtrl);
}

//--------------------------------------------------------------------

void cParticleEditor::OnLoadConfig()
{
	//////////////////////////////////////////////////////////////
	// Set up loading stuff that is specific to this editor, 
	// and stuff like log filename (this is done pre engine creation)
	tWString sConfigFile = GetHomeDir() + _W("ParticleEditor.cfg");

	mpLocalConfig = hplNew(cConfigFile, ( sConfigFile));
	mpLocalConfig->Load();

	SetSettingValue("ScreenWidth", mpLocalConfig->GetString("Screen","Width","1024"));
	SetSettingValue("ScreenHeight", mpLocalConfig->GetString("Screen","Height","768"));
	SetSettingValue("FullScreen", mpLocalConfig->GetString("Screen","Fullscreen","false"));

	 // Set Input config
	SetSettingValue("TumbleFactor", mpLocalConfig->GetString("Input", "TumbleSpeed", "0.005"));
	SetSettingValue("TrackFactor", mpLocalConfig->GetString("Input", "TrackFactor", "0.01"));
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
	msCaption = "HPL Particle Editor";
	
	SetLogFile(GetHomeDir() + _W("ParticleEditor.log"));

	msLastLoadPath = mpLocalConfig->GetStringW("Directories", "LastUsedPath", GetMainLookUpDir(eDir_Particles));
	if(msLastLoadPath==_W(""))
		msLastLoadPath = GetMainLookUpDir(eDir_Particles);
}

//--------------------------------------------------------------------

void cParticleEditor::OnSaveConfig()
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

iEditorWorld* cParticleEditor::CreateSpecificWorld()
{
	return hplNew( cParticleEditorWorld, (this));
}
