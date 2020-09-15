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

#include "../Common/EdActionHandler.h"

#include "../Common/Editor.h"
#include "../Common/EdModule.h"

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cEdActionHandler::cEdActionHandler(iEditor* apEditor) : iEdModule(apEditor, _W("ActionHandler"))
{
    mlMaxUndoListSize = 50;
}

cEdActionHandler::~cEdActionHandler()
{
	OnReset();
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEdActionHandler::AddAction(iEdAction* apAction)
{
	if(apAction==NULL)
	{
		//Error("Error adding action: NULL action found\n");
		return;
	}

	if(apAction->Init()==false)
	{
		Error("Error adding action %ls\n", apAction->GetName().c_str());
		hplDelete(apAction);
		return;
	}

	Do(apAction);
	
	STLDeleteAll(mlstUndoneActions);

	UpdateStack();
}

//---------------------------------------------------------------

void cEdActionHandler::Undo()
{
	if(HasDoneActions()==false) return;

	iEdAction* pAction = mlstDoneActions.back();
	mlstDoneActions.pop_back();

	pAction->Undo();
	mlstUndoneActions.push_back(pAction);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_ActionExec, NULL);
}

//---------------------------------------------------------------

void cEdActionHandler::Redo()
{
	if(HasUndoneActions()==false) return;

	iEdAction* pAction = mlstUndoneActions.back();
	mlstUndoneActions.pop_back();

	Do(pAction);
}

//---------------------------------------------------------------

bool cEdActionHandler::HasDoneActions()
{
	return mlstDoneActions.empty()==false;
}

bool cEdActionHandler::HasUndoneActions()
{
	return mlstUndoneActions.empty()==false;
}

//---------------------------------------------------------------

void cEdActionHandler::SetMaxUndoStackSize(size_t alSize)
{
	mlMaxUndoListSize = alSize;

	UpdateStack();
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEdActionHandler::Do(iEdAction* apAction)
{
	apAction->Do();

	mlstDoneActions.push_back(apAction);

	mpEditor->BroadcastModuleMsg(eEdModuleMsg_ActionExec, NULL);
}

//---------------------------------------------------------------

void cEdActionHandler::OnReset()
{
	STLDeleteAll(mlstDoneActions);
	STLDeleteAll(mlstUndoneActions);
}

//---------------------------------------------------------------

void cEdActionHandler::OnLoadGlobalConfig(cConfigFile* apCfg)
{
	mlMaxUndoListSize = apCfg->GetInt("General", "UndoStackSize", 50);
}

//---------------------------------------------------------------

void cEdActionHandler::UpdateStack()
{
	for(size_t i=mlstDoneActions.size(); i>mlMaxUndoListSize; --i)
	{
		iEdAction* pAction = mlstDoneActions.front();
		hplDelete(pAction);

		mlstDoneActions.pop_front();
	}
}

//---------------------------------------------------------------
