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

#ifndef EDITOR_H
#define EDITOR_H

#include "../Common/StdAfx.h"

#include "../Common/EdModule.h"
#include "../Common/EdEditMode.h"

#include "../Common/EdViewport.h"

//-----------------------------------------------------------------------

class iEdAction;
class iEdMenu;
class iEdWorld;

class cEdConfigHandler;
class cEdActionHandler;
class cEdDirectoryHandler;
class cEdInputHandler;
class cEdWindowHandler;
class cEdEditModeHandler;
class cEdThumbnailBuilder;

class cViewportHandler;

class cEdWindowViewport;
class cViewportClick;

//-----------------------------------------------------------------------

enum eDir
{
	eDir_Textures,
	eDir_Materials,
	eDir_Billboards,
	eDir_Lights,
	eDir_Sounds,
	eDir_Particles,

	eDir_LastEnum
};

enum eLayoutBlock_Editor
{
	eLayoutBlock_Sidebar = eLayoutBlock_LastEnum,
	eLayoutBlock_EditMode,
	eLayoutBlock_ViewportArea,
	
	eLayoutBlock_Editor_LastEnum,
};

//-----------------------------------------------------------------------

/**
 * \ingroup 
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2010
 *
 * \author Luis
 *
 */
class iEditor : public iUpdateable, public iEdModule
{
public:
	iEditor(const tString&, const tString&);
	virtual ~iEditor();

	static iEditor* GetInstance(const tWString& asCommandLine=_W(""));
	static iEditor* CreateInstance(const tWString&);

	const tString& GetName() { return msName; }
	const tString& GetCaption() { return msCaption; }

	/**
	 *
	 * \param cEngine* 
	 * \param char* 
	 * \param char* 
	 * \param bool 
	 * \return 
	 */
	cEngine* Init(cEngine*, bool);

	void Reset();

	void Destroy();

	float GetTimeStep() { return mfTimeStep; }

	///////////////////////////////////////////////
	// World (Scene)
	iEdWorld* GetWorld() { return mpWorld; }

	///////////////////////////////////////////////
	// Action management
	cEdActionHandler* GetActionHandler() { return mpActionHandler; }
	void AddAction(iEdAction*);

	///////////////////////////////////////////////
	// Directory Management
	cEdDirectoryHandler* GetDirHandler() { return mpDirHandler; }
	
	const tWString& GetWorkingDir();
	const tWString& GetHomeDir();
	const tWString& GetTempDir();
	const tWString& GetThumbnailDir();

	const tWString& GetMainLookUpDir(int);
	tWStringVec GetLookUpDirs(int);

	tWString GetPathRelToWD(const tWString&);

	///////////////////////////////////////////////
	// Input Management
	cEdInputHandler* GetInputHandler() { return mpInputHandler; }

	///////////////////////////////////////////////
	// EditMode Handler
	cEdEditModeHandler* GetEditModeHandler() { return mpEditModeHandler; }

	void AddEditMode(iEdEditMode*);
	iEdEditMode* GetActiveEditMode();
	bool SetActiveEditMode(iEdEditMode*);

	const tEditModeList& GetEditModes();

	///////////////////////////////////////////////
	// Layout Handler
	cEdWindowHandler* GetWindowHandler() { return mpWindowHandler; }

	void AddWindow(iEdWindow*, int alLayoutBlockID=-1);

	// Viewports
	iEdViewport* GetFocusedViewport()	{ return mpFocusedViewport; }
	void SetFocusedViewport(iEdViewport* apViewport);

	void SetViewportsActive(bool);
	bool IsViewportsActive();

	int GetNumViewports();
	iEdViewport* GetViewport(int);

	void LockToViewport(iEdViewport* apViewport);
	bool IsLockedToViewport()	{ return mbViewportLocked; }

	void SetViewportEnlarge(bool);
	bool IsViewportEnlarged();

	// Special windows
	void ShowPreferencesDialog();


	cEdConfigHandler* GetConfigHandler() { return mpConfigHandler; }

	

	///////////////////////////////////////////////
	// Thumbnail Builder
	cEdThumbnailBuilder* GetThumbnailBuilder() { return mpThumbnailBuilder; }

	///////////////////////////////////////////////
	// Engine
	cEngine* GetEngine() { return mpEngine; }

	cGuiSet* GetGuiSet() { return mpSet; }

	iFrameBuffer* GetMainFrameBuffer() { return mpFrameBuffer; }

	// iUpdateable implementation
	virtual void Update(float afTimeStep);

	///////////////////////////////////////////////
	// Module intercommunication
	void BroadcastModuleMsg(eEdModuleMsg, void*);

	///////////////////////////////////////////////
	// User interaction
	void ViewportMouseDown(const cViewportClick&);
	void ViewportMouseUp(const cViewportClick&);

	cConfigFile*	mpGlobalCfg;
	cConfigFile*	mpLocalCfg;
protected:
	iEdModule* AddModule(iEdModule*);
	void RemoveModule(const tWString&);

	bool LoadGlobalConfig();
	bool LoadLocalConfig();

	virtual iEdMenu* CreateMenu()=0;
	virtual iEdWorld* CreateWorld()=0;

	virtual void SetUpRenderTarget(const cVector2f& );

	virtual void SetUpDirectories()=0;
	virtual void SetUpEditModes()=0;

	///////////////////////////////////////////////
	// Layout
	void CreateLayout();
	virtual void CreateAppSpecificLayout()=0;

	void AddViewport(iEdViewport* apViewport, 
						const cVector3f& avNormalPos, const cVector2f& avNormalSize, 
						const cVector2f& avFBPos, const cVector2f& avFBSize,
						int alStartMode=0);
	void SetEnlargedViewportPosAndSize(const cVector3f&, const cVector2f&);

	void InitViewports();

	virtual iEdWindow* CreatePreferencesDialog()=0;

	void ShowDialog(iEdWindow*, bool abUnique);

	////////////////////////////////////////////////////////////////////
	// Events
	virtual void OnReset();

	virtual void OnWorldLoad();
	virtual void OnWorldSave();
	virtual void OnWorldModify();
	virtual void OnWorldObjModify();
	virtual void OnWorldParamsChange();

	////////////////////////////////////////////////////////////////////
	// Data
	tString msName;
	tString msCaption;

	float mfTimeStep;

	///////////////////////////////////////////
	// Modules
	cEdConfigHandler*		mpConfigHandler;
	cEdActionHandler*		mpActionHandler;
	cEdDirectoryHandler*	mpDirHandler;
	cEdInputHandler*		mpInputHandler;
	cEdWindowHandler*		mpWindowHandler;
	cEdEditModeHandler*		mpEditModeHandler;

	iEdMenu*		mpMenu;
	iEdWorld*		mpWorld;

	cEdThumbnailBuilder*	mpThumbnailBuilder;

	///////////////////////////////////////////
	// Edit Modes
	tEdModuleVec	mvModules;
	bool			mbWaitingViewportClickUp;

	////////////////////////////////////////////////////////
	// Viewport related (TODO: move this to a ViewportHandler class?)
	iEdViewport*	mpFocusedViewport;
	tViewportVec	mvViewports;
	tVector3fVec	mvViewportPositions;
	tVector2fVec	mvViewportSizes;
	tVector2fVec	mvViewportFBPositions;
	tVector2fVec	mvViewportFBSizes;
	cVector3f		mvViewportEnlargedPos;
	cVector2f		mvViewportEnlargedSize;
	bool mbViewportLocked;

	//////////////////////////////////////////////
	// Engine related data
	bool mbDestroyEngineOnExit;
	cEngine*		mpEngine;
	iFrameBuffer*	mpFrameBuffer;
	cViewport*		mpViewport;

	cGuiSet*		mpSet;

	bool mbDestroying;


	//////////////////////////////////////////////
	// Editor instance : only one at all times
	static iEditor* mpInstance;
};

#endif // EDITOR_H
