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

#include "../Common/EdModule.h"

//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdModule::iEdModule(iEditor* apEditor, const tWString& asName) : mpEditor(apEditor), msName(asName)
{
}

iEdModule::~iEdModule()
{
}

//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void iEdModule::DispatchMessage(eEdModuleMsg aMsg, void* apData)
{
	switch(aMsg)
	{
	case eEdModuleMsg_Init:					OnInit(); break;

	case eEdModuleMsg_CreateLayout:			OnCreateLayout(); break;

	case eEdModuleMsg_Reset:				OnReset(); break;

	case eEdModuleMsg_Update:				OnUpdate(); break;

	case eEdModuleMsg_Draw:					OnDraw(*static_cast<cModuleDrawData*>(apData)); break;

	case eEdModuleMsg_LoadGlobalCfg:		OnLoadGlobalConfig(static_cast<cConfigFile*>(apData)); break;

	case eEdModuleMsg_LoadLocalCfg:			OnLoadLocalConfig(static_cast<cConfigFile*>(apData)); break;

	case eEdModuleMsg_SaveLocalCfg:			OnSaveLocalConfig(static_cast<cConfigFile*>(apData)); break;

	case eEdModuleMsg_SaveGlobalCfg:		OnSaveGlobalConfig(static_cast<cConfigFile*>(apData)); break;

	case eEdModuleMsg_ActionExec:			OnActionExec();			break;

	////////////////////////////////////////////////////////
	// World events
	case eEdModuleMsg_WorldLoad:			OnWorldLoad();			break;
	case eEdModuleMsg_WorldSave:			OnWorldSave();			break;
	case eEdModuleMsg_WorldModify:			OnWorldModify();		break;
	case eEdModuleMsg_WorldAddRemObject:	OnWorldAddRemObject();	break;
	case eEdModuleMsg_WorldObjModify:		OnWorldObjModify();		break;
	case eEdModuleMsg_WorldParamsChange:	OnWorldParamsChange();	break;
	case eEdModuleMsg_WorldSelectChange:	OnWorldSelectChange();	break;


	case eEdModuleMsg_ViewportFocusChange:	OnViewportFocusChange(static_cast<iEdViewport*>(apData));break;
	case eEdModuleMsg_ViewportParamsChange:	OnViewportParamsChange(static_cast<iEdViewport*>(apData));break;

	case eEdModuleMsg_ViewportMouseMove:	OnViewportMouseMove();break;

	case eEdModuleMsg_ViewportMouseDown:	OnViewportMouseDown(*static_cast<cViewportClick*>(apData)); break;
	case eEdModuleMsg_ViewportMouseUp:		OnViewportMouseUp(*static_cast<cViewportClick*>(apData)); break;
	}
}

//---------------------------------------------------------------------------
