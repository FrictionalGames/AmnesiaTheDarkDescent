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

#ifndef HPLEDITOR_EDITOR_BASE_CLASSES_H
#define HPLEDITOR_EDITOR_BASE_CLASSES_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorTypes.h"

//---------------------------------------------------------------

class iEditorBase;
class iEditorWindow;

class cDirectoryHandler;

class cEditorWindowLoaderStatus;
class cEditorWindowViewport;
class cEditorActionHandler;
class cEditorThumbnailBuilder;
class cEditorGrid;
class iEditorWorld;
class cEditorSelection;
class iEditorWindowLowerToolbar;
class iEditorWindowEditModeSidebar;
class cEditorWindowTextureBrowser;
class cEditorWindowMaterialEditor;
class cEditorWindowSoundBrowser;

class cEditorUserClassDefinitionManager;

class cEditorInputFile;

typedef std::vector<cEditorWindowViewport*> tEditorViewportVec;

//-----------------------------------------------------------

////////////////////////////////////////////////////////////
// Editor lookup directory categories
////////////////////////////////////////////////////////////

enum eDir
{
	// Global lookup dirs
	eDir_Lights,
	eDir_Billboards,
	eDir_Particles,
	eDir_Sounds,
	eDir_Decals,

	eDir_LastEnum,
};

//-----------------------------------------------------------

enum eUserClassDefinition
{
	eUserClassDefinition_Entity,
	eUserClassDefinition_Area,

	eUserClassDefinition_LastEnum
};

enum eEditorFlag
{
	eEditorFlag_LayoutUpdated		=0x00000001,
	eEditorFlag_MouseMoved			=0x00000002,
	eEditorFlag_Selecting			=0x00000004,
	eEditorFlag_Placing				=0x00000008,
	eEditorFlag_Editing				=0x00000010,
	eEditorFlag_ViewLocked			=0x00000020,
	eEditorFlag_ViewportEnlarged	=0x00000040,
	eEditorFlag_ViewportsDisabled	=0x00000080,
	eEditorFlag_AttentionLocked		=0x00000100,
	eEditorFlag_ViewModeActive		=0x00000200,

	eEditorFlag_PopUpActive			=0x00000400,

	eEditorFlag_LastEnum			=0x00000800,
};

enum eLayoutVec3
{
	eLayoutVec3_ViewportAreaPos,
	eLayoutVec3_EditModeWindowPos,

	eLayoutVec3_LastEnum
};

enum eLayoutVec2
{
	eLayoutVec2_ScreenSize,
	eLayoutVec2_ViewportAreaSize,

	eLayoutVec2_LastEnum
};

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////
// cEnditorEntityLoader
//	Class derived from EntityLoader_Object used to load Entities in editors 
//	(mainly for physics test in ModelEditor)
//	Can choose what portions of the entity to load at a time (bodies, animations, ...)
class cEditorEntityLoader : public cEntityLoader_Object
{
public:
	cEditorEntityLoader(iEditorBase* apEditor);

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars){}
	
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars){}

	cMeshEntity* LoadEntFile(int alID, const tString& asName, const tString& asFilename, 
							 cWorld* apWorld,
							 bool abLoadAnims=false,
							 bool abLoadParticles=false,
							 bool abLoadBillboards=false,
							 bool abLoadSounds=false,
							 bool abLoadLights=true);

	cMeshEntity* LoadEntityFromElement(int alID, const tString& asName, cXmlElement* apElement, 
									   cWorld* apWorld, const tString& asFilename="", const tWString& asFullPath=_W(""), 
									   bool abLoadAnims=false,
									   bool abLoadParticles=false,
									   bool abLoadBillboards=false,
									   bool abLoadSounds=false,
									   bool abLoadLights=true);

	tLightVec& GetLights() { return mvLights; }
	std::vector<cParticleSystem*>& GetParticleSystems() { return mvParticleSystems; }
	std::vector<cBillboard*>& GetBillboards() { return mvBillboards; }
	std::vector<iPhysicsJoint*>& GetJoints() { return mvJoints; }
	std::vector<cSoundEntity*>& GetSounds() { return mvSoundEntities; }

	iEditorBase* mpEditor;
};

//---------------------------------------------------------------

//////////////////////////////////////////////////////////
// iEditorBase
// Base interface
class iEditorBase : public iUpdateable
{
public:
	iEditorBase(const tWString& asFileCategoryName, const tWString& asFileCategoryString);
	virtual ~iEditorBase();

	void SetWorldModified() { mbWorldModified = true; }
	iEditorWorld* GetEditorWorld() { return mpEditorWorld; }

	void SetSelectionChanged() { mbSelectionChanged = true; }
	cEditorSelection* GetSelection() { return mpSelection; }

	///////////////////////////////////////////////
	// Editor Windows Management
	void AddWindow(iEditorWindow* apWindow);
	void RemoveWindow(iEditorWindow* apWindow);
	bool IsDestroyingEditor() { return mbDestroyingEditor; }

	void DestroyPopUpWindow(iEditorWindow* apWindow);

	virtual void SetViewportsEnabled(bool abX){}

	///////////////////////////////////////////////
	// User variable definitions
	// Called by implemented class 
	virtual void SetUpClassDefinitions(cEditorUserClassDefinitionManager* apManager)=0;
	cEditorUserClassDefinitionManager* GetClassDefinitionManager() { return mpClassDefManager; }

	///////////////////////////////////////////////
	// Commands
	void Command_New();
	void Command_Load();
	void Command_Save();
	void Command_SaveAs();
	void Command_Exit();

	void Reset();
	void Load();
	void Save();
	void Exit();

	virtual void OnNew(){}
	virtual void OnPostWorldLoad(){}
	
	void AddRecentFile(const tWString& asFilename);

	///////////////////////////////////////////////
	// Regular Pop Ups
	virtual void ShowLoadFilePicker(tWStringVec& avLoadFilenames,const tWString& asStartPath, void* apCallbackObject, tGuiCallbackFunc apCallback, const tWString& asCategoryName=_W(""), const tWStringList& alstCategoryStrings=tWStringList());
	virtual void ShowSaveFilePicker(void* apCallbackObject, tGuiCallbackFunc apCallback);
	
	void ShowMessageBox(const tWString& asCaption, 
						const tWString& asText, 
						const tWString& asButton1, 
						const tWString& asButton2,
						void* apCallbackObject,
						tGuiCallbackFunc apCallback);

	///////////////////////////////////////////////
	// Utility windows
	cEditorWindowLoaderStatus* GetLoaderStatusWindow() { return mpLoaderStatusWindow; }
	void ShowLoadingWindow(const tWString& asCaption, const tWString& asStatus);
	cEditorWindowTextureBrowser* ShowTextureBrowser(eEditorTextureResourceType aType,
													const tWString& asStartDir,
													tWString& asDestFilename,
													void* apCallbackObject, tGuiCallbackFunc apCallback,
													const tWStringList& alstFilters=tWStringList());

	cEditorWindowMaterialEditor* ShowMaterialEditor(cEditorInputFile* apInput=NULL, const tWString& asFile=_W(""));

	cEditorWindowSoundBrowser* ShowSoundBrowser(const tWString& asStartDir, tWString& asDestFilename, 
												void *apCallbackObject, tGuiCallbackFunc apCallback);

	virtual iEditorWindowLowerToolbar* CreateLowerToolbar();
	virtual iEditorWindowLowerToolbar* CreateSpecificLowerToolbar();

	virtual iEditorWindowEditModeSidebar* CreateEditModeSidebar();
	virtual iEditorWindowEditModeSidebar* CreateSpecificEditModeSidebar();


	///////////////////////////////////////////////
	// Directory Management
	virtual void SetUpDirectories();
	virtual void OnSetUpDirectories()=0;

	cDirectoryHandler* GetDirHandler() { return mpDirHandler; }
	
	const tWString& GetWorkingDir();
	const tWString& GetHomeDir();
	const tWString& GetTempDir();
	const tWString& GetThumbnailDir();

	const tWString& GetMainLookUpDir(int alCategory);
	tWStringVec GetLookUpDirs(int alCategory);

	tWString GetPathRelToWD(const tWString& asPath);
	tWString GetPathRelToWD(const tString& asPath);

	///////////////////////////////////////////////
	// Action management
	cEditorActionHandler* GetActionHandler() { return mpActionHandler; }
	void AddAction(iEditorAction* apAction);

	///////////////////////////////////////////////
	// Layout Management
	void SetLayoutVec3f(int alIdx, const cVector3f& avX);
	void SetLayoutVec3l(int alIdx, const cVector3l& avX);
	void SetLayoutVec2f(int alIdx, const cVector2f& avX);
	void SetLayoutVec2l(int alIdx, const cVector2l& avX);

	const cVector3f& GetLayoutVec3f(int alIdx);
	cVector3l GetLayoutVec3l(int alIdx);
	const cVector2f& GetLayoutVec2f(int alIdx);
	cVector2l GetLayoutVec2l(int alIdx);

	void SetLayoutNeedsUpdate(bool abX) { mbLayoutNeedsUpdate=abX;}
	void UpdateLayout();

    ///////////////////////////////////////////////
	// Grid Functions
	cEditorGrid* GetGrid();
	void SetPosOnGrid(const cVector3f& avPos);
	cVector3f& GetPosOnGridFromMousePos(bool abSnapped=true);

	///////////////////////////////////////////////
	// Thumbnails
	cEditorThumbnailBuilder* GetThumbnailBuilder() { return mpThumbnailBuilder; }

	///////////////////////////////////////////////
	// Edit Mode management
	iEditorEditMode* GetCurrentEditMode() { return mpCurrentEditMode; }
	void SetCurrentEditMode(iEditorEditMode* apEditMode);

	void AddEditMode(iEditorEditMode* apEditMode);

	iEditorEditMode* GetEditMode(const tString& asName);
	tEditorEditModeVec& GetEditModes() { return mvEditModes;} 


	///////////////////////////////////////////////
	// Control Flags
	void SetFlags(int alFlags, bool abX);
	bool GetFlags(int alFlags);
	void ClearFlags();

	virtual bool OnChangeFlags(int alFlags);

	///////////////////////////////////////////////
	// Hotkeys
    

	///////////////////////////////////////////////
	// Viewport Management
	virtual void CreateViewports();
	void SetFocusedViewport(cEditorWindowViewport* apViewport);
	cEditorWindowViewport* GetFocusedViewport() { return mpFocusedViewport; }
	void SetViewportLocked(bool abX);
	bool IsViewportLocked();

	void SetViewportEnlarged(bool abX);
	void SetWorldReflectionActive(bool abX);
	bool GetWorldReflectionActive();

	void SetViewportCameraPlanes(const cVector2f& avX);
	const cVector2f& GetViewportCameraPlanes();

	void SetViewportBGColor(const cColor& aX);
	const cColor& GetColor();

	tEditorViewportVec& GetViewports() { return mvViewports; }

	iEditorAction* CreateFocusOnSelectionAction();

	///////////////////////////////////////////////
	// Viewport Interaction event control	
	virtual void ViewportMouseDown(cEditorWindowViewport* apViewport, int alButtons);
	virtual void ViewportMouseUp(cEditorWindowViewport* apViewport, int alButtons);

	///////////////////////////////////////////////
	// Temp stuff
	void SetUpTempWorld();

	cWorld* GetTempWorld() { return mpTempWorld; }
	void AddTempMeshEntity(cMeshEntity* apMeshEntity);
	void RemoveTempMeshEntity(cMeshEntity* apMeshEntity);
	void ClearTempEntities();

	///////////////////////////////////////////////
	// Timing
	float GetTime() { return mfTimer; }

	///////////////////////////////////////////////
	// Input config	and options
	void SetSettingValue(const tString& asSetting, const tString& asValue) { mmapEditorSettings[asSetting] = asValue; }
	const tString& GetSetting(const tString& asSetting) { return mmapEditorSettings[asSetting]; }


	///////////////////////////////////
	// Execution Control
	/**
	 * Init the editor
	 * \param apEngine If this is null, then the editor will will init engine by itself and use settings from the setting file. Else current settings are used. 
	 */
	cEngine* Init(cEngine* apEngine, const char* asName, const char* asBuildDate, bool abDestroyEngineOnExit=false);
	

	///////////////////////////////////
	// HPL Engine stuff
	cGuiSet* GetSet() { return mpSet; }
	cGuiSkin* GetSkin() { return mpSkin; }
	cEngine* GetEngine() { return mpEngine; }

	cViewport* GetEngineViewport() { return mpViewport; }
	iFrameBuffer* GetViewportsFrameBuffer() { return mpFrameBuffer; }

	cEditorEntityLoader* GetEngineEntityLoader() { return mpEntityLoader; }


	///////////////////////////////////
	// Various Data
	cWidgetMainMenu* GetMainMenu() { return mpMainMenu; }


	/////////////////////////////////////////////
	// Temporary objects world (for thumbnails/indexing)
	cWorld* mpTempWorld;
	tMeshEntityList mlstTempMeshEntities;


	////////////////////////////////////
	// iUpdateable implementation
	void Update(float afTimeStep);

	void OnDraw(float afFrameTime){}
	void OnStart(){}
	void OnExit(){}

	void AppGotMouseFocus();
	void AppLostMouseFocus();

protected:
    ////////////////////////////
	// Own functions
	void InitInput();
	void InitLayout();

	void LoadConfig();
	void SaveConfig();

	virtual iEditorWorld* CreateSpecificWorld()=0;

	virtual void InitRenderTarget(const cVector2f& avSize);
	virtual void SetUpWindowAreas();
	virtual void SetUpViewports();

	virtual void OnUpdate(float afTimeStep){}
	virtual void AppGotInputFocus();

	virtual void OnLoadConfig()=0;
	virtual void OnSaveConfig()=0;

	virtual void OnInit(){}
	virtual void OnInitInput(){}
	virtual void OnInitLayout(){}

	virtual void OnSetFocusedViewport(){}

	virtual void OnPostUpdateLayout(){}

	virtual void OnWorldModify();
	virtual void OnSelectionChange();

	virtual void AppSpecificReset()=0;

	virtual void LoadEditorSession(iXmlDocument* apDoc, cXmlElement** apElement);
	virtual void SaveEditorSession(iXmlDocument* apDoc, cXmlElement** apElement);

	virtual cWidgetMainMenu* CreateMainMenu() { return NULL; }
	virtual void UpdateMenu(){}

	bool EscapeKeyHandlerCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(EscapeKeyHandlerCallback);

	////////////////////////////////////////////////////////////////////
	// New / Save / Load callbacks (for file picker popups)
	bool MessageBox_New_Callback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBox_New_Callback);
	bool MessageBox_Load_Callback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBox_Load_Callback);
	bool MessageBox_Exit_Callback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBox_Exit_Callback);
	bool MessageBox_Exit_UnsavedChanges_Callback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBox_Exit_UnsavedChanges_Callback);
	bool MessageBox_RecentFile_Callback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(MessageBox_RecentFile_Callback);

	bool RecentFileCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(RecentFileCallback);
	bool LoadFileCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LoadFileCallback);
	bool SaveFileCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SaveFileCallback);
	bool SaveFileAndQuitCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SaveFileAndQuitCallback);

	bool QuitCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(QuitCallback);

	
	bool PopUpCloseCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PopUpCloseCallback);

	////////////////////////////////////////////////////////////////////////////////////
	// Data
	bool mbDestroyEngineOnExit;
	bool mbDestroyingEditor;
	bool mbWorldModified;
	bool mbSelectionChanged;

	iEditorEditMode* mpCurrentEditMode;
	iEditorWorld* mpEditorWorld;
	cEditorSelection* mpSelection;
	cEditorActionHandler* mpActionHandler;
	cEditorUserClassDefinitionManager* mpClassDefManager;

	tEditorWindowList mlstWindows;
	tEditorWindowList mlstWindowsToDestroy;

	iEditorWindowLowerToolbar* mpLowerToolbar;
	iEditorWindowEditModeSidebar* mpEditModeSidebar;

	cEditorWindowViewport* mpFocusedViewport;
	bool mbViewportLocked;

	cEditorWindowLoaderStatus* mpLoaderStatusWindow;
	cEditorWindowMaterialEditor* mpMaterialEditor;

	cEditorThumbnailBuilder* mpThumbnailBuilder;

	//////////////////////////////////
	// Mouse pointer world position
	cVector3f mvSnappedPosOnGrid;
	cVector3f mvRealPosOnGrid;

	//////////////////////////////////
	// Folder stuff
	cDirectoryHandler* mpDirHandler;

	//////////////////////////////////
	// Config stuff
	cConfigFile* mpMainConfig;

	tString msCaption;
	std::map<tString, tString> mmapEditorSettings;

	/////////////////////////
	// Engine stuff	
	cEngine* mpEngine;
	iFrameBuffer* mpFrameBuffer;
	cGuiSet* mpSet;
	cGuiSkin* mpSkin;
	cViewport* mpViewport;

	cEditorEntityLoader* mpEntityLoader;

	float mfTimer;

	bool mbMouseMoved;
	bool mbLayoutNeedsUpdate;

	//////////////////////////
	// Flags
	int mlFlags;

	tEditorEditModeVec mvEditModes;

	// Layout
	cVector2f mvScreenSize;
	cWidgetFrame* mpBGFrame;
	cWidgetMainMenu* mpMainMenu;

	tEditorViewportVec mvViewports;

	std::map<int, cVector3f> mmapLayoutVecs3f;
	std::map<int, cVector2f> mmapLayoutVecs2f;

	///////////////////////
	// Saving / Loading
	tWString msFileCategoryName;
	tWString msFileCategoryString;

	tWString msSaveFilename;
	tWStringVec mvLoadFilenames;

	tWStringList mlstRecentFiles;

	tWString msLastLoadPath;
	tWString msLastSavePath;

	tWString msFilename;
};

//---------------------------------------------------------------

#endif //HPLEDITOR_EDITOR_BASE_CLASSES_H

