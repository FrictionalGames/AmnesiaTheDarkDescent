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

#ifndef ED_SCN_EDITOR_MENU_H
#define ED_SCN_EDITOR_MENU_H

#include "../Common/EdGenericMenu.h"

//----------------------------------------------------------

class cEdScnEditorMenu : public cEdGenericMenu
{
public:
	cEdScnEditorMenu(iEdWindow*);

protected:
	void AddEditControls(cWidgetMenuItem*);

	void OnWorldSelectChange();

	virtual void UpdateEditControls();

	bool DuplicateObject(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(DuplicateObject);

	bool DeleteObject(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(DeleteObject);

	////////////////////////////////////////////////////////
	// Data
	cWidgetMenuItem* mpClone;
	cWidgetMenuItem* mpDelete;
};

#endif // ED_SCN_EDITOR_MENU_H
