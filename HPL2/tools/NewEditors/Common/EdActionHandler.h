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

#ifndef ED_ACTION_HANDLER_H
#define ED_ACTION_HANDLER_H

//----------------------------------------------------------------

#include "../Common/EdModule.h"
#include "../Common/EdAction.h"

//----------------------------------------------------------------

class iEditor;
class iEdAction;

//----------------------------------------------------------------

class cEdActionHandler : public iEdModule
{
public:

	cEdActionHandler(iEditor* apEditor);
	~cEdActionHandler();

	/** Calls iEdAction::Do for the given action and stores it in the done list
	 *
	 * \param apAction 
	 */
	void AddAction(iEdAction* apAction);

	/** Undoes the last done action
	 *
	 */
	void Undo();

	/** Redoes the last undone action
	 *
	 */
	void Redo();

	bool HasDoneActions();
	bool HasUndoneActions();
	//bool IsDoneActionsListEmpty() { return mlstDoneActions.empty(); }
	//bool IsUndoneActionsListEmpty() { return mlstUndoneActions.empty(); }

	void SetMaxUndoStackSize(size_t);
	int GetMaxUndoStackSize() { return (int) mlMaxUndoListSize; }

protected:
	void Do(iEdAction*);


	void OnReset();

	void OnLoadGlobalConfig(cConfigFile*);

	void UpdateStack();
	/////////////////////////
	// Data
	tEdActionList mlstDoneActions;
	tEdActionList mlstUndoneActions;

	size_t mlMaxUndoListSize;
};

//----------------------------------------------------------------


#endif //ED_ACTION_HANDLER_H
