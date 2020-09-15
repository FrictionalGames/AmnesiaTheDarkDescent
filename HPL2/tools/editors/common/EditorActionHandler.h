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

#ifndef HPLEDITOR_EDITOR_ACTION_HANDLER_H
#define HPLEDITOR_EDITOR_ACTION_HANDLER_H

#include "EditorTypes.h"

//----------------------------------------------------------------

class iEditorAction;
class iEditorBase;

//----------------------------------------------------------------

class cEditorActionHandler
{
public:

	cEditorActionHandler(iEditorBase* apEditor);

	~cEditorActionHandler();

	void Reset();

	/** Calls iEditorAction::Do for the given action and stores it in the done list
	 *
	 * \param apAction 
	 */
	void Do(iEditorAction* apAction);

	/** Undoes the last done action
	 *
	 */
	void Undo();

	/** Redoes the last undone action
	 *
	 */
	void Redo();

	bool IsDoneActionsListEmpty() { return mlstDoneActions.empty(); }
	bool IsUndoneActionsListEmpty() { return mlstUndoneActions.empty(); }

	void SetMaxUndoSize(int alSize) { mlMaxUndoListSize = alSize; }
	int GetMaxUndoSize() { return mlMaxUndoListSize; }

	iEditorBase* GetEditor() { return mpEditor; }

protected:
	/////////////////////////
	// Data
	iEditorBase* mpEditor;

	tEditorActionList mlstDoneActions;
	tEditorActionList mlstUndoneActions;

	int mlMaxUndoListSize;
};

//----------------------------------------------------------------


#endif //HPLEDITOR_EDITOR_ACTION_HANDLER_H
