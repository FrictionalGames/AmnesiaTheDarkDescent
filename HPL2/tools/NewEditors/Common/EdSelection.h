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

#ifndef ED_SELECTION_H
#define ED_SELECTION_H

#include "../Common/StdAfx.h"

#include "../Common/EdScnObject.h"

//---------------------------------------------------------------------------

class iEditor;
class iEdAction;

class cEdSelection;
class cEdEditModeSelect;

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECTION TYPE
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEdSelectionType : public iBaseAggregateType
{
public:
	cEdSelectionType(cEdSelection*);

	int GetNumManipulators();
	iScnObjManipulator* GetManipulator(int);

	void UpdateType();

	iEdEditPane* CreateGroupEditPane(iEdScnObject*);

	bool AllowNullEngineObject() { return true; }

	iEdScnObjType* GetSelectionType() { return mpMostRestrictiveType; }

protected:
	iEdObjectData* CreateTypeSpecificData() { return NULL; }

	cEdSelection* mpSelection;
	iEdScnObjType* mpMostRestrictiveType;
};

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECTION CLASS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEdSelection : public iBaseAggregateObject
{
	friend class cEdEditModeSelect;
public:
	cEdSelection(cEdEditModeSelect*);
	~cEdSelection();

	///////////////////////////////////////////
	// Object management
	void Clear();
	
	bool HasObject(iEdScnObject*);
	bool HasObject(int);

	void CheckObjectsAreValid();

	void SetGlobalTransform(bool abX);
	bool GetGlobalTransform()		  { return mbGlobalTransform; }

	void SetTranslation(const cVector3f&);
	void SetRotation(const cVector3f&);
	void SetScale(const cVector3f&);

	//void SetWorldMatrix(

	// Action helpers
	iEdAction* CreateSelectObjectsAction(const tScnObjList&);
	iEdAction* CreateDeselectObjectsAction(const tScnObjList&);
	iEdAction* CreateToggleObjSelectionAction(const tScnObjList&);

	iEdAction* CreateDeleteObjectsAction();
	iEdAction* CreateCloneObjectsAction(bool abSelectClones=true);

	iEdAction* CreateCompoundObjectAction();

	/*
	iEdAction* CreateSetIntAction(int, int);
	iEdAction* CreateSetFloatAction(int, float);
	iEdAction* CreateSetStringAction(int, const tString&);
	iEdAction* CreateSetVector2fAction(int, const cVector2f&);
	iEdAction* CreateSetVector3fAction(int, const cVector3f&);
	iEdAction* CreateSetColorAction(int, const cColor&);
	*/

	iEdAction* CreateSetIntAction(const tString&, int);
	iEdAction* CreateSetFloatAction(const tString&, float);
	iEdAction* CreateSetStringAction(const tString&, const tString&);
	iEdAction* CreateSetVector2fAction(const tString&, const cVector2f&);
	iEdAction* CreateSetVector3fAction(const tString&, const cVector3f&);
	iEdAction* CreateSetColorAction(const tString&, const cColor&);

	iEdAction* CreateSetVector3fAction(int, const cVector3f&);

	void SetChangeEditModeOnChangeSelection(bool abX) { mbChangeEditMode = abX; }

	iEdEditPane* CreateEditPane();

	cBoundingVolume* GetBoundingVolume();
protected:
	bool OnUpdate(bool);

	void OnDraw(const cModuleDrawData&);

	void OnAddComponent(iEdScnObject*);
	void OnRemoveComponent(iEdScnObject*);
	void UpdateComponents();

	void UpdateBV();

	cEdEditModeSelect* mpEditMode;
	tIntList mlstObjectIDs;

	bool mbBVUpdated;
	cBoundingVolume mBV;

	cMatrixf mmtxInvTranslateMatrix;
	cMatrixf mmtxInvRotateMatrix;
	cMatrixf mmtxInvScaleMatrix;

	tMatrixfList mlstRelTranslationMatrices;
	tMatrixfList mlstRelRotationMatrices;
	tMatrixfList mlstRelScaleMatrices;

	bool mbChangeEditMode;

	bool mbGlobalTransform;
	bool mbKeepTransform;

	cVector3f mvStoredRotate;
	cVector3f mvStoredScale;
};

#endif // ED_SELECTION_H
