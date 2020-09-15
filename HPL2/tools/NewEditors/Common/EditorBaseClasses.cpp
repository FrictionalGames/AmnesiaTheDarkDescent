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

#include "EditorBaseClasses.h"

#include "EditorWindowFactory.h"
#include "EditorWindow.h"
#include "EditorWindowViewport.h"
#include "EditorWindowLoaderStatus.h"
#include "EditorWindowTextureBrowser.h"
#include "EditorWindowMaterialEditor.h"
#include "EditorWindowSoundBrowser.h"
#include "EditorWindowLowerToolbar.h"
#include "EditorWindowEditModeSidebar.h"

#include "EditorEditMode.h"

#include "EditorThumbnailBuilder.h"
#include "EditorActionHandler.h"
#include "EditorGrid.h"
#include "EditorWorld.h"
#include "EditorSelection.h"
#include "EditorHelper.h"

#include "EditorInput.h"

#include "EditorAction.h"
#include "EditorActionMisc.h"

#include <algorithm>

#include "../common/DirectoryHandler.h"

#include "../common/StdAfx.h"

using namespace hpl;

static cVector3f gvZeroVec3f = cVector3f(0,0,0);
static cVector2f gvZeroVec2f = cVector2f(0,0);

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// ENTITY LOADER CLASS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEditorEntityLoader::cEditorEntityLoader(iEditorBase* apEditor) : cEntityLoader_Object("EditorEntityLoader")
{
	Log("Created Entity Loader\n");
	mpEditor = apEditor;
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cMeshEntity* cEditorEntityLoader::LoadEntFile(int alID, const tString& asName, const tString& asFilename,
											  cWorld* apWorld, bool abLoadAnims, bool abLoadParticles, bool abLoadBillboards, bool abLoadSounds, bool abLoadLights)
{
	cResources* pRes = mpEditor->GetEngine()->GetResources();
	tWString sFullPath = pRes->GetFileSearcher()->GetFilePath(asFilename);

	cMeshEntity* pEntity = NULL;
	cXmlElement* pXmlEntity = NULL;

	iXmlDocument* pDoc = pRes->LoadXmlDocument(asFilename);
	if(pDoc==NULL)
		return NULL;

	pEntity = LoadEntityFromElement(alID, 
									asName,
									pDoc,
									apWorld,
									asFilename,
									sFullPath,
									abLoadAnims,
									abLoadParticles,
									abLoadBillboards,
									abLoadSounds,
									abLoadLights);

	
	pRes->DestroyXmlDocument(pDoc);

	return pEntity;
}
//-----------------------------------------------------------------------

cMeshEntity* cEditorEntityLoader::LoadEntityFromElement(int alID, const tString& asName, cXmlElement* apElement,
														cWorld* apWorld, const tString& asFilename, const tWString& asFullPath, 
														bool abLoadAnims, bool abLoadParticles, bool abLoadBillboards, bool abLoadSounds, bool abLoadLights)
{
	mbLoadAnimations = abLoadAnims;
	mbLoadParticleSystems = abLoadParticles;
	mbLoadBillboards = abLoadBillboards;
	mbLoadSounds = abLoadSounds;
	mbLoadLights = abLoadLights;

	cMeshEntity* pEntity = (cMeshEntity*) Load(asName,
											   alID,
											   true,
											   apElement,
											   cMatrixf::Identity,
											   cVector3f(1),
											   apWorld,
											   asFilename, 
											   asFullPath, 
											   NULL);


	return pEntity;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// EDITOR BASE CLASS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iEditorBase::iEditorBase(const tWString& asFileCategoryName, const tWString& asFileCategoryString) : iUpdateable("EditorUpdate")
{
	mpCurrentEditMode = NULL;
	mpFocusedViewport = NULL;
	msCaption = "";

	mpMainMenu = NULL;

	mpDirHandler = hplNew(cDirectoryHandler,(this));
	mpActionHandler = hplNew(cEditorActionHandler, (this));
	mpSelection = hplNew(cEditorSelection, (this));

	mfTimer = 0;
	mlFlags = 0;

	mbLayoutNeedsUpdate = false;
	mbDestroyingEditor = false;
	mbWorldModified = false;

	msFileCategoryName = asFileCategoryName;
	msFileCategoryString = asFileCategoryString;

	mpTempWorld=NULL;

	mpEntityLoader=NULL;

	mbDestroyEngineOnExit=false;

	mbViewportLocked = false;

	mpLowerToolbar = NULL;
}

iEditorBase::~iEditorBase()
{
	mbDestroyingEditor = true;

	hplDelete(mpClassDefManager);

	/////////////////////////////////////
	//Save current configuration
	SaveConfig();

	/////////////////////////////////////
	//Destroy all windows
	for(tEditorWindowListIt it=mlstWindows.begin(); it!=mlstWindows.end(); ++it)
		cEditorWindowFactory::DestroyEditorWindow(*it);

	STLDeleteAll(mvEditModes);

	/////////////////////////////////////
	//Destroy all helper classes
	hplDelete(mpDirHandler);
	hplDelete(mpThumbnailBuilder);
	hplDelete(mpActionHandler);
	hplDelete(mpSelection);

	hplDelete(mpEditorWorld);
	
	/////////////////////////////////////
	//Destroy all Widgets
	mpEngine->GetGui()->DestroySet(mpSet);

	if(mbDestroyEngineOnExit)
		DestroyHPLEngine(mpEngine);
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

void iEditorBase::Reset()
{
	Log("\n/////////////////////////////////////////////////////////////////\n");
	Log("// Resetting scene\n");
	Log("/////////////////////////////////////////////////////////////////\n\n");

	mpSelection->ClearEntities();
	SetLayoutNeedsUpdate(true);

	///////////////////////////////////////
	// Clear save filename
	msSaveFilename = _W("");

	ClearFlags();

	//////////////////////////////////////
	// Reset EditModes
	for(int i=0;i<(int)mvEditModes.size();++i)
		if(mvEditModes[i]) mvEditModes[i]->Reset();


	///////////////////////////////////////
	// Clear actions
	mpActionHandler->Reset();

	///////////////////////////////////////
	// Clear Scene data
	if(mpEditorWorld)
		mpEditorWorld->Reset();

	AppSpecificReset();
}

//-----------------------------------------------------------------------

void iEditorBase::Command_New()
{
	ShowMessageBox(_W("Warning"), 
				   _W("All unsaved data will be lost, continue?"), 
				   _W("Yes"), _W("No"), 
				   this, kGuiCallback(MessageBox_New_Callback));
}

//-----------------------------------------------------------------------

void iEditorBase::Command_Load()
{
	if(mpEditorWorld->IsModified())
	{
		ShowMessageBox(_W("Warning"), 
					   _W("All unsaved data will be lost, continue?"), 
					   _W("Yes"), _W("No"),
					   this, kGuiCallback(MessageBox_Load_Callback));

	}
	else
	{
		ShowLoadFilePicker(mvLoadFilenames, msLastLoadPath, this, kGuiCallback(LoadFileCallback));
	}
}

//-----------------------------------------------------------------------

void iEditorBase::Command_Save()
{
	if(msSaveFilename==_W("") || 
	   cPlatform::FileExists(msSaveFilename)==false)
	{
		Command_SaveAs();
	}
	else
		Save();
}

void iEditorBase::Command_SaveAs()
{
	ShowSaveFilePicker(this, kGuiCallback(SaveFileCallback));
}

//-----------------------------------------------------------------------

void iEditorBase::Command_Exit()
{
	/////////////////////////////////////////////////////////////////////
	// Check if scene has been modified and show a save message popup if so.
	if(mpEditorWorld && mpEditorWorld->IsModified())
	{
		ShowMessageBox(_W("Warning"),
					   _W("There are unsaved changes. Do you want to save them?"),
					   _W("Yes"), _W("No"), 
					   this, kGuiCallback(MessageBox_Exit_UnsavedChanges_Callback));
	}
	else
	{
		ShowMessageBox(_W("Exiting"),_W("Sure you want to exit?"),
				        _W("Yes"), _W("No"),
						this, kGuiCallback(QuitCallback));
	}
}

//-----------------------------------------------------------------------

void iEditorBase::AddRecentFile(const tWString& asFilename)
{
	tWString sFilenameLowerCase = cString::ToLowerCaseW(asFilename);

	//////////////////////////////////////////////////////////////////////////////////////
	// Look for the entry in the recent file list and store positions for all appearances
	tWStringListIt it = mlstRecentFiles.begin();
	std::list<tWStringListIt> lstEntriesToErase;

	for(;it!=mlstRecentFiles.end();++it)
	{
		tWString sRecentFilenameLowerCase = cString::ToLowerCaseW(*it);

		if(sFilenameLowerCase==sRecentFilenameLowerCase)
			lstEntriesToErase.push_back(it);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////
	// Remove all appearances found
	std::list<tWStringListIt>::iterator itEntriesToErase;
	for(itEntriesToErase=lstEntriesToErase.begin(); itEntriesToErase!=lstEntriesToErase.end(); ++itEntriesToErase)
		mlstRecentFiles.erase(*itEntriesToErase);
	
	// Add to back
	mlstRecentFiles.push_back(asFilename);

	// And cut if needed
	if(mlstRecentFiles.size()>10)
		mlstRecentFiles.pop_front();
}

//-----------------------------------------------------------------------

cEditorGrid* iEditorBase::GetGrid()
{
	return mpFocusedViewport->GetGrid();
}

void iEditorBase::SetPosOnGrid(const cVector3f& avPos)
{
	/*if(mpGrid)
		mvSnappedPosOnGrid = mpGrid->GetSnappedPosition(avPos);*/
}

//-----------------------------------------------------------------------

void iEditorBase::Load()
{
	Reset();

	msSaveFilename = mvLoadFilenames.front();
	Log("\n/////////////////////////////////////////////////////////////////\n");
	Log("// Loading file %ls\n", msSaveFilename.c_str());
	Log("/////////////////////////////////////////////////////////////////\n\n");

	iXmlDocument* pDoc = GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument();
	if(pDoc->CreateFromFile(msSaveFilename)==false)
	{
		GetEngine()->GetResources()->DestroyXmlDocument(pDoc);
		return;
	}

	AddRecentFile(msSaveFilename);
	SetLayoutNeedsUpdate(true);
	msLastLoadPath = cString::GetFilePathW(msSaveFilename);
	msLastSavePath = msLastLoadPath;

	//////////////////////////////////////
	// Editor session data
	cXmlElement* pSession = NULL;
	LoadEditorSession(pDoc, &pSession);
	
	//////////////////////////////////
	// Load Scene data
	mpEditorWorld->Load(pDoc);

	OnPostWorldLoad();

	GetEngine()->GetResources()->DestroyXmlDocument(pDoc);
	Log("\n/////////////////////////////////////////////////////////////////\n");
	Log("// Loaded successfully\n");
	Log("/////////////////////////////////////////////////////////////////\n\n");
}

//-----------------------------------------------------------------------

void iEditorBase::Save()
{

	SetLayoutNeedsUpdate(true);

	iXmlDocument* pDoc = GetEngine()->GetResources()->GetLowLevel()->CreateXmlDocument();
	
	AddRecentFile(msSaveFilename);
	msLastLoadPath = cString::GetFilePathW(msSaveFilename);
	msLastSavePath = msLastLoadPath;

	//////////////////////////////////////
	// Editor session data
	cXmlElement* pSession = NULL;
	SaveEditorSession(pDoc, &pSession);

	///////////////////////////////////
	// Save Scene data
	mpEditorWorld->Save(pDoc);
	mpEditorWorld->UpdateSavedModifications();

	pDoc->SaveToFile(msSaveFilename);

	GetEngine()->GetResources()->DestroyXmlDocument(pDoc);
}

//-----------------------------------------------------------------------

void iEditorBase::LoadEditorSession(iXmlDocument* apDoc, cXmlElement **apElement)
{
	///////////////////////////////////////////////////////////////////
	// (TODO: this needs clean up, or be moved elsewhere)
	*apElement = apDoc->GetFirstElement("EditorSession");
	cXmlElement* pSession = *apElement;

	if(pSession==NULL)
		return;

	/////////////////////////////////////
	// Load performance options 
	cXmlElement* pPerfOptions = pSession->GetFirstElement("Performance");
	if(pPerfOptions)
	{
		mpEditorWorld->SetTypeActive(eEditorEntityType_Light,(pPerfOptions->GetAttributeBool("LightsActive", true)));
		mpEditorWorld->SetTypeActive(eEditorEntityType_ParticleSystem,(pPerfOptions->GetAttributeBool("PSActive", true)));
		SetWorldReflectionActive(pPerfOptions->GetAttributeBool("WorldReflection", true));
		cEditorWindowViewport::SetCamPlanes(pPerfOptions->GetAttributeVector2f("CamClipPlanes", cVector2f(0.05f, 1000)));
		mpEditorWorld->SetShowFog(pPerfOptions->GetAttributeBool("ShowFog", true));
		mpEditorWorld->SetShowSkybox(pPerfOptions->GetAttributeBool("ShowSkybox", true));
	}
	cXmlElement* pVPData = pSession->GetFirstElement("ViewportConfig");
	if(pVPData==NULL)
		return;
	int i=0;
	cXmlNodeListIterator it = pVPData->GetChildIterator();
	while(it.HasNext() && i<(int)mvViewports.size())
	{
		cXmlElement* pVP = it.Next()->ToElement();
		mvViewports[i++]->Load(pVP);
	}
	SetFocusedViewport(mvViewports[pVPData->GetAttributeInt("SelectedViewport",0)]);
	SetFlags(eEditorFlag_ViewportEnlarged, pVPData->GetAttributeBool("UsingEnlargedViewport",false));
	mpEditorWorld->SetGlobalAmbientLightEnabled(pVPData->GetAttributeBool("GAmbientLight", true));
	mpEditorWorld->SetGlobalPointLightEnabled(pVPData->GetAttributeBool("GPointLight", true));
}

//-----------------------------------------------------------------------

void iEditorBase::SaveEditorSession(iXmlDocument* apDoc, cXmlElement** apElement)
{
	///////////////////////////////////////////////////////////////////
	// (TODO: this needs clean up, or be moved elsewhere)
	*apElement = apDoc->CreateChildElement("EditorSession");

	// Save performance options
	cXmlElement* pPerfOptions = (*apElement)->CreateChildElement("Performance");
	pPerfOptions->SetAttributeBool("LightsActive", mpEditorWorld->GetTypeActive(eEditorEntityType_Light));
	pPerfOptions->SetAttributeBool("PSActive", mpEditorWorld->GetTypeActive(eEditorEntityType_ParticleSystem));
	pPerfOptions->SetAttributeBool("WorldReflection", GetWorldReflectionActive());
	pPerfOptions->SetAttributeVector2f("CamClipPlanes", cEditorWindowViewport::GetCamPlanes());
	pPerfOptions->SetAttributeBool("ShowFog", mpEditorWorld->GetShowFog());
	pPerfOptions->SetAttributeBool("ShowSkybox", mpEditorWorld->GetShowSkybox());

	// Save viewport data
	cXmlElement* pVPData = (*apElement)->CreateChildElement("ViewportConfig");
	pVPData->SetAttributeBool("GridSnap", cEditorGrid::GetSnapToGrid());
	pVPData->SetAttributeFloat("GridSnapSeparation", cEditorGrid::GetSnapSeparation());
	pVPData->SetAttributeBool("UsingEnlargedViewport", mpFocusedViewport->IsEnlarged());
	pVPData->SetAttributeBool("GAmbientLight", mpEditorWorld->GetGlobalAmbientLightEnabled());
	pVPData->SetAttributeBool("GPointLight", mpEditorWorld->GetGlobalPointLightEnabled());
	pVPData->SetAttributeColor("BGColor", mpEditorWorld->GetBGDefaultColor());
	for(int i=0;i<(int)mvViewports.size();++i)
	{
		cXmlElement* pVP = pVPData->CreateChildElement("Viewport");
		cEditorWindowViewport* pViewport = mvViewports[i];
		pViewport->Save(pVP);

		if(mpFocusedViewport==mvViewports[i])
			pVPData->SetAttributeInt("SelectedViewport", i);
	}
}

//-----------------------------------------------------------------------

void iEditorBase::OnWorldModify()
{
	///////////////////////////////////////////////
	// Update Windows
	tEditorWindowListIt it=mlstWindows.begin();
	for(;it!=mlstWindows.end();++it)
	{
		iEditorWindow* pWin = *it;

		pWin->OnWorldModify();
	}
}

void iEditorBase::OnSelectionChange()
{
	///////////////////////////////////////////////
	// Update Windows
	tEditorWindowListIt it=mlstWindows.begin();
	for(;it!=mlstWindows.end();++it)
	{
		iEditorWindow* pWin = *it;

		pWin->OnSelectionChange();
	}
}

//-----------------------------------------------------------------------

void iEditorBase::Exit()
{
	mpEngine->Exit();
}

//-----------------------------------------------------------------------

cVector3f& iEditorBase::GetPosOnGridFromMousePos(bool abSnapped)
{
	if(abSnapped)
		return mvSnappedPosOnGrid;
	else
		return mvRealPosOnGrid;
}

//-----------------------------------------------------------------------

void iEditorBase::SetCurrentEditMode(iEditorEditMode* apEditMode)
{
	if(mpCurrentEditMode==apEditMode)
	{
		return;
	}
	
	if(mpCurrentEditMode)
		mpCurrentEditMode->SetCurrent(false);
	
	mpCurrentEditMode = apEditMode;

	if(mpCurrentEditMode)
		mpCurrentEditMode->SetCurrent(true);
}

//-----------------------------------------------------------------------

void iEditorBase::AddAction(iEditorAction* apAction)
{
	if(apAction)
	{
		if(apAction->Create()) 
			mpActionHandler->Do(apAction);
		else
			hplDelete(apAction);
	}
}


//-----------------------------------------------------------------------

void iEditorBase::AddEditMode(iEditorEditMode* apEditMode)
{
	if(apEditMode==NULL)
		return;

	tEditorEditModeVecIt it = find(mvEditModes.begin(),mvEditModes.end(),apEditMode);
	if(it==mvEditModes.end())
	{
		apEditMode->OnAdd();
		mvEditModes.push_back(apEditMode);
	}
}

//-----------------------------------------------------------------------

iEditorEditMode* iEditorBase::GetEditMode(const tString& asName)
{
	for(int i=0;i<(int)mvEditModes.size();++i)
	{
		if(mvEditModes[i]->GetName()==asName)
			return mvEditModes[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------

void iEditorBase::SetFlags(int alFlags, bool abX)
{
	if(abX)	mlFlags |= alFlags;
	else	mlFlags &= (~alFlags);

	if(OnChangeFlags(alFlags))
		SetLayoutNeedsUpdate(true);
}

//-----------------------------------------------------------------------

bool iEditorBase::GetFlags(int alFlags)
{
	return ((mlFlags & alFlags)!=0);
}

//-----------------------------------------------------------------------

void iEditorBase::ClearFlags()
{
	mlFlags = 0;

	SetFlags(0xFFFFFFFF, false);
}

//-----------------------------------------------------------------------

bool iEditorBase::OnChangeFlags(int alFlags)
{
	if(alFlags==eEditorFlag_MouseMoved)
		return false;

	bool bUpdateLayout = false;

	bool bEnlarged = GetFlags(eEditorFlag_ViewportEnlarged);
	if(alFlags&eEditorFlag_ViewportEnlarged)
	{
		SetViewportEnlarged(bEnlarged);
		bUpdateLayout = true;
	}
	if(alFlags&eEditorFlag_ViewportsDisabled)
	{
		for(int i=0;i<(int)mvViewports.size();++i)
		{
			bool bActive = GetFlags(eEditorFlag_ViewportsDisabled)==false && 
				(bEnlarged==false || (bEnlarged==true && mvViewports[i]->IsEnlarged()));
			mvViewports[i]->SetActive(bActive);
		}
		bUpdateLayout = true;
	}
	if(alFlags&eEditorFlag_Editing)
	{
		SetFlags(eEditorFlag_ViewLocked, GetFlags(eEditorFlag_Editing));
	}

	return bUpdateLayout;
}


//-----------------------------------------------------------------------

cEngine* iEditorBase::Init(cEngine* apEngine, const char* asName, const char* asBuildDate, bool abDestroyEngineOnExit)
{
	mbDestroyEngineOnExit = abDestroyEngineOnExit;

	LoadConfig();

	bool bEngineWasCreated = apEngine!=NULL; //If engine was supplied as argument or not.
    	
	////////////////////////////////////////////////////
	// Engine is already created
	//  No need to create it, just set the member var
	if(apEngine)
	{
		mpEngine = apEngine;
	}
	////////////////////////////////////////////////////
	// Engine is NULL, create it
	//  Get settings from loaded config file
	else
	{
		cEngineInitVars vars;
		vars.mGraphics.mvScreenSize.x = cString::ToInt(GetSetting("ScreenWidth").c_str(),1024);
		vars.mGraphics.mvScreenSize.y = cString::ToInt(GetSetting("ScreenHeight").c_str(), 768);
		vars.mGraphics.mbFullscreen = cString::ToBool(GetSetting("FullScreen").c_str(), false);
		vars.mGraphics.msWindowCaption = msCaption;

		iRenderer::SetShadowMapQuality(eShadowMapQuality_Medium);		

		mpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_All, &vars);
		mpEngine->GetInput()->GetLowLevel()->LockInput(false);

		mpEngine->GetResources()->GetMaterialManager()->SetTextureSizeDownScaleLevel(cString::ToInt(GetSetting("TexQuality").c_str(), 0));

		mpEngine->GetResources()->LoadResourceDirsFile("resources.cfg");
	}

	Log("-----------------------------------------------------\n%s Build ID: %s\n-----------------------------------------------------\n", 
		asName, asBuildDate);
	
	/////////////////////////
	//Set up engine
	
	mpEngine->SetLimitFPS(false);
	mpEngine->SetWaitIfAppOutOfFocus(true);

	mpViewport = mpEngine->GetScene()->CreateViewport(NULL,NULL);
	
	//Add all used resource dirs
	SetUpDirectories();

	mpEngine->GetUpdater()->AddUpdate("Default", this);

	/////////////////////////
	//Create and set up GUI
	mpSkin = mpEngine->GetGui()->CreateSkin("gui_default.skin");
	mpSet = mpEngine->GetGui()->CreateSet("MainSet",mpSkin);

	mpSet->SetDrawMouse(true);
	mpSet->SetMouseZ(1000);

	//If the engine was already created, we are inside another app and to not want to set focus
	if(bEngineWasCreated==false)
		mpEngine->GetGui()->SetFocus(mpSet);

	////////////////////////////////////
	// FIXME: this is the weirdest thing ever... trying to build this in my comp in the ModelEditor
	// the compiler will somehow ignore the hplNew and take it for a plain new, so no addition to the pointers map.
	// LevelEditor builds okay O_O
	//mpEntityLoader = (cEditorEntityLoader*)cMemoryManager::AddPointer(cAllocatedPointer(new cEditorEntityLoader(this), __FILE__, __LINE__, sizeof(cEditorEntityLoader)));
	mpEntityLoader = hplNew(cEditorEntityLoader,(this));
	mpEngine->GetResources()->AddEntityLoader(mpEntityLoader, true);
	mpViewport->AddGuiSet(mpSet);
    
	/////////////////////////
	//Create/Set up helper classes
	SetUpTempWorld();
	mpThumbnailBuilder = hplNew(cEditorThumbnailBuilder, (this));
	cEditorHelper::mpEditor = this;

	cMeshCreator *pMeshCreator = GetEngine()->GetGraphics()->GetMeshCreator();
	cMeshManager *pMeshManager = GetEngine()->GetResources()->GetMeshManager();
	tVertexElementFlag lFlags = eVertexElementFlag_Position | eVertexElementFlag_Normal | eVertexElementFlag_Texture0 | eVertexElementFlag_Color0;
	
	////////////////////////////////////
	// Compile Custom Var definitions
	mpClassDefManager = hplNew(cEditorUserClassDefinitionManager,(this));
	SetUpClassDefinitions(mpClassDefManager);
	mpClassDefManager->Init();

	////////////////////////////////////
	// Create world
	mpEditorWorld = CreateSpecificWorld();
	
	OnInit();
	for(int i=0;i<(int)mvEditModes.size();++i)
		mvEditModes[i]->PostEditModesCreation();

	InitInput();
	InitLayout();

	///////////////////////////////////////////////
	// Final init stuff
	Reset();
	OnNew();

	return mpEngine;
}


//-----------------------------------------------------------------

void iEditorBase::AddWindow(iEditorWindow* apWindow)
{
	if(apWindow)
	{
		//apWindow->Init();
		mlstWindows.push_back(apWindow);
	}
}

//-----------------------------------------------------------------------

void iEditorBase::RemoveWindow(iEditorWindow* apWindow)
{
	if(apWindow && mbDestroyingEditor==false)
		mlstWindows.remove(apWindow);
}

//-----------------------------------------------------------------------

void iEditorBase::DestroyPopUpWindow(iEditorWindow* apWindow)
{
	if(apWindow)
		mlstWindowsToDestroy.push_back(apWindow);
}

//-----------------------------------------------------------------------

void iEditorBase::CreateViewports()
{
	/////////////////////////////////////////////
	// Create 4 viewports

	// Set up viewport positions (on screen)
	const cVector2f& vLargeVPSize = GetLayoutVec2f(eLayoutVec2_ViewportAreaSize);
	cVector2f vSmallVPSize = vLargeVPSize*0.5f;

	const cVector3f& vViewportAreaPosition = GetLayoutVec3f(eLayoutVec3_ViewportAreaPos);
	tVector3fVec vViewportPos;
	vViewportPos.push_back(vViewportAreaPosition);
	vViewportPos.push_back(vViewportAreaPosition + cVector2f(vSmallVPSize.x,0));
	vViewportPos.push_back(vViewportAreaPosition + cVector2f(0,vSmallVPSize.y));
	vViewportPos.push_back(vViewportAreaPosition + vSmallVPSize);

	// Set up viewport positions (on framebuffer)
	cVector2l vFBPos[4];
	vFBPos[0] = cVector2l(0);
	vFBPos[1] = cVector2l((int)vSmallVPSize.x,0);
	vFBPos[2] = cVector2l(0,(int)vSmallVPSize.y);
	vFBPos[3] = cVector2l((int)vSmallVPSize.x, (int)vSmallVPSize.y);

	for(int i=0; i<4; ++i)
	{
		cEditorWindowViewport* pViewport = hplNew(cEditorWindowViewport,(this, mpFrameBuffer));
		pViewport->Init();
		AddWindow(pViewport);
		
		pViewport->SetNormalPosition(vViewportPos[i]);
		pViewport->SetEnlargedPosition(vViewportPos[0]);
		pViewport->SetNormalSize(vSmallVPSize-4);
		pViewport->SetEnlargedSize(vLargeVPSize);
		pViewport->SetEngineViewportNormalPosition(vFBPos[i]);
		pViewport->SetEngineViewportNormalSize(cVector2l((int)vSmallVPSize.x, (int)vSmallVPSize.y));

		pViewport->SetViewportActive(true);
		pViewport->SetEnlarged(false);

		mvViewports.push_back(pViewport);
	}

	SetFocusedViewport(mvViewports[0]);
}

//-----------------------------------------------------------------------

void iEditorBase::SetFocusedViewport(cEditorWindowViewport* apViewport)
{
	if(mpFocusedViewport==apViewport || mbViewportLocked) return;

	if(mpFocusedViewport) mpFocusedViewport->SetFocused(false);

	mpFocusedViewport = apViewport;

	if(mpFocusedViewport)
		mpFocusedViewport->SetFocused(true);

	if(mpLowerToolbar)
		mpLowerToolbar->Update(0);
	//SetLayoutNeedsUpdate(true);

	OnSetFocusedViewport();
}

//-----------------------------------------------------------------------

void iEditorBase::SetViewportLocked(bool abX)
{
	mbViewportLocked = abX;
}

//-----------------------------------------------------------------------

void iEditorBase::SetViewportEnlarged(bool abX)
{
	bool bCanBeActive = (GetFlags(eEditorFlag_ViewportsDisabled)==false);
	for(int i=0;i<(int)mvViewports.size();++i)
	{
		cEditorWindowViewport* pViewport = mvViewports[i];

		bool bIsSelected = (pViewport==mpFocusedViewport);

		pViewport->SetActive( abX ? bCanBeActive&&bIsSelected : bCanBeActive&&true);
		pViewport->SetEnlarged( abX? bIsSelected:false);
		pViewport->UpdateViewport();

		if(bIsSelected)
			pViewport->LogDebug();
	}
}

//-----------------------------------------------------------------------

void iEditorBase::SetWorldReflectionActive(bool abX)
{
	tEditorViewportVec& vViewports = GetViewports();
	for(int i=0;i<(int)vViewports.size();++i)
	{
		cEditorWindowViewport* pViewport = vViewports[i];
		pViewport->GetEngineViewport()->GetRenderSettings()->mbRenderWorldReflection = abX;
	}
}

bool iEditorBase::GetWorldReflectionActive()
{
	return GetFocusedViewport()->GetEngineViewport()->GetRenderSettings()->mbRenderWorldReflection;
}

//-----------------------------------------------------------------------

void iEditorBase::SetViewportCameraPlanes(const cVector2f& avX)
{
	cEditorWindowViewport::SetCamPlanes(avX);
	tEditorViewportVec& vViewports = GetViewports();
	for(int i=0;i<(int)vViewports.size();++i)
	{
		cEditorWindowViewport* pViewport = vViewports[i];
		pViewport->UpdateCameraPlanes();
	}
}

const cVector2f& iEditorBase::GetViewportCameraPlanes()
{
	return cEditorWindowViewport::GetCamPlanes();
}

//-----------------------------------------------------------------------

void iEditorBase::SetViewportBGColor(const cColor& aX)
{
	tEditorViewportVec& vViewports = GetViewports();
	for(int i=0;i<(int)vViewports.size();++i)
	{
		cEditorWindowViewport* pViewport = vViewports[i];
		pViewport->SetClearColor(aX);
	}
}

//-----------------------------------------------------------------------

iEditorAction* iEditorBase::CreateFocusOnSelectionAction()
{
	if(mpSelection->IsEmpty())
		return NULL;

	const cVector3f& vHeights = mpSelection->GetCenterTranslation();
	bool bAtLeastOneHeightChanged = false;

	cEditorActionCompoundAction* pAction = hplNew(cEditorActionCompoundAction,("Focus"));
	for(int i=0;i<(int)mvViewports.size();++i)
	{
		cEditorWindowViewport* pViewport = mvViewports[i];
		cEditorGrid* pGrid = pViewport->GetGrid();

		if(bAtLeastOneHeightChanged || vHeights!=pGrid->GetHeights())
		{
			bAtLeastOneHeightChanged = true;
			pAction->AddAction(hplNew(cEditorActionPlaneChangeHeight,(pViewport->GetGrid(), vHeights)));
		}
		pViewport->FocusOnSelection(mpSelection);
	}

	if(pAction->IsEmpty())
		return NULL;

	return pAction;
}

//-----------------------------------------------------------------------

void iEditorBase::ViewportMouseDown(cEditorWindowViewport* apViewport, int alButtons)
{
	if(mpFocusedViewport!=apViewport || mpCurrentEditMode==NULL)
		return;

	mpCurrentEditMode->OnViewportMouseDown(apViewport, alButtons);
}

void iEditorBase::ViewportMouseUp(cEditorWindowViewport* apViewport, int alButtons)
{
	if(mpFocusedViewport!=apViewport || mpCurrentEditMode==NULL)
		return;

	mpCurrentEditMode->OnViewportMouseUp(apViewport, alButtons);
}

//-----------------------------------------------------------------------

void iEditorBase::SetUpDirectories()
{
	///////////////////////////////////////////////////
	// Editor resources
	mpEngine->GetResources()->AddResourceDir(_W("editor/"),true);

	///////////////////////////////////////////////////
	// Redist (global lookup dirs)
	const tWString& sWorkingDir = GetWorkingDir();

	mpDirHandler->AddLookUpDir(eDir_Lights, sWorkingDir + mpMainConfig->GetStringW("Directories", "LightsDir", _W("lights")), true);
	mpDirHandler->AddLookUpDir(eDir_Billboards, sWorkingDir + mpMainConfig->GetStringW("Directories", "BillboardsDir", _W("billboards")), true);
	mpDirHandler->AddLookUpDir(eDir_Particles, sWorkingDir + mpMainConfig->GetStringW("Directories", "ParticlesDir", _W("particles")), true);
	mpDirHandler->AddLookUpDir(eDir_Sounds, sWorkingDir + mpMainConfig->GetStringW("Directories", "SoundsDir", _W("sounds")), true);

	// Let derived app add its own
	OnSetUpDirectories();

	///////////////////////////////////////////////////
	// Update lookup resources
	mpDirHandler->RefreshLookupDirs();
}

//-----------------------------------------------------------------------

const tWString& iEditorBase::GetWorkingDir()
{
	return mpDirHandler->GetWorkingDir();
}

//-----------------------------------------------------------------------

const tWString& iEditorBase::GetHomeDir()
{
	return mpDirHandler->GetHomeDir();
}

//-----------------------------------------------------------------------

const tWString& iEditorBase::GetTempDir()
{
	return mpDirHandler->GetTempDir();
}

//-----------------------------------------------------------------------

const tWString& iEditorBase::GetThumbnailDir()
{
	return mpDirHandler->GetThumbnailDir();
}


//-----------------------------------------------------------------------

const tWString& iEditorBase::GetMainLookUpDir(int alCategory)
{
    return mpDirHandler->GetMainLookUpDir(alCategory);	
}

//-----------------------------------------------------------------------

tWStringVec iEditorBase::GetLookUpDirs(int alCategory)
{
	return mpDirHandler->GetLookUpDirs(alCategory);
}

//-----------------------------------------------------------------------

tWString iEditorBase::GetPathRelToWD(const tWString& asPath)
{
	return mpDirHandler->GetPathRelToWD(asPath);
}

//-----------------------------------------------------------------------

void iEditorBase::SetLayoutVec3f(int alIdx, const cVector3f& avX)
{
	mmapLayoutVecs3f[alIdx] = avX;
}

void iEditorBase::SetLayoutVec3l(int alIdx, const cVector3l& avX)
{
	mmapLayoutVecs3f[alIdx] = cVector3f((float)avX.x, (float)avX.y, (float)avX.z);
}

void iEditorBase::SetLayoutVec2f(int alIdx, const cVector2f& avX)
{
	mmapLayoutVecs2f[alIdx] = avX;
}

void iEditorBase::SetLayoutVec2l(int alIdx, const cVector2l& avX)
{
	mmapLayoutVecs2f[alIdx] = cVector2f((float)avX.x, (float)avX.y);
}

const cVector3f& iEditorBase::GetLayoutVec3f(int alIdx)
{
	std::map<int,cVector3f>::iterator it = mmapLayoutVecs3f.find(alIdx);
	if(it!=mmapLayoutVecs3f.end())
		return it->second;
	else
		return gvZeroVec3f;
}

cVector3l iEditorBase::GetLayoutVec3l(int alIdx)
{
	const cVector3f& vTemp = GetLayoutVec3f(alIdx);
	return cVector3l((int)vTemp.x, (int)vTemp.y, (int)vTemp.y);
}

const cVector2f& iEditorBase::GetLayoutVec2f(int alIdx)
{
	std::map<int,cVector2f>::iterator it = mmapLayoutVecs2f.find(alIdx);
	if(it!=mmapLayoutVecs2f.end())
		return it->second;
	else
		return gvZeroVec2f;
}

cVector2l iEditorBase::GetLayoutVec2l(int alIdx)
{
	const cVector2f& vTemp = GetLayoutVec2f(alIdx);
	return cVector2l((int)vTemp.x, (int)vTemp.y);
}

//-----------------------------------------------------------------------

void iEditorBase::UpdateLayout()
{
	if(mbLayoutNeedsUpdate==false)
		return;
	mbLayoutNeedsUpdate = false;

	///////////////////////////////////////////////
	// Update Windows
	tEditorWindowListIt it=mlstWindows.begin();
	for(;it!=mlstWindows.end();++it)
	{
		iEditorWindow* pWin = *it;

		pWin->Update(0);
	}

	///////////////////////////////////////////////
	// Update Main Menu
	UpdateMenu();

	OnPostUpdateLayout();
}

//-----------------------------------------------------------------------

void iEditorBase::Update(float afTimeStep)
{

	/////////////////////////////////////////////////////////////////////////////////////////
	// Update Input
	cInput* pInput = mpEngine->GetInput();
	cGui* pGui = mpEngine->GetGui();

	iKeyboard* pKB = pInput->GetKeyboard();
	int lKeyModifier = pKB->GetModifier();

	
	///////////////////////////////
	// Update Mouse
	cVector2l vMouseAbsPos = pInput->GetMouse()->GetAbsPosition();
	cVector2l vMouseRelPos = pInput->GetMouse()->GetRelPosition();

	SetFlags(eEditorFlag_MouseMoved,  (vMouseRelPos!=0));

	pGui->SendMousePos(vMouseAbsPos, vMouseRelPos);

	// Left button
	if(pInput->BecameTriggerd("LeftClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Left, lKeyModifier);
	if(pInput->WasTriggerd("LeftClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Left, lKeyModifier);
	if(pInput->DoubleTriggerd("LeftClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Left, lKeyModifier);

	// Middle button
	if(pInput->BecameTriggerd("MiddleClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Middle, lKeyModifier);
	if(pInput->WasTriggerd("MiddleClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Middle, lKeyModifier);
	if(pInput->DoubleTriggerd("MiddleClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Middle, lKeyModifier);

	// Right button
	if(pInput->BecameTriggerd("RightClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Right, lKeyModifier);
	if(pInput->WasTriggerd("RightClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Right, lKeyModifier);
	if(pInput->DoubleTriggerd("RightClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Right, lKeyModifier);
	
	// Wheel
	if(pInput->BecameTriggerd("WheelUp"))
		pGui->SendMouseClickDown(eGuiMouseButton_WheelUp);
	if(pInput->WasTriggerd("WheelUp"))
		pGui->SendMouseClickUp(eGuiMouseButton_WheelUp);
	if(pInput->BecameTriggerd("WheelDown"))
		pGui->SendMouseClickDown(eGuiMouseButton_WheelDown);
	if(pInput->WasTriggerd("WheelDown"))
		pGui->SendMouseClickUp(eGuiMouseButton_WheelDown);

	///////////////////////////////
	// GUI Key presses && releases
	bool bKeyProcessed = false;
	while(pKB->KeyIsPressed())
		bKeyProcessed = pGui->SendKeyPress(pKB->GetKey());
	while(pKB->KeyIsReleased())
		bKeyProcessed = pGui->SendKeyRelease(pKB->GetReleasedKey());

	////////////////////////////////////////////////////////////////////
	// Update Mouse position in the 3D scene
	if(mpFocusedViewport)
	{
		mvRealPosOnGrid = mpFocusedViewport->GetMouseWorldPosition();
		mvSnappedPosOnGrid = GetGrid()->GetSnappedPosOnGrid(mvRealPosOnGrid);
	}

	////////////////////////////////////////////////////////////////////
	// Keep track of the time
	mfTimer += afTimeStep;

	////////////////////////////////////////////////////////////////////
	// Update current edit mode
	if(mpCurrentEditMode) 
		mpCurrentEditMode->OnEditorUpdate(afTimeStep);

	////////////////////////////////////////////////////////////////////
	// Run world update routines, if any
	if(mpEditorWorld)
		mpEditorWorld->OnEditorUpdate();


	////////////////////////////////////////////////////////////////////
	// Destroy pop ups in list
	if(mlstWindowsToDestroy.empty()==false)
	{
		tEditorWindowListIt it = mlstWindowsToDestroy.begin();
		for(;it!=mlstWindowsToDestroy.end();++it)
		{
			RemoveWindow(*it);
			cEditorWindowFactory::DestroyEditorWindow(*it);
		}
		mlstWindowsToDestroy.clear();
	}
    
	if(mbWorldModified)
	{
		mbWorldModified=false;
		OnWorldModify();
	}
	////////////////////////////////////////////////////////////////////
	// Update layout if needed
	UpdateLayout();

	if(mbSelectionChanged)
	{
		mbSelectionChanged=false;
		OnSelectionChange();
	}

	////////////////////////////////////////////////////////////////////
	// Call app specific update routines
	OnUpdate(afTimeStep);
}

//----------------------------------------------------------------------------

void iEditorBase::ShowLoadingWindow(const tWString& asCaption, const tWString& asStatus)
{
	if(mpLoaderStatusWindow)
	{
		mpLoaderStatusWindow->SetCaption(asCaption);
		mpLoaderStatusWindow->SetStatusString(asStatus);

		mpLoaderStatusWindow->Draw();
	}
}

//----------------------------------------------------------------------------

cEditorWindowTextureBrowser* iEditorBase::ShowTextureBrowser(eEditorTextureResourceType aType, 
															 const tWString& asStartDir,
															 tWString& asDestFilename,
															 void* apCallbackObject, tGuiCallbackFunc apCallback,
															 const tWStringList& alstFilters)
{
	cEditorWindowTextureBrowser* pWindow = hplNew(cEditorWindowTextureBrowser,(this, aType, asStartDir, asDestFilename, apCallbackObject, apCallback,alstFilters));
	pWindow->Init();
	pWindow->SetActive(true);

	AddWindow(pWindow);

	return pWindow;
}

//----------------------------------------------------------------------------

cEditorWindowMaterialEditor* iEditorBase::ShowMaterialEditor(cEditorInputFile* apInput, const tWString& asFile)
{
	tWString sMatFile = asFile;
	if(apInput)
		sMatFile=apInput->GetValue();

	mpMaterialEditor = hplNew( cEditorWindowMaterialEditor,(this, NULL, sMatFile, apInput));
	mpMaterialEditor->Init();
	mpMaterialEditor->SetActive(true);

	AddWindow(mpMaterialEditor);

	return mpMaterialEditor;
}

//----------------------------------------------------------------------------

cEditorWindowSoundBrowser* iEditorBase::ShowSoundBrowser(const tWString& asStartDir, tWString& asDestFilename, void* apCallbackObject, tGuiCallbackFunc apCallback)
{
	cEditorWindowSoundBrowser* pBrowser = hplNew(cEditorWindowSoundBrowser,(this, asStartDir, asDestFilename, apCallbackObject, apCallback));

	pBrowser->Init();
	pBrowser->SetActive(true);
	AddWindow(pBrowser);

	return pBrowser;
}

//----------------------------------------------------------------------------

iEditorWindowLowerToolbar* iEditorBase::CreateLowerToolbar()
{
	iEditorWindowLowerToolbar* pToolbar = CreateSpecificLowerToolbar();
	if(pToolbar)
	{
		((iEditorWindow*)pToolbar)->Init();
		AddWindow(pToolbar);
	}

	return pToolbar;
}

iEditorWindowLowerToolbar* iEditorBase::CreateSpecificLowerToolbar()
{
	return hplNew(iEditorWindowLowerToolbar,(this));
}

//----------------------------------------------------------------------------

iEditorWindowEditModeSidebar* iEditorBase::CreateEditModeSidebar()
{
	iEditorWindowEditModeSidebar* pSidebar = CreateSpecificEditModeSidebar();
	if(pSidebar)
	{
		((iEditorWindow*)pSidebar)->Init();
		AddWindow(pSidebar);
	}

	return pSidebar;
}

iEditorWindowEditModeSidebar* iEditorBase::CreateSpecificEditModeSidebar()
{
	return hplNew(iEditorWindowEditModeSidebar,(this));
}

//----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void iEditorBase::InitInput()
{
	//////////////////
	// Enable input
	cInput* pInput = mpEngine->GetInput();

	pInput->CreateAction("LeftClick")->AddMouseButton(eMouseButton_Left);
	pInput->CreateAction("MiddleClick")->AddMouseButton(eMouseButton_Middle);
	pInput->CreateAction("RightClick")->AddMouseButton(eMouseButton_Right);
	pInput->CreateAction("WheelUp")->AddMouseButton(eMouseButton_WheelUp);
	pInput->CreateAction("WheelDown")->AddMouseButton(eMouseButton_WheelDown);

	pInput->CreateAction("LeftAlt")->AddKey(eKey_LeftAlt);
	pInput->CreateAction("LeftShift")->AddKey(eKey_LeftShift);
	pInput->CreateAction("LeftControl")->AddKey(eKey_LeftCtrl);

	pInput->CreateAction("Duplicate")->AddKey(eKey_D);

	OnInitInput();
}

//----------------------------------------------------------------------------

void iEditorBase::InitLayout()
{
	mvScreenSize = mpSet->GetVirtualSize();

	/////////////////////////////////
	// Setup background
	mpBGFrame = mpSet->CreateWidgetFrame(cVector3f(0,0,-1),mvScreenSize,false);
	mpBGFrame->SetBackGroundColor(cColor(0.5f,0.5f,0.5f,1));
	mpBGFrame->SetDrawBackground(true);
	mpBGFrame->AddCallback(eGuiMessage_KeyPress, this, kGuiCallback(EscapeKeyHandlerCallback));

	mpMainMenu = CreateMainMenu();
	mpEditModeSidebar = CreateEditModeSidebar();

	SetUpWindowAreas();

	mpLowerToolbar = CreateLowerToolbar();

	const cVector3f& vViewportAreaPos = GetLayoutVec3f(eLayoutVec3_ViewportAreaPos);
	const cVector2f& vViewportAreaSize = GetLayoutVec2f(eLayoutVec2_ViewportAreaSize);

	InitRenderTarget(vViewportAreaSize);	
	CreateViewports();
	if(mvViewports.empty()==false)
		SetFocusedViewport(mvViewports[0]);

	////////////////////////////////////////////////////////
	// Set up EditMode Windows
	//SetLayoutVec3f(eLayoutVec3_EditModeWindowPos, vViewportAreaPos+cVector3f(vViewportAreaSize.x,0,0));

	cVector3f vEditWindowPos = vViewportAreaPos+cVector3f(vViewportAreaSize.x,-2,0);
	for(int i=0;i<(int)mvEditModes.size();++i)
	{
		iEditorWindow* pWin = mvEditModes[i]->CreateWindow();
		if(pWin)
			pWin->SetPosition(vEditWindowPos);
	}

	mpLoaderStatusWindow = cEditorWindowFactory::CreateLoaderStatusWindow(this);
	mpLoaderStatusWindow->SetEnabled(false);
	mpLoaderStatusWindow->SetVisible(false);

	OnInitLayout();
}

//----------------------------------------------------------------------------

void iEditorBase::InitRenderTarget(const cVector2f& avSize)
{
	//////////////////////////////////////////////
	// Creates a render target and framebuffer with size avSize
	cGraphics* pGfx = mpEngine->GetGraphics();

	int lTexW, lTexH;
	lTexW = (int) avSize.x;
	lTexH = (int) avSize.y;
	
	iTexture* pRenderTexture = pGfx->CreateTexture("RenderTexture",eTextureType_Rect, eTextureUsage_RenderTarget);
	pRenderTexture->SetWrapR(eTextureWrap_ClampToEdge);
	pRenderTexture->SetWrapS(eTextureWrap_ClampToEdge);
	pRenderTexture->CreateFromRawData(cVector3l(lTexW, lTexH, 0), ePixelFormat_RGBA, 0);

	mpFrameBuffer = pGfx->CreateFrameBuffer("MainRenderTarget");
	mpFrameBuffer->SetTexture2D(0, pRenderTexture);

	iDepthStencilBuffer *pDepthBuffer = pGfx->CreateDepthStencilBuffer(mpFrameBuffer->GetSize(),24,8,false);

	mpFrameBuffer->SetDepthStencilBuffer(pDepthBuffer);
	mpFrameBuffer->CompileAndValidate();
}

//----------------------------------------------------------------------------

void iEditorBase::SetUpWindowAreas()
{
	cVector2f vMenuSize = mpMainMenu->GetSize();
	SetLayoutVec2f(eLayoutVec2_ViewportAreaSize, cVector2f(0.77f * mvScreenSize.x, mvScreenSize.y-50 - vMenuSize.y-4));
	SetLayoutVec3f(eLayoutVec3_ViewportAreaPos, cVector3f(33, vMenuSize.y+2, 1));
}

//----------------------------------------------------------------------------

void iEditorBase::SetUpViewports()
{
	/////////////////////////////////////
	// TODO: create 4 viewports here
	// Create 4 viewports
	for(int i=0; i<4; ++i)
	{
		cEditorWindowViewport* pViewport = hplNew(cEditorWindowViewport,(this, mpFrameBuffer, true));
		pViewport->Init();
		AddWindow(pViewport);

		mvViewports.push_back(pViewport);
	}
}

//----------------------------------------------------------------------------

void iEditorBase::LoadConfig()
{
	mpMainConfig = hplNew(cConfigFile, ( _W("MainEditorSettings.cfg") ));
	mpMainConfig->Load();

	mpDirHandler->OnLoadGlobalConfig(mpMainConfig);

	OnLoadConfig();
}

//----------------------------------------------------------------------------

void iEditorBase::SaveConfig()
{
	//OnSaveConfig();

	/*
	mpMainConfig->SetString("Directories","EditorHomeDir",cString::To8Char(mvFolders[eDir_Home]));
	mpMainConfig->SetString("Directories","EditorTempDir",cString::To8Char(mvFolders[eDir_Temp]));
	mpMainConfig->SetString("Directories","ThumbnailsDir",cString::To8Char(mvFolders[eDir_Thumbnails]));
	
	mpMainConfig->SetString("Directories","MapsDir", cString::To8Char(mvFolders[eDir_Maps]));
	mpMainConfig->SetString("Directories","StaticObjectsDir",cString::To8Char(mvFolders[eDir_StaticObjects]));
	mpMainConfig->SetString("Directories","EntitiesDir",cString::To8Char(mvFolders[eDir_Entities]));
	mpMainConfig->SetString("Directories","LightsDir", cString::To8Char(mvFolders[eDir_Lights]));
	mpMainConfig->SetString("Directories","BillboardsDir", cString::To8Char(mvFolders[eDir_Billboards]));
	mpMainConfig->SetString("Directories","ParticlesDir", cString::To8Char(mvFolders[eDir_Particles]));
	mpMainConfig->SetString("Directories","SoundsDir", cString::To8Char(mvFolders[eDir_Sounds]));
	*/

	mpMainConfig->Save();

	hplDelete(mpMainConfig);
}

//----------------------------------------------------------------------------

void iEditorBase::AppGotInputFocus()
{
	tEditorWindowListIt it = mlstWindows.begin();
	for(;it!=mlstWindows.end();++it)
	{
		iEditorWindow* pWin = *it;
		if(pWin==mpMaterialEditor && mpMaterialEditor->GetMaterial()->ReloadTextures())
		{
			mpMaterialEditor->Update(0);
			break;
		}
	}
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool iEditorBase::EscapeKeyHandlerCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mKeyPress.mKey==eKey_Escape && aData.mKeyPress.mlModifier==0)
		Command_Exit();

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, EscapeKeyHandlerCallback);

//----------------------------------------------------------------------------

void iEditorBase::AppGotMouseFocus()
{
	mpSet->SetDrawMouse(true);
}

//----------------------------------------------------------------------------

void iEditorBase::AppLostMouseFocus()
{
	mpSet->SetDrawMouse(false);
}

//----------------------------------------------------------------------------

bool iEditorBase::MessageBox_New_Callback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==0)
	{
		Reset();
		OnNew();
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, MessageBox_New_Callback);

//----------------------------------------------------------------------------

bool iEditorBase::MessageBox_Load_Callback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==0)
		ShowLoadFilePicker(mvLoadFilenames, msLastLoadPath, this, kGuiCallback(LoadFileCallback));

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, MessageBox_Load_Callback);

//----------------------------------------------------------------------------

bool iEditorBase::MessageBox_RecentFile_Callback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==0)
		Load();
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, MessageBox_RecentFile_Callback);

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool iEditorBase::MessageBox_Exit_UnsavedChanges_Callback(iWidget* apWidget, const cGuiMessageData& aData)
{
	//////////////////////////
	// If "Yes" was pressed
	if(aData.mlVal==0)
	{
		if(msSaveFilename==_W(""))
		{
			ShowSaveFilePicker(this, kGuiCallback(SaveFileAndQuitCallback));
		}
		else
		{
			Save();
			QuitCallback(NULL, cGuiMessageData(0));
		}
	}
	else
	{
		ShowMessageBox(_W("Exiting"),_W("Sure you want to exit?"),
					   _W("Yes"), _W("No"),
					   this, kGuiCallback(QuitCallback));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, MessageBox_Exit_UnsavedChanges_Callback);


//----------------------------------------------------------------------------

bool iEditorBase::RecentFileCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	mvLoadFilenames.clear();
	mvLoadFilenames.push_back(apWidget->GetText());

	if(mpEditorWorld->IsModified())
		ShowMessageBox(_W("Warning"),_W("All unsaved data will be lost, continue?"),
					    _W("Yes"), _W("No"),
						this, kGuiCallback(MessageBox_RecentFile_Callback));
	else
		Load();

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, RecentFileCallback);

//----------------------------------------------------------------------------

bool iEditorBase::LoadFileCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
		Load();

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, LoadFileCallback);

//----------------------------------------------------------------------------

bool iEditorBase::SaveFileCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1 && msSaveFilename!=_W(""))
		Save();

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, SaveFileCallback);

//----------------------------------------------------------------------------

bool iEditorBase::SaveFileAndQuitCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	//////////////////////////////////////
	// If picker "Save" was pressed
	if(aData.mlVal==1)
	{
		Save();
		QuitCallback(NULL, cGuiMessageData(0));
	}
	else
	{
		ShowMessageBox(_W("Exiting"),_W("Sure you want to exit?"),
					   _W("Yes"), _W("No"),
					   this, kGuiCallback(QuitCallback));
	}
	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, SaveFileAndQuitCallback);

//----------------------------------------------------------------------------

bool iEditorBase::QuitCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==0)
	{
		Reset();
		Exit();
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, QuitCallback);

//----------------------------------------------------------------------------

void iEditorBase::ShowMessageBox(const tWString& asCaption,
								  const tWString& asText,
								  const tWString& asButton1,
								  const tWString& asButton2,
								  void* apCallbackObject,
								  tGuiCallbackFunc apCallback)
{
	SetFlags(eEditorFlag_PopUpActive, true);

	cGuiPopUpMessageBox* pBox = mpSet->CreatePopUpMessageBox( asCaption, asText, asButton1, asButton2, apCallbackObject, apCallback );
	pBox->AddOnDestroyCallback(this, kGuiCallback(PopUpCloseCallback));
}

//----------------------------------------------------------------------------

void iEditorBase::ShowLoadFilePicker(tWStringVec& avLoadFilenames, const tWString& asStartPath, 
									 void* apCallbackObject, tGuiCallbackFunc apCallback, 
									 const tWString& asCategoryName, const tWStringList& alstCategoryStrings)
{
	SetFlags(eEditorFlag_PopUpActive, true);

	cGuiPopUpFilePicker* pPicker = mpSet->CreatePopUpLoadFilePicker(avLoadFilenames, 
																	false, 
																	asStartPath, 
																	false, apCallbackObject, apCallback);

	if(asCategoryName==_W("") || alstCategoryStrings.empty())
        pPicker->AddCategory(msFileCategoryName, msFileCategoryString);
	else
	{
		int lCat = pPicker->AddCategory(asCategoryName, _W(""));
		tWStringList::const_iterator it = alstCategoryStrings.begin();
		for(;it!=alstCategoryStrings.end();++it)
		{
			pPicker->AddFilter(lCat, *it);
		}

	}

	pPicker->AddOnDestroyCallback(this, kGuiCallback(PopUpCloseCallback));
}

//----------------------------------------------------------------------------

void iEditorBase::ShowSaveFilePicker(void* apCallbackObject, tGuiCallbackFunc apCallback)
{
	SetFlags(eEditorFlag_PopUpActive, true);

	cGuiPopUpFilePicker* pPicker = mpSet->CreatePopUpSaveFilePicker(msSaveFilename, msFileCategoryName, msFileCategoryString, msLastSavePath, false, apCallbackObject, apCallback);
	pPicker->AddOnDestroyCallback(this, kGuiCallback(PopUpCloseCallback));
}

//----------------------------------------------------------------------------

bool iEditorBase::PopUpCloseCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	SetFlags(eEditorFlag_PopUpActive, false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(iEditorBase, PopUpCloseCallback);

//----------------------------------------------------------------------------

void iEditorBase::SetUpTempWorld()
{
	if(mpTempWorld==NULL)
		mpTempWorld = mpEngine->GetScene()->CreateWorld("TempWorld");
}

//----------------------------------------------------------------------------

void iEditorBase::AddTempMeshEntity(cMeshEntity* apMeshEntity)
{
	if(apMeshEntity)
		mlstTempMeshEntities.push_back(apMeshEntity);
}

//----------------------------------------------------------------------------

void iEditorBase::RemoveTempMeshEntity(cMeshEntity* apMeshEntity)
{
	tMeshEntityListIt it = find(mlstTempMeshEntities.begin(), mlstTempMeshEntities.end(), apMeshEntity);
	if(it!=mlstTempMeshEntities.end())
	{
		mlstTempMeshEntities.erase(it);
		mpTempWorld->DestroyMeshEntity(apMeshEntity);
	}
}

//----------------------------------------------------------------------------

void iEditorBase::ClearTempEntities()
{
	tMeshEntityListIt itEntities = mlstTempMeshEntities.begin();
	for(;itEntities!=mlstTempMeshEntities.end();++itEntities)
	{
		mpTempWorld->DestroyMeshEntity(*itEntities);
	}
	mlstTempMeshEntities.clear();
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
