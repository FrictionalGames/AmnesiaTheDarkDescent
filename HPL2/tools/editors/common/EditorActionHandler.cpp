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

#include "EditorActionHandler.h"
#include "EditorAction.h"
#include "EditorBaseClasses.h"

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

cEditorActionHandler::cEditorActionHandler(iEditorBase* apEditor)
{
	mpEditor = apEditor;
    mlMaxUndoListSize = 50;
}

cEditorActionHandler::~cEditorActionHandler()
{
	STLDeleteAll(mlstDoneActions);
	STLDeleteAll(mlstUndoneActions);
}

//---------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

void cEditorActionHandler::Reset()
{
	STLDeleteAll(mlstUndoneActions);
	STLDeleteAll(mlstDoneActions);
}

//---------------------------------------------------------------

void cEditorActionHandler::Do(iEditorAction* apAction)
{
	if(apAction==NULL)
		return;

	apAction->Do();

	mlstDoneActions.push_back(apAction);
	if(mlMaxUndoListSize<(int)mlstDoneActions.size())
	{
		iEditorAction* pAction = mlstDoneActions.front();
		mlstDoneActions.pop_front();

		hplDelete(pAction);
	}

	STLDeleteAll(mlstUndoneActions);

	mpEditor->SetLayoutNeedsUpdate(true);
}

//---------------------------------------------------------------

void cEditorActionHandler::Undo()
{
	if(IsDoneActionsListEmpty()) return;

	iEditorAction* pAction = mlstDoneActions.back();

	mlstDoneActions.pop_back();

	pAction->Undo();

	mlstUndoneActions.push_back(pAction);

	mpEditor->SetLayoutNeedsUpdate(true);
}

//---------------------------------------------------------------

void cEditorActionHandler::Redo()
{
	if(IsUndoneActionsListEmpty()) return;

	iEditorAction* pAction = mlstUndoneActions.back();

	mlstUndoneActions.pop_back();

	pAction->Do();

	mlstDoneActions.push_back(pAction);

	mpEditor->SetLayoutNeedsUpdate(true);
}

//---------------------------------------------------------------
