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

#ifndef HPLEDITOR_EDITOR_EDIT_MODE_SELECT_H
#define HPLEDITOR_EDITOR_EDIT_MODE_SELECT_H

#include "EditorEditMode.h"
#include "EditorTypes.h"

#include "EditorEditModeSelectTool.h"
#include "EditorActionEntity.h"

#include "EditorUIPicker.h"

//---------------------------------------------------------------------------

class cEditorEditModeSelect;
class cEditorWindowSelect;
class cEditorWindowEntityEditBox;
class iEditorWorld;
class iEntityWrapper;
class iEntitySelector;

class cEditorActionCompoundAction;
class cEntityWrapperTypeCompoundObject;

//---------------------------------------------------------------------------

///////////////////////////////////////////////
// cUIPickMethodRay
//	Picks whatever is hit by a ray cast from 
//	the camera position to the direction where the 
//	mouse points (Extends cUIPickMethod)
class cUIPickMethodRay : public cUIPickMethod
{
public:
	bool PickSpecific(iEntityWrapper* apEnt);
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
	bool PickSpecific(iEntityWrapper* apEnt);

	void SetMouseBox(const cRect2l& aRect) { mRect = aRect; }

protected:
	cRect2l mRect;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// ENTITY SELECTOR BASE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class iEntitySelector
{
public:
	iEntitySelector(cEditorEditModeSelect* apEditMode);
	virtual ~iEntitySelector();

	virtual bool OnViewportMouseDown()=0;
	virtual bool OnViewportMouseUp()=0;

	virtual void OnEditorUpdate() {}

	virtual void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions) {}

	virtual void OnSetCurrent(bool abX) {}

	virtual void Reset() {}

protected:
	cUIPickMethodRay* mpRay;
	cEditorEditModeSelect* mpEditMode;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// EDIT MODE SELECT
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------


class cEditorEditModeSelect : public iEditorEditMode
{
	friend class cEditorEditModeSelectTool;
	friend class cEditorSelection;

public:
	cEditorEditModeSelect(iEditorBase* apEditor, iEditorWorld* apEditorWorld);
	~cEditorEditModeSelect();

	void PostEditModesCreation();

	void Reset();

	iEditorWorld* GetEditorWorld() { return mpEditorWorld; }

    /////////////////////////////////////////////////////////////////
	// Event Handling
	void OnEditorUpdate(float afTimeStep);

	void OnViewportMouseDown(int alButtons);
	void OnViewportMouseUp(int alButtons);

	void DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

	//////////////////////////////////////////////////////////////////
	// EditBox stuff
	void SetEditBoxNeedsRefresh(bool abX) { mbEditBoxNeedsRefresh=abX; }
	void ShowEditBox();
	cEditorWindowEntityEditBox* CreateEditBoxWindow();

	void SetSelectToolMode(eSelectToolMode aMode);
	eSelectToolMode GetSelectToolMode() { return mToolMode; }

	///////////////////////////////////////////////////////////////////
	// Actions
	iEditorAction* CreateSelectEntityAction(const tIntList& alstEntityIDs, eSelectActionType aType);
	iEditorAction* CreateDeleteEntitiesAction(const tIntList& alstEntityIDs);
	iEditorAction* CreateCloneEntitiesAction(const tIntList& alstEntityIDs);
	iEditorAction* CreateCompoundObjectAction();

	cEditorActionCompoundAction* BeginCompoundAction();
	cEditorActionCompoundAction* GetCurrentCompoundAction();
	void EndCurrentCompoundAction();


	///////////////////////////////////////////////////////////////////
	// Entity Filtering
	cUIPickFilter* GetFilter() { return mpFilter; }
	void AddEntityFilter(eEditorEntityType aType);

	tIntList& GetCurrentFilteredTypes() { return mlstEntityFilters; }

	///////////////////////////////////////////////////////////////////
	// Entity Selector management
	void SetCurrentEntitySelector(iEntitySelector* apSelector);
	iEntitySelector* GetCurrentEntitySelector() { return mpCurrentSelector; }
	
	void SetAllowTransform(bool abX) { mbAllowTransform = abX; }

	eSelectActionType GetSelectActionType();

	cVector3f mvRayStart;
	cVector3f mvRayEnd;

	tVector3fVec* GetDebugTriangle() { return &mvTriangle; }

protected:
	iEditorWindow* CreateSpecificWindow();
	
	void OnSetCurrent(bool abX);

	////////////////////////////////////////
	// Data
	eSelectToolMode mToolMode;
	std::vector<cEditorEditModeSelectTool*> mvTools;
	cEditorEditModeSelectTool* mpCurrentTool;

	bool mbToggleSelect;
	bool mbAlwaysDeselect;
	bool mbCloneOnTransform;

	iEntitySelector* mpCurrentSelector;
	iEntitySelector* mpDefaultSelector;

	cUIPickFilter* mpFilter;
	tIntList mlstEntityFilters;

	float mfPickingLength;

	bool mbAllowTransform;

	bool mbEditBoxNeedsRefresh;
	cEditorWindowEntityEditBox* mpEditBox;

	iEditorWorld* mpEditorWorld;


	cEditorActionCompoundAction* mpCurrentCompoundAction;
	cEntityWrapperTypeCompoundObject* mpType;

	tVector3fVec mvTriangle;
};


//---------------------------------------------------------------------------

class cEntitySelectorNormal : public iEntitySelector
{
public:
	cEntitySelectorNormal(cEditorEditModeSelect* apEditMode);
	~cEntitySelectorNormal();

    bool OnViewportMouseDown();
	bool OnViewportMouseUp();
	void OnEditorUpdate();

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);

	void OnSetCurrent(bool abX);

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

class cEntitySelectorHighlighter : public iEntitySelector
{
public:
	cEntitySelectorHighlighter(cEditorEditModeSelect* apEditMode);
	~cEntitySelectorHighlighter();

	bool OnViewportMouseDown();
	bool OnViewportMouseUp();
	void OnEditorUpdate();

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);

	void OnSetCurrent(bool abX);

	cUIPickFilter* GetFilter() { return mpFilter; }
	bool IsAtLeastOneFilterActive();

	void SetMouseDownCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);
	void SetMouseUpCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);
	void SetMouseOverCallback(void* apCallbackObject, tGuiCallbackFunc apCallback);

	iEntityWrapper* GetPickedEntity() { return mpEntityUnderPointer; }

	void SetHighlightedEntities(const tEntityWrapperList& alstHighlightedEntities);
	void AddHighlightedEntity(iEntityWrapper* apEntity);
	void RemoveHighlightedEntity(iEntityWrapper* apEntity);
	void ClearHighlightedEntities();

protected:
	iEntityWrapper* mpEntityUnderPointer;
	cUIPickFilter* mpFilter;

	tEntityWrapperList mlstHighlightedEntities;

	void* mpMouseDownCallbackObject;
	tGuiCallbackFunc mpMouseDownCallback;

	void* mpMouseUpCallbackObject;
	tGuiCallbackFunc mpMouseUpCallback;

	void* mpMouseOverCallbackObject;
	tGuiCallbackFunc mpMouseOverCallback;
};

//---------------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_EDIT_MODE_SELECT_H

