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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_SCALE_H
#define HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_SCALE_H


//-----------------------------------------------------------------------

#include "EditorEditModeSelectTool.h"

//-----------------------------------------------------------------------

class cEditorEditModeSelectToolScale : public cEditorEditModeSelectTool
{
public:
	cEditorEditModeSelectToolScale(cEditorEditModeSelect* apParent, cEditorSelection* apSelection);

	bool IsActive();

	bool CheckRayIntersectsAxis(eSelectToolAxis aeAxis, const cVector3f& avRayStart, const cVector3f& avRayEnd);

	void CheckMouseOverAxis();

	cMatrixf& GetTransformMatrix();

	void DrawAxes(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength);

	void UpdateTransformation();

	iEditorAction* CreateAction();

	void UpdateToolBoundingVolume();

	void Reset();


	cVector3f mvAxisAddedLength;

	cVector3f mvAxisMin[3];
	cVector3f mvAxisMax[3];
	cVector3f mvHeadMin[3];
	cVector3f mvHeadMax[3];

	cVector3f mvAllAxesHandleMin;
	cVector3f mvAllAxesHandleMax;

	cVector3f mvScreenSpaceAxis;
	cVector2f mvOldMousePos;

	tVertexVec mvCubeQuads[6];
};

//-----------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_SCALE_H
