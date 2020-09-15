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

#include "../Common/Editor.h"

#include "../Common/EdConfigHandler.h"
#include "../Common/EdActionHandler.h"
#include "../Common/DirectoryHandler.h"
#include "../Common/EdInputHandler.h"
#include "../Common/EdWindowHandler.h"
#include "../Common/EdEditModeHandler.h"

#include "../Common/EdThumbnailBuilder.h"

#include "../Common/EdWorld.h"

#include "../Common/EdWindowOptions.h"


//------------------------------------------------------------------------------

#define CreateModule(aClassType) (aClassType*)AddModule(hplNew(aClassType, (this) ));

//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// Singleton-ish instance management
iEditor* iEditor::mpInstance = NULL;

iEditor* iEditor::GetInstance(const tWString& asCommandLine)
{
	if(mpInstance==NULL)
		mpInstance = CreateInstance(asCommandLine);

	return mpInstance;
}

//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEditor::iEditor(const tString& asName, const tString& asCaption) : iUpdateable(asName), iEdModule(NULL, _W("Editor"))
{
	msName = asName;
	msCaption = asCaption;

	mpGlobalCfg = NULL;
	mpLocalCfg = NULL;

	mpFocusedViewport = NULL;
	mbViewportLocked = false;

	mpMenu = NULL;

	mbWaitingViewportClickUp = false;

	mbDestroying = false;
}

iEditor::~iEditor()
{
	mbDestroying = true;
	STLDeleteAll(mvModules);
	hplDelete(mpThumbnailBuilder);
}

//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEngine* iEditor::Init(cEngine* apEngine, bool abDestroyEngineOnExit)
{
	// If engine was supplied as argument or not.
	bool bEngineWasCreated = apEngine!=NULL; 

	mbDestroyEngineOnExit = abDestroyEngineOnExit;

	////////////////////////////////////////////////////////
	// Create modules
	mpConfigHandler = CreateModule(cEdConfigHandler);
	mpActionHandler = CreateModule(cEdActionHandler);
	mpDirHandler	= CreateModule(cEdDirectoryHandler);
	mpInputHandler	= CreateModule(cEdInputHandler);
	mpEditModeHandler = CreateModule(cEdEditModeHandler);

	// Create world module
	mpWorld = CreateWorld();
	AddModule(mpWorld);

	mpWindowHandler = CreateModule(cEdWindowHandler);

	////////////////////////////////////////////////////////
	// Load configuration
	LoadGlobalConfig();

	LoadLocalConfig();

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
		vars.mGraphics.mvScreenSize.x = mpConfigHandler->mlScreenWidth;
		vars.mGraphics.mvScreenSize.y = mpConfigHandler->mlScreenHeight;
		//vars.mGraphics.mbFullscreen = cString::ToBool(GetSetting("FullScreen").c_str(), false);
		vars.mGraphics.msWindowCaption = msCaption;

		iRenderer::SetShadowMapQuality(mpConfigHandler->mShadowQuality);

		mpEngine = CreateHPLEngine(eHplAPI_OpenGL, eHplSetup_All, &vars);
		mpEngine->GetUpdater()->AddUpdate("Default", this);
		
		cResources* pRes = mpEngine->GetResources();
		pRes->LoadResourceDirsFile("resources.cfg");

		cMaterialManager* pMatMgr = pRes->GetMaterialManager();
		pMatMgr->SetTextureSizeDownScaleLevel(mpConfigHandler->mlTextureQuality);
		pMatMgr->SetTextureAnisotropy(mpConfigHandler->mfTextureAnisotropy);
		pMatMgr->SetTextureFilter(mpConfigHandler->mTextureFilter);

		// HPL3?
		//pMatMgr->SetForceCacheLoadingAndSkipSaving(true);
	}

	/////////////////////////
	//Set up engine
	mpEngine->SetLimitFPS(false);
	mpEngine->SetWaitIfAppOutOfFocus(true);

	mpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");

	mpViewport = mpEngine->GetScene()->CreateViewport(NULL,NULL);

	//Add all used resource dirs
	SetUpDirectories();

	//Add EditModes
	SetUpEditModes();

	//////////////////////////////////////////
	// Initialize all modules now
	BroadcastModuleMsg(eEdModuleMsg_Init, NULL);

	mpThumbnailBuilder = hplNew(cEdThumbnailBuilder,(this));

	/////////////////////////
	//Create and set up GUI
	cGuiSkin* pSkin = mpEngine->GetGui()->CreateSkin("gui_default.skin");
	if(pSkin==NULL)
		FatalError("Could not load GUI skin");

	mpSet = mpEngine->GetGui()->CreateSet("EditorSet",pSkin);
	mpSet->SetActive(true);
	mpSet->SetDrawMouse(true);
	mpSet->SetMouseZ(1000);

	mpViewport->AddGuiSet(mpSet);

	//If the engine was already created, we are inside another app and do not want to set focus
	if(bEngineWasCreated==false)
		mpEngine->GetGui()->SetFocus(mpSet);

	//////////////////////////////////////////
	// Create editor layout
	CreateLayout();

	//////////////////////////////////////////////////////////
	// Initialization complete! Now reset to initial state
    Reset();

	return mpEngine;
}

//------------------------------------------------------------------------------

void iEditor::Reset()
{
	BroadcastModuleMsg(eEdModuleMsg_Reset, NULL);
}

//------------------------------------------------------------------------------

void iEditor::Destroy()
{
	BroadcastModuleMsg(eEdModuleMsg_SaveLocalCfg, mpLocalCfg);
	BroadcastModuleMsg(eEdModuleMsg_SaveGlobalCfg, mpGlobalCfg);

	mpLocalCfg->Save();
	mpGlobalCfg->Save();

	hplDelete(mpGlobalCfg);
	hplDelete(mpLocalCfg);
}

//------------------------------------------------------------------------------

void iEditor::AddAction(iEdAction* apAction)
{
	mpActionHandler->AddAction(apAction);
}

//------------------------------------------------------------------------------

const tWString& iEditor::GetWorkingDir()
{
	return mpDirHandler->GetWorkingDir();
}

//------------------------------------------------------------------------------

const tWString& iEditor::GetHomeDir()
{
	return mpDirHandler->GetHomeDir();
}

//------------------------------------------------------------------------------

const tWString& iEditor::GetTempDir()
{
	return mpDirHandler->GetTempDir();
}

//------------------------------------------------------------------------------

const tWString& iEditor::GetThumbnailDir()
{
	return mpDirHandler->GetThumbnailDir();
}

//------------------------------------------------------------------------------

const tWString& iEditor::GetMainLookUpDir(int alCategory)
{
	return mpDirHandler->GetMainLookUpDir(alCategory);
}

//------------------------------------------------------------------------------

tWStringVec iEditor::GetLookUpDirs(int alCategory)
{
	return mpDirHandler->GetLookUpDirs(alCategory);
}

//------------------------------------------------------------------------------

tWString iEditor::GetPathRelToWD(const tWString& asPath)
{
	return mpDirHandler->GetPathRelToWD(asPath);
}

//------------------------------------------------------------------------------

bool iEditor::SetActiveEditMode(iEdEditMode* apMode)
{
	return mpEditModeHandler->SetActiveMode(apMode);
}

iEdEditMode* iEditor::GetActiveEditMode()
{
	return mpEditModeHandler->GetActiveMode();
}

//------------------------------------------------------------------------------

const tEditModeList& iEditor::GetEditModes()
{
	return mpEditModeHandler->GetModes();
}

//------------------------------------------------------------------------------

void iEditor::AddWindow(iEdWindow* apWin, int alLayoutBlockID)
{
	mpWindowHandler->AddWindow(apWin, alLayoutBlockID);
}

//------------------------------------------------------------------------------

void iEditor::SetFocusedViewport(iEdViewport* apViewport)
{
	if(mbViewportLocked || mpFocusedViewport==apViewport) return;

	if(mpFocusedViewport) mpFocusedViewport->SetFocused(false);
	mpFocusedViewport = apViewport;
	if(mpFocusedViewport) mpFocusedViewport->SetFocused(true);

	BroadcastModuleMsg(eEdModuleMsg_ViewportFocusChange, apViewport);
}

//------------------------------------------------------------------------------

void iEditor::SetViewportsActive(bool abX)
{
	for(int i=0; i<GetNumViewports(); ++i)
	{
		iEdViewport* pViewport = GetViewport(i);
		pViewport->SetActive(abX);
	}
}

bool iEditor::IsViewportsActive()
{
	return true;
	//return mpFocusedViewport && mpFocusedViewport->IsActive();
}

//------------------------------------------------------------------------------

int iEditor::GetNumViewports()
{
	return (int)mvViewports.size();
}

iEdViewport* iEditor::GetViewport(int alIdx)
{
	if(alIdx>=0 || alIdx<GetNumViewports())
		return mvViewports[alIdx];

	return NULL;
}

//------------------------------------------------------------------------------

void iEditor::LockToViewport(iEdViewport* apViewport)
{
	if(mbViewportLocked && apViewport)
		return;
	if(mbViewportLocked==false && apViewport==NULL)
		return;

	if(mpFocusedViewport) mpFocusedViewport->OnSetLock(false);

	if(apViewport && mpFocusedViewport!=apViewport)
		SetFocusedViewport(apViewport);

	if(apViewport) apViewport->OnSetLock(true);

	mbViewportLocked = apViewport!=NULL;
}

//------------------------------------------------------------------------------

void iEditor::SetViewportEnlarge(bool abX)
{
	for(size_t i=0; i<mvViewports.size(); ++i)
	{
		iEdViewport* pViewport = mvViewports[i];
		if(pViewport==mpFocusedViewport)
		{
			pViewport->SetEnlarged(abX);
		}
		else
		{
			pViewport->SetEnlarged(false);
			pViewport->SetActive(!abX);
		}
	}

	BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
}

bool iEditor::IsViewportEnlarged()
{
	if(mpFocusedViewport)
		return mpFocusedViewport->IsEnlarged();

	return false;
}

//------------------------------------------------------------------------------

void iEditor::ShowPreferencesDialog()
{
	iEdWindow* pWin = mpWindowHandler->GetWindow(_W("Preferences"));
	if(pWin==NULL)
	{
		pWin = CreatePreferencesDialog();
		pWin->SetDestroyOnSetInactive(true);
		mpWindowHandler->AddPopUp(pWin);
	}
}

//------------------------------------------------------------------------------

void iEditor::Update(float afTimeStep)
{
	mfTimeStep = afTimeStep;
	BroadcastModuleMsg(eEdModuleMsg_Update, NULL);
}

//------------------------------------------------------------------------------

void iEditor::BroadcastModuleMsg(eEdModuleMsg aMsg, void* apData)
{
	if(mbDestroying) return;

	for(size_t i=0; i<mvModules.size(); ++i)
	{
		iEdModule* pModule = mvModules[i];

		pModule->DispatchMessage(aMsg, apData);
	}

	DispatchMessage(aMsg, apData);
}

//------------------------------------------------------------------------------

void iEditor::ViewportMouseDown(const cViewportClick& aData)
{
	mbWaitingViewportClickUp = true;
	BroadcastModuleMsg(eEdModuleMsg_ViewportMouseDown, (void*)&aData);
}

void iEditor::ViewportMouseUp(const cViewportClick& aData)
{
	if(mbWaitingViewportClickUp==false)
		return;

	mbWaitingViewportClickUp = false;
	BroadcastModuleMsg(eEdModuleMsg_ViewportMouseUp, (void*)&aData);
}

//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

iEdModule* iEditor::AddModule(iEdModule* apModule)
{
	if(apModule) mvModules.push_back(apModule);

	return apModule;
}

void iEditor::RemoveModule(const tWString& asName)
{
	tEdModuleVec::iterator it = mvModules.begin();
	for(; it<mvModules.end(); ++it)
	{
		iEdModule* pModule = *it;
		if(pModule->GetName()==asName)
		{
			mvModules.erase(it);
			break;
		}
	}
}

//------------------------------------------------------------------------------

bool iEditor::LoadGlobalConfig()
{
	mpGlobalCfg = hplNew(cConfigFile, ( _W("MainEditorSettings.cfg") ));
	mpGlobalCfg->Load();

	BroadcastModuleMsg(eEdModuleMsg_LoadGlobalCfg, mpGlobalCfg);

	return true;
}

//------------------------------------------------------------------------------

bool iEditor::LoadLocalConfig()
{
	tWString sAppHomeFileName = GetHomeDir() + cString::To16Char(GetName());

	SetLogFile(cString::SetFileExtW(sAppHomeFileName,_W("log")));

	tWString sAppCfgFile = cString::SetFileExtW(sAppHomeFileName, _W("cfg"));

	mpLocalCfg = hplNew(cConfigFile, (sAppCfgFile));
	mpLocalCfg->Load();

	BroadcastModuleMsg(eEdModuleMsg_LoadLocalCfg, mpLocalCfg);

	return true;
}

void iEditor::SetUpRenderTarget(const cVector2f& avSize)
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

//------------------------------------------------------------------------------

void iEditor::CreateLayout()
{
	mpWindowHandler->AddLayoutBlock(eLayoutBlock_BG, false, false, cVector3f(0,0,-1), true, true, 1, NULL);

	CreateAppSpecificLayout();
	
	mpMenu = CreateMenu();

	//////////////////////////
	//Set Up render target
	{
		cVector2f vRenderTargetSize = cVector2f(800,600);
		const cLayoutBlock* pLayoutBlock = mpWindowHandler->GetLayoutBlock(eLayoutBlock_ViewportArea);
		if(pLayoutBlock)
			vRenderTargetSize = pLayoutBlock->mvSize;

		SetUpRenderTarget(vRenderTargetSize);
	}

	BroadcastModuleMsg(eEdModuleMsg_CreateLayout, NULL);

	InitViewports();
}

//------------------------------------------------------------------------------

void iEditor::AddViewport(iEdViewport* apViewport, 
						  const cVector3f& avNormalPos, const cVector2f& avNormalSize, 
						  const cVector2f& avFBPos, const cVector2f& avFBSize,
						  int alStartMode)
{
	apViewport->SetStartMode(alStartMode);
	mvViewports.push_back(apViewport);

	mvViewportPositions.push_back(avNormalPos);
	mvViewportSizes.push_back(avNormalSize);
	mvViewportFBPositions.push_back(avFBPos);
	mvViewportFBSizes.push_back(avFBSize);

	mpWindowHandler->AddMainWindow(apViewport,-1);
}

//------------------------------------------------------------------------------

void iEditor::SetEnlargedViewportPosAndSize(const cVector3f& avPos, const cVector2f& avSize)
{
	mvViewportEnlargedPos = avPos;
	mvViewportEnlargedSize = avSize;
}

//------------------------------------------------------------------------------

void iEditor::InitViewports()
{
	const cLayoutBlock* pBlock = mpWindowHandler->GetLayoutBlock(eLayoutBlock_ViewportArea);
	if(pBlock)
	{
		mvViewportEnlargedPos = pBlock->mvPosition + cVector3f(0,0,1);
		mvViewportEnlargedSize = pBlock->mvSize;
	}

	for(size_t i=0; i<mvViewports.size(); ++i)
	{
		iEdViewport* pViewport = mvViewports[i];

		pViewport->SetFrameBuffer(mpFrameBuffer);

		pViewport->SetNormalPosition(mvViewportPositions[i]);
		pViewport->SetNormalSize(mvViewportSizes[i]);

		const cVector2f& vFBPosFloat = mvViewportFBPositions[i];
		const cVector2f& vFBPosSize = mvViewportFBSizes[i];
		pViewport->SetEngineViewportNormalPosition(cVector2l((int)vFBPosFloat.x, (int)vFBPosFloat.y));
		pViewport->SetEngineViewportNormalSize(cVector2l((int)vFBPosSize.x, (int)vFBPosSize.y));

		pViewport->SetEnlargedPosition(mvViewportEnlargedPos);
		pViewport->SetEnlargedSize(mvViewportEnlargedSize);
		
		pViewport->SetViewportActive(true);
		pViewport->SetEnlarged(false);

		if(mpFocusedViewport==NULL)
			SetFocusedViewport(pViewport);
	}
}

//------------------------------------------------------------------------------

void iEditor::ShowDialog(iEdWindow* apWindow, bool abUnique)
{
	if(apWindow==NULL) return;

	iEdWindow* pWin = NULL;
	if(abUnique)
	{
		pWin = mpWindowHandler->GetWindow(apWindow->GetName());
		if(pWin==NULL)
		{
			pWin = apWindow;
			pWin->SetDestroyOnSetInactive(true);
		}
		else
		{
			hplDelete(apWindow);
		}
	}
	else
		pWin = apWindow;

	if(pWin) mpWindowHandler->AddPopUp(pWin);
}

//------------------------------------------------------------------------------

void iEditor::OnReset()
{
	OnWorldParamsChange();
}

void iEditor::OnWorldLoad()
{
	OnWorldParamsChange();
}

void iEditor::OnWorldSave()
{
	OnWorldParamsChange();
}

void iEditor::OnWorldModify()
{
	OnWorldParamsChange();
}

void iEditor::OnWorldParamsChange()
{
	const tWString& sFilename = mpWorld->GetCurrentFilename();
	tWString sCaptionText = sFilename.empty()? _W("Unnamed ") + mpWorld->GetName() +_W("") : cString::GetFileNameW(sFilename);
	if(mpWorld->IsModified()) sCaptionText += _W(" (modified)");

	cPlatform::SetWindowCaption(cString::To8Char(sCaptionText) + " - " + GetCaption());
}

void iEditor::OnWorldObjModify()
{
	OnWorldParamsChange();
}

//------------------------------------------------------------------------------
