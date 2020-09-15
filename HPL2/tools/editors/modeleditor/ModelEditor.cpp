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

#include "ModelEditor.h"

#include "../common/DirectoryHandler.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorEditModeFactory.h"
#include "../common/EditorEditModeDummy.h"
#include "../common/EditorEditModeStaticObjects.h"
#include "../common/EditorEditModeSelect.h"
#include "../common/EditorEditModeBodies.h"
#include "../common/EditorEditModeJoints.h"
#include "../common/EditorEditModeLights.h"
#include "../common/EditorEditModeBillboards.h"
#include "../common/EditorEditModeParticleSystems.h"
#include "../common/EditorEditModeSounds.h"

#include "../common/EditorWindowFactory.h"
#include "../common/EditorWindowViewport.h"
#include "../common/EditorWindowSelect.h"
#include "../common/EditorWindowEntitySearch.h"
#include "../common/EditorWindowTextureBrowser.h"
#include "../common/EditorWindowEditModeSidebar.h"
#include "../common/EditorGrid.h"
#include "../common/EditorSelection.h"
#include "../common/EditorVar.h"

#include "../common/EntityWrapper.h"

#include <algorithm>

#include "../common/EditorWindowMaterialEditor.h"

#include "ModelEditorEditModeToolbox.h"
#include "ModelEditorLowerToolbar.h"
#include "ModelEditorWindowUserSettings.h"
#include "ModelEditorWindowOutline.h"
#include "ModelEditorWindowPhysicsTest.h"
#include "ModelEditorWindowAnimations.h"

#include "ModelEditorWorld.h"
#include "ModelEditorActions.h"



//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cModelEditor::cModelEditor() : iEditorBase(_W("Models"), _W("*.ent"))
{
}

cModelEditor::~cModelEditor()
{
	OnSaveConfig();
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void cModelEditor::AppSpecificReset()
{
	msImportedMeshFilename = _W("");

	///////////////////////////////////////
	// Viewport Config
	for(int i=0;i<4;++i)
	{
		mvViewports[i]->SetPreset( (eEditorWindowViewportPreset)i ) ;
		mvViewports[i]->GetVCamera()->LockToGrid(false);
	}
	SetFocusedViewport(mvViewports[0]);
}

//--------------------------------------------------------------------

void cModelEditor::SetUpClassDefinitions(cEditorUserClassDefinitionManager* apManager)
{
	apManager->RegisterDefFilename(eUserClassDefinition_Entity, "EntityTypes.cfg", eEditorVarCategory_Type);
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

bool cModelEditor::MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	// TODO: duh this really needs to be done some other way!!
	if(mpCurrentEditMode==GetEditMode("Select"))
	{
		cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)mpCurrentEditMode;
		bool bCanDelete = true;
		bool bCanClone = true;
		tEntityWrapperList& lstEntities = mpSelection->GetEntities();
		tEntityWrapperListIt it = lstEntities.begin();
		for(;it!=lstEntities.end();++it)
		{
			iEntityWrapper* pEnt = *it;
			bCanDelete = bCanDelete && pEnt->IsDeletable();
			bCanClone = bCanClone && pEnt->IsCloneable();
		}

		tIntList& lstEntityIDs = mpSelection->GetEntityIDs();
		if(bCanDelete && apWidget==mpMainMenuDelete)
			AddAction(pEditMode->CreateDeleteEntitiesAction(lstEntityIDs));
		else if(bCanClone && apWidget==mpMainMenuClone)
			AddAction(pEditMode->CreateCloneEntitiesAction(lstEntityIDs));
	}

	///////////////////////////////////////////////
	// Menu Item "File.New"
	if(apWidget==mpMainMenuNew)
	{
		Command_New();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Save"
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
	/////////////////////////////////////////////////////////////////
	// Menu Item "File.Import Mesh"
	else if(apWidget==mpMainMenuImport)
	{
		tWStringList vFilters = tWStringList(1,_W("*.dae"));
		vFilters.push_back(_W("*.fbx"));
		ShowLoadFilePicker(mvLoadFilenames, msLastMeshPath,this, kGuiCallback(MeshImport_Callback), _W("Meshes"), vFilters);
	}
	/////////////////////////////////////////////////////////////////
	// Menu Item "Edit.Search Entity"
	else if(apWidget==mpMainMenuSearch)
	{
		cEditorWindowEntitySearch* pWin = cEditorWindowFactory::CreateSearchWindow(this,(cEditorEditModeSelect*)GetEditMode("Select"));
		pWin->SetActive(true);
	}
	/////////////////////////////////////////////////////////////////
	// Menu Item "Settings.User defined variables"
	else if(apWidget==mpMainMenuUserSettings)
	{
		cModelEditorWindowUserSettings* pWin = hplNew(cModelEditorWindowUserSettings,(this));
		pWin->Init();
		AddWindow(pWin);
		pWin->SetActive(true);
	}
	else if(apWidget==mpMainMenuAnimations)
	{
		mpWindowAnimations->SetActive(true);		
	}
	/////////////////////////////////////////////////////////////////
	// Menu Item "View.Outline"
	else if(apWidget==mpMainMenuOutline)
	{
		cModelEditorWindowOutline* pWin = hplNew(cModelEditorWindowOutline,(this));
		pWin->Init();
		AddWindow(pWin);
		pWin->SetActive(true);
	}
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditor, MainMenu_ItemClick);

//--------------------------------------------------------------------

bool cModelEditor::MainMenu_UndoRedo(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mpMainMenuUndo)
		mpActionHandler->Undo();
	else if(apWidget==mpMainMenuRedo)
		mpActionHandler->Redo();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditor, MainMenu_UndoRedo);

//--------------------------------------------------------------------

bool cModelEditor::OnChangeFlags(int alFlags)
{
	bool bMustUpdateLayout = iEditorBase::OnChangeFlags(alFlags);
	if(alFlags&eModelEditorFlag_TestWindowActive)
	{
		bool bActive = GetFlags(eModelEditorFlag_TestWindowActive);

		if(bActive)
		{
			mpTestWindow = hplNew(cModelEditorWindowPhysicsTest,(this, GetViewportsFrameBuffer()));
			mpTestWindow->Init();
			AddWindow(mpTestWindow);
			mpTestWindow->SetPosition(GetLayoutVec3f(eLayoutVec3_ViewportAreaPos));
			mpTestWindow->SetSize(GetLayoutVec2f(eLayoutVec2_ViewportAreaSize));
			mpTestWindow->SetActive(true);			
		}
		else
		{
			if(mpTestWindow)
			{
				mpTestWindow->SetActive(false);
				cEditorWindowFactory::DestroyEditorWindow(mpTestWindow);
				mpTestWindow = NULL;
			}
		}
	}

	return bMustUpdateLayout;
}

//--------------------------------------------------------------------

void cModelEditor::UpdateEditMenu()
{
	mpMainMenuUndo->SetEnabled( !mpActionHandler->IsDoneActionsListEmpty());
	mpMainMenuRedo->SetEnabled( !mpActionHandler->IsUndoneActionsListEmpty());

	bool bHasSelectedObjects = (mpSelection->IsEmpty()==false);

	mpMainMenuDelete->SetEnabled(bHasSelectedObjects && mpSelection->IsDeletable());
	mpMainMenuClone->SetEnabled(bHasSelectedObjects && mpSelection->IsCloneable());

	/*mpMainMenuUndo->SetEnabled( !mpActionHandler->IsDoneActionsListEmpty());
	mpMainMenuRedo->SetEnabled( !mpActionHandler->IsUndoneActionsListEmpty());

	bool bHasSelectedObjects = (mpCurrentEditMode==mvEditModes[eModelEditorEditMode_Select] && 
							   mpSelection->IsEmpty()==false);

	mpMainMenuDelete->SetEnabled(bHasSelectedObjects);
	mpMainMenuClone->SetEnabled(bHasSelectedObjects);*/
}

//--------------------------------------------------------------------

void cModelEditor::OnInit()
{
	mpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");

	/*
	tStringList lstTypes,lstSubTypes;
	cEntityCustomVarHelper::GetTypes(GetCustomVarDefinition(eCustomVarDefinition_Entity), lstTypes);
	msDefaultTypeName = lstTypes.front();
	cEntityCustomVarHelper::GetSubTypesByTypeName(GetCustomVarDefinition(eCustomVarDefinition_Entity), msDefaultTypeName, lstSubTypes, msDefaultSubTypeName);
	*/
	
	AddEditMode(hplNew(cEditorEditModeSelect,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeLights,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeBillboards,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeParticleSystems,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeSounds,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeBodies,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeJoints,(this, mpEditorWorld)));

	cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)GetEditMode("Select");
	pEditMode->AddEntityFilter(eEditorEntityType_SubMesh);
	pEditMode->AddEntityFilter(eEditorEntityType_Bone);

	cEditorGrid::SetSnapToGrid(false);
}

//--------------------------------------------------------------------

void cModelEditor::OnInitLayout()
{
	///////////////////////////////////
	// EditMode Tool box
	mpEditModeSidebar->SetPosition(cVector3f(0,mpMainMenu->GetSize().y+2,1));

    ///////////////////////////////////
	// Lower Toolbar
	mpLowerToolbar->SetPosition(cVector3f(GetLayoutVec3f(eLayoutVec3_ViewportAreaPos).x, mvScreenSize.y-50,1));
	
	SetCurrentEditMode(GetEditMode("Select"));

	mpWindowAnimations = hplNew(cModelEditorWindowAnimations,(this));
	mpWindowAnimations->Init();
	AddWindow(mpWindowAnimations);
	mpWindowAnimations->SetActive(false);

	mpTestWindow = NULL;
}

//--------------------------------------------------------------------

void cModelEditor::OnSetUpDirectories()
{
	const tWString& sWorkingDir = GetWorkingDir();

	mpDirHandler->AddLookUpDir(eDir_Maps, sWorkingDir + mpMainConfig->GetStringW("Directories", "MapsDir", _W("maps")), true); 
}

//--------------------------------------------------------------------

void cModelEditor::OnUpdate(float afTimeStep)
{
}

//--------------------------------------------------------------------

void cModelEditor::OnPostUpdateLayout()
{
	tString sTitlebarFilename = "";

	tString sModified = mpEditorWorld->IsModified()?"(modified)":"";

	if(msSaveFilename ==_W(""))
	{
		sTitlebarFilename = "Unnamed Model " + sModified;
	}
	if(msSaveFilename !=_W(""))
	{
		int sLastSlashPos = cString::GetLastStringPosW(msSaveFilename,_W("/"));
		sTitlebarFilename = cString::To8Char(cString::SubW(msSaveFilename, sLastSlashPos+1)) + sModified;
	}

    mpEngine->GetGraphics()->GetLowLevel()->SetWindowCaption(sTitlebarFilename + " - " + msCaption);

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

void cModelEditor::OnInitInput()
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

void cModelEditor::OnLoadConfig()
{
	//////////////////////////////////////////////////////////////
	// Set up loading stuff that is specific to this editor, 
	// and stuff like log filename (this is done pre engine creation)
	tWString sConfigFile = GetHomeDir() + _W("ModelEditor.cfg");

	mpLocalConfig = hplNew(cConfigFile, (sConfigFile));
	mpLocalConfig->Load();

	SetSettingValue("ScreenWidth", mpLocalConfig->GetString("Screen","Width","1024"));
	SetSettingValue("ScreenHeight", mpLocalConfig->GetString("Screen","Height","768"));
	SetSettingValue("FullScreen", mpLocalConfig->GetString("Screen","Fullscreen","false"));

	 // Set Input config
	SetSettingValue("TumbleFactor", mpLocalConfig->GetString("Input", "TumbleSpeed", "0.005"));
	SetSettingValue("TrackFactor", mpLocalConfig->GetString("Input", "TrackFactor", "0.005"));
	SetSettingValue("ZoomFactor", mpLocalConfig->GetString("Input", "ZoomFactor", "0.001"));
	SetSettingValue("MouseWheelZoom", mpLocalConfig->GetString("Input", "MouseWheelZoom", "0.1"));

	SetSettingValue("RotateSnap", mpLocalConfig->GetString("Options","RotateSnap", cString::ToString(kPi2f/3)));
	SetSettingValue("ScaleSnap",  mpLocalConfig->GetString("Options","ScaleSnap", "0.5"));
	mpActionHandler->SetMaxUndoSize(mpLocalConfig->GetInt("Options","UndoStackSize", 10));

	for(int i=0;i<10;++i)
	{
		tWString sRecent = mpLocalConfig->GetStringW("Misc", "RecentFile" + cString::ToString(i+1), _W(""));

		if(sRecent==_W(""))
			break;

		if(cPlatform::FileExists(sRecent))
			mlstRecentFiles.push_back(sRecent);
	}


	msCaption = "HPL Model Editor";
	SetLogFile(GetHomeDir() + _W("ModelEditor.log"));

	msLastLoadPath = mpLocalConfig->GetStringW("Directories", "LastUsedPath", GetMainLookUpDir(eDir_Maps));
	msLastMeshPath = mpLocalConfig->GetStringW("Directories", "LastUsedMeshPath", _W(""));
}

//--------------------------------------------------------------------

void cModelEditor::OnSaveConfig()
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

	int i=0;
	for(tWStringListIt it=mlstRecentFiles.begin(); it!=mlstRecentFiles.end();++it)
	{
		tWString sRecent = *it;

		if(sRecent==_W(""))
			break;
		
		if(cPlatform::FileExists(sRecent))
		{
			mpLocalConfig->SetString("Misc", "RecentFile" + cString::ToString(++i), (const tString&)cString::To8Char(sRecent));
		}
	}

	mpLocalConfig->SetString("Directories", "LastUsedPath", (const tString&)cString::To8Char(msLastLoadPath));
	mpLocalConfig->SetString("Directories", "LastUsedMeshPath", (const tString&)cString::To8Char(msLastMeshPath));

	mpLocalConfig->Save();
	hplDelete(mpLocalConfig);
}

//--------------------------------------------------------------------

bool cModelEditor::MeshImport_Callback(iWidget* apWidget, const cGuiMessageData& aData)
{
	//SetEditorState(eEditorState_Initial);

	if(aData.mlVal==1)
	{
		tWString sFilenameFullPath = mvLoadFilenames.front();
		tString sFilename = cString::To8Char(sFilenameFullPath);
		
		msImportedMeshFilename = cString::To16Char(cString::Sub(sFilename, 0, cString::GetLastStringPos(sFilename, "."))) + _W(".ent");
		
		iEditorAction* pAction = hplNew( cModelEditorActionMeshLoad,( (cModelEditorWorld*)mpEditorWorld, sFilename));
		AddAction(pAction);

		msSaveFilename=msImportedMeshFilename;
		msLastMeshPath = cString::GetFilePathW(sFilenameFullPath);
		msLastSavePath = msLastMeshPath;
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cModelEditor, MeshImport_Callback);

//--------------------------------------------------------------------

void cModelEditor::AppSpecificLoad(iXmlDocument* apDoc)
{
	
}

void cModelEditor::AppSpecificSave(iXmlDocument* apDoc)
{
	apDoc->SetValue("Entity");
	
	////////////////////////////////////////
	// TODO: Save user settings data here
	/*
	cXmlElement* pXmlUserVars = apDoc->CreateChildElement("UserDefinedVariables");
	pXmlUserVars->SetAttributeString("EntityType", msCurrentTypeName);
	pXmlUserVars->SetAttributeString("EntitySubType", msCurrentSubTypeName);
	tCustomVarListIt it = mlstCurrentUserVars.begin();
	for(;it!=mlstCurrentUserVars.end();++it)
	{
		iCustomVar* pVar = *it;
		cXmlElement* pXmlVar = pXmlUserVars->CreateChildElement("");
		pVar->SaveValueToElement(pXmlVar);
	}
	*/
}

//--------------------------------------------------------------------

iEditorWorld* cModelEditor::CreateSpecificWorld()
{
	return hplNew(cModelEditorWorld,(this));
}

//--------------------------------------------------------------------

cWidgetMainMenu* cModelEditor::CreateMainMenu()
{
	cWidgetMainMenu* pMenu = mpSet->CreateWidgetMainMenu();
	/////////////////////////////////
	// Setup main menu
	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem = NULL;
    
	// File menu
	pItem = pMenu->AddMenuItem(_W("File"));
	mpMainMenuNew = pItem->AddMenuItem(_W("New"));
	mpMainMenuNew->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	
	pItem->AddSeparator();
	
	mpMainMenuLoad = pItem->AddMenuItem(_W("Open"));
	mpMainMenuLoad->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	
	pItem->AddSeparator();
	
	mpMainMenuSave = pItem->AddMenuItem(_W("Save"));
	mpMainMenuSave->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuSave->AddShortcut(eKeyModifier_Ctrl,eKey_S);
	
	mpMainMenuSaveAs = pItem->AddMenuItem(_W("Save as"));
	mpMainMenuSaveAs->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	
	pItem->AddSeparator();

	mpMainMenuImport = pItem->AddMenuItem(_W("Import Mesh"));
	mpMainMenuImport->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	
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
	pItem = pMenu->AddMenuItem(_W("Edit"));
	mpMainMenuUndo =  pItem->AddMenuItem(_W("Undo"));
	mpMainMenuUndo->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_UndoRedo));
	mpMainMenuUndo->AddShortcut(eKeyModifier_Ctrl,eKey_Z);
	mpMainMenuRedo = pItem->AddMenuItem(_W("Redo"));
	mpMainMenuRedo->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_UndoRedo));
	mpMainMenuRedo->AddShortcut(eKeyModifier_Ctrl,eKey_Y);
	pItem->AddSeparator();
	mpMainMenuDelete = pItem->AddMenuItem(_W("Delete"));
	mpMainMenuDelete->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuDelete->AddShortcut(eKeyModifier_None,eKey_BackSpace);
	mpMainMenuDelete->AddShortcut(eKeyModifier_None,eKey_Delete);
	mpMainMenuClone = pItem->AddMenuItem(_W("Clone"));
	mpMainMenuClone->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuClone->AddShortcut(eKeyModifier_Ctrl,eKey_D);

	pItem = pMenu->AddMenuItem(_W("View"));
	mpMainMenuOutline = pItem->AddMenuItem(_W("Outline"));
	mpMainMenuOutline->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	
	// Settings menu
	pItem = pMenu->AddMenuItem(_W("Settings"));
	mpMainMenuUserSettings = pItem->AddMenuItem(_W("User defined variables"));
	mpMainMenuUserSettings->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuAnimations = pItem->AddMenuItem(_W("Animations"));
	mpMainMenuAnimations->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));

	return pMenu;
}

//--------------------------------------------------------------------

iEditorWindowLowerToolbar* cModelEditor::CreateSpecificLowerToolbar()
{
	return hplNew(cModelEditorLowerToolbar,(this));
}

//--------------------------------------------------------------------
