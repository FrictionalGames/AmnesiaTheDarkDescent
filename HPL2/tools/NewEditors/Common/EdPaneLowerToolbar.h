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

#ifndef ED_PANE_LOWER_TOOLBAR_H
#define ED_PANE_LOWER_TOOLBAR_H

#include "../Common/StdAfx.h"

#include "../Common/EdWindowHandler.h"

using namespace hpl;

//---------------------------------------------------------------

//---------------------------------------------------------------

class iEdPaneLowerToolbar : public iEdPane
{
public:
	iEdPaneLowerToolbar(iEditor*);
	~iEdPaneLowerToolbar();

	iWidget* AddGridControls();
	iWidget* AddViewportControls();
	iWidget* AddLightingControls();
	iWidget* AddCameraControls();
protected:
	////////////////////////////////
	// Own functions
	bool WindowSpecificInputCallback(iEdInput* apInput);

	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);

	////////////////////////////////
	// Implemented functions
	void OnCreateLayout();
	void OnUpdate();

	void OnViewportFocusChange(iEdViewport*);
	void OnViewportParamsChange(iEdViewport*);

	void OnWorldParamsChange();

	////////////////////////////////
	// Data

	// Grid controls
	cWidgetGroup*	mpGridControlsGroup;
	cWidgetButton*	mpBPlaneSwitch;
	cWidgetButton*	mpBSnap;
	cEdInputNumber* mpInpSnapSep;
	cEdInputNumber* mpInpPlaneHeight;

	// Viewport controls
	cWidgetDummy* mpHandleViewportControls;
	cWidgetButton* mpBEnlargeViewport;

	// Editor lighting controls
	cWidgetDummy* mpHandleLighting;
	cWidgetButton* mpBGlobalAmbientLight;
	cWidgetButton* mpBGlobalPointLight;

	// Camera controls
	cWidgetDummy* mpHandleCamera;
	cWidgetButton* mpBCameraLockToGrid;
	cWidgetButton* mpBCameraFocusOnSelection;
};

//--------------------------------------------------------------------

#endif //ED_PANE_LOWER_TOOLBAR_H
