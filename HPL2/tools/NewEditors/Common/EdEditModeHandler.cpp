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

#include "../Common/EdEditModeHandler.h"

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cEdEditModeHandler::cEdEditModeHandler(iEditor* apEditor) : iEdModule(apEditor, _W("EditModeHandler"))
{
	mpActiveMode = NULL;
}

cEdEditModeHandler::~cEdEditModeHandler()
{
	STLDeleteAll(mlstModes);
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cEdEditModeHandler::AddMode(iEdEditMode* apMode)
{
	if(apMode==NULL)
		return;

	mlstModes.push_back(apMode);
}

//--------------------------------------------------------------------------------

iEdEditMode* cEdEditModeHandler::GetActiveMode()
{
	return mpActiveMode;
}

//--------------------------------------------------------------------------------

bool cEdEditModeHandler::SetActiveMode(iEdEditMode* apMode)
{
	if(mpActiveMode==apMode) return false;

	if(mpActiveMode)
		mpActiveMode->SetActive(false);

	mpActiveMode = apMode;

	if(mpActiveMode)
		mpActiveMode->SetActive(true);

	return true;
}

//--------------------------------------------------------------------------------

iEdEditMode* cEdEditModeHandler::GetMode(const tWString& asName)
{
	tWString sName = _W("EditMode: ") + asName;
	tEditModeListIt it = mlstModes.begin();
	for(; it!=mlstModes.end(); ++it)
	{
		iEdEditMode* pMode = *it;
		if(pMode->GetName()==sName)
			return pMode;
	}

	return NULL;
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnInit()
{
	BroadcastMsgToModes(eEdModuleMsg_Init, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnCreateLayout()
{
	BroadcastMsgToModes(eEdModuleMsg_CreateLayout, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnWorldLoad()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_WorldLoad, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnWorldSave()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_WorldSave, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnWorldAddRemObject()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_WorldAddRemObject, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnWorldObjModify()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_WorldObjModify, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnWorldSelectChange()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_WorldSelectChange, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnReset()
{
	// Reset all added modes
	SetActiveMode(NULL);
	BroadcastMsgToModes(eEdModuleMsg_Reset, NULL);

	// Choose first added mode, if any.
	iEdEditMode* pFirstMode = NULL;
	if(mlstModes.empty()==false)
		pFirstMode = mlstModes.front();

	SetActiveMode(pFirstMode);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnUpdate()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_Update, NULL);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnDraw(const cModuleDrawData& aData)
{
	if(mpActiveMode) mpActiveMode->OnDraw(aData);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::OnViewportMouseMove()
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_ViewportMouseMove,NULL);
}

void cEdEditModeHandler::OnViewportMouseDown(const cViewportClick& aData)
{
	if(mpActiveMode) mpActiveMode->DispatchMessage(eEdModuleMsg_ViewportMouseDown, (void*)&aData);
}

void cEdEditModeHandler::OnViewportMouseUp(const cViewportClick& aData)
{
	if(mpActiveMode && mpActiveMode->IsBusy()) mpActiveMode->DispatchMessage(eEdModuleMsg_ViewportMouseUp, (void*)&aData);
}

//--------------------------------------------------------------------------------

void cEdEditModeHandler::BroadcastMsgToModes(eEdModuleMsg aMsg, void *apData)
{
	tEditModeListIt it = mlstModes.begin();
	for(;it!=mlstModes.end();++it)
	{
		iEdEditMode* pMode = *it;
		
		pMode->DispatchMessage(aMsg, apData);
	}
}

//--------------------------------------------------------------------------------
