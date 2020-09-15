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

#ifndef ED_TRANSLATE_MANIPULATOR_H
#define ED_TRANSLATE_MANIPULATOR_H


#include "../Common/EdEditModeSelectTool.h"

class cTranslatePlane;

//---------------------------------------------------------------------------

class iTranslateComponent : public iManipulatorComponent
{
public:
	iTranslateComponent(iScnObjManipulator*);

	virtual cPlanef GetEditPlane()=0;

	virtual cVector3f GetAxis()=0;

	virtual cVector3f GetRestrainedPos(const cVector3f&)=0;

protected:
	cBoundingVolume mBV;
	bool mbMouseOver;
	cVector3f mvMousePickPos;
};

//---------------------------------------------------------------------------

class cTranslateAxis : public iTranslateComponent
{
public:
	cTranslateAxis(iScnObjManipulator*, const cMatrixf& amtxOffset);

	void OnDraw(const cModuleDrawData&);

	void OnUpdate();

	cPlanef GetEditPlane();

	cVector3f GetAxis() { return mvAxis; }

	void AddPlane(cTranslatePlane*);
	cVector3f GetRestrainedPos(const cVector3f&);

protected:
	cVector3f mvAxis;
	iVertexBuffer* mpVB;
	cMatrixf mmtxOffset;

	std::vector<cTranslatePlane*> mvPlanes;
};

//---------------------------------------------------------------------------

class cTranslatePlane : public iTranslateComponent
{
public:
	cTranslatePlane(iScnObjManipulator*, cTranslateAxis*, cTranslateAxis*);

	void OnDraw(const cModuleDrawData&);

	void OnUpdate();

	cPlanef GetEditPlane();

	cVector3f GetAxis();

	cVector3f GetRestrainedPos(const cVector3f&);

protected:
	cTranslateAxis* mpAxis1;
	cTranslateAxis* mpAxis2;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECT TOOL TRANSLATE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cTranslateManipulator : public iScnObjManipulator
{
public:
	cTranslateManipulator();

	bool IsBusy() { return this->mbTransformStarted; }

	void OnDraw(const cModuleDrawData&);

	const cMatrixf& GetSizeMatrix() { return mmtxSize; }

	void OnUpdate();

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

	void StartTransform();
	void EndTransform();

	void UpdateTransform();

	cVector3f mvDragStartPos;
	cVector3f mvDragStartOffset;

	cVector3f mvDragEndPos;

	bool mbTransformStarted;
	cMatrixf mmtxSize;
	cMatrixf mmtxDrawSize;
	float mfT;
	cPlanef mPlane;

	cVector3f mvOldTranslation;
};
#endif // ED_TRANSLATE_MANIPULATOR_H
