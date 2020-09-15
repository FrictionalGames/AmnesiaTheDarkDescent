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

#ifndef ED_LOWER_TOOLBAR_MODEL_H
#define ED_LOWER_TOOLBAR_MODEL_H

#include "../Common/EdPaneLowerToolbar.h"

class cEdLowerToolbarModel : public iEdPaneLowerToolbar
{
public:
	cEdLowerToolbarModel(iEditor*);
	~cEdLowerToolbarModel();

protected:
	bool WindowSpecificInputCallback(iEdInput*);

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool ShowPhysicsTest(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ShowPhysicsTest);

	void OnCreateLayout();
	void OnUpdate();

	void OnWorldParamsChange();

	cWidgetButton* mpBTest;
};

#endif // ED_LOWER_TOOLBAR_MODEL_H

