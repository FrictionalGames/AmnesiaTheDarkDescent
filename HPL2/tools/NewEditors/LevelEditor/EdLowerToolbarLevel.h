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

#ifndef ED_LOWER_TOOLBAR_LEVEL_H
#define ED_LOWER_TOOLBAR_LEVEL_H

#include "../Common/EdPaneLowerToolbar.h"

class cEdLowerToolbarLevel : public iEdPaneLowerToolbar
{
public:
	cEdLowerToolbarLevel(iEditor*);
	~cEdLowerToolbarLevel();

	iWidget* AddClipPlaneControls();
	
	void SetFocusedClipPlane(int);
	int GetFocusedClipPlane();
protected:
	bool WindowSpecificInputCallback(iEdInput*);

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	void UpdateClipPlaneControls();

	void OnCreateLayout();
	void OnUpdate();

	void OnReset();

	void OnWorldParamsChange();

	// Clip Plane controls
	bool mbClipPlaneAdded;
	bool mbClipPlaneRemoved;

	cWidgetGroup* mpGClipPlanes;
	cWidgetButton* mpBAddClipPlane;
	cWidgetButton* mpBRemClipPlane;
	cEdInputEnum* mpInpClipPlanes;

	cEdInputBool* mpInpClipPlaneActive;
	cWidgetButton* mpBClipNormalSwitch;
	cEdInputNumber* mpInpClipPlaneHeight;
	cWidgetButton* mpBClipPlaneCullSide;
};

#endif // ED_LOWER_TOOLBAR_LEVEL_H

