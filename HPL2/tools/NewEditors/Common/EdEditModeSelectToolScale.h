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

#ifndef ED_SCALE_MANIPULATOR_H
#define ED_SCALE_MANIPULATOR_H


#include "../Common/EdEditModeSelectTool.h"

//---------------------------------------------------------------------------

class cScaleAxis : public iManipulatorComponent
{
public:
	cScaleAxis(iScnObjManipulator*, const cMatrixf& amtxOffset);

	void OnDraw(const cModuleDrawData&);

	void OnUpdate();

	cPlanef GetEditPlane();

	const cVector3f& GetAxis() { return mvAxis; }

protected:
	cVector3f mvAxis;

	cBoundingVolume mBV;
	iVertexBuffer* mpVB;

	cMatrixf mmtxOffset;

	bool mbMouseOver;
	cVector3f mvMousePickPos;
};

//---------------------------------------------------------------------------

/*
class cUniformScaler :public iManipulatorComponent
{
public:
	cUniformScaler(iScnObjManipulator*);

	void OnDraw(const cModuleDrawData&);

	void OnUpdate();

	cPlanef GetEditPlane();

	const

protected:

};
*/

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECT TOOL TRANSLATE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cScaleManipulator : public iScnObjManipulator
{
public:
	cScaleManipulator(const tWString& asX=_W("GenericScale"));
	virtual void Init();

	bool IsBusy() { return mbTransformStarted; }

	void OnDraw(const cModuleDrawData&);

	const cMatrixf& GetSizeMatrix() { return mmtxSize; }

	void OnUpdate();

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

	void StartTransform();
	void EndTransform();

	void UpdateTransform();

	iEdAction* CreateAction();

	virtual void StoreOldTransform();
	virtual void StoreCurrentTransform();
	virtual void SetTransform();
	virtual void FallToOldTransform();

	cVector3f mvDragStartPos;
	cVector3f mvDragEndPos;

	bool mbTransformStarted;
	cMatrixf mmtxSize;
	cMatrixf mmtxRotate;
	cMatrixf mmtxDrawSize;
	float mfT;
	cPlanef mPlane;

	cVector3f mvOldScale;
	cVector3f mvTempScale;
	cVector3f mvNewScale;
};

//---------------------------------------------------------------------------

#endif // ED_SCALE_MANIPULATOR_H
