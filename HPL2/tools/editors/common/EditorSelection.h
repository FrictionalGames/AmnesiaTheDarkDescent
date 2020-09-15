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

#ifndef HPLEDITOR_EDITOR_SELECTION_H
#define HPLEDITOR_EDITOR_SELECTION_H

#include "EditorEditMode.h"
#include "EditorTypes.h"
#include "EntityWrapper.h"

//---------------------------------------------------------------------------

class cEditorEditModeSelect;
class cEditorWindowSelect;
class cEditorWindowEntityEditBox;
class iEditorWorld;
class iEntityWrapper;
class iEntitySelector;

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SELECTION CLASS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

class cEditorSelection
{
public:
	cEditorSelection(iEditorBase* apEditor);

	bool IsUpdated() { return mbSelectionUpdated; }
	void SetUpdated(bool abX) { mbSelectionUpdated = abX; }

	///////////////////////////////////////////
	// Entity management
	void AddEntity(iEntityWrapper* apEntity);
	void RemoveEntity(iEntityWrapper* apEntity);
	void ClearEntities();
	
	bool HasEntity(iEntityWrapper* apEntity, tEntityWrapperListIt* apIt=NULL );
	bool HasEntity(int alEntityID, tIntListIt* apIt=NULL );

	// Action helpers
	void AddEntitiesByID(const tIntList& alstIDs);
	void RemoveEntitiesByID(const tIntList& alstIDs);
	void ToggleEntitySelectionByID(const tIntList& alstIDs);

	tEntityWrapperList& GetEntities() { return mlstEntities; }
	tIntList& GetEntityIDs() { return mlstEntityIDs; }
	
	bool IsEmpty() { return mlstEntities.empty(); }
	int GetNumEntities() { return (int)mlstEntities.size(); }

	//////////////////////////////////////////
	// Transform related
	void UpdateReferenceTransforms();
	void FallToReferenceTransforms();

	void SetRelativeTranslation(const cVector3f& avPosition, bool abUseSnap);
	void SetRelativeRotation(const cVector3f& avRotation, bool abUseSnap);
	void SetRelativeScale(const cVector3f& avScale, bool abUseSnap);

	void SnapScale(cVector3f &avVector);
	void SnapValue(cVector3f &avVector, float afSnapAmount);

	void SnapRotate(const cVector3f &avBaseRotation, cVector3f &avAddedRotation);

	/////////////////////////////////////////
	// Selection Center (Handle)
	void UpdateCenter();

	const cVector3f& GetSelectionAABBMin() { return mvSelectionMin; }
	const cVector3f& GetSelectionAABBMax() { return mvSelectionMax; }

	cVector3f& GetCenterTranslation() { return mvCenterTranslation; }
	cVector3f& GetCenterRotation() { return mvCenterRotation; }

	void SetCenterTranslation(const cVector3f& avTranslation);
	void SetCenterRotation(const cVector3f& avRotation);

	cVector3f& GetCenterOldTranslation() { return mvCenterOldTranslation; }
	cVector3f& GetCenterOldRotation() { return mvCenterOldRotation; }


	/////////////////////////////////////////
	// Misc methods
	bool CanTranslate();
	bool CanRotate();
	bool CanScale();
	bool IsCloneable();
	bool IsDeletable();
	
	bool IsHomogeneousSelection();

	void UpdateProperties();

	static void SetRotateSnap(float afX) { mfRotateSnap = afX; }
	static void SetScaleSnap(float afX) { mfScaleSnap = afX; }
	static float GetRotateSnap() { return mfRotateSnap; }
	static float GetScaleSnap() { return mfScaleSnap; }

protected:
	iEditorBase* mpEditor;
	bool mbSelectionUpdated;
	bool mbPropertiesUpdated;

	tEntityWrapperList mlstEntities;
	tIntList mlstEntityIDs;
	tVector3fList mlstReferenceTranslations;
	tVector3fList mlstReferenceRotations;
	tVector3fList mlstReferenceScales;

	tMatrixfList mlstEntityRelativeMatrices;
	cMatrixf mmtxCenterTransform;

	cVector3f mvSelectionMax;
	cVector3f mvSelectionMin;

	cVector3f mvMouseOffset;

	cVector3f mvCenterTranslation;
	cVector3f mvCenterRotation;

	cVector3f mvCenterOldTranslation;
	cVector3f mvCenterOldRotation;
	
	bool mbTransformed;

	bool mbCanTranslate;
	bool mbCanRotate;
	eScaleType mScaleType;
	bool mbCloneable;
	bool mbDeletable;

	bool mbHomogeneous;

	static float mfScaleSnap;
	static float mfRotateSnap;
};

#endif // HPLEDITOR_EDITOR_SELECTION_H
