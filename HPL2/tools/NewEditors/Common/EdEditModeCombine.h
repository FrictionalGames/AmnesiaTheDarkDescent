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

#ifndef ED_EDIT_MODE_COMBINE_H
#define ED_EDIT_MODE_COMBINE_H

#include "../Common/EdEditMode.h"
#include "../LevelEditor/LevelEditorStaticObjectCombo.h"

class cEdObjectPicker;

//--------------------------------------------------------------

enum eCombinePickMode
{
	eCombinePickMode_Add,
	eCombinePickMode_Remove,
	eCombinePickMode_Toggle,
};

//--------------------------------------------------------------

class cEdEditModeCombine : public iEdEditMode
{
public:
	cEdEditModeCombine(iEditor*);
	~cEdEditModeCombine();

	bool IsBusy();

	void OnViewportMouseDown(const cViewportClick&);
	void OnViewportMouseUp(const cViewportClick&);

	void OnUpdate();

	void OnDraw(const cModuleDrawData& aData);

	///////////////////////////////////
	// Interface for window
	const tStaticObjectComboList& GetCombos();
	cLevelEditorStaticObjectCombo* GetCombo(int alID);

	void AddCombo();
	void RemoveCombo(int alID);
	void SetComboColor(int alID, const cColor& aCol);

protected:
	iEdWindow* CreateWindow();

	void OnInit();

	void OnViewportMouseMove();

	/**
	 * Functions that do the actual picking
	 */
	void DoRayPicking(cLevelEditorStaticObjectCombo* apCombo, iEdViewport*, cEdObjectPicker*);
	void DoBoxPicking(cLevelEditorStaticObjectCombo* apCombo, iEdViewport*, cEdObjectPicker*);
	
	/**
	 * Create an action according to pick mode
	 */
	iEdAction* CreateProperAction(cLevelEditorStaticObjectCombo* apCombo, iEdScnObject* apObj,  eCombinePickMode aMode);

	/**
	 * Returns true if the user is not attempting to do a box picking (box is too small)
	 */
	bool IsRayPickingActive();

	///////////////////////////////////
	// Data
	bool mbPressed;
	cRect2l mMouseRect;

	tScnObjList mlstObjsToBePicked;

	cEdObjectPicker* mpPicker;
	cEdEditModeSelect* mpSelect;

	cUIPickFilter* mpFilter;
	cUIPickMethod* mpRay;
	cUIPickMethod* mpBox;
};

//--------------------------------------------------------------

#endif // ED_EDIT_MODE_COMBINE_H
