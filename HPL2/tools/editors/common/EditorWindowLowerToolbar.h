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

#ifndef HPLEDITOR_EDITOR_WINDOW_LOWER_TOOLBAR_H
#define HPLEDITOR_EDITOR_WINDOW_LOWER_TOOLBAR_H

#include "../common/StdAfx.h"

#include "EditorWindow.h"

using namespace hpl;

//---------------------------------------------------------------

//---------------------------------------------------------------

class iEditorWindowLowerToolbar : public iEditorWindow
{
public:
	iEditorWindowLowerToolbar(iEditorBase* apEditor);
	~iEditorWindowLowerToolbar();

	iWidget* AddGridControls();
	iWidget* AddViewportControls();
	iWidget* AddLightingControls();
	iWidget* AddCameraControls();
	iWidget* AddClipPlaneControls();

	void SetFocusedClipPlane(int alX);
	int GetFocusedClipPlane();
protected:
	////////////////////////////////
	// Own functions
	bool WindowSpecificInputCallback(iEditorInput* apInput);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	void UpdateClipPlaneControls();

	////////////////////////////////
	// Implemented functions
	void OnInitLayout();
	void OnUpdate(float afTimeStep);

	////////////////////////////////
	// Data

	// Grid controls
	cWidgetGroup*	mpGridControlsGroup;
	cWidgetButton*	mpBPlaneSwitch;
	cWidgetButton*	mpBSnap;
	cEditorInputNumber* mpInpSnapSep;
	cEditorInputNumber* mpInpPlaneHeight;

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

	// Clip Plane controls
	bool mbClipPlaneVecUpdated;
	bool mbClipPlaneAdded;
	bool mbClipPlaneRemoved;
	cWidgetGroup* mpGClipPlanes;
	cWidgetButton* mpBAddClipPlane;
	cWidgetButton* mpBRemClipPlane;
	cEditorInputEnum* mpInpClipPlanes;

	cEditorInputBool* mpInpClipPlaneActive;
	cWidgetButton* mpBClipNormalSwitch;
	cEditorInputNumber* mpInpClipPlaneHeight;
	cWidgetButton* mpBClipPlaneCullSide;
};

//--------------------------------------------------------------------

#endif //HPLEDITOR_EDITOR_WINDOW_LOWER_TOOLBAR_H
