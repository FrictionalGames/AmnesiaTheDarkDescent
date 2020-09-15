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

#ifndef ED_MODULE_H
#define ED_MODULE_H

//-----------------------------------------------

#include "../Common/StdAfx.h"

using namespace hpl;
using namespace std;

//-----------------------------------------------

class iEditor;

class iEdViewport;
class cViewportClick;

//-----------------------------------------------

enum eEdModuleMsg
{
	eEdModuleMsg_Init,
	eEdModuleMsg_Reset,
	eEdModuleMsg_Update,

	eEdModuleMsg_Draw,

	eEdModuleMsg_CreateLayout,
	
	eEdModuleMsg_LoadGlobalCfg,
	eEdModuleMsg_LoadLocalCfg,

	eEdModuleMsg_SaveGlobalCfg,
	eEdModuleMsg_SaveLocalCfg,

	eEdModuleMsg_ActionExec,

	// World Events
	eEdModuleMsg_WorldLoad,
	eEdModuleMsg_WorldSave,
	eEdModuleMsg_WorldModify,
	eEdModuleMsg_WorldAddRemObject,
	eEdModuleMsg_WorldObjModify,
	eEdModuleMsg_WorldParamsChange,
	eEdModuleMsg_WorldSelectChange,

	eEdModuleMsg_ViewportFocusChange,
	eEdModuleMsg_ViewportParamsChange,

	eEdModuleMsg_ViewportMouseMove,
	eEdModuleMsg_ViewportMouseDown,
	eEdModuleMsg_ViewportMouseUp,
};

//-----------------------------------------------

enum eEdDrawStep
{
	eEdDrawStep_PostSolid,
	eEdDrawStep_PostTrans,
};

class cModuleDrawData
{
public:
	cModuleDrawData(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
					eEdDrawStep aStep, iGpuProgram* apProg=NULL) :	mpViewport(apViewport), 
																	mpFunctions(apFunctions),
																	mStep(aStep),
																	mpProgram(apProg)
	{
	}

	iEdViewport* mpViewport;
	cRendererCallbackFunctions* mpFunctions;
	eEdDrawStep mStep;
	iGpuProgram* mpProgram;
};

//-----------------------------------------------

/**
 * \ingroup
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2010
 *
 * \author Luis
 *
 *
 */
class iEdModule
{
public:
	iEdModule(iEditor*, const tWString&);
	virtual ~iEdModule();
	
	iEditor* GetEditor()				{ return mpEditor; }
	const tWString& GetName()			{ return msName; }

	virtual void DispatchMessage(eEdModuleMsg, void*);
protected:
	virtual void OnInit()				{}
	virtual void OnCreateLayout()		{}
	virtual void OnReset()				{}
	virtual void OnUpdate()				{}

	virtual void OnLoadGlobalConfig(cConfigFile*)	{}
	virtual void OnLoadLocalConfig(cConfigFile*)	{}

	virtual void OnSaveGlobalConfig(cConfigFile*)	{}
	virtual void OnSaveLocalConfig(cConfigFile*)	{}

	virtual void OnActionExec()						{}

	virtual void OnDraw(const cModuleDrawData&)		{}

	virtual void OnWorldLoad()			{}
	virtual void OnWorldSave()			{}
	virtual void OnWorldModify()		{}
	virtual void OnWorldAddRemObject()	{}
	virtual void OnWorldObjModify()		{}
	virtual void OnWorldParamsChange()	{}
	virtual void OnWorldSelectChange()	{}

	virtual void OnViewportFocusChange(iEdViewport*)	{}
	virtual void OnViewportParamsChange(iEdViewport*)	{}
	virtual void OnViewportMouseMove()					{}
	virtual void OnViewportMouseDown(const cViewportClick&)	{}
	virtual void OnViewportMouseUp(const cViewportClick&)	{}

	/////////////////////////////////////////
	// Data
	iEditor* mpEditor;
	tWString msName;
};

//-----------------------------------------------

typedef std::vector<iEdModule*> tEdModuleVec;

//-----------------------------------------------

#endif // ED_MODULE_H
              