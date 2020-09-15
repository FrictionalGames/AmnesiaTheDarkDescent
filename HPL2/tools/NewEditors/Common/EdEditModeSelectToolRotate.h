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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_ROTATE_H
#define HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_ROTATE_H


#include "../Common/EdEditModeSelectTool.h"


class cRotateManipulator : public iScnObjManipulator
{
public:
	cRotateManipulator();

	bool IsBusy() { return this->mbTransformStarted; }

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

	bool IsActive();

	void OnUpdate();

	bool CheckRayIntersectsAxis(eSelectToolAxis aeAxis, const cVector3f& avRayStart, const cVector3f& avRayEnd){return false;}
	bool PointIntersectsAxis(eSelectToolAxis aeAxis, const cVector2f& avPoint);

	void CheckMouseOverAxis();

	bool CheckAxisSelected();

	cMatrixf& GetTransformMatrix();

	void OnDraw(const cModuleDrawData&);
	void DrawAxes(iEdViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength);
	
	void UpdateTransformation();


	void UpdateToolBoundingVolume();
	void UpdateMouseRay();

	void Reset();
	
	
	cPlanef mClipPlane;

	std::vector<cRect2f> mRectAxisAreaSphCoords[3];
	cRect2f mrectXAxisSphCoords1;
	cRect2f mrectXAxisSphCoords2;

	cRect2f mrectYAxisSphCoords1;
	cRect2f mrectYAxisSphCoords2;

	cRect2f mrectZAxisSphCoords1;
	cRect2f mrectZAxisSphCoords2;
	cRect2f mrectZAxisSphCoords3;

	cVector3f mvMouseDownCoords;
	cVector3f mvMouseUpCoords;

	cVector2f mvOldMousePos;
	cVector2f mvMousePos;

	cVector3f mvDisplacement;

	cVector3f mvReferenceRotation;

	cVector3f mvTangentVector;

	cVector3f mvScreenSpaceTangentVector;

	cColor mColorMouseOver;
	cColor mColorSelected;

	float mfAxisBaseLength;
	float mfUsedAxisLength;

	cMatrixf mmtxTransformMatrix;

	bool mvAxisMouseOver[3];
	cColor mvAxisColor[3];
	bool mvAxisSelected[3];

	cVector3f mvEditVectorStart;
	cVector3f mvEditVectorEnd;

	cVector3f mvRayStart, mvRayEnd;
	cVector3f mvTransformedRayStart, mvTransformedRayEnd;

	bool mbTransformStarted;

	cVector3f mvOldRotation;
	cMatrixf mmtxOldRotation;

};

#endif // HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_ROTATE_H
