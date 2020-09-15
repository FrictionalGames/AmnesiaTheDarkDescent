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

#ifndef ED_MODEL_EDITOR_MENU_H
#define ED_MODEL_EDITOR_MENU_H

#include "../Common/EdScnEditorMenu.h"

class cModelEditorMenu : public cEdScnEditorMenu
{
public:
	cModelEditorMenu(iEdWindow*);

protected:
	void OnCreateLayout();

	void OnWorldParamsChange();

	bool ImportMesh(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ImportMesh);

	bool ShowOutlineWindow(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ShowOutlineWindow);

	bool ShowClassWindow(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ShowClassWindow);

	bool ShowAnimationsWindow(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ShowAnimationsWindow);

};

#endif // ED_MODEL_EDITOR_MENU_H
