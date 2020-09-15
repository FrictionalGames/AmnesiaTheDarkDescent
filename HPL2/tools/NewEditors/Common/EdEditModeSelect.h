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

#ifndef ED_EDIT_MODE_SELECT_H
#define ED_EDIT_MODE_SELECT_H

#include "../Common/EdEditMode.h"

#include "../Common/EdScnObject.h"
//#include "../Common/EditorTypes.h"

#include "../Common/EdEditModeSelectTool.h"
//#include "../Common/EditorActionEntity.h"

#include "../Common/EdUIPicker.h"

//---------------------------------------------------------------------------

class cEdEditModeSelect;
class cEdWindowSelect;
class iEdScnObjEditPane;
class iEdWorld;
class iEdObject;
class iEdSelector;

class cEdActionCompound;
class cEdObjTypeCompoundObject;

//---------------------------------------------------------------------------

///////////////////////////////////////////////
// cUIPickMethodRay
//	Picks whatever is hit by a ray cast from 
//	the camera position to the direction where the 
//	mouse points (Extends cUIPickMethod)

class cUIPickMethodRay : public cUIPickMethod
{
public:
	bool PickSpecific(iEdScnObject* apEnt);
};

//------------------------------------------------------------

///////////////////////////////////////////////
// cUIPickMethodBox
//	Picks whatever has a clip bounding rectangle
//	that intersects a 2D box drawn in a viewport.
//  (Extends cUIPickMethod)
class cUIPickMethodBox : public cUIPickMethod
{
public:
	bool PickSpecific(iEdScnObject* apEnt);

	void SetMouseBox(const cRect2l& aRect) { mRect = aRect; }

protected:
	cRect2l mRect;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// ENTITY SELECTOR BASE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdSelector
{
public:
	iEdSelector(cEdEditModeSelect* apEditMode);
	virtual ~iEdSelector();

	virtual bool IsBusy()=0;

	virtual bool OnViewportMouseDown(const cViewportClick&)=0;
	virtual bool OnViewportMouseUp(const cViewportClick&)=0;

	virtual void OnUpdate() {}

	virtual void Draw(const cModuleDrawData&) {}

	virtual void OnSetActive(bool abX) {}

	virtual void Reset() {}

protected:
	cUIPickMethodRay* mpRay;
	cEdEditModeSelect* mpSelect;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECT MODE BASE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEdSelectMode
{
public:
	iEdSelectMode(cEdEditModeSelect*);

	virtual bool IsSelectionUpdated(const tScnObjList&)=0;
	const tScnObjList& GetObjectsToSelect() { return mlstObjectsToSelect; }

protected:
	void AddObjectToSelect(iEdScnObject*);
	void RemoveObjectToSelect(iEdScnObject*);
	void SetObjectsToSelect(const tScnObjList&);
	void ClearObjects();

	tScnObjList mlstObjectsToSelect;
	static cEdEditModeSelect* mpSelect;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECT MODES
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEdSelectMode_SelectMain : public iEdSelectMode
{
public:
	cEdSelectMode_SelectMain(cEdEditModeSelect*);

	bool IsSelectionUpdated(const tScnObjList&);
};

//---------------------------------------------------------------------------

class cEdSelectMode_SelectAlways : public iEdSelectMode
{
public:
	cEdSelectMode_SelectAlways(cEdEditModeSelect*);

	bool IsSelectionUpdated(const tScnObjList&);
};

//---------------------------------------------------------------------------

class cEdSelectMode_Toggle : public iEdSelectMode
{
public:
	cEdSelectMode_Toggle(cEdEditModeSelect*);

	bool IsSelectionUpdated(const tScnObjList&);
};

//---------------------------------------------------------------------------

class cEdSelectMode_Deselect : public iEdSelectMode
{
public:
	cEdSelectMode_Deselect(cEdEditModeSelect*);

	bool IsSelectionUpdated(const tScnObjList&);
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// EDIT MODE SELECT
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEdEditModeSelect : public iEdEditMode
{
	friend class iScnObjManipulator;
	friend class cEdSelection;
public:
	cEdEditModeSelect(iEditor*);
	~cEdEditModeSelect();

	cEdSelection* GetSelection() { return mpSelection; }

	bool IsBusy();
	bool IsSelecting();
	bool IsManipulating();

	void SetCurrentSelector(iEdSelector*);
	iEdSelector* GetCurrentSelector() { return mpCurrentSelector; }

	void SetCurrentManipulator(iScnObjManipulator*);

	///////////////////////////////////////////////////////////////////
	// Actions
	cEdActionCompound*	BeginAction();
	cEdActionCompound*	GetCurrentAction();
	void				EndCurrentAction();

	///////////////////////////////////////////////////////////////////
	// Entity Filtering
	cUIPickFilter* GetFilter() { return mpFilter; }
	cEdObjectPicker* GetPicker() { return mpPicker; }

	iEdSelectMode* GetCurrentSelectMode();

	//tVector3fVec* GetDebugTriangle() { return &mvTriangle; }

	void SetAllowTransform(bool abX) { mbAllowTransform = abX; }

	bool GetPickThroughActive() { return mbPickThrough; }

protected:
	/////////////////////////////////////////////////////////////////
	// Event Handling
	void OnReset();
	void OnUpdate();

	void OnWorldAddRemObject();
	void OnWorldObjModify();
	void OnWorldSelectChange();

	void OnViewportMouseMove();

	void OnViewportMouseDown(const cViewportClick&);
	void OnViewportMouseUp(const cViewportClick&);

	void OnDraw(const cModuleDrawData&);

	iEdWindow* CreateWindow();
	
	void OnSetActive(bool);

	////////////////////////////////////////
	// Data
	cEdSelection* mpSelection;

	iEdSelectMode* mpModeSelect;
	iEdSelectMode* mpModeToggle;
	iEdSelectMode* mpModeDeselect;

	iEdSelector* mpCurrentSelector;
	iEdSelector* mpDefaultSelector;
	iScnObjManipulator* mpCurrentManipulator;

	bool mbShiftPressed;
	bool mbCtrlPressed;
	bool mbCloneOnTransform;

	cUIPickFilter* mpFilter;
	cEdObjectPicker* mpPicker;

	float mfPickingLength;

	bool mbAllowTransform;

	bool mbUpdateEditPane;
	iEdEditPane* mpEditPane;

	bool mbSelectChange;

	cEdActionCompound* mpCurrentCompoundAction;
	
	tVector3fVec mvTriangle;

	bool mbPickThrough;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEdSelectorMain : public iEdSelector
{
public:
	cEdSelectorMain(cEdEditModeSelect* apEditMode);
	~cEdSelectorMain();

	bool IsBusy() { return mbIsSelecting; }

    bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);
	void OnUpdate();

	void Draw(const cModuleDrawData&);

	void OnSetActive(bool abX);

	void Reset();

	bool IsRayPickingActive();

protected:
	bool ComparePickVectors(const tPickVec& av1, const tPickVec& av2);
	tPickVec mvPicks;

	cUIPickMethodBox* mpBox;
	cRect2l mMouseRect;
	bool mbIsSelecting;
};

//---------------------------------------------------------------------------


class cEdSelectorHighlighter : public iEdSelector
{
public:
	cEdSelectorHighlighter(cEdEditModeSelect* apEditMode);
	~cEdSelectorHighlighter();

	bool IsBusy() { return true; }

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);
	void OnUpdate();

	void Draw(const cModuleDrawData&);

	void OnSetActive(bool abX);

	cUIPickFilter* GetFilter() { return mpFilter; }
	bool IsAtLeastOneFilterActive();

	void SetMouseDownCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);
	void SetMouseUpCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);
	void SetMouseOverCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);

	iEdObject* GetPickedEntity() { return mpObjectUnderPointer; }

	void SetHighlightedEntities(const tScnObjList&);
	void AddHighlightedEntity(iEdScnObject*);
	void RemoveHighlightedEntity(iEdScnObject*);
	void ClearHighlightedEntities();

protected:
	iEdScnObject* mpObjectUnderPointer;
	cUIPickFilter* mpFilter;

	tScnObjList mlstHighlightedEntities;

	void* mpMouseDownCallbackObject;
	tGuiCallbackFunc mpMouseDownCallback;

	void* mpMouseUpCallbackObject;
	tGuiCallbackFunc mpMouseUpCallback;

	void* mpMouseOverCallbackObject;
	tGuiCallbackFunc mpMouseOverCallback;
};

//---------------------------------------------------------------------------

#endif // ED_EDIT_MODE_SELECT_H

