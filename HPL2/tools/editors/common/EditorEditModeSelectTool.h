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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_H
#define HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_H

#include "../common/StdAfx.h"

using namespace hpl;

//-------------------------------------------------------------------------------------

class iEditorAction;

class cEditorEditModeSelect;
class cEditorSelection;

class cEditorWindowViewport;

//-------------------------------------------------------------------------------------

enum eSelectToolMode
{
	eSelectToolMode_Translate,
	eSelectToolMode_Rotate,
	eSelectToolMode_Scale,
	
	eSelectToolMode_LastEnum
};

enum eSelectToolAxis
{
	eSelectToolAxis_X,
	eSelectToolAxis_Y,
	eSelectToolAxis_Z,

	eSelectToolAxis_LastEnum
};

//-------------------------------------------------------------------------------------

class cEditorEditModeSelectTool
{
	friend class cEditorEditModeSelect;
public:
	cEditorEditModeSelectTool(eSelectToolMode aToolMode, cEditorEditModeSelect* apEditMode, cEditorSelection* apSelection);

	virtual bool IsActive()=0;

	eSelectToolMode GetToolMode() { return mToolMode; }

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);
	virtual void DrawAxes(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, float afAxisLength){}

	void OnEditorUpdate();
	virtual void UpdateTransformation(){}

	virtual bool OnViewportMouseDown(int alButtons);
	virtual bool OnViewportMouseUp(int alButtons);

	virtual void Reset();

	bool CheckAxisSelected();
	virtual bool CheckRayIntersectsAxis(eSelectToolAxis aeAxis, const cVector3f& avRayStart, const cVector3f& avRayEnd)=0;

	virtual void CheckMouseOverAxis(){}

	virtual cMatrixf& GetTransformMatrix()=0;

	virtual void BuildEditPlane();


	virtual iEditorAction* CreateAction()=0;

protected:
	void UpdateMouseRay();

	virtual void UpdateToolGraphics(){}
	virtual void UpdateToolBoundingVolume(){}

	eSelectToolMode mToolMode;
	cEditorEditModeSelect* mpEditMode;
	cEditorSelection* mpSelection;

	cColor mColorMouseOver;
	cColor mColorSelected;

	cColor mvAxisColor[3];
	bool   mvAxisSelected[3];
	bool   mvAxisMouseOver[3];

	float mfUsedAxisLength;
	float mfAxisBaseLength;
	float mfOrthoAxisPercentSize;

	cVector3f mvRayStart;
	cVector3f mvRayEnd;

	cVector3f mvTransformedRayStart;
	cVector3f mvTransformedRayEnd;

	cVector3f mvMouseOffset;
	cVector3f mvEditVectorStart;
	cVector3f mvEditVectorEnd;


	cMatrixf mmtxTransformMatrix;
	cPlanef mEditingPlane;

};

//-------------------------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_EDIT_MODE_SELECT_TOOL_TRANSLATE_H
