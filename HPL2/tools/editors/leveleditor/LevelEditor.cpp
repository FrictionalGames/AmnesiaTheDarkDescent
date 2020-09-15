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

#include "LevelEditor.h"

#include "../common/DirectoryHandler.h"

#include "../common/EditorActionHandler.h"

#include "../common/EditorEditModeStaticObjects.h"
#include "../common/EditorEditModeEntities.h"
#include "../common/EditorEditModeSelect.h"
#include "../common/EditorEditModeLights.h"
#include "../common/EditorEditModeAreas.h"
#include "../common/EditorEditModeBillboards.h"
#include "../common/EditorEditModeSounds.h"
#include "../common/EditorEditModeParticleSystems.h"
#include "../common/EditorEditModePrimitive.h"
#include "../common/EditorEditModeDecals.h"
#include "../common/EditorEditModeFogAreas.h"
#include "../common/EditorEditModeCombine.h"

#include "../common/EditorWindowFactory.h"
#include "../common/EditorWindowViewport.h"
#include "../common/EditorWindowStaticObjects.h"
#include "../common/EditorWindowSelect.h"
#include "../common/EditorWindowEntitySearch.h"
#include "../common/EditorWindowTextureBrowser.h"
#include "../common/EditorWindowOptions.h"
#include "../common/EditorWindowLowerToolbar.h"
#include "../common/EditorWindowEditModeSidebar.h"


#include "../common/EditorGrid.h"
#include "../common/EditorClipPlane.h"
#include "../common/EditorSelection.h"

#include "../common/EntityWrapper.h"
#include "../common/EntityWrapperCompoundObject.h"
#include "../common/EditorActionCompoundObject.h"
#include "../common/EditorHelper.h"
#include "../common/EditorUserClassDefinitionManager.h"

#include "../common/EngineEntity.h"

#include "LevelEditorWindowGroup.h"
#include "LevelEditorWindowLevelSettings.h"
#include "LevelEditorWorld.h"
#include "LevelEditorActions.h"

#include <algorithm>


unsigned int cLevelEditorGroup::mlGroupCounter = 1;

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// GROUP
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cLevelEditorGroup::cLevelEditorGroup()
{
}

//--------------------------------------------------------------------

cLevelEditorGroup::cLevelEditorGroup(cLevelEditor* apEditor, unsigned int alID,const tString& asName)
{
	mpEditor = apEditor;
	mlID = alID;
	msName = asName;
}

//--------------------------------------------------------------------

void cLevelEditorGroup::SetVisibility(bool abX)
{
	mbVisibility = abX;

	tIntList lstEntityIDs;
	tEntityWrapperMapIt it = mpEditor->GetEditorWorld()->GetEntities().begin();

	for(;it!=mpEditor->GetEditorWorld()->GetEntities().end();++it)
	{
		iEntityWrapper* pEnt = it->second;
		cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)pEnt->GetEntityExtData();
        
		if(pData->mlGroupID==mlID)
			pEnt->SetVisible(abX);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// LEVEL EDITOR
///////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

cLevelEditor::cLevelEditor() : iEditorBase(_W("Maps"), _W("*.map"))
{
}

cLevelEditor::~cLevelEditor()
{
	OnSaveConfig();
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void cLevelEditor::AppSpecificReset()
{
	///////////////////////////////////////
	// Clear state
	// This sets select as the current edit mode.
	SetCurrentEditMode(GetEditMode("Select"));

	///////////////////////////////////////
	// Viewport Config
	// Reset all view ports to their presets
	for(int i=0;i<4;++i)
	{
		mvViewports[i]->SetPreset( (eEditorWindowViewportPreset)i ) ;
	}

	///////////////////////////////////////
	// Groups
	// Remove all previously created groups and create a "None" group. 
    mmapGroups.clear();
	cLevelEditorGroup group(this,0,"None");
	group.SetVisibility(true);
	mmapGroups.insert(pair<unsigned int, cLevelEditorGroup>(0,group));
}

//--------------------------------------------------------------------

void cLevelEditor::LookAtEntity(int alEntityID)
{
	iEntityWrapper* pEntity = mpEditorWorld->GetEntity(alEntityID);

	if(pEntity==NULL)
		return;

	for(int i=0;i<4;++i)
	{
		mvViewports[i]->LookAtEntity(pEntity);
	}
}

//--------------------------------------------------------------------

void cLevelEditor::Command_Import()
{
	ShowLoadFilePicker(mvLoadFilenames, msLastImportPath, 
						this, kGuiCallback(ImportFileCallback), 
						_W("Exported objects"), tWStringList(1, _W("*.expobj")));
}

//--------------------------------------------------------------------

void cLevelEditor::Command_Export()
{
	SetFlags(eEditorFlag_PopUpActive, true);

	cGuiPopUpFilePicker* pPicker = mpSet->CreatePopUpSaveFilePicker(msLastImportFile, _W("Exported objects"), 
																	_W("*.expobj"), msLastSavePath, 
																	false, this, kGuiCallback(ExportFileCallback));
	pPicker->AddOnDestroyCallback(this, kGuiCallback(PopUpCloseCallback));
}

//--------------------------------------------------------------------

void cLevelEditor::AddGroup(unsigned int alID, const tString& asName)
{
	cLevelEditorGroup group(this,alID,asName);
	group.SetVisibility(true);

	mmapGroups.insert(pair<unsigned int, cLevelEditorGroup>(alID,group));
}

//--------------------------------------------------------------------

cLevelEditorGroup& cLevelEditor::GetGroup(unsigned int alID)
{
	return mmapGroups[alID];
}

//--------------------------------------------------------------------

void cLevelEditor::RemoveGroup(unsigned int alID)
{
	mmapGroups.erase(alID);
}

//--------------------------------------------------------------------

void cLevelEditor::SetUpClassDefinitions(cEditorUserClassDefinitionManager* apManager)
{
	apManager->RegisterDefFilename(eUserClassDefinition_Entity, "EntityTypes.cfg", eEditorVarCategory_Instance|eEditorVarCategory_EditorSetup);
	apManager->RegisterDefFilename(eUserClassDefinition_Area, "AreaTypes.cfg", eEditorVarCategory_Instance|eEditorVarCategory_EditorSetup);
}

//--------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

bool cLevelEditor::MainMenu_ItemClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	////////////////////////////////
	// Update Delete, Clone and Compound menu items
	if(mpCurrentEditMode==GetEditMode("Select"))
	{
		cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*) mpCurrentEditMode;

		tIntList lstEntityIDs = mpSelection->GetEntityIDs();

		if(apWidget==mpMainMenuDelete)
			AddAction(pEditMode->CreateDeleteEntitiesAction(lstEntityIDs));
		else if(apWidget==mpMainMenuClone)
			AddAction(pEditMode->CreateCloneEntitiesAction(lstEntityIDs));
		else if(apWidget==mpMainMenuCompound)
		{
			if(mpSelection->GetNumEntities()==1 && mpSelection->GetEntities().front()->GetTypeID()==eEditorEntityType_Compound)
			{
				cEntityWrapperCompoundObject* pObj = (cEntityWrapperCompoundObject*)mpSelection->GetEntities().front();
				tIntList lstSubEntityIDs;
				cEditorHelper::GetIDsFromEntityList(pObj->GetComponents(), lstSubEntityIDs);
				AddAction(hplNew(cEditorActionCompoundObjectRemoveEntities,(mpEditorWorld,lstSubEntityIDs)));
			}
			else
                AddAction(pEditMode->CreateCompoundObjectAction());
		}
	}

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
	// Menu Item "File.Import"
	else if(apWidget==mpMainMenuImport)
	{
		Command_Import();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Export"
	else if(apWidget==mpMainMenuExport)
	{
		Command_Export();
	}
	///////////////////////////////////////////////
	// Menu Item "File.Exit"
	else if(apWidget==mpMainMenuExit)
	{
		Command_Exit();
	}
	///////////////////////////////////////////////
	// Menu Item "Edit.Entity Search"
	else if(apWidget==mpMainMenuSearch)
	{
		mpWindowSearch->SetActive(true);
	}
	///////////////////////////////////////////////
	// Menu Item "Edit.Group Window"
	else if(apWidget==mpMainMenuGroup)
	{
		mpWindowGroup = hplNew( cLevelEditorWindowGroup, (this));
		mpWindowGroup->Init();
		mpWindowGroup->SetActive(true);
		AddWindow(mpWindowGroup);
	}
	///////////////////////////////////////////////
	// Menu Item "Edit.Level settings"
	else if(apWidget==mpMainMenuLevelSettings)
	{
		iEditorWindow* pWindow = hplNew(cLevelEditorWindowLevelSettings,(this));
		pWindow->Init();
		pWindow->SetActive(true);
		AddWindow(pWindow);
	}
	///////////////////////////////////////////////
	// Menu Item "Edit.Options"
	else if(apWidget==mpMainMenuOptions)
	{
		mpWindowOptions = hplNew(cEditorWindowOptions,(this));
		mpWindowOptions->Init();
		mpWindowOptions->SetActive(true);
		AddWindow(mpWindowOptions);
	}


	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditor, MainMenu_ItemClick);

//--------------------------------------------------------------------

bool cLevelEditor::MainMenu_UndoRedo(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget==mpMainMenuUndo)
		mpActionHandler->Undo();
	else if(apWidget==mpMainMenuRedo)
		mpActionHandler->Redo();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditor, MainMenu_UndoRedo);

//--------------------------------------------------------------------

iEditorWorld* cLevelEditor::CreateSpecificWorld()
{
	return hplNew(cLevelEditorWorld,(this));
}

//--------------------------------------------------------------------

void cLevelEditor::LoadEditorSession(iXmlDocument* apDoc, cXmlElement** apElement)
{
	iEditorBase::LoadEditorSession(apDoc, apElement);
	cXmlElement* pSession = *apElement;

	///////////////////////////////////////
	// Group Data
	cXmlElement* pGroups = pSession->GetFirstElement("Groups");
	if(pGroups)
	{
		cXmlNodeListIterator it = pGroups->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pGroup = it.Next()->ToElement();
			unsigned int lID = pGroup->GetAttributeInt("ID", 0);
			tString sName = pGroup->GetAttributeString("Name", "");
			bool bVisible = pGroup->GetAttributeBool("Visible", true);

			AddGroup(lID,sName);
			GetGroup(lID).SetVisibility(bVisible);
		}
	}

	cXmlElement* pClipPlanes = pSession->GetFirstElement("ClipPlanes");
	if(pClipPlanes)
	{
		cXmlNodeListIterator it = pClipPlanes->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pXmlPlane = it.Next()->ToElement();
			cEditorClipPlane* pPlane = mpEditorWorld->AddClipPlane();
			pPlane->Load(pXmlPlane);
		}

		mpLowerToolbar->SetFocusedClipPlane(pClipPlanes->GetAttributeInt("Focused",0));
	}
}

//--------------------------------------------------------------------

bool cLevelEditor::ImportFileCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastImportFile = mvLoadFilenames.front();
		tIntList lstImportedIDs;
		mpEditorWorld->ImportObjects(cString::To8Char(msLastImportFile), lstImportedIDs);

		cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)GetEditMode("Select");
		AddAction(pEditMode->CreateSelectEntityAction(tIntList(), eSelectActionType_Clear));
		AddAction(pEditMode->CreateSelectEntityAction(lstImportedIDs, eSelectActionType_Select));
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditor, ImportFileCallback);

//--------------------------------------------------------------------

bool cLevelEditor::ExportFileCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		mpEditorWorld->ExportObjects(cString::To8Char(msLastImportFile), mpSelection->GetEntities());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLevelEditor, ExportFileCallback);

//--------------------------------------------------------------------


void cLevelEditor::SaveEditorSession(iXmlDocument* apDoc, cXmlElement** apElement)
{
	iEditorBase::SaveEditorSession(apDoc, apElement);
	cXmlElement* pSession = *apElement;

	///////////////////////////////////////
	// Group Data
	cXmlElement* pXmlGroups = pSession->CreateChildElement("Groups");
	tGroupMapIt itGroups = mmapGroups.begin();
	for(;itGroups!=mmapGroups.end();++itGroups)
	{
		cLevelEditorGroup* pGroup = &(itGroups->second);
		cXmlElement* pXmlGroup = pXmlGroups->CreateChildElement("Group");

		pXmlGroup->SetAttributeInt("ID", pGroup->GetID());
		pXmlGroup->SetAttributeString("Name", pGroup->GetName());
		pXmlGroup->SetAttributeBool("Visible", pGroup->GetVisibility());
	}
	
	tEditorClipPlaneVec& vClipPlanes = mpEditorWorld->GetClipPlanes();
	if(vClipPlanes.empty()==false)
	{
		cXmlElement* pXmlClipPlanes = pSession->CreateChildElement("ClipPlanes");
		for(int i=0;i<(int)vClipPlanes.size();++i)
		{
			cEditorClipPlane* pPlane = vClipPlanes[i];
			cXmlElement* pXmlPlane = pXmlClipPlanes->CreateChildElement();
			
			pPlane->Save(pXmlPlane);
		}

		pXmlClipPlanes->SetAttributeInt("Focused", mpLowerToolbar->GetFocusedClipPlane());
	}
}

//--------------------------------------------------------------------

void cLevelEditor::UpdateEditMenu()
{
	mpMainMenuUndo->SetEnabled( !mpActionHandler->IsDoneActionsListEmpty());
	mpMainMenuRedo->SetEnabled( !mpActionHandler->IsUndoneActionsListEmpty());

	bool bHasSelectedObjects = (mpSelection->IsEmpty()==false);

	mpMainMenuDelete->SetEnabled(bHasSelectedObjects && mpSelection->IsDeletable());
	mpMainMenuClone->SetEnabled(bHasSelectedObjects && mpSelection->IsCloneable());
	mpMainMenuCompound->SetEnabled(mpSelection->GetNumEntities()>0);
}

//--------------------------------------------------------------------


//--------------------------------------------------------------------

void cLevelEditor::OnInit()
{
	/////////////////////////////////////////////////////////
	// Add any extra dirs to resources!
	// static objects
	for(int i=0;i<(int)mvExtraSODirs.size();++i)
	{
		const tWString& sExtraDir = mvExtraSODirs[i];
		mpEngine->GetResources()->AddResourceDir(sExtraDir, true);
	}
	// entities
	for(int i=0;i<(int)mvExtraEntDirs.size();++i)
	{
		const tWString& sExtraDir = mvExtraEntDirs[i];
		mpEngine->GetResources()->AddResourceDir(sExtraDir, true);
	}
	///////////////////////////////////////////////////
	// Add EditModes here!
	mpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");

	AddEditMode(hplNew(cEditorEditModeSelect,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeLights,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeBillboards,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeParticleSystems,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeSounds,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeStaticObjects,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeEntities,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeAreas,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModePrimitives,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeDecals,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeFogAreas,(this, mpEditorWorld)));
	AddEditMode(hplNew(cEditorEditModeCombine, (this)));
}

//--------------------------------------------------------------------

void cLevelEditor::OnInitLayout()
{	
	///////////////////////////////////
	// EditMode Tool box
	mpEditModeSidebar->SetPosition(cVector3f(0,mpMainMenu->GetSize().y+2,1));

    ///////////////////////////////////
	// Lower Toolbar
	mpLowerToolbar->SetPosition(cVector3f(GetLayoutVec3f(eLayoutVec3_ViewportAreaPos).x, mvScreenSize.y-50,1));
	cVector3f vHandlePos = cVector3f(5,5,0.1f);
	iWidget* pHandle = mpLowerToolbar->AddGridControls();
	pHandle->SetPosition(vHandlePos);
	vHandlePos = vHandlePos + cVector3f(pHandle->GetSize().x+10,0,0);
	pHandle = mpLowerToolbar->AddViewportControls();
	pHandle->SetPosition(vHandlePos);
	vHandlePos = vHandlePos + cVector3f(pHandle->GetSize().x+10,0,0);
	pHandle = mpLowerToolbar->AddLightingControls();
	pHandle->SetPosition(vHandlePos);
	vHandlePos += cVector3f(pHandle->GetSize().x+10, 0, 0);
	pHandle = mpLowerToolbar->AddCameraControls();
	pHandle->SetPosition(vHandlePos);
	vHandlePos += cVector3f(pHandle->GetSize().x+10, 0, 0);
	pHandle = mpLowerToolbar->AddClipPlaneControls();
	pHandle->SetPosition(vHandlePos);

	////////////////////////////////////
	// Search Window
	mpWindowSearch = cEditorWindowFactory::CreateSearchWindow(this, (cEditorEditModeSelect*)GetEditMode("Select"));
}

//--------------------------------------------------------------------

void cLevelEditor::OnSetUpDirectories()
{
	const tWString& sWorkingDir = GetWorkingDir();

	mpDirHandler->AddLookUpDir(eDir_Maps, sWorkingDir + mpMainConfig->GetStringW("Directories", "MapsDir", _W("maps")), true); 
	mpDirHandler->AddLookUpDir(eDir_StaticObjects, sWorkingDir + mpMainConfig->GetStringW("Directories", "StaticObjectsDir", _W("static_objects")), true);
	mpDirHandler->AddLookUpDir(eDir_Entities, sWorkingDir + mpMainConfig->GetStringW("Directories", "EntitiesDir", _W("entities")), true);
	mpDirHandler->AddLookUpDir(eDir_Decals, sWorkingDir + mpMainConfig->GetStringW("Directories", "DecalsDir", _W("textures/decals")), true);
}

//--------------------------------------------------------------------

void cLevelEditor::OnUpdate(float afTimeStep)
{
	/*iWidget* pWidget = mpSet->GetAttentionWidget();
	iWidget* pWidgetParent = pWidget?pWidget->GetParent():NULL;
	Log("Current attention widget: 0x%x - %s, parent: 0x%x - %s\n", pWidget, pWidget?pWidget->GetTypeString().c_str():"NULL", 
																	pWidgetParent, pWidgetParent?pWidgetParent->GetTypeString().c_str():"NULL");
																	*/
}

//--------------------------------------------------------------------

void cLevelEditor::OnPostUpdateLayout()
{
	///////////////////////////////////////
	// Update Title Bar
	tString sTitlebarFilename;

	if(msSaveFilename ==_W(""))
		sTitlebarFilename = "Unnamed Map";
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

void cLevelEditor::OnInitInput()
{
	/////////////////////////////////////////////////////////
	// Set up Special input keys
}

//--------------------------------------------------------------------

void cLevelEditor::OnLoadConfig()
{
	//////////////////////////////////////////////////////////////
	// Set up loading stuff that is specific to this editor, 
	// and stuff like log filename (this is done pre engine creation)
	tWString sConfigFile = GetHomeDir() + _W("LevelEditor.cfg");

	mpLocalConfig = hplNew(cConfigFile, ( sConfigFile));
	mpLocalConfig->Load();

	SetSettingValue("ScreenWidth", mpLocalConfig->GetString("Screen","Width","1024"));
	SetSettingValue("ScreenHeight", mpLocalConfig->GetString("Screen","Height","768"));
	SetSettingValue("FullScreen", mpLocalConfig->GetString("Screen","Fullscreen","false"));

	SetSettingValue("TexQuality", mpLocalConfig->GetString("Screen", "TexQuality", "0"));

	 // Set Input config
	SetSettingValue("TumbleFactor", mpLocalConfig->GetString("Input", "TumbleSpeed", "0.005"));
	SetSettingValue("TrackFactor", mpLocalConfig->GetString("Input", "TrackFactor", "0.01"));
	SetSettingValue("ZoomFactor", mpLocalConfig->GetString("Input", "ZoomFactor", "0.001"));
	SetSettingValue("MouseWheelZoom", mpLocalConfig->GetString("Input", "MouseWheelZoom", "0.1"));

	cEditorSelection::SetRotateSnap(mpLocalConfig->GetFloat("Options","RotateSnap", kPi2f/6));
	cEditorSelection::SetScaleSnap(mpLocalConfig->GetFloat("Options","ScaleSnap", 0.25f));

	mpActionHandler->SetMaxUndoSize(mpLocalConfig->GetInt("Options","UndoStackSize", 50));
	iEngineEntityMesh::SetDisabledCoverage(mpLocalConfig->GetFloat("Options", "DisabledCoverage", 0.5f));

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
			if(itFoundString!=mlstRecentFiles.end())
				mlstRecentFiles.erase(itFoundString);
            
			mlstRecentFiles.push_back(sRecent);
		}
	}

	// Window caption
	msCaption = "HPL Level Editor";
	
	SetLogFile(GetHomeDir() + _W("LevelEditor.log"));

	msLastLoadPath = mpLocalConfig->GetStringW("Directories", "LastUsedPath", GetMainLookUpDir(eDir_Maps));

	///////////////////////////////////////////
	// Get extra dirs for static objects
	int j=1;
	while(true)
	{
		tWString sExtraDir = mpLocalConfig->GetStringW("Directories", "ExtraStaticObjectDir" + cString::ToString(j), _W(""));
		if(sExtraDir==_W(""))
		{
			break;
		}

		if(cPlatform::FolderExists(sExtraDir))
		{
			mvExtraSODirs.push_back(sExtraDir);
		}
		else
		{
			Log("Extra StaticObject Directory %ls does not exist\n", sExtraDir.c_str());
		}

		++j;
	}

	///////////////////////////////////////////
	// Get extra dirs for entities
	j=1;
	while(true)
	{
		tWString sExtraDir = mpLocalConfig->GetStringW("Directories", "ExtraEntityDir" + cString::ToString(j), _W(""));
		if(sExtraDir==_W(""))
		{
			break;
		}

		if(cPlatform::FolderExists(sExtraDir))
			mvExtraEntDirs.push_back(sExtraDir);
		else
			Log("Extra Entity Directory %ls does not exist\n", sExtraDir.c_str());

		++j;
	}
}

//--------------------------------------------------------------------

void cLevelEditor::OnSaveConfig()
{
	mpLocalConfig->SetString("Screen","Width", GetSetting("ScreenWidth"));
	mpLocalConfig->SetString("Screen","Height", GetSetting("ScreenHeight"));
	mpLocalConfig->SetString("Screen","Fullscreen", GetSetting("Fullscreen"));

	mpLocalConfig->SetString("Screen","TexQuality", GetSetting("TexQuality"));

	mpLocalConfig->SetString("Input", "TumbleFactor", GetSetting("TumbleFactor"));
	mpLocalConfig->SetString("Input", "TrackFactor", GetSetting("TrackFactor"));
	mpLocalConfig->SetString("Input", "ZoomFactor", GetSetting("ZoomFactor"));
	mpLocalConfig->SetString("Input", "MouseWheelZoom", GetSetting("MouseWheelZoom"));

	mpLocalConfig->SetFloat("Options", "RotateSnap", cEditorSelection::GetRotateSnap());
	mpLocalConfig->SetFloat("Options", "ScaleSnap", cEditorSelection::GetScaleSnap());

	mpLocalConfig->SetInt("Options","UndoStackSize",(int)mpActionHandler->GetMaxUndoSize());
	mpLocalConfig->SetFloat("Options", "DisabledCoverage", iEngineEntityMesh::GetDisabledCoverage());

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

	// Save extra dirs
	for(int j=0;j<(int)mvExtraSODirs.size();++j)
	{
		tString sExtraDir = cString::S16BitToUTF8(mvExtraSODirs[j]);
		
		mpLocalConfig->SetString("Directories", "ExtraStaticObjectDir" + cString::ToString(j+1), sExtraDir);
	}
	for(int j=0;j<(int)mvExtraEntDirs.size();++j)
	{
		tString sExtraDir = cString::S16BitToUTF8(mvExtraEntDirs[j]);
		
		mpLocalConfig->SetString("Directories", "ExtraEntityDir" + cString::ToString(j+1), sExtraDir);
	}

	mpLocalConfig->Save();
	hplDelete(mpLocalConfig);
}

//--------------------------------------------------------------------

cWidgetMainMenu* cLevelEditor::CreateMainMenu()
{
	/////////////////////////////////
	//Setup main menu
	cWidgetMenuItem* pItem = NULL;
	cWidgetMenuItem* pSubItem = NULL;
    
	mpMainMenu = mpSet->CreateWidgetMainMenu(mpBGFrame);

	//File menu
	pItem = mpMainMenu->AddMenuItem(_W("File"));
	// New
	mpMainMenuNew = pItem->AddMenuItem(_W("New"));
	mpMainMenuNew->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	// Open
	mpMainMenuLoad = pItem->AddMenuItem(_W("Open"));
	mpMainMenuLoad->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();
	// Save
	mpMainMenuSave = pItem->AddMenuItem(_W("Save"));
	mpMainMenuSave->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuSave->AddShortcut(eKeyModifier_Ctrl, eKey_S);

	// Save As
	mpMainMenuSaveAs = pItem->AddMenuItem(_W("Save as"));
	mpMainMenuSaveAs->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	// Open Recent
	mpMainMenuRecent = pItem->AddMenuItem(_W("Open Recent"));

	pItem->AddSeparator();

	// Import
	mpMainMenuImport = pItem->AddMenuItem(_W("Import objects"));
	mpMainMenuImport->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	
	// Export
	mpMainMenuExport = pItem->AddMenuItem(_W("Export selection"));
	mpMainMenuExport->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	// Quit
	mpMainMenuExit = pItem->AddMenuItem(_W("Quit"));
	mpMainMenuExit->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
#if defined(WIN32)
	mpMainMenuExit->AddShortcut(eKeyModifier_Alt, eKey_F4);
#elif defined(__linux__)
	mpMainMenuExit->AddShortcut(eKeyModifier_Ctrl, eKey_Q);
#elif defined(__APPLE__)
	mpMainMenuExit->AddShortcut(eKeyModifier_Ctrl, eKey_Q);
#endif
    
	//Edit menu
	pItem = mpMainMenu->AddMenuItem(_W("Edit"));
	// Undo
	mpMainMenuUndo =  pItem->AddMenuItem(_W("Undo"));
	mpMainMenuUndo->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_UndoRedo));
	mpMainMenuUndo->AddShortcut(eKeyModifier_Ctrl, eKey_Z);
	// Redo
	mpMainMenuRedo = pItem->AddMenuItem(_W("Redo"));
	mpMainMenuRedo->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_UndoRedo));
	mpMainMenuRedo->AddShortcut(eKeyModifier_Ctrl, eKey_Y);

	pItem->AddSeparator();

	// Delete
	mpMainMenuDelete = pItem->AddMenuItem(_W("Delete"));
	mpMainMenuDelete->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuDelete->AddShortcut(eKeyModifier_None, eKey_BackSpace);
	mpMainMenuDelete->AddShortcut(eKeyModifier_None, eKey_Delete);
	// Duplicate
	mpMainMenuClone = pItem->AddMenuItem(_W("Duplicate"));
	mpMainMenuClone->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuClone->AddShortcut(eKeyModifier_Ctrl, eKey_D);
	// Create/Destroy compound
	mpMainMenuCompound = pItem->AddMenuItem(_W("Create/Destroy Compound object"));
	mpMainMenuCompound->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(MainMenu_ItemClick));
	mpMainMenuCompound->AddShortcut(eKeyModifier_None, eKey_B);

	pItem->AddSeparator();

	// Search
	mpMainMenuSearch = pItem->AddMenuItem(_W("Find objects"));
	mpMainMenuSearch->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));
	mpMainMenuSearch->AddShortcut(eKeyModifier_Ctrl, eKey_F);

	// Groups
	mpMainMenuGroup = pItem->AddMenuItem(_W("Browse groups"));
	mpMainMenuGroup->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	// Level settings
	mpMainMenuLevelSettings = pItem->AddMenuItem(_W("Level settings"));
	mpMainMenuLevelSettings->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	pItem->AddSeparator();

	// Options
	mpMainMenuOptions = pItem->AddMenuItem(_W("Options"));
	mpMainMenuOptions->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(MainMenu_ItemClick));

	return mpMainMenu;
}

//--------------------------------------------------------------------
