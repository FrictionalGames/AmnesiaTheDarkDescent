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

#ifndef ED_WINDOW_EDIT_MODE_SIDEBAR_H
#define ED_WINDOW_EDIT_MODE_SIDEBAR_H

#include "../common/StdAfx.h"

using namespace hpl;

//----------------------------------------------------------------

#include "../Common/EdWindowHandler.h"

//----------------------------------------------------------------

class cEdWindowEditModeSidebar : public iEdPane
{
public:
	cEdWindowEditModeSidebar(iEditor*);

protected:
	void OnCreateLayout();
	void OnUpdate();

	void OnWorldSelectChange();
	///////////////////////////
	// Own functions
	bool Button_Pressed(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(Button_Pressed);

	///////////////////////////
	// Data
	tWidgetList mlstModeButtons;
};

//----------------------------------------------------------------

#endif // ED_WINDOW_EDIT_MODE_SIDEBAR_H

