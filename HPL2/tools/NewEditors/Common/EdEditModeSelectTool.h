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

#ifndef ED_SCN_OBJ_MANIPULATOR_H
#define ED_SCN_OBJ_MANIPULATOR_H

#include "../common/StdAfx.h"

using namespace hpl;

//-------------------------------------------------------------------------------------

class iEdScnObject;

class iScnObjManipulator;
class iEdViewport;
class cViewportClick;
class cModuleDrawData;

class iEdAction;

//-------------------------------------------------------------------------------------

enum eSelectToolAxis
{
	eSelectToolAxis_X,
	eSelectToolAxis_Y,
	eSelectToolAxis_Z,

	eSelectToolAxis_LastEnum
};

//-------------------------------------------------------------------------------------

class iManipulatorComponent
{
public:
	iManipulatorComponent(iScnObjManipulator*);

	virtual void OnDraw(const cModuleDrawData&) {}
	virtual void OnUpdate() {};

	iScnObjManipulator* mpParent;
};

typedef std::vector<iManipulatorComponent*> tManipComponentVec;

//-------------------------------------------------------------------------------------

class iScnObjManipulator
{
	friend class cEdEditModeSelect;
public:
	iScnObjManipulator(const tWString&);
	virtual void Init() {}

	const tWString& GetName() { return msName; }

	cGuiGfxElement* GetIcon();

	virtual bool IsBusy()=0;

	void Activate(iEdScnObject* apObj) { mpTargetObject = apObj; }

	virtual bool OnViewportMouseDown(const cViewportClick&) { return true; }
	virtual bool OnViewportMouseUp(const cViewportClick&) { return true; }
	virtual void OnUpdate();

	void SetActiveComponent(iManipulatorComponent*);

	virtual iEdAction* CreateAction() { return NULL; }

	virtual void OnDraw(const cModuleDrawData&);

	const cMatrixf& GetTransform();

	cMatrixf mmtxManipTransform;
	cVector3f mvTransformedRayStart;
	cVector3f mvTransformedRayEnd;

	iManipulatorComponent* mpActiveComponent;
	iEdScnObject*		mpTargetObject;
protected:
	void AddComponent(iManipulatorComponent*);

	virtual void StartTransform() {}
	virtual void EndTransform() {}
	
	virtual void UpdateTransform() {}

	virtual void SaveCurrentTransform() {}

	tWString msName;

	tManipComponentVec	mvComponents;
};

//-------------------------------------------------------------------------------------

typedef std::vector<iScnObjManipulator*> tManipulatorVec;

typedef std::map<tWString, iScnObjManipulator*> tManipulatorMap;

//-------------------------------------------------------------------------------------

#endif // ED_SCN_OBJ_MANIPULATOR_H
